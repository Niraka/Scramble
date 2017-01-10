#ifndef BASE_ARCADE_H
#define BASE_ARCADE_H

#include <ctime>
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include <vector>
#include "GameObject.h"
#include "SFML/system/clock.hpp"

using namespace sf;


//! The BaseArcade class

/*!
This is the main engine class.
*/


class BaseArcade
{
public:
	//! screen width
	static const int SCREEN_WIDTH = 800;
	//! screen height
	static const int SCREEN_HEIGHT = 600;

	//! BaseArcade constructor. Called by main().
	/*!
	\param rw the sf:RenderWindow.
	*/
	BaseArcade(sf::RenderWindow& rw);

	//! Timing function. Starts the frame. Called by main().
	bool startFrame();
	
	//! The main game control function. Called by main().
	/*! 
	\param sKeyPressed a string containing the key press code, if a key has been pressed.
	*/
	virtual void gameMain(std::string sKeyPressed);
	//! The render function.  Called by main().
	/*!
	This is the main rendering function.
	*/
	virtual void render();

protected:
	//! The maximum number of alarms that can exist at any one moment.
	static const int MAX_ALARMS = 32;

	//! Register a message 'listener'
	/*!
	This function registers an object that will receive messages about events in the game.
	Events include collisions between objects and alarms. When these events occur, other
	functions will be called, as detailed elsewhere in this API.
	\param pListener the object that will receive the messages.
	*/
	void registerListener(BaseArcade* pListener);

	//! Add a GameObject to the game.
	/*!
	Adds a GameObject to the game. This will cause the GameObject to be drawn, involved in collisions, etc. where relevant. 
	\param pGO a pointer to a GameObject.
	*/
	void addGameObject(GameObject* pGO);

	//! Remove a GameObject from the game.
	/*!
	Removes a GameObject from the game. 
	\param pGO a pointer to a GameObject.
	*/
	void removeGameObject(GameObject* pGO);

	//! Remove all GameObjects of the specified type.
	/*!
	\param sType the type of object to remove all instances of.
	*/
	void removeGameObjectsOfType(std::string sType);

	//! Get the GameObject of the specified type. 
	/*!
	This function is only really useful for objects with a unique type. If more than one object of the type exists the first
	one found will be returned.
	\param sType the type of object.
	\return the requested GameObject.
	*/
	GameObject* getGameObject(std::string sName);

	//! Get the GameObject at the specified index.
	/*!
	The index is an arbitrary so this function is mostly useful for iterating through all of
	the GameObjects. 
	\param iIndex the index of the GameObject. 
	\return the requested GameObject.
	*/
	GameObject* getGameObject(int iIndex);
	
	//! Get the number of GameObjects.
	/*!
	\return the number of GameObjects added to the game.
	*/
	int getNumGameObjects();
	//! Load a texture. All textures are stored centrally. There is no need to store a texture yourself.
	/*!
	\param acPath the path and filename of the texture to load. You must supply the file extension too.
	\param sTextureIdentifier the identifier you wish to use for the texture. This enables you to obtain the texture later. 
	\return the texture that was loaded.
	*/
	Texture* loadTexture(std::string sPath, std::string sTextureIdentifier);

	//! Get a pointer to a texture. 
	/*!
	\param sTextureIdentifier the identifier of the texture you wish to obtain a pointer to.
	\return the texture that was requested.
	*/
	Texture* getTexture(std::string sTextureIdentifier);

	//! Set an alarm 
	/*!
	An alarm allows you to control the timing of state changes etc. in your game.
	The game will call the function alarmComplete() with the ID of the alarm
	when the time is up.
	\param sAlarmID an arbitrary string that identifies the alarm.
	\param fAlarmTime the time in seconds after which the alarm goes off and  the alarmComplete() function is called.
	*/
	void setAlarm(std::string sAlarmID, float fAlarmTime);

	//! An alarm has completed. 
	/*!
	This function will be called when an alarm has gone off. The BaseArcade
	version is empty. See the derived class version for the code that deals
	with alarms. 
	\param sAlarmID the string that identifies the alarm.
	*/
	virtual void alarmComplete(std::string sAlarmID){}

	//! Cancel all alarms 
	/*!
	This function will cancel all outstanding alarms.
	*/
	void cancelAlarms();

	//! An a collisions between solid objects has occurred. 
	/*!
	This function will be called when a collision between two solid GameObjects has
	occurred. The two objects are passed via pointer parameters. Objects cannot
	collide with themselves and with other objects of the same type. 
	\param pGO1 the first object involved in the collision.
	\param pGO1 the second object involved in the collision.
	*/
	virtual void collisionEvent(GameObject* pGO1, GameObject* pGO2){}

