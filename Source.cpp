#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>

using namespace std;

// ANALISIS_TM_TP6_FabbriMateo

// CONDITIONS:
/*
	Condiciones mínimas:
		1. Las minas, las banderas, las celdas "inexploradas" y las
			descubiertas (vacías o con números) poseen caracteres que las
			diferencien entre ellas de forma fácil y efectiva.
		2. Cada vez que se empieza una partida nueva, el tablero es
			randomizado.
		3. La cantidad mínima de minas es de 10.
		4. El tamaño del tablero debe ser mínimo de 8x8.

	Condiciones avanzadas:
		1. Hacer que el juego tenga, por lo menos, tres dificultades: Fácil,
			Normal y Difícil. A medida que aumenta la dificultad aumenta el
			tamaño del tablero y la cantidad de minas.
		2. El jugador puede optar por terminar el juego en mitad de la
			partida.
		3. Hacer que haya un contador que muestre la cantidad de minas
			restantes.

	"Ultimate Conditions" :
		1. Hacer que el tablero se genere a partir de la primera selección
			del jugador, y por ende, el primer intento nunca es una mina.
		2. Hacer un cronómetro para que al final el juego indique el tiempo
			transcurrido. NO tiene que mostrarse en pantalla ni actualizarse.
			*mirar ejemplo provisto*
*/

// INDICATIONS:
/*
	- NO se pueden utilizar vectores, colas, pilas o listas. (no es lo mismo
		"vector" que "array").
	- Realizar primero las condiciones mínimas, luego las avanzadas y
		finalmente las "ultimate conditions" en este orden. No se tendrán en
		cuenta las consignas realizadas de un nivel superior a menos que
		primero estén completas las del nivel anterior.
	- El trabajo se considera aprobado una vez que se cumplan todas las
		"condiciones mínimas", es decir que se puede entregar el trabajo con
		sólo la primera parte de las consignas.
*/

#pragma region ENUMS AND STRUCTS
enum COLORS
{
	BlueOnBlack = 9,
	GreenOnBlack = 2,
	RedOnBlack = 4,
	PurpleOnBlack = 5,
	YellowOnBlack = 6,
	CyanOnBlack = 11,
	OrangeOnBlack = 12,
	WhiteOnBlack = 15,
	BlackOnBlue = 16,
	BlackOnGreen = 32,
	BlackOnRed = 64,
	BlackOnPurple = 80,
	WhiteOnPurple = 87,
	BlackOnYellow = 96,
	BlackOnWhite = 240,
	BlueOnWhite = 241,
	GreenOnWhite = 242,
	CyanOnWhite = 243,
	PurpleOnWhite = 245,
	YellowOnWhite = 246,
	OrangeOnWhite = 252,
};

enum KEYS
{
	Back,
	Up,
	Down,
	Left,
	Right,
	Select,
	Flag,
	Cheats,
	ClearFlags,
	Ops,
	Restart,
	Time,
};

enum MODES
{
	Quit,
	Game,
	Options,
	Tutorial,
	MainMenu,
};

enum CHARN
{
	upLeftC,							// ╔
	hor,								// ═
	upT,								// ╦
	upRightC,							// ╗
	ver,								// ║
	leftT,								// ╠
	cross,								// ╬
	rightT,								// ╣
	lowLeftC,							// ╚
	lowT,								// ╩
	lowRightC,							// ╝
};

enum PRESETS
{
	Easy = 1,
	Normal,
	Hard,
	Custom
};

struct POSITION
{
	int x;
	int y;
};

struct CELL
{
	bool opened;
	bool mine;
	bool flagged;
	short mineCount;
};

struct DIFFICULTY
{
	string preset;
	int columns;
	int lines;
	int mines;
};
#pragma endregion

#pragma region VARIABLE DECLARATION
HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);

// Enumerator Variables
MODES Nav = MainMenu;									 // Scene selector
PRESETS difficulty = Easy;								 // Difficulty selector
COLORS numColor[8] = 									 // Mine number colors
{
	BlueOnBlack,
	CyanOnBlack,
	GreenOnBlack,
	PurpleOnBlack,
	YellowOnBlack,
	WhiteOnBlack,
	OrangeOnBlack,
	RedOnBlack
};

// Windows cursor locations
COORD postBoardLoc;										 // Recurrent location below the board
COORD TimePosition;										 // Time print location
COORD FlagNumLoc;										 // Show flags print location

// Max dimension variables
const int maxLengthX = 52;								 // Max array lengths
const int maxLengthY = 25;								 // Max array lengths

COORD curPos;
POSITION cursor;										 // Board cursor
CELL board[maxLengthY][maxLengthX];						 // Max container of board data
DIFFICULTY preset[5] = 									 // Difficulty presets
{
	{},
	{"Easy", 8, 8, 10}, 								 // Easy preset
	{"Normal", 16, 16, 40}, 							 // Normal preset
	{"Hard", 22, 22, 99}, 								 // Hard preset
	{"Custom", 20, 20, 5} 								 // Custom preset
};

// Parameters
int columns = preset[Easy].columns;						 // Amount of columns currently used
int lines = preset[Easy].lines;							 // Amount of lines currently used
int mineQty = preset[Easy].mines;						 // Amount of mines currently used
int flagQty;											 // Amount of flags user has
bool soundOn = false;									 // Turn sound on and off
bool autoFlag = false;									 // Turn autoFlag on and off
bool negFlags = true;									 // Can user keep setting flags

// Ingame modifiable
bool usedCheats = false;								 // Store if user has used cheats
bool cheats = false;									 // Are cheats activated?
bool showKeys = true;									 // Show input to move to each cell
bool boardCreated = false;								 // Is the board created?
bool endGame = true;									 // Switch game state ingame/ended
int soundFreq = 100;									 // Sound frequency multiplier

// Time variables
bool shouldPrintTime = true;							 // should print on-screen time
float gameInitTime = 0.0f;								 // Time at which the game starts
float timeElapsed = 0.0f;								 // Time elapsed since game starts
short finalTime[2];										 // Time counted since game starts and ends
short printTime[2];										 // Time converted to minutes and seconds
short easyBests[3][2]									 // Best times in easy mode
{
	{ 0, 9 },
	{ 0, 10 },
	{ 0, 11 }
};
short normalBests[3][2]									 // Best times in normal mode
{
	{ 1, 04 },
	{ 1, 13 },
	{ 1, 43 }
};
short hardBests[3][2]									 // Best times in hard mode
{
	{ 4, 54 },
	{ 5, 21 },
	{ 9, 06 }
};

// Game controls
char controls[20]{ '0', 'w', 's', 'a', 'd', '1', '2', '3', '4', '5', '6', '7' };

#pragma endregion

#pragma region FUNCTION DECLARING
void SceneManager();									  // Switch between scenes
void RunProgram();										  // Execute the game
void Defaults();										  // Set console defaults
void DrawBoard();										  // Print the whole board
void ResetBoard();										  // Prepare the board for a new game
void RunGame();											  // Main game controller
void CreateBoard();										  // Create a random mine pattern
int SetCellValues(int x, int y);						  // Set the board's cell values
void CheckCell(int x, int y);							  // Open cells
void InGameControls();									  // Move cursor with keys
void Menu();											  // Show menu
bool CheckWinLose();									  // Check for winning/losing conditions
void DrawFirstLine(char wall[]);						  // Print board's roof
void DrawContentLines(char wall[], int y);				  // Print board's content lines
void DrawCellFloors(char wall[], int y);				  // Print each cell's floor and divisions
void DrawColumnNumbers();								  // Prints the first line of numbers for columns
void GameControls();									  // Show game controls and set them
void OptionsMenu();										  // Options menu
void ChangeDimensions();								  // Dimensions menu
void SmartFlag(int x, int y);							  // Smart auto-flag
void FlagClear();										  // Clears all flags
void ErasePrevCursor();									  // Delete previous cursor
COLORS SelectColor();									  // Choose and return a color
COORD BoardLocToConLoc(int x, int y);					  // Translate board coordinates to console coordinates
void DrawCellContent(int x, int y, bool erase);			  // Prints each cell's data
void DrawCursorCases(int x, int y);						  // Prints cell with the cursor on
void DrawCursor();										  // Prints cursor when moved
void GetTime();											  // Calculate and print time
void MovementControls(char key);						  // Execute movement
void MainMechanicsControls(char key);					  // Execute flag and select
void GameStateControls(char key);						  // Execute cheat, options and quit
void RunTutorial();										  // Execute game tutorial
void PrintOnMap(string text);							  // Prints text on board
void MovementTutorial();								  // Start movement tutorial
void TutorialBoard();									  // Create a specific tutorial board
void OpenTutorial();									  // Start the selecting tutorial
void SmartFlagTutorial();								  // Start the smartflag tutorial
#pragma endregion

