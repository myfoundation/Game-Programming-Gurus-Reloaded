
// =======================================================================
/*
########   #######   ######             ##       ##      ## #### ##    ## ########   #######  ##      ##  ######     ########   #######  ########  ########    ########  #######   #######  ##       ##    ## #### ########
##     ## ##     ## ##    ##             ##      ##  ##  ##  ##  ###   ## ##     ## ##     ## ##  ##  ## ##    ##    ##     ## ##     ## ##     ##    ##          ##    ##     ## ##     ## ##       ##   ##   ##  ##     ##
##     ## ##     ## ##                    ##     ##  ##  ##  ##  ####  ## ##     ## ##     ## ##  ##  ## ##          ##     ## ##     ## ##     ##    ##          ##    ##     ## ##     ## ##       ##  ##    ##  ##     ##
##     ## ##     ##  ######     #######    ##    ##  ##  ##  ##  ## ## ## ##     ## ##     ## ##  ##  ##  ######     ########  ##     ## ########     ##          ##    ##     ## ##     ## ##       #####     ##  ########
##     ## ##     ##       ##              ##     ##  ##  ##  ##  ##  #### ##     ## ##     ## ##  ##  ##       ##    ##        ##     ## ##   ##      ##          ##    ##     ## ##     ## ##       ##  ##    ##  ##   ##
##     ## ##     ## ##    ##             ##      ##  ##  ##  ##  ##   ### ##     ## ##     ## ##  ##  ## ##    ##    ##        ##     ## ##    ##     ##          ##    ##     ## ##     ## ##       ##   ##   ##  ##    ##
########   #######   ######             ##        ###  ###  #### ##    ## ########   #######   ###  ###   ######     ##         #######  ##     ##    ##          ##     #######   #######  ######## ##    ## #### ##     ##
*/
// =======================================================================
// COPYRIGHT (c) 2019, 2022 MASTER MENTOR, MIT LICENSE

/*
You can use it for book
TRICK OF THE GAME-PROGRAMMING GURUS
Andre LaMothe and anothers
Sams Publishing 1995
*/

/*
VERSION 1.0.1    3:17 02.10.2021
VERSION 1.0.2    14:27 12.10.2022

*/

// =======================================================================
/*
#### ##    ##  ######  ######## ########  ##     ##  ######  ######## ####  #######  ##    ##
 ##  ###   ## ##    ##    ##    ##     ## ##     ## ##    ##    ##     ##  ##     ## ###   ##
 ##  ####  ## ##          ##    ##     ## ##     ## ##          ##     ##  ##     ## ####  ##
 ##  ## ## ##  ######     ##    ########  ##     ## ##          ##     ##  ##     ## ## ## ##
 ##  ##  ####       ##    ##    ##   ##   ##     ## ##          ##     ##  ##     ## ##  ####
 ##  ##   ### ##    ##    ##    ##    ##  ##     ## ##    ##    ##     ##  ##     ## ##   ###
#### ##    ##  ######     ##    ##     ##  #######   ######     ##    ####  #######  ##    ##
*/
// =======================================================================

/*
1. YOU NEED ONLY DOSEmu.h AND DOSEmu.cpp ADD TO PROJECT

2. RENAME FILE EXTENSION FROM .C -> .CPP

3. PUT NEXT LINES TO TOP OF THE MAIN DOS .CPP PROGRAM'S FILE

#include "stdafx.h"
#include "DOSEmu.h"

4. COMMENT
// #include <graph.h>
// #include <bios.h>

5. RENAME
main()	-> main2()

6. PUT IN main2() AFTER _setvideomode() OR Set_Mode()

video_buffer = (unsigned char far *)MEMORY_0xA0000000; // vram byte ptr

7. ENJOY!

8. YOU MAY COMPARE AND SEE DIFFERENCE BETWEEN ORIGINAL *.C FILES AT SOURCES_ORIGINAL FOLDER
AND THE SAME *.CPP FILES AT SOURCES_PORTED FOLDER

9. ADVANCED

YOU MAY REPLACE Delay() AND Timer() FUNCTION BY
void Delay(int clicks)
{
	// EACH CLICK IS APPROX. 55 MILLISECONDS.
	Sleep(clicks*55);
}

ALL ANOTHER USEFUL STUFF SEE AT SECTIONS BELOW

	// ADDONS FUNCIONS
	// graph.h CONSTANS
	// CUSTOM VGA CONSTANS
	// HARDWARE / BIOS / ECT.

*/

// =======================================================================
/*
#### ##    ##  ######  ##       ##     ## ########  ########  ######
 ##  ###   ## ##    ## ##       ##     ## ##     ## ##       ##    ##
 ##  ####  ## ##       ##       ##     ## ##     ## ##       ##
 ##  ## ## ## ##       ##       ##     ## ##     ## ######    ######
 ##  ##  #### ##       ##       ##     ## ##     ## ##             ##
 ##  ##   ### ##    ## ##       ##     ## ##     ## ##       ##    ##
#### ##    ##  ######  ########  #######  ########  ########  ######
*/
// =======================================================================

#if !defined(AFX_DOSEMU2_H__EE5134BE_526F_4556_A6BE_151BD8ED0F39__INCLUDED_)
#define AFX_DOSEMU2_H__EE5134BE_526F_4556_A6BE_151BD8ED0F39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include <mmsystem.h>
#pragma comment (lib, "Winmm.lib")

// =======================================================================
/*
######## ##     ## ##     ## ##          ###    ########  #######  ########     ########  ######## ##    ## ########  ######## ########  ######## ########     ########  ######## ##          ###    ######## ######## ########
##       ###   ### ##     ## ##         ## ##      ##    ##     ## ##     ##    ##     ## ##       ###   ## ##     ## ##       ##     ## ##       ##     ##    ##     ## ##       ##         ## ##      ##    ##       ##     ##
##       #### #### ##     ## ##        ##   ##     ##    ##     ## ##     ##    ##     ## ##       ####  ## ##     ## ##       ##     ## ##       ##     ##    ##     ## ##       ##        ##   ##     ##    ##       ##     ##
######   ## ### ## ##     ## ##       ##     ##    ##    ##     ## ########     ########  ######   ## ## ## ##     ## ######   ########  ######   ########     ########  ######   ##       ##     ##    ##    ######   ##     ##
##       ##     ## ##     ## ##       #########    ##    ##     ## ##   ##      ##   ##   ##       ##  #### ##     ## ##       ##   ##   ##       ##   ##      ##   ##   ##       ##       #########    ##    ##       ##     ##
##       ##     ## ##     ## ##       ##     ##    ##    ##     ## ##    ##     ##    ##  ##       ##   ### ##     ## ##       ##    ##  ##       ##    ##     ##    ##  ##       ##       ##     ##    ##    ##       ##     ##
######## ##     ##  #######  ######## ##     ##    ##     #######  ##     ##    ##     ## ######## ##    ## ########  ######## ##     ## ######## ##     ##    ##     ## ######## ######## ##     ##    ##    ######## ########
*/
// =======================================================================

