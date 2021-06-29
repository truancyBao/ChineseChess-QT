#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QComboBox>
#include <QVector>
#include <QThread>
#include <QTimer>
#include <QTextEdit>
#include <QMetaType>

#include "MoveControler.h"
#include "AlphaBeta.h"
#include "AlphaBetaAndTT.h"
#include "AlphaBetaAndHistory.h"
#include "AlphaBeta_TTHH.h"
#include "IDAlphaBeta.h"

const static int MARGIN_X = 32;//边缘宽
const static int MARGIN_Y = 32+20;//边缘高
const static int Grid_Width = 57;//格子宽度
const static int Radius = 28;//棋子半径

class ChessBoard : public QMainWindow
{
    Q_OBJECT

public:
    ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();

    QComboBox* choiceBox;//选项
    QPushButton* startButton;//开始
    QPushButton* backButton;//悔棋
    QPushButton* RotateButton;//旋转棋盘
    QPushButton* RelayButton;//电脑接替
    QPushButton* PauseBUtton;//暂停
    QComboBox* difficultyBox;//难度选择
    QComboBox* engineBox;//引擎选择
    QTextEdit* showStepsArea;//步骤显示域

    //每次移动要维护这三张表
    int Board[256];//棋盘数组,下标对应棋盘网格，内容存棋子类型
    int Pieces[48];//棋子数组，下标表示棋子编号，内容存棋子在Board坐标
    int PiecesInBoard[256];//联系数组

    QVector<Step*> AllMovedSteps;

    MoveControler* MC;//走法控制器
    TranspositionTable* TT;//置换表
    AlphaBeta* alphaBeta;//搜索
    AlphaBetaAndTT* alphaBetaAndTT;//加置换表的搜索
    AlphaBetaAndHistory* alphaBetaAndHistory;//历史启发搜索
    AlphaBeta_TTHH* alphaBeta_TTHH;//置换表结合历史启发
    IDAlphaBeta* IDalphaBeta;//迭代深化

    bool Rotate;//是否旋转棋盘

    int selectId=-1;//被选的棋子编号

    bool isRedTurn;//是否红方走
    bool isComputerGo;//是否电脑走棋
    bool computerColor;//电脑颜色（true红色，false黑色）
    bool isGaming;//是否游戏结束
    unsigned long long int Zobrist;//当前zobrist值

    //信息变量
    QString turnMessage;//右上角变量
    QString showMessage;//右下角变量

    void paintEvent(QPaintEvent *);//绘图
    void mouseReleaseEvent(QMouseEvent *event);//点击

    //辅助函数
    QPoint getPos(int row,int col);//根据网格计算坐标
    QRect getRect(int row,int col);//获取矩形区域
    bool getRowCol(QPoint& pt, int& boardPosition);//根据点击坐标范围计算网格

    void drawBoard(QPainter&);//画棋盘
    void drawChess(QPainter&);//画棋子
    void drawSelectChess(QPainter&);

    void ReadFEN(QString FEN);//读取FEN串
    void initBoard();//棋盘初始化
    void manGo(QMouseEvent*);//人走棋
    void putChess(Step*);//下棋
    bool isWin();
    //遇见重复局面，查看是否长将，是否长捉（三次捉同一棋子），都不是则和棋
    //注意置换表对于长走判负，路径不同不一定是长走
    int checkRepeat();//返回0正常，1和棋，2长捉长将负
    void showLastStep();//展示走过的步

public slots:
    void startGame();//开始游戏
    void goBack();//悔棋
    void rotateBoard();//旋转棋盘
    void computerRelay();//电脑接替
    void pauseGame();//暂停游戏
    void computerGo(Step*);
    void doComputerGo();

signals:
    void startAlphaBeta(int*,int*,int*,bool,int);
    void startAlphaBetaAndTT(int*,int*,int*,bool,int,QVector<Step*>&);
    void startAlphaBetaAndHistory(int*,int*,int*,bool,int);
    void startAlphaBeta_TTHH(int*,int*,int*,bool,int,QVector<Step*>&);
    void startIDAlphaBeta(int*,int*,int*,bool,int,QVector<Step*>&);
    void endComputerGo();
};
#endif // CHESSBOARD_H
