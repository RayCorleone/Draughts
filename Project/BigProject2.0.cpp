#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8
#define EMPTY 0
#define MY_FLAG 2
#define MY_KING 4
#define ENEMY_FLAG 1
#define ENEMY_KING 3
//基础棋盘信息

typedef int BOOL;
#define TRUE 1
#define FALSE 0
//Bool判断

#define MAX_STEP 15
//最大步数

#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"
//棋局相关指令

struct Command
{
	int x[MAX_STEP];
	int y[MAX_STEP];
	int numStep;
};//下棋指令的打包，成为Command的新类型

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };
//棋盘初始化

int myFlag;
//我的棋子——用数字表示，X-2，*-4.

int moveDir[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };//移动
//左下1，右下2，左上3，右上4
int jumpDir[4][2] = { {2, -2}, {2, 2}, {-2, -2}, {-2, 2} };//吃子
//左下1，右下2，左上3，右上4

int numMyFlag;
//我棋子的个数

struct Command moveCmd;
//移动的结构体
struct Command jumpCmd;
//吃子的结构体
struct Command longestJumpCmd;
//最长吃子的结构体

void debug(const char* str)
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}//排除故障，尽量不要用

void printBoard()
{
	char visualBoard[BOARD_SIZE][BOARD_SIZE + 1] = { 0 };
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			switch (board[i][j])
			{
			case EMPTY:
				visualBoard[i][j] = '.';
				break;
			case ENEMY_FLAG:
				visualBoard[i][j] = 'O';
				break;
			case MY_FLAG:
				visualBoard[i][j] = 'X';
				break;
			case ENEMY_KING:
				visualBoard[i][j] = '@';
				break;
			case MY_KING:
				visualBoard[i][j] = '*';
				break;
			default:
				break;
			}
		}
		printf("%s\n", visualBoard[i]);
	}
}//打印棋盘的函数

BOOL isInBound(int x, int y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

void rotateCommand(struct Command* cmd)
{
	if (myFlag == ENEMY_FLAG)
	{
		for (int i = 0; i < cmd->numStep; i++)
		{
			cmd->x[i] = BOARD_SIZE - 1 - cmd->x[i];
			cmd->y[i] = BOARD_SIZE - 1 - cmd->y[i];
		}
	}
}//旋转输出

int tryToMove(int x, int y)
{
	int newX, newY;
	for (int i = 0; i < board[x][y]; i++)
	{
		newX = x + moveDir[i][0];
		newY = y + moveDir[i][1];
		if (isInBound(newX, newY) && board[newX][newY] == EMPTY)
		{
			moveCmd.x[0] = x;
			moveCmd.y[0] = y;
			moveCmd.x[1] = newX;
			moveCmd.y[1] = newY;
			return i;
		}
	}
	return -1;
}//移动棋子

void place(struct Command cmd)
{
	int midX, midY, curFlag;
	curFlag = board[cmd.x[0]][cmd.y[0]];
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		board[cmd.x[i]][cmd.y[i]] = EMPTY;
		board[cmd.x[i + 1]][cmd.y[i + 1]] = curFlag;
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			midX = (cmd.x[i] + cmd.x[i + 1]) / 2;
			midY = (cmd.y[i] + cmd.y[i + 1]) / 2;
			if ((board[midX][midY] & 1) == 0)
			{
				numMyFlag--;
			}
			board[midX][midY] = EMPTY;
		}
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (board[0][i] == ENEMY_FLAG)
		{
			board[0][i] = ENEMY_KING;
		}
		if (board[BOARD_SIZE - 1][i] == MY_FLAG)
		{
			board[BOARD_SIZE - 1][i] = MY_KING;
		}
	}
}//落子

void initAI(int me)
{
	numMyFlag = 12;
}

//*****************************个人代码起点*****************************//

/*side：1是我，2是敌方*/

#define INF 100000
double wfr = 0, Dfr = 0, wss = 0, Ds = 0, wch = 0, Dch = 0, Dcent = 0, wcent = 0, Ddef = 0, wdef = 0, Dcont = 0, wcont = 0;
int MySet = 7;
int TheBestMove = 0;

int KingVal = 8;
int PawnVal = 4;

double Cent = 1;
double Defend = 1;
double Cheng = 1;
double Value = 2;
double Val1Bi = 1;
double Val2Bi = 1;

typedef struct NODE
{
	int x;
	int y;
	int state;//兵2，王4
	int height;
};//存储一个棋子信息的结构

struct NODE w_t_p[10][12];
//用来储存不同层度多组不同的可行棋子

struct Command Movelist[10][70];
//用来储存不同层度多种可行的走法

