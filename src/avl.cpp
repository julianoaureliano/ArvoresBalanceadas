// avl.cpp - Árvore AVL implementada do zero (sem bibliotecas de árvore).
//
// Invariante AVL: para todo nó, |altura(esq) - altura(dir)| <= 1.
// Após inserir/remover, voltamos pelo caminho da recursão atualizando alturas
// e restaurando o fator de balanceamento com rotações:
//   Esq-Esq  -> rotação simples à direita
//   Dir-Dir  -> rotação simples à esquerda
//   Esq-Dir  -> rotação dupla (esquerda no filho, depois direita no nó)
//   Dir-Esq  -> rotação dupla (direita no filho, depois esquerda no nó)
// Cada rotação simples incrementa stats_.rotations (dupla = 2).
#include "tree.hpp"
#include <algorithm>
#include <cstdlib>
#include <functional>

namespace {

struct Node {
    int key;
    int h = 1; // altura do nó (folha = 1)
    Node *l = nullptr, *r = nullptr;
    explicit Node(int k) : key(k) {}
};

int H(const Node* n) { return n ? n->h : 0; }
int bf(const Node* n) { return n ? H(n->l) - H(n->r) : 0; } // fator de balanceamento
void upd(Node* n) { n->h = 1 + std::max(H(n->l), H(n->r)); }

class AVL : public ITree {
    Node* root_ = nullptr;
    std::size_t n_ = 0;

    // ---- rotações -------------------------------------------------------
    Node* rotR(Node* y) { //      y          x
        Node* x = y->l;   //     / \        / \.
        y->l = x->r;      //    x   C  ->  A   y
        x->r = y;         //   / \            / \.
        upd(y); upd(x);   //  A   B          B   C
        ++stats_.rotations;
        return x;
    }
    Node* rotL(Node* x) {
        Node* y = x->r;
        x->r = y->l;
        y->l = x;
        upd(x); upd(y);
        ++stats_.rotations;
        return y;
    }

    // Restaura a propriedade AVL no nó n (cujos filhos já são AVL).
    Node* rebalance(Node* n) {
        upd(n);
        int b = bf(n);
        if (b > 1) {                       // esquerda mais alta
            if (bf(n->l) < 0) n->l = rotL(n->l); // caso Esq-Dir
            return rotR(n);                       // Esq-Esq
        }
        if (b < -1) {                      // direita mais alta
            if (bf(n->r) > 0) n->r = rotR(n->r); // caso Dir-Esq
            return rotL(n);                       // Dir-Dir
        }
        return n;
    }

    Node* ins(Node* n, int k, bool& ok) {
        if (!n) { ok = true; return new Node(k); }
        if (k < n->key) n->l = ins(n->l, k, ok);
        else if (k > n->key) n->r = ins(n->r, k, ok);
        else { ok = false; return n; } // duplicado: nada muda
        return ok ? rebalance(n) : n;
    }

    Node* rem(Node* n, int k, bool& ok) {
        if (!n) { ok = false; return nullptr; } // inexistente
        if (k < n->key) n->l = rem(n->l, k, ok);
        else if (k > n->key) n->r = rem(n->r, k, ok);
        else {
            ok = true;
            if (!n->l || !n->r) {            // 0 ou 1 filho
                Node* c = n->l ? n->l : n->r;
                delete n;
                return c;
            }
            Node* s = n->r;                  // 2 filhos: sucessor em ordem
            while (s->l) s = s->l;
            n->key = s->key;
            bool dummy;
            n->r = rem(n->r, s->key, dummy);
        }
        return ok ? rebalance(n) : n;
    }

    void destroy(Node* n) { if (n) { destroy(n->l); destroy(n->r); delete n; } }

    void pr(std::ostream& os, const Node* n, const std::string& pre, int dir) const {
        // dir: 0 raiz, 1 = filho direito (impresso acima), 2 = filho esquerdo
        if (!n) return;
        pr(os, n->r, pre + (dir == 2 ? "|   " : "    "), 1);
        os << pre << (dir == 0 ? "" : dir == 1 ? "/-- " : "\\-- ") << n->key
           << " (fb=" << bf(n) << ")\n";
        pr(os, n->l, pre + (dir == 1 ? "|   " : "    "), 2);
    }

    // retorna altura; marca erro se invariantes quebradas
    int chk(const Node* n, long long lo, long long hi, std::string& why, bool& ok) const {
        if (!n || !ok) return 0;
        if (n->key <= lo || n->key >= hi) { ok = false; why = "ordem BST violada em " + std::to_string(n->key); return 0; }
        int hl = chk(n->l, lo, n->key, why, ok);
        int hr = chk(n->r, n->key, hi, why, ok);
        if (!ok) return 0;
        if (n->h != 1 + std::max(hl, hr)) { ok = false; why = "altura armazenada incorreta em " + std::to_string(n->key); }
        else if (std::abs(hl - hr) > 1) { ok = false; why = "fator de balanceamento fora de [-1,1] em " + std::to_string(n->key); }
        return 1 + std::max(hl, hr);
    }

public:
    ~AVL() override { destroy(root_); }
    std::string name() const override { return "AVL"; }

    bool insert(int k) override { bool ok = false; root_ = ins(root_, k, ok); if (ok) ++n_; return ok; }
    bool remove(int k) override { bool ok = false; root_ = rem(root_, k, ok); if (ok) --n_; return ok; }
    bool contains(int k) const override {
        for (const Node* p = root_; p;) {
            if (k == p->key) return true;
            p = k < p->key ? p->l : p->r;
        }
        return false;
    }
    std::vector<int> inorder() const override {
        std::vector<int> out; out.reserve(n_);
        std::function<void(const Node*)> go = [&](const Node* n) {
            if (!n) return;
            go(n->l); out.push_back(n->key); go(n->r);
        };
        go(root_);
        return out;
    }
    void print(std::ostream& os, bool) const override {
        if (!root_) { os << "(arvore vazia)\n"; return; }
        pr(os, root_, "", 0);
    }
    int height() const override { return H(root_); }
    std::size_t size() const override { return n_; }
    void clear() override { destroy(root_); root_ = nullptr; n_ = 0; }
    bool validate(std::string& why) const override {
        bool ok = true;
        chk(root_, -(1LL << 40), (1LL << 40), why, ok);
        return ok;
    }
};

} // namespace

std::unique_ptr<ITree> makeAVL() { return std::make_unique<AVL>(); }
