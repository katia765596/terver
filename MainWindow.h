#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "StepFigureModel.h"
class PlotWidget;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onGenerate();
    void onPrevFigure();
    void onNextFigure();
    void onLoadConfig();
    void onSaveConfig();
    void setPinkStyle();
private:
    void setupUI();
    void readConfigFromFile(const QString& filename);
    void writeConfigToFile(const QString& filename);
    FigureParams getParamsFromUI() const;
    void setParamsToUI(const FigureParams& p);
    void updateCurrentFigure();
    PlotWidget* m_plot;
    QLineEdit* m_MEdit;
    QLineEdit* m_hEdit;
    QLineEdit* m_tauEdit;
    QLineEdit* m_nEdit;
    QLineEdit* m_NEdit;
    QLineEdit* m_pEdit;
    QComboBox* m_distCombo;
    QPushButton* m_generateBtn;
    QPushButton* m_loadBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_prevBtn;
    QPushButton* m_nextBtn;
    QLabel* m_pageLabel;
    QTextEdit* m_resultEdit;
    StepFigureModel m_model;
    int m_currentIndex;
};
class PlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlotWidget(QWidget* parent = nullptr);
    void setData(const std::vector<int>& indices, double tau, int numIntervals, double M);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    std::vector<int> m_indices;
    double m_tau;
    int m_numIntervals;
    double m_M;
};
#endif