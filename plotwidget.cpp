#include "plotwidget.h"
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QtMath>
PlotWidget::PlotWidget(QWidget *parent): QWidget(parent)
{
    setMinimumSize(500, 350);
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, Qt::white);
    setPalette(palette);}
void PlotWidget::setData(const QVector<QPointF> &new_fall_data,const QVector<QPointF> &new_return_data)
{
    fall_data = new_fall_data;
    return_data = new_return_data;
    update();}
void PlotWidget::clearData()
{
    fall_data.clear();
    return_data.clear();
    update();}
void PlotWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect().adjusted(70,30,-40,-60);
    if (rect.width() <= 0 || rect.height() <= 0){return;}
    if (fall_data.isEmpty())
    {
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 11));
        painter.drawText(rect,Qt::AlignCenter,"Введите параметры и нажмите «Запустить»");
        return;}
    double x_max = fall_data.last().x();
    if (x_max < 1.0){x_max = 1.0;}
    draw_axes(painter,rect,x_max);
    draw_curve(painter,fall_data,Qt::red,rect,x_max);
    draw_curve(painter,return_data,Qt::blue,rect,x_max);
    draw_legend(painter,rect);}
void PlotWidget::draw_axes(QPainter &painter,const QRect &rect,double x_max)
{
    painter.save();
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(rect.left(),rect.bottom(),rect.right(),rect.bottom());
    painter.drawLine(rect.left(),rect.top(),rect.left(),rect.bottom());
    painter.setFont(QFont("Arial", 9));
    painter.drawText(rect.right() - 10,rect.bottom() + 40,"B");
    painter.drawText(rect.left() - 55,rect.top() - 10,"P");
    int tick_count = qMin(static_cast<int>(x_max),10);
    if (tick_count < 1){tick_count = 1;}
    for (int i = 0; i <= tick_count; ++i)
    {
        double value =x_max * i / tick_count;
        int x =rect.left()+ static_cast<int>(value / x_max * rect.width());
        painter.drawLine(x,rect.bottom(),x,rect.bottom() + 5);
        painter.drawText(x - 12,rect.bottom() + 22,
        QString::number(value,'f',0));}
    for (int i = 0; i <= 5; ++i)
    {
        double value = i * 0.2;
        int y =rect.bottom()- static_cast<int>(value * rect.height());
        painter.drawLine(rect.left() - 5,y,rect.left(),y);
        painter.drawText(rect.left() - 35,y + 4,QString::number(value,'f',1));}
    painter.drawText(rect.left() + rect.width() / 2 - 80,rect.bottom() + 45,"Расстояние B от обрыва");
    painter.save();
    painter.translate(rect.left() - 50,rect.top() + rect.height() / 2);
    painter.rotate(-90);
    painter.drawText(0,0,"Эмпирическая вероятность");
    painter.restore();
    painter.restore();
}
void PlotWidget::draw_curve(QPainter &painter,const QVector<QPointF> &data,const QColor &color,const QRect &rect,double x_max)
{
    if (data.isEmpty()){return;}
    painter.save();
    painter.setPen(QPen(color, 2));
    auto to_pixel = [&](double x, double y)
    {
        int px =rect.left()+ static_cast<int>(x / x_max * rect.width());
        int py =rect.bottom() - static_cast<int>(y * rect.height());
        return QPoint(px,py);};
    QPoint previous =to_pixel(data.first().x(),data.first().y());
    for (int i = 1; i < data.size(); ++i)
    {
        QPoint current =to_pixel(data[i].x(),data[i].y());
        painter.drawLine(previous,current);
        previous = current;}
    for (const QPointF &point : data){QPoint pixel =to_pixel(point.x(),point.y());
    painter.drawEllipse(pixel,2,2);}
    painter.restore();
}
void PlotWidget::draw_legend(QPainter &painter,const QRect &rect)
{
    painter.save();
    int legend_x =rect.right() - 160;
    int legend_y =rect.top() + 10;
    int legend_w = 150;
    int legend_h = 60;
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(legend_x,legend_y,legend_w,legend_h);
    painter.setFont( QFont("Arial", 9));
    int y = legend_y + 18;
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(legend_x + 10,y,legend_x + 30,y);
    painter.setPen(Qt::black);
    painter.drawText(legend_x + 38,y + 4,"Падение");
    y += 25;
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(legend_x + 10,y,legend_x + 30,y);
    painter.setPen(Qt::black);
    painter.drawText(legend_x + 38,y + 4,"Возврат");
    painter.restore();
}