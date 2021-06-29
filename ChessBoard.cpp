#include "ChessBoard.h"
#include <QDebug>

ChessBoard::ChessBoard(QWidget *parent)
    : QMainWindow(parent)
{
    qRegisterMetaType<Step>("QVector<Step*>&");//注册Step类型
    this->setWindowIcon(QIcon("../ChineseChess_2020/images/ChineseChess.ico"));
    this->resize(520+260,576+20*2);
    //开始
    startButton = new QPushButton("开始",this);
    startButton->move(520+30,Grid_Width+20);
    startButton->resize(100,40);
    //悔棋
    backButton = new QPushButton("悔棋",this);
    backButton->move(520+30,Grid_Width*2+20);
    backButton->resize(100,40);
    //旋转棋盘
    RotateButton = new QPushButton("旋转棋盘",this);
    RotateButton->move(520+30,Grid_Width*3+20);
    RotateButton->resize(100,40);
    //电脑接替
    RelayButton = new QPushButton("电脑接替",this);
    RelayButton->move(520+30,Grid_Width*4+20);
    RelayButton->resize(100,40);
    //暂停按钮
    PauseBUtton = new QPushButton("暂停/继续",this);
    PauseBUtton->move(520+30,Grid_Width*5+20);
    PauseBUtton->resize(100,40);
    //选项
    choiceBox = new QComboBox(this);
    choiceBox->move(640+30,Grid_Width+20);
    choiceBox->addItem(QString("玩家先走"));
    choiceBox->addItem(QString("电脑先走"));
    choiceBox->addItem(QString("电脑对弈"));
    choiceBox->addItem(QString("不用电脑"));
    choiceBox->resize(100,40);
    //难度选择
    difficultyBox = new QComboBox(this);
    difficultyBox->addItem(QString("简单"));
    difficultyBox->addItem(QString("中等"));
    difficultyBox->addItem(QString("困难"));
    difficultyBox->addItem(QString("棋圣"));
    difficultyBox->move(640+30,Grid_Width*2+20);
    difficultyBox->resize(100,40);
    //引擎选择
    engineBox = new QComboBox(this);
    engineBox->addItem(QString("负极大值"));
    engineBox->addItem(QString("置换表"));
    engineBox->addItem(QString("历史启发"));
    engineBox->addItem(QString("历史+置换"));
    engineBox->addItem(QString("迭代深化"));
    engineBox->move(640+30,Grid_Width*3+20);
    engineBox->resize(100,40);
    //步骤显示域
    showStepsArea = new QTextEdit(this);
    showStepsArea->move(520+30,Grid_Width*6+20);
    showStepsArea->resize(140,120);
    showStepsArea->setReadOnly(true);//只读

    initBoard();//初始化棋盘

    TT = new TranspositionTable(Board);//置换表
    MC = new MoveControler(Board,Pieces,PiecesInBoard);//走法控制器
    alphaBeta = new AlphaBeta();
    alphaBetaAndTT = new AlphaBetaAndTT(TT);
    alphaBetaAndHistory = new AlphaBetaAndHistory();
    alphaBeta_TTHH = new AlphaBeta_TTHH(TT);
    IDalphaBeta = new IDAlphaBeta(TT);

    //开辟搜索线程
    QThread* SearchThread = new QThread();
    alphaBeta->moveToThread(SearchThread);
    alphaBetaAndTT->moveToThread(SearchThread);
    alphaBetaAndHistory->moveToThread(SearchThread);
    alphaBeta_TTHH->moveToThread(SearchThread);
    IDalphaBeta->moveToThread(SearchThread);
    SearchThread->start();

    //按钮绑定
    connect(startButton,&QPushButton::clicked,this,&ChessBoard::startGame);
    connect(backButton,&QPushButton::clicked,this,&ChessBoard::goBack);
    connect(RotateButton,&QPushButton::clicked,this,&ChessBoard::rotateBoard);
    connect(RelayButton,&QPushButton::clicked,this,&ChessBoard::computerRelay);
    connect(PauseBUtton,&QPushButton::clicked,this,&ChessBoard::pauseGame);
    //线程控制
    connect(this,&ChessBoard::startAlphaBeta,alphaBeta,&AlphaBeta::getBestStep);//启动AlphaBeta引擎
    connect(this,&ChessBoard::startAlphaBetaAndTT,alphaBetaAndTT,&AlphaBetaAndTT::getBestStep);//启动AlphaBetaTT引擎
    connect(this,&ChessBoard::startAlphaBetaAndHistory,alphaBetaAndHistory,&AlphaBetaAndHistory::getBestStep);//启动历史启发引擎
    connect(this,&ChessBoard::startAlphaBeta_TTHH,alphaBeta_TTHH,&AlphaBeta_TTHH::getBestStep);//置换表+历史启发
    connect(this,&ChessBoard::startIDAlphaBeta,IDalphaBeta,&IDAlphaBeta::getBestStep);//迭代深化
    //电脑走棋信号控制
    connect(alphaBeta,&AlphaBeta::endSearch,this,&ChessBoard::computerGo);
    connect(alphaBetaAndTT,&AlphaBetaAndTT::endSearch,this,&ChessBoard::computerGo);
    connect(alphaBetaAndHistory,&AlphaBetaAndHistory::endSearch,this,&ChessBoard::computerGo);
    connect(alphaBeta_TTHH,&AlphaBeta_TTHH::endSearch,this,&ChessBoard::computerGo);
    connect(IDalphaBeta,&IDAlphaBeta::endSearch,this,&ChessBoard::computerGo);
}

