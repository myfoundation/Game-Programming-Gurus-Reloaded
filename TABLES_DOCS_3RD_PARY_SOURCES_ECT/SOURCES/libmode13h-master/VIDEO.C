/*
void SetVidMode(char mode);
void SetActPage(Block hpage);
void SetVgaPage(void);
void CopyBlock(int l,int t,int r,int b,Block source,int x,int y,Block dest);
void WaitVerRetrace(void);
*/

void SetVidMode(char mode)
{
	asm mov ah,00;
    _AL=mode;
	asm int 10h;
}

void SetActPage(Block hpage)
{
    VgaMem=hpage;
}

void SetVgaPage(void)
{
    VgaMem=MK_FP(0xA000,0x00);
}

void CopyBlock(int l,int t,int r,int b,int width,Block source,int x,int y,Block dest)
{
    int rwidth=(r-l)/2;

    source+=Ycor[t]+l;
    dest+=Ycor[y]+x;
/*
    for (;t<b;y++,t++,source+=width)
        _fmemcpy(dest+Ycor[y],source,rwidth);
*/
    asm push ds;
    asm cld;

    for (;t<b;y++,t++,source+=width,dest+=320)
    {
        asm les di,dest;
        asm lds si,source;

        asm mov cx,rwidth;
        asm rep movsw;
    }
    asm pop ds;
}

void WaitVerRetrace(void)
{
//  while ((inportb(0x3da)&0x08)==0);  // wait for vertical rescan (start)
//  while ((inportb(0x3da)&0x08)!=0);  // wait for vertical rescan (end)

    asm mov dx,3DAh
l1:
    asm in al,dx
    asm and al,08h
    asm jnz l1
l2:
    asm in al,dx
    asm and al,08h
    asm jz  l2
}
