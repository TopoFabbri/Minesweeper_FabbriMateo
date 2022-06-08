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
			diferencien entre ellas de forma fácil y efectiva .
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
	BlackOnRed = 64,
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
};

enum MODES
{
	Quit,
	Game,
	Options,
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
COORD postBoardLoc;
COORD TimePosition;
COORD FlagNumLoc;

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
bool soundOn = true;									 // Turn sound on and off
bool autoFlag = false;									 // Turn autoFlag on and off

// Ingame modifiable
bool usedCheats = false;								 // Store if user has used cheats
bool cheats = false;									 // Are cheats activated
bool showKeys = true;									 // Show input to move to each cell
bool boardCreated = false;								 // Is the board created?
bool endGame = true;									 // Switch game state ingame/ended
int soundFreq = 100;									 // Sound frequency multiplier

// Time variables
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
	{ 5, 21 },
	{ 9, 06 },
	{ 14, 8 }
};

// Game controls
const char defControls[20]{ '0', 'w', 's', 'a', 'd', '1', '2', '3', '4', '5', '6' };
char controls[20]{ '0', 'w', 's', 'a', 'd', '1', '2', '3', '4', '5', '6' };

#pragma endregion

#pragma region FUNCTION DECLARING
void DrawBoard();										  // Print the whole board
void ResetBoard();										  // Prepare the board for a new game
void GameFlow();										  // Main game controller
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
COLORS SelectColor();									  // Choose and return a color
void ErasePrevCursor();									  // Delete previous cursor
COORD BoardLocToConLoc(int x, int y);					  // Translate board coordinates to console coordinates
void DrawCellContent(int x, int y, bool erase);			  // Prints each cell's data
void DrawCursorCases(int x, int y);						  // Prints cell with the cursor on
void DrawCursor();										  // Prints cursor when moved
void GetTime();											  // Calculate and print time
#pragma endregion

// Default function
void main()
{
	srand(time(nullptr));

	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	do
	{
		switch (Nav)
		{
		case MainMenu:
			Menu();
			break;

		case Game:
			GameFlow();
			break;

		case Options:
			OptionsMenu();
			break;

		case Quit:
			break;

		default:
			break;
		}
	} while (Nav != Quit);

	return;
}

