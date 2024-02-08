#include "servermain.h"
#include "ui_servermain.h"

ServerMain::ServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMain)
{
    ui->setupUi(this);
}

ServerMain::~ServerMain()
{
    delete ui;
}
