#ifndef RANDOMDISTRIBUTIONS_H
#define RANDOMDISTRIBUTIONS_H
#include <random>
#include <memory>
#include <QVariantMap>
#include <QString>
class Distribution {
public:
    virtual ~Distribution() = default;
    virtual int generate_index(std::mt19937& rng) const = 0;
    static std::unique_ptr<Distribution> create(const QString& type, int n_values, const QVariantMap& params);
};
class UniformDistribution : public Distribution {
public:
    explicit UniformDistribution(int n_values);
    int generate_index(std::mt19937& rng) const override;
private:
    int n_values;
};
class BinomialDistribution : public Distribution {
public:
    BinomialDistribution(int n_values, double p);
    int generate_index(std::mt19937& rng) const override;
private:
    mutable std::binomial_distribution<int> dist;
};
class GeometricDistribution : public Distribution {
public:
    GeometricDistribution(int n_values, double p);
    int generate_index(std::mt19937& rng) const override;
private:
    int n_values;
    mutable std::geometric_distribution<int> dist;
};
class TriangularDistribution : public Distribution {
public:
    explicit TriangularDistribution(int n_values);
    int generate_index(std::mt19937& rng) const override;
private:
    mutable std::discrete_distribution<int> dist;
};
#endif