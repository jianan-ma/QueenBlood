#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "testcard.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    field = new BattleField(this);
    connect(field,&BattleField::sig_noCardCanPlace,this,[this](){
        ui->statusbar->showMessage("无可放置卡牌");
    });
    connect(field,&BattleField::sig_result,this,&MainWindow::getGameResult);
    connect(field,&BattleField::sig_clear,this,[this](){
        ui->statusbar->clearMessage();
    });
    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
    layout->addWidget(field);
    QHBoxLayout *hlayout = new QHBoxLayout;
    btn_skip = new QPushButton;
    connect(btn_skip,&QPushButton::clicked,field,&BattleField::skipTurn);
    hlayout->addWidget(btn_skip);
    layout->addLayout(hlayout);

    for(int i=1;i<=9;i++){
        QString jsonfile = ":/json/cards/card00"+QString::number(i)+".json";
        Card *card = new Card;
        card->loadCard(jsonfile);
        Card *card2 = new Card(-1);
        card2->loadCard(jsonfile);
        field->addRedCard(card);
        field->addBlueCard(card2);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getGameResult(GAME_RESULT result)
{
    if(result==GAME_RESULT::RED_WIN)
        ui->statusbar->showMessage("RED WIN!");
    else if(result==GAME_RESULT::BLUE_WIN)
        ui->statusbar->showMessage("BLUE WIN!");
    else
        ui->statusbar->showMessage("DRAW");
}

