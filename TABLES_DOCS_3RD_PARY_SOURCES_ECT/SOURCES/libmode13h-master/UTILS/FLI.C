/*************************** Animation Playing routine *********************/
void loadfli(int l,int t,int r,int b,char *filename,block *Frame,int tframes)
{
    struct fliheader
    {
        unsigned long size;
        unsigned int magic;
        unsigned int frames;
        unsigned int width;
        unsigned int height;
        unsigned int depth;
        unsigned int flags;
        unsigned int speed;
    } flih;

    struct frameheader
    {
        unsigned long size;
        unsigned int magic;
        unsigned int chunks;
        double expand;
    } frameh;

    struct chunkheader
    {
        unsigned long size;
        unsigned int type;
    } chunkh;

    #define FLI_COLOR   11
    #define FLI_LC      12
    #define FLI_BLACK   13
    #define FLI_BRUN    15
    #define FLI_COPY    16

    FILE *fp;

    unsigned int count,temp,chunkm,bytes,skip,change,ny;
    unsigned long end;
    unsigned char packets,data,skip_count;
    char dups;
    Block OLDMEM,TempArea;
    int number;

    if (!(fp=fopen(filename,"rb")))
        FNotFound(filename);

    TempArea=FarMemAlloc(64000U);
    SetActPage(TempArea);

	fread(&flih,sizeof(flih),1,fp);
    if (tframes>flih.frames)
    {
        setvidmode(0x3);
        printf("number of frames exceeds.\n");
        exit(1);
    }

    fseek(fp,128L,SEEK_SET);
    for (number=0;number<tframes;)
    {
        fread(&frameh,sizeof(frameh),1,fp);
        for (chunk=0;chunk<frameh.chunks;chunk++)
        {
            fread(&chunkh,sizeof(chunkh),1,fp);
            switch (chunkh.type)
            {
                case FLI_COLOR:
                fseek(fp,772L,SEEK_CUR);
                break;

                case FLI_LC:

                end=(ftell(fp))+(chunkh.size-sizeof(chunkh));
                skip=getw(fp);
                change=(getw(fp))+skip;
                TVgaMem=VgaMem+Ycor[skip];

                for (;skip<change;skip++)
                {
                    OLDMEM=TEMPVGAMEM;
                    packets=fgetc(fp);
                    for (temp=0;temp<packets;temp++)
                    {
                        skip_count=fgetc(fp);
                        TEMPVGAMEM+=skip_count;
                        dups=fgetc(fp);
                        if (dups>0)
                        {
                            for (count=0;count<dups;count++,TEMPVGAMEM++)
                                *TEMPVGAMEM=fgetc(fp);
                        }
                        if (dups<0)
                        {
                            dups=0-dups;
                            data=fgetc(fp);
                            _fmemset(TEMPVGAMEM,data,dups);
                            TEMPVGAMEM+=dups;
                        }
                    }
                    TEMPVGAMEM=OLDMEM+320;
                }
                Frame[number++]=grabimage(l,t,r,b);
                fseek(fp,end,SEEK_SET);
                break;

                case FLI_BLACK:
                break;

                case FLI_BRUN:

                bytes=0;
                end=(ftell(fp))+(chunkh.size-sizeof(chunkh));
                while (bytes<64000U)
                {
                    packets=fgetc(fp);
                    for (temp=0;temp<packets&&bytes<64000U;temp++)
                    {
                        dups=fgetc(fp);
                        if (dups>0)
                        {
                            data=fgetc(fp);
                            _fmemset(VGAMEM+bytes,data,dups);
                            bytes+=dups;
                        }
                        if (dups<0)
                        {
                            dups=0-dups;
                            for (count=0;count<dups;count++,bytes++)
                                *(VGAMEM+bytes)=fgetc(fp);
                        }
                    }
                }
                Frame[number++]=grabimage(l,t,r,b);
                fseek(fp,end,SEEK_SET);
                break;

                case FLI_COPY:

                fread(TempArea,1,64000U,fp);
                Frame[number++]=grabimage(l,t,r,b);
                break;

                default:

                setvidmode(0x3);
                printf("Error in %s\n",filename);
                exit(1);
            }
        }
    }
    freefarmem(TempArea);
    fclose(fp);
    setvgapage();
}
/***************************************************************************/
