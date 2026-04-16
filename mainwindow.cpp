#include "mainwindow.h"
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPainter>
#include <QPen>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    updateStatsDisplay();
}

MainWindow::~MainWindow() {}

void MainWindow::pushToHistory() {
    if (currentRV.isValid()) history.push(currentRV);
    if (history.size() > 10) history.removeFirst();
}

void MainWindow::undo() {
    if (history.isEmpty()) {
        QMessageBox::information(this, "Отмена 💔", "Нет действий для отмены.");
        return;
    }
    currentRV = history.pop();
    updateStatsDisplay();
    QMessageBox::information(this, "Отмена 💗", "Последняя операция отменена.");
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    QHBoxLayout *topLayout = new QHBoxLayout();

    // Группа ввода
    QGroupBox *inputGroup = new QGroupBox("💗 Задание распределения 💗");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    tableDistribution = new QTableWidget(3, 2, this);
    tableDistribution->setHorizontalHeaderLabels({"Значение (x)", "Вероятность P(x)"});
    tableDistribution->horizontalHeader()->setStretchLastSection(true);
    tableDistribution->setAlternatingRowColors(true);
    inputLayout->addWidget(tableDistribution);
    btnAddRow = new QPushButton("➕ Добавить строку 💖");
    connect(btnAddRow, &QPushButton::clicked, this, &MainWindow::onAddRow);
    inputLayout->addWidget(btnAddRow);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnSet = new QPushButton("💗 Задать СВ 💗");
    btnScalar = new QPushButton("💖 Умножить на скаляр 💖");
    btnLayout->addWidget(btnSet);
    btnLayout->addWidget(btnScalar);
    inputLayout->addLayout(btnLayout);
    topLayout->addWidget(inputGroup);

    // Группа операций
    QGroupBox *opGroup = new QGroupBox("💕 Операции над СВ 💕");
    QVBoxLayout *opLayout = new QVBoxLayout(opGroup);
    btnAdd = new QPushButton("💞 Сложение (текущая + загруженная) 💞");
    btnMul = new QPushButton("💗 Умножение (текущая × загруженная) 💗");
    btnSave = new QPushButton("💾 Сохранить в файл 💖");
    btnLoad = new QPushButton("📂 Загрузить (вторая СВ) 💕");
    btnUndo = new QPushButton("↩️ Отменить последнюю операцию 💔");
    opLayout->addWidget(btnAdd);
    opLayout->addWidget(btnMul);
    opLayout->addWidget(btnSave);
    opLayout->addWidget(btnLoad);
    opLayout->addWidget(btnUndo);
    topLayout->addWidget(opGroup);

    // Группа статистики
    QGroupBox *statsGroup = new QGroupBox("Числовые характеристики💖    ");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    statsLayout->setContentsMargins(10, 15, 10, 15);
    labelExpectation = new QLabel("Мат. ожидание: —");
    labelVariance = new QLabel("Дисперсия: —");
    labelAsymmetry = new QLabel("Асимметрия: —");
    labelKurtosis = new QLabel("Эксцесс: —");
    statsLayout->addWidget(labelExpectation);
    statsLayout->addWidget(labelVariance);
    statsLayout->addWidget(labelAsymmetry);
    statsLayout->addWidget(labelKurtosis);
    topLayout->addWidget(statsGroup);

    mainLayout->addLayout(topLayout);

    // Группа отображения
    QGroupBox *displayGroup = new QGroupBox("📈 Отображение 💖");
    QHBoxLayout *displayLayout = new QHBoxLayout(displayGroup);
    btnLaw = new QPushButton("📋 Закон распределения 💗");
    btnPolyline = new QPushButton("📉 Полилайн 💕");
    btnCDF = new QPushButton("📊 Функция распределения 💞");
    displayLayout->addWidget(btnLaw);
    displayLayout->addWidget(btnPolyline);
    displayLayout->addWidget(btnCDF);
    mainLayout->addWidget(displayGroup);

    QLabel *statusLabel = new QLabel("💗 Готово. Заполните таблицу и нажмите «Задать СВ». 💗");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("background-color: #ffc0cb; color: #600030; font: italic 10pt;");
    mainLayout->addWidget(statusLabel);

    connect(btnSet, &QPushButton::clicked, this, &MainWindow::onSetDistribution);
    connect(btnScalar, &QPushButton::clicked, this, &MainWindow::onMultiplyByScalar);
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddRV);
    connect(btnMul, &QPushButton::clicked, this, &MainWindow::onMultiplyRV);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveToFile);
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadFromFile);
    connect(btnLaw, &QPushButton::clicked, this, &MainWindow::onDisplayLaw);
    connect(btnPolyline, &QPushButton::clicked, this, &MainWindow::onDisplayPolyline);
    connect(btnCDF, &QPushButton::clicked, this, &MainWindow::onDisplayCDF);
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::onUndo);

    QString style = R"(
        QMainWindow { background-color: #ffe0f0; }
        QWidget { background-color: #ffe0f0; }
        QGroupBox {
            font: bold 12pt "Segoe UI";
            border: 2px solid #ffb6c1;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: #fff0f5;
            color: #b0306e;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            background-color: #ffc0cb;
            color: #8b0045;
        }
        QPushButton {
            background-color: #ffb6c1;
            border: 1px solid #ff69b4;
            border-radius: 6px;
            padding: 1px;
            font: bold 10pt "Segoe UI";
            color: #8b0045;
        }
        QPushButton:hover { background-color: #ff9eb5; border-color: #ff1493; }
        QPushButton:pressed { background-color: #ff80a0; }
        QLabel {
            color: #8b0045;
            font: 10pt "Segoe UI";
            background-color: rgba(255, 240, 245, 180);
            border-radius: 4px;
            padding: 4px;
        }
        QTableWidget {
            background-color: #ffffff;
            alternate-background-color: #ffb6c1;
            gridline-color: #ff69b4;
            border: 1px solid #ff69b4;
            border-radius: 6px;
            font: 10pt "Segoe UI";
            color: #4a0025;
        }
        QTableWidget::item:alternate { background-color: #ffb6c1; }
        QHeaderView::section {
            background-color: #ffc0cb;
            color: #8b0045;
            font: bold 9pt;
            border: 1px solid #ffb6c1;
        }
    )";
    this->setStyleSheet(style);
}

void MainWindow::onAddRow() {
    tableDistribution->insertRow(tableDistribution->rowCount());
}

void MainWindow::onSetDistribution() {
    QVector<QPair<double,double>> pairs;
    for (int i = 0; i < tableDistribution->rowCount(); ++i) {
        QTableWidgetItem *valItem = tableDistribution->item(i, 0);
        QTableWidgetItem *probItem = tableDistribution->item(i, 1);
        if (!valItem || !probItem) continue;
        bool ok1, ok2;
        double val = valItem->text().toDouble(&ok1);
        double prob = probItem->text().toDouble(&ok2);
        if (ok1 && ok2 && prob >= 0) {
            pairs.append({val, prob});
        } else if (!valItem->text().isEmpty() || !probItem->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка 💔", "Некорректные данные в строке " + QString::number(i+1));
            return;
        }
    }
    if (pairs.isEmpty()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет данных");
        return;
    }
    DiscreteRandomVariable rv;
    if (rv.setDistribution(pairs)) {
        history.clear();
        currentRV = rv;
        updateStatsDisplay();
        QMessageBox::information(this, "Успех 💗", "Распределение задано корректно.");
    } else {
        QMessageBox::warning(this, "Ошибка 💔", rv.lastError());
    }
}

void MainWindow::onMultiplyByScalar() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Сначала задайте случайную величину.");
        return;
    }
    bool ok;
    double scalar = QInputDialog::getDouble(this, "Умножение на скаляр 💖", "Введите число:", 1.0, -1e9, 1e9, 5, &ok);
    if (!ok) return;
    pushToHistory();
    DiscreteRandomVariable result = currentRV * scalar;
    if (result.isValid()) {
        currentRV = result;
        updateStatsDisplay();
        QMessageBox::information(this, "Успех 💗", "Операция выполнена.");
    } else {
        QMessageBox::warning(this, "Ошибка 💔", "Результат некорректен.");
        if (!history.isEmpty()) history.pop();
    }
}

void MainWindow::onAddRV() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет текущей СВ.");
        return;
    }
    if (!otherRV.isValid()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Сложение 💗",
            "Вторая СВ не загружена. Хотите загрузить её из файла?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            onLoadFromFile();
            if (!otherRV.isValid()) {
                QMessageBox::warning(this, "Ошибка 💔", "Не удалось загрузить вторую СВ.");
                return;
            }
        } else {
            return;
        }
    }
    pushToHistory();
    DiscreteRandomVariable result = currentRV + otherRV;
    if (result.isValid()) {
        currentRV = result;
        updateStatsDisplay();
        QMessageBox::information(this, "Успех 💗", "Сложение выполнено.");
    } else {
        QMessageBox::warning(this, "Ошибка 💔", "Ошибка при сложении.");
        if (!history.isEmpty()) history.pop();
    }
}

