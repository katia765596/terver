#include <iostream>
#include <random>
#include <vector>
#include <iomanip>
#include <cmath>
std::mt19937 rng(std::random_device{}());
bool bernoulli(double p) {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng) < p;
}
int discrete_uniform(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}
struct result {
    double empirical;
    double theoretical;
};
result task92(int simulations) {
    int hit = 0;
    for (int i = 0; i < simulations; ++i) {
        bool with_scope = (discrete_uniform(1, 5) <= 3);
        double p = with_scope ? 0.95 : 0.7;
        if (bernoulli(p)) ++hit;
    }
    double emp = double(hit) / simulations;
    double theor = (3.0 / 5.0) * 0.95 + (2.0 / 5.0) * 0.7;
    return { emp, theor };
}
result task93(int simulations) {
    int excellent = 0;
    for (int i = 0; i < simulations; ++i) {
        int r = discrete_uniform(1, 50);
        double p;
        if (r <= 12) p = 0.9;
        else if (r <= 32) p = 0.6;
        else p = 0.9;
        if (bernoulli(p)) ++excellent;
    }
    double emp = double(excellent) / simulations;
    double theor = (12.0 / 50.0) * 0.9 + (20.0 / 50.0) * 0.6 + (18.0 / 50.0) * 0.9;
    return { emp, theor };
}
result task95(int simulations) {
    int white_final = 0;
    for (int i = 0; i < simulations; ++i) {
        bool from_i_white = bernoulli(4.0 / 10.0);
        int ii_white = 4 + (from_i_white ? 1 : 0);
        int ii_black = 6 + (from_i_white ? 0 : 1);
        int ii_total = ii_white + ii_black;
        bool from_ii_white = bernoulli(double(ii_white) / ii_total);
        int iii_white = 4 + (from_ii_white ? 1 : 0);
        int iii_black = 6 + (from_ii_white ? 0 : 1);
        int iii_total = iii_white + iii_black;
        if (bernoulli(double(iii_white) / iii_total)) ++white_final;
    }
    double emp = double(white_final) / simulations;
    double theor = 0.4;
    return { emp, theor };
}
std::vector<result> task98(int simulations) {
    int hit_with_scope = 0, hit_without_scope = 0;
    for (int i = 0; i < simulations; ++i) {
        bool with_scope = (discrete_uniform(1, 10) <= 4);
        double p = with_scope ? 0.95 : 0.8;
        bool hit = bernoulli(p);
        if (hit) {
            if (with_scope) ++hit_with_scope;
            else ++hit_without_scope;
        }
    }
    int total_hits = hit_with_scope + hit_without_scope;
    double emp1 = (total_hits > 0) ? double(hit_with_scope) / total_hits : 0.0;
    double emp2 = (total_hits > 0) ? double(hit_without_scope) / total_hits : 0.0;
    double p_a = 0.4 * 0.95 + 0.6 * 0.8;
    double theor1 = (0.4 * 0.95) / p_a;
    double theor2 = (0.6 * 0.8) / p_a;
    return { {emp1, theor1}, {emp2, theor2} };
}
result task100(int simulations) {
    int error_by_first = 0, total_errors = 0;
    for (int i = 0; i < simulations; ++i) {
        bool first = bernoulli(0.5);
        double p = first ? 0.05 : 0.1;
        if (bernoulli(p)) {
            ++total_errors;
            if (first) ++error_by_first;
        }
    }
    double emp = (total_errors > 0) ? double(error_by_first) / total_errors : 0.0;
    double theor = 1.0 / 3.0;
    return { emp, theor };
}
result task102(int simulations) {
    int recognized_by_second = 0, total_recognized = 0;
    for (int i = 0; i < simulations; ++i) {
        bool first = bernoulli(0.55);
        double p = first ? 0.9 : 0.98;
        if (bernoulli(p)) {
            ++total_recognized;
            if (!first) ++recognized_by_second;
        }
    }
    double emp = (total_recognized > 0) ? double(recognized_by_second) / total_recognized : 0.0;
    double theor = (0.45 * 0.98) / (0.55 * 0.9 + 0.45 * 0.98);
    return { emp, theor };
}
result task104(int simulations) {
    int b3_given_a = 0, total_a = 0;
    for (int i = 0; i < simulations; ++i) {
        int hypothesis = discrete_uniform(1, 3);
        double p_a;
        if (hypothesis == 1) p_a = 0.6;
        else if (hypothesis == 2) p_a = 0.3;
        else p_a = 0.1;
        if (bernoulli(p_a)) {
            ++total_a;
            if (hypothesis == 3) ++b3_given_a;
        }
    }
    double emp = (total_a > 0) ? double(b3_given_a) / total_a : 0.0;
    double theor = 0.1;
    return { emp, theor };
}
result task107(int simulations) {
    int exactly_two_hits = 0, third_hit_given_two = 0;
    for (int i = 0; i < simulations; ++i) {
        bool h1 = bernoulli(0.6);
        bool h2 = bernoulli(0.5);
        bool h3 = bernoulli(0.4);
        int hits = (h1 ? 1 : 0) + (h2 ? 1 : 0) + (h3 ? 1 : 0);
        if (hits == 2) {
            ++exactly_two_hits;
            if (h3) ++third_hit_given_two;
        }
    }
    double emp = (exactly_two_hits > 0) ? double(third_hit_given_two) / exactly_two_hits : 0.0;
    double theor = (0.6 * 0.5 * 0.4 + 0.4 * 0.5 * 0.4) /
        (0.6 * 0.5 * 0.6 + 0.6 * 0.5 * 0.4 + 0.4 * 0.5 * 0.4);
    return { emp, theor };
}
int main() {
    std::cout << std::fixed << std::setprecision(6);
    int simulations = 100000;
    std::cout << "monte carlo simulation for tasks 92,93,95,98,100,102,104,107\n";
    std::cout << "simulations: " << simulations << "\n\n";
    auto r92 = task92(simulations);
    std::cout << "task 92: empirical = " << r92.empirical << ", theoretical = " << r92.theoretical << "\n\n";
    auto r93 = task93(simulations);
    std::cout << "task 93: empirical = " << r93.empirical << ", theoretical = " << r93.theoretical << "\n\n";
    auto r95 = task95(simulations);
    std::cout << "task 95: empirical = " << r95.empirical << ", theoretical = " << r95.theoretical << "\n\n";
    auto r98 = task98(simulations);
    std::cout << "task 98:\n";
    std::cout << "  with scope | hit: empirical = " << r98[0].empirical << ", theoretical = " << r98[0].theoretical << "\n";
    std::cout << "  without scope | hit: empirical = " << r98[1].empirical << ", theoretical = " << r98[1].theoretical << "\n";
    std::cout << "  conclusion: " << (r98[1].empirical > r98[0].empirical ? "without scope is more likely" : "with scope is more likely") << "\n\n";
    auto r100 = task100(simulations);
    std::cout << "task 100: empirical = " << r100.empirical << ", theoretical = " << r100.theoretical << "\n\n";
    auto r102 = task102(simulations);
    std::cout << "task 102: empirical = " << r102.empirical << ", theoretical = " << r102.theoretical << "\n\n";
    auto r104 = task104(simulations);
    std::cout << "task 104: empirical = " << r104.empirical << ", theoretical = " << r104.theoretical << "\n\n";
    auto r107 = task107(simulations);
    std::cout << "task 107: empirical = " << r107.empirical << ", theoretical = " << r107.theoretical << "\n\n";
    return 0;
}