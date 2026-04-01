#include <boost/multiprecision/cpp_int.hpp>
#include <random>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
namespace rsa {
    using namespace boost::multiprecision;
    static const std::vector<cpp_int> SMALL_PRIMES = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
        73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
        157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
        239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
        331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419,
        421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499
    };
    inline int bitLength(const cpp_int& x) {
        if (x == 0) return 1;
        return static_cast<int>(msb(x)) + 1;
    }
    cpp_int integerSqrt(const cpp_int& x) {
        if (x < 0) throw std::invalid_argument("integerSqrt: negative number");
        if (x == 0 || x == 1) return x;
        cpp_int low = 0, high = x;
        while (low <= high) {
            cpp_int mid = (low + high) / 2;
            cpp_int sq = mid * mid;
            if (sq == x) return mid;
            else if (sq < x) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }
        return high;
    }
    bool isPerfectSquare(const cpp_int& x, cpp_int& root) {
        if (x < 0) return false;
        root = integerSqrt(x);
        return (root * root == x);
    }
    cpp_int randomNumber(int bits) {
        if (bits <= 0) return 0;
        static thread_local std::mt19937_64 rng(std::random_device{}());
        cpp_int result = 0;
        const int word_bits = 64;
        int words = (bits + word_bits - 1) / word_bits;
        for (int i = 0; i < words; ++i) {
            uint64_t word = std::uniform_int_distribution<uint64_t>{}(rng);
            result |= (cpp_int(word) << (i * word_bits));
        }
        result &= (cpp_int(1) << bits) - 1;
        result |= (cpp_int(1) << (bits - 1));
        return result;
    }
    inline cpp_int modPow(const cpp_int& base, const cpp_int& exp, const cpp_int& mod) {
        cpp_int result = 1;
        cpp_int b = base % mod;
        cpp_int e = exp;
        while (e > 0) {
            if (e & 1) result = (result * b) % mod;
            b = (b * b) % mod;
            e >>= 1;
        }
        return result;
    }
    cpp_int modInverse(const cpp_int& a, const cpp_int& m) {
        if (m == 1) return 0;
        cpp_int m0 = m, y = 0, x = 1;
        cpp_int a0 = a % m;
        if (a0 == 0) throw std::runtime_error("modInverse: not coprime");
        while (a0 > 1) {
            cpp_int q = a0 / m0;
            cpp_int t = m0;
            m0 = a0 % m0;
            a0 = t;
            t = y;
            y = x - q * y;
            x = t;
        }
        if (x < 0) x += m;
        return x;
    }
    inline bool hasSmallFactor(const cpp_int& n) {
        for (const auto& prime : SMALL_PRIMES) {
            if (prime * prime > n) break;
            if (n % prime == 0) return true;
        }
        return false;
    }
    bool isPrime(const cpp_int& n, int rounds = 12) {
        if (n < 2) return false;
        if (n == 2 || n == 3) return true;
        if (n % 2 == 0) return false;
        if (n < 1000000) {
            for (const auto& prime : SMALL_PRIMES) {
                if (prime == n) return true;
                if (n % prime == 0) return false;
                if (prime * prime > n) break;
            }
            return true;
        }
        if (hasSmallFactor(n)) return false;
        cpp_int d = n - 1;
        int s = 0;
        while (d % 2 == 0) {
            d /= 2;
            ++s;
        }
        for (int i = 0; i < rounds; ++i) {
            cpp_int a;
            do {
                a = randomNumber(bitLength(n)) % (n - 2) + 2;
            } while (a < 2 || a >= n);
            cpp_int x = modPow(a, d, n);
            if (x == 1 || x == n - 1) continue;
            bool composite = true;
            for (int r = 0; r < s - 1; ++r) {
                x = (x * x) % n;
                if (x == n - 1) {
                    composite = false;
                    break;
                }
            }
            if (composite) return false;
        }
        return true;
    }
    cpp_int generatePrime(int bits) {
        if (bits < 2) throw std::invalid_argument("bits must be >= 2");
        const int max_attempts = 1000;
        for (int attempt = 0; attempt < max_attempts; ++attempt) {
            cpp_int candidate = randomNumber(bits);
            if (candidate % 2 == 0) ++candidate;
            bool bad = false;
            for (const auto& prime : SMALL_PRIMES) {
                if (prime > 50) break;
                if (candidate % prime == 0) {
                    bad = true;
                    break;
                }
            }
            if (bad) continue;

            if (isPrime(candidate, 12)) {
                return candidate;
            }
        }
        throw std::runtime_error("Failed to generate prime");
    }
    cpp_int nextPrime(const cpp_int& n) {
        cpp_int candidate = n;
        if (candidate % 2 == 0) ++candidate;
        const int max_steps = 5000;
        for (int step = 0; step < max_steps; ++step) {
            bool bad = false;
            for (const auto& prime : SMALL_PRIMES) {
                if (prime > 50) break;
                if (candidate % prime == 0) {
                    bad = true;
                    break;
                }
            }
            if (!bad && isPrime(candidate, 12)) {
                return candidate;
            }
            candidate += 2;
        }
        throw std::runtime_error("Failed to find next prime");
    }
    inline cpp_int fourthRoot(const cpp_int& x) {
        double d = static_cast<double>(x);
        double r = pow(d, 0.25);
        cpp_int guess = static_cast<cpp_int>(r);
        while ((guess + 1) * (guess + 1) * (guess + 1) * (guess + 1) <= x) ++guess;
        while (guess * guess * guess * guess > x) --guess;
        return guess;
    }
    struct PublicKey {
        cpp_int n;
        cpp_int e;
    };
    struct PrivateKey {
        cpp_int n;
        cpp_int d;
        cpp_int p;
        cpp_int q;
    };
    struct KeyPair {
        PublicKey public_key;
        PrivateKey private_key;
    };
    class RSAService {
    public:
        KeyPair generateFermaVulnerable(int bits = 512) {
            if (bits < 256 || bits % 2 != 0) {
                throw std::invalid_argument("bits must be even and >= 256");
            }
            int half_bits = bits / 2;
            const cpp_int MAX_OFFSET = cpp_int(1) << 12;
            for (int attempt = 0; attempt < 15; ++attempt) {
                cpp_int p = generatePrime(half_bits);
                cpp_int offset = randomNumber(bitLength(MAX_OFFSET)) % MAX_OFFSET + 1;
                cpp_int q_candidate = p + offset;
                if (bitLength(q_candidate) > half_bits) continue;
                cpp_int q = nextPrime(q_candidate);
                if (q == p) continue;
                if (bitLength(q) > half_bits) continue;
                cpp_int n = p * q;
                cpp_int phi = (p - 1) * (q - 1);
                cpp_int e = 65537;
                while (gcd(e, phi) != 1) {
                    e += 2;
                }
                cpp_int d = modInverse(e, phi);
                cpp_int bound = fourthRoot(n) / 3;
                if (d < bound) continue;
                return { {n, e}, {n, d, p, q} };
            }
            throw std::runtime_error("Failed to generate Ferma-vulnerable key");
        }
        KeyPair generateWienerVulnerable(int bits = 512) {
            if (bits < 256 || bits % 2 != 0) {
                throw std::invalid_argument("bits must be even and >= 256");
            }
            int half_bits = bits / 2;
            for (int attempt = 0; attempt < 15; ++attempt) {
                cpp_int p = generatePrime(half_bits);
                cpp_int q = generatePrime(half_bits);
                cpp_int n = p * q;
                cpp_int diff = (p > q) ? p - q : q - p;
                cpp_int bound_diff = fourthRoot(n);
                if (diff < bound_diff) continue;
                cpp_int phi = (p - 1) * (q - 1);
                cpp_int max_d = fourthRoot(n) / 3;
                if (max_d < 2) continue;
                int d_bits = std::max(2, bitLength(max_d) - 4);
                cpp_int d;
                do {
                    d = randomNumber(d_bits) % (max_d - 2) + 2;
                } while (d < 2 || d >= max_d);
                cpp_int e;
                try {
                    e = modInverse(d, phi);
                }
                catch (...) {
                    continue;
                }
                if (e <= 1 || e >= phi) continue;

                return { {n, e}, {n, d, p, q} };
            }
            throw std::runtime_error("Failed to generate Wiener-vulnerable key");
        }
    };
    std::pair<cpp_int, cpp_int> fermaAttack(const cpp_int& n, int max_iterations = 1000000) {
        if (n < 4) throw std::runtime_error("fermaAttack: n is too small");
        cpp_int a = integerSqrt(n);
        if (a * a < n) a += 1;
        cpp_int b2, b;
        int iterations = 0;
        while (iterations < max_iterations) {
            b2 = a * a - n;
            if (b2 < 0) {
                ++a;
                ++iterations;
                continue;
            }
            if (isPerfectSquare(b2, b)) {
                cpp_int p = a - b;
                cpp_int q = a + b;
                if (p > 1 && q > 1 && p * q == n) {
                    if (p > q) std::swap(p, q);
                    return std::make_pair(p, q);
                }
            }
            ++a;
            ++iterations;
        }
        throw std::runtime_error("fermaAttack: failed to factorize n");
    }

} 
#include <iostream>
using namespace rsa;
static bool testSuccess(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "  [OK] " << test_name << "\n";
        return true;
    }
    else {
        std::cout << "  [FAIL] " << test_name << "\n";
        return false;
    }
}
static bool testFermaAttackOnVulnerableKey() {
    try {
        RSAService service;
        KeyPair key = service.generateFermaVulnerable(256);
        std::pair<cpp_int, cpp_int> factors = fermaAttack(key.public_key.n);
        cpp_int p_found = factors.first;
        cpp_int q_found = factors.second;

        bool correct = (p_found == key.private_key.p && q_found == key.private_key.q);
        return testSuccess(correct, "Attack on vulnerable key");
    }
    catch (const std::exception& e) {
        std::cout << "  [FAIL] Attack on vulnerable key: " << e.what() << "\n";
        return false;
    }
}
static bool testFermaAttackOnUnvulnerableKey() {
    try {
        RSAService service;
        KeyPair key = service.generateWienerVulnerable(256);
        std::pair<cpp_int, cpp_int> factors = fermaAttack(key.public_key.n);
        cpp_int p_found = factors.first;
        cpp_int q_found = factors.second;
        bool correct = (p_found * q_found == key.public_key.n);
        return testSuccess(correct, "Attack on Wiener key (may succeed if close)");
    }
    catch (const std::runtime_error&) {
        std::cout << "  [OK] Attack on Wiener key (expected failure)\n";
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "  [FAIL] Attack on Wiener key: " << e.what() << "\n";
        return false;
    }
}
static bool testFermaAttackOnKnownValues() {
    bool all_passed = true;
    try {
        std::pair<cpp_int, cpp_int> factors = fermaAttack(15);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        all_passed &= testSuccess((p == 3 && q == 5) || (p == 5 && q == 3), "n=15 -> 3*5");
    }
    catch (...) {
        all_passed = false;
        std::cout << "  [FAIL] n=15 attack failed\n";
    }
    try {
        std::pair<cpp_int, cpp_int> factors = fermaAttack(35);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        all_passed &= testSuccess((p == 5 && q == 7) || (p == 7 && q == 5), "n=35 -> 5*7");
    }
    catch (...) {
        all_passed = false;
        std::cout << "  [FAIL] n=35 attack failed\n";
    }
    try {
        std::pair<cpp_int, cpp_int> factors = fermaAttack(21);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        all_passed &= testSuccess((p == 3 && q == 7) || (p == 7 && q == 3), "n=21 -> 3*7");
    }
    catch (...) {
        all_passed = false;
        std::cout << "  [FAIL] n=21 attack failed\n";
    }

    return all_passed;
}
static bool testFermaAttackOnPerfectSquare() {
    try {
        std::pair<cpp_int, cpp_int> factors = fermaAttack(49);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        bool correct = (p == 7 && q == 7);
        return testSuccess(correct, "n=49 (perfect square)");
    }
    catch (const std::exception& e) {
        std::cout << "  [FAIL] Perfect square: " << e.what() << "\n";
        return false;
    }
}
static bool testFermaAttackProductCheck() {
    try {
        RSAService service;
        KeyPair key = service.generateFermaVulnerable(256);
        std::pair<cpp_int, cpp_int> factors = fermaAttack(key.public_key.n);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        bool correct = (p * q == key.public_key.n);
        return testSuccess(correct, "Product check");
    }
    catch (const std::exception& e) {
        std::cout << "  [FAIL] Product check: " << e.what() << "\n";
        return false;
    }
}
static bool testFermaAttackPrimeFactors() {
    try {
        RSAService service;
        KeyPair key = service.generateFermaVulnerable(256);
        std::pair<cpp_int, cpp_int> factors = fermaAttack(key.public_key.n);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        bool p_prime = isPrime(p, 8);
        bool q_prime = isPrime(q, 8);
        bool correct = p_prime && q_prime;
        return testSuccess(correct, "Factors are prime");
    }
    catch (const std::exception& e) {
        std::cout << "  [FAIL] Prime factors: " << e.what() << "\n";
        return false;
    }
}
static bool testFermaAttackInvalidInput() {
    bool passed = true;
    try {
        fermaAttack(0);
        std::cout << "  [FAIL] n=0 should throw exception\n";
        passed = false;
    }
    catch (const std::runtime_error&) {
        std::cout << "  [OK] n=0 throws exception\n";
    }
    catch (...) {
        std::cout << "  [FAIL] n=0 throws wrong exception\n";
        passed = false;
    }

    try {
        fermaAttack(1);
        std::cout << "  [FAIL] n=1 should throw exception\n";
        passed = false;
    }
    catch (const std::runtime_error&) {
        std::cout << "  [OK] n=1 throws exception\n";
    }
    catch (...) {
        std::cout << "  [FAIL] n=1 throws wrong exception\n";
        passed = false;
    }

    try {
        fermaAttack(2);
        std::cout << "  [FAIL] n=2 should throw exception\n";
        passed = false;
    }
    catch (const std::runtime_error&) {
        std::cout << "  [OK] n=2 throws exception\n";
    }
    catch (...) {
        std::cout << "  [FAIL] n=2 throws wrong exception\n";
        passed = false;
    }

    return passed;
}
static bool testFermaAttackOrdering() {
    try {
        RSAService service;
        KeyPair key = service.generateFermaVulnerable(256);
        std::pair<cpp_int, cpp_int> factors = fermaAttack(key.public_key.n);
        cpp_int p = factors.first;
        cpp_int q = factors.second;
        bool correct = (p <= q);
        return testSuccess(correct, "p <= q ordering");
    }
    catch (const std::exception& e) {
        std::cout << "  [FAIL] Ordering: " << e.what() << "\n";
        return false;
    }
}
static bool testFermaAttackMultipleKeys() {
    bool all_passed = true;
    for (int i = 0; i < 3; ++i) {
        try {
            RSAService service;
            KeyPair key = service.generateFermaVulnerable(256);
            std::pair<cpp_int, cpp_int> factors = fermaAttack(key.public_key.n);
            cpp_int p = factors.first;
            cpp_int q = factors.second;
            if (p * q != key.public_key.n) {
                all_passed = false;
            }
        }
        catch (const std::exception& e) {
            std::cout << "  [FAIL] Multiple keys test " << i << ": " << e.what() << "\n";
            all_passed = false;
        }
    }
    return testSuccess(all_passed, "Multiple keys (3 attempts)");
}
int main() {
    auto start_time = std::chrono::steady_clock::now();
    bool all_passed = true;
    int passed = 0, total = 0;
    std::cout << "========================================\n";
    std::cout << "Ferma Attack Tests (C++14)\n";
    std::cout << "========================================\n\n";
    std::cout << "Running attack tests...\n\n";
    total++; if (testFermaAttackOnVulnerableKey()) passed++; else all_passed = false;
    total++; if (testFermaAttackOnUnvulnerableKey()) passed++; else all_passed = false;
    total++; if (testFermaAttackOnKnownValues()) passed++; else all_passed = false;
    total++; if (testFermaAttackOnPerfectSquare()) passed++; else all_passed = false;
    total++; if (testFermaAttackProductCheck()) passed++; else all_passed = false;
    total++; if (testFermaAttackPrimeFactors()) passed++; else all_passed = false;
    total++; if (testFermaAttackInvalidInput()) passed++; else all_passed = false;
    total++; if (testFermaAttackOrdering()) passed++; else all_passed = false;
    total++; if (testFermaAttackMultipleKeys()) passed++; else all_passed = false;
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "\n========================================\n";
    std::cout << "Results: " << passed << "/" << total << " tests passed\n";
    std::cout << "Time: " << elapsed / 1000.0 << " seconds\n";
    if (all_passed) {
        std::cout << "\n✓ ALL TESTS PASSED!\n";
        std::cout << "Ferma attack successfully implemented and tested\n";
        std::cout << "========================================\n";
        return 0;
    }
    else {
        std::cout << "\n✗ SOME TESTS FAILED!\n";
        std::cout << "========================================\n";
        return 1;
    }
}