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

#include"key.h"
#include"vga13h.h"

#define TABLECOLOR GREEN				/*****球桌颜色*****/
#define STARNUM 300					/*****星星数量*****/
#define STICKLENTH 300					/*****球杆长度*****/
#define DASH_LENTH 300					/****瞄准线长度****/
#define LENTH 300					/**力量进度条长度**/
#define ZIKU 16						/***使用16X16字库**/
#define DOT_R 3						/******点半径******/

#define NoError 0
#define ErrorFileOpen 1
#define ErrorFileType 2
#define ErrorImageColor 3

union REGS regs;

char *s_cn="斯诺克";
char far *logo;
char far *vs1;
char far *vs2;

int mm=-1;

struct STAR
{
	int x;
	int y;
}*star;

void InitGraph(void);								/******初始化屏幕******/
void initstar(struct STAR *);							/****初始化星星位置****/
void drawstar(struct STAR *);							/*******绘制星星*******/
double ball_distance(struct BALL,struct BALL);
void getdat(void);
void toprank(void);
void practice(void);

int huge DetectVga256()								/*****检测256色模式****/
{
	return 2;
}

void InitGraph256()								/***初始化屏幕为256色**/
{
	int graphdriver=DETECT,graphmode;
	int ErrorCode;

	installuserdriver("svga256",DetectVga256);
	initgraph(&graphdriver,&graphmode,"");
	ErrorCode=graphresult();
	if (ErrorCode!=grOk)
	{
		printf("%s\n",grapherrormsg(ErrorCode));
		getch();
		exit(1);
	}

	cleardevice();
	maxx=getmaxx();
	maxy=getmaxy();
}

void InitGraph()								/***初始化屏幕为16色***/
{
	int gdriver=VGA,gmode=VGAHI;
	int ErrorCode;

	initgraph(&gdriver,&gmode,".\\source");
	ErrorCode=graphresult();
	if (ErrorCode!=grOk)
	{
		printf("%s\n",grapherrormsg(ErrorCode));
		getch();
		exit(1);
	}

	cleardevice();

	maxx=getmaxx();
	maxy=getmaxy();

}

void draw_net()									/*******绘制球网*******/
{
	int i,s[] = {0,2,3,4,5};
	setcolor(WHITE);
	setlinestyle(0,0,2);
	for (i=0;i<5;i++)
	{
		circle(net[s[i]].x,net[s[i]].y,NET_R);
		setfillstyle(SOLID_FILL,BLACK);
		floodfill(net[s[i]].x,net[s[i]].y,WHITE);
		setfillstyle(8,BLUE);
		floodfill(net[s[i]].x,net[s[i]].y,WHITE);
	}
	arc(net[1].x,net[1].y,150,30,NET_R);
	setfillstyle(SOLID_FILL,BLACK);
	floodfill(net[1].x,net[1].y,WHITE);
	setfillstyle(8,BLUE);
	floodfill(net[1].x,net[1].y,WHITE);
}

void draw_mark()								/****绘制球桌上的标记***/
{
	setlinestyle(0,0,0);
	setcolor(WHITE);
	setwritemode(COPY_PUT);


	line(137,121,137,459);
	arc(137,290,90,270,53);
	line(136,289,138,289);

	line(319,290,321,290);
	line(320,289,320,291);
}

void draw_boundary()								/******绘制球桌边界*****/
{
	int x_off,y_off;

	setlinestyle(0,0,0);
	setcolor(WHITE);
	setwritemode(COPY_PUT);

	line(22,149,22,433);
	line(49,120,300,120);
	line(339,121,588,121);
	line(616,148,616,432);
	line(50,459,299,459);
	line(339,459,588,459);


	x_off=20;y_off=13;

	line(50,121,50-x_off,121-y_off);
	line(588,121,588+x_off,121-y_off);
	line(50,459,50-x_off,459+y_off);
	line(588,459,588+x_off,459+y_off);

	x_off=13;y_off=20;

	line(23,148,23-x_off,148-y_off);
	line(23,432,23-x_off,432+y_off);
	line(616,148,616+x_off,148-y_off);
	line(616,432,616+x_off,432+y_off);

	x_off=4;y_off=11;

	line(299,121,299+x_off,121-y_off);
	line(339,121,339-x_off,121-y_off);
	line(299,459,299+x_off,459+y_off);
	line(339,459,339-x_off,459+y_off);
}

