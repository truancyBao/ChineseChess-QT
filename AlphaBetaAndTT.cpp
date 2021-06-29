#include "AlphaBetaAndTT.h"
#include <QDebug>

AlphaBetaAndTT::AlphaBetaAndTT(TranspositionTable* TT)
{
    this->TT=TT;
    EV = new Evaluate(Board,Pieces,PiecesInBoard);
}

AlphaBetaAndTT::~AlphaBetaAndTT(){
    delete EV;
}

//获取最佳步
void AlphaBetaAndTT::getBestStep(int* _Board,int* _Pieces,int* _PiecesInBoard,bool computerColor,int DEPTH,QVector<Step*>&Steps){
    int time_begin=clock();
    //棋盘传入
    for(int i=0;i<256;i++){
        this->Board[i]=_Board[i];
        this->PiecesInBoard[i]=_PiecesInBoard[i];
    }
    for(int i=0;i<48;i++){
        this->Pieces[i]=_Pieces[i];
    }
    //传入走过的步骤
    movedSteps=Steps;
    evaluate_count=0;//evaluate计数
    node_count=0;//节点计数
    nDistance=0;//距离根节点距离
    MoveSide=computerColor;//走棋方颜色
    Zobrist=TT->getZobrist(computerColor);//获取当前走棋方的ZobristHash初始值
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
        value=-NegaMax(DEPTH-1,-LARGENUMBER,LARGENUMBER);
        backFakeMove(*iter);
        if(value>maxValue){
            maxValue=value;
            *bestStep=*(*iter);
        }
        qDebug()<<PiecesInBoard[(*iter)->Src_Position]<<chess<<"("<<x_src<<","<<y_src<<")"
                                                     <<"---->"<<"("<<x_dst<<","<<y_dst<<")"<<"得分"<<value;
    }
    qDeleteAll(possibleSteps);
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
void AlphaBetaAndTT::fakeMove(Step *step){
    Board[step->Src_Position]=0;
    PiecesInBoard[step->Src_Position]=0;
    Pieces[step->Src_PiecesId]=step->Dst_Position;
    Board[step->Dst_Position]=step->Src_PiecesType;
    PiecesInBoard[step->Dst_Position]=step->Src_PiecesId;
    Pieces[step->Dst_PiecesId]=0;
    MoveSide=!MoveSide;
    TT->getFakeMoveZobrist(step,Zobrist);
    step->Zobrist=Zobrist;
    movedSteps.append(step);
}

//撤回移动
void AlphaBetaAndTT::backFakeMove(Step *step){
    Board[step->Src_Position]=step->Src_PiecesType;
    PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
    Pieces[step->Src_PiecesId]=step->Src_Position;
    Board[step->Dst_Position]=step->Dst_PiecesType;
    PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
    if(step->Dst_PiecesId!=0){//目标位置以前有棋子
        Pieces[step->Dst_PiecesId]=step->Dst_Position;
    }
    MoveSide=!MoveSide;
    TT->getbackFakeMoveZobrist(step,Zobrist);
    movedSteps.removeLast();
}

//负极大值
int AlphaBetaAndTT::NegaMax(int depth, int alpha, int beta){
    node_count++;
    nDistance++;
    //是否结束游戏
    if(isWin()){
        nDistance--;
        return -(80080-nDistance);
    }
    //检测重复局面(考虑应该返回的分数)
    if(CheckRepeat()==1){
        nDistance--;
        return 80060;
    }else if(CheckRepeat()==2){
        nDistance--;
        return 80088;
    }
    int value;
    //查找置换表获取的值
    if(TT->lookUpTT(Zobrist,depth,value,alpha,beta)){
        nDistance--;
        return value;
    }
    //叶子节点hash
    if(depth<=0){
        evaluate_count++;
        if(MoveSide){
            value = -EV->evaluateGame();
        }else{
            value = EV->evaluateGame();
        }
        //存确切值
        TT->storeToTT(Zobrist,depth,value,TranspositionTable::exact);
        nDistance--;
        return value;
    }
    QVector<Step*> steps;
    //根据走棋方获取不同棋路
    if(MoveSide){
        MoveGeneratorR MVR = MoveGeneratorR(Board,Pieces,PiecesInBoard);
        MVR.getAllPossibleSteps(steps);
    }else{
        MoveGeneratorB MVB = MoveGeneratorB(Board,Pieces,PiecesInBoard);
        MVB.getAllPossibleSteps(steps);
    }
    int value_type=2;//0确切值，1下界值，2上界值
    //遍历走棋
    for(auto iter=steps.begin();iter!=steps.end();iter++){
        fakeMove(*iter);
        value=-NegaMax(depth-1,-beta,-alpha);
        backFakeMove(*iter);
        if(value>=beta){//beta剪枝
            value_type=1;//下界值
            alpha=value;
            break;
        }
        if(value>alpha){
            value_type=0;//没有经过剪枝为确切值
            alpha=value;
        }
    }
    //存入置换表
    if(value_type==0){//确切值
        TT->storeToTT(Zobrist,depth,alpha,TranspositionTable::exact);
    }else if(value_type==1){//下界值
        TT->storeToTT(Zobrist,depth,alpha,TranspositionTable::lower_bound);
    }else if(value_type==2){//上界值
        TT->storeToTT(Zobrist,depth,alpha,TranspositionTable::upper_bound);
    }
    qDeleteAll(steps);
    nDistance--;
    return alpha;
}

//是否胜利
bool AlphaBetaAndTT::isWin(){
    if(Pieces[31]==0||Pieces[47]==0){
        return true;
    }
    return false;
}

//检测重复局面
int AlphaBetaAndTT::CheckRepeat(){
    int size = movedSteps.size();
    if(size<20){
        return 0;
    }
    int checkNum = 0;
    auto iter = movedSteps.end()-2;
    for(int i=size;i!=20;i--){
        if((*iter)->Zobrist==Zobrist){
            checkNum++;
        }
        iter--;
    }
    if(checkNum>=5){
        qDebug()<<"检测到和棋--------------------------------------------------";
        return 1;
    }
    if(checkNum>=3){
        if(EV->Check(MoveSide)){
            qDebug()<<"检测到长将----------------------------------------------";
            return 2;
        }
    }
    return 0;
}
