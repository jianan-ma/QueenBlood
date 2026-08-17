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
    DESTROY,
    FIRSTUP,
    FIRSTDOWN,
    FIRSTTO,
    GAMEOVER,
    OURSIDE_DESTROY,
    OPPONENT_DESTROY
};

enum class SKILL_TYPE
{
    DESIGNATED_GAIN,
    HAND_SUMMON,
    GAINNUMBER_GAIN,
    SELF_GAIN,
    DESIGNATED_DESTROY,
    CELL_SUMMON
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
    SKILL_TYPE getSkillType();
    virtual void destroy();
    virtual void skill();
    QVector<int> getSummonVector();

    //判断场上被增益卡数量
    bool gainedPos;//true:强化 false:弱化
    bool gainedCamp;//true:己方 false:对方
    int gainPerCard=0;//每张卡增益数值,与每销毁一张卡增益值通用,与自身周次威力达到通用（克劳德）
protected:
    TYPE m_type;
    QString m_source;
    QString m_name;
    int m_power;
    int m_cost;
    REIN_RANGE rein_range;
    SKILL_TIMING skill_timing;
    SKILL_TYPE skill_type;
    QHash<Offset,int> reinCamp;
    QHash<Offset,int> reinScore;
    int m_mirror=1;
    QVector<int> summon;

signals:
    void sig_destroyed(QHash<Offset,int>,REIN_RANGE);
    void sig_reinforced();
    void sig_weakened();
    void sig_addCard(Card*);
    void sig_summon(int);
};

#endif // CARD_H
