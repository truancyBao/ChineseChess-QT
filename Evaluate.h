#ifndef EVALUATE_H
#define EVALUATE_H

#include <QObject>
#include "Step.h"
#include "MoveGeneratorB.h"
#include "MoveGeneratorR.h"

const static int _K=5000;//将的子力价值
const static int _R=1050;//車的子力价值
const static int _N=450;//马的子力价值
const static int _C=500;//炮的子力价值
const static int _B=160;//象的子力价值
const static int _A=180;//士的子力价值
const static int _P=100;//兵的子力价值

const static int LARGENUMBER=10000000;//一个很大的数

const static int BaseValue_protect=18;//保子价值
const static int BaseValue_kill=16;//吃子价值
const static int BaseValue_check=200;//将军价值
const static int LinkedHourses=50;//连环马
const static int LinkedPawns=50;//牵手兵

//棋子机动性
const static int Mobility_R=7;
const static int Mobility_N=12;
const static int Mobility_C=1;
const static int Mobility_B=1;
const static int Mobility_A=1;
const static int Mobility_K=0;
const static int Mobility_P=10;

class Evaluate:public QObject
{
    Q_OBJECT
public:
    Evaluate(int*,int*,int*);

    int* Board;//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int* Pieces;//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int* PiecesInBoard;//联系数组

    int evaluateGame();
    int evaluete_K(int);
    int evaluete_R(int);
    int evaluete_N(int);
    int evaluete_C(int);
    int evaluete_B(int);
    int evaluete_A(int);
    int evaluete_P(int);

    bool isSameColor(int,int);
    bool Check(bool);//将军检测
};

#endif // EVALUATE_H
