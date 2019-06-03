    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <dos.h>
    #include "paral.h"

    char *MemBuf,            // pointer to memory buffer
         *BackGroundBmp,     // pointer to background bitmap data
         *ForeGroundBmp,     // pointer to foreground bitmap data
         *VideoRam;          // pointer to VGA memory

    PcxFile pcx;             // data structure for reading PCX files

    int volatile KeyScan;    // modified by keyboard interrupt handler

    int frames=0,            // number of frames drawn
        PrevMode;            // holds original video mode

    int background,  // tracks scroll position in background bitmap
        foreground,  // tracks scroll position in foreground bitmap
        position;    // tracks total scroll distance

    void _interrupt (*OldInt9)(void); // ptr to BIOS keyboard handler

//
//  This routine loads a 256 color PCX file.
//
    int ReadPcxFile(char *filename,PcxFile *pcx)
    {
      long i;
      int mode=NORMAL,nbytes;
      char abyte,*p;
      FILE *f;

      f=fopen(filename,"rb");
      if(f==NULL)
        return PCX_NOFILE;
      fread(&pcx->hdr,sizeof(PcxHeader),1,f);
      pcx->width=1+pcx->hdr.xmax-pcx->hdr.xmin;
      pcx->height=1+pcx->hdr.ymax-pcx->hdr.ymin;
      pcx->imagebytes=(unsigned int)(pcx->width*pcx->height);
      if(pcx->imagebytes > PCX_MAX_SIZE)
        return PCX_TOOBIG;
      pcx->bitmap=(char*)malloc(pcx->imagebytes);
      if(pcx->bitmap == NULL)
        return PCX_NOMEM;

      p=pcx->bitmap;
      for(i=0;i<pcx->imagebytes;i++)
      {
        if(mode == NORMAL)
        {
          abyte=fgetc(f);
          if((unsigned char)abyte > 0xbf)
          {
            nbytes=abyte & 0x3f;
            abyte=fgetc(f);
            if(--nbytes > 0)
              mode=RLE;
          }
        }
        else if(--nbytes == 0)
          mode=NORMAL;
        *p++=abyte;
      }

      fseek(f,-768L,SEEK_END);      // get palette from pcx file
      fread(pcx->pal,768,1,f);
      p=pcx->pal;
      for(i=0;i<768;i++)            // bit shift palette
        *p++=*p >>2;
      fclose(f);
      return PCX_OK;                // return success
    }

//
//  This is the new int 9h handler.  This allows for smooth interactive
//  scrolling.  If the BIOS keyboard handler was not disabled holding
//  down one of the arrow keys would overflow the keyboard buffer and
//  cause a very annoying beep.
//
    void _interrupt NewInt9(void)
    {
      register char x;

      KeyScan=inp(0x60);       // read key code from keyboard
      x=inp(0x61);             // tell keyboard that key was processed
      outp(0x61,(x|0x80));
      outp(0x61,x);
      outp(0x20,0x20);                  // send End-Of-Interrupt
      if(KeyScan == RIGHT_ARROW_REL ||  // check for keys
         KeyScan == LEFT_ARROW_REL)
         KeyScan=0;
    }

//
//  This routine restores the original BIOS keyboard interrupt handler
//
    void RestoreKeyboard(void)
    {
      _dos_setvect(KEYBOARD,OldInt9);   // restore BIOS keyboard interrupt
    }

//
//  This routine saves the original BIOS keyboard interrupt handler and
//  then installs a customer handler for this program.
//
    void InitKeyboard(void)
    {
      OldInt9=_dos_getvect(KEYBOARD);   // save BIOS keyboard interrupt
      _dos_setvect(KEYBOARD,NewInt9);   // install new int 9h handler
    }

//
//  This routine calls the video BIOS to set all the DAC registers
//  of the VGA based on the contents of pal[].
//
    void SetAllRgbPalette(char *pal)
    {
      struct SREGS s;
      union REGS r;

      segread(&s);                    // get current segment values
      s.es=FP_SEG((void far*)pal);    // point ES to pal array
      r.x.dx=FP_OFF((void far*)pal);  // get offset to pal array
      r.x.ax=0x1012;                  // BIOS func 10h sub 12h
      r.x.bx=0;                       // starting DAC register
      r.x.cx=256;                     // ending DAC register
      int86x(0x10,&r,&r,&s);          // call video BIOS
    }

//
//  This routine sets up the video mode to BIOS mode 13h.  This mode
//  is the MCGA compatible 320x200x256 mode.
//
    void InitVideo()
    {
      union REGS r;

      r.h.ah=0x0f;               // BIOS func 0fh
      int86(0x10,&r,&r);         // call video BIOS
      PrevMode=r.h.al;           // save current video mode
      r.x.ax=0x13;               // set video mode 13h: 320x200x256
      int86(0x10,&r,&r);         // call video BIOS
      VideoRam=MK_FP(0xa000,0);  // create a pointer to video memory
    }

