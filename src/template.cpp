#include "template.h"

#define LZZ_INLINE inline
using std::cin;
using std::cout;
using std::endl;
using std::flush;
using std::getline;
using std::greater;
using std::pair;
using std::priority_queue;
using std::queue;
using std::string;
using std::vector;
namespace TankGame {
int const(baseX)[sideCount] = {fieldWidth / 2, fieldWidth / 2};
}
namespace TankGame {
int const(baseY)[sideCount] = {0, fieldHeight - 1};
}
namespace TankGame {
int const(dx)[4] = {0, 1, 0, -1};
}
namespace TankGame {
int const(dy)[4] = {-1, 0, 1, 0};
}
namespace TankGame {
FieldItem const((tankItemTypes)[sideCount])[tankPerSide] = {{Blue0, Blue1},
                                                            {Red0, Red1}};
}
namespace TankGame {
int maxTurn = 100;
}
namespace TankGame {
bool DisappearLog::operator<(DisappearLog const &b) const {
    if (x == b.x) {
        if (y == b.y) return item < b.item;
        return y < b.y;
    }
    return x < b.x;
}
}  // namespace TankGame
namespace TankGame {
bool TankField::ActionIsValid(int side, int tank, Action act) {
    if (act == Invalid) return false;
    if (act > Left &&
        previousActions[currentTurn - 1][side][tank] > Left)  // 连续两回合射击
        return false;
    if (act == Stay || act > Left) return true;
    int x = tankX[side][tank] + dx[act], y = tankY[side][tank] + dy[act];
    return CoordValid(x, y) &&
           gameField[y][x] == None;  // water cannot be stepped on
}
}  // namespace TankGame
namespace TankGame {
bool TankField::ActionIsValid() {
    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++)
            if (tankAlive[side][tank] &&
                !ActionIsValid(side, tank, nextAction[side][tank]))
                return false;
    return true;
}
}  // namespace TankGame
namespace TankGame {
void TankField::_destroyTank(int side, int tank) {
    tankAlive[side][tank] = false;
    tankX[side][tank] = tankY[side][tank] = -1;
}
}  // namespace TankGame
namespace TankGame {
void TankField::_revertTank(int side, int tank, DisappearLog &log) {
    int &currX = tankX[side][tank], &currY = tankY[side][tank];
    if (tankAlive[side][tank])
        gameField[currY][currX] &= ~tankItemTypes[side][tank];
    else
        tankAlive[side][tank] = true;
    currX = log.x;
    currY = log.y;
    gameField[currY][currX] |= tankItemTypes[side][tank];
}
}  // namespace TankGame
namespace TankGame {
bool TankField::DoAction() {
    if (!ActionIsValid()) return false;

    // 1 移动
    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++) {
            Action act = nextAction[side][tank];

            // 保存动作
            previousActions[currentTurn][side][tank] = act;
            if (tankAlive[side][tank] && ActionIsMove(act)) {
                int &x = tankX[side][tank], &y = tankY[side][tank];
                FieldItem &items = gameField[y][x];

                // 记录 Log
                DisappearLog log;
                log.x = x;
                log.y = y;
                log.item = tankItemTypes[side][tank];
                log.turn = currentTurn;
                logs.push(log);

                // 变更坐标
                x += dx[act];
                y += dy[act];

                // 更换标记（注意格子可能有多个坦克）
                gameField[y][x] |= log.item;
                items &= ~log.item;
            }
        }

