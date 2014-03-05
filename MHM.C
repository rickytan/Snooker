#include<dos.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<graphics.h>

#define FNX(x) (int)(xo+(x)*1.0)
#define FNY(y) (int)(getmaxy()-(yo+(y)*1.0))
#define FNX2(phi) cos(phi)*ac-sin(phi)*bs
#define FNY2(phi) cos(phi)*as+sin(phi)*bc

#define M1 262
#define M2 296
#define M3 330
#define M4 349
#define M5 392
#define M6 440
#define M7 494
#define H1 523
#define H2 587
#define H3 659
#define H4 699
#define H5 784
#define H6 880
#define H7 988
#define S1 1047
#define S2 1175
#define S3 1319
#define S4 1397
#define S5 1568
#define S6 1760
#define S7 1976

void restore(void);
void ellip(int xo,int yo,int a,int b,double theta);
void draw(int x,int y);

int freq0[]={
		 784,660,588,660,523,523,
         588,494,440,523,392,392,
         330,392,440,523,784,440,523,392,
         784,1048,880,784,660,784,588,588,
         588,660,494,440,392,440,523,588,
         330,523,440,392,440,523,392,392,
         660,784,494,588,440,523,392,392,
         330,392,330,392,392,440,494,588,440,440,392,440,
         523,588,784,660,588,660,588,523,440,392,
         330,523,440,523,440,392,330,392,440,523,
		 392,392,660,784,588,660,588,523,494,440,784,784,0
		 };

int freq1[]={
		S1,S1,S1,H6,S1,S1,H6,S1,S1,
		H7,H7,H7,H5,H7,H7,H5,H7,H7,
		0
		};
int freq2[]={
		M5,H5,H4,H3,H3,H4,H3,H3,H4,H3,H4,H3,H2,H1,H1,H1,H3,H5,
		H6,H6,H5,H5,H2,H2,H4,H3,H3,
		0
		};
int dely2[]={
		3,2,2,4,2,1,5,2,2,2,2,2,1,1,2,2,2,2,
		4,2,1,1,2,2,2,1,1,8,
		0
		};
int dely0[]={
		 25,50,12,12,50,50,
         25,50,12,12,50,50,
         50,38,12,38,12,12,12,25,
         38,12,12,12,12,12,50,50,
         38,12,25,25,38,12,25,25,
         25,25,12,12,12,12,50,50,
         38,12,25,25,12,12,50,25,
         12,12,12,12,12,12,12,12,50,25,12,12,
         38,12,25,25,25,12,12,25,12,12,
         50,50,12,12,12,12,12,12,12,12,
		 50,25,12,12,12,12,12,12,25,25,50,50,0
		 };

int dely1[]={
		 2,2,2,1,2,2,1,2,2,
		 2,2,2,1,2,2,1,2,2,0
		 };

struct ELLI
{
	int x;
	int y;
	int a;
	int b;
	char *s;
	int fill;
}elli[6];

void interrupt music0()
{
	static no,j;
	if (freq0[no]!=0)
	{
		if (j==0) sound(freq0[no]);
		if (j<dely0[no]/5)
		{
			j++;
			return;
		}
		nosound();
		no++;
		j=0;
	}
	else no=0;
}

void interrupt music1()
{
	static no,j;
	if (freq1[no]!=0)
	{
		if (j==0) sound(freq1[no]);
		if (j<dely1[no])
		{
			j++;
			return;
		}
		nosound();
		no++;
		j=0;
	}
	else no=0;
}
void interrupt music2()
{
	static no,j;
	if (freq2[no]!=0)
	{
		if (j==0) sound(freq2[no]);
		if (j<dely2[no]*2)
		{
			j++;
			return;
		}
		nosound();
		no++;
		j=0;
	}
	else no=0;
}
void play(n)
{
	disable();
	setvect(0x1c,music[n]);
	enable();
}
void noplay()
{
	nosound();
	disable();
	setvect(0x1c,handler);
	enable();
}

