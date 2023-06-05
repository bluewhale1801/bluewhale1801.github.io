#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogmain.h"
#include <QLabel>

#include <QDebug>
#include <sstream>
#include <iostream>
#include <QTimer>
#include <cstring>
#include <QVector>
#include <QThread>
#include <QTime>

int k , counter = 10000;

QVector<double> a(10001), b(10001), e(10001), f(10001),z(10001);

QVector<double> c , d , g , h;

QByteArray bDataRev;
float vel, posi;
int8_t Inter, Error;


MainWindow::~MainWindow()
{
    delete ui;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("PID Tunning GUI");
    init_window();
}

void MainWindow::configAllButton(bool status)
{
    ui->clear_butt->setEnabled(status);
    ui->run_butt->setEnabled(status);
    ui->port_cb->setDisabled(status);
    ui->refresh_butt->setDisabled(status);
}

void MainWindow:: plotSetting(QCustomPlot  *plot, const char* xLabel, const char * yLabel)
{
    QFont legendFont = font();
    legendFont.setPointSize(8);
    plot->yAxis->setLabel(yLabel);
    plot->xAxis->setLabel(xLabel);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->legend->setVisible(true);
    plot->legend->setFont(legendFont);
    plot->legend->setSelectedFont(legendFont);
    plot->legend->setSelectableParts(QCPLegend::spItems);
}

void MainWindow::plotConfig()
{
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(Qt::GlobalColor::red);
    QPen pen2;
    pen2.setStyle(Qt::SolidLine);
    pen2.setWidth(2);
    pen2.setColor(Qt::GlobalColor::blue);
    QPen pen3;
    pen3.setStyle(Qt::SolidLine);
    pen3.setWidth(2);
    pen3.setColor(Qt::GlobalColor::black);
    //vel plot init
    ui->graph_vel->addGraph();
    ui->graph_vel->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->graph_vel->graph(0)->setPen(pen3);
    ui->graph_vel->graph(0)->setName("Vel");
    plotSetting(ui->graph_vel, "Sample", "Velocity");

    //pos plot init
    ui->graph_pos->addGraph();
    ui->graph_pos->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->graph_pos->graph(0)->setPen(pen);
    ui->graph_pos->graph(0)->setName("Set Point");

    ui->graph_pos->addGraph();
    ui->graph_pos->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->graph_pos->graph(1)->setPen(pen2);
    ui->graph_pos->graph(1)->setName("Position");
    f[0]=0;
    plotSetting(ui->graph_pos, "Sample", "Position");
}

void MainWindow::on_refresh_butt_clicked()
{
    mSerialPorts = QSerialPortInfo::availablePorts();
    ui->port_cb->clear();
    QStringList portList{"Select COM"};
    ui->port_cb->addItems(portList);
    for(const QSerialPortInfo &port: mSerialPorts)
    {
        ui->port_cb->addItem(port.portName(),port.systemLocation());
    }
}

void MainWindow::init_window()
{
    mSerial = new QSerialPort(this);
    configAllButton(false);
    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(5000);
    mSerialScanTimer->start();
    plotConfig();
    mSerialPorts = QSerialPortInfo::availablePorts();
    ui->port_cb->clear();
    QStringList portList{"Select COM"};
    ui->port_cb->addItems(portList);
    for(const QSerialPortInfo &port: mSerialPorts)
    {
        ui->port_cb->addItem(port.portName(),port.systemLocation());
    }
}

void MainWindow::on_connect_butt_clicked()
{    if (ui->connect_butt->text() == "Connect")
    {
        QString serialLoc=ui->port_cb->currentData().toString();
        mSerial->setPortName(serialLoc);
        mSerial->setBaudRate(static_cast<QSerialPort::BaudRate>(115200));
        mSerial->setDataBits(static_cast<QSerialPort::DataBits>(8));
        mSerial->setParity(QSerialPort::NoParity);
        mSerial->setStopBits(QSerialPort::OneStop);
        mSerial->setFlowControl(QSerialPort::NoFlowControl);

        if(mSerial->open(QIODevice::ReadWrite)) {
            showMess("SERIAL: OK",Qt::GlobalColor::blue);
            showMess("Connected",Qt::GlobalColor::green);
            configAllButton(true);
            QObject::connect(mSerial,SIGNAL(readyRead()),this,SLOT(Recievedata()));
            ui->connect_butt->setText("Disconnect");
            ui->connect_butt->setStyleSheet("QPushButton {color: red;}");
        } else {
            showMess("SERIAL: ERROR",Qt::GlobalColor::blue);
        }
//        QObject::connect(mSerial,SIGNAL(readyRead()),this,SLOT(Recievedata()));
//        ui->connect_butt->setText("Disconnect");
//        ui->connect_butt->setStyleSheet("QPushButton {color: red;}");
    }
    else
    {
        if (mSerial->isOpen())
        {
            mSerial->close();
            QString text = "Disconnected";
            ui->message_txt->append(text);
            configAllButton(false);
        }
        ui->connect_butt->setText("Connect");
        ui->connect_butt->setStyleSheet("QPushButton {color: green;}");
    }
}

