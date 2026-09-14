#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <cmath>
#include <iomanip>
using namespace std;
struct doc {
    string type;
    vector<string> words;
};
struct model {
    unordered_map<string, int> spam_words;
    unordered_map<string, int> ham_words;
    unordered_set<string> dict;//словарь
    int spam_docs = 0;//кол-во писем
    int ham_docs = 0;
    int spam_total = 0;//кол-во слов в письмах класса
    int ham_total = 0;
};
vector<string> split_words(string s) {
    stringstream ss(s);
    string w;
    vector<string> words;
    while (ss >> w)
        words.push_back(w);
    return words;
}
vector<doc> read_data(string file_name) {
    ifstream file(file_name);
    vector<doc> data;
    if (!file)
        return data;
    string line;
    while (getline(file, line)) {
        size_t pos = line.find('\t');
        if (pos == string::npos)
            continue;
        string type = line.substr(0, pos);
        string text = line.substr(pos + 1);
        if (type != "spam" && type != "ham")
            continue;
        doc d;
        d.type = type;
        d.words = split_words(text);
        if (!d.words.empty())
            data.push_back(d);
    }
    return data;
}
model train_model(const vector<doc>& data, int count) {
    model m;
    for (int i = 0; i < count; i++) {
        if (data[i].type == "spam")
            m.spam_docs++;
        else
            m.ham_docs++;
        for (string w : data[i].words) {
            m.dict.insert(w);
            if (data[i].type == "spam") {
                m.spam_words[w]++;
                m.spam_total++;
            }
            else {
                m.ham_words[w]++;
                m.ham_total++;
            }
        }
    }
    return m;
}
double word_prob(const model& m, const string& word, bool spam) {
    int count = 0;
    int total = 0;
    if (spam) {auto it = m.spam_words.find(word);
        if (it != m.spam_words.end())
            count = it->second;
        total = m.spam_total;
    }
    else {
        auto it = m.ham_words.find(word);
        if (it != m.ham_words.end())
            count = it->second;
        total = m.ham_total;
    }
    double alpha = 1.0;
    double v = m.dict.size();
    return (count + alpha) / (total + alpha * v);
}
string predict(const model& m, const vector<string>& words) {
    double all_docs = m.spam_docs + m.ham_docs;
    double spam_score =log((double)m.spam_docs / all_docs);
    double ham_score = log((double)m.ham_docs / all_docs);
    for (string w : words) {
        spam_score += log(word_prob(m, w, true));
        ham_score += log(word_prob(m, w, false));
    }
    if (spam_score > ham_score)
        return "spam";
    return "ham";
}
int main() {
    vector<doc> data = read_data("data.txt");
    if (data.size() < 2) {
        cout << "not enough data\n";
        return 0;
    }
    mt19937 gen(random_device{}());
    shuffle(data.begin(), data.end(), gen);
    int train_count = (int)(data.size() * 0.7);
    if (train_count <= 0)
        train_count = 1;
    if (train_count >= (int)data.size())
        train_count = data.size() - 1;
    model m = train_model(data, train_count);
    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;
    for (int i = train_count; i < (int)data.size(); i++) {
        string result = predict(m, data[i].words);
        if (data[i].type == "spam" && result == "spam")
            tp++;
        if (data[i].type == "ham" && result == "ham")
            tn++;
        if (data[i].type == "ham" && result == "spam")
            fp++;
        if (data[i].type == "spam" && result == "ham")
            fn++;
    }
    int all = tp + tn + fp + fn;
    double accuracy = 0;
    double precision = 0;
    double recall = 0;
    double f1 = 0;
    if (all > 0)
        accuracy = (double)(tp + tn) / all;
    if (tp + fp > 0)
        precision = (double)tp / (tp + fp);
    if (tp + fn > 0)
        recall = (double)tp / (tp + fn);
    if (precision + recall > 0)
        f1 = 2 * precision * recall /(precision + recall);
    cout << fixed << setprecision(4);
    cout << "documents: " << data.size() << '\n';
    cout << "train: " << train_count << '\n';
    cout << "test: " << data.size() - train_count << '\n';
    cout << "\ntp: " << tp << '\n';
    cout << "tn: " << tn << '\n';
    cout << "fp: " << fp << '\n';
    cout << "fn: " << fn << '\n';
    cout << "\naccuracy: " << accuracy << '\n';
    cout << "precision: " << precision << '\n';
    cout << "recall: " << recall << '\n';
    cout << "f1: " << f1 << '\n';
    cout << "\ntext: ";
    cin.ignore();
    string text;
    getline(cin, text);
    vector<string> words = split_words(text);
    if (words.empty()) {
        cout << "empty text\n";
        return 0;
    }
    cout << "result: "<< predict(m, words) << '\n';
    return 0;
}