// Default function
void main()
{
	RunProgram();
}

// Execute the game
void RunProgram()
{
	Defaults();
	SceneManager();
}

// Set console defaults
void Defaults()
{
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	SetConsoleTitle(L"Minesweeper - Fabbri Mateo");
	system("color 07");
	srand(time(nullptr));
	//setlocale(LC_ALL, "spanish");
	//SetConsoleCP(1252);
	//SetConsoleOutputCP(1252); // Input, Output en español.
}

// Main scene selector
void SceneManager()
{
	do
	{
		switch (Nav)
		{
		case MainMenu:
			Menu();
			break;

		case Game:
			RunGame();
			break;

		case Options:
			OptionsMenu();
			break;

		case Tutorial:
			RunTutorial();
			Nav = MainMenu;
			break;

		case Quit:
			break;

		default:
			break;
		}
	} while (Nav != Quit);
}

// Prints given text on board
void PrintOnMap(string text)
{
	short startPopupPosY = 8;
	const short startPopupPosX = 3;
	const short contiguos = 1;
	const int lineLength = 29;
	const short textStart = 4;
	const short textLength = 28;
	const int lineQty = 4;

	char wall[11] = { 201, 205, 203, 187, 186, 204, 206, 185, 200, 202, 188 };					 // Cell Walls 

	curPos = { startPopupPosX, startPopupPosY };
	SetConsoleCursorPosition(hCon, curPos);

	SetConsoleTextAttribute(hCon, WhiteOnPurple);
	cout << wall[upLeftC];
	for (int i = 0; i < lineLength; i++)
	{
		cout << wall[hor];
	}
	cout << wall[upRightC];

	int finishLine[3] = { -1, -1, -1 };

	char line[lineQty][lineLength];

	for (int j = 0; j < lineQty; j++)
	{
		for (int i = 0; i < lineLength; i++)
		{
			line[j][i] = ' ';
		}
	}

	for (int i = lineLength; i >= 0; i--)
	{
		if (i <= text.length())
		{
			if (text[i] == ' ')
			{
				finishLine[0] = i;
				break;
			}
		}
	}

	for (int i = lineLength + finishLine[0] - 1; i >= finishLine[0]; i--)
	{
		if (i <= text.length())
		{
			if (text[i] == ' ')
			{
				finishLine[1] = i;
				break;
			}
		}
	}

	for (int i = lineLength + finishLine[1] - 1; i >= finishLine[1]; i--)
	{
		if (i <= text.length())
		{
			if (text[i] == ' ')
			{
				finishLine[2] = i;
				break;
			}
		}
	}

	for (int i = 0; i < text.length(); i++)
	{
		if (i > finishLine[2] && text.length() > finishLine[1] + lineLength)
		{
			line[3][i - finishLine[2] - 1] = text[i];
		}
		else if (i > finishLine[1] && text.length() > finishLine[0] + lineLength)
		{
			line[2][i - finishLine[1] - 1] = text[i];
		}
		else if (i > finishLine[0] && text.length() > lineLength)
		{
			line[1][i - finishLine[0] - 1] = text[i];
		}
		else
		{
			line[0][i] = text[i];
		}
	}

	for (int i = 0; i < lineQty; i++)
	{
		curPos = { startPopupPosX, (short)(startPopupPosY + i + contiguos) };
		SetConsoleCursorPosition(hCon, curPos);
		cout << wall[ver];
		SetConsoleTextAttribute(hCon, BlackOnPurple);
		for (int pos = 0; pos < lineLength; pos++)
		{
			cout << line[i][pos];
		}
		SetConsoleTextAttribute(hCon, WhiteOnPurple);
		cout << wall[ver];
	}

	curPos = { startPopupPosX, (short)(startPopupPosY + lineQty + contiguos) };
	SetConsoleCursorPosition(hCon, curPos);

	cout << wall[lowLeftC];
	for (int i = 0; i < lineLength; i++)
	{
		cout << wall[hor];
	}
	cout << wall[lowRightC];

	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	curPos = postBoardLoc;
	SetConsoleCursorPosition(hCon, curPos);
	system("pause");

	DrawBoard();
}

// Create a specific tutorial board
void TutorialBoard()
{
	ResetBoard();
	board[0][0].mine = true;
	board[0][1].mine = true;

	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[y][x].mineCount = SetCellValues(x, y);
		}
	}
	boardCreated = true;
}

// Execute tutorial
void RunTutorial()
{
	system("cls");
	cout << R"(The board is a table of modifyable dimentions in each table
position there is a square, and each of them has a chance to contain a
mine. The objective of this game, is to find all mines, while not opening
any of them. In order to do so, we have some tools that will be shown in
this tutorial.

)";
	system("pause");
	char ans;
	mineQty = 2;
	difficulty = Easy;
	columns = preset[difficulty].columns;
	lines = preset[difficulty].lines;
	mineQty = preset[difficulty].mines;
	TutorialBoard();

	system("cls");
	DrawBoard();
	PrintOnMap("Let's start with movement.");
	MovementTutorial();
	PrintOnMap("Good, now let's try opening a cell. Opening a cell will show you it's content.");
	OpenTutorial();
	PrintOnMap("Nice, this won't let you open that cell.");
	PrintOnMap("To win you must open all cells that dont have a mine which technically you did it from the start.");

	SetConsoleCursorPosition(hCon, postBoardLoc);
	SetConsoleTextAttribute(hCon, BlackOnYellow);
	cout << "Do you wish to exit the tutorial? (0/1)";
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << "           " << endl;

	do
	{
		ans = _getch();
		if (ans != '0' && ans != '1')
		{
			PrintOnMap("You can't do that now.");
		}
	} while (ans != '0' && ans != '1');

	if (ans == '1')
		return;

	SmartFlagTutorial();
	mineQty = preset[Easy].mines;

	PrintOnMap("That is all! You know everything you must know. Enjoy!");
}

// Start the smart flag tutorial
void SmartFlagTutorial()
{
	char in;

	FlagClear();
	autoFlag = true;
	PrintOnMap("In this case we have a cell touching one mine that has everything opened except one cell.");
	PrintOnMap("This means that cell is a mine.");
	PrintOnMap("Smart flag function will automatically flag it when you press select on that cell. Let's try.");

	SetConsoleCursorPosition(hCon, postBoardLoc);
	SetConsoleTextAttribute(hCon, BlackOnYellow);
	cout << "Press " << controls[Select] << " on the marked cell";
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << "           " << endl;

	do
	{
		SetConsoleCursorPosition(hCon, BoardLocToConLoc(2, 1));
		SetConsoleTextAttribute(hCon, BlackOnGreen);
		cout << " 1 ";
		SetConsoleCursorPosition(hCon, postBoardLoc);

		in = _getch();
		MovementControls(in);
		if (in != controls[Up] && in != controls[Down] && in != controls[Left] && in != controls[Right])
		{
			PrintOnMap("You can't do that now.");
		}
	} while (cursor.x != 2 || cursor.y != 1);

	PrintOnMap("Right. Now press select button on this cell.");

	do
	{
		in = _getch();
		if (in != controls[Select])
		{
			PrintOnMap("You can't do that now.");
		}
	} while (in != controls[Select]);
	MainMechanicsControls(in);

	PrintOnMap("Great, now as you can see, the cell with the bomb has been marked.");
	PrintOnMap("The same can be done in the opposite case.");
	PrintOnMap("If a cell which marks 1 is touching a flag, you can press it to open all touching cells.");

	autoFlag = false;
}

