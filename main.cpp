#pragma clang diagnostic push
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-multiway-paths-covered"
#pragma ide diagnostic ignored "cert-msc50-cpp"

#include "main.h"

using namespace std;

void enableAnsiSupport() {
	DWORD dwMode = 0;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

const int COL = 12;
const int ROW = 8;

const int LINE = 7;
const int PILAR = 3;

const int ARROW_UP = 0x48;
const int ARROW_LEFT = 0x4B;
const int ARROW_RIGHT = 0x4D;
const int ARROW_DOWN = 0x50;
const int ESC_KEY = 0x1B;
const int ENTER_KEY = 0x0D;

struct Box {
	char alphabet = ' ';
	bool invisible = false;
};

const int PADDING = 1;

const int MAZE_ROW = ROW + 2 * PADDING;
const int MAZE_COL = COL + 2 * PADDING;

Box** boxes;
bool** maze;

int x = PADDING;
int y = PADDING;

struct Selected {
	int x1;
	int x2;
	int y1;
	int y2;
};

Selected chosen{-1, -1, -1, -1};

char getRandomChar(int range_a = 65, int range_b = 90) {
	return (char) (range_a + (rand() % (range_b - range_a + 1)));
}

const char* NORMAL_ANSI = "\033[38;2;255;255;255m\033[49m";
const char* SELECT_COLOR_ANSI = "\033[48;2;123;23;131m";
const char* CURSOR_COLOR_ANSI = "\033[48;2;255;255;255m\x1B[38;2;0;0;0m";

void drawLine(ostringstream &sstr) {
	sstr << string(LINE, '-');
}

void drawNormalBoxContent(ostringstream &sstr, char c = ' ') {
	if (c == ' ') sstr << string(LINE, ' ');
	else sstr << string(LINE / 2, ' ') << c << string((LINE - 1) / 2, ' ');
}

void drawPilar(ostringstream &sstr) {
	sstr << "|";
}

void MoveCursorTo(COORD coord) {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hStdout, coord);
}

void draw() {
	ostringstream sstr;
	MoveCursorTo({0,0});
	sstr << "S1: " << chosen.x1 << ":" << chosen.y1 << "\nS2: " << chosen.x2 << ":" << chosen.y2 << "\n";
	for (int i = 0; i < MAZE_ROW; i++) {
		for (int j = 0; j < MAZE_COL; j++) {
			sstr << ' ';

//			if ((i == PADDING) && j != 0 && boxes[i - PADDING][j - PADDING].invisible) {
//				drawNormalBoxContent(sstr);
//				continue;
//			}
//			if (i != 0 && j != 0 && j != MAZE_COL - 1) {
//				if (boxes[i - PADDING - 1][j - PADDING].invisible && boxes[i - PADDING][j - PADDING].invisible) {
//					drawNormalBoxContent(sstr);
//				} else {
//					drawLine(sstr);
//				}
//			} else {
//				drawNormalBoxContent(sstr);
//			}
			if (i != 0 && i != MAZE_ROW - 1) {
//				if (i == MAZE_ROW - 2 && boxes[i - PADDING - 1][j - PADDING].invisible) drawNormalBoxContent(sstr);
				if (i == PADDING && boxes[i - PADDING][j - PADDING].invisible) drawNormalBoxContent(sstr);
				else if (i >= 2 && boxes[i - PADDING][j - PADDING].invisible && boxes[i - PADDING - 1][j - PADDING].invisible) drawNormalBoxContent(sstr);
				else if (j != 0 && j != MAZE_COL - 1) drawLine(sstr);
				else drawNormalBoxContent(sstr);
			} else {
				if (j != 0 && j != MAZE_COL - 1 && i == MAZE_ROW - 1 && !boxes[i - PADDING - 1][j - PADDING].invisible) drawLine(sstr);
				else drawNormalBoxContent(sstr);
			}
		}
		sstr << " \n";

		for (int k = 0; k < PILAR; k++) {
			for (int j = 0; j < MAZE_COL; j++) {
				if (i != 0 && i != MAZE_ROW - 1 && j != 0) {
					if (j == MAZE_COL - 1 && boxes[i - PADDING][j - PADDING - 1].invisible) sstr << ' ';
					else if (j == 1 && boxes[i - PADDING][j - PADDING].invisible) sstr << ' ';
					else if (j >= 2 && boxes[i - PADDING][j - PADDING].invisible && boxes[i - PADDING][j - PADDING - 1].invisible) sstr << ' ';
					else drawPilar(sstr);
				} else {
					sstr << ' ';
				}
				if (i >= 1 && j >= 1 && i <= MAZE_ROW - 2 && j <= MAZE_COL - 2 && boxes[i - PADDING][j - PADDING].invisible) {
					if (y == i && x == j) {
						sstr << CURSOR_COLOR_ANSI;
					}
					drawNormalBoxContent(sstr);
					sstr << NORMAL_ANSI;
					continue;
				}
				if (y == i && x == j) {
					sstr << CURSOR_COLOR_ANSI;
				}
				if ((chosen.x1 == j && chosen.y1 == i) || (chosen.x2 == j && chosen.y2 == i)) {
					sstr << SELECT_COLOR_ANSI;
				}
				if (k == PILAR / 2) {
					if (i != 0 && i != MAZE_ROW - 1 && j != 0 && j != MAZE_COL - 1) drawNormalBoxContent(sstr, boxes[i - PADDING][j - PADDING].alphabet);
					else drawNormalBoxContent(sstr);
				} else {
					drawNormalBoxContent(sstr);
				}
				if ((y == i && x == j) || (chosen.x1 == j && chosen.y1 == i) || (chosen.x2 == j && chosen.y2 == i)) {
					sstr << NORMAL_ANSI;
				}
			}
			sstr << " \n";
		}
	}
	cout << sstr.str();
}

