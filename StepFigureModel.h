#ifndef STEPFIGUREMODEL_H
#define STEPFIGUREMODEL_H
#include <vector>
#include <random>
#include <QString>
#include <QVariantMap>
#include "RandomDistributions.h"

struct FigureParams {
    double M;
    double h;
    double tau;
    int n;
    int N;
    QString distType;
    QVariantMap distParams;
};

class StepFigureModel {
public:
    StepFigureModel();
    void generateAll(const FigureParams& params);
    int getFigureCount() const;
    const std::vector<int>& getFigure(int index) const;
    int getNumIntervals() const;
    int getStrictlyIncreasingCount() const;
    double getProbability() const;
    void clear();
private:
    std::vector<std::vector<int>> m_figures;
    int m_numIntervals;
    int m_strictCount;
    double m_probability;
    std::mt19937 m_rng;
};
#endif