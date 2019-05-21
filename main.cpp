#include "src/bot.h"
#include "src/rule.h"

using namespace TankGame;

const double TIME_LIMIT = 0.80;

int main() {
#ifndef _BOTZONE_ONLINE
	freopen("in", "r", stdin);
#endif
    srand((unsigned)time(0));
    string data, globaldata;
    ReadInput(std::cin, data, globaldata);
    Bot bot(*field, 0.9);
    Policy decision = bot.GenDecision(1);
    field->DebugPrint();
    SubmitAndDontExit(Action(decision.act_0), Action(decision.act_1));
    while (1) {
        ReadInput(std::cin, data, globaldata);
        bot.Play(std::make_pair(
            Policy(field->previousActions[field->currentTurn - 1][0][0],
                   field->previousActions[field->currentTurn - 1][0][1]),
            Policy(field->previousActions[field->currentTurn - 1][1][0],
                   field->previousActions[field->currentTurn - 1][1][1])));
        decision = bot.GenDecision(0);
        field->DebugPrint();
        SubmitAndDontExit(Action(decision.act_0), Action(decision.act_1));
    }
}