void MYtryToJump(int x, int y, int currentStep)
{
	int newX, newY, midX, midY;
	int tmpFlag;
	jumpCmd.x[currentStep] = x;
	jumpCmd.y[currentStep] = y;
	jumpCmd.numStep++;
	for (int i = 0; i < 4; i++)
	{
		newX = x + jumpDir[i][0];
		newY = y + jumpDir[i][1];
		midX = (x + newX) / 2;
		midY = (y + newY) / 2;
		if (isInBound(newX, newY) && ((board[midX][midY] & 1) != 0) && (board[newX][newY] == EMPTY))
		{
			board[newX][newY] = board[x][y];
			board[x][y] = EMPTY;
			tmpFlag = board[midX][midY];
			board[midX][midY] = EMPTY;
			MYtryToJump(newX, newY, currentStep + 1);
			board[x][y] = board[newX][newY];
			board[newX][newY] = EMPTY;
			board[midX][midY] = tmpFlag;
		}
	}
	if (jumpCmd.numStep > longestJumpCmd.numStep)
	{
		memcpy(&longestJumpCmd, &jumpCmd, sizeof(struct Command));
	}
	jumpCmd.numStep--;
}//我方吃子

void EMtryToJump(int x, int y, int currentStep)
{
	int newX, newY, midX, midY;
	int tmpFlag;
	jumpCmd.x[currentStep] = x;
	jumpCmd.y[currentStep] = y;
	jumpCmd.numStep++;
	for (int i = 0; i < 4; i++)
	{
		newX = x + jumpDir[i][0];
		newY = y + jumpDir[i][1];
		midX = (x + newX) / 2;
		midY = (y + newY) / 2;
		if (isInBound(newX, newY) && (board[midX][midY] % 2 == 0) && (board[midX][midY] != 0) && (board[newX][newY] == EMPTY))
		{
			board[newX][newY] = board[x][y];
			board[x][y] = EMPTY;
			tmpFlag = board[midX][midY];
			board[midX][midY] = EMPTY;
			EMtryToJump(newX, newY, currentStep + 1);
			board[x][y] = board[newX][newY];
			board[newX][newY] = EMPTY;
			board[midX][midY] = tmpFlag;
		}
	}
	if (jumpCmd.numStep > longestJumpCmd.numStep)
	{
		memcpy(&longestJumpCmd, &jumpCmd, sizeof(struct Command));
	}
	jumpCmd.numStep--;
}//敌方吃子

int MYcaneat(int i, int j)
{
	int value = 0;
	if (i + 2 < 8 && j - 2 >= 0 && board[i + 1][j - 1] % 2 == 1 && board[i + 2][j - 2] == 0)
		value = value | (0x1 << 1);//左下
	if (i + 2 < 8 && j + 2 < 8 && board[i + 1][j + 1] % 2 == 1 && board[i + 2][j + 2] == 0)
		value = value | (0x1 << 2);//右下
	if (i - 2 >= 0 && j - 2 >= 0 && board[i - 1][j - 1] % 2 == 1 && board[i - 2][j - 2] == 0)
		value = value | (0x1 << 3);//左上
	if (i - 2 >= 0 && j + 2 < 8 && board[i - 1][j + 1] % 2 == 1 && board[i - 2][j + 2] == 0)
		value = value | (0x1 << 4);//右上
	return value;
}

int EMcaneat(int i, int j)
{
	int value = 0;
	if (i + 2 < 8 && j - 2 >= 0 && board[i + 1][j - 1] % 2 == 0 && board[i + 1][j - 1] != 0 && board[i + 2][j - 2] == 0)
		value = value | (0x1 << 1);//左下
	if (i + 2 < 8 && j + 2 < 8 && board[i + 1][j + 1] % 2 == 0 && board[i + 1][j + 1] != 0 && board[i + 2][j + 2] == 0)
		value = value | (0x1 << 2);//右下
	if (i - 2 >= 0 && j - 2 >= 0 && board[i - 1][j - 1] % 2 == 0 && board[i - 1][j - 1] != 0 && board[i - 2][j - 2] == 0)
		value = value | (0x1 << 3);//左上
	if (i - 2 >= 0 && j + 2 < 8 && board[i - 1][j + 1] % 2 == 0 && board[i - 1][j + 1] != 0 && board[i - 2][j + 2] == 0)
		value = value | (0x1 << 4);//右上
	return value;
}

