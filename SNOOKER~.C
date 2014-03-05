#include "variable.h"
#include "mhm.c"
#include "colli.c"
#include "GRAPHS.C"


int	change;
int	turn;

struct BALL past[BALLNUM-16];

int ballin[BALLNUM]={0};

void record()
{
	FILE *fp;
	int i,n;
	struct INFO
	{
		char name[20];
		int score;
		int rank;
		struct INFO *next;

	}*info[10],*head,*p1,*p2,p[2];

	if ((fp=fopen(".\\source\\rank.dat","r"))==NULL)
	{
		printf("File open Error!!!");
		getch();
		return;
	}
	for (i=0;i<2;i++)
	{
		strcpy(p[i].name,player[i].name);
		p[i].score=player[i].score;
		p[i].next=NULL;
	}
	for (i=0;i<10;i++)
		info[i]=malloc(sizeof(struct INFO));

	n=0;
	while(!feof(fp) && n<10)
	{
		fscanf(fp,"%s%d%d",info[n]->name,&info[n]->score,&info[n]->rank);
		info[n]->next=info[n+1];
		n++;
	}
	fclose(fp);
	info[n-1]->next=NULL;
	head=info[0];

	for (i=0;i<2;i++)
	{
		if (p[i].score>head->score)
		{
			p[i].next=head;
			head=p+i;
			continue;
		}
		for (p1=head;p1->next!=NULL;p1=p2)
		{
			p2=p1->next;
			if (p[i].score>p2->score)
			{
				p[i].next=p2;
				p1->next=p+i;
				break;
			}
		}
		if (p[i].score<=p1->score)
			p1->next=p+i;
	}
	fp=fopen(".\\source\\rank.dat","w");
	for (i=0,p1=head;p1!=NULL && i<10;p1=p1->next)
	{
		fprintf(fp,"%s %d %d\n",p1->name,p1->score,++i);
	}
	for (i=0;i<10;i++)
		free(info[i]);
	fclose(fp);
}


void win()
{
	int n;
	char *s[]={"这局的赢家是","平局"};

	if (player[0].score==player[1].score) n=-1;
	else n=(player[0].score < player[1].score);

	cleardevice();
	noplay();

	record();

	if (n!=-1)
	{
		outchinese(320-strlen(s[0])*4*4,100,s[0],YELLOW,4,1);
		settextstyle(0,0,10);
		setcolor(GREEN);
		outtextxy(200,170,player[n].name);
		sleep(2);
		closegraph();
		ShowBmp256(70,50,".\\source\\winner.bmp",0,0);
		InitGraph();
		toprank();
		End(0);
	}
	else
	{
		outchinese(320-strlen(s[1])*4*4,100,s[1],YELLOW,4,0);
		toprank();
		End(0);
	}
}

void check_overlap(int ba)
{
	int i;
	int loop;
	double dis;

	do
	{
		loop=0;
		for (i=0;i<BALLNUM;i++)
		{
			if (i==ba || ball[i].ball_in) continue;
			dis=ball_distance(ball[ba],ball[i]);
			if (dis == 0)
			{
				ball[ba].posi.x-=2*BALL_R;
				loop=1;
			}
			else if (dis<=2*BALL_R)
			{
				ball[ba].posi.x=ball[i].posi.x+2*(BALL_R+1)*(ball[ba].posi.x-ball[i].posi.x)/dis;
				ball[ba].posi.y=ball[i].posi.y+2*(BALL_R+1)*(ball[ba].posi.y-ball[i].posi.y)/dis;
				loop=1;
			}
		}
	}while(loop);
}

void resetball(int m)
{
	ball[m].posi.x=ball[m].origin.x;
	ball[m].posi.y=ball[m].origin.y;

	check_overlap(m);
	ball[m].velo.v = ball[m].velo.vx = ball[m].velo.vy = 0;
	ball[m].ball_in = 0;

	draw_ball (ball[m]);
}