//
//  This routine restores the video mode to its original state.
//
    void RestoreVideo()
    {
      union REGS r;

      r.x.ax=PrevMode;          // restore previous video mode
      int86(0x10,&r,&r);        // call video BIOS
    }

//
//  This routine loads the bitmap layers.
//
    int InitBitmaps()
    {
      int r;

      background=foreground=1;                // initial split location

      r=ReadPcxFile("backgrnd.pcx",&pcx);     // read in background bitmap
      if(r != PCX_OK)                         // check for errors
        return FALSE;
      BackGroundBmp=pcx.bitmap;               // save bitmap pointer
      SetAllRgbPalette(pcx.pal);              // setup VGA palette

      r=ReadPcxFile("foregrnd.pcx",&pcx);     // read in foreground bitmap
      if(r != PCX_OK)                         // check for errors
        return FALSE;
      ForeGroundBmp=pcx.bitmap;               // save bitmap pointer

      MemBuf=malloc(MEMBLK);                  // create system memory buffer
      if(MemBuf == NULL)                      // check for errors
        return FALSE;

      memset(MemBuf,0,MEMBLK);                // clear buffer
      return TRUE;                            // success!
    }

//
//  This routine frees all memory allocated by the program.
//
    void FreeMem()
    {
      free(MemBuf);
      free(BackGroundBmp);
      free(ForeGroundBmp);
    }

//
//  This routine draws the parallax layers.  The order of the functions
//  determines the Z-ordering of the layers.
//
    void DrawLayers()
    {
      OpaqueBlt(BackGroundBmp,0,100,background);
      TransparentBlt(ForeGroundBmp,50,100,foreground);
    }

//
//  This routine handles the animation.  Note that this is the most
//  time critical section of code.  To optimize the parallax drawing
//  this routine and its children (functions called by this routine)
//  could be re-written in assembly language.  A 100% increase in
//  drawing speed would be typical.
//
    void AnimLoop()
    {
      while(KeyScan != ESC_PRESSED)       // loop until ESC key hit
      {
        switch(KeyScan)                   // process key that was hit
        {
        case RIGHT_ARROW_PRESSED:         // right arrow is down
          position--;                     // update scroll total
          if(position < 0)                // stop scrolling if end is reached
          {
            position=0;
            break;
          }
          background-=1;                  // scroll background left 2 pixels
          if(background < 1)              // did we reach the end?
            background+=VIEW_WIDTH;       // ...then make it wrap around

          foreground-=2;                  // scroll foreground left 4 pixels
          if(foreground < 1)              // did we reach the end?
            foreground+=VIEW_WIDTH;       // ...then make it wrap around

          break;
        case LEFT_ARROW_PRESSED:          // left arrow is down
          position++;                     // updated scroll total
          if(position > TOTAL_SCROLL)     // stop scrolling if end is reached
          {
            position=TOTAL_SCROLL;
            break;
          }
          background+=1;                  // scroll background right 2 pixels
          if(background > VIEW_WIDTH-1)   // did we reach the end?
            background-=VIEW_WIDTH;       // ...then make it wrap around

          foreground+=2;                  // scroll foreground right 4 pixels
          if(foreground > VIEW_WIDTH-1)   // did we reach the end?
            foreground-=VIEW_WIDTH;       // ...then make it wrap around

          break;
        default:                          // handle any other keys
          break;
        }
        DrawLayers();                    // draw parallax layer(s) in MemBuf
        memcpy(VideoRam,MemBuf,MEMBLK);  // copy MemBuf to VGA memory
        frames++;                        // track of total frames drawn
      }
    }

//
//  This routine performs all the initialization.
//
    void Initialize()
    {
      position=0;
      InitVideo();            // set up mode 13h
      InitKeyboard();         // install our keyboard handler
      if(!InitBitmaps())      // read in the bitmaps
      {
        CleanUp();            // free up memory
        printf("\nError loading bitmaps\n");
        exit(1);
      }
    }

//
//  This routine performs all the necessary cleanup
//
    void CleanUp()
    {
      RestoreVideo();       // put VGA back in original state
      RestoreKeyboard();    // restore BIOS keyboard handling
      FreeMem();            // release all memory
    }

//
//  This is the main program start.  This function calls the initialization
//  routines.  Then it gets the current clock ticks, calls the animation
//  loop, and finally gets the ending clock ticks.  The clock ticks are
//  used to calculate the animation frame rate.
//
    int main()
    {
      clock_t begin,fini;

      Initialize();          // set video mode, load bitmaps, etc

      begin=clock();         // get clock ticks at animation start
      AnimLoop();            // do the animation
      fini=clock();          // get clock ticks at animation end

      CleanUp();             // free mem, etc
      printf("Frames: %d\nfps: %f\n",frames,(float)CLK_TCK*frames/(fini-begin));
      return 0;
    }
