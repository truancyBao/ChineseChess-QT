#ifndef IDALPHABETA_H
#define IDALPHABETA_H

#include <QObject>

#include "Evaluate.h"
#include "HistoryHeuristic.h"
#include "TranspositionTable.h"

class IDAlphaBeta:public QObject
{
    Q_OBJECT
public:
    IDAlphaBeta(TranspositionTable*);

    int Board[256];//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int Pieces[48];//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int PiecesInBoard[256];//联系数组
    Evaluate* EV;//估值
    TranspositionTable* TT;//置换表
    HistoryHeuristic* HH;

    int sensitivity=10;//随机灵敏度

    unsigned long long int Zobrist;//当前zobrist值
    bool MoveSide;//当前走棋方
    Step* bestStep;//存最佳步
    QVector<Step*> movedSteps;

    int evaluate_count;
    int node_count;
    int maxDepth=15;//最大搜索深度
    int nDepth;//当前深度
    int nDistance;//实际深入的层数
    int time_begin;//搜索开始时间
    int timeLimit=20;//时间限制

    void fakeMove(Step*);
    void backFakeMove(Step*);

    int NegaMax(int,int,int);//负极大值搜索
    int SearchLeafNode(int,int,int,bool);

    void showStep(Step*,int,bool);//显示步骤信息
    void showSteps(QVector<Step*>);
    bool isWin();//是否胜利
    int checkRepeat();//重复局面

public slots:
    void getBestStep(int*,int*,int*,bool,int,QVector<Step*>&);//获取最佳分数

signals:
    void endSearch(Step*);
};

#endif // IDALPHABETA_H
