#include "ArcadeGame.h"
#include <math.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <sstream>
#include <chrono>

using namespace std;

/* By standard, const floats cannot be defined in the header file, so it is done here. */
static const float s_kfSHOOT_COOLDOWN = 0.5;

/* Constructor */
ArcadeGame::ArcadeGame(sf::RenderWindow& rw):BaseArcade(rw)
{
	registerListener(this);

	setBackground("images/starfield1.png", 100);

	srand(time(NULL));

	loadTexture("images/ship.png", "shiptexture");
	loadTexture("images/comet.png", "comettexture");
	loadTexture("images/saucer.png", "saucertexture");
	loadTexture("images/bullet.png", "bullettexture");
	loadTexture("images/boss.png", "bosstexture");
	loadTexture("images/bossbullet.png", "bossbullettexture");

	for (int i = 0; i < s_kiNUM_SCORES_STORED; i++)
	{
		m_aiScores[i] = 0;
	}
	
	restartGame();
}

/* Returns the elapsed time since the start of the program. */
float ArcadeGame::getElapsedTime()
{
	using namespace std::chrono;
	static high_resolution_clock::time_point start = high_resolution_clock::now();

	return duration_cast<duration<float>>(start - high_resolution_clock::now()).count();
}

/* The main function. Cycled every "tick". */
/* Player input and game actions that should be repeated whilst in a particular state should (usually) go in here. */
void ArcadeGame::gameMain(std::string sKeyPressed)
{
	/* Check movement outside of Gamestate because it is almost always relevant. */
	if (m_pShip)
	{
		if (sKeyPressed.compare("RIGHT") == 0)
		{
			if (m_bCanMoveRight)
			{
				m_pShip->setVelocity(1, m_pShip->getVelocity().y, s_kiOBJECT_DEFAULT_SPEED);
			}
		}
		else if (sKeyPressed.compare("LEFT") == 0)
		{
			if (m_bCanMoveLeft)
			{
				m_pShip->setVelocity(-1, m_pShip->getVelocity().y, s_kiOBJECT_DEFAULT_SPEED);
			}
		}
		else if (sKeyPressed.compare("UP") == 0)
		{
			if (m_bCanMoveUp)
			{
				m_pShip->setVelocity(m_pShip->getVelocity().x, -1, s_kiOBJECT_DEFAULT_SPEED);
			}
		}
		else if (sKeyPressed.compare("DOWN") == 0)
		{
			if (m_bCanMoveDown)
			{
				m_pShip->setVelocity(m_pShip->getVelocity().x, 1, s_kiOBJECT_DEFAULT_SPEED);
			}	
		}
		else if (sKeyPressed.compare("SPACE") == 0)
		{
			if (m_bCanShoot)
			{
				spawnBullet();
				modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, false);
				createAlarm(ArcadeGame::Alarms::SHOT_FIRED, s_kfSHOOT_COOLDOWN);
			}
		}
	}

	/* Gamestate management. */
	if (m_GameState == GameState::INTRODUCTION)
	{
		setFontSize(50);
		createMessage(("S C R A M B L E"), 230, 100);
	}
	else if (m_GameState == GameState::INTERVAL)
	{
	}
	else if (m_GameState == GameState::COMET)
	{
		/* Spawns Comets (If able). */
		if (m_iNumComets < s_kiMAX_COMETS)
		{
			if (!alarmIsActive(ArcadeGame::Alarms::SPAWN_COMET))
			{
				createAlarm(ArcadeGame::Alarms::SPAWN_COMET, 0.8);
			}
		}
	}
	else if (m_GameState == GameState::SAUCER)
	{
		/* Auto-correct position. */
		if (!isBetween(290, 310, m_pShip->getPosition().y))
		{
			if (m_pShip->getPosition().y > 310 )
			{
				m_pShip->setVelocity(m_pShip->getVelocity().x, -1, s_kiOBJECT_DEFAULT_SPEED);
			}
			else if (m_pShip->getPosition().y < 290)
			{
				m_pShip->setVelocity(m_pShip->getVelocity().x, 1, s_kiOBJECT_DEFAULT_SPEED);
			}
		}

		/* Stop auto-correct when position is valid. */
		if (isBetween(290, 310, m_pShip->getPosition().y) && m_pShip->getVelocity().y != 0)
		{	
			m_pShip->setVelocity(m_pShip->getVelocity().x, 0, s_kiOBJECT_DEFAULT_SPEED);
		}
		
		/* Spawns Saucers in groups of 4 (If able). */
		if (m_iNumSaucers < (s_kiMAX_SAUCERS - 3))
		{
			if (!alarmIsActive(ArcadeGame::Alarms::SPAWN_SAUCER))
			{
				createAlarm(ArcadeGame::Alarms::SPAWN_SAUCER, 4.25);
			}
		}
		
		/* Apply the sin wave pathing for all active Saucers. */
		for (int i = 0; i < getNumGameObjects(); i++)
		{
			if(getGameObject(i)->getObjectType() == "saucer")
			{
				sf::Vector2f position = getGameObject(i)->getPosition();
				position.y = (sin((1300 - position.x) / 100) * 200) + 300;
				getGameObject(i)->setPosition(position);
			}
		}
	}
	else if (m_GameState == GameState::BOSS)
	{
		/* Auto-correct position. */
		if (!isBetween(40, 90, m_pShip->getPosition().x))
		{
			if (m_pShip->getPosition().x > 90)
			{
				m_pShip->setVelocity(-1, m_pShip->getVelocity().y, s_kiOBJECT_DEFAULT_SPEED);
			}
			else if (m_pShip->getPosition().x < 40)
			{
				m_pShip->setVelocity(1, m_pShip->getVelocity().y, s_kiOBJECT_DEFAULT_SPEED);
			}
		}

		/* Stop auto-correct when position is valid. */
		if (isBetween(40, 90, m_pShip->getPosition().x) && m_pShip->getVelocity().x != 0)
		{
			m_pShip->setVelocity(0, m_pShip->getVelocity().y, s_kiOBJECT_DEFAULT_SPEED);
		}
	}
	else if (m_GameState == GameState::SCOREBOARD)
	{
		setFontSize(30);
		createMessage("Score this round: " + convertIntToString(m_iScore), 245, 50); 
		createMessage("Highscores", 320, 100); 	
		for (int i = 0; i < s_kiNUM_SCORES_STORED; i++)
		{
			createMessage((convertIntToString(i + 1) + ")                " + convertIntToString(m_aiScores[i]) + " points"), 270, 160 + i * 30);
		}
		createMessage(("Press 'R' to restart the game"), 220, 500);

		if (sKeyPressed.compare("R") == 0)
		{
			restartGame();
		}
	}

	// leave this line of code here, last in the function.
	BaseArcade::gameMain(sKeyPressed);
}

