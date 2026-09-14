#include "simulator.h"
#include <random>
Simulator::Simulator(const IdealTree& tree,const std::vector<std::vector<double>>& transitionProbs,const std::vector<double>& stopProbs,int targetLeafIndex,int experiments)
    : tree(tree), transProbs(transitionProbs), stopProb(stopProbs),targetLeaf(targetLeafIndex), experiments(experiments),rng(std::random_device{}()) {}
int Simulator::simulateOne(std::vector<int>& outPath) {
    outPath.clear();
    int level = 0;
    int index = 0;
    outPath.push_back(index);
    int M = tree.getM();
    int depth = tree.getDepth();
    while (true) {
        if (level == depth) break;//уровень=глубине мы в листе
        double p = stopProb[level];//вер-ть остановки на тек уровне р
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        if (dist(rng) < p) break;
        const auto& probs = transProbs[level];//если не стоп выбираем след ребро, берутся вер-ти перехода на тек уровне
        double r = dist(rng);
        double cum = 0.0;//накопление вер
        int chosen = 0;//номер потомка от 0 до М-1
        for (int k = 0; k < M; ++k) {
            cum += probs[k];
            if (r <= cum) {
                chosen = k;
                break;
            }
        }
        int newIndex = index * M + chosen;
        outPath.push_back(newIndex);
        level++;
        index = newIndex;
    }
    return level + 1;//возвр число пройденных вершин
}
SimulationResult Simulator::run() {
    SimulationResult result;
    result.totalExperiments = experiments;
    int depth = tree.getDepth();
    result.probPassedVertices.assign(depth + 2, 0.0);//индекс 0 не юзать, накапливается кол-во эксп в которых  пройдено ровно l вершин
    int targetHits = 0;
    std::vector<int> lastPath;//посл путь
    for (int i = 0; i < experiments; ++i) {
        std::vector<int> path;
        int passed = simulateOne(path);
        if (passed >= 1 && passed <= depth + 1)
            result.probPassedVertices[passed] += 1.0;
        int lastLevel = path.size() - 1;//посл уровень
        int lastIndex = path.back();
        if (lastLevel == depth && lastIndex == targetLeaf) {
            targetHits++;
        }
        lastPath = path;
    }
    for (int l = 1; l <= depth + 1; ++l) {
        result.probPassedVertices[l] /= experiments;
    }
    result.probTargetLeaf = static_cast<double>(targetHits) / experiments;
    result.path = lastPath;
    return result;
}