ChessBoard::~ChessBoard()
{
}

//初始化棋盘
void ChessBoard::initBoard(){
    isGaming=false;
    isComputerGo=false;
    //RNBAKABNR/9/1C5C1/P1P1P1P1P/9/9/p1p1p1p1p/1c5c1/9/rnbakabnr w - - 0 1
    //rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1
    ReadFEN("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1");
    choiceBox->setCurrentText("玩家先走");
    difficultyBox->setCurrentText("中等");
    engineBox->setCurrentText("迭代深化");
}

//开始游戏
void ChessBoard::startGame(){
    isGaming=true;
    ReadFEN("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1");
    TT->initU64();
    Zobrist = TT->getZobrist(isRedTurn);//计算当前Zobrist值
    if(choiceBox->currentText()=="玩家先走"){
        Rotate=false;
        isComputerGo=false;
        computerColor=false;//电脑黑色
    }else if(choiceBox->currentText()=="电脑先走"){
        Rotate=true;
        isComputerGo=true;
        computerColor=true;//电脑红棋
        update();
        QTimer::singleShot(100,this,SLOT(doComputerGo()));
    }else if(choiceBox->currentText()=="电脑对弈"){
        Rotate=false;
        isComputerGo=true;
        computerColor=true;//电脑初始红棋
        update();
        QTimer::singleShot(100,this,SLOT(doComputerGo()));
    }else if(choiceBox->currentText()=="不用电脑"){
        Rotate=false;
        isComputerGo=false;
    }else{//默认人先走
        Rotate=false;
        isComputerGo=false;
        computerColor=false;//电脑黑色
    }
    if(!AllMovedSteps.isEmpty()){
        qDeleteAll(AllMovedSteps);
        QVector<Step*>().swap(AllMovedSteps);
    }
    showStepsArea->clear();
    selectId=-1;
    turnMessage="";
    showMessage="";
    update();
}

//旋转棋盘
void ChessBoard::rotateBoard(){
    Rotate=!Rotate;
    update();
}

//绘制界面
void ChessBoard::paintEvent(QPaintEvent*){
    // 棋盘起始点为(28,28),每格40
    QPainter painter(this);
    drawBoard(painter);
    drawChess(painter);
    drawSelectChess(painter);
    //写信息
    painter.setPen(QColor(163,51,46));
    painter.setFont(QFont("",Grid_Width/2,1000,false));
    if(!isGaming){
        turnMessage = "未开始";
    }else{
        turnMessage = isRedTurn?"红棋走":"黑棋走";
    }
    painter.drawText(520,10,Grid_Width*5,Grid_Width,Qt::AlignCenter,turnMessage);
    painter.drawText(520,500,Grid_Width*5,Grid_Width,Qt::AlignCenter,showMessage);

}

//绘制棋盘
void ChessBoard::drawBoard(QPainter& painter){
    painter.drawPixmap(0,20,521,577,QPixmap("../ChineseChess_2020/images/WOOD.JPG"));
    //九个数字
    QRect rect = QRect(0,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"九");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"9");
    }
    rect = QRect(0+Grid_Width,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"八");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"8");
    }
    rect = QRect(0+Grid_Width*2,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"七");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"7");
    }
    rect = QRect(0+Grid_Width*3,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"六");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"6");
    }
    rect = QRect(0+Grid_Width*4,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"五");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"5");
    }
    rect = QRect(0+Grid_Width*5,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"四");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"4");
    }
    rect = QRect(0+Grid_Width*6,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"三");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"3");
    }
    rect = QRect(0+Grid_Width*7,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"二");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"2");
    }
    rect = QRect(0+Grid_Width*8,576+20,57,20);
    painter.drawRect(rect);
    if(!Rotate){
       painter.drawText(rect,Qt::AlignCenter,"一");
    }else{
       painter.drawText(rect,Qt::AlignCenter,"1");
    }
    //9个数字
    rect = QRect(0,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"1");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"一");
    }
    rect = QRect(0+Grid_Width,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"2");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"二");
    }
    rect = QRect(0+Grid_Width*2,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"3");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"三");
    }
    rect = QRect(0+Grid_Width*3,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"4");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"四");
    }
    rect = QRect(0+Grid_Width*4,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"5");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"五");
    }
    rect = QRect(0+Grid_Width*5,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"6");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"六");
    }
    rect = QRect(0+Grid_Width*6,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"7");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"七");
    }
    rect = QRect(0+Grid_Width*7,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"8");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"八");
    }
    rect = QRect(0+Grid_Width*8,0,57,20);
    painter.drawRect(rect);
    if(!Rotate){
        painter.drawText(rect,Qt::AlignCenter,"9");
    }else{
        painter.drawText(rect,Qt::AlignCenter,"九");
    }
}

//画棋子(根据board数组画)
void ChessBoard::drawChess(QPainter& painter){
        for(int i=51;i<=203;i++){
            int X,Y;
            if(!Rotate){
                X=MARGIN_X+Grid_Width*(i%16-3)-Radius;
                Y=MARGIN_Y+Grid_Width*(i/16-3)-Radius;
            }else{
                X=MARGIN_X+Grid_Width*(8-(i%16-3))-Radius;
                Y=MARGIN_Y+Grid_Width*(9-(i/16-3))-Radius;
            }
            switch (Board[i]) {
            case 0:
                break;
            case 8:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RR.GIF"));
                break;
            case 9:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RN.GIF"));
                break;
            case 10:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RC.GIF"));
                break;
            case 11:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RB.GIF"));
                break;
            case 12:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RA.GIF"));
                break;
            case 13:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RP.GIF"));
                break;
            case 14:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RK.GIF"));
                break;
            case 16:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BR.GIF"));
                break;
            case 17:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BN.GIF"));
                break;
            case 18:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BC.GIF"));
                break;
            case 19:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BB.GIF"));
                break;
            case 20:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BA.GIF"));
                break;
            case 21:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BP.GIF"));
                break;
            case 22:
                painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BK.GIF"));
                break;
            }
        }
}

