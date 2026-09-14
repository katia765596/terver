#include "Simulator.h"
#include <QThread>//задержка для обновления интерфейса
#include <unordered_set>//для хранения множества уже посещенных людей
#include <algorithm>
Simulator::Simulator(QObject* parent) : QObject(parent)
{
    std::random_device rd;
    m_rng.seed(rd());}
void Simulator::stop()
{
    m_stopRequested = true;}
QString Simulator::runSingleExperiment(int n, int r, int N, ModelType type)
{
    if (r == 0) return "Успех (r=0)";
    if (type == ModelType::NoReturnToFirst)
    {
        int current = 0;
        for (int step = 1; step <= r; ++step)
        {
            std::vector<int> candidates;
            for (int i = 0; i <= n; ++i)
                if (i != current)
                    candidates.push_back(i);
            if (candidates.empty())
                return QString("Неудача: нет кандидатов на шаге %1").arg(step);
            std::uniform_int_distribution<int> dist(0, candidates.size() - 1);
            int chosen = candidates[dist(m_rng)];
            if (chosen == 0)
                return QString("Неудача: возврат к первому на шаге %1").arg(step);
            current = chosen;
        }
        return "Успех";
    }
    else if (type == ModelType::NoRepeatPerson)
    {
        std::unordered_set<int> known;
        known.insert(0);
        int current = 0;
        for (int step = 1; step <= r; ++step)
        {
            std::vector<int> candidates;
            for (int i = 0; i <= n; ++i)
                if (i != current && known.find(i) == known.end())
                    candidates.push_back(i);
            if (candidates.empty())
                return QString("Неудача: нет новых кандидатов на шаге %1").arg(step);
            std::uniform_int_distribution<int> dist(0, candidates.size() - 1);
            int chosen = candidates[dist(m_rng)];
            known.insert(chosen);
            current = chosen;
        }
        return "Успех";
    }
    else if (type == ModelType::GroupNoReturnToFirst)
    {
        int current = 0;
        for (int step = 1; step <= r; ++step)
        {
            std::vector<int> candidates;
            for (int i = 0; i <= n; ++i)
                if (i != current)
                    candidates.push_back(i);
            if ((int)candidates.size() < N)
                return QString("Неудача: недостаточно кандидатов для группы на шаге %1").arg(step);
            std::shuffle(candidates.begin(), candidates.end(), m_rng);
            for (int i = 0; i < N; ++i)
                if (candidates[i] == 0)
                    return QString("Неудача: возврат к первому в группе на шаге %1").arg(step);
            std::uniform_int_distribution<int> dist(0, N - 1);
            current = candidates[dist(m_rng)];
        }
        return "Успех";
    }
    else if (type == ModelType::GroupNoRepeatPerson)
    {
        std::unordered_set<int> known;
        known.insert(0);
        int current = 0;
        for (int step = 1; step <= r; ++step)
        {
            std::vector<int> candidates;
            for (int i = 0; i <= n; ++i)
                if (i != current && known.find(i) == known.end())
                    candidates.push_back(i);
            if ((int)candidates.size() < N)
                return QString("Неудача: недостаточно новых кандидатов для группы на шаге %1").arg(step);
            std::shuffle(candidates.begin(), candidates.end(), m_rng);
            for (int i = 0; i < N; ++i)
                known.insert(candidates[i]);
            std::uniform_int_distribution<int> dist(0, N - 1);
            current = candidates[dist(m_rng)];
        }
        return "Успех";
    }
    return "Неудача: неизвестная модель";
}
void Simulator::runExperiments(int n, int r, int N, int K, ModelType type)
{
    m_stopRequested = false;
    int successCount = 0;
    const int BATCH_SIZE = 50;
    QStringList batch;
    for (int i = 0; i < K; ++i)
    {
        if (m_stopRequested) {
            if (!batch.isEmpty()) {
                emit experimentResult(batch.join("\n"));
                batch.clear();
            }
            emit experimentResult("Моделирование прервано пользователем");
            break;
        }
        QString result = runSingleExperiment(n, r, N, type);
        batch.append(QString("Эксперимент %1: %2").arg(i+1).arg(result));
        if (result == "Успех" || result.startsWith("Успех"))
            ++successCount;
        if ((i+1) % BATCH_SIZE == 0 || i == K-1) {
            emit experimentResult(batch.join("\n"));
            batch.clear();
            QThread::msleep(10);
        }
        if ((i+1) % 100 == 0 || i == K-1) {
            emit progress(i+1, K);
        }
    }
    double prob = static_cast<double>(successCount) / K;
    emit finalResult(prob, successCount, K);
}