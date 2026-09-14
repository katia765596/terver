#ifndef TRAJECTORYMODEL_H
#define TRAJECTORYMODEL_H
#include <vector>
#include <random>
#include <QPointF>
#include <QVariantMap>
#include <QString>
class Distribution;
struct SimulationParams {
    double h = 1.0;
    double Y0 = 0.0;
    int steps = 100;
    std::vector<double> sValues;
    QString distType = "uniform";
    QVariantMap distParams;
};
struct SimulationResult {
    std::vector<QPointF> points;
    int crossings = 0;
};
class TrajectoryModel {
public:
    explicit TrajectoryModel(const SimulationParams& params);
    SimulationResult simulate() const;
private:
    SimulationParams m_params;
    mutable std::mt19937 m_rng;
};
#endif