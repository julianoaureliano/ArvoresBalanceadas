// tests.cpp - Testes automatizados. As MESMAS sequências são executadas nas
// duas árvores; após cada operação as invariantes são verificadas (validate()).
#include "../src/tree.hpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <numeric>
#include <random>
#include <set>

static int passed = 0, failed = 0;

#define CHECK(cond, msg)                                                        \
    do {                                                                        \
        if (cond) ++passed;                                                     \
        else { ++failed; std::cout << "    FALHOU: " << msg << "  (" #cond ")\n"; } \
    } while (0)

static void valid(const ITree& t, const std::string& ctx) {
    std::string why;
    bool ok = t.validate(why);
    CHECK(ok, ctx + " -> " + why);
}

static bool sortedUnique(const std::vector<int>& v) {
    for (size_t i = 1; i < v.size(); ++i) if (v[i - 1] >= v[i]) return false;
    return true;
}

using Factory = std::unique_ptr<ITree> (*)();

static void test_vazia_e_um_elemento(Factory f) {
    auto t = f();
    CHECK(t->size() == 0 && t->height() == 0, "vazia: tamanho/altura 0");
    CHECK(!t->contains(1), "busca em vazia");
    CHECK(!t->remove(1), "remocao em vazia retorna false");
    CHECK(t->inorder().empty(), "inorder de vazia");
    valid(*t, "vazia");
    CHECK(t->insert(7), "inserir 1o elemento");
    CHECK(t->size() == 1 && t->height() == 1, "um elemento: altura 1");
    CHECK(t->contains(7) && !t->contains(8), "busca com 1 elemento");
    valid(*t, "um elemento");
    CHECK(t->remove(7), "remover unico elemento");
    CHECK(t->size() == 0 && t->height() == 0, "volta a vazia");
    valid(*t, "apos remover unico");
    CHECK(t->insert(3), "reinsercao apos esvaziar");
}

static void test_duplicados(Factory f) {
    auto t = f();
    t->insert(5); t->insert(3); t->insert(8);
    CHECK(!t->insert(5), "duplicado rejeitado");
    CHECK(!t->insert(3), "duplicado (folha) rejeitado");
    CHECK(t->size() == 3, "tamanho nao muda com duplicado");
    valid(*t, "duplicados");
}

static void test_ordens(Factory f, const char* nome) {
    const int N = 1000;
    std::vector<int> asc(N), desc(N), rnd(N);
    std::iota(asc.begin(), asc.end(), 1);
    desc.assign(asc.rbegin(), asc.rend());
    rnd = asc;
    std::mt19937 g(42);
    std::shuffle(rnd.begin(), rnd.end(), g);
    std::vector<std::pair<const char*, std::vector<int>*>> sets = {{"crescente", &asc}, {"decrescente", &desc}, {"aleatoria", &rnd}};
    for (auto& [label, v] : sets) {
        auto t = f();
        for (int x : *v) { t->insert(x); }
        valid(*t, std::string(nome) + " insercao " + label);
        auto in = t->inorder();
        CHECK((int)in.size() == N && sortedUnique(in), std::string("inorder ordenado - ") + label);
        CHECK(t->height() <= 2 * 10 + 2, std::string("altura logaritmica - ") + label); // ~2*log2(1001)
        for (int x : *v) CHECK(t->contains(x), "busca existente");
        CHECK(!t->contains(0) && !t->contains(N + 1), "busca inexistente");
    }
}

// Constrói árvore de 15 chaves (1..15 em ordem BFS-balanceada) e remove
// folha, nó com 1 filho e nó com 2 filhos (incluindo a raiz).
static void test_remocoes(Factory f) {
    auto mk = [&]() {
        auto t = f();
        for (int x : {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}) t->insert(x);
        return t;
    };
    { auto t = mk(); CHECK(t->remove(1), "remove folha"); valid(*t, "remove folha"); CHECK(!t->contains(1), "folha sumiu"); }
    { auto t = mk(); t->remove(1); t->remove(3);                       // 2 fica sem filhos...
      CHECK(t->remove(4), "remove no com 1 filho (ou 2)"); valid(*t, "remove no interno"); }
    { auto t = mk(); CHECK(t->remove(8), "remove raiz com 2 filhos"); valid(*t, "remove raiz");
      auto in = t->inorder(); CHECK(in.size() == 14 && sortedUnique(in), "inorder pos-remocao raiz"); }
    { auto t = mk(); CHECK(t->remove(12), "remove no com 2 filhos"); valid(*t, "remove 2 filhos"); }
    { auto t = f(); t->insert(10); t->insert(5);                        // no com exatamente 1 filho
      CHECK(t->remove(10), "remove raiz com 1 filho"); valid(*t, "raiz 1 filho");
      CHECK(t->inorder() == std::vector<int>{5}, "resta apenas 5"); }
    { auto t = mk(); CHECK(!t->remove(99), "remove inexistente"); CHECK(t->size() == 15, "tamanho intacto");
      valid(*t, "remove inexistente"); CHECK(!t->remove(0), "remove inexistente (menor)"); }
    { auto t = mk(); // esvazia em ordem crescente, validando a cada passo
      for (int x = 1; x <= 15; ++x) { CHECK(t->remove(x), "esvaziar"); valid(*t, "esvaziando asc " + std::to_string(x)); }
      CHECK(t->size() == 0 && t->height() == 0, "vazia ao final"); }
    { auto t = mk();
      for (int x = 15; x >= 1; --x) { t->remove(x); valid(*t, "esvaziando desc " + std::to_string(x)); } }
}

// Casos que exercitam cada rotação / caso de conserto.
static void test_rotacoes(Factory f) {
    for (auto seq : std::vector<std::vector<int>>{{10, 20, 30}, {30, 20, 10}, {10, 30, 20}, {30, 10, 20}}) {
        auto t = f();
        for (int x : seq) t->insert(x);
        valid(*t, "rotacao 3 nos");
        CHECK(t->height() == 2, "3 nos -> altura 2 apos balancear");
        CHECK(t->stats().rotations >= 1, "houve ao menos uma rotacao");
    }
}

// Teste de estresse: operações aleatórias comparadas com std::set (oráculo),
// validando as invariantes a CADA operação.
static void test_estresse(Factory f, unsigned seed, int ops, int range) {
    auto t = f();
    std::set<int> ref;
    std::mt19937 g(seed);
    bool okAll = true;
    for (int i = 0; i < ops && okAll; ++i) {
        int k = (int)(g() % range), op = (int)(g() % 3);
        if (op == 0) { bool a = t->insert(k), b = ref.insert(k).second; if (a != b) { okAll = false; std::cout << "    divergiu insert\n"; } }
        else if (op == 1) { bool a = t->remove(k), b = ref.erase(k) > 0; if (a != b) { okAll = false; std::cout << "    divergiu remove\n"; } }
        else { if (t->contains(k) != (ref.count(k) > 0)) { okAll = false; std::cout << "    divergiu contains\n"; } }
        std::string why;
        if (!t->validate(why)) { okAll = false; std::cout << "    invariante violada: " << why << " (op " << i << ")\n"; }
    }
    CHECK(okAll, "estresse seed " + std::to_string(seed));
    CHECK(t->size() == ref.size(), "tamanho == oraculo");
    CHECK(t->inorder() == std::vector<int>(ref.begin(), ref.end()), "inorder == oraculo");
}

int main() {
    struct { const char* nome; Factory f; } trees[] = {{"AVL", makeAVL}, {"Rubro-Negra", makeRedBlack}};
    for (auto& [nome, f] : trees) {
        std::cout << "== " << nome << " ==\n";
        int before = failed;
        test_vazia_e_um_elemento(f);   std::cout << "  vazia / um elemento ok\n";
        test_duplicados(f);            std::cout << "  duplicados ok\n";
        test_ordens(f, nome);          std::cout << "  insercao asc/desc/aleatoria ok\n";
        test_remocoes(f);              std::cout << "  remocoes (folha, 1 filho, 2 filhos, inexistente) ok\n";
        test_rotacoes(f);              std::cout << "  rotacoes ok\n";
        for (unsigned s = 1; s <= 20; ++s) test_estresse(f, s, 3000, 200);
        test_estresse(f, 777, 30000, 5000);
        std::cout << "  estresse (21 sementes, validacao a cada operacao) ok\n";
        std::cout << (failed == before ? "  => TODOS PASSARAM\n" : "  => HA FALHAS\n");
    }
    std::cout << "\nVerificacoes: " << passed << " ok, " << failed << " falhas\n";
    return failed ? 1 : 0;
}