int MYEatcount(int x, int y)
{
	int t1, t2;
	int h = 0;
	int h1 = 0;
	int h2 = 0;
	int h3 = 0;
	int h4 = 0;
	int c = MYcaneat(x, y);

	if (c == 0)
		return 0;

	if ((c & 2) == 2)//左下方可以吃
	{
		t1 = board[x][y];
		t2 = board[x + 1][y - 1];
		board[x][y] = 0;
		board[x + 1][y - 1] = 0;
		board[x + 2][y - 2] = t1;
		h1 = MYEatcount(x + 2, y - 2);//递归
		board[x][y] = t1;
		board[x + 1][y - 1] = t2;
		board[x + 2][y - 2] = 0;//复原
	}
	else
		h1 = 0;

	if ((c & 4) == 4)//右下方可以吃
	{
		t1 = board[x][y];
		t2 = board[x + 1][y + 1];
		board[x][y] = 0;
		board[x + 1][y + 1] = 0;
		board[x + 2][y + 2] = t1;
		h1 = MYEatcount(x + 2, y + 2);//递归
		board[x][y] = t1;
		board[x + 1][y + 1] = t2;
		board[x + 2][y + 2] = 0;//复原
	}
	else
		h2 = 0;

	if ((c & 8) == 8)//左上方可以吃
	{
		t1 = board[x][y];
		t2 = board[x - 1][y - 1];
		board[x][y] = 0;
		board[x - 1][y - 1] = 0;
		board[x - 2][y - 2] = t1;
		h1 = MYEatcount(x - 2, y - 2);//递归
		board[x][y] = t1;
		board[x - 1][y - 1] = t2;
		board[x - 2][y - 2] = 0;//复原
	}
	else
		h3 = 0;

	if ((c & 16) == 16)//右上方可以吃
	{
		t1 = board[x][y];
		t2 = board[x - 1][y + 1];
		board[x][y] = 0;
		board[x - 1][y + 1] = 0;
		board[x - 2][y + 2] = t1;
		h1 = MYEatcount(x - 2, y + 2);//递归
		board[x][y] = t1;
		board[x - 1][y + 1] = t2;
		board[x - 2][y + 2] = 0;//复原
	}
	else
		h4 = 0;

	h = h1;
	if (h <= h2)
		h = h2;
	if (h <= h3)
		h = h3;
	if (h <= h4)
		h = h4;

	return h + 1;
}

int EMEatcount(int x, int y)
{
	int t1, t2;
	int h = 0;
	int h1 = 0;
	int h2 = 0;
	int h3 = 0;
	int h4 = 0;
	int c = EMcaneat(x, y);

	if (c == 0)
		return 0;

	if ((c & 2) == 2)//左下方可以吃
	{
		t1 = board[x][y];
		t2 = board[x + 1][y - 1];
		board[x][y] = 0;
		board[x + 1][y - 1] = 0;
		board[x + 2][y - 2] = t1;
		h1 = EMEatcount(x + 2, y - 2);//递归
		board[x][y] = t1;
		board[x + 1][y - 1] = t2;
		board[x + 2][y - 2] = 0;//复原
	}
	else
		h1 = 0;

	if ((c & 4) == 4)//右下方可以吃
	{
		t1 = board[x][y];
		t2 = board[x + 1][y + 1];
		board[x][y] = 0;
		board[x + 1][y + 1] = 0;
		board[x + 2][y + 2] = t1;
		h1 = EMEatcount(x + 2, y + 2);//递归
		board[x][y] = t1;
		board[x + 1][y + 1] = t2;
		board[x + 2][y + 2] = 0;//复原
	}
	else
		h2 = 0;

	if ((c & 8) == 8)//左上方可以吃
	{
		t1 = board[x][y];
		t2 = board[x - 1][y - 1];
		board[x][y] = 0;
		board[x - 1][y - 1] = 0;
		board[x - 2][y - 2] = t1;
		h1 = EMEatcount(x - 2, y - 2);//递归
		board[x][y] = t1;
		board[x - 1][y - 1] = t2;
		board[x - 2][y - 2] = 0;//复原
	}
	else
		h3 = 0;

	if ((c & 16) == 16)//右上方可以吃
	{
		t1 = board[x][y];
		t2 = board[x - 1][y + 1];
		board[x][y] = 0;
		board[x - 1][y + 1] = 0;
		board[x - 2][y + 2] = t1;
		h1 = EMEatcount(x - 2, y + 2);//递归
		board[x][y] = t1;
		board[x - 1][y + 1] = t2;
		board[x - 2][y + 2] = 0;//复原
	}
	else
		h4 = 0;

	h = h1;
	if (h <= h2)
		h = h2;
	if (h <= h3)
		h = h3;
	if (h <= h4)
		h = h4;

	return h + 1;
}

int MYPawncanmove(int i, int j)
{
	int value = 0;
	if (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0)
		value = value | (0x1 << 1);//左下
	if (i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0)
		value = value | (0x1 << 2);//右下
	return value;
}

int EMPawncanmove(int i, int j)
{
	int value = 0;
	if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0)
		value = value | (0x1 << 3);//左上
	if (i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0)
		value = value | (0x1 << 4);//右上
	return value;
}

int Kingcanmove(int i, int j)
{
	int value = 0;
	if (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0)
		value = value | (0x1 << 1);//左下
	if (i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0)
		value = value | (0x1 << 2);//右下
	if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0)
		value = value | (0x1 << 3);//左上
	if (i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0)
		value = value | (0x1 << 4);//右上
	return value;
}

