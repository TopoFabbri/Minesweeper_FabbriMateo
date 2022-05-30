﻿#include <iostream>
#include <time.h>
#include <Windows.h>
#include <conio.h>

using namespace std;

// ANALISIS_TM_TP6_FabbriMateo

// CONDITIONS
/*
	Condiciones mínimas:
		1. Hacer que el tablero/set de cartas tenga como mínimo 4 pares
			de cartas.
		2. Las cartas deben ser números o caracteres reconocibles/
			fácilmente distinguibles.
		3. Cada vez que se empieza una partida nueva, el tablero es
			randomizado.
		4. El juego se juega en solitario.

	Condiciones avanzadas:
		1. Hacer que el juego tenga, por lo menos, tres dificultades: Fácil,
			Normal y Difícil. A medida que aumenta la dificultad el número
			de pares de cartas también aumenta.
		2. El jugador puede optar por terminar el juego en mitad de la
			partida.

	"Ultimate Conditions" :
		1. Hacer que haya un sistema que cuente la cantidad de jugadas
			que fueron necesarias para completar el juego.
		2. Hacer que en tableros de cantidad de cartas impares exista una
			carta que no posea un par y sólo genere que el jugador haya
			utilizado su turno en vano.
		3. Hacer que el juego tenga una condición de derrota luego de que
			el jugador supere una cierta cantidad de jugadas.
*/

// INDICATIONS
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
	CyanOnBlack = 1,
	GreenOnBlack = 2,
	RedOnBlack = 4,
	PurpleOnBlack = 5,
	YellowOnBlack = 6,
	BlueOnBlack = 9,
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
	Ops,
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
bool showKeys = true;
bool boardCreated = false;
bool endGame = true;

// Game controls
char controls[9]{ '0', 'w', 's', 'a', 'd', '1', '2', '3', 'o' };

POSITION cursor;

CELL board[maxLengthX][maxLengthY];
#pragma endregion

void MineCountToColor(int mineCount);					  // Get cell color by number
void PrintBoard(POSITION cursor);						  // Print the whole board
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

