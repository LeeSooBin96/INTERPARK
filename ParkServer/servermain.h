#ifndef SERVERMAIN_H
#define SERVERMAIN_H

#include <QMainWindow>
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
    QTcpServer *tcpServer;
};
#endif // SERVERMAIN_H
