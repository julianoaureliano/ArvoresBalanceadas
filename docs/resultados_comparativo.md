# Resultados da analise comparativa (seed=2024)

Remocao: metade dos elementos (embaralhados), mesma sequencia nas duas arvores.
Altura em nos (arvore vazia = 0). Rotacao dupla conta 2.

## N = 1000

| Entrada | Estrutura | Altura apos insercao | Rotacoes (ins) | Recolor. (ins) | Altura apos remocao de N/2 | Rotacoes (rem) | Recolor. (rem) | Tempo ins (ms) | Tempo rem (ms) | Valida |
|---|---|---|---|---|---|---|---|---|---|---|
| crescente | AVL | 10 | 990 | - | 10 | 94 | - | 0.08 | 0.06 | sim |
| crescente | Rubro-Negra | 17 | 983 | 4909 | 10 | 93 | 1124 | 0.08 | 0.03 | sim |
| crescente | BST comum (ref.) | 1000 | - | - | - | - | - | - | - | - |
| decrescente | AVL | 10 | 990 | - | 10 | 87 | - | 0.05 | 0.06 | sim |
| decrescente | Rubro-Negra | 17 | 983 | 4909 | 10 | 109 | 1146 | 0.06 | 0.03 | sim |
| decrescente | BST comum (ref.) | 1000 | - | - | - | - | - | - | - | - |
| aleatoria | AVL | 12 | 718 | - | 11 | 185 | - | 0.10 | 0.06 | sim |
| aleatoria | Rubro-Negra | 12 | 580 | 2269 | 11 | 188 | 759 | 0.06 | 0.03 | sim |
| aleatoria | BST comum (ref.) | 21 | - | - | - | - | - | - | - | - |
| lotes (Black Friday) | AVL | 12 | 1145 | - | 11 | 176 | - | 0.08 | 0.06 | sim |
| lotes (Black Friday) | Rubro-Negra | 15 | 1579 | 4331 | 11 | 227 | 1184 | 0.06 | 0.03 | sim |
| lotes (Black Friday) | BST comum (ref.) | 184 | - | - | - | - | - | - | - | - |

Limites teoricos: altura AVL <= 1.44*log2(N+2) = 14.4; altura RB <= 2*log2(N+1) = 19.9; minimo possivel = ceil(log2(N+1)) = 10.

## N = 10000

| Entrada | Estrutura | Altura apos insercao | Rotacoes (ins) | Recolor. (ins) | Altura apos remocao de N/2 | Rotacoes (rem) | Recolor. (rem) | Tempo ins (ms) | Tempo rem (ms) | Valida |
|---|---|---|---|---|---|---|---|---|---|---|
| crescente | AVL | 14 | 9986 | - | 14 | 896 | - | 0.81 | 0.87 | sim |
| crescente | Rubro-Negra | 24 | 9976 | 49877 | 15 | 1003 | 11459 | 0.89 | 0.56 | sim |
| crescente | BST comum (ref.) | 10000 | - | - | - | - | - | - | - | - |
| decrescente | AVL | 14 | 9986 | - | 14 | 853 | - | 0.65 | 0.82 | sim |
| decrescente | Rubro-Negra | 24 | 9976 | 49877 | 15 | 1016 | 11438 | 0.79 | 0.47 | sim |
| decrescente | BST comum (ref.) | 10000 | - | - | - | - | - | - | - | - |
| aleatoria | AVL | 16 | 6972 | - | 15 | 1908 | - | 1.55 | 0.83 | sim |
| aleatoria | Rubro-Negra | 16 | 5918 | 23244 | 16 | 1849 | 7863 | 1.09 | 0.51 | sim |
| aleatoria | BST comum (ref.) | 33 | - | - | - | - | - | - | - | - |
| lotes (Black Friday) | AVL | 15 | 12103 | - | 15 | 1402 | - | 0.78 | 0.82 | sim |
| lotes (Black Friday) | Rubro-Negra | 21 | 18304 | 48834 | 16 | 1289 | 11723 | 0.74 | 0.48 | sim |
| lotes (Black Friday) | BST comum (ref.) | 1404 | - | - | - | - | - | - | - | - |

Limites teoricos: altura AVL <= 1.44*log2(N+2) = 19.1; altura RB <= 2*log2(N+1) = 26.6; minimo possivel = ceil(log2(N+1)) = 14.

## N = 100000

| Entrada | Estrutura | Altura apos insercao | Rotacoes (ins) | Recolor. (ins) | Altura apos remocao de N/2 | Rotacoes (rem) | Recolor. (rem) | Tempo ins (ms) | Tempo rem (ms) | Valida |
|---|---|---|---|---|---|---|---|---|---|---|
| crescente | AVL | 17 | 99983 | - | 17 | 8563 | - | 19.55 | 26.55 | sim |
| crescente | Rubro-Negra | 31 | 99969 | 499846 | 18 | 9330 | 113864 | 18.01 | 18.13 | sim |
| decrescente | AVL | 17 | 99983 | - | 17 | 8325 | - | 20.33 | 27.97 | sim |
| decrescente | Rubro-Negra | 31 | 99969 | 499846 | 18 | 9193 | 113589 | 16.54 | 19.90 | sim |
| aleatoria | AVL | 20 | 70083 | - | 19 | 18634 | - | 50.04 | 33.23 | sim |
| aleatoria | Rubro-Negra | 20 | 58248 | 231978 | 20 | 18133 | 77184 | 54.56 | 16.94 | sim |
| lotes (Black Friday) | AVL | 19 | 111458 | - | 19 | 12178 | - | 13.54 | 15.67 | sim |
| lotes (Black Friday) | Rubro-Negra | 28 | 192408 | 498006 | 19 | 9898 | 114273 | 10.52 | 10.88 | sim |

Limites teoricos: altura AVL <= 1.44*log2(N+2) = 23.9; altura RB <= 2*log2(N+1) = 33.2; minimo possivel = ceil(log2(N+1)) = 17.

## Altura media em 30 execucoes aleatorias (N = 10000)

| Estrutura | Altura media | Altura max | Rotacoes medias (ins) |
|---|---|---|---|
| AVL | 16.00 | 16 | 6964.97 |
| Rubro-Negra | 16.20 | 17 | 5818.47 |
