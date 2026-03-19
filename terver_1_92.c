#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define iter 1000000
double rand01() {
    return (double)rand() / RAND_MAX;
}
double theoretical_probability() {
    return (3.0 / 5.0) * 0.95 + (2.0 / 5.0) * 0.7;
}
double run_simulation(int iterations) {
    if (iterations <= 0) return 0.0;
    int hits = 0;
    for (int i = 0; i < iterations; i++) {
        double r = rand01();
        double hit_prob = (r < 3.0 / 5.0) ? 0.95 : 0.7;
        if (rand01() < hit_prob)
            hits++;
    }
    return (double)hits / iterations;
}
int main() {
    srand(42); 
    double empirical = run_simulation(iter);
    double theoretical = theoretical_probability();
    double error = fabs(empirical - theoretical);
    printf("Iterations: %d\n", iter);
    printf("Empirical probability: %.6f\n", empirical);
    printf("Theoretical probability: %.6f\n", theoretical);
    printf("Absolute error: %.6f\n", error);
    return 0;
}

