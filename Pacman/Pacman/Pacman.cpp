//Libraries
#include "Pacman.h"
#include "Constants.h"
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>

//Constructor method
Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	//uses time method to provide a seed for random functions to help mitigate the issues of pseudo randomness
	srand(time(NULL));

	//initialise munchies
	for (auto & munchieID : _munchie)
	{
		munchieID = new Munchie();
		munchieID->frameCount;
		munchieID->frameTime = 500;
		munchieID->currentFrameTime = rand() % 500;
	}

	//initialise dots
	for (auto & dotID : _dot)
	{
		dotID = new Dot();
		dotID->frameCount;
		dotID->frameTime = 500;
		dotID->currentFrameTime = rand() % 500;
	}

	//initialise ghosts
	for (auto & ghostID : _ghosts)
	{
		ghostID = new Ghost();
		ghostID->direction = 0;
		ghostID->speed = 0.01f;
		ghostID->frame = 0;
		ghostID->frameTime = 250;
		ghostID->currentFrameTime = 0;
		ghostID->correctedToTile = false;
	}

	//Initialise level
	_level = new Level();

	//Initialise cherry
	_cherry = new Cherry();

	//Munchie struct values
	_cherry->frameCount;
	_cherry->frameTime = 500;
	_cherry->currentFrameTime = rand() % 500;

	//Pacman struct values
	_pacman = new Player();
	_pacman->currentFrameTime = 0;
	_pacman->direction = 0;
	_pacman->frame = 0;
	_pacman->frameTime = 250;
	_pacman->speedMultiplier = 1.0f;
	_pacman->speed = 0.1f;

	//Pause menu struct values
	_pauseMenu = new PauseMenu();
	_pauseMenu->stringPosition;
	_pauseMenu->isPaused = false;
	_pauseMenu->pKeyDown = false;

	//Main menu struct values
	_mainMenu = new MainMenu();
	_mainMenu->stringPosition;
	_mainMenu->mainMenuOpen = true;

	//User interface struct values
	_gameUI = new UserInterface();

	//Initialise sounds
	_pop = new SoundEffect();
	_deathSound = new SoundEffect();
	_titleMusic = new SoundEffect();
	_gameOverMusic = new SoundEffect();
	_levelWin = new SoundEffect();

	//Initialise walls
	for (auto & wallID : _wall)
	{
		wallID = new Walls();
	}

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();


	//Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

//Load content
void Pacman::LoadContent()
{
	//Loads hi-score
	ifstream scoreSystem("hiscore.txt", ios::binary);
	if (!scoreSystem) //checks for errors
	{
		cerr << "File could not be opened!";
		std::exit(1);
	}

	//reads the file and displays it as highscore
	string lineOfText;
	getline(scoreSystem, lineOfText);
	istringstream(lineOfText) >> _gameUI->highScore;

	//Load Pacman
	_pacman->isDead = false;
	_pacman->isGameOver = false;
	_pacman->ghostScared = false;
	_pacman->position = new Vector2(550.0f, 550.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);

	//Load ghosts
	for (int ghostID = 0; ghostID < GHOSTCOUNT; ghostID++)
	{
		_ghosts[ghostID]->texture = new Texture2D();
		_ghosts[ghostID]->position = new Vector2(0, 0);
		_ghosts[ghostID]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}

	//Set menu parameters
	_pauseMenu->backgroundTexture = new Texture2D();
	_pauseMenu->backgroundTexture->Load("Textures/Transparency.png", false);
	_pauseMenu->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_pauseMenu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//Set Main Menu parameters
	_mainMenu->backgroundTexture = new Texture2D();
	_mainMenu->backgroundTexture->Load("Textures/MainMenuBG.png", false);
	_mainMenu->sourceRect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_mainMenu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//Load Munchies
	munchieTexture = new Texture2D();
	munchieTexture->Load("Textures/Munchie.png", true);
	for (int munchieID = 0; munchieID < MUNCHIECOUNT; munchieID++)
	{
		_munchie[munchieID]->texture = munchieTexture;
		_munchie[munchieID]->position = new Vector2(1000, 1000);
		_munchie[munchieID]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}

	//Load Dots
	dotTexture = new Texture2D();
	dotTexture->Load("Textures/Dot.png", true);
	for (int dotID = 0; dotID < DOTCOUNT; dotID++)
	{
		_dot[dotID]->texture = dotTexture;
		_dot[dotID]->position = new Vector2(1000, 1000);
		_dot[dotID]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}

	//Load Cherry
	_cherry->position = new Vector2(150.0f, 150.0f);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->texture = new Texture2D();
	_cherry->texture->Load("Textures/Cherry.png", false);

	//Set string position for user interface
	_gameUI->scoreUIPosition = new Vector2(10.0f, 25.0f);
	_gameUI->hiScoreUIPosition = new Vector2(10.0f, 25.0f);
	_gameUI->livesUIPosition = new Vector2(60.0f, 75.0f);

	//Load sounds
	_pop->Load("Sounds/pop.wav");
	_deathSound->Load("Sounds/deathSound.mp3");
	_titleMusic->Load("Sounds/titleMusic.mp3");
	_gameOverMusic->Load("Sounds/gameOver.mp3");
	_levelWin->Load("Sounds/levelWin.mp3");

	//Load walls
	for (int wallID = 0; wallID < WALLCOUNT; wallID++)
	{
		_wall[wallID]->texture = new Texture2D();
		_wall[wallID]->position = new Vector2(1000, 1000);
		_wall[wallID]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	}

	//Loads the tile map for the level
	LoadTileMap();
}

