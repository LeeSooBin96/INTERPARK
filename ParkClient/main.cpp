#include "clientmain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientMain w;
    w.show();
    return a.exec();
}
