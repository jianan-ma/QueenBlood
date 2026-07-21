#include "battlefield.h"

BattleField::BattleField(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    m_redCardPanel = new CardHandPanel(this);
    m_redCardPanel->setStyleSheet("background-color: rgba(255, 200, 200, 50); border-radius: 5px;");
    m_mainLayout->addWidget(m_redCardPanel);
    
    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(5);
    
    m_controller = new GameController(this);
    
    initBattleField();
    
    connect(m_controller, &GameController::sig_cellUpdated, this, &BattleField::onCellUpdated);
    connect(m_controller, &GameController::sig_gameStateChanged,this, &BattleField::onGameStateChanged);
    connect(m_controller,&GameController::sig_updateScore,this,&BattleField::onScoreUpdate);
    connect(m_controller,&GameController::sig_result,this,&BattleField::sig_result);
    connect(m_controller,&GameController::sig_cardPlaced,this,&BattleField::onCardPlaced);
    connect(m_controller,&GameController::sig_summonCard,this,&BattleField::getSummonCard);
    connect(m_redCardPanel, &CardHandPanel::cardSelected, this, &BattleField::onCardSelected);
    connect(m_redCardPanel, &CardHandPanel::cardDeselected, this, &BattleField::onCardDeselected);
    
    m_mainLayout->addLayout(m_gridLayout);
    
    m_blueCardPanel = new CardHandPanel(this);
    m_blueCardPanel->setStyleSheet("background-color: rgba(200, 200, 255, 50); border-radius: 5px;");
    m_mainLayout->addWidget(m_blueCardPanel);
    
    connect(m_blueCardPanel, &CardHandPanel::cardSelected, this, &BattleField::onCardSelected);
    connect(m_blueCardPanel, &CardHandPanel::cardDeselected, this, &BattleField::onCardDeselected);

}

BattleField::~BattleField()
{
    qDeleteAll(m_cellWidgets);
    qDeleteAll(m_cellUnits);
}

void BattleField::initBattleField()
{
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
    
    for(int row = 0; row < 3; ++row) {
        for(int col = 0; col < 5; ++col) {
            CellUnit* unit = new CellUnit(row, col);
            m_cellUnits.append(unit);
            
            CellWidget* widget = new CellWidget(unit, this);
            m_cellWidgets.append(widget);
            
            m_gridLayout->addWidget(widget, row, col+1);
            
            connect(widget, &CellWidget::clicked, 
                    this, &BattleField::onCellClicked);
            connect(widget, &CellWidget::hovered,
                    this, &BattleField::onCellHovered);
        }
    }
    
    initCampAreas();
    
    m_controller->initBattleField(m_cellUnits);
}

void BattleField::initCampAreas()
{
    m_cellUnits[0]->changeCampArea(CAMP::CAMP_RED, 1);
    m_cellUnits[5]->changeCampArea(CAMP::CAMP_RED, 1);
    m_cellUnits[10]->changeCampArea(CAMP::CAMP_RED, 1);
    
    m_cellUnits[4]->changeCampArea(CAMP::CAMP_BLUE, 1);
    m_cellUnits[9]->changeCampArea(CAMP::CAMP_BLUE, 1);
    m_cellUnits[14]->changeCampArea(CAMP::CAMP_BLUE, 1);
}

GameController* BattleField::getController() const
{
    return m_controller;
}

void BattleField::addRedCard(Card *card)
{
    m_controller->addRedCard(card);
    m_redCardPanel->addCard(card);
    connect(card,&Card::sig_summon,m_redCardPanel,QOverload<int>::of(&CardHandPanel::addCard));
}

void BattleField::addBlueCard(Card *card)
{
    m_controller->addBlueCard(card);
    m_blueCardPanel->addCard(card);
    connect(card,&Card::sig_summon,m_blueCardPanel,QOverload<int>::of(&CardHandPanel::addCard));
}

void BattleField::skipTurn()
{
    m_controller->endTurn();
}

void BattleField::onCellClicked(CellUnit* unit)
{
    m_controller->placeCard(unit);
}

void BattleField::onCellHovered(CellUnit* unit, bool entered)
{
    if(m_controller->getGameState() != GAME_STATE::STATE_PLAYER_SELECT_CELL) {
        return;
    }
    
    if(entered){
        m_controller->reviewMode(unit);
    }
    else{
        m_controller->recoverCampChange(unit);
    }
}

void BattleField::onCellUpdated(CellUnit* unit)
{
    for(CellWidget* widget : m_cellWidgets) {
        if(widget->getCellUnit() == unit) {
            widget->update();
            break;
        }
    }
}

void BattleField::onGameStateChanged(GAME_STATE state)
{
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

void BattleField::onCardSelected(Card* card)
{
    m_controller->selectCard(card);
}

void BattleField::onCardDeselected()
{
    m_controller->selectCard(nullptr);
}

void BattleField::onCardPlaced(CAMP_TURN turn, Card *card)
{
    if(turn==CAMP_TURN::TURN_RED)
        m_redCardPanel->removeCard(card);
    else
        m_blueCardPanel->removeCard(card);
}

void BattleField::getSummonCard(int id)
{
    if(m_controller->getCurrentTurn()==CAMP_TURN::TURN_RED){
        m_redCardPanel->addCard(id);
    }
    else{
        m_blueCardPanel->addCard(id);
    }
}