void draw_table()								/********绘制球桌********/
{
	setlinestyle(0,0,3);
	setcolor(WHITE);
	setwritemode(COPY_PUT);
	rectangle(1,100,638,480);

	draw_boundary();
	draw_net();

	setfillstyle(SOLID_FILL,TABLECOLOR);
	floodfill(300,400,WHITE);
	setfillstyle(SOLID_FILL,6);
	floodfill(20,290,WHITE);
	floodfill(620,290,WHITE);

	draw_mark();
}

void InitNet(struct NET net[])							/*****初始化球网位置*****/
{
	net[0].x=TABLEMINX-3;
	net[0].y=TABLEMINY-3;

	net[1].x=(TABLEMINX+TABLEMAXX)/2;
	net[1].y=TABLEMINY-12;

	net[2].x=TABLEMAXX+3;
	net[2].y=TABLEMINY-3;

	net[3].x=TABLEMAXX+3;
	net[3].y=TABLEMAXY+3;

	net[4].x=(TABLEMINX+TABLEMAXX)/2;
	net[4].y=TABLEMAXY+12;

	net[5].x=TABLEMINX-3;
	net[5].y=TABLEMAXY+3;

}

void InitBall(struct BALL ball[])						/*****初始化球的信息*****/
{
	int i,j,x=400,y=290;
	int ballnum=0;
	int col[]={0,-1,1};
	ball[ballnum].origin.x=ball[ballnum].posi.x=x-300;
	ball[ballnum].origin.y=ball[ballnum].posi.y=y;

	ball[ballnum].velo.vx=0;
	ball[ballnum].velo.vy=0;
	ball[ballnum].velo.v=0;
	ball[ballnum].color=WHITE;
	ball[ballnum].ball_in=0;

	ballnum++;

	for (i=1;i<=5;i++)
	{
		for (j=0;j<i;j++)
		{
			ball[ballnum].origin.x=ball[ballnum].posi.x=x+(i-1)*sqrt(3.0)*(BALL_R+1);
			ball[ballnum].origin.y=ball[ballnum].posi.y=y-(i-1)*(BALL_R+1)+j*2*(BALL_R+1);

			ball[ballnum].color=RED;
			ballnum++;
		}
	}
	for (i=2;i>=0;i--)
	{
		ball[ballnum].origin.x=ball[ballnum].posi.x=137;
		ball[ballnum].origin.y=ball[ballnum].posi.y=290+53*col[i];

		ball[ballnum].color=6+4*i;
		ballnum++;
	}
	ball[ballnum].origin.x=ball[ballnum].posi.x=320;
	ball[ballnum].origin.y=ball[ballnum].posi.y=290;
	ball[ballnum].color=BLUE;
	ballnum++;

	ball[ballnum].origin.x=ball[ballnum].posi.x=x-BALL_R*2-3;
	ball[ballnum].origin.y=ball[ballnum].posi.y=290;
	ball[ballnum].color=LIGHTMAGENTA;
	ballnum++;

	ball[ballnum].origin.x=ball[ballnum].posi.x=x+BALL_R*4+BALL_R*4.5*sqrt(3.0);
	ball[ballnum].origin.y=ball[ballnum].posi.y=290;
	ball[ballnum].color=DARKGRAY;


	for (i=0;i<BALLNUM;i++)
	{
		ball[i].velo.vx=0;
		ball[i].velo.vy=0;
		ball[i].velo.v=0;
		ball[i].score=(i<=15)?1:(i-14);
		ball[i].ball_in=0;
	}
}

void getdat()									/*将鼠标和瞄准线点的图像保存*/
{
	int mx=0,my=0,size;
	int r=DOT_R;

	setcolor(15);
	setlinestyle(0,0,1);

	line(mx,my,mx+6,my+6);
	line(mx+7,my+7,mx+5,my+7);
	line(mx+4,my+8,mx+6,my+11);
	line(mx+5,my+11,mx+4,my+12);
	line(mx+3,my+11,mx+2,my+9);
	line(mx+1,my+9,mx,my+10);
	line(mx,my+9,mx,my+1);

	setfillstyle(SOLID_FILL,15);
	floodfill(mx+1,my+2,15);
	size=imagesize(0,0,8,12);
	mouse=malloc(size);
	mouse_temp=malloc(size);
	getimage(0,0,7,12,mouse);
	cleardevice();

	setcolor(WHITE);
	setlinestyle(0,0,1);

	circle(r,r,r);
	setfillstyle(SOLID_FILL,WHITE);
	floodfill(r,r,WHITE);

	size=imagesize(0,0,2*r,2*r);
	dash_dot=malloc(size);
	getimage(0,0,2*r,2*r,dash_dot);
	cleardevice();
}

