#include "MoveControler.h"
#include "Auxiliary_Move.h"

MoveControler::MoveControler(int* Board,int* Pieces,int* PiecesInBoard)
{
    this->Board = Board;
    this->Pieces = Pieces;
    this->PiecesInBoard = PiecesInBoard;
}

bool MoveControler::CanMove(int piecesId, int DstBoard){
    //如果两者同色，不能走
    if(((piecesId&16)!=0&&(PiecesInBoard[DstBoard]&16)!=0) || ((piecesId&32)!=0&&(PiecesInBoard[DstBoard]&32)!=0)){
        return false;
    }
    switch (Board[Pieces[piecesId]]) {
        case 8: return CanMoveR(piecesId,DstBoard);
        case 9: return CanMoveN(piecesId,DstBoard);
        case 10: return CanMoveC(piecesId,DstBoard);
        case 11: return CanMoveB(piecesId,DstBoard);
        case 12: return CanMoveA(piecesId,DstBoard);
        case 13: return CanMoveP(piecesId,DstBoard);
        case 14: return CanMoveK(piecesId,DstBoard);
        case 16: return CanMoveR(piecesId,DstBoard);
        case 17: return CanMoveN(piecesId,DstBoard);
        case 18: return CanMoveC(piecesId,DstBoard);
        case 19: return CanMoveB(piecesId,DstBoard);
        case 20: return CanMoveA(piecesId,DstBoard);
        case 21: return CanMoveP(piecesId,DstBoard);
        case 22: return CanMoveK(piecesId,DstBoard);
    }
    return false;
}

//車
bool MoveControler::CanMoveR(int piecesId, int DstBoard){
    //判断是否是直线
    if((Pieces[piecesId]-DstBoard)%16!=0 && Pieces[piecesId]/16!=DstBoard/16){
        return false;
    }
    //上下
    if((Pieces[piecesId]-DstBoard)%16==0){
        int min = Pieces[piecesId]<DstBoard?Pieces[piecesId]:DstBoard;
        int max = Pieces[piecesId]>DstBoard?Pieces[piecesId]:DstBoard;
        for(int i=min+16;i<max;i+=16){
            if(PiecesInBoard[i]!=0){
                return false;
            }
        }
    }
    //左右
    if(Pieces[piecesId]/16==DstBoard/16){
        int min = Pieces[piecesId]<DstBoard?Pieces[piecesId]:DstBoard;
        int max = Pieces[piecesId]>DstBoard?Pieces[piecesId]:DstBoard;
        for(int i=min+1;i<max;i++){
            if(PiecesInBoard[i]!=0){
                return false;
            }
        }
    }
    return true;
}

//炮
bool MoveControler::CanMoveC(int piecesId, int DstBoard){
    //判断是否是直线
    if((Pieces[piecesId]-DstBoard)%16!=0 && Pieces[piecesId]/16!=DstBoard/16){
        return false;
    }
    //不吃子
    if(PiecesInBoard[DstBoard]==0){
        //上下
        if((Pieces[piecesId]-DstBoard)%16==0){
            int min = Pieces[piecesId]<DstBoard?Pieces[piecesId]:DstBoard;
            int max = Pieces[piecesId]>DstBoard?Pieces[piecesId]:DstBoard;
            for(int i=min+16;i<max;i+=16){
                if(PiecesInBoard[i]!=0){
                    return false;
                }
            }
        }
        //左右
        if(Pieces[piecesId]/16==DstBoard/16){
            int min = Pieces[piecesId]<DstBoard?Pieces[piecesId]:DstBoard;
            int max = Pieces[piecesId]>DstBoard?Pieces[piecesId]:DstBoard;
            for(int i=min+1;i<max;i++){
                if(PiecesInBoard[i]!=0){
                    return false;
                }
            }
        }
        return true;
    }else{//吃子
        int pieces=0;
        //上下
        if((Pieces[piecesId]-DstBoard)%16==0){
            int min = Pieces[piecesId]<DstBoard?Pieces[piecesId]:DstBoard;
            int max = Pieces[piecesId]>DstBoard?Pieces[piecesId]:DstBoard;
            for(int i=min+16;i<max;i+=16){
                if(PiecesInBoard[i]!=0){
                    pieces++;
                }
            }
        }else if(Pieces[piecesId]/16==DstBoard/16){//左右
            int min = Pieces[piecesId]<DstBoard?Pieces[piecesId]:DstBoard;
            int max = Pieces[piecesId]>DstBoard?Pieces[piecesId]:DstBoard;
            for(int i=min+1;i<max;i++){
                if(PiecesInBoard[i]!=0){
                    pieces++;
                }
            }
        }
        if(pieces==1){
            return true;
        }
    }
    return false;
}

