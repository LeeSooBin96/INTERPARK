#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include "servermain.h"
#include "ui_servermain.h"

ServerMain::ServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMain)
{
    ui->setupUi(this);

    initialize();
}

ServerMain::~ServerMain()
{
    delete ui;
}

void ServerMain::initialize()
{
    QHostAddress hostAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // localhost(127.0.0.1) 가 아닌 것을 사용
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            hostAddress = ipAddressesList.at(i);
            break;
        }
    }

    if (hostAddress.toString().isEmpty())
        hostAddress = QHostAddress(QHostAddress::LocalHost);

    tcpServer = new QTcpServer(this);//서버 소켓(객체) 생성
    if (!tcpServer->listen (hostAddress, 25480)) { //서버 오픈 --연결 대기
        qDebug()<<"서버 오픈 실패!";
        close();
        return;
    }
    else
    {
        ui->noticeConnection->setText("서버 오픈... 대기중...");
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}
//클라이언트 연결 들어오면
void ServerMain::newConnection()
{
    tcpClient = tcpServer->nextPendingConnection();
    connect(tcpClient, SIGNAL(disconnected()),
            tcpClient, SLOT(deleteLater()));
    //클라이언트 접속 시간 확인
    QString currTime = QTime::currentTime().toString("hh시 mm분 ss초");
    QString text = QString("클라이언트 접속 (%1)").arg(currTime);
    ui->noticeConnection->append(text);
    //클라이언트에게 메시지 전송
    // QByteArray message = QByteArray("안녕 한글 테스트해볼게 ~ 숫자랑 123 영어도 asdf 섞어서! ");
    // clientConnection->write(message);
    // clientConnection->disconnectFromHost();
    sendPerformList();
    // connect(tcpClient, SIGNAL(readyRead()),
    //         this,      SLOT(readMessage()));
}
void ServerMain::readMessage()
{
    QString tmp;
    if(tcpClient->bytesAvailable() >= 0)
    {
        QByteArray readData = tcpClient->readAll();
        tmp=QString::fromStdU16String(QString::fromUtf8(readData).toStdU16String());
    }
    // ui->noticeConnection->append(tmp);
    if(tmp.split('\n')[0]=="Pay")
    {
        // qDebug()<<tmp.split('\n');
        SavePayment(tmp.split('\n'));
    }
    if(tmp.split('\n').size()==5)
    {
        //회원등록
        EnrollAccount(tmp.split('\n'));
    }
    else if(tmp.split('\n').size()==1)
    {
        if(tmp.split('\n')[0]=="p")
        {
            //공연목록 새로고침
            sendPerformList();
        }
    }
    else if(tmp.split('\n').size()==2)
    {
        if(tmp.split('\n')[0]=="S")
        {
            sendResultSearch(tmp.split('\n')[1]);
        }
        else if(tmp.split('\n')[0]=="D")
        {
            sendDetailOfPerform(tmp.split('\n')[1]);
        }
        else if(tmp.split('\n')[0]=="EI")
        {
            CKID(tmp.split('\n')[1]);
        }
        else if(tmp.split('\n')[0]=="EEML")
        {
            CKEML(tmp.split('\n')[1]);
        }
        else if(tmp.split('\n')[0]=="EPH")
        {
            CKPH(tmp.split('\n')[1]);
        }
        else if(tmp.split('\n')[0]=="SN")
        {
            SendSelectedSeatInfo(tmp.split('\n')[1]);
        }

    }
    else if(tmp.split('\n').size()==3)
    {
        if(tmp.split('\n')[0]=="L")
        {
            CKAccount(tmp.split('\n')[1],tmp.split('\n')[2]);
        }
        else if(tmp.split('\n')[0]=="SI")
        {
            SearchID(tmp.split('\n')[1],tmp.split('\n')[2]);
        }
        else if(tmp.split('\n')[0]=="B")
        {
            SendPerformDay(tmp.split('\n')[1],tmp.split('\n')[2]);
        }
        else if(tmp.split('\n')[0]=="V")
        {
            SendSeatInfo(tmp.split('\n')[1],tmp.split('\n')[2]);
        }
    }
    else if(tmp.split('\n').size()==4)
    {
        if(tmp.split('\n')[0]=="SP")
        {
            SearchPW(tmp.split('\n')[1],tmp.split('\n')[2],tmp.split('\n')[3]);
        }
    }
    // QByteArray readData = tcpSocket->read(1);
    // qDebug()<<readData.toInt();
    // readData = tcpSocket->readAll();
    // qDebug()<<QString::fromUtf8(readData).toStdU16String();
}
//공연 목록에 들어갈 간단 정보 전송
void ServerMain::sendPerformList()
{
    //공연번호, 제목, 장소, 기간 전송해야함(4개)
    //일단은 DB에서 해당정보 받아오기
    QSqlQuery qry;
    quint64 rnum=rand()%3+3;
    qry.prepare("SELECT PNUM_PK,PTITLE,V.VNAME,PDURATE FROM PERFORM_TB AS P JOIN VENUES_TB AS V ON P.VNUM =V.VNUM_PK ORDER BY RANDOM() LIMIT "+QString::number(rnum));
    qry.exec();
    QByteArray message;
    while(qry.next())
    {
        // qDebug()<<qry.value(0).toInt();//공연번호
        // qDebug()<<qry.value(1).toByteArray();//공연제목
        // qDebug()<<qry.value(2).toByteArray();//공연장소
        // qDebug()<<qry.value(3).toByteArray();//공연기간
        //어떻게 보낼까...
        message.push_back(qry.value(0).toByteArray());
        message.push_back(QByteArray("@"));
        message.push_back(qry.value(1).toByteArray());
        message.push_back(QByteArray("@"));
        message.push_back(qry.value(2).toByteArray());
        message.push_back(QByteArray("@"));
        message.push_back(qry.value(3).toByteArray());
        tcpClient->write(message);
        tcpClient->waitForBytesWritten(1000);
        tcpClient->waitForReadyRead(3000);
        tcpClient->readAll();
        message.clear();
    }
    // clientConnection[0]->waitForBytesWritten();
    // clientConnection[0]->disconnectFromHost();
    connect(tcpClient, SIGNAL(readyRead()),
            this,      SLOT(readMessage()));
}
//검색어에 해당하는 공연 목록 전송
void ServerMain::sendResultSearch(QString keyword)
{
    QSqlQuery qry;
    qry.prepare("SELECT PNUM_PK,PTITLE,V.VNAME,PDURATE,PCAST FROM PERFORM_TB AS P JOIN VENUES_TB AS V ON P.VNUM =V.VNUM_PK");
    qry.exec();
    QByteArray message;
    while(qry.next())
    {
        bool ck=false;
        for(int i=1;i<5;i++)
        {
            if(qry.value(i).toString().indexOf(keyword)>=0)
            {
                ck=true;
                break;
            }
        }
        if(ck)
        {
            message.push_back(qry.value(0).toByteArray());
            message.push_back(QByteArray("@"));
            message.push_back(qry.value(1).toByteArray());
            message.push_back(QByteArray("@"));
            message.push_back(qry.value(2).toByteArray());
            message.push_back(QByteArray("@"));
            message.push_back(qry.value(3).toByteArray());
            tcpClient->write(message);
            tcpClient->waitForBytesWritten(1000);
            tcpClient->waitForReadyRead(3000);
            tcpClient->readAll();
            message.clear();
        }
    }
}
//공연 상세 정보 전송
void ServerMain::sendDetailOfPerform(QString pnum)
{
    QSqlQuery qry;
    qry.prepare("SELECT PTITLE,V.VNAME,PDURATE,PTIME,PAGE,P_R_PRICE,PCAST,PAVAIL,PSCHE,PNOTICE FROM PERFORM_TB AS P JOIN VENUES_TB AS V ON P.VNUM =V.VNUM_PK WHERE PNUM_PK="+pnum);
    qry.exec();
    QByteArray message;
    qry.next();
    for(int i=0;i<9;i++)
    {
        message.push_back(qry.value(i).toByteArray());
        message.push_back(QByteArray("@"));
    }
    message.push_back(qry.value(9).toByteArray());

    tcpClient->write(message);
}
//공연 일자에 해당하는 정보 전송
void ServerMain::SendPerformDay(QString pnum,QString date)
{

    QSqlQueryModel model;
    model.setQuery("SELECT * FROM PSEAT_"+pnum);
    // qDebug()<<date;
    bool validDate=false;
    for(int i=2;i<model.columnCount();i++)
    {
        // qDebug()<<model.headerData(i,Qt::Horizontal).toString();
        if(model.headerData(i,Qt::Horizontal).toString()==date)
        {
            validDate=true;
            break;
        }
    }
    if(validDate)//예매 가능한 날짜
    {
        //전송할 내용 SD+공연제목+공연장소+좌석수(층,열별)+등급별잔여좌석수
        //우선 공연, 장소, 장소 번호 전송
        QByteArray message="SD@";
        QSqlQuery qry;
        qry.prepare("SELECT PTITLE,VNAME,VNUM FROM PERFORM_TB JOIN VENUES_TB ON VNUM =VNUM_PK WHERE PNUM_PK ="+pnum);
        qry.exec();
        qry.next();
        message.append(qry.value(0).toByteArray()+"@");
        message.append(qry.value(1).toByteArray()+"@");
        message.append(qry.value(2).toByteArray());
        tcpClient->write(message);
        selectedDate=date;
    }
    else
    {
        //예매불가능
        tcpClient->write("FD");
    }
}
//공연 좌석 정보 전송
void ServerMain::SendSeatInfo(QString vnum,QString pnum)
{
    QByteArray message="US@";
    //우선 등급별 좌석수 부터 보내보자
    QSqlQuery qry;
    qry.prepare("SELECT RNUM,COUNT() FROM PSEAT_"+pnum+" WHERE \""+selectedDate+"\" = 'F'  GROUP BY RNUM HAVING RNUM>0");
    qry.exec();
    unsigned int rowCount=0;
    QString tmp;
    while(qry.next())
    {
        tmp.append(qry.value(0).toByteArray()+"@");
        tmp.append(qry.value(1).toByteArray()+"@");
        rowCount++;
    }
    message.append(QString::number(rowCount).toUtf8()+"@"+tmp.toUtf8());
    //좌석 정보 보내기
    QSqlQuery state;
    state.prepare("SELECT RNUM,\""+selectedDate+"\" FROM PSEAT_"+pnum);
    state.exec();
    QSqlQueryModel model;
    unsigned int floor=1;
    while(true)
    {
        model.setQuery("SELECT COUNT() FROM VSEAT_"+vnum+" v WHERE SFLOOR ="+QString::number(floor)+" GROUP BY SCOL");
        if(model.rowCount()==0) break;
        message.append("F"+QString::number(floor).toUtf8()+"@");
        for(int i=0;i<model.rowCount();i++)
        {
            qDebug()<<model.record(i).value(0).toInt(); //열좌석수
            message.append(model.record(i).value(0).toByteArray()+"@");
            for(int j=0;j<model.record(i).value(0).toInt();j++)
            {
                state.next();
                message.append(state.value(0).toByteArray()+"@");
                message.append(state.value(1).toByteArray()+"@");
            }
        }
        floor++;
    }
    
    selectedVenues=vnum;
    tcpClient->write(message);
}
//좌석 정보 전송
void ServerMain::SendSelectedSeatInfo(QString snum)
{
    //필요한것 공연장 번호!
    QSqlQuery qry;
    qry.prepare("SELECT * FROM VSEAT_"+selectedVenues+" WHERE SNUM_PK="+snum);
    qry.exec(); qry.next();
    tcpClient->write("SN@"+qry.value(1).toByteArray()+"@"+qry.value(2).toByteArray()+"@"+qry.value(3).toByteArray()+"@"+qry.value(4).toByteArray());
}
//결제 정보 저장
void ServerMain::SavePayment(QStringList plist)
{
    //좌석 정보 업데이트
    for(int i=0;i<plist[3].toInt();i++)
    {
        QSqlQuery qry;
        qry.prepare("UPDATE PSEAT_"+plist[2]+" SET \""+selectedDate+"\"='T' WHERE SNUM_PK="+plist[4+i]);
        qry.exec();
        ui->noticeConnection->append(plist[2]+"번 공연 "+plist[4+i]+"번 좌석이 결제 되었습니다.");
    }
    //고객 예매 정보에 저장--나중에
    // QSqlQuery qry;
    // qry.prepare("INSERT INTO USER_TICKET_TB(")
}
//계정 등록
void ServerMain::EnrollAccount(QStringList ilist)
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO USER_TB(UID,UPW,UNAME,UEMAIL,UPHONE) VALUES('"+ilist[0]+"','"+ilist[1]+"','"+ilist[2]+"','"+ilist[3]+"','"+ilist[4]+"')");
    if(qry.exec())
    {
        ui->noticeConnection->append("회원정보 등록 완료");
    }
}
//로그인 정보 확인
void ServerMain::CKAccount(QString id,QString pw)
{
    QSqlQuery qry;
    qry.prepare("SELECT UNUM_PK FROM USER_TB WHERE UID='"+id+"' AND UPW='"+pw+"'");
    qry.exec();
    if(qry.next())
    {//로그인 성공
        ui->noticeConnection->append("로그인한 회원:"+ id);
        tcpClient->write("L@S");
    }
    else
    {//로그인 실패
        tcpClient->write("L@F");
    }

}
//아이디 찾기
void ServerMain::SearchID(QString name,QString emlOrphone)
{
    QSqlQuery qry;
    qry.prepare("SELECT UID FROM USER_TB WHERE UNAME='"+name+"' AND (UEMAIL='"+emlOrphone+"' OR UPHONE='"+emlOrphone+"')");
    qry.exec();
    if(qry.next())
    {
        ui->noticeConnection->append("아이디 찾기 조회:"+name);
        tcpClient->write("SI@"+qry.value(0).toByteArray());
    }
    else
    {
        tcpClient->write("SI@F");
    }
}
//비밀번호 찾기
void ServerMain::SearchPW(QString name,QString id,QString emlOrphone)
{
    QSqlQuery qry;
    qry.prepare("SELECT UPW FROM USER_TB WHERE UID='"+id+"' AND UNAME='"+name+"' AND (UEMAIL='"+emlOrphone+"' OR UPHONE='"+emlOrphone+"')");
    qry.exec();
    if(qry.next())
    {
        ui->noticeConnection->append("비밀번호 찾기 조회:"+name);
        tcpClient->write("SP@"+qry.value(0).toByteArray());
    }
    else
    {
        tcpClient->write("SP@F");
    }
}
//아이디 중복 검사
void ServerMain::CKID(QString id)
{
    QSqlQuery qry;
    qry.prepare("SELECT UNUM_PK FROM USER_TB WHERE UID='"+id+"'");
    qry.exec();
    if(qry.next())
    {
        tcpClient->write("DI");
    }
}
//이메일 중복 검사
void ServerMain::CKEML(QString eml)
{
    QSqlQuery qry;
    qry.prepare("SELECT UNUM_PK FROM USER_TB WHERE UEMAIL='"+eml+"'");
    qry.exec();
    if(qry.next())
    {
        tcpClient->write("DE");
    }
}
//휴대폰 중복 검사
void ServerMain::CKPH(QString ph)
{
    QSqlQuery qry;
    qry.prepare("SELECT UNUM_PK FROM USER_TB WHERE UPHONE='"+ph+"'");
    qry.exec();
    if(qry.next())
    {
        tcpClient->write("DP");
    }
    else
    {
        tcpClient->write("CK");
    }
}
