#include "card009.h"

Card009::Card009()
{
    m_type = TYPE::NORMAL;
    m_name = "掘地鼠";
    m_power = 1;
    m_cost = 2;
    reinCamp.insert({-1,1},1);
    reinCamp.insert({0,1},1);
    reinScore.insert({1,0},1);
    rein_range=REIN_RANGE::SELF;
    skill_timing=SKILL_TIMING::ALWAYS;
}