void drawmenu(m)
{
	int i;
	char *h[6]={"二人对战","练习","选项","高分榜","游戏帮助","退出"};

  	for (i=0;i<6;i++)
	{
		elli[i].x=60+82*(i%2);
		elli[i].y=180+50*i;
		elli[i].a=40;
		elli[i].b=15;
		elli[i].s=h[i];
		elli[i].fill=0;
	}
    setlinestyle(0,0,3);
	setcolor(6);
	line(101,180,101,480);

	setcolor(LIGHTGREEN);
	setlinestyle(0,0,1);
	for (i=0;i<6;i++)
	{
		ellipse(elli[i].x,elli[i].y,0,360,elli[i].a,elli[i].b);
		outchinese(elli[i].x-strlen(elli[i].s)*4,elli[i].y-8,elli[i].s,YELLOW,1,0);
	}

	if(0<=m && m<6)
	{
		elli[m].fill=1;
		setcolor(RED);
		ellipse(elli[m].x,elli[m].y,0,360,elli[m].a,elli[m].b);
	}
}
int fill()
{
	int i,temp=0;
	int index=-1;
	setlinestyle(0,0,0);
	for (i=0;i<6;i++)
	{
		temp=elli[i].fill;

		if (pow(((double)mousex-elli[i].x)/elli[i].a,2)+pow(((double)mousey-elli[i].y)/elli[i].b,2) < 1)
		{
			elli[i].fill=1;
			index=i;
		}
		else elli[i].fill=0;

		if (temp!=elli[i].fill && elli[i].fill)
		{
			setcolor(RED);
			ellipse(elli[i].x,elli[i].y,0,360,elli[i].a,elli[i].b);
		}
		else if (temp!=elli[i].fill)
		{
			setcolor(LIGHTGREEN);
            ellipse(elli[i].x,elli[i].y,0,360,elli[i].a,elli[i].b);
		}
	}
	return index;
}

void colli_sound()
{
	int freq;
	for(freq=1060;freq>0;freq-=20)
   	{
      		sound (freq);
			delay(100);
   	}
   	nosound();
}

void ellip(int xo,int yo,int a,int b,double theta)
{
	int i;
	double da,c,s,ac,as,bc,bs,phi,x,y;
	theta=theta*0.01745;
	da=3*0.1745;
	c=cos(theta);s=sin(theta);
	ac=a*c;as=a*s;bc=b*c;bs=b*s;
	x=FNX2(0);y=FNY2(0);
	moveto(FNX(x),FNY(y));
	for(i=1;i<=360;i++)
	{
		phi=i*da;
		x=FNX2(phi);y=FNY2(phi);
		lineto(FNX(x),FNY(y));
	}
}

