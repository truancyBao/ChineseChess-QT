#include "AlphaBetaAndHistory.h"
#include <QDebug>

AlphaBetaAndHistory::AlphaBetaAndHistory()
{
    EV = new Evaluate(Board,Pieces,PiecesInBoard);
    HH = new HistoryHeuristic();
}

//获取最佳步
void AlphaBetaAndHistory::getBestStep(int* _Board,int* _Pieces,int* _PiecesInBoard,bool computerColor,int DEPTH){
    int time_begin=clock();
    //棋盘传入
    for(int i=0;i<256;i++){
        this->Board[i]=_Board[i];
        this->PiecesInBoard[i]=_PiecesInBoard[i];
    }
    for(int i=0;i<48;i++){
        this->Pieces[i]=_Pieces[i];
    }
    evaluate_count=0;//evaluate计数
    node_count=0;//节点计数
    _depth=0;//实际走的深度（与根节点的距离）
    MoveSide=computerColor;//走棋方颜色
    int maxValue=-LARGENUMBER;
    Step* bestStep=new Step();
    int value;//暂存
    QVector<Step*> possibleSteps;
    if(computerColor){
        MoveGeneratorR MVR = MoveGeneratorR(Board,Pieces,PiecesInBoard);
        MVR.getAllPossibleSteps(possibleSteps);
    }else{
        MoveGeneratorB MVB = MoveGeneratorB(Board,Pieces,PiecesInBoard);
        MVB.getAllPossibleSteps(possibleSteps);
    }
    for(auto iter=possibleSteps.begin();iter!=possibleSteps.end();iter++){
        node_count++;

        int x_src=(*iter)->Src_Position%16-3;
        int y_src=(*iter)->Src_Position/16-3;
        int x_dst=(*iter)->Dst_Position%16-3;
        int y_dst=(*iter)->Dst_Position/16-3;
        QString chess;
        switch (Board[(*iter)->Src_Position]) {
        case 16:chess="黑車";break;
        case 17:chess="黑马";break;
        case 18:chess="黑炮";break;
        case 19:chess="黑象";break;
        case 20:chess="黑士";break;
        case 21:chess="黑兵";break;
        case 22:chess="黑将";break;
        case 8:chess="红車";break;
        case 9:chess="红马";break;
        case 10:chess="红炮";break;
        case 11:chess="红象";break;
        case 12:chess="红士";break;
        case 13:chess="红兵";break;
        case 14:chess="红将";break;
        }
        fakeMove(*iter);
        _depth++;
        if(Pieces[31]==0||Pieces[47]==0){
            maxValue=80080-_depth;
            *bestStep=*(*iter);
            qDebug()<<this->PiecesInBoard[(*iter)->Src_Position]<<chess<<"("<<x_src<<","<<y_src<<")"
                                                         <<"---->"<<"("<<x_dst<<","<<y_dst<<")"<<"得分"<<maxValue;
            backFakeMove(*iter);
            _depth--;
            break;
        }
        value=-NegaMax(DEPTH-1,-LARGENUMBER,LARGENUMBER);
        backFakeMove(*iter);
        _depth--;
        if(value>maxValue){
            maxValue=value;
            *bestStep=*(*iter);
        }
        qDebug()<<PiecesInBoard[(*iter)->Src_Position]<<chess<<"("<<x_src<<","<<y_src<<")"
                                                     <<"---->"<<"("<<x_dst<<","<<y_dst<<")"<<"得分"<<value;
    }
    qDeleteAll(possibleSteps);
    //清空历史表
    HH->initHistoryTable();
    int x_src=bestStep->Src_Position%16-3;
    int y_src=bestStep->Src_Position/16-3;
    int x_dst=bestStep->Dst_Position%16-3;
    int y_dst=bestStep->Dst_Position/16-3;
    QString chess;
    switch (Board[bestStep->Src_Position]) {
    case 16:chess="黑車";break;
    case 17:chess="黑马";break;
    case 18:chess="黑炮";break;
    case 19:chess="黑象";break;
    case 20:chess="黑士";break;
    case 21:chess="黑兵";break;
    case 22:chess="黑将";break;
    case 8:chess="红車";break;
    case 9:chess="红马";break;
    case 10:chess="红炮";break;
    case 11:chess="红象";break;
    case 12:chess="红士";break;
    case 13:chess="红兵";break;
    case 14:chess="红将";break;
    }
    qDebug()<<"最佳步骤："<<PiecesInBoard[bestStep->Src_Position]<<chess
           <<"("<<x_src<<","<<y_src<<")"<<"---->"<<"("<<x_dst<<","<<y_dst<<")"<<"得分"<<maxValue;
    int time_end=clock();
    qDebug()<<"共遍历"<<node_count<<"个节点,调用"<<evaluate_count<<"次估值函数,"<<"耗时"<<(time_end-time_begin)/1000<<"秒";
    emit endSearch(bestStep);
}

//移动棋子
void AlphaBetaAndHistory::fakeMove(Step *step){
    Board[step->Src_Position]=0;
    PiecesInBoard[step->Src_Position]=0;
    Pieces[step->Src_PiecesId]=step->Dst_Position;
    Board[step->Dst_Position]=step->Src_PiecesType;
    PiecesInBoard[step->Dst_Position]=step->Src_PiecesId;
    Pieces[step->Dst_PiecesId]=0;
    MoveSide=!MoveSide;
}

//撤回移动
void AlphaBetaAndHistory::backFakeMove(Step *step){
    Board[step->Src_Position]=step->Src_PiecesType;
    PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
    Pieces[step->Src_PiecesId]=step->Src_Position;
    Board[step->Dst_Position]=step->Dst_PiecesType;
    PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
    if(step->Dst_PiecesId!=0){//目标位置以前有棋子
        Pieces[step->Dst_PiecesId]=step->Dst_Position;
    }
    MoveSide=!MoveSide;
}

//负极大值
int AlphaBetaAndHistory::NegaMax(int depth, int alpha, int beta){
    node_count++;
    if(depth<=0){
        evaluate_count++;
        if(MoveSide){
            return -EV->evaluateGame();
        }else{
            return EV->evaluateGame();
        }
    }
    //对叶子节点的搜索
//    if(depth==1){
//        return SearchLeafNode(depth,alpha,beta,computerColor);
//    }
    int value;
    QVector<Step*> steps;
    Step* bestStep=nullptr;//最佳步骤
    //根据走棋方获取不同棋路
    if(MoveSide){
        MoveGeneratorR MVR = MoveGeneratorR(Board,Pieces,PiecesInBoard);
        MVR.getAllPossibleSteps(steps);
    }else{
        MoveGeneratorB MVB = MoveGeneratorB(Board,Pieces,PiecesInBoard);
        MVB.getAllPossibleSteps(steps);
    }
    //历史启发排序
    HH->sortSteps(steps);
    //遍历走棋
    for(auto iter=steps.begin();iter!=steps.end();iter++){
        fakeMove(*iter);
        _depth++;
        if(Pieces[31]==0||Pieces[47]==0){
            alpha=80080-_depth;
            backFakeMove(*iter);
            _depth--;
            break;
        }
        value=-NegaMax(depth-1,-beta,-alpha);
        backFakeMove(*iter);
        _depth--;
        if(value>alpha){
            alpha=value;
            bestStep=*iter;//更新最佳步
            if(alpha>=beta){
                break;
            }
        }
    }
    //添加历史记录表
    HH->addHistoryScore(bestStep,depth);
    qDeleteAll(steps);
    return alpha;
}
