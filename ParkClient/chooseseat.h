#ifndef CHOOSESEAT_H
#define CHOOSESEAT_H

#include <QDialog>
#include <QDate>

namespace Ui {
class ChooseSeat;
}

class ChooseSeat : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseSeat(QWidget *parent = nullptr);
    ~ChooseSeat();
    //예매페이지에 공연 정보 출력
    void PrintPerformInfo(QString,QString,QDate);
    //좌석 정보 갱신
    void UpdateSeatInfo(QStringList);
    //선택된 좌석 출력
    void PrintSelectedSeat(QStringList);
    //선택된 좌석 목록에서 삭제
    void DeleteSelectedSeat(int);

private:
    Ui::ChooseSeat *ui;
    unsigned int seatNumMax;

private slots:
    // //선택한 좌석 정보 보여주기
    // void ShowSelectedSeat();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();

};

#endif // CHOOSESEAT_H