void MainWindow::Recievedata()
{
    QString strDataRev;
    QByteArray bDataRevx = mSerial -> readAll();
    bDataRev.append(bDataRevx);
    if(bDataRev.size() > 22) {  bDataRev.clear();}
    if(bDataRev.endsWith('\x03') && bDataRev.startsWith('\x02') && (bDataRev.size() == 22) )
    {
        strDataRev = (QString(bDataRev.left(5)));
        strDataRev.append("\n");
        if((strDataRev == "\u0002PTUN\n" )|| (strDataRev == "PTUN\n"))
        {
            uint8_t byte0 = bDataRev[9];
            uint8_t byte1 = bDataRev[10];
            uint8_t byte2 = bDataRev[11];
            uint8_t byte3 = bDataRev[12];
            uint32_t intValuev = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
            vel = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(&intValuev));

            uint8_t byte4 = bDataRev[13];
            uint8_t byte5 = bDataRev[14];
            uint8_t byte6 = bDataRev[15];
            uint8_t byte7 = bDataRev[16];
            uint32_t intValuep = (byte7 << 24) | (byte6 << 16) | (byte5 << 8) | byte4;
            posi = qFromLittleEndian<float>(reinterpret_cast<const uchar*>(&intValuep));

            QString text = "Position: ";
            text.append(QString::number(posi));
            ui->ref_pos->append(text);
            if(k<counter)
            {
                e[k]=(posi);
                z[k]=(vel);
                if(k>1){
                    f[k]=(f[k-1]+1);
                }
                ui->graph_vel->graph(0)->setData(f,z);
                ui->graph_vel->rescaleAxes();
                ui->graph_vel->replot();
                ui->graph_vel->update();

                ui->graph_pos->graph(1)->setData(f,e);
                ui->graph_pos->graph(1)->rescaleAxes();
                ui->graph_pos->replot();
                ui->graph_pos->update();
                k++;
            }
            else if(k==counter)
            {
                QByteArray a ("02 53 54 4F 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03");
                a = QByteArray::fromHex(a);
                mSerial->write(a);
                if(mSerial -> isWritable())
                {
                    QString text = "STOP";
                    ui->message_txt->append(text);
                }
            }
        }
        bDataRev.clear();
    }
}

void MainWindow::showMess(const QString mess, const Qt::GlobalColor colorTxt)
{
    ui->message_txt->setTextColor(colorTxt);
    ui->message_txt->append(mess);
    ui->message_txt->setTextColor(Qt::black);
}

void MainWindow::on_clear_butt_clicked()
{
    for(k=0;k<counter;k++)
    {
        a[k]=0;
        b[k]=0;
        e[k]=0;
        f[k]=0;
        z[k]=0;
    }
    QVector<double> emptx = {0}, empty ={0};
//    ui->graph_vel->graph(1)->setData(emptx,empty);
    ui->graph_vel->graph(0)->setData(emptx,empty);
    ui->graph_vel->rescaleAxes();
    ui->graph_vel->replot();
    ui->graph_vel->update();

    ui->graph_pos->graph(1)->setData(emptx,empty);
    ui->graph_pos->graph(0)->setData(emptx,empty);
    ui->graph_pos->rescaleAxes();
    ui->graph_pos->replot();
    ui->graph_pos->update();

    k=0;
}

