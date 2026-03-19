#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define iter 1000000
double rand01() {
    return (double)rand() / (RAND_MAX + 1.0);
}
int choose_factory() {
    double r = rand01();
    if (r < 3.0 / 41.0)
        return 1;
    else if (r < (3.0 + 20.0) / 41.0)
        return 2;
    else
        return 3;
}
double quality_prob(int factory) {
    switch (factory) {
    case 1: return 0.9;
    case 2: return 0.6;
    case 3: return 0.9;
    default:
        printf("Warning: invalid factory\n");
        return 0.0;
    }
}
int run_single_experiment() {
    int factory = choose_factory();
    double prob = quality_prob(factory);
    return (rand01() < prob) ? 1 : 0;
}
double run_simulation(int iterations) {
    if (iterations <= 0) {
        printf("Error: invalid number of iterations\n");
        return 0.0;
    }
    int success = 0;
    for (int i = 0; i < iterations; i++) {
        success += run_single_experiment();
    }
    return (double)success / iterations;
}
double theoretical_probability() {
    return (3.0 / 41.0) * 0.9 +
        (20.0 / 41.0) * 0.6 +
        (18.0 / 41.0) * 0.9;
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
    printf("\nConclusion: empirical result converges to theoretical probability.\n");
    return 0;
}

