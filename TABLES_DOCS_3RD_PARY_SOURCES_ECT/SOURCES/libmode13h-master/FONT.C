/*
void LoadFont(char *filename);
void SetHorspace(int hs);
void SetVerspace(int vs);
void VgaPutch(int x,int y,unsigned char colour,int ch);
void VgaOPutch(int x,int y,unsigned char ocolor,unsigned char color,int ch);
void VgaPrintf(int x,int y,unsigned char color,char *fmt,...);
*/

static unsigned char far FontData[4096];
static int Bit[]={ 128,64,32,16,8,4,2,1 };
static int HorSpace=6;
static int VerSpace=7;

/****************************** Font routines ******************************/
void LoadFont(char *filename)
{
    FILE *fp;

    if (!(fp=fopen(filename,"rb")))
        FNotFound(filename);
    fread(FontData,4096,1,fp);
    fclose(fp);
}

void SetHorSpace(int hs)
{
    HorSpace=hs;
}

void SetVerSpace(int vs)
{
    VerSpace=vs;
}

void VgaPutch(int x,int y,unsigned char colour,int ch)
{
    int count,offset;

    TVgaMem=VgaMem+Ycor[y]+x;
    offset=(16*ch)+16;
    for (count=offset-16;count<offset;count++,TVgaMem+=320)
        for (x=0;x<8;x++)
            if ((FontData[count]&Bit[x])>0)
                *(TVgaMem+x)=colour;
}

void VgaOPutch(int x,int y,unsigned char ocolor,unsigned char color,int ch)
{
    VgaPutch(x+1,y,ocolor,ch);
    VgaPutch(x+1,y+2,ocolor,ch);

    VgaPutch(x,y+1,ocolor,ch);
    VgaPutch(x+2,y+1,ocolor,ch);

    VgaPutch(x+1,y+1,color,ch);
}

void VgaPrintf(int x,int y,unsigned char color,char *fmt,...)
{
    va_list ap;
    char string[80];
    int count;

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);

    for (count=0;string[count]!='\0';count++)
    {
        VgaPutch(x,y,color,string[count]);
        x+=HorSpace;
    }
}
/***************************************************************************/
