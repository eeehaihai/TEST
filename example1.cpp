// 同化棋（Ataxx）简易交互样例程序
// 随机策略
// 作者：zhouhy zys
// 游戏信息：http://www.botzone.org/games#Ataxx

#include <iostream> // 标准输入输出流库
#include <string> // 字符串库
#include <cstdlib> // 标准库中的通用工具函数
#include <ctime> // 时间相关的库

using namespace std; // 使用标准命名空间

int currBotColor; // 当前机器人执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[7][7] = { 0 }; // 先x后y，记录棋盘状态，初始化为全0
int blackPieceCount = 2, whitePieceCount = 2; // 黑白棋子数量初始为2
int startX, startY, resultX, resultY; // 起始和结果坐标
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 }, // 方向数组，用于表示移动方向
                            { -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
                            { 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
                            { 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
                            { -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
                            { 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };

// 判断坐标是否在地图内
inline bool inMap(int x, int y)
{
    if (x < 0 || x > 6 || y < 0 || y > 6) // 检查是否超出边界
        return false; // 超出返回false
    return true; // 未超出返回true
}

// 向指定方向移动，并返回是否越界
inline bool MoveStep(int &x, int &y, int Direction)
{
    x = x + delta[Direction][0]; // 改变x坐标
    y = y + delta[Direction][1]; // 改变y坐标
    return inMap(x, y); // 返回是否在地图内
}

// 在指定坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int color)
{
    if (color == 0) // 颜色为0（无子），不合法
        return false;
    if (x1 == -1) // 无法移动，跳过此回合
        return true;
    if (!inMap(x0, y0) || !inMap(x1, y1)) // 坐标超出边界
        return false;
    if (gridInfo[x0][y0] != color) // 起始位置无当前颜色的子
        return false;

    int dx, dy, x, y, currCount = 0, dir; // 定义变量
    int effectivePoints[8][2]; // 有效位置数组
    dx = abs((x0 - x1)), dy = abs((y0 - y1)); // 计算横纵坐标差值
    if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 移动范围超出5x5区域或未移动
        return false;
    if (gridInfo[x1][y1] != 0) // 目标位置非空
        return false;
    if (dx == 2 || dy == 2) // 如果走的是5x5的外围，则不是复制粘贴
        gridInfo[x0][y0] = 0; // 原位置清空
    else
    {
        if (color == 1)
            blackPieceCount++; // 黑子数量增加
        else
            whitePieceCount++; // 白子数量增加
    }

    gridInfo[x1][y1] = color; // 目标位置放置当前颜色的子
    for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
    {
        x = x1 + delta[dir][0]; // 计算新x坐标
        y = y1 + delta[dir][1]; // 计算新y坐标
        if (!inMap(x, y)) // 新坐标是否在地图内
            continue;
        if (gridInfo[x][y] == -color) // 如果是对方的子
        {
            effectivePoints[currCount][0] = x; // 记录有效位置
            effectivePoints[currCount][1] = y;
            currCount++;
            gridInfo[x][y] = color; // 转变为己方的子
        }
    }
    if (currCount != 0)
    {
        if (color == 1)
        {
            blackPieceCount += currCount; // 黑子数量增加
            whitePieceCount -= currCount; // 白子数量减少
        }
        else
        {
            whitePieceCount += currCount; // 白子数量增加
            blackPieceCount -= currCount; // 黑子数量减少
        }
    }
    return true; // 落子成功
}

int main()
{
    istream::sync_with_stdio(false); // 关闭同步

    int x0, y0, x1, y1; // 定义坐标变量

    // 初始化棋盘
    gridInfo[0][0] = gridInfo[6][6] = 1;  // 黑子
    gridInfo[6][0] = gridInfo[0][6] = -1; // 白子

    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID;
    currBotColor = -1; // 第一回合收到坐标是-1, -1，说明我是黑方
    cin >> turnID; // 输入回合数
    for (int i = 0; i < turnID - 1; i++)
    {
        // 根据这些输入输出逐渐恢复状态到当前回合
        cin >> x0 >> y0 >> x1 >> y1; // 输入对方走法
        if (x1 >= 0)
            ProcStep(x0, y0, x1, y1, -currBotColor); // 模拟对方落子
        else
            currBotColor = 1;
        cin >> x0 >> y0 >> x1 >> y1; // 输入己方走法
        if (x1 >= 0)
            ProcStep(x0, y0, x1, y1, currBotColor); // 模拟己方落子
    }

    // 看看自己本回合输入
    cin >> x0 >> y0 >> x1 >> y1; // 输入对方本回合走法
    if (x1 >= 0)
        ProcStep(x0, y0, x1, y1, -currBotColor); // 模拟对方落子
    else
        currBotColor = 1;

    // 找出合法落子点
    int beginPos[1000][2], possiblePos[1000][2], posCount = 0, choice, dir; // 定义变量

    for (y0 = 0; y0 < 7; y0++) // 遍历棋盘
        for (x0 = 0; x0 < 7; x0++)
        {
            if (gridInfo[x0][y0] != currBotColor) // 当前位置不是己方棋子
                continue;
            for (dir = 0; dir < 24; dir++) // 遍历所有可能的移动方向
            {
                x1 = x0 + delta[dir][0]; // 计算新x坐标
                y1 = y0 + delta[dir][1]; // 计算新y坐标
                if (!inMap(x1, y1)) // 新坐标是否在地图内
                    continue;
                if (gridInfo[x1][y1] != 0) // 新位置非空
                    continue;
                beginPos[posCount][0] = x0; // 记录起始位置
                beginPos[posCount][1] = y0;
                possiblePos[posCount][0] = x1; // 记录目标位置
                possiblePos[posCount][1] = y1;
                posCount++;
            }
        }

    // 做出决策（你只需修改以下部分）
    int startX, startY, resultX, resultY; // 定义决策变量
    if (posCount > 0)
    {
        srand(time(0)); // 初始化随机数种子
        choice = rand() % posCount; // 随机选择一个合法落子点
        startX = beginPos[choice][0]; // 记录起始位置
        startY = beginPos[choice][1];
        resultX = possiblePos[choice][0]; // 记录目标位置
        resultY = possiblePos[choice][1];
    }
    else
    {
        startX = -1; // 无合法落子点
        startY = -1;
        resultX = -1;
        resultY = -1;
    }

    // 决策结束，输出结果（你只需修改以上部分）

    cout << startX << " " << startY << " " << resultX << " " << resultY; // 输出决策结果
    return 0; // 程序结束
}