void draw_ball(struct BALL ball)						/*********绘制球*********/
{
	int off_x,off_y;
	off_x=0.6*BALL_R*cos(1);
	off_y=0.6*BALL_R*sin(1);
	draw_mark();
	setcolor(BLACK);
	setfillstyle(SOLID_FILL,ball.color);

	circle(ball.posi.x,ball.posi.y,BALL_R);
	floodfill(ball.posi.x,ball.posi.y,BLACK);
	setcolor(ball.color);
	circle(ball.posi.x,ball.posi.y,BALL_R);
	setcolor(WHITE);
	line(ball.posi.x+off_x-1,ball.posi.y+off_y,ball.posi.x+off_x+1,ball.posi.y+off_y);
	line(ball.posi.x+off_x,ball.posi.y+off_y-1,ball.posi.x+off_x,ball.posi.y+off_y+1);
}

void erase_ball(struct BALL ball)						/**********擦球**********/
{
	setcolor(TABLECOLOR);
	setfillstyle(SOLID_FILL,TABLECOLOR);

	circle(ball.posi.x,ball.posi.y,BALL_R);
	floodfill(ball.posi.x,ball.posi.y,TABLECOLOR);
}

void initstar(struct STAR star[])						/*******初始化星星*******/
{
	int i;
	for (i=0;i<STARNUM;i++)
	{
		star[i].x=random(maxx);
		star[i].y=random(maxy);
	}
}

void drawstar(struct STAR star[])						/*********画星星*********/
{
	static i;
	putpixel(star[i].x,star[i].y,rand()%16);
	i=(1+i)%STARNUM;
}

void fire()									/********火焰动画********/
{
	unsigned int x,y,c;
	setmode();
	for (x=1;x<=32;x++)
	{
		setpal(x,    x*2-1, 0,     0    );
		setpal(x+32, 63,    x*2-1, 0    );
		setpal(x+64, 63,    63,    x*2-1);
		setpal(x+96, 63,    63,    63   );
	}

	while(bioskey(1)==0)
	{
		for (x=0;x<320;x+=2)
		{
			for (y=1;y<=202;y+=2)
			{
				c=(point(x,y)+point(x+2,y)+point(x,y+2)+point(x+2,y+2))>>2;
				if (c--)
				{
					poke(0xa000,(y-2)*320+x,(c<<8)+c);
					poke(0xa000,(y-1)*320+x,(c<<8)+c);
				}
			}
		y-=2;
		pset(x,y,random(320));
		}
	}
	getch();
	closemode();
}

void beforegame()								/******游戏主界面*******/
{
	int i,j;

	fire();

	ShowBmp256(70,65,".\\source\\login.bmp",1,0);

	InitGraph();
	star=malloc(sizeof(struct STAR)*STARNUM);
	initstar(star);
	InitMouse(0,0,maxx,maxy);

	getdat();
	handler=getvect(0x1c);

	mousex=getmaxx()/2;
	mousey=getmaxy()/2;

	logo=bmp_to_dat(".\\source\\snooker.bmp");
	vs1=bmp_to_dat(".\\source\\head0.bmp");
	vs2=bmp_to_dat(".\\source\\head1.bmp");

	play(1);
	for (i=0;i<3;i++)
	{
		putimage(0,0,logo,NOT_PUT);
		for (j=20;j<=1000;j+=10)
		{
			delay(1000);
		}
		delay(3000);
		putimage(0,0,logo,COPY_PUT);
		for (j=1000;j>=20;j-=10)
		{
			delay(2000);
		}
		delay(3000);
	}
	putimage(0,0,logo,COPY_PUT);
	for (i=0;i<STARNUM;i++) drawstar(star);

	outchinese(400,80,s_cn,EOF,4,2);
	putimage(300,280,vs1,COPY_PUT);
	putimage(520,280,vs2,COPY_PUT);

	settextstyle(1,0,12);
	setcolor(GREEN);
	outtextxy(380,260,"Vs");

	drawmouse(mousex,mousey);
	play(2);
	drawmenu(mm);
	while(1)
	{
		while(bioskey(1)!=0) getch();
		position(1);
		mm=fill();
		drawmouse(mousex,mousey);
		if (button==1)
		{
			switch (mm)
			{
				case 0:
					{
						free(logo);
						free(star);
						cleardevice();
						return;
					}
				case 1:
					{
						practice();
						restore();
						break;
					}
				case 2:
					{
						noplay();
						option();
						break;
					}
				case 3:
					{
						noplay();
						toprank();
						restore();
						break;
					}
				case 4:
					{
						if (Music_on==1) play(0);
						showhelp();
						break;
					}
				case 5:
					{
						position(0);
						End(1);
						drawmouse(mousex,mousey);
						break;
					}
			}
		}
		button=0;
	}
}

