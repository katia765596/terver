#include <boost/multiprecision/cpp_int.hpp>
#include <random>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cassert>
using namespace boost::multiprecision;
namespace utils {
    int bitLength(const cpp_int& x) {
        if (x == 0) return 1;
        return static_cast<int>(msb(x)) + 1;
    }
    cpp_int integerSqrt(const cpp_int& x) {
        if (x < 0) throw std::invalid_argument("negative sqrt");
        if (x == 0 || x == 1) return x;
        cpp_int low = 0, high = x;
        while (low <= high) {
            cpp_int mid = (low + high) / 2;
            cpp_int sq = mid * mid;
            if (sq == x) return mid;
            else if (sq < x) low = mid + 1;
            else high = mid - 1;
        }
        return high;
    }
    bool isPerfectSquare(const cpp_int& x, cpp_int& root) {
        if (x < 0) return false;
        root = integerSqrt(x);
        return root * root == x;
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
    cpp_int modPow(const cpp_int& base, const cpp_int& exp, const cpp_int& mod) {
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
    bool isPrime(const cpp_int& n, int rounds = 40) {
        if (n < 2) return false;
        if (n == 2 || n == 3) return true;
        if (n % 2 == 0) return false;
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
        if (bits < 2) throw std::invalid_argument("bits >= 2");
        const int max_attempts = 10000;
        for (int attempt = 0; attempt < max_attempts; ++attempt) {
            cpp_int candidate = randomNumber(bits);
            if (candidate % 2 == 0) ++candidate;
            if (isPrime(candidate, 40)) return candidate;
        }
        throw std::runtime_error("cannot generate prime");
    }
    cpp_int nextPrime(const cpp_int& n) {
        cpp_int candidate = n;
        if (candidate % 2 == 0) ++candidate;
        const int max_steps = 1000000;
        for (int step = 0; step < max_steps; ++step) {
            if (isPrime(candidate, 40)) return candidate;
            candidate += 2;
        }
        throw std::runtime_error("cannot find next prime");
    }
    cpp_int fourthRoot(const cpp_int& x) {
        double d = static_cast<double>(x);
        double r = std::pow(d, 0.25);
        cpp_int guess = static_cast<cpp_int>(r);
        while ((guess + 1) * (guess + 1) * (guess + 1) * (guess + 1) <= x) ++guess;
        while (guess * guess * guess * guess > x) --guess;
        return guess;
    }
}
class Fraction {
public:
    cpp_int num, den;

    Fraction(const cpp_int& n = 0, const cpp_int& d = 1) : num(n), den(d) {
        if (den == 0) throw std::invalid_argument("denominator cannot be zero");
        if (den < 0) { num = -num; den = -den; }
        reduce();
    }
    void reduce() {
        cpp_int g = gcd(num, den);
        if (g != 0) {
            num /= g;
            den /= g;
        }
    }
    bool operator==(const Fraction& other) const {
        return num == other.num && den == other.den;
    }
    bool operator<(const Fraction& other) const {
        return num * other.den < other.num * den;
    }
    bool operator>(const Fraction& other) const {
        return num * other.den > other.num * den;
    }
    friend std::ostream& operator<<(std::ostream& os, const Fraction& f) {
        os << f.num << "/" << f.den;
        return os;
    }
};
class ContinuedFractionService {
public:
    static std::vector<cpp_int> getContinuedFraction(const Fraction& frac) {
        std::vector<cpp_int> result;
        cpp_int a = frac.num, b = frac.den;
        while (b != 0) {
            cpp_int q = a / b;
            result.push_back(q);
            cpp_int r = a % b;
            a = b;
            b = r;
        }
        return result;
    }
    static Fraction fromContinuedFraction(const std::vector<cpp_int>& cf) {
        if (cf.empty()) throw std::invalid_argument("empty continued fraction");
        cpp_int num = 1, den = 0;
        for (auto it = cf.rbegin(); it != cf.rend(); ++it) {
            cpp_int new_num = *it * num + den;
            den = num;
            num = new_num;
        }
        return Fraction(num, den);
    }
    static std::vector<Fraction> getConvergents(const Fraction& frac) {
        auto cf = getContinuedFraction(frac);
        std::vector<Fraction> result;
        cpp_int p0 = 0, p1 = 1;
        cpp_int q0 = 1, q1 = 0;
        for (size_t i = 0; i < cf.size(); ++i) {
            cpp_int a = cf[i];
            cpp_int p = a * p1 + p0;
            cpp_int q = a * q1 + q0;
            result.emplace_back(p, q);
            p0 = p1; p1 = p;
            q0 = q1; q1 = q;
        }
        return result;
    }
};
class CalkinWilfTree {
public:
    static std::string getPath(const Fraction& frac) {
        std::string path;
        cpp_int p = frac.num, q = frac.den;
        while (!(p == 1 && q == 1)) {
            if (p < q) {
                path.push_back('L');
                q = q - p;
            }
            else {
                path.push_back('R');
                p = p - q;
            }
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
    static Fraction fromPath(const std::string& path) {
        cpp_int p = 1, q = 1;
        for (char c : path) {
            if (c == 'L')
                q = p + q;
            else if (c == 'R')
                p = p + q;
            else
                throw std::invalid_argument("invalid path character");
        }
        return Fraction(p, q);
    }
};
class SternBrocotTree {
private:
    using Pair = std::pair<cpp_int, cpp_int>;
public:
    static std::string getPath(const Fraction& frac) {
        Pair left(0, 1);
        Pair right(1, 0);
        std::string path;
        while (true) {
            Pair med(left.first + right.first, left.second + right.second);
            if (frac.num * med.second == med.first * frac.den)
                break;
            if (frac.num * med.second < med.first * frac.den) {
                path.push_back('L');
                right = med;
            }
            else {
                path.push_back('R');
                left = med;
            }
        }
        return path;
    }
    static Fraction fromPath(const std::string& path) {
        Pair left(0, 1);
        Pair right(1, 0);
        for (char c : path) {
            if (c == 'L')
                right = Pair(left.first + right.first, left.second + right.second);
            else if (c == 'R')
                left = Pair(left.first + right.first, left.second + right.second);
            else
                throw std::invalid_argument("invalid path character");
        }
        return Fraction(left.first + right.first, left.second + right.second);
    }
    static std::vector<Fraction> getConvergents(const std::string& path) {
        std::vector<Fraction> result;
        Pair left(0, 1);
        Pair right(1, 0);
        for (char c : path) {
            Pair med(left.first + right.first, left.second + right.second);
            result.emplace_back(med.first, med.second);
            if (c == 'L')
                right = med;
            else if (c == 'R')
                left = med;
            else
                throw std::invalid_argument("invalid path character");
        }
        Pair final_med(left.first + right.first, left.second + right.second);
        result.emplace_back(final_med.first, final_med.second);
        return result;
    
    }
};
struct RSAPublicKey {
    cpp_int n, e;
};
struct RSAPrivateKey {
    cpp_int n, d, p, q;
};
struct RSAKeyPair {
    RSAPublicKey public_key;
    RSAPrivateKey private_key;
};
class RSAService {
public:
    RSAKeyPair generateFermaVulnerable(int bits = 1024) {
        if (bits < 256 || bits % 2 != 0)  // исправлено: 256 вместо 512
            throw std::invalid_argument("bits must be even and >= 256");
        int half_bits = bits / 2;
        const cpp_int MAX_OFFSET = cpp_int(1) << 20;
        for (int attempt = 0; attempt < 100; ++attempt) {
            cpp_int p = utils::generatePrime(half_bits);
            cpp_int offset = utils::randomNumber(utils::bitLength(MAX_OFFSET)) % MAX_OFFSET + 1;
            cpp_int q_candidate = p + offset;
            if (utils::bitLength(q_candidate) > half_bits) continue;
            cpp_int q = utils::nextPrime(q_candidate);
            if (q == p) continue;
            if (utils::bitLength(q) > half_bits) continue;
            cpp_int n = p * q;
            cpp_int phi = (p - 1) * (q - 1);
            cpp_int e = 65537;
            while (gcd(e, phi) != 1) e += 2;
            cpp_int d = utils::modInverse(e, phi);
            cpp_int bound = utils::fourthRoot(n) / 3;
            if (d < bound) continue;
            return { {n, e}, {n, d, p, q} };
        }
        throw std::runtime_error("failed to generate Ferma‑vulnerable key");
    }
    RSAKeyPair generateWienerVulnerable(int bits = 1024) {
        if (bits < 256 || bits % 2 != 0)
            throw std::invalid_argument("bits must be even and >= 256");
        int half_bits = bits / 2;
        for (int attempt = 0; attempt < 100; ++attempt) {
            cpp_int p = utils::generatePrime(half_bits);
            cpp_int q = utils::generatePrime(half_bits);
            cpp_int n = p * q;
            cpp_int diff = (p > q) ? p - q : q - p;
            cpp_int bound_diff = utils::fourthRoot(n);
            if (diff < bound_diff) continue;
            cpp_int phi = (p - 1) * (q - 1);
            cpp_int max_d = 1000000;
            if (max_d > n) max_d = n / 2;
            cpp_int d;
            do {
                d = utils::randomNumber(utils::bitLength(max_d)) % max_d + 2;
            } while (d < 2 || d >= max_d);
            cpp_int e;
            try {
                e = utils::modInverse(d, phi);
            }
            catch (...) {
                continue;
            }
            if (e <= 1 || e >= phi) continue;
            return { {n, e}, {n, d, p, q} };
        }
        throw std::runtime_error("failed to generate Wiener‑vulnerable key");
    }
};
class WienerAttack {
public:
    static std::pair<cpp_int, cpp_int> attack(const cpp_int& n, const cpp_int& e) {
        Fraction ef(e, n);
        std::vector<Fraction> conv = ContinuedFractionService::getConvergents(ef);
        for (size_t i = 0; i < conv.size(); ++i) {
            const Fraction& cand = conv[i];
            cpp_int k = cand.num;
            cpp_int d = cand.den;
            if (k <= 0 || d <= 0) continue;
            if ((e * d - 1) % k != 0) continue;
            cpp_int phi = (e * d - 1) / k;
            cpp_int b = n - phi + 1; // p+q
            cpp_int disc = b * b - 4 * n;
            cpp_int sqrt_disc;
            if (disc >= 0 && utils::isPerfectSquare(disc, sqrt_disc)) {
                cpp_int p = (b - sqrt_disc) / 2;
                cpp_int q = (b + sqrt_disc) / 2;
                if (p > 0 && q > 0 && p * q == n) {
                    if (p > q) std::swap(p, q);
                    return std::make_pair(p, q);
                }
            }
        }
        throw std::runtime_error("Wiener attack failed");
    }
};
void testContinuedFraction() {
    std::cout << "Testing continued fractions...\n";
    Fraction f(3, 4);
    std::vector<cpp_int> cf = ContinuedFractionService::getContinuedFraction(f);
    assert(cf.size() == 3 && cf[0] == 0 && cf[1] == 1 && cf[2] == 3);
    Fraction f2 = ContinuedFractionService::fromContinuedFraction(cf);
    assert(f2 == f);
    Fraction f3(7, 2);
    std::vector<cpp_int> cf2 = ContinuedFractionService::getContinuedFraction(f3);
    assert(cf2.size() == 2 && cf2[0] == 3 && cf2[1] == 2);
    Fraction f4 = ContinuedFractionService::fromContinuedFraction(cf2);
    assert(f4 == f3);
    std::vector<Fraction> conv = ContinuedFractionService::getConvergents(f);
    assert(conv.size() == 3 && conv[0] == Fraction(0, 1) && conv[1] == Fraction(1, 1) && conv[2] == Fraction(3, 4));
    std::cout << "  Passed\n";
}
void testCalkinWilf() {
    std::cout << "Testing Calkin‑Wilf tree...\n";
    assert(CalkinWilfTree::getPath(Fraction(1, 2)) == "L");
    assert(CalkinWilfTree::fromPath("L") == Fraction(1, 2));
    assert(CalkinWilfTree::getPath(Fraction(2, 1)) == "R");
    assert(CalkinWilfTree::fromPath("R") == Fraction(2, 1));
    assert(CalkinWilfTree::getPath(Fraction(3, 2)) == "LR");
    assert(CalkinWilfTree::fromPath("LR") == Fraction(3, 2));
    assert(CalkinWilfTree::fromPath("") == Fraction(1, 1));
    bool thrown = false;
    try { CalkinWilfTree::fromPath("X"); }
    catch (const std::invalid_argument&) { thrown = true; }
    assert(thrown);
    std::cout << "  Passed\n";
}
void testSternBrocot() {
    std::cout << "Testing Stern‑Brocot tree...\n";
    assert(SternBrocotTree::getPath(Fraction(1, 2)) == "L");
    assert(SternBrocotTree::fromPath("L") == Fraction(1, 2));
    assert(SternBrocotTree::getPath(Fraction(2, 1)) == "R");
    assert(SternBrocotTree::fromPath("R") == Fraction(2, 1));
    assert(SternBrocotTree::getPath(Fraction(3, 2)) == "RL");
    assert(SternBrocotTree::fromPath("RL") == Fraction(3, 2));
    std::vector<Fraction> conv = SternBrocotTree::getConvergents("RL");
    assert(conv.size() == 3 && conv[0] == Fraction(1, 1) && conv[1] == Fraction(2, 1) && conv[2] == Fraction(3, 2));
    std::cout << "  Passed\n";
}
void testRSA() {
    std::cout << "Testing RSA service...\n";
    RSAService rsa;
    std::cout << "  Generating Ferma-vulnerable key (256 bits)...\n";
    RSAKeyPair keyF = rsa.generateFermaVulnerable(256);
    assert(keyF.private_key.p * keyF.private_key.q == keyF.public_key.n);
    cpp_int phi = (keyF.private_key.p - 1) * (keyF.private_key.q - 1);
    assert((keyF.public_key.e * keyF.private_key.d) % phi == 1);
    std::cout << "  Ferma key OK\n";
    std::cout << "  Generating Wiener-vulnerable key (256 bits)...\n";
    RSAKeyPair keyW = rsa.generateWienerVulnerable(256);
    assert(keyW.private_key.p * keyW.private_key.q == keyW.public_key.n);
    phi = (keyW.private_key.p - 1) * (keyW.private_key.q - 1);
    assert((keyW.public_key.e * keyW.private_key.d) % phi == 1);
    std::cout << "  Wiener key OK\n";
}
void testWienerAttack() {
    std::cout << "Testing Wiener attack...\n";
    RSAService rsa;
    std::cout << "  Generating Wiener-vulnerable key (256 bits)...\n";
    RSAKeyPair key = rsa.generateWienerVulnerable(256);
    std::cout << "  Running attack...\n";
    std::pair<cpp_int, cpp_int> pq = WienerAttack::attack(key.public_key.n, key.public_key.e);
    assert(pq.first == key.private_key.p && pq.second == key.private_key.q);
    assert(pq.first * pq.second == key.public_key.n);
    std::cout << "  Attack succeeded: p = " << pq.first << ", q = " << pq.second << "\n";
    std::cout << "  Testing on non-Wiener key (should fail)...\n";
    RSAKeyPair key2 = rsa.generateFermaVulnerable(256);
    bool thrown = false;
    try {
        WienerAttack::attack(key2.public_key.n, key2.public_key.e);
    }
    catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
    std::cout << "  Attack correctly fails on non-Wiener key\n";
}
int main() {
    try {
        testContinuedFraction();
        testCalkinWilf();
        testSternBrocot();
        testRSA();
        testWienerAttack();
        std::cout << "\nAll unit tests passed.\n\n";
        std::cout << "=== Demonstration of Wiener attack ===\n";
        RSAService rsa;
        std::cout << "Generating Wiener‑vulnerable RSA key (1024 bits)...\n";
        RSAKeyPair key = rsa.generateWienerVulnerable(1024);
        std::cout << "n = " << key.public_key.n << "\n";
        std::cout << "e = " << key.public_key.e << "\n";
        std::cout << "\nRunning Wiener attack...\n";
        std::pair<cpp_int, cpp_int> pq_attack = WienerAttack::attack(key.public_key.n, key.public_key.e);
        std::cout << "Recovered p = " << pq_attack.first << "\n";
        std::cout << "Recovered q = " << pq_attack.second << "\n";
        std::cout << "Check: p*q == n? " << (pq_attack.first * pq_attack.second == key.public_key.n ? "YES" : "NO") << "\n";
        std::cout << "\n=== Additional demonstration ===\n";
        Fraction demo(41, 13);
        std::cout << "Fraction " << demo << " -> continued fraction: [";
        std::vector<cpp_int> cf = ContinuedFractionService::getContinuedFraction(demo);
        for (size_t i = 0; i < cf.size(); ++i) {
            if (i > 0) std::cout << ";";
            std::cout << cf[i];
        }
        std::cout << "]\n";
        std::string cw_path = CalkinWilfTree::getPath(demo);
        std::cout << "Calkin‑Wilf path: " << cw_path << "\n";
        std::string sb_path = SternBrocotTree::getPath(demo);
        std::cout << "Stern‑Brocot path: " << sb_path << "\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}