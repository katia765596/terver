#ifndef TREE_H
#define TREE_H
#include <vector>
#include <QPointF>
struct TreeNode {
    int level;//уровень узла
    int index;//номер узла на урове
    QPointF pos;//коорд на плоскости
    std::vector<int> children;};
class IdealTree {
public:
    IdealTree(int M, int depth);//кол-во потомков и глубину
    void build();//метод построения дерева: заполняет уровни и вычисляет связи
    int getM() const { return M; }
    int getDepth() const { return depth; }
    const std::vector<std::vector<TreeNode>>& getLevels() const { return levels; }//возвр матрицу узлов по уровням (внешний вектор-уровни, внутр -узлы на уровне)
    TreeNode getNode(int level, int index) const;
private:
    int M;
    int depth;
    std::vector<std::vector<TreeNode>> levels;
};
#endif