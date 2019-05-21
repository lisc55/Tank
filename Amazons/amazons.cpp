#ifdef _BOTZONE_ONLINE

#define JSON_MODE

#else

#define LOCAL_MODE

#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

typedef long long LL;
typedef unsigned long long ULL;

const double C = 0.4;
const double KAPPA = 0.0;
const double SIG_THRES = 1;
const double BEGIN = 33.0;
const double MIDUM = 19.0;
const double PENAL_MIN = 0.0;
const int D = 5;

const int TRAIN_UNIT = 100;
const double TIME_LIMIT = 0.95;

// 0blank  1black  2white  3obstacle

const int SIZE = 8;
const int dx[] = {1, 1, 0, -1, -1, -1, 0, 1};
const int dy[] = {0, -1, -1, -1, 0, 1, 1, 1};
const int init_pos[8][2] = {{0, 2}, {2, 0}, {5, 0}, {7, 2}, {0, 5}, {2, 7}, {5, 7}, {7, 5}};
const double INF = 1e10;

const int FACT8 = 40320;
const int FACT4 = 24;

int PERM8[FACT8 + 5][8];
int PERM4[FACT4 + 5][4];

ULL timing;

struct state {
    int a[SIZE][SIZE];
    int pos[8][2];
    state() {
        memset(a, 0, sizeof a);
        a[0][2] = a[2][0] = a[5][0] = a[7][2] = 1;
        a[0][5] = a[2][7] = a[5][7] = a[7][5] = 2;
        for (int i = 0; i < 8; i++) pos[i][0] = init_pos[i][0], pos[i][1] = init_pos[i][1];
    }
    int *operator[](int x) { return a[x]; }
};

state A;

inline bool out_of_bound(int x, int y) { return x < 0 || x >= SIZE || y < 0 || y >= SIZE; }

inline bool eq(double a, double b) { return abs(a - b) < 1e-6; }

inline int policy(int x_0, int y_0, int x_1, int y_1, int x_2, int y_2) {
    return (x_0 << 15) + (y_0 << 12) + (x_1 << 9) + (y_1 << 6) + (x_2 << 3) + y_2;
}

inline void trans_policy(int p, int &x_0, int &y_0, int &x_1, int &y_1, int &x_2, int &y_2) {
    y_2 = p & 7, p >>= 3;
    x_2 = p & 7, p >>= 3;
    y_1 = p & 7, p >>= 3;
    x_1 = p & 7, p >>= 3;
    y_0 = p & 7, p >>= 3;
    x_0 = p;
}

struct node {
    int vis;
    double val;
    node *fa, *bst_ch;
    map<int, node *> ch;
    int dep, pol;
    bool full;
    node(int pol, node *fa = nullptr)
        : vis(0), val(0.0), fa(fa), bst_ch(nullptr), dep(fa ? fa->dep + 1 : 0), pol(pol), full(0) {}
    ~node() {
        for (auto &p : ch) delete p.second;
    }
    node *new_child(int pol) { return ch[pol] = new node(pol, this); }
    void del_fa() {
        fa->ch[pol] = nullptr;
        delete fa;
        fa = nullptr;
    }
};

node *root = new node(-1);

inline double UCB1(node *p) { return ((p->dep & 1) ? (p->val) : (1 - p->val)) + C * sqrt(log(p->fa->vis) / p->vis); }

void update(node *p) {
    node *bst = nullptr;
    double mx = 0, tmp;
    for (auto &i : p->ch) {
        tmp = UCB1(i.second);
        if (tmp > mx) {
            mx = tmp;
            bst = i.second;
        }
    }
    p->bst_ch = bst;
}

