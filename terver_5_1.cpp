#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
using namespace std;
double less_prob(int m, double l) {
    double sum = 0;
    double p = exp(-l);
    for (int k = 0; k < m; k++) {
        if (k > 0)
            p *= l / k;
        sum += p;
    }
    return 1 - sum;
}
int main() {
    int m;
    long long n = 1000000;
    cout << "m: ";
    cin >> m;
    if (m < 1) {
        cout << "wrong m\n";
        return 0;
    }
    double l = 1.94;
    mt19937 gen(random_device{}());
    poisson_distribution<int> child_count(l);
    uniform_int_distribution<int> sex(0, 1);
    long long no_girls = 0;
    long long no_girls_m = 0;
    long long count_m = 0;
    for (long long i = 0; i < n; i++) {
        int k = child_count(gen);
        if (k >= m)
            count_m++;
        bool girls = false;
        for (int j = 0; j < k; j++) {
            if (sex(gen) == 1)
                girls = true;
        }
        if (!girls) {
            no_girls++;
            if (k >= m)
                no_girls_m++;
        }
    }
    double exp_normal = (double)count_m / n;
    double exp_cond = (double)no_girls_m / no_girls;
    double theory_normal = less_prob(m, l);
    double theory_cond = less_prob(m, l / 2);
    cout << fixed << setprecision(6);
    cout << "\ntheory normal: " << theory_normal << '\n';
    cout << "experiment normal: " << exp_normal << '\n';
    cout << "\ntheory conditional: " << theory_cond << '\n';
    cout << "experiment conditional: " << exp_cond << '\n';
    cout << "\ndifference theory: "
        << abs(theory_normal - theory_cond) << '\n';
    return 0;
}