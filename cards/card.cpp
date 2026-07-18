#include "card.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

Card::Card(int mirror, QObject *parent)
{
    m_mirror = mirror;
}

void Card::loadCard(const QString &cardFile)
{
    QFile file(cardFile);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QString json_str = file.readAll();
        file.close();
        QJsonDocument docment = QJsonDocument::fromJson(json_str.toUtf8());
        QJsonObject obj = docment.object();
        m_name = obj.value("name").toString();
        m_type = TYPE(obj.value("type").toInt());
        m_power = obj.value("power").toInt();
        m_cost = obj.value("cost").toInt();
        if(obj.contains("reinCamp")){
            QJsonArray arr_camp = obj.value("reinCamp").toArray();
            for(QJsonValue value:arr_camp){
                QJsonObject obj_camp = value.toObject();
                int dr = obj_camp.value("dr").toInt();
                int dc = obj_camp.value("dc").toInt();
                int v = obj_camp.value("value").toInt();
                reinCamp.insert({dr,dc*m_mirror},v);
            }
        }
        if(obj.contains("reinScore")){
            QJsonArray arr_camp = obj.value("reinScore").toArray();
            for(QJsonValue value:arr_camp){
                QJsonObject obj_camp = value.toObject();
                int dr = obj_camp.value("dr").toInt();
                int dc = obj_camp.value("dc").toInt();
                int v = obj_camp.value("value").toInt();
                reinScore.insert({dr,dc*m_mirror},v);
            }
        }
        if(obj.contains("rein_range")){
            rein_range = REIN_RANGE(obj.value("rein_range").toInt());
        }
        if(obj.contains("skill_timing")){
            skill_timing = SKILL_TIMING(obj.value("skill_timing").toInt());
        }
    }
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
