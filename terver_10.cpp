#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>
#include <queue>
#include <functional>
#include <set>
#include <map>
#include <chrono>
#include <stdexcept>
#include <iomanip>
#include <cassert>
using namespace std;
struct Graph {
    int n;                              
    vector<vector<bool>> adj;           
    vector<vector<int>> weight;         
    Graph(int vertices) : n(vertices),
        adj(vertices, vector<bool>(vertices, false)),
        weight(vertices, vector<int>(vertices, 0)) {
    }
    void add_edge(int u, int v, int w) {
        if (u == v) return;
        adj[u][v] = adj[v][u] = true;
        weight[u][v] = weight[v][u] = w;
    }
    bool has_edge(int u, int v) const {
        return adj[u][v];
    }
    int get_weight(int u, int v) const {
        return weight[u][v];
    }
    vector<int> neighbours(int u) const {
        vector<int> res;
        for (int v = 0; v < n; ++v)
            if (adj[u][v]) res.push_back(v);
        return res;
    }
    int degree(int u) const {
        return count(adj[u].begin(), adj[u].end(), true);
    }
};
Graph generate_random_graph(int n, double p, mt19937& rng) {
    Graph g(n);
    uniform_real_distribution<double> prob_dist(0.0, 1.0);
    uniform_int_distribution<int> weight_dist(1, 10);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (prob_dist(rng) < p) {
                g.add_edge(i, j, weight_dist(rng));
            }
        }
    }
    return g;
}
Graph generate_connected_random_graph(int n, double p, mt19937& rng) {
    Graph g(n);
    uniform_int_distribution<int> weight_dist(1, 10);
    uniform_real_distribution<double> prob_dist(0.0, 1.0);
    vector<bool> in_tree(n, false);
    vector<int> min_weight(n, numeric_limits<int>::max());
    vector<int> parent(n, -1);
    min_weight[0] = 0;
    for (int i = 0; i < n; ++i) {
        int u = -1;
        for (int v = 0; v < n; ++v) {
            if (!in_tree[v] && (u == -1 || min_weight[v] < min_weight[u]))
                u = v;
        }
        assert(u != -1);
        in_tree[u] = true;
        if (parent[u] != -1) {
            int w = weight_dist(rng);
            g.add_edge(u, parent[u], w);
        }
        for (int v = 0; v < n; ++v) {
            if (!in_tree[v]) {
                int rw = weight_dist(rng);
                if (rw < min_weight[v]) {
                    min_weight[v] = rw;
                    parent[v] = u;
                }
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (!g.has_edge(i, j) && prob_dist(rng) < p) {
                g.add_edge(i, j, weight_dist(rng));
            }
        }
    }
    return g;
}
bool is_connected(const Graph& g) {
    if (g.n == 0) return true;
    vector<bool> visited(g.n, false);
    queue<int> q;
    q.push(0);
    visited[0] = true;
    int count = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        ++count;
        for (int v : g.neighbours(u)) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    return count == g.n;
}
long long compute_A(const Graph& g) {
    int n = g.n;
    vector<bool> in_mst(n, false);
    vector<int> key(n, numeric_limits<int>::max());
    key[0] = 0;
    long long total_weight = 0;
    for (int i = 0; i < n; ++i) {
        int u = -1;
        int min_key = numeric_limits<int>::max();
        for (int v = 0; v < n; ++v) {
            if (!in_mst[v] && key[v] < min_key) {
                min_key = key[v];
                u = v;
            }
        }
        assert(u != -1);
        in_mst[u] = true;
        total_weight += key[u];
        for (int v = 0; v < n; ++v) {
            if (!in_mst[v] && g.has_edge(u, v)) {
                int w = g.get_weight(u, v);
                if (w < key[v]) {
                    key[v] = w;
                }
            }
        }
    }
    return total_weight;
}
void dfs_cycles(const Graph& g, int start, int u, vector<bool>& visited,
    vector<int>& path, vector<vector<int>>& all_cycles) {
    visited[u] = true;
    path.push_back(u);
    for (int v : g.neighbours(u)) {
        if (v == start && path.size() >= 3) {
            all_cycles.push_back(path);
        }
        else if (!visited[v]) {
            dfs_cycles(g, start, v, visited, path, all_cycles);
        }
    }
    path.pop_back();
    visited[u] = false;
}
vector<vector<int>> find_all_cycles(const Graph& g) {
    vector<vector<int>> cycles;
    int n = g.n;
    for (int start = 0; start < n; ++start) {
        vector<bool> visited(n, false);
        vector<int> path;
        dfs_cycles(g, start, start, visited, path, cycles);
    }
    return cycles;
}
int compute_B(const Graph& g) {
    auto cycles = find_all_cycles(g);
    int max_len = 0;
    for (const auto& cycle : cycles) {
        int len = (int)cycle.size();
        if (len > max_len) max_len = len;
    }
    return max_len;
}
int compute_D(const Graph& g) {
    auto cycles = find_all_cycles(g);
    long long max_weight = -1;
    int best_edge_count = 0;
    for (const auto& cycle : cycles) {
        long long sum = 0;
        int m = (int)cycle.size();
        for (int i = 0; i < m; ++i) {
            int u = cycle[i];
            int v = cycle[(i + 1) % m];
            sum += g.get_weight(u, v);
        }
        if (sum > max_weight) {
            max_weight = sum;
            best_edge_count = m;
        }
    }
    return best_edge_count;
}
int compute_E(const Graph& g) {
    int isolated = 0;
    for (int i = 0; i < g.n; ++i) {
        if (g.degree(i) == 0) ++isolated;
    }
    return isolated;
}
long long compute_F(const Graph& g) {
    if (!is_connected(g)) return 0;
    int n = g.n;
    if (n == 1) return 1;
    vector<vector<long long>> laplacian(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i) {
        laplacian[i][i] = g.degree(i);
        for (int j = 0; j < n; ++j) {
            if (i != j && g.has_edge(i, j)) {
                laplacian[i][j] = -1;
            }
        }
    }
    int sz = n - 1;
    vector<vector<long long>> matrix(sz, vector<long long>(sz));
    for (int i = 0; i < sz; ++i) {
        for (int j = 0; j < sz; ++j) {
            matrix[i][j] = laplacian[i][j];
        }
    }
    long long det = 1;
    for (int i = 0; i < sz; ++i) {
        int pivot = i;
        while (pivot < sz && matrix[pivot][i] == 0) ++pivot;
        if (pivot == sz) {
            return 0;
        }
        if (pivot != i) {
            swap(matrix[i], matrix[pivot]);
            det = -det;
        }
        det *= matrix[i][i];
        for (int j = i + 1; j < sz; ++j) {
            long long factor = matrix[j][i];
            if (factor == 0) continue;
            for (int k = i; k < sz; ++k) {
                matrix[j][k] = matrix[j][k] * matrix[i][i] - matrix[i][k] * factor;
            }
        }
    }
    return det;
}
int compute_G(const Graph& g) {
    int n = g.n;
    vector<bool> visited(n, false);
    int components = 0;
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            ++components;
            queue<int> q;
            q.push(i);
            visited[i] = true;
            while (!q.empty()) {
                int u = q.front(); q.pop();
                for (int v : g.neighbours(u)) {
                    if (!visited[v]) {
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }
        }
    }
    return components;
}
int compute_H(const Graph& g) {
    int n = g.n;
    vector<bool> visited(n, false);
    int count = 0;
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            vector<int> comp;
            queue<int> q;
            q.push(i);
            visited[i] = true;
            while (!q.empty()) {
                int u = q.front(); q.pop();
                comp.push_back(u);
                for (int v : g.neighbours(u)) {
                    if (!visited[v]) {
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }
            int sz = (int)comp.size();
            bool complete = true;
            for (int a = 0; a < sz && complete; ++a) {
                for (int b = a + 1; b < sz; ++b) {
                    if (!g.has_edge(comp[a], comp[b])) {
                        complete = false;
                        break;
                    }
                }
            }
            if (complete) ++count;
        }
    }
    return count;
}
struct Stats {
    double mean;
    double variance;  
};
template<typename Func>
Stats compute_stats(int n, double p, int num_samples, Func value_func, mt19937& rng, bool need_connected) {
    vector<double> samples;
    samples.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        Graph g = need_connected
            ? generate_connected_random_graph(n, p, rng)
            : generate_random_graph(n, p, rng);
        double val = static_cast<double>(value_func(g));
        samples.push_back(val);
    }
    double sum = accumulate(samples.begin(), samples.end(), 0.0);
    double mean = sum / num_samples;
    double sq_sum = 0.0;
    for (double x : samples) {
        sq_sum += (x - mean) * (x - mean);
    }
    double variance = (num_samples > 1) ? sq_sum / (num_samples - 1) : 0.0;
    return { mean, variance };
}
int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <n> <num_samples> [p] [seed]\n";
        cerr << "  n            - number of vertices (>=2)\n";
        cerr << "  num_samples  - number of graphs to generate\n";
        cerr << "  p            - edge probability (default 0.5)\n";
        cerr << "  seed         - RNG seed (default random)\n";
        return 1;
    }
    int n = stoi(argv[1]);
    int num_samples = stoi(argv[2]);
    if (n < 2) {
        cerr << "Error: n must be >= 2\n";
        return 1;
    }
    if (num_samples <= 0) {
        cerr << "Error: num_samples must be positive\n";
        return 1;
    }
    double p = 0.5;
    if (argc >= 4) {
        p = stod(argv[3]);
        if (p < 0.0 || p > 1.0) {
            cerr << "Error: p must be in [0,1]\n";
            return 1;
        }
    }
    unsigned seed;
    if (argc >= 5) {
        seed = static_cast<unsigned>(stoul(argv[4]));
    }
    else {
        seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
    }
    mt19937 rng(seed);

    cout << "Parameters: n = " << n << ", p = " << p << ", samples = " << num_samples << ", seed = " << seed << "\n\n";

    if (n > 20) {
        cout << "Warning: n > 20, cycle enumeration may take a long time.\n";
    }
    Stats stats_A = compute_stats(n, p, num_samples, compute_A, rng, true);
    Stats stats_B = compute_stats(n, p, num_samples, compute_B, rng, false);
    Stats stats_D = compute_stats(n, p, num_samples, compute_D, rng, false);
    Stats stats_E = compute_stats(n, p, num_samples, compute_E, rng, false);
    Stats stats_F = compute_stats(n, p, num_samples, compute_F, rng, false);
    Stats stats_G = compute_stats(n, p, num_samples, compute_G, rng, false);
    Stats stats_H = compute_stats(n, p, num_samples, compute_H, rng, false);
    cout << fixed << setprecision(6);
    cout << "Variable A (MST total weight):\n";
    cout << "  Mean     ~ " << stats_A.mean << "\n";
    cout << "  Variance ~ " << stats_A.variance << "\n\n";
    cout << "Variable B (max cycle length by edges):\n";
    cout << "  Mean     ~ " << stats_B.mean << "\n";
    cout << "  Variance ~ " << stats_B.variance << "\n\n";
    cout << "Variable D (edge count of cycle with max total weight):\n";
    cout << "  Mean     ~ " << stats_D.mean << "\n";
    cout << "  Variance ~ " << stats_D.variance << "\n\n";
    cout << "Variable E (number of isolated vertices):\n";
    cout << "  Mean     ~ " << stats_E.mean << "\n";
    cout << "  Variance ~ " << stats_E.variance << "\n\n";
    cout << "Variable F (number of spanning trees):\n";
    cout << "  Mean     ~ " << stats_F.mean << "\n";
    cout << "  Variance ~ " << stats_F.variance << "\n\n";
    cout << "Variable G (number of connected components):\n";
    cout << "  Mean     ~ " << stats_G.mean << "\n";
    cout << "  Variance ~ " << stats_G.variance << "\n\n";
    cout << "Variable H (number of complete components):\n";
    cout << "  Mean     ~ " << stats_H.mean << "\n";
    cout << "  Variance ~ " << stats_H.variance << "\n";
    return 0;
}