// Start select tutorial
void OpenTutorial()
{
	char in;

	SetConsoleTextAttribute(hCon, BlackOnYellow);
	cout << "Move to the displayed position" << endl;

	do
	{
		SetConsoleCursorPosition(hCon, BoardLocToConLoc(3, 0));
		SetConsoleTextAttribute(hCon, BlackOnGreen);

		cout << " O ";
		SetConsoleCursorPosition(hCon, postBoardLoc);

		in = _getch();
		MovementControls(in);
		if (in != controls[Up] && in != controls[Down] && in != controls[Left] && in != controls[Right])
		{
			PrintOnMap("You can't do that now.");
		}
	} while (cursor.x != 3 || cursor.y != 0);

	SetConsoleCursorPosition(hCon, postBoardLoc);
	SetConsoleTextAttribute(hCon, BlackOnYellow);
	cout << "Press " << controls[Select] << " to open cell";
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << "           " << endl;

	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	do
	{
		in = _getch();
		if (in != controls[Select])
		{
			PrintOnMap("You can't do that now.");
		}
	} while (in != controls[Select]);

	MainMechanicsControls(in);
	SetConsoleCursorPosition(hCon, postBoardLoc);
	PrintOnMap("Your action resolved in opening many cells.");
	PrintOnMap("This is because when you open a cell that isn't touching any mines, you know all touching cells aren't mines.");
	PrintOnMap("This functionality will repeat until it encounters with a number. Then, you are on your own.");

	SetConsoleCursorPosition(hCon, postBoardLoc);
	SetConsoleTextAttribute(hCon, BlackOnYellow);
	cout << "Try opening one of the cells left";
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << "           " << endl;

	bool inPosition;
	do
	{
		in = _getch();
		MovementControls(in);
		inPosition = ((cursor.x == 0 && cursor.y == 0) || (cursor.x == 1 && cursor.y == 0));
		if (in != controls[Up] && in != controls[Down] && in != controls[Left] && in != controls[Right])
		{
			PrintOnMap("You can't do that now.");
		}

	} while (!inPosition);

	do
	{
		in = _getch();
		MainMechanicsControls(in);
		if (in != controls[Select])
		{
			PrintOnMap("You can't do that now.");
		}

	} while (in != controls[Select]);
	DrawBoard();
	PrintOnMap("The cell you opened was a bomb, this means you lose, but we'll overlook it.");
	PrintOnMap("Now you know that this cell is a bomb. You won't want to open it so you can tag it as bomb.");
	PrintOnMap("To do this you can mark a cell with a flag. Let's try it");

	board[cursor.y][cursor.x].opened = false;
	DrawBoard();

	SetConsoleCursorPosition(hCon, postBoardLoc);
	SetConsoleTextAttribute(hCon, BlackOnYellow);
	cout << "Press " << controls[Flag] << " to mark current cell";
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << "           " << endl;

	do
	{
		in = _getch();
		if (in != controls[Flag])
		{
			PrintOnMap("You can't do that now.");
		}

	} while (in != controls[Flag]);
	MainMechanicsControls(in);
}

// Start movement tutorial
void MovementTutorial()
{
	const int movementQty = 4;
	const short middleLine = 3;
	const short contiguos = 1;
	const short textStart = 0;

	char in;

	PrintOnMap("Try moving around.");

	SetConsoleCursorPosition(hCon, curPos);
	cout << "                               ";

	for (int i = 0; i < movementQty; i++)
	{
		SetConsoleCursorPosition(hCon, curPos);
		SetConsoleTextAttribute(hCon, BlackOnYellow);
		cout << "Use " << controls[Up] << controls[Left] << controls[Down] << controls[Right]
			<< " to move.";
		do
		{
			in = _getch();
			if (in != controls[Up] && in != controls[Left] && in != controls[Down] && in != controls[Right])
			{
				PrintOnMap("You can't do that now.");
			}
		} while (in != controls[Up] && in != controls[Left] && in != controls[Down] && in != controls[Right]);

		ErasePrevCursor();
		MovementControls(in);
		DrawCursor();
	}
}

// Main controller for the gameplay
void RunGame()
{
	boardCreated = false;
	endGame = false;
	usedCheats = false;
	bool doOnce = true;

	ResetBoard();

	system("cls");
	DrawBoard();
	do
	{
		endGame = CheckWinLose();
		if (!endGame)
		{
			if (cheats && boardCreated)
			{
				usedCheats = true;
				if (doOnce)
				{
					doOnce = false;
					DrawBoard();
				}
			}
			InGameControls();
			if (clock() % 1000 == 0)
			{
				GetTime();
			}
		}
	} while (!endGame);
	return;
}

// Calculate and print time
void GetTime()
{
	timeElapsed = clock() - gameInitTime;

	if (boardCreated)
	{
		printTime[0] = timeElapsed / 60000;
		printTime[1] = timeElapsed / 1000 - 60 * printTime[0];
	}

	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	SetConsoleCursorPosition(hCon, TimePosition);

	if (shouldPrintTime)
	{
		cout << "Time: " << (printTime[0] > 9 ? "" : "0") << printTime[0] << ":" << (printTime[1] > 9 ? "" : "0") << printTime[1];
	}
	else
	{
		cout << "               ";
	}
	curPos = postBoardLoc;
	SetConsoleCursorPosition(hCon, curPos);
}

// Print board
void DrawBoard()
{
	char wall[]
	{
		static_cast<char>(201),
		static_cast<char>(205),
		static_cast<char>(203),
		static_cast<char>(187),
		static_cast<char>(186),
		static_cast<char>(204),
		static_cast<char>(206),
		static_cast<char>(185),
		static_cast<char>(200),
		static_cast<char>(202),
		static_cast<char>(188)
	};					 // Cell Walls variables

	curPos = { 0, 0 };
	SetConsoleCursorPosition(hCon, curPos);
	cout << "  ";
	SetConsoleTextAttribute(hCon, BlackOnRed);
	cout << "MINESWEEPER	MATEO FABBRI" << endl;
	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	DrawColumnNumbers();

	DrawFirstLine(wall);

	// Loop for each line
	for (int y = 0; y < lines; y++)
	{
		DrawContentLines(wall, y);

		DrawCellFloors(wall, y);
	}
	cout << endl << endl;

	curPos = { (short)(7 + columns * 4), 4 };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, BlueOnBlack);
	cout << controls[Up] << ": Move up	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, YellowOnBlack);
	cout << controls[Left] << ": Move left	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, PurpleOnBlack);
	cout << controls[Right] << ": Move right	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, GreenOnBlack);
	cout << controls[Down] << ": Move down	" << endl;

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, RedOnBlack);
	cout << controls[Back] << ": Quit		";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, CyanOnBlack);
	cout << controls[Select] << ": Select	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, YellowOnBlack);
	cout << controls[Flag] << ": Flag		";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, RedOnBlack);
	cout << controls[Cheats] << ": Cheats";

	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << (usedCheats ? " *used" : "") << endl;

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, GreenOnBlack);
	cout << controls[ClearFlags] << ": Clear flags	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, PurpleOnBlack);
	cout << controls[Ops] << ": Options	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, OrangeOnBlack);
	cout << controls[Restart] << ": Restart	";

	curPos = { (short)(7 + columns * 4), (short)(curPos.Y + 1) };
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, BlueOnBlack);
	cout << controls[Time] << ": Show/hide time	";

	GetTime();

	SetConsoleCursorPosition(hCon, postBoardLoc);

	// Clean below board
	cout << "                                                                                " << endl
		<< "                                                                                "
		<< "                                                                                "
		<< "                                                                                "
		<< "                                                                                ";
}

// Set all cells' values to 0
void ResetBoard()
{
	const int columnWide = 4;
	const int lineHeight = 2;
	const int boardStartX = 3;
	const int boardStartY = 2;
	const int tabSize = 4;
	const int afterBoardLines = 2;
	const int flagTextLength = 12;

	cursor = { 0, 0 };
	flagQty = mineQty;
	for (int y = 0; y < maxLengthY; y++)
	{
		for (int x = 0; x < maxLengthX; x++)
		{
			board[y][x].mineCount = 0;
			board[y][x].mine = false;
			board[y][x].opened = false;
			board[y][x].flagged = false;
		}
	}

	postBoardLoc = { 0, (short)(lines * lineHeight + boardStartY + afterBoardLines) };
	TimePosition = { (short)(boardStartX + tabSize + columnWide * columns), boardStartY };
	FlagNumLoc =
	{
		(short)(flagTextLength + boardStartX + tabSize + columnWide * columns),
		(short)(boardStartY + lineHeight * lines)
	};

	return;
}