void FindCMP(int side1,int depth)//找到可走的棋子,side中,我为1，敌为2.
{
	struct NODE temp = { 0,0,0,0 };
	int geshu = 0;
	for (geshu = 0; geshu < 12; geshu++)
	{
		w_t_p[depth][geshu].height = 0;
		w_t_p[depth][geshu].state = 0;
		w_t_p[depth][geshu].x = 0;
		w_t_p[depth][geshu].y = 0;
	}//储存可走棋子的结构组的初始化

	int i, j;
	geshu = 0;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (side1 == 1)
			{
				if (board[i][j] == 2)
				{
					temp.height = MYEatcount(i, j);
					temp.x = i;
					temp.y = j;
					temp.state = 2;
				}

				if (board[i][j] == 4)
				{
					temp.height = MYEatcount(i, j);
					temp.x = i;
					temp.y = j;
					temp.state = 4;
				}

				if (board[i][j] == 2 || board[i][j] == 4)
				{
					if (geshu == 0)
					{
						w_t_p[depth][0] = temp;
						geshu = geshu + 1;
					}
					else
					{
						if (w_t_p[depth][0].height < temp.height)
						{
							for (geshu = 0; geshu < 12; geshu++)
							{
								w_t_p[depth][geshu].height = 0;
								w_t_p[depth][geshu].state = 0;
								w_t_p[depth][geshu].x = 0;
								w_t_p[depth][geshu].y = 0;
							}//清空现有的走法
							geshu = 0;
							w_t_p[depth][0] = temp;
							geshu = geshu + 1;
						}

						if (w_t_p[depth][0].height == temp.height)
						{
							w_t_p[depth][geshu] = temp;
							geshu = geshu + 1;
						}
					}
				}//用w_t_p记录可以合法出行棋子的信息
			}//计算我方的。
			else
			{
				if (board[i][j] == 1)
				{
					temp.height = EMEatcount(i, j);
					temp.x = i;
					temp.y = j;
					temp.state = 1;
				}

				if (board[i][j] == 3)
				{
					temp.height = EMEatcount(i, j);
					temp.x = i;
					temp.y = j;
					temp.state = 3;
				}

				if (board[i][j] == 1 || board[i][j] == 3)
				{
					if (geshu == 0)
					{
						w_t_p[depth][0] = temp;
						geshu = geshu + 1;
					}
					else
					{
						if (w_t_p[depth][0].height < temp.height)
						{
							for (geshu = 0; geshu < 12; geshu++)
							{
								w_t_p[depth][geshu].height = 0;
								w_t_p[depth][geshu].state = 0;
								w_t_p[depth][geshu].x = 0;
								w_t_p[depth][geshu].y = 0;
							}//清空现有的走法
							geshu = 0;
							w_t_p[depth][0] = temp;
							geshu = geshu + 1;
						}

						if (w_t_p[depth][0].height == temp.height)
						{
							w_t_p[depth][geshu] = temp;
							geshu = geshu + 1;
						}
					}
				}//用w_t_p记录可以合法出行棋子的信息
			}//计算敌方的。
		}
	}//对所有的棋子进行了一次吃子数的判断

	if (w_t_p[depth][0].height == 0)//没有一个可以吃,只能移动
	{
		for (geshu = 0; geshu < 12; geshu++)
		{
			w_t_p[depth][geshu].height = 0;
			w_t_p[depth][geshu].state = 0;
			w_t_p[depth][geshu].x = 0;
			w_t_p[depth][geshu].y = 0;
		}//储存可走棋子的结构组的重新初始化

		geshu = 0;
		for (i = 0; i < 8; i++)
		{
			for (j = 0; j < 8; j++)
			{
				if (side1 == 1)
				{
					if (board[i][j] == 2)
					{
						if (MYPawncanmove(i, j) != 0)
						{
							w_t_p[depth][geshu].x = i;
							w_t_p[depth][geshu].y = j;
							w_t_p[depth][geshu].state = 2;
							geshu = geshu + 1;
						}
					}

					if (board[i][j] == 4)
					{
						if (Kingcanmove(i, j) != 0)
						{
							w_t_p[depth][geshu].x = i;
							w_t_p[depth][geshu].y = j;
							w_t_p[depth][geshu].state = 4;
							geshu = geshu + 1;
						}
					}
				}//我方的
				else
				{
					if (board[i][j] == 1)
					{
						if (EMPawncanmove(i, j) != 0)
						{
							w_t_p[depth][geshu].x = i;
							w_t_p[depth][geshu].y = j;
							w_t_p[depth][geshu].state = 1;
							geshu = geshu + 1;
						}
					}

					if (board[i][j] == 3)
					{
						if (Kingcanmove(i, j) != 0)
						{
							w_t_p[depth][geshu].x = i;
							w_t_p[depth][geshu].y = j;
							w_t_p[depth][geshu].state = 3;
							geshu = geshu + 1;
						}
					}
				}//敌方的
			}
		}
	}
}//记录的是可走棋子的基本信息，以及它可以吃子的个数