void restore()									/****恢复屏幕到主菜单****/
{
	int i;
	cleardevice();
	putimage(0,0,logo,COPY_PUT);
	for (i=0;i<STARNUM;i++) drawstar(star);

	outchinese(400,80,s_cn,EOF,4,0);
	drawmenu(mm);
	drawmouse(mousex,mousey);
	if (Music_on) play(2);
	else noplay();
}

void draw_stick(struct BALL ball,double w)					/**********球杆*********/
{
	setcolor(YELLOW);
	setlinestyle(0,0,3);
	setwritemode(XOR_PUT);

	line(ball.posi.x+(BALL_R+5)*cos(w),ball.posi.y+(BALL_R+5)*sin(w),\
	     ball.posi.x+(BALL_R+40)*cos(w),ball.posi.y+(BALL_R+40)*sin(w));

	setcolor(DARKGRAY);
	setlinestyle(0,0,0);
	moveto(ball.posi.x+(BALL_R+41)*cos(w-0.02),ball.posi.y+(BALL_R+41)*sin(w-0.02));

	lineto(ball.posi.x+STICKLENTH*cos(w-0.01),ball.posi.y+STICKLENTH*sin(w-0.01));

	setlinestyle(0,0,3);
	lineto(ball.posi.x+STICKLENTH*cos(w+0.01),ball.posi.y+STICKLENTH*sin(w+0.01));

	setlinestyle(0,0,0);
	lineto(ball.posi.x+(BALL_R+41)*cos(w+0.02),ball.posi.y+(BALL_R+41)*sin(w+0.02));

}

void hitball(struct BALL ball,double w)						/********击球动画********/
{
	struct BALL temp;
	double r;
	temp=ball;
	draw_stick(ball,w);

	for (r=PI;r<=8*PI;r+=0.02)
	{
		temp.posi.x=ball.posi.x+20.0*(cos(r)+1)*cos(w);
		temp.posi.y=ball.posi.y+20.0*(cos(r)+1)*sin(w);
		draw_stick(temp,w);

		delay(300);
		draw_stick(temp,w);
	}
	for (r=0;r<=PI;r+=0.02)
	{
		temp.posi.x=ball.posi.x+(BALL_R+5+(35-BALL_R)*cos(r))*cos(w);
		temp.posi.y=ball.posi.y+(BALL_R+5+(35-BALL_R)*cos(r))*sin(w);
		draw_stick(temp,w);
		draw_stick(temp,w);
	}
}

double energy()
{
	int i = 0;
	int flag=1,kk=0;

	setcolor(WHITE);
	setlinestyle(0,0,0);
	setwritemode(COPY_PUT);
	rectangle(maxx/2-LENTH/2-2,50,maxx/2+LENTH/2+2,60);

	settextstyle(4,0,3);
	setcolor(BLUE);
	outtextxy(170,25,"MIN");
	outtextxy(420,25,"MAX");
	do
	{
		while(bioskey(1)!=0) getch();
		if (0<=(i+flag)&&(i+flag)<=LENTH) i+=flag;
		else flag=-flag;

		setcolor(BLUE);
		setwritemode(XOR_PUT);
		line(maxx/2-LENTH/2+i,52,maxx/2-LENTH/2+i,58);

		if (Music_on)
		{
			sound(15*i);
			delay(400);
			nosound();
		}
		else delay(600);
	}while((kk=bioskey(1))!=ESC && kk!=SPACE);

	setfillstyle(SOLID_FILL,BLACK);
	floodfill(maxx/2-LENTH/2,52,WHITE);

	if (bioskey(0)==ESC) return -1;

	return 4.0*i/LENTH;
}

