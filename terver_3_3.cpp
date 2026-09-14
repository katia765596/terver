#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
using namespace std;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<double> dist(0.0, 1.0);
struct Simulation {
    int AM, RM;
    int n;
    vector<double> recipe;
    int t_sb, t_pr;
    vector<double> speed;
    int m_sb, m_pr;
    vector<vector<double>> stock;      
    vector<double> products;           
    vector<int> times3, times4, times5;
    int count_legendary = 0;
    vector<int> sb_finish, pr_finish;
    vector<int> sb_output, pr_output;  
    int current_time = 0;
    Simulation(int AM_, int RM_, int n_, const vector<double>& recipe_,
        int t_sb_, int t_pr_, const vector<double>& speed_,
        int m_sb_ = 0, int m_pr_ = 0)
        : AM(AM_), RM(RM_), n(n_), recipe(recipe_),
        t_sb(t_sb_), t_pr(t_pr_), speed(speed_),
        m_sb(m_sb_), m_pr(m_pr_)
    {
        stock.assign(n, vector<double>(6, 0.0));
        products.assign(6, 0.0);
        sb_finish.assign(AM, 0);
        pr_finish.assign(RM, 0);
        sb_output.assign(AM, 0);
        pr_output.assign(RM, 0);
    }
    bool can_assemble() const {
        for (int i = 0; i < n; ++i) {
            double total = 0.0;
            for (int lv = 1; lv <= 5; ++lv)
                total += stock[i][lv];
            if (total < recipe[i] - 1e-9)
                return false;
        }
        return true;
    }
    bool try_assemble() {
        int idx = -1;
        for (int i = 0; i < AM; ++i) {
            if (sb_finish[i] == 0) {
                idx = i;
                break;
            }
        }
        if (idx == -1) return false;
        if (!can_assemble()) return false;

        int L = 5;
        for (int i = 0; i < n; ++i) {
            double need = recipe[i];
            for (int lv = 5; lv >= 1 && need > 1e-9; --lv) {
                double take = min(stock[i][lv], need);
                if (take > 1e-9) {
                    stock[i][lv] -= take;
                    need -= take;
                    L = min(L, lv);
                }
            }
            if (need > 1e-9) return false;
        }
        int newLevel = L;
        vector<double> probs(5, 0.0);
        double prob_sum = 0.0;
        for (int k = 1; k <= 4; ++k) {
            probs[k] = m_sb * 52.0 * pow(10.0, -(2 + k));
            prob_sum += probs[k];
        }
        if (prob_sum > 1.0) prob_sum = 1.0;
        double r = dist(gen);
        double cum = 0.0;
        int chosen_k = 0;
        for (int k = 1; k <= 4; ++k) {
            cum += probs[k];
            if (r <= cum) {
                chosen_k = k;
                break;
            }
        }
        if (chosen_k > 0) {
            newLevel = min(5, L + chosen_k);
        }
        sb_output[idx] = newLevel;
        int duration = (int)ceil(t_sb * (1.0 + 0.1 * m_sb));
        sb_finish[idx] = current_time + duration;
        return true;
    }
    bool can_recycle() const {
        for (int lv = 1; lv <= 4; ++lv) {
            if (products[lv] > 1e-9)
                return true;
        }
        return false;
    }
    bool try_recycle() {
        if (RM == 0) return false;
        int idx = -1;
        for (int i = 0; i < RM; ++i) {
            if (pr_finish[i] == 0) {
                idx = i;
                break;
            }
        }
        if (idx == -1) return false;
        if (!can_recycle()) return false;
        int L = 0;
        for (int lv = 4; lv >= 1; --lv) {
            if (products[lv] > 1e-9) {
                L = lv;
                break;
            }
        }
        if (L == 0) return false;
        products[L] -= 1.0;
        int newLevel = L;
        vector<double> probs(5, 0.0);
        double prob_sum = 0.0;
        for (int k = 1; k <= 4; ++k) {
            probs[k] = m_pr * 52.0 * pow(10.0, -(2 + k));
            prob_sum += probs[k];
        }
        if (prob_sum > 1.0) prob_sum = 1.0;
        double r = dist(gen);
        double cum = 0.0;
        int chosen_k = 0;
        for (int k = 1; k <= 4; ++k) {
            cum += probs[k];
            if (r <= cum) {
                chosen_k = k;
                break;
            }
        }
        if (chosen_k > 0) {
            newLevel = min(5, L + chosen_k);
        }
        pr_output[idx] = newLevel;
        int duration = (int)ceil(t_pr * (1.0 + 0.1 * m_pr));
        pr_finish[idx] = current_time + duration;
        return true;
    }
    void complete_operations() {
        for (int i = 0; i < AM; ++i) {
            if (sb_finish[i] != 0 && sb_finish[i] == current_time) {
                int lv = sb_output[i];
                if (lv > 0) {
                    products[lv] += 1.0;
                    if (lv == 3) times3.push_back(current_time);
                    else if (lv == 4) times4.push_back(current_time);
                    else if (lv == 5) {
                        times5.push_back(current_time);
                        count_legendary++;
                    }
                }
                sb_output[i] = 0;
                sb_finish[i] = 0;
            }
        }
        for (int i = 0; i < RM; ++i) {
            if (pr_finish[i] != 0 && pr_finish[i] == current_time) {
                int lv = pr_output[i];
                if (lv > 0) {
                    for (int j = 0; j < n; ++j) {
                        stock[j][lv] += 0.25 * recipe[j];
                    }
                }
                pr_output[i] = 0;
                pr_finish[i] = 0;
            }
        }
    }
    void run() {
        while (count_legendary < 25) {
            for (int i = 0; i < n; ++i) {
                stock[i][1] += speed[i];
            }
            complete_operations();
            if (count_legendary >= 25) break;
            bool assembled = true;
            while (assembled) assembled = try_assemble();
            bool recycled = true;
            while (recycled) recycled = try_recycle();
            if (count_legendary >= 25) break;
            current_time++;
            if (current_time > 10000000) {
                cerr << "Simulation aborted: too long, no progress.\n";
                break;
            }
        }
    }
    void print_results() const {
        auto print_vec_with_header = [](const string& header, const vector<int>& v) {
            cout << header << " (count=" << v.size() << "):\n";
            if (v.empty()) {
                cout << "  (none)\n\n";
                return;
            }
            for (size_t i = 0; i < v.size(); ++i) {
                if (i && i % 10 == 0) cout << "\n  ";
                if (i) cout << ' ';
                cout << v[i];
            }
            cout << "\n\n";
            };

        print_vec_with_header("Times for level 3 (quality 3)", times3);
        print_vec_with_header("Times for level 4 (quality 4)", times4);
        print_vec_with_header("Times for level 5 (quality 5, legendary)", times5);
    }
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 8) {
            cerr << "Usage: " << argv[0]
                << " AM RM n a1 a2 ... an t_sb t_pr s1 s2 ... sn [m_sb m_pr]\n";
            return 1;
        }
        int pos = 1;
        int AM = stoi(argv[pos++]);
        int RM = stoi(argv[pos++]);
        int n = stoi(argv[pos++]);
        vector<double> recipe;
        for (int i = 0; i < n; ++i) {
            recipe.push_back(stod(argv[pos++]));
        }
        int t_sb = stoi(argv[pos++]);
        int t_pr = stoi(argv[pos++]);
        vector<double> speed;
        for (int i = 0; i < n; ++i) {
            speed.push_back(stod(argv[pos++]));
        }
        int m_sb = 0, m_pr = 0;
        if (pos < argc) m_sb = stoi(argv[pos++]);
        if (pos < argc) m_pr = stoi(argv[pos++]);

        if (AM < 1 || RM < 0 || n < 1 || t_sb < 1 || t_pr < 1) {
            cerr << "Error: AM, n, t_sb, t_pr must be >= 1; RM >= 0.\n";
            return 1;
        }
        for (double a : recipe) if (a < 1) {
            cerr << "Error: component quantities must be >= 1.\n";
            return 1;
        }
        for (double s : speed) if (s < 0) {
            cerr << "Error: supply rates must be >= 0.\n";
            return 1;
        }
        if (m_sb < 0 || m_sb > 4 || m_pr < 0 || m_pr > 4) {
            cerr << "Error: number of quality modules must be between 0 and 4.\n";
            return 1;
        }
        Simulation sim(AM, RM, n, recipe, t_sb, t_pr, speed, m_sb, m_pr);
        sim.run();
        sim.print_results();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        cerr << "Unknown error.\n";
        return 1;
    }
    return 0;
}