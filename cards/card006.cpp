#include "card006.h"

Card006::Card006()
{
    m_type = TYPE::NORMAL;
    m_name = "生化鼠";
    m_power = 2;
    m_cost = 2;
    reinCamp.insert({1,0},1);
    reinScore.insert({1,1},-3);
    rein_range=REIN_RANGE::BOTH;
    skill_timing=SKILL_TIMING::PLACED;
}
