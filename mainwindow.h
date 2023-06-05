
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QList>
#include <QIODevice>
#include <QVector>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include "D:/Qt/GUI-PID/GUI_PID/qcustomplot.h"
#include "uart.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


    void Recievedata();

    void on_connect_butt_clicked();

    void showMess(const QString mess, const Qt::GlobalColor colorTxt);

    void on_clear_butt_clicked();

    void on_run_butt_clicked();

    void on_refresh_butt_clicked();

    void init_window();

    void on_send_butt_clicked();

    void on_Xmessage_butt_clicked();

    void on_X_ref_pos_butt_clicked();

    void on_saveGraph_butt_clicked();

private:
    Ui::MainWindow *ui;

    QSerialPort *mSerial;
    QList <QSerialPortInfo> mSerialPorts;
    QTimer *mSerialScanTimer;
    QString code;

    void configAllButton(bool status);
    void plotSetting(QCustomPlot  *plot, const char* xLabel, const char * yLabel);
    void plotConfig();

    QString tex;
};
#endif // MAINWINDOW_H
