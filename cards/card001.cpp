#include "card001.h"

Card001::Card001()
{
    m_type = TYPE::NORMAL;
    m_name = "警备兵";
    m_power = 1;
    m_cost = 1;
    reinCamp.insert({-1,0},1);
    reinCamp.insert({0,1},1);
    reinCamp.insert({1,0},1);
    reinCamp.insert({0,-1},1);
}
