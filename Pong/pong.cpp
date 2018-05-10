#include <conio.h>
#include <iostream>
#include <time.h>
#include <Windows.h>
using namespace std;
enum PongDirectionX {LEFT = -1, STOPX = 0, RIGHT = 1};
enum PongDirectionY {UP = -1, STOPY = 0, DOWN = 1};
class PongBall
{
private:
	int x, y, startX, startY;
	PongDirectionX directionX;
	PongDirectionY directionY;
public:
	PongBall(int x, int y)
	{
		this->x = x;
		this->y = y;
		startX = x;
		startY = y;
		directionX = STOPX;
		directionY = STOPY;
	}
	void move()
	{
		if (directionX == STOPX)
		{
			directionX = (rand() % 2) ? LEFT : RIGHT;
			directionY = (PongDirectionY) ((rand() % 3) - 1);
		}
		x += directionX;
		y += directionY;
	}
	void reset()
	{
		directionX = STOPX;
		directionY = STOPY;
		x = startX;
		y = startY;
	}
	void bounceX() {directionX = (PongDirectionX) (-1 * directionX);}
	void bounceY() {directionY = (PongDirectionY) (-1 * directionY);}
	void pushY(int push)
	{
		int temp = directionY + push;
		if (temp > DOWN)
			temp = DOWN;
		if (temp < UP)
			temp = UP;
		directionY = (PongDirectionY) temp;
	}
	inline int getX() { return x; }
	inline int getY() { return y; }
	friend ostream& operator<<(ostream& out, PongBall* ball)
	{
		out << "[" << ball->x << "," << ball->y << "," << ball->directionX << "," << ball->directionY << "]";
		return out;
	}
};
class PongPaddle
{
private:
	int y, startY;
	string name;
public:
	PongPaddle(int y, string name)
	{
		this->y = y;
		startY = y;
		this->name = name;
	}
	inline int getY() { return y; }
	inline string getName() { return name; }
	void moveUp() { y--; }
	void moveDown() { y++; }
	void reset()
	{
		y = startY;
	}
	friend ostream& operator<<(ostream& out, PongPaddle* paddle)
	{
		out << "[" << paddle->y << "]";
		return out;
	}
};
class PongManager
{
private:
	int width, height;
	int score1, score2;
	char paddle1Up, paddle1Down, paddle2Up, paddle2Down, quit;
	bool gameOver;
	PongBall* ball;
	PongPaddle* paddle1;
	PongPaddle* paddle2;
	int ballX, ballY, p1Y, p2Y;
	char cEmptySpace, cWall, cPaddle, cBall;
	HANDLE hOut;
	void drawInit()
	{
		cEmptySpace = ' ';
		cWall = '\xB2';
		cPaddle = '\xDB';
		cBall = 'o';
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO lpCursor;	
		lpCursor.bVisible = false;
		lpCursor.dwSize = 1;
		SetConsoleCursorInfo(hOut,&lpCursor);
		COORD position;
		position.X = 0;
		position.Y = 0;
		SetConsoleCursorPosition(hOut, position);
		for (int j = 0; j < width + 4; j++)
			printf("%c", cWall);
		printf("\n");
		for (int i = 0; i < height; i++)
		{
			printf("%c%c", cWall, cEmptySpace);
			for (int j = 0; j < width; j++)
			{
				if (i == ball->getY() && j == ball->getX())
					printf("%c", cBall);
				else if (j == 0 && paddle1->getY() - 2 <= i && i <= paddle1->getY() + 2)
					printf("%c", cPaddle);
				else if (j == width - 1 && paddle2->getY() - 2 <= i && i <= paddle2->getY() + 2)
					printf("%c", cPaddle);
				else
					printf("%c", cEmptySpace);
			}
			printf("%c%c\n", cEmptySpace, cWall);
		}
		for (int j = 0; j < width + 4; j++)
			printf("%c", cWall);
		printf("\n%c", cEmptySpace);
		cout << paddle1->getName();
		for (int i = 0; i < width + 2 - paddle1->getName().size() - paddle2->getName().size(); i++)
			printf("%c", cEmptySpace);
		cout << paddle2->getName();
		printf("%c\n%c", cEmptySpace, cEmptySpace);
		for (int i = 0; i < (paddle1->getName().size() - to_string(score1).size()) / 2; i++)
			printf("%c", cEmptySpace);
		printf("%d", score1);
		for (int i = 0; i < paddle1->getName().size() - ((paddle1->getName().size() - to_string(score1).size()) / 2) - to_string(score1).size(); i++)
			printf("%c", cEmptySpace);
		for (int i = 0; i < width + 2 - paddle1->getName().size() - paddle2->getName().size(); i++)
			printf("%c", cEmptySpace);
		for (int i = 0; i < (paddle2->getName().size() - to_string(score2).size()) / 2; i++)
			printf("%c", cEmptySpace);
		printf("%d", score2);
		for (int i = 0; i < paddle2->getName().size() - ((paddle2->getName().size() - to_string(score2).size()) / 2) - to_string(score2).size(); i++)
			printf("%c", cEmptySpace);
		printf("\n");
	}
	void drawPixel(int x, int y, char c)
	{
		COORD position;
		position.X = x + 2;
		position.Y = y + 1;
		SetConsoleCursorPosition(hOut, position);
		printf("%c", c);
	}
	void drawUpdate()
	{
		drawPixel(ballX, ballY, cEmptySpace);
		ballX = ball->getX();
		ballY = ball->getY();
		drawPixel(ballX, ballY, cBall);
		if (paddle1->getY() == p1Y + 1)
		{
			p1Y = paddle1->getY();
			drawPixel(0, p1Y - 3, cEmptySpace);
			drawPixel(0, p1Y + 2, cPaddle);
		}
		else if (paddle1->getY() == p1Y - 1)
		{
			p1Y = paddle1->getY();
			drawPixel(0, p1Y + 3, cEmptySpace);
			drawPixel(0, p1Y - 2, cPaddle);
		}
		if (paddle2->getY() == p2Y + 1)
		{
			p2Y = paddle2->getY();
			drawPixel(width - 1, p2Y - 3, cEmptySpace);
			drawPixel(width - 1, p2Y + 2, cPaddle);
		}
		else if (paddle2->getY() == p2Y - 1)
		{
			p2Y = paddle2->getY();
			drawPixel(width - 1, p2Y + 3, cEmptySpace);
			drawPixel(width - 1, p2Y - 2, cPaddle);
		}
		Sleep(100);
	}
	void score(PongPaddle *paddle)
	{
		if (paddle == paddle1)
			score1++;
		else if (paddle == paddle2)
			score2++;
		ball->reset();
		paddle1->reset();
		paddle2->reset();
		ballX = ball->getX();
		ballY = ball->getY();
		p1Y = paddle1->getY();
		p2Y = paddle2->getY();
		drawInit();
	}
	void input()
	{
		if (_kbhit())
		{
			char input = _getch();
			if (input == paddle1Up)
			{
				if (paddle1->getY() > 2)
					paddle1->moveUp();
			}
			else if (input == paddle1Down)
			{
				if (paddle1->getY() < height - 3)
					paddle1->moveDown();
			}
			else if (input == paddle2Up)
			{
				if (paddle2->getY() > 2)
					paddle2->moveUp();
			}
			else if (input == paddle2Down)
			{
				if (paddle2->getY() < height - 3)
					paddle2->moveDown();
			}
			else if (input == quit)
			{
				gameOver = true;
			}
		}
	}
	void nextMove()
	{
		if (ball->getX() == 0)
		{
			score(paddle2);
			return;
		}
		else if (ball->getX() == width - 1)
		{
			score(paddle1);
			return;
		}
		if (ball->getX() == 1 && abs(paddle1->getY() - ball->getY()) <= 2)
		{
			ball->bounceX();
			ball->pushY(ball->getY() - paddle1->getY());
		}
		else if (ball->getX() == width - 2 && abs(paddle2->getY() - ball->getY()) <= 2)
		{
			ball->bounceX();
			ball->pushY(ball->getY() - paddle2->getY());
		}
		else if (ball->getY() == 0 || ball->getY() == height - 1)
			ball->bounceY();
		ball->move();
	}
public:
	PongManager(int width, int height, string player1, string player2)
	{
		system("cls");
		srand(time(NULL));
		this->width = (2 * width) + 1;
		this->height = (2 * height) + 1;
		score1 = score2 = 0;
		ball = new PongBall(width, height);
		paddle1 = new PongPaddle(height, player1);
		paddle2 = new PongPaddle(height, player2);
		ballX = width;
		ballY = height;
		p1Y = p2Y = height;
		paddle1Up = 'w';
		paddle1Down = 's';
		paddle2Up = 'i';
		paddle2Down = 'k';
		quit = 'q';
		gameOver = false;
		drawInit();
	}
	PongManager(int width, int height) : PongManager(width, height, "Player 1", "Player 2")
	{}
	~PongManager()
	{
		delete ball, paddle1, paddle2;
	}
	void run()
	{
		while (!gameOver)
		{
			drawUpdate();
			input();
			nextMove();
		}
		drawInit();
	}
};
int main(int argc, char const *argv[])
{
	PongManager* manager = new PongManager(30, 10, "ME", "YOU");
	manager->run();
	return 0;
}