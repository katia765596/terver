#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QPen>
#include <QPalette>
#include <QApplication>
#include <QIntValidator>
#include <QDoubleValidator>
#include <climits>
#include <cmath>
PlotWidget::PlotWidget(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(400);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(255, 220, 240));
    setPalette(pal);
}
void PlotWidget::set_points(const std::vector<QPointF>& new_points)
{
    points = new_points;
    update();
}
void PlotWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(255, 255, 255));
    if (points.empty())
        return;
    double min_x = points[0].x();
    double max_x = points[0].x();
    double min_y = points[0].y();
    double max_y = points[0].y();
    for (const auto& point : points) {
        if (point.x() < min_x)
            min_x = point.x();
        if (point.x() > max_x)
            max_x = point.x();
        if (point.y() < min_y)
            min_y = point.y();
        if (point.y() > max_y)
            max_y = point.y();
    }
    double margin_x = (max_x - min_x) * 0.05;
    double margin_y = (max_y - min_y) * 0.05;
    if (margin_x == 0)
        margin_x = 1;
    if (margin_y == 0)
        margin_y = 1;
    min_x -= margin_x;
    max_x += margin_x;
    min_y -= margin_y;
    max_y += margin_y;
    auto to_widget = [&](QPointF point) -> QPoint {
        double x = (point.x() - min_x) / (max_x - min_x) * width();
        double y = height() - (point.y() - min_y) / (max_y - min_y) * height();
        return QPoint(static_cast<int>(x), static_cast<int>(y));
    };
    painter.setPen(QPen(Qt::black, 1));
    QPoint origin = to_widget(QPointF(0, 0));
    painter.drawLine(0, origin.y(), width(), origin.y());
    painter.drawLine(origin.x(), 0, origin.x(), height());
    QPen pen(QColor(255, 20, 147), 2);
    painter.setPen(pen);
    QPoint previous = to_widget(points[0]);
    for (size_t i = 1; i < points.size(); ++i) {
        QPoint current = to_widget(points[i]);
        painter.drawLine(previous, current);
        previous = current;
    }
    painter.setBrush(QColor(255, 105, 180));
    for (const auto& point : points)
        painter.drawEllipse(to_widget(point), 4, 4);
}
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setup_ui();
    set_pink_style();
}
MainWindow::~MainWindow() {}
void MainWindow::set_pink_style()
{
    QPalette pink_palette;
    pink_palette.setColor(QPalette::Window, QColor(255, 220, 240));
    pink_palette.setColor(QPalette::WindowText, QColor(128, 0, 64));
    pink_palette.setColor(QPalette::Button, QColor(255, 182, 193));
    pink_palette.setColor(QPalette::ButtonText, QColor(128, 0, 64));
    pink_palette.setColor(QPalette::Base, QColor(255, 240, 245));
    pink_palette.setColor(QPalette::Text, QColor(128, 0, 64));
    pink_palette.setColor(QPalette::Highlight, QColor(255, 105, 180));
    pink_palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    qApp->setPalette(pink_palette);
    setStyleSheet("QMainWindow { background-color: #FFE4E1; }");
    QString button_style = "QPushButton { background-color: #FF69B4; color: white; border-radius: 5px; padding: 6px; font-weight: bold; } QPushButton:hover { background-color: #FF1493; } QPushButton:pressed { background-color: #DB7093; }";
    load_btn->setStyleSheet(button_style);
    save_btn->setStyleSheet(button_style);
    run_single_btn->setStyleSheet(button_style);
    run_multiple_btn->setStyleSheet(button_style);
    QString group_style = "QGroupBox { border: 2px solid #FF69B4; border-radius: 5px; margin-top: 10px; font-weight: bold; color: #8B008B; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }";
    for (auto* group : findChildren<QGroupBox*>())
        group->setStyleSheet(group_style);
    rho_edit->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    n_edit->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    max_steps_edit->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    m_edit->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    p_edit->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    dist_combo->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    result_edit->setStyleSheet("background-color: #FFF0F5; border: 1px solid #FF69B4; color: #8B008B;");
}
void MainWindow::setup_ui()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* main_layout = new QVBoxLayout(central);
    QGroupBox* params_group = new QGroupBox("Параметры моделирования");
    QFormLayout* form = new QFormLayout(params_group);
    rho_edit = new QLineEdit;
    rho_edit->setText("1.0");
    rho_edit->setValidator(new QDoubleValidator(0.0, 1e100, 12, this));
    n_edit = new QLineEdit;
    n_edit->setText("4");
    n_edit->setValidator(new QIntValidator(4, INT_MAX, this));
    max_steps_edit = new QLineEdit;
    max_steps_edit->setText("1000");
    max_steps_edit->setValidator(new QIntValidator(1, INT_MAX, this));
    m_edit = new QLineEdit;
    m_edit->setText("1000");
    m_edit->setValidator(new QIntValidator(1, INT_MAX, this));
    dist_combo = new QComboBox;
    dist_combo->addItems({"uniform", "binomial", "geometric", "triangular"});
    p_edit = new QLineEdit;
    p_edit->setText("0.5");
    p_edit->setValidator(new QDoubleValidator(0.0, 1.0, 12, this));
    form->addRow("ρ (длина шага):", rho_edit);
    form->addRow("n (множество z_n):", n_edit);
    form->addRow("Макс. шагов k:", max_steps_edit);
    form->addRow("Число запусков m:", m_edit);
    form->addRow("Распределение ξ:", dist_combo);
    form->addRow("Параметр p (bin/geom):", p_edit);
    main_layout->addWidget(params_group);
    QHBoxLayout* button_layout = new QHBoxLayout;
    load_btn = new QPushButton("Загрузить json");
    save_btn = new QPushButton("Сохранить json");
    run_single_btn = new QPushButton("Построить одну траекторию");
    run_multiple_btn = new QPushButton("Оценить вероятность возвращения");
    button_layout->addWidget(load_btn);
    button_layout->addWidget(save_btn);
    button_layout->addWidget(run_single_btn);
    button_layout->addWidget(run_multiple_btn);
    main_layout->addLayout(button_layout);
    plot = new PlotWidget;
    main_layout->addWidget(plot);
    result_edit = new QTextEdit;
    result_edit->setReadOnly(true);
    result_edit->setMaximumHeight(150);
    main_layout->addWidget(result_edit);
    connect(load_btn, &QPushButton::clicked, this, &MainWindow::on_load_config);
    connect(save_btn, &QPushButton::clicked, this, &MainWindow::on_save_config);
    connect(run_single_btn, &QPushButton::clicked, this, &MainWindow::on_run_single);
    connect(run_multiple_btn, &QPushButton::clicked, this, &MainWindow::on_run_multiple);
}
SimulationConfig MainWindow::get_config_from_ui() const
{
    SimulationConfig config;
    config.rho = rho_edit->text().toDouble();
    config.n = n_edit->text().toInt();
    config.max_steps = max_steps_edit->text().toInt();
    config.dist_type = dist_combo->currentText();
    config.dist_params.clear();
    if (config.dist_type == "binomial" || config.dist_type == "geometric")
        config.dist_params["p"] = p_edit->text().toDouble();
    return config;
}
void MainWindow::set_config_to_ui(const SimulationConfig& config)
{
    rho_edit->setText(QString::number(config.rho));
    n_edit->setText(QString::number(config.n));
    max_steps_edit->setText(QString::number(config.max_steps));
    dist_combo->setCurrentText(config.dist_type);
    if (config.dist_params.contains("p"))
        p_edit->setText(QString::number(config.dist_params["p"].toDouble()));
}
void MainWindow::update_plot(const std::vector<QPointF>& points)
{
    plot->set_points(points);
}
void MainWindow::read_config_from_file(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parse_error;
    QJsonDocument document = QJsonDocument::fromJson(data, &parse_error);
    if (document.isNull() || !document.isObject()) {
        QMessageBox::warning(this, "Ошибка", "Неверный json");
        return;
    }
    QJsonObject object = document.object();
    SimulationConfig config;
    config.rho = object.value("rho").toDouble(1.0);
    config.n = object.value("n").toInt(4);
    config.max_steps = object.value("k").toInt(1000);
    if (config.n < 4)
        config.n = 4;
    if (config.max_steps < 1)
        config.max_steps = 1000;
    QJsonObject distribution = object.value("distribution").toObject();
    config.dist_type = distribution.value("type").toString("uniform");
    if (config.dist_type != "uniform" && config.dist_type != "binomial" && config.dist_type != "geometric" && config.dist_type != "triangular")
        config.dist_type = "uniform";
    config.dist_params.clear();
    QJsonObject params = distribution.value("params").toObject();
    for (auto it = params.begin(); it != params.end(); ++it)
        config.dist_params[it.key()] = it.value().toVariant();
    double p = config.dist_params.value("p", 0.5).toDouble();
    if (p < 0.0 || p > 1.0)
        config.dist_params["p"] = 0.5;
    int m = object.value("m").toInt(1000);
    if (m < 1)
        m = 1000;
    m_edit->setText(QString::number(m));
    set_config_to_ui(config);
    QMessageBox::information(this, "Загрузка", "Конфигурация загружена");
}
void MainWindow::write_config_to_file(const QString& filename)
{
    SimulationConfig config = get_config_from_ui();
    QJsonObject object;
    object["rho"] = config.rho;
    object["n"] = config.n;
    object["k"] = config.max_steps;
    object["m"] = m_edit->text().toInt();
    QJsonObject distribution;
    distribution["type"] = config.dist_type;
    QJsonObject params;
    for (auto it = config.dist_params.begin(); it != config.dist_params.end(); ++it)
        params[it.key()] = QJsonValue::fromVariant(it.value());
    distribution["params"] = params;
    object["distribution"] = distribution;
    QJsonDocument document(object);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось записать файл");
        return;
    }
    file.write(document.toJson(QJsonDocument::Indented));
    file.close();
    QMessageBox::information(this, "Сохранение", "Конфигурация сохранена");
}
void MainWindow::on_load_config()
{
    QString filename = QFileDialog::getOpenFileName(this, "Загрузить json", "", "json (*.json)");
    if (!filename.isEmpty())
        read_config_from_file(filename);
}
void MainWindow::on_save_config()
{
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить json", "", "json (*.json)");
    if (!filename.isEmpty())
        write_config_to_file(filename);
}
void MainWindow::on_run_single()
{
    SimulationConfig config = get_config_from_ui();
    if (config.rho <= 0.0) {
        QMessageBox::warning(this, "Ошибка", "ρ должно быть больше 0");
        return;
    }
    if (config.n < 4) {
        QMessageBox::warning(this, "Ошибка", "n должно быть не меньше 4");
        return;
    }
    if (config.max_steps < 1) {
        QMessageBox::warning(this, "Ошибка", "k должно быть больше 0");
        return;
    }
    if (config.dist_type == "binomial" || config.dist_type == "geometric") {
        double p = config.dist_params.value("p", 0.5).toDouble();
        if (p <= 0.0 || p > 1.0) {
            QMessageBox::warning(this, "Ошибка", "p должно быть в диапазоне от 0 до 1");
            return;
        }
    }
    TrajectoryModel model(config);
    SimulationResult result = model.simulate_one();
    update_plot(result.points);
    QString info = QString("шагов до возвращения: %1\nвернулась в начало: %2").arg(result.steps).arg(result.returned ? "да" : "нет");
    result_edit->setText(info);
}
void MainWindow::on_run_multiple()
{
    SimulationConfig config = get_config_from_ui();
    int m = m_edit->text().toInt();
    if (config.rho <= 0.0) {
        QMessageBox::warning(this, "Ошибка", "ρ должно быть больше 0");
        return;
    }
    if (config.n < 4) {
        QMessageBox::warning(this, "Ошибка", "n должно быть не меньше 4");
        return;
    }
    if (config.max_steps < 1) {
        QMessageBox::warning(this, "Ошибка", "k должно быть больше 0");
        return;
    }
    if (m < 1) {
        QMessageBox::warning(this, "Ошибка", "m должно быть больше 0");
        return;
    }
    if (config.dist_type == "binomial" || config.dist_type == "geometric") {
        double p = config.dist_params.value("p", 0.5).toDouble();
        if (p <= 0.0 || p > 1.0) {
            QMessageBox::warning(this, "Ошибка", "p должно быть в диапазоне от 0 до 1");
            return;
        }
    }
    TrajectoryModel model(config);
    int returned_count = 0;
    for (int i = 0; i < m; ++i) {
        SimulationResult result = model.simulate_one();
        if (result.returned)
            ++returned_count;
    }
    double probability = static_cast<double>(returned_count) / static_cast<double>(m);
    QString text = QString("запусков: %1\nвернулось: %2\nдоля возвращений: %3").arg(m).arg(returned_count).arg(probability, 0, 'f', 6);
    result_edit->setText(text);
}