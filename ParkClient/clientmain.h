#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QMainWindow>
#include <QTcpSocket>

#include "chooseseat.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ClientMain;
}
QT_END_NAMESPACE

class ClientMain : public QMainWindow
{
    Q_OBJECT

public:
    ClientMain(QWidget *parent = nullptr);
    ~ClientMain();
    //공연 정보 받아 메인 리스트에 출력
    void UpdatePerformInfo(QStringList);
    //공연 상세 정보 받아 상세 페이지 출력
    void PrintDetailOfPerform(QStringList);
    //로그인 진행
    void ProgressLogin(QString);
    //찾은 아이디 출력
    void ShowSID(QString);
    //찾은 비밀번호 출력
    void ShowSPW(QString);



private:
    Ui::ClientMain *ui;
    void initialize();
    QTcpSocket *tcpSocket;
    QVector<int> pNumList;
    bool loginFlag=false;
    int selectedPNum;
    ChooseSeat* bookseat;
    QVector<int> sNumList;
    QString Uid;

private slots:
    //통신 관련
    void connectServer();
    void readMessage();
    void disconnected();
    //로그인 화면 이동
    void GotoLogin();
    //회원가입 페이지 이동
    void GotoJoin();
    //선택한 좌석 정보 보여주기
    void ShowSelectedSeat();
    //결제 정보 전송
    void SendPayInfo();

    void on_Jid_textChanged(const QString &arg1);
    void on_Jpw1_textChanged(const QString &arg1);
    void on_Jpw2_textChanged(const QString &arg1);
    void on_Jemail_textChanged(const QString &arg1);
    void on_BtnEnroll_clicked();
    void BtnLogoClicked();
    void BtnFindClicked();
    void on_LWPerform_clicked(const QModelIndex &index);
    void on_BtnLogIN_clicked();
    void on_BtnSID_clicked();
    void on_BtnSearchID_clicked();
    void on_BtnSearchPW_clicked();
    void on_BtnSPW_clicked();

    void on_Jid_editingFinished();
    void on_showPW1_clicked();
    void on_showPW2_clicked();
    void on_Jemail_editingFinished();
    void on_BtnATphone_clicked();

    void on_BtnBooking_clicked();
};
#endif // CLIENTMAIN_H
