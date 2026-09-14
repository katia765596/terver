#include "GraphModel.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
GraphModel::GraphModel()
{
    std::random_device rd;
    m_rng.seed(rd());
}
bool GraphModel::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&file);
    int n;
    if (in.atEnd())
        return false;
    in >> n;
    if (n <= 0)
        return false;
    in.readLine();
    m_people.clear();
    m_adj.clear();
    m_people.reserve(n);
    m_adj.resize(n);
    for (int i = 0; i < n; ++i)
    {
        if (in.atEnd())
            return false;
        QString name = in.readLine().trimmed();
        if (name.isEmpty())
            return false;
        m_people.push_back({name, 0});
    }
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            continue;
        bool ok1 = false;
        bool ok2 = false;
        int u = parts[0].toInt(&ok1);
        int v = parts[1].toInt(&ok2);
        if (!ok1 || !ok2)
            return false;
        if (u < 0 || u >= n || v < 0 || v >= n)
            return false;
        if (u == v)
            continue;
        if (!m_adj[u].contains(v))
            m_adj[u].append(v);
        if (!m_adj[v].contains(u))
            m_adj[v].append(u);
    }
    return true;
}
void GraphModel::initializeRandomInfected()
{
    reset();
    if (m_people.empty())
        return;
    std::uniform_int_distribution<int> dist(0,static_cast<int>(m_people.size()) - 1);
    int idx = dist(m_rng);
    m_people[idx].state = 1;
}
void GraphModel::reset()
{
    for (auto& person : m_people)
        person.state = 0;
}
void GraphModel::step(double p1, double p2)
{
    if (m_people.empty())
        return;
    int n = static_cast<int>(m_people.size());
    std::vector<int> new_states(n);
    for (int i = 0; i < n; ++i)
        new_states[i] = m_people[i].state;

    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < n; ++i)
    {
        if (m_people[i].state == 1 && dist(m_rng) < p2)
            new_states[i] = 2;
    }
    for (int i = 0; i < n; ++i)
    {
        if (m_people[i].state != 0)
            continue;
        bool has_infected_neighbor = false;
        for (int neighbor : m_adj[i])
        {
            if (m_people[neighbor].state == 1)
            {
                has_infected_neighbor = true;
                break;
            }
        }
        if (has_infected_neighbor && dist(m_rng) < p1)
            new_states[i] = 1;
    }
    for (int i = 0; i < n; ++i)
        m_people[i].state = new_states[i];
}
int GraphModel::getPersonCount() const
{
    return static_cast<int>(m_people.size());
}
QString GraphModel::getPersonName(int idx) const
{
    if (idx < 0 || idx >= static_cast<int>(m_people.size()))
        return QString();
    return m_people[idx].name;
}
int GraphModel::getPersonState(int idx) const
{
    if (idx < 0 || idx >= static_cast<int>(m_people.size()))
        return -1;

    return m_people[idx].state;
}
const QVector<int>& GraphModel::getNeighbors(int idx) const
{
    static const QVector<int> empty;
    if (idx < 0 || idx >= m_adj.size())
        return empty;
    return m_adj[idx];
}
QVector<int> GraphModel::findNeverInfected() const
{
    QVector<int> result;
    for (int i = 0; i < static_cast<int>(m_people.size()); ++i)
    {
        if (m_people[i].state == 0)
            result.append(i);
    }
    return result;
}
QVector<int> GraphModel::findRecovered() const
{
    QVector<int> result;
    for (int i = 0; i < static_cast<int>(m_people.size()); ++i)
    {
        if (m_people[i].state == 2)
            result.append(i);
    }
    return result;
}
QVector<int> GraphModel::findRecoveredWithNotRecoveredNeighbors() const
{
    QVector<int> result;
    for (int i = 0; i < static_cast<int>(m_people.size()); ++i)
    {
        if (m_people[i].state != 2)
            continue;

        for (int neighbor : m_adj[i])
        {
            if (m_people[neighbor].state != 2)
            {
                result.append(i);
                break;
            }
        }
    }
    return result;
}
QVector<int> GraphModel::findNeverInfectedButAllNeighborsInfected() const
{
    QVector<int> result;
      for (int i = 0; i < static_cast<int>(m_people.size()); ++i)
    {
        if (m_people[i].state != 0)
            continue;
        if (m_adj[i].isEmpty())
            continue;
        bool all_neighbors_infected = true;
        for (int neighbor : m_adj[i])
        {
            if (m_people[neighbor].state != 1)
            {
                all_neighbors_infected = false;
                break;
            }
        }
        if (all_neighbors_infected)
            result.append(i);
    }
    return result;
}