#define VALUE_NOT_DEFINED   -1

//------------------------------------------
// START ENTRY
//------------------------------------------
// YOU MUST DEFINE IT!
void main2(void);
//------------------------------------------

//------------------------------------------
// GENARAL API
//------------------------------------------
// FIRST DRAW ALL GRAPHICS, THEN FOR SHOW IT BY CALL _redraw_screen() FUNCTION
#define REDRAW_IMMEDIATELY			1
#define REDRAW_DEFAULT				2
void _redraw_screen(int redraw_mode = REDRAW_IMMEDIATELY);
//------------------------------------------

//------------------------------------------
// ENGINE SETUP / RENDER FUNCIONS
//------------------------------------------
enum
{
	RENDER_EMPTY_STATE			= 0,

	// SPEED OPTIMIZING: CALL _set_render_options(TRUE, RENDER_MANUAL_REDRAW) FOR SWITCH OFF REDRAW SCREEN
	// AFTER THAT _setpixel() OR _lineto() CALLED NOT SHOW RESULT IMMEDIATELY
	RENDER_MANUAL_REDRAW		= 1 << 0,

	// SPEED OPTIMIZING: CALL IT FOR NOT REDRAW SCREEN AFTER EACH _outp(0x3c9, xx) CALL
	RENDER_NOT_REDRAW_IF_BY_PORT_PALETTE_CHANGED		= 1 << 1,

	// SPECIFY IS NATIVE DOS PALETTE CORRECTION NEED
	// YOU NEED CORRECT PALLETTE BECAUSE DOS COLORS TOO SHADY
	RENDER_CORRECT_PALETTE		= 1 << 2,	// DEFAULT

	// SET SCALE FOR LOW-RES WINDOW'S RESOLUTION
	RENDER_SCALE_VGA_SCREEN		= 1 << 3,
};

// USE FOR SET ON/OFF OPTIONS ABOVE,
// F.E. _set_render_options(TRUE, RENDER_CORRECT_PALETTE | RENDER_SCALE_VGA_SCREEN);
void _set_render_options(BOOL is_set, int options);

// SET SCALE FOR OUTPUT WINDOW'S RESOLUTION
// USE _set_render_options(TRUE, RENDER_SCALE_VGA_SCREEN) FOR SET SCALE MODE ON
#define SCALE_RES_CX_DEFAULT 800
#define SCALE_IF_RES_LESS_THAN_CX_DEFAULT 640
void _set_screen_scale_params(int apply_if_screen_cx_less_than = VALUE_NOT_DEFINED, int apply_screen_cx = VALUE_NOT_DEFINED);

//------------------------------------------

// =======================================================================
/*
   ##  ######   ########     ###    ########  ##     ##     ##     ## ##       ######## ##     ## ##    ##  ######  ####  #######  ##    ##  ######
  ##  ##    ##  ##     ##   ## ##   ##     ## ##     ##     ##     ##  ##      ##       ##     ## ###   ## ##    ##  ##  ##     ## ###   ## ##    ##
 ##   ##        ##     ##  ##   ##  ##     ## ##     ##     ##     ##   ##     ##       ##     ## ####  ## ##        ##  ##     ## ####  ## ##
##    ##   #### ########  ##     ## ########  #########     #########    ##    ######   ##     ## ## ## ## ##        ##  ##     ## ## ## ##  ######
 ##   ##    ##  ##   ##   ######### ##        ##     ##     ##     ##   ##     ##       ##     ## ##  #### ##        ##  ##     ## ##  ####       ##
  ##  ##    ##  ##    ##  ##     ## ##        ##     ## ### ##     ##  ##      ##       ##     ## ##   ### ##    ##  ##  ##     ## ##   ### ##    ##
   ##  ######   ##     ## ##     ## ##        ##     ## ### ##     ## ##       ##        #######  ##    ##  ######  ####  #######  ##    ##  ######
*/
// =======================================================================

//------------------------------------------
// <graph.h> CONSTANS
//------------------------------------------
#define _DEFAULTMODE    (-1)    /* restore screen to original mode */
#define _MRES16COLOR    13      /* 320 x 200, 16 color             */
#define _VRES2COLOR     17      /* 640 x 480, BW                   */
#define _VRES16COLOR    18      /* 640 x 480, 16 color             */
#define _MRES256COLOR   19      /* 320 x 200, 256 color            */
#define _VRES256COLOR   0x101   /* 640 x 480, 256 color            */
#define _SVRES16COLOR   0x102   /* 800 x 600, 16 color             */
#define _SVRES256COLOR  0x103   /* 800 x 600, 256 color            */
#define _XRES16COLOR    0x104   /* 1024 x 768, 16 color            */
#define _XRES256COLOR   0x105   /* 1024 x 768, 256 color           */
#define _YRES16COLOR    0x106   /* 1280 x 1024, 16 color           */
#define _YRES256COLOR   0x107   /* 1280 x 1024, 256 color          */
#define _MRESTRUECOLOR  0x10F   /* 320 x 200, TRUE color           */
#define _VRESTRUECOLOR  0x112   /* 640 x 480, 16.8M color          */
#define _SVRESTRUECOLOR 0x115   /* 800 x 600, 16.8M color          */
#define _XRESTRUECOLOR  0x118   /* 1024 x 768, 16.8M color         */
#define _YRESTRUECOLOR  0x11B   /* 1280 x 1024, 16.8M color        */
#define _ZRESTRUECOLOR  0x11F   /* 1600 x 1200, 16.8M color        */
//------------------------------------------

//------------------------------------------
// CUSTOM VGA CONSTANS
//------------------------------------------
#define _LVRESMODE_LAST_INDEX	(_ZRESTRUECOLOR+1)   // FOR INDEX