void End(int flag)
{
	int i;
	int key=0;
	char *buffer;
	char *s[]={"真的退出吗？（ｙ／ｎ）：","是","否","制作小组",\
		   "图形相关：谭歆","主程序：汪航勋","碰撞相关：林远骥",\
		   "菜单等：黄懿可","指导老师：肖少拥","版权所有，盗版不究","谢谢！"};
	noplay();
	if (flag)
	{
		buffer=malloc(imagesize(200,210,440,290));
		getimage(200,210,440,290,buffer);
		draw_window(210,220,430,280);
		outchinese(220,227,s[0],YELLOW,1,0);
		for (i=1;i<=2;i++)
			outchinese(150+i*100,245,s[i],GREEN,1,0);
		drawmouse(mousex,mousey);
		while(1)
		{
			while(bioskey(1)!=0) getch();
			position(0);
			drawmouse(mousex,mousey);
	
			key=bioskey(1);
			if (button==1)
			{
				for (i=1;i<=2;i++)
				{
					if (150+i*100 < mousex && mousex <166+i*100 && 245 <mousey && mousey <261)
						break;
				}
			}
			if (key== LOWER_y || i==1) break;
			else if (key == LOWER_n || i==2)
			{
				position(0);
				putimage(200,210,buffer,COPY_PUT);
				free(buffer);
				if (Music_on) play(2);
				return;
			}
		}
	}
	free(mouse);
	free(mouse_temp);
	free(dash_dot);

	cleardevice();
	while(bioskey(1)!=0) getch();
	for (i=3;i<10;i++)
	{
		outchinese(maxx/2-strlen(s[i])*4,100+18*i+200*(i/9),s[i],WHITE,1,0);
		delay(30000);
		delay(30000);
	}
	getch();
	cleardevice();
	outchinese(250,200,s[10],GREEN,4,1);
	sleep(1);
	closegraph();
	exit(0);
}

void draw_dash(struct BALL ba,double w)						/**********瞄准线**********/
{
	int i,j,R=DASH_LENTH;
	int n,m,in=0;
	int x=0,y=0,butt=0;

	struct LOCATION
	{
		int x;
		int y;
	}*cir;
	struct BALL temp={{0.0,0.0,2.0},{0.0,0.0},{0.0,0.0},0,0,0};

	cir=malloc(sizeof(struct LOCATION)*R/6);

	temp.velo.vx=-temp.velo.v*cos(w);
	temp.velo.vy=-temp.velo.v*sin(w);

	collision_table(&temp);

	temp.posi.x=ba.posi.x+temp.velo.vx;
	temp.posi.y=ba.posi.y+temp.velo.vy;

	cir[0].x=(int)temp.posi.x;
	cir[0].y=(int)temp.posi.y;

	for (m=1;m<R/6;m++)
	{
		for (n=0;n<6;n++)
		{
			collision_table(&temp);
			temp.posi.x+=temp.velo.vx;
			temp.posi.y+=temp.velo.vy;
			if (ball_in(temp))
			{
				in=1;
				break;
			}
		}
		cir[m].x = (int)temp.posi.x;
		cir[m].y = (int)temp.posi.y;
		if (in)
		{
			in=0;
			break;
		}
	}
	if (Line_on) direction(ball[0],ww);
	i=0;
	do
	{
		for (n=0;n<2;n++)
		{
			for (j=i;j<=m;j+=3)
				putimage(cir[j].x-DOT_R,cir[j].y-DOT_R,dash_dot,XOR_PUT);
			delay(6000);
		}
		i=(i+1)%3;
	}while(!check_change(&x,&y,&butt));
	if (Line_on) direction(ball[0],ww);
	if (bioskey(2) & 0x04) neww=ww+x/700.0;
	else
		neww=ww+x/100.0;
	free(cir);
}

