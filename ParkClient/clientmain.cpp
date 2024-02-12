#include <QListWidgetItem>
#include <QMessageBox>


#include "clientmain.h"
#include "ui_clientmain.h"

ClientMain::ClientMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMain)
{
    ui->setupUi(this);
    ui->mainStack->setCurrentIndex(0);
    // ui->LWPerform->setViewMode(QListWidget::IconMode);
    initialize();
}

ClientMain::~ClientMain()
{
    delete ui;
}
//소켓 초기화
void ClientMain::initialize()
{
    tcpSocket = new QTcpSocket(this);
    //서버에 연결
    connectServer();
    // readMessage();
    // connect(tcpSocket, SIGNAL(readyRead()),
    //         this,      SLOT(readMessage()));
    connect(tcpSocket, SIGNAL(disconnected()),
            this,      SLOT(disconnected()));
}

void ClientMain::connectServer()
{
    QString serverip   = "192.168.0.14";

    QHostAddress serverAddress(serverip);
    tcpSocket->connectToHost(serverAddress, 25480);
    readMessage();
}

void ClientMain::readMessage()
{
    QString tmp;
    if(tcpSocket->bytesAvailable() >= 0)
    {
        QByteArray readData = tcpSocket->readAll();
        // qDebug()<<QString::fromUtf8(readData).toStdU16String();
        tmp=QString::fromStdU16String(QString::fromUtf8(readData).toStdU16String());
    }
    // qDebug()<<tmp.split('@');
    if(tmp.split('@')[0]=="US")
    {
        bookseat->UpdateSeatInfo(tmp.split('@'));
        return;
    }
    else if(tmp.split('@')[0]=="SN")
    {
        bookseat->PrintSelectedSeat(tmp.split('@'));
        return;
    }
    if(tmp.split('@').size()==4)
    {
        if(tmp.split('@')[0]=="SD")
        {
            sNumList.clear();
            //예매 페이지에 공연 정보 출력 및 공연장 번호 서버에 전송(->좌석 현황 정보 얻기위함=>좌석 배치도 업데이트 해야함)
            bookseat=new ChooseSeat(this);
            bookseat->show();
            bookseat->PrintPerformInfo(tmp.split('@')[1],tmp.split('@')[2],ui->calendarWidget->selectedDate());
            tcpSocket->write("V\n"+tmp.split('@')[3].toUtf8()+"\n"+QString::number(selectedPNum).toUtf8());
            return;
        }
        UpdatePerformInfo(tmp.split('@'));
        tcpSocket->write("y");
        // readMessage();
        return;
    }
    else if(tmp.split('@').size()==10)
    {
        PrintDetailOfPerform(tmp.split('@'));
        return;
    }
    else if(tmp.split('@').size()==2)
    {
        if(tmp.split('@')[0]=="L")
        {
            ProgressLogin(tmp.split('@')[1]);
        }
        else if(tmp.split('@')[0]=="SI")
        {
            ShowSID(tmp.split('@')[1]);
        }
        else if(tmp.split('@')[0]=="SP")
        {
            ShowSPW(tmp.split('@')[1]);
        }
        return;
    }
    else if(tmp.split('@').size()==1&&tmp.split('@')[0]!="")
    {
        if(tmp.split('@')[0]=="DI")
        {
            ui->noticID->setText("중복된 아이디입니다.");
        }
        else if(tmp.split('@')[0]=="DE")
        {
            ui->noticEML->setText("중복된 이메일입니다.");
        }
        else if(tmp.split('@')[0]=="DP")
        {
            QMessageBox::critical(this,"경고","중복된 휴대폰 번호입니다.",QMessageBox::Ok);
        }
        else if(tmp.split('@')[0]=="CK")
        {
            QMessageBox::information(this,"알림","인증되었습니다.",QMessageBox::Ok);
            ui->BtnEnroll->setEnabled(true);
        }
        else if(tmp.split('@')[0]=="FD")
        {
            QMessageBox::critical(this,"경고","현재 예매가 불가능한 회차입니다.",QMessageBox::Ok);
        }
        return;
    }
    // QByteArray readData = tcpSocket->read(1);
    // qDebug()<<readData.toInt();
    // readData = tcpSocket->readAll();
    // qDebug()<<QString::fromUtf8(readData).toStdU16String();

    connect(tcpSocket, SIGNAL(readyRead()),
            this,      SLOT(readMessage()));
}