inline bool is_fully_expanded(node *p) {
    if (p->full) return 1;
    int Dx, Dy, DX, DY;
    for (int id = (p->dep & 1) ? 4 : 0, U = (p->dep & 1) ? 8 : 4; id < U; id++) {
        int cur_x = A.pos[id][0], cur_y = A.pos[id][1];
        for (int dir1 = 0; dir1 < 8; dir1++) {
            Dx = dx[dir1], Dy = dy[dir1];
            for (int nxt_x = cur_x + Dx, nxt_y = cur_y + Dy; !out_of_bound(nxt_x, nxt_y) && !A[nxt_x][nxt_y];
                 nxt_x += Dx, nxt_y += Dy) {
                for (int dir2 = 0; dir2 < 8; dir2++) {
                    DX = dx[dir2], DY = dy[dir2];
                    for (int arr_x = nxt_x + DX, arr_y = nxt_y + DY;
                         !out_of_bound(arr_x, arr_y) && (!A[arr_x][arr_y] || cur_x == arr_x && cur_y == arr_y);
                         arr_x += DX, arr_y += DY) {
                        if (!p->ch.count(policy(cur_x, cur_y, nxt_x, nxt_y, arr_x, arr_y))) return p->full = 0;
                    }
                }
            }
        }
    }
    return p->full = 1;
}

inline void move(int pol) {
    if (!~pol) return;
    int x = (pol & 229376) >> 15, y = (pol & 28672) >> 12;
    int role = A[x][y];
    for (int i = 0; i < 8; i++) {
        if (A.pos[i][0] == x && A.pos[i][1] == y) {
            A[A.pos[i][0] = (pol & 3584) >> 9][A.pos[i][1] = (pol & 448) >> 6] = role;
            A[x][y] = 0;
            A[(pol & 56) >> 3][pol & 7] = 3;
            break;
        }
    }
}

int *shuffled_list1;
int *shuffled_list2;
int *shuffled_list3;

inline node *random_move(node *p) {
    if (is_fully_expanded(p)) return nullptr;
    shuffled_list1 = PERM8[rand() % FACT8];
    shuffled_list2 = PERM8[rand() % FACT8];
    shuffled_list3 = PERM4[rand() % FACT4];
    bool flag = p->dep & 1;
    int Dx, Dy, DX, DY;
    for (int i = 0; i < 4; i++) {
        int id = shuffled_list3[i] + (flag ? 4 : 0);  // printf("%d\n",id);
        int cur_x = A.pos[id][0], cur_y = A.pos[id][1];
        for (int dir1 = 0; dir1 < 8; dir1++) {
            int d1 = shuffled_list1[dir1];
            int CNT = 0, POL;
            Dx = dx[d1], Dy = dy[d1];
            for (int nxt_x = cur_x + Dx, nxt_y = cur_y + Dy; !out_of_bound(nxt_x, nxt_y) && !A[nxt_x][nxt_y];
                 nxt_x += Dx, nxt_y += Dy) {
                for (int dir2 = 0; dir2 < 8; dir2++) {
                    int d2 = shuffled_list2[dir2];
                    int cnt = 0, pol;
                    DX = dx[d2], DY = dy[d2];
                    for (int arr_x = nxt_x + DX, arr_y = nxt_y + DY;
                         !out_of_bound(arr_x, arr_y) && (!A[arr_x][arr_y] || cur_x == arr_x && cur_y == arr_y);
                         arr_x += DX, arr_y += DY) {
                        int cur_pol = policy(cur_x, cur_y, nxt_x, nxt_y, arr_x, arr_y);
                        if (!p->ch.count(cur_pol)) {
                            if (!(rand() % (++cnt))) pol = cur_pol;
                        }
                    }
                    if (cnt) {
                        if (!(rand() % (++CNT))) POL = pol;
                    }
                }
            }
            if (CNT) return p->new_child(POL);
        }
    }
    return nullptr;
}

inline void back_propagation(node *p, double eva) {
    while (p) {
        p->val = (p->val * p->vis + eva) / (p->vis + 1);
        p->vis++;
        update(p);
        p = p->fa;
    }
}

inline double Delta(int d1, int d2) {
    if ((!~d1) && (!~d2))
        return 0.0;
    else if (!~d1)
        return -1.0;
    else if (!~d2)
        return 1.0;
    else if (d1 == d2)
        return KAPPA;
    else if (d1 > d2)
        return -1.0;
    else
        return 1.0;
}

struct Qnode {
    int x, y, d;
    Qnode() {}
    Qnode(int x, int y, int d) : x(x), y(y), d(d) {}
};

int Queen_1[SIZE][SIZE], Queen_2[SIZE][SIZE];
int King_1[SIZE][SIZE], King_2[SIZE][SIZE];
int L[SIZE][SIZE];

