/*********************************************************/
/*                     VGA13Hģʽ����                    */
/*********************************************************/
/*********************************************************/

# if !defined(__DOS_DEF_)
#    include <dos.h>
# endif
/* �˴�������һЩͨ�õĺ� */

# define BYTE  unsigned char
# define WORD  unsigned int
# define DWORD unsigned long
# define BOOL  BYTE
# define TRUE  1
# define FALSE !TRUE

/* BIOS 8*8 �����ֿ�Ķε�ַ��ƫ���� */
WORD FONT_SEG;
WORD FONT_OFF;

void setmode();
void waitkey();
void closemode();
void GetFontAdd();
void locate(int Line,int Col);
void pset(int x,int y,BYTE color);
void setpal(int Color,BYTE r,BYTE g,BYTE b);
BYTE ScanKey(void);
BYTE point(int x,int y);

/* ��ȡBIOS 8*8 �����ֿ�Ķε�ַ��ƫ���� */
void GetFontAdd()
{
  struct REGPACK regs;
  regs.r_bx=0x0300;
  regs.r_ax=0x1130;
  intr(0x10,&regs);
  FONT_SEG=regs.r_es;
  FONT_OFF=regs.r_bp;
}

/* �ȴ��������� */
void waitkey()
{
  _AX=0;
  geninterrupt(0x16);
}
/* ����VGA 13Hģʽ */
void setmode()
{
  _AX=0x13;
  geninterrupt(0x10);
  GetFontAdd();
}

/* �����ı�ģʽ */
void closemode()
{
  _AX=0x3;
  geninterrupt(0x10);
}

/* ���õ�ɫ�� */
void setpal(int Color,BYTE r,BYTE g,BYTE b)
{
  outportb(0x3c8,Color);
  outportb(0x3c9,r);
  outportb(0x3c9,g);
  outportb(0x3c9,b);
}

/* ��Ļ��λ����������ַ���*/
void locate(int Line,int Col)
{
  _DH=Line;
  _DL=Col;
  _AH=2;
  _BX=0;
  geninterrupt(0x10);
}

/* �Ӽ��̻�������ֱ�Ӷ���ɨ���� */
BYTE ScanKey(void)
{
  int  start,end;
  WORD key=0;
  start=peek(0,0x41a);
  end=peek(0,0x41c);
  if (start==end) return(0);
  else
  {
    key=peek(0x40,start);
    start+=2;
    if (start==0x3e) start=0x1e;
    poke(0x40,0x1a,start);
    return(key/256);
  }
}

/* �ڣ�X��Y������� */
void pset(int x,int y,BYTE color)
{
  pokeb(0xa000,y*320+x,color);
}

/* ȡ��X��Y������ɫ */
BYTE point(int x,int y)
{
  return peekb(0xa000,y*320+x);
}