// Main controller for the gameplay
void GameFlow()
{
	boardCreated = false;
	endGame = false;
	usedCheats = false;

	ResetBoard();

	system("cls");
	DrawBoard();
	do
	{
		endGame = CheckWinLose();
		if (!endGame)
		{
			if (cheats)
			{
				usedCheats = true;
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
	curPos = TimePosition;
	SetConsoleCursorPosition(hCon, curPos);
	cout << "Time: " << (printTime[0] > 9 ? "" : "0") << printTime[0] << ":" << (printTime[1] > 9 ? "" : "0") << printTime[1];
	curPos = postBoardLoc;
	SetConsoleCursorPosition(hCon, curPos);
}

// Print board
void DrawBoard()
{
	char wall[11] = { 201, 205, 203, 187, 186, 204, 206, 185, 200, 202, 188 };					 // Cell Walls variables

	system("cls");
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
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << controls[Restart] << ": Restart	";

	GetTime();

	return;
}

// Set all cells' values to 0
void ResetBoard()
{
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

	postBoardLoc = { 0, (short)(lines * 2 + 4) };
	TimePosition = { (short)(7 + 4 * columns), 2 };
	FlagNumLoc = { (short)(20 + 4 * columns), (short)(2 + 2 * lines) };

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

	if (key >= 65 && key <= 90)
		key += 32;

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
	else if (key == controls[Back])
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
	else if (key == controls[Select] && !board[cursor.y][cursor.x].flagged)
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

			curPos = postBoardLoc;
			SetConsoleCursorPosition(hCon, curPos);
		}
		else if (flagQty > 0)
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

			curPos = postBoardLoc;
			SetConsoleCursorPosition(hCon, curPos);
		}
		else
		{
			cout << "No flags left!";
			system("pause");
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
	else if (key == controls[ClearFlags])
	{
		if (soundOn)
		{
			Beep(5 * soundFreq, 100);
			Beep(5 * soundFreq, 100);
		}
		FlagClear();
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
		ResetBoard();
		CreateBoard();
	}

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
		<< "2: Options" << endl;

	do
	{
		ans = _getch();
		ans -= 48;
	} while (ans > 2 || ans < 0);

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

			cout << "Best easy times: " << endl;
			for (int i = 0; i < 3; i++)
			{
				cout << i + 1 << ": " << easyBests[i][0] << ":" << easyBests[i][1] << endl;
			}
		}break;

		case Normal:
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

			cout << "Best normal times: " << endl;
			for (int i = 0; i < 3; i++)
			{
				cout << i + 1 << ": " << normalBests[i][0] << ":" << normalBests[i][1] << endl;
			}
		}break;

		case Hard:
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
			cout << "	Flags left: " << flagQty;
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
				cout << "Current: " << controls[Options] << endl
					<< "Press new key for 'Options'";
				controls[Options] = _getch();
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

		if (endGame)
		{
			cout << Difficulty << ": Difficulty" << endl;
		}

		ans = _getch();
		ConfigOps = (CONFIGS)(ans - 48);

		switch (ConfigOps)
		{
		case EnterGame:
			Nav = Game;
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
	int minX = x < 1 ? x : x - 1;
	int maxX = x >= columns - 1 ? x : x + 1;
	int minY = y < 1 ? y : y - 1;
	int maxY = y >= lines - 1 ? y : y + 1;
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
	int ans;
	COLORS selected;

	system("cls");
	SetConsoleTextAttribute(hCon, BlackOnWhite);
	cout << "                                                                         " << endl
		<< "                               C O L O R S                               " << endl
		<< "                                                                         " << endl << endl;
	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	cout << "Enter one of the following numbers:" << endl;
	for (int i = 0; i <= 15; i++)
	{
		SetConsoleTextAttribute(hCon, i);
		cout << " " << i << " ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << (i % 5 == 0 ? "\n\n" : "		");
	}

	do
	{
		cin >> ans;
		if (ans < 0 || ans > 15)
		{
			cout << "Input out of bounds. Try again: ";
		}
	} while (ans < 0 || ans > 15);

	return (COLORS)ans;
}

// Translate Board coordinates to console coordinates
COORD BoardLocToConLoc(int x, int y)
{
	COORD op;
	op.X = (short)(3 + x * 4);
	op.Y = (short)(3 + y * 2);
	return op;
}

// Delete cursor when position changes
void ErasePrevCursor()
{
	curPos = BoardLocToConLoc(cursor.x, cursor.y);
	SetConsoleCursorPosition(hCon, curPos);
	SetConsoleTextAttribute(hCon, BlackOnWhite);
	DrawCellContent(cursor.x, cursor.y, true);

	if (cursor.y > 0)
	{
		curPos = BoardLocToConLoc(cursor.x, cursor.y - 1);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x, cursor.y - 1, true);
	}

	if (cursor.x > 0)
	{
		curPos = BoardLocToConLoc(cursor.x - 1, cursor.y);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x - 1, cursor.y, true);
	}

	if (cursor.x < columns - 1)
	{
		curPos = BoardLocToConLoc(cursor.x + 1, cursor.y);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x + 1, cursor.y, true);
	}

	if (cursor.y < lines - 1)
	{
		curPos = BoardLocToConLoc(cursor.x, cursor.y + 1);
		SetConsoleCursorPosition(hCon, curPos);
		DrawCellContent(cursor.x, cursor.y + 1, true);
	}
}

// Print cell's data
void DrawCellContent(int x, int y, bool erase)
{
	// Bomb case
	if (board[y][x].mine && (board[y][x].opened || cheats))
	{
		SetConsoleTextAttribute(hCon, BlackOnRed);
		cout << " X ";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
	}
	// Flag case
	else if (board[y][x].flagged)
	{
		SetConsoleTextAttribute(hCon, BlackOnYellow);
		cout << " ! ";
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
		cout << "X";
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << "<";

	}
	// Cursor and flag case
	else if (board[y][x].flagged)
	{
		SetConsoleTextAttribute(hCon, BlackOnBlue);
		cout << ">";
		SetConsoleTextAttribute(hCon, BlackOnYellow);
		cout << "!";
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