// Default function
void main()
{
	srand(time(nullptr));

	//ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

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

// Main controlle for the gameplay
void GameFlow()
{
	boardCreated = false;
	endGame = false;

	ResetBoard();

	do
	{
		system("cls");
		PrintBoard(cursor);
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
void PrintBoard(POSITION cursor)
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
	cout << endl << endl;
	SetConsoleTextAttribute(hCon, BlueOnBlack);
	cout << controls[Up] << ": Move up	";
	SetConsoleTextAttribute(hCon, YellowOnBlack);
	cout << controls[Left] << ": Move left	";
	SetConsoleTextAttribute(hCon, PurpleOnBlack);
	cout << controls[Right] << ": Move right	";
	SetConsoleTextAttribute(hCon, GreenOnBlack);
	cout << controls[Down] << ": Move down	" << endl;
	SetConsoleTextAttribute(hCon, RedOnBlack);
	cout << controls[Back] << ": Quit		";
	SetConsoleTextAttribute(hCon, CyanOnBlack);
	cout << controls[Select] << ": Select	";
	SetConsoleTextAttribute(hCon, YellowOnBlack);
	cout << controls[Flag] << ": Flag		";
	SetConsoleTextAttribute(hCon, RedOnBlack);
	cout << controls[Cheats] << ": Cheats	" << endl;
	SetConsoleTextAttribute(hCon, WhiteOnBlack);
	cout << controls[Ops] << ": Options	" << endl;

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

	if (key == controls[Up])
	{
		if (cursor.y > 0)
		{
			cursor.y--;
		}
		else
		{
			cursor.y = lines - 1;
		}
	}
	else if (key == controls[Down])
	{
		if (cursor.y < lines - 1)
		{
			cursor.y++;
		}
		else
		{
			cursor.y = 0;
		}
	}
	else if (key == controls[Left])
	{
		if (cursor.x > 0)
		{
			cursor.x--;
		}
		else
		{
			cursor.x = columns - 1;
		}
	}
	else if (key == controls[Right])
	{
		if (cursor.x < columns - 1)
		{
			cursor.x++;
		}
		else
		{
			cursor.x = 0;
		}
	}
	else if (key == controls[Back])
	{
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
	}
	else if (key == controls[Select] && !board[cursor.x][cursor.y].flagged)
	{
		if (!board[cursor.x][cursor.y].opened)
		{
			CheckCell(cursor.x, cursor.y);
		}
		else if (board[cursor.x][cursor.y].mineCount > 0)
		{
			SmartFlag(cursor.x, cursor.y);
		}
	}
	else if (key == controls[Flag] && !board[cursor.x][cursor.y].opened)
	{
		board[cursor.x][cursor.y].flagged = !board[cursor.x][cursor.y].flagged;
	}
	else if (key == controls[Cheats])
	{
		cheats = !cheats;
	}
	else if (key == controls[Ops])
	{
		OptionsMenu();
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

		// Cursor cases
		if (x == cursor.x && y == cursor.y)
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
			else if (board[x][y].mine && (board[x][y].opened || cheats))
			{
				SetConsoleTextAttribute(hCon, BlackOnBlue);
				cout << ">";
				SetConsoleTextAttribute(hCon, BlackOnRed);
				cout << "X";
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
		// Bomb case
		else if (board[x][y].mine && (board[x][y].opened || cheats))
		{
			SetConsoleTextAttribute(hCon, BlackOnRed);
			cout << " X ";
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
			cout << " ";
			if (cursor.x == x && cursor.y == y - 1 && showKeys)
			{
				SetConsoleTextAttribute(hCon, GreenOnWhite);
				cout << "s";
			}
			else if (cursor.x == x && cursor.y == y + 1 && showKeys)
			{
				SetConsoleTextAttribute(hCon, BlueOnWhite);
				cout << "w";
			}
			else if (cursor.x == x - 1 && cursor.y == y && showKeys)
			{
				SetConsoleTextAttribute(hCon, PurpleOnWhite);
				cout << "d";
			}
			else if (cursor.x == x + 1 && cursor.y == y && showKeys)
			{
				SetConsoleTextAttribute(hCon, YellowOnWhite);
				cout << "a";
			}
			else
			{
				SetConsoleTextAttribute(hCon, BlackOnWhite);
				cout << " ";
			}
			cout << " ";
		}
		// Closed case
		else if (board[x][y].mineCount > 0)
		{
			MineCountToColor(board[x][y].mineCount);
			cout << " " << board[x][y].mineCount << " ";
		}
		// Empty case opened
		else
		{
			SetConsoleTextAttribute(hCon, WhiteOnBlack);
			cout << " ";
			if (cursor.x == x && cursor.y == y - 1 && showKeys)
			{
				SetConsoleTextAttribute(hCon, GreenOnBlack);
				cout << "s";
			}
			else if (cursor.x == x && cursor.y == y + 1 && showKeys)
			{
				SetConsoleTextAttribute(hCon, BlueOnBlack);
				cout << "w";
			}
			else if (cursor.x == x - 1 && cursor.y == y && showKeys)
			{
				SetConsoleTextAttribute(hCon, PurpleOnBlack);
				cout << "d";
			}
			else if (cursor.x == x + 1 && cursor.y == y && showKeys)
			{
				SetConsoleTextAttribute(hCon, YellowOnBlack);
				cout << "a";
			}
			else
			{
				SetConsoleTextAttribute(hCon, WhiteOnBlack);
				cout << " ";
			}
			cout << " ";

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

// View/set controls
void GameControls()
{
	char ans;
	do
	{
		system("cls");
		SetConsoleTextAttribute(hCon, BlackOnWhite);
		cout << "                                                                         " << endl
			<< "                             C O N T R O L S                             " << endl
			<< "                                                                         " << endl << endl;
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
		cout << "9: 'Options' key:             " << controls[Ops] << endl << endl;
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << "                               " << endl;
		SetConsoleTextAttribute(hCon, RedOnBlack);
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

		default:
			break;
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
		BoardKeys,
		Dimensions,
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

		cout << "0: Back to menu" << endl
			<< "1: Play" << endl
			<< "2: In-game controls" << endl
			<< "3: Show navigation keys			Default: ";
		SetConsoleTextAttribute(hCon, GreenOnBlack);
		cout << "ON	";
		SetConsoleTextAttribute(hCon, WhiteOnBlack);
		cout << "Current: ";
		SetConsoleTextAttribute(hCon, (showKeys ? GreenOnBlack : RedOnBlack));
		cout << (showKeys ? "ON" : "OFF") << endl;
		SetConsoleTextAttribute(hCon, WhiteOnBlack);


		if (endGame)
		{
			cout << "4: Dimensions" << endl
				<< "		Lines			Default: 8	Current: " << lines << endl
				<< "		Columns			Default: 8	Current: " << columns << endl;
		}

		ans = _getch();
		ConfigOps = (CONFIGS)(ans - 48);

		switch (ConfigOps)
		{
		case EnterGame:
			Nav = Game;
			return;
			break;

		case SetControls:
			GameControls();
			break;

		case BoardKeys:
			showKeys = !showKeys;
			break;

		case Dimensions:
			if (endGame)
			{
				ChangeDimensions();
			}
			break;

		case ExitOptions:
			Nav = MainMenu;
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
			<< "2: Column amount		Default 8	Current: " << columns << endl;

		ans = _getch();
		switch (ans)
		{
		case '0':
			break;

			// Select amount of lines
		case '1':
			cout << endl << "Enter the amount of lines on the tilemap (1 - " << maxLengthY << "): ";
			do
			{
				cin >> lines;
				cout << "Input out of bounds. Try again: ";
			} while (lines < 1 || lines > maxLengthY);
			break;

			// Select amount of columns
		case '2':
			cout << endl << "Enter the amount of columns on the tilemap (1 - " << maxLengthX << "): ";
			do
			{
				cin >> columns;
				cout << "Input out of bounds. Try again: ";
			} while (columns < 1 || columns > maxLengthX);

			if (columns > 25)
			{
				ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
			}
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
	int contacts = (((maxY + 1) - minY) * ((maxX + 1) - minX) - 1);

	for (int j = minY; j <= maxY; j++)
	{
		for (int i = minX; i <= maxX; i++)
		{
			if (i == x && j == y)
			{
				continue;
			}
			else if (board[i][j].opened)
			{
				openedCounter++;
			}
		}
	}

	if (openedCounter >= contacts - board[x][y].mineCount)
	{
		for (int j = minY; j <= maxY; j++)
		{
			for (int i = minX; i <= maxX; i++)
			{
				if (i == x && j == y)
				{
					continue;
				}
				else if (!board[i][j].opened)
				{
					board[i][j].flagged = true;
				}
			}
		}
	}
}