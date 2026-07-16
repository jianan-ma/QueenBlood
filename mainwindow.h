#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include "battlefield.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onCbo1(int index);
    void onCbo2(int index);
    void getGameResult(GAME_RESULT result);
    void onCardPlaced(CAMP_TURN turn,Card* pcard);
private:
    Ui::MainWindow *ui;
    BattleField *field=nullptr;
    QComboBox *cbocard1=nullptr;
    QComboBox *cbocard2=nullptr;
    QPushButton *btn_skip=nullptr;
    QVector<Card*> cards_red;
    QVector<Card*> cards_blue;

};
#endif // MAINWINDOW_H
