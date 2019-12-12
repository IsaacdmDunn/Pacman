#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"
#include "Constants.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

//struct for pacman player
struct Player
{
	// Data to represent Pacman
	bool isDead;
	bool isGameOver;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int frame;
	int livesLeft = 3;
	int currentFrameTime;
	int direction = 0;
	int frameTime;
	float speedMultiplier;
	float speed;
	bool ghostScared;
	int munchieEffectTimer;
	int deathAnimationTimer;
};

//struct for ghosts
struct Ghost
{
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int direction = rand() % 3;
	float speed;
	int frame;
	int frameTime;
	int currentFrameTime;
	bool correctedToTile;
};

//struct for munchie
struct Munchie
{
	int frameCount;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int frameTime;
	int currentFrameTime;
};

//struct for munchie
struct Dot
{
	int frameCount;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int frameTime;
	int currentFrameTime;
};

//struct for cherry
struct Cherry
{
	int frameCount;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int frameTime;
	int currentFrameTime;
	bool cherryEaten;
};

//struct for pause menu
struct PauseMenu
{
	//Data to represent pause menu
	Texture2D* backgroundTexture;
	Rect* sourceRect;
	Vector2* stringPosition;
	bool isPaused;
	bool pKeyDown;
};

//struct for main menu
struct MainMenu 
{
	Texture2D* backgroundTexture;
	Rect* sourceRect;
	Vector2* stringPosition;
	bool mainMenuOpen;
};

//struct for UI
struct UserInterface 
{
	// Position for String
	Vector2* scoreUIPosition;
	Vector2* hiScoreUIPosition;
	Vector2* livesUIPosition;
	int score;
	int highScore;
};

//struct for level
struct Level 
{
	char mapTiles[24][24];
	int levelDotCount;
	int levelNum = 1;
};

//struct for walls
struct Walls 
{
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
	//private
private:
	//methods
	void LoadTileMap();
	void SetWallPosition(int &wallID, int &mapTileX, int &mapTileY);
	void CheckLevelCompletion();

	void Input(int elapsedTime, Input::KeyboardState *state);
	void CheckPaused(Input::KeyboardState* keyboardState);
	
	void UpdateGhost(Ghost* _ghost[], int ghostID, int elapsedTime);
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime);
	void UpdateDot(int elapsedTime);
	void UpdateCherry(int elapsedTime);

	void CheckPacmanViewportCollision();
	void CheckGhostViewportCollision(Ghost* ghost);
	void CheckGhostCollisions();
	void CheckMunchieCollision();
	void CheckDotCollision();
	void CheckCherryCollision();
	void CheckWallCollision(int elapsedTime);
	
	void CorrectPacmanToTile(int elapsedTime, Player* _pacman);
	void PacmanDeathAnimation();
	void MunchieEffect(int elapsedTime);

	void DrawPacman(Player* _pacman, MainMenu* _mainMenu);

	//game items
	Player* _pacman;
	PauseMenu* _pauseMenu;
	MainMenu* _mainMenu;
	UserInterface* _gameUI;
	Munchie* _munchie[MUNCHIECOUNT];
	Dot* _dot[DOTCOUNT];
	Cherry* _cherry;
	Ghost* _ghosts[GHOSTCOUNT];
	Level* _level;
	Walls* _wall[WALLCOUNT];
	Texture2D* munchieTexture;
	Texture2D* dotTexture;

	//Game sounds
	SoundEffect* _pop;
	SoundEffect* _deathSound;
	SoundEffect* _titleMusic;
	SoundEffect* _gameOverMusic;
	SoundEffect* _levelWin;

	
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