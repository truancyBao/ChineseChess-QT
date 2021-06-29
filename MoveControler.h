#ifndef MOVECONTROLER_H
#define MOVECONTROLER_H

#include <QString>

class MoveControler
{
public:
    MoveControler(int*,int*,int*);

    //每次移动要维护这三张表
    int* Board;//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int* Pieces;//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int* PiecesInBoard;//联系数组

    bool CanMove(int,int);//传入棋子编号和要走的位置
    bool CanMoveK(int,int);//将
    bool CanMoveA(int,int);//士
    bool CanMoveB(int,int);//象
    bool CanMoveN(int,int);//马
    bool CanMoveR(int,int);//车
    bool CanMoveC(int,int);//炮
    bool CanMoveP(int,int);//兵

    bool Check(bool);//将军判断

    bool isSameColor(int,int);

};

#endif // MOVECONTROLER_H