void FindATM(int side, int depth)//找到某一层度所有的走法，返回一堆Command的Movelist结构体组
{
	int MoveCount = 0;
	int PieceCount = 0;
	int i = 0;
	for (i = 0; i < 70; i++)
	{
		Movelist[depth][i].x[0] = 0;
		Movelist[depth][i].y[0] = 0;
		Movelist[depth][i].numStep = 0;
	}//走法数组的初始化

	FindCMP(side,depth);

	i = 0;
	while (w_t_p[depth][i].state != 0)
	{
		PieceCount = PieceCount + 1;
		i = i + 1;
	}//计算有多少个可行棋子

	MoveCount = 0;
	for (i = 0; i < PieceCount; i++)
	{
		if (w_t_p[depth][0].height == 0)//只可以移动
		{			
			if (side == 1)
			{
				if (w_t_p[depth][i].state == 2)
				{
					int DIRE = MYPawncanmove(w_t_p[depth][i].x, w_t_p[depth][i].y);
					if ((DIRE & 2) == 2)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x + 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y - 1;
						MoveCount = MoveCount + 1;
					}//左下可走
					if ((DIRE & 4) == 4)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x + 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y + 1;
						MoveCount = MoveCount + 1;
					}//右下可走
				}
				if (w_t_p[depth][i].state == 4)
				{
					int DIRE = Kingcanmove(w_t_p[depth][i].x, w_t_p[depth][i].y);
					if ((DIRE & 8) == 8)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x - 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y - 1;
						MoveCount = MoveCount + 1;
					}//左上可走
					if ((DIRE & 2) == 2)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x + 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y - 1;
						MoveCount = MoveCount + 1;
					}//左下可走
					if ((DIRE & 16) == 16)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x - 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y + 1;
						MoveCount = MoveCount + 1;
					}//右上可走
					if ((DIRE & 4) == 4)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x + 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y + 1;
						MoveCount = MoveCount + 1;
					}//右下可走
				}
			}
			else
			{
				if (w_t_p[depth][i].state == 1)
				{
					int DIRE = EMPawncanmove(w_t_p[depth][i].x, w_t_p[depth][i].y);
					if ((DIRE & 8) == 8)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x - 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y - 1;
						MoveCount = MoveCount + 1;
					}//左上可走
					if ((DIRE & 16) == 16)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x - 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y + 1;
						MoveCount = MoveCount + 1;
					}//右上可走
				}
				if (w_t_p[depth][i].state == 3)
				{
					int DIRE = Kingcanmove(w_t_p[depth][i].x, w_t_p[depth][i].y);
					if ((DIRE & 8) == 8)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x - 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y - 1;
						MoveCount = MoveCount + 1;
					}//左上可走
					if ((DIRE & 2) == 2)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x + 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y - 1;
						MoveCount = MoveCount + 1;
					}//左下可走
					if ((DIRE & 16) == 16)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x - 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y + 1;
						MoveCount = MoveCount + 1;
					}//右上可走
					if ((DIRE & 4) == 4)
					{
						Movelist[depth][MoveCount].numStep = 2;
						Movelist[depth][MoveCount].x[0] = w_t_p[depth][i].x;
						Movelist[depth][MoveCount].x[1] = w_t_p[depth][i].x + 1;
						Movelist[depth][MoveCount].y[0] = w_t_p[depth][i].y;
						Movelist[depth][MoveCount].y[1] = w_t_p[depth][i].y + 1;
						MoveCount = MoveCount + 1;
					}//右下可走
				}
			}
		}
		
		if (w_t_p[depth][0].height != 0)//可以吃子
		{
			if (side == 1)
			{
				int k = 0;
				longestJumpCmd.numStep = 0;
				for (k = 0; k < 15; k++)
				{
					longestJumpCmd.x[k] = 0;
					longestJumpCmd.y[k] = 0;
				}
				MYtryToJump(w_t_p[depth][i].x, w_t_p[depth][i].y, 0);
				memcpy(&Movelist[depth][MoveCount], &longestJumpCmd, sizeof(struct Command));
				MoveCount = MoveCount + 1;
			}
			else
			{
				int k = 0;
				longestJumpCmd.numStep = 0;
				for (k = 0; k < 15; k++)
				{
					longestJumpCmd.x[k] = 0;
					longestJumpCmd.y[k] = 0;
				}
				EMtryToJump(w_t_p[depth][i].x, w_t_p[depth][i].y, 0);
				memcpy(&Movelist[depth][MoveCount], &longestJumpCmd, sizeof(struct Command));
				MoveCount = MoveCount + 1;
			}
		}
	}
}

