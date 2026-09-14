#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <random>
using namespace std;
double freq[26] = {
    8.167,1.492,2.782,4.253,12.702,2.228,2.015,
    6.094,6.966,0.153,0.772,4.025,2.406,
    6.749,7.507,1.929,0.095,5.987,6.327,
    9.056,2.758,0.978,2.360,0.150,1.974,0.074
};
int encrypt(int m, int k) {return m ^ k;}
double p_cipher(int c,const int messages[],const double p_m[],int m_count,const int keys[],const double p_k[],int k_count) {
    double p = 0;
    for (int i = 0; i < m_count; i++) {
        for (int j = 0; j < k_count; j++) {
            if (encrypt(messages[i], keys[j]) == c)
                p += p_m[i] * p_k[j];
        }
    }
    return p;
}
double p_cipher_given_message(int c,int m,const int keys[],const double p_k[],int k_count) {
    double p = 0;
    for (int i = 0; i < k_count; i++) {
        if (encrypt(m, keys[i]) == c)
            p += p_k[i];
    }
    return p;
}
double p_message_given_cipher(int c,int m,const double p_m[],int m_index,const int keys[],const double p_k[],int k_count,double p_c) {
    double p = p_cipher_given_message(c, m, keys, p_k, k_count);
    return p * p_m[m_index] / p_c;
}
string clear_text(string s) {
    string r;
    for (char c : s) {
        if (isalpha((unsigned char)c))
            r += tolower((unsigned char)c);
    }
    return r;
}
string vigenere_encrypt(string text, string key) {
    string r;
    for (int i = 0; i < (int)text.size(); i++) {
        int a = text[i] - 'a';
        int b = key[i % key.size()] - 'a';
        r += char('a' + (a + b) % 26);
    }
    return r;
}
string vigenere_decrypt(string text, string key) {
    string r;
    for (int i = 0; i < (int)text.size(); i++) {
        int a = text[i] - 'a';
        int b = key[i % key.size()] - 'a';
        r += char('a' + (a - b + 26) % 26);
    }
    return r;
}
double get_ic(string s) {
    if (s.size() < 2) return 0;
    int count[26] = {};
    for (char c : s)
        count[c - 'a']++;
    double sum = 0;
    for (int i = 0; i < 26; i++)
        sum += count[i] * (count[i] - 1);
    return sum / (s.size() * (s.size() - 1));
}
double get_avg_ic(string text, int key_len) {
    double sum = 0;
    int count = 0;
    for (int pos = 0; pos < key_len; pos++) {
        string part;
        for (int i = pos; i < (int)text.size(); i += key_len)
            part += text[i];
        if (part.size() >= 2) {
            sum += get_ic(part);
            count++;
        }
    }
    if (count == 0) return 0;
    return sum / count;
}
int find_key_len(string text, int max_len) {
    double best = -1;
    int best_len = 1;
    for (int len = 1; len <= max_len; len++) {
        double value = get_avg_ic(text, len);
        if (value > best) {
            best = value;
            best_len = len;
        }
    }
    return best_len;
}
double chi(string s, int shift) {
    vector<int> count(26);
    for (char c : s) {
        int x = (c - 'a' - shift + 26) % 26;
        count[x]++;
    }
    double result = 0;
    for (int i = 0; i < 26; i++) {
        double expected = freq[i] * s.size() / 100.0;
        if (expected > 0)
            result +=
            (count[i] - expected) *
            (count[i] - expected) /
            expected;
    }
    return result;
}
char find_letter(string part) {
    double best = 1e100;
    int best_shift = 0;
    for (int shift = 0; shift < 26; shift++) {
        double value = chi(part, shift);
        if (value < best) {
            best = value;
            best_shift = shift;
        }
    }
    return char('a' + best_shift);
}
string vigenere_attack(string cipher, int key_len) {
    string key;
    for (int pos = 0; pos < key_len; pos++) {
        string part;
        for (int i = pos; i < (int)cipher.size(); i += key_len)
            part += cipher[i];
        key += find_letter(part);
    }
    return key;
}
string xor_text(const string& text, const string& key) {
    string result = text;
    for (int i = 0; i < (int)text.size(); i++)
        result[i] = text[i] ^ key[i];
    return result;
}
string make_key(int n, mt19937& gen) {
    uniform_int_distribution<int> dist(0, 255);
    string key(n, 0);
    for (int i = 0; i < n; i++)
        key[i] = (char)dist(gen);
    return key;
}
void print_hex(const string& s) {
    for (unsigned char c : s)
        cout << hex << setw(2) << setfill('0') << (int)c << ' ';
    cout << dec << '\n';
}
void probability_demo() {
    int messages[] = { 0, 1 };
    double p_m[] = { 0.6, 0.4 };
    int keys[] = { 0, 1 };
    double p_k[] = { 0.7, 0.3 };
    int m_count = 2;
    int k_count = 2;
    double p_c0 = p_cipher(0, messages, p_m, m_count,keys, p_k, k_count);
    double p_c1 = p_cipher(
        1, messages, p_m, m_count,
        keys, p_k, k_count
    );
    cout << fixed << setprecision(4);
    cout << "\np(c=0) = " << p_c0 << '\n';
    cout << "p(c=1) = " << p_c1 << '\n';
    cout << "\np(c=0|m=0) = " << p_cipher_given_message(0, 0, keys, p_k, k_count) << '\n';
    cout << "p(c=1|m=0) = "<< p_cipher_given_message(1, 0, keys, p_k, k_count) << '\n';
    cout << "\np(m=0|c=0) = "<< p_message_given_cipher(0, 0, p_m, 0,keys, p_k, k_count, p_c0) << '\n';
    cout << "p(m=1|c=0) = "<< p_message_given_cipher(0, 1, p_m, 1,keys, p_k, k_count, p_c0) << '\n';
}
void vigenere_demo() {
    string text;
    string key;
    cout << "\ntext: ";
    cin.ignore();
    getline(cin, text);
    cout << "key: ";
    cin >> key;
    text = clear_text(text);
    key = clear_text(key);
    if (text.empty() || key.empty()) {
        cout << "wrong data\n";
        return;
    }
    string cipher = vigenere_encrypt(text, key);
    cout << "\ncipher:\n";
    cout << cipher << '\n';
    int max_len = min(12, (int)cipher.size());
    int key_len = find_key_len(cipher, max_len);
    cout << "\nfound key length: ";
    cout << key_len << '\n';
    string found_key = vigenere_attack(cipher, key_len);
    string found_text = vigenere_decrypt(cipher, found_key);
    cout << "found key: ";
    cout << found_key << '\n';
    cout << "\nfound text:\n";
    cout << found_text << '\n';
}
void vernam_demo() {
    string m1;
    string m2;
    string known;
    cout << "\nm1: ";
    cin.ignore();
    getline(cin, m1);
    cout << "m2: ";
    getline(cin, m2);
    if (m1.size() != m2.size() || m1.empty()) {
        cout << "texts must have same length\n";
        return;
    }
    cout << "known part of m1: ";
    getline(cin, known);
    if (known.empty() || known.size() > m1.size()) {
        cout << "wrong known part\n";
        return;
    }
    mt19937 gen(random_device{}());
    string key = make_key(m1.size(), gen);
    string c1 = xor_text(m1, key);
    string c2 = xor_text(m2, key);
    cout << "\nc1: ";
    print_hex(c1);
    cout << "c2: ";
    print_hex(c2);
    string x = xor_text(c1, c2);
    cout << "\nc1 xor c2: ";
    print_hex(x);
    string key_part;
    for (int i = 0; i < (int)known.size(); i++)
        key_part += c1[i] ^ known[i];
    string found_part;
    for (int i = 0; i < (int)key_part.size(); i++)
        found_part += c2[i] ^ key_part[i];
    cout << "\nfound part of m2: ";
    cout << found_part << '\n';
}
int main() {
    int mode;
    cout << "1 - probability\n";
    cout << "2 - vigenere\n";
    cout << "3 - vernam\n";
    cout << "mode: ";
    cin >> mode;
    if (mode == 1) probability_demo();
    else if (mode == 2)vigenere_demo();
    else if (mode == 3)vernam_demo();
    else cout << "wrong mode\n";
    return 0;
}