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
    connect(field,&BattleField::sig_cardPlaced,this,&MainWindow::onCardPlaced);
    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
    layout->addWidget(field);
    QHBoxLayout *hlayout = new QHBoxLayout;
    cbocard1 = new QComboBox;
    cbocard2 = new QComboBox;
    btn_skip = new QPushButton;
    connect(cbocard1,QOverload<int>::of(&QComboBox::activated),this,&MainWindow::onCbo1);
    connect(cbocard2,QOverload<int>::of(&QComboBox::activated),this,&MainWindow::onCbo2);
    connect(btn_skip,&QPushButton::clicked,field,&BattleField::skipTurn);
    hlayout->addWidget(cbocard1);
    hlayout->addWidget(btn_skip);
    hlayout->addWidget(cbocard2);
    layout->addLayout(hlayout);

    TestCard *card1 = new TestCard;
    cards_red.append(card1);
    field->addRedCard(card1);
    TestCard *card2 = new TestCard;
    cards_red.append(card2);
    field->addRedCard(card2);
    TestCard *card3 = new TestCard;
    cards_red.append(card3);
    field->addRedCard(card3);
    TestCard *card4 = new TestCard;
    cards_red.append(card4);
    field->addRedCard(card4);
    TestCard *card5 = new TestCard;
    cards_red.append(card5);
    field->addRedCard(card5);

    TestCard *card6 = new TestCard;
    cards_blue.append(card6);
    field->addBlueCard(card6);
    TestCard *card7 = new TestCard;
    cards_blue.append(card7);
    field->addBlueCard(card7);
    TestCard *card8 = new TestCard;
    cards_blue.append(card8);
    field->addBlueCard(card8);
    TestCard *card9 = new TestCard;
    cards_blue.append(card9);
    field->addBlueCard(card9);
    TestCard *card10 = new TestCard;
    cards_blue.append(card10);
    field->addBlueCard(card10);

    cbocard1->addItem("testcard1");
    cbocard1->addItem("testcard2");
    cbocard1->addItem("testcard3");
    cbocard1->addItem("testcard4");
    cbocard1->addItem("testcard5");
    cbocard2->addItem("testcard6");
    cbocard2->addItem("testcard7");
    cbocard2->addItem("testcard8");
    cbocard2->addItem("testcard9");
    cbocard2->addItem("testcard10");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCbo1(int index)
{
    field->setSelectedCard(cards_red[index]);
}

void MainWindow::onCbo2(int index)
{

    field->setSelectedCard(cards_blue[index]);
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

void MainWindow::onCardPlaced(CAMP_TURN turn, Card *pcard)
{
    if(turn==CAMP_TURN::TURN_RED){
        cards_red.removeOne(pcard);
        cbocard1->removeItem(cbocard1->currentIndex());
    }
    else{
        cards_blue.removeOne(pcard);
        cbocard2->removeItem(cbocard2->currentIndex());
    }
}

