#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define experiments 10000000LL
static double rand_double(void) {
    return (double)rand() / RAND_MAX;
}
int main() {
    srand((unsigned int)time(NULL));
    const double pH1 = 0.55;
    const double pH2 = 0.45;
    const double pA_H1 = 0.9;
    const double pA_H2 = 0.98;
    long long countA = 0;      
    long long countH2A = 0;    
    for (long long i = 0; i < experiments; i++) {
        int isH2 = rand_double() < pH2;
        int A = isH2 ? (rand_double() < pA_H2)
            : (rand_double() < pA_H1);
        if (A) {
            countA++;
            if (isH2) countH2A++;
        }
    }
    if (countA == 0) {
        fprintf(stderr, "Error^ event A did not exists\n");
        return 1;
    }
    double empirical = (double)countH2A / (double)countA;
    double theoretical = 0.441 / 0.936;
    printf("Empirical P(H2|A): %.6f\n", empirical);
    printf("Theoretical P(H2|A): %.6f\n", theoretical);
    printf("Error: %.6f\n", fabs(empirical - theoretical));
    return 0;
}