void ClientMain::disconnected()
{
    qDebug() << Q_FUNC_INFO << "서버 접속 종료.";
}
//공연 정보 받아 메인 리스트에 출력
void ClientMain::UpdatePerformInfo(QStringList plist)
{
    pNumList.push_back(plist[0].toInt());
    QListWidgetItem* item=new QListWidgetItem;
    item->setIcon(QIcon(":/resouces/"+plist[0]+".png"));
    item->setText(plist[1]+"\n"+plist[2]+"\n"+plist[3]);
    item->setTextAlignment(Qt::AlignLeft);
    ui->LWPerform->addItem(item);
    ui->LWPerform->setIconSize(QSize(200,250));
}
//공연 상세 정보 받아 상세 페이지 출력
void ClientMain::PrintDetailOfPerform(QStringList info)
{
    ui->Ltitle->setText(info[0]);
    ui->Lvenues->setText(info[1]);
    ui->Ldurate->setText(info[2]);
    ui->Lptime->setText(info[3]);
    ui->Lage->setText(info[4]);
    ui->Lprice->setText(info[5]);
    ui->Lcast->setText(info[6]);
    ui->Lavail->setText(info[7]);
    ui->Lsche->setText(info[8]);
    ui->Lnotic->setText(info[9]);
    //관람일에 맞춰서 달력 설정
    QStringList dateS=info[2].split('~')[0].split('.');
    QStringList dateF=info[2].split('~')[1].split('.');

    ui->calendarWidget->setDateRange(QDate(dateS[0].toInt(),dateS[1].toInt(),dateS[2].toInt()),QDate(dateF[0].toInt(),dateF[1].toInt(),dateF[2].toInt()));

}
//선택한 좌석 정보 보여주기
void ClientMain::ShowSelectedSeat()
{

    bool ck=true;
    for(int num:sNumList)
    {
        if(num==sender()->objectName().split('_')[1].toInt())
        {
            qDebug()<<sNumList.indexOf(num);
            bookseat->DeleteSelectedSeat(sNumList.indexOf(num));
            sNumList.removeOne(num);
            ck=false;
            break;
        }
    }
    if(ck)
    {
        tcpSocket->write("SN\n"+sender()->objectName().split('_')[1].toUtf8());
        sNumList.push_back(sender()->objectName().split('_')[1].toInt());
    }
    qDebug()<<sender()->objectName().split('_')[1].toInt()<<sNumList;
}
//결제 정보 전송
void ClientMain::SendPayInfo()
{
    QMessageBox::information(this,"알림","결제가 완료되었습니다.",QMessageBox::Ok);
    bookseat->close();
    delete bookseat;
    //id랑 공연번호, 결제 좌석수, 선택한 좌석 번호
    QByteArray msg="Pay\n"+Uid.toUtf8()+"\n"+QString::number(selectedPNum).toUtf8()+"\n"+QString::number(sNumList.size()).toUtf8()+"\n";
    for(int i=0;i<sNumList.size();i++)
    {
        msg.append(QString::number(sNumList[i]).toUtf8()+"\n");
    }
    tcpSocket->write(msg);
}
//로그인 진행
void ClientMain::ProgressLogin(QString ck)
{
    if(ck=="S")
    {
        QMessageBox::information(this,"알림","로그인되었습니다.",QMessageBox::Ok);
        ui->mainStack->setCurrentIndex(0);
        ui->BtnLogin->setText("로그아웃");
        loginFlag=true;
    }
    else
    {
        QMessageBox::critical(this,"경고","아이디 또는 비밀번호가 일치하지 않습니다.",QMessageBox::Ok);
        ui->LEId->clear();
        ui->LEPw->clear();
    }
}
//찾은 아이디 출력
void ClientMain::ShowSID(QString id)
{
    if(!id.compare("F"))
    {
        QMessageBox::critical(this,"경고","일치하는 아이디가 존재하지 않습니다.",QMessageBox::Ok);
        ui->LESIname->clear();
        ui->LESIemailOrphone->clear();
    }
    else
    {
        QMessageBox::information(this,"알림","아이디 : "+id,QMessageBox::Ok);
        ui->loginStack->setCurrentIndex(0);
    }
}
//찾은 비밀번호 출력
void ClientMain::ShowSPW(QString pw)
{
    if(!pw.compare("F"))
    {
        QMessageBox::critical(this,"경고","일치하는 계정이 존재하지 않습니다.",QMessageBox::Ok);
        ui->LESPname->clear();
        ui->LESPid->clear();
        ui->LESPemlOrphone->clear();
    }
    else
    {
        QMessageBox::information(this,"알림","비밀번호 : "+pw,QMessageBox::Ok);
        ui->loginStack->setCurrentIndex(0);
    }
}
//로그인 화면 이동
void ClientMain::GotoLogin()
{
    if(loginFlag)//로그인되어있을때
    {
        loginFlag=false;
        ui->BtnLogin->setText("로그인");
    }
    ui->mainStack->setCurrentIndex(1);
    ui->loginStack->setCurrentIndex(0);
    ui->LEId->clear();
    ui->LEPw->clear();

}
//회원가입 페이지 이동
void ClientMain::GotoJoin()
{
    ui->loginStack->setCurrentIndex(1);
    ui->Jid->clear();
    ui->noticID->clear();
    ui->Jpw1->clear();
    ui->noticPW1->clear();
    ui->Jpw2->clear();
    ui->noticPW2->clear();
    ui->Jname->clear();
    ui->Jemail->clear();
    ui->noticEML->clear();
    ui->Jphone->clear();
    ui->BtnEnroll->setEnabled(false);
}

