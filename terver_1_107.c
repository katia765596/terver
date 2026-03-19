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
    const double p1 = 0.6;
    const double p2 = 0.5;
    const double p3 = 0.4;
    long long countA = 0;       
    long long countH3A = 0;     
    for (long long i = 0; i < experiments; i++) {
        int H1 = rand_double() < p1;
        int H2 = rand_double() < p2;
        int H3 = rand_double() < p3;
        int sum = H1 + H2 + H3;
        if (sum == 2) {
            countA++;
            if (H3) countH3A++;
        }
    }
    if (countA == 0) {
        fprintf(stderr, "Error: event A did not exists\n");
        return 1;
    }
    double empirical = (double)countH3A / (double)countA;
    double theoretical = 10.0 / 19.0;
    printf("Empirical P(H3|A): %.6f\n", empirical);
    printf("Theoretical P(H3|A): %.6f\n", theoretical);
    printf("Error: %.6f\n", fabs(empirical - theoretical));
    return 0;
}