//画选择棋子和移动轨迹
void ChessBoard::drawSelectChess(QPainter &painter){
    //画选择棋子
    if(selectId!=-1){
        int position = Pieces[selectId];//棋子坐标
        int chessType = Board[position];//棋子类型
        int X,Y;
        if(!Rotate){
            X=MARGIN_X+Grid_Width*(position%16-3)-Radius;
            Y=MARGIN_Y+Grid_Width*(position/16-3)-Radius;
        }else{
            X=MARGIN_X+Grid_Width*(8-(position%16-3))-Radius;
            Y=MARGIN_Y+Grid_Width*(9-(position/16-3))-Radius;
        }
        switch (chessType) {
        case 8:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RRS.GIF"));
            break;
        case 9:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RNS.GIF"));
            break;
        case 10:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RCS.GIF"));
            break;
        case 11:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RBS.GIF"));
            break;
        case 12:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RAS.GIF"));
            break;
        case 13:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RPS.GIF"));
            break;
        case 14:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RKS.GIF"));
            break;
        case 16:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BRS.GIF"));
            break;
        case 17:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BNS.GIF"));
            break;
        case 18:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BCS.GIF"));
            break;
        case 19:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BBS.GIF"));
            break;
        case 20:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BAS.GIF"));
            break;
        case 21:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BPS.GIF"));
            break;
        case 22:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BKS.GIF"));
            break;
        }
    }

    //画最后一个棋路轨迹
    if(!AllMovedSteps.isEmpty()){
        Step* step=AllMovedSteps.back();
        int X,Y;
        int chessType = Board[step->Dst_Position];
        if(!Rotate){
            X=MARGIN_X+Grid_Width*(step->Dst_Position%16-3)-Radius;
            Y=MARGIN_Y+Grid_Width*(step->Dst_Position/16-3)-Radius;
        }else{
            X=MARGIN_X+Grid_Width*(8-(step->Dst_Position%16-3))-Radius;
            Y=MARGIN_Y+Grid_Width*(9-(step->Dst_Position/16-3))-Radius;
        }
        switch (chessType) {
        case 8:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RRS.GIF"));
            break;
        case 9:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RNS.GIF"));
            break;
        case 10:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RCS.GIF"));
            break;
        case 11:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RBS.GIF"));
            break;
        case 12:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RAS.GIF"));
            break;
        case 13:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RPS.GIF"));
            break;
        case 14:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/RKS.GIF"));
            break;
        case 16:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BRS.GIF"));
            break;
        case 17:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BNS.GIF"));
            break;
        case 18:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BCS.GIF"));
            break;
        case 19:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BBS.GIF"));
            break;
        case 20:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BAS.GIF"));
            break;
        case 21:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BPS.GIF"));
            break;
        case 22:
            painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/BKS.GIF"));
            break;
        }
        if(!Rotate){
            X=MARGIN_X+Grid_Width*(step->Src_Position%16-3)-Radius;
            Y=MARGIN_Y+Grid_Width*(step->Src_Position/16-3)-Radius;
        }else{
            X=MARGIN_X+Grid_Width*(8-(step->Src_Position%16-3))-Radius;
            Y=MARGIN_Y+Grid_Width*(9-(step->Src_Position/16-3))-Radius;
        }
        painter.drawPixmap(X,Y,Radius*2,Radius*2,QPixmap("../ChineseChess_2020/images/WOOD/OOS.GIF"));
    }

}

