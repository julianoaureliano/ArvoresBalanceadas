// demo.cpp - Gera exemplos "antes e depois" de rotações/ajustes de cor
// (material para os slides de AVL e Rubro-Negra).
#include "../src/tree.hpp"

static void step(ITree& t, int k) {
    t.resetStats();
    t.insert(k);
    std::cout << "\n-> inserir " << k << "   (rotacoes: " << t.stats().rotations
              << ", recolorizacoes: " << t.stats().recolorings << ")\n";
    t.print(std::cout, false);
}

int main() {
    std::cout << "################ AVL: rotacao simples (caso Dir-Dir) ################\n";
    { auto t = makeAVL(); for (int k : {10, 20}) t->insert(k);
      std::cout << "\nANTES (10, 20):\n"; t->print(std::cout);
      step(*t, 30); }

    std::cout << "\n################ AVL: rotacao dupla (caso Esq-Dir) ################\n";
    { auto t = makeAVL(); for (int k : {30, 10}) t->insert(k);
      std::cout << "\nANTES (30, 10):\n"; t->print(std::cout);
      step(*t, 20); }

    std::cout << "\n################ AVL: sequencia 1..7 em ordem crescente ################\n";
    { auto t = makeAVL(); for (int k = 1; k <= 7; ++k) step(*t, k); }

    std::cout << "\n################ Rubro-Negra: tio vermelho -> recolorir ################\n";
    { auto t = makeRedBlack(); for (int k : {10, 5, 15}) t->insert(k);
      std::cout << "\nANTES (10, 5, 15 | pai e tio vermelhos):\n"; t->print(std::cout);
      step(*t, 1); }

    std::cout << "\n################ Rubro-Negra: tio preto -> rotacao + recolorir ################\n";
    { auto t = makeRedBlack(); for (int k : {10, 20}) t->insert(k);
      std::cout << "\nANTES (10, 20):\n"; t->print(std::cout);
      step(*t, 30); }

    std::cout << "\n################ Rubro-Negra: sequencia 1..7 em ordem crescente ################\n";
    { auto t = makeRedBlack(); for (int k = 1; k <= 7; ++k) step(*t, k); }

    std::cout << "\n################ Remocao (AVL e Rubro-Negra) ################\n";
    for (auto make : {makeAVL, makeRedBlack}) {
        auto t = make();
        for (int k : {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}) t->insert(k);
        std::cout << "\n[" << t->name() << "] arvore inicial (1..15):\n"; t->print(std::cout);
        for (int k : {1, 3, 8}) {
            t->resetStats(); t->remove(k);
            std::cout << "\n[" << t->name() << "] apos remover " << k << " (rotacoes: " << t->stats().rotations
                      << ", recolorizacoes: " << t->stats().recolorings << "):\n";
            t->print(std::cout);
        }
    }
    return 0;
}
