#ifndef CELLUNIT_H
#define CELLUNIT_H

#include <QObject>
#include "card.h"
enum class CAMP
{
    CAMP_NULL,
    CAMP_RED,
    CAMP_BLUE
};
class CellUnit : public QObject
{
    Q_OBJECT
public:
    explicit CellUnit(quint8 row,quint8 col,QObject *parent = nullptr);
    CAMP getCampArea();
    CAMP getCampCard();
    int getCampLevel();
    int getScore();
    int getrow();
    int getcol();
    QString getCardName();

    void changeCampArea(CAMP newCamp,int level);
    void changeCampCard(Card* newCard,CAMP camp_card,int score);
    void addLevel(int lev);
    void redLevel(int lev);
    void addScore(int sco);
    void redScore(int sco);
    void destroyCard();
private:
    CAMP m_camp_area = CAMP::CAMP_NULL;
    CAMP m_camp_card = CAMP::CAMP_NULL;
    int m_camplevel = 0;
    int m_score = 0;
    quint8 m_row;
    quint8 m_col;
    Card* m_card = nullptr;
signals:

};

#endif // CELLUNIT_H
