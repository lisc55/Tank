#include "rule.h"

namespace TankGame {
bool AbleToShoot(int color, int tankNum, TankField& s) {
    if (s.currentTurn == 1) return true;
    return s.ActionIsValid(color, tankNum, UpShoot);
}

void MarkTheField(TankField& s) {  // 标记每点是否在双方坦克的攻击范围内及攻击方向
    for (int i = 0; i < sideCount; i++) {
        for (int j = 0; j < tankPerSide; j++) {
            if (s.tankX[i][j] == -1)
                continue;
            else if (!AbleToShoot(i, j, s))
                continue;
            else {
                for (int k = 0; k < 4; k++) {
                    for (int l = 1;; l++) {
                        int x = s.tankX[i][j] + l * dx[k], y = s.tankY[i][j] + l * dy[k];
                        if (!CoordValid(x, y)) break;
                        if (dy[k]) {
                            if (s.mark[i][y][x] == Nof)
                                s.mark[i][y][x] = Y;
                            else if (s.mark[i][y][x] == X)
                                s.mark[i][y][x] = Both;
                        } else {
                            if (s.mark[i][y][x] == Nof)
                                s.mark[i][y][x] = X;
                            else if (s.mark[i][y][x] == Y)
                                s.mark[i][y][x] = Both;
                        }
                        if (!((s.gameField[y][x] == None) || (s.gameField[y][x] == Water) ||
                              (s.gameField[y][x] == (1 << (5 - 2 * i))) || (s.gameField[y][x] == (1 << (6 - 2 * i))) ||
                              (s.gameField[y][x] == (1 << (5 - 2 * i)) + (1 << (6 - 2 * i)))))
                            break;
                    }
                }
            }
        }
    }
}

void DistanceToBase(int color, TankField& s) {  // 算距离
    struct tempdst {
        int x;
        int y;
        int d;

        tempdst(int _x, int _y, int _d) : x(_x), y(_y), d(_d) {}
    };
    class cmp {
       public:
        bool operator()(const tempdst& a, const tempdst& b) const { return (a.d > b.d); }
    };
    std::priority_queue<tempdst, std::vector<tempdst>, cmp> td;
    memset(s.distance[color], -1, sizeof(s.distance[color]));
    s.distance[color][baseY[color]][baseX[color]] = 0;
    for (int k = 0; k < 4; k++) {
        for (int l = 1;; l++) {
            int x = baseX[color] + l * dx[k];
            int y = baseY[color] + l * dy[k];
            if (!CoordValid(x, y)) break;
            bool Push = true;
            if (s.gameField[y][x] == Water) {  // bug
                Push = false;
            }
            if (s.gameField[y][x] == Steel || s.gameField[y][x] == Base) break;
            s.distance[color][y][x] = s.distance[color][y - dy[k]][x - dx[k]];
            if (s.gameField[y][x] == Brick) { s.distance[color][y][x] += 2; }
            if (Push) td.push(tempdst(x, y, s.distance[color][y][x]));
        }
    }
    while (!td.empty()) {
        int xt = (td.top()).x;
        int yt = (td.top()).y;
        for (int k = 0; k < 4; k++) {
            int x = xt + dx[k];
            int y = yt + dy[k];
            if (!CoordValid(x, y)) continue;
            if (s.gameField[y][x] == Steel || s.gameField[y][x] == Water || s.gameField[y][x] == Base ||
                s.distance[color][y][x] != -1)
                continue;
            if (s.gameField[y][x] == Brick) {
                s.distance[color][y][x] = s.distance[color][yt][xt] + 2;
                td.push(tempdst(x, y, s.distance[color][y][x]));
            } else {
                s.distance[color][y][x] = s.distance[color][yt][xt] + 1;
                td.push(tempdst(x, y, s.distance[color][y][x]));
            }
        }
        td.pop();
    }
}
// 钢墙距离是-1，水的距离可能不是-1，不能通过距离判断这个格子上是什么
}  // namespace TankGame
namespace TankGame {
std::pair<int, int> RuleDecision(int color, TankField& s) {
    int decision[2] = {-1, -1};
    std::vector<int> valid[2][2];
    std::vector<int> survive[2][2];
    bool finished[2] = {};
    bool fire[2][2] = {};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) { fire[i][j] = AbleToShoot(i, j, s); }
    }
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 2; i++) {
            for (int j = -1; j < 8; j++) {
                if (s.ActionIsValid(c, i, Action(j))) valid[c][i].push_back(j);
            }
        }
        for (int i = 0; i < 2; i++) {
            if (s.mark[1 - c][s.tankY[c][i]][s.tankX[c][i]] == 3) continue;
            if (!(s.tankAlive[color][i])) continue;
            if (fire[c][i]) {
                if (s.mark[1 - c][s.tankY[c][i]][s.tankX[c][i]] == 2) {
                    int x = s.tankX[c][i];
                    int y = s.tankY[c][i];
                    bool up = false, down = false;
                    for (int j = 1;; j++) {
                        if (!CoordValid(x, y + j)) break;
                        if (s.gameField[y + j][x] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (s.tankY[1 - c][k] == y + j && s.tankX[1 - c][k] == x) up = true;
                            }
                            break;
                        }
                    }
                    for (int j = -1;; j--) {
                        if (!CoordValid(x, y + j)) break;
                        if (s.gameField[y + j][x] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (s.tankY[1 - c][k] == y + j && s.tankX[1 - c][k] == x) down = true;
                            }
                            break;
                        }
                    }
                    if (up && (!down))
                        survive[c][i].push_back(4);
                    else if (down && (!up))
                        survive[c][i].push_back(6);
                } else if (s.mark[1 - c][s.tankY[c][i]][s.tankX[c][i]] == 1) {
                    int x = s.tankX[c][i];
                    int y = s.tankY[c][i];
                    bool left = false, right = false;
                    for (int j = 1;; j++) {
                        if (!CoordValid(x + j, y)) break;
                        if (s.gameField[y][x + j] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (s.tankY[1 - c][k] == y && s.tankX[1 - c][k] == x + j) right = true;
                            }
                            break;
                        }
                    }
                    for (int j = -1;; j--) {
                        if (!CoordValid(x + j, y)) break;
                        if (s.gameField[y][x + j] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (s.tankY[1 - c][k] == y && s.tankX[1 - c][k] == x + j) left = true;
                            }
                            break;
                        }
                    }
                    if (left && (!right))
                        survive[c][i].push_back(7);
                    else if (right && (!left))
                        survive[c][i].push_back(5);
                } else {
                    survive[c][i].push_back(4);
                    survive[c][i].push_back(5);
                    survive[c][i].push_back(6);
                    survive[c][i].push_back(7);
                }
            }
            if (s.mark[1 - c][s.tankY[c][i]][s.tankX[c][i]] == 0) survive[c][i].push_back(-1);
            if (s.ActionIsValid(c, i, Action(0)) && s.mark[1 - c][s.tankY[c][i] - 1][s.tankX[c][i]] == 0)
                survive[c][i].push_back(0);
            if (s.ActionIsValid(c, i, Action(1)) && s.mark[1 - c][s.tankY[c][i]][s.tankX[c][i] + 1] == 0)
                survive[c][i].push_back(1);
            if (s.ActionIsValid(c, i, Action(2)) && s.mark[1 - c][s.tankY[c][i] + 1][s.tankX[c][i]] == 0)
                survive[c][i].push_back(2);
            if (s.ActionIsValid(c, i, Action(3)) && s.mark[1 - c][s.tankY[c][i]][s.tankX[c][i] - 1] == 0)
                survive[c][i].push_back(3);
        }
    }
    for (int i = 0; i < sideCount; i++) {
        if (s.distance[1 - color][s.tankY[color][i]][s.tankX[color][i]] == 0 && fire[color][i] &&
            s.tankAlive[color][i]) {
            if (baseY[1 - color] - s.tankY[color][i] > 0)
                decision[i] = 6;
            else if (baseY[1 - color] - s.tankY[color][i] < 0)
                decision[i] = 4;
            else if (baseX[1 - color] - s.tankX[color][i] < 0)
                decision[i] = 7;
            else if (baseX[1 - color] - s.tankX[color][i] > 0)
                decision[i] = 5;
            if (!survive[color][1 - i].empty())
                decision[1 - i] = survive[color][1 - i][0];
            else
                decision[1 - i] = -1;
            return std::make_pair(decision[0], decision[1]);
        }
    }
    int nextItem[2][2][4] = {};  // 上右下左
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 2; i++) {
            for (int dir = 0; dir < 4; dir++) {
                for (int l = 1;; l++) {
                    int x = s.tankX[c][i] + l * dx[dir];
                    int y = s.tankY[c][i] + l * dy[dir];
                    if (!CoordValid(x, y)) break;
                    if (s.gameField[y][x] != None && s.gameField[y][x] != Water) {
                        nextItem[c][i][dir] = int(s.gameField[y][x]);
                        break;
                    }
                }
            }
        }
    }
    for (int i = 0; i < sideCount; i++) {
        if (survive[1 - color][i].empty()) {  // 对方坦克必死
            int base = 2 * (1 - color) + i + 3;
            for (int j = 0; j < 2; j++) {
                if (fire[color][j]) {
                    for (int dir = 0; dir < 4; dir++) {
                        if (nextItem[color][j][dir] & (1 << base)) {
                            decision[j] = dir + 4;
                            finished[j] = true;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        if (finished[i]) continue;
        bool move = false;
        if (s.mark[1 - color][s.tankY[color][i]][s.tankX[color][i]] != Nof) move = true;
        int minNone = 200;
        int nonedir = -1;  // 这些可以改成vector
        int brickmin = 200;
        int brickdir = -1;
        for (int dir = 0; dir < 4; dir++) {
            int x = s.tankX[color][i] + dx[dir];
            int y = s.tankY[color][i] + dy[dir];
            if (!CoordValid(x, y)) continue;
            if (s.mark[1 - color][y][x] != Nof) continue;
            if (s.gameField[y][x] == None) {
                if (s.mark[1 - color][y][x] == 0) {
                    if (s.distance[1 - color][y][x] < minNone) {
                        minNone = s.distance[1 - color][y][x];
                        nonedir = dir;
                    }
                }
            } else if (s.gameField[y][x] == Brick) {
                if (s.mark[1 - color][y][x] == 0) {
                    if (s.distance[1 - color][y][x] < brickmin) {
                        brickmin = s.distance[1 - color][y][x];
                        brickdir = dir;
                    }
                }
            }
        }
        if (move) {
            if (minNone == s.distance[1 - color][s.tankY[color][i]][s.tankX[color][i]] - 1) {
                decision[i] = nonedir;
            } else if (fire[color][i]) {
                for (int dir = 0; dir < 4; dir++) {
                    for (int j = 0; j < 2; j++) {
                        if ((nextItem[color][i][dir] & (1 << (2 * (1 - color) + 3 + j))) && fire[1 - color][j]) {
                            if ((nextItem[color][i][(dir + 2) % 4] & (1 << (2 * (1 - color) + 4 - j))) &&
                                fire[1 - color][1 - j]) {
                                decision[i] = nonedir;
                            } else
                                decision[i] = dir + 4;
                        }
                    }
                }
            } else
                decision[i] = nonedir;
        } else {
            if (!fire[color][i]) {
                if (minNone == s.distance[1 - color][s.tankY[color][i]][s.tankX[color][i]] - 1) {
                    decision[i] = nonedir;
                } else
                    decision[i] = -1;
            } else {
                if (brickmin == s.distance[1 - color][s.tankY[color][i]][s.tankX[color][i]] - 1) {
                    if ((int(s.mark[1 - color][s.tankY[color][i] + dy[brickdir]][s.tankX[color][i] + dx[brickdir]]) !=
                             3 &&
                         (int(s.mark[1 - color][s.tankY[color][i] + dy[brickdir]][s.tankX[color][i] + dx[brickdir]]) -
                          brickdir) %
                                 2 !=
                             0) ||
                        (int(s.mark[1 - color][s.tankY[color][i] + dy[brickdir]][s.tankX[color][i] + dx[brickdir]]) ==
                         0)) {
                        decision[i] = brickdir + 4;
                    }
                } else {
                    if (minNone == s.distance[1 - color][s.tankY[color][i]][s.tankX[color][i]] - 1) {
                        if (nextItem[color][i][nonedir] == int(Brick)) {
                            int x = s.tankX[color][i];
                            int y = s.tankY[color][i];
                            bool shoot = true;
                            int l = 1;
                            for (;; l++) {
                                if (s.gameField[y + l * dy[nonedir]][x + l * dx[nonedir]] == None) {
                                    if (s.distance[1 - color][y + l * dy[nonedir]][x + l * dx[nonedir]] !=
                                        s.distance[1 - color][y + (l - 1) * dy[nonedir]][x + (l - 1) * dx[nonedir]] -
                                            1) {
                                        shoot = false;
                                        break;
                                    }
                                } else {
                                    if (s.gameField[y + l * dy[nonedir]][x + l * dx[nonedir]] != Brick) {
                                        shoot = false;
                                        break;
                                    }
                                    if (s.distance[1 - color][y + l * dy[nonedir]][x + l * dx[nonedir]] !=
                                        s.distance[1 - color][y + (l - 1) * dy[nonedir]][x + (l - 1) * dx[nonedir]] -
                                            1) {
                                        shoot = false;
                                    }
                                    break;
                                }
                            }
                            if (shoot) {
                                int bx = x + l * dx[nonedir];
                                int by = y + l * dy[nonedir];
                                if ((int(s.mark[1 - color][by][bx]) != 3 &&
                                     (int(s.mark[1 - color][by][bx]) - nonedir) % 2 != 0) ||
                                    (int(s.mark[1 - color][by][bx] == 0))) {
                                    decision[i] = nonedir + 4;
                                } else
                                    decision[i] = nonedir;
                            } else
                                decision[i] = nonedir;
                        } else
                            decision[i] = nonedir;
                    } else if (s.tankX[color][i] == baseX[1 - color]) {
                        int ddy;
                        int ddx = 0;
                        if (s.tankY[color][i] < baseY[1 - color])
                            ddy = 1;
                        else
                            ddy = -1;
                        bool shoot = !HasMultipleTank(s.gameField[s.tankY[color][i]][s.tankX[color][i]]);
                        for (int l = 1;; l++) {
                            int x = s.tankX[color][i] + l * ddx;
                            int y = s.tankY[color][i] + l * ddy;
                            if (s.gameField[y][x] == Base) break;
                            if (s.gameField[y][x] != None && s.gameField[y][x] != Brick && s.gameField[y][x] != Water) {
                                shoot = false;
                                break;
                            }
                        }
                        if (shoot) {
                            if (ddy == 1) {
                                decision[i] = 6;
                            } else
                                decision[i] = 4;
                        }
                    } else if (s.tankY[color][i] == baseY[1 - color]) {
                        int ddy = 0;
                        int ddx;
                        if (s.tankX[color][i] < baseX[1 - color])
                            ddx = 1;
                        else
                            ddx = -1;
                        bool shoot = true;
                        for (int l = 1;; l++) {
                            int x = s.tankX[color][i] + l * ddx;
                            int y = s.tankY[color][i] + l * ddy;
                            if (s.gameField[y][x] == Base) break;
                            if (s.gameField[y][x] != None && s.gameField[y][x] != Brick && s.gameField[y][x] != Water) {
                                shoot = false;
                                break;
                            }
                        }
                        if (shoot) {
                            if (ddx == 1) {
                                decision[i] = 5;
                            } else
                                decision[i] = 7;
                        }
                    }
                }
            }
        }
    }
    return std::make_pair(decision[0], decision[1]);
}
}  // namespace TankGame