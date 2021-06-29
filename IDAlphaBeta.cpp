#include "IDAlphaBeta.h"
#include <QtDebug>

IDAlphaBeta::IDAlphaBeta(TranspositionTable* TT)
{
    this->TT = TT;
    HH = new HistoryHeuristic();
    EV = new Evaluate(Board,Pieces,PiecesInBoard);
    bestStep=new Step();
}

//获取最佳步  为什么有时红棋不走
void IDAlphaBeta::getBestStep(int* _Board,int* _Pieces,int* _PiecesInBoard,bool computerColor,int timeLimit,QVector<Step*>&Steps){
    time_begin=clock();
    //棋盘赋值
    for(int i=0;i<256;i++){
        this->Board[i]=_Board[i];
        this->PiecesInBoard[i]=_PiecesInBoard[i];
    }
    for(int i=0;i<48;i++){
        this->Pieces[i]=_Pieces[i];
    }
    movedSteps=Steps;
    this->timeLimit=timeLimit;//限制时间
    //估值函数
    evaluate_count=0;//evaluate计数
    node_count=0;//节点计数
    nDistance=0;//实际走的深度（与根节点的距离）
    MoveSide=computerColor;//走棋方颜色
    Zobrist=TT->getZobrist(computerColor);//获取当前Zobrist
    Step* backupBestMove = new Step();
    //迭代搜索
    for(nDepth=1;nDepth<=maxDepth;nDepth++){
        //如果没有超时,保存最佳着法
        if(NegaMax(nDepth,-LARGENUMBER,LARGENUMBER)==66666){
            break;
        }
        *backupBestMove=*bestStep;
    }
    HH->initHistoryTable();//清空历史记录表
    int time_end=clock();
    qDebug()<<"共遍历"<<node_count<<"个节点,调用"<<evaluate_count<<"次估值函数,"<<"耗时"<<(time_end-time_begin)/1000<<"秒";
    emit endSearch(backupBestMove);
}

