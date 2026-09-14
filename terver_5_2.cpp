#include <iostream>
#include <unordered_set>
#include <random>
#include <cmath>
#include <iomanip>
using namespace std;
double get_r(int k, int n) {
    if (2 * k > n)
        return 1;
    double log_p = 0;
    for (int i = 0; i < k; i++)
        log_p += log((double)(n - k - i) / (n - i));
    return 1 - exp(log_p);
}
unordered_set<int> make_set(int k, int n, mt19937& gen) {
    unordered_set<int> a;
    uniform_int_distribution<int> dist(0, n - 1);
    while ((int)a.size() < k)
        a.insert(dist(gen));
    return a;
}
bool have_intersection(const unordered_set<int>& a,
    const unordered_set<int>& b) {
    for (int x : a) {
        if (b.count(x))
            return true;
    }
    return false;
}
int encrypt(int m, int key) {
    return m ^ key;
}
bool have_cipher_intersection(const unordered_set<int>& a,
    const unordered_set<int>& b,
    int key) {
    unordered_set<int> c;
    for (int x : a)
        c.insert(encrypt(x, key));
    for (int x : b) {
        if (c.count(encrypt(x, key)))
            return true;
    }
    return false;
}
int main() {
    int n, k;
    cout << "n: ";
    cin >> n;
    cout << "k: ";
    cin >> k;
    if (n <= 0 || k <= 0 || k > n) {
        cout << "wrong values\n";
        return 0;
    }
    double exact = get_r(k, n);
    double bound = 1 - exp(-(double)k * k / n);
    mt19937 gen(random_device{}());
    int tests = 10000;
    int hit = 0;
    int cipher_hit = 0;
    uniform_int_distribution<int> key_dist(0, n - 1);
    for (int t = 0; t < tests; t++) {
        auto x1 = make_set(k, n, gen);
        auto x2 = make_set(k, n, gen);
        if (have_intersection(x1, x2))
            hit++;
        int key = key_dist(gen);
        if (have_cipher_intersection(x1, x2, key))
            cipher_hit++;
    }
    double exp_r = (double)hit / tests;
    double exp_cipher = (double)cipher_hit / tests;
    cout << fixed << setprecision(6);
    cout << "\nexact r: " << exact << '\n';
    cout << "bound: " << bound << '\n';
    cout << "experiment r: " << exp_r << '\n';
    cout << "experiment e(x1)=e(x2): " << exp_cipher << '\n';
    if (n == 65536) {
        int bound_k = floor(sqrt(n * log(2))) + 1;
        cout << "\nk from bound: " << bound_k << '\n';
        for (int i = 1; i <= n; i++) {
            if (get_r(i, n) >= 0.5) {
                cout << "first exact k: " << i << '\n';
                break;
            }
        }
    }
    return 0;
}