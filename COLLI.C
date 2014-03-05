#include<math.h>
#include<stdio.h>



int
collision_table (struct BALL *t_ball)
{
	int i;
	struct BALL balle;
        double w;
	double dot[12][2]={
			54.142,111,
			13,152.142,
			13,427.858,
			54.142,469,
			291.326,469,
			346.673,469,
			583.858,469,
			626,427.858,
			626,152.142,
			583.858,111,
			346.673,111,
			291.326,111,
			};


	if (t_ball->posi.x + t_ball->velo.vx <= TABLEMINX + BALL_R)
	{
		if (t_ball->posi.y + t_ball->velo.vy <= 427.858 \
		 && t_ball->posi.y + t_ball->velo.vy >= 152.142)
		{
			t_ball->velo.vx = -t_ball->velo.vx;
			return 1;
		}
	}
	if (t_ball->posi.x + t_ball->velo.vx >= TABLEMAXX - BALL_R)
	{
		if (t_ball->posi.y + t_ball->velo.vy <= 427.858 \
		 && t_ball->posi.y + t_ball->velo.vy >= 152.142)
		{
			t_ball->velo.vx = -t_ball->velo.vx;
			return 1;
		}
	}

	if (t_ball->posi.y + t_ball->velo.vy >= TABLEMAXY - BALL_R)
	{
		if (t_ball->posi.x + t_ball->velo.vx <= 583.858
		 && t_ball->posi.x + t_ball->velo.vx >= 346.673
		 || t_ball->posi.x + t_ball->velo.vx <= 299
		 && t_ball->posi.x + t_ball->velo.vx >= 54.142)
		{
			t_ball->velo.vy = -t_ball->velo.vy;
			return 1;
		}
	}

	if (t_ball->posi.y + t_ball->velo.vy <= TABLEMINY + BALL_R)
	{
		if (t_ball->posi.x + t_ball->velo.vx <= 583.858
		 && t_ball->posi.x + t_ball->velo.vx >= 346.673
		 || t_ball->posi.x <= 291.326 + t_ball->velo.vx
		 && t_ball->posi.x + t_ball->velo.vx >= 54.142)
		{
		t_ball->velo.vy = -t_ball->velo.vy;
		return 1;
		}
	}

	balle=*t_ball;
	balle.velo.vx=-balle.velo.vx;
	balle.velo.vy=-balle.velo.vy;
	for (i=0;i<12;i++)
	{

                balle.posi.x=dot[i][0];
                balle.posi.y=dot[i][1];
		if (is_collision(*t_ball,balle))
		{

			collision(t_ball,&balle);
			return 1;
		}
	}

	return 0;
}

int
is_collision (struct BALL ball_a, struct BALL ball_b)
{
	double distance;
	distance =
		sqrt(pow((ball_a.posi.x + ball_a.velo.vx - ball_b.posi.x - ball_b.velo.vx), 2) +
			 pow((ball_a.posi.y + ball_a.velo.vy - ball_b.posi.y - ball_b.velo.vy), 2));
	if (distance <= 2 * BALL_R)
		return 1;
	else
		return 0;

}

