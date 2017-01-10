#ifndef ARCADE_G_H
#define ARCADE_G_H

#include "BaseArcade.h"

#define PI 3.142

class ArcadeGame: public BaseArcade
{
public:
	static enum GameState {INTRODUCTION, INTERVAL, COMET, SAUCER, BOSS, SCOREBOARD};

	static const int s_kiOBJECT_DEFAULT_SPEED = 200;

	ArcadeGame(sf::RenderWindow& rw);

	void alarmComplete(std::string sAlarmID);
	void gameMain(std::string sKeyPressed);
	void collisionEvent(GameObject* pGO1, GameObject* pGO2);
	void objectDeleted(GameObject* pGO);
	/*!
	This derived class function exists only for additional rendering not dealt with by BaseArcade::render().
	BaseArcade::render() renders only GameObjects and text. Other rendering needs to go in this function.
	*/
	void render();

private:
	/* Private constants */
	static const int s_kiINTRO_STAGE_DURATION = 5;
	static const int s_kiINTERVAL_STAGE_DURATION = 5;
	static const int s_kiSAUCER_STAGE_DURATION = 30;
	static const int s_kiCOMET_STAGE_DURATION = 30;
	static const int s_kiREVIVE_IMMUNITY_DURATION = 3;
	static const int s_kiPOINTS_PER_SECOND = 20;
	static const int s_kiPOINTS_PER_SAUCER_KILL = 20;
	static const int s_kiMAX_COMETS = 10;
	static const int s_kiMAX_SAUCERS = 12;
	
	static const int s_kiSAUCER_SPEED = 100;
	static const int s_kiCOMET_SPEED = 150;
	static const int s_kiBULLET_SPEED = 400;

	static const int s_kiNUM_SCORES_STORED = 8;

	static enum Flags {CAN_MOVE_LEFT, CAN_MOVE_RIGHT, CAN_MOVE_UP, CAN_MOVE_DOWN, CAN_SHOOT, CAN_TAKE_DAMAGE};
	static enum Alarms {SHOT_FIRED, INTRO_STAGE_DURATION, INTERVAL_STAGE_DURATION, COMET_STAGE_DURATION, 
						SAUCER_STAGE_DURATION, REVIVE_IMMUNITY, SPAWN_COMET, SPAWN_SAUCER, BOSS_VULNERABILITY, 
						BOSS_ATTACK, BOSS_DEATH};

	/* Private variables */
	bool m_bCanMoveUp;
	bool m_bCanMoveLeft;
	bool m_bCanMoveDown;
	bool m_bCanMoveRight;
	bool m_bCanShoot;
	bool m_bCanTakeDamage;

	int m_aiScores[8];
	std::map<int, bool> m_ActiveAlarms;
	time_t m_GameStartTime;
	float m_fDifficulty;
	int m_iNumSaucers;
	int m_iNumComets;

	int m_iPlayerHealth;
	int m_iScore;
	int m_iBossHealth;
	bool m_bBossIsVulnerable;

	GameObject* m_pShip;
	GameState m_GameState;
	GameState m_PreviousGameState;

	/* Private functions */
	void restartGame();
	void changeGameState(ArcadeGame::GameState newGameState);
	void killGameObject(GameObject* pGO);
	bool collisionWasBetween(GameObject* pGO1, GameObject* pGO2, std::string sGOType1, std::string sGOType2);
	void modifyPlayerScore(int iModification);
	void modifyPlayerHealth(int iModification);
	void modifyPlayerFlag(Flags flag, bool bEnabled);
	void drawHealth();
	void spawnShip();
	void spawnSaucer(int iXPositionOffset);
	void spawnComet();
	void spawnBullet();
	void spawnBoss();
	void spawnBossBullet(int iXOffset, int iYPosition);
	void spawnHealthIndicator(int iIndex);
	void bossAttack();
	void revivePlayer();
	bool hasHealthRemaining(std::string sUnit);
	void endGame();
	void initialiseStage();
	void finishStage();
	void createAlarm(Alarms alarm, float fAlarmTime);
	bool alarmIsActive(Alarms alarm);
	void removeAlarm(Alarms alarm);
	GameObject* selectGOType(std::string sGOType, GameObject* pGO1, GameObject* pGO2);
	void commitScore();
	void bubbleSortScores();
	float getElapsedTime();
	void animateBoss();

	/* General utility functions */
	int getRandom(int iMaxValue);
	std::string convertIntToString(int iNumber);
	bool isBetween(int iLower, int iUpper, int iValue);
	int lowestValue(int aiValues[], int iArraySize);
};

#endif