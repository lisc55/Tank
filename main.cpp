#include "src/bot.h"
#include "src/rule.h"

using namespace TankGame;

int main() {
    srand((unsigned)time(nullptr));
    string data, globaldata;
    ReadInput(std::cin, data, globaldata);
    Bot bot(*field, field->mySide);
    Policy decision = bot.Play();
    field->DebugPrint();
    SubmitAndExit(Action(decision.act_0), Action(decision.act_1));
}