/* Creates a Ship GameObject, setting up important parameters where necessary. */
void ArcadeGame::spawnShip()
{
	Texture* pTexture = getTexture("shiptexture");
	sf::IntRect* areaToLoad = new IntRect(0, 0, 79, 30);
	pTexture->loadFromFile("images/ship.png", *areaToLoad);
	m_pShip = new GameObject(pTexture, "ship");
	m_pShip->setPosition(50, 300);
	m_pShip->setVelocity(0, 0, s_kiOBJECT_DEFAULT_SPEED);
	m_pShip->setStayOnScreen(true);
	m_pShip->setAliveZone(-10, -10, 820, 620);
	addGameObject(m_pShip);
}

/* Creates a Boss GameObject, setting up important parameters where necessary. */
void ArcadeGame::spawnBoss()
{
	m_iBossHealth = 20;
	m_bBossIsVulnerable = false;
	Texture* pTexture = getTexture("bosstexture");
	sf::IntRect* areaToLoad = new IntRect(75, 0, 75, 600);
	pTexture->loadFromFile("images/boss.png", *areaToLoad);
	GameObject* boss = new GameObject(pTexture, "boss");
	boss->setPosition(770, 300);
	boss->setStayOnScreen(false);
	boss->setSolid(true);
	addGameObject(boss);
}

