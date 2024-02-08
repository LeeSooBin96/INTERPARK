#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QMainWindow>
#include <QTcpSocket>

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

private:
    Ui::ClientMain *ui;
};
#endif // CLIENTMAIN_H
