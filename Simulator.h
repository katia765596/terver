#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <QObject>
#include <random>
#include <atomic>
#include <QString>
enum class ModelType {
    NoReturnToFirst,
    NoRepeatPerson,
    GroupNoReturnToFirst,
    GroupNoRepeatPerson
};
class Simulator : public QObject
{
    Q_OBJECT
public:
    explicit Simulator(QObject* parent = nullptr);
    void stop();
signals:
    void experimentResult(const QString& message);
    void progress(int current, int total);
    void finalResult(double probability, int successCount, int total);
public slots:
    void runExperiments(int n, int r, int N, int K, ModelType type);

private:
    QString runSingleExperiment(int n, int r, int N, ModelType type);
    std::mt19937 m_rng;
    std::atomic<bool> m_stopRequested{false};
};
#endif