void MainWindow::onMultiplyRV() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет текущей СВ.");
        return;
    }
    if (!otherRV.isValid()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Умножение 💗",
            "Вторая СВ не загружена. Хотите загрузить её из файла?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            onLoadFromFile();
            if (!otherRV.isValid()) {
                QMessageBox::warning(this, "Ошибка 💔", "Не удалось загрузить вторую СВ.");
                return;
            }
        } else {
            return;
        }
    }
    pushToHistory();
    DiscreteRandomVariable result = currentRV * otherRV;
    if (result.isValid()) {
        currentRV = result;
        updateStatsDisplay();
        QMessageBox::information(this, "Успех 💗", "Умножение выполнено.");
    } else {
        QMessageBox::warning(this, "Ошибка 💔", "Ошибка при умножении.");
        if (!history.isEmpty()) history.pop();
    }
}

void MainWindow::onSaveToFile() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет корректной СВ для сохранения.");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить распределение 💾", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;
    if (currentRV.saveToFile(filename))
        QMessageBox::information(this, "Успех 💗", "Сохранено в " + filename);
    else
        QMessageBox::warning(this, "Ошибка 💔", "Не удалось сохранить файл.");
}

void MainWindow::onLoadFromFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Загрузить распределение 📂", "", "Text files (*.txt)");
    if (filename.isEmpty()) return;
    DiscreteRandomVariable rv;
    if (rv.loadFromFile(filename)) {
        otherRV = rv;
        QMessageBox::information(this, "Успех 💗", "Загружена вторая СВ (для операций).\nТеперь можно сложить или умножить.");
    } else {
        QMessageBox::warning(this, "Ошибка 💔", "Не удалось загрузить или распределение некорректно.");
    }
}

