#include "TranspositionTable.h"
#include <QDebug>

TranspositionTable::TranspositionTable(int* Board)
{
    this->Board=Board;
    initU64();
}

//初始化U64数组
void TranspositionTable::initU64(){
    qsrand(static_cast<unsigned int>(QTime(0,0,0).secsTo(QTime::currentTime())));
    for(int i=0;i<256;i++){
        for(int j=0;j<23;j++){
            unsigned long long int _U64=static_cast<unsigned long long int>(qrand())^static_cast<unsigned long long int>(qrand())<<15^static_cast<unsigned long long int>(qrand())<<30^static_cast<unsigned long long int>(qrand())<<45^static_cast<unsigned long long int>(qrand())<<60;
            U64[i][j]=_U64;
        }
    }
    MoveSideHash=static_cast<unsigned long long int>(qrand())^static_cast<unsigned long long int>(qrand())<<15^static_cast<unsigned long long int>(qrand())<<30^static_cast<unsigned long long int>(qrand())<<45^static_cast<unsigned long long int>(qrand())<<60;
}

//计算Zobrist
unsigned long long int TranspositionTable::getZobrist(bool isRedTurn){
    unsigned long long int ZobristValue=0;
    for(int i=51;i<=203;i++){
        if(Board[i]!=0){
            ZobristValue=ZobristValue^U64[i][Board[i]];
        }
    }
    //黑棋走再异或一次
    if(!isRedTurn){
        ZobristValue=ZobristValue^MoveSideHash;
    }
    return ZobristValue;
}

//走棋后计算Zobrist
void TranspositionTable::getFakeMoveZobrist(Step* step,unsigned long long int& Zobrist){
    //走棋后走棋方一定变化
    Zobrist=Zobrist^MoveSideHash;
    Zobrist=Zobrist^U64[step->Src_Position][step->Src_PiecesType];//源棋子移除
    Zobrist=Zobrist^U64[step->Dst_Position][step->Src_PiecesType];//源棋子落子
    if(step->Dst_PiecesType!=0){
        //目标位置有棋子
        Zobrist=Zobrist^U64[step->Dst_Position][step->Dst_PiecesType];//目标棋子移除
    }
}

//撤销棋子后计算
void TranspositionTable::getbackFakeMoveZobrist(Step* step,unsigned long long int& Zobrist){
    Zobrist=Zobrist^MoveSideHash;
    Zobrist=Zobrist^U64[step->Dst_Position][step->Src_PiecesType];//源棋子移除
    Zobrist=Zobrist^U64[step->Src_Position][step->Src_PiecesType];//源棋子落回
    if(step->Dst_PiecesType!=0){
        //目标位置有棋子
        Zobrist=Zobrist^U64[step->Dst_Position][step->Dst_PiecesType];//目标棋子添加
    }
}

//查找置换表(注意边界条件)
bool TranspositionTable::lookUpTT(unsigned long long int ZobristValue, int depth, int& value,int alpha,int beta){
    int HashIndex = ZobristValue%HashTableSize;
    HASHITEM Item = TT[HashIndex];//取出元素
    bool flag=false;//命中成功标志
    //更靠近根节点的才有用
    if(ZobristValue==Item.ZobristHash && Item.depth>=depth){
        //重复局面产生的值因为路径不同可能不同，不能直接用
        if(Item.value==-80088 || Item.value==-80060 || Item.value==80088 || Item.value==80060
                || Item.value==66666 || Item.value==-66666){
            return false;
        }
        switch (Item.value_type) {
        case exact:value=Item.value;flag=true;break;
        case lower_bound:
            if(Item.value>=beta){
                value=Item.value;
                flag=true;
            }
            break;
        case upper_bound:
            if(Item.value<=alpha){
                value=Item.value;
                flag=true;
            }
            break;
        }
    }
    return flag;
}

//存入置换表
void TranspositionTable::storeToTT(unsigned long long int ZobristValue, int depth, int value,TYPE value_type){
    int HashIndex = ZobristValue%HashTableSize;
    HASHITEM Item = TT[HashIndex];
    //更靠近根节点的不应该覆盖
    if(ZobristValue==Item.ZobristHash && Item.depth>depth){
        if(Item.value==-80088 || Item.value==-80060 || Item.value==80088 || Item.value==80060
                || Item.value==66666 || Item.value==-66666){//和棋局面无价值
            TT[HashIndex].ZobristHash=ZobristValue;
            TT[HashIndex].depth=depth;
            TT[HashIndex].value=value;
            TT[HashIndex].value_type=value_type;
        }
        return;
    }
    TT[HashIndex].ZobristHash=ZobristValue;
    TT[HashIndex].depth=depth;
    TT[HashIndex].value=value;
    TT[HashIndex].value_type=value_type;
}
