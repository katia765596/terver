#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int is_hit(double probability) {
    return (rand() / (double)RAND_MAX) < probability;
}
double run_experiment(int trials, double p1, double p2) {
    int hits = 0;
    for (int i = 0; i < trials; i++) {
        int hit1 = is_hit(p1);
        int hit2 = is_hit(p2);
        if (hit1 || hit2) {
            hits++;
        }
    }
    return (double)hits / trials;
}
int get_valid_trials() {
    int trials;
    while (1) {
        printf("Enter a positive number for the number of trials ");
        if (scanf("%d", &trials) != 1 || trials <= 0) {
            printf("ERROR: input a positive number for the number of trials.\n");
            while (getchar() != '\n');
        }
        else {
            return trials;
        }
    }
}
int main() {
    srand(time(NULL));
    double p1 = 0.95;
    double p2 = 0.8;
    int trials = get_valid_trials();
    double result = run_experiment(trials, p1, p2);
    printf("Empirical probability of hitting at least one arrow %.4f\n", result);
    return 0;
} программа не работает у меня код не запускается