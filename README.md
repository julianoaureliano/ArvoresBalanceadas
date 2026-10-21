# Amazon Engineering Challenge — Árvores Balanceadas (AVL e Rubro-Negra)

**Disciplina:** Estruturas de Dados Básicas II (Trabalho da Unidade II)
**Autor:** Juliano Aureliano - 20230053719
**Linguagem:** C++

Protótipo de um sistema de logística para a Black Friday: registrar, localizar e
remover identificadores de encomendas com eficiência, mesmo quando os IDs chegam
em ordem desfavorável (crescente, decrescente, em lotes). O usuário escolhe em
tempo de execução entre **Árvore AVL** e **Árvore Rubro-Negra**, ambas
implementadas do zero (nenhuma biblioteca de árvores ou de balanceamento foi usada;
`std::set` aparece **apenas** nos testes, como oráculo de verificação).

## Estrutura do repositório

```
src/tree.hpp         Interface comum (ITree) + contadores de rotações/recolorizações
src/avl.cpp          Árvore AVL (rotações simples e duplas)
src/redblack.cpp     Árvore Rubro-Negra (CLRS, com nó sentinela nil)
src/main.cpp         Programa interativo (menu)
tests/tests.cpp      Testes automatizados (mesmas sequências nas duas árvores)
bench/compare.cpp    Análise comparativa (alturas, rotações, recolorizações, tempos)
bench/demo.cpp       Exemplos "antes/depois" de rotações e ajustes de cor
exemplos/            Arquivo de exemplo para a opção "Carregar de arquivo"
docs/                Saídas geradas: testes, comparativo, demo e sessões de uso
build/               Binários já compilados (Linux x86-64)
Makefile
```

## Como compilar e executar

Requisitos: `g++` com suporte a C++17 e `make`.

```bash
make            # compila tudo em build/
./build/amazon_trees     # programa interativo
make test       # roda os testes automatizados
make bench      # roda a análise comparativa (imprime tabelas em Markdown)
make demo       # imprime exemplos de rotação/recoloração antes e depois
```

Sem `make`: `g++ -std=c++17 -O2 -o amazon_trees src/main.cpp src/avl.cpp src/redblack.cpp`.

## Funcionalidades

Menu do programa (idêntico para as duas árvores):

| Opção | Operação                                                                                           |
| ----- | -------------------------------------------------------------------------------------------------- |
| 1     | Inserir encomenda                                                                                  |
| 2     | Remover encomenda                                                                                  |
| 3     | Buscar encomenda                                                                                   |
| 4     | Percurso em ordem (valores crescentes)                                                             |
| 5     | Imprimir a árvore (AVL mostra o fator de balanceamento `fb`; Rubro-Negra mostra a cor `[R]`/`[B]`) |
| 6     | Estatísticas (n, altura, rotações, recolorizações) e **validação de todas as propriedades**        |
| 7     | Inserir vários valores de uma vez                                                                  |
| 8     | Carregar valores de um arquivo (ex.: `exemplos/encomendas.txt`)                                    |
| 9     | Trocar o tipo de árvore (descarta os dados atuais)                                                 |

A impressão é "deitada": a raiz fica à esquerda, o filho direito aparece acima e o
esquerdo abaixo. Exemplo (Rubro-Negra com 8 elementos):

```
            /-- 80 [R]
        /-- 70 [B]
        |   \-- 60 [R]
    /-- 50 [R]
    |   \-- 40 [B]
30 [B]
    \-- 20 [B]
        \-- 10 [R]
```

Sessões completas de uso estão em `docs/sessao_avl.txt` e `docs/sessao_rubronegra.txt`.

## Decisões de projeto (tratamento de casos especiais)

- **Valores repetidos:** a árvore é tratada como um _conjunto_ (ID de encomenda é único). Inserir um valor já existente **não altera** a árvore e é informado ao usuário (`insert` retorna `false`).
- **Remoção/busca de inexistentes:** a árvore permanece intacta; o programa avisa que o valor não existe (`remove` retorna `false`).
- **Árvore vazia / com 1 elemento:** todas as operações funcionam (altura 0 e 1, respectivamente); remover o último elemento devolve a árvore vazia, que aceita novas inserções.
- **Altura:** contada em nós (vazia = 0, um nó = 1).
- **Remoção com dois filhos:** o nó é substituído pelo **sucessor em ordem** (menor da subárvore direita).
- **Rotações contadas:** cada rotação simples conta 1; a rotação dupla da AVL conta 2.
- **Recolorizações contadas (Rubro-Negra):** contam-se mudanças de cor de nós **já existentes**. A cor vermelha inicial de um nó recém-inserido e a herança de cor na remoção (o sucessor assume a cor do nó removido) não são contadas.
- **Entrada inválida** (texto no lugar de inteiro) é rejeitada sem encerrar o programa.
- **Chaves:** inteiros (`int`).

