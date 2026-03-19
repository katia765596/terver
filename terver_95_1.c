#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define iter 1000000
#define all_card 52
double rand01() {
    return (double)rand() / (RAND_MAX + 1.0);
}
int draw_two_cards() {
    int cards[all_card];
    for (int i = 0; i < all_card; i++) {
        if (i < 4)
            cards[i] = 1; 
        else if (i < 17)
            cards[i] = 2; 
        else
            cards[i] = 3; 
    }
    int idx1 = rand() % all_card;
    int card1 = cards[idx1];
    cards[idx1] = cards[all_card - 1];
    int idx2 = rand() % (all_card - 1);
    int card2 = cards[idx2];
    if ((card1 == 1 && card2 == 1) || (card1 == 2 && card2 == 2))
        return 1;
    else
        return 0; 
}
double run_simulation(int iterations) {
    int success = 0;
    for (int i = 0; i < iterations; i++) {
        success += draw_two_cards();
    }
    return (double)success / iterations;
}
double theoretical_probability() {
    double p1 = 4.0 / 52.0 * 3.0 / 51.0;   
    double p2 = 13.0 / 52.0 * 12.0 / 51.0; 
    return p1 + p2;
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

