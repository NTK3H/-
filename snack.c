#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#define MAX_X	60
#define MAX_Y	20

//记录蛇的所有信息
typedef struct _snack_info{
	unsigned int cur_dir:2;				//记录了蛇的当前走向，上下左右分别用0、1、2、3表示
	unsigned long speed;				//记录了蛇的速度(毫秒为单位)
	unsigned int len;					//记录了蛇的长度
	struct _snack_position{				//记录了整条蛇每个节点的信息
		int x[MAX_X*MAX_Y];
		int y[MAX_X*MAX_Y];
	} sp;
} si;

//记录了食物坐标
typedef struct _snack_food_position_info{
	int x;
	int y;
} sfpi;

/*函数声明*/
si *si_init();
int map_init();
void err(char *info);
void refresh_score(si *si);
void snack_dir();
int snack_newfood(si *si, sfpi *fpi);
int snack_move(si *si);
int snack_isfail(si *si);
int snack_iswin(si *si);
void gotoxy(int x,int y);
void help();

int main()
{
	si *si = si_init();
	if(si == NULL)
		err("Failed to init, please check init info");
	sfpi *fpi = (sfpi*)malloc(sizeof(sfpi));
	map_init();
	help();
	refresh_score(si);
	int last_score = 0;
	srand((unsigned) time (NULL));
	snack_newfood(si,fpi);
	while("Enjoy :)")
	{
		snack_move(si);
		if(si->sp.x[0] == fpi->x && si->sp.y[0] == fpi->y)
		{
			snack_newfood(si,fpi);
			si->len++;
		}
		//此举为了防止屏幕闪烁
		if(last_score != si->len-3)
		{
			refresh_score(si);
			last_score = si->len-3;
		}
		snack_isfail(si);
		snack_iswin(si);
		Sleep(si->speed);
		snack_dir(si);
	}
	getch();
	return 0;
}

//初始化蛇的所有信息
si *si_init()
{
	si *inf_si = (si*)malloc(sizeof(si));
	inf_si->cur_dir = 3;		//开始方向向右
	inf_si->speed = 50;
	inf_si->len = 3;
	memset(inf_si->sp.x,0,sizeof inf_si->sp.x);
	memset(inf_si->sp.y,0,sizeof inf_si->sp.y);
	int hx = 5;
	int hy = 2;
	int j=0;
	for(int i=hx;i>hx-inf_si->len;i--)
	{
		inf_si->sp.x[j] = i;
		inf_si->sp.y[j++] = hy;
	}
	if(inf_si->sp.x[0] < inf_si->len)
		return NULL;
	return inf_si;
}

//初始化地图
int map_init()
{
	for(int i=0;i<MAX_Y+2;i++)
	{
		for(int j=0;j<MAX_X/2+2;j++)
		{
			if(i==0)
			{
				if(j==0)printf("┌");
				else if(j==MAX_X/2+1)printf("┐");
				else printf("─");
			}
			else if(i==MAX_Y+1)
			{
				if(j==0)printf("└");
				else if(j==MAX_X/2+1)printf("┘");
				else printf("─");
			}
			else
			{
				if(j==0 || j==MAX_X/2+1)printf("│");
				else printf("  ");
			}
		}
		printf("\n");
	}
	return 1;
}

//如果出错会调用这个函数
void err(char *info)
{
	fprintf(stderr,"Error: %s\n",info);
	Sleep(3000);
	exit(3);
}

void refresh_score(si *si)
{
	gotoxy(MAX_X+3,3);
	printf("###############");
	gotoxy(MAX_X+3,4);
	printf("#  Score:%3d  #",si->len-3);
	gotoxy(MAX_X+3,5);
	printf("###############");
}

//控制蛇走向和处理暂停
void snack_dir(si *si)
{
	if(kbhit())
		switch(getch())
		{
			case 'w':
				if(si->cur_dir!=1)
					si->cur_dir = 0;
				break;
			case 's':
				if(si->cur_dir!=0)
					si->cur_dir = 1;
				break;
			case 'a':
				if(si->cur_dir!=3)
					si->cur_dir = 2;
				break;
			case 'd':
				if(si->cur_dir!=2)
					si->cur_dir = 3;
				break;
			case ' ':
				getch();
				break;
		}
}

int snack_newfood(si *si, sfpi *fpi)
{
	int x,y;
retry:
	x = rand()%MAX_X+1;
	y = rand()%MAX_Y+1;
	for(int i=0;i<si->len;i++)
		if(x == si->sp.x[i] && y == si->sp.y[i])
			goto retry;
	fpi->x=x;
	fpi->y=y;
	gotoxy(x,y);
	printf("#");
}

//这个函数使蛇走一步
int snack_move(si *si)
{
	for(int i=si->len-1;i>=0;i--)
	{
		si->sp.x[i+1] = si->sp.x[i];
		si->sp.y[i+1] = si->sp.y[i];
	}
	switch(si->cur_dir)
	{
		case 0:
			si->sp.x[0] = si->sp.x[1];
			si->sp.y[0] = si->sp.y[1]-1;
			break;
		case 1:
			si->sp.x[0] = si->sp.x[1];
			si->sp.y[0] = si->sp.y[1]+1;
			break;
		case 2:
			si->sp.x[0] = si->sp.x[1]-1;
			si->sp.y[0] = si->sp.y[1];
			break;
		case 3:
			si->sp.x[0] = si->sp.x[1]+1;
			si->sp.y[0] = si->sp.y[1];
			break;
	}
	gotoxy(si->sp.x[0],si->sp.y[0]);
	printf("*");
	gotoxy(si->sp.x[si->len],si->sp.y[si->len]);
	printf(" ");
}

int snack_isfail(si *si)
{
	for(int i=1;i<=si->len-1;i++)
	{
		if(si->sp.x[0] == si->sp.x[i] && si->sp.y[0] == si->sp.y[i])
		{
			gotoxy(MAX_X/2-5,MAX_Y/2);
			printf("You Lose!");
			Sleep(3000);
			exit(0);
		}
	}
	if(si->sp.x[0] == 0 || si->sp.x[0] == MAX_X+1
			|| si->sp.y[0] == 0 || si->sp.y[0] == MAX_Y+1)
	{
		gotoxy(MAX_X/2-5,MAX_Y/2);
		printf("You Lose!");
		Sleep(3000);
		exit(0);
	}
}

int snack_iswin(si *si)
{
	if(si->len == MAX_X*MAX_Y)
	{
		gotoxy(MAX_X/2-5,MAX_Y/2);
		printf("You Win!");
		Sleep(3000);
		exit(0);
	}
}

void gotoxy(int x,int y)
{
	CONSOLE_SCREEN_BUFFER_INFO        csbiInfo;
	HANDLE    hConsoleOut;

	hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsoleOut,&csbiInfo);

	csbiInfo.dwCursorPosition.X = x+1;
	csbiInfo.dwCursorPosition.Y = y;
	SetConsoleCursorPosition(hConsoleOut,csbiInfo.dwCursorPosition);
}

void help()
{
	gotoxy(MAX_X+3,11);
	printf("Control key:");
	gotoxy(MAX_X+8,12);
	printf("w");
	gotoxy(MAX_X+6,13);
	printf("a s d");
	gotoxy(MAX_X+3,15);
	printf("Pause:");
	gotoxy(MAX_X+6,16);
	printf("Space key");
}
