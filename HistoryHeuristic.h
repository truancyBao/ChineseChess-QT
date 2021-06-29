#ifndef HISTORYHEURISTIC_H
#define HISTORYHEURISTIC_H

#include <QVector>
#include "Step.h"

class HistoryHeuristic
{
public:
    HistoryHeuristic();

    int HistoryTable[256][256];

    void initHistoryTable();
    void addHistoryScore(Step*,int);
    void sortSteps(QVector<Step*>&);
};

#endif // HISTORYHEURISTIC_H
