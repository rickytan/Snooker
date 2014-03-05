#include<string.h>
#include<graphics.h>
#include<stdlib.h>
#include<bios.h>
#include<math.h>
#include<stdio.h>
#include<conio.h>
#include<dos.h>
#include<dir.h>
#include<alloc.h>


#define BALLNUM		22			/*************球的数量************/
#define ACCEL		0.015			/************球的加速度***********/ 
#define E		0.9			/***********碰撞恢复系数**********/
#define IN		1			/**************球进了*************/
#define TRUE		1
#define BALL_R		10			/*************球的半径************/
#define NET_R		15			/*************球网半径************/
#define PI		3.1415926


#define TABLEMINX	23			/*				 */
#define TABLEMAXX	616			/*		定义		 */
#define TABLEMINY	121			/*	      球桌边界		 */
#define TABLEMAXY	459			/* 				 */

int	mousex,mousey,button=0;			/**********鼠标位置与按键*********/
int 	maxx,maxy;				/*************屏幕范围************/
int	Diffi=0;				/*************进球难度************/
int	Line_on=0;				/************方向线开启***********/
int	ispractising=0;				/**********是否为练习模式*********/
int	redflag;
char	*mouse,*mouse_temp,*dash_dot;
char	*ball_dat[BALLNUM],*ball_temp;

double	ww=PI,neww=PI;				/*************球杆角度************/

int	Music_on=1;				/***********是否开启音乐**********/
void interrupt (*handler)();
void interrupt music0();
void interrupt music1();
void interrupt music2();
void interrupt (*music[3])()={music0,music1,music2};
						/*************音乐中断************/
void play(int);					/*************播放音乐************/

struct NET					/*************球网位置************/
{
	int x;
	int y;
}net[6];					
		
struct Velocity					/*************球的速度************/
{
	double vx;
	double vy;
	double v;				/*****  v=sqrt(vx*vx+vy*vy)  *****/
};

struct Position					/*************球的位置************/
{
	double x;
	double y;
};

struct BALL					/*************球的信息************/
{
	struct Velocity velo;
	struct Position posi;
	struct Position origin;
	int score;				/*************球的分值************/
	int color;				/*************球的颜色************/
	int ball_in;				/**********球是否已经进网*********/
}ball[BALLNUM];

struct PLAYER					/*************玩家信息************/
{
	char name[20];
	int score;
	char *head;
	int pace;				/********记录玩家应击什么球*******/
}player[2];


void 	showballnum(struct BALL *);		/**********显示球的编号***********/
void 	showhelp(void);				/**********显示帮助信息***********/

void 	drawmenu(int);				/************绘制菜单*************/

void 	draw_ball(struct BALL);			/**************画球***************/
void	erase_ball(struct BALL);		/**************擦球***************/
void 	draw_table(void);			/************绘制球桌*************/
void 	beforegame(void);			/************游戏界面*************/
void 	draw_stick(struct BALL,double);		/************绘制球杆*************/
void	draw_dash(struct BALL,double);		/***********绘制瞄准线************/
void	direction(struct BALL,double);		/***********绘制方向线************/


int	ShowBmp(int x,int y,char *filename,int mode,int speed);
						/*********显示16色BMP图片*********/


char far *bmp_to_dat(char *);			/******将BMP图片存到buffer中******/

int  	ball_in(struct BALL);			/************判断进球*************/
void 	hitball(struct BALL,double);		/************击球动画*************/
int  	is_collision(struct BALL,struct BALL);	/**********判断两球相碰***********/

int 	collision_table(struct BALL *);		/************球碰桌子*************/

void 	collision(struct BALL *,struct BALL *);	/**********两球碰后速度***********/
double	energy(void);				/**  击球力度，返回球的总速度v  **/

void 	InitMouse(int,int,int,int);		/************初始化鼠标***********/
void 	position(int);				/************读鼠标信息***********/
void	drawmouse(int x,int y);			/*************绘制鼠标************/

void	InitBall(struct BALL *);		/*********初始化球的信息**********/
void	InitNet(struct NET *);			/*********初始化球网位置**********/

void	outchinese(int,int,char *,int,int,int);	/************显示汉字*************/

void	End(int);				/************游戏结束*************/