inline void calc_Queen_1() {
    memset(Queen_1, -1, sizeof Queen_1);
    queue<Qnode> Q;
    for (int x, y, i = 0; i < 4; i++) {
        x = A.pos[i][0], y = A.pos[i][1];
        Q.push(Qnode(x, y, 0));
        Queen_1[x][y] = 0;
    }
    while (!Q.empty()) {
        Qnode cur = Q.front();
        Q.pop();
        int x = cur.x, y = cur.y, d = cur.d, Dx, Dy;
        for (int i = 0; i < 8; i++) {
            Dx = dx[i], Dy = dy[i];
            for (int _x = x + Dx, _y = y + Dy; !out_of_bound(_x, _y) && !A[_x][_y]; _x += Dx, _y += Dy) {
                if (!~Queen_1[_x][_y]) { Q.push(Qnode(_x, _y, Queen_1[_x][_y] = d + 1)); }
            }
        }
    }
}

inline void calc_Queen_2() {
    memset(Queen_2, -1, sizeof Queen_2);
    queue<Qnode> Q;
    for (int x, y, i = 4; i < 8; i++) {
        x = A.pos[i][0], y = A.pos[i][1];
        Q.push(Qnode(x, y, 0));
        Queen_2[x][y] = 0;
    }
    while (!Q.empty()) {
        Qnode cur = Q.front();
        Q.pop();
        int x = cur.x, y = cur.y, d = cur.d, Dx, Dy;
        for (int i = 0; i < 8; i++) {
            Dx = dx[i], Dy = dy[i];
            for (int _x = x + Dx, _y = y + Dy; !out_of_bound(_x, _y) && !A[_x][_y]; _x += Dx, _y += Dy) {
                if (!~Queen_2[_x][_y]) { Q.push(Qnode(_x, _y, Queen_2[_x][_y] = d + 1)); }
            }
        }
    }
}

inline double calc_Q() {
    calc_Queen_1();
    calc_Queen_2();
    double sum = 0.0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!A[i][j]) sum += Delta(Queen_1[i][j], Queen_2[i][j]);
        }
    }
    return sum;
}

inline double calc_omega() {
    double sum = 0.0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int &X = Queen_1[i][j], &Y = Queen_2[i][j];
            if ((~X) && (~Y)) { sum += pow(2, -abs(X - Y)); }
        }
    }
    return sum;
}

inline void calc_King_1() {
    memset(King_1, -1, sizeof King_1);
    queue<Qnode> Q;
    for (int x, y, i = 0; i < 4; i++) {
        x = A.pos[i][0], y = A.pos[i][1];
        Q.push(Qnode(x, y, 0));
        King_1[x][y] = 0;
    }
    while (!Q.empty()) {
        Qnode cur = Q.front();
        Q.pop();
        int x = cur.x, y = cur.y, d = cur.d;
        for (int i = 0; i < 8; i++) {
            int _x = x + dx[i], _y = y + dy[i];
            if (!out_of_bound(_x, _y) && !A[_x][_y] && !~King_1[_x][_y]) {
                Q.push(Qnode(_x, _y, King_1[_x][_y] = d + 1));
            }
        }
    }
}

inline void calc_King_2() {
    memset(King_2, -1, sizeof King_2);
    queue<Qnode> Q;
    for (int x, y, i = 4; i < 8; i++) {
        x = A.pos[i][0], y = A.pos[i][1];
        Q.push(Qnode(x, y, 0));
        King_2[x][y] = 0;
    }
    while (!Q.empty()) {
        Qnode cur = Q.front();
        Q.pop();
        int x = cur.x, y = cur.y, d = cur.d;
        for (int i = 0; i < 8; i++) {
            int _x = x + dx[i], _y = y + dy[i];
            if (!out_of_bound(_x, _y) && !A[_x][_y] && !~King_2[_x][_y]) {
                Q.push(Qnode(_x, _y, King_2[_x][_y] = d + 1));
            }
        }
    }
}

inline double calc_K() {
    calc_King_1();
    calc_King_2();
    double sum = 0.0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!A[i][j]) sum += Delta(King_1[i][j], King_2[i][j]);
        }
    }
    return sum;
}

inline void calc_Liberty() {
    memset(L, 0, sizeof L);
    int ti, tj;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            for (int dir = 0; dir < 4; dir++) {
                ti = i + dx[dir], tj = j + dy[dir];
                if (!out_of_bound(ti, tj) && !A[ti][tj] && !A[i][j]) { L[i][j]++, L[ti][tj]++; }
            }
        }
    }
}

