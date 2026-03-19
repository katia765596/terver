#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <locale.h>
#define n 65536          
#define max_k 600
#define experiments 20000
int rand_int(int max) {
    return rand() % max;
}
uint16_t encrypt(uint16_t m, uint16_t key) {
    return m ^ key;
}
void vernam_attack(int k) {
    uint16_t key = rand() % 65536;
    uint16_t X1[1000];
    uint16_t X2[1000];
    for (int i = 0; i < k; i++) {
        X1[i] = rand() % 65536;
        X2[i] = rand() % 65536;
    }
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            if (encrypt(X1[i], key) == encrypt(X2[j], key)) {
                printf("\nfound collizion\n");
                printf("x1 = %u, x2 = %u\n", X1[i], X2[j]);
                printf("E(x1) = E(x2) = %u\n", encrypt(X1[i], key));
                return;
            }
        }
    }
    printf("\ncollizion not found(increase k)\n");
}
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));
    FILE* f = fopen("text2_results.txt", "w");
    if (!f) {
        printf("Error open file\n");
        return 1;
    }
    printf("k\tEmpirical\tTheory\t\tDiff\n");
    fprintf(f, "k Empirical Theory Diff\n");
    for (int k = 10; k <= max_k; k += 10) {
        int success = 0;
        for (int exp = 0; exp < experiments; exp++) {
            char used[n];
            memset(used, 0, sizeof(used));
            for (int i = 0; i < k; i++) {
                int x = rand_int(n);
                used[x] = 1;
            }
            int intersect = 0;
            for (int i = 0; i < k; i++) {
                int y = rand_int(n);
                if (used[y]) {
                    intersect = 1;
                    break;
                }
            }
            if (intersect) success++;
        }
        double R_emp = (double)success / experiments;
        double R_theory = 1 - exp(-(double)k * k / n);
        double diff = fabs(R_emp - R_theory);
        printf("%d\t%.6f\t%.6f\t%.6f\n", k, R_emp, R_theory, diff);
        fprintf(f, "%d %.6f %.6f %.6f\n", k, R_emp, R_theory, diff);
    }
    fclose(f);
    double k_half = sqrt(n * log(2));
    printf("\nThreshold (R > 0.5): k ≈ %.2f\n", k_half);
    printf("\nDemonstration of an attack on the Vernam cipher\n");
    vernam_attack((int)k_half);
    return 0;
}