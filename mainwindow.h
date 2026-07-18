#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void getGameResult(GAME_RESULT result);
private:
    Ui::MainWindow *ui;
    BattleField *field=nullptr;
    QPushButton *btn_skip=nullptr;

};
#endif // MAINWINDOW_H