inline double f(double x) { return 30 / (5 + x); }

inline double calc_M() {
    calc_Liberty();
    int Dx, Dy;
    double sum = 0;
    double mi = INF;
    for (int x, y, i = 0; i < 4; i++) {
        x = A.pos[i][0], y = A.pos[i][1];
        double m = 0.0;
        for (int dir = 0; dir < 8; dir++) {
            Dx = dx[dir], Dy = dy[dir];
            for (int _x = x + Dx, _y = y + Dy, len = 1; !out_of_bound(_x, _y) && !A[_x][_y];
                 _x += Dx, _y += Dy, len++) {
                if (~Queen_2[_x][_y]) { m += pow(2, -len) * L[_x][_y]; }
            }
        }
        mi = min(mi, m);
        sum -= f(m);
    }
    sum -= PENAL_MIN * f(mi);
    mi = INF;
    for (int x, y, i = 4; i < 8; i++) {
        x = A.pos[i][0], y = A.pos[i][1];
        double m = 0.0;
        for (int dir = 0; dir < 8; dir++) {
            Dx = dx[dir], Dy = dy[dir];
            for (int _x = x + Dx, _y = y + Dy, len = 1; !out_of_bound(_x, _y) && !A[_x][_y];
                 _x += Dx, _y += Dy, len++) {
                if (~Queen_1[_x][_y]) { m += pow(2, -len) * L[_x][_y]; }
            }
        }
        mi = min(mi, m);
        sum += f(m);
    }
    sum += PENAL_MIN * f(mi);
    return sum;
}

inline void print_Queen_1() {
    printf("Queen_1:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) { printf("%d ", Queen_1[i][j]); }
        puts("");
    }
    puts("");
}

inline void print_Queen_2() {
    printf("Queen_2:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) { printf("%d ", Queen_2[i][j]); }
        puts("");
    }
    puts("");
}

inline void print_King_1() {
    printf("King_1:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) { printf("%d ", King_1[i][j]); }
        puts("");
    }
    puts("");
}

inline void print_King_2() {
    printf("King_2:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) { printf("%d ", King_2[i][j]); }
        puts("");
    }
    puts("");
}

inline void print_L() {
    printf("L:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) { printf("%d ", L[i][j]); }
        puts("");
    }
    puts("");
}

inline void print() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (A[j][i] == 0)
                printf(". ");
            else if (A[j][i] == 1)
                printf("B ");
            else if (A[j][i] == 2)
                printf("W ");
            else if (A[j][i] == 3)
                printf("X ");
        }
        puts("");
    }
    puts("");
}

inline double sigmoid(double x) {  //\R -> (0,1)
#ifdef LOCAL_MODE
    printf("sigmoid: %lf\n\n\n", x);
#endif
    return 1.0 / (1.0 + exp(-SIG_THRES * x));
}

inline double sigma_1(double omega) {
    if (omega > BEGIN)
        return 0.24;
    else if (omega > MIDUM)
        return 0.60;
    else
        return 0.90;
}

inline double sigma_2(double omega) {
    if (omega > BEGIN)
        return 0.50;
    else if (omega > MIDUM)
        return 0.30;
    else
        return 0.10;
}

inline double sigma_3(double omega) {
    if (omega > BEGIN)
        return 0.26;
    else if (omega > MIDUM)
        return 0.10;
    else
        return 0;
}

inline double eval() {
    double Q = calc_Q();
    double K = calc_K();
    double omega = calc_omega();
    double M = (omega <= MIDUM) ? 0.0 : calc_M();

#ifdef LOCAL_MODE
    printf("omega:%.3lf\n", omega);
    printf("Q:%.3lf,\n", Q);
    printf("K:%.3lf,\n", K);
    printf("M:%.3lf,\n", M);
    print();
    puts("\n");
#endif

    return sigmoid(sigma_1(omega) * Q + sigma_2(omega) * K + sigma_3(omega) * M);
}

