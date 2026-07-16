#include "gamecontroller.h"
#include <QDebug>

GameController::GameController(QObject *parent) 
    : QObject(parent), 
      m_gameState(GAME_STATE::STATE_PLAYER_TURN),
      m_currentTurn(CAMP_TURN::TURN_RED),
      m_selectedCard(nullptr)
{
}

void GameController::initBattleField(QVector<CellUnit*>& cells)
{
    m_cells = cells;
}

void GameController::selectCard(Card* card)
{
    m_selectedCard = card;
    m_gameState = GAME_STATE::STATE_PLAYER_SELECT_CELL;
    emit sig_gameStateChanged(m_gameState);
}

bool GameController::canPlaceCard(CellUnit* cell) const
{
    if(!m_selectedCard) return false;
    // 检查目标单元格是否为空
    if(cell->getCampCard() != CAMP::CAMP_NULL) return false;
    // 检查费用
    if(cell->getCampLevel()<m_selectedCard->getCost())
        return false;
    // 检查是否在己方阵营区域
    CAMP playerCamp = (m_currentTurn == CAMP_TURN::TURN_RED) ? 
                      CAMP::CAMP_RED : CAMP::CAMP_BLUE;
    
    return cell->getCampArea() == playerCamp;
}

bool GameController::canOperateTurn()
{
    if(m_currentTurn==CAMP_TURN::TURN_RED){
        for(Card* pcard:cards_red){
            for(CellUnit* pcell:m_cells){
                if(pcell->getCampArea()!=CAMP::CAMP_RED)
                    continue;
                if(pcell->getCampLevel()<pcard->getCost())
                    continue;
                return true;
            }
        }
    }
    else{
        for(Card* pcard:cards_blue){
            for(CellUnit* pcell:m_cells){
                if(pcell->getCampArea()!=CAMP::CAMP_BLUE)
                    continue;
                if(pcell->getCampLevel()<pcard->getCost())
                    continue;
                return true;
            }
        }
    }
    return false;
}

bool GameController::placeCard(CellUnit* targetCell)
{
    if(!canPlaceCard(targetCell)) return false;
    
    CAMP playerCamp = (m_currentTurn == CAMP_TURN::TURN_RED) ? 
                      CAMP::CAMP_RED : CAMP::CAMP_BLUE;
    if(targetCell->getCampLevel()<m_selectedCard->getCost())
        return false;
    skipTurn = false;
    // 放置卡牌
    targetCell->changeCampCard(m_selectedCard, playerCamp, m_selectedCard->getPower());
//    targetCell->changeCampArea(playerCamp, m_selectedCard->getCost());
    
    emit sig_cardPlaced(m_currentTurn,m_selectedCard);
    emit sig_cellUpdated(targetCell);
    calculateCampChange(targetCell);
    // 计算强化效果
    calculateReinforcements(targetCell);
    
    // 检查战斗
    checkBattles(targetCell);
    
    calculateRowsScore();
    // 重置选择状态
    m_selectedCard = nullptr;
    m_gameState = GAME_STATE::STATE_PLAYER_TURN;
    emit sig_gameStateChanged(m_gameState);
    m_currentTurn = m_currentTurn==CAMP_TURN::TURN_RED ? CAMP_TURN::TURN_BLUE : CAMP_TURN::TURN_RED;
    return true;
}

void GameController::calculateReinforcements(CellUnit* placedCell)
{
//    if(!placedCell->getCampCard()) return;
    
//    QHash<int, int> reinCamp = placedCell->getCampCard()->getReinCamp();
    
//    for(auto it = reinCamp.begin(); it != reinCamp.end(); ++it) {
//        int offset = it.key();
//        int levelBonus = it.value();
        
//        int targetPos = placedCell->getPos() + offset;
//        if(targetPos >= 0 && targetPos < m_cells.size()) {
//            CellUnit* targetCell = m_cells[targetPos];
//            if(targetCell->getCampCard() != CAMP::CAMP_NULL &&
//               targetCell->getCampCard() == placedCell->getCampCard()) {
//                targetCell->addLevel(levelBonus);
//                emit sig_cellUpdated(targetCell);
//            }
//        }
    //    }
}

void GameController::calculateRowsScore()
{
    score_red1 = 0;
    score_red2 = 0;
    score_red3 = 0;
    score_blue1 = 0;
    score_blue2 = 0;
    score_blue3 = 0;
    for(int i=0;i<5;i++){
        int score = m_cells[i]->getScore();
        if(m_cells[i]->getCampCard()==CAMP::CAMP_RED)
            score_red1 += score;
        else if(m_cells[i]->getCampCard()==CAMP::CAMP_BLUE)
            score_blue1 += score;
    }
    for(int i=5;i<10;i++){
        int score = m_cells[i]->getScore();
        if(m_cells[i]->getCampCard()==CAMP::CAMP_RED)
            score_red2 += score;
        else if(m_cells[i]->getCampCard()==CAMP::CAMP_BLUE)
            score_blue2 += score;
    }
    for(int i=10;i<15;i++){
        int score = m_cells[i]->getScore();
        if(m_cells[i]->getCampCard()==CAMP::CAMP_RED)
            score_red3 += score;
        else if(m_cells[i]->getCampCard()==CAMP::CAMP_BLUE)
            score_blue3 += score;
    }
    emit sig_updateScore(score_red1,score_blue1,score_red2,score_blue2,score_red3,score_blue3);
}

