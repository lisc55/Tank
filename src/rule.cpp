#include "rule.h"

namespace TankGame {
bool AbleToShoot(int color, int tankNum) {
    if (field->currentTurn == 1) return true;
    return field->ActionIsValid(color, tankNum, UpShoot);
}

void MarkTheField() {  // 标记每点是否在双方坦克的攻击范围内及攻击方向
    for (int i = 0; i < sideCount; i++) {
        for (int j = 0; j < tankPerSide; j++) {
            if (field->tankX[i][j] == -1)
                continue;
            else if (!AbleToShoot(i, j))
                continue;
            else {
                for (int k = 0; k < 4; k++) {
                    for (int l = 1;; l++) {
                        int x = field->tankX[i][j] + l * dx[k],
                            y = field->tankY[i][j] + l * dy[k];
                        if (!CoordValid(x, y)) break;
                        if (dy[k]) {
                            if (field->mark[i][y][x] == Nof)
                                field->mark[i][y][x] = Y;
                            else if (field->mark[i][y][x] == X)
                                field->mark[i][y][x] = Both;
                        } else {
                            if (field->mark[i][y][x] == Nof)
                                field->mark[i][y][x] = X;
                            else if (field->mark[i][y][x] == Y)
                                field->mark[i][y][x] = Both;
                        }
                        if (!((field->gameField[y][x] == None) ||
                              (field->gameField[y][x] == Water) ||
                              (field->gameField[y][x] == (1 << (5 - 2 * i))) ||
                              (field->gameField[y][x] == (1 << (6 - 2 * i))) ||
                              (field->gameField[y][x] ==
                               (1 << (5 - 2 * i)) + (1 << (6 - 2 * i)))))
                            break;
                    }
                }
            }
        }
    }
}

void DistanceToBase(int color) {  // 算距离
    struct tempdst {
        int x;
        int y;
        int d;

        tempdst(int _x, int _y, int _d) : x(_x), y(_y), d(_d) {}
    };
    class cmp {
       public:
        bool operator()(const tempdst &a, const tempdst &b) const {
            return (a.d > b.d);
        }
    };
    std::priority_queue<tempdst, std::vector<tempdst>, cmp> td;
    memset(field->distance[color], -1, sizeof(field->distance[color]));
    field->distance[color][baseY[color]][baseX[color]] = 0;
    for (int k = 0; k < 4; k++) {
        for (int l = 1;; l++) {
            int x = baseX[color] + l * dx[k];
            int y = baseY[color] + l * dy[k];
            if (!CoordValid(x, y)) break;
            bool Push = true;
            if (field->gameField[y][x] == Water) {  // bug
                Push = false;
            }
            if (field->gameField[y][x] == Steel ||
                field->gameField[y][x] == Base)
                break;
            field->distance[color][y][x] =
                field->distance[color][y - dy[k]][x - dx[k]];
            if (field->gameField[y][x] == Brick) {
                field->distance[color][y][x] += 2;
            }
            if (Push) td.push(tempdst(x, y, field->distance[color][y][x]));
        }
    }
    while (!td.empty()) {
        int xt = (td.top()).x;
        int yt = (td.top()).y;
        for (int k = 0; k < 4; k++) {
            int x = xt + dx[k];
            int y = yt + dy[k];
            if (!CoordValid(x, y)) continue;
            if (field->gameField[y][x] == Steel ||
                field->gameField[y][x] == Water ||
                field->gameField[y][x] == Base ||
                field->distance[color][y][x] != -1)
                continue;
            if (field->gameField[y][x] == Brick) {
                field->distance[color][y][x] =
                    field->distance[color][yt][xt] + 2;
                td.push(tempdst(x, y, field->distance[color][y][x]));
            } else {
                field->distance[color][y][x] =
                    field->distance[color][yt][xt] + 1;
                td.push(tempdst(x, y, field->distance[color][y][x]));
            }
        }
        td.pop();
    }
}
// 钢墙距离是-1，水的距离可能不是-1，不能通过距离判断这个格子上是什么
}  // namespace TankGame
namespace TankGame {
std::pair<int, int> RuleDecision() {
    int color = field->mySide;
    MarkTheField();
    DistanceToBase(0);
    DistanceToBase(1);
    int decision[2] = {-1, -1};
    std::vector<int> valid[2][2];
    std::vector<int> survive[2][2];
    bool finished[2] = {};
    bool fire[2][2] = {};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) { fire[i][j] = AbleToShoot(i, j); }
    }
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 2; i++) {
            for (int j = -1; j < 8; j++) {
                if (field->ActionIsValid(c, i, Action(j)))
                    valid[c][i].push_back(j);
            }
        }
        for (int i = 0; i < 2; i++) {
            if (field->mark[1 - c][field->tankY[c][i]][field->tankX[c][i]] == 3)
                continue;
            if (!(field->tankAlive[color][i])) continue;
            if (fire[c][i]) {
                if (field->mark[1 - c][field->tankY[c][i]]
                               [field->tankX[c][i]] == 2) {
                    int x = field->tankX[c][i];
                    int y = field->tankY[c][i];
                    bool up = false, down = false;
                    for (int j = 1;; j++) {
                        if (!CoordValid(x, y + j)) break;
                        if (field->gameField[y + j][x] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (field->tankY[1 - c][k] == y + j &&
                                    field->tankX[1 - c][k] == x)
                                    up = true;
                            }
                            break;
                        }
                    }
                    for (int j = -1;; j--) {
                        if (!CoordValid(x, y + j)) break;
                        if (field->gameField[y + j][x] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (field->tankY[1 - c][k] == y + j &&
                                    field->tankX[1 - c][k] == x)
                                    down = true;
                            }
                            break;
                        }
                    }
                    if (up && (!down))
                        survive[c][i].push_back(4);
                    else if (down && (!up))
                        survive[c][i].push_back(6);
                } else if (field->mark[1 - c][field->tankY[c][i]]
                                      [field->tankX[c][i]] == 1) {
                    int x = field->tankX[c][i];
                    int y = field->tankY[c][i];
                    bool left = false, right = false;
                    for (int j = 1;; j++) {
                        if (!CoordValid(x + j, y)) break;
                        if (field->gameField[y][x + j] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (field->tankY[1 - c][k] == y &&
                                    field->tankX[1 - c][k] == x + j)
                                    right = true;
                            }
                            break;
                        }
                    }
                    for (int j = -1;; j--) {
                        if (!CoordValid(x + j, y)) break;
                        if (field->gameField[y][x + j] != None) {
                            for (int k = 0; k < 2; k++) {
                                if (field->tankY[1 - c][k] == y &&
                                    field->tankX[1 - c][k] == x + j)
                                    left = true;
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
            if (field->mark[1 - c][field->tankY[c][i]][field->tankX[c][i]] == 0)
                survive[c][i].push_back(-1);
            if (field->ActionIsValid(c, i, Action(0)) &&
                field->mark[1 - c][field->tankY[c][i] - 1]
                           [field->tankX[c][i]] == 0)
                survive[c][i].push_back(0);
            if (field->ActionIsValid(c, i, Action(1)) &&
                field->mark[1 - c][field->tankY[c][i]]
                           [field->tankX[c][i] + 1] == 0)
                survive[c][i].push_back(1);
            if (field->ActionIsValid(c, i, Action(2)) &&
                field->mark[1 - c][field->tankY[c][i] + 1]
                           [field->tankX[c][i]] == 0)
                survive[c][i].push_back(2);
            if (field->ActionIsValid(c, i, Action(3)) &&
                field->mark[1 - c][field->tankY[c][i]]
                           [field->tankX[c][i] - 1] == 0)
                survive[c][i].push_back(3);
        }
    }
    for (int i = 0; i < sideCount; i++) {
        if (field->distance[1 - color][field->tankY[color][i]]
                           [field->tankX[color][i]] == 0 &&
            fire[color][i] && field->tankAlive[color][i]) {
            if (baseY[1 - color] - field->tankY[color][i] > 0)
                decision[i] = 6;
            else if (baseY[1 - color] - field->tankY[color][i] < 0)
                decision[i] = 4;
            else if (baseX[1 - color] - field->tankX[color][i] < 0)
                decision[i] = 7;
            else if (baseX[1 - color] - field->tankX[color][i] > 0)
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
                    int x = field->tankX[c][i] + l * dx[dir];
                    int y = field->tankY[c][i] + l * dy[dir];
                    if (!CoordValid(x, y)) break;
                    if (field->gameField[y][x] != None &&
                        field->gameField[y][x] != Water) {
                        nextItem[c][i][dir] = int(field->gameField[y][x]);
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
    //-//-//-//-//-//-//-//-//-//-//-//-
    for (int i = 0; i < 2; i++) {
        if (finished[i]) continue;
        bool move = false;
        if (field->mark[1 - color][field->tankY[color][i]]
                       [field->tankX[color][i]] != Nof)
            move = true;
        int minNone = 200;
        int nonedir = -1;  // 这些可以改成vector
        int brickmin = 200;
        int brickdir = -1;
        for (int dir = 0; dir < 4; dir++) {
            int x = field->tankX[color][i] + dx[dir];
            int y = field->tankY[color][i] + dy[dir];
            if (!CoordValid(x, y)) continue;
            if (field->mark[1 - color][y][x] != Nof) continue;
            if (field->gameField[y][x] == None) {
                if (field->mark[1 - color][y][x] == 0) {
                    if (field->distance[1 - color][y][x] < minNone) {
                        minNone = field->distance[1 - color][y][x];
                        nonedir = dir;
                    }
                }
            } else if (field->gameField[y][x] == Brick) {
                if (field->mark[1 - color][y][x] == 0) {
                    if (field->distance[1 - color][y][x] < brickmin) {
                        brickmin = field->distance[1 - color][y][x];
                        brickdir = dir;
                    }
                }
            }
        }
        if (move) {
            if (minNone == field->distance[1 - color][field->tankY[color][i]]
                                          [field->tankX[color][i]] -
                               1) {
                decision[i] = nonedir;
            } else if (fire[color][i]) {
                for (int dir = 0; dir < 4; dir++) {
                    for (int j = 0; j < 2; j++) {
                        if ((nextItem[color][i][dir] &
                             (1 << (2 * (1 - color) + 3 + j))) &&
                            fire[1 - color][j]) {
                            if ((nextItem[color][i][(dir + 2) % 4] &
                                 (1 << (2 * (1 - color) + 4 - j))) &&
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
                if (minNone ==
                    field->distance[1 - color][field->tankY[color][i]]
                                   [field->tankX[color][i]] -
                        1) {
                    decision[i] = nonedir;
                } else
                    decision[i] = -1;
            } else {
                if (brickmin ==
                    field->distance[1 - color][field->tankY[color][i]]
                                   [field->tankX[color][i]] -
                        1) {
                    if ((int(field->mark[1 - color]
                                        [field->tankY[color][i] + dy[brickdir]]
                                        [field->tankX[color][i] +
                                         dx[brickdir]]) != 3 &&
                         (int(field->mark[1 - color]
                                         [field->tankY[color][i] + dy[brickdir]]
                                         [field->tankX[color][i] +
                                          dx[brickdir]]) -
                          brickdir) %
                                 2 !=
                             0) ||
                        (int(field->mark[1 - color]
                                        [field->tankY[color][i] + dy[brickdir]]
                                        [field->tankX[color][i] +
                                         dx[brickdir]]) == 0)) {
                        decision[i] = brickdir + 4;
                    }
                } else {
                    if (minNone ==
                        field->distance[1 - color][field->tankY[color][i]]
                                       [field->tankX[color][i]] -
                            1) {
                        if (nextItem[color][i][nonedir] == int(Brick)) {
                            int x = field->tankX[color][i];
                            int y = field->tankY[color][i];
                            bool shoot = true;
                            int l = 1;
                            for (;; l++) {
                                if (field->gameField[y + l * dy[nonedir]]
                                                    [x + l * dx[nonedir]] ==
                                    None) {
                                    if (field->distance[1 - color]
                                                       [y + l * dy[nonedir]]
                                                       [x + l * dx[nonedir]] !=
                                        field->distance[1 - color]
                                                       [y +
                                                        (l - 1) * dy[nonedir]]
                                                       [x +
                                                        (l - 1) * dx[nonedir]] -
                                            1) {
                                        shoot = false;
                                        break;
                                    }
                                } else {
                                    if (field->gameField[y + l * dy[nonedir]]
                                                        [x + l * dx[nonedir]] !=
                                        Brick) {
                                        shoot = false;
                                        break;
                                    }
                                    if (field->distance[1 - color]
                                                       [y + l * dy[nonedir]]
                                                       [x + l * dx[nonedir]] !=
                                        field->distance[1 - color]
                                                       [y +
                                                        (l - 1) * dy[nonedir]]
                                                       [x +
                                                        (l - 1) * dx[nonedir]] -
                                            1) {
                                        shoot = false;
                                    }
                                    break;
                                }
                            }
                            if (shoot) {
                                int bx = x + l * dx[nonedir];
                                int by = y + l * dy[nonedir];
                                if ((int(field->mark[1 - color][by][bx]) != 3 &&
                                     (int(field->mark[1 - color][by][bx]) -
                                      nonedir) %
                                             2 !=
                                         0) ||
                                    (int(field->mark[1 - color][by][bx] ==
                                         0))) {
                                    decision[i] = nonedir + 4;
                                } else
                                    decision[i] = nonedir;
                            } else
                                decision[i] = nonedir;
                        } else
                            decision[i] = nonedir;
                    } else if (field->tankX[color][i] == baseX[1 - color]) {
                        int ddy;
                        int ddx = 0;
                        if (field->tankY[color][i] < baseY[1 - color])
                            ddy = 1;
                        else
                            ddy = -1;
                        bool shoot = !HasMultipleTank(
                            field->gameField[field->tankY[color][i]]
                                            [field->tankX[color][i]]);
                        for (int l = 1;; l++) {
                            int x = field->tankX[color][i] + l * ddx;
                            int y = field->tankY[color][i] + l * ddy;
                            if (field->gameField[y][x] == Base) break;
                            if (field->gameField[y][x] != None &&
                                field->gameField[y][x] != Brick &&
                                field->gameField[y][x] != Water) {
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
                    } else if (field->tankY[color][i] == baseY[1 - color]) {
                        int ddy = 0;
                        int ddx;
                        if (field->tankX[color][i] < baseX[1 - color])
                            ddx = 1;
                        else
                            ddx = -1;
                        bool shoot = true;
                        for (int l = 1;; l++) {
                            int x = field->tankX[color][i] + l * ddx;
                            int y = field->tankY[color][i] + l * ddy;
                            if (field->gameField[y][x] == Base) break;
                            if (field->gameField[y][x] != None &&
                                field->gameField[y][x] != Brick &&
                                field->gameField[y][x] != Water) {
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