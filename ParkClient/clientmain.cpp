#include "clientmain.h"
#include "ui_clientmain.h"

ClientMain::ClientMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMain)
{
    ui->setupUi(this);
}

ClientMain::~ClientMain()
{
    delete ui;
}