//帅K士A象B/E马N/H車R炮C兵P,小写黑,大写红
void ChessBoard::ReadFEN(QString FEN){
    //清空棋盘和棋子数组
    for(int i=0;i<256;i++){
        Board[i]=0;
        PiecesInBoard[i]=0;
    }

    for(int i=0;i<48;i++){
        Pieces[i]=0;
    }

    //rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1
    QStringList list = FEN.split(" ");//分割成六串
    //棋子摆法,分成十行
    QStringList list1 = list[0].split("/");

    for(int i=0;i<list1.size();i++){//十行
        int k=0;//辅助扫描两个数组
        QStringList _list = list1[i].split("");//前后都会多一个""空字符串

        for(int j=1;j<_list.size()-1;j++){//每行每个字符读取
            if(_list[j]=="r"){//黑車
                Board[51+k+16*i]=16;//更新棋盘
                k++;
            }else if(_list[j]=="n"||_list[j]=="h"){//黑马
                Board[51+k+16*i]=17;
                k++;
            }else if(_list[j]=="c"){//黑炮
                Board[51+k+16*i]=18;
                k++;
            }else if(_list[j]=="b"||_list[j]=="e"){//黑象
                Board[51+k+16*i]=19;
                k++;
            }else if(_list[j]=="a"){//黑士
                Board[51+k+16*i]=20;
                k++;
            }else if(_list[j]=="p"){//黑兵
                Board[51+k+16*i]=21;
                k++;
            }else if(_list[j]=="k"){//黑将
                Board[51+k+16*i]=22;
                k++;
            }else if(_list[j]=="R"){//红車
                Board[51+k+16*i]=8;
                k++;
            }else if(_list[j]=="N"||_list[j]=="H"){//红马
                Board[51+k+16*i]=9;
                k++;
            }else if(_list[j]=="C"){//红炮
                Board[51+k+16*i]=10;
                k++;
            }else if(_list[j]=="B"||_list[j]=="E"){//红象
                Board[51+k+16*i]=11;
                k++;
            }else if(_list[j]=="A"){//红士
                Board[51+k+16*i]=12;
                k++;
            }else if(_list[j]=="P"){//红兵
                Board[51+k+16*i]=13;
                k++;
            }else if(_list[j]=="K"){//红将
                Board[51+k+16*i]=14;
                k++;
            }else if(_list[j].toInt()>=1&&_list[j].toInt()<=9){
                int s=_list[j].toInt();
                while(s--){
                    Board[51+k+16*i]=0;
                    k++;
                }
            }
        }
    }

    //设置棋子数组（扫描棋盘得到）
    int R=0;
    int N=0;
    int C=0;
    int B=0;
    int A=0;
    int P=0;
    int r=0;
    int n=0;
    int c=0;
    int b=0;
    int a=0;
    int p=0;
    for(int i=51;i<=203;i++){
        switch(Board[i]){
        case 8://红車
            if(R==0){
                Pieces[16]=i;
                R++;
            }else{
                Pieces[17]=i;
            }
            break;
        case 9://红马
            if(N==0){
                Pieces[18]=i;
                N++;
            }else{
                Pieces[19]=i;
            }
            break;
        case 10://红炮
            if(C==0){
                Pieces[20]=i;
                C++;
            }else{
                Pieces[21]=i;
            }
            break;
        case 11://红象
            if(B==0){
                Pieces[22]=i;
                B++;
            }else{
                Pieces[23]=i;
            }
            break;
        case 12://红士
            if(A==0){
                Pieces[24]=i;
                A++;
            }else{
                Pieces[25]=i;
            }
            break;
        case 13://红兵
            if(P==0){
                Pieces[26]=i;
                P++;
            }else if(P==1){
                Pieces[27]=i;
                P++;
            }else if(P==2){
                Pieces[28]=i;
                P++;
            }else if(P==3){
                Pieces[29]=i;
                P++;
            }else if(P==4){
                Pieces[30]=i;
            }
            break;
        case 14://红将
            Pieces[31]=i;
            break;
        case 16://黑車
            if(r==0){
                Pieces[32]=i;
                r++;
            }else{
                Pieces[33]=i;
            }
            break;
        case 17://黑马
            if(n==0){
                Pieces[34]=i;
                n++;
            }else{
                Pieces[35]=i;
            }
            break;
        case 18://黑炮
            if(c==0){
                Pieces[36]=i;
                c++;
            }else{
                Pieces[37]=i;
            }
            break;
        case 19://黑象
            if(b==0){
                Pieces[38]=i;
                b++;
            }else{
                Pieces[39]=i;
            }
            break;
        case 20://黑士
            if(a==0){
                Pieces[40]=i;
                a++;
            }else{
                Pieces[41]=i;
            }
            break;
        case 21://黑兵
            if(p==0){
                Pieces[42]=i;
                p++;
            }else if(p==1){
                Pieces[43]=i;
                p++;
            }else if(p==2){
                Pieces[44]=i;
                p++;
            }else if(p==3){
                Pieces[45]=i;
                p++;
            }else if(p==4){
                Pieces[46]=i;
                p++;
            }
            break;
        case 22://黑将
            Pieces[47]=i;
            break;
        }
    }
    //设置联系数组
    for(int i=51;i<256;i++){
        for(int j=16;j<48;j++){
            if(Pieces[j]==i){
                PiecesInBoard[i]=j;
            }
        }
    }

    //读取哪一方走棋
    QString list2 = list[1];
    if(list2!=b){
        isRedTurn=true;
    }else{
        isRedTurn=false;
    }

}

//鼠标控制
void ChessBoard::mouseReleaseEvent(QMouseEvent *event){
    manGo(event);
}

//点击后对应棋盘数组
bool ChessBoard::getRowCol(QPoint& pt, int& boardPosition){
    //棋盘纵横格
    int row;
    int col;
    if((pt.rx()-MARGIN_X)%Grid_Width<Radius){
        row = (pt.rx()-MARGIN_X)/Grid_Width;
    }else{
        row = (pt.rx()-MARGIN_X)/Grid_Width+1;
    }
    if((pt.ry()-MARGIN_Y)%Grid_Width<Radius){
        col = (pt.ry()-MARGIN_Y)/Grid_Width;
    }else{
        col = (pt.ry()-MARGIN_Y)/Grid_Width+1;
    }
    if(Rotate){//旋转
        row=8-row;
        col=9-col;
    }
    if(row<0||row>8){
        return false;
    }
    if(col<0||col>9){
        return false;
    }
    //对应逻辑棋盘位置
    boardPosition=51+col*16+row;
    return true;
}

