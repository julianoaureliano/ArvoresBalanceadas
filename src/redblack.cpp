// redblack.cpp - Árvore Rubro-Negra implementada do zero (algoritmo de CLRS).
//
// Propriedades:
//   1. Todo nó é vermelho ou preto.
//   2. A raiz é preta.
//   3. Folhas (nil) são pretas.
//   4. Filho de nó vermelho é preto (não há dois vermelhos seguidos).
//   5. Todo caminho de um nó até suas folhas tem o mesmo número de nós pretos.
//
// Usa um nó sentinela 'nil_' (preto) no lugar de nullptr, o que simplifica o
// conserto da remoção. Contadores: stats_.rotations e stats_.recolorings.
#include "tree.hpp"
#include <algorithm>
#include <functional>

namespace {

enum Color { RED, BLACK };

struct Node {
    int key = 0;
    Color c = BLACK;
    Node *l = nullptr, *r = nullptr, *p = nullptr;
};

class RB : public ITree {
    Node* nil_;
    Node* root_;
    std::size_t n_ = 0;

    void setColor(Node* x, Color c) {
        if (x != nil_ && x->c != c) ++stats_.recolorings;
        x->c = c;
    }

    // ---- rotações -------------------------------------------------------
    void rotL(Node* x) {
        Node* y = x->r;
        x->r = y->l;
        if (y->l != nil_) y->l->p = x;
        y->p = x->p;
        if (x->p == nil_) root_ = y;
        else if (x == x->p->l) x->p->l = y;
        else x->p->r = y;
        y->l = x;
        x->p = y;
        ++stats_.rotations;
    }
    void rotR(Node* y) {
        Node* x = y->l;
        y->l = x->r;
        if (x->r != nil_) x->r->p = y;
        x->p = y->p;
        if (y->p == nil_) root_ = x;
        else if (y == y->p->l) y->p->l = x;
        else y->p->r = x;
        x->r = y;
        y->p = x;
        ++stats_.rotations;
    }

    // ---- inserção -------------------------------------------------------
    void insertFix(Node* z) {
        while (z->p->c == RED) {
            Node* g = z->p->p; // avô existe: pai vermelho nunca é a raiz
            if (z->p == g->l) {
                Node* u = g->r; // tio
                if (u->c == RED) {            // caso 1: tio vermelho -> recolorir
                    setColor(z->p, BLACK);
                    setColor(u, BLACK);
                    setColor(g, RED);
                    z = g;
                } else {
                    if (z == z->p->r) { z = z->p; rotL(z); } // caso 2: triângulo
                    setColor(z->p, BLACK);                   // caso 3: linha
                    setColor(g, RED);
                    rotR(g);
                }
            } else {                              // simétrico
                Node* u = g->l;
                if (u->c == RED) {
                    setColor(z->p, BLACK);
                    setColor(u, BLACK);
                    setColor(g, RED);
                    z = g;
                } else {
                    if (z == z->p->l) { z = z->p; rotR(z); }
                    setColor(z->p, BLACK);
                    setColor(g, RED);
                    rotL(g);
                }
            }
        }
        setColor(root_, BLACK);
    }

    // ---- remoção --------------------------------------------------------
    void transplant(Node* u, Node* v) {
        if (u->p == nil_) root_ = v;
        else if (u == u->p->l) u->p->l = v;
        else u->p->r = v;
        v->p = u->p; // vale mesmo se v == nil_ (CLRS depende disso)
    }
    Node* minimum(Node* x) const { while (x->l != nil_) x = x->l; return x; }

    void deleteFix(Node* x) {
        while (x != root_ && x->c == BLACK) {
            if (x == x->p->l) {
                Node* w = x->p->r; // irmão
                if (w->c == RED) {                         // caso 1
                    setColor(w, BLACK);
                    setColor(x->p, RED);
                    rotL(x->p);
                    w = x->p->r;
                }
                if (w->l->c == BLACK && w->r->c == BLACK) { // caso 2
                    setColor(w, RED);
                    x = x->p;
                } else {
                    if (w->r->c == BLACK) {                 // caso 3
                        setColor(w->l, BLACK);
                        setColor(w, RED);
                        rotR(w);
                        w = x->p->r;
                    }
                    setColor(w, x->p->c);                   // caso 4
                    setColor(x->p, BLACK);
                    setColor(w->r, BLACK);
                    rotL(x->p);
                    x = root_;
                }
            } else {                                        // simétrico
                Node* w = x->p->l;
                if (w->c == RED) {
                    setColor(w, BLACK);
                    setColor(x->p, RED);
                    rotR(x->p);
                    w = x->p->l;
                }
                if (w->r->c == BLACK && w->l->c == BLACK) {
                    setColor(w, RED);
                    x = x->p;
                } else {
                    if (w->l->c == BLACK) {
                        setColor(w->r, BLACK);
                        setColor(w, RED);
                        rotL(w);
                        w = x->p->l;
                    }
                    setColor(w, x->p->c);
                    setColor(x->p, BLACK);
                    setColor(w->l, BLACK);
                    rotR(x->p);
                    x = root_;
                }
            }
        }
        setColor(x, BLACK);
    }

