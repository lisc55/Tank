#include "bot.h"
#include <cassert>
#include "rule.h"

void Print(Node *p) {
    for (auto i : p->ch) {
        printf("((%d, %d), (%d, %d)) %p\n", i.first.first.act_0, i.first.first.act_1, i.first.second.act_0,
               i.first.second.act_1, i.second);
    }
    puts("");
}

std::pair<double, double> Bot::Eval(Node *p, TankGame::TankField &s) {
    double resb, resr;
    TankGame::MarkTheField(s);
    TankGame::DistanceToBase(0, s);
    TankGame::DistanceToBase(1, s);
    // 距离和中枪的评估，红蓝互为相反数
    resb = (-s.distance[1][s.tankY[0][0]][s.tankX[0][0]] - s.distance[1][s.tankY[0][1]][s.tankX[0][1]] +
            s.distance[0][s.tankY[1][0]][s.tankX[1][0]] + s.distance[0][s.tankY[1][1]][s.tankX[1][1]]) *
               dist_c +
           (-s.mark[0][s.tankY[0][0]][s.tankX[0][0]] - s.mark[0][s.tankY[0][1]][s.tankX[0][1]] +
            s.mark[1][s.tankY[1][0]][s.tankX[1][0]] + s.mark[1][s.tankY[1][1]][s.tankX[1][1]]) *
               shoot_c;
    resr = -resb;
    return std::make_pair(resb, resr);
}

std::pair<double, double> Bot::Penalty(Node *p, TankGame::TankField &s) {
    double resb = 0, resr = 0;
    // 注意s要返回之前的局面一下
    if (!s.Revert()) return std::make_pair(resb, resr);
    // 向空地、钢铁和队友射击的惩罚
    short noeff_penalty[2] = {2, 2}, self_kill_penalty[2] = {0};
    // 看看在向什么地方射击
    for (int side = 0; side < TankGame::sideCount; side++)
        for (int tank = 0; tank < TankGame::tankPerSide; tank++) {
            TankGame::Action act = s.previousActions[s.currentTurn][side][tank];
            if (ActionIsShoot(act)) {
                int dir = ExtractDirectionFromAction(act);
                int x = s.tankX[side][tank], y = s.tankY[side][tank];
                while (true) {
                    x += TankGame::dx[dir];
                    y += TankGame::dy[dir];
                    if (!TankGame::CoordValid(x, y)) break;
                    TankGame::FieldItem items = s.gameField[y][x];
                    if (items != TankGame::None && items != TankGame::Water && items != TankGame::Steel) {
                        if (x == s.tankX[side][!tank] && y == s.tankY[side][!tank]) self_kill_penalty[side]++;
                        noeff_penalty[side]--;
                        break;
                    }
                }
            } else
                noeff_penalty[side]--;
        }
    resb -= noeff_penalty[0] * noteff_shoot_c + self_kill_penalty[0] * self_kill_c;
    resr -= noeff_penalty[1] * noteff_shoot_c + self_kill_penalty[1] * self_kill_c;

    // 如果和纯规则的策略一样，给予适当的奖励
    std::pair<int, int> ruleBlue = TankGame::RuleDecision(0, s);
    std::pair<int, int> ruleRed = TankGame::RuleDecision(1, s);
    if (p->fa->pol.first == Policy(ruleBlue.first, ruleBlue.second)) resb += rule_c;
    if (p->fa->pol.second == Policy(ruleRed.first, ruleRed.second)) resb += rule_c;
    return std::make_pair(resb, resr);
}

bool Bot::MCTS() {
    TankGame::TankField copy = state;
    Node *p = root;
    for (; p; p = p->bstCh) {
        if (p != root) Move(p->pol);
        if (!IsFullyExpanded(p)) break;
    }
    if (!p) return std::swap(state, copy), 0;
    p = RandomMove(p);
    Move(p->pol);
    RollOut(p);
    std::swap(state, copy);
    return 1;
}

void Bot::Move(const std::pair<Policy, Policy> &pol) {
    if (pol.first.empty() && pol.second.empty()) return;
    state.nextAction[0][0] = TankGame::Action(pol.first.act_0),
    state.nextAction[0][1] = TankGame::Action(pol.first.act_1);
    state.nextAction[1][0] = TankGame::Action(pol.second.act_0);
    state.nextAction[1][1] = TankGame::Action(pol.second.act_1);
    state.DoAction();
    // state.DebugPrint();
}

