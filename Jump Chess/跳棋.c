#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// board information
#define BOARD_SIZE 8
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define move_BLACK 3
#define eat_BLACK 5
#define move_WHITE 4
#define eat_WHITE 6
#define king_BLACK 7
#define king_WHITE 8

// bool
typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define MAX_BYTE 10000
#define MAX_STEP 15

#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"

static int count = 0;
static int num = 1;
int eat_count = 0;
struct Command
{
	int x[MAX_STEP];
	int y[MAX_STEP];
	int numStep;
};

char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
char virtual_board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int me_flag;
int other_flag;

void debug(const char* str)
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}
void printBoard()
{
	char Visual_board[BOARD_SIZE][BOARD_SIZE + 1] = { 0 };
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] == EMPTY)
			{
				Visual_board[i][j] = '.';
			}
			else if (board[i][j] == BLACK)
			{
				Visual_board[i][j] = 'O';
			}
			else if (board[i][j] == WHITE)
			{
				Visual_board[i][j] = 'X';
			}
		}
		printf("%s\n", Visual_board[i]);
	}
}
BOOL isInBound(int x, int y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}
void place(struct Command cmd, int cur_flag)
{
	int x_mid, y_mid;
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		board[cmd.x[i]][cmd.y[i]] = EMPTY;
		board[cmd.x[i + 1]][cmd.y[i + 1]] = cur_flag;
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			x_mid = (cmd.x[i] + cmd.x[i + 1]) / 2;
			y_mid = (cmd.y[i] + cmd.y[i + 1]) / 2;
			board[x_mid][y_mid] = EMPTY;
		}
	}
}
/**
 * YOUR CODE BEGIN
 * 你的代码开始
 */
int moveDir[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };
int jumpDir[4][2] = { {2, -2}, {2, 2}, {-2, -2}, {-2, 2} };
struct SCORE { int score[13]; int times; int x[13]; int y[13]; };
struct SCORE Score = { .score = {0},.times = 0,.x = {0},.y = {0}, };
//struct Command cur_scor ={.x = {0},.y = {0},.numStep = 0};
struct Command cur_step = { .x = {0},.y = {0},.numStep = 0 };
struct Command best_step = { .x = {0},.y = {0},.numStep = 0 };
struct Command cur_command = { .x = {0},.y = {0},.numStep = 0 };
struct Command jumpCmd = { .x = {0},.y = {0},.numStep = 0 };
struct Command longestJumpCmd = { .x = {0},.y = {0},.numStep = 1 };

