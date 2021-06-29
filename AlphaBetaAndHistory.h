#ifndef ALPHABETAANDHISTORY_H
#define ALPHABETAANDHISTORY_H

#include <QObject>
#include "Evaluate.h"
#include "HistoryHeuristic.h"

class AlphaBetaAndHistory:public QObject
{
    Q_OBJECT
public:
    AlphaBetaAndHistory();

    int Board[256];//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int Pieces[48];//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int PiecesInBoard[256];//联系数组
    Evaluate* EV;//估值
    QVector<Step*> movedSteps;//走过的步，用来检测重复局面

    HistoryHeuristic* HH;

    bool MoveSide;//当前走棋方

    int evaluate_count;
    int node_count;
    int _depth;//实际深入的层数

    void fakeMove(Step*);
    void backFakeMove(Step*);

    int NegaMax(int,int,int);//负极大值搜索
    int SearchLeafNode(int,int,int,bool);

public slots:
    void getBestStep(int*,int*,int*,bool,int);//获取最佳分数

signals:
    void endSearch(Step*);
};

#endif // ALPHABETAANDHISTORY_H
