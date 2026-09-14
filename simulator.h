#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "tree.h"
#include <vector>
#include <random>
struct SimulationResult {
    int totalExperiments;
    double probTargetLeaf;
    std::vector<double> probPassedVertices;//вектор вер где эл с индекс l содержит вер пройти ровно l вершин (0-й индекс не испол)
    std::vector<int> path;//последний пройденный путь
};
class Simulator {
public:
    Simulator(
        const IdealTree& tree,
        const std::vector<std::vector<double>>& transitionProbs,//матрица вер-ей переходов (размер dethp*M)
        const std::vector<double>& stopProbs,//вектор вер-ей остановки на каждом уровне
        int targetLeafIndex,
        int experiments);
    SimulationResult run();
private:
    const IdealTree& tree;
    std::vector<std::vector<double>> transProbs;
    std::vector<double> stopProb;
    int targetLeaf;
    int experiments;
    std::mt19937 rng;
    int simulateOne(std::vector<int>& outPath);//заполняет outpath послед-ью индексов узлов, возвр число проейденных вершин
};
#endif