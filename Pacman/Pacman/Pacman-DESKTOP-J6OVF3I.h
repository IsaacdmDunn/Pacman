#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

//definitions
#define MUNCHIECOUNT 50

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

//struct for pacman player
struct Player
{
	// Data to represent Pacman
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int frame;
	int currentFrameTime;
	int direction = 0;
	int frameTime;
	float speedMultiplier;
};

struct Munchie
{
	// Data to represent Munchie
	int frameCount;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int frameTime;
	int currentFrameTime;
};

struct PauseMenu
{
	//Data to represent pause menu
	Texture2D* backgroundTexture;
	Rect* sourceRect;
	Vector2* stringPosition;
	bool isPaused;
	bool pKeyDown;
};

struct MainMenu 
{
	//Data to represent main menu
	Texture2D* backgroundTexture;
	Rect* sourceRect;
	Vector2* stringPosition;
	bool mainMenuOpen;
};

struct UserInterface 
{
	// Position for String
	Vector2* _stringPosition;
};

struct Enemy 
{

};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	void Input(int elapsedTime, Input::KeyboardState *state);
	void CheckPaused(Input::KeyboardState* keyboardState);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime);

	Player* _pacman;
	PauseMenu* _pauseMenu;
	MainMenu* _mainMenu;
	UserInterface* _gameUI;
	Munchie* _munchie[MUNCHIECOUNT];

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};