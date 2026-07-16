#include "card007.h"

Card007::Card007()
{
    m_type = TYPE::NORMAL;
    m_name = "列伏利孔鸟";
    m_power = 2;
    m_cost = 1;
    reinCamp.insert({0,1},1);
    reinCamp.insert({1,0},1);
}