// Select random positions for mines
void CreateBoard()
{
	gameInitTime = clock();

	POSITION random;

	// Plant mines
	for (int i = 0; i < mineQty; i++)
	{
		do
		{
			random.x = rand() % columns;
			random.y = rand() % lines;
		} while (board[random.y][random.x].mine || board[random.y][random.x].opened);

		board[random.y][random.x].mine = true;
	}

	// Burn cell values on board
	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[y][x].mineCount = SetCellValues(x, y);
		}
	}
	DrawBoard();

	return;
}

// Burn cell's values
int SetCellValues(int x, int y)
{
	int counter = 0;
	int minX = x < 1 ? x : x - 1;
	int maxX = x >= columns - 1 ? x : x + 1;
	int minY = y < 1 ? y : y - 1;
	int maxY = y >= lines - 1 ? y : y + 1;

	if (board[y][x].mine)
	{
		return 0;
	}
	else
	{
		for (int j = minY; j <= maxY; j++)
		{
			for (int i = minX; i <= maxX; i++)
			{
				if (j == y && i == x)
				{
					i++;
				}

				if (board[j][i].mine)
				{
					counter++;
				}
			}
		}
	}

	return counter;
}

// Get and execute in-game input
void InGameControls()
{
	if (!_kbhit())
		return;

	char key = _getch();

	if (isupper(key))
		key = tolower(key);

	MovementControls(key);
	MainMechanicsControls(key);
	GameStateControls(key);

	return;
}

// Open selected cell
void CheckCell(int x, int y)
{
	int minX = x < 1 ? x : x - 1;
	int maxX = x >= columns - 1 ? x : x + 1;
	int minY = y < 1 ? y : y - 1;
	int maxY = y >= lines - 1 ? y : y + 1;

	// Create board if not created
	if (!boardCreated)
	{
		boardCreated = true;
		board[y][x].opened = true;
		CreateBoard();
		board[y][x].opened = false;
		CheckCell(x, y);
	}
	// Exit if cell is opened
	else if (board[y][x].opened)
	{
		return;
	}
	// Open mine if selected
	else if (board[y][x].mine)
	{
		board[y][x].opened = true;

	}
	// Stop calling recursion if touching a mine
	else if (board[y][x].mineCount > 0)
	{
		board[y][x].opened = true;
		if (board[y][x].flagged)
		{
			board[y][x].flagged = false;
			flagQty++;
		}
	}
	// Check touching cells
	else
	{
		if (board[y][x].flagged)
		{
			board[y][x].flagged = false;
			flagQty++;
		}
		board[y][x].opened = true;

		for (int j = minY; j <= maxY; j++)
		{
			for (int i = minX; i <= maxX; i++)
			{
				if (i == x && j == y)
				{
					continue;
				}
				else if (i >= 0 && i < columns && j >= 0 && j < lines)
				{
					CheckCell(i, j);
				}
			}
		}
	}

	curPos = BoardLocToConLoc(x, y);
	SetConsoleCursorPosition(hCon, curPos);
	if (x != cursor.x || y != cursor.y)
		DrawCellContent(x, y, false);
	else
		DrawCursorCases(x, y);

	curPos = postBoardLoc;
	SetConsoleCursorPosition(hCon, curPos);
}

// Get menu
void Menu()
{
	char ans;

	system("cls");
	SetConsoleTextAttribute(hCon, BlackOnWhite);
	cout << "                                                                         " << endl
		<< "                                 M E N U                                 " << endl
		<< "                                                                         " << endl << endl;
	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	cout << "0: Exit application" << endl
		<< "1: Play" << endl
		<< "2: Options" << endl
		<< "3: Tutorial" << endl;

	do
	{
		ans = _getch();
		ans -= 48;
	} while (ans > 3 || ans < 0);

	Nav = (MODES)ans;

	return;
}

// Check for winning/losing conditions
bool CheckWinLose()
{
	int openedCounter = 0;

	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			if (board[y][x].mine && board[y][x].opened)
			{
				SetConsoleTextAttribute(hCon, BlackOnRed);
				cout << "KA-BOOM! You lose!" << endl << endl;
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
				if (soundOn)
				{
					Beep(10 * soundFreq, 100);
					Beep(7.5 * soundFreq, 100);
					Beep(5 * soundFreq, 100);
					Beep(2.5 * soundFreq, 100);
					Beep(2 * soundFreq, 100);
				}

				system("pause");
				return true;
			}
			if (board[y][x].opened)
			{
				openedCounter++;
			}
		}
	}

	if (openedCounter == lines * columns - mineQty)
	{
		finalTime[0] = printTime[0];
		finalTime[1] = printTime[1];

		SetConsoleTextAttribute(hCon, GreenOnBlack);
		cout << "Congratulations! You found all " << mineQty << " mines!" << endl
			<< "Your time: " << (finalTime[0] > 9 ? "" : "0") << finalTime[0] << ":" << (finalTime[1] > 9 ? "" : "0") << finalTime[1] << endl;
		SetConsoleTextAttribute(hCon, WhiteOnBlack);

		if (soundOn)
		{
			Beep(7.5 * soundFreq, 100);
			Beep(10 * soundFreq, 100);
			Beep(15 * soundFreq, 100);
			Beep(20 * soundFreq, 750);
		}

		switch (difficulty)
		{
		case Easy:
		{
			if (!usedCheats)
			{
				if (finalTime[0] < easyBests[0][0] || (finalTime[0] == easyBests[0][0] && finalTime[1] < easyBests[0][1]))
				{
					for (int i = 3 - 1; i > 0; i--)
					{
						easyBests[i][0] = easyBests[i - 1][0];
						easyBests[i][1] = easyBests[i - 1][1];
					}
					easyBests[0][0] = finalTime[0];
					easyBests[0][1] = finalTime[1];
				}
				else if (finalTime[0] < easyBests[1][0] || (finalTime[0] == easyBests[1][0] && finalTime[1] < easyBests[1][1]))
				{
					easyBests[2][0] = easyBests[1][0];
					easyBests[2][1] = easyBests[1][1];

					easyBests[1][0] = finalTime[0];
					easyBests[1][1] = finalTime[1];
				}
				else if (finalTime[0] < easyBests[2][0] || (finalTime[0] == easyBests[2][0] && finalTime[1] < easyBests[2][1]))
				{
					easyBests[2][0] = finalTime[0];
					easyBests[2][1] = finalTime[1];
				}
			}

			cout << "Best easy times: " << endl;
			for (int i = 0; i < 3; i++)
			{
				cout << i + 1 << ": " << easyBests[i][0] << ":" << easyBests[i][1] << endl;
			}
		}break;

		case Normal:
		{
			if (!usedCheats)
			{
				if (finalTime[0] < normalBests[0][0] || (finalTime[0] == normalBests[0][0] && finalTime[1] < normalBests[0][1]))
				{
					for (int i = 3 - 1; i > 0; i--)
					{
						normalBests[i][0] = normalBests[i - 1][0];
						normalBests[i][1] = normalBests[i - 1][1];
					}
					normalBests[0][0] = finalTime[0];
					normalBests[0][1] = finalTime[1];
				}
				else if (finalTime[0] < normalBests[1][0] || (finalTime[0] == normalBests[1][0] && finalTime[1] < normalBests[1][1]))
				{
					normalBests[2][0] = normalBests[1][0];
					normalBests[2][1] = normalBests[1][1];

					normalBests[1][0] = finalTime[0];
					normalBests[1][1] = finalTime[1];
				}
				else if (finalTime[0] < normalBests[2][0] || (finalTime[0] == normalBests[2][0] && finalTime[1] < normalBests[2][1]))
				{
					normalBests[2][0] = finalTime[0];
					normalBests[2][1] = finalTime[1];

				}
			}

			cout << "Best normal times: " << endl;
			for (int i = 0; i < 3; i++)
			{
				cout << i + 1 << ": " << normalBests[i][0] << ":" << normalBests[i][1] << endl;
			}
		}break;

		case Hard:
		{
			if (!usedCheats)
			{
				if (finalTime[0] < hardBests[0][0] || (finalTime[0] == hardBests[0][0] && finalTime[1] < hardBests[0][1]))
				{
					for (int i = 3 - 1; i > 0; i--)
					{
						hardBests[i][0] = hardBests[i - 1][0];
						hardBests[i][1] = hardBests[i - 1][1];
					}
					hardBests[0][0] = finalTime[0];
					hardBests[0][1] = finalTime[1];
				}
				else if (finalTime[0] < hardBests[1][0] || (finalTime[0] == hardBests[1][0] && finalTime[1] < hardBests[1][1]))
				{
					hardBests[2][0] = hardBests[1][0];
					hardBests[2][1] = hardBests[1][1];

					hardBests[1][0] = finalTime[0];
					hardBests[1][1] = finalTime[1];
				}
				else if (finalTime[0] < hardBests[2][0] || (finalTime[0] == hardBests[2][0] && finalTime[1] < hardBests[2][1]))
				{
					hardBests[2][0] = finalTime[0];
					hardBests[2][1] = finalTime[1];

				}
			}

			cout << "Best hard times: " << endl;
			for (int i = 0; i < 3; i++)
			{
				cout << i + 1 << ": " << hardBests[i][0] << ":" << hardBests[i][1] << endl;
			}
		}break;

		case Custom:
			break;

		default:
			break;
		}

		system("pause");
		return true;
	}
	else
	{
		return false;
	}
}

