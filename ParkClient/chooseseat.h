#ifndef CHOOSESEAT_H
#define CHOOSESEAT_H

#include <QStackedWidget>

namespace Ui {
class ChooseSeat;
}

class ChooseSeat : public QStackedWidget
{
    Q_OBJECT

public:
    explicit ChooseSeat(QWidget *parent = nullptr);
    ~ChooseSeat();

private:
    Ui::ChooseSeat *ui;
};

#endif // CHOOSESEAT_H
