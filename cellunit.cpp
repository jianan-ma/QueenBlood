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

void CellUnit::changeCampArea(CAMP newCamp, int level)
{
    m_camp_area = newCamp;
    m_camplevel = level;
}

void CellUnit::changeCampCard(Card *newCard, CAMP camp_card, int score)
{
    m_card = newCard;
    m_camp_card = camp_card;
    m_score = score;
}

void CellUnit::addLevel(int lev)
{
    m_camplevel+=lev;
}

void CellUnit::redLevel(int lev)
{
    if(m_camplevel>=lev)
        m_camplevel-=lev;
    else
        m_camplevel=0;
}

void CellUnit::addScore(int sco)
{
    m_score+=sco;
}

void CellUnit::redScore(int sco)
{
    if(m_score>sco)
        m_score-=sco;
    else{
        m_score=0;
        destroyCard();
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
