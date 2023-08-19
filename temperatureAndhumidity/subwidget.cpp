#include "subwidget.h"
#include "ui_subwidget.h"
#include<QPushButton>
#include<QSqlQuery>
#include<form.h>



Subwidget::Subwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Subwidget)
{
    ui->setupUi(this);
    setFixedSize( this->width(),this->height());//窗口定大小
    //this->resize(800,600);
    line_w = new QLineSeries();
    line_s = new QLineSeries();
    updateDB();
    initChart();

}

Subwidget::~Subwidget()
{
    delete ui;
}
void Subwidget::updateDB()
{
    QSqlQuery query;
    query.exec("select temperature,humidity from t_");
    int row = 0;
    QStringList l;
    QStringList li;//不能放进循环里
    //qDebug()<<query.value("temperature").toString();//为空
    while(query.next())
    {

        l<<query.value("temperature").toString();
        line_w->append(row+1,l.at(row).toUInt());//坐标

        li<<query.value("humidity").toString();
        line_s->append(row+1,li.at(row).toUInt());
        row++;

    }

}
void  Subwidget::initChart()
{
    axisX = new  QValueAxis();
    axisY = new  QValueAxis();
    axisX->setTitleText("time(1s)");
    axisY->setTitleText("value");
    chart=new QChart();
    line_w->setName("temperature");
    line_s->setName("humidity");
    line_s->setPointsVisible(true);//线由点构成
    line_w->setPointsVisible(true);
    line_s->setColor(QColor(255,2,1));
    line_w->setColor(QColor(22,33,250));

    chart->addSeries(line_w);//向图表中添加线
    chart->addSeries(line_s);
    chart->createDefaultAxes();//设置网格
    chart->setTitle("温湿度变化");
    //chart->setTheme(QChart::ChartThemeQt);
    chart->legend()->setVisible(true);
    chart->setTitleFont(QFont("微软雅黑"));

    chart->setAxisX(axisX,line_w);//添加坐标轴,必须再addseries后调用
    chart->setAxisY(axisY,line_w);
    chart->setAxisX(axisX,line_s);
    chart->setAxisY(axisY,line_s);

    ui->widget->setChart(chart);//把图表固定到视图，先把widget提升为QChartView

}

void Subwidget::on_pushButton_clicked()
{

    emit back();

}

void Subwidget::on_checkBox_clicked()//温度
{
    if(ui->checkBox->isChecked())
    {
        line_w->show();
    }
    else
    {
        line_w->hide();
    }


}

void Subwidget::on_checkBox_2_clicked()//湿度
{
    if(ui->checkBox_2->isChecked())
    {
        line_s->show();
    }

    else
    {
        line_s->hide();
    }

}

void Subwidget::on_pushButton_2_clicked()//刷新图表
{
    line_w = new QLineSeries();
    line_s = new QLineSeries();
    updateDB();
    initChart();
}