void MainWindow::updateStatsDisplay() {
    if (!currentRV.isValid()) {
        labelExpectation->setText("Мат. ожидание: —");
        labelVariance->setText("Дисперсия: —");
        labelAsymmetry->setText("Асимметрия: —");
        labelKurtosis->setText("Эксцесс: —");
        return;
    }
    labelExpectation->setText(QString("Мат. ожидание: %1").arg(currentRV.expectation(), 0, 'f', 6));
    labelVariance->setText(QString("Дисперсия: %1").arg(currentRV.variance(), 0, 'f', 6));
    labelAsymmetry->setText(QString("Асимметрия: %1").arg(currentRV.asymmetry(), 0, 'f', 6));
    labelKurtosis->setText(QString("Эксцесс: %1").arg(currentRV.kurtosis(), 0, 'f', 6));
}

void MainWindow::onDisplayLaw() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет корректной СВ.");
        return;
    }
    QDialog dialog(this);
    dialog.setWindowTitle("Закон распределения 💖");
    QVBoxLayout layout(&dialog);
    QTableWidget table;
    table.setColumnCount(2);
    table.setHorizontalHeaderLabels({"Значение", "Вероятность"});
    auto dist = currentRV.distribution();
    table.setRowCount(dist.size());
    for (int i = 0; i < dist.size(); ++i) {
        table.setItem(i, 0, new QTableWidgetItem(QString::number(dist[i].first)));
        table.setItem(i, 1, new QTableWidgetItem(QString::number(dist[i].second)));
    }
    layout.addWidget(&table);
    QDialogButtonBox buttonBox(QDialogButtonBox::Close);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&buttonBox);
    dialog.exec();
}