void collision (struct BALL *ball_a, struct BALL *ball_b)
{
	double sin, cos, ball_avx, ball_avy, ball_bvx, ball_bvy;
	double xx,yy,dd;
	int flag;

	yy=ball_a->posi.y - ball_b->posi.y;
	xx=ball_a->posi.x - ball_b->posi.x;
	dd=sqrt(xx*xx+yy*yy);

	sin=fabs(yy/dd);
	cos=fabs(xx/dd);

	if ((ball_a->posi.y - ball_b->posi.y) * (ball_a->posi.x - ball_b->posi.x) >= 0)
										/**********改变球速度坐标至球心连线的切向及法向***********/
    {
		flag = 1;
		ball_avx = cos * ball_a->velo.vx + sin * ball_a->velo.vy;
		ball_avy = cos * ball_a->velo.vy - sin * ball_a->velo.vx;
		ball_bvx = cos * ball_b->velo.vx + sin * ball_b->velo.vy;
		ball_bvy = cos * ball_b->velo.vy - sin * ball_b->velo.vx;
    }
	if ((ball_a->posi.y - ball_b->posi.y) * (ball_a->posi.x - ball_b->posi.x) < 0)
										/**********改变球速度坐标至球心连线的切向及法向***********/
    {
		flag = 0;
		ball_avx = cos * ball_a->velo.vx - sin * ball_a->velo.vy;
		ball_avy = cos * ball_a->velo.vy + sin * ball_a->velo.vx;
		ball_bvx = cos * ball_b->velo.vx - sin * ball_b->velo.vy;
		ball_bvy = cos * ball_b->velo.vy + sin * ball_b->velo.vx;
    }

								   /**********碰撞***********/
	ball_a->velo.vx = ((1 - E) * ball_avx + (1 + E) * ball_bvx) / 2;
	ball_a->velo.vy = ball_avy;
	ball_b->velo.vx = ((1 + E) * ball_avx + (1 - E) * ball_bvx) / 2;
	ball_b->velo.vy = ball_bvy;


	if (flag == 1)						   /**********恢复球速度坐标至xy***********/
    {
		ball_avx = cos * ball_a->velo.vx - sin * ball_a->velo.vy;
		ball_avy = cos * ball_a->velo.vy + sin * ball_a->velo.vx;
		ball_bvx = cos * ball_b->velo.vx - sin * ball_b->velo.vy;
		ball_bvy = cos * ball_b->velo.vy + sin * ball_b->velo.vx;
    }

	if (flag == 0)						    /**********恢复球速度坐标至xy***********/
    {
		ball_avx = cos * ball_a->velo.vx + sin * ball_a->velo.vy;
		ball_avy = cos * ball_a->velo.vy - sin * ball_a->velo.vx;
		ball_bvx = cos * ball_b->velo.vx + sin * ball_b->velo.vy;
		ball_bvy = cos * ball_b->velo.vy - sin * ball_b->velo.vx;
    }
								     /**********改变球速度***********/
	ball_a->velo.vx = ball_avx;
	ball_a->velo.vy = ball_avy;
	ball_b->velo.vx = ball_bvx;
	ball_b->velo.vy = ball_bvy;

	ball_a->velo.v = sqrt (ball_a->velo.vx * ball_a->velo.vx + ball_a->velo.vy * ball_a->velo.vy);
	ball_b->velo.v = sqrt (ball_b->velo.vx * ball_b->velo.vx + ball_b->velo.vy * ball_b->velo.vy);

}

int
ball_in (struct BALL ball)
{
  int i;
  double distance;
  for (i = 0; i < 6; i++)
    {
      distance =
	sqrt (pow ((net[i].x - ball.posi.x), 2) +
	      pow ((net[i].y - ball.posi.y), 2));
	  if (distance <= NET_R+Diffi)
	{
	  ball.ball_in = 1;	    /**************按坐标差判断球进*************/
	  return 1;
	}
    }
  return 0;
}

void
direction (struct BALL balle, double w)
{
	int i, flag = 0, colliball;

	while (balle.posi.x < TABLEMAXX
		&& balle.posi.x > TABLEMINX
		&& balle.posi.y < TABLEMAXY
		&& balle.posi.y > TABLEMINY)
	{
		flag = 0;
		balle.posi.x -= 2*cos (w);
		balle.posi.y -= 2*sin (w);

		for (i=1;i < ((ispractising)?2:BALLNUM);i++)
		{
			if (ball[i].ball_in) continue;
			if (is_collision (balle, ball[i]))
			{
				flag = 1;
				colliball = i;
				break;
			}
		}

		if (flag)
		{
			setwritemode(XOR_PUT);
			setlinestyle(3,0,1);
			setcolor(WHITE);
			line(ball[colliball].posi.x,
				 ball[colliball].posi.y,
				 ball[colliball].posi.x+(ball[colliball].posi.x-balle.posi.x)*3,
				 ball[colliball].posi.y+(ball[colliball].posi.y-balle.posi.y)*3);

			break;
		}
    }
}