void Initial()  //棋子本身价值 
{
	int i, j;

	Ds = 0;
	Dch = 0;
	Dcent = 0;

	wss = 0;
	wch = 0;
	wcent = 0;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (board[i][j] % 2 == 1)
			{
				if (board[i][j] == 3)//对于王棋的价值计算
				{
					Ds = Ds + KingVal;
				}
				else//对于普通棋子的价值计算
				{
					Ds = Ds + PawnVal;
					Dch = Dch + 7 - i;//******************//
				}

				//if (i == 2 && j == 1)  Dcent++;
				if (i == 2 && j == 3)  Dcent++;
				if (i == 2 && j == 5)  Dcent++;
				if (i == 3 && j == 2)  Dcent++;
				if (i == 3 && j == 4)  Dcent++;
				//if (i == 3 && j == 6)  Dcent++;
				//if (i == 4 && j == 1)  Dcent++;
				if (i == 4 && j == 3)  Dcent++;
				if (i == 4 && j == 5)  Dcent++;
				if (i == 5 && j == 2)  Dcent++;
				if (i == 5 && j == 4)  Dcent++;
				//if (i == 5 && j == 6)  Dcent++;
			}//对敌方估值(优势大，估值大)

			if (board[i][j] % 2 == 0 && board[i][j] != 0)
			{
				if (board[i][j] == 4)
				{
					wss = wss + KingVal;
				}
				else
				{
					wss = wss + PawnVal;
					wch = wch + i;//******************//
				}

				//if (i == 2 && j == 1)  wcent++;
				if (i == 2 && j == 3)  wcent++;
				if (i == 2 && j == 5)  wcent++;
				if (i == 3 && j == 2)  wcent++;
				if (i == 3 && j == 4)  wcent++;
				//if (i == 3 && j == 6)  wcent++;
				//if (i == 4 && j == 1)  wcent++;
				if (i == 4 && j == 3)  wcent++;
				if (i == 4 && j == 5)  wcent++;
				if (i == 5 && j == 2)  wcent++;
				if (i == 5 && j == 4)  wcent++;
				//if (i == 5 && j == 6)  wcent++;
			}//对我方估值（优势大，估值大）
		}
	}
}

void Fre()//行动力 
{
	int i, j;
	Dfr = 0;
	wfr = 0;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (board[i][j] % 2 == 1)
			{
				if (board[i][j] == 3)//敌王自由度
				{
					if (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0)
						Dfr = Dfr + 1;//左下
					if (i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0)
						Dfr = Dfr + 1;//右下
					if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0)
						Dfr = Dfr + 1;//左上
					if (i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0)
						Dfr = Dfr + 1;//右上
				}
				else//敌兵自由度
				{
					if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0)
						Dfr = Dfr + 1;//左上
					if (i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0)
						Dfr = Dfr + 1;//右上
				}
			}//敌方自由度

			if (board[i][j] % 2 == 0 && board[i][j] != 0)
			{
				if (board[i][j] == 4)//我王自由度
				{
					if (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0)
						wfr = wfr + 1;//左下
					if (i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0)
						wfr = wfr + 1;//右下
					if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1] == 0)
						wfr = wfr + 1;//左上
					if (i - 1 >= 0 && j + 1 < 8 && board[i - 1][j + 1] == 0)
						wfr = wfr + 1;//右上
				}
				else//我兵自由度
				{
					if (i + 1 < 8 && j - 1 >= 0 && board[i + 1][j - 1] == 0)
						wfr = wfr + 1;//左下
					if (i + 1 < 8 && j + 1 < 8 && board[i + 1][j + 1] == 0)
						wfr = wfr + 1;//右下
				}//我方自由度
			}
		}
	}
}

