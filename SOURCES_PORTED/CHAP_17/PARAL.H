//
//  Paral.h - This header defines the constants and data structures
//            used in the parallax demos.
//
    #define KEYBOARD 0x09
//
// Keyboard press/release codes for the INT 9h handler
//
    #define RIGHT_ARROW_PRESSED   77
    #define RIGHT_ARROW_REL      205
    #define LEFT_ARROW_PRESSED    75
    #define LEFT_ARROW_REL       203
    #define ESC_PRESSED          129
    #define UP_ARROW_PRESSED      72
    #define UP_ARROW_REL         200
    #define DOWN_ARROW_PRESSED    80
    #define DOWN_ARROW_REL       208

    #define VIEW_WIDTH    320
    #define VIEW_HEIGHT   150
    #define MEMBLK        VIEW_WIDTH*VIEW_HEIGHT
    #define TRANSPARENT   0       // color index of see-thru pixels
    #define TOTAL_SCROLL  320

    enum {NORMAL,RLE};
    //enum {FALSE,TRUE};

    typedef struct
    {
      char manufacturer;    /* Always set to 0 */
      char version;         /* Always 5 for 256-color files */
      char encoding;        /* Always set to 1 */
      char bits_per_pixel;  /* Should be 8 for 256-color files */
      short int  xmin,ymin;       /* Coordinates for top left corner */
      short int  xmax,ymax;       /* Width and height of image */
      short int  hres;            /* Horizontal resolution of image */
      short int  vres;            /* Vertical resolution of image */
      char palette16[48];   /* EGA palette; not used for 256-color files */
      char reserved;        /* Reserved for future use */
      char color_planes;    /* Color planes */
      short int  bytes_per_line;  /* Number of bytes in 1 line of pixels */
      short int  palette_type;    /* Should be 2 for color palette */
      char filler[58];      /* Nothing but junk */
    } PcxHeader;

    typedef struct
    {
      PcxHeader hdr;
      char *bitmap;
      char pal[768];
      unsigned imagebytes,width,height;
    } PcxFile;

    #define PCX_MAX_SIZE 1640000L
    enum {PCX_OK,PCX_NOMEM,PCX_TOOBIG,PCX_NOFILE};

#ifdef __cplusplus
extern "C" {
#endif

    int ReadPcxFile(char *filename,PcxFile *pcx);
    void _interrupt NewInt9(void);
    void RestoreKeyboard(void);
    void InitKeyboard(void);
    void SetAllRgbPalette(char *pal);
    void InitVideo(void);
    void RestoreVideo(void);
    int InitBitmaps(void);
    void FreeMem(void);
    void DrawLayers(void);
    void AnimLoop(void);
    void Initialize(void);
    void CleanUp(void);
    void OpaqueBlt(char *,int,int,int);
    void TransparentBlt_2(char *,int,int,int);

#ifdef __cplusplus
  }
#endif
