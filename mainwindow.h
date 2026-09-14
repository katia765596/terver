#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include "trajectorymodel.h"
class PlotWidget;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_load_config();
    void on_save_config();
    void on_run_single();
    void on_run_multiple();
    void set_pink_style();
private:
    void setup_ui();
    void read_config_from_file(const QString& filename);
    void write_config_to_file(const QString& filename);
    SimulationConfig get_config_from_ui() const;
    void set_config_to_ui(const SimulationConfig& cfg);
    void update_plot(const std::vector<QPointF>& points);
    PlotWidget* plot;
    QLineEdit* rho_edit;
    QLineEdit* n_edit;
    QLineEdit* max_steps_edit;
    QLineEdit* m_edit;
    QComboBox* dist_combo;
    QLineEdit* p_edit;
    QPushButton* load_btn;
    QPushButton* save_btn;
    QPushButton* run_single_btn;
    QPushButton* run_multiple_btn;
    QTextEdit* result_edit;
};
class PlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlotWidget(QWidget* parent = nullptr);
    void set_points(const std::vector<QPointF>& points);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    std::vector<QPointF> points;
};
#endif