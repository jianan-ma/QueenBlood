#include "card008.h"

Card008::Card008()
{
    m_type = TYPE::NORMAL;
    m_name = "卡姆之牙";
    m_power = 2;
    m_cost = 1;
    reinCamp.insert({-1,0},1);
    reinCamp.insert({0,1},1);
}