/* Creates a BossBullet GameObject, setting up important parameters where necessary. */
/* Boss bullets are offset when they are created to achieve a "wave" effect. */
void ArcadeGame::spawnBossBullet(int iXOffset, int iYPosition)
{
	if (m_iBossHealth > 0)
	{
		Texture* pTexture = getTexture("bossbullettexture");
		GameObject* bullet = new GameObject(pTexture, "bossbullet");
		bullet->setPosition(730 + iXOffset, iYPosition);
		bullet->setVelocity(-1, 0, (s_kiBULLET_SPEED * m_fDifficulty));
		bullet->setStayOnScreen(false);
		bullet->setAliveZone(-100, -100, 1000, 800);
		addGameObject(bullet);
	}		
}

/* Creates a Saucer GameObject, setting up important parameters where necessary. */
/* Saucers are offset when they are created to achieve a "line" effect. */
void ArcadeGame::spawnSaucer(int iXPositionOffset)
{
	if (m_GameState == GameState::SAUCER)
	{
		Texture* pTexture = getTexture("saucertexture");
		GameObject* m_pSaucer = new GameObject(pTexture, "saucer");
		m_pSaucer->setPosition(900 + iXPositionOffset, 300);
		m_pSaucer->setVelocity(-1, 0, (s_kiSAUCER_SPEED * m_fDifficulty));
		m_pSaucer->setStayOnScreen(false);
		m_pSaucer->setAliveZone(-100, -100, 1100 + iXPositionOffset, 800);
		addGameObject(m_pSaucer);	
		m_iNumSaucers++;
	}
}

/* Creates a Comet GameObject, setting up important parameters where necessary. */
void ArcadeGame::spawnComet()
{
	if (m_GameState == GameState::COMET)
	{
		Texture* pTexture = getTexture("comettexture");
		GameObject* comet = new GameObject(pTexture, "comet");
		comet->setPosition(900, getRandom(600));
		comet->setVelocity(-1, 0, (s_kiCOMET_SPEED * m_fDifficulty));
		comet->setStayOnScreen(false);
		comet->setAliveZone(-100, -100, 1100, 800);
		addGameObject(comet);	
		m_iNumComets++;
	}
}

/* Creates a Bullet GameObject, setting up important parameters where necessary. */
void ArcadeGame::spawnBullet()
{
	Texture* pTexture = getTexture("bullettexture");
	GameObject* bullet = new GameObject(pTexture, "bullet");
	bullet->setPosition(m_pShip->getPosition().x + 40, m_pShip->getPosition().y);
	bullet->setVelocity(1, 0, s_kiBULLET_SPEED);
	bullet->setStayOnScreen(false);
	bullet->setAliveZone(-100, -100, 1000, 800);
	addGameObject(bullet);	
}

/* Creates a HealthIndicator GameObject, setting up important parameters where necessary. */
/* HealthIndicators use their index to position themselves correctly. */
void ArcadeGame::spawnHealthIndicator(int iIndex)
{
	Texture* pTexture = getTexture("shiptexture");
	GameObject* healthIndic = new GameObject(pTexture, "healthIndicator");
	healthIndic->setPosition(38 + (73 * iIndex), 35);
	healthIndic->setSolid(false);
	addGameObject(healthIndic);	
}

/* Restarts the game, reverting all variables back to their default states excluding the Highscores array. */
void ArcadeGame::restartGame()
{
	clearMessages();

	for (int i = 0; i < getNumGameObjects(); i++)
	{
		removeGameObject(getGameObject(i));
	}

	modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, true);
	modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, true);
	modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, true);
	modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, true);
	modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, true);
	modifyPlayerFlag(ArcadeGame::Flags::CAN_TAKE_DAMAGE, true);

	m_PreviousGameState = GameState::INTRODUCTION;
	changeGameState(GameState::INTRODUCTION);

	m_fDifficulty = 1.0;
	m_iScore = 0;
	m_iPlayerHealth = 5;
	m_iBossHealth = 20;
	m_iNumSaucers = 0;
	m_iNumComets = 0;

	m_ActiveAlarms.clear();
	m_GameStartTime = time(&m_GameStartTime);

	drawHealth();
	revivePlayer();
}

