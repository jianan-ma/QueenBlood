#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QVector>
#include "cellunit.h"
#include "card.h"

enum class GAME_STATE {
    STATE_PLAYER_TURN,
    STATE_ENEMY_TURN,
    STATE_PLAYER_SELECT_CARD,
    STATE_PLAYER_SELECT_CELL,
    STATE_BATTLE,
    STATE_GAME_OVER
};

enum class CAMP_TURN {
    TURN_RED,
    TURN_BLUE
};

enum class GAME_RESULT {
    RED_WIN,
    BLUE_WIN,
    DRAW
};

class GameController : public QObject
{
    Q_OBJECT
public:
    explicit GameController(QObject *parent = nullptr);
    
    // 初始化战场
    void initBattleField(QVector<CellUnit*>& cells);
    
    // 选择卡牌准备放置
    void selectCard(Card* card);
    
    // 放置卡牌到单元格
    bool placeCard(CellUnit* targetCell);
    
    // 检查是否可以放置卡牌
    bool canPlaceCard(CellUnit* cell) const;
    // 检查是否有有效牌可操作
    bool canOperateTurn();
    // 获取当前状态
    GAME_STATE getGameState() const;
    
    // 获取当前回合
    CAMP_TURN getCurrentTurn() const;
    
    // 结束回合
    void endTurn();
    
    // 获取选中的卡牌
    Card* getSelectedCard() const;
    // 添加卡牌
    void addRedCard(Card* card);
    void addBlueCard(Card* card);
    
signals:
    void sig_gameStateChanged(GAME_STATE state);
    void sig_cellUpdated(CellUnit* cell);
    void sig_cardPlaced(CAMP_TURN,Card*);
    void sig_battleResult(CellUnit* attacker, CellUnit* defender, bool attackerWin);
    void sig_updateScore(int r1,int b1,int r2,int b2,int r3,int b3);
    void sig_result(GAME_RESULT result);
private:
    // 计算阵营扩张
    void calculateCampChange(CellUnit* placedCell);
    // 计算强化效果
    void calculateReinforcements(CellUnit* placedCell);
    // 计算得分
    void calculateRowsScore();
    // 检查并处理战斗
    void checkBattles(CellUnit* placedCell);
    
    // 执行战斗逻辑
    void executeBattle(CellUnit* attacker, CellUnit* defender);
    
    QVector<CellUnit*> m_cells;
    GAME_STATE m_gameState;
    CAMP_TURN m_currentTurn;
    Card* m_selectedCard;
    QVector<Card*> cards_red;
    QVector<Card*> cards_blue;
    int score_red1=0;
    int score_red2=0;
    int score_red3=0;
    int score_blue1=0;
    int score_blue2=0;
    int score_blue3=0;
    bool skipTurn=false;
};

#endif // GAMECONTROLLER_H
