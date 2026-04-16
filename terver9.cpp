#include "discreterandomvariable.h"
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <algorithm>
#include <QSet>
DiscreteRandomVariable::DiscreteRandomVariable() : m_isValid(false) {}
bool DiscreteRandomVariable::checkDistinctValues() const {
    QSet<double> values;
    for (const auto &pair : m_distribution) {
        if (values.contains(pair.first)) return false;
        values.insert(pair.first);
    }
    return true;
}
bool DiscreteRandomVariable::setDistribution(const QVector<QPair<double, double>> &pairs) {
    m_distribution = pairs;
    if (!checkDistinctValues()) {
        m_error = "Значения случайной величины должны быть попарно различны";
        m_isValid = false;
        return false;
    }
    m_isValid = validateAndNormalize();
    if (!m_isValid) {
        m_error = "Сумма вероятностей не равна 1 или есть отрицательные вероятности";
        return false;
    }
    sortAndMerge();
    m_isValid = validateAndNormalize();
    return m_isValid;
}
void DiscreteRandomVariable::setDistributionNoCheck(const QVector<QPair<double, double>> &pairs) {
    m_distribution = pairs;
    sortAndMerge();
    m_isValid = true;
    m_error.clear();
}
void DiscreteRandomVariable::sortAndMerge() {
    if (m_distribution.isEmpty()) return;
    std::sort(m_distribution.begin(), m_distribution.end(),
              [](const QPair<double,double> &a, const QPair<double,double> &b) {
                  return a.first < b.first;
              });
    QVector<QPair<double,double>> merged;
    for (const auto &pair : m_distribution) {
        if (!merged.isEmpty() && qFuzzyCompare(merged.last().first, pair.first))
            merged.last().second += pair.second;
        else
            merged.append(pair);
    }
    m_distribution = merged;
}
bool DiscreteRandomVariable::validateAndNormalize() {
    double sum = 0.0;
    for (const auto &pair : m_distribution) {
        if (pair.second < 0) return false;
        sum += pair.second;
    }
    if (qFabs(sum - 1.0) > 1e-9) return false;
    return true;
}
DiscreteRandomVariable DiscreteRandomVariable::operator*(double scalar) const {
    DiscreteRandomVariable result;
    if (!m_isValid) return result;
    QVector<QPair<double,double>> newDist;
    for (const auto &pair : m_distribution)
        newDist.append({pair.first * scalar, pair.second});
    result.setDistribution(newDist);
    return result;
}
DiscreteRandomVariable DiscreteRandomVariable::operator+(const DiscreteRandomVariable &other) const {
    DiscreteRandomVariable result;
    if (!m_isValid || !other.m_isValid) return result;
    QVector<QPair<double,double>> newDist;
    for (const auto &a : m_distribution)
        for (const auto &b : other.m_distribution)
            newDist.append({a.first + b.first, a.second * b.second});
    result.setDistribution(newDist);
    return result;
}
DiscreteRandomVariable DiscreteRandomVariable::operator*(const DiscreteRandomVariable &other) const {
    DiscreteRandomVariable result;
    if (!m_isValid || !other.m_isValid) return result;
    QVector<QPair<double,double>> newDist;
    for (const auto &a : m_distribution)
        for (const auto &b : other.m_distribution)
            newDist.append({a.first * b.first, a.second * b.second});
    result.setDistribution(newDist);
    return result;
}
double DiscreteRandomVariable::expectation() const {
    if (!m_isValid) return 0.0;
    double ex = 0.0;
    for (const auto &pair : m_distribution)
        ex += pair.first * pair.second;
    return ex;
}
double DiscreteRandomVariable::variance() const {
    if (!m_isValid) return 0.0;
    double ex = expectation();
    double var = 0.0;
    for (const auto &pair : m_distribution) {
        double diff = pair.first - ex;
        var += diff * diff * pair.second;
    }
    return var;
}
double DiscreteRandomVariable::asymmetry() const {
    if (!m_isValid) return 0.0;
    double ex = expectation();
    double sigma = qSqrt(variance());
    if (qFabs(sigma) < 1e-12) return 0.0;
    double mu3 = 0.0;
    for (const auto &pair : m_distribution) {
        double diff = pair.first - ex;
        mu3 += diff * diff * diff * pair.second;
    }
    return mu3 / (sigma * sigma * sigma);
}
double DiscreteRandomVariable::kurtosis() const {
    if (!m_isValid) return 0.0;
    double ex = expectation();
    double sigma = qSqrt(variance());
    if (qFabs(sigma) < 1e-12) return 0.0;
    double mu4 = 0.0;
    for (const auto &pair : m_distribution) {
        double diff = pair.first - ex;
        mu4 += diff * diff * diff * diff * pair.second;
    }
    double kurt = mu4 / (sigma * sigma * sigma * sigma);
    return kurt - 3.0;
}
bool DiscreteRandomVariable::saveToFile(const QString &filename) const {
    if (!m_isValid) return false;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    for (const auto &pair : m_distribution)
        out << pair.first << " " << pair.second << "\n";
    file.close();
    return true;
}
bool DiscreteRandomVariable::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QVector<QPair<double,double>> pairs;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        bool ok1, ok2;
        double val = parts[0].toDouble(&ok1);
        double prob = parts[1].toDouble(&ok2);
        if (ok1 && ok2 && prob >= 0)
            pairs.append({val, prob});
        else
            return false;
    }
    file.close();
    return setDistribution(pairs);
}
