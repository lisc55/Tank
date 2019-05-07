#include "bot.h"

inline void DecodePolicy(int p, int &c0, int &c1) {
    c1 = p & 7, p >>= 3;
    c0 = p;
}

Node *Node::NewChild(int pol) { return ch[pol] = new Node(pol, this); }

void Node::DelFather() {
    fa->ch[pol] = nullptr;
    delete fa;
    fa = nullptr;
}

inline double Bot::UCB1(Node *p) {
    return ((p->dep & 1) == role ? (p->val) : (1 - p->val)) +
           C * sqrt(log(p->fa->vis) / p->vis);
}

inline void Bot::Update(Node *p) {
    Node *bst = nullptr;
    double mx = 0, tmp;
    for (auto &i : p->ch) {
        tmp = UCB1(i.second);
        if (tmp > mx) {
            mx = tmp;
            bst = i.second;
        }
    }
    p->bstCh = bst;
}

inline bool Bot::IsFullyExpanded(Node *p) {
    if (p->full) return true;
    int turn = (p->dep & 1) == role;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (state.ActionIsValid(turn, 0, TankGame::Action(i - 1)) &&
                state.ActionIsValid(turn, 1, TankGame::Action(i - 1)) &&
                !p->ch.count(Policy(i, j)))
                return p->full = false;
    return p->full = true;
}

inline void Bot::Move(int pol) {
    if (!~pol) return;
    int c0 = (pol & 0x38) >> 15, c1 = pol & 7;
    state.nextAction[state.mySide][0] = TankGame::Action(c0 - 1),
    state.nextAction[state.mySide][1] = TankGame::Action(c1 - 1);
    state.DoAction();
}

inline Node *Bot::RandomMove(Node *p) {
    if (IsFullyExpanded(p)) return nullptr;
    while (true) {
        auto act1 = (TankGame::Action)RandBetween(TankGame::Stay,
                                                  TankGame::LeftShoot + 1);
        auto act2 = (TankGame::Action)RandBetween(TankGame::Stay,
                                                  TankGame::LeftShoot + 1);
        if (state.ActionIsValid(state.mySide, 0, act1) &&
            state.ActionIsValid(state.mySide, 1, act2) &&
            !p->ch.count(Policy(act1 - 1, act2 - 1)))
            return p->NewChild(Policy(act1 - 1, act2 - 1));
    }
}

inline void Bot::BackPropagation(Node *p, double eval) {
    while (p) {
        p->val = (p->val * p->vis + eval) / (p->vis + 1);
        p->vis++;
        Update(p);
        p = p->fa;
    }
}

inline void Bot::Expand(Node *p) {
    Node *t = p, *ch;
    for (int i = 0; i < D; i++) {
        ch = RandomMove(t);
        if (!ch) break;
        t = ch;
        Move(t->pol);
    }
    if (!p->ch.empty()) {
        delete p->ch.begin()->second;
        p->full = false;
        p->ch.clear();
    }
    BackPropagation(p, Eval());
}

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
    Expand(p);
    std::swap(state, copy);
    return true;
}

inline void Bot::Train() {
    auto timing = clock() + int(0.95 * CLOCKS_PER_SEC);
    while ((unsigned)clock() < timing) {
        for (int i = 0; i < TRAIN_UNIT; i++)
            if (!UCTSearch()) return;
    }
}

std::pair<int, int> Bot::Play() {
    Train();
    Node *nxt = nullptr;
    if (root->ch.empty()) return std::make_pair(-1, -1);
    for (auto &p : root->ch)
        if (!nxt || p.second->vis > nxt->vis) nxt = p.second;
    if (!nxt) return std::make_pair(-1, -1);
    root = nxt;
    root->DelFather();
    int x, y;
    DecodePolicy(root->pol, x, y);
    return std::make_pair(x, y);
}

inline double Bot::Eval() { return 0.0; }