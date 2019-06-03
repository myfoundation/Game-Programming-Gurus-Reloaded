/*
void InitCOL(char *filename);
void LoadCOL(char *filename,unsigned char palette[256][3]);
void SetCOL(int start,int number,unsigned char palette[256][3]);
void GetCOL(int start, int number,unsigned char palette[256][3]);
void SetSameCOL(int begin,int end,unsigned char colour);
void FadeIn(int begin,int end,int speed,unsigned char palette[256][3]);
void FadeOut(int begin,int end,int speed,unsigned char colour[3]);
void CycUpCOL(int begin,int end,unsigned char palette[256][3]);
*/
/***************************** Palette routines ****************************/
void InitCOL(char *filename)
{
    unsigned char palette[256][3];                     // Load a col file

    LoadCOL(filename,palette);         // and set it.
    SetCOL(0,256,palette);
}

void LoadCOL(char *filename,unsigned char palette[256][3])
{
	FILE *fp;
    int count;

    if (!(fp=fopen(filename,"rb")))
        FNotFound(filename);

    for (count=0;count<256;count++)
        fread(&palette[count],1,3,fp);

    fclose(fp);
}

void SetCOL(int begin,int end,unsigned char palette[256][3])
{
    char temp=begin;

    WaitVerRetrace();

    asm mov al,temp;
    asm mov dx,3c8h;
    asm out dx,al;

    asm inc dx;
    asm mov ax,end;
    asm sub ax,begin;
    asm dec ax;
    asm mov cx,ax;      // cx=ax*3
    asm add cx,ax;
    asm add cx,ax;

    asm les di,palette;
    asm mov bx,di;

    Set_Palette:
        asm mov al,es:[bx];
        asm out dx,al;
        asm inc bx;
    asm loop Set_Palette;
}

void GetCOL(int begin,int end,unsigned char palette[256][3])
{
    unsigned int i;

    for (i=begin;i<end;i++)
    {
        outportb(0x3C7,i);
        palette[i][0]=inportb(0x3C9);
        palette[i][1]=inportb(0x3C9);
        palette[i][2]=inportb(0x3C9);
    }
}

void SetSameCOL(int begin,int end,unsigned char colour)
{
    unsigned int i;
    unsigned char palette[256][3];

    for (i=begin;i<end;i++)
        palette[i][0]=colour,palette[i][1]=colour,palette[i][2]=colour;

    SetCOL(begin,end,palette);
}

void FadeIn(int begin,int end,int speed,unsigned char palette[256][3])
{
    int i,j,cycle;
    unsigned char newpal[256][3];

    GetCOL(begin,end,newpal);

    for (cycle=0;cycle<64;cycle++)
    {

        for (i=begin;i<end;i++)
            for (j=0;j<3;j++)
                if (newpal[i][j]<palette[i][j])
                    newpal[i][j]++;
                else if (newpal[i][j]>palette[i][j])
                    newpal[i][j]--;

        SetCOL(begin,end,newpal);
        delay(speed);
    }
}

void FadeOut(int begin,int end,int speed,unsigned char colour[3])
{
    int i,j,cycle;
    unsigned char newpal[256][3];

    GetCOL(begin,end,newpal);

    for (cycle=0;cycle<64;cycle++)
    {
        for (i=begin;i<end;i++)
            for (j=0;j<3;j++)
                if (newpal[i][j]<colour[j])
                    newpal[i][j]++;
                else if (newpal[i][j]>colour[j])
                    newpal[i][j]--;

        SetCOL(begin,end,newpal);
        delay(speed);
    }
}

void CycUpCOL(int begin,int end,unsigned char palette[256][3])
{
    int i,j;
    unsigned char temp[3];

    for (j=0;j<3;j++)
        temp[j]=palette[begin][j];

    for (i=begin;i<end;i++)
        for (j=0;j<3;j++)
            palette[i][j]=palette[i+1][j];

    for (j=0;j<3;j++)
        palette[end][j]=temp[j];

    SetCOL(0,end,palette);
}


/*
void cyclecol(int start,int number,unsigned char palette[256][3],int way)
{
    int i,j,swap;
    unsigned char temp[3];

    if (way==-1)
        swap=start+number;
    if (way==1)
        swap=start;

    for (j=0;j<3;j++)
        temp[j]=palette[swap][j];

    switch (way)
    {
        case 1: for (i=start;i<=start+number-1;i++)
                    for (j=0;j<3;j++)
                        palette[i][j]=palette[i+1][j];
                swap=start+number;
                break;

        case -1:for (i=start+number;i>=start+1;i--)
                    for (j=0;j<3;j++)
                        palette[i][j]=palette[i-1][j];
                swap=start;
                break;
    }

    for (j=0;j<3;j++)
        palette[swap][j]=temp[j];

    for (i=start;i<=(start+number);i++)
    {
        asm cli;
        outportb(0x3C8,i);
        for (j=0;j<3;j++)
            outportb(0x3C9,palette[i][j]);
        asm sti;
    }
} */
/***************************************************************************/