void draw(int x,int y)
{
	register i;
/*画粉红色玫瑰*/
	setcolor(12);
	arc(x+65,y-60,150,350,8);
	arc(x+66,y-54,300,470,8);
	arc(x+65,y-56,30,230,10);
	arc(x+64,y-57,300,490,17);
	ellipse(x+73,y-30,250,450,27,40);
	ellipse(x+59,y-30,100,290,27,40);
	ellipse(x+65,y-40,140,270,20,30);
	setfillstyle(SOLID_FILL,5);
	floodfill(x+65,y-20,12);
/*画红色玫瑰*/
	arc(x,y,150,350,12);
	arc(x+1,y+8,280,470,12);
	arc(x,y+2,30,230,16);
	arc(x,y+3,80,240,28);
	arc(x+2,y+8,180,330,22);
	arc(x-2,y+2,310,460,25);
	ellipse(x-12,y+30,120,300,30,40);
	ellipse(x+10,y+28,250,423,30,42);
	ellipse(x-4,y+10,290,393,30,40);
	setfillstyle(SOLID_FILL,4);
	floodfill(x+5,y+31,12);
/*画紫色花骨朵*/
	ellipse(x+120,y+5,0,360,15,25);
	setfillstyle(SOLID_FILL,1);
	floodfill(x+120,y,12);
/*画黄色花骨朵*/
	ellipse(x-70,y+10,0,360,14,20);
	setfillstyle(SOLID_FILL,14);
	floodfill(x-70,y+10,12);
	setcolor(10);
/*画红花花萼*/
	ellipse(x-15,y+32,190,310,30,35);
	ellipse(x+16,y+32,235,355,26,35);
	ellipse(x,y+35,190,350,43,50);
	arc(x,y+82,190,350,6);
	setfillstyle(SOLID_FILL,2);
	floodfill(x,y+75,10);
/*画粉花花萼*/
	ellipse(x+50,y-48,190,320,22,50);
	ellipse(x+80,y-48,220,350,22,50);
	ellipse(x+65,y-28,180,360,36,50);
	floodfill(x+65,y+18,10);
/*画主枝*/
	for(i=0;i<3;i++ )
	{
		ellipse(x-98,y+100+i,255,371,100,80);
		ellipse(x-20,y+30+i,260,358,140,140);
		ellipse(x+224,y+20+i,180,218,160,140);
	}
/*画侧枝*/
	ellipse(x+70,y+34,180,233,140,140);
	ellipse(x,y+40,205,255,100,120);
	ellipse(x+135,y-30,209,249,72,120);
	ellipse(x,y+20,263,301,100,120);
	ellipse(x+85,y-10,278,305,100,120);
	ellipse(x+100,y-62,282,308,90,120);
	ellipse(x-50,y-10,277,314,30,120);
	ellipse(x+70,y+80,222,266,52,120);
	ellipse(x-60,y-45,229,266,52,120);
	ellipse(x+79,y-45,229,266,52,120);
	ellipse(x+84,y,224,273,52,120);
	ellipse(x+110,y+40,240,282,100,120);
/*画紫花骨朵花萼*/
	ellipse(x+120,y-6,200,340,17,25);
	ellipse(x+120,y+7,160,380,17,27);
	floodfill(x+122,y+30,10);
/*画黄花骨朵花萼*/
	ellipse(x-70,y+15,140,390,17,20);
	ellipse(x-75,y-10,205,340,10,30);
	ellipse(x-60,y-10,195,340,5,30);
	floodfill(x-70,y+32,10);
/*画叶*/
/*画右边的叶子*/
	ellip(x+168,y+282,10,20,-40);
	ellip(x+160,y+252,8,16,260);
	ellip(x+145,y+270,8,16,-15);
	ellip(x+156,y+224,10,20,-45);
	ellip(x+150,y+200,8,16,270);
	ellip(x+135,y+220,8,16,-10);
	ellip(x+146,y+144,8,16,-80);
	ellip(x+130,y+130,6,12,235);
	ellip(x+125,y+154,7,14,-10);
	ellip(x+78,y+98,6,12,-90);
	ellip(x+60,y+90,5,10,180);
	ellip(x+70,y+109,5,10,-45);
/*画左边的叶子*/
	ellip(x-125,y+270,12,24,60);
	ellip(x-95,y+270,10,20,10);
	ellip(x-110,y+245,10,20,90);
	ellip(x-105,y+220,10,20,45);
	ellip(x-100,y+190,8,16,135);
	ellip(x-75,y+210,8,16,-45);
/*画中间的叶子*/
	ellip(x+65,y+190,10,20,-45);
	ellip(x+40,y+185,8,16,0);
	ellip(x+55,y+165,8,16,90);
}
void showhelp()
{
	int i;
	char *help[]={
			"使用相同的主球击打目标球，共",
			"有目标球二十一只，十五只红球",
			"各一分，黄色球两分，绿色球三",
			"分，棕色球四分，蓝色球五分，",
			"粉色球六分，黑色球七分。将红",
			"色球与彩色球分别交替击落袋直",
			"至所有红色球全部离台，然后按",
			"彩球分值由低至高的顺序也至全",
			"部离台为止。最后得分高者胜！",
			"游戏中使用鼠标左右移动调整球",
			"杆角度（上下移动无效！），按",
			"住Ｃｔｒｌ＋左右移动可以微调",
			"按空格键选择击球力度，再按一",
			"次击球，若想再调整，按Ｅｓｃ",
			"。游戏过程中按Ｅｓｃ随时可以",
			"退出。",
			"★★★★玩得愉快！！☆☆☆☆",
			NULL};

	char *s={"按任意键退出……"};
	cleardevice();
	while(bioskey(1)!=0) getch();
	draw(450,90);
	rectangle(0,0,639,479);
  	setbkcolor(BLACK);

  	setfillstyle(1,2);
  	setcolor(YELLOW);
  	rectangle(10,10,300,440);
  	floodfill(50,50,14);          /*  用绿色填画出的矩形框	*/
  	rectangle(30,30,280,420);
	rectangle(5,5,305,445);
	setfillstyle(1,13);
	floodfill(7,7,14);          /*  用淡洋红色填画出的矩形框	*/
  	setcolor(BLACK);
  	settextstyle(1,0,6);          /*  设要显示字符串的字形方向,尺寸	*/
	outtextxy(100,40,"help");
	outchinese(5,460,s,WHITE,1,0);
	for(i=0;help[i]!=NULL;i++)
	{
		outchinese(36,100+i*17,help[i],WHITE,1,0);
	}
	getch();
	restore();

}

void draw_window(int x1,int y1,int x2,int y2)
{
	setwritemode(COPY_PUT);
	setcolor(MAGENTA);
	setlinestyle(0,0,0);
	rectangle(x1-3,y1-3,x2+3,y2+3);
	setfillstyle(SOLID_FILL,BLACK);
	floodfill(x1,y1,MAGENTA);

	setcolor(YELLOW);
	setlinestyle(0,0,3);
	rectangle(x1,y1,x2,y2);

	setcolor(GREEN);
	setlinestyle(0,0,1);
	rectangle(x1+5,y1+5,x2-5,y2-5);
}

