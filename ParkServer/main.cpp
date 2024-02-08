#include "servermain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerMain w;
    w.show();
    return a.exec();
}
