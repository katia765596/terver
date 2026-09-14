#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <locale>
#ifdef _WIN32
#include <windows.h>
#endif
std::mt19937 rng(std::random_device{}());
bool bernoulli(double p) {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng) < p;
}
int discrete_uniform(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}
double uniform_01() {
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
std::vector<int> sample_without_replacement(int N, int k) {
    std::vector<int> pool(N);
    std::iota(pool.begin(), pool.end(), 0);
    std::shuffle(pool.begin(), pool.end(), rng);
    return std::vector<int>(pool.begin(), pool.begin() + k);
}
struct Result {
    double empirical;
    double theoretical;
};
std::vector<Result> task57(int simulations) {
    double p[4] = { 0.6, 0.7, 0.8, 0.9 };
    int count_leq3 = 0, count_geq2 = 0;
    for (int i = 0; i < simulations; ++i) {
        int cnt = 0;
        for (int j = 0; j < 4; ++j)
            if (bernoulli(p[j])) ++cnt;
        if (cnt <= 3) ++count_leq3;
        if (cnt >= 2) ++count_geq2;
    }
    double theor_leq3 = 1.0 - 0.6 * 0.7 * 0.8 * 0.9; 
    double theor_geq2 = 1.0 - 0.4 * 0.3 * 0.2 * 0.1 - (0.6 * 0.3 * 0.2 * 0.1 + 0.4 * 0.7 * 0.2 * 0.1 + 0.4 * 0.3 * 0.8 * 0.1 + 0.4 * 0.3 * 0.2 * 0.9);
    return { {double(count_leq3) / simulations, theor_leq3},
            {double(count_geq2) / simulations, theor_geq2} };
}
std::vector<Result> task58(int simulations) {
    int all5 = 0, all_same = 0;
    for (int i = 0; i < simulations; ++i) {
        int a = discrete_uniform(1, 6), b = discrete_uniform(1, 6), c = discrete_uniform(1, 6);
        if (a == 5 && b == 5 && c == 5) ++all5;
        if (a == b && b == c) ++all_same;
    }
    return { {double(all5) / simulations, 1.0 / 216},
            {double(all_same) / simulations, 6.0 / 216} };
}
std::vector<Result> task59(int simulations) {
    int two_ones = 0, two_same = 0, all_diff = 0;
    for (int i = 0; i < simulations; ++i) {
        int a = discrete_uniform(1, 6), b = discrete_uniform(1, 6), c = discrete_uniform(1, 6);
        if ((a == 1 && b == 1 && c != 1) || (a == 1 && c == 1 && b != 1) || (b == 1 && c == 1 && a != 1))
            ++two_ones;
        if ((a == b && b != c) || (a == c && c != b) || (b == c && c != a))
            ++two_same;
        if (a != b && b != c && a != c)
            ++all_diff;
    }
    double theor_two_ones = 15.0 / 216;
    double theor_two_same = 90.0 / 216;
    double theor_all_diff = 120.0 / 216;
    return { {double(two_ones) / simulations, theor_two_ones},
            {double(two_same) / simulations, theor_two_same},
            {double(all_diff) / simulations, theor_all_diff} };
}
Result task61(int simulations) {
    int n4_all_hit = 0, n5_all_hit = 0;
    for (int i = 0; i < simulations; ++i) {
        bool all4 = true, all5 = true;
        for (int j = 0; j < 4; ++j) if (!bernoulli(0.8)) { all4 = false; break; }
        for (int j = 0; j < 5; ++j) if (!bernoulli(0.8)) { all5 = false; break; }
        if (all4) ++n4_all_hit;
        if (all5) ++n5_all_hit;
    }
    double emp4 = double(n4_all_hit) / simulations;
    double emp5 = double(n5_all_hit) / simulations;
    double theor4 = std::pow(0.8, 4);
    double theor5 = std::pow(0.8, 5);
    std::cout << "Для n=4 эмпир. = " << emp4 << ", теор. = " << theor4 << "\n";
    std::cout << "Для n=5 эмпир. = " << emp5 << ", теор. = " << theor5 << "\n";
    return { 0.0, 0.0 };
}
Result task62(int simulations) {
    int hit = 0;
    for (int i = 0; i < simulations; ++i) {
        bool any = false;
        for (int j = 0; j < 10; ++j)
            if (bernoulli(0.2)) { any = true; break; }
        if (any) ++hit;
    }
    double emp = double(hit) / simulations;
    double theor = 1.0 - std::pow(0.8, 10);
    return { emp, theor };
}
Result task63(int simulations) {
    int success = 0;
    for (int i = 0; i < simulations; ++i) {
        double x = uniform_01(), y = uniform_01(), z = uniform_01();
        int part_x = int(x * 3); 
        int part_y = int(y * 3);
        int part_z = int(z * 3);
        if (part_x != part_y && part_y != part_z && part_x != part_z)
            ++success;
    }
    double emp = double(success) / simulations;
    double theor = 2.0 / 9; 
    return { emp, theor };
}
Result task65(int simulations) {
    int success = 0;
    for (int i = 0; i < simulations; ++i) {
        auto sample = sample_without_replacement(100, 2);
        if (sample[0] < 5 && sample[1] < 5) ++success;
    }
    double emp = double(success) / simulations;
    double theor = double(5 * 4) / (100 * 99); 
    return { emp, theor };
}
Result task67(int simulations) {
    int success = 0;
    for (int i = 0; i < simulations; ++i) {
        auto sample = sample_without_replacement(10, 4);
        bool all_colored = true;
        for (int idx : sample) if (idx >= 6) { all_colored = false; break; }
        if (all_colored) ++success;
    }
    double emp = double(success) / simulations;
    double theor = double(6 * 5 * 4 * 3) / (10 * 9 * 8 * 7); 
    return { emp, theor };
}
std::vector<Result> task68(int simulations) {
    int seq_1_4_5 = 0, set_1_4_5 = 0;
    for (int i = 0; i < simulations; ++i) {
        auto sample = sample_without_replacement(5, 3);
        if (sample[0] == 0 && sample[1] == 3 && sample[2] == 4) ++seq_1_4_5;
        std::sort(sample.begin(), sample.end());
        if (sample == std::vector<int>{0, 3, 4}) ++set_1_4_5;
    }
    double theor_seq = 1.0 / 60;
    double theor_set = 1.0 / 10;
    return { {double(seq_1_4_5) / simulations, theor_seq},
            {double(set_1_4_5) / simulations, theor_set} };
}
Result task69(int simulations) {
    int success = 0;
    for (int i = 0; i < simulations; ++i) {
        auto sample = sample_without_replacement(25, 3);
        bool all_known = true;
        for (int idx : sample) if (idx >= 20) { all_known = false; break; }
        if (all_known) ++success;
    }
    double emp = double(success) / simulations;
    double theor = double(20 * 19 * 18) / (25 * 24 * 23); 
    return { emp, theor };
}
std::vector<Result> task70(int simulations) {
    int without_rep = 0, with_rep = 0;
    for (int i = 0; i < simulations; ++i) {
        auto sample = sample_without_replacement(10, 3);
        if (sample[0] == 0 && sample[1] == 1 && sample[2] == 2) ++without_rep;
    }
    for (int i = 0; i < simulations; ++i) {
        int a = discrete_uniform(0, 9), b = discrete_uniform(0, 9), c = discrete_uniform(0, 9);
        if (a == 0 && b == 1 && c == 2) ++with_rep;
    }
    double theor_without = 1.0 / 720;
    double theor_with = 1.0 / 1000;
    return { {double(without_rep) / simulations, theor_without},
            {double(with_rep) / simulations, theor_with} };
}
Result task68_method(int simulations) {
    int n2_success = 0, n3_success = 0;
    for (int i = 0; i < simulations; ++i) {
        bool old_informed = bernoulli(0.3);
        bool stud1 = bernoulli(0.5), stud2 = bernoulli(0.5);
        bool stud3 = bernoulli(0.5);
        if (old_informed || stud1 || stud2) ++n2_success;
        if (old_informed || stud1 || stud2 || stud3) ++n3_success;
    }
    double emp2 = double(n2_success) / simulations;
    double emp3 = double(n3_success) / simulations;
    double theor2 = 1.0 - 0.7 * 0.5 * 0.5;
    double theor3 = 1.0 - 0.7 * 0.5 * 0.5 * 0.5;
    std::cout << "n=2: эмпир. = " << emp2 << ", теор. = " << theor2 << "\n";
    std::cout << "n=3: эмпир. = " << emp3 << ", теор. = " << theor3 << "\n";
    return { 0.0, 0.0 };
}
std::vector<Result> task69_method_card(int N, int K, bool with_replacement, int simulations) {
    int cnt[4] = { 0, 0, 0, 0 };
    for (int iter = 0; iter < simulations; ++iter) {
        std::vector<int> hand;
        if (with_replacement) {
            for (int i = 0; i < 4; ++i)
                hand.push_back(discrete_uniform(0, K - 1));
        }
        else {
            auto indices = sample_without_replacement(N, 4);
            for (int idx : indices) {
                int rank = idx / 4;
                hand.push_back(rank);
            }
        }
        std::sort(hand.begin(), hand.end());
        std::vector<int> counts;
        for (int i = 0; i < 4; ) {
            int j = i;
            while (j < 4 && hand[j] == hand[i]) ++j;
            counts.push_back(j - i);
            i = j;
        }
        std::sort(counts.begin(), counts.end(), std::greater<int>());
        if (counts.size() == 2 && counts[0] == 3 && counts[1] == 1)
            ++cnt[0];
        else if (counts.size() == 2 && counts[0] == 2 && counts[1] == 2)
            ++cnt[1];
        else if (counts.size() == 4 && counts[0] == 1 && counts[1] == 1 && counts[2] == 1 && counts[3] == 1)
            ++cnt[2];
        else if (counts.size() == 1 && counts[0] == 4)
            ++cnt[3];
    }
    double emp[4];
    for (int i = 0; i < 4; ++i)
        emp[i] = double(cnt[i]) / simulations;
    double theor[4];
    if (!with_replacement) {
        if (N == 36) {
            theor[0] = 1152.0 / 58905;
            theor[1] = 1296.0 / 58905;
            theor[2] = 32256.0 / 58905;
            theor[3] = 9.0 / 58905;
        }
        else {
            theor[0] = 2496.0 / 270725;
            theor[1] = 2808.0 / 270725;
            theor[2] = 183040.0 / 270725;
            theor[3] = 13.0 / 270725;
        }
    }
    else {
        if (N == 36) {
            theor[0] = 288.0 / 6561;
            theor[1] = 216.0 / 6561;
            theor[2] = 3024.0 / 6561;
            theor[3] = 9.0 / 6561;
        }
        else {
            theor[0] = 624.0 / 28561;
            theor[1] = 468.0 / 28561;
            theor[2] = 17160.0 / 28561;
            theor[3] = 13.0 / 28561;
        }
    }
    std::vector<Result> results;
    for (int i = 0; i < 4; ++i)
        results.push_back({ emp[i], theor[i] });
    return results;
}
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#else
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
    int simulations = 100000;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Моделирование вероятностей\n";
    std::cout << "Число симуляций: " << simulations << "\n\n";
    auto res57 = task57(simulations);
    std::cout << "Задача 57:\n";
    std::cout << "  а) не более чем в 3 ящиках: эмп.=" << res57[0].empirical << ", теор.=" << res57[0].theoretical << "\n";
    std::cout << "  б) не менее чем в 2 ящиках: эмп.=" << res57[1].empirical << ", теор.=" << res57[1].theoretical << "\n\n";
    auto res58 = task58(simulations);
    std::cout << "Задача 58:\n";
    std::cout << "  а) все 5: эмп.=" << res58[0].empirical << ", теор.=" << res58[0].theoretical << "\n";
    std::cout << "  б) все одинаковые: эмп.=" << res58[1].empirical << ", теор.=" << res58[1].theoretical << "\n\n";
    auto res59 = task59(simulations);
    std::cout << "Задача 59:\n";
    std::cout << "  а) на двух 1, на третьей другое: эмп.=" << res59[0].empirical << ", теор.=" << res59[0].theoretical << "\n";
    std::cout << "  б) две одинаковые, третья другая: эмп.=" << res59[1].empirical << ", теор.=" << res59[1].theoretical << "\n";
    std::cout << "  в) все разные: эмп.=" << res59[2].empirical << ", теор.=" << res59[2].theoretical << "\n\n";
    std::cout << "Задача 61:\n";
    task61(simulations);
    std::cout << "Минимальное n = 5 (теоретически)\n\n";
    auto res62 = task62(simulations);
    std::cout << "Задача 62:\n";
    std::cout << "  хотя бы одно попадание: эмп.=" << res62.empirical << ", теор.=" << res62.theoretical << "\n\n";
    auto res63 = task63(simulations);
    std::cout << "Задача 63:\n";
    std::cout << "  по одной точке в каждой части: эмп.=" << res63.empirical << ", теор.=" << res63.theoretical << "\n\n";
    auto res65 = task65(simulations);
    std::cout << "Задача 65:\n";
    std::cout << "  два выигрышных билета: эмп.=" << res65.empirical << ", теор.=" << res65.theoretical << "\n\n";
    auto res67 = task67(simulations);
    std::cout << "Задача 67:\n";
    std::cout << "  все четыре окрашенные: эмп.=" << res67.empirical << ", теор.=" << res67.theoretical << "\n\n";
    auto res68 = task68(simulations);
    std::cout << "Задача 68:\n";
    std::cout << "  а) последовательно 1,4,5: эмп.=" << res68[0].empirical << ", теор.=" << res68[0].theoretical << "\n";
    std::cout << "  б) набор {1,4,5}: эмп.=" << res68[1].empirical << ", теор.=" << res68[1].theoretical << "\n\n";
    auto res69 = task69(simulations);
    std::cout << "Задача 69:\n";
    std::cout << "  студент знает все 3 вопроса: эмп.=" << res69.empirical << ", теор.=" << res69.theoretical << "\n\n";
    auto res70 = task70(simulations);
    std::cout << "Задача 70:\n";
    std::cout << "  а) без возвращения: эмп.=" << res70[0].empirical << ", теор.=" << res70[0].theoretical << "\n";
    std::cout << "  б) с возвращением: эмп.=" << res70[1].empirical << ", теор.=" << res70[1].theoretical << "\n\n";
    std::cout << "Задача 68:\n";
    task68_method(simulations);
    std::cout << "Минимальное n = 3\n\n";
    std::cout << "Задача 69 из методички (колода карт):\n";
    std::cout << "  36 карт, без возвращения:\n";
    auto res69_36_no = task69_method_card(36, 9, false, simulations);
    std::cout << "    3+1: эмп.=" << res69_36_no[0].empirical << ", теор.=" << res69_36_no[0].theoretical << "\n";
    std::cout << "    2+2: эмп.=" << res69_36_no[1].empirical << ", теор.=" << res69_36_no[1].theoretical << "\n";
    std::cout << "    все разные: эмп.=" << res69_36_no[2].empirical << ", теор.=" << res69_36_no[2].theoretical << "\n";
    std::cout << "    каре: эмп.=" << res69_36_no[3].empirical << ", теор.=" << res69_36_no[3].theoretical << "\n";
    std::cout << "  36 карт, с возвращением:\n";
    auto res69_36_yes = task69_method_card(36, 9, true, simulations);
    std::cout << "    3+1: эмп.=" << res69_36_yes[0].empirical << ", теор.=" << res69_36_yes[0].theoretical << "\n";
    std::cout << "    2+2: эмп.=" << res69_36_yes[1].empirical << ", теор.=" << res69_36_yes[1].theoretical << "\n";
    std::cout << "    все разные: эмп.=" << res69_36_yes[2].empirical << ", теор.=" << res69_36_yes[2].theoretical << "\n";
    std::cout << "    каре: эмп.=" << res69_36_yes[3].empirical << ", теор.=" << res69_36_yes[3].theoretical << "\n";
    std::cout << "  52 карты, без возвращения:\n";
    auto res69_52_no = task69_method_card(52, 13, false, simulations);
    std::cout << "    3+1: эмп.=" << res69_52_no[0].empirical << ", теор.=" << res69_52_no[0].theoretical << "\n";
    std::cout << "    2+2: эмп.=" << res69_52_no[1].empirical << ", теор.=" << res69_52_no[1].theoretical << "\n";
    std::cout << "    все разные: эмп.=" << res69_52_no[2].empirical << ", теор.=" << res69_52_no[2].theoretical << "\n";
    std::cout << "    каре: эмп.=" << res69_52_no[3].empirical << ", теор.=" << res69_52_no[3].theoretical << "\n";
    std::cout << "  52 карты, с возвращением:\n";
    auto res69_52_yes = task69_method_card(52, 13, true, simulations);
    std::cout << "    3+1: эмп.=" << res69_52_yes[0].empirical << ", теор.=" << res69_52_yes[0].theoretical << "\n";
    std::cout << "    2+2: эмп.=" << res69_52_yes[1].empirical << ", теор.=" << res69_52_yes[1].theoretical << "\n";
    std::cout << "    все разные: эмп.=" << res69_52_yes[2].empirical << ", теор.=" << res69_52_yes[2].theoretical << "\n";
    std::cout << "    каре: эмп.=" << res69_52_yes[3].empirical << ", теор.=" << res69_52_yes[3].theoretical << "\n\n";
    return 0;
}
