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

void GameController::reviewMode(CellUnit* placedCell)
{
    if(m_selectedCard == nullptr)
        return;
    
    if(!canPlaceCard(placedCell)) {
        recoverCampChange(nullptr);
        return;
    }
    
    recoverCampChange(nullptr);
    
    m_isPreviewing = true;
    calculateCampChange(placedCell);
    for(CellUnit* cell : m_previewedCells) {
        emit sig_cellUpdated(cell);
    }
}

bool GameController::canPlaceCard(CellUnit* cell) const
{
    if(!m_selectedCard) return false;
    if(m_selectedCard->getType()==TYPE::NORMAL){
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
    else{
        if(cell->getCampCard()==CAMP::CAMP_RED && m_currentTurn==CAMP_TURN::TURN_RED)
            return true;
        else if(cell->getCampCard()==CAMP::CAMP_BLUE && m_currentTurn==CAMP_TURN::TURN_BLUE)
            return true;
        else
            return false;
    }
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
    
    recoverCampChange(nullptr);
    
    CAMP playerCamp = (m_currentTurn == CAMP_TURN::TURN_RED) ? 
                      CAMP::CAMP_RED : CAMP::CAMP_BLUE;
    skipTurn = false;
    int oldscore = targetCell->getScore();
    targetCell->changeCampCard(m_selectedCard, playerCamp, m_selectedCard->getPower());
    int index = targetCell->getrow()*5+targetCell->getcol();
    cell_sequence.append(index);
    emit sig_cardPlaced(m_currentTurn,m_selectedCard);
    emit sig_cellUpdated(targetCell);
    calculateCoverCard(targetCell,oldscore);
    
    calculateCampChange(targetCell);
    
    calculateReinforcements(targetCell);
    summonSkill();
    calculatePlacedGainNumber();
    calculateRowsScore();
    
    m_selectedCard = nullptr;
    m_gameState = GAME_STATE::STATE_PLAYER_TURN;
    emit sig_gameStateChanged(m_gameState);
    m_currentTurn = m_currentTurn==CAMP_TURN::TURN_RED ? CAMP_TURN::TURN_BLUE : CAMP_TURN::TURN_RED;
    return true;
}

void GameController::calculateReinforcements(CellUnit* placedCell)
{
    //计算当前放置卡牌给其他卡牌的增益效果
    int placerow=placedCell->getrow();
    int placecol=placedCell->getcol();
    if((m_selectedCard->getSkillTiming()==SKILL_TIMING::PLACED || m_selectedCard->getSkillTiming()==SKILL_TIMING::ALWAYS) &&
            m_selectedCard->getSkillType()==SKILL_TYPE::DESIGNATED_GAIN && m_selectedCard->getType()==TYPE::NORMAL){
        QHash<Offset,int> reinScore = m_selectedCard->getReinScore();
        for(QHash<Offset,int>::const_iterator i = reinScore.constBegin();i != reinScore.constEnd();++i){
            Offset point = i.key();
            if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
                continue;
            int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
            CAMP cardcamp = m_cells[targetindex]->getCampCard();
            if(cardcamp==CAMP::CAMP_NULL)
                continue;
            if(m_selectedCard->getReinRange()==REIN_RANGE::SELF){
                if(m_currentTurn==CAMP_TURN::TURN_RED && cardcamp==CAMP::CAMP_RED){
                    m_cells[targetindex]->addScore(i.value());
                }
                else if(m_currentTurn==CAMP_TURN::TURN_BLUE && cardcamp==CAMP::CAMP_BLUE){
                    m_cells[targetindex]->addScore(i.value());
                }
            }
            else if(m_selectedCard->getReinRange()==REIN_RANGE::OPPONENT){
                if(m_currentTurn==CAMP_TURN::TURN_RED && cardcamp==CAMP::CAMP_BLUE){
                    m_cells[targetindex]->addScore(i.value());
                }
                else if(m_currentTurn==CAMP_TURN::TURN_BLUE && cardcamp==CAMP::CAMP_RED){
                    m_cells[targetindex]->addScore(i.value());
                }
            }
            else{
                m_cells[targetindex]->addScore(i.value());
            }
        }
    }
    //计算当前放置卡牌受其他在场时增益效果卡牌的增益
    for(CellUnit *unit:m_cells){
        if(unit->getCampCard()==CAMP::CAMP_NULL)
            continue;
        if(unit->getCard()->getSkillTiming()==SKILL_TIMING::ALWAYS && unit->getCard()->getSkillType()==SKILL_TYPE::DESIGNATED_GAIN){
            int row = unit->getrow();
            int col = unit->getcol();
            QHash<Offset,int> rein = unit->getCard()->getReinScore();
            for(QHash<Offset,int>::const_iterator i = rein.constBegin();i != rein.constEnd();++i){
                Offset pos = i.key();
                if(pos.drow+row==placerow && pos.dcol+col==placecol){
                    placedCell->addScore(i.value());
                }
            }
        }
    }
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

void GameController::calculatePlacedGainNumber()
{
    for(CellUnit *unit:m_cells){
        if(unit->getCampCard()==CAMP::CAMP_NULL)
            continue;
        if(unit->getCard()->getSkillTiming()==SKILL_TIMING::ALWAYS && unit->getCard()->getSkillType()==SKILL_TYPE::GAINNUMBER_GAIN){
            for(CellUnit *sec_unit:m_cells){
                if(sec_unit->getCampCard()==CAMP::CAMP_NULL)
                    continue;
                if(unit->getCard()->gainedCamp && sec_unit->getCampCard()==unit->getCampCard()){
                    unit->addScore(unit->getCard()->gainPerCard);
                }
                if(!unit->getCard()->gainedCamp && sec_unit->getCampCard()!=unit->getCampCard()){
                    unit->addScore(unit->getCard()->gainPerCard);
                }
            }
        }
    }
}

void GameController::summonSkill()
{
    if(m_selectedCard->getSummonVector().isEmpty())
        return;
    if(m_selectedCard->getSkillTiming()!=SKILL_TIMING::PLACED)
        return;
    //召唤至手牌
    if(m_selectedCard->getSkillType()==SKILL_TYPE::HAND_SUMMON){
        QVector<int> summonlist = m_selectedCard->getSummonVector();
        for(int id:summonlist){
            emit sig_summonCard(id);
        }
    }
    //召唤至己方场上
//    else if(m_selectedCard->getSkillType()==SKILL_TYPE::CELL_SUMMON){
//        QVector<int> summonlist = m_selectedCard->getSummonVector();

    //    }
}

void GameController::calculateCoverCard(CellUnit* placedCell,int oldscore)
{
    if(m_selectedCard->getType()!=TYPE::COVER)
        return;
    int placerow=placedCell->getrow();
    int placecol=placedCell->getcol();
//    if(m_selectedCard->getSkillTiming()==SKILL_TIMING::PLACED && m_selectedCard->getSkillType()==SKILL_TYPE::DESIGNATED_GAIN){
        QHash<Offset,int> reinScore = m_selectedCard->getReinScore();
        for(QHash<Offset,int>::const_iterator i = reinScore.constBegin();i != reinScore.constEnd();++i){
            Offset point = i.key();
            if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
                continue;
            int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
            CAMP cardcamp = m_cells[targetindex]->getCampCard();
            if(cardcamp==CAMP::CAMP_NULL)
                continue;
            if(m_selectedCard->getReinRange()==REIN_RANGE::SELF){
                if(m_currentTurn==CAMP_TURN::TURN_RED && cardcamp==CAMP::CAMP_RED){
                    m_cells[targetindex]->addScore(oldscore*i.value());
                }
                else if(m_currentTurn==CAMP_TURN::TURN_BLUE && cardcamp==CAMP::CAMP_BLUE){
                    m_cells[targetindex]->addScore(oldscore*i.value());
                }
            }
            else if(m_selectedCard->getReinRange()==REIN_RANGE::OPPONENT){
                if(m_currentTurn==CAMP_TURN::TURN_RED && cardcamp==CAMP::CAMP_BLUE){
                    m_cells[targetindex]->addScore(oldscore*i.value());
                }
                else if(m_currentTurn==CAMP_TURN::TURN_BLUE && cardcamp==CAMP::CAMP_RED){
                    m_cells[targetindex]->addScore(oldscore*i.value());
                }
            }
            else{
                m_cells[targetindex]->addScore(oldscore*i.value());
            }
        }
//    }
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
        qDebug()<<"turn"<<int(m_currentTurn);
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

void GameController::whenFirstRein(CellUnit *unit)
{
    int placerow=unit->getrow();
    int placecol=unit->getcol();
    QHash<Offset,int> reinScore = unit->getCard()->getReinScore();
    REIN_RANGE reinrange = unit->getCard()->getReinRange();
    for(QHash<Offset,int>::const_iterator i = reinScore.constBegin();i != reinScore.constEnd();++i){
        Offset point = i.key();
        if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
            continue;
        int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
        CAMP cardcamp = m_cells[targetindex]->getCampCard();
        if(cardcamp==CAMP::CAMP_NULL)
            continue;
        if(reinrange==REIN_RANGE::SELF && unit->getCampCard()==cardcamp){
            m_cells[targetindex]->addScore(i.value());
        }
        if(reinrange==REIN_RANGE::OPPONENT && unit->getCampCard()!=cardcamp){
            m_cells[targetindex]->addScore(i.value());
        }
        if(reinrange==REIN_RANGE::BOTH){
            m_cells[targetindex]->addScore(i.value());
        }
    }
}

void GameController::onCardDestroy(CellUnit *unit, QHash<Offset, int> reinscore, REIN_RANGE reinrange)
{
    //计算销毁时对指定格指定方进行增益
    int placerow=unit->getrow();
    int placecol=unit->getcol();
    for(QHash<Offset,int>::const_iterator i = reinscore.constBegin();i != reinscore.constEnd();++i){
        Offset point = i.key();
        if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
            continue;
        int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
        CAMP cardcamp = m_cells[targetindex]->getCampCard();
        if(cardcamp==CAMP::CAMP_NULL)
            continue;
        if(reinrange==REIN_RANGE::SELF){
            if(cardcamp==unit->getCampCard())
                m_cells[targetindex]->addScore(i.value());
        }
        else if(reinrange==REIN_RANGE::OPPONENT){
            if(cardcamp!=unit->getCampCard())
                m_cells[targetindex]->addScore(i.value());
        }
        else{
            m_cells[targetindex]->addScore(i.value());
        }
    }
    //计算指定方销毁时对自身增益
    for(CellUnit *punit:m_cells){
        if(punit->getCampCard()==CAMP::CAMP_NULL)
            continue;
        if(punit==unit)
            continue;
        if(punit->getCard()->getSkillTiming()==SKILL_TIMING::OURSIDE_DESTROY && punit->getCampCard()==unit->getCampCard()){
            punit->addScore(punit->getCard()->gainPerCard);
        }
        if(punit->getCard()->getSkillTiming()==SKILL_TIMING::OPPONENT_DESTROY && punit->getCampCard()!=unit->getCampCard()){
            punit->addScore(punit->getCard()->gainPerCard);
        }
        //计算因销毁受增益的卡牌而导致根据增益卡牌数量对自身增益卡牌的增益消失
        if(punit->getCard()->getSkillTiming()==SKILL_TIMING::ALWAYS && punit->getCard()->getSkillType()==SKILL_TYPE::GAINNUMBER_GAIN){
            if(punit->getCard()->gainedCamp && punit->getCampCard()==unit->getCampCard()){
                punit->addScore(-punit->getCard()->gainPerCard);
            }
            if(!punit->getCard()->gainedCamp && punit->getCampCard()!=unit->getCampCard()){
                punit->addScore(-punit->getCard()->gainPerCard);
            }
        }
    }
    //计算在场时增益的卡牌因为销毁失去对指定格的增益效果
    if(unit->getCard()->getSkillTiming()==SKILL_TIMING::ALWAYS && unit->getCard()->getSkillType()==SKILL_TYPE::DESIGNATED_GAIN){
        QHash<Offset,int> reinScore = unit->getCard()->getReinScore();
        REIN_RANGE reinRange = unit->getCard()->getReinRange();
        for(QHash<Offset,int>::const_iterator i = reinScore.constBegin();i != reinScore.constEnd();++i){
            Offset point = i.key();
            if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
                continue;
            int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
            CAMP cardcamp = m_cells[targetindex]->getCampCard();
            if(cardcamp==CAMP::CAMP_NULL)
                continue;
            if(reinRange==REIN_RANGE::SELF && unit->getCampCard()==cardcamp){
                m_cells[targetindex]->addScore(-i.value());
            }
            else if(reinRange==REIN_RANGE::OPPONENT && unit->getCampCard()!=cardcamp){
                m_cells[targetindex]->addScore(-i.value());
            }
            else if(reinRange==REIN_RANGE::BOTH){
                m_cells[targetindex]->addScore(-i.value());
            }
        }
    }
    //重新计算阵地等级
    int index = placerow*5+placecol;
    cell_sequence.removeOne(index);
    m_cells[0]->setLevel(1);
    m_cells[5]->setLevel(1);
    m_cells[10]->setLevel(1);
    m_cells[4]->setLevel(1);
    m_cells[9]->setLevel(1);
    m_cells[14]->setLevel(1);
    m_cells[1]->setLevel(0);
    m_cells[2]->setLevel(0);
    m_cells[3]->setLevel(0);
    m_cells[6]->setLevel(0);
    m_cells[7]->setLevel(0);
    m_cells[8]->setLevel(0);
    m_cells[11]->setLevel(0);
    m_cells[12]->setLevel(0);
    m_cells[13]->setLevel(0);
    for(int i:cell_sequence){
        QHash<Offset,int> reinCamp = m_cells[i]->getCard()->getReinCamp();
        placerow = m_cells[i]->getrow();
        placecol = m_cells[i]->getcol();
        for(QHash<Offset,int>::const_iterator it = reinCamp.constBegin();it != reinCamp.constEnd();++it){
            Offset point = it.key();
            if(placerow+point.drow<0 || placerow+point.drow>2 || placecol+point.dcol<0 || placecol+point.dcol>4)
                continue;
            int targetindex = (placerow+point.drow)*5+placecol+point.dcol;
            if(m_cells[targetindex]->getCampCard()!=CAMP::CAMP_NULL)
                continue;
            if(m_cells[i]->getCampCard()==m_cells[targetindex]->getCampArea())
                m_cells[targetindex]->addLevel(it.value());
            else
                m_cells[targetindex]->changeCampArea(m_cells[i]->getCampCard(),it.value());
        }
    }
    unit->destroyCard();
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
        
        if(m_isPreviewing){
            m_cells[targetindex]->previewCampChange(true);
            m_previewedCells.append(m_cells[targetindex]);
        }
        
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

void GameController::recoverCampChange(CellUnit* placedCell)
{
    Q_UNUSED(placedCell);
    
    if(!m_isPreviewing || m_selectedCard==nullptr)
        return;
    
    m_isPreviewing = false;
    
    for(CellUnit* cell : m_previewedCells) {
        cell->previewCampChange(false);
        emit sig_cellUpdated(cell);
    }
    
    m_previewedCells.clear();
}
