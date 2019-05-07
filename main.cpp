#include "src/bot.h"
#include "src/rule.h"

using namespace TankGame;

int main() {
    srand((unsigned)time(nullptr));
    string data, globaldata;
    ReadInput(std::cin, data, globaldata);
    // std::pair<int, int> decision = RuleDecision();
    Bot bot(*field, field->mySide);
    std::pair<int, int> decision = bot.Play();
    field->DebugPrint();
    SubmitAndExit(Action(decision.first), Action(decision.second));
}