//人走棋
void ChessBoard::manGo(QMouseEvent* event){
    if(!isGaming){
        return;
    }
    if(isComputerGo){
        return;
    }
    QPoint pt = event->pos();
    int boardPosition;//点击位置对应棋盘坐标
    if(!getRowCol(pt,boardPosition)){
        return;
    }
    if(selectId==-1&&Board[boardPosition]!=0){
        selectId=PiecesInBoard[boardPosition];
        if(isRedTurn&&(selectId&32)){
            selectId=-1;
            return;
        }else if(!isRedTurn&&(selectId&16)){
            selectId=-1;
            return;
        }
    }else if(selectId!=-1 && (((selectId&16)!=0&&(PiecesInBoard[boardPosition]&16)!=0)
                              || ((selectId&32)!=0&&(PiecesInBoard[boardPosition]&32)!=0))){
        selectId=PiecesInBoard[boardPosition];
        if(isRedTurn&&(selectId&32)){
            selectId=-1;
            return;
        }else if(!isRedTurn&&(selectId&16)){
            selectId=-1;
            return;
        }
    }else if(selectId!=-1){
        //走棋时注意六个部分数值，即三个数组对应走棋之前的值和走棋之后的值
        //selectId棋子ID，boardPosition目标位置在棋盘中的坐标
        if(!MC->CanMove(selectId,boardPosition)){
            return;
        }
        //保存步骤
        Step* step = new Step();
        step->Src_Position=Pieces[selectId];
        step->Src_PiecesType=Board[Pieces[selectId]];
        step->Src_PiecesId=selectId;
        step->Dst_Position=boardPosition;
        step->Dst_PiecesType=Board[boardPosition];
        step->Dst_PiecesId=PiecesInBoard[boardPosition];
        //下棋
        putChess(step);

        //如果走完被将军，则撤销走法(考虑走完后吃对方将)
        if(MC->Check(!isRedTurn) && Pieces[31]!=0 && Pieces[47]!=0){
            //撤回
            Board[step->Src_Position]=step->Src_PiecesType;
            PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
            Pieces[step->Src_PiecesId]=step->Src_Position;
            Board[step->Dst_Position]=step->Dst_PiecesType;
            PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
            if(step->Dst_PiecesId!=0){
                Pieces[step->Dst_PiecesId]=step->Dst_Position;
            }
            AllMovedSteps.removeLast();
            showStepsArea->undo();
            delete step;
            step=nullptr;
            isRedTurn=!isRedTurn;
            if(choiceBox->currentText()=="玩家先走"||choiceBox->currentText()=="电脑先走"){
                isComputerGo=!isComputerGo;
            }
            showMessage="被将";
            update();
            return;
        }
        //是否胜利
        if(isWin()){
            isGaming=false;
            return;
        }
        //是否和棋
        if(AllMovedSteps.size()>300){
            isGaming=false;
            showMessage="和棋";
            return;
        }
        //是否存在重复局面
        int checkFlag=checkRepeat();
        if(checkFlag==1){
            isGaming=false;
            showMessage="和棋";
            return;
        }else if(checkFlag==2){
            isGaming=false;
            showMessage="长将负";
            return;
        }else if(checkFlag==3){
            isGaming=false;
            showMessage="长捉负";
            return;
        }
        if(MC->Check(isRedTurn)){
            showMessage="将军";
        }else{
            showMessage="";
        }
        update();
        if(choiceBox->currentText()=="玩家先走"||choiceBox->currentText()=="电脑先走"){
            QTimer::singleShot(100,this,SLOT(doComputerGo()));
        }
    }
    update();
}

void ChessBoard::doComputerGo(){
    if(isGaming==false){
        return;
    }
    if(difficultyBox->currentText()=="简单"){
        if(engineBox->currentText()=="负极大值"){
            emit startAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,4);
        }else if(engineBox->currentText()=="置换表"){
            emit startAlphaBetaAndTT(Board,Pieces,PiecesInBoard,computerColor,4,AllMovedSteps);
        }else if(engineBox->currentText()=="历史启发"){
            emit startAlphaBetaAndHistory(Board,Pieces,PiecesInBoard,computerColor,4);
        }else if(engineBox->currentText()=="历史+置换"){
            emit startAlphaBeta_TTHH(Board,Pieces,PiecesInBoard,computerColor,4,AllMovedSteps);
        }else if(engineBox->currentText()=="迭代深化"){
            emit startIDAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,5,AllMovedSteps);
        }
    }else if(difficultyBox->currentText()=="中等"){
        if(engineBox->currentText()=="负极大值"){
            emit startAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,5);
        }else if(engineBox->currentText()=="置换表"){
            emit startAlphaBetaAndTT(Board,Pieces,PiecesInBoard,computerColor,5,AllMovedSteps);
        }else if(engineBox->currentText()=="历史启发"){
            emit startAlphaBetaAndHistory(Board,Pieces,PiecesInBoard,computerColor,5);
        }else if(engineBox->currentText()=="历史+置换"){
            emit startAlphaBeta_TTHH(Board,Pieces,PiecesInBoard,computerColor,5,AllMovedSteps);
        }else if(engineBox->currentText()=="迭代深化"){
            emit startIDAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,10,AllMovedSteps);
        }
    }else if(difficultyBox->currentText()=="困难"){
        if(engineBox->currentText()=="负极大值"){
            emit startAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,6);
        }else if(engineBox->currentText()=="置换表"){
            emit startAlphaBetaAndTT(Board,Pieces,PiecesInBoard,computerColor,6,AllMovedSteps);
        }else if(engineBox->currentText()=="历史启发"){
            emit startAlphaBetaAndHistory(Board,Pieces,PiecesInBoard,computerColor,6);
        }else if(engineBox->currentText()=="历史+置换"){
            emit startAlphaBeta_TTHH(Board,Pieces,PiecesInBoard,computerColor,6,AllMovedSteps);
        }else if(engineBox->currentText()=="迭代深化"){
            emit startIDAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,20,AllMovedSteps);
        }
    }else if(difficultyBox->currentText()=="棋圣"){
        if(engineBox->currentText()=="负极大值"){
            emit startAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,7);
        }else if(engineBox->currentText()=="置换表"){
            emit startAlphaBetaAndTT(Board,Pieces,PiecesInBoard,computerColor,7,AllMovedSteps);
        }else if(engineBox->currentText()=="历史启发"){
            emit startAlphaBetaAndHistory(Board,Pieces,PiecesInBoard,computerColor,7);
        }else if(engineBox->currentText()=="历史+置换"){
            emit startAlphaBeta_TTHH(Board,Pieces,PiecesInBoard,computerColor,7,AllMovedSteps);
        }else if(engineBox->currentText()=="迭代深化"){
            emit startIDAlphaBeta(Board,Pieces,PiecesInBoard,computerColor,30,AllMovedSteps);
        }
    }
}