/*********估值函数********/
int point(int me)//再检测是否会被吃子
{
	int score = 0;
	int me_score = 0, other_score = 0;
	int other = 3 - me;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			//统计己方子数与王子数
			if (virtual_board[i][j] == BLACK)
				me_score += 5;
			else if (virtual_board[i][j] == king_BLACK)
				me_score += 50;
			else if (virtual_board[i][j] == WHITE)
				other_score += 5;
			else if (virtual_board[i][j] == WHITE && i == 7)
				other_score += 50;
			if (virtual_board[i][j] == BLACK)
			{
				//连吃需尽可能避免，
				if (isInBound(i - 1, j - 1) && isInBound(i + 1, j + 1) && virtual_board[i - 1][j - 1] == WHITE && virtual_board[i + 1][j + 1] == EMPTY && (if_eat_white(i + 1, j + 1) == 1))
					me_score -= 30;
				else if (isInBound(i - 1, j + 1) && isInBound(i + 1, j - 1) && virtual_board[i - 1][j + 1] == WHITE && virtual_board[i + 1][j - 1] == EMPTY && (if_eat_white(i + 1, j - 1)== 1))
					me_score -= 30;
				else if (isInBound(i + 1, j + 1) && isInBound(i - 1, j - 1) && virtual_board[i + 1][j + 1] == WHITE && virtual_board[i - 1][j - 1] == EMPTY && (if_eat_white(i - 1, j - 1) == 1))
					me_score -= 30;
				else if (isInBound(i + 1, j - 1) && isInBound(i - 1, j + 1) && virtual_board[i + 1][j - 1] == WHITE && virtual_board[i - 1][j + 1] == EMPTY && (if_eat_white(i - 1, j + 1) == 1))
					me_score -= 30;
				//单吃
				else if (isInBound(i - 1, j - 1) && isInBound(i + 1, j + 1) && virtual_board[i - 1][j - 1] == WHITE && virtual_board[i + 1][j + 1] == EMPTY && (if_eat_white(i + 1, j + 1) == 0))
					me_score -= 20;
				else if (isInBound(i - 1, j + 1) && isInBound(i + 1, j - 1) && virtual_board[i - 1][j + 1] == WHITE && virtual_board[i + 1][j - 1] == EMPTY && (if_eat_white(i + 1, j - 1) == 0))
					me_score -= 20;
				else if (isInBound(i + 1, j + 1) && isInBound(i - 1, j - 1) && virtual_board[i + 1][j + 1] == WHITE && virtual_board[i - 1][j - 1] == EMPTY && (if_eat_white(i - 1, j - 1) == 0))
					me_score -= 20;
				else if (isInBound(i + 1, j - 1) && isInBound(i - 1, j + 1) && virtual_board[i + 1][j - 1] == WHITE && virtual_board[i - 1][j + 1] == EMPTY && (if_eat_white(i - 1, j + 1) == 0))
					me_score -= 20;

				if (isInBound(i + 2, j) && isInBound(i + 1, j + 1) && isInBound(i + 2, j + 2) && isInBound(i, j + 2) && virtual_board[i][j] == BLACK && virtual_board[i + 2][j] == BLACK && virtual_board[i + 1][j + 1] == EMPTY && virtual_board[i][j + 2] == BLACK && virtual_board[i + 2][j + 2] == BLACK)
					me_score -= 10;//四子空心情况
			}

		}
	}
	score = me_score - other_score;
	return score;
}
//黑白转置
void rotateCommand(struct Command* cmd)
{
	if (me_flag == WHITE)
	{
		for (int i = 0; i < cmd->numStep; i++)
		{
			cmd->x[i] = BOARD_SIZE - 1 - cmd->x[i];
			cmd->y[i] = BOARD_SIZE - 1 - cmd->y[i];
		}
	}
}
// ai运行中走子
void set(int x1, int y1, int x_next, int y_next)
{
	virtual_board[x_next][y_next] = virtual_board[x1][y1];
	virtual_board[x1][y1] = EMPTY;
}
/*
*你可以在这里初始化你的AI*
*/
void tryToJump(int x, int y, int currentStep)
{
	int newX, newY, midX, midY;
	char tmpFlag;
	jumpCmd.x[currentStep] = x;
	jumpCmd.y[currentStep] = y;
	jumpCmd.numStep++;
	for (int i = 0; i < 4; i++)
	{
		newX = x + jumpDir[i][0];
		newY = y + jumpDir[i][1];
		midX = (x + newX) / 2;
		midY = (y + newY) / 2;
		if (isInBound(newX, newY) && (virtual_board[midX][midY] == WHITE) && (virtual_board[newX][newY] == EMPTY))
		{
			virtual_board[newX][newY] = virtual_board[x][y];
			virtual_board[x][y] = EMPTY;
			tmpFlag = virtual_board[midX][midY];
			virtual_board[midX][midY] = EMPTY;
			tryToJump(newX, newY, currentStep + 1);
			virtual_board[x][y] = virtual_board[newX][newY];
			virtual_board[newX][newY] = EMPTY;
			virtual_board[midX][midY] = tmpFlag;
		}
	}
	if (jumpCmd.numStep >= longestJumpCmd.numStep)//是选择后搜到的好还是先搜到的好？待测试
	{
		memcpy(&longestJumpCmd, &jumpCmd, sizeof(struct Command));
	}
	jumpCmd.numStep--;
}
int if_king_move(int x, int y)
{
	if (isInBound(x - 1, y + 1) && board[x - 1][y + 1] == EMPTY)
		return 1;
	else if (isInBound(x - 1, y - 1) && board[x - 1][y - 1] == EMPTY)
		return 1;
	else if (isInBound(x + 1, y + 1) && board[x + 1][y + 1] == EMPTY)
		return 1;
	else if (isInBound(x + 1, y - 1) && board[x + 1][y - 1] == EMPTY)
		return 1;
	else
		return 0;
}
int if_move(int x, int y)
{
	if (isInBound(x - 1, y + 1) && board[x - 1][y + 1] == EMPTY)
		return 1;
	else if (isInBound(x - 1, y - 1) && board[x - 1][y - 1] == EMPTY)
		return 1;
	else
		return 0;
}
int if_eat(int x, int y)
{
	if (isInBound(x - 2, y + 2) && isInBound(x - 1, y + 1) && virtual_board[x - 2][y + 2] == EMPTY && virtual_board[x - 1][y + 1] == WHITE)
		return 1;
	if (isInBound(x - 2, y - 2) && isInBound(x - 1, y - 1) && virtual_board[x - 2][y - 2] == EMPTY && virtual_board[x - 1][y - 1] == WHITE)
		return 1;
	if (isInBound(x + 2, y + 2) && isInBound(x + 1, y + 1) && virtual_board[x + 2][y + 2] == EMPTY && virtual_board[x + 1][y + 1] == WHITE)
		return 1;
	if (isInBound(x + 2, y - 2) && isInBound(x + 1, y - 1) && virtual_board[x + 2][y - 2] == EMPTY && virtual_board[x + 1][y - 1] == WHITE)
		return 1;
	else
		return 0;
}
int if_eat_white(int x, int y)
{
	if (isInBound(x - 2, y + 2) && isInBound(x - 1, y + 1) && virtual_board[x - 2][y + 2] == EMPTY && virtual_board[x - 1][y + 1] == BLACK)
		return 1;
	if (isInBound(x - 2, y - 2) && isInBound(x - 1, y - 1) && virtual_board[x - 2][y - 2] == EMPTY && virtual_board[x - 1][y - 1] == BLACK)
		return 1;
	if (isInBound(x + 2, y + 2) && isInBound(x + 1, y + 1) && virtual_board[x + 2][y + 2] == EMPTY && virtual_board[x + 1][y + 1] == BLACK)
		return 1;
	if (isInBound(x + 2, y - 2) && isInBound(x + 1, y - 1) && virtual_board[x + 2][y - 2] == EMPTY && virtual_board[x + 1][y - 1] == BLACK)
		return 1;
	else
		return 0;
}
//************************************************先写一个三层maxmin,如何初始化？什么是初始化？
//// initai里的值如何传出来，还是直接在aiturn里写？
//暂定初始化为 “将可动棋子活性化”
void initAI(int me)
{

	return;
}
/**
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * me表示你所代表的棋子(1或2)
 * 你需要返回一个结构体Command，其中numStep是你要移动的棋子经过的格子数（含起点、终点），
 * x、y分别是该棋子依次经过的每个格子的横、纵坐标
 */
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	//if (num <= 2 && me == BLACK)
	//return firstTwo(me);//前三板固定套路，尽量节省时间

	int eat_count = 0;
	int score_max = 0, cur_score = 0;
	struct Command command = {
		.x = {0},
		.y = {0},
		.numStep = 0 };

	int times = 0;
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			if (board[x][y] == BLACK)
			{
				if (if_eat(x, y))
				{
					tryToJump(x, y, 0);
					eat_count++;
				}
				else if (if_move(x, y) && eat_count == 0)
				{
					if (isInBound(x - 1, y + 1) && virtual_board[x - 1][y + 1] == EMPTY)
					{
						set(x, y, x - 1, y + 1);
						cur_step.numStep = 2, cur_step.x[0] = x, cur_step.y[0] = y;
						cur_step.x[1] = x - 1, cur_step.y[1] = y + 1;
						if (times == 0)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						else if (point(me) > score_max)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						times++;
						memcpy(virtual_board, board, sizeof(virtual_board));
					}
					if (isInBound(x - 1, y - 1) && virtual_board[x - 1][y - 1] == EMPTY)
					{
						set(x, y, x - 1, y - 1);
						cur_step.numStep = 2, cur_step.x[0] = x, cur_step.y[0] = y;
						cur_step.x[1] = x - 1, cur_step.y[1] = y - 1;
						if (times == 0)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						else if (point(me) >= score_max)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						times++;
						memcpy(virtual_board, board, sizeof(virtual_board));
					}
				}
			}
			else if (board[x][y] == king_BLACK)
			{
				if (if_eat(x, y))
				{
					tryToJump(x, y, 0);
					eat_count++;
				}
				else if (if_king_move(x, y) && eat_count == 0)
				{
					if (isInBound(x - 1, y + 1) && virtual_board[x - 1][y + 1] == EMPTY)
					{
						set(x, y, x - 1, y + 1);
						cur_step.numStep = 2, cur_step.x[0] = x, cur_step.y[0] = y;
						cur_step.x[1] = x - 1, cur_step.y[1] = y + 1;
						if (times == 0)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						else if (point(me) > score_max)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						times++;
						memcpy(virtual_board, board, sizeof(virtual_board));
					}
					if (isInBound(x - 1, y - 1) && virtual_board[x - 1][y - 1] == EMPTY)
					{
						set(x, y, x - 1, y - 1);
						cur_step.numStep = 2, cur_step.x[0] = x, cur_step.y[0] = y;
						cur_step.x[1] = x - 1, cur_step.y[1] = y - 1;
						if (times == 0)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						else if (point(me) >= score_max)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						times++;
						memcpy(virtual_board, board, sizeof(virtual_board));
					}

					if (isInBound(x + 1, y + 1) && virtual_board[x + 1][y + 1] == EMPTY)
					{
						set(x, y, x + 1, y + 1);
						cur_step.numStep = 2, cur_step.x[0] = x, cur_step.y[0] = y;
						cur_step.x[1] = x + 1, cur_step.y[1] = y + 1;
						if (times == 0)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						else if (point(me) > score_max)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						times++;
						memcpy(virtual_board, board, sizeof(virtual_board));
					}
					if (isInBound(x + 1, y - 1) && virtual_board[x + 1][y - 1] == EMPTY)
					{
						set(x, y, x + 1, y - 1);
						cur_step.numStep = 2, cur_step.x[0] = x, cur_step.y[0] = y;
						cur_step.x[1] = x + 1, cur_step.y[1] = y - 1;
						if (times == 0)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						else if (point(me) >= score_max)
						{
							score_max = point(me);
							memcpy(&command, &cur_step, sizeof(struct Command));
						}
						times++;
						memcpy(virtual_board, board, sizeof(virtual_board));
					}
				}
			}
		}
	}
	if (eat_count != 0)
	{
		return longestJumpCmd;
	}
	else
		return command;
}

 //.X.X.X.X
 //X.X.X.X.
 //.X.X.X.X
 //........
 //........
 //O.O.O.O.
 //.O.O.O.O
 //O.O.O.O.
