#include "trajectorymodel.h"
#include <cmath>
#include <complex>
#include <random>
TrajectoryModel::TrajectoryModel(const SimulationConfig& config) : config(config)
{
    std::random_device rd;
    rng.seed(rd());
}
SimulationResult TrajectoryModel::simulate_one() const
{
    SimulationResult result;
    result.points.push_back(QPointF(0.0, 0.0));
    std::complex<double> z(0.0, 0.0);
    auto dist = Distribution::create(config.dist_type, config.n, config.dist_params);
    double eps = std::max(1e-12, std::abs(config.rho) * 1e-9);
    for (int step = 1; step <= config.max_steps; ++step) {
        int xi = dist->generate_index(rng);
        double angle = 2.0 * M_PI * static_cast<double>(xi) / static_cast<double>(config.n);
        std::complex<double> delta = config.rho * std::complex<double>(std::cos(angle), std::sin(angle));
        z += delta;
        result.points.push_back(QPointF(z.real(), z.imag()));
        if (std::abs(z) <= eps) {
            result.returned = true;
            result.steps = step;
            break;
        }
    }
    if (!result.returned)
        result.steps = config.max_steps;
    return result;
}