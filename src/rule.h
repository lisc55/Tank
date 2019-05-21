#ifndef TANK_RULE_H
#define TANK_RULE_H

#include "template.h"

namespace TankGame {
bool AbleToShoot(int, int, TankGame::TankField &);

void MarkTheField(TankGame::TankField &);

void DistanceToBase(int, TankGame::TankField &);

std::pair<int, int> RuleDecision(int, TankGame::TankField &);
}  // namespace TankGame

#endif  // TANK_RULE_H
