#include "widget.h"
#include "ui_widget.h"
#include"form.h"
#include<QPushButton>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QDebug>
#include<QMessageBox>
#include<QSqlError>
#include<QRadioButton>
#include<QGroupBox>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

//    QGroupBox *groupBox_3 = new QGroupBox(this);
//    QRadioButton *radioButton =new QRadioButton("温度",groupBox_3);//指定其父对象是分组控件
//    QRadioButton *radioButton_2 =new QRadioButton("湿度",groupBox_3);
    setFixedSize( this->width(),this->height());//窗口定大小
    //this->resize(800,600);
    this->setWindowTitle("温湿度监控系统");
    ui->lcdNumber->setDigitCount(4);    //显示位数
    ui->lcdNumber->setMode(QLCDNumber::Dec);    //十进制
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);       //显示方式

    ui->lcdNumber_2->setDigitCount(4);    //显示位数
    ui->lcdNumber_2->setMode(QLCDNumber::Dec);    //十进制
    ui->lcdNumber_2->setSegmentStyle(QLCDNumber::Flat);       //显示方式


    ui->comboBox_2->addItem("1200");
    ui->comboBox_2->addItem("2400");
    ui->comboBox_2->addItem("4800");
    ui->comboBox_2->addItem("9600");
    ui->comboBox_2->addItem("115200");
    m_serialPort = new QSerialPort;
    serialflag = 0;
    timer = new QTimer;
    timer->start(1000);//计时1s
    connect(timer,&QTimer::timeout,this,&Widget::sendCommand);
    //connect(ui->getData,&QPushButton::clicked,this,&Widget::sendCommand);
    connect(m_serialPort,&QSerialPort::readyRead,this,&Widget::read_info);//接受反馈数据
//    struct Data{
//        int tem;
//        int hum;
//    };
    series_w =new QSplineSeries(this);
    series_s=new QSplineSeries(this);
//    this->setMouseTracking(true);//可以追踪到鼠标的轨迹，鼠标放到点上就显示出坐标
//    m_coordX=new QLabel;
//    m_coordY=new QLabel;

    timer2 = new QTimer;
    connect(timer2,&QTimer::timeout,this,&Widget::updateData);
    initGraph();

//    disconnect(timer2,&QTimer::timeout,this,&Widget::updateData);



//监测子窗口的回退信号
connect(&f,&Form::back,[=](){

    f.hide();
    this->show();
});
connect(s,&Subwidget::back,[=](){
    s->hide();
    this->show();
});
}

Widget::~Widget()
{
    delete ui;
    delete m_serialPort;
}


void Widget::initGraph()//绘实时温湿度图
{

        QPen blue;
        blue.setWidth(2);//线条宽度
        series_w->setPen(blue);
        QPen red;
        red.setWidth(2);
        series_s->setPen(red);


        axisX = new  QValueAxis();
        axisY = new  QValueAxis();
        axisX->setTitleText("time(1s)");
        axisX->setRange(0,30);
        axisX->setTickCount(7);//大刻度线有7条，把30分成了6份
        axisX->setMinorTickCount(4);//2个大刻度线之间有4条小刻度线，分成了5份
        axisX->setGridLineVisible(true);//设置是否显示网格线
        axisY->setRange(0,90);
        axisY->setTitleText("value");
        chart=new QChart();

        series_w->setName("temperature");
        series_s->setName("humidity");
        series_s->setPointsVisible(true);//线由点构成
        series_w->setPointsVisible(true);
        series_s->setColor(QColor(255,0,0));
        series_w->setColor(QColor(22,33,250));

        chart->addSeries(series_w);//向图表中添加线
        chart->addSeries(series_s);
        chart->createDefaultAxes();
        chart->setTitle("温湿度实时变化");
        //chart->setTheme(QChart::ChartThemeQt);
        chart->legend()->setVisible(true);
        chart->setTitleFont(QFont("微软雅黑"));

        chart->setAxisX(axisX,series_w);//添加坐标轴,必须再addseries后调用
        chart->setAxisY(axisY,series_w);
        chart->setAxisX(axisX,series_s);
        chart->setAxisY(axisY,series_s);

        chart->setAnimationOptions(QChart::AllAnimations);
        ui->widget->setChart(chart);//把图表固定到视图，先把widget提升为QChartView




}

void Widget::on_pushButton_clicked()
{

    f.show();
    this->hide();
}


void Widget::on_pushButton_5_clicked()
{

    s->show();
    this->hide();


}

void Widget::on_searchSerialBtn_clicked()
{
    ui->comboBox->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);   //设置串口
        if(serial.open(QIODevice::ReadWrite))
        {
        ui->comboBox->addItem(info.portName());
        serial.close();
        }
    }
}