void ClientMain::on_Jid_textChanged(const QString &arg1)
{
    ui->noticID->setText("영문으로 시작하는 6~20자 영문(소문자), 숫자만 사용 가능합니다.");
    if(arg1[0].isLower()&&arg1.size()>=6&&arg1.size()<=20)
    {
        for(QChar l:arg1)
        {
            if(!l.isLower()&&!l.isDigit())
            {
                ui->noticID->setText("영문으로 시작하는 6~20자 영문(소문자), 숫자만 사용 가능합니다.");
                return;
            }
            else
            {
                ui->noticID->clear();
            }
        }
    }

}


void ClientMain::on_Jpw1_textChanged(const QString &arg1)
{
    unsigned short lnum=0, dnum=0, pnum=0;
    ui->noticPW1->setText("8~12자의 영문, 숫자, 특수문자 중 2가지 이상으로만 가능합니다.");
    if(arg1.size()>=8&&arg1.size()<=12)
    {
        for(QChar l: arg1)
        {
            if(l.isLetter())
            {
                lnum++;
            }
            else if(l.isDigit())
            {
                dnum++;
            }
            else if(l.isPunct())
            {
                pnum++;
            }
            else
            {
                ui->noticPW1->setText("8~12자의 영문, 숫자, 특수문자 중 2가지 이상으로만 가능합니다.");
                return;
            }
        }
    }
    if((lnum>0&&dnum>0)||(lnum>0&&pnum>0)||(pnum>0&&dnum>0))
    {
        ui->noticPW1->clear();
    }
}