## Algoritmos

### AVL (`src/avl.cpp`)

Invariante: para todo nó, `|h(esq) − h(dir)| ≤ 1`. Após inserir/remover (recursão), no
retorno atualiza-se a altura e calcula-se `fb = h(esq) − h(dir)`:

| Caso    | Condição                  | Correção                                      |
| ------- | ------------------------- | --------------------------------------------- |
| Esq-Esq | `fb > 1` e `fb(esq) ≥ 0`  | rotação simples à direita                     |
| Esq-Dir | `fb > 1` e `fb(esq) < 0`  | rotação à esquerda no filho + à direita no nó |
| Dir-Dir | `fb < −1` e `fb(dir) ≤ 0` | rotação simples à esquerda                    |
| Dir-Esq | `fb < −1` e `fb(dir) > 0` | rotação à direita no filho + à esquerda no nó |

Na inserção, no máximo um ajuste (simples ou duplo) é necessário; na remoção, podem
ser necessários ajustes em vários níveis do caminho até a raiz. Altura ≤ ~1,44·log₂(n+2).

### Rubro-Negra (`src/redblack.cpp`)

Propriedades: (1) nós vermelhos ou pretos; (2) raiz preta; (3) folhas `nil` pretas;
(4) filho de nó vermelho é preto; (5) todo caminho até as folhas tem o mesmo número de
nós pretos. Altura ≤ 2·log₂(n+1).

- **Inserção:** o nó entra vermelho. Se o pai é vermelho: _tio vermelho_ → recolorir pai, tio e avô e subir; _tio preto_ → (triângulo: rotação no pai) + rotação no avô com recoloração. No fim a raiz é pintada de preto.
- **Remoção:** se o nó efetivamente removido era preto surge um "preto duplo", resolvido por 4 casos (irmão vermelho; irmão preto com sobrinhos pretos; sobrinho distante preto; sobrinho distante vermelho), com no máximo 3 rotações.

## Testes (`make test`)

As **mesmas** sequências são executadas nas duas árvores e, depois de **cada** operação, `validate()` confere as invariantes:

- **AVL:** ordem BST, alturas armazenadas corretas e `|fb| ≤ 1` em todos os nós.
- **Rubro-Negra:** ordem BST, raiz preta, ausência de dois vermelhos consecutivos, igualdade das alturas-negras e consistência dos ponteiros para o pai.

Cenários cobertos: árvore vazia e com um elemento · duplicados · inserção crescente, decrescente e embaralhada (1000 chaves) · busca de existentes e inexistentes · remoção de folha, nó com um filho, nó com dois filhos, raiz e inexistente · esvaziar a árvore em ordem crescente e decrescente · os 4 padrões de rotação com 3 nós · **estresse**: 20 sementes × 3000 operações aleatórias + 1 execução de 30 000 operações, comparadas com `std::set`. Resultado: 6334 verificações, 0 falhas (`docs/resultado_testes.txt`).

## Análise comparativa (`make bench`, tabelas completas em `docs/resultados_comparativo.md`)

Resumo (seed 2024, N = 10 000 inserções; depois remoção de N/2 em ordem aleatória):

| Entrada              | Altura AVL | Altura RB | Rotações AVL | Rotações RB | Altura BST comum |
| -------------------- | ---------- | --------- | ------------ | ----------- | ---------------- |
| crescente            | 14         | 24        | 9986         | 9976        | 10 000           |
| decrescente          | 14         | 24        | 9986         | 9976        | 10 000           |
| aleatória            | 16         | 16        | 6972         | 5918        | 33               |
| lotes (Black Friday) | 15         | 21        | 12103        | 18304       | 1404             |

A AVL mantém a árvore mais compacta em entradas ordenadas/em lotes (altura ≈ ideal, ⌈log₂(N+1)⌉ = 14), enquanto a Rubro-Negra tolera até ~2·log₂ N, mas faz menos rotações em entradas aleatórias (5918 contra 6972); na remoção os custos ficaram muito próximos.
Já com inserção em lotes a Rubro-Negra fez mais rotações que a AVL. As duas validam todas as propriedades e ambas evitam o colapso da BST comum (altura N).

## Referências

- Material da disciplina IMD0039 sobre árvores AVL e Rubro-Negras.
- Testes e tabelas comparativas do programa
- Inteligencia Artificial usada para fazer o código

## 👩‍🚀 Autor

Desenvolvido por Juliano Aureliano. 😊

> **Nota:** Este projeto foi feito para estudo/exercício.