/* Ends the game and moves the game on to the Scoreboard stage. */
void ArcadeGame::endGame()
{
	changeGameState(GameState::SCOREBOARD);
	time_t gameEndTime = time(&gameEndTime);
	modifyPlayerScore((gameEndTime - m_GameStartTime) * s_kiPOINTS_PER_SECOND);
	commitScore();
	cancelAlarms();
}

/* Add the score to the high scores list */
void ArcadeGame::commitScore()
{
	if (m_iScore > 0 && m_iScore > lowestValue(m_aiScores, s_kiNUM_SCORES_STORED))
	{
		m_aiScores[(s_kiNUM_SCORES_STORED - 1)] = m_iScore;
	}
	bubbleSortScores();
}

/* Called at the start of a new stage. */
/* Does any set-up work required for the newly starting stage. */
void ArcadeGame::initialiseStage()
{
	switch (m_GameState)
	{
	case GameState::INTRODUCTION:
		createAlarm(ArcadeGame::Alarms::INTRO_STAGE_DURATION, s_kiINTRO_STAGE_DURATION);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_TAKE_DAMAGE, true);		
		break;
	case GameState::INTERVAL:
		createAlarm(ArcadeGame::Alarms::INTERVAL_STAGE_DURATION, s_kiINTERVAL_STAGE_DURATION);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, true);
		break;	
	case GameState::SAUCER:
		createAlarm(ArcadeGame::Alarms::SAUCER_STAGE_DURATION, s_kiSAUCER_STAGE_DURATION);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_TAKE_DAMAGE, true);
		break;
	case GameState::COMET:
		createAlarm(ArcadeGame::Alarms::COMET_STAGE_DURATION, s_kiCOMET_STAGE_DURATION);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_TAKE_DAMAGE, true);
		break;
	case GameState::BOSS:
		createAlarm(ArcadeGame::Alarms::BOSS_VULNERABILITY, 5);
		createAlarm(ArcadeGame::Alarms::BOSS_ATTACK, 4);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, true);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_TAKE_DAMAGE, true);
		spawnBoss();
		break;
	case GameState::SCOREBOARD:
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_LEFT, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_RIGHT, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_UP, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_MOVE_DOWN, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_SHOOT, false);
		modifyPlayerFlag(ArcadeGame::Flags::CAN_TAKE_DAMAGE, false);
		for (int i = 0; i < getNumGameObjects(); i++)
		{
			removeGameObject(getGameObject(i));
		}
		break;
	}
}

/* Called when a new stage is starting to "clean up" the last stage. */
/* Most entries are left blank to indicate that they exist but nothing will happen. */
void ArcadeGame::finishStage()
{
	switch (m_GameState)
	{
	case GameState::INTRODUCTION:
		break;
	case GameState::INTERVAL:
		break;
	case GameState::COMET:
		break;
	case GameState::SAUCER:
		break;
	case GameState::BOSS:
		m_fDifficulty = m_fDifficulty + 0.2;
		break;
	case GameState::SCOREBOARD:
		break;
	}
}

/* Simulates an attack by the boss in a "bullet wave" style. */
/* Is constantly called by the expirey of a boss bullet wave timer during the boss stage. */
void ArcadeGame::bossAttack()
{
	int iAttackSpace = getRandom(3);
	if (iAttackSpace == 1)
	{
		iAttackSpace = 5;
	}
	else if (iAttackSpace == 2)
	{
		iAttackSpace = 205;
	}
	else if (iAttackSpace == 3)
	{
		iAttackSpace = 405;
	}
	for (int i = 0; i < 9; i++)
	{
		spawnBossBullet(i * 15, iAttackSpace + (i * 22));
	}
}

