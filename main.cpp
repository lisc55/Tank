#include "src/rule.h"

using namespace TankGame;

int main() {
    srand((unsigned)time(nullptr));
    string data, globaldata;
    ReadInput(std::cin, data, globaldata);
    std::pair<int, int> decision = RuleDecision();
    field->DebugPrint();
    SubmitAndExit(Action(decision.first), Action(decision.second));
}
