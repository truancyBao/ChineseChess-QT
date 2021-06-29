#ifndef ALPHABETA_H
#define ALPHABETA_H

#include <QObject>
#include "Step.h"
#include "Evaluate.h"

class AlphaBeta:public QObject
{
    Q_OBJECT
public:
    AlphaBeta();

    int Board[256];//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int Pieces[48];//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int PiecesInBoard[256];//联系数组
    Evaluate* EV;//估值

    bool MoveSide;//当前走棋方

    int evaluate_count;
    int node_count;
    int nDistance;//实际深入的层数

    void fakeMove(Step*);
    void backFakeMove(Step*);

    int NegaMax(int,int,int);//负极大值搜索
    int SearchLeafNode(int,int,int,bool);

    void showStep(Step*,int,bool);
    bool isWin();

public slots:
    void getBestStep(int*,int*,int*,bool,int);//获取最佳分数

signals:
    void endSearch(Step*);
};

#endif // ALPHABETA_H
