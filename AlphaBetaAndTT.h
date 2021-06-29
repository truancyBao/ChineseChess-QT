#ifndef ALPHABETAANDTT_H
#define ALPHABETAANDTT_H

#include <QObject>
#include "TranspositionTable.h"
#include "Evaluate.h"

class AlphaBetaAndTT:public QObject
{
    Q_OBJECT
public:
    AlphaBetaAndTT(TranspositionTable*);
    ~AlphaBetaAndTT();

    int Board[256];//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int Pieces[48];//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int PiecesInBoard[256];//联系数组
    TranspositionTable* TT;//置换表
    Evaluate* EV;//估值
    QVector<Step*> movedSteps;

    bool MoveSide;//当前走棋方
    unsigned long long int Zobrist;//初始值Zobrist

    int evaluate_count;
    int node_count;
    int nDistance;//实际深入的层数

    void fakeMove(Step*);
    void backFakeMove(Step*);

    int NegaMax(int,int,int);//负极大值搜索
    int SearchLeafNode(int,int,int,bool);

    bool isWin();//是否胜利
    int CheckRepeat();//检测重复局面

public slots:
    void getBestStep(int*,int*,int*,bool,int,QVector<Step*>&);//获取最佳分数

signals:
    void endSearch(Step*);
};

#endif // ALPHABETAANDTT_H