void Def()  //防御力 
{
	int i, j;
	Ddef = 0;
	wdef = 0;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (board[i][j] % 2 == 1)
			{
				if (board[i - 1][j + 1] % 2 == 1) Ddef++;
				//if (board[i - 2][j + 2] % 2 == 1) Ddef = Ddef + 0.5;
				//if (board[i - 3][j + 3] % 2 == 1) Ddef++;
				if (board[i + 1][j + 1] % 2 == 1) Ddef++;
				//if (board[i + 2][j + 2] % 2 == 1) Ddef = Ddef + 0.5;
				//if (board[i + 3][j + 3] % 2 == 1) Ddef++;
				if (board[i - 1][j - 1] % 2 == 1) Ddef++;
				if (board[i + 1][j - 1] % 2 == 1) Ddef++;

				if (board[i + 2][j] % 2 == 1) Ddef = Ddef + 0.5;
				if (board[i - 2][j] % 2 == 1) Ddef = Ddef + 0.5;
				if (board[i][j + 2] % 2 == 1) Ddef = Ddef + 0.5;
				if (board[i][j - 2] % 2 == 1) Ddef = Ddef + 0.5;

				if (i == 1 && j == 0) Ddef = Ddef + 1.5;
				if (i == 3 && j == 0) Ddef = Ddef + 1.5;
				if (i == 5 && j == 0) Ddef = Ddef + 1.5;
				if (i == 7 && j == 0) Ddef = Ddef + 1.5;

				if (i == 0 && j == 7) Ddef = Ddef + 1.5;
				if (i == 2 && j == 7) Ddef = Ddef + 1.5;
				if (i == 4 && j == 7) Ddef = Ddef + 1.5;
				if (i == 6 && j == 7) Ddef = Ddef + 1.5;
				
				if (i == 0 && j == 1) Ddef = Ddef + 1.5;
				if (i == 0 && j == 3) Ddef = Ddef + 1.5;
				if (i == 0 && j == 5) Ddef = Ddef + 1.5;
				if (i == 0 && j == 7) Ddef = Ddef + 1.5;

				if (i == 7 && j == 0) Ddef = Ddef + 1.5;
				if (i == 7 && j == 2) Ddef = Ddef + 1.5;
				if (i == 7 && j == 4) Ddef = Ddef + 1.5;
				if (i == 7 && j == 6) Ddef = Ddef + 1.5;
			}//敌方防御度

			if (board[i][j] % 2 == 0 && board[i][j] != 0)
			{
				if (board[i - 1][j + 1] % 2 == 0 && board[i - 1][j + 1] != 0) wdef++;
				//if (board[i - 2][j + 2] % 2 == 0 && board[i - 2][j + 2] != 0) wdef = wdef + 0.5;
				//if (board[i - 3][j + 3] % 2 == 0 && board[i - 3][j + 3] != 0) wdef++;
				if (board[i + 1][j + 1] % 2 == 0 && board[i + 1][j + 1] != 0) wdef++;
				//if (board[i + 2][j + 2] % 2 == 0 && board[i + 2][j + 2] != 0) wdef = wdef + 0.5;
				//if (board[i + 3][j + 3] % 2 == 0 && board[i + 3][j + 3] != 0) wdef++;
				if (board[i - 1][j - 1] % 2 == 0 && board[i - 1][j - 1] != 0) wdef++;
				if (board[i + 1][j - 1] % 2 == 0 && board[i + 1][j - 1] != 0) wdef++;

				if (board[i + 2][j] % 2 == 0 && board[i + 2][j] != 0) wdef = wdef + 0.5;
				if (board[i - 2][j] % 2 == 0 && board[i - 2][j] != 0) wdef = wdef + 0.5;
				if (board[i][j + 2] % 2 == 0 && board[i][j + 2] != 0) wdef = wdef + 0.5;
				if (board[i][j - 2] % 2 == 0 && board[i][j - 2] != 0) wdef = wdef + 0.5;

				if (i == 1 && j == 0) wdef = wdef + 1.5;
				if (i == 3 && j == 0) wdef = wdef + 1.5;
				if (i == 5 && j == 0) wdef = wdef + 1.5;
				if (i == 7 && j == 0) wdef = wdef + 1.5;

				if (i == 0 && j == 7) wdef = wdef + 1.5;
				if (i == 2 && j == 7) wdef = wdef + 1.5;
				if (i == 4 && j == 7) wdef = wdef + 1.5;
				if (i == 6 && j == 7) wdef = wdef + 1.5;

				if (i == 0 && j == 1) wdef = wdef + 1.5;
				if (i == 0 && j == 3) wdef = wdef + 1.5;
				if (i == 0 && j == 5) wdef = wdef + 1.5;
				if (i == 0 && j == 7) wdef = wdef + 1.5;

				if (i == 7 && j == 0) wdef = wdef + 1.5;
				if (i == 7 && j == 2) wdef = wdef + 1.5;
				if (i == 7 && j == 4) wdef = wdef + 1.5;
				if (i == 7 && j == 6) wdef = wdef + 1.5;
			}//我方防御度
		}
	}
}

void Cont()//控制力 
{
	int i, j, k;
	Dcont = 0;
	wcont = 0;
	for (i = 0; i < 8; i++) 
	{
		for (j = 0; j < 8; j++) 
		{
			if (board[i][j] % 2 == 1)//敌方
			{
				for (k = 0; k >= i - 7 && k <= i && k >= j - 7 && k <= j; k++)
				{
					if (board[i - k][j - k] != 0) 
						Dcont = Dcont + k / 2;
					else 
						break;
				}
				for (k = 0; k <= 7 - i && k >= j - 7 && k <= j; k++)
				{
					if (board[i + k][j - k] != 0) 
						Dcont = Dcont + k / 2;
					else 
						break;
				}
				for (k = 0; k <= 7 - i && k <= j - 7; k++)
				{
					if (board[i + k][j + k] != 0) 
						Dcont = Dcont + k / 2;
					else 
						break;
				}
				for (k = 0; k >= i - 7 && k <= i && k <= j - 7; k++)
				{
					if (board[i - k][j + k] != 0) 
						Dcont = Dcont + k / 2;
					else 
						break;
				}
			}

			if (board[i][j] % 2 == 0 && board[i][j] != 0)//我方
			{
				for (k = 0; k >= i - 7 && k <= i && k >= j - 7 && k <= j; k++)
				{
					if (board[i - k][j - k] != 0)
						wcont = wcont + k / 2;
					else
						break;
				}
				for (k = 0; k <= 7 - i && k >= j - 7 && k <= j; k++)
				{
					if (board[i + k][j - k] != 0)
						wcont = wcont + k / 2;
					else
						break;
				}
				for (k = 0; k <= 7 - i && k <= j - 7; k++)
				{
					if (board[i + k][j + k] != 0)
						wcont = wcont + k / 2;
					else
						break;
				}
				for (k = 0; k >= i - 7 && k <= i && k <= j - 7; k++)
				{
					if (board[i - k][j + k] != 0)
						wcont = wcont + k / 2;
					else
						break;
				}
			}
		}
	}
}