void clearBox(int row, int col) {

}

///////////////////////////////////////////////////////////
// Maze zone

void resetMaze() {
	for (int i = 0; i < MAZE_ROW; i++) {
		for (int j = 0; j < MAZE_COL; j++) {
			if (i == 0 || i == MAZE_ROW - 1 || j == 0 || j == MAZE_COL -1) {
				maze[i][j] = true;
			} else {
				maze[i][j] = boxes[i - PADDING][j - PADDING].invisible;
			}
		}
	}
}

const char DIRECTION[] = "DLRU";

// Arrays to represent change in rows and columns
// DOWN, LEFT, RIGHT, UP
// 0 1 2 3
// 0 - 3 DOWN - UP -> 3 - 0 = 3
// 1 - 2 LEFT - RIGHT -> 3 - 2 = 1

const int dr[4] = {1, 0, 0, -1};
const int dc[4] = {0, -1, 1, 0};

bool isValid(int row, int col) {
	return row >= 0 && col >= 0 && row < MAZE_ROW && col < MAZE_COL;
}

struct Coord{
	int x;
	int y;
};

struct QueueNode{
	Coord coord;
	int old_dr;
	int turns;
};


void print2DArray(int a[MAZE_ROW][MAZE_COL], Coord src = {-1, -1}, Coord dist = {-1,-1}) {
	for (int i = 0; i < MAZE_ROW; i++) {
		for (int j = 0; j < MAZE_COL; j++) {
			if (i == dist.y && j == dist.x) cout << "\x1B[48;2;123;23;131m";
			if (i == src.y && j == src.x) cout << "\x1B[48;2;223;23;23m";
			cout << a[i][j] << NORMAL_ANSI << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

// Modified BFS Algorithm
int findPath(Coord src, Coord dest, string &path) {
	if ((src.x == dest.x && abs(src.y - dest.y) == 1) || (src.y == dest.y && abs(src.x - dest.x) == 1)) {
		return 0;
	}
	int c = 0;

	bool checkDest = false;
	for (int i = 0; i < 4; i++) {
		int row = dest.y + dr[i];
		int col = dest.x + dc[i];
		if (isValid(row, col) && maze[row][col]) {
			checkDest = true;
		}
	}
	if (!checkDest) return -1;

	if (!maze[src.y][src.x] || !maze[dest.y][dest.x]) {
		return -1;
	}

	int turnMap[MAZE_ROW][MAZE_COL];
	memset(turnMap, 0, sizeof turnMap);
	turnMap[src.y][src.x] = 0;

	queue<QueueNode> q;
	q.push({src, -1, 0});
	path = " ";

	while (!q.empty()) {
		auto cur = q.front();
		Coord coord = cur.coord;
		if (coord.x == dest.x && coord.y == dest.y) {
			print2DArray(turnMap, src, dest);
			cout << path << '\n';
			return 1;
		}
		q.pop();
		path += " > ";
//		path.pop_back();
		for (int i = 0; i < 4; i++) {
			if (i == 3 - cur.old_dr) {
//			if (i == 3 - cur.old_dr || i == bd_x) {
//			if (i == 3 - cur.old_dr || i == bd_x || i == bd_y) {
				continue;
			}
			int row = coord.y + dr[i];
			int col = coord.x + dc[i];

			if (isValid(row, col) && maze[row][col]) {
				int tPoint = cur.turns;
				if (cur.old_dr != -1 && i != cur.old_dr) tPoint++;
				if (tPoint > 2) {
					continue;
				}
				if (tPoint == 2 && (row != dest.y && col != dest.x)) {
					continue;
				}
				turnMap[row][col] = tPoint;
				path += DIRECTION[i];
				q.push({{col, row}, i, tPoint});
				c++;
			}
		}
	}
	print2DArray(turnMap, src, dest);
	cout << path << "\n";
	return -1;
}

///////////////////////////////////////////////////////////

void DisableResizeWindow() {
	HWND hWnd = GetConsoleWindow();
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
}

void DisableMinimizeButton() {
	HWND hWnd = GetConsoleWindow();
	HMENU hMenu = GetSystemMenu(hWnd, false);

	DeleteMenu(hMenu, SC_MINIMIZE, MF_BYCOMMAND);
}

void project_init() {
	SetConsoleOutputCP(65001);
//	system("mode con COLS=700");
	SetConsoleTitleW(L"Pikachu Game");
	ShowWindow(GetConsoleWindow(),SW_MAXIMIZE);
//	SendMessage(GetConsoleWindow(),WM_SYSKEYDOWN,VK_RETURN,0x20000000); Full screen
	enableAnsiSupport();
	HWND hWnd = GetConsoleWindow();
//	ShowScrollBar(hWnd, SB_BOTH, false);

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);

//	DisableResizeWindow(); //auto resize
	DisableMinimizeButton();
//	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);

	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
	SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

//	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_MINIMIZE, MF_BYCOMMAND);

	boxes = new Box*[ROW];
	for (int i = 0; i < ROW; i++) boxes[i] = new Box[COL];

	maze = new bool*[MAZE_ROW];
	for (int i = 0; i < MAZE_ROW; i++) maze[i] = new bool[MAZE_COL];

	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
			boxes[i][j].alphabet = getRandomChar(65, 70);

	resetMaze();
}

void project_cleanup() {
//	delete[] CLEAR_ANSI;
//	delete[] NORMAL_ANSI;
//	delete[] CURSOR_COLOR_ANSI;
//	delete[] SELECT_COLOR_ANSI;
	system("pause");
}

bool FLAG_RUNNING = true;

int main() {
	project_init();

	const char* s1 = "Meow";
	const char* s2 = string(s1).append(" Goofiest").c_str();
	//cout << s2 << "\n";

	draw();
	unsigned char ch;
	while (FLAG_RUNNING) {
		ch = getch();
		switch (ch) {
			case ARROW_UP:
			case 'w':
				if (y > 0) {
					y--;
					if (y == 0) y = ROW + PADDING - 1;
					draw();
				}
				break;
			case ARROW_LEFT:
			case 'a':
				if (x > 0) {
					x--;
					if (x == 0) x = COL + PADDING - 1;
					draw();
				}
				break;
			case ARROW_RIGHT:
			case 'd':
				if (x < COL + PADDING) {
					x++;
					if (x == COL + PADDING) x = PADDING;
					draw();
				}
				break;
			case ARROW_DOWN:
			case 's':
				if (y < ROW + PADDING) {
					y++;
					if (y == ROW + PADDING) y = PADDING;
					draw();
				}
				break;
			case 'c':
				chosen.x1 = -1;
				chosen.y1 = -1;
				chosen.x2 = -1;
				chosen.y2 = -1;
				draw();
				break;
			case ENTER_KEY:
				if (maze[y][x]) break;
				if (chosen.x1 == -1 && (chosen.x2 != x || chosen.y2 != y)) {
					chosen.x1 = x;
					chosen.y1 = y;
				} else if (x == chosen.x1 && y == chosen.y1) {
					chosen.x1 = -1;
					chosen.y1 = -1;
				} else if (chosen.x2 == -1) {
					chosen.x2 = x;
					chosen.y2 = y;
				} else if (x == chosen.x2 && y == chosen.y2) {
					chosen.x2 = -1;
					chosen.y2 = -1;
				}
				draw();
				if (chosen.x1 != -1 && chosen.x2 != -1) {
					if (boxes[chosen.y1 - PADDING][chosen.x1 - PADDING].alphabet == boxes[chosen.y2 - PADDING][chosen.x2 - PADDING].alphabet) {
						maze[chosen.y1][chosen.x1] = true;
						maze[chosen.y2][chosen.x2] = true;
						string tempString;
						if (findPath(Coord{chosen.x1, chosen.y1}, Coord{chosen.x2, chosen.y2}, tempString) != -1) {
							//cout << tempString << "\n--------------\n";
							Sleep(50);
							boxes[chosen.y1 - PADDING][chosen.x1 - PADDING].invisible = true;
							boxes[chosen.y2 - PADDING][chosen.x2 - PADDING].invisible = true;
						} else {
							Sleep(1);
							maze[chosen.y1][chosen.x1] = false;
							maze[chosen.y2][chosen.x2] = false;
						}
					}
					chosen.x1 = -1;
					chosen.y1 = -1;
					chosen.x2 = -1;
					chosen.y2 = -1;
					draw();
				}
				break;
			case ESC_KEY:
				FLAG_RUNNING = false;
				break;
		}
	}
	project_cleanup();
	return 0;
}