//显示步骤
void IDAlphaBeta::showStep(Step* step, int value, bool flag){
    int x_src=(step)->Src_Position%16-3;
    int y_src=(step)->Src_Position/16-3;
    int x_dst=(step)->Dst_Position%16-3;
    int y_dst=(step)->Dst_Position/16-3;
    QString chess;
    switch (this->Board[(step)->Src_Position]) {
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
    if(!flag){
        qDebug()<<this->PiecesInBoard[(step)->Src_Position]<<chess<<"("<<x_src<<","<<y_src<<")"
                                                     <<"---->"<<"("<<x_dst<<","<<y_dst<<")"<<"得分"<<value;
    }else{
        qDebug()<<"最佳步骤："<<this->PiecesInBoard[(step)->Src_Position]<<chess<<"("<<x_src<<","<<y_src<<")"
                                                     <<"---->"<<"("<<x_dst<<","<<y_dst<<")"<<"得分"<<value;
    }

}

void IDAlphaBeta::showSteps(QVector<Step*> steps){
    for(auto iter=steps.begin();iter!=steps.end();iter++){
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
        qDebug()<<this->PiecesInBoard[(*iter)->Src_Position]<<chess<<"("<<x_src<<","<<y_src<<")"
                                                     <<"---->"<<"("<<x_dst<<","<<y_dst<<")";
    }
}

//移动棋子
void IDAlphaBeta::fakeMove(Step *step){
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
void IDAlphaBeta::backFakeMove(Step *step){
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
int IDAlphaBeta::NegaMax(int depth, int alpha, int beta){
    //是否胜利
    if(isWin()){
        return -(80080-nDistance);
    }
    //检测重复局面
    int checkFlag=checkRepeat();
    if(checkFlag==1){
        return 80060;//尽量不下和棋
    }else if(checkFlag==2){
        return 80088;//坚决避免长将长捉
    }
    //检测是否超时
    int time_now=clock();
    if((time_now-time_begin)/1000>=timeLimit){
        if(nDistance%2==0){
            return 66666;
        }else{
            return -66666;
        }
    }
    int value;//暂存价值
    //查置换表
    if(nDepth!=depth){//第一层没有最佳走法产生，所以不能用置换表
        if(TT->lookUpTT(Zobrist,depth,value,alpha,beta)){
            return value;
        }
    }
    node_count++;//节点数增加
    nDistance++;//距根节点距离
    //叶子节点估值
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
    //历史记录表排序
    HH->sortSteps(steps);
    if(depth==nDepth){
        qDebug()<<nDepth<<"层搜索--------------------------------------";
    }
    int valueType=2;//0确切值，1beta，2alpha,3不存
    Step* _bestStep=nullptr;//暂存最佳步
    //遍历走棋
    for(auto iter=steps.begin();iter!=steps.end();iter++){
        fakeMove(*iter);
        value=-NegaMax(depth-1,-beta,-alpha);
        backFakeMove(*iter);
        if(depth==nDepth){
            //showStep(*iter,value,false);
        }
        if(value>alpha){
            valueType=0;//没有经过剪枝
            alpha=value;
            _bestStep=*iter;//更换最佳步
            if(depth==nDepth){//最上层才保存
                *bestStep=**iter;
            }
            if(alpha>=beta){//经过剪枝
                valueType=1;
                break;
            }
        }
    }
    if(depth==nDepth){
        showStep(bestStep,alpha,true);
    }
    //存置换表
    if(valueType==0){//确切值
        TT->storeToTT(Zobrist,depth,alpha,TranspositionTable::exact);
    }else if(valueType==1){//下界值
        TT->storeToTT(Zobrist,depth,alpha,TranspositionTable::lower_bound);
    }else if(valueType==2){//上界值
        TT->storeToTT(Zobrist,depth,alpha,TranspositionTable::upper_bound);
    }
    //存历史表
    HH->addHistoryScore(_bestStep,depth);
    qDeleteAll(steps);//一定记得释放,否则会造成严重的内存泄漏
    nDistance--;
    return alpha;
}

bool IDAlphaBeta::isWin(){
    if(Pieces[31]==0||Pieces[47]==0){
        return true;
    }
    return false;
}

//检查重复局面(0正常，1和棋，2长将输棋)
int IDAlphaBeta::checkRepeat(){
    int size = movedSteps.size();
    int returnType=0;//返回值类型,0,正常,1和棋，2长捉
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
    //重复局面大于五次,和棋
    if(checkNum>=4){
        return 1;
    }
    //重复局面大于三（二）次（是否长捉长将）(电脑搜索可以更少，防止无用着)
    if(checkNum>=1){
        //长将
        if(EV->Check(MoveSide)){
            return 2;
        }
        Step* step = movedSteps.back();
        //长捉
        QVector<Step*> steps;
        MoveGeneratorB MGB = MoveGeneratorB(Board,Pieces,PiecesInBoard);
        MoveGeneratorR MGR = MoveGeneratorR(Board,Pieces,PiecesInBoard);
        switch (step->Src_PiecesType){
        case 8:MGR.getSteps_R(step->Dst_Position,steps);break;
        case 9:MGR.getSteps_N(step->Dst_Position,steps);break;
        case 10:MGR.getSteps_C(step->Dst_Position,steps);break;
        case 16:MGB.getSteps_R(step->Dst_Position,steps);break;
        case 17:MGB.getSteps_N(step->Dst_Position,steps);break;
        case 18:MGB.getSteps_C(step->Dst_Position,steps);break;
        }
        //黑車马炮兵16 17 18 21
        //红車马炮兵8 9 10 13
        //車捉马炮兵
        if(step->Src_PiecesType==8){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 17:returnType=2;break;
                case 18:returnType=2;break;
                case 21:returnType=2;break;
                }
            }
        }
        if(step->Src_PiecesType==16){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 9:returnType=2;break;
                case 10:returnType=2;break;
                case 13:returnType=2;break;
                }
            }
        }
        //马捉车炮兵
        if(step->Src_PiecesType==9){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 16:returnType=2;break;
                case 18:returnType=2;break;
                case 21:returnType=2;break;
                }
            }
        }
        if(step->Src_PiecesType==17){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 8:returnType=2;break;
                case 10:returnType=2;break;
                case 13:returnType=2;break;
                }
            }
        }
        //炮捉车马兵
        if(step->Src_PiecesType==10){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 16:returnType=2;break;
                case 17:returnType=2;break;
                case 21:returnType=2;break;
                }
            }
        }
        if(step->Src_PiecesType==18){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 8:returnType=2;break;
                case 9:returnType=2;break;
                case 13:returnType=2;break;
                }
            }
        }
        if(!steps.isEmpty()){
            qDeleteAll(steps);
            steps.clear();
        }
    }
    return returnType;
}
