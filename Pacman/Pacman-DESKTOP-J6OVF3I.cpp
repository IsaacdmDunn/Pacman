#include "Pacman.h"
#include <sstream>
#include <time.h>

//constructor method
Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	//uses time method to provide a seed for random functions to help mitigate the issues of pseudo randomness
	srand(time(NULL));

	//itnitialise munchies
	int i;
	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Munchie();
		//munchie struct values
		_munchie[i]->frameCount;
		_munchie[i]->frameTime = 500;
		_munchie[i]->currentFrameTime= rand() % 500;
	}

	//pacman struct values
	_pacman = new Player(); 
	_pacman->currentFrameTime = 0;
	_pacman->direction = 0;
	_pacman->frame = 0;
	_pacman->frameTime = 250;
	_pacman->speedMultiplier = 0.1f;

	
	//_munchie->currentFrameTime = 0;

	//pause menu struct values
	_pauseMenu = new PauseMenu();
	_pauseMenu->stringPosition;
	_pauseMenu->isPaused = false;
	_pauseMenu->pKeyDown = false;

	//main menu struct values
	_mainMenu = new MainMenu();
	_mainMenu->stringPosition;
	_mainMenu->mainMenuOpen = true;

	//user interface struct values
	_gameUI = new UserInterface();

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	// Clean up pointers within the Pacman structure 
	delete _pacman->position;
	delete _pacman->sourceRect;
	delete _pacman->texture;

	delete _munchie[0]->texture;
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		delete _munchie[i]->sourceRect;
	}
	delete[] _munchie;

	delete _mainMenu->backgroundTexture;
	delete _mainMenu->sourceRect;

	delete _pauseMenu->backgroundTexture;
	delete _pauseMenu->sourceRect;

	// Clean up the Pacman structure pointer 
	delete _pacman;
	delete _munchie;
	delete _mainMenu;
	delete _pauseMenu;
	delete _gameUI;

	
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.tga", false);
	
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

	Texture2D* munchieTexture = new Texture2D();
	munchieTexture->Load("Textures/Munchie.png", true);

	// Load Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->texture = munchieTexture;
		_munchie[i]->position = new Vector2(rand() % Graphics::GetViewportWidth(),
			rand() % Graphics::GetViewportHeight());
		_munchie[i]->sourceRect = new Rect(0.0f, 0.0f, 12, 12);
	}
	

	// Set string position
	_gameUI->_stringPosition = new Vector2(10.0f, 25.0f);
}

//updates the game
void Pacman::Update(int elapsedTime)
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	
	// Gets the current state of the keyboard
	if (_pauseMenu->isPaused == false)
	{
		//manages input for the game
		Input(elapsedTime, keyboardState);
		CheckPaused(keyboardState);

		CheckViewportCollision();

		UpdatePacman(elapsedTime);
		UpdateMunchie(elapsedTime);
	}
}

//manages input for the game
void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState)
{
	// Checks if D key is pressed
	if (keyboardState->IsKeyDown(Input::Keys::D)) {
		_pacman->position->X += _pacman->speedMultiplier * elapsedTime; //Moves Pacman across X axis
		_pacman->direction = 0;
	}
	// Checks if S key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::S)) {
		_pacman->position->Y += _pacman->speedMultiplier * elapsedTime; //Moves Pacman across Y axis
		_pacman->direction = 1;
	}
	// Checks if A key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::A)) {
		_pacman->position->X -= _pacman->speedMultiplier * elapsedTime; //Moves Pacman across X axis
		_pacman->direction = 2;
	}
	// Checks if W key is pressed
	else if (keyboardState->IsKeyDown(Input::Keys::W)) {
		_pacman->position->Y -= _pacman->speedMultiplier * elapsedTime; //Moves Pacman across Y axis
		_pacman->direction = 3;
	}
}

//manages the input for menus and pausing the game
void Pacman::CheckPaused(Input::KeyboardState* keyboardState)
{
	// Checks if P key is pressed
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

void Pacman::CheckViewportCollision()
{
	//checks if pacman is going off-screen
	if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth())
	{
		//pacman hit left wall - reset to right wall
		_pacman->position->X = -32.0f + _pacman->sourceRect->Width;
	}
	//checks if pacman is going off-screen
	if (_pacman->position->X - _pacman->sourceRect->Width < -32.0f)
	{
		//pacman hit right wall - reset to left wall
		_pacman->position->X = Graphics::GetViewportWidth() - _pacman->sourceRect->Width;
	}
	//checks if pacman is going off-screen
	if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight())
	{
		//pacman hit up wall - reset to down wall
		_pacman->position->Y = -32.0f + _pacman->sourceRect->Height;
	}
	//checks if pacman is going off-screen
	if (_pacman->position->Y - _pacman->sourceRect->Height < -32.0f)
	{
		//pacman hit down wall - reset to up wall
		_pacman->position->Y = Graphics::GetViewportHeight() - _pacman->sourceRect->Height;
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

//updates the pacman sprite based on the frame rate
void Pacman::UpdateMunchie(int elapsedTime)
{
	//keeps track of the animation frames for munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i]->currentFrameTime += elapsedTime;
		if (_munchie[i]->currentFrameTime > _munchie[i]->frameTime)
		{
			_munchie[i]->frameCount++;
			if (_munchie[i]->frameCount >= 2) _munchie[i]->frameCount = 0;

			_munchie[i]->currentFrameTime = 0;
		}
	}
	
}

//draws all objects for the game
void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->position->X << " Y: " << _pacman->position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing

	//draws menu
	if (_pauseMenu->isPaused == true)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED";

		SpriteBatch::Draw(_pauseMenu->backgroundTexture, _pauseMenu->sourceRect, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _mainMenu->stringPosition, Color::Magenta);
	}
	
	//draws munchie
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchie[i]->texture, _munchie[i]->position, _munchie[i]->sourceRect);
		if (_munchie[i]->frameCount == 0)
		{
			_munchie[i]->sourceRect->X = 0;
		}
		else
		{
			_munchie[i]->sourceRect->X = 12;
		}
	}
	
	SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	
	SpriteBatch::DrawString(stream.str().c_str(), _gameUI->_stringPosition, Color::Green); // Draws String

	//draws main menu
	if (_mainMenu->mainMenuOpen == true)
	{
		std::stringstream mainMenuStream;
		mainMenuStream << "Press Space To Start";

		SpriteBatch::Draw(_mainMenu->backgroundTexture, _mainMenu->sourceRect, nullptr);
		SpriteBatch::DrawString(mainMenuStream.str().c_str(), _mainMenu->stringPosition, Color::Yellow);
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}