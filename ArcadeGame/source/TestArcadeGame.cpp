#include "ArcadeGame.h"

int main ()
{
	sf::RenderWindow app(sf::VideoMode(BaseArcade::SCREEN_WIDTH, BaseArcade::SCREEN_HEIGHT), "MyTestGame",sf::Style::Close);
	app.setKeyRepeatEnabled(false);

	ArcadeGame game(app);

	while (app.isOpen())
	{
		if (!game.startFrame())
			continue;

		sf::Event Event;

		std::string sKeyPressed;

		while (app.pollEvent(Event))
		{			
			if (Event.type == sf::Event::Closed)
                app.close();

			if ((Event.type == sf::Event::KeyPressed))
			{
				switch (Event.key.code){
					case sf::Keyboard::R: sKeyPressed = "R"; break;
					case sf::Keyboard::A: sKeyPressed = "A"; break;
					case sf::Keyboard::S: sKeyPressed = "S"; break;
					case sf::Keyboard::W: sKeyPressed = "W"; break;
					case sf::Keyboard::D: sKeyPressed = "D"; break;
					case sf::Keyboard::Space: sKeyPressed = "SPACE"; break;
					case sf::Keyboard::Left: sKeyPressed = "LEFT"; break;
					case sf::Keyboard::Right: sKeyPressed = "RIGHT"; break;
					case sf::Keyboard::Up: sKeyPressed = "UP"; break;
					case sf::Keyboard::Down: sKeyPressed = "DOWN"; break;
				}
			}	
		}

		game.gameMain(sKeyPressed);
	
		game.render();

		app.display();

//		game.endFrame();
	}

	return 0;
}