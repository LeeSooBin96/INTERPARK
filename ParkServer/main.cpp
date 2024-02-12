#include "servermain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //데이터베이스 연결 --빌드파일에 db파일 넣어줄것
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("DB_Ticket.db");
    if(!db.open())
    {
        qDebug()<<db.lastError()<<"데이터베이스 연결 실패!";
    }
    else
    {
        qDebug()<<("데이터베이스 연결 완료");
    }
    ServerMain w;
    w.show();
    return a.exec();
}
