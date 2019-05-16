#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include "../json/json.h"

#ifndef TANK_TEMPLATE_H
#define TANK_TEMPLATE_H
#define LZZ_INLINE inline
namespace TankGame {
enum GameResult { NotFinished = -2, Draw = -1, Blue = 0, Red = 1 };
}
namespace TankGame {
using std::istream;
using std::set;
using std::stack;
using std::string;
}  // namespace TankGame
namespace TankGame {
enum FieldItem {
    None = 0,
    Brick = 1,
    Steel = 2,
    Base = 4,
    Blue0 = 8,
    Blue1 = 16,
    Red0 = 32,
    Red1 = 64,
    Water = 128
};
}
namespace TankGame {
enum FireDirection { Nof = 0, X = 1, Y = 2, Both = 3 };
}
namespace TankGame {
template <typename T>
T operator~(T a);
}
namespace TankGame {
template <typename T>
T operator|(T a, T b);
}
namespace TankGame {
template <typename T>
T operator&(T a, T b);
}
namespace TankGame {
template <typename T>
T operator^(T a, T b);
}
namespace TankGame {
template <typename T>
T &operator|=(T &a, T b);
}
namespace TankGame {
template <typename T>
T &operator&=(T &a, T b);
}
namespace TankGame {
template <typename T>
T &operator^=(T &a, T b);
}
namespace TankGame {
enum Action {
    Invalid = -2,
    Stay = -1,
    Up,
    Right,
    Down,
    Left,
    UpShoot,
    RightShoot,
    DownShoot,
    LeftShoot
};
}
namespace TankGame {
int const fieldHeight = 9;
}
namespace TankGame {
int const fieldWidth = 9;
}
namespace TankGame {
int const sideCount = 2;
}
namespace TankGame {
int const tankPerSide = 2;
}
namespace TankGame {
extern int const(baseX)[sideCount];
}
namespace TankGame {
extern int const(baseY)[sideCount];
}
namespace TankGame {
extern int const(dx)[4];
}
namespace TankGame {
extern int const(dy)[4];
}
namespace TankGame {
extern FieldItem const((tankItemTypes)[sideCount])[tankPerSide];
}
namespace TankGame {
extern int maxTurn;
}
namespace TankGame {
bool ActionIsMove(Action x);
}
namespace TankGame {
bool ActionIsShoot(Action x);
}
namespace TankGame {
bool ActionDirectionIsOpposite(Action a, Action b);
}
namespace TankGame {
bool CoordValid(int x, int y);
}
namespace TankGame {
bool HasMultipleTank(FieldItem item);
}
namespace TankGame {
int GetTankSide(FieldItem item);
}
namespace TankGame {
int GetTankID(FieldItem item);
}
namespace TankGame {
int ExtractDirectionFromAction(Action x);
}
namespace TankGame {
struct DisappearLog {
    FieldItem item;
    int turn;
    int x;
    int y;

    bool operator<(DisappearLog const &b) const;
};
}  // namespace TankGame
namespace TankGame {
class TankField {
   public:
    FireDirection mark[sideCount][fieldHeight][fieldWidth] = {};
    int distance[sideCount][fieldHeight][fieldWidth] = {};
    FieldItem gameField[fieldHeight][fieldWidth];
    bool tankAlive[sideCount][tankPerSide] = {{true, true}, {true, true}};
    bool baseAlive[sideCount] = {true, true};
    int tankX[sideCount][tankPerSide] = {
        {fieldWidth / 2 - 2, fieldWidth / 2 + 2},
        {fieldWidth / 2 + 2, fieldWidth / 2 - 2}};
    int tankY[sideCount][tankPerSide] = {{0, 0},
                                         {fieldHeight - 1, fieldHeight - 1}};
    int currentTurn = 1;
    int mySide;
    stack<DisappearLog> logs;
    Action previousActions[101][sideCount][tankPerSide] = {
        {{Stay, Stay}, {Stay, Stay}}};
    Action nextAction[sideCount][tankPerSide] = {{Invalid, Invalid},
                                                 {Invalid, Invalid}};

    bool ActionIsValid(int side, int tank, Action act);

    bool ActionIsValid();

   private:
    void _destroyTank(int side, int tank);

    void _revertTank(int side, int tank, DisappearLog &log);

   public:
    bool DoAction();

    bool Revert();

    GameResult GetGameResult();

    TankField(int(hasBrick)[3], int(hasWater)[3], int(hasSteel)[3], int mySide);

    void DebugPrint();

    bool operator!=(TankField const &b) const;
};
}  // namespace TankGame
namespace TankGame {
extern TankField *field;
}
namespace TankGame {
namespace Internals {
extern Json::Reader reader;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
extern Json::StyledWriter writer;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _processRequestOrResponse(Json::Value &value, bool isOpponent);
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _submitAction(Action tank0,
                   Action tank1,
                   string debug = "",
                   string data = "",
                   string globalData = "");
}
}  // namespace TankGame
namespace TankGame {
void ReadInput(istream &in, string &outData, string &outGlobalData);
}
namespace TankGame {
void SubmitAndExit(Action tank0,
                   Action tank1,
                   string debug = "",
                   string data = "",
                   string globalData = "");
}
namespace TankGame {
void SubmitAndDontExit(Action tank0, Action tank1);
}

int RandBetween(int from, int to);

TankGame::Action RandAction(int tank);

namespace TankGame {
template <typename T>
LZZ_INLINE T operator~(T a) {
    return (T) ~(int)a;
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T operator|(T a, T b) {
    return (T)((int)a | (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T operator&(T a, T b) {
    return (T)((int)a & (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T operator^(T a, T b) {
    return (T)((int)a ^ (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T &operator|=(T &a, T b) {
    return (T &)((int &)a |= (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T &operator&=(T &a, T b) {
    return (T &)((int &)a &= (int)b);
}
}  // namespace TankGame
namespace TankGame {
template <typename T>
LZZ_INLINE T &operator^=(T &a, T b) {
    return (T &)((int &)a ^= (int)b);
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool ActionIsMove(Action x) { return x >= Up && x <= Left; }
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool ActionIsShoot(Action x) {
    return x >= UpShoot && x <= LeftShoot;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool ActionDirectionIsOpposite(Action a, Action b) {
    return a >= Up && b >= Up && (a + 2) % 4 == b % 4;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool CoordValid(int x, int y) {
    return x >= 0 && x < fieldWidth && y >= 0 && y < fieldHeight;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE bool HasMultipleTank(FieldItem item) {
    // 如果格子上只有一个物件，那么 item 的值是 2 的幂或 0
    // 对于数字 x，x & (x - 1) == 0 当且仅当 x 是 2 的幂或 0
    return !!(item & (item - 1));
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE int GetTankSide(FieldItem item) {
    return item == Blue0 || item == Blue1 ? Blue : Red;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE int GetTankID(FieldItem item) {
    return item == Blue0 || item == Red0 ? 0 : 1;
}
}  // namespace TankGame
namespace TankGame {
LZZ_INLINE int ExtractDirectionFromAction(Action x) {
    if (x >= Up) return x % 4;
    return -1;
}
}  // namespace TankGame
#undef LZZ_INLINE
#endif