inline void expand(node *p) {
    node *t = p, *ch;
    for (int i = 0; i < D; i++) {
        ch = random_move(t);
        if (!ch) break;
        t = ch;
        move(t->pol);
    }
    // if(!(p->dep&1)){
    // 	ch=random_move(t);
    // 	if(ch)t=ch,move(t->pol);
    // }
#ifdef LOCAL_MODE
    printf("dep:%d,\n", t->dep);
#endif
    if (!p->ch.empty()) {
        delete p->ch.begin()->second;
        p->full = 0;
        p->ch.clear();
    }
    back_propagation(p, eval());
}

inline bool UCT_search() {
    state copy = A;
    node *p = root;
    for (; p; p = p->bst_ch) {
        if (p != root) move(p->pol);
        if (!is_fully_expanded(p)) break;
    }
    if (!p) return swap(A, copy), 0;
    p = random_move(p);
    move(p->pol);
    expand(p);
    swap(A, copy);
    return 1;
}

#ifdef JSON_MODE

int TRAIN_TIMES;

#endif

inline void train() {
    int cnt = 0;
    while ((unsigned)clock() < timing) {
        for (int i = 0; i < TRAIN_UNIT; i++) {
            if (!UCT_search()) return;
        }
        cnt++;
    }

#ifdef LOCAL_MODE
    fprintf(stderr, "%d\n", cnt);
#endif

#ifdef JSON_MODE
    TRAIN_TIMES = cnt;
#endif
}

inline void play(int pol) {
    if (!root->ch.count(pol))
        root = root->new_child(pol);
    else
        root = root->ch[pol];
    root->del_fa();
    move(root->pol);
}

inline int play() {
    train();
    node *nxt = nullptr;
    if (root->ch.empty()) return -1;
    for (auto &p : root->ch) {
        if (!nxt || p.second->vis > nxt->vis) nxt = p.second;
    }
    if (!nxt) return -1;
    root = nxt;
    root->del_fa();
    move(root->pol);
    return root->pol;
}

#ifdef JSON_MODE

#include "jsoncpp/json.h"

string str;
Json::Reader reader;
Json::Value input;
Json::FastWriter writer;
Json::Value ret;

inline void json_test() {
    getline(cin, str);
    reader.parse(str, input);
    int x_0, y_0, x_1, y_1, x_2, y_2;
    int role = input["requests"][(Json::Value::UInt)0]["x0"].asInt() < 0 ? 1 : 2;
    if (role == 2) {
        x_0 = input["requests"][(Json::Value::UInt)0]["x0"].asInt();
        y_0 = input["requests"][(Json::Value::UInt)0]["y0"].asInt();
        x_1 = input["requests"][(Json::Value::UInt)0]["x1"].asInt();
        y_1 = input["requests"][(Json::Value::UInt)0]["y1"].asInt();
        x_2 = input["requests"][(Json::Value::UInt)0]["x2"].asInt();
        y_2 = input["requests"][(Json::Value::UInt)0]["y2"].asInt();
        play(policy(x_0, y_0, x_1, y_1, x_2, y_2));
    }
    timing = clock() + int((1 + TIME_LIMIT) * CLOCKS_PER_SEC);
    trans_policy(play(), x_0, y_0, x_1, y_1, x_2, y_2);
    ret["response"]["x0"] = x_0;
    ret["response"]["y0"] = y_0;
    ret["response"]["x1"] = x_1;
    ret["response"]["y1"] = y_1;
    ret["response"]["x2"] = x_2;
    ret["response"]["y2"] = y_2;
    ret["debug"]["train_times"] = (Json::UInt)TRAIN_TIMES;
    TRAIN_TIMES = 0;
    cout << writer.write(ret) << endl;
    cout << "\n>>>BOTZONE_REQUEST_KEEP_RUNNING<<<\n";
    while (getline(cin, str)) {
        input.clear();
        reader.parse(str, input);
        x_0 = input["x0"].asInt();
        y_0 = input["y0"].asInt();
        x_1 = input["x1"].asInt();
        y_1 = input["y1"].asInt();
        x_2 = input["x2"].asInt();
        y_2 = input["y2"].asInt();
        play(policy(x_0, y_0, x_1, y_1, x_2, y_2));
        timing = clock() + int(TIME_LIMIT * CLOCKS_PER_SEC);
        trans_policy(play(), x_0, y_0, x_1, y_1, x_2, y_2);
        ret.clear();
        ret["response"]["x0"] = x_0;
        ret["response"]["y0"] = y_0;
        ret["response"]["x1"] = x_1;
        ret["response"]["y1"] = y_1;
        ret["response"]["x2"] = x_2;
        ret["response"]["y2"] = y_2;
        ret["debug"]["train_times"] = (Json::UInt)TRAIN_TIMES;
        TRAIN_TIMES = 0;
        cout << writer.write(ret) << endl;
        cout << "\n>>>BOTZONE_REQUEST_KEEP_RUNNING<<<\n";
    }
}