//电脑下棋
void ChessBoard::computerGo(Step* step){
    if(!isGaming){
        return;
    }
    if(!isComputerGo){
        return;
    }
    //没有获取到步骤,则认输
    if(step==nullptr){
        isGaming=false;
        showMessage=computerColor?"红认输":"黑认输";
        return;
    }
    putChess(step);
    //是否胜利
    if(isWin()){
        isGaming=false;
        update();
        return;
    }
    //是否和棋
    if(AllMovedSteps.size()>300){
        isGaming=false;
        showMessage="和棋";
        return;
    }
    //检查重复局面
    int repeatFlag=checkRepeat();
    if(repeatFlag==1){
        isGaming=false;
        showMessage="和棋";
        return;
    }else if(repeatFlag==2){
        isGaming=false;
        showMessage="长将负";
        return;
    }else if(repeatFlag==3){
        isGaming=false;
        showMessage="长捉负";
        return;
    }
    //是否将军
    if(MC->Check(isRedTurn)){
        showMessage="将军";
    }else{
        showMessage="";
    }
    if(choiceBox->currentText()=="电脑对弈"){
        computerColor=!computerColor;
        QTimer::singleShot(100,this,SLOT(doComputerGo()));
    }
}

//下棋
void ChessBoard::putChess(Step *step){
    Board[step->Src_Position]=0;
    PiecesInBoard[step->Src_Position]=0;
    Pieces[step->Src_PiecesId]=step->Dst_Position;
    Board[step->Dst_Position]=step->Src_PiecesType;
    PiecesInBoard[step->Dst_Position]=step->Src_PiecesId;
    Pieces[step->Dst_PiecesId]=0;

    selectId=-1;
    isRedTurn=!isRedTurn;
    if(choiceBox->currentText()=="玩家先走"||choiceBox->currentText()=="电脑先走"){
        isComputerGo=!isComputerGo;
    }
    //更新Zobrist信息
    Zobrist=TT->getZobrist(isRedTurn);
    step->Zobrist=Zobrist;
    AllMovedSteps.append(step);
    showLastStep();//显示信息
    update();
}

//悔棋
void ChessBoard::goBack(){
    isGaming=true;
    if(AllMovedSteps.isEmpty()){
        return;
    }
    Step* step=AllMovedSteps.back();
    Board[step->Src_Position]=step->Src_PiecesType;
    PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
    Pieces[step->Src_PiecesId]=step->Src_Position;
    Board[step->Dst_Position]=step->Dst_PiecesType;
    PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
    if(step->Dst_PiecesId!=0){
        Pieces[step->Dst_PiecesId]=step->Dst_Position;
    }
    AllMovedSteps.removeLast();
    showStepsArea->undo();
    delete step;
    step=nullptr;
    isRedTurn=!isRedTurn;

    //如果是和电脑对弈，需退回两步
    if(choiceBox->currentText()=="玩家先走"||choiceBox->currentText()=="电脑先走"){
        if(AllMovedSteps.isEmpty()){
            return;
        }
        step=AllMovedSteps.back();
        Board[step->Src_Position]=step->Src_PiecesType;
        PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
        Pieces[step->Src_PiecesId]=step->Src_Position;
        Board[step->Dst_Position]=step->Dst_PiecesType;
        PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
        if(step->Dst_PiecesId!=0){
            Pieces[step->Dst_PiecesId]=step->Dst_Position;
        }
        AllMovedSteps.removeLast();
        showStepsArea->undo();
        delete step;
        step=nullptr;
        isRedTurn=!isRedTurn;
    }
    selectId=-1;
    update();
}

//电脑接替
void ChessBoard::computerRelay(){
    if(!isGaming){
        return;
    }
    if(isComputerGo){
        return;
    }
    if(choiceBox->currentText()=="玩家先走"||choiceBox->currentText()=="电脑先走"){
        choiceBox->setCurrentText("电脑对弈");
        computerColor=isRedTurn;//电脑颜色为当前颜色
        isComputerGo=true;
        QTimer::singleShot(100,this,SLOT(doComputerGo()));
    }else if(choiceBox->currentText()=="不用电脑"){
        if(computerColor){
            choiceBox->setCurrentText("电脑先走");
        }else{
            choiceBox->setCurrentText("玩家先走");
        }
        computerColor=isRedTurn;
        isComputerGo=true;
        QTimer::singleShot(100,this,SLOT(doComputerGo()));
    }
}