// ---------- Виджет полилайна с началом координат ----------
class PolylineWidget : public QWidget {
public:
    PolylineWidget(const QVector<QPair<double, double>> &points, QWidget *parent = nullptr)
        : QWidget(parent), m_points(points) {}
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(255, 240, 245));
        if (m_points.isEmpty()) return;

        double minX = m_points[0].first, maxX = m_points[0].first;
        double minY = m_points[0].second, maxY = m_points[0].second;
        for (const auto &p : m_points) {
            if (p.first < minX) minX = p.first;
            if (p.first > maxX) maxX = p.first;
            if (p.second < minY) minY = p.second;
            if (p.second > maxY) maxY = p.second;
        }
        // Если все X положительные, начинаем ось с 0
        if (minX > 0) minX = 0;
        else minX -= 0.1 * (maxX - minX);
        if (minY > 0) minY = 0;
        else minY -= 0.1 * (maxY - minY);
        maxX += 0.1 * (maxX - minX);
        maxY += 0.1 * (maxY - minY);

        int w = width(), h = height();
        int left = 80, right = 50, top = 50, bottom = 70;
        int drawWidth = w - left - right;
        int drawHeight = h - top - bottom;

        auto toX = [&](double x) { return left + (x - minX) / (maxX - minX) * drawWidth; };
        auto toY = [&](double y) { return top + drawHeight - (y - minY) / (maxY - minY) * drawHeight; };

        painter.setPen(QPen(Qt::black, 1.5));
        painter.drawLine(left, top, left, top + drawHeight);
        painter.drawLine(left, top + drawHeight, left + drawWidth, top + drawHeight);
        painter.drawLine(left, top, left-5, top+8);
        painter.drawLine(left, top, left+5, top+8);
        painter.drawLine(left+drawWidth, top+drawHeight, left+drawWidth-8, top+drawHeight-5);
        painter.drawLine(left+drawWidth, top+drawHeight, left+drawWidth-8, top+drawHeight+5);

        painter.drawText(left + drawWidth/2 - 30, top + drawHeight + 35, "Значение (x) 💗");
        painter.save();
        painter.translate(left - 35, top + drawHeight/2);
        painter.rotate(-90);
        painter.drawText(0, 0, "Вероятность P(x) 💖");
        painter.restore();

        painter.setPen(QPen(Qt::gray, 0.5, Qt::DashLine));
        for (int i = 0; i <= 5; ++i) {
            double y = minY + i * (maxY - minY) / 5.0;
            int yPix = toY(y);
            painter.drawLine(left, yPix, left+drawWidth, yPix);
            painter.setPen(QPen(Qt::black, 1));
            painter.drawText(left-45, yPix+3, QString::number(y, 'f', 3));
            painter.setPen(QPen(Qt::gray, 0.5, Qt::DashLine));
        }
        for (int i = 0; i <= 5; ++i) {
            double x = minX + i * (maxX - minX) / 5.0;
            int xPix = toX(x);
            painter.drawLine(xPix, top, xPix, top+drawHeight);
            painter.setPen(QPen(Qt::black, 1));
            painter.drawText(xPix-20, top+drawHeight+20, QString::number(x, 'f', 3));
            painter.setPen(QPen(Qt::gray, 0.5, Qt::DashLine));
        }

        painter.setPen(QPen(QColor(255, 105, 180), 2));
        QPoint prevPoint;
        for (int i = 0; i < m_points.size(); ++i) {
            QPoint curPoint(toX(m_points[i].first), toY(m_points[i].second));
            if (i > 0) painter.drawLine(prevPoint, curPoint);
            painter.setBrush(QBrush(QColor(255, 20, 147)));
            painter.drawEllipse(curPoint, 6, 6);
            prevPoint = curPoint;
        }
    }
private:
    QVector<QPair<double, double>> m_points;
};

