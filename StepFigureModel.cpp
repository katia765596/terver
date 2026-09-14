#include "StepFigureModel.h"
#include <cmath>

StepFigureModel::StepFigureModel() {
    std::random_device rd;
    m_rng.seed(rd());
}

void StepFigureModel::clear() {
    m_figures.clear();
    m_numIntervals = 0;
    m_strictCount = 0;
    m_probability = 0.0;
}

void StepFigureModel::generateAll(const FigureParams& params) {
    clear();
    int steps = static_cast<int>(std::round(params.M / params.h));
    if (steps < 1) steps = 1;
    m_numIntervals = steps;
    auto dist = Distribution::create(params.distType, params.n + 1, params.distParams);
    if (!dist) dist = std::make_unique<UniformDistribution>(params.n + 1);
    m_figures.resize(params.N);
    m_strictCount = 0;
    for (int i = 0; i < params.N; ++i) {
        std::vector<int> figure(steps);
        for (int j = 0; j < steps; ++j)
            figure[j] = dist->generate_index(m_rng);
        m_figures[i] = figure;
        bool inc = true;
        for (int j = 1; j < steps; ++j) {
            if (figure[j] <= figure[j - 1]) { inc = false; break; }
        }
        if (inc) ++m_strictCount;
    }
    m_probability = static_cast<double>(m_strictCount) / params.N;
}

int StepFigureModel::getFigureCount() const {
    return static_cast<int>(m_figures.size());
}

const std::vector<int>& StepFigureModel::getFigure(int index) const {
    return m_figures[index];
}

int StepFigureModel::getNumIntervals() const {
    return m_numIntervals;
}

int StepFigureModel::getStrictlyIncreasingCount() const {
    return m_strictCount;
}

double StepFigureModel::getProbability() const {
    return m_probability;
}