bool Bot::IsFullyExpanded(Node *p) {
    if (p->full) return 1;
    for (int actBlue0 = -1; actBlue0 < 8; actBlue0++) {
        if (!state.tankAlive[0][0] && actBlue0 >= 0) break;
        for (int actBlue1 = -1; actBlue1 < 8; actBlue1++) {
            if (!state.tankAlive[0][1] && actBlue1 >= 0) break;
            for (int actRed0 = -1; actRed0 < 8; actRed0++) {
                if (!state.tankAlive[1][0] && actRed0 >= 0) break;
                for (int actRed1 = -1; actRed1 < 8; actRed1++) {
                    if (!state.tankAlive[1][1] && actRed1 >= 0) break;
                    if (state.ActionIsValid(0, 0, TankGame::Action(actBlue0)) &&
                        state.ActionIsValid(0, 1, TankGame::Action(actBlue1)) &&
                        state.ActionIsValid(1, 0, TankGame::Action(actRed0)) &&
                        state.ActionIsValid(1, 1, TankGame::Action(actRed1)) &&
                        !p->ch.count(std::make_pair(Policy(actBlue0, actBlue1), Policy(actRed0, actRed1))))
                        return p->full = 0;
                }
            }
        }
    }
    return p->full = 1;
}

int shuffled_list0[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
int shuffled_list1[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
int shuffled_list2[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};
int shuffled_list3[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7};

Node *Bot::RandomMove(Node *p) {
    if (IsFullyExpanded(p)) return nullptr;
    std::random_shuffle(shuffled_list0, shuffled_list0 + 9);
    std::random_shuffle(shuffled_list1, shuffled_list1 + 9);
    std::random_shuffle(shuffled_list2, shuffled_list2 + 9);
    std::random_shuffle(shuffled_list3, shuffled_list3 + 9);
    for (auto actBlue0 : shuffled_list0) {
        if (!state.tankAlive[0][0]) actBlue0 = -1;
        for (auto actBlue1 : shuffled_list1) {
            if (!state.tankAlive[0][1]) actBlue1 = -1;
            for (auto actRed0 : shuffled_list2) {
                if (!state.tankAlive[1][0]) actRed0 = -1;
                for (auto actRed1 : shuffled_list3) {
                    if (!state.tankAlive[1][1]) actRed1 = -1;
                    if (state.ActionIsValid(0, 0, TankGame::Action(actBlue0)) &&
                        state.ActionIsValid(0, 1, TankGame::Action(actBlue1)) &&
                        state.ActionIsValid(1, 0, TankGame::Action(actRed0)) &&
                        state.ActionIsValid(1, 1, TankGame::Action(actRed1))) {
                        std::pair<Policy, Policy> pol =
                            std::make_pair(Policy(actBlue0, actBlue1), Policy(actRed0, actRed1));
                        if (!p->ch.count(pol)) { return p->NewChild(pol); }
                    }
                    if (!state.tankAlive[1][1]) break;
                }
                if (!state.tankAlive[1][0]) break;
            }
            if (!state.tankAlive[0][1]) break;
        }
        if (!state.tankAlive[0][0]) break;
    }
    return nullptr;
}

// roll-out
void Bot::RollOut(Node *p) {
    Node *ch;
    int count = 0;
    std::pair<double, double> pen = Penalty(p, state);
    Move(p->pol);
    for (Node *t = p;; t = ch, Move(t->pol)) {
        ch = RandomMove(t);
        if (++count > rollOut) break;
    }
    if (!p->ch.empty()) {
        delete p->ch.begin()->second;
        p->full = 0;
        p->ch.clear();
    }
    std::pair<double, double> eva = Eval(p, state);
    eva.first += pen.first;
    eva.second += pen.second;
    BackPropagation(p, eva);
}

void Bot::BackPropagation(Node *p, const std::pair<double, double> &u) {
    while (p) {
        p->val.first += u.first;
        p->val.second += u.second;
        p->vis++;
        Update(p);
        p = p->fa;
    }
}

// Using Decoupled UCT
void Bot::Update(Node *p) {
    if (p->ch.empty()) {
        p->bstCh = nullptr;
        return;
    }
    memset(val, 0, sizeof val);
    memset(vis, 0, sizeof vis);
    for (auto &i : p->ch) {
        // if(i.first.first.act_0==-2){
        //     puts("----");
        // }
        val[i.first.first.act_0 + 1][i.first.first.act_1 + 1][0] += i.second->val.first;
        val[i.first.second.act_0 + 1][i.first.second.act_1 + 1][1] += i.second->val.second;
        vis[i.first.first.act_0 + 1][i.first.first.act_1 + 1][0] += i.second->vis;
        vis[i.first.second.act_0 + 1][i.first.second.act_1 + 1][1] += i.second->vis;
    }
    double mx = -1, tmp;
    int act_1 = -1, act_2 = -1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (vis[i][j][0]) {
                tmp = val[i][j][0] / vis[i][j][0] +  // Expectaion
                      C * sqrt(log(p->vis) / vis[i][j][0]);  // UCB length
                if (tmp > mx) {
                    mx = tmp;
                    act_1 = i, act_2 = j;
                }
            }
        }
    }
    Policy polBlue(act_1 - 1, act_2 - 1);
    mx = -1;
    act_1 = -1, act_2 = -1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (vis[i][j][1]) {
                tmp = val[i][j][1] / vis[i][j][1] +  // Expectaion
                      C * sqrt(log(p->vis) / vis[i][j][1]);  // UCB length
                if (tmp > mx) {
                    mx = tmp;
                    act_1 = i, act_2 = j;
                }
            }
        }
    }
    Policy polRed(act_1 - 1, act_2 - 1);
    std::pair<Policy, Policy> pol = std::make_pair(polBlue, polRed);
    if (!p->ch.count(pol))
        p->bstCh = p->NewChild(pol);
    else
        p->bstCh = p->ch[pol];
}

