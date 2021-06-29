#ifndef ALPHABETA_TTHH_H
#define ALPHABETA_TTHH_H

#include <QObject>
#include "Evaluate.h"
#include "TranspositionTable.h"
#include "HistoryHeuristic.h"

class AlphaBeta_TTHH:public QObject
{
    Q_OBJECT
public:
    AlphaBeta_TTHH(TranspositionTable*);

    int Board[256];//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int Pieces[48];//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int PiecesInBoard[256];//联系数组
    Evaluate* EV;//估值
    TranspositionTable* TT;//置换表
    HistoryHeuristic* HH;//历史记录表
    QVector<Step*> movedSteps;//存储所有走过的步，用来检测重复局面

    int Sensitivity=20;//随机灵敏度

    bool MoveSide;//当前走棋方
    unsigned long long int Zobrist;

    int evaluate_count;
    int node_count;
    int DEPTH;//传入的搜索层数
    int nDistance;//实际深入的层数

    void fakeMove(Step*);
    void backFakeMove(Step*);

    int NegaMax(int,int,int);//负极大值搜索
    int SearchLeafNode(int,int,int);

    bool gameOver();//游戏是否结束
    int checkRepeat();//检查重复局面

public slots:
    void getBestStep(int*,int*,int*,bool,int,QVector<Step*>&);//获取最佳分数

signals:
    void endSearch(Step*);
};

#endif // ALPHABETA_TTHH_H