void start(int flag)
{
	memset(board, 0, sizeof(board));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			board[i][j + (i + 1) % 2] = WHITE;
		}
	}
	for (int i = 5; i < 8; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			board[i][j + (i + 1) % 2] = BLACK;
		}
	}
	initAI(flag);
}
void turn()
{
	// AI
	for (int k = 0; k < BOARD_SIZE; k++)
		if (board[0][k] == BLACK)
			board[0][k] = king_BLACK;
	struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, me_flag);
	if (me_flag == BLACK)
	{
		place(command, me_flag);
	}
	else if (me_flag == WHITE)
	{
		place(command, other_flag);
	}
	memcpy(virtual_board, board, sizeof(board));
	if (me_flag == BLACK)
	{
		printf("%d", command.numStep);
		for (int i = 0; i < command.numStep; i++)
		{
			printf(" %d,%d", command.x[i], command.y[i]);
		}
		printf("\n");
	}
	else if (me_flag == WHITE)
	{
		rotateCommand(&command);
		printf("%d", command.numStep);
		for (int i = 0; i < command.numStep; i++)
		{
			printf(" %d,%d", command.x[i], command.y[i]);
		}
		printf("\n");
	}
	memset(longestJumpCmd.x, 0, sizeof(MAX_STEP));
	memset(longestJumpCmd.y, 0, sizeof(MAX_STEP));
	longestJumpCmd.numStep = 1;
	fflush(stdout);
}
void end(int x)
{

}
void loop()
{
	//  freopen("../input", "r", stdin);
	char tag[10] = { 0 };
	struct Command command = {
		.x = {0},
		.y = {0},
		.numStep = 0 };
	int status;
	while (TRUE)
	{
		memset(tag, 0, sizeof(tag));
		scanf_s("%s", tag, 10);
		if (strcmp(tag, START) == 0)
		{
			scanf_s("%d", &me_flag);
			other_flag = 3 - me_flag;
			start(me_flag);
			memcpy(virtual_board, board, sizeof(board));
			printf("OK\n");
			fflush(stdout);
		}
		else if (strcmp(tag, PLACE) == 0)
		{
			scanf_s("%d", &command.numStep);
			for (int i = 0; i < command.numStep; i++)
			{
				scanf_s("%d,%d", &command.x[i], &command.y[i]);
			}
			if (me_flag == BLACK)
				place(command, other_flag);
			else if (me_flag == WHITE)
			{
				rotateCommand(&command);
				place(command, me_flag);
			}
			memcpy(virtual_board, board, sizeof(board));
		}
		else if (strcmp(tag, TURN) == 0)
		{
			turn();
		}
		else if (strcmp(tag, END) == 0)
		{
			scanf_s("%d", &status);
			end(status);
		}
	}
}
int main(int argc, char* argv[])
{
	loop();
	return 0;
}