	//! Create a message to be printed on the screen at the given location.
	/*!
	The font size can be set by calling setFontSize() prior to calling createMessage(). 
	The font colour can be set by calling setFontColour() prior to calling createMessage().
	\param sString the string to appear on the screen.
	\param iXPos the x-coordinate of the message.
	\param iYPos the y-coordinate of the message.
	*/
	void createMessage(std::string sString, int iXPos,int iYPos);

	//! Create a message to be printed on the screen at the given location.
	/*!
	The font size can be set by calling setFontSize() prior to calling createMessage(). 
	The font colour can be set by calling setFontColour() prior to calling createMessage().
	\param iInt an integer that will be converted to a string for displaying.
	\param iXPos the x-coordinate of the message.
	\param iYPos the y-coordinate of the message.
	*/
	void createMessage(int iInt, int iXPos,int iYPos);

	//! Change the text font size.
	/*!
	The font size will remain at the size given until this function is called again. 
	\param iSize the font size.
	*/
	void setFontSize(unsigned int iSize);

	//! Change the text colour.
	/*!
	The font size will remain the colour given until this function is called again. 
	\param r the red component.
	\param g the green component.
	\param b the blue component.
	*/
	void setFontColour(int r, int g, int b);

	//! Clear all onscreen messages.
	void clearMessages();

	//! Change the alpha mask (transparency) colour.
	/*!
	This function sets the colour that will be ignored when rendering sprites. The colour
	given will be treated as transparent. The default is black.
	\param r the red component.
	\param g the green component.
	\param b the blue component.
	*/
	void setAlphaMaskColour(int r, int g, int b);

	//! Set the background image.
	/*!
	This function sets the background image to the image provided. An optional parameter
	can be used to set the scroll speed of the background if desired. 
	\param sPath the image name and path.
	\param fScrollSpeed the scroll speed in pixels per second.
	*/
	void setBackground(std::string sPath, float fScrollSpeed = 0);

	//! Set the background scroll speed.
	/*!
	\param fScrollSpeed the scroll speed in pixels per second.
	*/
	void setBackgroundScrollSpeed(float fScrollSpeed);

	//! This function is called if a GameObject is deleted.
	/*!
	This function is called when a GameObject is deleted. This gives you an opportunity to do 
	something with that information if necessary, for example, to null your own pointer to the object
	\param pGO the game object that is being deleted.
	*/
	virtual void objectDeleted(GameObject* pGO){}

	//! This function gets the last frame duration in microseconds.
	/*!
	\return the last frame duration in microseconds.
	*/
	float getLastFrameTime();

	//! This function allows you to pause all events, i.e. stop listening to them.
	/*!
	\param b set this to false to unpause events. The defaul value is true.
	*/
	void pauseEvents(bool b = true);
	
	sf::RenderWindow& m_rw;
	unsigned int m_iFramesPerSecond;

private:
	static const int MAX_MESSAGES = 32;
	static const int FRAME_RATE = 30;
	static const int MAX_MESSAGE_LENGTH = 32;

	class MSG
	{
	public:
		std::string s;
		int x;
		int y;
	};


	class AlarmData
	{
	public:
		std::string sID;
		BaseArcade* pOwner;
		sf::Int64 alarmDuration;
	};

	void printMessages();
	void printMessage(BaseArcade::MSG& msg);
	void updateGameObjects();
	void checkAlarms();
	void removeOffscreenObjects();
	void checkCollisions();

	sf::Sprite* m_pBackground1;
	sf::Sprite* m_pBackground2;
	float m_fBackgroundScrollSpeed;
	float m_fScrollX;
	std::vector<GameObject*> m_vGameObjects;
	std::map<std::string, Texture*> m_vTextures;
	sf::Color m_AlphaMask;
	sf::Clock m_MainClock;
	int m_CurrFrameTime;
	unsigned int m_LastFrameTime;
	unsigned int m_MaxFrameDuration;
	sf::Clock m_aAlarmClocks[MAX_ALARMS];
	AlarmData m_aAlarmData[MAX_ALARMS];
	BaseArcade* m_pListener;
	sf::Font m_Font;
	BaseArcade::MSG m_aMessages[MAX_MESSAGES];
	int m_iNumMessages;
	sf::Text text;
	bool m_pbEventsPaused;
};

#endif