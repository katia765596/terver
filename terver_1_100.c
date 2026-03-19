#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define experiment 10000000LL
static double rand_double(void) {
    return (double)rand() / (double)RAND_MAX;
}
int main(int argc, char* argv[]) {
    long long experiments = experiment;
    if (argc > 1) {
        char* endptr = NULL;
        long long input = strtoll(argv[1], &endptr, 10);
        if (endptr == argv[1] || input <= 0) {
            fprintf(stderr, "Incirrect input default value used\n");
        }
        else {
            experiments = input;
        }
    }
    srand((unsigned int)time(NULL));
    const double p1 = 0.05; 
    const double p2 = 0.10; 
    long long count_A = 0;
    long long count_H1_and_A = 0;
    for (long long i = 0; i < experiments; i++) {
        int H1 = rand_double() < p1;
        int H2 = rand_double() < p2;
        int A = H1 || H2;
        if (A) {
            count_A++;
            if (H1) {
                count_H1_and_A++;
            }
        }
    }
    if (count_A == 0) {
        fprintf(stderr, "Error: event A did not exists.\n");
        return EXIT_FAILURE;
    }
    double empirical = (double)count_H1_and_A / (double)count_A;
    double theoretical = 10.0 / 29.0;
    printf(" Conditional probability modeling \n");
    printf("Experiments: %lld\n", experiments);
    printf("P(B1|A) empirical: %.6f\n", empirical);
    printf("P(B1|A) theoretical: %.6f\n", theoretical);
    printf("Error: %.6f\n", fabs(empirical - theoretical));
    return EXIT_SUCCESS;
}