int Bot::Train() {
    int ret = 0;
#ifndef DEBUG
    while ((unsigned)clock() < timing) {
#else
    for (int __T = 0; __T < 60; __T++) {
#endif
        // printf("%d\n", root->CountSize());
        for (int i = 0; i < TRAIN_UNIT; i++) {
            if (!MCTS()) return ret;
        }
        ret++;
    }
    return ret;
}

// return mySide policy
Policy Bot::GenDecision(bool first) {
    if (first)
        timing = clock() + int((1.0 + TIME_LIMIT) * CLOCKS_PER_SEC);
    else
        timing = clock() + int(TIME_LIMIT * CLOCKS_PER_SEC);
    int res = Train();
    // printf("Train rounds: %d\n", res);
    if (root->ch.empty()) return Policy(-2, -2);
    memset(vis, 0, sizeof vis);
    memset(val, 0, sizeof val);
    bool flag = state.mySide == 1;
    if (flag) {
        for (auto &p : root->ch) {
            vis[p.first.second.act_0 + 1][p.first.second.act_1 + 1][1] += p.second->vis;
            val[p.first.second.act_0 + 1][p.first.second.act_1 + 1][1] += p.second->val.second;
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (vis[i][j][1])
                    val[i][j][0] = val[i][j][1] / vis[i][j][1];
                else
                    val[i][j][0] = 0;
            }
        }
    } else {
        for (auto &p : root->ch) {
            vis[p.first.first.act_0 + 1][p.first.first.act_1 + 1][0] += p.second->vis;
            val[p.first.first.act_0 + 1][p.first.first.act_1 + 1][0] += p.second->val.first;
        }
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (vis[i][j][0])
                    val[i][j][0] = val[i][j][0] / vis[i][j][0];
                else
                    val[i][j][0] = 0;
            }
        }
    }
    // for (int i = 0; i < 9; i++) {
    //     for (int j = 0; j < 9; j++) { printf("%.2lf ", val[i][j][0]); }
    //     puts("");
    // }
    double max = 0;
    int act_0 = -1, act_1 = -1;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (max < val[i][j][0]) {
                max = val[i][j][0];
                act_0 = i, act_1 = j;
            }
        }
    }
    return Policy(act_0 - 1, act_1 - 1);
}

void Bot::Play(const std::pair<Policy, Policy> &pol) {
    if (!root->ch.count(pol))
        root = root->NewChild(pol);
    else
        root = root->ch[pol];
    root->DelFather();
    Move(root->pol);
}
