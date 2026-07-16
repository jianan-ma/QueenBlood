#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "cellunit.h"
#include "cellwidget.h"
#include "gamecontroller.h"

class BattleField : public QWidget
{
    Q_OBJECT
public:
    explicit BattleField(QWidget *parent = nullptr);
    ~BattleField();
    
    void initBattleField();
    GameController* getController() const;
    void setSelectedCard(Card* card);
    void addRedCard(Card* card);
    void addBlueCard(Card* card);
public slots:
    void skipTurn();
private slots:
    void onCellClicked(CellUnit* unit);
    void onCellHovered(CellUnit* unit, bool entered);
    void onCellUpdated(CellUnit* unit);
    void onGameStateChanged(GAME_STATE state);
    void onScoreUpdate(int r1,int b1,int r2,int b2,int r3,int b3);
private:
    QGridLayout* m_gridLayout;
    QVector<CellWidget*> m_cellWidgets;
    QVector<CellUnit*> m_cellUnits;
    GameController* m_controller;
    QLabel *lb_r1;
    QLabel *lb_r2;
    QLabel *lb_r3;
    QLabel *lb_b1;
    QLabel *lb_b2;
    QLabel *lb_b3;
    // 初始化阵营区域
    void initCampAreas();
signals:
    void sig_noCardCanPlace();
    void sig_clear();
    void sig_result(GAME_RESULT result);
    void sig_cardPlaced(CAMP_TURN,Card*);
};

#endif // BATTLEFIELD_H
