#include "servermain.h"
#include "ui_servermain.h"

ServerMain::ServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMain)
{
    ui->setupUi(this);

    //서버 소켓 초기화
    QHostAddress hostAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses(); //주소 목록을 가져옴
    // localhost(127.0.0.1) 가 아닌 것을 사용
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {//로컬주소가 아닌 주소가 아니고 ipv4주소로 변환 되었을때
            hostAddress = ipAddressesList.at(i); //호스트주소로 저장
            break;
        }
    }

    if (hostAddress.toString().isEmpty())//로컬 주소밖에 없으면
        hostAddress = QHostAddress(QHostAddress::LocalHost); //로컬 주소로 저장

    tcpServer = new QTcpServer(this);//서버 소켓(객체) 생성
    if (!tcpServer->listen(hostAddress, 25000)) { //서버 오픈 --연결 대기
        qDebug()<<"서버 오픈 실패!";
        close();
        return;
    }
}

ServerMain::~ServerMain()
{
    delete ui;
}
