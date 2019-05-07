#ifndef TANK_RULE_H
#define TANK_RULE_H

#include "template.h"

namespace TankGame {
bool AbleToShoot(int, int);

void MarkTheField();

void DistanceToBase(int);

std::pair<int, int> RuleDecision();
}  // namespace TankGame

#endif  // TANK_RULE_H
