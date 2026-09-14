#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H
#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QPainter>
#include <QRect>
#include <QColor>
class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);
    void setData(const QVector<QPointF> &fall_data,const QVector<QPointF> &return_data);
    void clearData();
protected:
    void paintEvent(QPaintEvent *event) override;//переопр защищенный метод, вызывается при небоходимости перерисовки виджета
private:
    QVector<QPointF> fall_data;//точки падения
    QVector<QPointF> return_data;//точки кривой влзврата (x,y)
    void draw_axes(QPainter &painter,const QRect &rect,double x_max);//рисовка осей, внутри рект, с макс знач по оси Х
    void draw_curve(QPainter &painter,const QVector<QPointF> &data,const QColor &color,const QRect &rect,double x_max);//рисовка кривой с масштабом по Х
    void draw_legend(QPainter &painter,const QRect &rect);//рисоква легенды пояснение какая кривая что означает
};
#endif