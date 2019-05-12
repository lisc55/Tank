#ifndef BOT
#define BOT

#include "template.h"

// Policy, act_0, act_1 \in [-1,7], -2 represents for empty policy
class Policy {
   public:
    int act_0, act_1;

    Policy() {}
    Policy(int act_0, int act_1) : act_0(act_0), act_1(act_1) {}

    bool operator<(const Policy &rhs) const {
        return act_0 < rhs.act_0 || act_0 == rhs.act_0 && act_1 < rhs.act_1;
    }

    bool empty() const { return act_0 == -2; }
};

// Monte-Carlo Tree Search
class Node {
   public:
    int vis;
    double val;
    Node *fa, *bstCh;
    std::map<std::pair<Policy, Policy>, Node *> ch;
    int dep;
    // 0 for Blue, 1 for Red
    std::pair<Policy, Policy> pol;
    bool full;

    Node(const std::pair<Policy, Policy> pol, Node *fa = nullptr)
        : vis(0),
          val(0.0),
          fa(fa),
          bstCh(nullptr),
          dep(fa ? fa->dep + 1 : 0),
          pol(pol),
          full(false) {}

    ~Node() {
        for (auto &p : ch) delete p.second;
    }

    Node *NewChild(const std::pair<Policy, Policy> &pol) {
        return ch[pol] = new Node(pol, this);
    }

    void DelFather() {
        fa->ch[pol] = nullptr;
        delete fa;
        fa = nullptr;
    }
};

class Bot {
   public:
    static constexpr double C = 0.9;
    static const int TRAIN_UNIT = 100;
    int role;

    Node *root;
    TankGame::TankField state;

    Bot(const TankGame::TankField &s, const int &r) : role(r), state(s) {
        root = new Node(std::make_pair(Policy(-2, -2), Policy(-2, -2)));
    }
    ~Bot() { delete root; }

    inline void Update(Node *);

    inline bool IsFullyExpanded(Node *);

    inline void Move(const std::pair<Policy, Policy> &);

    inline Node *RandomMove(Node *);

    inline double Utility(TankGame::GameResult);

    inline void BackPropagation(Node *, double);

    inline void RollOut(Node *);

    inline bool UCTSearch();

    inline void Train();

    Policy Play();
};

#endif