#include "AlphaBeta_TTHH.h"
#include "QDebug"

AlphaBeta_TTHH::AlphaBeta_TTHH(TranspositionTable* TT)
{
    this->TT=TT;

    EV = new Evaluate(Board,Pieces,PiecesInBoard);
    HH = new HistoryHeuristic();
}

//获取最佳步
void AlphaBeta_TTHH::getBestStep(int* _Board,int* _Pieces,int* _PiecesInBoard,bool computerColor,int DEPTH,QVector<Step*>& Steps){
    int time_begin=clock();
    //棋盘传入
    for(int i=0;i<256;i++){
        this->Board[i]=_Board[i];
        this->PiecesInBoard[i]=_PiecesInBoard[i];
    }
    for(int i=0;i<48;i++){
        this->Pieces[i]=_Pieces[i];
    }
    movedSteps=Steps;
    this->DEPTH=DEPTH;
    evaluate_count=0;//evaluate计数
    node_count=0;//节点计数
    nDistance=0;//实际走的深度（与根节点的距离）
    MoveSide=computerColor;//走棋方颜色
    Zobrist = TT->getZobrist(MoveSide);
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
        switch (this->Board[(*iter)->Src_Position]) {
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
void AlphaBeta_TTHH::fakeMove(Step *step){
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
void AlphaBeta_TTHH::backFakeMove(Step *step){
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
int AlphaBeta_TTHH::NegaMax(int depth, int alpha, int beta){
    //对叶子节点的搜索
    if(depth==1){
        return SearchLeafNode(depth,alpha,beta);
    }
    nDistance++;//距离根节点距离
    node_count++;
    //游戏结束
    if(gameOver()){
        nDistance--;
        return -(80080-nDistance);//距根节点越近的越有价值
    }
    //检测重复局面
    if(checkRepeat()==1){
        return 80060;
    }else if(checkRepeat()==2){
        return 80088;
    }
    int value;//暂存value
    //查找置换表
    if(TT->lookUpTT(Zobrist,DEPTH-nDistance,value,alpha,beta)){
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
    //历史记录表排序
    HH->sortSteps(steps);
    Step* bestStep=nullptr;
    int valueType=2;//0确切值,1beta值，2alpha值
    //遍历走棋
    for(auto iter=steps.begin();iter!=steps.end();iter++){
        fakeMove(*iter);
        value=-NegaMax(depth-1,-beta,-alpha);
        backFakeMove(*iter);
        //beta剪枝
        if(value>=beta){
            valueType=1;//下界值
            bestStep=*iter;//找到好的节点
            alpha=value;
            break;
        }
        if(value>alpha){
            valueType=0;//确切值
            bestStep=*iter;//找到好的节点
            alpha=value;
        }
    }
    //添加置换表
    if(valueType==0){
        TT->storeToTT(Zobrist,DEPTH-nDistance,alpha,TranspositionTable::exact);
    }else if(value==1){
        TT->storeToTT(Zobrist,DEPTH-nDistance,alpha,TranspositionTable::lower_bound);
    }else if(value==2){
        TT->storeToTT(Zobrist,DEPTH-nDistance,alpha,TranspositionTable::upper_bound);
    }
    //添加历史记录表
    HH->addHistoryScore(bestStep,depth);
    qDeleteAll(steps);
    nDistance--;
    return alpha;
}

//叶子节点搜索
int AlphaBeta_TTHH::SearchLeafNode(int depth, int alpha, int beta){
    node_count++;
    nDistance++;
    //是否胜负已分
    if(gameOver()){
        nDistance--;
        return -(80080-nDistance);
    }
    //对方被将，结束
    if(EV->Check(!MoveSide)){
        nDistance--;
        return 80080-nDistance;
    }
    //检测重复局面
    if(checkRepeat()==1){
        return 80060;
    }else if(checkRepeat()==2){
        return 80088;
    }
    int value;//暂存value
    //查找置换表(depth指DEPTH-nDistance)
    if(TT->lookUpTT(Zobrist,DEPTH-nDistance,value,alpha,beta)){
        nDistance--;
        return value;
    }
    if(depth<=0){
        evaluate_count++;
        if(MoveSide){
            value = -EV->evaluateGame();
        }else{
            value = EV->evaluateGame();
        }
        //存确切值
        TT->storeToTT(Zobrist,DEPTH-nDistance,value,TranspositionTable::exact);
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
    //历史记录表排序
    HH->sortSteps(steps);
    Step* bestStep=nullptr;
    int valueType=2;//0确切值,1beta值，2alpha值,3极值
    //遍历走棋
    for(auto iter=steps.begin();iter!=steps.end();iter++){
        fakeMove(*iter);
        //选择延伸(对方如果没有吃将，则会无限延伸)
        if((*iter)->Dst_PiecesType!=0 || EV->Check(MoveSide)){
            value=-SearchLeafNode(depth,-beta,-alpha);
        }else{
            value=-SearchLeafNode(depth-1,-beta,-alpha);
        }
        backFakeMove(*iter);
        //beta剪枝
        if(value>=beta){
            valueType=1;//下界值
            bestStep=*iter;//找到好的节点
            alpha=value;
            break;
        }
        if(value>alpha){
            valueType=0;//确切值
            bestStep=*iter;//找到好的节点
            alpha=value;
            //对alpha作浮动
            qsrand(static_cast<unsigned int>(QTime(0,0,0).secsTo(QTime::currentTime())));
            if(qrand()%2==0){
                alpha+=qrand()%Sensitivity;
            }else{
                alpha-=qrand()%Sensitivity;
            }
        }
    }
    //添加置换表
    if(valueType==0){
        TT->storeToTT(Zobrist,DEPTH-nDistance,alpha,TranspositionTable::exact);
    }else if(value==1){
        TT->storeToTT(Zobrist,DEPTH-nDistance,alpha,TranspositionTable::lower_bound);
    }else if(value==2){
        TT->storeToTT(Zobrist,DEPTH-nDistance,alpha,TranspositionTable::upper_bound);
    }
    //添加历史记录表
    HH->addHistoryScore(bestStep,depth);
    qDeleteAll(steps);
    nDistance--;
    return alpha;
}

//是否游戏结束
bool AlphaBeta_TTHH::gameOver(){
    if(Pieces[31]==0||Pieces[47]==0){
        return true;
    }
    return false;
}

//检查重复局面(0正常，1和棋，2长将输棋)
int AlphaBeta_TTHH::checkRepeat(){
    int size = movedSteps.size();
    if(size<30){
        return 0;
    }
    int checkNum = 0;
    auto iter = movedSteps.end()-2;
    for(int i=size;i!=size-20;i--){
        if((*iter)->Zobrist==Zobrist){
            checkNum++;
        }
        iter--;
    }
    if(checkNum>=5){
        return 1;
    }
    if(checkNum>=2){
        if(EV->Check(MoveSide)){
            return 2;
        }
    }
    return 0;
}
