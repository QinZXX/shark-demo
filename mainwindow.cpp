#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QIcon>
#include <QDebug>
#include "multhread.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showNetworkCard();
    multhread *thread = new multhread;
    static bool run_stop_if_run = false; // 放在这里是否恰当？ static可以放在匿名函数里吗？
    connect(ui->actionrun_and_stop, &QAction::triggered, this, [=](){
        run_stop_if_run = !run_stop_if_run;
        if (run_stop_if_run) {
            // begin to capture packects
            int res = capture();
            if ((res != -1) && (pointer)) {
                thread->setPointer(pointer);
                thread->setFlag();
                thread->start();
                ui->actionrun_and_stop->setIcon(QIcon(":/stop.png"));
                // 捕获时不能切换网卡，设置为不可选择combox
                ui->comboBox->setEnabled(false);
            }

        } else {
            // stop capturing packects
            thread->resetFlag();
            thread->quit();
            thread->wait();
            ui->actionrun_and_stop->setIcon(QIcon(":/start.png"));
            ui->comboBox->setEnabled(true); // 可编辑状态
            // 释放pointer
            pcap_close(pointer);
            pointer = nullptr;
        }
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNetworkCard()
{
    int n = pcap_findalldevs(&all_device, errbuf);
    if (-1 ==n) {
        ui->comboBox->addItem("error: " + QString(errbuf));
    } else {
        ui->comboBox->clear();
        ui->comboBox->addItem("pls choose network card!");
        for (device = all_device; device != nullptr;device = device->next) {
            QString device_name = device->name;
            device_name.replace("\\Device\\","");
            QString des = device->description;
            QString item = device_name + des;
            ui->comboBox->addItem(item);
        }
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    int i = 0;
    if (index != 0) {
        for (device = all_device;i < index - 1;device = device->next) {
            ++i;
        }
    }
    return;
}

int MainWindow::capture()
{
    if (device) {
        pointer = pcap_open_live(device->name, 65536, 1, 1000, errbuf);
    } else {
        qDebug() << "current variable device is nullptr!";
        return -1;
    }
    if (!pointer) {
        qDebug() << "open device(" << device->name << ") failed!";
        pcap_freealldevs(all_device);
        device = nullptr;
        return -1;
    } else {
        qDebug() << "now only capture the packeckts on IEEE802.3(DLT_EN10MB)!";
        if (pcap_datalink(pointer) != DLT_EN10MB) { // 只抓IEEE802.3协议
            qDebug() << "The packects on device(" << device->name << ") is not on IEEE802.3(DLT_EN10MB)!";
            pcap_close(pointer);
            pcap_freealldevs(all_device);
            device = nullptr;
            pointer = nullptr;
            return -1;
        }
        statusBar()->showMessage(device->name);

    }
    return 0;
}
