#include "card005.h"

Card005::Card005()
{
    m_type = TYPE::NORMAL;
    m_name = "女王蜂";
    m_power = 1;
    m_cost = 1;
    reinCamp.insert({-2,0},1);
    reinCamp.insert({2,0},1);
}
