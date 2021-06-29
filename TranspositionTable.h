#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include <QTime>
#include "Step.h"

const static int HashTableSize=2048*2048;//哈希表大小

class TranspositionTable
{
public:
    TranspositionTable(int*);
    int* Board;

    //Zobrist键值
    unsigned long long int U64[256][23];//坐标,棋子类型
    unsigned long long int MoveSideHash;//走棋方
    enum TYPE{exact,lower_bound,upper_bound};
    //置换表
    struct HASHITEM{
        unsigned long long int ZobristHash;
        int depth;
        int value;
        TYPE value_type;
        char m_padding[4];//填充
    }TT[HashTableSize];

    void initU64();//初始化U64数组
    unsigned long long int getZobrist(bool);//计算Zobrist值
    void getFakeMoveZobrist(Step*,unsigned long long int&);//走棋后计算Zobrist值(做几个改变即可)
    void getbackFakeMoveZobrist(Step*,unsigned long long int&);
    bool lookUpTT(unsigned long long int,int,int&,int,int);//键值，深度，返回值,alpha,beta
    void storeToTT(unsigned long long int,int,int,TYPE);//键值，深度，价值,类型
};

#endif // TRANSPOSITIONTABLE_H