#define _256COLORS   256

#define VGA_COLOR_BLACK			0
#define VGA_COLOR_BLUE			1
#define VGA_COLOR_GREEN			2
#define VGA_COLOR_CYAN			3
#define VGA_COLOR_RED			4
#define VGA_COLOR_MAGENTA		5
#define VGA_COLOR_BROWN			6
#define VGA_COLOR_LIGHT_GRAY	7
#define VGA_COLOR_DARK_GRAY		8
#define VGA_COLOR_LIGHT_BLUE	9
#define VGA_COLOR_LIGHT_GREEN	10
#define VGA_COLOR_LIGHT_CYAN	11
#define VGA_COLOR_LIGHT_RED		12
#define VGA_COLOR_LIGHT_MAGENTA	13
#define VGA_COLOR_YELLOW		14
#define VGA_COLOR_WHITE			15
//------------------------------------------

//------------------------------------------
// <graph.h> FUNCTIONS
//------------------------------------------
#define _GCLEARSCREEN VGA_COLOR_BLACK
void _clearscreen(char color);

void _setvideomode(short mode);
void _setpalette(BYTE* palette, int size = VALUE_NOT_DEFINED, int is_correct_palette = VALUE_NOT_DEFINED);
void _setcolor(char color);
void _setpixel(int x, int y, int redraw_mode = REDRAW_DEFAULT);
char _getpixel(int x, int y);
void _moveto(int x, int y);
void _lineto(int x, int y, int redraw_mode = REDRAW_DEFAULT);

#define _GBORDER 2			// DRAW BORDER ONLY
#define _GFILLINTERIOR 3	// FILL SHAPE
void _rectangle(int draw_mode, int x1, int y1, int x2, int y2); // LEFT, TOP, INT RIGHT, BOTTOM
void _ellipse(int fill, int x1, int y1, int x2, int y2 );

void _settextposition(int x, int y);
//------------------------------------------

// =======================================================================
/*
##     ##    ###    ########  ########  ##      ##    ###    ########  ########          ##    ########  ####  #######   ######           ##    ########  ######  ########
##     ##   ## ##   ##     ## ##     ## ##  ##  ##   ## ##   ##     ## ##               ##     ##     ##  ##  ##     ## ##    ##         ##     ##       ##    ##    ##
##     ##  ##   ##  ##     ## ##     ## ##  ##  ##  ##   ##  ##     ## ##              ##      ##     ##  ##  ##     ## ##              ##      ##       ##          ##
######### ##     ## ########  ##     ## ##  ##  ## ##     ## ########  ######         ##       ########   ##  ##     ##  ######        ##       ######   ##          ##
##     ## ######### ##   ##   ##     ## ##  ##  ## ######### ##   ##   ##            ##        ##     ##  ##  ##     ##       ##      ##        ##       ##          ##
##     ## ##     ## ##    ##  ##     ## ##  ##  ## ##     ## ##    ##  ##           ##         ##     ##  ##  ##     ## ##    ##     ##         ##       ##    ##    ##    ###
##     ## ##     ## ##     ## ########   ###  ###  ##     ## ##     ## ########    ##          ########  ####  #######   ######     ##          ########  ######     ##    ###
*/
// =======================================================================

// DEFAULT 256 VGA PALETE
extern BYTE VGA_PALETTE[];
// FONT(ROM CHARACTERS 8X8)
extern BYTE VGA_FONT_8X8[];

// VIDEOMEMORY
extern BYTE* MEMORY_0xA0000000;
// ROM FONT MEMORY (THE SAME AS VGA_FONT_8X8)
extern BYTE* MEMORY_0xF000FA6EL;

// DOS CLOCK (TICK) COUNTER
// DON'T USE IT DIRECTLY, USE FOR READ PMEM_0x0000046C() FUNC CALL
extern BYTE* MEMORY_0x0000046C; // 18.2 clicks/sec TIMER
// RETUURN VALUE OF MEMORY_0x0000046C
ULONG PMEM_0x0000046C();

// PRESSED RAW DOS KEY VALUE
extern BYTE PRESSED_RAW_DOS_KEY;
// FUNCTION SETTED BY _dos_setvect(0x09, ...) FOR CATCH KEYBOARD INTERRUPT
extern void (*KEYBOARD_INTERRUPT_FUNC)(void);

// FOR NOT OVERLOAD MODERN FAST CPU BY ROTATING USELESS CYCLES
#define FAST_CPU_WAIT(ms)	Sleep(ms);

//------------------------------------------
/*
// Allocating a specific address in your process's address space is a bit tricky
// and platform-specific. On Unix systems, mmap() is probably the closest you're going to get.
// The Windows equivalent is VirtualAlloc().
// There are, of course, no guarantees since the address might already be in use.

long desiredAddress = 0xD0000000;
long size = 1024;
char* p = VirtualAlloc(desiredAddress, size,
      MEM_COMMIT | MEM_RESERVE,
      PAGE_READWRITE);
*/
//------------------------------------------

// =======================================================================
/*
##     ##  ######  ######## ######## ##     ## ##           ######  ######## ##     ## ######## ########
##     ## ##    ## ##       ##       ##     ## ##          ##    ##    ##    ##     ## ##       ##
##     ## ##       ##       ##       ##     ## ##          ##          ##    ##     ## ##       ##
##     ##  ######  ######   ######   ##     ## ##           ######     ##    ##     ## ######   ######
##     ##       ## ##       ##       ##     ## ##                ##    ##    ##     ## ##       ##
##     ## ##    ## ##       ##       ##     ## ##          ##    ##    ##    ##     ## ##       ##
 #######   ######  ######## ##        #######  ########     ######     ##     #######  ##       ##
*/
// =======================================================================

//------------------------------------------
// DEVICE INDEPENDED BITMAP (DIB) & RGB RELATED
//------------------------------------------
BITMAPINFO* NewBITMAPINFOHEADER(int biWidth, int biHeight, int biBitCount, int biPlanes);

/**
CONVERT RAW BYTES PALETTE TO [{(R)(G)(B)(0)}] ARRAY
*/
void PaletteRAW2RGB(RGBQUAD* rgb, BYTE* colors, int offset = VALUE_NOT_DEFINED, int count = VALUE_NOT_DEFINED);

/**
CORRECT PALETTE FROM/TO DOS COLOR <-> WINDOWS RGB MATCH
*/
void PaletteCorrect(BOOL is_from_dos, BYTE* palette, int size);

