#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QTimer>
#include "GraphModel.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private slots:
    void onLoadGraph();
    void onStartSimulation();
    void onStopSimulation();
    void onStepSimulation();
    void onTimerTick();
    void onShowNeverInfected();
    void onShowRecovered();
    void onShowRecoveredWithBadNeighbors();
    void onShowNeverInfectedButAllNeighborsInfected();
private:
    void setupUI();
    void updateTable();
    void setPinkStyle();
    bool hasInfected();
    GraphModel m_model;
    QTimer* m_timer;
    bool m_running;
    QTableWidget* m_table;
    QPushButton* m_load_btn;
    QPushButton* m_start_btn;
    QPushButton* m_stop_btn;
    QPushButton* m_step_btn;
    QDoubleSpinBox* m_p1_spin;
    QDoubleSpinBox* m_p2_spin;
    QTextEdit* m_result_edit;
    QPushButton* m_btn_never_infected;
    QPushButton* m_btn_recovered;
    QPushButton* m_btn_recov_bad_neighbors;
    QPushButton* m_btn_never_inf_all_neighbors_inf;
};
#endif