void ClientMain::on_Jpw2_textChanged(const QString &arg1)
{
    unsigned short lnum=0, dnum=0, pnum=0;
    ui->noticPW2->setText("8~12자의 영문, 숫자, 특수문자 중 2가지 이상으로만 가능합니다.");
    if(arg1.size()>=8&&arg1.size()<=12)
    {
        for(QChar l: arg1)
        {
            if(l.isLetter())
            {
                lnum++;
            }
            else if(l.isDigit())
            {
                dnum++;
            }
            else if(l.isPunct())
            {
                pnum++;
            }
            else
            {
                ui->noticPW2->setText("8~12자의 영문, 숫자, 특수문자 중 2가지 이상으로만 가능합니다.");
                return;
            }
        }
    }
    if((lnum>0&&dnum>0)||(lnum>0&&pnum>0)||(pnum>0&&dnum>0))
    {
        ui->noticPW2->clear();
    }
    if(ui->Jpw1->text()!=ui->Jpw2->text())
    {
        ui->noticPW2->setText("비밀번호가 일치하지 않습니다. 다시 입력해주세요.");
    }
}


void ClientMain::on_Jemail_textChanged(const QString &arg1)
{
    ui->noticEML->setText("이메일 주소 양식에 맞게 작성해주세요.");
    if(arg1.split('@').size()==2&&arg1.split('.').size()==2)
        if(arg1.indexOf('@')+1<arg1.indexOf('.')&&arg1.indexOf('.')+1<arg1.size())
        {
            ui->noticEML->clear();
            // ui->BtnEnroll->setEnabled(true);//임시
        }
}


void ClientMain::on_BtnEnroll_clicked()
{
    //입력값 검사 모두 완료되었는지 확인해야함
    if(!ui->Jid->text().isEmpty()&&!ui->Jpw1->text().isEmpty()&&!ui->Jpw2->text().isEmpty()&&!ui->Jname->text().isEmpty()&&!ui->Jemail->text().isEmpty())
    {
        if(ui->noticID->text().isEmpty()&&ui->noticPW1->text().isEmpty()&&ui->noticPW2->text().isEmpty()&&ui->noticEML->text().isEmpty())
        {
            //서버에 회원 정보 등록
            QByteArray message;
            message.append(ui->Jid->text().toUtf8());
            message.append('\n');
            message.append(ui->Jpw1->text().toUtf8());
            message.append('\n');
            message.append(ui->Jname->text().toUtf8());
            message.append('\n');
            message.append(ui->Jemail->text().toUtf8());
            message.append('\n');
            message.append(ui->Jphone->text().toUtf8());
            tcpSocket->write(message);
            QMessageBox::information(this,"알림","회원가입이 완료되었습니다. 로그인 화면으로 돌아갑니다.",QMessageBox::Ok);
            ui->loginStack->setCurrentIndex(0);
        }
    }
}


void ClientMain::BtnLogoClicked()
{
    //공연목록 새로고침
    pNumList.clear();
    ui->LWPerform->clear();
    ui->mainStack->setCurrentIndex(0);
    tcpSocket->write("p");
}


void ClientMain::BtnFindClicked()
{
    if(!ui->LEFind->text().isEmpty())
    {
        pNumList.clear();
        ui->LWPerform->clear();
        ui->mainStack->setCurrentIndex(0);
        tcpSocket->write("S\n"+ui->LEFind->text().toUtf8());
    }
}


void ClientMain::on_LWPerform_clicked(const QModelIndex &index)
{
    // qDebug()<<index.row(); //공연 목록 번호 --서버에 D+공연 번호 전송
    //공연 번호
    // qDebug()<<pNumList[index.row()];
    selectedPNum=pNumList[index.row()];
    QString pnum=QString::number(pNumList[index.row()]);
    //서버에 전송
    tcpSocket->write("D\n"+pnum.toUtf8());
    //이미지 삽입
    ui->Lposter->setPixmap(":/resouces/"+pnum+".png");
    ui->LnoticIMG1->setPixmap(":/resouces/"+pnum+"_notice1.png");
    ui->LnoticIMG2->setPixmap(":/resouces/"+pnum+"_notice2.png");
    ui->Ldetail->setPixmap(":/resouces/"+pnum+"_detail.png");

    ui->mainStack->setCurrentIndex(2);
}