#endif

#ifdef LOCAL_MODE

inline int read() {
    int x = 0, f = 1;
    char ch = getchar();
    while (ch < '0' || ch > '9') {
        if (ch == '-') f = -f;
        ch = getchar();
    }
    while (ch >= '0' && ch <= '9') { x = x * 10 + ch - '0', ch = getchar(); }
    return x * f;
}

inline void local_test() {
    int MODE = read();
    if (MODE == 0) {
        int x_0, y_0, x_1, y_1, x_2, y_2;
        bool flag = 1;
        while (1) {
            if (flag)
                timing = clock() + int(1.95 * CLOCKS_PER_SEC), flag = 0;
            else
                timing = clock() + int(0.95 * CLOCKS_PER_SEC);
            int T = clock();
            int res = play();
            if (!~res) return;
            trans_policy(res, x_0, y_0, x_1, y_1, x_2, y_2);
            fprintf(stderr, "%d %d %d %d %d %d    %.3lf\n", x_0, y_0, x_1, y_1, x_2, y_2,
                    (double)(clock() - T) / CLOCKS_PER_SEC);  // print();
        }
    } else {
        int x_0, y_0, x_1, y_1, x_2, y_2;
        x_0 = read(), y_0 = read(), x_1 = read(), y_1 = read(), x_2 = read(), y_2 = read();
        if (x_0 > 0) play(policy(x_0, y_0, x_1, y_1, x_2, y_2));
        trans_policy(play(), x_0, y_0, x_1, y_1, x_2, y_2);
        print();
        printf("%d %d %d %d %d %d\n", x_0, y_0, x_1, y_1, x_2, y_2);
        while (scanf("%d%d%d%d%d%d", &x_0, &y_0, &x_1, &y_1, &x_2, &y_2) == 6) {
            play(policy(x_0, y_0, x_1, y_1, x_2, y_2));
            print();
            trans_policy(play(), x_0, y_0, x_1, y_1, x_2, y_2);
            printf("%d %d %d %d %d %d\n", x_0, y_0, x_1, y_1, x_2, y_2);
            print();
        }
    }
}

#endif

int cnt;
bool placed[SIZE];

inline void dfs8(int dep) {
    if (dep >= 8) {
        cnt++;
        for (int i = 0; i < 8; i++) PERM8[cnt][i] = PERM8[cnt - 1][i];
        //	printf("%d: ",cnt);
        //	for(int i=0;i<8;i++)printf("%d ",PERM8[0][i]);puts("");
        return;
    }
    for (int i = 0; i < 8; i++) {
        if (!placed[i]) {
            placed[i] = 1;
            PERM8[cnt][dep] = i;
            dfs8(dep + 1);
            placed[i] = 0;
        }
    }
}

inline void dfs4(int dep) {
    if (dep >= 4) {
        cnt++;
        for (int i = 0; i < 4; i++) PERM4[cnt][i] = PERM4[cnt - 1][i];
        //	printf("%d: ",cnt);
        //	for(int i=0;i<8;i++)printf("%d ",PERM8[0][i]);puts("");
        return;
    }
    for (int i = 0; i < 4; i++) {
        if (!placed[i]) {
            placed[i] = 1;
            PERM4[cnt][dep] = i;
            dfs4(dep + 1);
            placed[i] = 0;
        }
    }
}

inline void init() {
    srand((unsigned)time(0));
    memset(placed, 0, sizeof placed), cnt = 0;
    dfs8(0);
    memset(placed, 0, sizeof placed), cnt = 0;
    dfs4(0);
}

int main() {
    init();

#ifdef JSON_MODE
    json_test();
#endif

#ifdef LOCAL_MODE
    local_test();
#endif

    return 0;
}