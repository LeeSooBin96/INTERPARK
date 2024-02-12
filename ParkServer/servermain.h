#ifndef SERVERMAIN_H
#define SERVERMAIN_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtNetwork/QtNetwork>

class QTcpServer;
class QNetworkSession;

QT_BEGIN_NAMESPACE
namespace Ui {
class ServerMain;
}
QT_END_NAMESPACE

class ServerMain : public QMainWindow
{
    Q_OBJECT

public:
    ServerMain(QWidget *parent = nullptr);
    ~ServerMain();

private:
    Ui::ServerMain *ui;
    void initialize();
    QTcpServer *tcpServer;
    // QVector<QTcpSocket*> clientConnection;
    // int cnum=0;
    QTcpSocket *tcpClient;
    QSqlDatabase db;
    QString selectedDate;
    QString selectedVenues;

private slots:
    void newConnection();
    void readMessage();
    //공연 목록에 들어갈 간단정보 전송
    void sendPerformList();
    //검색어에 해당하는 공연 목록 전송
    void sendResultSearch(QString);
    //공연 상세 정보 전송
    void sendDetailOfPerform(QString);
    //공연 일자 유효성 검사 --임시
    void SendPerformDay(QString,QString);
    //공연 좌석 정보 전송
    void SendSeatInfo(QString,QString);
    //계정 등록
    void EnrollAccount(QStringList);
    //로그인 정보 확인
    void CKAccount(QString,QString);
    //아이디 찾기
    void SearchID(QString,QString);
    //비밀번호 찾기
    void SearchPW(QString,QString,QString);
    //아이디 중복 검사
    void CKID(QString);
    //이메일 중복 검사
    void CKEML(QString);
    //휴대폰 중복 검사
    void CKPH(QString);
    //선택 좌석 정보 전송
    void SendSelectedSeatInfo(QString);
    //결제 정보 저장
    void SavePayment(QStringList);
};
#endif // SERVERMAIN_H
