#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>

using namespace std;

// ANALISIS_TM_TP6_FabbriMateo

// CONDITIONS
/*

*/

struct POSITION
{
	int x;
	int y;
};

void MineCountToColor(int mineCount);					  // Get cell color by number
void PrintMap(POSITION cursor);							  // Print the whole board
void ResetBoard();										  // Prepare the board for a new game
void GameFlow();										  // Main game controller
void CreateBoard();										  // Create a random mine pattern
int SetCellValues(int x, int y);						  // Set the board's cell values
void CheckCell(int x, int y);							  // 
void InGameControls();				  // Move cursor with keys

HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);

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

struct CELL
{
	bool opened;
	bool mine;
	int mineCount;
};

enum MODES
{
	Quit,
	Menu,
	Game,
	Options,
};

MODES Nav = Game;

const int maxLengthX = 40;
const int maxLengthY = 25;

int columns = 8;
int lines = 8;
int mineQty = 10;

bool endGame;

POSITION cursor;

CELL board[maxLengthX][maxLengthY];

int main()
{
	srand(time(nullptr));

	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	do
	{
		switch (Nav)
		{
		case Menu:
			break;

		case Game:
			GameFlow();
			break;

		case Options:
			break;

		default:
			break;

		case Quit:
			break;
		}
	} while (Nav != Quit);
}

void GameFlow()
{
	endGame = false;

	ResetBoard();
	CreateBoard();

	do
	{
		system("cls");
		PrintMap(cursor);
		InGameControls();
	} while (!endGame);
	return;
}

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
}

void PrintMap(POSITION cursor)
{
	// Cell Walls variables
	char rightWallT = 185;						// ╣
	char verWall = 186;							// ║
	char upRightC = 187;						// ╗
	char lowRightC = 188;						// ╝
	char lowLeftC = 200;						// ╚
	char upLeftC = 201;							// ╔
	char lowWallT = 202;						// ╩
	char upWallT = 203;							// ╦
	char leftWallT = 204;						// ╠
	char horWall = 205;							// ═
	char cross = 206;							// ╬

	// Print line numbers
	cout << " ";
	for (int i = 0; i < columns; i++)
	{
		cout << "  " << (i < 9 ? " " : "") << i + 1;
	}

	cout << endl << "  ";
	SetConsoleTextAttribute(hCon, BlackOnWhite);
	cout << upLeftC;

	// Print top line
	for (int i = 0; i < columns; i++)
	{
		cout << horWall << horWall << horWall << (i >= columns - 1 ? upRightC : upWallT);
	}
	SetConsoleTextAttribute(hCon, WhiteOnBlack);

	// Loop for each line
	for (int y = 0; y < lines; y++)
	{
		cout << endl;

		// Loop for each cell
		for (int x = 0; x < columns; x++)
		{
			if (x <= 0)
			{
				cout << y + 1 << (y < 9 ? " " : "");
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << verWall;
			}

			if (x == cursor.x && y == cursor.y)
			{
				if (board[x][y].mineCount > 0 && board[x][y].opened)
				{
					SetConsoleTextAttribute(hCon, BlackOnBlue);
					cout << " ";
					MineCountToColor(board[x][y].mineCount);
					cout << board[x][y].mineCount;
					SetConsoleTextAttribute(hCon, BlackOnBlue);
					cout << " ";
				}
				else
				{
					SetConsoleTextAttribute(hCon, BlackOnBlue);
					cout << "   ";
				}
			}
			else if (!board[x][y].opened)
			{
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << "   ";
			}
			else if (board[x][y].mine)
			{
				SetConsoleTextAttribute(hCon, BlackOnRed);
				cout << "   ";
			}
			else if (board[x][y].mineCount > 0)
			{
				MineCountToColor(board[x][y].mineCount);
				cout << " " << board[x][y].mineCount << " ";
			}
			else
			{
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
				cout << "   ";
			}
			SetConsoleTextAttribute(hCon, BlackOnWhite);
			cout << verWall;
		}

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
					cout << leftWallT;
				}

				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << horWall << horWall << horWall << (i >= columns - 1 ? rightWallT : cross);
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
					cout << lowLeftC;
				}

				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << horWall << horWall << horWall << (i >= columns - 1 ? lowRightC : lowWallT);
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
			}
		}
	}
	cout << endl;
}

void ResetBoard()
{
	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[x][y].mineCount = 0;
			board[x][y].mine = false;
			board[x][y].opened = false;
		}
	}
}

void CreateBoard()
{
	int posX;
	int posY;

	for (int i = 0; i < mineQty; i++)
	{
		posX = rand() % columns;
		posY = rand() % lines;

		if (board[posX][posY].mine)
		{
			i--;
		}
		else
		{
			board[posX][posY].mine = true;
		}
	}

	for (int y = 0; y < lines; y++)
	{
		for (int x = 0; x < columns; x++)
		{
			board[x][y].mineCount = SetCellValues(x, y);
		}
	}
}

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

		return counter;
	}
}

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
			Nav = Menu;
		}
		break;

	case '1':
		CheckCell(cursor.x, cursor.y);
		break;

	default:
		break;
	}
}

void CheckCell(int x, int y)
{
	int minX = x < 1 ? x : x - 1;
	int maxX = x >= columns - 1 ? x : x + 1;
	int minY = y < 1 ? y : y - 1;
	int maxY = y >= lines - 1 ? y : y + 1;

	if (board[x][y].mine || board[x][y].opened)
	{
		return;
	}
	else if (board[x][y].mineCount > 0)
	{
		board[x][y].opened = true;
		return;
	}
	else
	{
		board[x][y].opened = true;

		for (int j = minY; j <= maxY; j++)
		{
			for (int i = minX; i <= maxX; i++)
			{
				if (i == x && j == y)
				{
					i++;
				}
				CheckCell(i, j);
			}
		}
		return;
	}
}