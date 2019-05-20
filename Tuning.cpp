#include "src/bot.h"
#include "src/template.h"
#include "src/verdict.cpp"

int main(){
    srand((unsigned)time(0));
    TankJudge::InitializeField();
    TankGame::TankField *field = new TankGame::TankField(TankJudge::fieldBinary,TankJudge::waterBinary,TankJudge::steelBinary, 0);
    TankGame::TankField *fieldBlue = new TankGame::TankField(TankJudge::fieldBinary,TankJudge::waterBinary,TankJudge::steelBinary, 0);
    TankGame::TankField *fieldRed = new TankGame::TankField(TankJudge::fieldBinary,TankJudge::waterBinary,TankJudge::steelBinary, 1);
    field->DebugPrint();
    Bot botBlue(*fieldBlue);
    Bot botRed(*fieldRed);
    Policy polBlue = botBlue.GenDecision(1);
    Policy polRed = botRed.GenDecision(1);
    printf("%d %d %d %d\n", polBlue.act_0, polBlue.act_1, polRed.act_0, polRed.act_1);
    field->nextAction[0][0] = TankGame::Action(polBlue.act_0);
    field->nextAction[0][1] = TankGame::Action(polBlue.act_1);
    field->nextAction[1][0] = TankGame::Action(polRed.act_0);
    field->nextAction[1][1] = TankGame::Action(polRed.act_1);
    field->DoAction();
    TankGame::GameResult res = field->GetGameResult();
    if(res != TankGame::NotFinished){
        if (res == TankGame::Blue) {
            puts("Blue");
        }
        else if (res == TankGame::Red) {
            puts("Red");
        }
        else if (res == TankGame::Draw) {
            puts("Draw");
        }
        return 0;
    }
    std::pair<Policy, Policy> pol(polBlue, polRed);
    botBlue.Play(pol);
    botRed.Play(pol);
    field->DebugPrint();
    while (1) {
        Policy polBlue = botBlue.GenDecision(0);
        Policy polRed = botRed.GenDecision(0);
        printf("%d %d %d %d\n", polBlue.act_0, polBlue.act_1, polRed.act_0, polRed.act_1);
        field->nextAction[0][0] = TankGame::Action(polBlue.act_0);
        field->nextAction[0][1] = TankGame::Action(polBlue.act_1);
        field->nextAction[1][0] = TankGame::Action(polRed.act_0);
        field->nextAction[1][1] = TankGame::Action(polRed.act_1);
        field->DoAction();
        TankGame::GameResult res = field->GetGameResult();
        if(res != TankGame::NotFinished){
            if (res == TankGame::Blue) {
                puts("Blue");
            }
            else if (res == TankGame::Red) {
                puts("Red");
            }
            else if (res == TankGame::Draw) {
                puts("Draw");
            }
            return 0;
        }
        std::pair<Policy, Policy> pol(polBlue, polRed);
        botBlue.Play(pol);
        botRed.Play(pol);
        field->DebugPrint();
    }
    return 0;
}