//------------------------------------------
// BOOK HELPERS
//------------------------------------------
/**
RETURN SCREEN cx OR cy (I.E. RESOLUTION) FOR GIVEN VGA MODE
*/
int videomodeCXCY(BOOL is_CX, short mode);

struct DIBSurface;
/**
LOAD PCX IMAGE
*/
DIBSurface* LoadPCX( LPCSTR lpcszFilename );

//------------------------------------------
// BOOK HELPERS
//------------------------------------------
void Set_Mode(int mode);

// =======================================================================
/*
########  ########  ######## ########  ######## ######## #### ##    ## ######## ########     ########    ###    ########  ##       ########  ######      ######  ######## ##     ## ######## ########
##     ## ##     ## ##       ##     ## ##       ##        ##  ###   ## ##       ##     ##       ##      ## ##   ##     ## ##       ##       ##    ##    ##    ##    ##    ##     ## ##       ##
##     ## ##     ## ##       ##     ## ##       ##        ##  ####  ## ##       ##     ##       ##     ##   ##  ##     ## ##       ##       ##          ##          ##    ##     ## ##       ##
########  ########  ######   ##     ## ######   ######    ##  ## ## ## ######   ##     ##       ##    ##     ## ########  ##       ######    ######      ######     ##    ##     ## ######   ######
##        ##   ##   ##       ##     ## ##       ##        ##  ##  #### ##       ##     ##       ##    ######### ##     ## ##       ##             ##          ##    ##    ##     ## ##       ##
##        ##    ##  ##       ##     ## ##       ##        ##  ##   ### ##       ##     ##       ##    ##     ## ##     ## ##       ##       ##    ##    ##    ##    ##    ##     ## ##       ##
##        ##     ## ######## ########  ######## ##       #### ##    ## ######## ########        ##    ##     ## ########  ######## ########  ######      ######     ##     #######  ##       ##
*/
// =======================================================================

// PREDEFINED TABLES
extern BYTE* WINKEY_DOSKEY; // MAP [WIN_KEY -> DOS_KEY]
// CALL FIRST 	WINKEY_DOSKEY = create_map_winkey_doskey();
BYTE* create_map_winkey_doskey();

// =======================================================================
/*
 ######  ######## ########      ######     ##       #### ########  ########     ###    ########  ##    ##  ######      ######  ######## ##     ## ########   ######
##    ##    ##    ##     ##    ##    ##    ##        ##  ##     ## ##     ##   ## ##   ##     ##  ##  ##  ##    ##    ##    ##    ##    ##     ## ##     ## ##    ##
##          ##    ##     ##    ##          ##        ##  ##     ## ##     ##  ##   ##  ##     ##   ####   ##          ##          ##    ##     ## ##     ## ##
 ######     ##    ##     ##    ##          ##        ##  ########  ########  ##     ## ########     ##     ######      ######     ##    ##     ## ########   ######
      ##    ##    ##     ##    ##          ##        ##  ##     ## ##   ##   ######### ##   ##      ##          ##          ##    ##    ##     ## ##     ##       ##
##    ##    ##    ##     ##    ##    ##    ##        ##  ##     ## ##    ##  ##     ## ##    ##     ##    ##    ##    ##    ##    ##    ##     ## ##     ## ##    ##
 ######     ##    ########      ######     ######## #### ########  ##     ## ##     ## ##     ##    ##     ######      ######     ##     #######  ########   ######
*/
// =======================================================================
//------------------------------------------
// STD C LIBRARYS STUBS
//------------------------------------------

/**
READ BYTE FROM SPECIFIED PORT
*/
unsigned char _inp(int port);

/**
WRITE BYTE TO SPECIFIED PORT
*/
void _outp(int port, int value);

/**
STUB. MUST BE RETURN CALLBACK FUNCTION WHICH SERVE SPECIFIED DOS INTERRUPT
*/
#define _dos_getvect(interupt_id) NULL

/**
SET CALLBACK FUNCTION FOR SERVE SPECIFIED DOS INTERRUPT
*/
void _dos_setvect(int interupt_id, void* fn);

//------------------------------------------
// <bios.h>
//------------------------------------------
#define _KEYBRD_READ        0   /* read next character from keyboard */
#define _KEYBRD_READY       1   /* check for keystroke */
#define _KEYBRD_SHIFTSTATUS 2   /* get current shift key status */
unsigned short _bios_keybrd(unsigned __cmd);

//------------------------------------------
// MEMORY FUNC ALIASES
//------------------------------------------
#define _fmalloc		malloc
#define _ffree			free
#define _fmemcpy		memcpy
#define _fmemset		memset
#define _fmemmove		memmove
//------------------------------------------

//------------------------------------------
// OBSOLETE COMPILER DIRECTIVES
//------------------------------------------
#define _far
#define __far
#define __huge
#define __near
#define _interrupt
//------------------------------------------

//------------------------------------------
// <dos.h> <i86.h> STRUCTS AND STUBES
//------------------------------------------
// INTERRUTPS HANDING

// I386 PROCESSOR FLAGS
// FLAGS register is the status register in Intel x86 microprocessors
// https://en.wikipedia.org/wiki/FLAGS_register
#define I386_FLAG_CF		0x0001
#define I386_FLAG_PF		0x0004
#define I386_FLAG_AF		0x0010
#define I386_FLAG_ZF		0x0040
#define I386_FLAG_SF		0x0080
#define I386_FLAG_TF		0x0100
#define I386_FLAG_IF		0x0200
#define I386_FLAG_DF		0x0400
#define I386_FLAG_OF		0x0800
#define I386_FLAG_IOPL		0x3000
#define I386_FLAG_NT		0x4000

struct SREGS
{
	unsigned short es, cs, ss, ds;
	unsigned short fs, gs;
};

/* word registers */

struct WORDREGS {
        unsigned short ax;
        unsigned short bx;
        unsigned short cx;
        unsigned short dx;
        unsigned short si;
        unsigned short di;
        unsigned short cflag;

		// FOR WORK WITH 64 BIT COMPILERS
		#ifdef _SIZE_T_DEFINED
				size_t _ax;
				size_t _bx;
				size_t _cx;
				size_t _dx;
				size_t _si;
				size_t _di;
				size_t _cflag;
		#endif
};
struct BYTEREGS {
        unsigned char al, ah;
        unsigned char bl, bh;
        unsigned char cl, ch;
        unsigned char dl, dh;
};

