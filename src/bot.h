#ifndef BOT
#define BOT

#include "template.h"

// 一次决策
class Policy {
   public:
    int val;

    Policy() {}

    Policy(int val) : val(val) {}

    // 分别是第一个和第二个坦克的命令
    Policy(int cmd0, int cmd1) { val = (cmd0 << 3) + cmd1; }

    operator int() { return val; }
};

class Node {
   public:
    int vis;
    double val;
    Node *fa, *bstCh;
    std::map<int, Node *> ch;
    // 深度和所采取的决策
    int dep, pol;
    bool full;

    Node(int pol, Node *fa = nullptr)
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

    Node *NewChild(int);

    void DelFather();
};

inline void DecodePolicy(int, int &, int &);

class Bot {
   public:
    static constexpr double C = 0.9;
    static constexpr double D = 5;
    static const int TRAIN_UNIT = 100;
    int role;

    Node *root;
    TankGame::TankField state;

    Bot(const TankGame::TankField &s, const int &r) : state(s), role(r) {}

    ~Bot() { delete root; }

    inline double UCB1(Node *);

    inline void Update(Node *);

    inline bool IsFullyExpanded(Node *);

    inline void Move(int);

    inline Node *RandomMove(Node *);

    inline void BackPropagation(Node *, double);

    inline void Expand(Node *);

    inline bool UCTSearch();

    inline void Train();

    std::pair<int, int> Play();

    inline double Eval();
};

#endif