#ifndef MOVEGENERATORB_H
#define MOVEGENERATORB_H

#include <QVector>
#include "Step.h"

class MoveGeneratorB
{
public:
    MoveGeneratorB(int*,int*,int*);

    int* Board;//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int* Pieces;//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int* PiecesInBoard;//联系数组

    void getAllPossibleSteps(QVector<Step*>&);
    void getSteps_R(int,QVector<Step*>&);//車
    void getSteps_N(int,QVector<Step*>&);//马
    void getSteps_B(int,QVector<Step*>&);//象
    void getSteps_A(int,QVector<Step*>&);//士
    void getSteps_K(int,QVector<Step*>&);//将
    void getSteps_C(int,QVector<Step*>&);//炮
    void getSteps_P(int,QVector<Step*>&);//兵
};

#endif // MOVEGENERATORB_H
