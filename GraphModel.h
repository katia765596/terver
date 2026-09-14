#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H
#include <vector>
#include <random>
#include <QString>
#include <QVector>
struct Person
{
    QString name;
    int state;
};
class GraphModel
{
public:
    GraphModel();
    bool loadFromFile(const QString& filename);
    void initializeRandomInfected();
    void step(double p1, double p2);
    void reset();
    int getPersonCount() const;
    QString getPersonName(int idx) const;
    int getPersonState(int idx) const;
    const QVector<int>& getNeighbors(int idx) const;
    QVector<int> findNeverInfected() const;
    QVector<int> findRecovered() const;
    QVector<int> findRecoveredWithNotRecoveredNeighbors() const;
    QVector<int> findNeverInfectedButAllNeighborsInfected() const;
private:
    std::vector<Person> m_people;
    QVector<QVector<int>> m_adj;
    std::mt19937 m_rng;
};
#endif