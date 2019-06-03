#include "include.h"

typedef struct
{
    unsigned int width;
    unsigned int height;
    Block data;
}   RAW;

typedef struct
{
    RAW m;
    unsigned int limX;
    unsigned int limY;
    Block *tile;
}   MAP;

void Update(int deltax,int deltay,Block source);
void ShowTile(int mapx,int mapy,Block tile);
void XShowTile(int mapx,int mapy,Block tile);
MAP LoadMAP(int MapNum);
RAW LoadPCXData(char *filename);

void main(int argc,char *argv[])
{
    long t1,t2,frame=0;
    Block tbuffer=FarMemAlloc(64000U);
    unsigned char palette[256][3];
    int skip=atoi(argv[1]);
    Block sprite;
    MAP map;
    int Mx[10],My[9];
    int cx,cy,dx=0,dy=0,wx=0,wy=0,mx=4,my=4,mdx=0,mdy=0;
    unsigned int WorldPosY[200];

    if (skip<1||skip>32)
        skip=2;

    map=LoadMAP(1);

    SetVidMode(0x13);
    sprite=Load256PCX("tank.pcx");
    GetCOL(0,256,palette);

    SetSameCOL(0,256,0);

    DrawBox(0,0,320,200,1);
    SetActPage(tbuffer);

    for (cy=0;cy<9;cy++)
        My[cy]=cy*20;

    for (cx=0;cx<10;cx++)
        Mx[cx]=cx*24;

    for (cy=0;cy<map.m.height;cy++)
        WorldPosY[cy]=cy*map.m.width;

    for (cy=0;cy<9;cy++)
        for (cx=0;cx<10;cx++)
            ShowTile(Mx[cx],My[cy],map.tile[*(map.m.data+WorldPosY[wy+cy]+wx+cx)]);

    Update(dx,dy,tbuffer);
    FadeIn(0,256,20,palette);
    InitKeys();

    t1=time(0);
    while (!kbhit())
    {
        if (KeyStatus[0])   //UP
        {
            dy-=skip,mdy-=skip;

            if (wy==map.limY&&my>4)
            {
                dy=20;
                if (mdy<=0)
                    mdy=20,my--;
            }

            else if (dy<=0)
            {
                if (wy>0)
                {
                    dy=20,wy--;
                }
                else
                    dy=0;
            }

            if (wy==0&&my>0)
            {
                if (dy==0&&mdy<=0)
                   mdy=20,my--;
            }

            if (mdy<=0)
                mdy=dy;
        }


        if (KeyStatus[1])   //DOWN
        {
            dy+=skip;
            if (dy>=20)
            {
                if (wy<map.limY)
                {
                    dy=0,wy++;
                }
                else
                    dy=20;
            }

            mdy+=skip;

            if (wy==0&&my<4)
            {
                dy=0;
                if (mdy>=20)
                   mdy=0,my++;
            }

            if (wy==map.limY&&my<7)
            {
                dy=0;
                if (mdy>=20)
                    mdy=0,my++;
            }

            if (mdy>=20)
                mdy=dy;
        }


        if (KeyStatus[2])   //LEFT
        {
            dx-=skip,mdx-=skip;
            if (dx<=0||mdx<=0)
            {
//              if (*(map.m.data+WorldPosY[wy+my]+wx+mx-1)!=2)
//              {
                    if (wx==0)
                        if (mx>0)
                            mdx=24,mx--;

                    if (wx>0)
                    {
                        if (wx==(map.limX))
                        {
                            if (mx>4)
                                mdx=24,mx--;
                            else
                                mdx=dx=24,wx--;
                        }
                        else
                            mdx=dx=24,wx--;
                    }
                    else
                        mdx=dx=0;
//              }
            }
        }

        if (KeyStatus[3])   //RIGHT
        {
//          if (*(map.m.data+WorldPosY[wy+my]+wx+mx+1)!=2)
//          {
                dx+=skip,mdx+=skip;
                if (dx>=24||mdx>=24)
                {
                    if (wx==(map.limX))
                        if (mx<8)
                            mdx=0,mx++;

                    if (wx<map.limX)
                    {
                        if (wx==0)
                        {
                            if (mx<4)
                                mdx=0,mx++;
                            else
                                mdx=dx=0,wx++;
                        }
                        else
                            mdx=dx=0,wx++;
                    }
                    else
                        mdx=dx=24;
                }
//          }
        }

        for (cy=0;cy<9;cy++)
            for (cx=0;cx<10;cx++)
                ShowTile(Mx[cx],My[cy],map.tile[*(map.m.data+WorldPosY[wy+cy]+wx+cx)]);

//              ShowTile(Mx[cx],My[cy],map.tile[*(map.m.data+tpos+cx)]);
//              ShowTile(Mx[cx],My[cy],map.tile[*(map.m.data+Ycor[wy+cy]+wx+cx)]);


        XShowTile(Mx[mx]+mdx,My[my]+mdy,sprite);
        Update(dx,dy,tbuffer);

//      if (frame%33==0)
//          CycUpCOL(130,137,palette);

        frame++;
        gotoxy(1,22);printf("dx:%d dy:%d wx:%d wy:%d mdx:%d mdy:%d mx:%d my:%d  ",dx,dy,wx,wy,mdx,mdy,mx,my);//farcoreleft());
    }
    t2=time(0);

    UnInitKeys();
    FarMemFree(tbuffer);

    FarMemFree(map.m.data);
    FarMemFree(map.tile);
    FarMemFree(sprite);
    FadeOut(0,256,20,palette[55]);
    SetVidMode(0x3);
    printf( "%ld seconds, %ld frames (%ld FPS)\n",t2-t1,frame,frame/(t2-t1));
}

