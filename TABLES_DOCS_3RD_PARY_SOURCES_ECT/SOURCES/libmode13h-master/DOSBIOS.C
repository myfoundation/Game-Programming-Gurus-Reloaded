/*
unsigned long GetFSize(FILE *fp);
void FNotFound(char *filename);
void PrintError(char *fmt,...);
void Wait(unsigned long delayticks);
unsigned long GetTimeTicks(void);
void CheckMem(unsigned long size);
void far *FarMemAlloc(unsigned long size);
void FarMemFree(void far *farheap);
*/

/**************************** BIOS & DOS routines **************************/
unsigned long GetFSize(FILE *fp)
{
    unsigned long currentpos,filesize;

    currentpos=ftell(fp);
    fseek(fp,0L,SEEK_END);            // Go to the end of the file
    filesize=ftell(fp);               // and find its position, which
    fseek(fp,currentpos,SEEK_SET);    // is the file size.
    return (filesize);
}

void FNotFound(char *filename)
{
    PrintError("Can't find %s\n",filename);
}

void PrintError(char *fmt,...)
{
    va_list ap;
    char string[80];
    int count;

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);

    SetVidMode(0x3);
    printf("%s",string);
    exit(1);
}

void Wait(unsigned long delayticks)
{
    long far *timer=MK_FP(0x40,0x6C);

    delayticks+=*timer;
    while ((*timer)<delayticks);
}

unsigned long GetTimeTicks(void)
{
    long far *timeticks=MK_FP(0x40,0x6C);
    return (*timeticks);
}

void CheckMem(unsigned long size)
{
    unsigned long cmem=farcoreleft();

    if (cmem<size)
        PrintError("AVA: %lu\nREQ: %lu\nNEEDED: %lu\n",cmem,size,size-cmem);
}
void far *FarMemAlloc(unsigned long size)
{
    void far *farheap;

    CheckMem(size);
    farheap=farmalloc(size);
    if (farheap==NULL)
        PrintError("Memory allocating error!\n");
    return (farheap);
}

void FarMemFree(void far *farheap)
{
    if (farheap!=NULL)
        farfree(farheap);
}
/***************************************************************************/