/* Changes the GameState to the newly specified GameState. The previous GameState is passed down. */
/* This functions stage intialisation and finishing whether it is necessary or not. */
void ArcadeGame::changeGameState(ArcadeGame::GameState newGameState)
{
	finishStage();
	m_PreviousGameState = m_GameState;
	m_GameState = newGameState;
	initialiseStage();
}

/* Returns true if the Alarm exists in the ActiveAlarms array and is set to true. */
bool ArcadeGame::alarmIsActive(Alarms alarm)
{
	if (m_ActiveAlarms[alarm] == true)
	{
		return true;
	}
	return false;
}

/* Removes an alarm from the ActiveAlarms list, allowing it to be created once again. */
void ArcadeGame::removeAlarm(Alarms alarm)
{
	m_ActiveAlarms.erase(alarm);
}

/* Used for creating alarms with the provided parameters. Alarms are added to a list of active alarms. */
/* Alarms that do not match a pre-defined alarm name are not created (Prevents miss-spellings). */
/* Alarms that already exist are not created. */
void ArcadeGame::createAlarm(Alarms alarm, float fAlarmTime)
{
	if (!alarmIsActive(alarm))
	{
		switch (alarm)
		{
		case ArcadeGame::Alarms::COMET_STAGE_DURATION:
			setAlarm("CometStageDuration", fAlarmTime);	
			break;
		case ArcadeGame::Alarms::SAUCER_STAGE_DURATION:
			setAlarm("SaucerStageDuration", fAlarmTime);
			break;
		case ArcadeGame::Alarms::INTERVAL_STAGE_DURATION:
			setAlarm("IntervalStageDuration", fAlarmTime);
			break;
		case ArcadeGame::Alarms::INTRO_STAGE_DURATION:
			setAlarm("IntroStageDuration", fAlarmTime);
			break;
		case ArcadeGame::Alarms::SHOT_FIRED:
			setAlarm("ShotFired", fAlarmTime);
			break;
		case ArcadeGame::Alarms::SPAWN_COMET:
			setAlarm("SpawnComet", fAlarmTime);
			break;
		case ArcadeGame::Alarms::SPAWN_SAUCER:
			setAlarm("SpawnSaucer", fAlarmTime);
			break;
		case ArcadeGame::Alarms::REVIVE_IMMUNITY:
			setAlarm("ReviveImmunity", fAlarmTime);
			break;
		case ArcadeGame::Alarms::BOSS_VULNERABILITY:
			setAlarm("BossVulnerability", fAlarmTime);
			break;
		case ArcadeGame::Alarms::BOSS_ATTACK:
			setAlarm("BossAttack", fAlarmTime);
			break;
		case ArcadeGame::Alarms::BOSS_DEATH:
			setAlarm("BossDeath", fAlarmTime);
			break;
		}
		m_ActiveAlarms[alarm] = true;
	}
}

