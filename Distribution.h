#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H
#include <random>
#include <memory>
#include <QVariantMap>
#include <QString>
class Distribution {
public:
    virtual ~Distribution() = default;
    virtual int generateIndex(std::mt19937& rng) const = 0;
    static std::unique_ptr<Distribution> create(const QString& type, int nValues, const QVariantMap& params);
};
class UniformDistribution : public Distribution {
public:
    explicit UniformDistribution(int nValues);
    int generateIndex(std::mt19937& rng) const override;
private:
    int m_nValues;
};
class BinomialDistribution : public Distribution {
public:
    BinomialDistribution(int nValues, double p);
    int generateIndex(std::mt19937& rng) const override;
private:
    int m_nValues;
    double m_p;
    mutable std::binomial_distribution<int> m_dist;
};
class GeometricDistribution : public Distribution {
public:
    GeometricDistribution(int nValues, double p);
    int generateIndex(std::mt19937& rng) const override;
private:
    int m_nValues;
    double m_p;
    mutable std::geometric_distribution<int> m_dist;
};
class TriangularDistribution : public Distribution {
public:
    explicit TriangularDistribution(int nValues);
    int generateIndex(std::mt19937& rng) const override;
private:
    int m_nValues;
    mutable std::discrete_distribution<int> m_dist;
};
#endif