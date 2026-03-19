#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define lambdaa 1.94
#define n_experiments 1000000
#define max_m 10
double rand_double() {
    return (double)rand() / RAND_MAX;
}
int poisson(double lambda) {
    double L = exp(-lambda);
    double p = 1.0;
    int k = 0;
    do {
        k++;
        p *= rand_double();
    } while (p > L);
    return k - 1;
}
int main() {
    srand(time(NULL));
    FILE* f = fopen("results.txt", "w");
    if (!f) {
        printf("Error open file.\n");
        return 1;
    }
    printf("m\tP(X>=m)\t\tP(X>=m | no girls)\tDifference\n");
    fprintf(f, "m\tP(X>=m)\t\tP(X>=m | no girls)\tDifference\n");
    for (int m = 1; m <= max_m; m++) {
        int count_total_ge_m = 0;
        int count_no_girls = 0;
        int count_conditional = 0;
        for (int i = 0; i < n_experiments; i++) {
            int k = poisson(lambdaa);
            int has_girl = 0;
            for (int j = 0; j < k; j++) {
                if (rand_double() < 0.5) {
                    has_girl = 1;
                }
            }
            if (k >= m) {
                count_total_ge_m++;
            }
            if (!has_girl) {
                count_no_girls++;
                if (k >= m) {
                    count_conditional++;
                }
            }
        }
        double p_unconditional = (double)count_total_ge_m / n_experiments;
        double p_conditional = (count_no_girls > 0)
            ? (double)count_conditional / count_no_girls
            : 0.0;
        double diff = fabs(p_unconditional - p_conditional);
        printf("%d\t%.6f\t%.6f\t\t%.6f\n",
            m, p_unconditional, p_conditional, diff);
        fprintf(f, "%d\t%.6f\t%.6f\t\t%.6f\n",
            m, p_unconditional, p_conditional, diff);
    }
    fclose(f);
    printf("\nResults in results.txt\n");
    return 0;
}