//马
bool MoveControler::CanMoveN(int piecesId, int DstBoard){
    //不需要判断是否在棋盘内
    for(int i=0;i<8;i++){//判断是否是马步及是否撇马脚
        if((Pieces[piecesId]+KnightMoveTab[i])==DstBoard&&Board[Pieces[piecesId]+HorseLegTab[i]]==0){
            return true;
        }
    }
    return false;
}

//象
bool MoveControler::CanMoveB(int piecesId, int DstBoard){
    //判断是否过河(分红黑棋)
    if((piecesId&32)!=0 && ((DstBoard&0x80)!=0)){
        return false;
    }else if((piecesId&16)!=0 && (DstBoard&0x80)==0){
        return false;
    }

    for(int i=0;i<4;i++){
        if((Pieces[piecesId]+ElephantMoveTab[i])==DstBoard && Board[Pieces[piecesId]+ElephantEyeTab[i]]==0){
            return true;
        }
    }
    return false;
}

//士
bool MoveControler::CanMoveA(int piecesId, int DstBoard){
    //判断是否在九宫内
    if(InFort[DstBoard]==0){
        return false;
    }

    for(int i=0;i<4;i++){
        if((Pieces[piecesId]+AdvisorMoveTab[i])==DstBoard){
            return true;
        }
    }
    return false;
}

//将
bool MoveControler::CanMoveK(int piecesId, int DstBoard){
    //判断是否在九宫内
    if(InFort[DstBoard]==0){
        return false;
    }

    for(int i=0;i<4;i++){
        if((Pieces[piecesId]+KingMoveTab[i])==DstBoard){
            return true;
        }
    }
    //检查照将
    if(Pieces[31]%16==Pieces[47]%16){
        int min = Pieces[31]>Pieces[47]?Pieces[31]:Pieces[47];
        int max = Pieces[31]<Pieces[47]?Pieces[31]:Pieces[47];
        for(int i=min+16;i<max;i+=16){
            if(PiecesInBoard[i]!=0){
                return false;
            }
        }
        return true;
    }
    return false;
}

bool MoveControler::CanMoveP(int piecesId, int DstBoard){
    //黑兵
    if((piecesId&32)!=0){
        if(DstBoard==Pieces[piecesId]+16){
            return true;
        }else if((DstBoard==Pieces[piecesId]+1 || DstBoard==Pieces[piecesId]-1) && (DstBoard&0x80)!=0){
            return true;
        }
    }else if((piecesId&16)!=0){//红兵
        if(DstBoard==Pieces[piecesId]-16){
            return true;
        }else if((DstBoard==Pieces[piecesId]+1 || DstBoard==Pieces[piecesId]-1) && (DstBoard&0x80)==0){
            return true;
        }
    }
    return false;
}

