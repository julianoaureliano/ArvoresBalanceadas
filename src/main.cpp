// main.cpp - Programa interativo: Amazon Engineering Challenge (IMD0039)
// Registro de identificadores de encomendas em árvore AVL ou Rubro-Negra.
#include "tree.hpp"
#include <fstream>
#include <limits>
#include <sstream>

namespace {

bool readInt(std::istream& in, int& out) {
    std::string tok;
    if (!(in >> tok)) return false;
    try {
        std::size_t pos = 0;
        long long v = std::stoll(tok, &pos);
        if (pos != tok.size() || v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max())
            throw std::invalid_argument("x");
        out = (int)v;
        return true;
    } catch (...) {
        std::cout << "  Entrada invalida: '" << tok << "' (use inteiros).\n";
        out = 0;
        return false;
    }
}

void menu(const ITree& t) {
    std::cout << "\n=== Amazon Logistica | Arvore atual: " << t.name()
              << " | n=" << t.size() << " altura=" << t.height() << " ===\n"
              << " 1) Inserir encomenda      2) Remover encomenda\n"
              << " 3) Buscar encomenda       4) Percurso em ordem\n"
              << " 5) Imprimir arvore        6) Estatisticas + validar propriedades\n"
              << " 7) Inserir varios (informe N e depois N valores)\n"
              << " 8) Carregar de arquivo    9) Trocar tipo de arvore (descarta dados)\n"
              << " 0) Sair\n> ";
}

std::unique_ptr<ITree> choose(std::istream& in) {
    for (;;) {
        std::cout << "Escolha a estrutura:\n  1) Arvore AVL\n  2) Arvore Rubro-Negra\n> ";
        int c;
        if (!(in >> c)) return nullptr;
        if (c == 1) return makeAVL();
        if (c == 2) return makeRedBlack();
        std::cout << "  Opcao invalida.\n";
    }
}

void doInsert(ITree& t, int k) {
    if (t.insert(k)) std::cout << "  [OK] " << k << " inserido.\n";
    else std::cout << "  [AVISO] " << k << " ja existe; insercao ignorada (sem duplicatas).\n";
}

} // namespace

int main() {
    std::cout << "IMD0039 - Estruturas de Dados Basicas II | Amazon Engineering Challenge\n";
    auto tree = choose(std::cin);
    if (!tree) return 0;
    const bool color = false; // use a marcacao textual [R]/[B]; mude para true p/ ANSI

    for (;;) {
        menu(*tree);
        int op;
        if (!(std::cin >> op)) break;
        int k;
        switch (op) {
        case 0: std::cout << "Encerrando.\n"; return 0;
        case 1:
            std::cout << "  Valor: ";
            if (readInt(std::cin, k)) doInsert(*tree, k);
            break;
        case 2:
            std::cout << "  Valor: ";
            if (readInt(std::cin, k)) {
                if (tree->remove(k)) std::cout << "  [OK] " << k << " removido.\n";
                else std::cout << "  [AVISO] " << k << " nao existe; nada removido.\n";
            }
            break;
        case 3:
            std::cout << "  Valor: ";
            if (readInt(std::cin, k))
                std::cout << "  " << k << (tree->contains(k) ? " ENCONTRADO.\n" : " NAO encontrado.\n");
            break;
        case 4: {
            auto v = tree->inorder();
            if (v.empty()) { std::cout << "  (arvore vazia)\n"; break; }
            std::cout << "  Em ordem:";
            for (int x : v) std::cout << ' ' << x;
            std::cout << '\n';
            break;
        }
        case 5:
            if (tree->name() != "AVL") std::cout << "  Legenda: [R]=vermelho  [B]=preto\n";
            else std::cout << "  Legenda: fb = fator de balanceamento (altura esq - altura dir)\n";
            tree->print(std::cout, color);
            break;
        case 6: {
            std::string why;
            bool ok = tree->validate(why);
            std::cout << "  Estrutura: " << tree->name() << "\n  Elementos: " << tree->size()
                      << "\n  Altura: " << tree->height()
                      << "\n  Rotacoes: " << tree->stats().rotations
                      << "\n  Recolorizacoes: " << tree->stats().recolorings
                      << "\n  Propriedades: " << (ok ? "VALIDAS" : "VIOLADAS -> " + why) << '\n';
            break;
        }
        case 7: {
            int n;
            std::cout << "  Quantos valores? ";
            if (!readInt(std::cin, n) || n < 0) break;
            for (int i = 0; i < n; ++i) {
                if (!readInt(std::cin, k)) break;
                doInsert(*tree, k);
            }
            break;
        }
        case 8: {
            std::string path;
            std::cout << "  Caminho do arquivo (inteiros separados por espaco/linha): ";
            std::cin >> path;
            std::ifstream f(path);
            if (!f) { std::cout << "  [ERRO] nao foi possivel abrir '" << path << "'.\n"; break; }
            int cnt = 0, dup = 0;
            while (readInt(f, k)) { tree->insert(k) ? ++cnt : ++dup; }
            std::cout << "  Inseridos: " << cnt << " | duplicados ignorados: " << dup << '\n';
            break;
        }
        case 9: {
            auto t2 = choose(std::cin);
            if (t2) tree = std::move(t2);
            break;
        }
        default: std::cout << "  Opcao invalida.\n";
        }
    }
    return 0;
}
