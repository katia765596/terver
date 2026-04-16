#include "randomwalksimulator.h"
#include <QTimer>
#include <QElapsedTimer>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <QDebug>
RandomWalkSimulator::RandomWalkSimulator(QObject *parent)
    : QObject(parent), m_startPos(0.0), m_currentPos(0.0),
    m_totalSteps(0), m_currentStep(0), m_running(false), m_animTimer(nullptr)
{
    std::srand(std::time(nullptr));
}
RandomWalkSimulator::~RandomWalkSimulator()
{
    if (m_animTimer) {
        m_animTimer->stop();
        m_animTimer->deleteLater();
    }
}
void RandomWalkSimulator::setStartPosition(double pos)
{
    m_startPos = pos;
    if (!m_running) {
        m_currentPos = m_startPos;
        emit positionChanged(m_currentPos);
    }
}
void RandomWalkSimulator::setStepsCount(int steps)
{
    m_totalSteps = steps;
}
void RandomWalkSimulator::setStepDistribution(const DiscreteRandomVariable &dist)
{
    m_stepDist = dist;
}
bool RandomWalkSimulator::loadStepDistributionFromFile(const QString &filename)
{
    DiscreteRandomVariable dist;
    if (!dist.loadFromFile(filename)) {
        emit errorOccurred("Не удалось загрузить закон перемещения из файла");
        return false;
    }
    if (!dist.isValid()) {
        emit errorOccurred("Закон перемещения некорректный");
        return false;
    }
    m_stepDist = dist;
    return true;
}
void RandomWalkSimulator::start()
{
    if (m_running) {
        emit errorOccurred("Моделирование уже запущено");
        return;
    }
    if (!m_stepDist.isValid()) {
        emit errorOccurred("Закон перемещения не задан или некорректен");
        return;
    }
    if (m_totalSteps <= 0) {
        emit errorOccurred("Количество шагов должно быть положительным");
        return;
    }
    m_currentPos = m_startPos;
    m_currentStep = 0;
    m_running = true;
    emit positionChanged(m_currentPos);
    doStep();
}
void RandomWalkSimulator::stop()
{
    if (m_running) {
        m_running = false;
        if (m_animTimer) {
            m_animTimer->stop();
            m_animTimer->deleteLater();
            m_animTimer = nullptr;
        }
        emit simulationFinished();
    }
}
void RandomWalkSimulator::doStep()
{
    if (!m_running) return;

    if (m_currentStep >= m_totalSteps) {
        qDebug() << "Вычисление финального распределения ";
        DiscreteRandomVariable sum = m_stepDist;
        if (!sum.isValid()) {
            qDebug() << "Исходное распределение невалидно!";
            emit errorOccurred("Закон распределения шага недействителен");
            m_running = false;
            emit simulationFinished();
            return;
        }
        for (int i = 1; i < m_totalSteps; ++i) {
            sum = sum + m_stepDist;
            if (!sum.isValid()) {
                auto dist = sum.distribution();
                double total = 0.0;
                for (const auto &p : dist) total += p.second;
                if (total > 0) {
                    QVector<QPair<double,double>> normDist;
                    for (auto &p : dist) normDist.append({p.first, p.second / total});
                    sum.setDistributionNoCheck(normDist);
                }
                if (!sum.isValid()) {
                    qDebug() << "Свёртка дала невалидный результат на шаге" << i;
                    emit errorOccurred("Ошибка в свёртке распределений");
                    m_running = false;
                    emit simulationFinished();
                    return;
                }
            }
        }
        auto dist = sum.distribution();
        qDebug() << "После свёртки получено значений:" << dist.size();

        if (dist.isEmpty()) {
            qDebug() << "Свёртка дала пустой результат";
            emit errorOccurred("Ошибка при вычислении распределения суммы.");
            m_running = false;
            emit simulationFinished();
            return;
        }
        QVector<QPair<double, double>> shifted;
        for (const auto &p : dist) {
            shifted.append({p.first + m_startPos, p.second});
        }
        std::sort(shifted.begin(), shifted.end(),
                  [](const QPair<double,double> &a, const QPair<double,double> &b) {
                      return a.first < b.first;
                  });
        QVector<QPair<double, double>> merged;
        double totalProb = 0.0;
        const double eps = 1e-12;
        for (const auto &p : shifted) {
            totalProb += p.second;
            if (!merged.isEmpty() && std::abs(merged.last().first - p.first) < eps) {
                merged.last().second += p.second;
            } else {
                merged.append(p);
            }
        }
        qDebug() << "Общая вероятность до нормализации:" << totalProb;
        if (totalProb > 0.0) {
            for (auto &p : merged) {
                p.second /= totalProb;
            }
        } else {
            qDebug() << "Суммарная вероятность равна 0!";
            emit errorOccurred("Суммарная вероятность равна нулю.");
            m_running = false;
            emit simulationFinished();
            return;
        }
        m_finalDist.setDistributionNoCheck(merged);
        qDebug() << "Финальное распределение есть. Записей:" << merged.size();
        m_running = false;
        emit simulationFinished();
        return;
    }
    auto dist = m_stepDist.distribution();
    double r = (double)rand() / RAND_MAX;
    double cum = 0.0;
    double stepValue = 0.0;
    for (const auto &pair : dist) {
        cum += pair.second;
        if (r <= cum) {
            stepValue = pair.first;
            break;
        }
    }
    double startPosStep = m_currentPos;
    double endPosStep = m_currentPos + stepValue;
    QElapsedTimer timer;
    timer.start();
    const int durationMs = 1000;
    if (m_animTimer) {
        m_animTimer->stop();
        m_animTimer->deleteLater();
        m_animTimer = nullptr;
    }
    m_animTimer = new QTimer(this);
    m_animTimer->setInterval(20);
    connect(m_animTimer, &QTimer::timeout, this,
            [this, startPosStep, endPosStep, durationMs, timer]() mutable {
                if (!m_running) {
                    m_animTimer->stop();
                    m_animTimer->deleteLater();
                    m_animTimer = nullptr;
                    return;
                }
                qint64 elapsed = timer.elapsed();
                if (elapsed >= durationMs) {
                    m_animTimer->stop();
                    m_animTimer->deleteLater();
                    m_animTimer = nullptr;
                    m_currentPos = endPosStep;
                    emit positionChanged(m_currentPos);
                    m_currentStep++;
                    QMetaObject::invokeMethod(this, "doStep", Qt::QueuedConnection);
                    return;
                }
                double t = static_cast<double>(elapsed) / durationMs;
                double pos = startPosStep + t * (endPosStep - startPosStep);
                emit positionChanged(pos);
            });
    m_animTimer->start();
}
