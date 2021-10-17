#ifndef MULTHREAD_H
#define MULTHREAD_H
#include <QThread>
#include "pcap.h"

class multhread:public QThread
{
    Q_OBJECT
public:
    multhread();
    bool setPointer(pcap_t *pointer); // 打开设备描述符的地址
    void setFlag();
    void resetFlag();
    void run() override;
private:
    pcap_t *pointer; // 打开设备的描述符
    struct pcap_pkthdr *header; // 数据包header
    const u_char *pkt_data; // 数据包数据
    time_t local_time_sec; //
    struct tm local_time; //
    char timeString[16]; // 你懂得
    bool isDone;
};

#endif // MULTHREAD_H
