#include "bot.h"

inline bool Bot::UCTSearch() {
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

inline void Bot::Move(const std::pair<Policy, Policy> &pol) {
    if (pol.first.empty() && pol.first.empty()) return;
    state.nextAction[TankGame::Blue][0] = TankGame::Action(pol.first.act_0),
    state.nextAction[TankGame::Blue][1] = TankGame::Action(pol.second.act_1);
    state.nextAction[TankGame::Red][0] = TankGame::Action(pol.first.act_0);
    state.nextAction[TankGame::Red][1] = TankGame::Action(pol.first.act_1);
    state.DoAction();
}

inline bool Bot::IsFullyExpanded(Node *p) {
    if (p->full) return 1;
    for (int actBlue0 = -1; actBlue0 < 8; actBlue0++) {
        for (int actBlue1 = -1; actBlue1 < 8; actBlue1++) {
            for (int actRed0 = -1; actRed0 < 8; actRed0++) {
                for (int actRed1 = -1; actRed1 < 8; actRed1++) {
                    if (state.ActionIsValid(TankGame::Blue, 0,
                                            TankGame::Action(actBlue0)) &&
                        state.ActionIsValid(TankGame::Blue, 1,
                                            TankGame::Action(actBlue1)) &&
                        state.ActionIsValid(TankGame::Red, 0,
                                            TankGame::Action(actRed0)) &&
                        state.ActionIsValid(TankGame::Red, 1,
                                            TankGame::Action(actRed1)) &&
                        !p->ch.count(std::make_pair(Policy(actBlue0, actBlue1),
                                                    Policy(actRed0, actRed1))))
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

inline Node *Bot::RandomMove(Node *p) {
    if (IsFullyExpanded(p)) return nullptr;
    std::random_shuffle(shuffled_list0, shuffled_list0 + 9);
    std::random_shuffle(shuffled_list1, shuffled_list1 + 9);
    std::random_shuffle(shuffled_list2, shuffled_list2 + 9);
    std::random_shuffle(shuffled_list3, shuffled_list3 + 9);
    for (auto actBlue0 : shuffled_list0) {
        for (auto actBlue1 : shuffled_list1) {
            for (auto actRed0 : shuffled_list2) {
                for (auto actRed1 : shuffled_list3) {
                    if (state.ActionIsValid(TankGame::Blue, 0,
                                            TankGame::Action(actBlue0)) &&
                        state.ActionIsValid(TankGame::Blue, 0,
                                            TankGame::Action(actBlue1)) &&
                        state.ActionIsValid(TankGame::Red, 0,
                                            TankGame::Action(actRed0)) &&
                        state.ActionIsValid(TankGame::Red, 1,
                                            TankGame::Action(actRed1))) {
                        std::pair<Policy, Policy> pol =
                            std::make_pair(Policy(actBlue0, actBlue1),
                                           Policy(actRed0, actRed1));
                        if (!p->ch.count(pol)) { return p->NewChild(pol); }
                    }
                }
            }
        }
    }
    return nullptr;
}

inline double Bot::Utility(TankGame::GameResult res) {
    if (res == TankGame::Blue)
        return 1.0;
    else if (res == TankGame::Red)
        return 0.0;
    else if (res == TankGame::Draw)
        return 0.5;
}

// random roll-out until terminal state
inline void Bot::RollOut(Node *p) {
    Node *ch;
    for (Node *t = p;; t = ch, Move(t->pol)) {
        ch = RandomMove(t);
        if (t->ch.empty()) {
            TankGame::GameResult res = state.GetGameResult();
            if (!p->ch.empty()) {
                delete p->ch.begin()->second;
                p->full = 0;
                p->ch.clear();
            }
            BackPropagation(p, Utility(res));
            return;
        }
    }
}

inline void Bot::BackPropagation(Node *p, double utility) {
    while (p) {
        p->val = (p->val * p->vis + utility) / (p->vis + 1);
        p->vis++;
        Update(p);
        p = p->fa;
    }
}

// notice that the policy is in [-1,7], so we should +1 in the following
double val[9][9][2];
int vis[9][9][2];

// Using Decoupled UCT
inline void Bot::Update(Node *p) {
    memset(val, 0, sizeof val);
    memset(vis, 0, sizeof vis);
    for (auto &i : p->ch) {
        val[i.first.first.act_0 + 1][i.first.first.act_1 + 1][0] +=
            i.second->val;
        val[i.first.second.act_0 + 1][i.first.second.act_1 + 1][1] +=
            1 - i.second->val;
        vis[i.first.first.act_0 + 1][i.first.first.act_1 + 1][0] +=
            i.second->vis;
        vis[i.first.second.act_0 + 1][i.first.second.act_1 + 1][1] +=
            i.second->vis;
    }
    double mx = 0, tmp;
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
    mx = 0;
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
    p->bstCh = p->ch[std::make_pair(polBlue, polRed)];
}

unsigned long long timing;

inline void Bot::Train() {
    while ((unsigned)clock() < timing) {
        for (int i = 0; i < TRAIN_UNIT; i++) {
            if (!UCTSearch()) return;
        }
    }
}

Policy Bot::Play() {
    Train();
    Node *nxt = nullptr;
    if (root->ch.empty()) return Policy(-2, -2);
    for (auto &p : root->ch) {
        if (!nxt || p.second->vis > nxt->vis) nxt = p.second;
    }
    if (!nxt) return Policy(-2, -2);
    root = nxt;
    root->DelFather();
    Move(root->pol);
    if (state.mySide == TankGame::Blue)
        return root->pol.first;
    else if (state.mySide == TankGame::Red)
        return root->pol.second;
}