double Eval(int side)
{
	double val = 0, val1 = 0, val2 = 0;
	Initial();
	Fre();
	Def();
	Cont();
	val1 = Ds * Value + Dch * Cheng + Dcent * Cent + Dfr * 1 + Ddef * Defend + Dcont * 1;//敌方估值
	val2 = wss * Value + wch * Cheng + wcent * Cent + Dfr * 1 + wdef * Defend + wcont * 1;//我方估值 

	val = val1 * Val1Bi - val2 * Val2Bi;//敌 - 我

	if (side == 1)//我方
	{
		return -val;
	}
	if (side == 2)//敌方
	{
		return val;
	}
	return val;
}

void Place(struct Command cmd)
{
	int midX, midY, curFlag;
	curFlag = board[cmd.x[0]][cmd.y[0]];
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		board[cmd.x[i]][cmd.y[i]] = EMPTY;
		board[cmd.x[i + 1]][cmd.y[i + 1]] = curFlag;
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			midX = (cmd.x[i] + cmd.x[i + 1]) / 2;
			midY = (cmd.y[i] + cmd.y[i + 1]) / 2;
			board[midX][midY] = EMPTY;
		}
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (board[0][i] == ENEMY_FLAG)
		{
			board[0][i] = ENEMY_KING;
		}
		if (board[BOARD_SIZE - 1][i] == MY_FLAG)
		{
			board[BOARD_SIZE - 1][i] = MY_KING;
		}
	}
}

double MinMax(int Depth, int Side, int alpha, int beta)
{
	int MoveNum = 0;
	int _MoveCount = 0;
	double score = 0;
	int i = 0;
	int Tempboard[8][8];
	int x = 0;
	int y = 0;

	FindATM(Side, Depth);//所有走法储存在其中

	while (Movelist[Depth][_MoveCount].numStep != 0)
	{
		MoveNum = MoveNum + 1;
		_MoveCount = _MoveCount + 1;
	}//数出有多少种走法

	if (MoveNum == 0)
		return 0;
	if (Depth == 0)
		return score = Eval(Side);

	for (i = 0; i < MoveNum; i++)
	{
		for (x = 0; x < 8; x++)
		{
			for (y = 0; y < 8; y++)
			{
				Tempboard[x][y] = board[x][y];
			}
		}

		Place(Movelist[Depth][i]);
		score = -MinMax(Depth - 1, 3 - Side, -beta, -alpha);
				
		for (x = 0; x < 8; x++)
		{
			for (y = 0; y < 8; y++)
			{
				board[x][y] = Tempboard[x][y];
			}
		}//复原之前的棋盘

		if (score > alpha)
		{
			alpha = score;
			if (Depth == MySet)
				TheBestMove = i;
		}

		if (alpha >= beta)
			return alpha;
	}
	return alpha;
}

struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	struct Command command = { {0},{0},0 };
	
	/*int MYcheak = 0;
	int EMcheak = 0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] % 2 == 0 && board[i][j] != 0)
				MYcheak++;
			if (board[i][j] % 2 == 1)
				EMcheak++;
		}
	}

	if (MYcheak >= 6)
	{
		MySet = 9;
	}
	else
	{
		MySet = 7;
	}*/

	TheBestMove = 0;
	MinMax(MySet, 1, -INF, INF);

	memcpy(&command, &Movelist[MySet][TheBestMove], sizeof(struct Command));
	return command;
}

//*****************************个人代码终点*****************************//

void start(int flag)
{
	memset(board, 0, sizeof(board));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			board[i][j + (i + 1) % 2] = MY_FLAG;
		}
	}
	for (int i = 5; i < 8; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			board[i][j + (i + 1) % 2] = ENEMY_FLAG;
		}
	}

	initAI(flag);
}

void turn()
{
	struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, myFlag);
	place(command);
	rotateCommand(&command);
	printf("%d", command.numStep);
	for (int i = 0; i < command.numStep; i++)
	{
		printf(" %d,%d", command.x[i], command.y[i]);
	}
	printf("\n");
	fflush(stdout);
}

void end(int x)
{
	exit(0);
}

void loop()
{
	char tag[10] = { 0 };
	struct Command command = { {0},{0},0 };
	int status;
	while (TRUE)
	{
		memset(tag, 0, sizeof(tag));
		scanf_s("%s", tag, 10);
		//scanf("%s", tag);
		if (strcmp(tag, START) == 0)
		{
			scanf_s("%d", &myFlag);
			//scanf("%d", &myFlag);
			start(myFlag);
			printf("OK\n");
			fflush(stdout);
		}
		else if (strcmp(tag, PLACE) == 0)
		{
			scanf_s("%d", &command.numStep);
			//scanf("%d", &command.numStep);
			for (int i = 0; i < command.numStep; i++)
			{
				scanf_s("%d,%d", &command.x[i], &command.y[i]);
				//scanf("%d,%d", &command.x[i], &command.y[i]);
			}
			rotateCommand(&command);
			place(command);
		}
		else if (strcmp(tag, TURN) == 0)
		{
			turn();
		}
		else if (strcmp(tag, END) == 0)
		{
			scanf_s("%d", &status);
			//scanf("%d", &status);
			end(status);
		}
		printBoard();
	}
}

int main(int argc, char* argv[])
{
	loop();
	return 0;
}