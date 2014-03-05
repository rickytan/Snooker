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


#define BALLNUM		22			/*************�������************/
#define ACCEL		0.015			/************��ļ��ٶ�***********/ 
#define E		0.9			/***********��ײ�ָ�ϵ��**********/
#define IN		1			/**************�����*************/
#define TRUE		1
#define BALL_R		10			/*************��İ뾶************/
#define NET_R		15			/*************�����뾶************/
#define PI		3.1415926


#define TABLEMINX	23			/*				 */
#define TABLEMAXX	616			/*		����		 */
#define TABLEMINY	121			/*	      �����߽�		 */
#define TABLEMAXY	459			/* 				 */

int	mousex,mousey,button=0;			/**********���λ���밴��*********/
int 	maxx,maxy;				/*************��Ļ��Χ************/
int	Diffi=0;				/*************�����Ѷ�************/
int	Line_on=0;				/************�����߿���***********/
int	ispractising=0;				/**********�Ƿ�Ϊ��ϰģʽ*********/
int	redflag;
char	*mouse,*mouse_temp,*dash_dot;
char	*ball_dat[BALLNUM],*ball_temp;

double	ww=PI,neww=PI;				/*************��˽Ƕ�************/

int	Music_on=1;				/***********�Ƿ�������**********/
void interrupt (*handler)();
void interrupt music0();
void interrupt music1();
void interrupt music2();
void interrupt (*music[3])()={music0,music1,music2};
						/*************�����ж�************/
void play(int);					/*************��������************/

struct NET					/*************����λ��************/
{
	int x;
	int y;
}net[6];					
		
struct Velocity					/*************����ٶ�************/
{
	double vx;
	double vy;
	double v;				/*****  v=sqrt(vx*vx+vy*vy)  *****/
};

struct Position					/*************���λ��************/
{
	double x;
	double y;
};

struct BALL					/*************�����Ϣ************/
{
	struct Velocity velo;
	struct Position posi;
	struct Position origin;
	int score;				/*************��ķ�ֵ************/
	int color;				/*************�����ɫ************/
	int ball_in;				/**********���Ƿ��Ѿ�����*********/
}ball[BALLNUM];

struct PLAYER					/*************�����Ϣ************/
{
	char name[20];
	int score;
	char *head;
	int pace;				/********��¼���Ӧ��ʲô��*******/
}player[2];


void 	showballnum(struct BALL *);		/**********��ʾ��ı��***********/
void 	showhelp(void);				/**********��ʾ������Ϣ***********/

void 	drawmenu(int);				/************���Ʋ˵�*************/

void 	draw_ball(struct BALL);			/**************����***************/
void	erase_ball(struct BALL);		/**************����***************/
void 	draw_table(void);			/************��������*************/
void 	beforegame(void);			/************��Ϸ����*************/
void 	draw_stick(struct BALL,double);		/************�������*************/
void	draw_dash(struct BALL,double);		/***********������׼��************/
void	direction(struct BALL,double);		/***********���Ʒ�����************/


int	ShowBmp(int x,int y,char *filename,int mode,int speed);
						/*********��ʾ16ɫBMPͼƬ*********/


char far *bmp_to_dat(char *);			/******��BMPͼƬ�浽buffer��******/

int  	ball_in(struct BALL);			/************�жϽ���*************/
void 	hitball(struct BALL,double);		/************���򶯻�*************/
int  	is_collision(struct BALL,struct BALL);	/**********�ж���������***********/

int 	collision_table(struct BALL *);		/************��������*************/

void 	collision(struct BALL *,struct BALL *);	/**********���������ٶ�***********/
double	energy(void);				/**  �������ȣ�����������ٶ�v  **/

void 	InitMouse(int,int,int,int);		/************��ʼ�����***********/
void 	position(int);				/************�������Ϣ***********/
void	drawmouse(int x,int y);			/*************�������************/

void	InitBall(struct BALL *);		/*********��ʼ�������Ϣ**********/
void	InitNet(struct NET *);			/*********��ʼ������λ��**********/

void	outchinese(int,int,char *,int,int,int);	/************��ʾ����*************/

void	End(int);				/************��Ϸ����*************/