void Widget::on_openSerialBtn_clicked()//打开了串口，控件时不可调的
{
    if(ui->openSerialBtn->text()==tr("打开串口"))//点击按钮之前，串口已关，下面设置串口信息
        {

            m_serialPort->setPortName(ui->comboBox->currentText());

            switch(ui->comboBox_2->currentIndex())
            {
                case 0:
                m_serialPort->setBaudRate(QSerialPort::Baud1200);break;
                case 1:
                    m_serialPort->setBaudRate(QSerialPort::Baud2400);break;
                case 2:
                    m_serialPort->setBaudRate(QSerialPort::Baud4800);break;
                case 3:
                    m_serialPort->setBaudRate(QSerialPort::Baud9600);break;
                case 4:
                    m_serialPort->setBaudRate(QSerialPort::Baud115200);break;
                default:
                    break;
            }
            m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
            m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
            m_serialPort->setParity(QSerialPort::NoParity);	//校验位
            m_serialPort->setStopBits(QSerialPort::TwoStop); //二位停止位
            if(m_serialPort->open(QIODevice::ReadWrite)==false)//串口是关闭的(没有扫描就打开串口）
                    {
                        QMessageBox::warning(NULL , "提示", "串口打开失败！");
                        return;
                    }
            ui->comboBox->setEnabled(false);
            ui->comboBox_2->setEnabled(false);
            ui->openSerialBtn->setText(tr("关闭串口"));
            serialflag = 1;//串口打开


        }
    else//点击按钮前，现在是串口是开着的，点了槽函数就关闭串口
    {
        disconnect(timer2,&QTimer::timeout,this,&Widget::updateData);//停止实时更新曲线
        m_serialPort->clear();
        m_serialPort->close();//先关串口
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->openSerialBtn->setText(tr("打开串口"));
        ui->lcdNumber->display(0);
        ui->lcdNumber_2->display(0);
        serialflag = 0;
        //timer->stop();//停止定时发送

    }
}

void Widget::writedata(int tem,int hum)
{
    QSqlQuery query;
//    query.exec("create table t(id int primary key auto_increment,time datetime, temperature int, humidity int);");
    QString sqlquery = QString("insert into t_ values(0,now(),'%1','%2')").arg(tem).arg(hum);//插入一个数据
    query.exec(sqlquery);
}

void Widget::sendCommand()
{
    QByteArray sendByte;//协议根据下位机来定
       sendByte.resize(8);
       sendByte[0]=0Xcc;//帧头
       sendByte[1]=0X01;//从站地址
       sendByte[2]=0X0d;//功能码
       sendByte[3]=0X01;//数据个数
       sendByte[4]=0X00;//数据
       sendByte[5]=0X00;//校验，根据数据，由CRC算法自动生成
       sendByte[6]=0X00;//校验
       sendByte[7]=0Xcd;//帧尾

       qint64 ref;
       ref = m_serialPort->write(sendByte);
      //把反馈回来的ref转化为string
//      QString B=tr("%1").arg(ref);
//      info_text("已成功发送"+B+"个字节");//8B
//      info_text("发送数据为:"+Delimited_Arrary(sendByte.toHex()));

}

////显示数据
//void Widget::info_text(QString str)
//{
//    QDateTime time;
//    ui->textBrowser->append(time.currentDateTime().toString("yyyy-MM-dd::hh:mm:ss  ")+str);
//}
//读取反馈回来的数据
int Widget::read_info()
{

    QByteArray ba;
    ba.resize(1024);
    ba=m_serialPort->readAll();
    //qDebug()<<ba.toHex().mid(8,2);//转化成16进制的字符串，无空格
    QString str1=ba.toHex().mid(8,2);
    QString str2=ba.toHex().mid(24,2);
    int tem=str1.toInt(0,16);
    int hum=str2.toInt(0,16);
    ui->lcdNumber->display(tem);
    ui->lcdNumber_2->display(hum);
    writedata(tem,hum);//写数据进数据库
    //ba.toHex()这一步很关键，没有这一步显示的是乱码
//    info_text("接受数据为:"+Delimited_Arrary(ba.toHex()));
    ba.clear();
    return tem;

}
////把字节数组转化为字符串并且有空格隔开
//QString Widget::Delimited_Arrary(QByteArray ba)
//{
//    QString ba_str;
//    for (int i=0;i<ba.size();i++ )
//    {
//        ba_str+=ba[i];
//        ba_str+=ba[++i];
//        ba_str+=" ";
//    }
//    return ba_str;
//}

void Widget::on_startBtn_clicked()
{
    timer2->start(1000);//计时
    ui->pauseBtn->setEnabled(true);
    ui->startBtn->setEnabled(false);
}

void Widget::on_pauseBtn_clicked()
{
    timer2->stop();
    ui->pauseBtn->setEnabled(false);
    ui->startBtn->setEnabled(true);
}
void Widget::updateData()
{
    ui->pauseBtn->setEnabled(true);
    ui->startBtn->setEnabled(false);
    QSqlQuery query;;
    query.exec("select temperature,humidity from t_");


    if(query.last())//指向数据库中最后一条数据
    {
        static int id = 0;//保证每次启动程序后实时曲线的横坐标为0开始，静态值保证每次进函数后id从上一次调用后的值自增下去
        int tem = query.value("temperature").toInt();
        //int id = query.value("id").toInt();
        int hum = query.value("humidity").toInt();

        series_w->append(id,tem);
        series_s->append(id,hum);
        id++;



    }
    m_x++;
    //qDebug()<<m_x;
    if(m_x>30)

        chart->scroll(20,0);//一次向右移动坐标轴,20是窗口像素大小qreal，即chart->plotArea().width()/(axisX->tickCount()*2);


}
void Widget::mouseMoveEvent(QMouseEvent*event)
{
    m_coordX->setText(QString("X: %1").arg(chart->mapToValue(event->pos()).x()));
    m_coordY->setText(QString("Y: %1").arg(chart->mapToValue(event->pos()).y()));
    qDebug()<<chart->mapToValue(event->pos()).x();
    QWidget::mouseMoveEvent(event);
}
void Widget::wheelEvent(QWheelEvent *event)
{
    //滚轮事件
    if(event->angleDelta().y()>0)
    {
        qDebug()<<"鼠标滚轮事件：向前";
        chart->zoom(1.1);//大于1是放大，小于1是缩小

    }
    if(event->angleDelta().y()<0)
    {
        qDebug()<<"鼠标滚轮事件：向后";
        chart->zoom(0.9);//大于1是放大，小于1是缩小
    }
}
