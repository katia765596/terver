#include "tree.h"
#include <cmath>
IdealTree::IdealTree(int M, int depth) : M(M), depth(depth) {}
void IdealTree::build() {
    levels.clear();
    levels.resize(depth + 1);
    TreeNode root;//корень
    root.level = 0;
    root.index = 0;
    root.pos = QPointF(0, 0);
    levels[0].push_back(root);
    for (int l = 0; l < depth; ++l) {//цикл по уровням, строим детей для каждого узла уровня l
        int count = static_cast<int>(std::pow(M, l));//кол-во узлов на тек уровне
        int childCount = static_cast<int>(std::pow(M, l + 1));//на слеж
        levels[l + 1].resize(childCount);//размер вектора узлов на след уровне
        for (int i = 0; i < count; ++i) {
            for (int k = 0; k < M; ++k) {
                int childIndex = i * M + k;//для каждого узла i на уровне l создаем M детей
                levels[l + 1][childIndex].level = l + 1;
                levels[l + 1][childIndex].index = childIndex;
                levels[l][i].children.push_back(childIndex);
            }//добавляем индекс ребенка в список чилдрен род узла
        }
    }
}
TreeNode IdealTree::getNode(int level, int index) const {
    if (level >= 0 && level < static_cast<int>(levels.size()) &&
        index >= 0 && index < static_cast<int>(levels[level].size())) {
        return levels[level][index];
    }
    return TreeNode();
}