union REGS {
        struct WORDREGS  x;
        struct WORDREGS  w;
        struct BYTEREGS  h;
};

/**
CALL INTERRUPT
*/
int int86( int, union REGS *, union REGS *);
/**
CALL INTERRUPT
*/
int int86x( int, union REGS *, union REGS *, struct SREGS * );
void segread( struct SREGS * );

#define _int86 int86
#define _REGS REGS

// OBSOLETE MACROS
#ifdef _SIZE_T_DEFINED
	#define FP_SEG(__p) ((unsigned)((unsigned long)(size_t)(__p) >> 16))
	#define FP_OFF(__p) ((unsigned)(size_t)(__p))
	#define MK_FP(__s,__o) ( (char*)((__s << 16) | __o) )
#else
	#define FP_SEG(__p) ((unsigned)((unsigned long)(void __far*)(__p) >> 16))
	#define FP_OFF(__p) ((unsigned)(__p))
	#define MK_FP(__s,__o) ( (char*)((__s << 16) | __o) )
#endif

//------------------------------------------

/**
<time.h> clock() ALIAS
*/
long c_clock(void);

/**
FILE HELPER
*/
unsigned long file_length(BYTE* path);

//------------------------------------------
// TRACE MACRO SUPPORT
//------------------------------------------
#ifdef _DEBUG
bool _trace(TCHAR *format, ...);
#define TRACE _trace
#else
#define TRACE
#endif

// =======================================================================
/*
##     ## ##     ## ##       ######## #### ######## ##     ## ########  ########    ###    ########  #### ##    ##  ######      ########     ###     ######  ########     ######  ######## ##     ## ######## ########
###   ### ##     ## ##          ##     ##     ##    ##     ## ##     ## ##         ## ##   ##     ##  ##  ###   ## ##    ##     ##     ##   ## ##   ##    ## ##          ##    ##    ##    ##     ## ##       ##
#### #### ##     ## ##          ##     ##     ##    ##     ## ##     ## ##        ##   ##  ##     ##  ##  ####  ## ##           ##     ##  ##   ##  ##       ##          ##          ##    ##     ## ##       ##
## ### ## ##     ## ##          ##     ##     ##    ######### ########  ######   ##     ## ##     ##  ##  ## ## ## ##   ####    ########  ##     ##  ######  ######       ######     ##    ##     ## ######   ######
##     ## ##     ## ##          ##     ##     ##    ##     ## ##   ##   ##       ######### ##     ##  ##  ##  #### ##    ##     ##     ## #########       ## ##                ##    ##    ##     ## ##       ##
##     ## ##     ## ##          ##     ##     ##    ##     ## ##    ##  ##       ##     ## ##     ##  ##  ##   ### ##    ##     ##     ## ##     ## ##    ## ##          ##    ##    ##    ##     ## ##       ##
##     ##  #######  ########    ##    ####    ##    ##     ## ##     ## ######## ##     ## ########  #### ##    ##  ######      ########  ##     ##  ######  ########     ######     ##     #######  ##       ##
*/
// =======================================================================

struct CBoolEvent
{
	CBoolEvent(BOOL set = FALSE) { m_event = CreateEvent(NULL, TRUE, set, NULL); }
	virtual ~CBoolEvent() { CloseHandle(m_event); }

	BOOL IsTrue(DWORD timeout = 0) { return WaitForSingleObject( m_event, timeout ) != WAIT_TIMEOUT; }
	void SetBool(BOOL set) { if(set) SetEvent(m_event); else ResetEvent(m_event); }

private:
	HANDLE m_event;
};

struct CComAutoCriticalSection__
{
	void Lock() {EnterCriticalSection(&m_sec);}
	void Unlock() {LeaveCriticalSection(&m_sec);}
	CComAutoCriticalSection__() {InitializeCriticalSection(&m_sec);}
	~CComAutoCriticalSection__() {DeleteCriticalSection(&m_sec);}
	CRITICAL_SECTION m_sec;
};

struct CLockObject : public CComAutoCriticalSection__ {};

struct CAutoLock
{
    CAutoLock( CLockObject& cs )
    {
        m_cs = &cs;
        m_cs->Lock();
    }
    virtual ~CAutoLock() { m_cs->Unlock(); }

    CLockObject* m_cs;
};

template<class T>
T def_val(T def, T check, T ret_if_def) { return (check == def) ? (ret_if_def) : (check); }

#define AUTOLOCK(name) CAutoLock lock##__COUNTER__(name);
#define AUTOLOCKTHIS CAutoLock lock##__COUNTER__(*this);
#define SAFE_OBJECT(obj, expression)	{ CAutoLock lock##__COUNT__(obj); expression; }
template<class To, class Tl> To SAFE_ITEM(Tl& lock, To& obj) { CAutoLock lock_(lock); return obj; }

HANDLE Thread(void* start, void* lpParameter = NULL, BOOL close_handle = TRUE);
void Terminate(ULONG tcode = 0);
// =======================================================================
/*
######## ##     ## #### ########  ########     ########     ###    ########  ######## ##    ##     ######   ########     ###    ########  ##     ## ####  ######   ######      ######   #######  ########  ########
   ##    ##     ##  ##  ##     ## ##     ##    ##     ##   ## ##   ##     ##    ##     ##  ##     ##    ##  ##     ##   ## ##   ##     ## ##     ##  ##  ##    ## ##    ##    ##    ## ##     ## ##     ## ##
   ##    ##     ##  ##  ##     ## ##     ##    ##     ##  ##   ##  ##     ##    ##      ####      ##        ##     ##  ##   ##  ##     ## ##     ##  ##  ##       ##          ##       ##     ## ##     ## ##
   ##    #########  ##  ########  ##     ##    ########  ##     ## ########     ##       ##       ##   #### ########  ##     ## ########  #########  ##  ##        ######     ##       ##     ## ##     ## ######
   ##    ##     ##  ##  ##   ##   ##     ##    ##        ######### ##   ##      ##       ##       ##    ##  ##   ##   ######### ##        ##     ##  ##  ##             ##    ##       ##     ## ##     ## ##
   ##    ##     ##  ##  ##    ##  ##     ##    ##        ##     ## ##    ##     ##       ##       ##    ##  ##    ##  ##     ## ##        ##     ##  ##  ##    ## ##    ##    ##    ## ##     ## ##     ## ##
   ##    ##     ## #### ##     ## ########     ##        ##     ## ##     ##    ##       ##        ######   ##     ## ##     ## ##        ##     ## ####  ######   ######      ######   #######  ########  ########
*/
// =======================================================================

