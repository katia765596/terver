#include "TrajectoryModel.h"
#include "Distribution.h"
#include <random>
TrajectoryModel::TrajectoryModel(const SimulationParams& params) : m_params(params) {
    std::random_device rd;
    m_rng.seed(rd());
}
SimulationResult TrajectoryModel::simulate() const {
    SimulationResult res;
    const int steps = m_params.steps;
    const double h = m_params.h;
    double x = 0.0;
    double y = m_params.Y0;
    res.points.emplace_back(x, y);
    auto dist = Distribution::create(m_params.distType, static_cast<int>(m_params.sValues.size()), m_params.distParams);
    if (!dist) dist = std::make_unique<UniformDistribution>(m_params.sValues.size());
    double prevY = y;
    for (int i = 1; i <= steps; ++i) {
        int idx = dist->generateIndex(m_rng);
        double s = m_params.sValues[idx];
        y += s;
        x = i * h;
        res.points.emplace_back(x, y);
        if (prevY * y < 0.0) ++res.crossings;
        prevY = y;
    }
    return res;
}