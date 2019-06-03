/*
void DrawPixel(int x,int y,unsigned char colour);
unsigned char GrabPixel(int x,int y);
void Cls(unsigned char colour);
void DrawBar(int l,int t,int r,int b,unsigned char colour);
void DrawBox(int l,int t,int r,int b,unsigned char colour);
void DrawLine(int l,int t,int r,int b,unsigned char colour);
*/

/*********************** Basic graph routines ******************************/
void DrawPixel(int x,int y,unsigned char colour)
{
    *(VgaMem+Ycor[y]+x)=colour;
}

unsigned char GrabPixel(int x,int y)
{
    return(*(VgaMem+Ycor[y]+x));
}

void Cls(unsigned char colour)
{
    _fmemset(VgaMem,colour,64000U);
}

void DrawBar(int l,int t,int r,int b,unsigned char colour)
{
    unsigned int width=r-l;

    TVgaMem=VgaMem+l;

    for (;t<b;t++)
        _fmemset(TVgaMem+Ycor[t],colour,width);
}

void DrawBox(int l,int t,int r,int b,unsigned char colour)
{
    unsigned int width=r-l-1;

    TVgaMem=VgaMem+l;
    _fmemset(TVgaMem+Ycor[t],colour,width+1),t+=1;
    for (;t<b;t++)
        *(TVgaMem+Ycor[t])=colour,*(TVgaMem+Ycor[t]+width)=colour;
    _fmemset(TVgaMem+Ycor[b-1],colour,width+1);
}

void DrawLine(int l,int t,int r,int b,unsigned char colour)
{
    int d,x,y,ax,ay,sx,sy,dx,dy;

    dx=r-l;
    ax=ABS(dx)<<1;
    sx=SGN(dx);
    dy=b-t;
    ay=ABS(dy)<<1;
    sy=SGN(dy);

    x=l;
    y=t;
    if(ax>ay)
    {
        d=ay-(ax>>1);
        while(x!=r)
        {
            DrawPixel(x,y,colour);
            if(d>=0)
            {
                y += sy;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    }
    else
    {
        d=ax-(ay>>1);
        while(y!=b)
        {
            DrawPixel(x,y,colour);
            if(d>=0)
            {
                x+=sx;
                d-=ay;
            }
            y+=sy;
            d+=ax;
        }
    }
}
/***************************************************************************/