//------------------------------------------
// THIRD PARTY GRAPHICS LIBRARY
//------------------------------------------

/*
void DrawPixel(int x,int y,unsigned char colour);
unsigned char GrabPixel(int x,int y);
void Cls(unsigned char colour);
void DrawBar(int l,int t,int r,int b,unsigned char colour);
void DrawBox(int l,int t,int r,int b,unsigned char colour);
void DrawLine(int l,int t,int r,int b,unsigned char colour);
*/

#define ABS__(x)  ((x<0) ? -x:x)
#define SGN__(x)  ((x<0) ? -1:1)

struct BasicGraphRoutines
{
	BYTE* VgaMem;
	long VgaMem_size;
	long* Ycor;

	int m_cx;
	int m_cy;
	BYTE* m_mem_last;

	BasicGraphRoutines()
	{
		init(0, 0, NULL, NULL);
	}

	void init(int cx, int cy, BYTE* _VgaMem, long* _Ycor)
	{
		m_cx = cx;
		m_cy = cy;
		VgaMem_size = cx*cy;
		m_mem_last = _VgaMem + VgaMem_size;

		VgaMem = _VgaMem;
		Ycor = _Ycor;
	}

/*********************** Basic graph routines ******************************/
/*
(c) https://github.com/runwuf/libmode13h
*/


	BYTE* XY_to_offset(int x,int y)
	{
		if((y < 0) || (y >= m_cy) || (x < 0) || (x >= m_cx)) return (BYTE*)-1;
		BYTE* cell = VgaMem+Ycor[y]+x;

		// YET CHECK
		if(cell >= m_mem_last) return (BYTE*)-1;

		return cell;
	}

	/**
	THIS SAFE FUNCION FOR NEGATIV OR HIGH OFFSETS VALUES
	*/
	void DrawPixel(int x,int y,BYTE colour)
	{
		BYTE* cell = XY_to_offset(x, y);
		if(cell == (BYTE*)-1) return;
		*cell=colour;
	}

	/**
	THIS SAFE FUNCION FOR NEGATIV OR HIGH OFFSETS VALUES
	*/
	unsigned char GrabPixel(int x,int y)
	{
		BYTE* cell = XY_to_offset(x, y);
		if(cell == (BYTE*)-1) return 0;
		return(*cell);
	}

	void Cls(unsigned char colour)
	{
		memset(VgaMem,colour,VgaMem_size);
	}