void outchinese(int x0,int y0,char *hz,int color,int size,int speed)		/*******汉字输出*********/
{
	FILE *fp;
	int flag=0;
	register int i,j,x,y;
	char mat[ZIKU*ZIKU/8];
	unsigned char high,low;
	unsigned long position=0;
	unsigned MASK=0x80;

	if ((fp=fopen(".\\source\\hzk16","rb"))==NULL)
	{
		closegraph();
		printf("Error!!!");
		exit(0);
	}
	if (color==EOF) flag=1;

	while(*(hz))
	{
		high=*(hz)-0xa0;
		low=*(hz+1)-0xa0;

		position = ((high-1)*94+(low-1))*(ZIKU*ZIKU/8L);
		fseek(fp, position, SEEK_SET);
		fread(mat,sizeof(char),ZIKU*ZIKU/8,fp);
		y=y0;
		for (i=0;i<10*speed;i++)
			delay(20000);

		if (size==1)
		{
			for(i=0;i<ZIKU;i++)
			{
				x=x0;
				for (j = 0; j < ZIKU; j++)
				{
					if((MASK>>(j%8) & *(mat+ZIKU/8*i+j/8)))
						putpixel(x,y,(color==EOF)?random(16):color);
					x++;
				}
			y++;
			}
		}
		else
		{
			setlinestyle(0,0,1);
			setwritemode(COPY_PUT);

			for(i=0;i<ZIKU;i++)
			{
				x=x0;
				for (j = 0; j < ZIKU; ++j)
				{
					if((MASK>>(j%8) & *(mat+ZIKU/8*i+j/8)))
					{
						if (flag) color=random(16);
						setcolor(color);
						rectangle(x,y,x+size-1,y+size-1);
						setfillstyle(SOLID_FILL,color);
						floodfill(x+1,y+1,color);
					}
					x+=size;
				}
			y+=size;
			}
		}
		hz+=2;

		if (x0+(ZIKU+1)*size>getmaxx()-(ZIKU+1)*size)	/*超出屏幕自动换行*/
		{
			y0+=(ZIKU+1)*size;
			x0=0;
		}
		else
			x0+=(ZIKU+1)*size;
	}
	fclose(fp);
}

/**********************************************************/
/***********************  bmp  ****************************/
/**********************************************************/

int ShowBmp(int xx,int yy,char *FileName,int mode,int speed)
{
	FILE *fp;

	register n,i,j;
	long width,height;

	unsigned char far *ptr;

	struct tagCOLORS
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char null;
	}color[16];

	fp=fopen(FileName,"rb");

	fseek(fp,0x12,SEEK_SET);
	fread(&width,4,1,fp);
	fread(&height,4,1,fp);

	fseek(fp,54,SEEK_SET);
	fread(color,4,16,fp);

	for (i=0;i<16;i++)
		setrgbpalette(i,color[i].r>>2,color[i].g>>2,color[i].b>>2);


	n=width*4;
	n=((n+31)>>5)<<2;

	ptr=malloc(n);

	fseek(fp,118,SEEK_SET);

	if (mode==0)
	{
		for(j=height-1;j>=0;j--)
		{
			fread(ptr,n,1,fp);
			for(i=0;i<n;i++)
			{
				putpixel(2*i+xx,j+yy,(ptr[i] >> 4));
				putpixel(2*i+1+xx,j+yy,(ptr[i] & 0x0f));
			}
			delay(speed*1000);
		}
	}
	else if (mode==1)
	{
		for(i=0;i<n;i++)
		{

			for(j=height-1;j>=0;j--)
			{
				fread(ptr,n,1,fp);
				putpixel(2*i+xx,j+yy,(ptr[i] >> 4));
				putpixel(2*(n-1-i)+1+xx,j+yy,(ptr[n-1-i] & 0x0f));
			}
			fseek(fp,-height*n,SEEK_CUR);
			delay(speed*1000);
		}
	}
	else if (mode==2)
	{
		for(i=0;i<n;i++)
		{

			for(j=height-1;j>=0;j--)
			{
				fread(ptr,n,1,fp);
				putpixel(2*i+xx,j+yy,(ptr[i] >> 4));
			}
			fseek(fp,-height*n,SEEK_CUR);
		}
		for(i=n-1;i>=0;i--)
		{

			for(j=height-1;j>=0;j--)
			{
				fread(ptr,n,1,fp);
				putpixel(2*i+1+xx,j+yy,(ptr[i] & 0x0f));
			}
			fseek(fp,-height*n,SEEK_CUR);
			delay(speed*1000);
		}
	}
	free(ptr);
	fclose(fp);
	return NoError;
}

