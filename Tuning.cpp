#include "src/bot.h"
#include "src/template.h"
#include "src/verdict.cpp"

FILE *LOG, *RES;

TankGame::GameResult play(double C1, double C2) {
    TankJudge::InitializeField();
    TankGame::TankField *field =
        new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary,
                                TankJudge::steelBinary, 0);
    TankGame::TankField *fieldBlue =
        new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary,
                                TankJudge::steelBinary, 0);
    TankGame::TankField *fieldRed =
        new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary,
                                TankJudge::steelBinary, 1);
    // field->DebugPrint();
    Bot botBlue(*fieldBlue, C1);
    Bot botRed(*fieldRed, C2);
    Policy polBlue = botBlue.GenDecision(1);
    Policy polRed = botRed.GenDecision(1);
    fprintf(LOG, "%d %d %d %d\n", polBlue.act_0, polBlue.act_1, polRed.act_0,
            polRed.act_1);
    field->nextAction[0][0] = TankGame::Action(polBlue.act_0);
    field->nextAction[0][1] = TankGame::Action(polBlue.act_1);
    field->nextAction[1][0] = TankGame::Action(polRed.act_0);
    field->nextAction[1][1] = TankGame::Action(polRed.act_1);
    field->DoAction();
    TankGame::GameResult res = field->GetGameResult();
    if (res != TankGame::NotFinished) {
        fprintf(LOG, "Blue_C = %.3lf, Red_C = %.3lf, Result = ", C1, C2);
        if (res == TankGame::Blue)
            fprintf(LOG, "Blue\n\n");
        else if (res == TankGame::Red)
            fprintf(LOG, "Red\n\n");
        else
            fprintf(LOG, "Draw\n\n");
        delete field;
        delete fieldBlue;
        delete fieldRed;
        return res;
    }
    std::pair<Policy, Policy> pol(polBlue, polRed);
    botBlue.Play(pol);
    botRed.Play(pol);
    // field->DebugPrint();
    while (1) {
        Policy polBlue = botBlue.GenDecision(0);
        Policy polRed = botRed.GenDecision(0);
        fprintf(LOG, "%d %d %d %d\n", polBlue.act_0, polBlue.act_1,
                polRed.act_0, polRed.act_1);
        field->nextAction[0][0] = TankGame::Action(polBlue.act_0);
        field->nextAction[0][1] = TankGame::Action(polBlue.act_1);
        field->nextAction[1][0] = TankGame::Action(polRed.act_0);
        field->nextAction[1][1] = TankGame::Action(polRed.act_1);
        field->DoAction();
        TankGame::GameResult res = field->GetGameResult();
        if (res != TankGame::NotFinished) {
            fprintf(LOG, "Blue_C = %.3lf, Red_C = %.3lf, Result = ", C1, C2);
            if (res == TankGame::Blue)
                fprintf(LOG, "Blue\n\n");
            else if (res == TankGame::Red)
                fprintf(LOG, "Red\n\n");
            else
                fprintf(LOG, "Draw\n\n");
            delete field;
            delete fieldBlue;
            delete fieldRed;
            return res;
        }
        std::pair<Policy, Policy> pol(polBlue, polRed);
        botBlue.Play(pol);
        botRed.Play(pol);
        // field->DebugPrint();
    }
}

int main() {
    srand((unsigned)time(0));
    LOG = fopen("log", "w");
    RES = fopen("res", "w");
    std::vector<double> C_list = {0.2, 0.4, 0.6, 0.8, 1.0};
    std::map<double, int> result;
    for (auto i : C_list) {
        for (auto j : C_list) {
            if (abs(i - j) > 1e-5) {
                int T = 5;
                for (int _T = 1; _T < T; _T++) {
                    printf("%.3lf %.3lf round %d", i, j, _T);
                    TankGame::GameResult res = play(i, j);
                    if (res == TankGame::Blue)
                        result[i] += 2;
                    else if (res == TankGame::Red)
                        result[j] += 2;
                    else
                        result[i] += 1, result[j] += 1;
                }
            }
        }
    }
    for (auto i : result) { fprintf(RES, "%.3lf %d\n", i.first, i.second); }
    puts("FINISHED");
    return 0;
}
