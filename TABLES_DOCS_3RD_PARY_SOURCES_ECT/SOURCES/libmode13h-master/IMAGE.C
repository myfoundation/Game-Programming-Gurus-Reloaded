/*
void ShowImage(int x,int y,int width,int height,Block buffer);
void XShowImage(int x,int y,int width,int height,Block buffer,unsigned char colour);
Block GrabImage(int l,int t,int r,int b);
Block Load256PCX(char *filename);
*/
/************************** Block image routines ***************************/
void ShowImage(int x,int y,int width,int height,Block buffer)
{
    int theight=y+height;

    TVgaMem=VgaMem+x;

    for (;y<theight;y++,buffer+=width)
        _fmemcpy(TVgaMem+Ycor[y],buffer,width);
}

void XShowImage(int x,int y,int width,int height,Block buffer,unsigned char colour)
{
    int theight=y+height;

    TVgaMem=VgaMem+x;

    for (;y<theight;y++,buffer+=width)
        for (x=0;x<width;x++)
            if (*(buffer+x)!=colour)
                *(TVgaMem+Ycor[y]+x)=*(buffer+x);
}

Block GrabImage(int l,int t,int r,int b)
{
    unsigned int width=r-l;
    Block buffer;
    unsigned int total=(unsigned)width*(unsigned)(b-t);

    TVgaMem=VgaMem+l;

    buffer=FarMemAlloc(total);
    for (;t<b;t++,buffer+=width)
        _fmemcpy(buffer,TVgaMem+Ycor[t],width);

    buffer-=total;
    return (buffer);
}
/***************************************************************************/

/******************************** PCX image routine ************************/
Block Load256PCX(char *filename)
{
    FILE *fp;
    int colour,colour1;
    unsigned int count,maxx,maxy;
    unsigned int total,lcount=0;
    Block buffer;
	long position;
	unsigned char palette[256][3];

    if (!(fp=fopen(filename,"rb")))
        FNotFound(filename);

    fseek(fp,8L,SEEK_SET);
    maxx=getw(fp);
    maxy=getw(fp);
    maxx++;  // Real width and height is the value in the file add by one.
	maxy++;

    total=maxx*maxy;
    // Locating memory to store decompressed Block file.
    buffer=FarMemAlloc(total);

    position=GetFSize(fp);        // From file size - 768 is the palette
    position-=768;                // of the PCX.
    fseek(fp,position,SEEK_SET);
    fread(palette,sizeof(palette),1,fp);
    for (colour=0;colour<256;colour++)
        for (colour1=0;colour1<3;colour1++)
            palette[colour][colour1]/=4.047;  // R,G,B colours range from 0 to 63.
    SetCOL(0,256,palette);

    fseek(fp,128L,SEEK_SET);
    while (lcount<total)
    {
        colour=fgetc(fp);
        if ((colour&0xC0)==0xC0)
        {
            count=(colour&0x3F);
            colour1=fgetc(fp);
			while (count--)
                buffer[lcount++]=colour1;
        }
        else
           buffer[lcount++]=colour;
    }

    fclose(fp);
    return (buffer);   // return the Block buffer.
}
/***************************************************************************/
