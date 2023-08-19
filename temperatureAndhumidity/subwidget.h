#ifndef SUBWIDGET_H
#define SUBWIDGET_H
#include <QtCharts>
#include<qchartview.h>//把widget提升为QChartView后加的头文件
using namespace QtCharts;
#include <QWidget>
#include<QLineSeries>
#include<QValueAxis>
#include<QColor>

namespace Ui {
class Subwidget;
}

class Subwidget : public QWidget
{
    Q_OBJECT

public:
    explicit Subwidget(QWidget *parent = nullptr);
    ~Subwidget();
    void updateDB();
    void initChart();



private slots:
    void on_pushButton_clicked();

    void on_checkBox_clicked();

    void on_checkBox_2_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Subwidget *ui;
    QLineSeries * line_w;//(折线)
    QLineSeries * line_s;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    //int lineMax=24;




signals:
    void back();
};

#endif // SUBWIDGET_H
