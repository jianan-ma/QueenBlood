#include "card002.h"

Card002::Card002()
{
    m_type = TYPE::NORMAL;
    m_name = "镇压兵";
    m_power = 3;
    m_cost = 2;
    reinCamp.insert({-2,0},1);
    reinCamp.insert({-1,0},1);
    reinCamp.insert({0,1},1);
    reinCamp.insert({1,0},1);
    reinCamp.insert({2,0},1);
}