//暂停游戏
void ChessBoard::pauseGame(){
    isGaming=!isGaming;
    if(!isGaming){
        showMessage="暂停";
    }else{
        showMessage="";
    }
    if(isGaming&&isComputerGo){
        doComputerGo();
    }
    update();
}

//显示步骤
void ChessBoard::showLastStep(){
    Step* step = AllMovedSteps.back();
    QString s0;
    QString s1;
    QString s2;
    QString s3;
    QString s4;
    if(AllMovedSteps.size()%2!=0){
        int size=(AllMovedSteps.size()+1)/2;
        s0=QString::number(size)+".";
    }else{
        if((AllMovedSteps.size()+1)/2<10){
            s0="  ";
        }else if((AllMovedSteps.size()+1)/2<100){
            s0="   ";
        }else{
            s0="    ";
        }
    }
    //红棋走法记录
    if((step->Src_PiecesType&8)!=0){
        switch (step->Src_PiecesType) {
        case 8:s1="車";break;
        case 9:s1="马";break;
        case 10:s1="炮";break;
        case 11:s1="相";break;
        case 12:s1="仕";break;
        case 13:s1="兵";break;
        case 14:s1="帥";break;
        }
        switch (step->Src_Position%16-2) {
        case 1:s2="九";break;
        case 2:s2="八";break;
        case 3:s2="七";break;
        case 4:s2="六";break;
        case 5:s2="五";break;
        case 6:s2="四";break;
        case 7:s2="三";break;
        case 8:s2="二";break;
        case 9:s2="一";break;
        }
        //三四个字符分情况讨论
        //走直线棋子
        if(s1=="車"||s1=="炮"||s1=="兵"||s1=="帥"){
            if(step->Src_Position/16<step->Dst_Position/16){
                s3="退";
            }else if(step->Src_Position/16>step->Dst_Position/16){
                s3="进";
            }else if(step->Src_Position/16==step->Dst_Position/16){
                s3="平";
            }
            if(s3=="退"||s3=="进"){
                int pos1=step->Src_Position/16;
                int pos2=step->Dst_Position/16;
                int distance=pos1>pos2?(pos1-pos2):(pos2-pos1);
                switch (distance) {
                case 1:s4="一";break;
                case 2:s4="二";break;
                case 3:s4="三";break;
                case 4:s4="四";break;
                case 5:s4="五";break;
                case 6:s4="六";break;
                case 7:s4="七";break;
                case 8:s4="八";break;
                case 9:s4="九";break;
                }
            }
            if(s3=="平"){
                switch (step->Dst_Position%16-2) {
                case 1:s4="九";break;
                case 2:s4="八";break;
                case 3:s4="七";break;
                case 4:s4="六";break;
                case 5:s4="五";break;
                case 6:s4="四";break;
                case 7:s4="三";break;
                case 8:s4="二";break;
                case 9:s4="一";break;
                }
            }
        }
        //斜走棋子
        if(s1=="马"||s1=="相"||s1=="仕"){
            if(step->Src_Position/16<step->Dst_Position/16){
                s3="退";
            }else if(step->Src_Position/16>step->Dst_Position/16){
                s3="进";
            }
            switch (step->Dst_Position%16-2){
            case 1:s4="九";break;
            case 2:s4="八";break;
            case 3:s4="七";break;
            case 4:s4="六";break;
            case 5:s4="五";break;
            case 6:s4="四";break;
            case 7:s4="三";break;
            case 8:s4="二";break;
            case 9:s4="一";break;
            }
        }
    }
    //黑棋走法记录
    if((step->Src_PiecesType&16)!=0){
        switch (step->Src_PiecesType) {
        case 16:s1="車";break;
        case 17:s1="马";break;
        case 18:s1="炮";break;
        case 19:s1="象";break;
        case 20:s1="士";break;
        case 21:s1="卒";break;
        case 22:s1="将";break;
        }
        switch (step->Src_Position%16-2) {
        case 1:s2="1 ";break;
        case 2:s2="2 ";break;
        case 3:s2="3 ";break;
        case 4:s2="4 ";break;
        case 5:s2="5 ";break;
        case 6:s2="6 ";break;
        case 7:s2="7 ";break;
        case 8:s2="8 ";break;
        case 9:s2="9 ";break;
        }
        //三四个字符分情况讨论
        //走直线棋子
        if(s1=="車"||s1=="炮"||s1=="卒"||s1=="将"){
            if(step->Src_Position/16>step->Dst_Position/16){
                s3="退";
            }else if(step->Src_Position/16<step->Dst_Position/16){
                s3="进";
            }else if(step->Src_Position/16==step->Dst_Position/16){
                s3="平";
            }
            if(s3=="退"||s3=="进"){
                int pos1=step->Src_Position/16;
                int pos2=step->Dst_Position/16;
                int distance=pos1>pos2?(pos1-pos2):(pos2-pos1);
                switch (distance) {
                case 1:s4="1 ";break;
                case 2:s4="2 ";break;
                case 3:s4="3 ";break;
                case 4:s4="4 ";break;
                case 5:s4="5 ";break;
                case 6:s4="6 ";break;
                case 7:s4="7 ";break;
                case 8:s4="8 ";break;
                case 9:s4="9 ";break;
                }
            }
            if(s3=="平"){
                switch (step->Dst_Position%16-2) {
                case 1:s4="1 ";break;
                case 2:s4="2 ";break;
                case 3:s4="3 ";break;
                case 4:s4="4 ";break;
                case 5:s4="5 ";break;
                case 6:s4="6 ";break;
                case 7:s4="7 ";break;
                case 8:s4="8 ";break;
                case 9:s4="9 ";break;
                }
            }
        }
        //斜走棋子
        if(s1=="马"||s1=="象"||s1=="士"){
            if(step->Src_Position/16>step->Dst_Position/16){
                s3="退";
            }else if(step->Src_Position/16<step->Dst_Position/16){
                s3="进";
            }
            switch (step->Dst_Position%16-2){
            case 1:s4="1 ";break;
            case 2:s4="2 ";break;
            case 3:s4="3 ";break;
            case 4:s4="4 ";break;
            case 5:s4="5 ";break;
            case 6:s4="6 ";break;
            case 7:s4="7 ";break;
            case 8:s4="8 ";break;
            case 9:s4="9 ";break;
            }
        }
    }
    showStepsArea->append(s0+s1+s2+s3+s4);
}