void GameController::checkBattles(CellUnit* placedCell)
{
    // 检查上下左右四个方向
//    QVector<int> directions = {-5, -1, 1, 5}; // 上、左、右、下
    
//    for(int dir : directions) {
//        int targetPos = placedCell->getPos() + dir;
//        if(targetPos >= 0 && targetPos < m_cells.size()) {
//            CellUnit* adjacentCell = m_cells[targetPos];
            
//            // 检查是否有敌方卡牌
//            if(adjacentCell->getCampCard() != CAMP::CAMP_NULL &&
//               adjacentCell->getCampCard() != placedCell->getCampCard()) {
//                executeBattle(placedCell, adjacentCell);
//            }
//        }
//    }
}

void GameController::executeBattle(CellUnit* attacker, CellUnit* defender)
{
    int attackerPower = attacker->getScore() * attacker->getCampLevel();
    int defenderPower = defender->getScore() * defender->getCampLevel();
    
    bool attackerWin = attackerPower >= defenderPower;
    
    emit sig_battleResult(attacker, defender, attackerWin);
    
    if(attackerWin) {
        // 攻击方胜利，防御方卡牌被摧毁
        defender->redScore(defender->getScore());
        // 占领格子
        defender->changeCampArea(attacker->getCampArea(), attacker->getCampLevel());
    } else {
        // 防御方胜利，攻击方卡牌被摧毁
        attacker->redScore(attacker->getScore());
    }
    
    emit sig_cellUpdated(attacker);
    emit sig_cellUpdated(defender);
}

GAME_STATE GameController::getGameState() const
{
    return m_gameState;
}

CAMP_TURN GameController::getCurrentTurn() const
{
    return m_currentTurn;
}

void GameController::endTurn()
{
    if(skipTurn){
        //游戏结束，开始结算
        int sum_red=0;
        int sum_blue=0;
        if(score_red1>score_blue1)
            sum_red += score_red1;
        else if(score_red1<score_blue1)
            sum_blue += score_blue1;
        if(score_red2>score_blue2)
            sum_red += score_red2;
        else if(score_red2<score_blue2)
            sum_blue += score_blue2;
        if(score_red3>score_blue3)
            sum_red += score_red3;
        else if(score_red3<score_blue3)
            sum_blue += score_blue3;
        if(sum_red>sum_blue)
            emit sig_result(GAME_RESULT::RED_WIN);
        else if(sum_red<sum_blue)
            emit sig_result(GAME_RESULT::BLUE_WIN);
        else
            emit sig_result(GAME_RESULT::DRAW);
    }
    else{
        m_currentTurn = (m_currentTurn == CAMP_TURN::TURN_RED) ?
                        CAMP_TURN::TURN_BLUE : CAMP_TURN::TURN_RED;
        m_selectedCard = nullptr;
        m_gameState = GAME_STATE::STATE_PLAYER_TURN;
        skipTurn = true;
        emit sig_gameStateChanged(m_gameState);
    }
}

Card* GameController::getSelectedCard() const
{
    return m_selectedCard;
}

void GameController::addRedCard(Card *card)
{
    cards_red.append(card);
}

void GameController::addBlueCard(Card *card)
{
    cards_blue.append(card);
}

void GameController::calculateCampChange(CellUnit *placedCell)
{
    QHash<Offset,int> reinCamp = m_selectedCard->getReinCamp();
    int placerow=placedCell->getrow();
    int placecol=placedCell->getcol();
    for(QHash<Offset,int>::const_iterator i = reinCamp.constBegin();i != reinCamp.constEnd();++i){
        Offset point = i.key();
        if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
            continue;
        int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
        if(m_cells[targetindex]->getCampCard()!=CAMP::CAMP_NULL)
            continue;
        int value = i.value();
        if(m_currentTurn==CAMP_TURN::TURN_RED){
            if(m_cells[targetindex]->getCampArea()==CAMP::CAMP_RED){
                m_cells[targetindex]->addLevel(value);
            }
            else{
                m_cells[targetindex]->changeCampArea(CAMP::CAMP_RED,value);
            }
        }
        else{
            if(m_cells[targetindex]->getCampArea()==CAMP::CAMP_BLUE){
                m_cells[targetindex]->addLevel(value);
            }
            else{
                m_cells[targetindex]->changeCampArea(CAMP::CAMP_BLUE,value);
            }
        }
    }
}
