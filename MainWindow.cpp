#include "MainWindow.h"
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
#include <algorithm>
#include <climits>
PlotWidget::PlotWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(300);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(255, 220, 240));
    setPalette(pal);
}
void PlotWidget::setData(const std::vector<int>& indices, double tau, int numIntervals, double M) {
    m_indices = indices;
    m_tau = tau;
    m_numIntervals = numIntervals;
    m_M = M;
    update();
}
void PlotWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);
    if (m_indices.empty() || m_numIntervals < 1) return;
    double maxY = 0;
    for (int v : m_indices) if (v * m_tau > maxY) maxY = v * m_tau;
    if (maxY == 0) maxY = 1;
    maxY *= 1.1;
    double xScale = width() / m_M;
    double yScale = height() / maxY;
    QPen pen(Qt::blue, 2);
    painter.setPen(pen);
    for (int i = 0; i < m_numIntervals; ++i) {
        double x1 = i * (m_M / m_numIntervals);
        double x2 = (i + 1) * (m_M / m_numIntervals);
        double yVal = m_indices[i] * m_tau;
        double y = height() - yVal * yScale;
        painter.drawLine(x1 * xScale, y, x2 * xScale, y);
        if (i < m_numIntervals - 1) {
            double nextY = height() - m_indices[i+1] * m_tau * yScale;
            painter.drawLine(x2 * xScale, y, x2 * xScale, nextY);
        }
    }
    painter.setPen(Qt::black);
    painter.drawLine(0, height(), width(), height());
    painter.drawLine(0, 0, 0, height());
}
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_currentIndex(0) {
    setupUI();
    setPinkStyle();
}
MainWindow::~MainWindow() {}
void MainWindow::setPinkStyle() {
    QPalette pinkPalette;
    pinkPalette.setColor(QPalette::Window, QColor(255, 220, 240));
    pinkPalette.setColor(QPalette::WindowText, QColor(128, 0, 64));
    pinkPalette.setColor(QPalette::Button, QColor(255, 182, 193));
    pinkPalette.setColor(QPalette::ButtonText, QColor(128, 0, 64));
    pinkPalette.setColor(QPalette::Base, QColor(255, 240, 245));
    pinkPalette.setColor(QPalette::Text, QColor(128, 0, 64));
    pinkPalette.setColor(QPalette::Highlight, QColor(255, 105, 180));
    pinkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    qApp->setPalette(pinkPalette);
    setStyleSheet("QMainWindow { background-color: #FFE4E1; }");
    QString buttonStyle = "QPushButton { background-color: #FF69B4; color: white; border-radius: 5px; padding: 6px; font-weight: bold; } QPushButton:hover { background-color: #FF1493; } QPushButton:pressed { background-color: #DB7093; }";
    m_generateBtn->setStyleSheet(buttonStyle);
    m_loadBtn->setStyleSheet(buttonStyle);
    m_saveBtn->setStyleSheet(buttonStyle);
    m_prevBtn->setStyleSheet(buttonStyle);
    m_nextBtn->setStyleSheet(buttonStyle);
    QString groupStyle = "QGroupBox { border: 2px solid #FF69B4; border-radius: 5px; margin-top: 10px; font-weight: bold; color: #8B008B; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }";
    for (auto* group : findChildren<QGroupBox*>()) group->setStyleSheet(groupStyle);
    QString editStyle = "background-color: #FFE4E1; border: 1px solid #FF69B4;";
    m_MEdit->setStyleSheet(editStyle);
    m_hEdit->setStyleSheet(editStyle);
    m_tauEdit->setStyleSheet(editStyle);
    m_nEdit->setStyleSheet(editStyle);
    m_NEdit->setStyleSheet(editStyle);
    m_pEdit->setStyleSheet(editStyle);
    m_distCombo->setStyleSheet("background-color: #FFE4E1; border: 1px solid #FF69B4;");
    m_resultEdit->setStyleSheet("background-color: #FFF0F5; border: 1px solid #FF69B4; color: #8B008B;");
}
void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    QGroupBox* paramsGroup = new QGroupBox("Параметры");
    QFormLayout* form = new QFormLayout(paramsGroup);
    m_MEdit = new QLineEdit; m_MEdit->setText("10.0");
    m_hEdit = new QLineEdit; m_hEdit->setText("1.0");
    m_tauEdit = new QLineEdit; m_tauEdit->setText("1.0");
    m_nEdit = new QLineEdit; m_nEdit->setText("5");
    m_NEdit = new QLineEdit; m_NEdit->setText("1000");
    m_distCombo = new QComboBox; m_distCombo->addItems({"uniform","binomial","geometric","triangular"});
    m_pEdit = new QLineEdit; m_pEdit->setText("0.5");
    form->addRow("M (длина отрезка):", m_MEdit);
    form->addRow("h (шаг разбиения):", m_hEdit);
    form->addRow("τ (шаг значений):", m_tauEdit);
    form->addRow("n (макс. кратное):", m_nEdit);
    form->addRow("N (число генераций):", m_NEdit);
    form->addRow("Распределение ξ:", m_distCombo);
    form->addRow("Параметр p (bin/geom):", m_pEdit);
    mainLayout->addWidget(paramsGroup);
    QHBoxLayout* btnLayout = new QHBoxLayout;
    m_generateBtn = new QPushButton("Сгенерировать");
    m_loadBtn = new QPushButton("Загрузить JSON");
    m_saveBtn = new QPushButton("Сохранить JSON");
    btnLayout->addWidget(m_generateBtn);
    btnLayout->addWidget(m_loadBtn);
    btnLayout->addWidget(m_saveBtn);
    mainLayout->addLayout(btnLayout);
    m_plot = new PlotWidget;
    mainLayout->addWidget(m_plot);
    QHBoxLayout* navLayout = new QHBoxLayout;
    m_prevBtn = new QPushButton("◀ Предыдущая");
    m_nextBtn = new QPushButton("Следующая ▶");
    m_pageLabel = new QLabel("Фигура 0 из 0");
    navLayout->addWidget(m_prevBtn);
    navLayout->addWidget(m_pageLabel);
    navLayout->addWidget(m_nextBtn);
    mainLayout->addLayout(navLayout);
    m_resultEdit = new QTextEdit;
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setMaximumHeight(150);
    mainLayout->addWidget(m_resultEdit);
    connect(m_generateBtn, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadConfig);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveConfig);
    connect(m_prevBtn, &QPushButton::clicked, this, &MainWindow::onPrevFigure);
    connect(m_nextBtn, &QPushButton::clicked, this, &MainWindow::onNextFigure);
}
FigureParams MainWindow::getParamsFromUI() const {
    FigureParams p;
    p.M = m_MEdit->text().toDouble();
    p.h = m_hEdit->text().toDouble();
    p.tau = m_tauEdit->text().toDouble();
    p.n = m_nEdit->text().toInt();
    p.N = m_NEdit->text().toInt();
    p.distType = m_distCombo->currentText();
    p.distParams.clear();
    if (p.distType == "binomial" || p.distType == "geometric")
        p.distParams["p"] = m_pEdit->text().toDouble();
    return p;
}
void MainWindow::setParamsToUI(const FigureParams& p) {
    m_MEdit->setText(QString::number(p.M));
    m_hEdit->setText(QString::number(p.h));
    m_tauEdit->setText(QString::number(p.tau));
    m_nEdit->setText(QString::number(p.n));
    m_NEdit->setText(QString::number(p.N));
    m_distCombo->setCurrentText(p.distType);
    if (p.distParams.contains("p"))
        m_pEdit->setText(QString::number(p.distParams["p"].toDouble()));
}
void MainWindow::updateCurrentFigure() {
    int total = m_model.getFigureCount();
    if (total == 0) {
        m_plot->setData({}, 1, 0, 1);
        m_pageLabel->setText("Фигура 0 из 0");
        return;
    }
    if (m_currentIndex < 0) m_currentIndex = 0;
    if (m_currentIndex >= total) m_currentIndex = total - 1;
    auto params = getParamsFromUI();
    const auto& fig = m_model.getFigure(m_currentIndex);
    m_plot->setData(fig, params.tau, m_model.getNumIntervals(), params.M);
    m_pageLabel->setText(QString("Фигура %1 из %2").arg(m_currentIndex+1).arg(total));
}
void MainWindow::onGenerate() {
    FigureParams params = getParamsFromUI();
    if (params.M <= 0 || params.h <= 0 || params.tau <= 0 || params.n < 0 || params.N <= 0) {
        QMessageBox::warning(this, "Ошибка", "Все параметры должны быть положительными (n >= 0)");
        return;
    }
    m_model.generateAll(params);
    m_currentIndex = 0;
    updateCurrentFigure();
    int total = m_model.getFigureCount();
    double prob = m_model.getProbability();
    QString info = QString("Сгенерировано фигур: %1\nСтрого возрастающих: %2\nВероятность: %3")
                       .arg(total).arg(m_model.getStrictlyIncreasingCount()).arg(prob, 0, 'f', 6);
    m_resultEdit->setText(info);
}
void MainWindow::onPrevFigure() {
    if (m_model.getFigureCount() == 0) return;
    --m_currentIndex;
    if (m_currentIndex < 0) m_currentIndex = m_model.getFigureCount() - 1;
    updateCurrentFigure();
}
void MainWindow::onNextFigure() {
    if (m_model.getFigureCount() == 0) return;
    ++m_currentIndex;
    if (m_currentIndex >= m_model.getFigureCount()) m_currentIndex = 0;
    updateCurrentFigure();
}
void MainWindow::readConfigFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Неверный JSON");
        return;
    }
    QJsonObject obj = doc.object();
    FigureParams p;
    p.M = obj["M"].toDouble(10.0);
    p.h = obj["h"].toDouble(1.0);
    p.tau = obj["tau"].toDouble(1.0);
    p.n = obj["n"].toInt(5);
    p.N = obj["N"].toInt(1000);
    p.distType = obj["distribution"].toObject()["type"].toString("uniform");
    p.distParams.clear();
    QJsonObject params = obj["distribution"].toObject()["params"].toObject();
    for (auto it = params.begin(); it != params.end(); ++it)
        p.distParams[it.key()] = it.value().toVariant();
    setParamsToUI(p);
    QMessageBox::information(this, "Загрузка", "Конфигурация загружена");
}
void MainWindow::writeConfigToFile(const QString& filename) {
    FigureParams p = getParamsFromUI();
    QJsonObject obj;
    obj["M"] = p.M; obj["h"] = p.h; obj["tau"] = p.tau; obj["n"] = p.n; obj["N"] = p.N;
    QJsonObject distObj;
    distObj["type"] = p.distType;
    QJsonObject paramsObj;
    for (auto it = p.distParams.begin(); it != p.distParams.end(); ++it)
        paramsObj[it.key()] = it.value().toJsonValue();
    distObj["params"] = paramsObj;
    obj["distribution"] = distObj;
    QJsonDocument doc(obj);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось записать файл");
        return;
    }
    file.write(doc.toJson());
    QMessageBox::information(this, "Сохранение", "Конфигурация сохранена");
}
void MainWindow::onLoadConfig() {
    QString fileName = QFileDialog::getOpenFileName(this, "Загрузить JSON", "", "JSON (*.json)");
    if (!fileName.isEmpty()) readConfigFromFile(fileName);
}
void MainWindow::onSaveConfig() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить JSON", "", "JSON (*.json)");
    if (!fileName.isEmpty()) writeConfigToFile(fileName);
}