// Print top line
void DrawFirstLine(char wall[])
{
	cout << endl << "  ";
	SetConsoleTextAttribute(hCon, BlackOnWhite);

	cout << wall[upLeftC];
	for (int i = 0; i < columns; i++)
	{
		cout << wall[hor] << wall[hor] << wall[hor] << (i >= columns - 1 ? wall[upRightC] : wall[upT]);
	}

	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	cout << "	";
}

// Loop for each cell
void DrawContentLines(char wall[], int y)
{
	cout << endl;

	for (int x = 0; x < columns; x++)
	{
		if (x <= 0)
		{
			cout << y + 1 << (y < 9 ? " " : "");
			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << wall[ver];
		}

		// Cursor cases
		if (x == cursor.x && y == cursor.y)
		{
			DrawCursorCases(cursor.x, cursor.y);
		}
		else
		{
			DrawCellContent(x, y, false);
		}
		SetConsoleTextAttribute(hCon, BlackOnWhite);
		cout << wall[ver];
	}
}

// Horizontal divisions
void DrawCellFloors(char wall[], int y)
{
	cout << endl;

	if (y < lines - 1)
	{
		// Cell divisions
		for (int i = 0; i < columns; i++)
		{
			if (i <= 0)
			{
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
				cout << "  ";
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << wall[leftT];
			}

			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << wall[hor] << wall[hor] << wall[hor] << (i >= columns - 1 ? wall[rightT] : wall[cross]);
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
		}
	}
	else
	{
		// Board floor
		for (int i = 0; i < columns; i++)
		{
			if (i <= 0)
			{
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
				cout << "  ";
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << wall[lowLeftC];
			}

			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << wall[hor] << wall[hor] << wall[hor] << (i >= columns - 1 ? wall[lowRightC] : wall[lowT]);
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
		}
		if (flagQty <= 0)
		{
			SetConsoleTextAttribute(hCon, RedOnBlack);
			cout << "	No flags left!";
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
		}
		else
		{
			cout << "    Flags left: " << flagQty;
		}
	}
}

// Print column numbers
void DrawColumnNumbers()
{
	cout << " ";
	for (int i = 0; i < columns; i++)
	{
		cout << "  " << (i < 9 ? " " : "") << i + 1;
	}
}

// View/set controls
void GameControls()
{
	int page = 1;
	char ans;
	do
	{
		system("cls");
		if (page == 1)
		{
			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << "                                                                         " << endl
				<< "                             C O N T R O L S                             " << endl
				<< "Page 1                                                                   " << endl << endl;

			SetConsoleTextAttribute(hCon, RedOnBlack);
			cout << "1: 'Back' key:                " << controls[Back] << endl << endl;
			SetConsoleTextAttribute(hCon, BlueOnBlack);
			cout << "2: 'Navigate up' key:         " << controls[Up] << endl << endl;
			SetConsoleTextAttribute(hCon, GreenOnBlack);
			cout << "3: 'Navigate down' key:       " << controls[Down] << endl << endl;
			SetConsoleTextAttribute(hCon, YellowOnBlack);
			cout << "4: 'Navigate left' key:       " << controls[Left] << endl << endl;
			SetConsoleTextAttribute(hCon, PurpleOnBlack);
			cout << "5: 'Navigate right' key:      " << controls[Right] << endl << endl;
			SetConsoleTextAttribute(hCon, CyanOnBlack);
			cout << "6: 'Select' key:              " << controls[Select] << endl << endl;
			SetConsoleTextAttribute(hCon, YellowOnBlack);
			cout << "7: 'Flag' key:                " << controls[Flag] << endl << endl;
			SetConsoleTextAttribute(hCon, OrangeOnBlack);
			cout << "8: 'Toggle cheats' key:       " << controls[Cheats] << endl << endl;
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << "9: Page 2" << endl << endl;
			SetConsoleTextAttribute(hCon, RedOnBlack);
			cout << "                               " << endl;
			cout << "0: Back to options menu        " << endl;
			SetConsoleTextAttribute(hCon, WhiteOnBlack);

			ans = _getch();

			system("cls");

			switch (ans)
			{
			case '1':
				cout << "Current: " << controls[Back] << endl
					<< "Press new key for 'Back'";
				controls[Back] = _getch();
				break;

			case '2':
				cout << "Current: " << controls[Up] << endl
					<< "Press new key for 'Navigate up'";
				controls[Up] = _getch();
				break;

			case '3':
				cout << "Current: " << controls[Down] << endl
					<< "Press new key for 'Navigate down'";
				controls[Down] = _getch();
				break;

			case '4':
				cout << "Current: " << controls[Left] << endl
					<< "Press new key for 'Navigate left'";
				controls[Left] = _getch();
				break;

			case '5':
				cout << "Current: " << controls[Right] << endl
					<< "Press new key for 'Navigate right'";
				controls[Right] = _getch();
				break;

			case '6':
				cout << "Current: " << controls[Select] << endl
					<< "Press new key for 'Select'";
				controls[Select] = _getch();
				break;

			case '7':
				cout << "Current: " << controls[Flag] << endl
					<< "Press new key for 'Flag'";
				controls[Flag] = _getch();
				break;

			case '8':
				cout << "Current: " << controls[Cheats] << endl
					<< "Press new key for 'Cheats'";
				controls[Cheats] = _getch();
				break;

			case '9':
				page = 2;
				break;

			default:
				break;
			}
		}
		else
		{
			system("cls");
			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << "                                                                         " << endl
				<< "                             C O N T R O L S                             " << endl
				<< "Page 2                                                                   " << endl << endl;

			SetConsoleTextAttribute(hCon, GreenOnBlack);
			cout << "1: 'Clear flags' key:         " << controls[ClearFlags] << endl << endl;
			SetConsoleTextAttribute(hCon, YellowOnBlack);
			cout << "2: 'Options' key:             " << controls[Ops] << endl << endl;
			SetConsoleTextAttribute(hCon, OrangeOnBlack);
			cout << "3: 'Reset' key:             " << controls[Restart] << endl << endl;
			SetConsoleTextAttribute(hCon, BlueOnBlack);
			cout << "4: 'Time' key:             " << controls[Time] << endl << endl;
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << "9: Page 1" << endl << endl;
			SetConsoleTextAttribute(hCon, RedOnBlack);
			cout << "                               " << endl;
			cout << "0: Back to options menu        " << endl;
			SetConsoleTextAttribute(hCon, WhiteOnBlack);

			ans = _getch();

			system("cls");

			switch (ans)
			{
			case '1':
				cout << "Current: " << controls[ClearFlags] << endl
					<< "Press new key for 'FlagClear'";
				controls[ClearFlags] = _getch();
				break;

			case '2':
				cout << "Current: " << controls[Ops] << endl
					<< "Press new key for 'Options'";
				controls[Ops] = _getch();
				break;

			case '3':
				cout << "Current: " << controls[Restart] << endl
					<< "Press new key for 'Restart'";
				controls[Restart] = _getch();
				break;

			case '4':
				cout << "Current: " << controls[Time] << endl
					<< "Press new key for 'Show time'";
				controls[Time] = _getch();
				break;

			case '9':
				page = 1;

			default:
				break;
			}
		}
	} while (ans != '0');
}

