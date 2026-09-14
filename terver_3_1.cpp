#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <numeric>
bool are_connected(const std::string& left, const std::string& right,
    const std::string& pattern, int n, int k) {
    int a_min = std::max(1, k - n);//вычисляет мин возм кол-во символов, которое можно взять из левого кластера а, тк а+b=k,a<=n,b<=n, то для а max(1,k-n) тк b=k-a<=n,a>=k-n,a>=1
    int a_max = std::min(n, k - 1);//b>=1 a<=k-1, a<=n
    for (int a = a_min; a <= a_max; ++a) {
        int b = k - a;
        if (b < 1 || b > n) continue;
        std::string suffix = left.substr(n - a, a);//суффикс левого кластера длины а, последние а символов, начинает с поз n-a и берет а символов
        std::string prefix = right.substr(0, b);//первые b символов
        if (suffix + prefix == pattern) {
            return true;
        }
    }
    return false;
}
std::string generate_cluster(int n, std::mt19937& gen,
    const std::vector<double>& probs,
    const std::string& alphabet) {//генерирует кластер длины n возвр строку
    std::discrete_distribution<int> dist(probs.begin(), probs.end());//генер r символов алф
    std::string cluster;
    cluster.reserve(n);
    for (int i = 0; i < n; ++i) {
        int idx = dist(gen);
        cluster.push_back(alphabet[idx]);
    }
    return cluster;
}
bool parse_args(int argc, char* argv[],
    int& M, int& n, int& r, int& k, std::string& pattern,
    int& D, int& C,
    std::vector<double>& probs, std::string& alphabet) {
    if (argc < 8) {
        std::cerr << "Usage: " << argv[0]
            << " M n r k pattern D C p0 p1 ... p_(r-1)\n";
        std::cerr << "  M     : number of clusters in a series\n";
        std::cerr << "  n     : length of each cluster\n";
        std::cerr << "  r     : alphabet size (symbols 0..r-1)\n";
        std::cerr << "  k     : pattern length\n";
        std::cerr << "  pattern: string of length k over symbols 0..r-1\n";
        std::cerr << "  D     : number of connected pairs for event (b)\n";
        std::cerr << "  C     : number of experiments\n";
        std::cerr << "  p0..p_{r-1} : probabilities sum=1\n";
        return false;
    }
    M = std::atoi(argv[1]);
    n = std::atoi(argv[2]);
    r = std::atoi(argv[3]);
    k = std::atoi(argv[4]);
    pattern = argv[5];
    D = std::atoi(argv[6]);
    C = std::atoi(argv[7]);
    if (M < 2) { std::cerr << "M must be >= 2\n"; return false; }
    if (n < 1) { std::cerr << "n must be >= 1\n"; return false; }
    if (r < 1 || r > 62) { std::cerr << "r must be 1..62\n"; return false; }
    if (k < 1) { std::cerr << "k must be >= 1\n"; return false; }
    if ((int)pattern.size() != k) { std::cerr << "pattern length != k\n"; return false; }
    if (D < 0 || D > M - 1) { std::cerr << "D must be 0.." << M - 1 << "\n"; return false; }
    if (C <= 0) { std::cerr << "C must be > 0\n"; return false; }
    alphabet.clear();
    for (int i = 0; i < r; ++i) {
        if (i < 10) alphabet.push_back('0' + i);
        else if (i < 36) alphabet.push_back('A' + (i - 10));//заглавные буквы
        else alphabet.push_back('a' + (i - 36));
    }
    for (char ch : pattern) {
        if (alphabet.find(ch) == std::string::npos) {//нпос -маркер что подстроки в строке не найдено
            std::cerr << "Pattern contains symbol '" << ch
                << "' not in alphabet (0.." << r - 1 << ")\n";
            return false;
        }
    }
    int probCount = argc - 8;
    if (probCount == 0) {
        probs.assign(r, 1.0 / r);//если вер нет то равном распр метод из библ вектор заменяет содержимое контейнера новыми данными при этом изменяя размер
    }
    else if (probCount == r) {
        probs.resize(r);
        double sum = 0.0;
        for (int i = 0; i < r; ++i) {
            probs[i] = std::atof(argv[8 + i]);
            if (probs[i] < 0) {
                std::cerr << "Negative probability\n";
                return false;
            }
            sum += probs[i];
        }
        if (std::abs(sum - 1.0) > 1e-9) {
            std::cerr << "Probabilities do not sum to 1\n";
            return false;
        }
    }
    else {
        std::cerr << "Number of probabilities must be 0 or " << r << "\n";
        return false;
    }
    return true;
}
int main(int argc, char* argv[]) {
    int M, n, r, k, D, C;
    std::string pattern;
    std::vector<double> probs;
    std::string alphabet;
    if (!parse_args(argc, argv, M, n, r, k, pattern, D, C, probs, alphabet)) {
        return 1;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    long long count_all_connected = 0;
    long long count_exactly_D = 0;
    long long count_none_connected = 0;
    std::vector<std::string> example_all, example_D, example_none;
    for (int exp = 0; exp < C; ++exp) {
        std::vector<std::string> clusters(M);
        for (int i = 0; i < M; ++i) {
            clusters[i] = generate_cluster(n, gen, probs, alphabet);
        }
        int connected_pairs = 0;
        for (int i = 0; i < M - 1; ++i) {
            if (are_connected(clusters[i], clusters[i + 1], pattern, n, k)) {
                ++connected_pairs;
            }
        }
        if (connected_pairs == M - 1) {
            ++count_all_connected;
            if (example_all.empty()) example_all = clusters;
        }
        if (connected_pairs == D) {
            ++count_exactly_D;
            if (example_D.empty()) example_D = clusters;
        }
        if (connected_pairs == 0) {
            ++count_none_connected;
            if (example_none.empty()) example_none = clusters;
        }
    }
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Parameters: M=" << M << ", n=" << n << ", r=" << r
        << ", k=" << k << ", D=" << D << ", C=" << C << "\n";
    std::cout << "Pattern: " << pattern << "\n";
    std::cout << "Alphabet: " << alphabet << "\n";
    std::cout << "Symbol probabilities: ";
    for (double p : probs) std::cout << p << " ";
    std::cout << "\n";
    double prob_all = static_cast<double>(count_all_connected) / C;
    double prob_D = static_cast<double>(count_exactly_D) / C;
    double prob_none = static_cast<double>(count_none_connected) / C;
    std::cout << "a) Probability that all pairs are connected: "
        << prob_all << "\n";
    std::cout << "b) Probability that exactly " << D << " pairs are connected: "
        << prob_D << "\n";
    std::cout << "c) Probability that no pairs are connected: "
        << prob_none << "\n\n";
    if (!example_all.empty()) {
        std::cout << "Example series for (a) (all connected):\n";
        for (size_t i = 0; i < example_all.size(); ++i) {
            std::cout << "Cluster " << i + 1 << ": " << example_all[i] << "\n";
        }
        std::cout << "\n";
    }
    else {
        std::cout << "(a) No example found in " << C << " experiments.\n\n";
    }
    if (!example_D.empty()) {
        std::cout << "Example series for (b) (exactly " << D << " connected pairs):\n";
        for (size_t i = 0; i < example_D.size(); ++i) {
            std::cout << "Cluster " << i + 1 << ": " << example_D[i] << "\n";
        }
        std::cout << "\n";
    }
    else {
        std::cout << "(b) No example found in " << C << " experiments.\n\n";
    }
    if (!example_none.empty()) {
        std::cout << "Example series for (c) (no connected pairs):\n";
        for (size_t i = 0; i < example_none.size(); ++i) {
            std::cout << "Cluster " << i + 1 << ": " << example_none[i] << "\n";
        }
        std::cout << "\n";
    }
    else {
        std::cout << "(c) No example found in " << C << " experiments.\n";
    }
    return 0;
} 