    // 2 射击!
    set<DisappearLog> itemsToBeDestroyed;
    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++) {
            Action act = nextAction[side][tank];
            if (tankAlive[side][tank] && ActionIsShoot(act)) {
                int dir = ExtractDirectionFromAction(act);
                int x = tankX[side][tank], y = tankY[side][tank];
                bool hasMultipleTankWithMe = HasMultipleTank(gameField[y][x]);
                while (true) {
                    x += dx[dir];
                    y += dy[dir];
                    if (!CoordValid(x, y)) break;
                    FieldItem items = gameField[y][x];
                    // tank will not be on water, and water will not be shot, so
                    // it can be handled as None
                    if (items != None && items != Water) {
                        // 对射判断
                        if (items >= Blue0 && !hasMultipleTankWithMe &&
                            !HasMultipleTank(items)) {
                            // 自己这里和射到的目标格子都只有一个坦克
                            Action theirAction = nextAction[GetTankSide(items)]
                                                           [GetTankID(items)];
                            if (ActionIsShoot(theirAction) &&
                                ActionDirectionIsOpposite(act, theirAction)) {
                                // 而且我方和对方的射击方向是反的
                                // 那么就忽视这次射击
                                break;
                            }
                        }

                        // 标记这些物件要被摧毁了（防止重复摧毁）
                        for (int mask = 1; mask <= Red1; mask <<= 1)
                            if (items & mask) {
                                DisappearLog log;
                                log.x = x;
                                log.y = y;
                                log.item = (FieldItem)mask;
                                log.turn = currentTurn;
                                itemsToBeDestroyed.insert(log);
                            }
                        break;
                    }
                }
            }
        }

    for (auto &log : itemsToBeDestroyed) {
        switch (log.item) {
            case Base: {
                int side =
                    log.x == baseX[Blue] && log.y == baseY[Blue] ? Blue : Red;
                baseAlive[side] = false;
                break;
            }
            case Blue0:
                _destroyTank(Blue, 0);
                break;
            case Blue1:
                _destroyTank(Blue, 1);
                break;
            case Red0:
                _destroyTank(Red, 0);
                break;
            case Red1:
                _destroyTank(Red, 1);
                break;
            case Steel:
                continue;
            default:;
        }
        gameField[log.y][log.x] &= ~log.item;
        logs.push(log);
    }

    // for (int side = 0; side < sideCount; side++)
        // for (int tank = 0; tank < tankPerSide; tank++)
            // nextAction[side][tank] = Invalid;

    currentTurn++;
    return true;
}
}  // namespace TankGame
namespace TankGame {
bool TankField::Revert() {
    if (currentTurn == 1) return false;

    currentTurn--;
    while (!logs.empty()) {
        DisappearLog &log = logs.top();
        if (log.turn == currentTurn) {
            logs.pop();
            switch (log.item) {
                case Base: {
                    int side = log.x == baseX[Blue] && log.y == baseY[Blue] ?
                                   Blue :
                                   Red;
                    baseAlive[side] = true;
                    gameField[log.y][log.x] = Base;
                    break;
                }
                case Brick:
                    gameField[log.y][log.x] = Brick;
                    break;
                case Blue0:
                    _revertTank(Blue, 0, log);
                    break;
                case Blue1:
                    _revertTank(Blue, 1, log);
                    break;
                case Red0:
                    _revertTank(Red, 0, log);
                    break;
                case Red1:
                    _revertTank(Red, 1, log);
                    break;
                default:;
            }
        } else
            break;
    }
    return true;
}
}  // namespace TankGame
namespace TankGame {
GameResult TankField::GetGameResult() {
    bool fail[sideCount] = {};
    for (int side = 0; side < sideCount; side++)
        if ((!tankAlive[side][0] && !tankAlive[side][1]) || !baseAlive[side])
            fail[side] = true;
    if (fail[0] == fail[1])
        return fail[0] || currentTurn > maxTurn ? Draw : NotFinished;
    if (fail[Blue]) return Red;
    return Blue;
}
}  // namespace TankGame
namespace TankGame {
TankField::TankField(int(hasBrick)[3],
                     int(hasWater)[3],
                     int(hasSteel)[3],
                     int mySide)
    : mySide(mySide) {
    for (int i = 0; i < 3; i++) {
        int mask = 1;
        for (int y = i * 3; y < (i + 1) * 3; y++) {
            for (int x = 0; x < fieldWidth; x++) {
                if (hasBrick[i] & mask)
                    gameField[y][x] = Brick;
                else if (hasWater[i] & mask)
                    gameField[y][x] = Water;
                else if (hasSteel[i] & mask)
                    gameField[y][x] = Steel;
                mask <<= 1;
            }
        }
    }
    for (int side = 0; side < sideCount; side++) {
        for (int tank = 0; tank < tankPerSide; tank++)
            gameField[tankY[side][tank]][tankX[side][tank]] =
                tankItemTypes[side][tank];
        gameField[baseY[side]][baseX[side]] = Base;
    }
}
}  // namespace TankGame
namespace TankGame {
void TankField::DebugPrint() {
#ifndef _BOTZONE_ONLINE
    const string side2String[] = {"blue", "red"};
    const string boolean2String[] = {"exploded", "alive"};
    const char *boldHR = "==============================";
    const char *slimHR = "------------------------------";
    cout << boldHR << endl
         << "legend: " << endl
         << ". - empty\t# - brick\t% - steel\t* - base\t@ - more than 1 tanks"
         << endl
         << "b - blue0\tB - blue1\tr - red0\tR - red1\tW - water"
         << endl  // Tank2 feature
         << slimHR << endl;
    for (int y = 0; y < fieldHeight; y++) {
        for (int x = 0; x < fieldWidth; x++) {
            switch (gameField[y][x]) {
                case None:
                    cout << '.';
                    break;
                case Brick:
                    cout << '#';
                    break;
                case Steel:
                    cout << '%';
                    break;
                case Base:
                    cout << '*';
                    break;
                case Blue0:
                    cout << 'b';
                    break;
                case Blue1:
                    cout << 'B';
                    break;
                case Red0:
                    cout << 'r';
                    break;
                case Red1:
                    cout << 'R';
                    break;
                case Water:
                    cout << 'W';
                    break;
                default:
                    cout << '@';
                    break;
            }
            cout << ' ';
        }
        cout << endl;
    }
    cout << "blue distance:" << endl;
    for (int i = 0; i < fieldHeight; i++) {
        for (int j = 0; j < fieldWidth; j++) {
            cout << std::setw(2) << distance[0][i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
    cout << "red distance:" << endl;
    for (int i = 0; i < fieldHeight; i++) {
        for (int j = 0; j < fieldWidth; j++) {
            cout << std::setw(2) << distance[1][i][j] << ' ';
        }
        cout << endl;
    }
    cout << slimHR << endl;
    for (int side = 0; side < sideCount; side++) {
        cout << side2String[side] << ": base" << ' '
             << boolean2String[baseAlive[side]];
        for (int tank = 0; tank < tankPerSide; tank++)
            cout << ", tank" << tank << ' '
                 << boolean2String[tankAlive[side][tank]];
        cout << endl;
    }
    cout << "current turn: " << currentTurn << ", ";
    GameResult result = GetGameResult();
    if (result == -2)
        cout << "The game is not over" << endl;
    else if (result == -1)
        cout << "Draw" << endl;
    else
        cout << side2String[result] << " wins" << endl;
    cout << boldHR << endl;
#endif
}
}  // namespace TankGame
namespace TankGame {
bool TankField::operator!=(TankField const &b) const {
    for (int y = 0; y < fieldHeight; y++)
        for (int x = 0; x < fieldWidth; x++)
            if (gameField[y][x] != b.gameField[y][x]) return true;

    for (int side = 0; side < sideCount; side++)
        for (int tank = 0; tank < tankPerSide; tank++) {
            if (tankX[side][tank] != b.tankX[side][tank]) return true;
            if (tankY[side][tank] != b.tankY[side][tank]) return true;
            if (tankAlive[side][tank] != b.tankAlive[side][tank]) return true;
        }

    if (baseAlive[0] != b.baseAlive[0] || baseAlive[1] != b.baseAlive[1])
        return true;

    if (currentTurn != b.currentTurn) return true;

    return false;
}
}  // namespace TankGame
namespace TankGame {
TankField *field;
}
namespace TankGame {
namespace Internals {
Json::Reader reader;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
Json::StyledWriter writer;
}
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _processRequestOrResponse(Json::Value &value, bool isOpponent) {
    if (value.isArray()) {
        if (!isOpponent) {
            for (int tank = 0; tank < tankPerSide; tank++)
                field->nextAction[field->mySide][tank] =
                    (Action)value[tank].asInt();
        } else {
            for (int tank = 0; tank < tankPerSide; tank++)
                field->nextAction[1 - field->mySide][tank] =
                    (Action)value[tank].asInt();
            field->DoAction();
        }
    } else {
        // 是第一回合，裁判在介绍场地
        int hasBrick[3], hasWater[3], hasSteel[3];
        for (int i = 0; i < 3; i++) {  // Tank2 feature(?????????????]
            hasWater[i] = value["waterfield"][i].asInt();
            hasBrick[i] = value["brickfield"][i].asInt();
            hasSteel[i] = value["steelfield"][i].asInt();
        }
        field = new TankField(hasBrick, hasWater, hasSteel,
                              value["mySide"].asInt());
    }
}
}  // namespace Internals
}  // namespace TankGame
namespace TankGame {
namespace Internals {
void _submitAction(Action tank0,
                   Action tank1,
                   string debug,
                   string data,
                   string globalData) {
    Json::Value output(Json::objectValue), response(Json::arrayValue);
    response[0U] = tank0;
    response[1U] = tank1;
    output["response"] = response;
    if (!debug.empty()) output["debug"] = debug;
    if (!data.empty()) output["data"] = data;
    if (!globalData.empty()) output["globalData"] = globalData;
    cout << writer.write(output) << endl;
}
}  // namespace Internals
}  // namespace TankGame
namespace TankGame {
void ReadInput(istream &in, string &outData, string &outGlobalData) {
    Json::Value input;
    string inputString;
    do { getline(in, inputString); } while (inputString.empty());
#ifndef _BOTZONE_ONLINE
    // 猜测是单行还是多行
    char lastChar = inputString[inputString.size() - 1];
    if (lastChar != '}' && lastChar != ']') {
        // 第一行不以}或]结尾，猜测是多行
        string newString;
        do {
            getline(in, newString);
            inputString += newString;
        } while (newString != "}" && newString != "]");
    }
#endif
    Internals::reader.parse(inputString, input);

    if (input.isObject()) {
        Json::Value requests = input["requests"],
                    responses = input["responses"];
        if (!requests.isNull() && requests.isArray()) {
            size_t i, n = requests.size();
            for (i = 0; i < n; i++) {
                Internals::_processRequestOrResponse(
                    requests[(Json::Value::UInt)i], true);
                if (i < n - 1)
                    Internals::_processRequestOrResponse(
                        responses[(Json::Value::UInt)i], false);
            }
            outData = input["data"].asString();
            outGlobalData = input["globaldata"].asString();
            return;
        }
    }
    Internals::_processRequestOrResponse(input, true);
}
}  // namespace TankGame
namespace TankGame {
void SubmitAndExit(Action tank0,
                   Action tank1,
                   string debug,
                   string data,
                   string globalData) {
    Internals::_submitAction(tank0, tank1, debug, data, globalData);
    exit(0);
}
}  // namespace TankGame
namespace TankGame {
void SubmitAndDontExit(Action tank0, Action tank1) {
    Internals::_submitAction(tank0, tank1);
    field->nextAction[field->mySide][0] = tank0;
    field->nextAction[field->mySide][1] = tank1;
    cout << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << endl;
}
}  // namespace TankGame

int RandBetween(int from, int to) { return rand() % (to - from) + from; }

TankGame::Action RandAction(int tank) {
    while (true) {
        auto act = (TankGame::Action)RandBetween(TankGame::Stay,
                                                 TankGame::LeftShoot + 1);
        if (TankGame::field->ActionIsValid(TankGame::field->mySide, tank, act))
            return act;
    }
}

#undef LZZ_INLINE