void ClientMain::on_BtnLogIN_clicked()
{
    if(!ui->LEId->text().isEmpty()||!ui->LEPw->text().isEmpty())
    {
        tcpSocket->write("L\n"+ui->LEId->text().toUtf8()+"\n"+ui->LEPw->text().toUtf8());
        Uid=ui->LEId->text();
    }
    else
    {
        QMessageBox::critical(this,"경고","아이디, 비밀번호를 모두 입력하세요",QMessageBox::Ok);
    }
}


void ClientMain::on_BtnSID_clicked()
{
    ui->loginStack->setCurrentIndex(2);
    ui->LESIname->clear();
    ui->LESIemailOrphone->clear();
}


void ClientMain::on_BtnSearchID_clicked()
{
    if(!ui->LESIname->text().isEmpty()||!ui->LESIemailOrphone->text().isEmpty())
        tcpSocket->write("SI\n"+ui->LESIname->text().toUtf8()+"\n"+ui->LESIemailOrphone->text().toUtf8());
}

void ClientMain::on_BtnSPW_clicked()
{
    ui->loginStack->setCurrentIndex(3);
    ui->LESPname->clear();
    ui->LESPid->clear();
    ui->LESPemlOrphone->clear();
}

void ClientMain::on_BtnSearchPW_clicked()
{
    if(!ui->LESPname->text().isEmpty()||!ui->LESPid->text().isEmpty()||!ui->LESPemlOrphone->text().isEmpty())
        tcpSocket->write("SP\n"+ui->LESPname->text().toUtf8()+"\n"+ui->LESPid->text().toUtf8()+"\n"+ui->LESPemlOrphone->text().toUtf8());
}



void ClientMain::on_Jid_editingFinished()
{
    //서버에 아이디 보내 중복확인해야함
    tcpSocket->write("EI\n"+ui->Jid->text().toUtf8());
}


void ClientMain::on_showPW1_clicked()
{
    if(ui->Jpw1->echoMode()==QLineEdit::Password)
        ui->Jpw1->setEchoMode(QLineEdit::Normal);
    else
        ui->Jpw1->setEchoMode(QLineEdit::Password);
}


void ClientMain::on_showPW2_clicked()
{
    if(ui->Jpw2->echoMode()==QLineEdit::Password)
        ui->Jpw2->setEchoMode(QLineEdit::Normal);
    else
        ui->Jpw2->setEchoMode(QLineEdit::Password);
}


void ClientMain::on_Jemail_editingFinished()
{
    tcpSocket->write("EEML\n"+ui->Jemail->text().toUtf8());
}


void ClientMain::on_BtnATphone_clicked()
{
    for(QChar c:ui->Jphone->text())
    {
        if(!c.isDigit())
        {
            QMessageBox::critical(this,"경고","숫자만 입력해주세요",QMessageBox::Ok);
            ui->Jphone->clear();
            return;
        }
    }
    tcpSocket->write("EPH\n"+ui->Jphone->text().toUtf8());
}





void ClientMain::on_BtnBooking_clicked()
{
    if(!loginFlag)
    {
        QMessageBox::critical(this,"경고","로그인을 해주세요.",QMessageBox::Ok);
        ui->mainStack->setCurrentIndex(1);
        return;
    }
    //서버에 공연번호와 선택된 날짜 보내서 좌석 현황 받아와야함
    //관람일자는 디폴트 선택되어 따로 선택하지 않아도 선택된 값 존재
    if(ui->calendarWidget->selectedDate()<QDate::currentDate())
    {
        QMessageBox::critical(this,"경고","예매가 종료된 회차입니다.",QMessageBox::Ok);
        return;
    }
    int y,m,d;
    ui->calendarWidget->selectedDate().getDate(&y,&m,&d);
    tcpSocket->write("B\n"+QString::number(selectedPNum).toUtf8() +"\n"+QString::number(m).toUtf8()+"."+QString::number(d).toUtf8());
    //예매창띄우기
    //방법 1
    // ChooseSeat bookseat;
    // bookseat.setModal(true);
    // bookseat.exec();
    //방법 2 헤더에 ChooseSeat* bookseat; 추가
    // bookseat=new ChooseSeat(this);
    // bookseat->show();
}