//  32x26

void Update(int deltax,int deltay,Block source)
{
    asm push ds;

    asm mov ax,0a000h;      // set dest segment to 0xA000, offset to 1928
    asm mov es,ax;
    asm mov di,1928d;

    asm mov bx,deltay;      // ax=Ycor[deltay]+deltax;
    asm shl bx,1;
    asm mov ax,Ycor[bx];
    asm add ax,deltax;
    asm lds si,source;      // load source segment to ds, offet to si
    asm add si,ax;          // add ax to source offset

    WaitVerRetrace();

    asm mov dx,160d;        // 20 * 8 = 160, 24 * 9 = 216

    Update_Line:
        asm mov cx,54d;     // 216 / 4 = 54
        asm rep movsd;

        asm add di,104d;     // 320 - 216 = 104
        asm add si,104d;     // 320 - 216 = 104

        asm dec dx;
    asm jnz Update_Line;

    asm pop ds;
}

void ShowTile(int mapx,int mapy,Block tile)
{
    asm push ds;

    asm mov bx,mapy;        // ax=Ycor[mapy]+mapx;
    asm shl bx,1;
    asm mov ax,Ycor[bx];
    asm add ax,mapx;
    asm les di,VgaMem;      // load dest segment and offset to VgaMem
    asm add di,ax;          // add ax to dest offset
    asm lds si,tile;        // load souce segment and offset to tile

    asm mov dx,20d;
    Tile_Line:
        asm mov cx,6d;      // 24 / 4 = 6
        asm rep movsd;

        asm add di,296d;    // 320 - 24 = 296
        asm dec dx;
    asm jnz Tile_Line;

    asm pop ds;
}

void XShowTile(int mapx,int mapy,Block tile)
{
    asm push ds;

    asm mov bx,mapy;        // ax=Ycor[mapy]+mapx;
    asm shl bx,1;
    asm mov ax,Ycor[bx];
    asm add ax,mapx;
    asm les di,VgaMem;      // load dest segment and offset to VgaMem
    asm add di,ax;          // add ax to dest offset
    asm lds si,tile;        // load souce segment and offset to tile

    asm mov dx,20d;
    Tile_Line:
        asm mov cx,24d;
        Check_Pixel:
            asm lodsb;
            asm cmp al,0d;
            asm je Skip:
            asm mov [es:di],al;
            Skip:
            asm inc di;
        asm loop Check_Pixel;

        asm add di,296d;
        asm dec dx;
    asm jnz Tile_Line;

    asm pop ds;
}

MAP LoadMAP(int MapNum)
{
    char filename[13];
    MAP map;
    RAW temp;
    unsigned long size;
    int count,total;

    sprintf(filename,"MAP%d.MAP",MapNum);
    map.m=LoadPCXData(filename);

    map.limX=map.m.width-10;
    map.limY=map.m.height-9;

    sprintf(filename,"MAP%d.TIL",MapNum);
    temp=LoadPCXData(filename);

    size=temp.width*temp.height;
    total=size/480;

    for (count=0,size=0;count<total;count++,size+=480)
    {
        map.tile[count]=FarMemAlloc(480);
        _fmemcpy(map.tile[count],temp.data+size,480);
    }

    FarMemFree(temp.data);
    return (map);
}

RAW LoadPCXData(char *filename)
{
    RAW buffer;

    FILE *fp;
    int colour,colour1;
    unsigned int count;
    unsigned int total,lcount=0;

    if (!(fp=fopen(filename,"rb")))
        FNotFound(filename);

    fseek(fp,8L,SEEK_SET);
    buffer.width=getw(fp);
    buffer.height=getw(fp);
    buffer.width++;
    buffer.height++;
    
    total=buffer.width*buffer.height;
    buffer.data=FarMemAlloc(total);

    fseek(fp,128L,SEEK_SET);
    while (lcount<total)
    {
        colour=fgetc(fp);
        if ((colour&0xC0)==0xC0)
        {
            count=(colour&0x3F);
            colour1=fgetc(fp);
			while (count--)
                buffer.data[lcount++]=colour1;
        }
        else
           buffer.data[lcount++]=colour;
    }
    fclose(fp);

    return (buffer);
}
//  printf("%lu\n",farcoreleft());getch();