//Deconstructor
Pacman::~Pacman()
{
	// Clean up pointers within the Pacman structure 
	delete _pacman->position;
	delete _pacman->sourceRect;
	delete _pacman->texture;

	//Deletes munchie pointers
	delete munchieTexture;
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i]->sourceRect;
	}
	delete[] _munchie;

	//Deletes dot pointers
	delete dotTexture;
	for (int i = 0; i < DOTCOUNT; i++)
	{
		delete _dot[i]->sourceRect;
	}
	delete[] _dot;

	//Deletes ghost's pointers
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		delete _ghosts[i]->texture;
		delete _ghosts[i]->sourceRect;
	}
	delete[] _ghosts;

	//Deletes wall pointers
	for (int i = 0; i < WALLCOUNT; i++)
	{
		delete _wall[i]->texture;
		delete _wall[i]->sourceRect;
	}
	delete[] _wall;

	//Deletes cherry pointers
	delete _cherry->texture;
	delete _cherry->sourceRect;

	//Deletes main menu pointers
	delete _mainMenu->backgroundTexture;
	delete _mainMenu->sourceRect;

	//Deletes pause menu pointers
	delete _pauseMenu->backgroundTexture;
	delete _pauseMenu->sourceRect;

	//Clean up the Pacman structure pointer 
	delete _pacman;
	delete _munchie;
	delete _dot;
	delete _wall;
	delete _mainMenu;
	delete _pauseMenu;
	delete _gameUI;

	//Clean up sound pointers
	delete _pop;
	delete _deathSound;
	delete _titleMusic;
	delete _gameOverMusic;
	delete _levelWin;
}

