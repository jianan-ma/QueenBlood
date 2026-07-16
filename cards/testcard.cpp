#include "testcard.h"

TestCard::TestCard()
{
    m_type = TYPE::NORMAL;
    //m_source = 3;
    m_name = "测试卡牌";
    m_power = 3;
    m_cost = 1;
    reinCamp.insert({-1,0},1);
    reinCamp.insert({0,1},1);
    reinCamp.insert({1,0},1);
    reinCamp.insert({0,-1},1);
}
