// compare.cpp - Análise comparativa AVL x Rubro-Negra (x BST comum como referência).
// Os MESMOS conjuntos de dados e sequências de operações são aplicados às estruturas.
// Saída em Markdown (tabelas) pronta para uso nos slides.
//
// Uso: ./build/compare [seed]
#include "../src/tree.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <numeric>
#include <random>

// BST comum (sem balanceamento) apenas para evidenciar o problema da ordem de chegada.
struct PlainBST {
    struct N { int k; N *l = nullptr, *r = nullptr; };
    N* root = nullptr;
    void insert(int k) {
        N** p = &root;
        while (*p) { if (k == (*p)->k) return; p = k < (*p)->k ? &(*p)->l : &(*p)->r; }
        *p = new N{k};
    }
    int height() const { // iterativo por níveis (evita estouro de pilha)
        if (!root) return 0;
        std::vector<N*> cur{root}; int h = 0;
        while (!cur.empty()) { ++h; std::vector<N*> nx; for (N* n : cur) { if (n->l) nx.push_back(n->l); if (n->r) nx.push_back(n->r); } cur.swap(nx); }
        return h;
    }
    ~PlainBST() { std::vector<N*> st; if (root) st.push_back(root); while (!st.empty()) { N* n = st.back(); st.pop_back(); if (n->l) st.push_back(n->l); if (n->r) st.push_back(n->r); delete n; } }
};

using Clock = std::chrono::steady_clock;
static double ms(Clock::time_point a, Clock::time_point b) { return std::chrono::duration<double, std::milli>(b - a).count(); }

struct Result {
    int hIns, hRem; long long rotIns, colIns, rotRem, colRem; double tIns, tRem; bool valid;
};

static Result run(ITree& t, const std::vector<int>& ins, const std::vector<int>& rem) {
    Result r{};
    t.resetStats();
    auto a = Clock::now();
    for (int x : ins) t.insert(x);
    auto b = Clock::now();
    r.hIns = t.height(); r.rotIns = t.stats().rotations; r.colIns = t.stats().recolorings; r.tIns = ms(a, b);
    t.resetStats();
    a = Clock::now();
    for (int x : rem) t.remove(x);
    b = Clock::now();
    r.hRem = t.height(); r.rotRem = t.stats().rotations; r.colRem = t.stats().recolorings; r.tRem = ms(a, b);
    std::string why; r.valid = t.validate(why);
    return r;
}

int main(int argc, char** argv) {
    unsigned seed = argc > 1 ? (unsigned)std::atoi(argv[1]) : 2024;
    std::mt19937 g(seed);
    std::cout << "# Resultados da analise comparativa (seed=" << seed << ")\n\n";
    std::cout << "Remocao: metade dos elementos (embaralhados), mesma sequencia nas duas arvores.\n"
                 "Altura em nos (arvore vazia = 0). Rotacao dupla conta 2.\n\n";

    for (int N : {1000, 10000, 100000}) {
        std::vector<int> asc(N);
        std::iota(asc.begin(), asc.end(), 1);
        std::vector<int> desc(asc.rbegin(), asc.rend());
        std::vector<int> rnd = asc; std::shuffle(rnd.begin(), rnd.end(), g);
        // "Black Friday": lotes de IDs sequenciais chegando em ordem de lote embaralhada.
        std::vector<int> burst; {
            const int B = std::max(10, N / 50);
            std::vector<int> starts; for (int s = 0; s < N; s += B) starts.push_back(s);
            std::shuffle(starts.begin(), starts.end(), g);
            for (int s : starts) for (int i = s; i < std::min(N, s + B); ++i) burst.push_back(asc[i]);
        }
        std::vector<std::pair<std::string, std::vector<int>*>> sets = {
            {"crescente", &asc}, {"decrescente", &desc}, {"aleatoria", &rnd}, {"lotes (Black Friday)", &burst}};

        std::vector<int> remSeq = asc; std::shuffle(remSeq.begin(), remSeq.end(), g); remSeq.resize(N / 2);

        std::cout << "## N = " << N << "\n\n";
        std::cout << "| Entrada | Estrutura | Altura apos insercao | Rotacoes (ins) | Recolor. (ins) | Altura apos remocao de N/2 | Rotacoes (rem) | Recolor. (rem) | Tempo ins (ms) | Tempo rem (ms) | Valida |\n";
        std::cout << "|---|---|---|---|---|---|---|---|---|---|---|\n";
        for (auto& [label, v] : sets) {
            for (auto make : {makeAVL, makeRedBlack}) {
                auto t = make();
                Result r = run(*t, *v, remSeq);
                std::cout << std::fixed << std::setprecision(2)
                          << "| " << label << " | " << t->name() << " | " << r.hIns << " | " << r.rotIns << " | "
                          << (t->name() == "AVL" ? std::string("-") : std::to_string(r.colIns)) << " | "
                          << r.hRem << " | " << r.rotRem << " | "
                          << (t->name() == "AVL" ? std::string("-") : std::to_string(r.colRem)) << " | "
                          << r.tIns << " | " << r.tRem << " | " << (r.valid ? "sim" : "NAO") << " |\n";
            }
            // referência: BST sem balanceamento (limitada a N<=10000 por custo O(n^2) no pior caso)
            if (N <= 10000) {
                PlainBST b; for (int x : *v) b.insert(x);
                std::cout << "| " << label << " | BST comum (ref.) | " << b.height() << " | - | - | - | - | - | - | - | - |\n";
            }
        }
        std::cout << "\nLimites teoricos: altura AVL <= 1.44*log2(N+2) = " << std::setprecision(1)
                  << 1.44 * std::log2(N + 2.0) << "; altura RB <= 2*log2(N+1) = " << 2 * std::log2(N + 1.0)
                  << "; minimo possivel = ceil(log2(N+1)) = " << (int)std::ceil(std::log2(N + 1.0)) << ".\n\n";
    }

    // Media de alturas em varias sementes (N=10000, entrada aleatoria)
    std::cout << "## Altura media em 30 execucoes aleatorias (N = 10000)\n\n| Estrutura | Altura media | Altura max | Rotacoes medias (ins) |\n|---|---|---|---|\n";
    for (auto make : {makeAVL, makeRedBlack}) {
        double sh = 0, sr = 0; int mx = 0; std::string nm;
        for (int rep = 0; rep < 30; ++rep) {
            std::mt19937 gg(1000 + rep);
            std::vector<int> v(10000); std::iota(v.begin(), v.end(), 1); std::shuffle(v.begin(), v.end(), gg);
            auto t = make(); nm = t->name();
            for (int x : v) t->insert(x);
            sh += t->height(); sr += (double)t->stats().rotations; mx = std::max(mx, t->height());
        }
        std::cout << "| " << nm << " | " << std::setprecision(2) << std::fixed << sh / 30 << " | " << mx << " | " << sr / 30 << " |\n";
    }
    return 0;
}
