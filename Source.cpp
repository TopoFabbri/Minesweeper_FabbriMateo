#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>

using namespace std;

// ANALISIS_TM_TP6_FabbriMateo

// CONDITIONS
/*

*/

#pragma region ENUMS AND STRUCTS
enum COLORS
{
	CyanOnBlack = 1,
	GreenOnBlack = 2,
	RedOnBlack = 4,
	PurpleOnBlack = 5,
	YellowOnBlack = 6,
	BlueOnBlack = 11,
	OrangeOnBlack = 12,
	WhiteOnBlack = 15,
	BlackOnBlue = 24,
	BlackOnRed = 72,
	BlackOnYellow = 104,
	BlackOnWhite = 248,
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

#pragma endregion

#pragma region VARIABLE DECLARATION
void MineCountToColor(int mineCount);					  // Get cell color by number
void PrintMap(POSITION cursor);							  // Print the whole board
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

HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);

// Enumerator Variables
MODES Nav = MainMenu;

// Max dimension variables
const int maxLengthX = 52;
const int maxLengthY = 25;

// Parameters
int columns = 8;
int lines = 8;
int mineQty = 10;

// Ingame modifiable
bool cheats = false;
bool boardCreated = false;
bool endGame;

POSITION cursor;

CELL board[maxLengthX][maxLengthY];
#pragma endregion

// Default function
void main()
{
	srand(time(nullptr));

	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	char ans;

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
			Nav = MainMenu;
			break;

		case Quit:
			break;

		default:
			break;
		}
	} while (Nav != Quit);
	return;
}

// Main controlle for the gameplay
void GameFlow()
{
	boardCreated = false;
	endGame = false;

	ResetBoard();

	do
	{
		system("cls");
		PrintMap(cursor);
		endGame = CheckWinLose();
		if (!endGame)
		{
			InGameControls();
		}
	} while (!endGame);
	return;
}

// Get cell's color by the number it should display
void MineCountToColor(int mineCount)
{
	switch (mineCount)
	{
	case 0:
		SetConsoleTextAttribute(hCon, BlackOnWhite);
		break;

	case 1:
		SetConsoleTextAttribute(hCon, CyanOnBlack);
		break;

	case 2:
		SetConsoleTextAttribute(hCon, BlueOnBlack);
		break;

	case 3:
		SetConsoleTextAttribute(hCon, GreenOnBlack);
		break;

	case 4:
		SetConsoleTextAttribute(hCon, PurpleOnBlack);
		break;

	case 5:
		SetConsoleTextAttribute(hCon, YellowOnBlack);
		break;

	case 6:
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		break;

	case 7:
		SetConsoleTextAttribute(hCon, OrangeOnBlack);
		break;

	case 8:
		SetConsoleTextAttribute(hCon, RedOnBlack);
		break;

	default:
		break;
	}

	return;
}

// Print board
void PrintMap(POSITION cursor)
{
	char wall[11] = { 201, 205, 203, 187, 186, 204, 206, 185, 200, 202, 188 };					 // Cell Walls variables

	DrawColumnNumbers();

	DrawFirstLine(wall);

	// Loop for each line
	for (int y = 0; y < lines; y++)
	{
		DrawContentLines(wall, y);

		DrawCellFloors(wall, y);
	}
	cout << endl;

	return;
}

// Set all cells' values to 0
void ResetBoard()
{
	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[x][y].mineCount = 0;
			board[x][y].mine = false;
			board[x][y].opened = false;
			board[x][y].flagged = false;
		}
	}

	return;
}

// Select random positions for mines
void CreateBoard()
{
	POSITION random;

	// Plant mines
	for (int i = 0; i < mineQty; i++)
	{
		do
		{
			random.x = rand() % columns;
			random.y = rand() % lines;
		} while (board[random.x][random.y].mine || board[random.x][random.y].opened);

		board[random.x][random.y].mine = true;
	}

	// Burn cell values on board
	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[x][y].mineCount = SetCellValues(x, y);
		}
	}

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

	if (board[x][y].mine)
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

				if (board[i][j].mine)
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
	char key = _getch();

	switch (key)
	{
	case 'w':
		if (cursor.y > 0)
		{
			cursor.y--;
		}
		break;

	case 's':
		if (cursor.y < lines - 1)
		{
			cursor.y++;
		}
		break;

	case 'a':
		if (cursor.x > 0)
		{
			cursor.x--;
		}
		break;

	case 'd':
		if (cursor.x < columns - 1)
		{
			cursor.x++;
		}
		break;

	case '0':
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
		break;

	case '1':
		CheckCell(cursor.x, cursor.y);
		break;

	case '2':
		board[cursor.x][cursor.y].flagged = !board[cursor.x][cursor.y].flagged;
		break;

	case '3':
		cheats = !cheats;
		break;

	default:
		break;
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
		board[x][y].opened = true;
		CreateBoard();
	}
	// Exit if cell is opened
	else if (board[x][y].opened)
	{
		return;
	}
	// Open mine if selected
	else if (board[x][y].mine)
	{
		board[x][y].opened = true;
	}
	// Stop calling recursion if touching a mine
	else if (board[x][y].mineCount > 0)
	{
		board[x][y].opened = true;
		return;
	}
	// Check touching cells
	else
	{
		board[x][y].opened = true;

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

	return;
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
			if (board[x][y].mine && board[x][y].opened)
			{
				cout << "KA-BOOM! You lose!" << endl << endl;
				system("pause");
				return true;
			}
			if (board[x][y].opened)
			{
				openedCounter++;
			}
		}
	}

	if (openedCounter == lines * columns - mineQty)
	{
		cout << "Congratulations! You found all " << mineQty << " mines!" << endl << endl;
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

		// Bomb case
		if (board[x][y].mine && (board[x][y].opened || cheats))
		{
			SetConsoleTextAttribute(hCon, BlackOnRed);
			cout << " X ";
		}
		// Cursor cases
		else if (x == cursor.x && y == cursor.y)
		{
			// Cursor and counter case
			if (board[x][y].mineCount > 0 && board[x][y].opened)
			{
				SetConsoleTextAttribute(hCon, BlackOnBlue);
				cout << ">";
				MineCountToColor(board[x][y].mineCount);
				cout << board[x][y].mineCount;
				SetConsoleTextAttribute(hCon, BlackOnBlue);
				cout << "<";
			}
			// Cursor and flag case
			else if (board[x][y].flagged)
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
				cout << "> <";
			}
		}
		// Flag case
		else if (board[x][y].flagged)
		{
			SetConsoleTextAttribute(hCon, BlackOnYellow);
			cout << " ! ";
		}
		// Empty case
		else if (!board[x][y].opened)
		{
			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << "   ";
		}
		// Closed case
		else if (board[x][y].mineCount > 0)
		{
			MineCountToColor(board[x][y].mineCount);
			cout << " " << board[x][y].mineCount << " ";
		}
		// Empty case
		else
		{
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << "   ";
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