/* EVENT: Fired when an alarm completes. */
void ArcadeGame::alarmComplete(string sAlarmID)
{
	if (sAlarmID == "ShotFired")
	{
		m_bCanShoot = true;
		removeAlarm(Alarms::SHOT_FIRED);
	}
	else if (sAlarmID == "SpawnComet")
	{
		spawnComet();
		removeAlarm(Alarms::SPAWN_COMET);
	}
	else if (sAlarmID == "BossVulnerability")
	{
		m_bBossIsVulnerable = !m_bBossIsVulnerable;
		removeAlarm(Alarms::BOSS_VULNERABILITY);
		animateBoss();
		if (m_GameState == GameState::BOSS)
		{
			createAlarm(ArcadeGame::Alarms::BOSS_VULNERABILITY, 5);
		}
	}
	else if (sAlarmID == "BossAttack")
	{
		bossAttack();
		removeAlarm(Alarms::BOSS_ATTACK);
		if (m_GameState == GameState::BOSS)
		{
			createAlarm(ArcadeGame::Alarms::BOSS_ATTACK, 2.5);
		}
	}
	else if (sAlarmID == "BossDeath")
	{
		removeGameObjectsOfType("boss");
		changeGameState(GameState::INTERVAL);
		removeAlarm(Alarms::BOSS_DEATH);
	}
	else if (sAlarmID == "SpawnSaucer")
	{
		spawnSaucer(0);
		spawnSaucer(40);
		spawnSaucer(80);
		spawnSaucer(120);
		removeAlarm(Alarms::SPAWN_SAUCER);
	}
	else if (sAlarmID == "ReviveImmunity")
	{
		modifyPlayerFlag(Flags::CAN_TAKE_DAMAGE, true);
		removeAlarm(Alarms::REVIVE_IMMUNITY);
	}
	else if (sAlarmID == "IntervalStageDuration")
	{
		if (m_PreviousGameState == GameState::COMET)
		{
			changeGameState(GameState::SAUCER);
		}
		else if (m_PreviousGameState == GameState::SAUCER)
		{
			changeGameState(GameState::BOSS);
		}
		else if (m_PreviousGameState == GameState::BOSS)
		{
			changeGameState(GameState::COMET);
		}
		else if (m_PreviousGameState == GameState::INTRODUCTION)
		{
			changeGameState(GameState::COMET);
		}
		removeAlarm(Alarms::INTERVAL_STAGE_DURATION);
	}
	else if (sAlarmID == "CometStageDuration")
	{
		changeGameState(GameState::INTERVAL);
		removeAlarm(Alarms::COMET_STAGE_DURATION);
	}
	else if (sAlarmID == "SaucerStageDuration")
	{
		changeGameState(GameState::INTERVAL);
		removeAlarm(Alarms::SAUCER_STAGE_DURATION);
	}
	else if (sAlarmID == "IntroStageDuration")
	{
		changeGameState(GameState::INTERVAL);
		removeAlarm(Alarms::INTRO_STAGE_DURATION);
	}
}

/* Updates the boss's animation frame based on its status. */
void ArcadeGame::animateBoss()
{
	if (getGameObject("boss"))
	{
		if (isBetween(11, 21, m_iBossHealth))
		{
			if (m_bBossIsVulnerable == true)
			{
				Texture* pTexture = getTexture("bosstexture");
				sf::IntRect* areaToLoad = new IntRect(0, 0, 75, 600);
				pTexture->loadFromFile("images/boss.png", *areaToLoad);			
				getGameObject("boss")->setTextureRect(*areaToLoad);
			}
			else
			{
				Texture* pTexture = getTexture("bosstexture");
				sf::IntRect* areaToLoad = new IntRect(75, 0, 300, 600);
				pTexture->loadFromFile("images/boss.png", *areaToLoad);	
				getGameObject("boss")->setTextureRect(*areaToLoad);
			}
		}
		else if (isBetween(0, 11, m_iBossHealth))
		{
			if (m_bBossIsVulnerable == true)
			{
				Texture* pTexture = getTexture("bosstexture");
				sf::IntRect* areaToLoad = new IntRect(150, 0, 300, 600);
				pTexture->loadFromFile("images/boss.png", *areaToLoad);	
				getGameObject("boss")->setTextureRect(*areaToLoad);
			}
			else
			{
				Texture* pTexture = getTexture("bosstexture");
				sf::IntRect* areaToLoad = new IntRect(225, 0, 300, 600);
				pTexture->loadFromFile("images/boss.png", *areaToLoad);
				getGameObject("boss")->setTextureRect(*areaToLoad);
			}
		}
		if (!hasHealthRemaining("boss"))
		{
			Texture* pTexture = getTexture("bosstexture");
			sf::IntRect* areaToLoad = new IntRect(300, 0, 75, 600);
			pTexture->loadFromFile("images/boss.png", *areaToLoad);	
			getGameObject("boss")->setTextureRect(*areaToLoad);
		}
	}
}

/* Draws health indicators to the screen as GameObjects. */
void ArcadeGame::drawHealth()
{
	removeGameObjectsOfType("healthIndicator");
	for (int i = 0; i < m_iPlayerHealth; i++)
	{
		spawnHealthIndicator(i);
	}
}

void ArcadeGame::render()
{
	BaseArcade::render();

	// any additional rendering you want to do add below this comment.
}

