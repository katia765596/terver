#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#define alph 26
#define text_len 256
void safe_input(char* buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}
void vigenere_encrypt(char* text, char* key, char* out) {
    int key_len = strlen(key);
    for (int i = 0; text[i]; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            out[i] = ((text[i] - 'A' + key[i % key_len] - 'A') % 26) + 'A';
        }
        else {
            out[i] = text[i];
        }
    }
    out[strlen(text)] = '\0';
}
void frequency_analysis(char* text) {
    int freq[alph] = { 0 };
    for (int i = 0; text[i]; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            freq[text[i] - 'A']++;
        }
    }
    printf("\nЧастоты:\n");
    for (int i = 0; i < alph; i++) {
        printf("%c: %d\n", 'A' + i, freq[i]);
    }
}
void vigenere_attack(char* cipher) {
    printf("\nатака виженера\n");
    frequency_analysis(cipher);
    printf("\nПредположение: самая частая буква ≈ 'E'\n");
    printf("Это позволяет оценить сдвиг ключа.\n");
}
void vernam_encrypt(char* text, char* key, char* out) {
    for (int i = 0; text[i]; i++) {
        out[i] = text[i] ^ key[i];
    }
    out[strlen(text)] = '\0';
}
void vernam_attack(char* c1, char* c2) {
    printf("\nатака вернама\n");
    char result[text_len];
    for (int i = 0; c1[i]; i++) {
        result[i] = c1[i] ^ c2[i];
    }
    result[strlen(c1)] = '\0';
    printf("C1 XOR C2 = %s\n", result);
    printf("Это равно M1 XOR M2 возможна изменение текста!\n");
}
void probability_demo() {
    double Pm[3] = { 0.5, 0.3, 0.2 };
    double Pk[2] = { 0.6, 0.4 };
    int E[2][3] = {
        {0, 1, 2},
        {1, 2, 0}
    };
    double Pc[3] = { 0 };
    for (int l = 0; l < 3; l++) {
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 2; i++) {
                if (E[i][j] == l) {
                    Pc[l] += Pm[j] * Pk[i];
                }
            }
        }
    }
    printf("\nP(C = c_l):\n");
    for (int l = 0; l < 3; l++) {
        printf("c%d: %.3f\n", l, Pc[l]);
    }
    printf("\nP(C | M):\n");
    for (int j = 0; j < 3; j++) {
        for (int l = 0; l < 3; l++) {
            double val = 0;
            for (int i = 0; i < 2; i++) {
                if (E[i][j] == l) val += Pk[i];
            }
            printf("%.3f ", val);
        }
        printf("\n");
    }
    printf("\nP(M | C):\n");
    for (int j = 0; j < 3; j++) {
        for (int l = 0; l < 3; l++) {
            double num = 0;
            for (int i = 0; i < 2; i++) {
                if (E[i][j] == l) num += Pk[i];
            }
            num *= Pm[j];
            double val = (Pc[l] > 0) ? num / Pc[l] : 0;
            printf("%.3f ", val);
        }
        printf("\n");
    }
}
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));
    printf("Вероятности\n");
    probability_demo();
    char text[text_len];
    char key[text_len];
    printf("\nВведите текст (A-Z): ");
    safe_input(text, text_len);
    printf("Введите ключ Виженера: ");
    safe_input(key, text_len);
    char cipher[text_len];
    vigenere_encrypt(text, key, cipher);
    printf("\nШифротекст: %s\n", cipher);
    vigenere_attack(cipher);
    char key2[text_len];
    char c1[text_len], c2[text_len];
    for (int i = 0; text[i]; i++) {
        key2[i] = rand() % 256;
    }
    key2[strlen(text)] = '\0';
    vernam_encrypt(text, key2, c1);
    vernam_encrypt(text, key2, c2);
    vernam_attack(c1, c2);
    return 0;
}