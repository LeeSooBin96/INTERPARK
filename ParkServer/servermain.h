#ifndef SERVERMAIN_H
#define SERVERMAIN_H

#include <QMainWindow>

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
};
#endif // SERVERMAIN_H
