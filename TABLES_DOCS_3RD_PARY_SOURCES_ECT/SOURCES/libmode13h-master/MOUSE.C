/*
int MInit(void);
void MOn(void);
void MOff(void);
void GetMPos(int *x,int *y);
void SetMPos(int x,int y);
void MRead(int *x,int *y,int *button);
void SetMBound(int l,int t,int r,int b);
void WaitMBRelease();
int CheckMArea(int l,int t,int r,int b);
*/

/***************************** Mouse routines ******************************/
int MInit(void)
{
    int status;

    asm mov ax,0000;
    asm int 33h;
    status=_AX;

    if (status!=0)
        status=1;
    return(status);
}

void MOn(void)
{
    asm mov ax,0001;
    asm int 33h;
}

void MOff(void)
{
    asm mov ax,0002;
    asm int 33h;
}

void GetMPos(int *x,int *y)
{
    asm mov ax,0003;
    asm int 33h;
    *x=_CX/2;
    *y=_DX;
}

void SetMPos(int x,int y)
{
    x*=2;
    asm mov ax,0004;
    _CX=x;
    _DX=y;
    asm int 33h;
}

void MRead(int *x,int *y,int *button)
{
    int temp;

    asm mov ax,0003;
    asm int 33h;
    asm mov temp,bx;
    *button=temp;
    *x=_CX/2;
    *y=_DX;
}

void SetMBound(int l,int t,int r,int b)
{
    l*=2,r*=2;

    asm mov ax,0x07;
    _CX=l;
    _DX=r;
    asm int 33h;
    asm mov ax,0x08;
    _CX=t;
    _DX=b;
    asm int 33h;
}

void WaitMBRelease()
{
    int button=1,status=1;

    while (status!=0)
    {
        if (button>3)
            button=1;
        asm mov ax,0006;
        asm mov bx,button;
        asm int 33h;
        asm mov status,ax;
        button++;
    }
}

int CheckMArea(int l,int t,int r,int b)
{
    int x=0,y=0,button=0,status=False;

    MRead(&x,&y,&button);

    if (l<x&&x<r&&t<y&&y<b)
    {
        if (button>0)
            status=True+button;
        else
            status=True;
    }
    return (status);
}
/***************************************************************************/
