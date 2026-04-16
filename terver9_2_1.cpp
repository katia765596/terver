#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    connect(&simulator, &RandomWalkSimulator::positionChanged, this, &MainWindow::onPositionChanged);
    connect(&simulator, &RandomWalkSimulator::simulationFinished, this, &MainWindow::onSimulationFinished);
    connect(&simulator, &RandomWalkSimulator::errorOccurred, this, &MainWindow::onError);
}
MainWindow::~MainWindow() {}
void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    QGroupBox *paramGroup = new QGroupBox("💗 Параметры блуждания 💗");
    paramGroup->setAlignment(Qt::AlignCenter);
    QFormLayout *formLayout = new QFormLayout(paramGroup);
    formLayout->setSpacing(8);
    formLayout->setLabelAlignment(Qt::AlignRight);
    startPosEdit = new QLineEdit("0.0");
    stepsSpin = new QSpinBox();
    stepsSpin->setRange(1, 1000);
    stepsSpin->setValue(10);
    formLayout->addRow("💖 Начальное положение:", startPosEdit);
    formLayout->addRow("💕 Количество шагов:", stepsSpin);
    mainLayout->addWidget(paramGroup);
    QGroupBox *distGroup = new QGroupBox("📈 Закон перемещения (скорости) 😍");
    distGroup->setAlignment(Qt::AlignCenter);
    QVBoxLayout *distLayout = new QVBoxLayout(distGroup);
    distLayout->setSpacing(6);
    distTable = new QTableWidget(5, 2);
    distTable->setHorizontalHeaderLabels({"Скорость", "Вероятность"});
    distTable->horizontalHeader()->setStretchLastSection(true);
    distTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    distTable->verticalHeader()->setVisible(false);
    distTable->setAlternatingRowColors(true);
    distTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    distTable->setColumnWidth(0, 180);
    distTable->setColumnWidth(1, 150);
    distTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    distTable->setMinimumWidth(360);
    distTable->verticalHeader()->setDefaultSectionSize(32);
    distTable->setMinimumHeight(180);
    distTable->setMaximumHeight(220);
    distLayout->addWidget(distTable);
    QHBoxLayout *rowButtons = new QHBoxLayout();
    rowButtons->setSpacing(10);
    QPushButton *btnAddRow = new QPushButton("➕ Добавить строку");
    QPushButton *btnDelRow = new QPushButton("➖ Удалить строку");
    rowButtons->addWidget(btnAddRow);
    rowButtons->addWidget(btnDelRow);
    distLayout->addLayout(rowButtons);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    btnLoad = new QPushButton("📂 Загрузить из файла 💗");
    btnSetDist = new QPushButton("💖 Задать закон 💖");
    btnLayout->addWidget(btnLoad);
    btnLayout->addWidget(btnSetDist);
    distLayout->addLayout(btnLayout);
    mainLayout->addWidget(distGroup);
    QGroupBox *controlGroup = new QGroupBox("🎮 Управление 💕");
    controlGroup->setAlignment(Qt::AlignCenter);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    controlLayout->setSpacing(15);
    btnStart = new QPushButton("▶️ Старт 💗");
    btnStop = new QPushButton("⏹️ Стоп 💔");
    controlLayout->addWidget(btnStart);
    controlLayout->addWidget(btnStop);
    mainLayout->addWidget(controlGroup);
    positionLabel = new QLabel("💗 Текущая позиция: 0.00 💗");
    positionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(positionLabel);
    positionWidget = new PositionWidget(this);
    positionWidget->setMinimumHeight(70);
    positionWidget->setPosition(0.0, -10, 20);
    mainLayout->addWidget(positionWidget);
    QGroupBox *resultGroup = new QGroupBox("📊 Итоговое распределение положений 💖");
    resultGroup->setAlignment(Qt::AlignCenter);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    finalDistText = new QTextEdit();
    finalDistText->setReadOnly(true);
    finalDistText->setMinimumHeight(100);
    finalDistText->setMaximumHeight(130);
    resultLayout->addWidget(finalDistText);
    mainLayout->addWidget(resultGroup);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadDistribution);
    connect(btnSetDist, &QPushButton::clicked, this, &MainWindow::onSetDistribution);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopSimulation);
    connect(btnAddRow, &QPushButton::clicked, this, [this]() {
        distTable->insertRow(distTable->rowCount());
    });
    connect(btnDelRow, &QPushButton::clicked, this, [this]() {
        int row = distTable->currentRow();
        if (row >= 0 && distTable->rowCount() > 1)
            distTable->removeRow(row);
        else if (distTable->rowCount() <= 1)
            QMessageBox::warning(this, "Ошибка 💔", "Нельзя удалить единственную строку");
    });
    QString style = R"(
        QMainWindow { background-color: #ffe0f0; }
        QWidget { background-color: #ffe0f0; }
        QGroupBox {
            font: bold 10pt "Segoe UI";
            border: 1.5px solid #ffb6c1;
            border-radius: 8px;
            margin-top: 2ex;
            background-color: #fff0f5;
            color: #b0306e;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            left: 0px;
            right: 0px;
            padding: 0 8px;
            background-color: #ffc0cb;
            color: #8b0045;
        }
        QPushButton {
            background-color: #ffb6c1;
            border: 1px solid #ff69b4;
            border-radius: 6px;
            padding: 4px 8px;
            font: bold 9pt "Segoe UI";
            color: #8b0045;
            min-width: 100px;
        }
        QPushButton:hover { background-color: #ff9eb5; border-color: #ff1493; }
        QPushButton:pressed { background-color: #ff80a0; }
        QLabel {
            color: #8b0045;
            font: 9pt "Segoe UI";
            background-color: rgba(255, 240, 245, 200);
            border-radius: 5px;
            padding: 3px;
        }
        QLineEdit, QSpinBox {
            background-color: #fff0f5;
            border: 1px solid #ffb6c1;
            border-radius: 5px;
            padding: 2px;
            color: #4a0025;
            font-size: 9pt;
            min-height: 20px;
        }
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #fff5f7;
            gridline-color: #ffd0d8;
            border: 1px solid #ff69b4;
            border-radius: 6px;
            font: 9pt "Segoe UI";
            color: #4a0025;
        }
        QTableWidget::item {
            min-height: 30px;   /* гарантия высоты строки */
            padding: 4px;
        }
        QTableWidget::item:alternate { background-color: #fff5f7; }
        QHeaderView::section {
            background-color: #ffc0cb;
            color: #8b0045;
            font: bold 9pt;
            border: 1px solid #ffb6c1;
            padding: 4px;
        }
        QTextEdit {
            background-color: #fff0f5;
            border: 1px solid #ff69b4;
            border-radius: 6px;
            font: 9pt "Segoe UI";
            color: #4a0025;
            padding: 4px;
        }
    )";
    this->setStyleSheet(style);

    setMinimumSize(500, 650);
    resize(550, 700);
}
void MainWindow::onLoadDistribution()
{
    QString filename = QFileDialog::getOpenFileName(this, "💗 Выберите файл с законом перемещения 💗", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;
    if (simulator.loadStepDistributionFromFile(filename)) {
        stepRV = simulator.stepDistribution();
        QMessageBox::information(this, "Успех 💗", "Закон перемещения загружен");
    }
}
void MainWindow::onSetDistribution()
{
    QVector<QPair<double, double>> pairs;
    for (int i = 0; i < distTable->rowCount(); ++i) {
        QTableWidgetItem *valItem = distTable->item(i, 0);
        QTableWidgetItem *probItem = distTable->item(i, 1);
        if (!valItem || !probItem) continue;
        bool ok1, ok2;
        double val = valItem->text().toDouble(&ok1);
        double prob = probItem->text().toDouble(&ok2);
        if (ok1 && ok2 && prob >= 0) {
            pairs.append({val, prob});
            qDebug() << "Добавлена пара:" << val << prob;
        } else if (!valItem->text().isEmpty() || !probItem->text().isEmpty()) {
            qDebug() << "Ошибка в строке" << i+1 << ":" << valItem->text() << probItem->text();
            QMessageBox::warning(this, "Ошибка 💔", "Некорректные данные в строке " + QString::number(i+1));
            return;
        }
    }
    if (pairs.isEmpty()) {
        qDebug() << "Пары не найдены";
        QMessageBox::warning(this, "Ошибка 💔", "Нет данных");
        return;
    }
    qDebug() << "Всего пар:" << pairs.size();
    DiscreteRandomVariable rv;
    if (rv.setDistribution(pairs)) {
        qDebug() << "Распределение успешно задано.";
        stepRV = rv;
        simulator.setStepDistribution(stepRV);
        QMessageBox::information(this, "Успех 💗", "Закон перемещения задан");
    } else {
        qDebug() << "Ошибка задания распределения:" << rv.lastError();
        QMessageBox::warning(this, "Ошибка 💔", rv.lastError());
    }
}
void MainWindow::onStartSimulation()
{
    bool ok;
    double startPos = startPosEdit->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Ошибка 💔", "Некорректное начальное положение");
        return;
    }
    int steps = stepsSpin->value();
    simulator.setStartPosition(startPos);
    simulator.setStepsCount(steps);
    simulator.start();
    btnStart->setEnabled(false);
    btnStop->setEnabled(true);
}
void MainWindow::onStopSimulation()
{
    simulator.stop();
    btnStart->setEnabled(true);
    btnStop->setEnabled(false);
}
void MainWindow::onPositionChanged(double pos)
{
    positionLabel->setText(QString("💗 Текущая позиция: %1 💗").arg(pos, 0, 'f', 4));
    double margin = 10.0;
    double minRange = positionWidget->property("minRange").toDouble();
    double maxRange = positionWidget->property("maxRange").toDouble();
    if (minRange == 0 && maxRange == 0) {
        minRange = -10;
        maxRange = 20;
    }
    if (pos < minRange + margin) {
        minRange = pos - margin;
        maxRange = maxRange + (minRange - (pos - margin));
    }
    if (pos > maxRange - margin) {
        maxRange = pos + margin;
        minRange = minRange - ((pos + margin) - maxRange);
    }
    positionWidget->setPosition(pos, minRange, maxRange);
    positionWidget->setProperty("minRange", minRange);
    positionWidget->setProperty("maxRange", maxRange);
}
void MainWindow::onSimulationFinished()
{
    btnStart->setEnabled(true);
    btnStop->setEnabled(false);
    DiscreteRandomVariable finalDist = simulator.finalDistribution();
    QString text;
    if (finalDist.distribution().isEmpty()) {
        text = "💔 Распределение не получено  💔";
    } else {
        for (const auto &p : finalDist.distribution()) {
            text += QString("💖 x = %1 : P = %2 💖\n")
                        .arg(p.first, 0, 'f', 4)
                        .arg(p.second, 0, 'f', 6);
        }
    }
    finalDistText->setPlainText(text);
}
void MainWindow::onError(const QString &err)
{
    QMessageBox::critical(this, "Ошибка 💔", err);
    btnStart->setEnabled(true);
    btnStop->setEnabled(false);
}
