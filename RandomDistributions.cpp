#include "RandomDistributions.h"
#include <vector>
#include <cmath>
std::unique_ptr<Distribution> Distribution::create(const QString& type, int n_values, const QVariantMap& params)
{
    if (type == "uniform")
        return std::make_unique<UniformDistribution>(n_values);
    if (type == "binomial") {
        double p = params.value("p", 0.5).toDouble();
        return std::make_unique<BinomialDistribution>(n_values, p);
    }
    if (type == "geometric") {
        double p = params.value("p", 0.5).toDouble();
        return std::make_unique<GeometricDistribution>(n_values, p);
    }
    if (type == "triangular")
        return std::make_unique<TriangularDistribution>(n_values);
    return std::make_unique<UniformDistribution>(n_values);
}
UniformDistribution::UniformDistribution(int n_values) : n_values(n_values) {}
int UniformDistribution::generate_index(std::mt19937& rng) const
{
    std::uniform_int_distribution<int> dist(0, n_values - 1);
    return dist(rng);
}
BinomialDistribution::BinomialDistribution(int n_values, double p) : dist(n_values - 1, p) {}
int BinomialDistribution::generate_index(std::mt19937& rng) const
{
    return dist(rng);
}
GeometricDistribution::GeometricDistribution(int n_values, double p) : n_values(n_values), dist(p) {}
int GeometricDistribution::generate_index(std::mt19937& rng) const
{
    int index;
    do {
        index = dist(rng);
    } while (index >= n_values);
    return index;
}
TriangularDistribution::TriangularDistribution(int n_values)
{
    std::vector<double> probs(n_values);
    int center = n_values / 2;
    for (int i = 0; i < n_values; ++i) {
        int distance = std::abs(i - center);
        probs[i] = static_cast<double>(n_values - distance);
    }
    dist = std::discrete_distribution<int>(probs.begin(), probs.end());
}
int TriangularDistribution::generate_index(std::mt19937& rng) const
{
    return dist(rng);
}