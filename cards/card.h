#ifndef CARD_H
#define CARD_H
#include <QHash>
#include <QObject>
enum class TYPE
{
    NORMAL,
    COVER
};

enum class REIN_RANGE
{
    SELF,
    OPPONENT,
    BOTH
};

enum class SKILL_TIMING
{
    PLACED,
    ALWAYS,
    DESTROY
};

struct Offset {
    int drow;
    int dcol;

    bool operator==(const Offset &other) const {
        return drow == other.drow && dcol == other.dcol;
    }
};

inline uint qHash(const Offset &offset, uint seed = 0) {
    return qHash(offset.drow, seed) ^ qHash(offset.dcol, seed);
}
class Card : public QObject
{
    Q_OBJECT
public:
    explicit Card(int mirror = 1, QObject *parent = nullptr);
    void loadCard(const QString &cardFile);
    TYPE getType();
    QString getSource();
    QString getName();
    int getPower();
    int getCost();
    QHash<Offset,int> getReinCamp();
    QHash<Offset,int> getReinScore();
    REIN_RANGE getReinRange();
    SKILL_TIMING getSkillTiming();
    virtual void destroy();
    virtual void skill();
    QVector<int> getSummonVector();
protected:
    TYPE m_type;
    QString m_source;
    QString m_name;
    int m_power;
    int m_cost;
    REIN_RANGE rein_range;
    SKILL_TIMING skill_timing;
    QHash<Offset,int> reinCamp;
    QHash<Offset,int> reinScore;
    int m_mirror=1;
    QVector<int> summon;

signals:
    void sig_destroyed();
    void sig_reinforced();
    void sig_weakened();
    void sig_addCard(Card*);
    void sig_summon(int);
};

#endif // CARD_H