//将军检测(哪一方被将军)
bool MoveControler::Check(bool isRedturn){
    int PiecesId = isRedturn?31:47;
    //将作为車
    bool flag=false;//中间是否有棋子
    //向上
    for(int i=Pieces[PiecesId]-16;InBoard[i]!=0;i-=16){
        if((Board[i]==8 || Board[i]==16) && !isSameColor(PiecesId,PiecesInBoard[i])){//遇见异色車
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+16;j<max;j+=16){
                if(PiecesInBoard[j]!=0){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                return true;
            }
            flag=false;//重置flag
            break;
        }
    }
    //向下
    for(int i=Pieces[PiecesId]+16;InBoard[i]!=0;i+=16){
        if((Board[i]==8 || Board[i]==16) && !isSameColor(PiecesId,PiecesInBoard[i])){//遇见异色車
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+16;j<max;j+=16){
                if(PiecesInBoard[j]!=0){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                return true;
            }
            flag=false;
            break;
        }
    }
    //向左
    for(int i=Pieces[PiecesId]-1;InBoard[i]!=0;i--){
        if((Board[i]==8 || Board[i]==16) && !isSameColor(PiecesId,PiecesInBoard[i])){//遇见异色車
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+1;j<max;j++){
                if(PiecesInBoard[j]!=0){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                return true;
            }
            flag=false;
            break;
        }
    }
    //向右
    for(int i=Pieces[PiecesId]+1;InBoard[i]!=0;i++){
        if((Board[i]==8 || Board[i]==16) && !isSameColor(PiecesId,PiecesInBoard[i])){//遇见异色車
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+1;j<max;j++){
                if(PiecesInBoard[j]!=0){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                return true;
            }
            flag=false;
            break;
        }
    }

    //将作为马
    int LagTab[8] = {-17, -15, +15, +17, -17, +15, -15, +17};
    for(int i=0;i<8;i++){
        int DstPiece = Board[Pieces[PiecesId]+KnightMoveTab[i]];//目标类型
        if(DstPiece==9||DstPiece==17){
            int DstPieceId = PiecesInBoard[Pieces[PiecesId]+KnightMoveTab[i]];//目标编号
            int lag = Board[Pieces[PiecesId]+LagTab[i]];//马腿
            if(!isSameColor(DstPieceId,PiecesId) && lag==0){
                return true;
            }
        }
    }

    //将作为炮（只需扫描一条纵横线）
    int piecesNum=0;
    for(int i=Pieces[PiecesId]-16;InBoard[i]!=0;i-=16){
        if((Board[i]==10 || Board[i]==18) && !isSameColor(PiecesId,PiecesInBoard[i])){//如果是不同颜色炮
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+16;j<max;j+=16){
                if(PiecesInBoard[j]!=0){
                    piecesNum++;
                }
            }
            if(piecesNum==1){
                return true;
            }
            piecesNum=0;
        }
    }
    piecesNum=0;//多余？
    for(int i=Pieces[PiecesId]+16;InBoard[i]!=0;i+=16){
        if((Board[i]==10 || Board[i]==18) && !isSameColor(PiecesId,PiecesInBoard[i])){
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+16;j<max;j+=16){
                if(PiecesInBoard[j]!=0){
                    piecesNum++;
                }
            }
            if(piecesNum==1){
                return true;
            }
            piecesNum=0;
        }
    }
    piecesNum=0;
    for(int i=Pieces[PiecesId]-1;InBoard[i]!=0;i--){
        if((Board[i]==10 || Board[i]==18) && !isSameColor(PiecesId,PiecesInBoard[i])){
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+1;j<max;j++){
                if(PiecesInBoard[j]!=0){
                    piecesNum++;
                }
            }
            if(piecesNum==1){
                return true;
            }
            piecesNum=0;
        }
    }
    piecesNum=0;
    for(int i=Pieces[PiecesId]+1;InBoard[i]!=0;i++){
        if((Board[i]==10 || Board[i]==18) && !isSameColor(PiecesId,PiecesInBoard[i])){
            int min = Pieces[PiecesId]<i?Pieces[PiecesId]:i;
            int max = Pieces[PiecesId]>i?Pieces[PiecesId]:i;
            for(int j=min+1;j<max;j++){
                if(PiecesInBoard[j]!=0){
                    piecesNum++;
                }
            }
            if(piecesNum==1){
                return true;
            }
            piecesNum=0;
        }
    }

    //将作为过河兵
    //红兵(黑将)
    if(PiecesId==47){
        if(Board[Pieces[PiecesId]+16]==13){
            return true;
        }else if(Board[Pieces[PiecesId]-1]==13){
            return true;
        }else if(Board[Pieces[PiecesId]+1]==13){
            return true;
        }
    }
    //黑兵(红将)
    if(PiecesId==31){
        if(Board[Pieces[PiecesId]-16]==21){
            return true;
        }else if(Board[Pieces[PiecesId]-1]==21){
            return true;
        }else if(Board[Pieces[PiecesId]+1]==21){
            return true;
        }
    }

    //照将
    if(Pieces[31]%16==Pieces[47]%16){
        int flag=false;
        for(int i=Pieces[47]+16;i<Pieces[31];i+=16){
            if(Board[i]!=0){
                flag=true;
                break;
            }
        }
        if(!flag){
            return true;
        }
    }

    return false;
}

//判断两个棋子是否同色
bool MoveControler::isSameColor(int id1, int id2){
    if(Pieces[id1]==0||Pieces[id2]==0){
        return false;
    }
    if((id1&16)==0&&(id2&16)==0){
        return true;
    }
    if((id1&32)==0&&(id2&32)==0){
        return true;
    }
    return false;
}
