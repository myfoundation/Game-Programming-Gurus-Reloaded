#define False 0
#define True  1
#define ABS(x)  ((x<0) ? -x:x)
#define SGN(x)  ((x<0) ? -1:1)

#include <stdio.h>
#include <io.h>
#include <process.h>
#include <dir.h>
#include <conio.h>
#include <stdlib.h>
#include <bios.h>
#include <dos.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <alloc.h>
#include <math.h>
#include <ctype.h>
#include <mem.h>
#include <string.h>

unsigned int Ycor[]={
       0,  320,  640,  960, 1280, 1600, 1920, 2240,
    2560, 2880, 3200, 3520, 3840, 4160, 4480, 4800,
    5120, 5440, 5760, 6080, 6400, 6720, 7040, 7360,
    7680, 8000, 8320, 8640, 8960, 9280, 9600, 9920,
   10240,10560,10880,11200,11520,11840,12160,12480,
   12800,13120,13440,13760,14080,14400,14720,15040,
   15360,15680,16000,16320,16640,16960,17280,17600,
   17920,18240,18560,18880,19200,19520,19840,20160,
   20480,20800,21120,21440,21760,22080,22400,22720,
   23040,23360,23680,24000,24320,24640,24960,25280,
   25600,25920,26240,26560,26880,27200,27520,27840,
   28160,28480,28800,29120,29440,29760,30080,30400,
   30720,31040,31360,31680,32000,32320,32640,32960,
   33280,33600,33920,34240,34560,34880,35200,35520,
   35840,36160,36480,36800,37120,37440,37760,38080,
   38400,38720,39040,39360,39680,40000,40320,40640,
   40960,41280,41600,41920,42240,42560,42880,43200,
   43520,43840,44160,44480,44800,45120,45440,45760,
   46080,46400,46720,47040,47360,47680,48000,48320,
   48640,48960,49280,49600,49920,50240,50560,50880,
   51200,51520,51840,52160,52480,52800,53120,53440,
   53760,54080,54400,54720,55040,55360,55680,56000,
   56320,56640,56960,57280,57600,57920,58240,58560,
   58880,59200,59520,59840,60160,60480,60800,61120,
   61440,61760,62080,62400,62720,63040,63360,63680    };

typedef unsigned char far * Block;

Block VgaMem=MK_FP(0xA000,0x00),TVgaMem;

void SetVidMode(char mode);
void SetActPage(Block hpage);
void SetVgaPage(void);
void CopyBlock(int l,int t,int r,int b,int width,Block source,int x,int y,Block dest);
void WaitVerRetrace(void);

unsigned long GetFSize(FILE *fp);
void FNotFound(char *filename);
void PrintError(char *fmt,...);
void Wait(unsigned long delayticks);
unsigned long GetTimeTicks(void);
void CheckMem(unsigned long size);
void far *FarMemAlloc(unsigned long size);
void FarMemFree(void far *farheap);

void ShowImage(int x,int y,int width,int height,Block buffer);
void XShowImage(int x,int y,int width,int height,Block buffer,unsigned char colour);
Block GrabImage(int l,int t,int r,int b);
Block Load256PCX(char *filename);

void InitCOL(char *filename);
void LoadCOL(char *filename,unsigned char palette[256][3]);
void SetCOL(int begin,int end,unsigned char palette[256][3]);
void GetCOL(int start, int number,unsigned char palette[256][3]);
void SetSameCOL(int begin,int end,unsigned char colour);
void FadeIn(int begin,int end,int speed,unsigned char palette[256][3]);
void FadeOut(int begin,int end,int speed,unsigned char colour[3]);
void CycUpCOL(int begin,int end,unsigned char palette[256][3]);

int MInit(void);
void MOn(void);
void MOff(void);
void GetMPos(int *x,int *y);
void SetMPos(int x,int y);
void MRead(int *x,int *y,int *button);
void SetMBound(int l,int t,int r,int b);
void WaitMBRelease();
int CheckMArea(int l,int t,int r,int b);

void LoadFont(char *filename);
void SetHorspace(int hs);
void SetVerspace(int vs);
void VgaPutch(int x,int y,unsigned char colour,int ch);
void VgaOPutch(int x,int y,unsigned char ocolor,unsigned char color,int ch);
void VgaPrintf(int x,int y,unsigned char color,char *fmt,...);

void DrawPixel(int x,int y,unsigned char colour);
unsigned char GrabPixel(int x,int y);
void Cls(unsigned char colour);
void DrawBar(int l,int t,int r,int b,unsigned char colour);
void DrawBox(int l,int t,int r,int b,unsigned char colour);
void DrawLine(int l,int t,int r,int b,unsigned char colour);

void interrupt (*oldint9)();
void interrupt int9(void);
void InitKeys(void);
void UnInitKeys(void);

#include "video.c"
#include "dosbios.c"
#include "palette.c"
#include "image.c"
#include "mouse.c"
#include "font.c"
#include "graph.c"
#include "keyboard.c"
