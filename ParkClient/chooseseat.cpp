#include <QMessageBox>

#include "chooseseat.h"
#include "ui_chooseseat.h"

ChooseSeat::ChooseSeat(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChooseSeat)
{
    ui->setupUi(this);

    connect(ui->BtnPay,SIGNAL(clicked()),this->parent(),SLOT(SendPayInfo()));
}

ChooseSeat::~ChooseSeat()
{
    delete ui;
}
//예매페이지에 공연 정보 출력
void ChooseSeat::PrintPerformInfo(QString title,QString venues,QDate date)
{
    ui->stackedWidget->setCurrentIndex(0);
    int y,m,d;
    date.getDate(&y,&m,&d);
    ui->LPerformInfo->setText(title+"\n장소: "+venues+"\n공연일자: "+QString::number(y)+"년"+QString::number(m)+"월"+QString::number(d)+"일");
}
//좌석 정보 갱신
void ChooseSeat::UpdateSeatInfo(QStringList slist)
{
    // qDebug()<<slist;
    int count=slist[1].toInt(); //등급 개수
    int rnum1,rnum2,rnum3,rnum4; //등급번호 저장
    if(count==4)
    {
        ui->TBRseat->clear();
        rnum1=slist[2].toInt();
        ui->TBRseat->append("VIP석 "+slist[3]+"\n");
        rnum2=slist[4].toInt();
        ui->TBRseat->append("R석 "+slist[5]+"\n");
        rnum3=slist[6].toInt();
        ui->TBRseat->append("S석 "+slist[7]+"\n");
        rnum4=slist[8].toInt();
        ui->TBRseat->append("A석 "+slist[9]+"\n");
    }
    //좌석 배치도 생성 --여기가 젤 어렵...ㅠㅠ
    // qDebug()<<ui->pushButton->objectName(); //버튼이름 이렇게 가져올 수 있다!
    unsigned int seatNum=1;
    QVBoxLayout *defaultlayout=new QVBoxLayout();
    QLabel* lbl=new QLabel("STAGE");
    lbl->setAlignment(Qt::AlignHCenter);
    lbl->setStyleSheet("QLabel {background-color: rgb(241, 241, 241);}");
    defaultlayout->addWidget(lbl);
    int index=11;
    while(true)
    {
        QHBoxLayout *col=new QHBoxLayout();
        for(int i=0;i<slist[index].toInt()*2;i+=2)
        {
            QPushButton* btn=new QPushButton();
            btn->setObjectName("Btn_"+QString::number(seatNum));
            // QPalette btncolor=btn->palette();
            if(count==4)
            {
                if(slist[index+1+i].toInt()==rnum1)
                {
                    btn->setStyleSheet("QPushButton{background-color: rgb(142, 67, 231);border-radius: 5px;}"
                                       "QPushButton::checked{background-color: rgb(254, 254, 254);border: 5px solid black;}");
                    // btn->setPalette(QPalette(QColor(qRgb(142, 67, 231))));
                    // btn->setPalette(QPalette::setColor(QPalette::Button,QColor(qRgb(142, 67, 231))));
                    // btncolor.setColor(QPalette::Button,QColor(Qt::red));
                }
                else if(slist[index+1+i].toInt()==rnum2)
                {
                    btn->setStyleSheet("QPushButton{background-color: rgb(28, 168, 20);border-radius: 5px;}"
                                       "QPushButton::checked{background-color: rgb(254, 254, 254);border: 5px solid black;}");
                }
                else if(slist[index+1+i].toInt()==rnum3)
                {
                    btn->setStyleSheet("QPushButton{background-color: rgb(23, 179, 255);border-radius: 5px;}"
                                       "QPushButton::checked{background-color: rgb(254, 254, 254);border: 5px solid black;}");
                }
                else if(slist[index+1+i].toInt()==rnum4)
                {
                    btn->setStyleSheet("QPushButton{background-color: rgb(251, 126, 79);border-radius: 5px;}"
                                       "QPushButton::checked{background-color: rgb(254, 254, 254);border: 5px solid black;}");
                }
            }

            // btn->setStyleSheet(btn->styleSheet().append("clicked:border:5px solid black;"));
            // btn->setPalette(btncolor);
            if(slist[index+2+i]!="F")
            {
                btn->setStyleSheet("background-color: rgb(153, 153, 153);border-radius: 5px;");
                btn->setEnabled(false);
            }
            btn->setCursor(Qt::PointingHandCursor);
            btn->setCheckable(true);
            connect(btn,SIGNAL(clicked()),this->parent(),SLOT(ShowSelectedSeat()));
            col->addWidget(btn);
            seatNum++;
        }

        col->setContentsMargins(500-slist[index].toInt()*20,10,500-slist[index].toInt()*20,10);
        defaultlayout->addLayout(col);
        index+=slist[index].toInt()*2+1;
        if(slist[index]=="F2"||slist[index]=="F3"||slist[index]=="F4")
        {
            index++;
            QFrame* hline=new QFrame();
            hline->setFrameStyle(QFrame::HLine);
            defaultlayout->addWidget(hline);
        }
        else if(slist[index]=="") break;
    }


    ui->WSeat->setLayout(defaultlayout);
    seatNumMax=seatNum;
}
//선택된 좌석 출력
void ChooseSeat::PrintSelectedSeat(QStringList sinfo)
{
    qDebug()<<sinfo;
    QListWidgetItem* item=new QListWidgetItem();
    item->setText(sinfo[1]+"층 "+QString(QChar(sinfo[2].toInt()+64))+"구역 "+sinfo[3]+"열 "+sinfo[4]);
    ui->listWidget->addItem(item);
}
//선택된 좌석 목록에서 삭제
void ChooseSeat::DeleteSelectedSeat(int index)
{
    qDebug()<<"함수내"<<index;
    ui->listWidget->takeItem(index);
}
// //선택한 좌석 정보 보여주기
// void ChooseSeat::ShowSelectedSeat()
// {
//     qDebug()<<sender()->objectName();//이제 이걸 서버에...어떻게 전하지
// }

void ChooseSeat::on_pushButton_clicked()
{
    if(ui->listWidget->count()==0)
    {
        QMessageBox::critical(this,"경고","좌석을 선택해주세요.",QMessageBox::Ok);
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);
    for(int i=0;i<ui->listWidget->count();i++)
    {
        QListWidgetItem* item=new QListWidgetItem();
        item->setText(ui->listWidget->item(i)->text());
        ui->listWidget_2->addItem(item);
    }
}


void ChooseSeat::on_pushButton_4_clicked()
{
    ui->listWidget_2->clear();
    ui->stackedWidget->setCurrentWidget(0);
}