/* Instantly moves a GameObject outside of its alive zone to force it to die. */
void ArcadeGame::killGameObject(GameObject* pGO)
{
	pGO->setSolid(false);
	pGO->setPosition(pGO->getAliveZone().left - 100, pGO->getAliveZone().top - 100);
}

/* EVENT: Fired when a GameObject is deleted. */
void ArcadeGame::objectDeleted(GameObject* pGO)
{
	if (pGO->getObjectType() == "comet")												
	{
		m_iNumComets--;
	}
	else if (pGO->getObjectType() == "saucer")
	{
		m_iNumSaucers--;
	}
}

/* EVENT: Fired when two GameObjects collide. */
void ArcadeGame::collisionEvent(GameObject* pGO1, GameObject* pGO2)
{
	if (collisionWasBetween(pGO1, pGO2, "comet", "ship"))
	{	
		if (m_bCanTakeDamage)
		{
			killGameObject(selectGOType("ship", pGO1, pGO2));
			modifyPlayerHealth(-1);
			if (hasHealthRemaining("player"))
			{
				revivePlayer();
			}
			else
			{
				endGame();
			}
		}
		else
		{
			killGameObject(selectGOType("comet", pGO1, pGO2));
		}
	}
	else if (collisionWasBetween(pGO1, pGO2, "saucer", "ship"))
	{	
		if (m_bCanTakeDamage)
		{
			killGameObject(pGO1);
			killGameObject(pGO2);
			modifyPlayerHealth(-1);
			if (hasHealthRemaining("player"))
			{
				revivePlayer();
			}
			else
			{
				endGame();
			}
		}
		else
		{
			killGameObject(selectGOType("saucer", pGO1, pGO2));	
		}	
	}
	else if (collisionWasBetween(pGO1, pGO2, "saucer", "bullet"))
	{
		killGameObject(pGO1);
		killGameObject(pGO2);
		modifyPlayerScore(s_kiPOINTS_PER_SAUCER_KILL);
	}
	else if (collisionWasBetween(pGO1, pGO2, "comet", "bullet"))
	{
		killGameObject(selectGOType("bullet", pGO1, pGO2));
	}
	else if (collisionWasBetween(pGO1, pGO2, "boss", "bullet"))
	{
		if (isBetween(270, 330, selectGOType("bullet", pGO1, pGO2)->getPosition().y) && m_bBossIsVulnerable)
		{
			m_iBossHealth -=1;
			animateBoss();
			if (!hasHealthRemaining("boss"))
			{
				createAlarm(Alarms::BOSS_DEATH, 2);
			}
		}
		killGameObject(selectGOType("bullet", pGO1, pGO2));
	}
	else if (collisionWasBetween(pGO1, pGO2, "bossbullet", "bullet"))
	{
		killGameObject(pGO1);
		killGameObject(pGO2);
	}
	else if (collisionWasBetween(pGO1, pGO2, "bossbullet", "ship"))
	{
		killGameObject(selectGOType("bossbullet", pGO1, pGO2));
		if (m_bCanTakeDamage)
		{
			killGameObject(selectGOType("ship", pGO1, pGO2));
			modifyPlayerHealth(-1);
			if (hasHealthRemaining("player"))
			{
				revivePlayer();
			}
			else
			{
				endGame();
			}
		}
	}
}

