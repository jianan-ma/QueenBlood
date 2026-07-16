#include "card.h"

Card::Card(QObject *parent) : QObject(parent)
{

}

TYPE Card::getType()
{
    return m_type;
}

QString Card::getSource()
{
    return m_source;
}

QString Card::getName()
{
    return m_name;
}

int Card::getPower()
{
    return m_power;
}

int Card::getCost()
{
    return m_cost;
}

QHash<Offset,int> Card::getReinCamp()
{
    return reinCamp;
}

QHash<Offset,int> Card::getReinScore()
{
    return reinScore;
}

REIN_RANGE Card::getReinRange()
{
    return rein_range;
}

SKILL_TIMING Card::getSkillTiming()
{
    return skill_timing;
}

void Card::destroy()
{
    emit sig_destroyed();
}

void Card::skill()
{

}
