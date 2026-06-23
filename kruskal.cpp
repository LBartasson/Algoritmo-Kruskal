#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

struct Aresta {
    int u, v, peso;

    bool operator<(const Aresta& outra) const {
        return peso < outra.peso;
    }
};

struct UnionFind {
    vector<int> pai, rank_;

    UnionFind(int n) : pai(n), rank_(n, 0) {
        for (int i = 0; i < n; ++i) pai[i] = i;
    }

    int encontrar(int x) {
        if (pai[x] != x)
            pai[x] = encontrar(pai[x]);
        return pai[x];
    }

    bool unir(int a, int b) {
        int ra = encontrar(a);
        int rb = encontrar(b);
        if (ra == rb) return false;         

        if (rank_[ra] < rank_[rb]) swap(ra, rb);
        pai[rb] = ra;
        if (rank_[ra] == rank_[rb]) ++rank_[ra];
        return true;
    }
};

struct ResultadoKruskal {
    vector<Aresta> mst; 
    int           pesoTotal = 0;
    bool          conectado  = true;
};

ResultadoKruskal kruskal(int V, vector<Aresta>& arestas) {
    sort(arestas.begin(), arestas.end());

    UnionFind uf(V);
    ResultadoKruskal res;

    cout << "\n=== Passo a passo do Algoritmo de Kruskal ===\n";
    cout << left << setw(6)  << "Passo"
         << setw(8)  << "Aresta"
         << setw(8)  << "Peso"
         << "Decisão\n";
    cout << string(45, '-') << "\n";

    int passo = 1;
    for (auto& a : arestas) {
        string aresta_str = "(" + to_string(a.u) + "," + to_string(a.v) + ")";

        if (uf.unir(a.u, a.v)) {
            res.mst.push_back(a);
            res.pesoTotal += a.peso;
            cout << left << setw(6)  << passo
                 << setw(8)  << aresta_str
                 << setw(8)  << a.peso
                 << "ACEITA  → incluída na AGM\n";
        } else {
            cout << left << setw(6)  << passo
                 << setw(8)  << aresta_str
                 << setw(8)  << a.peso
                 << "REJEITADA → formaria ciclo\n";
        }
        ++passo;
        if ((int)res.mst.size() == V - 1) break;
    }

    if ((int)res.mst.size() < V - 1)
        res.conectado = false;

    return res;
}

bool lerGrafo(istream& in, int& V, int& E, vector<Aresta>& arestas) {
    if (!(in >> V >> E) || V <= 0 || E < 0) {
        cerr << "ERRO: cabeçalho inválido (V E).\n";
        return false;
    }
    arestas.resize(E);
    for (int i = 0; i < E; ++i) {
        if (!(in >> arestas[i].u >> arestas[i].v >> arestas[i].peso)) {
            cerr << "ERRO: aresta " << i+1 << " incompleta.\n";
            return false;
        }
        if (arestas[i].u < 0 || arestas[i].u >= V ||
            arestas[i].v < 0 || arestas[i].v >= V) {
            cerr << "ERRO: vértice fora do intervalo [0," << V-1 << "].\n";
            return false;
        }
    }
    return true;
}

void salvarResultado(const string& nomeArquivo,
                     int V, int E,
                     const vector<Aresta>& arestas,
                     const ResultadoKruskal& res) {
    ofstream out(nomeArquivo);
    if (!out) {
        cerr << "Aviso: não foi possível salvar em " << nomeArquivo << "\n";
        return;
    }
    out << "=== RESULTADO DO ALGORITMO DE KRUSKAL ===\n";
    out << "Vértices: " << V << "  |  Arestas do grafo: " << E << "\n\n";

    out << "Arestas da Árvore Geradora Mínima (AGM):\n";
    for (auto& a : res.mst)
        out << "  (" << a.u << ", " << a.v << ")  peso = " << a.peso << "\n";

    out << "\nPeso total da AGM: " << res.pesoTotal << "\n";
    out << "Grafo " << (res.conectado ? "CONECTADO" : "DESCONECTADO") << "\n";

    cout << "\nResultado salvo em: " << nomeArquivo << "\n";
}

void exibirResultado(int V, int E, const ResultadoKruskal& res) {
    cout << "\n========================================\n";
    cout << "  ÁRVORE GERADORA MÍNIMA (AGM)\n";
    cout << "  Algoritmo de Kruskal\n";
    cout << "========================================\n";
    cout << "Grafo: " << V << " vértices, " << E << " arestas\n\n";

    if (!res.conectado) {
        cout << "⚠  O grafo NÃO é conexo. A AGM não existe.\n"
             << "   Floresta geradora mínima retornada abaixo.\n\n";
    }

    cout << "Arestas selecionadas:\n";
    for (size_t i = 0; i < res.mst.size(); ++i) {
        cout << "  " << i+1 << ". ("
             << res.mst[i].u << " -- " << res.mst[i].v
             << ")  peso = " << res.mst[i].peso << "\n";
    }
    cout << "\nPeso total da AGM: " << res.pesoTotal << "\n";
    cout << "========================================\n";
}

int main(int argc, char* argv[]) {
    int V = 0, E = 0;
    vector<Aresta> arestas;
    string arquivoEntrada;

    cout << "╔══════════════════════════════════════════════╗\n";
    cout << "║   Algoritmo de Kruskal — Teoria dos Grafos   ║\n";
    cout << "║   IF Goiano — Campus Rio Verde               ║\n";
    cout << "╚══════════════════════════════════════════════╝\n\n";

    if (argc >= 2) {
        arquivoEntrada = argv[1];
        ifstream fin(arquivoEntrada);
        if (!fin) {
            cerr << "ERRO: arquivo '" << arquivoEntrada << "' não encontrado.\n";
            return 1;
        }
        cout << "Lendo grafo de: " << arquivoEntrada << "\n";
        if (!lerGrafo(fin, V, E, arestas)) return 1;
    } else {
        cout << "Entrada interativa (ou redirecione um arquivo).\n";
        cout << "Formato: primeira linha 'V E', depois E linhas 'u v peso'.\n\n";
        cout << "Digite V e E: ";
        if (!lerGrafo(cin, V, E, arestas)) return 1;
    }

    ResultadoKruskal res = kruskal(V, arestas);

    exibirResultado(V, E, res);

    string arquivoSaida = "resultado_kruskal.txt";
    salvarResultado(arquivoSaida, V, E, arestas, res);

    return 0;
}
