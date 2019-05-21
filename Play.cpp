#include "src/bot.h"
#include "src/template.h"
#include "src/verdict.cpp"

FILE *LOG;

int main(int argc, char **argv) {
    LOG = fopen("log", "a");
    double C1 = atof(argv[1]);
    double C2 = atof(argv[2]);
    srand((unsigned)time(0));
    TankJudge::InitializeField();
    TankGame::TankField *fieldCur =
        new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary,
                                TankJudge::steelBinary, 0);
    TankGame::TankField *fieldBlue =
        new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary,
                                TankJudge::steelBinary, 0);
    TankGame::TankField *fieldRed =
        new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary,
                                TankJudge::steelBinary, 1);
    // fieldCur->DebugPrint();
    Bot botBlue(*fieldBlue, C1);
    Bot botRed(*fieldRed, C2);
    Policy polBlue = botBlue.GenDecision(1);
    Policy polRed = botRed.GenDecision(1);
    fprintf(LOG, "%d %d %d %d\n", polBlue.act_0, polBlue.act_1, polRed.act_0,
            polRed.act_1);
    fieldCur->nextAction[0][0] = TankGame::Action(polBlue.act_0);
    fieldCur->nextAction[0][1] = TankGame::Action(polBlue.act_1);
    fieldCur->nextAction[1][0] = TankGame::Action(polRed.act_0);
    fieldCur->nextAction[1][1] = TankGame::Action(polRed.act_1);
    fieldCur->DoAction();
    TankGame::GameResult res = fieldCur->GetGameResult();
    if (res != TankGame::NotFinished) {
        fprintf(LOG, "Blue_C = %.3lf, Red_C = %.3lf, Result = ", C1, C2);
        if (res == TankGame::Blue)
            fprintf(LOG, "Blue\n\n");
        else if (res == TankGame::Red)
            fprintf(LOG, "Red\n\n");
        else if (res == TankGame::Draw)
            fprintf(LOG, "Draw\n\n");
        delete fieldCur;
        delete fieldBlue;
        delete fieldRed;
        return res + 1;
    }
    std::pair<Policy, Policy> pol(polBlue, polRed);
    botBlue.Play(pol);
    botRed.Play(pol);
    // fieldCur->DebugPrint();
    while (1) {
        Policy polBlue = botBlue.GenDecision(0);
        Policy polRed = botRed.GenDecision(0);
        fprintf(LOG, "%d %d %d %d\n", polBlue.act_0, polBlue.act_1,
                polRed.act_0, polRed.act_1);
        fieldCur->nextAction[0][0] = TankGame::Action(polBlue.act_0);
        fieldCur->nextAction[0][1] = TankGame::Action(polBlue.act_1);
        fieldCur->nextAction[1][0] = TankGame::Action(polRed.act_0);
        fieldCur->nextAction[1][1] = TankGame::Action(polRed.act_1);
        fieldCur->DoAction();
        TankGame::GameResult res = fieldCur->GetGameResult();
        if (res != TankGame::NotFinished) {
            fprintf(LOG, "Blue_C = %.3lf, Red_C = %.3lf, Result = ", C1, C2);
            if (res == TankGame::Blue)
                fprintf(LOG, "Blue\n\n");
            else if (res == TankGame::Red)
                fprintf(LOG, "Red\n\n");
            else if (res == TankGame::Draw)
                fprintf(LOG, "Draw\n\n");
            delete fieldCur;
            delete fieldBlue;
            delete fieldRed;
            return res + 1;
        }
        std::pair<Policy, Policy> pol(polBlue, polRed);
        botBlue.Play(pol);
        botRed.Play(pol);
        // fieldCur->DebugPrint();
    }
    return 0;
}