void MainWindow::on_run_butt_clicked()
{
    if(ui->run_butt->text()=="Run")
    {
//        float fkp, fki, fkd, fsetPoint;
//        //reading params from Line Edit
//        QString kp = ui->Kp_edit->text();
//        QString ki = ui->Ki_edit->text();
//        QString kd = ui->Kd_edit->text();
//        QString setPoint= ui->SetP_edit->text();

//        fkp=kp.toFloat();
//        QByteArray bKp(reinterpret_cast<const char*>(&fkp), sizeof(fkp));
//        fki=ki.toFloat();
//        QByteArray bKi(reinterpret_cast<const char*>(&fki), sizeof(fki));
//        fkd=kd.toFloat();
//        QByteArray bKd(reinterpret_cast<const char*>(&fkd), sizeof(fkd));
//        fsetPoint=setPoint.toFloat();
//        QByteArray bsetPoint(reinterpret_cast<const char*>(&fsetPoint), sizeof(fsetPoint));
//        QByteArray bytes;
//        QByteArray a ("02 53 50 49 44 00 ");
//        a = QByteArray::fromHex(a);
//        QByteArray b ("03");
//        b=QByteArray::fromHex(b);
//        bytes.append(a+bKp+bKi+bKd+bsetPoint+b);
//        qDebug() << bytes;
//        mSerial->write(bytes);


//        if(mSerial -> isWritable())
//        {
//            showMess("Send succeed\nTurning",Qt::GlobalColor::blue);
//        }
        QByteArray a ("02 50 54 55 4E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03");
        a = QByteArray::fromHex(a);
        mSerial->write(a);

        if(mSerial -> isWritable())
        {
            showMess("Run",Qt::GlobalColor::green);
        }
        //Plot print SetPoint
        QString setPoint= ui->SetP_edit->text();
        float fsetPoint = setPoint.toFloat();
        g={0,10000};
        h={fsetPoint,fsetPoint};
        ui->graph_pos->graph(0)->setData(g,h);

        // change to stop button
        ui->run_butt->setText("Stop");
        ui->run_butt->setStyleSheet("QPushButton {color: red;}");
        ui->clear_butt->setDisabled(true);
        ui->send_butt->setDisabled(true);
        ui->saveGraph_butt->setDisabled(true);
    }
    else
    {
        QByteArray a ("02 53 54 4F 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 ");
        a = QByteArray::fromHex(a);
        mSerial->write(a);
        if(mSerial -> isWritable())
        {
            showMess("Stop",Qt::GlobalColor::darkRed);
        }
        // change to run button
        ui->run_butt->setText("Run");
        ui->run_butt->setStyleSheet("QPushButton {color: green;}");
        ui->clear_butt->setDisabled(false);
        ui->send_butt->setDisabled(false);
        ui->saveGraph_butt->setDisabled(false);
    }
}



void MainWindow::on_send_butt_clicked()
{
    float fkp, fki, fkd, fsetPoint;
    //reading params from Line Edit
    QString kp = ui->Kp_edit->text();
    QString ki = ui->Ki_edit->text();
    QString kd = ui->Kd_edit->text();
    QString setPoint= ui->SetP_edit->text();

    fkp=kp.toFloat();
    QByteArray bKp(reinterpret_cast<const char*>(&fkp), sizeof(fkp));
    fki=ki.toFloat();
    QByteArray bKi(reinterpret_cast<const char*>(&fki), sizeof(fki));
    fkd=kd.toFloat();
    QByteArray bKd(reinterpret_cast<const char*>(&fkd), sizeof(fkd));
    fsetPoint=setPoint.toFloat();
    QByteArray bsetPoint(reinterpret_cast<const char*>(&fsetPoint), sizeof(fsetPoint));
    QByteArray bytes;
    QByteArray a ("02 53 50 49 44 00 ");
    a = QByteArray::fromHex(a);
    QByteArray b ("03");
    b=QByteArray::fromHex(b);
    bytes.append(a+bKp+bKi+bKd+bsetPoint+b);
    qDebug() << bytes;
    mSerial->write(bytes);
    if(mSerial->isWritable())
    {
        showMess("Send succeed",Qt::GlobalColor::blue);
    }
}

void MainWindow::on_Xmessage_butt_clicked()
{
    ui->message_txt->clear();
}

void MainWindow::on_X_ref_pos_butt_clicked()
{
    ui->ref_pos->clear();
}

void MainWindow::on_saveGraph_butt_clicked()
{
    QString fileName = "D:\\Qt\\GUI-PID\\document\\graph.jpg";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui->graph_pos->saveJpg(fileName);
    }
    file.close();
}

