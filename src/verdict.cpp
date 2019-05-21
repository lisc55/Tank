// Tank2 裁判程序
// 作者：289371298 upgraded from zhouhy
// https://www.botzone.org.cn/games/Tank2
// major changes in interfaces:
// field--->brickfield,waterfield & steelfield
// output["display"]=initdata["field"]--->output["display"]["brickfield"],["display"]["waterfield"],["display"]["steelfield"]
#include <cstring>
#include <ctime>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include "../json/json.h"
#include "template.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::queue;
using std::string;

namespace TankJudge {
using namespace TankGame;

int RandBetween(int from, int to) { return rand() % (to - from) + from; }

int fieldBinary[3], waterBinary[3], steelBinary[3];

bool visited[fieldHeight][fieldWidth];

/*****************copied from TankField****************/
int tankX[sideCount][tankPerSide] = {{fieldWidth / 2 - 2, fieldWidth / 2 + 2},
                                     {fieldWidth / 2 + 2, fieldWidth / 2 - 2}};
int tankY[sideCount][tankPerSide] = {{0, 0}, {fieldHeight - 1, fieldHeight - 1}};
/*****************************************************/

int dirEnumerateList[][4] = {
    {Up, Right, Down, Left}, {Up, Right, Left, Down}, {Up, Down, Right, Left}, {Up, Down, Left, Right},
    {Up, Left, Right, Down}, {Up, Left, Down, Right}, {Right, Up, Down, Left}, {Right, Up, Left, Down},
    {Right, Down, Up, Left}, {Right, Down, Left, Up}, {Right, Left, Up, Down}, {Right, Left, Down, Up},
    {Down, Up, Right, Left}, {Down, Up, Left, Right}, {Down, Right, Up, Left}, {Down, Right, Left, Up},
    {Down, Left, Up, Right}, {Down, Left, Right, Up}, {Left, Up, Right, Down}, {Left, Up, Down, Right},
    {Left, Right, Up, Down}, {Left, Right, Down, Up}, {Left, Down, Up, Right}, {Left, Down, Right, Up}};
// BFS to ensure that there is only one connected component
// InitializeField already ensures that water&steel will not appear on base and
// tank
struct node {
    int x, y;
    node() {}
    node(int xx, int yy) { x = xx, y = yy; }
};
std::queue<node> q;
bool EnsureConnected(bool hasWater[fieldHeight][fieldWidth], bool hasSteel[fieldHeight][fieldWidth]) {
    int jishu = 0, jishu2 = 0;
    bool vis[fieldHeight][fieldWidth] = {0};
    for (int i = 0; i < fieldHeight; i++)
        for (int j = 0; j < fieldWidth; j++)
            if (!(hasWater[i][j] || hasSteel[i][j])) jishu++;
    q.push(node(baseX[0], baseY[0]));
    vis[baseX[0]][baseY[0]] = 1;
    jishu2 = 1;
    while (!q.empty()) {
        int x = q.front().x, y = q.front().y;
        q.pop();
        for (int i = 0; i < 4; i++) {
            int eks = x + dx[i];
            int wai = y + dy[i];
            if (CoordValid(eks, wai) && !vis[eks][wai] && (!(hasWater[eks][wai] || hasSteel[eks][wai]))) {
                q.push(node(eks, wai));
                vis[eks][wai] = 1;
                jishu2++;
            }
        }
    }
    return jishu2 == jishu;
}
// initialize the battlefield before 1st round
// insert water here
void InitializeField() {
    memset(visited, 0, sizeof(visited));
    bool hasBrick[fieldHeight][fieldWidth] = {};
    bool hasWater[fieldHeight][fieldWidth] = {};
    bool hasSteel[fieldHeight][fieldWidth] = {};
    int portionH = (fieldHeight + 1) / 2;
    do {  // optimistic approach: assume that disconnectivity will not appear
          // normally
        for (int y = 0; y < portionH; y++)
            for (int x = 0; x < fieldWidth; x++) {
                hasBrick[y][x] = rand() % 3 > 1;  // 1/3 brick
                if (hasBrick[y][x] == false) hasWater[y][x] = rand() % 27 > 22;  // (3/4)*(4/27)= 1/9 water
                if (hasBrick[y][x] == false && hasWater[y][x] == false)
                    hasSteel[y][x] = rand() % 23 > 18;  //(3/4)*(23/27)*(4/23)=1/9 steel
            }
        int bx = baseX[0], by = baseY[0];
        hasBrick[by + 1][bx + 1] = hasBrick[by + 1][bx - 1] = hasBrick[by][bx + 1] = hasBrick[by][bx - 1] = true;
        hasWater[by + 1][bx + 1] = hasWater[by + 1][bx - 1] = hasWater[by][bx + 1] = hasWater[by][bx - 1] = false;
        hasSteel[by + 1][bx + 1] = hasSteel[by + 1][bx - 1] = hasSteel[by][bx + 1] = hasSteel[by][bx - 1] = false;
        hasBrick[by + 1][bx] = true;
        hasBrick[by][bx] = hasBrick[by][bx + 2] = hasBrick[by][bx - 2] = false;
        hasWater[by][bx] = hasWater[by + 1][bx] = hasWater[by][bx + 2] = hasWater[by][bx - 2] = false;
        hasSteel[by][bx] = hasSteel[by + 1][bx] = hasSteel[by][bx + 2] = hasSteel[by][bx - 2] = false;
        // symmetry
        for (int y = 0; y < portionH; y++)
            for (int x = 0; x < fieldWidth; x++) {
                hasBrick[fieldHeight - y - 1][fieldWidth - x - 1] = hasBrick[y][x];
                hasWater[fieldHeight - y - 1][fieldWidth - x - 1] = hasWater[y][x];
                hasSteel[fieldHeight - y - 1][fieldWidth - x - 1] = hasSteel[y][x];
            }
        // separate the field into 4 pieces, each with a tank
        for (int y = 2; y < fieldHeight - 2; y++) {
            hasBrick[y][fieldWidth / 2] = true;
            hasWater[y][fieldWidth / 2] = false;
            hasSteel[y][fieldWidth / 2] = false;
        }
        for (int x = 0; x < fieldWidth; x++) {
            hasBrick[fieldHeight / 2][x] = true;
            hasWater[fieldHeight / 2][x] = false;
            hasSteel[fieldHeight / 2][x] = false;
        }
        for (int side = 0; side < sideCount; side++) {
            for (int tank = 0; tank < tankPerSide; tank++)
                hasSteel[tankY[side][tank]][tankX[side][tank]] = hasWater[tankY[side][tank]][tankX[side][tank]] = false;
            hasSteel[baseY[side]][baseX[side]] = hasWater[baseY[side]][baseX[side]] =
                hasBrick[baseY[side]][baseX[side]] = false;
        }
        // add steel onto midpoint&midtankpoint
        hasBrick[fieldHeight / 2][fieldWidth / 2] = false;
        hasWater[fieldHeight / 2][fieldWidth / 2] = false;
        hasSteel[fieldHeight / 2][fieldWidth / 2] = true;

        for (int tank = 0; tank < tankPerSide; tank++) {
            hasSteel[fieldHeight / 2][tankX[0][tank]] = true;
            hasWater[fieldHeight / 2][tankX[0][tank]] = hasBrick[fieldHeight / 2][tankX[0][tank]] = false;
        }
    } while (!EnsureConnected(hasWater, hasSteel));
    for (int i = 0; i < 3; i++)  // 3 row as one number
    {
        int mask = 1;
        for (int y = i * 3; y < (i + 1) * 3; y++) {
            for (int x = 0; x < fieldWidth; x++) {
                if (hasBrick[y][x])
                    fieldBinary[i] |= mask;
                else if (hasWater[y][x])
                    waterBinary[i] |= mask;
                else if (hasSteel[y][x])
                    steelBinary[i] |= mask;
                mask <<= 1;
            }
        }
    }
}
}  // namespace TankJudge

