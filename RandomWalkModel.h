#ifndef RANDOMWALKMODEL_H
#define RANDOMWALKMODEL_H
#include <vector>
#include <random>
#include <cstdint>
struct WalkParams {
    double p;
    double q;
    std::uint64_t n;
    double s;
    int experiments;
};
struct WalkResult {
    std::uint64_t n;
    int returned_count;
    int experiments;
    double probability;
};
class RandomWalkModel {
public:
    RandomWalkModel();
    WalkResult simulate(std::uint64_t n, double p, double q, double s, int experiments) const;
    std::vector<WalkResult> simulate_all(const WalkParams& params) const;
    std::vector<double> get_trajectory(std::uint64_t n, double p, double s) const;
private:
    mutable std::mt19937_64 rng;
};
#endif