//是否结束游戏
bool ChessBoard::isWin(){
    if(Pieces[31]==0){
        showMessage="黑棋胜";
        return true;
    }else if(Pieces[47]==0){
        showMessage="红棋胜";
        return true;
    }
    //走一步,如果有一步不被将军,则没有输
    bool flag=true;
    QVector<Step*> possibleSteps;
    if(!isRedTurn){
        MoveGeneratorB MVB = MoveGeneratorB(Board,Pieces,PiecesInBoard);
        MVB.getAllPossibleSteps(possibleSteps);
    }else{
        MoveGeneratorR MVR = MoveGeneratorR(Board,Pieces,PiecesInBoard);
        MVR.getAllPossibleSteps(possibleSteps);
    }
    for(auto iter=possibleSteps.begin();iter!=possibleSteps.end();iter++){
        Step* step=*iter;
        //走棋
        Board[step->Src_Position]=0;
        PiecesInBoard[step->Src_Position]=0;
        Pieces[step->Src_PiecesId]=step->Dst_Position;
        Board[step->Dst_Position]=step->Src_PiecesType;
        PiecesInBoard[step->Dst_Position]=step->Src_PiecesId;
        Pieces[step->Dst_PiecesId]=0;
        if(!MC->Check(isRedTurn)){
            flag=false;
            //撤回
            Board[step->Src_Position]=step->Src_PiecesType;
            PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
            Pieces[step->Src_PiecesId]=step->Src_Position;
            Board[step->Dst_Position]=step->Dst_PiecesType;
            PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
            if(step->Dst_PiecesId!=0){
                Pieces[step->Dst_PiecesId]=step->Dst_Position;
            }
            break;
        }
        //撤回
        Board[step->Src_Position]=step->Src_PiecesType;
        PiecesInBoard[step->Src_Position]=step->Src_PiecesId;
        Pieces[step->Src_PiecesId]=step->Src_Position;
        Board[step->Dst_Position]=step->Dst_PiecesType;
        PiecesInBoard[step->Dst_Position]=step->Dst_PiecesId;
        if(step->Dst_PiecesId!=0){
            Pieces[step->Dst_PiecesId]=step->Dst_Position;
        }
    }
    if(!possibleSteps.isEmpty()){
        qDeleteAll(possibleSteps);
    }
    if(flag){
        if(isRedTurn){
            showMessage="黑棋胜";
            return true;
        }else{
            showMessage="红棋胜";
            return true;
        }
    }else{
        return false;
    }
}

//检查重复局面
int ChessBoard::checkRepeat(){
    int size = AllMovedSteps.size();
    int checkNum=0;
    int returnType=0;//返回值类型
    if(size<30){
        return 0;
    }
    auto iter0 = AllMovedSteps.end()-2;
    for(int i=size;i!=size-20;i--){
        if((*iter0)->Zobrist==Zobrist){
            checkNum++;
        }
        iter0--;
    }
    //和棋
    if(checkNum>=5){
        return 1;
    }
    if(checkNum>=2){
        qDebug()<<"重复局面达到三次";
        //长将
        if(MC->Check(isRedTurn)){
            return 2;
        }
        Step* step = AllMovedSteps.back();
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
                case 17:returnType=3;break;
                case 18:returnType=3;break;
                case 21:returnType=3;break;
                }
            }
        }
        if(step->Src_PiecesType==16){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 9:returnType=3;break;
                case 10:returnType=3;break;
                case 13:returnType=3;break;
                }
            }
        }
        //马捉车炮兵
        if(step->Src_PiecesType==9){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 16:returnType=3;break;
                case 18:returnType=3;break;
                case 21:returnType=3;break;
                }
            }
        }
        if(step->Src_PiecesType==17){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 8:returnType=3;break;
                case 10:returnType=3;break;
                case 13:returnType=3;break;
                }
            }
        }
        //炮捉车马兵
        if(step->Src_PiecesType==10){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 16:returnType=3;break;
                case 17:returnType=3;break;
                case 21:returnType=3;break;
                }
            }
        }
        if(step->Src_PiecesType==18){
            for(auto iter=steps.begin();iter!=steps.end();iter++){
                switch ((*iter)->Dst_PiecesType) {
                case 8:returnType=3;break;
                case 9:returnType=3;break;
                case 13:returnType=3;break;
                }
            }
        }
        //释放内存
        if(!steps.isEmpty()){
            qDeleteAll(steps);
            steps.clear();
        }
    }
    return returnType;
}