// Options menu
void OptionsMenu()
{
	enum CONFIGS
	{
		ExitOptions,
		EnterGame,
		SetControls,
		Audio,
		BoardKeys,
		AutoFlag,
		Colors,
		Records,
		NegativeFlags,
		Difficulty,
	};

	CONFIGS ConfigOps;

	char ans;

	do
	{
		system("cls");
		SetConsoleTextAttribute(hCon, BlackOnWhite);
		cout << "                                                                         " << endl
			<< "                              O P T I O N S                              " << endl
			<< "                                                                         " << endl << endl;
		SetConsoleTextAttribute(hCon, WhiteOnBlack);

		cout << ExitOptions << ": Back to menu" << endl
			<< EnterGame << ": Play" << endl
			<< SetControls << ": In-game controls" << endl
			<< Audio << ": Audio" << endl
			<< BoardKeys << ": Show navigation keys			Default: ";
		SetConsoleTextAttribute(hCon, GreenOnBlack);
		cout << "ON	";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << "Current: ";
		SetConsoleTextAttribute(hCon, (showKeys ? GreenOnBlack : RedOnBlack));
		cout << (showKeys ? "ON" : "OFF") << endl;
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << AutoFlag << ": \"Smart flag/open\"			Default: ";
		SetConsoleTextAttribute(hCon, RedOnBlack);
		cout << "OFF	";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << "Current: ";
		SetConsoleTextAttribute(hCon, (autoFlag ? GreenOnBlack : RedOnBlack));
		cout << (autoFlag ? "ON" : "OFF") << endl;
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << Colors << ": Change colors" << endl;
		cout << Records << ": View best times" << endl;

		if (endGame)
		{
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << NegativeFlags << ": Unlimited flags			Default: ";
			SetConsoleTextAttribute(hCon, GreenOnBlack);
			cout << "ON	";
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << "Current: ";
			SetConsoleTextAttribute(hCon, (negFlags ? GreenOnBlack : RedOnBlack));
			cout << (negFlags ? "ON" : "OFF") << endl;
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << Difficulty << ": Difficulty" << endl;
		}

		ans = _getch();
		ConfigOps = (CONFIGS)(ans - 48);

		switch (ConfigOps)
		{
		case EnterGame:
			Nav = Game;
			system("cls");
			DrawBoard();
			return;
			break;

		case SetControls:
			GameControls();
			break;

		case Audio:
			do
			{

				system("cls");
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << "                                                                         " << endl
					<< "                                A U D I O                                " << endl
					<< "                                                                         " << endl;
				SetConsoleTextAttribute(hCon, WhiteOnBlack);

				cout << endl << endl << controls[Back] << ": Back" << endl
					<< controls[Select] << ": Sound	Default: ";
				SetConsoleTextAttribute(hCon, RedOnBlack);
				cout << "OFF	";
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
				cout << "Current: ";
				SetConsoleTextAttribute(hCon, (soundOn ? GreenOnBlack : RedOnBlack));
				cout << (soundOn ? "ON" : "OFF") << endl;
				SetConsoleTextAttribute(hCon, WhiteOnBlack);

				cout << controls[Flag] << ": Play sound";

				cout << R"(
                          a     d
Frequency:                < )";
				cout << soundFreq << (soundFreq < 10 ? "  " : (soundFreq < 100 ? " " : "")) << " >" << endl;

				SetConsoleTextAttribute(hCon, BlackOnWhite);
				for (int i = 0; i <= soundFreq; i += 5)
				{
					cout << " ";
				}
				SetConsoleTextAttribute(hCon, WhiteOnBlack);

				ans = _getch();

				if (ans == 'a')
				{
					soundFreq--;
				}
				else if (ans == 'd')
				{
					soundFreq++;
				}
				else if (ans == controls[Select])
				{
					soundOn = !soundOn;
				}
				else if (ans == controls[Flag])
				{
					Beep(1 * soundFreq, 500);
				}
			} while (ans != controls[Back]);
			break;

		case BoardKeys:
			showKeys = !showKeys;
			break;

		case AutoFlag:
			autoFlag = !autoFlag;
			break;

		case NegativeFlags:
			if (endGame)
			{
				negFlags = !negFlags;
			}
			break;

		case Records:
			do
			{
				system("cls");
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << "                                                                         " << endl
					<< "                              R E C O R D S                              " << endl
					<< "                                                                         " << endl << endl;
				SetConsoleTextAttribute(hCon, WhiteOnBlack);

				cout << "0: Back" << endl;
				cout << "1: Easy				Best: "
					<< easyBests[0][0] << ":" << easyBests[0][1] << endl
					<< "2: Normal			Best: "
					<< normalBests[0][0] << ":" << normalBests[0][1] << endl
					<< "3: Hard				Best: "
					<< hardBests[0][0] << ":" << hardBests[0][1] << endl;

				ans = _getch();

				switch (ans)
				{
				case '1':
					system("cls");
					SetConsoleTextAttribute(hCon, BlackOnWhite);
					cout << "                                                                         " << endl
						<< "                         E A S Y   R E C O R D S                         " << endl
						<< "                                                                         " << endl << endl;
					SetConsoleTextAttribute(hCon, WhiteOnBlack);

					for (int i = 0; i < 3; i++)
					{
						cout << i + 1 << ": " << easyBests[i][0] << ":" << easyBests[i][1] << endl;
					}
					system("pause");
					break;

				case '2':
					system("cls");
					SetConsoleTextAttribute(hCon, BlackOnWhite);
					cout << "                                                                         " << endl
						<< "                       N O R M A L   R E C O R D S                       " << endl
						<< "                                                                         " << endl << endl;
					SetConsoleTextAttribute(hCon, WhiteOnBlack);

					for (int i = 0; i < 3; i++)
					{
						cout << i + 1 << ": " << normalBests[i][0] << ":" << normalBests[i][1] << endl;
					}
					system("pause");
					break;

				case '3':
					system("cls");
					SetConsoleTextAttribute(hCon, BlackOnWhite);
					cout << "                                                                         " << endl
						<< "                         H A R D   R E C O R D S                         " << endl
						<< "                                                                         " << endl << endl;
					SetConsoleTextAttribute(hCon, WhiteOnBlack);

					for (int i = 0; i < 3; i++)
					{
						cout << i + 1 << ": " << hardBests[i][0] << ":" << hardBests[i][1] << endl;
					}
					system("pause");
					break;

				default:
					break;
				}
			} while (ans != '0');
			break;

		case Difficulty:
			if (endGame)
			{
				do
				{
					system("cls");
					SetConsoleTextAttribute(hCon, BlackOnWhite);
					cout << "                                                                         " << endl
						<< "                           D I F F I C U L T Y                           " << endl
						<< "                                                                         " << endl << endl;
					SetConsoleTextAttribute(hCon, WhiteOnBlack);

					cout << "0: Back			" << preset[difficulty].preset << endl;
					cout << "1: Easy" << endl
						<< "2: Normal" << endl
						<< "3: Hard" << endl
						<< "4: Custom" << endl;
					do
					{
						ans = _getch();
						ans -= 48;
					} while (ans < 0 || ans > 4);

					if (ans != 0)
					{

						difficulty = (PRESETS)ans;

						columns = preset[difficulty].columns;
						lines = preset[difficulty].lines;
						mineQty = preset[difficulty].mines;

						if (ans == Custom)
						{
							ChangeDimensions();
						}
					}
				} while (ans != 0);
			}
			break;

		case ExitOptions:
			endGame = true;
			Nav = MainMenu;
			break;

		case Colors:
			do
			{

				do
				{
					system("cls");
					SetConsoleTextAttribute(hCon, BlackOnWhite);
					cout << "                                                                         " << endl
						<< "                               C O L O R S                               " << endl
						<< "                                                                         " << endl << endl;
					SetConsoleTextAttribute(hCon, WhiteOnBlack);

					for (int i = 1; i <= 8; i++)
					{
						SetConsoleTextAttribute(hCon, numColor[i]);
						cout << i;
						SetConsoleTextAttribute(hCon, WhiteOnBlack);
						cout << (i % 4 == 0 ? "\n\n" : "	");
					}
					cout << "Press 0 to go back";
					ans = _getch();
					ans -= 48;

					if (ans > 0)
					{
						numColor[(int)ans] = SelectColor();
					}
				} while (ans < 0 || ans > 8);
			} while (ans != 0);
			break;

		default:
			break;
		}
	} while (ConfigOps != ExitOptions);
}

