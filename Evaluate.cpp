#include "Evaluate.h"
#include "Position_Value.h"
#include "Auxiliary_Move.h"
#include "MoveControler.h"
#include <QDebug>

Evaluate::Evaluate(int* Board,int* Pieces,int* PiecesInBoard)
{
    this->Board = Board;
    this->Pieces = Pieces;
    this->PiecesInBoard = PiecesInBoard;
}

//估值
int Evaluate::evaluateGame(){
    int value=0;
    for(int i=16;i<48;i++){
        switch (i) {
        case 16:
        case 17:
        case 32:
        case 33:value+=evaluete_R(i);break;
        case 18:
        case 19:
        case 34:
        case 35:value+=evaluete_N(i);break;
        case 20:
        case 21:
        case 36:
        case 37:value+=evaluete_C(i);break;
        case 22:
        case 23:
        case 38:
        case 39:value+=evaluete_B(i);break;
        case 24:
        case 25:
        case 40:
        case 41:value+=evaluete_A(i);break;
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:value+=evaluete_P(i);break;
        case 31:
        case 47:value+=evaluete_K(i);break;
        }
    }
    return value;
}

//对马估值
int Evaluate::evaluete_N(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value = _N;
    for(int i=0;i<8;i++){
        int Dst = Pieces[PieciesId]+KnightMoveTab[i];
        int Lag = Pieces[PieciesId]+HorseLegTab[i];
        if(InBoard[Dst]==0 || Board[Lag]!=0){
            //什么也不做
            continue;
        }else if(Board[Dst]==0){
            value+=Mobility_N;//加机动性
        }else if(isSameColor(PiecesInBoard[Dst],PieciesId)){
            //保子价值
            switch (Board[Dst]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=LinkedHourses;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=LinkedHourses;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
        }else if(!isSameColor(PiecesInBoard[Dst],PieciesId)){
            value+=Mobility_N;
            //吃子价值
            switch (Board[Dst]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
        }
    }
    //针对黑棋的估值
    if((PieciesId&16)!=0){
        value+=positionValue_RN[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BN[Pieces[PieciesId]];
        return value;
    }
}

//象
int Evaluate::evaluete_B(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value=_B;
    for(int i=0;i<4;i++){
        int Dst = Pieces[PieciesId]+ElephantMoveTab[i];
        int Eye = Pieces[PieciesId]+ElephantEyeTab[i];
        if(InBoard[Dst]==0 || Board[Eye]!=0){
            continue;
        }else if((PieciesId&16)!=0){
            //红棋
            if((Dst&128)==0){//过河
                continue;
            }else if(Board[Dst]==0){
                value+=Mobility_B;
            }else if(isSameColor(PieciesId,PiecesInBoard[Dst])){
                //保子价值
                switch (Board[Dst]) {
                case 8:value+=_R/BaseValue_protect;break;
                case 9:value+=_N/BaseValue_protect;break;
                case 10:value+=_C/BaseValue_protect;break;
                case 11:value+=_B/BaseValue_protect;break;
                case 13:value+=_P/BaseValue_protect;break;
                case 16:value+=_R/BaseValue_protect;break;
                case 17:value+=_N/BaseValue_protect;break;
                case 18:value+=_C/BaseValue_protect;break;
                case 19:value+=_B/BaseValue_protect;break;
                case 21:value+=_P/BaseValue_protect;break;
                }
            }else if(!isSameColor(PieciesId,PiecesInBoard[Dst])){
                value+=Mobility_B;
                switch (Board[Dst]) {
                case 8:value+=_R/BaseValue_kill;break;
                case 9:value+=_N/BaseValue_kill;break;
                case 10:value+=_C/BaseValue_kill;break;
                case 13:value+=_P/BaseValue_kill;break;
                case 16:value+=_R/BaseValue_kill;break;
                case 17:value+=_N/BaseValue_kill;break;
                case 18:value+=_C/BaseValue_kill;break;
                case 21:value+=_P/BaseValue_kill;break;
                }
            }
        }else{//黑棋
            if((Dst&128)!=0){//过河
                continue;
            }else if(Board[Dst]==0){
                value+=Mobility_B;
            }else if(isSameColor(PieciesId,PiecesInBoard[Dst])){
                //保子价值
                switch (Board[Dst]) {
                case 8:value+=_R/BaseValue_protect;break;
                case 9:value+=_N/BaseValue_protect;break;
                case 10:value+=_C/BaseValue_protect;break;
                case 11:value+=_B/BaseValue_protect;break;
                case 13:value+=_P/BaseValue_protect;break;
                case 16:value+=_R/BaseValue_protect;break;
                case 17:value+=_N/BaseValue_protect;break;
                case 18:value+=_C/BaseValue_protect;break;
                case 19:value+=_B/BaseValue_protect;break;
                case 21:value+=_P/BaseValue_protect;break;
                }
            }else if(!isSameColor(PieciesId,PiecesInBoard[Dst])){
                value+=Mobility_B;
                switch (Board[Dst]) {
                case 8:value+=_R/BaseValue_kill;break;
                case 9:value+=_N/BaseValue_kill;break;
                case 10:value+=_C/BaseValue_kill;break;
                case 13:value+=_P/BaseValue_kill;break;
                case 16:value+=_R/BaseValue_kill;break;
                case 17:value+=_N/BaseValue_kill;break;
                case 18:value+=_C/BaseValue_kill;break;
                case 21:value+=_P/BaseValue_kill;break;
                }
            }
        }
    }
    if((PieciesId&16)!=0){
        value+=positionValue_RB[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BB[Pieces[PieciesId]];
        return value;
    }
}

//士
int Evaluate::evaluete_A(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value=_A;
    for(int i=0;i<4;i++){
        int Dst =Pieces[PieciesId]+AdvisorMoveTab[i];
        if(InFort[Dst]==0){
            continue;
        }else if(Board[Dst]==0){
            value+=Mobility_A;
        }else if(isSameColor(PieciesId,PiecesInBoard[PieciesId])){
            //保子价值
            switch (Board[Dst]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            }
        }else if(!isSameColor(PieciesId,PiecesInBoard[PieciesId])){
            value+=Mobility_A;
            switch (Board[Dst]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            }
        }
    }
    if((PieciesId&16)!=0){
        value+=positionValue_RA[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BA[Pieces[PieciesId]];
        return value;
    }
}

//将
int Evaluate::evaluete_K(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value=_K;
    int mobility=0;
    for(int i=0;i<4;i++){
        int Dst = Pieces[PieciesId]+KingMoveTab[i];
        if(InFort[Dst]==0){
            continue;
        }else if(Board[Dst]==0){
            mobility++;
        }else if(isSameColor(PieciesId,PiecesInBoard[PieciesId])){
            //保子价值
            switch (Board[Dst]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            }
        }else if(!isSameColor(PieciesId,PiecesInBoard[PieciesId])){
            mobility++;
            switch (Board[Dst]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
        }
    }
    if(mobility==0){
        value-=300;//将最好不困死
    }
    if((PieciesId&16)!=0){
        value+=positionValue_RK[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BK[Pieces[PieciesId]];
        return value;
    }
}

//兵(要怎么写最简洁)
int Evaluate::evaluete_P(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value=_P;
    if((PieciesId&16)!=0){//红兵
        int MoveTab[3]={0,0,0};
        MoveTab[0]=Pieces[PieciesId]-16;
        if((Pieces[PieciesId]&128)==0){//过河
            MoveTab[1]=Pieces[PieciesId]+1;
            MoveTab[2]=Pieces[PieciesId]-1;
        }
        for(int i=0;i<3;i++){
            int Dst=MoveTab[i];
            if(InBoard[Dst]==0){
                //什么也不做
            }else if(Board[Dst]==0){
                value+=Mobility_P;
            }else if(PiecesInBoard[Dst]&16){
                //保子价值
                switch (Board[Dst]) {
                case 8:value+=_R/BaseValue_protect;break;
                case 9:value+=_N/BaseValue_protect;break;
                case 10:value+=_C/BaseValue_protect;break;
                case 11:value+=_B/BaseValue_protect;break;
                case 13:
                    if(i!=0){
                       value+=LinkedPawns;break;//左右才是牵手兵
                    }else{
                       value+=_P/BaseValue_protect;break;
                    }
                case 14:break;
                }
            }else if(PiecesInBoard[Dst]&32){
                value+=Mobility_P;
                switch (Board[Dst]) {
                case 16:value+=_R/BaseValue_kill;break;
                case 17:value+=_N/BaseValue_kill;break;
                case 18:value+=_C/BaseValue_kill;break;
                case 19:value+=_B/BaseValue_kill;break;
                case 20:value+=_A/BaseValue_kill;break;
                case 21:value+=_P/BaseValue_kill;break;
                case 22:value+=BaseValue_check;break;//将军
                }
            }
        }
    }else{//黑兵
        int MoveTab[3]={0,0,0};
        MoveTab[0]=Pieces[PieciesId]+16;
        if((Pieces[PieciesId]&128)!=0){//过河
            MoveTab[1]=Pieces[PieciesId]+1;
            MoveTab[2]=Pieces[PieciesId]-1;
        }
        for(int i=0;i<3;i++){
            int Dst=MoveTab[i];
            if(InBoard[Dst]==0){
                //什么也不做
            }else if(Board[Dst]==0){
                value+=Mobility_P;
            }else if(PiecesInBoard[Dst]&32){
                //保子价值
                switch (Board[Dst]) {
                case 16:value+=_R/BaseValue_protect;break;
                case 17:value+=_N/BaseValue_protect;break;
                case 18:value+=_C/BaseValue_protect;break;
                case 19:value+=_B/BaseValue_protect;break;
                case 21:value+=_P/BaseValue_protect;break;
                case 22:break;
                }
            }else if(PiecesInBoard[Dst]&16){
                value+=Mobility_P;
                switch (Board[Dst]) {
                case 8:value+=_R/BaseValue_kill;break;
                case 9:value+=_N/BaseValue_kill;break;
                case 10:value+=_C/BaseValue_kill;break;
                case 11:value+=_B/BaseValue_kill;break;
                case 12:value+=_A/BaseValue_kill;break;
                case 13:value+=LinkedPawns;break;
                case 14:value+=BaseValue_check;break;//将军
                }
            }
        }
    }
    if((PieciesId&16)!=0){
        value+=positionValue_RP[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BP[Pieces[PieciesId]];
        return value;
    }
}

//車
int Evaluate::evaluete_R(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value=_R;
    //向上
    for(int i=Pieces[PieciesId]-16;InBoard[i]!=0;i-=16){
        if(Board[i]==0){
            value+=Mobility_R;
        }else if(isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_R;
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    //向下
    for(int i=Pieces[PieciesId]+16;InBoard[i]!=0;i+=16){
        if(Board[i]==0){
            value+=Mobility_R;
        }else if(isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_R;
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    //向左
    for(int i=Pieces[PieciesId]-1;InBoard[i]!=0;i--){
        if(Board[i]==0){
            value+=Mobility_R;
        }else if(isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_R;
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    //向右
    for(int i=Pieces[PieciesId]+1;InBoard[i]!=0;i++){
        if(Board[i]==0){
            value+=Mobility_R;
        }else if(isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_R;
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    if((PieciesId&16)!=0){
        value+=positionValue_RR[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BR[Pieces[PieciesId]];
        return value;
    }
}

//炮
int Evaluate::evaluete_C(int PieciesId){
    if(Pieces[PieciesId]==0){
        return 0;
    }
    int value=_C;
    bool flag=false;//隔子标志
    //向上
    for(int i=Pieces[PieciesId]-16;InBoard[i]!=0;i-=16){
        if(Board[i]==0&&!flag){
            value+=Mobility_C;
        }else if(Board[i]!=0&&!flag){
            flag=true;
        }else if(flag&&Board[i]==0){
            continue;
        }else if(flag&&isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(flag&&!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_C;
            //威胁价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    //向下
    flag=false;
    for(int i=Pieces[PieciesId]+16;InBoard[i]!=0;i+=16){
        if(Board[i]==0&&!flag){
            value+=Mobility_C;
        }else if(Board[i]!=0&&!flag){
            flag=true;
        }else if(flag&&Board[i]==0){
            continue;
        }else if(flag&&isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(flag&&!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_C;
            //威胁价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    //向左
    flag=false;
    for(int i=Pieces[PieciesId]-1;InBoard[i]!=0;i--){
        if(Board[i]==0&&!flag){
            value+=Mobility_C;
        }else if(Board[i]!=0&&!flag){
            flag=true;
        }else if(flag&&Board[i]==0){
            continue;
        }else if(flag&&isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(flag&&!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_C;
            //威胁价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    //向右
    flag=false;
    for(int i=Pieces[PieciesId]+1;InBoard[i]!=0;i++){
        if(Board[i]==0&&!flag){
            value+=Mobility_C;
        }else if(Board[i]!=0&&!flag){
            flag=true;
        }else if(flag&&Board[i]==0){
            continue;
        }else if(flag&&isSameColor(PieciesId,PiecesInBoard[i])){
            //保子价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_protect;break;
            case 9:value+=_N/BaseValue_protect;break;
            case 10:value+=_C/BaseValue_protect;break;
            case 11:value+=_B/BaseValue_protect;break;
            case 12:value+=_A/BaseValue_protect;break;
            case 13:value+=_P/BaseValue_protect;break;
            case 14:break;
            case 16:value+=_R/BaseValue_protect;break;
            case 17:value+=_N/BaseValue_protect;break;
            case 18:value+=_C/BaseValue_protect;break;
            case 19:value+=_B/BaseValue_protect;break;
            case 20:value+=_A/BaseValue_protect;break;
            case 21:value+=_P/BaseValue_protect;break;
            case 22:break;
            }
            break;
        }else if(flag&&!isSameColor(PieciesId,PiecesInBoard[i])){
            value+=Mobility_C;
            //威胁价值
            switch (Board[i]) {
            case 8:value+=_R/BaseValue_kill;break;
            case 9:value+=_N/BaseValue_kill;break;
            case 10:value+=_C/BaseValue_kill;break;
            case 11:value+=_B/BaseValue_kill;break;
            case 12:value+=_A/BaseValue_kill;break;
            case 13:value+=_P/BaseValue_kill;break;
            case 14:value+=BaseValue_check;break;//将军
            case 16:value+=_R/BaseValue_kill;break;
            case 17:value+=_N/BaseValue_kill;break;
            case 18:value+=_C/BaseValue_kill;break;
            case 19:value+=_B/BaseValue_kill;break;
            case 20:value+=_A/BaseValue_kill;break;
            case 21:value+=_P/BaseValue_kill;break;
            case 22:value+=BaseValue_check;break;//将军
            }
            break;
        }
    }
    if((PieciesId&16)!=0){
        value+=positionValue_RC[Pieces[PieciesId]];
        return -value;
    }else{
        value+=positionValue_BC[Pieces[PieciesId]];
        return value;
    }
}

//判断是否同色
bool Evaluate::isSameColor(int id1, int id2){
    if((id1&16)!=0 && (id2&16)!=0){
        return true;
    }else if((id1&32)!=0 && (id2&32)!=0){
        return true;
    }else{
        return false;
    }
}

//将军(检测哪一方被将)
bool Evaluate::Check(bool isRedturn){
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
