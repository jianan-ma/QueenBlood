#include "card004.h"

Card004::Card004()
{
    m_type = TYPE::NORMAL;
    m_name = "珠诺猎敌者";
    m_power = 2;
    m_cost = 2;
    reinCamp.insert({-1,0},1);
    reinCamp.insert({-1,1},1);
    reinCamp.insert({1,0},1);
    reinCamp.insert({1,1},1);
}