    Node* find(int k) const {
        Node* p = root_;
        while (p != nil_ && p->key != k) p = k < p->key ? p->l : p->r;
        return p;
    }

    void destroy(Node* n) {
        if (n == nil_) return;
        destroy(n->l); destroy(n->r); delete n;
    }

    int hgt(const Node* n) const { return n == nil_ ? 0 : 1 + std::max(hgt(n->l), hgt(n->r)); }

    void pr(std::ostream& os, const Node* n, const std::string& pre, int dir, bool color) const {
        if (n == nil_) return;
        pr(os, n->r, pre + (dir == 2 ? "|   " : "    "), 1, color);
        os << pre << (dir == 0 ? "" : dir == 1 ? "/-- " : "\\-- ");
        if (color) os << (n->c == RED ? "\033[1;31m" : "\033[1;30;47m");
        os << n->key << (n->c == RED ? " [R]" : " [B]");
        if (color) os << "\033[0m";
        os << "\n";
        pr(os, n->l, pre + (dir == 1 ? "|   " : "    "), 2, color);
    }

    // Retorna a altura-negra (nós pretos até a folha) ou -1 se houver erro.
    int chk(const Node* n, long long lo, long long hi, std::string& why) const {
        if (n == nil_) return 1;
        if (n->key <= lo || n->key >= hi) { why = "ordem BST violada em " + std::to_string(n->key); return -1; }
        if (n->c == RED && (n->l->c == RED || n->r->c == RED)) {
            why = "dois nos vermelhos seguidos em " + std::to_string(n->key); return -1;
        }
        if (n->l != nil_ && n->l->p != n) { why = "ponteiro pai inconsistente"; return -1; }
        if (n->r != nil_ && n->r->p != n) { why = "ponteiro pai inconsistente"; return -1; }
        int bl = chk(n->l, lo, n->key, why);
        if (bl < 0) return -1;
        int br = chk(n->r, n->key, hi, why);
        if (br < 0) return -1;
        if (bl != br) { why = "alturas-negras diferentes em " + std::to_string(n->key); return -1; }
        return bl + (n->c == BLACK ? 1 : 0);
    }

public:
    RB() {
        nil_ = new Node();
        nil_->c = BLACK;
        nil_->l = nil_->r = nil_->p = nil_;
        root_ = nil_;
    }
    ~RB() override { destroy(root_); delete nil_; }
    std::string name() const override { return "Rubro-Negra"; }

    bool insert(int k) override {
        Node* y = nil_;
        Node* x = root_;
        while (x != nil_) {
            y = x;
            if (k == x->key) return false; // duplicado
            x = k < x->key ? x->l : x->r;
        }
        Node* z = new Node();
        z->key = k; z->c = RED; z->l = z->r = nil_; z->p = y;
        if (y == nil_) root_ = z;
        else if (k < y->key) y->l = z;
        else y->r = z;
        ++n_;
        insertFix(z);
        return true;
    }

    bool remove(int k) override {
        Node* z = find(k);
        if (z == nil_) return false; // inexistente
        Node* y = z;
        Color yOrig = y->c;
        Node* x;
        if (z->l == nil_) { x = z->r; transplant(z, z->r); }
        else if (z->r == nil_) { x = z->l; transplant(z, z->l); }
        else {
            y = minimum(z->r);           // sucessor em ordem
            yOrig = y->c;
            x = y->r;
            if (y->p == z) x->p = y;
            else {
                transplant(y, y->r);
                y->r = z->r; y->r->p = y;
            }
            transplant(z, y);
            y->l = z->l; y->l->p = y;
            y->c = z->c; // y herda a cor de z (não conta como recoloração)
        }
        delete z;
        --n_;
        if (yOrig == BLACK) deleteFix(x);
        nil_->p = nil_; // limpa resíduo do sentinela
        return true;
    }

    bool contains(int k) const override { return find(k) != nil_; }

    std::vector<int> inorder() const override {
        std::vector<int> out; out.reserve(n_);
        std::function<void(const Node*)> go = [&](const Node* n) {
            if (n == nil_) return;
            go(n->l); out.push_back(n->key); go(n->r);
        };
        go(root_);
        return out;
    }

    void print(std::ostream& os, bool color) const override {
        if (root_ == nil_) { os << "(arvore vazia)\n"; return; }
        pr(os, root_, "", 0, color);
    }
    int height() const override { return hgt(root_); }
    std::size_t size() const override { return n_; }
    void clear() override { destroy(root_); root_ = nil_; n_ = 0; }

    bool validate(std::string& why) const override {
        if (nil_->c != BLACK) { why = "sentinela nao e preto"; return false; }
        if (root_ != nil_ && root_->c != BLACK) { why = "raiz nao e preta"; return false; }
        return chk(root_, -(1LL << 40), (1LL << 40), why) >= 0;
    }
};

} // namespace

std::unique_ptr<ITree> makeRedBlack() { return std::make_unique<RB>(); }