// Dimensions menu
void ChangeDimensions()
{
	char ans;
	do
	{
		system("cls");
		SetConsoleTextAttribute(hCon, 240);
		cout << "                                                                                   " << endl
			<< "                                D I M E N T I O N S                                " << endl
			<< "                                                                                   " << endl << endl;
		SetConsoleTextAttribute(hCon, 15);

		cout << "0: Back" << endl
			<< "1: line amount			Default 8	Current: " << lines << endl
			<< "2: Column amount		Default 8	Current: " << columns << endl
			<< "3: Mine amount			Default 10	Current: " << mineQty << endl;

		ans = _getch();
		switch (ans)
		{
		case '0':
			break;

			// Select amount of lines
		case '1':
			cout << endl << "Enter the amount of lines on the tilemap (8 - " << maxLengthY << "): ";
			do
			{
				cin >> lines;
				cout << "Input out of bounds. Try again: ";
			} while (lines < 8 || lines > maxLengthY);

			while (mineQty >= lines * columns || mineQty < 10)
			{
				cout << "There can't be more mines than cells. Please enter a new amount of mines (10 - " << lines * columns - 1 << "): ";

				cin >> mineQty;
			}

			break;

			// Select amount of columns
		case '2':
			cout << endl << "Enter the amount of columns on the tilemap (8 - " << maxLengthX << "): ";
			do
			{
				cin >> columns;
				cout << "Input out of bounds. Try again: ";
			} while (columns < 8 || columns > maxLengthX);

			if (columns > 25)
			{
				ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
			}

			while (mineQty >= lines * columns || mineQty < 10)
			{
				cout << "There can't be more mines than cells. Please enter a new amount of mines (10 - " << lines * columns - 1 << "): ";

				cin >> mineQty;
			}
			break;

		case '3':
			cout << "Mine quantity (10 - " << lines * columns - 1 << ")		Default: 10	Current: " << mineQty << endl;
			do
			{
				cin >> mineQty;
				cout << "Input out of bounds! Try again: ";
			} while (mineQty >= lines * columns || mineQty < 10);
			break;

		default:
			break;
		}
	} while (ans != '0');

}

// Smart auto-flag
void SmartFlag(int x, int y)
{
	const int firstNotBorderX = 1;
	const int cellUnit = 1;

	int minX = x < firstNotBorderX ? x : x - cellUnit;
	int maxX = x >= columns - cellUnit ? x : x + cellUnit;
	int minY = y < cellUnit ? y : y - cellUnit;
	int maxY = y >= lines - cellUnit ? y : y + cellUnit;
	int openedCounter = 0;
	int flagCounter = 0;
	int contacts = (((maxY + 1) - minY) * ((maxX + 1) - minX) - 1);

	for (int j = minY; j <= maxY; j++)
	{
		for (int i = minX; i <= maxX; i++)
		{
			if (i == x && j == y)
			{
				continue;
			}
			else if (board[j][i].opened)
			{
				openedCounter++;
			}
			else if (board[j][i].flagged)
			{
				flagCounter++;
			}
		}
	}

	if (openedCounter >= contacts - board[y][x].mineCount)
	{
		if (soundOn)
		{
			Beep(4 * soundFreq, 100);
		}
		for (int j = minY; j <= maxY; j++)
		{
			for (int i = minX; i <= maxX; i++)
			{
				if (i == x && j == y)
				{
					continue;
				}
				else if (!board[j][i].opened && flagQty > 0 && !board[j][i].flagged)
				{
					board[j][i].flagged = true;
					curPos = BoardLocToConLoc(i, j);
					SetConsoleCursorPosition(hCon, curPos);
					DrawCellContent(i, j, false);
					flagQty--;
					curPos = FlagNumLoc;
					SetConsoleCursorPosition(hCon, curPos);
					cout << flagQty << "   ";

					curPos = postBoardLoc;
					SetConsoleCursorPosition(hCon, curPos);

				}
			}
		}
	}

	if (flagCounter == board[y][x].mineCount)
	{
		if (soundOn)
		{
			Beep(4 * soundFreq, 100);
		}
		for (int j = minY; j <= maxY; j++)
		{
			for (int i = minX; i <= maxX; i++)
			{
				if (i == x && j == y)
				{
					continue;
				}
				else if (!board[j][i].opened && !board[j][i].flagged)
				{
					CheckCell(i, j);
				}
			}
		}
	}
}

// Clear all flags
void FlagClear()
{
	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[y][x].flagged = false;
			flagQty = mineQty;
		}
	}
	DrawBoard();
}

// Choose and return a color
COLORS SelectColor()
{
	const int colorQty = 15;
	const int lineItems = 5;
	int ans;
	COLORS selected;

	system("cls");
	SetConsoleTextAttribute(hCon, BlackOnWhite);
	cout << "                                                                         " << endl
		<< "                               C O L O R S                               " << endl
		<< "                                                                         " << endl << endl;
	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	cout << "Enter one of the following numbers:" << endl;
	for (int i = 0; i <= colorQty; i++)
	{
		SetConsoleTextAttribute(hCon, i);
		cout << " " << i << " ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << (i % lineItems == 0 ? "\n\n" : "		");
	}

	do
	{
		cin >> ans;
		if (ans < 0 || ans > colorQty)
		{
			cout << "Input out of bounds. Try again: ";
		}
	} while (ans < 0 || ans > colorQty);

	return (COLORS)ans;
}

// Translate Board coordinates to console coordinates
COORD BoardLocToConLoc(int x, int y)
{
	const int boardStartX = 3;
	const int boardStartY = 3;
	const int columnWide = 4;
	const int lineHeight = 2;

	COORD op;
	op.X = (short)(boardStartX + x * columnWide);
	op.Y = (short)(boardStartY + y * lineHeight);
	return op;
}

// Delete cursor when position changes
void ErasePrevCursor()
{
	const int firstColumn = 0;
	const int firstLine = 0;
	const int lastColumn = columns - 1;
	const int lastLine = lines - 1;

	curPos = BoardLocToConLoc(cursor.x, cursor.y);
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, BlackOnWhite);
	DrawCellContent(cursor.x, cursor.y, true);

	if (cursor.y > firstLine)
	{
		curPos = BoardLocToConLoc(cursor.x, cursor.y - 1);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x, cursor.y - 1, true);
	}

	if (cursor.x > firstColumn)
	{
		curPos = BoardLocToConLoc(cursor.x - 1, cursor.y);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x - 1, cursor.y, true);
	}

	if (cursor.x < lastColumn)
	{
		curPos = BoardLocToConLoc(cursor.x + 1, cursor.y);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x + 1, cursor.y, true);
	}

	if (cursor.y < lastLine)
	{
		curPos = BoardLocToConLoc(cursor.x, cursor.y + 1);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x, cursor.y + 1, true);
	}
}