int ShowBmp256(int xx,int yy,char *FileName,int mode,int speed)
{
	unsigned char far *buffer;
	int i,j,k,r,g,b;
	long width,length;
	FILE *fp;

	struct tagCOLOR
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char null;
	}color;

	InitGraph256();

	if((fp=fopen(FileName,"rb"))==NULL)
	{
		printf("can't open file:%s\n",FileName);
		return ErrorFileOpen;
	}
	fseek(fp,28,SEEK_SET);
	fread(&i,2,1,fp);
	if(i!=8)
	{
		puts("Not a 256 color bitmap!");
		fclose(fp);
		return ErrorFileType;
	}

	fseek(fp,18,SEEK_SET);
	fread(&width,4,1,fp);
	fread(&length,4,1,fp);

	buffer=malloc(width);

	fseek(fp,54,SEEK_SET);
	for(i=0;i<256;i++)
	{
		fread(&color,4,1,fp);
		setrgbpalette(i,color.r>>2,color.g>>2,color.b>>2);
	}

	k=(width%4)?(4-width%4):0;			/* 宽度修正值 */

	if (mode==0)
	{
		for(j=length-1;j>=0;j--)
		{
			fread(buffer,width,1,fp);
			for(i=0;i<width && i<maxx;i++)
			{
				putpixel(i+xx,j+yy,buffer[i]);
			}
			fseek(fp,k,SEEK_CUR);	/* 每行绘制完后修正宽度 */
			delay(speed*1000);
		}
	}
	if (mode==1)
	{
		for(i=0;i<width/2;i++)

		{

			for(j=length-1;j>=0;j--)
			{
				fread(buffer,width,1,fp);
				putpixel(i+xx,j+yy,buffer[i]);
				putpixel(width-1-i+xx,j+yy,buffer[width-1-i]);
				fseek(fp,k,SEEK_CUR);	/* 每行绘制完后修正宽度 */
			}
			fseek(fp,-(k+width)*length,SEEK_CUR);
			delay(speed*1000);
		}
	}
	free(buffer);
	sleep(1);
	closegraph();
	fclose(fp);
	return NoError;
}

/****************************************************/
/***********************  end  **********************/
/****************************************************/

char far *bmp_to_dat(char *bmpfile)						/**将16色BMP保存到buffer中**/
{
	unsigned char c[8],scan_times,scan_pixs;
	unsigned char workpos;
	int i,j,k,n,nowpos;
	long iw,ih,size,index;
	static int color[16]={0,4,2,6,1,5,3,7,8,12,10,14,9,13,11,15};
	unsigned char workline[640],scanline[640];
	FILE *fp;
	char far *buffer;

	union
	{
		unsigned char value;
		struct
		{
			unsigned cl:4;
			unsigned ch:4;
		}color;
	}mycolor;

	if((fp=fopen(bmpfile,"rb"))==NULL)
	{
		return NULL;
	}

	fseek(fp,0x2,SEEK_SET);
	fread(&size,sizeof(size),1,fp);

	if ((buffer=malloc(size))==NULL)
	{
		fclose(fp);
		return NULL;
	}

	fseek(fp,0x12,SEEK_SET);

	iw=0;ih=0;

	fread(&iw,4,1,fp);		/*读图像宽度*/
	fread(&ih,4,1,fp);		/*读图像高度*/

	if(iw==0 || ih==0 || iw>640 || ih>480)
	{
		fclose(fp);
		return NULL;
	}
	iw--;
	ih--;					/*∵putimage中的长宽比实际数值少1*/

	scan_times=iw/8+1;			/*行处理单位数*/
	scan_pixs=scan_times*4;			/*行像素字节数∵1单位=4字节*/

	index=0;
	buffer[index++]=iw&0xff;
	buffer[index++]=(iw>>8);
	buffer[index++]=ih&0xff;
	buffer[index++]=(ih>>8);

	fseek(fp,-scan_pixs,SEEK_END);

	for(j=0;j<=ih;j++)
	{
		nowpos=0;
		fread(scanline,scan_pixs,1,fp);
		fseek(fp,-scan_pixs*2,SEEK_CUR);
		for(n=3;n>=0;n--)			/*解码4个位面*/
		{
			for(i=0;i<scan_times;i++)	/*解码各编码单位*/
			{
				workpos=0;
				for(k=0;k<4;k++)	/*分离出8个像素*/
				{
					mycolor.value=scanline[i*4+k];
					c[k*2]=color[mycolor.color.ch];
					c[k*2+1]=color[mycolor.color.cl];
				}
				for(k=0;k<8;k++)
					workpos+=(c[k]>>n&1)<<(7-k);
				workline[nowpos]=workpos;
				nowpos++;
			}
		}
		for (i=0;i<scan_pixs;i++)
		{
			buffer[index+i]=workline[i];
		}
		index+=scan_pixs;
	}
	fclose(fp);
	return buffer;
}

