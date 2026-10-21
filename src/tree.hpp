// tree.hpp - Interface comum às árvores balanceadas do projeto.
// Permite que o programa, os testes e o benchmark usem AVL e Rubro-Negra
// exatamente da mesma forma (mesmas sequências de operações).
#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Contadores de esforço de balanceamento (usados na análise comparativa).
struct Stats {
    long long rotations = 0;   // rotações simples (uma rotação dupla conta 2)
    long long recolorings = 0; // trocas de cor (apenas Rubro-Negra)
    void reset() { rotations = recolorings = 0; }
};

class ITree {
public:
    virtual ~ITree() = default;

    virtual std::string name() const = 0;

    // Decisão: valores repetidos NÃO são inseridos (a árvore é um conjunto).
    // Retorna true se inseriu; false se o valor já existia.
    virtual bool insert(int key) = 0;

    // Retorna true se removeu; false se o valor não existia (árvore intacta).
    virtual bool remove(int key) = 0;

    virtual bool contains(int key) const = 0;

    // Percurso em ordem: valores em ordem crescente.
    virtual std::vector<int> inorder() const = 0;

    // Visualização "deitada": raiz à esquerda, filho direito em cima.
    // 'color' habilita códigos ANSI (apenas relevante para Rubro-Negra).
    virtual void print(std::ostream& os, bool color = false) const = 0;

    virtual int height() const = 0;  // altura em nós; árvore vazia = 0
    virtual std::size_t size() const = 0;
    virtual void clear() = 0;

    // Verifica TODAS as invariantes da estrutura (BST + balanceamento).
    // Em caso de violação, devolve false e descreve o problema em 'why'.
    virtual bool validate(std::string& why) const = 0;

    const Stats& stats() const { return stats_; }
    void resetStats() { stats_.reset(); }

protected:
    mutable Stats stats_;
};

std::unique_ptr<ITree> makeAVL();
std::unique_ptr<ITree> makeRedBlack();