// Print cell's data
void DrawCellContent(int x, int y, bool erase)
{
	char flag = 231;
	char bomb = 208;

	// Bomb case
	if (board[y][x].mine && (board[y][x].opened || cheats))
	{
		SetConsoleTextAttribute(hCon, BlackOnRed);
		cout << " " << bomb << " ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
	}
	// Flag case
	else if (board[y][x].flagged)
	{
		SetConsoleTextAttribute(hCon, BlackOnYellow);
		cout << " " << flag << " ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
	}
	// Empty case
	else if (!board[y][x].opened)
	{
		SetConsoleTextAttribute(hCon, BlackOnWhite);
		cout << " ";
		if (cursor.x == x && cursor.y == y - 1 && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, GreenOnWhite);
			cout << controls[Down];
		}
		else if (cursor.x == x && cursor.y == y + 1 && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, BlueOnWhite);
			cout << controls[Up];
		}
		else if (cursor.x == x - 1 && cursor.y == y && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, PurpleOnWhite);
			cout << controls[Right];
		}
		else if (cursor.x == x + 1 && cursor.y == y && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, YellowOnWhite);
			cout << controls[Left];
		}
		else
		{
			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << " ";
		}
		cout << " ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
	}
	// Closed case
	else if (board[y][x].mineCount > 0)
	{
		SetConsoleTextAttribute(hCon, numColor[board[y][x].mineCount]);
		cout << " " << board[y][x].mineCount << " ";
	}
	// Empty case opened
	else
	{
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << " ";
		if (cursor.x == x && cursor.y == y - 1 && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, GreenOnBlack);
			cout << controls[Down];
		}
		else if (cursor.x == x && cursor.y == y + 1 && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, BlueOnBlack);
			cout << controls[Up];
		}
		else if (cursor.x == x - 1 && cursor.y == y && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, PurpleOnBlack);
			cout << controls[Right];
		}
		else if (cursor.x == x + 1 && cursor.y == y && showKeys && !erase)
		{
			SetConsoleTextAttribute(hCon, YellowOnBlack);
			cout << controls[Left];
		}
		else
		{
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << " ";
		}
		cout << " ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
	}
}

// Print cell which the cursor stands on's data 
void DrawCursorCases(int x, int y)
{
	char bomb = 208;
	char flag = 231;

	// Cursor and counter case
	if (board[y][x].mineCount > 0 && board[y][x].opened)
	{
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << ">";
		SetConsoleTextAttribute(hCon, numColor[board[y][x].mineCount]);
		cout << board[y][x].mineCount;
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << "<";
	}
	else if (board[y][x].mine && (board[y][x].opened || cheats))
	{
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << ">";
		SetConsoleTextAttribute(hCon, BlackOnRed);
		cout << bomb;
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << "<";

	}
	// Cursor and flag case
	else if (board[y][x].flagged)
	{
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << ">";
		SetConsoleTextAttribute(hCon, BlackOnYellow);
		cout << flag;
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << "<";
	}
	// Cursor empty case
	else
	{
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << " X ";
	}
	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	return;
}

// Print cursor in its current position
void DrawCursor()
{
	curPos = BoardLocToConLoc(cursor.x, cursor.y);
	SetConsoleCursorPosition(hCon, curPos);
	DrawCursorCases(cursor.x, cursor.y);

	if (cursor.y > 0)
	{
		curPos = BoardLocToConLoc(cursor.x, cursor.y - 1);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x, cursor.y - 1, false);
	}

	if (cursor.x > 0)
	{
		curPos = BoardLocToConLoc(cursor.x - 1, cursor.y);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x - 1, cursor.y, false);
	}

	if (cursor.x < columns - 1)
	{
		curPos = BoardLocToConLoc(cursor.x + 1, cursor.y);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x + 1, cursor.y, false);
	}

	if (cursor.y < lines - 1)
	{
		curPos = BoardLocToConLoc(cursor.x, cursor.y + 1);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x, cursor.y + 1, false);
	}

	curPos = postBoardLoc;
	SetConsoleCursorPosition(hCon, curPos);
}

// Execute movement
void MovementControls(char key)
{
	if (key == controls[Up])
	{
		ErasePrevCursor();
		if (soundOn)
		{
			Beep(2 * soundFreq, 100);
		}
		if (cursor.y > 0)
		{
			cursor.y--;
		}
		else
		{
			cursor.y = lines - 1;
		}
		DrawCursor();
	}
	else if (key == controls[Down])
	{
		ErasePrevCursor();
		if (soundOn)
		{
			Beep(2 * soundFreq, 100);
		}
		if (cursor.y < lines - 1)
		{
			cursor.y++;
		}
		else
		{
			cursor.y = 0;
		}
		DrawCursor();
	}
	else if (key == controls[Left])
	{
		ErasePrevCursor();
		if (soundOn)
		{
			Beep(2 * soundFreq, 100);
		}
		if (cursor.x > 0)
		{
			cursor.x--;
		}
		else
		{
			cursor.x = columns - 1;
		}
		DrawCursor();
	}
	else if (key == controls[Right])
	{
		ErasePrevCursor();
		if (soundOn)
		{
			Beep(2 * soundFreq, 100);
		}
		if (cursor.x < columns - 1)
		{
			cursor.x++;
		}
		else
		{
			cursor.x = 0;
		}
		DrawCursor();
	}
}

// Execute game mechanics
void MainMechanicsControls(char key)
{
	if (key == controls[Select] && !board[cursor.y][cursor.x].flagged)
	{
		if (!board[cursor.y][cursor.x].opened)
		{
			if (soundOn)
			{
				Beep(3 * soundFreq, 100);
			}
			CheckCell(cursor.x, cursor.y);
		}
		else if (board[cursor.y][cursor.x].mineCount > 0 && autoFlag)
		{
			SmartFlag(cursor.x, cursor.y);
		}
	}
	else if (key == controls[Flag] && !board[cursor.y][cursor.x].opened)
	{
		if (board[cursor.y][cursor.x].flagged)
		{
			if (soundOn)
			{
				Beep(3 * soundFreq, 50);
				Beep(2 * soundFreq, 50);
			}
			flagQty++;
			board[cursor.y][cursor.x].flagged = false;
			curPos = FlagNumLoc;
			SetConsoleCursorPosition(hCon, curPos);
			cout << flagQty << "   ";

			curPos = BoardLocToConLoc(cursor.x, cursor.y);
			SetConsoleCursorPosition(hCon, curPos);
			DrawCursorCases(cursor.x, cursor.y);

			curPos = postBoardLoc;
			SetConsoleCursorPosition(hCon, curPos);
		}
		else if (flagQty > 0 || negFlags)
		{
			if (soundOn)
			{
				Beep(2 * soundFreq, 50);
				Beep(3 * soundFreq, 50);
			}
			flagQty--;
			board[cursor.y][cursor.x].flagged = true;
			curPos = FlagNumLoc;
			SetConsoleCursorPosition(hCon, curPos);
			cout << flagQty << "   ";

			curPos = BoardLocToConLoc(cursor.x, cursor.y);
			SetConsoleCursorPosition(hCon, curPos);
			DrawCursorCases(cursor.x, cursor.y);

			curPos = postBoardLoc;
			SetConsoleCursorPosition(hCon, curPos);
		}
		else
		{
			cout << "No flags left!";
			system("pause");
		}
	}
	else if (key == controls[ClearFlags])
	{
		if (soundOn)
		{
			Beep(5 * soundFreq, 100);
			Beep(5 * soundFreq, 100);
		}
		FlagClear();
	}
}

// Execute playing state controls
void GameStateControls(char key)
{
	if (key == controls[Back])
	{
		if (soundOn)
		{
			Beep(1.5 * soundFreq, 100);
		}
		system("cls");
		cout << "Do you wish to go back to menu?\n0: No\n1: Yes";
		do
		{
			key = _getch();
		} while (key != '0' && key != '1');

		if (key == '1')
		{
			endGame = true;
			Nav = MainMenu;
		}
		else
		{
			DrawBoard();
		}
	}
	else if (key == controls[Cheats])
	{
		if (soundOn)
		{
			Beep(10 * soundFreq, 100);
			Beep(10 * soundFreq, 100);
			Beep(10 * soundFreq, 100);
		}
		cheats = !cheats;
		DrawBoard();
	}
	else if (key == controls[Ops])
	{
		if (soundOn)
		{
			Beep(1.5 * soundFreq, 150);
		}
		OptionsMenu();
	}
	else if (key == controls[Restart])
	{
		if (soundOn)
		{
			Beep(10 * soundFreq, 150);
			Beep(5 * soundFreq, 150);
			Beep(2.5 * soundFreq, 150);
			Beep(5 * soundFreq, 200);
		}

		endGame = true;
	}
	else if (key == controls[Time])
	{
		if (soundOn)
		{
			Beep(3 * soundFreq, 100);
		}
		shouldPrintTime = !shouldPrintTime;
		GetTime();
	}
}