#include "chooseseat.h"
#include "ui_chooseseat.h"

ChooseSeat::ChooseSeat(QWidget *parent)
    : QStackedWidget(parent)
    , ui(new Ui::ChooseSeat)
{
    ui->setupUi(this);
}

ChooseSeat::~ChooseSeat()
{
    delete ui;
}
