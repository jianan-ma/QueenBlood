#include "card003.h"

Card003::Card003()
{
    m_type = TYPE::NORMAL;
    m_name = "掷弹兵";
    m_power = 1;
    m_cost = 2;
    reinScore.insert({0,2},-4);
    rein_range = REIN_RANGE::OPPONENT;
    skill_timing = SKILL_TIMING::PLACED;
}