//Loads the tile map for the level
void Pacman::LoadTileMap()
{
	//Local variables
	string fileLine;
	string level = "";
	int mapTileX = 0;
	int mapTileY = 0;
	int munchieID = 0;
	int wallID = 0;

	//Load level file and uses levelNum to decide which level file to load
	if (_level->levelNum == 1)
	{
		level = "levels/level_layout1.txt";
		_level->levelDotCount = 0;
	}
	else if (_level->levelNum == 2)
	{
		level = "levels/level_layout2.txt";
		_level->levelNum = 1;
	}

	ifstream infile(level);
	// Check if the file was opened.
	if (infile.is_open())
	{
		// Loop through the entire file and output each line to .
		while (getline(infile, fileLine))
		{
			//outputs line of text from file to console
			cout << fileLine << endl;

			//loop each character in the line of text from the file and check which tile it represents
			for (mapTileY = 0; mapTileY < MAPHEIGHT; mapTileY++)
			{
				//sets tile value
				_level->mapTiles[mapTileY][mapTileX] = (char)fileLine[mapTileY];

				//if O then set munchie position to tile position
				if (_level->mapTiles[mapTileY][mapTileX] == 'O')
				{
					_munchie[munchieID]->position->X = mapTileX * 32 + SCREENOFFSET;
					_munchie[munchieID]->position->Y = mapTileY * 32;
					munchieID++;
					
				}

				//if o then set munchie position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == 'o')
				{
					_dot[_level->levelDotCount]->position->X = mapTileX * 32 + SCREENOFFSET;
					_dot[_level->levelDotCount]->position->Y = mapTileY * 32;
					_level->levelDotCount++;
				}

				//if C then set cherry position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == 'C' && _cherry->cherryEaten == false)
				{
					_cherry->position->X = mapTileX * 32 + SCREENOFFSET;
					_cherry->position->Y = mapTileY * 32;
				}

				//if 1 then set ghost Inky position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == '1')
				{
					_ghosts[0]->position->X = mapTileX * 32 + SCREENOFFSET;
					_ghosts[0]->position->Y = mapTileY * 32;
					_ghosts[0]->texture->Load("Textures/GhostBlue.png", false);
				}

				//if 2 then set ghost Pinky position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == '2')
				{
					_ghosts[1]->position->X = mapTileX * 32 + SCREENOFFSET;
					_ghosts[1]->position->Y = mapTileY * 32;
					_ghosts[1]->texture->Load("Textures/GhostPinky.png", false);
				}

				//if 3 then set ghost Blinky position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == '3')
				{
					_ghosts[2]->position->X = mapTileX * 32 + SCREENOFFSET;
					_ghosts[2]->position->Y = mapTileY * 32;
					_ghosts[2]->texture->Load("Textures/GhostBlinky.png", false);
				}

				//if 4 then set ghost Clyde position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == '4')
				{
					_ghosts[3]->position->X = mapTileX * 32 + SCREENOFFSET;
					_ghosts[3]->position->Y = mapTileY * 32;
					_ghosts[3]->texture->Load("Textures/GhostClyde.png", false);
				}

				//if P then set Pacman position to tile position
				else if (_level->mapTiles[mapTileY][mapTileX] == 'P')
				{
					_pacman->position->X = mapTileX * 32 + SCREENOFFSET;
					_pacman->position->Y = mapTileY * 32;
				}

				//if - then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == '-')
				{
					_wall[wallID]->texture->Load("Textures/Walls/VerticalWallTile.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if | then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == '|')
				{
					_wall[wallID]->texture->Load("Textures/Walls/HorizontalWallTile.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if ^ then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == '^')
				{
					_wall[wallID]->texture->Load("Textures/Walls/LeftWallEnd.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if v then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == 'v')
				{
					_wall[wallID]->texture->Load("Textures/Walls/RightWallEnd.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if < then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == '<')
				{
					_wall[wallID]->texture->Load("Textures/Walls/UpperWallEnd.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if > then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == '>')
				{
					_wall[wallID]->texture->Load("Textures/Walls/BottomWallEnd.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if A then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == 'A')
				{
					_wall[wallID]->texture->Load("Textures/Walls/TopLeftCornerWall.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if Q then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == 'Q')
				{
					_wall[wallID]->texture->Load("Textures/Walls/TopRightCornerWall.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if S then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == 'S')
				{
					_wall[wallID]->texture->Load("Textures/Walls/BottomLeftCornerWall.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

				//if W then set wall position to tile position and texture
				else if (_level->mapTiles[mapTileY][mapTileX] == 'W')
				{
					_wall[wallID]->texture->Load("Textures/Walls/BottomRightCornerWall.png", false);
					SetWallPosition(wallID, mapTileX, mapTileY);
				}

			}
			mapTileX++;
		}
		infile.close(); // Close the file.
	}

	// File did not open. Inform the user.
	else
	{
		cout << "Unable to open file";
	}
}

//Sets the position of the walls
void Pacman::SetWallPosition(int &wallID, int &mapTileX, int &mapTileY)
{
	_wall[wallID]->position->X = mapTileX * 32 + SCREENOFFSET;
	_wall[wallID]->position->Y = mapTileY * 32;
	wallID++;
}

//Updates the game
void Pacman::Update(int elapsedTime)
{
	//Gets current state of keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	CheckPaused(keyboardState); //Checks if game's paused

	// Gets the current state of the keyboard
	if (_pauseMenu->isPaused == false)
	{
		//Manages input for the game
		Input(elapsedTime, keyboardState);

		//Checks if player finished the level
		CheckLevelCompletion();

		//If pacman eats a munchie then pause ghost's movement
		if (_pacman->ghostScared == true)
		{
			MunchieEffect(elapsedTime);
		}
		if (_pacman->isDead == true) 
		{
			PacmanDeathAnimation();
		}
		else 
		{
			UpdatePacman(elapsedTime);
		}

		//Manages collisions 
		CheckPacmanViewportCollision();
		CheckGhostCollisions();
		CheckMunchieCollision();
		CheckDotCollision();
		CheckCherryCollision();
		CheckWallCollision(elapsedTime);

		//Updates the game objects
		UpdateMunchie(elapsedTime);
		UpdateDot(elapsedTime);
		UpdateCherry(elapsedTime);
		for (int ghostID = 0; ghostID < GHOSTCOUNT; ghostID++)
		{
			UpdateGhost(_ghosts, ghostID, elapsedTime);
			CheckGhostViewportCollision(_ghosts[ghostID]); //checks if a ghost leaves the game area
		}
	}
}

//Manages input for the game
void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState)
{
	//sets the current speed of pacman
	float pacmanCurrentSpeed = _pacman->speedMultiplier * elapsedTime * _pacman->speed;

	//Checks if D key is pressed and pacman is vertically centred on a tile
	if (keyboardState->IsKeyDown(Input::Keys::D) && (int)_pacman->position->Y % 32 == 0) 
	{
		_pacman->position->X += pacmanCurrentSpeed; //Moves Pacman across X axis
		_pacman->direction = 0;
	}

	//Checks if S key is pressed and pacman is horizontally centred on a tile
	else if (keyboardState->IsKeyDown(Input::Keys::S) && (int)_pacman->position->X % 32 == 0) 
	{
		_pacman->position->Y += pacmanCurrentSpeed; //Moves Pacman across Y axis
		_pacman->direction = 1;
	}

	//Checks if A key is pressed and pacman is vertically centred on a tile
	else if (keyboardState->IsKeyDown(Input::Keys::A) && (int)_pacman->position->Y % 32 == 0) 
	{
		_pacman->position->X -= pacmanCurrentSpeed; //Moves Pacman across X axis
		_pacman->direction = 2;
	}

	//Checks if W key is pressed and pacman is horizontally centred on a tile
	else if (keyboardState->IsKeyDown(Input::Keys::W) && (int)_pacman->position->X % 32 == 0) 
	{
		_pacman->position->Y -= pacmanCurrentSpeed; //Moves Pacman across Y axis
		_pacman->direction = 3;
	}

	//Else, corrects the pacman position if they dont move the length/height of a tile based on their direction
	else {
		CorrectPacmanToTile(elapsedTime, _pacman);
	}

	//sprinting
	if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		//apply sprint
		_pacman->speedMultiplier = 2.0f;
	}
	else
	{
		//reset multiplier
		_pacman->speedMultiplier = 1.0f;
	}
}

//Corrects pacman to the nearest tile
void Pacman::CorrectPacmanToTile(int elapsedTime, Player* _pacman)
{
	//if pacman is not horizontally centred on a tile and is facing right then centre pacman towards the tile its facing
	if ((int)_pacman->position->X % 32 > 0 && _pacman->direction == 0)
	{
		_pacman->position->X += (int)(_pacman->speedMultiplier * elapsedTime * _pacman->speed);
	}

	//if pacman is not vertically centred on a tile and is facing down then centre pacman towards the tile its facing
	else if ((int)_pacman->position->Y % 32 > 0 && _pacman->direction == 1)
	{
		_pacman->position->Y += (int)(_pacman->speedMultiplier * elapsedTime * _pacman->speed);
	}

	//if pacman is not horizontally centred on a tile and is facing left then centre pacman towards the tile its facing
	else if ((int)_pacman->position->X % 32 > 0 && _pacman->direction == 2)
	{
		_pacman->position->X -= (int)(_pacman->speedMultiplier * elapsedTime * _pacman->speed);
	}
	//if pacman is not vertically centred on a tile and is facing up then centre pacman towards the tile its facing
	else if ((int)_pacman->position->Y % 32 > 0 && _pacman->direction == 3)
	{
		_pacman->position->Y -= (int)(_pacman->speedMultiplier * elapsedTime * _pacman->speed);
	}
}

//Manages the input for menus and pausing the game
void Pacman::CheckPaused(Input::KeyboardState* keyboardState)
{
	// Checks if P key is pressed check if game can be paused or unpaused
	if (keyboardState->IsKeyDown(Input::Keys::P) && _pauseMenu->pKeyDown == false)
	{
		_pauseMenu->pKeyDown = true;
		_pauseMenu->isPaused = !_pauseMenu->isPaused;
	}
	if (keyboardState->IsKeyUp(Input::Keys::P))
	{
		_pauseMenu->pKeyDown = false;
	}

	//checks is space was pressed while main menu is open
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && _mainMenu->mainMenuOpen == true)
	{
		_mainMenu->mainMenuOpen = false;
	}
}

//checks if Pacman leaves the game scene and teleports it to the otherside
void Pacman::CheckPacmanViewportCollision()
{
	//checks if pacman is going off-screen
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth() - SCREENOFFSET) //pacman hit right wall - reset to left wall
	{
		_pacman->position->X = -32.0f + (_pacman->sourceRect->Width + SCREENOFFSET);
	}
	if (_pacman->position->X - _pacman->sourceRect->Width < -32.0f + SCREENOFFSET) //pacman hit left wall - reset to right wall
	{	
		_pacman->position->X = -32.0f + Graphics::GetViewportWidth() - SCREENOFFSET;
	}
	if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight()) //pacman hit upper wall - reset to lower wall
	{
		_pacman->position->Y = -32.0f + (_pacman->sourceRect->Height); 
	}
	if (_pacman->position->Y - _pacman->sourceRect->Height < -32.0f)//pacman hit lower wall - reset to upper wall
	{	
		_pacman->position->Y = -32.0f + Graphics::GetViewportHeight(); 
	}
}

//checks if ghost leaves the game scene and teleports it to the otherside
void Pacman::CheckGhostViewportCollision(Ghost* ghost)
{
	//checks if ghost is going off-screen
	if (ghost->position->X + ghost->sourceRect->Width > Graphics::GetViewportWidth() - SCREENOFFSET) //ghost hit right wall - reset to left wall
	{
		ghost->position->X = -32.0f + (ghost->sourceRect->Width + SCREENOFFSET);
	}
	if (ghost->position->X - ghost->sourceRect->Width < -32.0f + SCREENOFFSET) //ghost hit left wall - reset to right wall
	{	
		ghost->position->X = -32.0f + Graphics::GetViewportWidth() - SCREENOFFSET;
	}
	if (ghost->position->Y + ghost->sourceRect->Height > Graphics::GetViewportHeight()) //ghost hit upper wall - reset to lower wall
	{
		ghost->position->Y = -32.0f + (ghost->sourceRect->Height);
	}
	if (ghost->position->Y - ghost->sourceRect->Height < -32.0f) //ghost hit lower wall - reset to upper wall
	{	
		ghost->position->Y = -32.0f + Graphics::GetViewportHeight();
	}
}

//updates the pacman sprite based on the frame rate
void Pacman::UpdatePacman(int elapsedTime)
{
	//keeps track of the animation frames for pacman
	_pacman->currentFrameTime += elapsedTime;
	if (_pacman->currentFrameTime > _pacman->frameTime)
	{
		_pacman->frame++;
		if (_pacman->frame >= 2) _pacman->frame = 0;
		_pacman->currentFrameTime = 0;
	}

	//changes the image of pacman from the spritesheet
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
}

//kills pacman and runs death animation
void Pacman::PacmanDeathAnimation()
{
	_pacman->ghostScared = true; //stops ghosts moving
	_pacman->deathAnimationTimer++; //increment death animation timer

	//after 12 frames set new death frame
	if (_pacman->deathAnimationTimer == 12) 
	{
		_pacman->sourceRect->X = 64;
		_pacman->sourceRect->Y = 0;
	}	

	//after 24 frames set new death frame
	else if (_pacman->deathAnimationTimer == 24) 
	{
		_pacman->sourceRect->X = 64;
		_pacman->sourceRect->Y = 32;
	}

	//after 36 frames set new death frame
	else if (_pacman->deathAnimationTimer == 36) 
	{
		_pacman->sourceRect->X = 64;
		_pacman->sourceRect->Y = 64;
	}

	//after 48 frames set new death frame
	else if (_pacman->deathAnimationTimer == 48) 
	{
		_pacman->sourceRect->X = 64;
		_pacman->sourceRect->Y = 96;
	}

	//resets game state after 60 frames
	else if (_pacman->deathAnimationTimer == 60)
	{
		_pacman->isDead = false;
		_pacman->ghostScared = false;
		_pacman->deathAnimationTimer = 0;
		LoadTileMap(); //loads map
	}
}

//updates ghosts
void Pacman::UpdateGhost(Ghost* _ghost[], int ghostID, int elapsedTime)
{
	//moves ghosts if munchie is eaten
	if (_ghost[0]->position->X <= _pacman->position->X && _pacman->ghostScared == false) //Moves inky right to follow pacman
	{
		_ghost[0]->position->X += _ghost[0]->speed * elapsedTime;
		_ghost[0]->direction = 0; //Change direction
	}
	else if (_ghost[0]->position->X > _pacman->position->X && _pacman->ghostScared == false) //Moves inky left to follow pacman
	{
		_ghost[0]->position->X -= _ghost[0]->speed * elapsedTime;
		_ghost[0]->direction = 1; //Change direction
	}
	if (_ghost[0]->position->Y <= _pacman->position->Y && _pacman->ghostScared == false) //Moves inky down to follow pacman
	{
		_ghost[0]->position->Y += _ghost[0]->speed * elapsedTime;
		_ghost[0]->direction = 2; //Change direction
	}
	else if (_ghost[0]->position->Y > _pacman->position->Y && _pacman->ghostScared == false) //Moves inky up to follow pacman
	{
		_ghost[0]->position->Y -= _ghost[0]->speed * elapsedTime;
		_ghost[0]->direction = 3; //Change direction
	}
	if (_pacman->ghostScared == false) //moves pinky diagonaly
	{
		_ghost[1]->position->Y += _ghost[1]->speed * elapsedTime;
		_ghost[1]->position->X += _ghost[1]->speed * elapsedTime;
		_ghost[1]->direction = 0; //Change direction
	}
	if (_pacman->ghostScared == false) //moves blinky up
	{
		_ghost[2]->position->Y -= _ghost[2]->speed * elapsedTime;
		_ghost[2]->direction = 3; //Change direction
	}
	if (_pacman->ghostScared == false) //moves clyde left
	{
		_ghost[3]->position->X -= _ghost[3]->speed * elapsedTime;
		_ghost[3]->direction = 1; //Change direction
	}
	//keeps track of the animation frames for ghosts
	_ghost[ghostID]->currentFrameTime += elapsedTime;
	if (_ghost[ghostID]->currentFrameTime > _ghost[ghostID]->frameTime)
	{
		_ghost[ghostID]->frame++;
		if (_ghost[ghostID]->frame >= 2) _ghost[ghostID]->frame = 0;
		_ghost[ghostID]->currentFrameTime = 0;
	}

	//changes the image of ghosts from the spritesheet
	_ghost[ghostID]->sourceRect->X = _ghost[ghostID]->sourceRect->Width * _ghost[ghostID]->frame;
	_ghost[ghostID]->sourceRect->Y = _ghost[ghostID]->sourceRect->Height * _ghost[ghostID]->direction;
}

//checks if a ghost collided with pacman
void Pacman::CheckGhostCollisions()
{
	//sets the bounding box for the ghost
	int top2 = 0;
	int right2 = 0;
	int bottom2 = 0;
	int left2 = 0;

	//sets the bounding box for the pacman
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int left1 = _pacman->position->X;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int top1 = _pacman->position->Y;

	//checks for collision in each ghost
	for (int ghostID = 0; ghostID < GHOSTCOUNT; ghostID++)
	{
		//set bounding box to current position of ghost
		bottom2 = _ghosts[ghostID]->position->Y + _ghosts[ghostID]->sourceRect->Height;
		left2 = _ghosts[ghostID]->position->X;
		right2 = _ghosts[ghostID]->position->X + _ghosts[ghostID]->sourceRect->Width;
		top2 = _ghosts[ghostID]->position->Y;

		//if collision is made then move pacman
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			
			_ghosts[ghostID]->position->X = 1000;
			_ghosts[ghostID]->position->Y = 1000;
			Audio::Play(_pop);
			_pacman->livesLeft--;
			_pacman->isDead = true;
		}
		if (_pacman->livesLeft == 0)
		{
			_pacman->isGameOver = true;

			//if score was higher then highscore then upload score as new high score
			if (_gameUI->highScore < _gameUI->score)
			{
				ofstream scoreSystem("hiscore.txt");
				scoreSystem << _gameUI->score;
			}
		}
	}
}

//updates the munchie animation
void Pacman::UpdateMunchie(int elapsedTime)
{
	//keeps track of the animation frames for munchie
	for (int munchieID = 0; munchieID < MUNCHIECOUNT; munchieID++)
	{
		_munchie[munchieID]->currentFrameTime += elapsedTime;
		if (_munchie[munchieID]->currentFrameTime > _munchie[munchieID]->frameTime)
		{
			_munchie[munchieID]->frameCount++;
			if (_munchie[munchieID]->frameCount >= 2) _munchie[munchieID]->frameCount = 0;

			_munchie[munchieID]->currentFrameTime = 0;
		}
	}
}

//checks if a munchie collided with pacman
void Pacman::CheckMunchieCollision()
{
	//sets the bounding box for the munchie
	int top2 = 0;
	int right2 = 0;
	int bottom2 = 0;
	int left2 = 0;

	//sets the bounding box for pacman
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int left1 = _pacman->position->X;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int top1 = _pacman->position->Y;

	//checks for collision in each munchie
	for (int munchieID = 0; munchieID < MUNCHIECOUNT; munchieID++)
	{
		//set bounding box to current position of munchie
		bottom2 = _munchie[munchieID]->position->Y + _munchie[munchieID]->sourceRect->Height;
		left2 = _munchie[munchieID]->position->X;
		right2 = _munchie[munchieID]->position->X + _munchie[munchieID]->sourceRect->Width;
		top2 = _munchie[munchieID]->position->Y;

		//if collision is made then move munchie, adds score, plays sound and sets munchie eaten to true
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_munchie[munchieID]->position->X = 1000;
			_munchie[munchieID]->position->Y = 1000;
			_pacman->ghostScared = true;
			Audio::Play(_pop);
			_gameUI->score += 50;
		}
	}
}

//checks if a munchie collided with pacman
void Pacman::CheckCherryCollision()
{
	//sets the bounding box for the munchie
	int top2 = 0;
	int right2 = 0;
	int bottom2 = 0;
	int left2 = 0;

	//sets the bounding box for pacman
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int left1 = _pacman->position->X;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int top1 = _pacman->position->Y;

	//checks for collision in each munchie
	for (int munchieID = 0; munchieID < MUNCHIECOUNT; munchieID++)
	{
		//set bounding box to current position of munchie
		bottom2 = _cherry->position->Y + _cherry->sourceRect->Height;
		left2 = _cherry->position->X;
		right2 = _cherry->position->X + _cherry->sourceRect->Width;
		top2 = _cherry->position->Y;

		//if collision is made then move munchie
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_cherry->position->X = 1000;
			_cherry->position->Y = 1000;
			_cherry->cherryEaten = true;

			Audio::Play(_pop);

			_gameUI->score += 1000;
		}
	}
}

//keeps ghostScared true for a set time before setting it to false again
void Pacman::MunchieEffect(int elapsedTime)
{
	if (_pacman->currentFrameTime == 0)
	{
		_pacman->munchieEffectTimer++;
	}
	if (_pacman->munchieEffectTimer == 25) 
	{
		_pacman->ghostScared = false;
		_pacman->munchieEffectTimer = 0;
	}
}

//checks if a dot collided with pacman
void Pacman::CheckDotCollision()
{
	//sets the bounding box for the dot
	int top2, right2, bottom2, left2;

	//sets the bounding box for pacman
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int left1 = _pacman->position->X;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int top1 = _pacman->position->Y;

	//checks for collision in each dot
	for (int dotID = 0; dotID < DOTCOUNT; dotID++)
	{
		//set bounding box to current position of dot
		bottom2 = _dot[dotID]->position->Y + _dot[dotID]->sourceRect->Height;
		left2 = _dot[dotID]->position->X;
		right2 = _dot[dotID]->position->X + _dot[dotID]->sourceRect->Width;
		top2 = _dot[dotID]->position->Y;

		//if collision is made then move/remove dot, play sound and add score
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_dot[dotID]->position->X = 1000;
			_dot[dotID]->position->Y = 1000;
			Audio::Play(_pop);
			_gameUI->score += 10;
			_level->levelDotCount--;
		}
	}
}

//checks if all the dots in the level have been eaten before changing level
void Pacman::CheckLevelCompletion() 
{
	if (_level->levelDotCount == 0) {

		Audio::Play(_levelWin); //play sound
		LoadTileMap();
	}
}

//updates the dot animation
void Pacman::UpdateDot(int elapsedTime)
{
	//keeps track of the animation frames for the dots
	for (int dotID = 0; dotID < DOTCOUNT; dotID++)
	{
		_dot[dotID]->currentFrameTime += elapsedTime;
		if (_dot[dotID]->currentFrameTime > _dot[dotID]->frameTime)
		{
			_dot[dotID]->frameCount++;
			if (_dot[dotID]->frameCount >= 2) _dot[dotID]->frameCount = 0;

			_dot[dotID]->currentFrameTime = 0;
		}
	}
}

//checks if a wall collided with pacman
void Pacman::CheckWallCollision(int elapsedTime)
{
	//sets the bounding box for the wall
	int top2 = 0;
	int right2 = 0;
	int bottom2 = 0;
	int left2 = 0;

	//sets the bounding box for pacman
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int left1 = _pacman->position->X;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int top1 = _pacman->position->Y;

	//checks for collision in each wall
	for (int wallID = 0; wallID < WALLCOUNT; wallID++)
	{
		//set bounding box to current position of wall
		bottom2 = _wall[wallID]->position->Y + _wall[wallID]->sourceRect->Height;
		left2 = _wall[wallID]->position->X;
		right2 = _wall[wallID]->position->X + _wall[wallID]->sourceRect->Width;
		top2 = _wall[wallID]->position->Y;

		//if collision is made then check pacman's direction and move it in the oppisite direction
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			if (_pacman->direction == 0) //if direction is left move right
			{
				_pacman->position->X -= _pacman->speedMultiplier * elapsedTime * _pacman->speed;
			}
			else if (_pacman->direction == 1) //if direction is down move up
			{
				_pacman->position->Y -= _pacman->speedMultiplier * elapsedTime * _pacman->speed;
			}
			else if (_pacman->direction == 2) //if direction is right move left
			{
				_pacman->position->X += _pacman->speedMultiplier * elapsedTime * _pacman->speed;
			}
			else if (_pacman->direction == 3) //if direction is up move down
			{
				_pacman->position->Y += _pacman->speedMultiplier * elapsedTime * _pacman->speed;
			}
		}
	}
}

//updates the cherry
void Pacman::UpdateCherry(int elapsedTime)
{
	//keeps track of the animation frames for munchie
	_cherry->currentFrameTime += elapsedTime;
	if (_cherry->currentFrameTime > _cherry->frameTime)
	{
		_cherry->frameCount++;
		if (_cherry->frameCount >= 2) _cherry->frameCount = 0;

		_cherry->currentFrameTime = 0;
	}

	//resets the cherry so it can reload after gaining 10000 points
	if (_gameUI->score % 10000 >= 0 && _gameUI->score > 10000)
	{
		_cherry->cherryEaten = false;
	}
	
}

//draws all objects for the game
void Pacman::Draw(int elapsedTime)
{
	//Creates the text for the user interface
	std::stringstream score;
	score << "Score: " << _gameUI->score << endl;
	std::stringstream highScore;
	score << "Hi-Score: " << _gameUI->highScore << endl;
	std::stringstream lives;
	score << "Lives: " << _pacman->livesLeft << endl;

	SpriteBatch::BeginDraw(); // Starts Drawing

	//draws munchie based on the current animation frame
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchie[i]->texture, _munchie[i]->position, _munchie[i]->sourceRect);
		if (_munchie[i]->frameCount == 0)
		{
			_munchie[i]->sourceRect->X = 0;
		}
		else
		{
			_munchie[i]->sourceRect->X = 32;
		}
	}

	//draws dot based on the current animation frame
	for (int dotID = 0; dotID < DOTCOUNT; dotID++)
	{
		SpriteBatch::Draw(_dot[dotID]->texture, _dot[dotID]->position, _dot[dotID]->sourceRect);
		if (_dot[dotID]->frameCount == 0)
		{
			_dot[dotID]->sourceRect->X = 0;
		}
		else
		{
			_dot[dotID]->sourceRect->X = 32;
		}
	}

	//draws cherry based on the current animation frame
	SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect);
	if (_cherry->frameCount == 0)
	{
		_cherry->sourceRect->X = 0;
	}
	else
	{
		_cherry->sourceRect->X = 32;
	}

	//draws UI text
	SpriteBatch::DrawString(score.str().c_str(), _gameUI->scoreUIPosition, Color::Green);
	SpriteBatch::DrawString(lives.str().c_str(), _gameUI->hiScoreUIPosition, Color::Green);
	SpriteBatch::DrawString(lives.str().c_str(), _gameUI->livesUIPosition, Color::Green);

	//draw walls
	for (int wallID = 0; wallID < WALLCOUNT; wallID++)
	{
		SpriteBatch::Draw(_wall[wallID]->texture, _wall[wallID]->position, _wall[wallID]->sourceRect);
	}

	//draw ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}

	//if pacman is alive draw pacman
	if (!_pacman->isGameOver)
	{
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	}
	//if pacman dead show game over screen
	else
	{
		std::stringstream mainMenuStream;
		mainMenuStream << "Game Over";
		SpriteBatch::Draw(_mainMenu->backgroundTexture, _mainMenu->sourceRect, nullptr);
		SpriteBatch::DrawString(mainMenuStream.str().c_str(), _mainMenu->stringPosition, Color::Yellow);
	}

	//draws main menu
	if (_mainMenu->mainMenuOpen == true)
	{
		std::stringstream mainMenuStream;
		mainMenuStream << "Press Space To Start";
		SpriteBatch::Draw(_mainMenu->backgroundTexture, _mainMenu->sourceRect, nullptr);
		SpriteBatch::DrawString(mainMenuStream.str().c_str(), _mainMenu->stringPosition, Color::Yellow);
	}

	//draws menu
	if (_pauseMenu->isPaused == true)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED";
		SpriteBatch::Draw(_pauseMenu->backgroundTexture, _pauseMenu->sourceRect, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _mainMenu->stringPosition, Color::Magenta);
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}
