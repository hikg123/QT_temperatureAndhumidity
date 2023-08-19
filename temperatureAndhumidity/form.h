#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QFileDialog>
#include <QFile>
#include<QTextStream>
#include<QDateTime>
#include<QTimer>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void tableFlash(QString selectSql = QString());
    void tablewidgetInit();
    void connectDB();
signals:
    void back();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Form *ui;
    void selectData();

};

#endif // FORM_H
