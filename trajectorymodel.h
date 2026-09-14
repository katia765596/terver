#ifndef TRAJECTORYMODEL_H
#define TRAJECTORYMODEL_H
#include <vector>
#include <complex>
#include <random>
#include <QPointF>
#include "randomdistributions.h"
struct SimulationConfig {
    double rho = 1.0;
    int n = 4;
    int max_steps = 1000;
    QString dist_type = "uniform";
    QVariantMap dist_params;
};
struct SimulationResult {
    std::vector<QPointF> points;
    bool returned = false;
    int steps = 0;
};
class TrajectoryModel {
public:
    explicit TrajectoryModel(const SimulationConfig& config);
    SimulationResult simulate_one() const;
private:
    SimulationConfig config;
    mutable std::mt19937 rng;
};
#endif