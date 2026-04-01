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
    bool isPrime(const cpp_int& n, int rounds = 8) {
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

            if (isPrime(candidate, 8)) {
                return candidate;
            }
        }
        throw std::runtime_error("Failed to generate prime");
    }
    cpp_int nextPrime(const cpp_int& n) {
        cpp_int candidate = n;
        if (candidate % 2 == 0) ++candidate;
        const int max_steps = 3000;
        for (int step = 0; step < max_steps; ++step) {
            bool bad = false;
            for (const auto& prime : SMALL_PRIMES) {
                if (prime > 50) break;
                if (candidate % prime == 0) {
                    bad = true;
                    break;
                }
            }
            if (!bad && isPrime(candidate, 8)) {
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

}
#include <iostream>
using namespace rsa;
static bool testEncryptDecrypt(const KeyPair& keys) {
    cpp_int msg = randomNumber(32) % keys.public_key.n;
    cpp_int encrypted = modPow(msg, keys.public_key.e, keys.public_key.n);
    cpp_int decrypted = modPow(encrypted, keys.private_key.d, keys.public_key.n);
    return msg == decrypted;
}
static bool testFerma_KeyCorrectness() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        if (key.public_key.n == 0 || key.public_key.e == 0) return false;
        if (key.private_key.n == 0 || key.private_key.d == 0) return false;
        if (key.private_key.p == 0 || key.private_key.q == 0) return false;
        if (key.public_key.n != key.private_key.p * key.private_key.q) return false;
        cpp_int phi = (key.private_key.p - 1) * (key.private_key.q - 1);
        if ((key.public_key.e * key.private_key.d) % phi != 1) return false;
        return true;
    }
    catch (...) { return false; }
}
static bool testFerma_VulnerabilityCondition() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        cpp_int diff = (key.private_key.p > key.private_key.q) ?
            key.private_key.p - key.private_key.q :
            key.private_key.q - key.private_key.p;
        cpp_int bound = fourthRoot(key.public_key.n);
        return diff < bound;
    }
    catch (...) { return false; }
}
static bool testFerma_NoWienerVulnerability() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        cpp_int bound = fourthRoot(key.public_key.n) / 3;
        return key.private_key.d >= bound;
    }
    catch (...) { return false; }
}
static bool testFerma_EncryptionDecryption() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        return testEncryptDecrypt(key);
    }
    catch (...) { return false; }
}
static bool testWiener_KeyCorrectness() {
    try {
        RSAService service;
        auto key = service.generateWienerVulnerable(256);
        if (key.public_key.n == 0 || key.public_key.e == 0) return false;
        if (key.private_key.n == 0 || key.private_key.d == 0) return false;
        if (key.private_key.p == 0 || key.private_key.q == 0) return false;
        if (key.public_key.n != key.private_key.p * key.private_key.q) return false;
        cpp_int phi = (key.private_key.p - 1) * (key.private_key.q - 1);
        if ((key.public_key.e * key.private_key.d) % phi != 1) return false;
        return true;
    }
    catch (...) { return false; }
}
static bool testWiener_VulnerabilityCondition() {
    try {
        RSAService service;
        auto key = service.generateWienerVulnerable(256);
        cpp_int bound = fourthRoot(key.public_key.n) / 3;
        return key.private_key.d < bound;
    }
    catch (...) { return false; }
}
static bool testWiener_NoFermaVulnerability() {
    try {
        RSAService service;
        auto key = service.generateWienerVulnerable(256);
        cpp_int diff = (key.private_key.p > key.private_key.q) ?
            key.private_key.p - key.private_key.q :
            key.private_key.q - key.private_key.p;
        cpp_int bound = fourthRoot(key.public_key.n);
        return diff >= bound;
    }
    catch (...) { return false; }
}
static bool testWiener_EncryptionDecryption() {
    try {
        RSAService service;
        auto key = service.generateWienerVulnerable(256);
        return testEncryptDecrypt(key);
    }
    catch (...) { return false; }
}
static bool testPublicKeyStructure() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        PublicKey pub = key.public_key;
        if (pub.n <= 1) return false;
        if (pub.e <= 1 || pub.e % 2 == 0) return false;
        return true;
    }
    catch (...) { return false; }
}
static bool testPrivateKeyStructure() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        PrivateKey priv = key.private_key;

        if (priv.n <= 1 || priv.d <= 1) return false;
        if (priv.p <= 1 || priv.q <= 1) return false;
        return true;
    }
    catch (...) { return false; }
}
static bool testKeyPairStructure() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        if (key.public_key.n != key.private_key.n) return false;
        cpp_int phi = (key.private_key.p - 1) * (key.private_key.q - 1);
        if ((key.public_key.e * key.private_key.d) % phi != 1) return false;
        return true;
    }
    catch (...) { return false; }
}
static bool testInvalidArguments() {
    RSAService service;
    bool ok = true;
    try { service.generateFermaVulnerable(128); ok = false; }
    catch (const std::invalid_argument&) {}
    catch (...) { ok = false; }
    try { service.generateWienerVulnerable(128); ok = false; }
    catch (const std::invalid_argument&) {}
    catch (...) { ok = false; }
    try { service.generateFermaVulnerable(257); ok = false; }
    catch (const std::invalid_argument&) {}
    catch (...) { ok = false; }
    try { service.generateWienerVulnerable(257); ok = false; }
    catch (const std::invalid_argument&) {}
    catch (...) { ok = false; }
    return ok;
}
static bool testNoSimultaneousVulnerabilities() {
    try {
        RSAService service;
        auto key = service.generateFermaVulnerable(256);
        cpp_int bound = fourthRoot(key.public_key.n);
        cpp_int diff = (key.private_key.p > key.private_key.q) ?
            key.private_key.p - key.private_key.q :
            key.private_key.q - key.private_key.p;
        if (!(diff < bound)) return false;
        if (!(key.private_key.d >= bound / 3)) return false;
        key = service.generateWienerVulnerable(256);
        bound = fourthRoot(key.public_key.n);
        diff = (key.private_key.p > key.private_key.q) ?
            key.private_key.p - key.private_key.q :
            key.private_key.q - key.private_key.p;
        if (!(key.private_key.d < bound / 3)) return false;
        if (!(diff >= bound)) return false;
        return true;
    }
    catch (...) { return false; }
}
int main() {
    auto start_time = std::chrono::steady_clock::now();
    bool all_passed = true;
    int passed = 0, total = 0;
    std::cout << "========================================\n";
    std::cout << "RSA Service Unit Tests (Fast Mode)\n";
    std::cout << "Using 256-bit keys for maximum speed\n";
    std::cout << "========================================\n\n";
    std::cout << "Running tests (this will take 10-20 seconds)...\n\n";
    std::cout << "Ferma Vulnerable Key Tests:\n";
    total++; std::cout << "  - Key correctness... ";
    if (testFerma_KeyCorrectness()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - Ferma condition... ";
    if (testFerma_VulnerabilityCondition()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - No Wiener condition... ";
    if (testFerma_NoWienerVulnerability()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - Encryption/decryption... ";
    if (testFerma_EncryptionDecryption()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    std::cout << "\nWiener Vulnerable Key Tests:\n";
    total++; std::cout << "  - Key correctness... ";
    if (testWiener_KeyCorrectness()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - Wiener condition... ";
    if (testWiener_VulnerabilityCondition()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - No Ferma condition... ";
    if (testWiener_NoFermaVulnerability()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - Encryption/decryption... ";
    if (testWiener_EncryptionDecryption()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    std::cout << "\nData Structure Tests:\n";
    total++; std::cout << "  - PublicKey structure... ";
    if (testPublicKeyStructure()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - PrivateKey structure... ";
    if (testPrivateKeyStructure()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    total++; std::cout << "  - KeyPair structure... ";
    if (testKeyPairStructure()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    std::cout << "\nException Handling Tests:\n";
    total++; std::cout << "  - Invalid arguments... ";
    if (testInvalidArguments()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    std::cout << "\nIntegration Tests:\n";
    total++; std::cout << "  - No simultaneous vulnerabilities... ";
    if (testNoSimultaneousVulnerabilities()) { passed++; std::cout << "PASSED\n"; }
    else { std::cout << "FAILED\n"; all_passed = false; }
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "\n========================================\n";
    std::cout << "Results: " << passed << "/" << total << " tests passed\n";
    std::cout << "Time: " << elapsed / 1000.0 << " seconds\n";

    if (all_passed) {
        std::cout << "\n✓ ALL TESTS PASSED!\n";
        std::cout << "========================================\n";
        return 0;
    }
    else {
        std::cout << "\n✗ SOME TESTS FAILED!\n";
        std::cout << "========================================\n";
        return 1;
    }
}