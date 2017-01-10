#ifndef GO_H_IK
#define GO_H_IK

#include "SFML/graphics/sprite.hpp"
#include <string>

using namespace sf;

class GameObject: public sf::Sprite
{
public:
	//! GameObject constructor. 
	/*!
	This is the constructor for creating game objects, i.e. objects that will actually appear
	in the game and can be interacted with. 
	\param texture the texture to use for the GameObject.
	\param sObjectType an arbitary string that you use to identify the object. Use a unique name if necessary.
	\param iImageWidth [optional] this should be set to the width of a frame within the texture for animated images.	
	\param iNumFrames [optional] the number of frames in the image for an animated image.	
	*/
	GameObject(sf::Texture* texture, std::string sObjectType, int iImageWidth = 0, int iNumFrames = 0);

	//! Sets the velocity of an object.
	/*!
	This function sets the velocity of the object. The velocity is set using three parameters:
	the x and y direction parameters set the direction of the velocity vector. These values
	don't affect the speed of the object because the magnitude of the vector isn't used to set the speed.
	The third parameter sets the speed. The velocity vector gets normalised. 
	\param fXDir the velocity vector x component.
	\param fYDir the velocity vector y component.
	\param fSpeed the speed in pixels per second.
	*/
	void setVelocity(float fXDir, float fYDir, float fSpeed); // speed in pixels per second
	
	//! Get the velocity vector. The velocity vector is normalised.
	/*!
	\return the velocity vector. Note that this doesn't tell you the speed of the object. See setVelocity
	for the explanation. Use getSpeedPerSecond() or getSpeedPerMicrosecond() to get the speed.
	*/
	sf::Vector2f& getVelocity();

	//! Set the object's speed in pixels per second.
	/*!
	\param fSpeed the speed in pixels per second.
	*/
	void setSpeed(float fSpeed);  

	//! Get the object's speed in pixels per second.
	/*!
	\return the speed in pixels per second.
	*/
	float getSpeedPerSecond(); 

	//! Get the object's speed in pixels per microsecond.
	/*!
	\return the speed in pixels per microsecond.
	*/
	float getSpeedPerMicrosecond();

	//! Perform movement update on the object based on its velocity and speed.
	/*!
	\param fTime the time in microseconds since the last update.
	*/
	void updatePosition(float fTime);

	//! Get the width of the object.
	/*!
	For animated objects, i.e. with multiple frames, this returns the width of a frame.
	\return the width of the object in pixels. 
	*/
	int getWidth();

	//! Get the height of the object.
	/*!
	The height is the same for animated and non-animated objects.
	*/
	int getHeight();

	//! Set the object to solid or non-solid, for collidable objects.
	/*!
	\param bSolid set to false to set the object to be non-solid. Other objects will not collide with it.
	set to true to set the object solid. The default value is true.
	*/
	void setSolid(bool bSolid = true);

	//! Get whether the object is solid or not.
	bool getSolid();

	//! Set the object to stay on the screen or not.
	/*!
	\param bStay set to false to allow the object to leave the screen, true otherwise. The default is true.
	*/
	void setStayOnScreen(bool bStay = true);

	//! Get whether the object is set to stay on the screen or not.
	bool getStayOnScreen();

	//! Set the object to be automatically updated for movement.
	/*!
	\param b set to false to avoid GameObject::update() being called. Set to true to make sure it is 
	called. The default is true.
	*/
	void setAutoUpdatePosition(bool b = true);
	
	//! Get whether the object is set to autoupdate or not.
	bool getAutoUpdatePosition();

	//! set the alive zone. Outside of this area the object will be deleted.
	/*! 
	If all parameters are set to 0 (the default), the alive zone is infinite.
	\param iLeft the x coordinate of the left edge of the alive zone. Set to INT_MIN for no left edge.
	\param iTop the y coordinate of the top edge of the alive zone. Set to INT_MIN for no top edge.
	\param iWidth the width of the zone. Set to INT_MAX for no limit.
	\param iHeight the height of the zone. Set to INT_MAX for no limit.
	*/
	void setAliveZone(int iLeft, int iTop, int iWidth, int iHeight);

	//! Get the alive zone
	sf::IntRect& getAliveZone();

	//void setAnimated(bool b = true);

	//bool getAnimated();

	//! Advance the animation to the next frame.
	void nextFrame();

	//! Get the current frame number.
	int getFrame();

	//! Get the number of frames in the animation.
	int getNumFrames();

	//! Set the current frame to a specific frame.
	/*!
	\param iFrame the frame number.
	*/
	void setFrame(int iFrame);

	//! get the object type. This returns the type set in the constructor.
	std::string getObjectType();

private:
	sf::Vector2f m_Velocity;
	float m_fSpeed;
	int m_iWidth;
	int m_iHeight;
	int m_iNumFrames;
	int m_iCurrentFrame;
	bool m_bSolid;
	bool m_bStayOnScreen;
	sf::IntRect m_AliveZone;
	std::string m_sObjectType;
	bool m_bAutoUpdatePosition;
	bool m_bAnimated;
};

#endif