void toprank()
{
	FILE *fp;
	int i;
	struct RANK
	{
		char s[20];
		int score;
		int rank;
	}temp;
	char info[40];

	char *s[]={"玩家姓名","得分","排行","按任意键退出……"};
	cleardevice();
	while(bioskey(1)!=0) getch();
	draw_window(170,40,470,400);

	for (i=0;i<3;i++)
		outchinese(200+100*i,60,s[i],RED,1,0);
	outchinese(178,376,s[3],WHITE,1,0);
	if ((fp=fopen(".\\source\\rank.dat","rb"))==NULL)
	{
		outchinese(230,100,"目前无排名",WHITE,2,0);
	}
	else
	{
		i=0;
		while(feof(fp)==0)
		{
			fscanf(fp,"%s%d%d",temp.s,&temp.score,&temp.rank);
			sprintf(info,"%8s     %3d          %2d",temp.s,temp.score,temp.rank);
			settextstyle(0,0,0);
			outtextxy(200,80+i*textheight("H"),info);
			i++;
		}
	}
	fclose(fp);

	getch();
}

void option()
{
	int i,confirm=0;
	char *s[]={"声音：","进球难度：","方向线：","关闭","开启","困难","普通","简单","关闭","开启","确认"};
	struct
	{
		int x;
		int y;
	}loc[]=
		{
			260,100,
			340,100,
			260,150,
			300,150,
			340,150,
			260,200,
			340,200,
			410,370
		};

	cleardevice();
	while(bioskey(1)!=0) getch();
	draw_window(170,40,470,400);

	outchinese(256,50,"游戏设置",BLUE,2,0);

	for (i=0;i<3;i++)
		outchinese(181,101+50*i,s[i],GREEN,1,0);
	for (i=3;i<11;i++)
	{
		outchinese(loc[i-3].x+1,loc[i-3].y+1,s[i],13+2*(i/10),1,0);
	}
	setcolor(LIGHTGREEN);
	rectangle(loc[Music_on].x,loc[Music_on].y,loc[Music_on].x+37,loc[Music_on].y+18);
	rectangle(loc[2+Diffi/2].x,loc[2+Diffi/2].y,loc[2+Diffi/2].x+37,loc[2+Diffi/2].y+18);
	rectangle(loc[5+Line_on].x,loc[5+Line_on].y,loc[5+Line_on].x+37,loc[5+Line_on].y+18);
	drawmouse(mousex,mousey);

	do
	{
		while (bioskey(1)!=0) getch();
		position(1);
		if (button==1)
		{
			for (i=0;i<8;i++)
			{
				if (loc[i].x < mousex && mousex < loc[i].x+37 && loc[i].y < mousey && mousey < loc[i].y+18)
					break;
			}
			if (i<2 && i!=Music_on)
			{
				setcolor(BLACK);
				rectangle(loc[Music_on].x,loc[Music_on].y,loc[Music_on].x+37,loc[Music_on].y+18);
				Music_on=i;
				setcolor(LIGHTGREEN);
				rectangle(loc[Music_on].x,loc[Music_on].y,loc[Music_on].x+37,loc[Music_on].y+18);
			}
			else if (2<=i && i<5 && (i-2)!=Diffi/2)
			{
				setcolor(BLACK);
				rectangle(loc[2+Diffi/2].x,loc[2+Diffi/2].y,loc[2+Diffi/2].x+37,loc[2+Diffi/2].y+18);
				Diffi=2*(i-2);
				setcolor(LIGHTGREEN);
				rectangle(loc[2+Diffi/2].x,loc[2+Diffi/2].y,loc[2+Diffi/2].x+37,loc[2+Diffi/2].y+18);
			}
			else if (5<=i && i<7 && (i-5)!=Line_on)
			{
				setcolor(BLACK);
				rectangle(loc[5+Line_on].x,loc[5+Line_on].y,loc[5+Line_on].x+37,loc[5+Line_on].y+18);
				Line_on=i-5;
				setcolor(LIGHTGREEN);
				rectangle(loc[5+Line_on].x,loc[5+Line_on].y,loc[5+Line_on].x+37,loc[5+Line_on].y+18);
			}
			else if (i==7)
				confirm=1;
		}
		drawmouse(mousex,mousey);
		button=0;
	}while(!confirm);
	restore();
}