#include "MoveGeneratorR.h"
#include "Auxiliary_Move.h"

MoveGeneratorR::MoveGeneratorR(int* Board,int* Pieces,int* PiecesInBoard)
{
    this->Board = Board;
    this->Pieces = Pieces;
    this->PiecesInBoard = PiecesInBoard;
}

//获取所有可能步
void MoveGeneratorR::getAllPossibleSteps(QVector<Step *> &steps){
    //扫描棋盘数组还是棋子数组？
    for(int i=16;i<=31;i++){//扫描所有红色棋
        if(Pieces[i]==0){
            continue;
        }
        switch (i) {
        case 16:
        case 17:getSteps_R(Pieces[i],steps);break;
        case 18:
        case 19:getSteps_N(Pieces[i],steps);break;
        case 20:
        case 21:getSteps_C(Pieces[i],steps);break;
        case 22:
        case 23:getSteps_B(Pieces[i],steps);break;
        case 24:
        case 25:getSteps_A(Pieces[i],steps);break;
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:getSteps_P(Pieces[i],steps);break;
        case 31:getSteps_K(Pieces[i],steps);break;
        }
    }
}

//马的所有可能步
void MoveGeneratorR::getSteps_N(int position,QVector<Step*>& steps){
    for(int i=0;i<8;i++){
        int Dst = position+KnightMoveTab[i];
        int Lag = position+HorseLegTab[i];
        if((InBoard[Dst])!=0 && Board[Lag]==0 && (Board[Dst]&8)==0){//目标位置不是红色
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=Dst;
            step->Dst_PiecesType=Board[Dst];
            step->Dst_PiecesId=PiecesInBoard[Dst];
            steps.append(step);
        }
    }
}

//象的所有可能步
void MoveGeneratorR::getSteps_B(int position, QVector<Step*> &steps){
    for(int i=0;i<4;i++){
        int Dst = position+ElephantMoveTab[i];
        int Eye = position+ElephantEyeTab[i];
        if((InBoard[Dst])!=0 && Board[Eye]==0 && (Board[Dst]&8)==0 && (Dst&128)!=0){//在棋盘内，没有塞象眼，不是红棋，没有过河
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=Dst;
            step->Dst_PiecesType=Board[Dst];
            step->Dst_PiecesId=PiecesInBoard[Dst];
            steps.append(step);
        }
    }
}

//士的可能走法
void MoveGeneratorR::getSteps_A(int position, QVector<Step *> &steps){
    for(int i=0;i<4;i++){
        int Dst = position+AdvisorMoveTab[i];
        if((InFort[Dst])!=0 && (Board[Dst]&8)==0){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=Dst;
            step->Dst_PiecesType=Board[Dst];
            step->Dst_PiecesId=PiecesInBoard[Dst];
            steps.append(step);
        }
    }
}

//将的可能走法
void MoveGeneratorR::getSteps_K(int position, QVector<Step *> &steps){
    for(int i=0;i<4;i++){
        int Dst = position+KingMoveTab[i];
        if((InFort[Dst])!=0 && (Board[Dst]&8)==0){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=Dst;
            step->Dst_PiecesType=Board[Dst];
            step->Dst_PiecesId=PiecesInBoard[Dst];
            steps.append(step);
        }
    }
    //照将
    if(Pieces[31]%16==Pieces[47]%16){
        bool flag=false;
        for(int i=Pieces[47]+16;i<Pieces[31];i+=16){
            if(PiecesInBoard[i]!=0){
                flag=true;
                break;
            }
        }
        if(!flag){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=Pieces[47];
            step->Dst_PiecesType=Board[Pieces[47]];
            step->Dst_PiecesId=PiecesInBoard[Pieces[47]];
            steps.append(step);
        }
    }
}

//红兵的可能走法
void MoveGeneratorR::getSteps_P(int position, QVector<Step*> &steps){
    //前进
    int Dst = position-16;
    if((InBoard[Dst])!=0 && (Board[Dst]&8)==0){
        Step* step = new Step();
        step->Src_Position=position;
        step->Src_PiecesType=Board[position];
        step->Src_PiecesId=PiecesInBoard[position];
        step->Dst_Position=Dst;
        step->Dst_PiecesType=Board[Dst];
        step->Dst_PiecesId=PiecesInBoard[Dst];
        steps.append(step);
    }
    //过河后可左右移动
    int MoveTab[2]={-1,+1};
    for(int i=0;i<2;i++){
        Dst = position+MoveTab[i];
        if(InBoard[Dst]!=0 && (Board[Dst]&8)==0 && (Dst&0x80)==0){//目标位置在棋盘内，不是红棋，过河
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=Dst;
            step->Dst_PiecesType=Board[Dst];
            step->Dst_PiecesId=PiecesInBoard[Dst];
            steps.append(step);
        }
    }
}