void validateImpl() {
    srand(time(0));
    int i = 0;
    while (true) {
        cout << i++ << endl;
        if (TankGame::field) delete TankGame::field;
        std::stack<TankGame::TankField> history;
        TankJudge::InitializeField();
        TankGame::field =
            new TankGame::TankField(TankJudge::fieldBinary, TankJudge::waterBinary, TankJudge::steelBinary, 0);
        while (TankGame::field->GetGameResult() == -2) {
            for (int side = 0; side < TankGame::sideCount; side++)
                for (int tank = 0; tank < TankGame::tankPerSide; tank++)
                    while (true) {
                        auto act = (TankGame::Action)TankJudge::RandBetween(TankGame::Stay, TankGame::LeftShoot + 1);
                        if (TankGame::field->ActionIsValid(side, tank, act)) {
                            TankGame::field->nextAction[side][tank] = act;
                            break;
                        }
                    }
            history.push(*TankGame::field);
            TankGame::field->DoAction();
        }
        while (!history.empty()) {
            auto& f = history.top();
            auto bak = *TankGame::field;
            TankGame::field->Revert();
            if (f != *TankGame::field) {
                f.DoAction();
                cout << "error";
            }
            history.pop();
        }
    }
}
/*
int main()
{
    unsigned int seed;
    const string int2str[] = { "0", "1" };

    Json::Reader reader;
    Json::Value input, temp, output;
    #ifdef _BOTZONE_ONLINE
    reader.parse(cin, input);
    #else
    char
*s="{\"log\":[{\"keep_running\":false,\"memory\":160,\"output\":{\"command\":\"request\",\"content\":{\"0\":{\"brickfield\":[71620266,4718352,44783889],\"waterfield\":[0,0,0],\"steelfield\":[0,0,0],\"mySide\":0},\"1\":{\"brickfield\":[71620266,4718352,44783889],\"waterfield\":[0,0,0],\"steelfield\":[0,0,0],\"mySide\":1}},\"display\":[71620266,4718352,44783889]},\"time\":3,\"verdict\":\"OK\"},{\"0\":{\"keep_running\":true,\"memory\":165,\"time\":89,\"verdict\":\"OK\",\"debug\":\"DEBUG!\",\"response\":[6,7]},\"1\":{\"keep_running\":true,\"memory\":20,\"time\":4,\"verdict\":\"OK\",\"response\":[0,4]}},{\"keep_running\":false,\"memory\":15,\"output\":{\"command\":\"request\",\"content\":{\"0\":[0,4],\"1\":[6,7]},\"display\":{\"0\":[6,7],\"1\":[0,4]}},\"time\":3,\"verdict\":\"OK\"},{\"0\":{\"keep_running\":true,\"memory\":165,\"time\":1,\"verdict\":\"OK\",\"debug\":\"DEBUG!\",\"response\":[2,-1]},\"1\":{\"keep_running\":true,\"memory\":20,\"time\":0,\"verdict\":\"OK\",\"response\":[0,0]}},{\"keep_running\":false,\"memory\":15,\"output\":{\"command\":\"request\",\"content\":{\"0\":[0,0],\"1\":[2,-1]},\"display\":{\"0\":[2,-1],\"1\":[0,0]}},\"time\":2,\"verdict\":\"OK\"},{\"0\":{\"keep_running\":true,\"memory\":165,\"time\":0,\"verdict\":\"OK\",\"debug\":\"DEBUG!\",\"response\":[0,7]},\"1\":{\"keep_running\":true,\"memory\":20,\"time\":0,\"verdict\":\"OK\",\"response\":[7,0]}}],\"initdata\":{\"brickfield\":[71620266,4718352,44783889],\"waterfield\":[0,0,0],\"steelfield\":[0,0,0],\"maxTurn\":100,\"seed\":1552650152}}";
    reader.parse(s,input);

    //test
    {Json::FastWriter writer;
     printf("%s\n\n",s);
    cout << writer.write(input);}
    #endif
    Json::Value initdata = input["initdata"];

    if (initdata.isString())
        reader.parse(initdata.asString(), initdata);
    if (initdata.isString())
        initdata = Json::Value(Json::objectValue);

    temp = initdata["maxTurn"];
    if (temp.isInt())
        TankGame::maxTurn = temp.asUInt();
    initdata["maxTurn"] = TankGame::maxTurn;

    temp = initdata["seed"];
    if (temp.isInt())
        srand(seed = temp.asUInt());
    else
        srand(seed = time(nullptr));
    initdata["seed"] = seed;

    temp = initdata["brickfield"];
    if (temp.isArray() && !temp.isNull()){
        for (int i = 0; i < 3; i++)
            TankJudge::fieldBinary[i] = temp[i].asInt();
        initdata["brickfield"] = temp;
        temp = initdata["waterfield"];
        for (int i = 0; i < 3; i++)
            TankJudge::waterBinary[i] = temp[i].asInt();
        initdata["waterfield"] = temp;
        temp = initdata["steelfield"];
        for (int i = 0; i < 3; i++)
            TankJudge::steelBinary[i] = temp[i].asInt();
        initdata["steelfield"] = temp;
    }
    else
    {
        TankJudge::InitializeField();
        temp = Json::Value(Json::arrayValue);
        for (int i = 0; i < 3; i++)
            temp[i] = TankJudge::fieldBinary[i];
        initdata["brickfield"] = temp;
        temp = Json::Value(Json::arrayValue);
        for (int i = 0; i < 3; i++)
            temp[i] = TankJudge::waterBinary[i];
        initdata["waterfield"] = temp;
        temp = Json::Value(Json::arrayValue);
        for (int i = 0; i < 3; i++)
            temp[i] = TankJudge::steelBinary[i];
        initdata["steelfield"] = temp;
    }


    TankGame::field = new
TankGame::TankField(TankJudge::fieldBinary,TankJudge::waterBinary,TankJudge::steelBinary,
0);

    input = input["log"];

    int size = input.size();
    if (size == 0)//before 1st round
    {
        for (int side = 0; side < TankGame::sideCount; side++)
        {
            auto obj = Json::Value(Json::objectValue);
            obj["brickfield"] = initdata["brickfield"];
            obj["waterfield"]=initdata["waterfield"];
            obj["steelfield"]=initdata["steelfield"];
            obj["mySide"] = side;
            output["content"][int2str[side]] = obj;
        }

        output["initdata"] = initdata;
        output["command"] = "request";
        //display has brick,water and steel
        output["display"]["brick"] = initdata["brickfield"];
        output["display"]["water"] = initdata["waterfield"];
        output["display"]["steel"] = initdata["steelfield"];
    }
    else
    {
        bool invalid[TankGame::sideCount] = {};
        //winning side {0,1}
        auto setWinner = [&] (int to) {
            if (to == -1)
                output["content"]["0"] = output["content"]["1"] = 1;
            else if (to == 1)
            {
                output["content"]["0"] = 0;
                output["content"]["1"] = 2;
            }
            else
            {
                output["content"]["0"] = 2;
                output["content"]["1"] = 0;
            }
        };
        for (int i = 1; i < size; i += 2)
        {
            bool isLast = size - 1 == i;
            Json::Value response = input[i];
            for (int side = 0; side < TankGame::sideCount; side++)//simulate
each round
            {
                Json::Value raw = response[int2str[side]],
                    answer = raw["response"].isNull() ? raw["content"] :
raw["response"]; TankGame::Action act0, act1; if (answer.isArray() &&
answer[0U].isInt() && answer[1U].isInt())
                {
                    act0 = (TankGame::Action)answer[0U].asInt();
                    act1 = (TankGame::Action)answer[1U].asInt();
                    if (isLast)
                    {
                        auto action = Json::Value(Json::arrayValue);
                        action[0U] = act0;
                        action[1U] = act1;
                        output["display"][int2str[side]] =
output["content"][int2str[1 - side]] = action; if
(!TankGame::field->tankAlive[side][0] || !TankGame::field->ActionIsValid(side,
0, act0)) output["content"][int2str[1 - side]][0U] = -1; if
(!TankGame::field->tankAlive[side][1] || !TankGame::field->ActionIsValid(side,
1, act1)) output["content"][int2str[1 - side]][1U] = -1;
                    }
                    if ((!TankGame::field->tankAlive[side][0] ||
TankGame::field->ActionIsValid(side, 0, act0)) &&
                        (!TankGame::field->tankAlive[side][1] ||
TankGame::field->ActionIsValid(side, 1, act1)))
                    {
                        TankGame::field->nextAction[side][0] = act0;
                        TankGame::field->nextAction[side][1] = act1;
                        continue;
                    }
                }
                invalid[side] = true;
                output["display"]["loseReason"][side] = "INVALID_INPUT_VERDICT_"
+ raw["verdict"].asString();
            }
            if (invalid[0] || invalid[1])
            {
                output["command"] = "finish";
                if (invalid[0] == invalid[1])
                    setWinner(-1);
                else if (invalid[0])
                    setWinner(1);
                else
                    setWinner(0);
                break;
            }
            else
                TankGame::field->DoAction();

            int result = TankGame::field->GetGameResult();
            if (result != -2)
            {
                output["command"] = "finish";
                setWinner(result);
                for (int side = 0; side < TankGame::sideCount; side++)
                {
                    bool tankExist = TankGame::field->tankAlive[side][0] ||
TankGame::field->tankAlive[side][1]; bool baseExist =
TankGame::field->baseAlive[side]; if (!tankExist && !baseExist)
                        output["display"]["loseReason"][side] =
"BASE_TANK_ALL_DESTROYED"; else if (!tankExist)
                        output["display"]["loseReason"][side] =
"TANK_ALL_DESTROYED"; else if (!baseExist) output["display"]["loseReason"][side]
= "BASE_DESTROYED";
                }
                break;
            }
            else if (isLast)
                output["command"] = "request";
            TankGame::field->DebugPrint();
        }
    }

    Json::FastWriter writer;
    cout << writer.write(output);
}
*/