#include "src/bot.h"
#include "src/rule.h"

using namespace TankGame;

const double TIME_LIMIT = 0.95;

int main() {
    freopen("test.in", "r", stdin);
    srand((unsigned)time(nullptr));
    string data, globaldata;
    ReadInput(std::cin, data, globaldata);
    Bot bot(*field);
    bot.SetTime(TIME_LIMIT + 1.0);
    Policy decision = bot.Play();
    field->DebugPrint();
    SubmitAndDontExit(Action(decision.act_0), Action(decision.act_1));
    while (1) {
        ReadInput(std::cin, data, globaldata);
        bot.SetTime(TIME_LIMIT);
        bot.Play(std::make_pair(
            Policy(field->previousActions[field->currentTurn][0][0],
                   field->previousActions[field->currentTurn][0][1]),
            Policy(field->previousActions[field->currentTurn][1][0],
                   field->previousActions[field->currentTurn][1][1])));
        decision = bot.Play();
        field->DebugPrint();
        SubmitAndDontExit(Action(decision.act_0), Action(decision.act_1));
    }
}
