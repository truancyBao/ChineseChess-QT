#include "HistoryHeuristic.h"

HistoryHeuristic::HistoryHeuristic()
{
    initHistoryTable();
}

//比较函数
static bool cmp(Step* step1,Step* step2){
    return step1->value>step2->value;
}

//初始化记录表
void HistoryHeuristic::initHistoryTable(){
    for(int i=0;i<256;i++){
        for(int j=0;j<256;j++){
            HistoryTable[i][j]=0;
        }
    }
}

//添加历史记录表
void HistoryHeuristic::addHistoryScore(Step* step, int depth){
    if(step==nullptr){
        return;
    }
    HistoryTable[step->Src_Position][step->Dst_Position]+=(depth*depth);//2,4,8,16,32...1,2,9,16,25
}

//排序步骤
void HistoryHeuristic::sortSteps(QVector<Step*>& steps){
    for(auto iter = steps.begin();iter!=steps.end();iter++){
        Step* step = *iter;
        step->value=HistoryTable[step->Src_Position][step->Dst_Position];
    }
    std::stable_sort(steps.begin(),steps.end(),cmp);
}
