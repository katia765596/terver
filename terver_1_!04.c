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
    const double pH1 = 0.6;
    const double pH2 = 0.3;
    const double pH3 = 0.1;
    const double pA_H1 = 0.6;
    const double pA_H2 = 0.3;
    const double pA_H3 = 0.6; 
    long long countH3 = 0;     
    long long countA_H3 = 0; 
    for (long long i = 0; i < experiments; i++) {
        double r = rand_double();
        int H;
        if (r < pH1) H = 1;
        else if (r < pH1 + pH2) H = 2;
        else H = 3;
        int A = 0;
        if (H == 1) A = rand_double() < pA_H1;
        else if (H == 2) A = rand_double() < pA_H2;
        else A = rand_double() < pA_H3;
        if (H == 3) {
            countH3++;
            if (A) countA_H3++;
        }
    }
    if (countH3 == 0) {
        fprintf(stderr, "Error: not experiments in H3\n");
        return 1;
    }
    double empirical = (double)countA_H3 / (double)countH3;
    printf("Empirical P(A|H3): %.6f\n", empirical);
    printf("Theoterical P(A|H3): %.6f\n", 0.6);
    printf("Error: %.6f\n", fabs(empirical - 0.6));
    return 0;
}
