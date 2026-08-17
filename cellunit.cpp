#include "cellunit.h"

CellUnit::CellUnit(quint8 row, quint8 col, QObject *parent)
{
    m_row = row;
    m_col = col;
}

CAMP CellUnit::getCampCard()
{
    return m_camp_card;
}

CAMP CellUnit::getCampArea()
{
    return m_camp_area;
}

int CellUnit::getCampLevel()
{
    return m_camplevel;
}

int CellUnit::getScore()
{
    return m_score;
}

int CellUnit::getGain()
{
    return m_gain;
}

int CellUnit::getrow()
{
    return m_row;
}

int CellUnit::getcol()
{
    return m_col;
}

QString CellUnit::getCardName()
{
    return m_card->getName();
}

Card *CellUnit::getCard()
{
    return m_card;
}

void CellUnit::changeCampArea(CAMP newCamp, int level)
{
    m_camp_area = newCamp;
    m_camplevel = level;
}

void CellUnit::changeCampCard(Card *newCard, CAMP camp_card, int score)
{
    m_card = newCard;
    connect(m_card,&Card::sig_destroyed,this,&CellUnit::onCardDestroy);
    m_camp_card = camp_card;
    m_score = score;
    isFirstUp = false;
    isFirstDown = false;
}

void CellUnit::addLevel(int lev)
{
    m_camplevel+=lev;
}

void CellUnit::setLevel(int lev)
{
    m_camplevel = lev;
}

void CellUnit::addScore(int sco)
{
    m_score+=sco;
    m_gain+=sco;
    if(m_score<=0){
        m_camp_card = CAMP::CAMP_NULL;
        m_card->destroy();
        m_score = 0;
        return;
    }
    if(m_card->getSkillTiming()==SKILL_TIMING::FIRSTTO){
        if(m_score>=m_card->gainPerCard && !isFirstTo){
            isFirstTo = true;
            emit sig_firstRein(this);
        }
    }
    if(sco>0&&!isFirstUp){
        isFirstUp = true;
        if(m_card->getSkillTiming()==SKILL_TIMING::FIRSTUP)
            emit sig_firstRein(this);
    }
    else if(sco<0&&!isFirstDown){
        isFirstDown = true;
        if(m_card->getSkillTiming()==SKILL_TIMING::FIRSTDOWN)
            emit sig_firstRein(this);
    }
}

void CellUnit::destroyCard()
{
    m_card->destroy();
    m_card = nullptr;
    m_camp_area = CAMP::CAMP_NULL;
    m_camp_card = CAMP::CAMP_NULL;
    m_camplevel = 0;
    m_score = 0;
    m_gain = 0;
    isFirstUp = false;
    isFirstDown = false;
}

void CellUnit::previewCampChange(bool ok)
{
    if(ok){
        m_original_camp_area = m_camp_area;
        m_original_camplevel = m_camplevel;
        m_isPreviewing = true;
    }
    else{
        m_camp_area = m_original_camp_area;
        m_camplevel = m_original_camplevel;
        m_isPreviewing = false;
    }
}

void CellUnit::onCardDestroy(QHash<Offset, int> reinscore, REIN_RANGE reinrange)
{
    emit sig_CardDestroy(this,reinscore,reinrange);
}