// ---------- Виджет функции распределения с началом координат ----------
class CDFWidget : public QWidget {
public:
    CDFWidget(const QVector<QPair<double, double>> &points, QWidget *parent = nullptr)
        : QWidget(parent), m_points(points) {}
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(255, 240, 245));
        if (m_points.isEmpty()) return;

        QVector<QPair<double, double>> cdf;
        double cum = 0.0;
        for (const auto &p : m_points) {
            cum += p.second;
            cdf.append({p.first, cum});
        }
        if (cdf.isEmpty()) return;

        double minX = cdf[0].first, maxX = cdf.last().first;
        if (minX > 0) minX = 0;
        else minX -= 0.1 * (maxX - minX);
        maxX += 0.1 * (maxX - minX);
        double minY = 0.0, maxY = 1.0;
        maxY += 0.05;

        int w = width(), h = height();
        int left = 80, right = 50, top = 50, bottom = 70;
        int drawWidth = w - left - right;
        int drawHeight = h - top - bottom;

        auto toX = [&](double x) { return left + (x - minX) / (maxX - minX) * drawWidth; };
        auto toY = [&](double y) { return top + drawHeight - (y - minY) / (maxY - minY) * drawHeight; };

        painter.setPen(QPen(Qt::black, 1.5));
        painter.drawLine(left, top, left, top + drawHeight);
        painter.drawLine(left, top + drawHeight, left + drawWidth, top + drawHeight);
        painter.drawLine(left, top, left-5, top+8);
        painter.drawLine(left, top, left+5, top+8);
        painter.drawLine(left+drawWidth, top+drawHeight, left+drawWidth-8, top+drawHeight-5);
        painter.drawLine(left+drawWidth, top+drawHeight, left+drawWidth-8, top+drawHeight+5);

        painter.drawText(left + drawWidth/2 - 30, top + drawHeight + 35, "x 💗");
        painter.save();
        painter.translate(left - 35, top + drawHeight/2);
        painter.rotate(-90);
        painter.drawText(0, 0, "F(x) 💖");
        painter.restore();

        painter.setPen(QPen(Qt::gray, 0.5, Qt::DashLine));
        for (int i = 0; i <= 5; ++i) {
            double y = minY + i * (maxY - minY) / 5.0;
            int yPix = toY(y);
            painter.drawLine(left, yPix, left+drawWidth, yPix);
            painter.setPen(QPen(Qt::black, 1));
            painter.drawText(left-45, yPix+3, QString::number(y, 'f', 2));
            painter.setPen(QPen(Qt::gray, 0.5, Qt::DashLine));
        }
        for (int i = 0; i <= 5; ++i) {
            double x = minX + i * (maxX - minX) / 5.0;
            int xPix = toX(x);
            painter.drawLine(xPix, top, xPix, top+drawHeight);
            painter.setPen(QPen(Qt::black, 1));
            painter.drawText(xPix-20, top+drawHeight+20, QString::number(x, 'f', 3));
            painter.setPen(QPen(Qt::gray, 0.5, Qt::DashLine));
        }

        painter.setPen(QPen(QColor(255, 105, 180), 2));
        double prevX = minX;
        double prevY = 0.0;
        for (const auto &p : cdf) {
            double curX = p.first;
            double curY = p.second;
            QPoint p1(toX(prevX), toY(prevY));
            QPoint p2(toX(curX), toY(prevY));
            painter.drawLine(p1, p2);
            QPoint p3(toX(curX), toY(curY));
            painter.drawLine(p2, p3);
            prevX = curX;
            prevY = curY;
        }
        QPoint last1(toX(prevX), toY(prevY));
        QPoint last2(toX(maxX), toY(prevY));
        painter.drawLine(last1, last2);
    }
private:
    QVector<QPair<double, double>> m_points;
};
// ... все предыдущие включения ...

void MainWindow::onDisplayPolyline() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет корректной СВ.");
        return;
    }
    auto dist = currentRV.distribution();
    // Отладочный вывод – проверьте, какие данные реально идут в график
    qDebug() << "Полилайн: точки:";
    for (auto &p : dist) qDebug() << p.first << p.second;

    PolylineWidget *widget = new PolylineWidget(dist);
    widget->setMinimumSize(800, 600);
    QDialog dialog(this);
    dialog.setWindowTitle("Полилайн 💕");
    QVBoxLayout layout(&dialog);
    layout.addWidget(widget);
    QDialogButtonBox buttonBox(QDialogButtonBox::Close);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&buttonBox);
    dialog.exec();
}

void MainWindow::onDisplayCDF() {
    if (!currentRV.isValid()) {
        QMessageBox::warning(this, "Ошибка 💔", "Нет корректной СВ.");
        return;
    }
    auto dist = currentRV.distribution();
    qDebug() << "CDF: точки:";
    for (auto &p : dist) qDebug() << p.first << p.second;

    CDFWidget *widget = new CDFWidget(dist);
    widget->setMinimumSize(800, 600);
    QDialog dialog(this);
    dialog.setWindowTitle("Функция распределения 💞");
    QVBoxLayout layout(&dialog);
    layout.addWidget(widget);
    QDialogButtonBox buttonBox(QDialogButtonBox::Close);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout.addWidget(&buttonBox);
    dialog.exec();
}

void MainWindow::onUndo() { undo(); }