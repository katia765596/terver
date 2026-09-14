#include "Distribution.h"
#include <cmath>
#include <vector>
std::unique_ptr<Distribution> Distribution::create(const QString& type, int nValues, const QVariantMap& params) {
    if (type == "uniform") return std::make_unique<UniformDistribution>(nValues);
    else if (type == "binomial") { double p = params.value("p", 0.5).toDouble(); return std::make_unique<BinomialDistribution>(nValues, p); }
    else if (type == "geometric") { double p = params.value("p", 0.5).toDouble(); return std::make_unique<GeometricDistribution>(nValues, p); }
    else if (type == "triangular") return std::make_unique<TriangularDistribution>(nValues);
    return std::make_unique<UniformDistribution>(nValues);
}
UniformDistribution::UniformDistribution(int nValues) : m_nValues(nValues) {}
int UniformDistribution::generateIndex(std::mt19937& rng) const {
    std::uniform_int_distribution<int> dist(0, m_nValues - 1);
    return dist(rng);
}
BinomialDistribution::BinomialDistribution(int nValues, double p) : m_nValues(nValues), m_p(p), m_dist(nValues - 1, p) {}
int BinomialDistribution::generateIndex(std::mt19937& rng) const { return m_dist(rng); }
GeometricDistribution::GeometricDistribution(int nValues, double p) : m_nValues(nValues), m_p(p), m_dist(p) {}
int GeometricDistribution::generateIndex(std::mt19937& rng) const {
    if (m_nValues == 0) return 0;
    int idx; do { idx = m_dist(rng); } while (idx >= m_nValues);
    return idx;
}
TriangularDistribution::TriangularDistribution(int nValues) : m_nValues(nValues) {
    std::vector<double> probs(nValues);
    int mid = (nValues - 1) / 2;
    for (int i = 0; i < nValues; ++i) { int d = std::abs(i - mid); probs[i] = static_cast<double>(nValues - d); }
    m_dist = std::discrete_distribution<int>(probs.begin(), probs.end());
}
int TriangularDistribution::generateIndex(std::mt19937& rng) const { return m_dist(rng); }