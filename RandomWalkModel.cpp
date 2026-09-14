#include "RandomWalkModel.h"
#include <cmath>
RandomWalkModel::RandomWalkModel()
{
    std::random_device rd;
    rng.seed(rd());
}
WalkResult RandomWalkModel::simulate(std::uint64_t n, double p, double q, double s, int experiments) const
{
    WalkResult result;
    result.n = n;
    result.returned_count = 0;
    result.experiments = experiments;
    result.probability = 0.0;
    if (n == 0 || experiments <= 0 || s == 0.0)
        return result;
    if (p < 0.0 || p > 1.0 || q < 0.0 || q > 1.0)
        return result;
    if (std::abs(p + q - 1.0) > 1e-9)
        return result;
    if (n % 2 != 0) {
        result.probability = 0.0;
        return result;
    }
    std::binomial_distribution<std::uint64_t> distribution(n, p);
    std::uint64_t target = n / 2;//кол-во шагов вправо, необходимое для возврата в 0
    for (int i = 0; i < experiments; ++i) {
        std::uint64_t right_steps = distribution(rng);
        if (right_steps == target)
            ++result.returned_count;
    }
    result.probability = static_cast<double>(result.returned_count) / static_cast<double>(experiments);
    return result;
}
std::vector<WalkResult> RandomWalkModel::simulate_all(const WalkParams& params) const
{
    std::vector<WalkResult> results;
    if (params.n < 2 || params.experiments <= 0)
        return results;
    std::uint64_t current = 2;
    while (current <= params.n) {
        results.push_back(simulate(current, params.p, params.q, params.s, params.experiments));
        if (current > params.n / 2)
            break;
        current *= 2;
    }
    return results;
}
std::vector<double> RandomWalkModel::get_trajectory(std::uint64_t n, double p, double s) const
{
    std::vector<double> trajectory;
    trajectory.push_back(0.0);
    if (n == 0)
        return trajectory;
    std::bernoulli_distribution distribution(p);
    double position = 0.0;
    for (std::uint64_t i = 0; i < n; ++i) {
        if (distribution(rng))
            position += s;
        else
            position -= s;
        trajectory.push_back(position);
    }
    return trajectory;
}