void InitMouse(int minx,int miny,int maxx,int maxy)				/*******初始化鼠标*******/
{
	int retcode;

	regs.x.ax=0;
	int86(0x33,&regs,&regs);
	retcode=regs.x.ax;

	if (retcode==0)
	{
		closegraph();
		printf ("Mouse isn't installed correctly!!!\n");
		getchar();
		exit(1);
	}

	regs.x.ax=7;
	regs.x.cx=minx;
	regs.x.dx=maxx;
	int86(0x33,&regs,&regs);

	regs.x.ax=8;
	regs.x.cx=miny;
	regs.x.dx=maxy;
	int86(0x33,&regs,&regs);
}

void drawmouse(int xx,int yy)							/*********画鼠标*********/
{
	getimage(mousex,mousey,mousex+7,mousey+12,mouse_temp);
	putimage(xx,yy,mouse,COPY_PUT);
}

int check_change(int *x,int *y,int *butt)					/******检查鼠标移动******/
{
	int bu=0;
	regs.x.ax=3;
	int86(0x33,&regs,&regs);

	bu=regs.x.bx;

	regs.x.ax=11;
	int86(0x33,&regs,&regs);

	*x=regs.x.cx;
	*y=regs.x.dx;

	if (*x==0&&*y==0&&*butt==bu&&!bioskey(1)) return 0;
	if (*butt==bu) return 1;
	else
	{
		*butt=bu;
		return 2;
	}
}


void position(int flag)								/*******鼠标位置********/
{
	int movex=0,movey=0;
	while(!check_change(&movex,&movey,&button))
		if (flag) drawstar(star);

	putimage(mousex,mousey,mouse_temp,COPY_PUT);
	mousex=(mousex+movex+maxx)%maxx;
	mousey=(mousey+movey+maxy)%maxy;

}

void playerinfo(m)								/******显示玩家信息*****/
{
	int i;
	char s[16];
	char note[2][10]={"你应当击","球"};
	char nt[8][3]={"红","彩","黄","绿","棕","蓝","粉","黑"};
	setlinestyle(0,0,3);
	setcolor(9);
	setwritemode(COPY_PUT);
	rectangle(5,5,150,95);
	setfillstyle(SOLID_FILL,BLACK);
	floodfill(10,10,9);

	settextstyle(0,0,2);
	setcolor(RED);
	outtextxy(10,10,player[m].name);
	sprintf(s,"%03d",player[m].score);
	outtextxy(10,10+textheight(player[m].name),s);
	outchinese(15,50,note[0],GREEN,1,0);
	outchinese(115,50,note[1],GREEN,1,0);
	if (player[m].pace==-1)
	{
		for (i=16;i<BALLNUM;i++)
			if (!ball[i].ball_in) break;
		outchinese(82,42,nt[i-14],ball[i].color,2,0);
	}
	else
		outchinese(82,42,nt[player[m].pace],(!player[m].pace)?RED:-1,2,0);
	putimage(550,5,player[m].head,COPY_PUT);
}

double ball_distance(struct BALL ball1,struct BALL ball2)			/******计算球间距离******/
{
	double dis;
	dis=sqrt(pow(ball1.posi.x-ball2.posi.x,2)+pow(ball1.posi.y-ball2.posi.y,2));
	return dis;
}


