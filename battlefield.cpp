#include "battlefield.h"

BattleField::BattleField(QWidget *parent) : QWidget(parent)
{
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(5);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    
    m_controller = new GameController(this);
    
    initBattleField();
    
    // 连接信号
    connect(m_controller, &GameController::sig_cellUpdated, this, &BattleField::onCellUpdated);
    connect(m_controller, &GameController::sig_gameStateChanged,this, &BattleField::onGameStateChanged);
    connect(m_controller,&GameController::sig_updateScore,this,&BattleField::onScoreUpdate);
    connect(m_controller,&GameController::sig_result,this,&BattleField::sig_result);
    connect(m_controller,&GameController::sig_cardPlaced,this,&BattleField::sig_cardPlaced);
}

BattleField::~BattleField()
{
    qDeleteAll(m_cellWidgets);
    qDeleteAll(m_cellUnits);
}

void BattleField::initBattleField()
{
    // 创建积分器
    lb_r1 = new QLabel("0");
    lb_r2 = new QLabel("0");
    lb_r3 = new QLabel("0");
    lb_b1 = new QLabel("0");
    lb_b2 = new QLabel("0");
    lb_b3 = new QLabel("0");
    m_gridLayout->addWidget(lb_r1,0,0);
    m_gridLayout->addWidget(lb_r2,1,0);
    m_gridLayout->addWidget(lb_r3,2,0);
    m_gridLayout->addWidget(lb_b1,0,6);
    m_gridLayout->addWidget(lb_b2,1,6);
    m_gridLayout->addWidget(lb_b3,2,6);
    // 创建单元格模型
    for(int row = 0; row < 3; ++row) {
        for(int col = 0; col < 5; ++col) {
            CellUnit* unit = new CellUnit(row, col);
            m_cellUnits.append(unit);
            
            CellWidget* widget = new CellWidget(unit, this);
            m_cellWidgets.append(widget);
            
            m_gridLayout->addWidget(widget, row, col+1);
            
            // 连接单元格信号
            connect(widget, &CellWidget::clicked, 
                    this, &BattleField::onCellClicked);
            connect(widget, &CellWidget::hovered,
                    this, &BattleField::onCellHovered);
        }
    }
    
    // 初始化阵营区域
    initCampAreas();
    
    // 初始化控制器
    m_controller->initBattleField(m_cellUnits);
}

void BattleField::initCampAreas()
{
    // 红方阵营（左侧三格）
    m_cellUnits[0]->changeCampArea(CAMP::CAMP_RED, 1);
    m_cellUnits[5]->changeCampArea(CAMP::CAMP_RED, 1);
    m_cellUnits[10]->changeCampArea(CAMP::CAMP_RED, 1);
    
    // 蓝方阵营（右侧三格）
    m_cellUnits[4]->changeCampArea(CAMP::CAMP_BLUE, 1);
    m_cellUnits[9]->changeCampArea(CAMP::CAMP_BLUE, 1);
    m_cellUnits[14]->changeCampArea(CAMP::CAMP_BLUE, 1);
}

GameController* BattleField::getController() const
{
    return m_controller;
}

void BattleField::setSelectedCard(Card *card)
{
    m_controller->selectCard(card);
}

void BattleField::addRedCard(Card *card)
{
    m_controller->addRedCard(card);
}

void BattleField::addBlueCard(Card *card)
{
    m_controller->addBlueCard(card);
}

void BattleField::skipTurn()
{
    m_controller->endTurn();
}

void BattleField::onCellClicked(CellUnit* unit)
{
    // 将点击事件转发给控制器
    m_controller->placeCard(unit);
}

void BattleField::onCellHovered(CellUnit* unit, bool entered)
{
    Q_UNUSED(unit);
    Q_UNUSED(entered);
    // 可以在这里处理悬停效果
}

void BattleField::onCellUpdated(CellUnit* unit)
{
    // 找到对应的widget并刷新
    for(CellWidget* widget : m_cellWidgets) {
        if(widget->getCellUnit() == unit) {
            widget->update();
            break;
        }
    }
}

void BattleField::onGameStateChanged(GAME_STATE state)
{
    // 更新高亮状态
    if(state==GAME_STATE::STATE_PLAYER_SELECT_CELL){
        for(CellWidget* widget : m_cellWidgets) {
            bool canPlace = m_controller->canPlaceCard(widget->getCellUnit());
            widget->highlight(canPlace);
        }
    }
    else if(state==GAME_STATE::STATE_PLAYER_TURN){
        for(CellWidget* widget : m_cellWidgets) {
            widget->highlight(false);
        }
        if(m_controller->canOperateTurn())
            emit sig_clear();
        else
            emit sig_noCardCanPlace();
    }
}

void BattleField::onScoreUpdate(int r1,int b1,int r2,int b2,int r3,int b3)
{
    lb_r1->setNum(r1);
    lb_r2->setNum(r2);
    lb_r3->setNum(r3);
    lb_b1->setNum(b1);
    lb_b2->setNum(b2);
    lb_b3->setNum(b3);
}