void putball()
{
	int i;
	int movex=0,movey=0;
	double dis1,dis2,mindis;
	ball[0].posi.x=ball[0].origin.x;
	ball[0].posi.y=ball[0].origin.y;

	check_overlap(0);

	draw_ball(ball[0]);
	do
	{
		while(bioskey(1)) getch();
		while(!check_change(&movex,&movey,&button));

		mindis=sqrt(pow((ball[0].posi.x+movex-ball[1].posi.x),2)+pow((ball[0].posi.y+movey-ball[1].posi.y),2));
		if (!ispractising)
		{
			for (i=2;i<BALLNUM;i++)
			{
				dis1=sqrt(pow((ball[0].posi.x+movex-ball[i].posi.x),2)+pow((ball[0].posi.y+movey-ball[i].posi.y),2));
				mindis=(dis1<mindis)?dis1:mindis;
			}
		}
		dis2=sqrt(pow(ball[0].posi.x+movex-137,2)+pow(ball[0].posi.y+movey-290,2));
		if (dis2<=53 && mindis>=(2*BALL_R+2) && (ball[0].posi.x+movex)<=137)
		{
			erase_ball(ball[0]);
			ball[0].posi.x+=movex;
			ball[0].posi.y+=movey;
			draw_ball(ball[0]);
		}
	}while(button!=1);
	button=0;
	ball[0].ball_in=0;
}

void adjust()
{
	int key;
	double force;

	draw_stick(ball[0],ww);
	do
	{
		while(bioskey(1)!=0) getch();
		force=0;
		draw_dash(ball[0],ww);

		while(1)
		{
			key=0;
			if (bioskey(1))
			{
				key=bioskey(0);

				switch(key)
				{
					case ESC:key=-1;break;
					case SPACE:key=-2;break;
				}

			}
			if (key==-1 && ispractising)
			{
				ispractising=0;
				return;
			}
			if (key==-1) End(1);
			if (key==-2) break;
			if (neww!=ww)
			{
				draw_stick(ball[0],ww);
				draw_stick(ball[0],neww);
				ww=neww;
			}

			draw_dash(ball[0],ww);

			if (ww<0) ww+=2*PI;
			if (ww>2*PI) ww-=2*PI;
		}
	}while((force=energy())==-1);

	ball[0].velo.v=2*force;
	ball[0].velo.vx=-ball[0].velo.v*cos(ww);
	ball[0].velo.vy=-ball[0].velo.v*sin(ww);

	hitball(ball[0],ww);
}

void InitPlayer ()
{
	int i;
	char s[20];
	char *n[2]={"①","②"};

	while(bioskey(1)) getch();
	for (i = 0; i < 2; i++)
	{
		player[i].score = 0;
		sprintf (s, ".\\source\\head%d.bmp", i);
		player[i].head = bmp_to_dat (s);
		player[i].pace=0;
		draw_window(210,212,430,246);
		sprintf (s,"玩家%s大名：",n[i]);
		outchinese(220,222,s,WHITE,1,0);
		gotoxy(41,15);
		scanf("%s",player[i].name);

	}
	cleardevice();
}

int move(int ballnumber)
{
	int i,j,k;
	int check_colli=0;
	int loop=0,index=0;
	int draw_bound=0;
	int first_colli=-1;

	do
	{
		loop=0;

		do
		{
			check_colli=0;
			draw_bound=0;
			for (i=0;i<ballnumber;i++)
			{
				if (ball[i].ball_in)
				{
					delay(20);
					continue;
				}
				if (collision_table(ball+i))
				{
					draw_bound=1;
					check_colli=1;
				}

				for (j=0;j<ballnumber;j++)
				{
					if (ball[j].ball_in || j==i)
					{
						delay(20);
						continue;
					}
					if (is_collision(ball[i],ball[j]))
					{
						if (first_colli==-1 && i==0) first_colli=j;
						colli_sound();
						collision(ball+i,ball+j);
						check_colli=1;
					}
				}
			}
			if (index==5) break;
			index++;
		}while(check_colli);

		for (i=0;i<ballnumber;i++)
		{
			if (!ball[i].ball_in && ball[i].velo.v!=0)
			{
				loop=1;
				erase_ball(ball[i]);

				ball[i].posi.x+=ball[i].velo.vx;
				ball[i].posi.y+=ball[i].velo.vy;

				draw_ball(ball[i]);

				if (draw_bound) draw_boundary();

				ball[i].velo.vx-=ACCEL*ball[i].velo.vx/ball[i].velo.v;
				ball[i].velo.vy-=ACCEL*ball[i].velo.vy/ball[i].velo.v;
				ball[i].velo.v-=ACCEL;
				if (ball[i].velo.v<.01)
				{
					ball[i].velo.vx=0;
					ball[i].velo.vy=0;
					ball[i].velo.v=0;
				}

				if (ball_in(ball[i]))
				{
					ball[i].ball_in=1;
					erase_ball(ball[i]);

					ball[i].posi.x=0;
					ball[i].posi.y=0;
					ball[i].velo.v=ball[i].velo.vx=ball[i].velo.vx=0;

					draw_net();
				}
			}
			else delay(80);
		}
	}while(loop);
	return first_colli;
}