/* Returns true if the listed object types were included in the collision between the given GameObjects. */
/* Again, this is mostly a readability method since it is just as easy to use many if-statements in the collision detection function. */
bool ArcadeGame::collisionWasBetween(GameObject* pGO1, GameObject* pGO2, string sObjectType1, string sObjectType2)
{
	if (pGO1->getObjectType() == sObjectType1 || pGO1->getObjectType() == sObjectType2)
	{
		if (pGO2->getObjectType() == sObjectType1 || pGO2->getObjectType() == sObjectType2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/* Modifies the score of the player*/
void ArcadeGame::modifyPlayerScore(int iModification)
{
	m_iScore = m_iScore + iModification;
}

/* Modifies the health of the player. */
void ArcadeGame::modifyPlayerHealth(int iModification)
{
	m_iPlayerHealth = m_iPlayerHealth + iModification;
	drawHealth();
}

/* Modifies one of the players status flags. */
/* This function is not absolutely necessary, but does provide a much cleaner code environment. One centralised location for flag modification is much easier 
   than adjusting player flags all over the place. */
void ArcadeGame::modifyPlayerFlag(ArcadeGame::Flags flag, bool bEnable)
{
	switch (flag)
	{
	case ArcadeGame::Flags::CAN_MOVE_LEFT:
		m_bCanMoveLeft = bEnable;
		break;
	case ArcadeGame::Flags::CAN_MOVE_RIGHT:
		m_bCanMoveRight = bEnable;
		break;
	case ArcadeGame::Flags::CAN_MOVE_UP:
		m_bCanMoveUp = bEnable;
		break;
	case ArcadeGame::Flags::CAN_MOVE_DOWN:
		m_bCanMoveDown = bEnable;
		break;
	case ArcadeGame::Flags::CAN_SHOOT:
		m_bCanShoot = bEnable;
		break;
	case ArcadeGame::Flags::CAN_TAKE_DAMAGE:
		m_bCanTakeDamage = bEnable;
		break;
	}
}

/* Revives the player and triggers temporary immunity to damage. */
void ArcadeGame::revivePlayer()
{
	spawnShip();
	modifyPlayerFlag(Flags::CAN_TAKE_DAMAGE, false);
	createAlarm(ArcadeGame::Alarms::REVIVE_IMMUNITY, s_kiREVIVE_IMMUNITY_DURATION);
}

/* Returns true if the given unit has health remaining.*/
/* This function only applies to "special" unit types that have a defined health variable. */
bool ArcadeGame::hasHealthRemaining(std::string sUnit)
{
	if (sUnit == "player")
	{
		if (m_iPlayerHealth > 0)
		{
			return true;
		}
	}
	else if (sUnit == "boss")
	{
		if (m_iBossHealth > 0)
		{
			return true;
		}
	}
	return false;
}

/* Returns the game object with the specified type. */
/* Note: Will cause a crash if one of the GameObjects is inaccessible. */
GameObject* ArcadeGame::selectGOType(std::string sGOType, GameObject* pGO1, GameObject* pGO2)
{
	if (pGO1->getObjectType() == sGOType)
	{
		return pGO1;
	}
	else
	{
		return pGO2;
	}
}

/* Performs a bubble sort on the scores array. */
void ArcadeGame::bubbleSortScores()
{
	bool bHasSwapped;
	do
	{
		bHasSwapped = false;
		for (int i = 0; i < (s_kiNUM_SCORES_STORED - 1); i++)
		{
			if (m_aiScores[i] < m_aiScores[(i + 1)])
			{
				bHasSwapped = true;
				int iTempVal = m_aiScores[i];
				m_aiScores[i] = m_aiScores[(i + 1)];
				m_aiScores[(i + 1)] = iTempVal;
			}
		}
	}
	while (bHasSwapped);
}

/* ===== General utility functions ===== */

/* Returns the lowest integer value found in the array aiValues. Returns the first value by default. */
int ArcadeGame::lowestValue(int aiValues[], int iArraySize)
{
	int iLowestValue = aiValues[0];
	for (int i = 0; i < iArraySize; i++)
	{
		if (iLowestValue > aiValues[i])
		{
			iLowestValue = aiValues[i];
		}
	}
	return iLowestValue;
}

/* Returns iNumber as a string. */
string ArcadeGame::convertIntToString(int iNumber)
{
		ostringstream ss;
		ss << iNumber;
		return ss.str();
}

/* Returns a random number between 1 and iMaxValue (Inclusive). */
int ArcadeGame::getRandom(int iMaxValue)
{
	return rand() % iMaxValue + 1;
}

/* Returns true if iValue is between iLower and iUpper (Exclusive). */
bool ArcadeGame::isBetween(int iLower, int iUpper, int iValue)
{
	return (iLower <iValue && iValue < iUpper);
}
