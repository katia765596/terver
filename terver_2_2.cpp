#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>
#include <cstdlib>
int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " K N\n";
            std::cerr << "  K number of experiments, N\n";
            return 1;
        }
        int K = std::atoi(argv[1]);
        if (K <= 0) {
            std::cerr << "K must be a positive integer.\n";
            return 1;
        }
        int n = std::atoi(argv[2]);
        if (n <= 0) {
            std::cerr << "n must be a positive integer.\n";
            return 1;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<int> attempts;
        attempts.reserve(K);
        for (int exp = 0; exp < K; ++exp) {
            std::vector<int> keys(n);
            for (int i = 0; i < n; ++i) keys[i] = i;
            std::shuffle(keys.begin(), keys.end(), gen);
            int pos = 0;
            for (int i = 0; i < n; ++i) {
                if (keys[i] == 0) {
                    pos = i;
                    break;
                }
            }
            int attempt = pos + 1;
            attempts.push_back(attempt);
            std::cout << "Experiment " << exp + 1 << ": key found at attempt " << attempt << "\n";
        }
        std::vector<int> counts(n + 1, 0);
        for (int a : attempts) {
            counts[a]++;
        }
        std::cout << "\nEmpirical probabilities:\n";
        for (int i = 1; i <= n; ++i) {
            double prob = static_cast<double>(counts[i]) / K;
            std::cout << "P(attempt = " << i << ") = " << std::fixed << std::setprecision(6) << prob << "\n";
        }
        std::cout << "\nTheoretical probability for each attempt: " << 1.0 / n << "\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Unk error\n";
        return 1;
    }
} 