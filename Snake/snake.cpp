#include <conio.h>
#include <iostream>
#include <queue>
#include <set>
#include <time.h>
#include <Windows.h>

using namespace std;
enum SnakeDirection { LEFT = 0, DOWN = 1, RIGHT = 2, UP = 3 };
class Snake
{
private:
	queue<pair<int, int>> segments, toDraw, toErase;
	int headX, headY;
	SnakeDirection direction;
public:
	Snake(int x, int y)
	{
		headX = x;
		headY = y;
		segments.push({x - 3, y});
		toDraw.push({x - 3, y});
		segments.push({x - 2, y});
		toDraw.push({x - 2, y});
		segments.push({x - 1, y});
		toDraw.push({x - 1, y});
		segments.push({x, y});
		direction = RIGHT;
	}
	pair<int, int> nextSpot()
	{
		int tempHeadX = headX, tempHeadY = headY;
		if (direction == LEFT)
			tempHeadX--;
		else if (direction == RIGHT)
			tempHeadX++;
		else if (direction == UP)
			tempHeadY--;
		else if (direction == DOWN)
			tempHeadY++;
		return {tempHeadX, tempHeadY};
	}
	void move(int foodX, int foodY)
	{
		toDraw.push({headX, headY});
		if (direction == LEFT)
			headX--;
		else if (direction == RIGHT)
			headX++;
		else if (direction == UP)
			headY--;
		else if (direction == DOWN)
			headY++;
		segments.push({headX, headY});
		if (headX != foodX || headY != foodY)
		{
			auto tail = segments.front();
			toErase.push(tail);
			segments.pop();
		}
	}
	void turn(SnakeDirection dir)
	{
		if (direction % 2 != dir % 2)
			direction = dir;
	}
	inline pair<int, int> getHead() { return {headX, headY}; }
	pair<int, int> drawSegment()
	{
		if (toDraw.empty())
			return {-1, -1};
		auto draw = toDraw.front();
		toDraw.pop();
		return draw;
	}
	pair<int, int> eraseSegment()
	{
		if (toErase.empty())
			return {-1, -1};
		auto erase = toErase.front();
		toErase.pop();
		return erase;
	}
	friend ostream& operator<<(ostream& out, Snake* snake)
	{
		while (!snake->segments.empty())
		{
			auto segment = snake->segments.front();
			snake->segments.pop();
			out << "[" << segment.first << "," << segment.second << "]";
		}
		return out;
	}
};
class SnakeManager
{
private:
	int width, height, foodX, foodY, score;
	bool gameOver;
	vector<vector<char>> gridCurrent, gridBuffer;
	char cWall, cSnake, cSnakeHeadLeft, cSnakeHeadRight, cSnakeHeadUp, cSnakeHeadDown, cFood, cEmptySpace;
	char snakeUp, snakeDown, snakeLeft, snakeRight, quit;
	HANDLE hOut;
	set<pair<int, int>> noGoZones;
	Snake* snake;
	void drawInit()
	{
		system("cls");
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO lpCursor;	
		lpCursor.bVisible = false;
		lpCursor.dwSize = 1;
		SetConsoleCursorInfo(hOut,&lpCursor);
		COORD position;
		position.X = 0;
		position.Y = 0;
		SetConsoleCursorPosition(hOut, position);
		cWall = '\xB2';
		cSnake = '\xDB';
		cFood = 'o';
		cEmptySpace = ' ';
		for (int i = 0; i < height; i++)
		{
			vector<char> row, rowbuff;
			gridCurrent.push_back(row);
			gridBuffer.push_back(rowbuff);
			for (int j = 0; j < width; j++)
			{
				if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
				{
					gridCurrent[i].push_back(cWall);
					gridBuffer[i].push_back(cWall);
					printf("%c", cWall);
				}
				else
				{
					gridCurrent[i].push_back(cEmptySpace);
					gridBuffer[i].push_back(cEmptySpace);
					printf("%c", cEmptySpace);
				}
			}
			printf("\n");
		}
		printf("SCORE: %d\n", score);
		pair<int, int> snakeSegment = snake->drawSegment();
		while(snakeSegment.first != -1)
		{
			gridCurrent[snakeSegment.second][snakeSegment.first] = cSnake;
			gridBuffer[snakeSegment.second][snakeSegment.first] = cSnake;
			drawPixel(snakeSegment.first, snakeSegment.second, cSnake);
			snakeSegment = snake->drawSegment();
		}
		snakeSegment = snake->getHead();
		gridCurrent[snakeSegment.second][snakeSegment.first] = cSnake;
		gridBuffer[snakeSegment.second][snakeSegment.first] = cSnake;
		drawPixel(snakeSegment.first, snakeSegment.second, cSnake);
		setDrawPosition(0, height);
	}
	void setDrawPosition(int x, int y)
	{
		COORD position;
		position.X = x;
		position.Y = y;
		SetConsoleCursorPosition(hOut, position);
	}
	void drawPixel(int x, int y, char c)
	{
		setDrawPosition(x, y);
		printf("%c", c);
	}
	void printDebug(string s)
	{
		cerr << s << endl;
	}
	void drawUpdate()
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (gridBuffer[y][x] != gridCurrent[y][x])
				{
					drawPixel(x, y, gridCurrent[y][x]);
					gridBuffer[y][x] = gridCurrent[y][x];
				}
			}
		}
		setDrawPosition(0, height);
		printf("SCORE: %d\n", score);
	}
	void createNextFood()
	{
		if (foodX < 0 || foodX >= width || foodY < 0 || foodY >= height)
		{
			foodX = rand() % width;
			foodY = rand() % height;
		}
		if (gridCurrent[foodY][foodX] == cSnake)
		{
			score++;
		}
		while(gridCurrent[foodY][foodX] != cEmptySpace && gridCurrent[foodY][foodX] != cFood)
		{
			foodX = rand() % width;
			foodY = rand() % height;
		}
		gridCurrent[foodY][foodX] = cFood;
	}
	void logic()
	{
		pair<int, int> nextSnakeHead = snake->nextSpot();
		int nextX = nextSnakeHead.first, nextY = nextSnakeHead.second;
		char nextBlock = gridCurrent[nextY][nextX];
		if (nextBlock != cEmptySpace && nextBlock != cFood)
		{
			gameOver = true;
		}
		else
		{
			snake->move(foodX, foodY);
			pair<int, int> update = snake->drawSegment();
			while (update.first != -1)
			{
				gridCurrent[update.second][update.first] = cSnake;
				update = snake->drawSegment();
			}
			update = snake->getHead();
			gridCurrent[update.second][update.first] = cSnake;
			update = snake->eraseSegment();
			while (update.first != -1)
			{
				gridCurrent[update.second][update.first] = cEmptySpace;
				update = snake->eraseSegment();
			}
			createNextFood();
		}
	}
	void input()
	{
		if (_kbhit())
		{
			char inputChar = _getch();
			if (inputChar == snakeUp)
			{
				snake->turn(UP);
			}
			else if (inputChar == snakeLeft)
			{
				snake->turn(LEFT);
			}
			else if (inputChar == snakeDown)
			{
				snake->turn(DOWN);
			}
			else if (inputChar == snakeRight)
			{
				snake->turn(RIGHT);
			}
			else if (inputChar == quit)
			{
				gameOver = true;
			}
		}
	}
public:
	SnakeManager(int width, int height)
	{
		srand(time(NULL));
		this->width = width;
		this->height = height;
		gameOver = false;
		snake = new Snake((width / 2) + 1, height / 2);
		snakeUp = 'w';
		snakeLeft = 'a';
		snakeDown = 's';
		snakeRight = 'd';
		quit = 'q';
		score = 0;
		drawInit();
		drawUpdate();
	}
	~SnakeManager()
	{
		delete snake;
		for (int i = 0; i < height; i++)
		{
			gridCurrent[i].clear();
			gridBuffer[i].clear();
		}
		gridCurrent.clear();
		gridBuffer.clear();
	}
	void run()
	{
		while (!gameOver)
		{
			input();
			logic();
			drawUpdate();
			Sleep(100);
		}
	}
};
int main()
{
	// Snake *snake = new Snake(25, 30);
    // cout << snake << endl;
	SnakeManager* manager = new SnakeManager(100, 50);
	manager->run();
    return 0;
}