	void DrawLine(int l,int t,int r,int b,unsigned char colour)
	{
		int d,x,y,ax,ay,sx,sy,dx,dy;

		dx=r-l;
		ax=ABS__(dx)<<1;
		sx=SGN__(dx);
		dy=b-t;
		ay=ABS__(dy)<<1;
		sy=SGN__(dy);

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

	/**
	FILLED RECTANGLE
	*/
	void DrawBar(int l,int t,int r,int b,unsigned char colour)
	{
		unsigned int width=r-l;

		BYTE* TVgaMem=VgaMem+l;

		for (;t<b;t++)
			memset(TVgaMem+Ycor[t],colour,width);
	}

	/**
	TRANSPARENT RECTANGLE
	*/
	void DrawBox(int l,int t,int r,int b,unsigned char colour)
	{
		unsigned int width=r-l-1;

		BYTE* TVgaMem=VgaMem+l;
		memset(TVgaMem+Ycor[t],colour,width+1),t+=1;
		for (;t<b;t++)
			*(TVgaMem+Ycor[t])=colour,*(TVgaMem+Ycor[t]+width)=colour;
		memset(TVgaMem+Ycor[b-1],colour,width+1);
	}

/***************************************************************************/

	/**
	 DrawEllipse(x1, y1, x2, y2, colour);
	  short x1, y1;     Upper-left corner
	  short x2, y2;     Lower-right corner
	  short colour;  Fill selection
	*/
	void DrawEllipse(int x1, int y1, int x2, int y2, int colour )
	{
		int height = y2-y1,
			width = x2-x1;

		for(int y=-height; y<=height; y++)
		{
			for(int x=-width; x<=width; x++)
			{
				if(x*x*height*height+y*y*width*width <= height*height*width*width)
				{
					DrawPixel(x1+x,y1+y,colour);
				}
			}
		}
	}


	void DrawCircle(int x0, int y0, int radius, int colour)
	{
		// Midpoint Circle Algorithm

		int x = radius;
		int y = 0;
		int err = 0;

		while (x >= y)
		{
			// putpixel()
			DrawPixel(x0 + x, y0 + y, colour);
			DrawPixel(x0 + y, y0 + x, colour);
			DrawPixel(x0 - y, y0 + x, colour);
			DrawPixel(x0 - x, y0 + y, colour);
			DrawPixel(x0 - x, y0 - y, colour);
			DrawPixel(x0 - y, y0 - x, colour);
			DrawPixel(x0 + y, y0 - x, colour);
			DrawPixel(x0 + x, y0 - y, colour);

			if (err <= 0)
			{
				y += 1;
				err += 2*y + 1;
			}

			if (err > 0)
			{
				x -= 1;
				err -= 2*x + 1;
			}
		}
	}
};


// =======================================================================
/*
##      ## #### ##    ##       ## ########   #######   ######     ##    ## ######## ##    ##     ######   #######  ########  ########  ######
##  ##  ##  ##  ###   ##      ##  ##     ## ##     ## ##    ##    ##   ##  ##        ##  ##     ##    ## ##     ## ##     ## ##       ##    ##
##  ##  ##  ##  ####  ##     ##   ##     ## ##     ## ##          ##  ##   ##         ####      ##       ##     ## ##     ## ##       ##
##  ##  ##  ##  ## ## ##    ##    ##     ## ##     ##  ######     #####    ######      ##       ##       ##     ## ##     ## ######    ######
##  ##  ##  ##  ##  ####   ##     ##     ## ##     ##       ##    ##  ##   ##          ##       ##       ##     ## ##     ## ##             ##
##  ##  ##  ##  ##   ###  ##      ##     ## ##     ## ##    ##    ##   ##  ##          ##       ##    ## ##     ## ##     ## ##       ##    ##
 ###  ###  #### ##    ## ##       ########   #######   ######     ##    ## ########    ##        ######   #######  ########  ########  ######
*/
// =======================================================================

#define WIN_VK_UNDEFINED			0			//	UNDEFINED KEY
#define WIN_VK_KEYS_TOTAL			256			//	KEYS TOTAL

// Windows Virtual-Key Codes

#define WIN_VK_LBUTTON        0x01
#define WIN_VK_RBUTTON        0x02
#define WIN_VK_CANCEL         0x03
#define WIN_VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#define WIN_VK_BACK           0x08
#define WIN_VK_TAB            0x09

#define WIN_VK_CLEAR          0x0C
#define WIN_VK_RETURN         0x0D

#define WIN_VK_SHIFT          0x10
#define WIN_VK_CONTROL        0x11
#define WIN_VK_MENU           0x12
#define WIN_VK_PAUSE          0x13
#define WIN_VK_CAPITAL        0x14

#define WIN_VK_KANA           0x15
#define WIN_VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define WIN_VK_HANGUL         0x15
#define WIN_VK_JUNJA          0x17
#define WIN_VK_FINAL          0x18
#define WIN_VK_HANJA          0x19
#define WIN_VK_KANJI          0x19

#define WIN_VK_ESCAPE         0x1B

#define WIN_VK_CONVERT        0x1C
#define WIN_VK_NONCONVERT     0x1D
#define WIN_VK_ACCEPT         0x1E
#define WIN_VK_MODECHANGE     0x1F

#define WIN_VK_SPACE          0x20
#define WIN_VK_PRIOR          0x21
#define WIN_VK_NEXT           0x22
#define WIN_VK_END            0x23
#define WIN_VK_HOME           0x24
#define WIN_VK_LEFT           0x25
#define WIN_VK_UP             0x26
#define WIN_VK_RIGHT          0x27
#define WIN_VK_DOWN           0x28
#define WIN_VK_SELECT         0x29
#define WIN_VK_PRINT          0x2A
#define WIN_VK_EXECUTE        0x2B
#define WIN_VK_SNAPSHOT       0x2C
#define WIN_VK_INSERT         0x2D
#define WIN_VK_DELETE         0x2E
#define WIN_VK_HELP           0x2F

/* WIN_VK_0 thru WIN_VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
/* WIN_VK_A thru WIN_VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */

#define WIN_VK_0              0x30
#define WIN_VK_1              0x31
#define WIN_VK_2              0x32
#define WIN_VK_3              0x33
#define WIN_VK_4              0x34
#define WIN_VK_5              0x35
#define WIN_VK_6              0x36
#define WIN_VK_7              0x37
#define WIN_VK_8              0x38
#define WIN_VK_9              0x39

#define WIN_VK_A              0x41
#define WIN_VK_B              0x42
#define WIN_VK_C              0x43
#define WIN_VK_D              0x44
#define WIN_VK_E              0x45
#define WIN_VK_F              0x46
#define WIN_VK_G              0x47
#define WIN_VK_H              0x48
#define WIN_VK_I              0x49
#define WIN_VK_J              0x4A
#define WIN_VK_K              0x4B
#define WIN_VK_L              0x4C
#define WIN_VK_M              0x4D
#define WIN_VK_N              0x4E
#define WIN_VK_O              0x4F
#define WIN_VK_P              0x50
#define WIN_VK_Q              0x51
#define WIN_VK_R              0x52
#define WIN_VK_S              0x53
#define WIN_VK_T              0x54
#define WIN_VK_U              0x55
#define WIN_VK_V              0x56
#define WIN_VK_W              0x57
#define WIN_VK_X              0x58
#define WIN_VK_Y              0x59
#define WIN_VK_Z              0x5A

#define WIN_VK_LWIN           0x5B
#define WIN_VK_RWIN           0x5C
#define WIN_VK_APPS           0x5D

#define WIN_VK_NUMPAD0        0x60
#define WIN_VK_NUMPAD1        0x61
#define WIN_VK_NUMPAD2        0x62
#define WIN_VK_NUMPAD3        0x63
#define WIN_VK_NUMPAD4        0x64
#define WIN_VK_NUMPAD5        0x65
#define WIN_VK_NUMPAD6        0x66
#define WIN_VK_NUMPAD7        0x67
#define WIN_VK_NUMPAD8        0x68
#define WIN_VK_NUMPAD9        0x69
#define WIN_VK_MULTIPLY       0x6A
#define WIN_VK_ADD            0x6B
#define WIN_VK_SEPARATOR      0x6C
#define WIN_VK_SUBTRACT       0x6D
#define WIN_VK_DECIMAL        0x6E
#define WIN_VK_DIVIDE         0x6F
#define WIN_VK_F1             0x70
#define WIN_VK_F2             0x71
#define WIN_VK_F3             0x72
#define WIN_VK_F4             0x73
#define WIN_VK_F5             0x74
#define WIN_VK_F6             0x75
#define WIN_VK_F7             0x76
#define WIN_VK_F8             0x77
#define WIN_VK_F9             0x78
#define WIN_VK_F10            0x79
#define WIN_VK_F11            0x7A
#define WIN_VK_F12            0x7B
#define WIN_VK_F13            0x7C
#define WIN_VK_F14            0x7D
#define WIN_VK_F15            0x7E
#define WIN_VK_F16            0x7F
#define WIN_VK_F17            0x80
#define WIN_VK_F18            0x81
#define WIN_VK_F19            0x82
#define WIN_VK_F20            0x83
#define WIN_VK_F21            0x84
#define WIN_VK_F22            0x85
#define WIN_VK_F23            0x86
#define WIN_VK_F24            0x87

#define WIN_VK_NUMLOCK        0x90
#define WIN_VK_SCROLL         0x91

/*
 * WIN_VK_L* & WIN_VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define WIN_VK_LSHIFT         0xA0
#define WIN_VK_RSHIFT         0xA1
#define WIN_VK_LCONTROL       0xA2
#define WIN_VK_RCONTROL       0xA3
#define WIN_VK_LMENU          0xA4
#define WIN_VK_RMENU          0xA5

#if(WINVER >= 0x0400)
#define WIN_VK_PROCESSKEY     0xE5
#endif /* WINVER >= 0x0400 */

#define WIN_VK_ATTN           0xF6
#define WIN_VK_CRSEL          0xF7
#define WIN_VK_EXSEL          0xF8
#define WIN_VK_EREOF          0xF9
#define WIN_VK_PLAY           0xFA
#define WIN_VK_ZOOM           0xFB
#define WIN_VK_NONAME         0xFC
#define WIN_VK_PA1            0xFD
#define WIN_VK_OEM_CLEAR      0xFE

// =======================================================================
// IBM AT-Style Keyboard

#define DOS_VK_UNDEFINED			0			//	UNDEFINED KEY
#define DOS_VK_KEYS_TOTAL			256			//	KEYS TOTAL


#define DOS_VK_ESCAPE				1			//	Esc
#define DOS_VK_1					2			//	1
#define DOS_VK_2					3			//	2
#define DOS_VK_3					4			//	3
#define DOS_VK_4					5			//	4
#define DOS_VK_5					6			//	5
#define DOS_VK_6					7			//	6
#define DOS_VK_7					8			//	7
#define DOS_VK_8					9			//	8
#define DOS_VK_9					10			//	9
#define DOS_VK_0					11			//	0
#define DOS_VK_SUBTRACT_1			12			//	-
#define DOS_VK_EQUAL				13			//	=
#define DOS_VK_BACK					14			//	BkSpc
#define DOS_VK_TAB					15			//	Tab
#define DOS_VK_Q					16			//	Q
#define DOS_VK_W					17			//	W
#define DOS_VK_E					18			//	E
#define DOS_VK_R					19			//	R
#define DOS_VK_T					20			//	T
#define DOS_VK_Y					21			//	Y
#define DOS_VK_U					22			//	U
#define DOS_VK_I					23			//	I
#define DOS_VK_O					24			//	O
#define DOS_VK_P					25			//	P
#define DOS_VK_LBRACKET				26			//	[
#define DOS_VK_RBRACKET				27			//	]
#define DOS_VK_ENTER				28			//	Enter
#define DOS_VK_LCONTROL				29			//	Left Ctrl
#define DOS_VK_A					30			//	A
#define DOS_VK_S					31			//	S
#define DOS_VK_D					32			//	D
#define DOS_VK_F					33			//	F
#define DOS_VK_G					34			//	G
#define DOS_VK_H					35			//	H
#define DOS_VK_J					36			//	J
#define DOS_VK_K					37			//	K
#define DOS_VK_L					38			//	L
#define DOS_VK_DOTCOMMA				39			//	;
#define DOS_VK_APOSTROPHE			40			//	'
#define DOS_VK_QUOTE3				41			//	`
#define DOS_VK_LSHIFT				42			//	Left Shift
#define DOS_VK_STICK1				43			//  \ n
#define DOS_VK_Z					44			//	Z
#define DOS_VK_X					45			//	X
#define DOS_VK_C					46			//	C
#define DOS_VK_V					47			//	V
#define DOS_VK_B					48			//	B
#define DOS_VK_N					49			//	N
#define DOS_VK_M					50			//	M
#define DOS_VK_COMMA				51			//	,
#define DOS_VK_DOT					52			//	.
#define DOS_VK_STICK2				53			//	/
#define DOS_VK_RSHIFT				54			//	Right Shift
#define DOS_VK_MULTIPLY				55			//	Gray *
#define DOS_VK_LALT					56			//	Left Alt
#define DOS_VK_SPACE				57			//	Space
#define DOS_VK_CAPS					58			//	Caps
#define DOS_VK_F1					59			//	F1
#define DOS_VK_F2					60			//	F2
#define DOS_VK_F3					61			//	F3
#define DOS_VK_F4					62			//	F4
#define DOS_VK_F5					63			//	F5
#define DOS_VK_F6					64			//	F6
#define DOS_VK_F7					65			//	F7
#define DOS_VK_F8					66			//	F8
#define DOS_VK_F9					67			//	F9
#define DOS_VK_F10					68			//	F10
#define DOS_VK_NUMLOCK				69			//	NumLock
#define DOS_VK_SCROLL				70			//	ScrollLock

#define DOS_VK_NUMPAD7					71			//	Pad 7
#define DOS_VK_NUMPAD8					72			//	Pad 8
#define DOS_VK_NUMPAD9					73			//	Pad 9
#define DOS_VK_SUBTRACTG				74			//	Gray Minus
#define DOS_VK_NUMPAD4					75			//	Pad 4
#define DOS_VK_NUMPAD5					76			//	Pad 5
#define DOS_VK_NUMPAD6					77			//	Pad 6
#define DOS_VK_PLUSG					78			//	Gray Plus
#define DOS_VK_NUMPAD1					79			//	Pad 1
#define DOS_VK_NUMPAD2					80			//	Pad 2
#define DOS_VK_NUMPAD3					81			//	Pad 3
#define DOS_VK_NUMPAD0					82			//	Pad 0/Ins
#define DOS_VK_NUMPADDOT				83			//	Pad ./Del
#define DOS_VK_PRINT				84			//	PrtScr/SysRq
#define DOS_VK_0x55					85			//
#define DOS_VK_0x56					86			//
#define DOS_VK_F11					87			//	F11
#define DOS_VK_F12					88			//	F12
#define DOS_VK_0x59					89			//
#define DOS_VK_PAUSE				90			//	Pause/Break
#define DOS_VK_INSERT				91			//	Insert
#define DOS_VK_HOME					92			//	Home
#define DOS_VK_PRIOR				93			//	PgUp
#define DOS_VK_STICKG				94			//	Gray /
#define DOS_VK_DELETE				95			//	Delete
#define DOS_VK_END					96			//	End
#define DOS_VK_NEXT					97			//	PgDn
#define DOS_VK_RALT					98			//	Right Alt
#define DOS_VK_RCONTROL				99			//	Right Ctrl
#define DOS_VK_UP					100			//	Up Arrow
#define DOS_VK_LEFT					101			//	Left Arrow
#define DOS_VK_DOWN					102			//	Down Arrow
#define DOS_VK_RIGHT				103			//	Right Arrow
#define DOS_VK_ENTERG				104			//	Gray Enter
#define DOS_VK_MOUSE				105			//	Mouse

// =======================================================================


#endif // !defined(AFX_DOSEMU2_H__EE5134BE_526F_4556_A6BE_151BD8ED0F39__INCLUDED_)
