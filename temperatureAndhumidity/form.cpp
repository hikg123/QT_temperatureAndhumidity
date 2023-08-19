#include "form.h"

#include "ui_form.h"
#include<QPushButton>
#include<QSqlQuery>
#include<QDebug>
#include<QTableWidget>
#include<QTableWidgetItem>
#include<QStringList>
#include<QMessageBox>
#include<QSqlError>
#include<QSqlDatabase>
#include<QSqlRecord>


Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    setFixedSize( this->width(),this->height());//窗口定大小
    //this->resize(800,600);
    this->setWindowTitle("历史记录");
    //query.exec("insert into t_ values(0,'2022-03-23 19:40:20',21,68),(0,now(),15,78)");
    //query.exec("delete from t_ where id>11");
    //query.exec("delete from t_ where id=10");
    //query.exec("insert into t_ values(10,'2022-3-24 14:43:52',14,70)");

    connectDB();
    tablewidgetInit();

//    typedef struct TestInfo
//    {
//        QString id;
//        QString time;
//        QString temperature;
//        QString humidity;
//    }testInfo;//类名
}
void Form::connectDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setPort(3306);
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("183068");
    db.setDatabaseName("2");
    bool ok = db.open();
    if(ok)
    {
        QMessageBox::information(this,"infor","success");
    }
    else
    {
        QMessageBox::information(this,"infor","open failed");
        qDebug()<<"error open database because"<<db.lastError().text();
    }
    QSqlQuery query;
}

void Form::tablewidgetInit()//动态读取数据库中的数据
{

    ui->tableWidget->setWindowTitle("温湿度");

    QSqlQuery query;
    query.exec("select * from t_");       //record保存了内容信息

    ui->tableWidget->setAlternatingRowColors(true);     //设置隔行变颜色
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);       //选中整行
    ui->tableWidget->setColumnCount(4);//要先设置列数，再建表
    //表头字体居中
    QStringList headers;
    headers<<tr("编号")<<tr("时间")<<tr("温度")<<tr("湿度");//字符串列表

    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");



    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);


    ui->tableWidget->verticalHeader()->setVisible(false);//不显示默认行号
    //ui->tableWidget->horizontalHeader() ->setVisible(false);//隐藏横向表头
    //动态调整表格大小
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    while(query.next())//行
    {
        QStringList data;
        data<<query.value("id").toString()
           <<query.value("time").toString()
          <<query.value("temperature").toString()
         <<query.value("humidity").toString();

        int rowcount = ui->tableWidget->rowCount();//动态读取数据库中的数据,有多少读多少
        //qDebug()<<rowcount;//0,1,2,3,4,5,6,7,8(若是9条数据，第一次插入数据，当前行数为0，实际上看到的是第一行..
        ui->tableWidget->insertRow(rowcount);
        for(int j =0;j<headers.count();j++)//列
        {
            ui->tableWidget->setItem(rowcount,j,new QTableWidgetItem(data.at(j)));//第一次循环后rowcount从0变为1
            //表格内容居中
            ui->tableWidget->item(rowcount,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        }

    }
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    emit this->back();
}

void Form::on_pushButton_2_clicked()//搜索
{
     selectData();
}
void Form::selectData()
{
    QString searchParam = ui->lineEdit->text().trimmed();
    if(searchParam.isEmpty())
    {
        tableFlash();
        return;
    }
    tableFlash(QString("select *from t_ where temperature like \"%1%2\" or humidity like \"%3%4\"").arg(searchParam,"%",searchParam,"%"));
}
void Form::tableFlash(QString selectSql)
{
    while(ui->tableWidget->rowCount())//清空表,不清除表头，第一次循环后，数据由n->n-1,直到n=0,tablewidget没有数据了,停止循环
    {
        ui->tableWidget->removeRow(0);
    }

//    //先移除表格所有行
//    for(int i =100-1;i>=0;i--)
//    {
//        ui->tableWidget->removeRow(i);
//    }
    QSqlQuery query;
    if(selectSql.isEmpty())
    {
        QString sql ="select *from t_ order by id";
        query.exec(sql);
    }
    else
    {

        query.exec(selectSql);
    }

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->verticalHeader()->setVisible(false);//不显示默认行号
    //动态调整表格大小
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    while(query.next())//行
    {
        QStringList data;//每次进循环都重新开始
        data<<query.value("id").toString()
        <<query.value("time").toString()
        <<query.value("temperature").toString()
        <<query.value("humidity").toString();
        int rowcount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowcount);

        for(int j =0;j<4;j++)//列
        {
            ui->tableWidget->setItem(rowcount,j,new QTableWidgetItem(data.at(j)));
            //表格内容居中
            ui->tableWidget->item(rowcount,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        }

    }

}


void Form::on_pushButton_3_clicked()
{
    //存到F盘根目录下
    QString filename = QFileDialog::getExistingDirectory(this,tr("选择文件夹"),"F:");
    qDebug()<<filename;
    if(filename == "")//对话框点取消
    {
        return;
    }
    QDateTime time = QDateTime::currentDateTime();
    QString date = time.toString("MM.dd");
    filename +=date;
    filename +=".txt";
    if(!filename.isEmpty())
    {
        QFile file(filename);
        bool ok=file.open(QIODevice::WriteOnly);
        //qDebug()<<ok;
        if(ok==true)
        {
            QTextStream out(&file);

            int rowcount=ui->tableWidget->rowCount();
            //qDebug()<<rowcount;//数据条数
            for(int i=0;i<rowcount;i++)//tablewidget里有多少行有数据的才能读出多少行
            {
                QString str;//默认构造空串
                for(int j=0;j<4;j++)
                {
                   str+=ui->tableWidget->item(i,j)->text();
                   str+="\t";
                }
                 str+="\n";
                 out<<str;
            }

            file.close();
            QMessageBox::information(NULL,tr("提示"),tr("保存成功!"));

        }
    }

}

void Form::on_pushButton_4_clicked()//刷新tablewidget
{
    tableFlash();

}
