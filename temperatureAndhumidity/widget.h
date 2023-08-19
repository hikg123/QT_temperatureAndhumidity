#ifndef WIDGET_H
#define WIDGET_H
#include<form.h>
#include<subwidget.h>
#include <QWidget>
#include <QtSerialPort/QSerialPort>//串口
#include <QtSerialPort/QSerialPortInfo>//串口
#include <QComboBox>
#include <QDebug>
#include <QDateTime>
#include<QTimer>
#include<QTime>
#include<QSplineSeries>
#include<QMouseEvent>//鼠标事件





QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void readData();
    void writedata(int tem,int hum);
    void sendCommand();
    //void info_text(QString str);
    int read_info();
    //QString Delimited_Arrary(QByteArray ba);
    void initGraph();
    void updateData();
    void cDB();
    void mouseMoveEvent(QMouseEvent*event);
    void wheelEvent(QWheelEvent *event);


private slots:

    void on_pushButton_clicked();



    void on_pushButton_5_clicked();


    void on_searchSerialBtn_clicked();

    void on_openSerialBtn_clicked();



    void on_startBtn_clicked();

    void on_pauseBtn_clicked();

private:
    Ui::Widget *ui;
    Form f;
    Subwidget *s=new Subwidget;
    QSerialPort* m_serialPort; //串口类
    QTimer *timer;
    QTimer *timer2;
    QByteArray buffer;//字节数组,数据是二进制文件
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QSplineSeries *series_w;
    QSplineSeries *series_s;
    bool serialflag;
    int m_x=0;
    QLabel *m_coordX;
    QLabel *m_coordY;



};
#endif // WIDGET_H