void practice(void)
{
	int i, j;
	InitNet (net);
	cleardevice ();
	draw_table ();
	InitBall(ball);

	draw_ball (ball[1]);
	ispractising = 1;

	putball();
	do
	{
		adjust ();
		if (ispractising == 0)
			return;
		move (2);
		if (ball[1].ball_in) resetball(1);
		if (ball[0].ball_in) putball();
	}while (ispractising);
}

void check_faul(int num)
{
	int i,j;
	static flag=0;
	change=1;
	redflag=1;
	for (i=1;i<=15;i++)
		if (!ball[i].ball_in)
		{
			redflag=0;
			break;
		}
	if (redflag && flag<2) flag++;
	if (ball[0].ball_in)
	{
		for (i=16;i<BALLNUM;i++)
		{
			if (ballin[i]==0 && ball[i].ball_in==1)
			{
				resetball(i);
			}
		}

		turn^=1;
		player[turn].score+=4;

		if (!redflag)
		{
			player[0].pace=0;
			player[1].pace=0;
		}
		else
		{
			player[0].pace=-1;
			player[1].pace=-1;
		}
	}
	else
	{
		if (num==-1)
		{
			turn^=1;
			player[turn].score+=4;
			if (!redflag)
			{
				player[0].pace=0;
				player[1].pace=0;
			}
			else
			{
				player[0].pace=-1;
				player[1].pace=-1;
			}
		}

		else if (player[turn].pace==0 && 1<=num && num<=15 || player[turn].pace==1 && 16<=num || player[turn].pace==-1)
		{
			for (i=1;i<=15;i++)
			{
				if (ballin[i]==0)
				{

					if (ball[i].ball_in==1)
					{
						player[turn].score+=ball[i].score;
						ballin[i]=1;
						change=0;
					}
				}
			}

			for (i=16;i<BALLNUM;i++)
			{
				if (ballin[i]==0 && ball[i].ball_in==1)
				{

					if (flag<2)
					{
						resetball(i);
						player[turn].score+=ball[i].score;
						change=0;
					}

					else
					{
						player[0].pace=-1;
						player[1].pace=-1;

						for (j=16;j<BALLNUM;j++)
							if (!ballin[j])
								break;

						if (num==j && i==j)
						{
							ballin[i]=1;
							player[turn].score+=ball[i].score;
							change=0;
						}
						else if (num==j)
						{
							resetball(i);
							player[turn].score+=ball[i].score;
							change=0;
						}
						else
						{
							ball[i].posi.x=past[i-16].posi.x;
							ball[i].posi.y=past[i-16].posi.y;
							check_overlap(i);
							ball[i].ball_in=0;
							draw_ball(ball[i]);
							player[turn^1].score+=4;
						}
					}
				}
			}

			if (change)
			{
				turn^=1;

				if (!redflag)
				{
					player[0].pace=0;
					player[1].pace=0;
				}
				else
				{
					player[0].pace=-1;
					player[1].pace=-1;
				}
			}
			else if(!redflag)
				player[turn].pace^=1;
			else
				player[0].pace=player[1].pace=-1;
		}
		else
		{
			turn^=1;

			if (!redflag)
			{
				player[0].pace=0;
				player[1].pace=0;
			}
			else
			{
				player[0].pace=-1;
				player[1].pace=-1;
			}

			player[turn].score+=4;
			for (i=1;i<=15;i++)
			{
				ballin[i]=ball[i].ball_in;
			}
			for (i=16;i<BALLNUM;i++)
			{
				if (ballin[i]==0 && ball[i].ball_in==1)
				{
					ball[i].posi.x=past[i-16].posi.x;
					ball[i].posi.y=past[i-16].posi.y;

					check_overlap(i);

					ball[i].ball_in=0;
					draw_ball(ball[i]);

				}
			}
		}
	}
}


int main ()
{

	int i, j, k;
	int GameEnd=0;
	beforegame ();

	InitNet (net);
	InitBall (ball);
	InitPlayer ();
	draw_table ();

	playerinfo (turn);

	for (i=1;i<BALLNUM;i++)
		draw_ball(ball[i]);

	putball();

	while(!GameEnd)
	{
		GameEnd=1;
		adjust ();
		for (i=16;i<BALLNUM;i++)
			past[i-16]=ball[i];

		check_faul(move(BALLNUM));
		for (i=1;i<BALLNUM;i++)
			if (ball[i].ball_in==0)
			{
				GameEnd=0;
				break;
			}

		playerinfo(turn);

		if (ball[0].ball_in)
			putball();

	}
	win();
}
