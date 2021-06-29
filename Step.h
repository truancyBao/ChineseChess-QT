#ifndef STEP_H
#define STEP_H


class Step
{
public:
    Step();
    //来源点原来的状态
    int Src_Position;//来源位置
    int Src_PiecesType;
    int Src_PiecesId;
    //目标点原来的状态
    int Dst_Position;//目标位置
    int Dst_PiecesType;
    int Dst_PiecesId;

    //历史启发得分
    int value=0;
    //Zobrist
    unsigned long long Zobrist;//保存走完这步之后的Zobrist
};

#endif // STEP_H
