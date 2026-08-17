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
    int getGain();
    int getrow();
    int getcol();
    QString getCardName();
    Card* getCard();

    void changeCampArea(CAMP newCamp,int level);
    void changeCampCard(Card* newCard,CAMP camp_card,int score);
    void addLevel(int lev);
    void setLevel(int lev);
    void addScore(int sco);
    void destroyCard();
    void previewCampChange(bool ok);
public slots:
    void onCardDestroy(QHash<Offset,int> reinscore,REIN_RANGE reinrange);
private:
    CAMP m_camp_area = CAMP::CAMP_NULL;
    CAMP m_camp_card = CAMP::CAMP_NULL;
    int m_camplevel = 0;
    int m_score = 0;
    quint8 m_row;
    quint8 m_col;
    Card* m_card = nullptr;
    CAMP m_original_camp_area = CAMP::CAMP_NULL;
    int m_original_camplevel = 0;
    bool m_isPreviewing = false;
    bool isFirstUp = false;
    bool isFirstDown = false;
    bool isFirstTo = false;
    int m_gain = 0;
signals:
    void sig_firstRein(CellUnit*);
    void sig_CardDestroy(CellUnit*,QHash<Offset,int>,REIN_RANGE);
};

#endif // CELLUNIT_H
