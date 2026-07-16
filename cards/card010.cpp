#include "card010.h"

Card010::Card010()
{
    m_type = TYPE::NORMAL;
    m_name = "曼德拉草";
    m_power = 1;
    m_cost = 1;
    reinCamp.insert({0,1},1);
    reinCamp.insert({1,0},1);
}