//車的可能走法
void MoveGeneratorR::getSteps_R(int position, QVector<Step *> &steps){
    //向上
    for(int i=position-16;InBoard[i]!=0;i-=16){
        if(PiecesInBoard[i]==0){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if((PiecesInBoard[i]&32)!=0){//黑棋
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }else if((PiecesInBoard[i]&16)!=0){//红棋
            break;
        }
    }
    //向下
    for(int i=position+16;InBoard[i]!=0;i+=16){
        if(PiecesInBoard[i]==0){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if((PiecesInBoard[i]&32)!=0){//黑棋
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }else if((PiecesInBoard[i]&16)!=0){//红棋
            break;
        }
    }
    //向左
    for(int i=position-1;InBoard[i]!=0;i--){
        if(PiecesInBoard[i]==0){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if((PiecesInBoard[i]&32)!=0){//黑棋
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }else if((PiecesInBoard[i]&16)!=0){//红棋
            break;
        }
    }
    //向右
    for(int i=position+1;InBoard[i]!=0;i++){
        if(PiecesInBoard[i]==0){
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if((PiecesInBoard[i]&32)!=0){//黑棋
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }else if((PiecesInBoard[i]&16)!=0){//红棋
            break;
        }
    }
}

//炮的可能走法
void MoveGeneratorR::getSteps_C(int position, QVector<Step *> &steps){
    bool flag=false;//中间是否隔子
    //向上
    for(int i=position-16;i>=51;i-=16){
        if(PiecesInBoard[i]==0&&!flag){//不隔子的空格
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if(PiecesInBoard[i]!=0&&!flag){//不隔子遇见棋子
            flag=true;
        }else if(PiecesInBoard[i]==0&&flag){//隔子遇见棋子

        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&16)!=0){//隔子遇见红色棋子
            break;
        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&32)!=0){//隔子遇见黑色棋子
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }
    }
    //向下
    flag=false;
    for(int i=position+16;i<=203;i+=16){
        if(PiecesInBoard[i]==0&&!flag){//不隔子的空格
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if(PiecesInBoard[i]!=0&&!flag){//不隔子遇见棋子
            flag=true;
        }else if(PiecesInBoard[i]==0&&flag){//隔子遇见空格

        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&16)!=0){//隔子遇见红色棋子
            break;
        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&32)!=0){//隔子遇见黑色棋子
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }
    }
    //向左
    flag=false;
    for(int i=position-1;InBoard[i]!=0;i--){
        if(PiecesInBoard[i]==0&&!flag){//不隔子的空格
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if(PiecesInBoard[i]!=0&&!flag){//不隔子遇见棋子
            flag=true;
        }else if(PiecesInBoard[i]==0&&flag){//隔子遇见棋子

        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&16)!=0){//隔子遇见红色棋子
            break;
        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&32)!=0){//隔子遇见黑色棋子
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }
    }
    //向右
    flag=false;
    for(int i=position+1;InBoard[i]!=0;i++){
        if(PiecesInBoard[i]==0&&!flag){//不隔子的空格
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
        }else if(PiecesInBoard[i]!=0&&!flag){//不隔子遇见棋子
            flag=true;
        }else if(PiecesInBoard[i]==0&&flag){//隔子遇见棋子

        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&16)!=0){//隔子遇见红色棋子
            break;
        }else if(PiecesInBoard[i]!=0&&flag&&(PiecesInBoard[i]&32)!=0){//隔子遇见黑色棋子
            Step* step = new Step();
            step->Src_Position=position;
            step->Src_PiecesType=Board[position];
            step->Src_PiecesId=PiecesInBoard[position];
            step->Dst_Position=i;
            step->Dst_PiecesType=Board[i];
            step->Dst_PiecesId=PiecesInBoard[i];
            steps.append(step);
            break;
        }
    }
}
