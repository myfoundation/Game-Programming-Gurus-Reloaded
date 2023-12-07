// COPYRIGHT (c) 2019, 2022 MASTER MENTOR, MIT LICENSE

// DOSEmu.cpp : Defines the entry point for the console application.
//

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

#include "stdafx.h"

// C LIBS
#include <io.h>            // _filelength()
#include <stdlib.h>     /* ldiv, ldiv_t */
#include <time.h>

// STL
#include <vector>
#include <map>
#include <string>
using namespace std;
//'identifier' : identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4786)

#include "DOSEmu.h"

// =======================================================================
/*
 #######   ######     ##    ## ######## ########  ##    ## ######## ##          ########  ######## ##          ###    ######## ######## ########
##     ## ##    ##    ##   ##  ##       ##     ## ###   ## ##       ##          ##     ## ##       ##         ## ##      ##    ##       ##     ##
##     ## ##          ##  ##   ##       ##     ## ####  ## ##       ##          ##     ## ##       ##        ##   ##     ##    ##       ##     ##
##     ##  ######     #####    ######   ########  ## ## ## ######   ##          ########  ######   ##       ##     ##    ##    ######   ##     ##
##     ##       ##    ##  ##   ##       ##   ##   ##  #### ##       ##          ##   ##   ##       ##       #########    ##    ##       ##     ##
##     ## ##    ##    ##   ##  ##       ##    ##  ##   ### ##       ##          ##    ##  ##       ##       ##     ##    ##    ##       ##     ##
 #######   ######     ##    ## ######## ##     ## ##    ## ######## ########    ##     ## ######## ######## ##     ##    ##    ######## ########
*/
// =======================================================================

#ifdef _DEBUG
bool _trace(TCHAR *format, ...)
{
   TCHAR buffer[1000];

   va_list argptr;
   va_start(argptr, format);
   wvsprintf(buffer, format, argptr);
   va_end(argptr);

   OutputDebugString(buffer);

   return TRUE;
}
#endif

HANDLE Thread(void* start, void* lpParameter, BOOL close_handle)
{
	DWORD id = 0;
	HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start, lpParameter, 0, &id);
	if(close_handle) { CloseHandle(h); h = NULL; }
	return h;
}

void Terminate(ULONG tcode) { TerminateProcess(GetCurrentProcess(), tcode); }

// =======================================================================
/*
########   #######   ######     ##    ## ######## ########  ##    ## ######## ##          ########  ######## ##          ###    ######## ######## ########
##     ## ##     ## ##    ##    ##   ##  ##       ##     ## ###   ## ##       ##          ##     ## ##       ##         ## ##      ##    ##       ##     ##
##     ## ##     ## ##          ##  ##   ##       ##     ## ####  ## ##       ##          ##     ## ##       ##        ##   ##     ##    ##       ##     ##
##     ## ##     ##  ######     #####    ######   ########  ## ## ## ######   ##          ########  ######   ##       ##     ##    ##    ######   ##     ##
##     ## ##     ##       ##    ##  ##   ##       ##   ##   ##  #### ##       ##          ##   ##   ##       ##       #########    ##    ##       ##     ##
##     ## ##     ## ##    ##    ##   ##  ##       ##    ##  ##   ### ##       ##          ##    ##  ##       ##       ##     ##    ##    ##       ##     ##
########   #######   ######     ##    ## ######## ##     ## ##    ## ######## ########    ##     ## ######## ######## ##     ##    ##    ######## ########
*/
// =======================================================================

int videomodeCXCY(BOOL is_CX, short mode)
{
	int ret = 0;

	switch(mode)
	{
	case _MRES16COLOR:
	case _MRES256COLOR:
	case _MRESTRUECOLOR:
		ret = is_CX ? 320:200;
		break;
	case _VRES2COLOR:
	case _VRES16COLOR:
	case _VRES256COLOR:
	case _VRESTRUECOLOR:
		ret = is_CX ? 640:480;
		break;
	case _SVRES16COLOR:
	case _SVRES256COLOR:
	case _SVRESTRUECOLOR:
		ret = is_CX ? 800:600;
		break;
	case _XRES16COLOR:
	case _XRES256COLOR:
	case _XRESTRUECOLOR:
		ret = is_CX ? 1024:768;
		break;
	case _YRESTRUECOLOR:
		ret = is_CX ? 1280:1024;
		break;
	case _ZRESTRUECOLOR:
		ret = is_CX ? 1600:1200;
		break;
	default:
		// ALERT ERROR
		// emuKernel.ALERT("ERR: You try setup unsupported videomode");
		break;
	}

	return ret;
}

// =======================================================================

//------------------------------------------
// HARDWARE AND BIOS ALLOCATED STRUCTURES
//------------------------------------------
int MAIN_ARGC = 0;
char** MAIN_ARGV = NULL;

BYTE* MEMORY_0xA0000000 = NULL;
BYTE* MEMORY_0xF000FA6EL = NULL;

BYTE PRESSED_RAW_DOS_KEY = DOS_VK_UNDEFINED;
void (*KEYBOARD_INTERRUPT_FUNC)(void) = NULL;
BYTE* WINKEY_DOSKEY = NULL;

void (*TIMER_INTERRUPT_FUNC)(void) = NULL;
void (*SERIAL_PORT0_INTERRUPT_FUNC)(void) = NULL;

// =======================================================================

//------------------------------------------
// PREDEFINED TABLE FOR SPEEDUP DRAW CALCULATIONS
//------------------------------------------
long** VRES_Y_COORD_PREDEFINED_TABLE = NULL;

void Init_VRES_Y_COORD_PREDEFINED_TABLE(short mode)
{
	if(!VRES_Y_COORD_PREDEFINED_TABLE)
	{
		VRES_Y_COORD_PREDEFINED_TABLE = new long*[_LVRESMODE_LAST_INDEX];
		memset(VRES_Y_COORD_PREDEFINED_TABLE, 0, sizeof(long*)*_LVRESMODE_LAST_INDEX);
	}
	if(!VRES_Y_COORD_PREDEFINED_TABLE[mode])
	{
		int cx = videomodeCXCY(TRUE, mode);
		int cy = videomodeCXCY(FALSE, mode);

		VRES_Y_COORD_PREDEFINED_TABLE[mode] = new long[cy];
		for (int ndx=0; ndx < cy; ndx++ )
		{
			VRES_Y_COORD_PREDEFINED_TABLE[mode][ndx] = ndx*cx;
		}
	}
}


// =======================================================================
/*
########  #### ########     #### ##     ##    ###     ######   ########     ######  ##          ###     ######   ######
##     ##  ##  ##     ##     ##  ###   ###   ## ##   ##    ##  ##          ##    ## ##         ## ##   ##    ## ##    ##
##     ##  ##  ##     ##     ##  #### ####  ##   ##  ##        ##          ##       ##        ##   ##  ##       ##
##     ##  ##  ########      ##  ## ### ## ##     ## ##   #### ######      ##       ##       ##     ##  ######   ######
##     ##  ##  ##     ##     ##  ##     ## ######### ##    ##  ##          ##       ##       #########       ##       ##
##     ##  ##  ##     ##     ##  ##     ## ##     ## ##    ##  ##          ##    ## ##       ##     ## ##    ## ##    ##
########  #### ########     #### ##     ## ##     ##  ######   ########     ######  ######## ##     ##  ######   ######
*/
// =======================================================================

// PALETE CORRECTION FLAGS
#define PALETTE_CORRECT_ON			1
#define PALETTE_CORRECT_OFF			2

/**
DEVICE INDEPENDED SURFACE (BITMAP) CLASS
*/
struct DIBSurface
{
	BITMAPINFO*	m_surfaceBI; // (R)(G)(B)(0) PALETTE HERE === RGBQUAD[256]
	BYTE*		m_surfaceBytes;
	BYTE*		m_surfacePalette; // JUST COPY OF RAW PALETTE STORAGE === BYTES[768]

	DIBSurface()
	{
		zero();
	}
	~DIBSurface()
	{
		free();
	}
	void free()
	{
		if(m_surfaceBI) delete m_surfaceBI;
		if(m_surfaceBytes) delete m_surfaceBytes;
		if(m_surfacePalette) delete m_surfacePalette;
		m_surfaceBI = NULL;
		m_surfaceBytes = NULL;
		m_surfacePalette = NULL;
	}
	void zero()
	{
		ZeroMemory(this, sizeof(DIBSurface));
	}
	BYTE* new_surface_raw_bytes_buffer(int size)
	{
		if(m_surfaceBytes) delete m_surfaceBytes;
		m_surfaceBytes = new BYTE[size];
		memset(m_surfaceBytes, 0, size);
		return m_surfaceBytes;
	}
	BYTE*  new_surface(int cx, int cy, int biBitCount = VALUE_NOT_DEFINED)
	{
		biBitCount = def_val(VALUE_NOT_DEFINED, biBitCount, 8);

		if(m_surfaceBI) delete m_surfaceBI;
		m_surfaceBI = NewBITMAPINFOHEADER(cx, cy, biBitCount, 1);
		return new_surface_raw_bytes_buffer(cx * cy);
	}

	void set_palette_for_surface(BYTE* palette, int size = VALUE_NOT_DEFINED, int is_correct_palette = PALETTE_CORRECT_ON)
	{
		size = def_val(VALUE_NOT_DEFINED, size, _256COLORS * 3);
		is_correct_palette = def_val(VALUE_NOT_DEFINED, is_correct_palette, PALETTE_CORRECT_ON);

		if(m_surfacePalette) delete m_surfacePalette;
		m_surfacePalette = new BYTE[_256COLORS * 3];
		memset(m_surfacePalette, 0, _256COLORS * 3);
		memcpy(m_surfacePalette, palette, size);

		if(is_correct_palette == PALETTE_CORRECT_ON)
		{
			PaletteCorrect(TRUE, m_surfacePalette, size);
		}

		PaletteRAW2RGB(m_surfaceBI->bmiColors, m_surfacePalette);
	}
};

// =======================================================================
/*
##     ##  ######      ###        ######   ######  ########  ######## ######## ##    ##     ######  ##          ###     ######   ######
##     ## ##    ##    ## ##      ##    ## ##    ## ##     ## ##       ##       ###   ##    ##    ## ##         ## ##   ##    ## ##    ##
##     ## ##         ##   ##     ##       ##       ##     ## ##       ##       ####  ##    ##       ##        ##   ##  ##       ##
##     ## ##   #### ##     ##     ######  ##       ########  ######   ######   ## ## ##    ##       ##       ##     ##  ######   ######
 ##   ##  ##    ##  #########          ## ##       ##   ##   ##       ##       ##  ####    ##       ##       #########       ##       ##
  ## ##   ##    ##  ##     ##    ##    ## ##    ## ##    ##  ##       ##       ##   ###    ##    ## ##       ##     ## ##    ## ##    ##
   ###     ######   ##     ##     ######   ######  ##     ## ######## ######## ##    ##     ######  ######## ##     ##  ######   ######
*/
// =======================================================================

#define SCREEN_WINDOW_CLASS_NAME "_DOS_EMU_TOOLBOX_CLASS_26_05_2019_"

/**
SCREEN IMPLENEMTATION
*/
struct VGAScreen : public CLockObject
{
	//------------------------------------------
	// COORD LIKE STUFF
	//------------------------------------------
	// SPECIFY VIDEOMODE
	int m_videomode;
	int m_cx;	// X-AXIS RESOLUTION
	int m_cy;	// Y-AXIS RESOLUTION

	// SPECIFY IS ORIGINAL LOW-RES SCREEN MUST BE SCALED
	double m_scale;
	int m_apply_if_screen_cx_less_than;
	int m_apply_screen_cx;

	//------------------------------------------
	// DOS MIRROR STUFF
	//------------------------------------------
	char VGAVAR_selected_color;
	int	VGAVAR_selected_X;
	int	VGAVAR_selected_Y;

	//------------------------------------------
	// RENDER STATE
	//------------------------------------------
	int m_VGA_RENDER_state;
	void _set_render_options(BOOL is_set, int options)
	{
		AUTOLOCK(*this);

		if(is_set) m_VGA_RENDER_state |= options;
		else m_VGA_RENDER_state = (m_VGA_RENDER_state & (~options));
	}
	//------------------------------------------

	//------------------------------------------
	// HARDWARE IN/OUT STUFF
	//------------------------------------------
	int INOUT_palette_offset;
	int INOUT_palette_write_count;


	HWND		m_screenHWND;
	DIBSurface* m_screen;
	BasicGraphRoutines* m_BasicGraphRoutines;

	VGAScreen()
	{
		m_screen = NULL;
		m_videomode = _MRES256COLOR;
		m_cx = 0;
		m_cy = 0;

		m_VGA_RENDER_state = RENDER_EMPTY_STATE;
		_set_render_options(TRUE, RENDER_CORRECT_PALETTE);

		VGAVAR_selected_X = 0;
		VGAVAR_selected_Y = 0;

		// 1.
		m_screen = new DIBSurface();
		m_BasicGraphRoutines = new BasicGraphRoutines();

		// 2.
		_set_render_options(TRUE, RENDER_SCALE_VGA_SCREEN);
		_set_screen_scale_params(SCALE_IF_RES_LESS_THAN_CX_DEFAULT, SCALE_RES_CX_DEFAULT);
	}
	~VGAScreen()
	{
		delete m_screen;
		delete m_BasicGraphRoutines;
	}

	BYTE* GetVgaMem()
	{
		return m_screen->m_surfaceBytes;
	}
	BYTE* GetVgaPaletteMem()
	{
		return m_screen->m_surfacePalette;
	}

	void _setpalette(BYTE* palette, int size = VALUE_NOT_DEFINED, int is_correct_palette = VALUE_NOT_DEFINED)
	{
		AUTOLOCKTHIS;

		is_correct_palette = def_val(VALUE_NOT_DEFINED, is_correct_palette,
			(m_VGA_RENDER_state & RENDER_CORRECT_PALETTE) ? PALETTE_CORRECT_ON : PALETTE_CORRECT_OFF);
		m_screen->set_palette_for_surface(palette, size, is_correct_palette);
	}

	void _setvideomode(short mode)
	{
		{ // LOCK STAT
			AUTOLOCKTHIS;

			if(mode == _DEFAULTMODE) return;

			Init_VRES_Y_COORD_PREDEFINED_TABLE(mode);

			m_videomode = mode;
			m_cx = videomodeCXCY(TRUE, mode);
			m_cy = videomodeCXCY(FALSE, mode);

			_set_screen_scale_params();

			m_screen->new_surface(m_cx, m_cy, 8);
			_setpalette(VGA_PALETTE);

			MEMORY_0xA0000000 = m_screen->m_surfaceBytes;

			InitBasicGraphRoutines();
		} // LOCK END

		CorrectWindowSize();
	}
	void InitBasicGraphRoutines()
	{
		AUTOLOCKTHIS;
		m_BasicGraphRoutines->init(m_cx, m_cy, GetVgaMem(), VRES_Y_COORD_PREDEFINED_TABLE[m_videomode]);
	}

	void _set_screen_scale_params(int apply_if_screen_cx_less_than = VALUE_NOT_DEFINED, int apply_screen_cx = VALUE_NOT_DEFINED)
	{
		m_apply_if_screen_cx_less_than = def_val(VALUE_NOT_DEFINED, apply_if_screen_cx_less_than, m_apply_if_screen_cx_less_than);
		m_apply_screen_cx = def_val(VALUE_NOT_DEFINED, apply_screen_cx, m_apply_screen_cx);

		// WARNING: m_cx MUST BE NOT NULL! NULL DIVIDE POSSIBLE!
		m_scale = (double)m_apply_screen_cx/(double)(m_cx ? m_cx : 1);
	}

	/**
	CORRECT WINDOW SIZE WHEN RESOLUTION (VIDEOMODE) CHANGED
	*/
	void CorrectWindowSize()
	{
		int CXSCREEN = GetSystemMetrics(SM_CXSCREEN);
		int CYSCREEN = GetSystemMetrics(SM_CYSCREEN);

		HWND screenHWND;
		int cx, cy;

		{ // LOCK STAT
			AUTOLOCKTHIS;
			if(!m_screenHWND) return;
			screenHWND = m_screenHWND;

			if(m_VGA_RENDER_state & RENDER_SCALE_VGA_SCREEN)
			{
				cx = m_cx*m_scale;
				cy = m_cy*m_scale;
			}
			else
			{
				cx = m_cx;
				cy = m_cy;
			}
		} // LOCK END


		SetWindowPos(screenHWND,
					NULL,
					(CXSCREEN-cx)/2,
					(CYSCREEN-cy)/2,
					cx,
					cy + GetSystemMetrics(SM_CYCAPTION),
					0);

		RedrawScreen();
	}

	/**
	CLONE VIDEOMEMORY TO SCREEN
	*/
	void RedrawScreen()
	{
		AUTOLOCKTHIS;

	   if (m_screen && m_screen->m_surfaceBytes && m_screen->m_surfaceBI )
	   {
			HDC hdc = GetDC(m_screenHWND);

			//------------------------------------------
			// SCALED SCRREN SHOW CASE
			//------------------------------------------
			if(m_VGA_RENDER_state & RENDER_SCALE_VGA_SCREEN)
			{
				//SetStretchBltMode(hdc,HALFTONE);
				StretchDIBits(hdc, 0, 0,
							m_screen->m_surfaceBI->bmiHeader.biWidth*m_scale,
							abs(m_screen->m_surfaceBI->bmiHeader.biHeight)*m_scale,
							0, 0,
							m_screen->m_surfaceBI->bmiHeader.biWidth,
							abs(m_screen->m_surfaceBI->bmiHeader.biHeight),
							m_screen->m_surfaceBytes, m_screen->m_surfaceBI, DIB_RGB_COLORS, SRCCOPY);
			}
			//------------------------------------------
			// NOT SCALED SCRREN SHOW CASE
			//------------------------------------------
			else
			{
				// Display the image
				SetDIBitsToDevice(hdc, 0, 0,
							m_screen->m_surfaceBI->bmiHeader.biWidth,
							abs(m_screen->m_surfaceBI->bmiHeader.biHeight),
							0, 0,
							0, // ZERO HERE
							abs(m_screen->m_surfaceBI->bmiHeader.biHeight),
							m_screen->m_surfaceBytes, m_screen->m_surfaceBI, DIB_RGB_COLORS);
			}
			//------------------------------------------

			ReleaseDC(m_screenHWND, hdc);
	   }
	}

	static HWND DoScereenWindowWinAPI(WNDPROC WndProc)
	{
		WNDCLASS wc={};
		wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.hCursor=LoadCursor(NULL, IDC_ARROW);
		wc.hInstance=NULL;
		wc.lpfnWndProc=WndProc;
		wc.lpszClassName= SCREEN_WINDOW_CLASS_NAME;
		wc.style=CS_HREDRAW | CS_VREDRAW;
		if (!RegisterClass(&wc))
		{
			// emuKernel.ALERT("Could not register class");
		}
		HWND hwnd = CreateWindow(SCREEN_WINDOW_CLASS_NAME,
							 NULL,
							 WS_POPUP | WS_CAPTION,
							 40,
							 40,
							 GetSystemMetrics(SM_CXSCREEN)/2,
							 GetSystemMetrics(SM_CYSCREEN)/2,

							 NULL,
							 NULL,
							 NULL,
							 NULL);

		ShowWindow(hwnd, SW_RESTORE);
		return hwnd;
	}

	void DoScereenWindow(WNDPROC WndProc)
	{
		HWND hwnd = VGAScreen::DoScereenWindowWinAPI(WndProc);

		SAFE_OBJECT(*this, m_screenHWND = hwnd);

		CorrectWindowSize();
	}

	void DestroyScereenWindow()
	{
		AUTOLOCKTHIS;

		if(m_screenHWND)
		{
			DestroyWindow(m_screenHWND);
			DeleteObject(m_screenHWND); //doing it just in case
			m_screenHWND = NULL;
		}
	}

};

// =======================================================================
/*
########  #### ######## ##     ##    ###    ########     ########  ######## ##          ###    ######## #### ##    ##  ######       ######  ########  ######  ######## ####  #######  ##    ##
##     ##  ##     ##    ###   ###   ## ##   ##     ##    ##     ## ##       ##         ## ##      ##     ##  ###   ## ##    ##     ##    ## ##       ##    ##    ##     ##  ##     ## ###   ##
##     ##  ##     ##    #### ####  ##   ##  ##     ##    ##     ## ##       ##        ##   ##     ##     ##  ####  ## ##           ##       ##       ##          ##     ##  ##     ## ####  ##
########   ##     ##    ## ### ## ##     ## ########     ########  ######   ##       ##     ##    ##     ##  ## ## ## ##   ####     ######  ######   ##          ##     ##  ##     ## ## ## ##
##     ##  ##     ##    ##     ## ######### ##           ##   ##   ##       ##       #########    ##     ##  ##  #### ##    ##           ## ##       ##          ##     ##  ##     ## ##  ####
##     ##  ##     ##    ##     ## ##     ## ##           ##    ##  ##       ##       ##     ##    ##     ##  ##   ### ##    ##     ##    ## ##       ##    ##    ##     ##  ##     ## ##   ###
########  ####    ##    ##     ## ##     ## ##           ##     ## ######## ######## ##     ##    ##    #### ##    ##  ######       ######  ########  ######     ##    ####  #######  ##    ##
*/
// =======================================================================


BITMAPINFO* NewBITMAPINFOHEADER(int biWidth, int biHeight, int biBitCount, int biPlanes)
{
	// Set the bitmap information
	int len = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*_256COLORS;
	BITMAPINFO* bi = (BITMAPINFO *)new BYTE[len];
	ZeroMemory(bi, len);

	bi->bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth          = biWidth;
	bi->bmiHeader.biHeight         = -biHeight;
	bi->bmiHeader.biPlanes         = biPlanes;
	bi->bmiHeader.biBitCount       = biBitCount;
	bi->bmiHeader.biCompression    = BI_RGB;
	return bi;
}

void PaletteRAW2RGB(RGBQUAD* rgb, BYTE* colors, int offset, int count)
{
	offset = def_val(VALUE_NOT_DEFINED, offset, 0);
	count = def_val(VALUE_NOT_DEFINED, count, _256COLORS);

	int ndx = offset*3;
	for (int ndx2=offset; ndx2 < offset+count; ndx2++ )
	{
		rgb[ndx2].rgbRed       = colors[ndx++];
		rgb[ndx2].rgbGreen     = colors[ndx++];
		rgb[ndx2].rgbBlue      = colors[ndx++];
		rgb[ndx2].rgbReserved  = 0;
	}
}

void PaletteCorrect(BOOL is_from_dos, BYTE* palette, int size)
{
	 for(int i=0;i<size;i++)            // bit shift palette
	 {
		palette[i]= is_from_dos ?
			(((unsigned char)(palette[i])) << 2) :
			(((unsigned char)(palette[i])) >> 2);
	 }
}

// =======================================================================
/*
########   #######   ######     ######## ##     ## ##     ## ##          ###    ######## #### ##    ##  ######         ###    ########  ####
##     ## ##     ## ##    ##    ##       ###   ### ##     ## ##         ## ##      ##     ##  ###   ## ##    ##       ## ##   ##     ##  ##
##     ## ##     ## ##          ##       #### #### ##     ## ##        ##   ##     ##     ##  ####  ## ##            ##   ##  ##     ##  ##
##     ## ##     ##  ######     ######   ## ### ## ##     ## ##       ##     ##    ##     ##  ## ## ## ##   ####    ##     ## ########   ##
##     ## ##     ##       ##    ##       ##     ## ##     ## ##       #########    ##     ##  ##  #### ##    ##     ######### ##         ##
##     ## ##     ## ##    ##    ##       ##     ## ##     ## ##       ##     ##    ##     ##  ##   ### ##    ##     ##     ## ##         ##
########   #######   ######     ######## ##     ##  #######  ######## ##     ##    ##    #### ##    ##  ######      ##     ## ##        ####
*/
// =======================================================================

//------------------------------------------
// VGA MIRROR FUNCS
//------------------------------------------

VGAScreen* vgaScreenPrimary = NULL;

void _set_render_options(BOOL is_set, int options)
{
	vgaScreenPrimary->_set_render_options(is_set, options);
}

void _redraw_screen(int redraw_mode)
{
	BOOL is_readraw = FALSE;

	{ // LOCK START
		AUTOLOCK(*vgaScreenPrimary);

		is_readraw = ((redraw_mode == REDRAW_IMMEDIATELY) ||
			((redraw_mode == REDRAW_DEFAULT) &&
				(!(vgaScreenPrimary->m_VGA_RENDER_state & RENDER_MANUAL_REDRAW)) ));
	} // LOCK END

	if(is_readraw)
	{
		vgaScreenPrimary->RedrawScreen();
	}
}

void _set_screen_scale_params(int apply_if_screen_cx_less_than, int apply_screen_cx)
{
	vgaScreenPrimary->_set_screen_scale_params(apply_if_screen_cx_less_than, apply_screen_cx);
}


void _clearscreen(char color)
{
	AUTOLOCK(*vgaScreenPrimary);
	vgaScreenPrimary->m_BasicGraphRoutines->Cls(color);
}

void _setvideomode(short mode)
{
	vgaScreenPrimary->_setvideomode(mode);
}

void _setpalette(BYTE* palette, int size, int is_correct_palette)
{
	vgaScreenPrimary->_setpalette(palette, size, is_correct_palette);
}

void _setcolor(char color)
{
	AUTOLOCK(*vgaScreenPrimary);
	vgaScreenPrimary->VGAVAR_selected_color = color;
}

void _setpixel(int x, int y, int redraw_mode)
{
	AUTOLOCK(*vgaScreenPrimary);
	vgaScreenPrimary->m_BasicGraphRoutines->DrawPixel(x, y, vgaScreenPrimary->VGAVAR_selected_color);

	_redraw_screen(redraw_mode);
}

char _getpixel(int x, int y)
{
	AUTOLOCK(*vgaScreenPrimary);
	return vgaScreenPrimary->m_BasicGraphRoutines->GrabPixel(x, y);
}

void _moveto(int x, int y)
{
	AUTOLOCK(*vgaScreenPrimary);
	vgaScreenPrimary->VGAVAR_selected_X = x;
	vgaScreenPrimary->VGAVAR_selected_Y = y;
}

void _lineto(int x, int y, int redraw_mode)
{
	AUTOLOCK(*vgaScreenPrimary);
	vgaScreenPrimary->m_BasicGraphRoutines->DrawLine(
			vgaScreenPrimary->VGAVAR_selected_X,
			vgaScreenPrimary->VGAVAR_selected_Y,
			x, y,
			vgaScreenPrimary->VGAVAR_selected_color);
	vgaScreenPrimary->VGAVAR_selected_X = x;
	vgaScreenPrimary->VGAVAR_selected_Y = y;

	_redraw_screen(redraw_mode);
}

void _rectangle(int draw_mode, int x1, int y1, int x2, int y2)
{
	AUTOLOCK(*vgaScreenPrimary);

	if(draw_mode == _GFILLINTERIOR)
	{
		vgaScreenPrimary->m_BasicGraphRoutines->DrawBar(x1, y1, x2, y2, vgaScreenPrimary->VGAVAR_selected_color);
		_redraw_screen(REDRAW_DEFAULT);
	}
	else // _GBORDER
	{
		vgaScreenPrimary->m_BasicGraphRoutines->DrawBox(x1, y1, x2, y2, vgaScreenPrimary->VGAVAR_selected_color);
		_redraw_screen(REDRAW_DEFAULT);

		/*
		long lines[] =
		{
			x1, y1,
			x2, y1,
			x2, y2,
			x1, y2,
			x1, y1,
		};
		_moveto(lines[0], lines[1]);
		for(int i = 2; i<2+4*2; i+=2)
		{
			_lineto(lines[i], lines[i+1]);
		}
		*/
	}
}

void _ellipse(int fill, int x1, int y1, int x2, int y2 )
{
	AUTOLOCK(*vgaScreenPrimary);
	vgaScreenPrimary->m_BasicGraphRoutines->DrawEllipse(x1, y1, x2, y2, vgaScreenPrimary->VGAVAR_selected_color);

	_redraw_screen(REDRAW_DEFAULT);
}

void console_gotoxy(int x, int y)
{
	// DON'T CLOCE STD HANDLES
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD pos = {x, y};
    SetConsoleCursorPosition(output, pos);
}

void _settextposition(int x, int y)
{
	console_gotoxy(x, y);
}

// =======================================================================

#define NUM_THREADS   2
#define WM_CUSTOM_SET_LINKED_SCREEN (WM_USER + 1)

/**
DOS EMULATION KERNEL
*/
struct DOSEmuKernel : public CLockObject
{
	//------------------------------------------
	// START/STOP THREADS SERVING
	//------------------------------------------
	CBoolEvent m_stop_event;
	BOOL IsStop(ULONG time = 0) { return m_stop_event.IsTrue(time); }

	HANDLE m_threads[NUM_THREADS];
	ULONG m_nInitedThreads;
	//------------------------------------------

	DOSEmuKernel()
	{
		m_nInitedThreads = 0;
	}

	/**
	ALERT LIKE DEBUG OUT ECT.
	*/
	void ALERT(const char* alert, BOOL is_show_message_box = FALSE)
	{
		MessageBox(NULL, alert, "Error", MB_OK);
	}

	/**
	WAITING ALL THREADS STOPPED
	*/
	BOOL StopServer(DWORD timeout)
	{
		BOOL ok = TRUE;

		if(WaitForMultipleObjects(NUM_THREADS, m_threads, TRUE, timeout) == WAIT_TIMEOUT)
		{
			ok = FALSE;
		}
		for (int ndx=0; ndx < NUM_THREADS; ndx++ )
		{
			TerminateThread(m_threads[ndx], -1);
			CloseHandle(m_threads[ndx]);

			TRACE("THREAD TERMINATED = %d : 0x%X\n", ndx, m_threads[ndx]);
		}

		return ok;
	}
};

// =======================================================================


DOSEmuKernel emuKernel;


// =======================================================================
/*
#### ##    ## ######## ######## ########  ########  ########   #######   ######  ########  ######   ######      ######   #######  ##     ## ##     ## ##     ## ##    ## ####  ######     ###    ######## ####  #######  ##    ##     ######  ########  ######  ######## ####  #######  ##    ##
 ##  ###   ##    ##    ##       ##     ## ##     ## ##     ## ##     ## ##    ## ##       ##    ## ##    ##    ##    ## ##     ## ###   ### ###   ### ##     ## ###   ##  ##  ##    ##   ## ##      ##     ##  ##     ## ###   ##    ##    ## ##       ##    ##    ##     ##  ##     ## ###   ##
 ##  ####  ##    ##    ##       ##     ## ##     ## ##     ## ##     ## ##       ##       ##       ##          ##       ##     ## #### #### #### #### ##     ## ####  ##  ##  ##        ##   ##     ##     ##  ##     ## ####  ##    ##       ##       ##          ##     ##  ##     ## ####  ##
 ##  ## ## ##    ##    ######   ########  ########  ########  ##     ## ##       ######    ######   ######     ##       ##     ## ## ### ## ## ### ## ##     ## ## ## ##  ##  ##       ##     ##    ##     ##  ##     ## ## ## ##     ######  ######   ##          ##     ##  ##     ## ## ## ##
 ##  ##  ####    ##    ##       ##   ##   ##        ##   ##   ##     ## ##       ##             ##       ##    ##       ##     ## ##     ## ##     ## ##     ## ##  ####  ##  ##       #########    ##     ##  ##     ## ##  ####          ## ##       ##          ##     ##  ##     ## ##  ####
 ##  ##   ###    ##    ##       ##    ##  ##        ##    ##  ##     ## ##    ## ##       ##    ## ##    ##    ##    ## ##     ## ##     ## ##     ## ##     ## ##   ###  ##  ##    ## ##     ##    ##     ##  ##     ## ##   ###    ##    ## ##       ##    ##    ##     ##  ##     ## ##   ###
#### ##    ##    ##    ######## ##     ## ##        ##     ##  #######   ######  ########  ######   ######      ######   #######  ##     ## ##     ##  #######  ##    ## ####  ######  ##     ##    ##    ####  #######  ##    ##     ######  ########  ######     ##    ####  #######  ##    ##
*/
// =======================================================================

/*
NOTE:
THIS IS SIMPLEST, BUT NOT BETTER IPC SOLUTION
IF YOU WANT, YOU MAY REWRITE IT USING SOCKETS (HE-HE-HE...)
*/

typedef struct
{
	int		packet_id;
	int		sender_id;
	int		bytes_length;
	char	bytes[0];
}
SIMPLE_IPC_PACKET;

#define SIPC_ID_PACKET			1
#define SIPC_ID_SERIAL_PORT_PACKET_BYTES	2

// CALBACL FOR EnumWindows()
vector <HWND> g_DosEmuWindows;
BOOL CALLBACK enumDosEmuWindowsProc(HWND hWnd, LPARAM lParam)
{
	char class_name[300];
	if(!GetClassName(hWnd, class_name, sizeof(class_name)-1)) return TRUE;

	if(strcmp(SCREEN_WINDOW_CLASS_NAME, class_name)) return TRUE;

	g_DosEmuWindows.push_back(hWnd);
	return TRUE;
}

void IPC_Call_OUT(char* chars, int size)
{
	// PREPARE PACKET FOR SEND
	int packet_size = size + sizeof(SIMPLE_IPC_PACKET);
	char* tmp = new char[packet_size];
	SIMPLE_IPC_PACKET* sip = (SIMPLE_IPC_PACKET*) tmp;
	sip->packet_id = SIPC_ID_SERIAL_PORT_PACKET_BYTES;
	sip->sender_id = GetCurrentProcessId();
	sip->bytes_length = size;
	memcpy(sip->bytes, chars, size);

	// PREPARE COPYDDATASTRUCT
	COPYDATASTRUCT cds = {};
	cds.dwData = SIPC_ID_PACKET;  // Can be used by the receiver to identify data
	cds.cbData = packet_size;
	cds.lpData = tmp;


	// GET WNDOWS FOR IPC CALLS
	g_DosEmuWindows.clear();
	::EnumWindows( enumDosEmuWindowsProc, NULL /* An application-defined value to be passed to the callback function. */ );

	// DO IPC CALLS
	for(int i =0; i<g_DosEmuWindows.size(); i++ )
	{
		// DON'T SEND TO SELF
		if(g_DosEmuWindows[i] == vgaScreenPrimary->m_screenHWND) continue;

		// Send the data. PostMessage NOT ALLOWED FOR WM_COPYDATA
		SendMessage(g_DosEmuWindows[i], WM_COPYDATA,
					(WPARAM)NULL, // hWndSender
					(LPARAM)(LPVOID)&cds);
	}

	delete[] tmp;
}

unsigned char m_curr_income_serial_port_byte = 0;
void IPC_Call_IN(COPYDATASTRUCT* cds)
{
	switch(cds->dwData)
	{
	case SIPC_ID_PACKET:
		{
			SIMPLE_IPC_PACKET* sip = (SIMPLE_IPC_PACKET*) cds->lpData;
			switch(sip->packet_id)
			{
			case SIPC_ID_SERIAL_PORT_PACKET_BYTES:
				{
					// THIS IS OWN INSTANCE MESSAGE
					// if(sip->sender_id == GetCurrentProcessId()) break;

					for(int i=0; i<sip->bytes_length; i++)
					{
						if(SERIAL_PORT0_INTERRUPT_FUNC)
						{
							m_curr_income_serial_port_byte = sip->bytes[i];
							SERIAL_PORT0_INTERRUPT_FUNC();
						}
					}
				}
				break;
			}
		}
		break;
	}
}

// =======================================================================
/*
 ######   ######  ########  ######## ######## ##    ##    ##      ## #### ##    ## ########   #######  ##      ##    ##     ##    ###    ##    ## ########  #### ##    ##  ######       ######  ########  ######  ######## ####  #######  ##    ##
##    ## ##    ## ##     ## ##       ##       ###   ##    ##  ##  ##  ##  ###   ## ##     ## ##     ## ##  ##  ##    ##     ##   ## ##   ###   ## ##     ##  ##  ###   ## ##    ##     ##    ## ##       ##    ##    ##     ##  ##     ## ###   ##
##       ##       ##     ## ##       ##       ####  ##    ##  ##  ##  ##  ####  ## ##     ## ##     ## ##  ##  ##    ##     ##  ##   ##  ####  ## ##     ##  ##  ####  ## ##           ##       ##       ##          ##     ##  ##     ## ####  ##
 ######  ##       ########  ######   ######   ## ## ##    ##  ##  ##  ##  ## ## ## ##     ## ##     ## ##  ##  ##    ######### ##     ## ## ## ## ##     ##  ##  ## ## ## ##   ####     ######  ######   ##          ##     ##  ##     ## ## ## ##
      ## ##       ##   ##   ##       ##       ##  ####    ##  ##  ##  ##  ##  #### ##     ## ##     ## ##  ##  ##    ##     ## ######### ##  #### ##     ##  ##  ##  #### ##    ##           ## ##       ##          ##     ##  ##     ## ##  ####
##    ## ##    ## ##    ##  ##       ##       ##   ###    ##  ##  ##  ##  ##   ### ##     ## ##     ## ##  ##  ##    ##     ## ##     ## ##   ### ##     ##  ##  ##   ### ##    ##     ##    ## ##       ##    ##    ##     ##  ##     ## ##   ###
 ######   ######  ##     ## ######## ######## ##    ##     ###  ###  #### ##    ## ########   #######   ###  ###     ##     ## ##     ## ##    ## ########  #### ##    ##  ######       ######  ########  ######     ##    ####  #######  ##    ##
*/
// =======================================================================


char TranslateVirtualKeyIntoChar(UINT key)
{
	// HETE MUST BE KEYBOADR MATCH MAP TABLE VirtualKey -> DOSKey
	// YOU MAY WRITE IT!

   char c = (char)MapVirtualKey(key, 2);  // MAPVK_VK_TO_CHAR = 2
   short shiftState = GetAsyncKeyState(VK_SHIFT); // MUST BE SHORT !

   if (shiftState) c = toupper(c);
   else c = tolower(c);

   return c;
}

/**
VGA SCREEN WINDOW WndProc
*/
LRESULT CALLBACK ScereenWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static VGAScreen* vgaScreen = NULL;

    // This structure will be used to create the keyboard
    // input event.
	// YOU MAY USE ToAscii() OR ToAsciiEx() WINAPI, BUT...

	//------------------------------------------
	// DOS KEYBOARD INTERRUPT SERVING
	//------------------------------------------
	void (*dos_int)(void) = NULL;
	if((message == WM_KEYUP) || (message == WM_KEYDOWN))
	{ // LOCK START
		AUTOLOCK(emuKernel);
		dos_int = KEYBOARD_INTERRUPT_FUNC;

		PRESSED_RAW_DOS_KEY = WINKEY_DOSKEY[(char)wparam];
		if(message == WM_KEYUP) PRESSED_RAW_DOS_KEY |= 0x80;
	} // LOCK END
	//------------------------------------------

	switch (message)
	{
		case WM_CUSTOM_SET_LINKED_SCREEN:
			vgaScreen = (VGAScreen*)wparam;

			{ // LOCK START
				AUTOLOCK(emuKernel);
				emuKernel.m_nInitedThreads++;
			} // LOCK END
		break;

		case WM_ERASEBKGND:
		{
			if(vgaScreen) vgaScreen->RedrawScreen();

			// WM_ERASEBKGND NOTE
			// An application should return nonzero if it erases the background; otherwise, it should return zero.
			return 1L;
		}
		break;

		case WM_KEYDOWN:
		{
			char chr = TranslateVirtualKeyIntoChar(wparam);

			//------------------------------------------
			// REDIRECT KEYPRESS TO CONSOLE WINDOW
			//------------------------------------------
			INPUT_RECORD ir[1];
			ir[0].EventType = KEY_EVENT;
			ir[0].Event.KeyEvent.bKeyDown = TRUE;
			ir[0].Event.KeyEvent.dwControlKeyState = 0;
			ir[0].Event.KeyEvent.uChar.UnicodeChar = chr;
			ir[0].Event.KeyEvent.wRepeatCount = 1;
			ir[0].Event.KeyEvent.wVirtualKeyCode = chr;
			ir[0].Event.KeyEvent.wVirtualScanCode = chr;

			DWORD dwTmp = 0;
			BOOL ret = WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 1, &dwTmp);
			//------------------------------------------

			//------------------------------------------
			// CALL DOS INTERRUPT FUNCTION
			//------------------------------------------
			if(dos_int)
			{
				dos_int();
			}
			//------------------------------------------

			// SHOW PRESSED KEY
			// TRACE("key = %d %#08x\n", (DWORD)wparam, (DWORD)wparam);
		}
		break;
		case WM_KEYUP:
		{
			//------------------------------------------
			// CALL DOS INTERRUPT FUNCTION
			//------------------------------------------
			if(dos_int)
			{
				dos_int();
			}

			{ // LOCK START
				AUTOLOCK(emuKernel);
				PRESSED_RAW_DOS_KEY = DOS_VK_UNDEFINED;
			} // LOCK END
			//------------------------------------------
		}
		break;

		case WM_CHAR: //this is just for a program exit besides window's borders/task bar
		if (wparam==VK_ESCAPE)
		{
			//emuKernel.m_stop_event.Stop();
		}
		break;

		case WM_COPYDATA:
			IPC_Call_IN((COPYDATASTRUCT*)lparam);
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, message, wparam, lparam);
	}

	return 0;
}

/**
THREAD FOR SERVING SCREEN WINDOW
*/
ULONG WINAPI ScreenThread(LPVOID pParam)
{
	VGAScreen *vgaScreen = (VGAScreen*) pParam;

	vgaScreen->DoScereenWindow(ScereenWndProc);
	vgaScreen->_setvideomode(_MRES256COLOR);

	// NOTIFY THAT SCEREEN WINDOW INITED
	SendMessage(vgaScreen->m_screenHWND, WM_CUSTOM_SET_LINKED_SCREEN, (WPARAM)vgaScreen, (LPARAM) NULL);

	// WINDOW MESSAGE LOOP
	MSG messages;
	while(GetMessage(&messages, NULL, 0, 0)>0)
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
		if(emuKernel.IsStop()) break;
	}
	vgaScreen->DestroyScereenWindow();
	return 0;

}

// =======================================================================
/*
#### ##    ##       ##  #######  ##     ## ########    ########   #######  ########  ########  ######     ######## ##     ## ##     ## ##          ###    ######## #### ##    ##  ######
 ##  ###   ##      ##  ##     ## ##     ##    ##       ##     ## ##     ## ##     ##    ##    ##    ##    ##       ###   ### ##     ## ##         ## ##      ##     ##  ###   ## ##    ##
 ##  ####  ##     ##   ##     ## ##     ##    ##       ##     ## ##     ## ##     ##    ##    ##          ##       #### #### ##     ## ##        ##   ##     ##     ##  ####  ## ##
 ##  ## ## ##    ##    ##     ## ##     ##    ##       ########  ##     ## ########     ##     ######     ######   ## ### ## ##     ## ##       ##     ##    ##     ##  ## ## ## ##   ####
 ##  ##  ####   ##     ##     ## ##     ##    ##       ##        ##     ## ##   ##      ##          ##    ##       ##     ## ##     ## ##       #########    ##     ##  ##  #### ##    ##
 ##  ##   ###  ##      ##     ## ##     ##    ##       ##        ##     ## ##    ##     ##    ##    ##    ##       ##     ## ##     ## ##       ##     ##    ##     ##  ##   ### ##    ##
#### ##    ## ##        #######   #######     ##       ##         #######  ##     ##    ##     ######     ######## ##     ##  #######  ######## ##     ##    ##    #### ##    ##  ######
*/
// =======================================================================

// =======================================================================

//------------------------------------------
// IN/OUT PORTS EMULATING
//------------------------------------------

//------------------------------------------
// VGA
//------------------------------------------
#define VGA_INPUT_STATUS_1__   0x3DA // vga status reg 1, bit 3 is the vsync
                                   // when 1 - retrace in progress
                                   // when 0 - no retrace
#define VGA_VSYNC_MASK__ 0x08      // masks off unwanted bit of status reg

//------------------------------------------
// PALETTE
//------------------------------------------
#define PALETTE_MASK__			0x3c6
#define PALETTE_REGISTER_RD__	0x3c7
#define PALETTE_REGISTER_WR__	0x3c8
#define PALETTE_DATA__			0x3c9

//------------------------------------------
// KEYBOARD
//------------------------------------------
#define KEY_BUFFER__			0x60

//------------------------------------------
// SERIAL PORT
//------------------------------------------
#define COM_1__           0x3F8 // base port address of port 0
#define COM_2__           0x2F8 // base port address of port 1

#define SER_LSR        5    // line status reg.
#define SER_RBF        0    // the read buffer
#define SER_THR        0    // the write buffer

#define COM_1__SER_LSR        (COM_1__ + SER_LSR)
#define COM_1__SER_RBF        (COM_1__ + SER_RBF)
#define COM_1__SER_THR        (COM_1__ + SER_THR)
#define COM_2__SER_LSR        (COM_2__ + SER_LSR)
#define COM_2__SER_RBF        (COM_2__ + SER_RBF)
#define COM_2__SER_THR        (COM_2__ + SER_THR)
//------------------------------------------

void _outp(int port, int value)
{
	AUTOLOCK(*vgaScreenPrimary);
	VGAScreen* scr = vgaScreenPrimary;

	switch(port)
	{
	case PALETTE_MASK__:
		break;
	case PALETTE_REGISTER_WR__:
	case PALETTE_REGISTER_RD__:
		scr->INOUT_palette_offset = value;
		scr->INOUT_palette_write_count = 0;
		break;
	case PALETTE_DATA__:
		if(vgaScreenPrimary->m_VGA_RENDER_state & RENDER_CORRECT_PALETTE)
		{
			value = ((unsigned int)value) << 2;
		}

		vgaScreenPrimary->m_screen->m_surfacePalette[scr->INOUT_palette_offset*3 + scr->INOUT_palette_write_count] = value;
		scr->INOUT_palette_write_count++;
		if(scr->INOUT_palette_write_count == 3)
		{
			PaletteRAW2RGB(vgaScreenPrimary->m_screen->m_surfaceBI->bmiColors,
				vgaScreenPrimary->m_screen->m_surfacePalette,
				scr->INOUT_palette_offset, 1);

			if(!(vgaScreenPrimary->m_VGA_RENDER_state &
				RENDER_NOT_REDRAW_IF_BY_PORT_PALETTE_CHANGED)) _redraw_screen();
		}
		break;

	//------------------------------------------
	// SERIAL WORK
	//------------------------------------------
	case COM_1__SER_THR:
	case COM_2__SER_THR:
		// WRITE TO COM HERE
		char v = (value);
		IPC_Call_OUT(&v, sizeof(v));
		break;
	//------------------------------------------
	}
}

unsigned char _inp(int port)
{
	AUTOLOCK(*vgaScreenPrimary);
	VGAScreen* scr = vgaScreenPrimary;

	unsigned char ret = 0;

	switch(port)
	{
	case PALETTE_DATA__:
		{
			ret = vgaScreenPrimary->m_screen->m_surfacePalette[scr->INOUT_palette_offset*3 + scr->INOUT_palette_write_count];
			scr->INOUT_palette_write_count++;

			if(vgaScreenPrimary->m_VGA_RENDER_state & RENDER_CORRECT_PALETTE)
			{
				ret = ((unsigned int)ret) >> 2;
			}
		}
		break;
	case VGA_INPUT_STATUS_1__:
		{
			static unsigned int state = 0;
			ret = (state++ % 2) ? VGA_VSYNC_MASK__ : 0;
		}
		break;
	case KEY_BUFFER__:
		{ // LOCK START
			AUTOLOCK(emuKernel);
			ret = PRESSED_RAW_DOS_KEY;
		} // LOCK END
		break;

	//------------------------------------------
	// SERIAL WORK
	//------------------------------------------
	case COM_1__SER_LSR:
	case COM_2__SER_LSR:
		ret = 0x20; // TELL COM WRITE READY
		break;
	case COM_1__SER_RBF:
	case COM_2__SER_RBF:
		ret = m_curr_income_serial_port_byte;
		break;
	//------------------------------------------

	default:
		ret = 0;
	}

	return ret;
}

// =======================================================================

//------------------------------------------
// DOS TIMER EMULATION SUBSYSTEM
//------------------------------------------

// HARDWARE RELATED
ULONG DOS_55_MS_TIMER = 0;
BYTE* MEMORY_0x0000046C = (BYTE*)&DOS_55_MS_TIMER; // 18.2 clicks/sec (i.e. 55 ms) TIMER
ULONG PMEM_0x0000046C()
{
	AUTOLOCK(emuKernel);
	return DOS_55_MS_TIMER;
}

/**
TIME INTRAVALS THREAD. COUNTERS ECT.
*/
ULONG WINAPI TimerThread(LPVOID pParam)
{
	{ // LOCK START
		AUTOLOCK(emuKernel);
		emuKernel.m_nInitedThreads++;
	} // LOCK END

	while(!emuKernel.IsStop())
	{
		Sleep(55); // MEMORY_0x0000046C TIMER RATE

		{ // LOCK START
			AUTOLOCK(emuKernel);
			DOS_55_MS_TIMER++;
		} // LOCK END

		//------------------------------------------
		// DOS TIMER INTERRUPT HANDING
		//------------------------------------------
		void (*dos_int)(void) = NULL;
		{ // LOCK START
			AUTOLOCK(emuKernel);
			dos_int = TIMER_INTERRUPT_FUNC;
		} // LOCK END

		if(dos_int) dos_int();
		//------------------------------------------

	}

    return 0;
}

// =======================================================================
/*
#### ##    ## ######## ######## ########  ########  ##     ## ########  ########  ######     ########  ######## ##          ###    ######## ######## ########
 ##  ###   ##    ##    ##       ##     ## ##     ## ##     ## ##     ##    ##    ##    ##    ##     ## ##       ##         ## ##      ##    ##       ##     ##
 ##  ####  ##    ##    ##       ##     ## ##     ## ##     ## ##     ##    ##    ##          ##     ## ##       ##        ##   ##     ##    ##       ##     ##
 ##  ## ## ##    ##    ######   ########  ########  ##     ## ########     ##     ######     ########  ######   ##       ##     ##    ##    ######   ##     ##
 ##  ##  ####    ##    ##       ##   ##   ##   ##   ##     ## ##           ##          ##    ##   ##   ##       ##       #########    ##    ##       ##     ##
 ##  ##   ###    ##    ##       ##    ##  ##    ##  ##     ## ##           ##    ##    ##    ##    ##  ##       ##       ##     ##    ##    ##       ##     ##
#### ##    ##    ##    ######## ##     ## ##     ##  #######  ##           ##     ######     ##     ## ######## ######## ##     ##    ##    ######## ########
*/
// =======================================================================

//------------------------------------------
// INTERRUPTS SETUP SECTION
//------------------------------------------

// 0x09h KEYBOARD
#define KEYBOARD_INT__			0x09
#define TIME_KEEPER_INT__		0x1C
#define INT_SER_PORT_0__    0x0C  // port 0 interrupt com 1 & 3
#define INT_SER_PORT_1__    0x0B  // port 0 interrupt com 2 & 4

/**
SET CALLBACK TO DOS SPECIFIED INTERRUPT
*/
void _dos_setvect(int interrupt_id, void* fn)
{
	switch(interrupt_id)
	{
	case KEYBOARD_INT__:
		{ // LOCK START
			AUTOLOCK(emuKernel);
			KEYBOARD_INTERRUPT_FUNC = (void (*)(void))fn;
		} // LOCK END
		break;
	case TIME_KEEPER_INT__:
		{ // LOCK START
			AUTOLOCK(emuKernel);
			TIMER_INTERRUPT_FUNC = (void (*)(void))fn;
		} // LOCK END
		break;
	case INT_SER_PORT_0__:
	case INT_SER_PORT_1__:
		{ // LOCK START
			AUTOLOCK(emuKernel);
			SERIAL_PORT0_INTERRUPT_FUNC = (void (*)(void))fn;
		} // LOCK END
		break;
	}
}

// =======================================================================

//------------------------------------------
// INTERRUPTS CALLS SECTION
//------------------------------------------

// 16h KEYBOARD
#define KEYBOARD_INT_16h__			0x16
#define KEYBOARD_INT_16h_AH_01		0x01 // function 1: is a key ready?
#define KEYBOARD_INT_16h_AH_00		0x00 // function 0: get the scan code

// 10h VIDEOBIOS
#define VIDEOBIOS_INT__			0x10
#define VGA_VIDEOMODE_13h__		0x13
#define VIDEOBIOS_FUNC_SETPALETTE__	0x1012

// 33h MOUSE
// mouse sub-function calls
#define MOUSE_INT__                0x33 //mouse interrupt number
#define MOUSE_RESET__              0x00 // reset the mouse
#define MOUSE_SHOW__               0x01 // show the mouse
#define MOUSE_HIDE__               0x02 // hide the mouse
#define MOUSE_BUTT_POS__           0x03 // get buttons and postion
#define MOUSE_SET_SENSITIVITY__    0x1A // set the sensitivity of mouse 0-100
#define MOUSE_MOTION_REL__         0x0B // query motion counters to compute
// defines to make reading buttons easier
#define MOUSE_LEFT_BUTTON__        0x01 // left button mask
#define MOUSE_RIGHT_BUTTON__       0x02 // right button mask
#define MOUSE_CENTER_BUTTON__      0x04 // center button mask


int int86(int interrupt_id, union REGS *r, union REGS *r_out)
{
	struct SREGS s;
	return int86x(interrupt_id, r, r_out, &s);
}
int int86x(int interrupt_id, union REGS *r, union REGS *r_out, struct SREGS * s)
{
	switch(interrupt_id)
	{
	case VIDEOBIOS_INT__:
		{
			switch(r->w.ax)
			{
			case VGA_VIDEOMODE_13h__:
				_setvideomode(_MRES256COLOR);
				break;
			case VIDEOBIOS_FUNC_SETPALETTE__:
				{
#ifdef _SIZE_T_DEFINED
					BYTE* raw_palette = (BYTE*)r->x._dx;
#else
					// THIS IS NOT WORKING UNDER 64 BIT COMPILERS
					BYTE* raw_palette = (BYTE*)MK_FP(s->es, r->x.dx);
#endif

					_setpalette(raw_palette);
				}
				break;
			}
		}
		break;

	case KEYBOARD_INT_16h__:
		{
			switch(r->h.ah)
			{
			case KEYBOARD_INT_16h_AH_01:
				r_out->w.cflag = (PRESSED_RAW_DOS_KEY == 0) ? I386_FLAG_ZF : 0;
				break;
			case KEYBOARD_INT_16h_AH_00:
				r->h.ah = PRESSED_RAW_DOS_KEY;
				break;
			}
		}
		break;

	case MOUSE_INT__:
		{
			switch(r->h.al)
			{
			case MOUSE_SHOW__:
				ShowCursor(TRUE);
				break;
			case MOUSE_HIDE__:
				ShowCursor(FALSE);//hides the cursor
				break;
			case MOUSE_BUTT_POS__:
			{
				AUTOLOCK(*vgaScreenPrimary);

				POINT p;
				memset(&p, 0, sizeof(p));

				if(vgaScreenPrimary->m_screenHWND && GetCursorPos(&p))
				{
					//cursor position now in p.x and p.y
					if (ScreenToClient(vgaScreenPrimary->m_screenHWND, &p))
					{
						//p.x and p.y are now relative to hwnd's client area
						if(vgaScreenPrimary->m_VGA_RENDER_state & RENDER_SCALE_VGA_SCREEN)
						{
							p.x = p.x / vgaScreenPrimary->m_scale;
							p.y = p.y / vgaScreenPrimary->m_scale;
						}
					}
				}

				if(p.x < 0 || p.x > vgaScreenPrimary->m_cx) p.x = 0;
				if(p.y < 0 || p.y > vgaScreenPrimary->m_cy) p.y = 0;

				r_out->w.cx = p.x;
				r_out->w.dx = p.y;

				r_out->w.bx = 0;
				if(GetAsyncKeyState(VK_LBUTTON)) r_out->w.bx	|= MOUSE_LEFT_BUTTON__;
				if(GetAsyncKeyState(VK_RBUTTON)) r_out->w.bx	|= MOUSE_RIGHT_BUTTON__;

			}
			break;
			}
		}
		break;
	}

	return 0;
}
void segread(struct SREGS * )
{
}

// =======================================================================

//------------------------------------------
// <bios.h>
//------------------------------------------
unsigned short _bios_keybrd(unsigned __cmd)
{
	switch(__cmd)
	{
	case _KEYBRD_READY:
	case _KEYBRD_READ:
		{ // LOCK START
			AUTOLOCK(emuKernel);
			return PRESSED_RAW_DOS_KEY;
		} // LOCK END
		break;
	case _KEYBRD_SHIFTSTATUS:
		{

#define SHIFT_R__             0x0001
#define SHIFT_L__             0x0002
#define CTRL__                0x0004
#define ALT__                 0x0008

			short key_status = 0;
			if(GetAsyncKeyState(VK_SHIFT) & 0x8000) key_status		|= (SHIFT_R__ | SHIFT_L__);
			if(GetAsyncKeyState(VK_MENU) & 0x8000) key_status		|= ALT__;
			if(GetAsyncKeyState(VK_CONTROL) & 0x8000) key_status	|= CTRL__;
			return key_status;
		}
		break;
	default:
		return 0;
	}
}


// =======================================================================

//------------------------------------------
// SOME HELPERS
//------------------------------------------

//------------------------------------------

long c_clock(void)
{
	return clock();
}

unsigned long file_length(BYTE* path)
{
    FILE *f;
    f = fopen((char*)path, "rb");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
	return len;
}

//------------------------------------------

void Set_Mode(int mode)
{
	switch(mode)
	{
	case 0x13:
		vgaScreenPrimary->_setvideomode(_MRES256COLOR);
		break;
	}
}

// =======================================================================
/*
 ######  ########    ###    ########  ######## ##     ## ########      ######  ########  ######  ######## ####  #######  ##    ##
##    ##    ##      ## ##   ##     ##    ##    ##     ## ##     ##    ##    ## ##       ##    ##    ##     ##  ##     ## ###   ##
##          ##     ##   ##  ##     ##    ##    ##     ## ##     ##    ##       ##       ##          ##     ##  ##     ## ####  ##
 ######     ##    ##     ## ########     ##    ##     ## ########      ######  ######   ##          ##     ##  ##     ## ## ## ##
      ##    ##    ######### ##   ##      ##    ##     ## ##                 ## ##       ##          ##     ##  ##     ## ##  ####
##    ##    ##    ##     ## ##    ##     ##    ##     ## ##           ##    ## ##       ##    ##    ##     ##  ##     ## ##   ###
 ######     ##    ##     ## ##     ##    ##     #######  ##            ######  ########  ######     ##    ####  #######  ##    ##
*/
// =======================================================================

//------------------------------------------
// JUST TEST
//------------------------------------------
void main22(void)
{
	// put the computer into graphics mode
	_setvideomode(_VRES16COLOR); //  640x480 in 16 colors

	// draw a simple polygon
	_setcolor(1);  // blue
	_moveto(100,100); // vertex 1
	_lineto(120,120);   // vertex 2

	_lineto(150,200);   // vertex 3
	_lineto(80,190);    // vertex 4
	_lineto(80,60);      // vertex 5
	_lineto(100,100);   // back to vertex 1 to close up the polygon

	// now highlight each vertex in white

	_setcolor(15);  // white
	_setpixel(100,100); // vertex 1
	_setpixel(120,120); // vertex 2
	_setpixel(150,200); // vertex 3
	_setpixel(80,190);  // vertex 4
	_setpixel(80,60);   // vertex 5


	// wait for the user to hit a key
	while(!kbhit()){}

	// place the computer back into text mode

	_setvideomode(_DEFAULTMODE);

} // end main
	//------------------------------------------

// =======================================================================

int main(int argc, char* argv[])
{
	MAIN_ARGC = argc;
	MAIN_ARGV = argv;

	// FO PREDEFINED TABLES AND SET GLOBALS
	WINKEY_DOSKEY = create_map_winkey_doskey();
	MEMORY_0xF000FA6EL = VGA_FONT_8X8;

	// CREATE DISPLAY AND SET RENDER OPTIONS
	vgaScreenPrimary = new VGAScreen();
	_set_render_options(TRUE, RENDER_CORRECT_PALETTE);

	// START THREADS
	emuKernel.m_threads[0] = Thread((void*)TimerThread, NULL, FALSE);
	emuKernel.m_threads[1] = Thread((void*)ScreenThread, vgaScreenPrimary, FALSE);

	// WAIT ALL THREADS INITED
	while(!emuKernel.IsStop(100))
	{
		{ // LOCK START
			AUTOLOCK(emuKernel);
			if(emuKernel.m_nInitedThreads == NUM_THREADS) break;
		} // LOCK END
	}

	//------------------------------------------
	// INTEGRIRY CHECK TESTS
	//------------------------------------------
	/*
	DIBSurface* image = LoadPCX(".\\320x200.PCX");

	vgaScreenPrimary->_setvideomode(_MRES256COLOR);
	{
		AUTOLOCK(*vgaScreenPrimary);
		vgaScreenPrimary->m_screen->free();
		vgaScreenPrimary->m_screen->m_surfaceBI = image->m_surfaceBI;
		vgaScreenPrimary->m_screen->m_surfaceBytes = image->m_surfaceBytes;
		vgaScreenPrimary->_setpalette(image->m_surfacePalette);
		vgaScreenPrimary->InitBasicGraphRoutines();
	}
	image->zero();
	delete image;
	vgaScreenPrimary->RedrawScreen();
	*/
	//------------------------------------------

	//------------------------------------------
	// DO THIS THREAD LOOP
	// while(!emuKernel.IsStop(200)) {};
	// START EMULATED PROGRAM main()
	main2();
	//------------------------------------------

	//------------------------------------------
	// STOP THREADS
	//------------------------------------------
	// SIGNAL FOR THREADS STOP
	emuKernel.m_stop_event.SetBool(TRUE);
	// WAIT THREADS STOP AND KILL NOT STOPPED THREADS
	emuKernel.StopServer(1000);
	//------------------------------------------

	//------------------------------------------
	// FREE RESOURCES
	//------------------------------------------
	delete vgaScreenPrimary;

	// FreeConsole();
	//------------------------------------------

	return 0;
}


// =======================================================================


DIBSurface* LoadPCX( LPCSTR lpcszFilename )
{
   // Standard PCX header
   struct PCXHead {
     char   ID;
     char   Version;
     char   Encoding;
     char   BitPerPixel;
     short  X1;
     short  Y1;
     short  X2;
     short  Y2;
     short  HRes;
     short  VRes;
     char   ClrMap[16*3];
     char   Reserved1;
     char   NumPlanes;
     short  BPL;
     short  Pal_t;
     char   Filler[58];
   } sHeader;

   // Open the file and put its entire content in memory
   FILE *pFile = fopen( lpcszFilename, "rb" );
   if ( !pFile )
   {
      //MessageBox("Unable to open the PCX file");
      return NULL;
   }
   const long clFileSize = _filelength(_fileno(pFile));
   BYTE *pabFileData = (BYTE *)new BYTE[ clFileSize ];
   fread( pabFileData, clFileSize, 1, pFile );
   fclose( pFile );

   // Get the header
   memcpy( &sHeader, pabFileData, sizeof(sHeader) );

   // Each scan line MUST have a size that can be divided by a 'long' data type
   int iScanLineSize = sHeader.NumPlanes * sHeader.BPL;
   ldiv_t sDivResult = ldiv( iScanLineSize, sizeof(long) );
   if ( sDivResult.rem > 0 )
      iScanLineSize = (iScanLineSize/sizeof(long)+1) * sizeof(long);

   // Set the bitmap size data member
   long cx = sHeader.X2-sHeader.X1+1, cy = sHeader.Y2-sHeader.Y1+1;
   const long clImageSize = iScanLineSize * cy;

   DIBSurface* image = new DIBSurface();
   image->m_surfaceBI = NewBITMAPINFOHEADER(cx, cy, sHeader.BitPerPixel, sHeader.NumPlanes);
   // Prepare a buffer large enough to hold the image
   image->m_surfaceBytes = (BYTE *)new BYTE[ clImageSize ];

   if ( !image->m_surfaceBytes )
   {
      //MessageBox( "Can't allocate memory for the image" );
      delete [] pabFileData;
	  delete image;
      return NULL;
   }

   // Get the compressed image
   long lDataPos = 0;
   long lPos = 128;     // That's where the data begins

   int iY = 0;
   int iX = 0;
   for ( iY=0; iY < cy; iY++ )
   {
      // Decompress the scan line
      for ( iX=0; iX < sHeader.BPL; )
      {
         UINT uiValue = pabFileData[lPos++];
         if ( uiValue > 192 ) {  // Two high bits are set = Repeat
            uiValue -= 192;                  // Repeat how many times?
            BYTE Color = pabFileData[lPos++];  // What color?

            if ( iX <= cx )
            {  // Image data.  Place in the raw bitmap.
               for ( BYTE bRepeat=0; bRepeat < uiValue; bRepeat++ )
               {
                  image->m_surfaceBytes[lDataPos++] = Color;
                  iX++;
               }
            }
            else
               iX += uiValue; // Outside the image.  Skip.
         }
         else
         {
            if ( iX <= cx )
               image->m_surfaceBytes[lDataPos++] = uiValue;
            iX++;
         }
      }

      // Pad the rest with zeros
      if ( iX < iScanLineSize )
      {
         for ( ;iX < iScanLineSize; iX++ )
            image->m_surfaceBytes[lDataPos++] = 0;
      }
   }

   if ( pabFileData[lPos++] == 12 )          // Simple validation
      // Get the palette
	  image->set_palette_for_surface(pabFileData + lPos);

   delete [] pabFileData;

   return image;
}

// =======================================================================
/*
########    ###    ########  ##       ########  ######
   ##      ## ##   ##     ## ##       ##       ##    ##
   ##     ##   ##  ##     ## ##       ##       ##
   ##    ##     ## ########  ##       ######    ######
   ##    ######### ##     ## ##       ##             ##
   ##    ##     ## ##     ## ##       ##       ##    ##
   ##    ##     ## ########  ######## ########  ######
*/
// =======================================================================

/**
NATIVE DOS PALETTE
*/
BYTE VGA_PALETTE[]=
{
  0x00,0x00,0x00,	0x00,0x00,0x2a,	0x00,0x2a,0x00,	0x00,0x2a,0x2a,	0x2a,0x00,0x00,	0x2a,0x00,0x2a,	0x2a,0x15,0x00,	0x2a,0x2a,0x2a,
  0x15,0x15,0x15,	0x15,0x15,0x3f,	0x15,0x3f,0x15,	0x15,0x3f,0x3f,	0x3f,0x15,0x15,	0x3f,0x15,0x3f,	0x3f,0x3f,0x15,	0x3f,0x3f,0x3f,
  0x00,0x00,0x00,	0x05,0x05,0x05,	0x08,0x08,0x08,	0x0b,0x0b,0x0b,	0x0e,0x0e,0x0e,	0x11,0x11,0x11,	0x14,0x14,0x14,	0x18,0x18,0x18,
  0x1c,0x1c,0x1c,	0x20,0x20,0x20,	0x24,0x24,0x24,	0x28,0x28,0x28,	0x2d,0x2d,0x2d,	0x32,0x32,0x32,	0x38,0x38,0x38,	0x3f,0x3f,0x3f,
  0x00,0x00,0x3f,	0x10,0x00,0x3f,	0x1f,0x00,0x3f,	0x2f,0x00,0x3f,	0x3f,0x00,0x3f,	0x3f,0x00,0x2f,	0x3f,0x00,0x1f,	0x3f,0x00,0x10,
  0x3f,0x00,0x00,	0x3f,0x10,0x00,	0x3f,0x1f,0x00,	0x3f,0x2f,0x00,	0x3f,0x3f,0x00,	0x2f,0x3f,0x00,	0x1f,0x3f,0x00,	0x10,0x3f,0x00,
  0x00,0x3f,0x00,	0x00,0x3f,0x10,	0x00,0x3f,0x1f,	0x00,0x3f,0x2f,	0x00,0x3f,0x3f,	0x00,0x2f,0x3f,	0x00,0x1f,0x3f,	0x00,0x10,0x3f,
  0x1f,0x1f,0x3f,	0x27,0x1f,0x3f,	0x2f,0x1f,0x3f,	0x37,0x1f,0x3f,	0x3f,0x1f,0x3f,	0x3f,0x1f,0x37,	0x3f,0x1f,0x2f,	0x3f,0x1f,0x27,

  0x3f,0x1f,0x1f,	0x3f,0x27,0x1f,	0x3f,0x2f,0x1f,	0x3f,0x37,0x1f,	0x3f,0x3f,0x1f,	0x37,0x3f,0x1f,	0x2f,0x3f,0x1f,	0x27,0x3f,0x1f,
  0x1f,0x3f,0x1f,	0x1f,0x3f,0x27,	0x1f,0x3f,0x2f,	0x1f,0x3f,0x37,	0x1f,0x3f,0x3f,	0x1f,0x37,0x3f,	0x1f,0x2f,0x3f,	0x1f,0x27,0x3f,
  0x2d,0x2d,0x3f,	0x31,0x2d,0x3f,	0x36,0x2d,0x3f,	0x3a,0x2d,0x3f,	0x3f,0x2d,0x3f,	0x3f,0x2d,0x3a,	0x3f,0x2d,0x36,	0x3f,0x2d,0x31,
  0x3f,0x2d,0x2d,	0x3f,0x31,0x2d,	0x3f,0x36,0x2d,	0x3f,0x3a,0x2d,	0x3f,0x3f,0x2d,	0x3a,0x3f,0x2d,	0x36,0x3f,0x2d,	0x31,0x3f,0x2d,
  0x2d,0x3f,0x2d,	0x2d,0x3f,0x31,	0x2d,0x3f,0x36,	0x2d,0x3f,0x3a,	0x2d,0x3f,0x3f,	0x2d,0x3a,0x3f,	0x2d,0x36,0x3f,	0x2d,0x31,0x3f,
  0x00,0x00,0x1c,	0x07,0x00,0x1c,	0x0e,0x00,0x1c,	0x15,0x00,0x1c,	0x1c,0x00,0x1c,	0x1c,0x00,0x15,	0x1c,0x00,0x0e,	0x1c,0x00,0x07,
  0x1c,0x00,0x00,	0x1c,0x07,0x00,	0x1c,0x0e,0x00,	0x1c,0x15,0x00,	0x1c,0x1c,0x00,	0x15,0x1c,0x00,	0x0e,0x1c,0x00,	0x07,0x1c,0x00,
  0x00,0x1c,0x00,	0x00,0x1c,0x07,	0x00,0x1c,0x0e,	0x00,0x1c,0x15,	0x00,0x1c,0x1c,	0x00,0x15,0x1c,	0x00,0x0e,0x1c,	0x00,0x07,0x1c,

  0x0e,0x0e,0x1c,	0x11,0x0e,0x1c,	0x15,0x0e,0x1c,	0x18,0x0e,0x1c,	0x1c,0x0e,0x1c,	0x1c,0x0e,0x18,	0x1c,0x0e,0x15,	0x1c,0x0e,0x11,
  0x1c,0x0e,0x0e,	0x1c,0x11,0x0e,	0x1c,0x15,0x0e,	0x1c,0x18,0x0e,	0x1c,0x1c,0x0e,	0x18,0x1c,0x0e,	0x15,0x1c,0x0e,	0x11,0x1c,0x0e,
  0x0e,0x1c,0x0e,	0x0e,0x1c,0x11,	0x0e,0x1c,0x15,	0x0e,0x1c,0x18,	0x0e,0x1c,0x1c,	0x0e,0x18,0x1c,	0x0e,0x15,0x1c,	0x0e,0x11,0x1c,
  0x14,0x14,0x1c,	0x16,0x14,0x1c,	0x18,0x14,0x1c,	0x1a,0x14,0x1c,	0x1c,0x14,0x1c,	0x1c,0x14,0x1a,	0x1c,0x14,0x18,	0x1c,0x14,0x16,
  0x1c,0x14,0x14,	0x1c,0x16,0x14,	0x1c,0x18,0x14,	0x1c,0x1a,0x14,	0x1c,0x1c,0x14,	0x1a,0x1c,0x14,	0x18,0x1c,0x14,	0x16,0x1c,0x14,
  0x14,0x1c,0x14,	0x14,0x1c,0x16,	0x14,0x1c,0x18,	0x14,0x1c,0x1a,	0x14,0x1c,0x1c,	0x14,0x1a,0x1c,	0x14,0x18,0x1c,	0x14,0x16,0x1c,
  0x00,0x00,0x10,	0x04,0x00,0x10,	0x08,0x00,0x10,	0x0c,0x00,0x10,	0x10,0x00,0x10,	0x10,0x00,0x0c,	0x10,0x00,0x08,	0x10,0x00,0x04,
  0x10,0x00,0x00,	0x10,0x04,0x00,	0x10,0x08,0x00,	0x10,0x0c,0x00,	0x10,0x10,0x00,	0x0c,0x10,0x00,	0x08,0x10,0x00,	0x04,0x10,0x00,

  0x00,0x10,0x00,	0x00,0x10,0x04,	0x00,0x10,0x08,	0x00,0x10,0x0c,	0x00,0x10,0x10,	0x00,0x0c,0x10,	0x00,0x08,0x10,	0x00,0x04,0x10,
  0x08,0x08,0x10,	0x0a,0x08,0x10,	0x0c,0x08,0x10,	0x0e,0x08,0x10,	0x10,0x08,0x10,	0x10,0x08,0x0e,	0x10,0x08,0x0c,	0x10,0x08,0x0a,
  0x10,0x08,0x08,	0x10,0x0a,0x08,	0x10,0x0c,0x08,	0x10,0x0e,0x08,	0x10,0x10,0x08,	0x0e,0x10,0x08,	0x0c,0x10,0x08,	0x0a,0x10,0x08,
  0x08,0x10,0x08,	0x08,0x10,0x0a,	0x08,0x10,0x0c,	0x08,0x10,0x0e,	0x08,0x10,0x10,	0x08,0x0e,0x10,	0x08,0x0c,0x10,	0x08,0x0a,0x10,
  0x0b,0x0b,0x10,	0x0c,0x0b,0x10,	0x0d,0x0b,0x10,	0x0f,0x0b,0x10,	0x10,0x0b,0x10,	0x10,0x0b,0x0f,	0x10,0x0b,0x0d,	0x10,0x0b,0x0c,
  0x10,0x0b,0x0b,	0x10,0x0c,0x0b,	0x10,0x0d,0x0b,	0x10,0x0f,0x0b,	0x10,0x10,0x0b,	0x0f,0x10,0x0b,	0x0d,0x10,0x0b,	0x0c,0x10,0x0b,
  0x0b,0x10,0x0b,	0x0b,0x10,0x0c,	0x0b,0x10,0x0d,	0x0b,0x10,0x0f,	0x0b,0x10,0x10,	0x0b,0x0f,0x10,	0x0b,0x0d,0x10,	0x0b,0x0c,0x10,
  // MY CODELINE
  0x0b,0x10,0x0b,	0x0b,0x10,0x0c,	0x0b,0x10,0x0d,	0x0b,0x10,0x0f,	0x0b,0x10,0x10,	0x0b,0x0f,0x10,	0x0b,0x0d,0x10,	0x0b,0x0c,0x10,
};


/**
// GENERATED BY vgapal.c UTILITY COLOR BRIGHT PALETTE
https://github.com/canidlogic/vgapal
*/
BYTE VGA_PALETTE1[]=
{
  0,   0,   0,    0,   0, 170,    0, 170,   0,    0, 170, 170,
170,   0,   0,  170,   0, 170,  170,  85,   0,  170, 170, 170,
 85,  85,  85,   85,  85, 255,   85, 255,  85,   85, 255, 255,
255,  85,  85,  255,  85, 255,  255, 255,  85,  255, 255, 255,
  0,   0,   0,   20,  20,  20,   32,  32,  32,   44,  44,  44,
 56,  56,  56,   69,  69,  69,   81,  81,  81,   97,  97,  97,
113, 113, 113,  130, 130, 130,  146, 146, 146,  162, 162, 162,
182, 182, 182,  203, 203, 203,  227, 227, 227,  255, 255, 255,
  0,   0, 255,   65,   0, 255,  125,   0, 255,  190,   0, 255,
255,   0, 255,  255,   0, 190,  255,   0, 125,  255,   0,  65,
255,   0,   0,  255,  65,   0,  255, 125,   0,  255, 190,   0,
255, 255,   0,  190, 255,   0,  125, 255,   0,   65, 255,   0,
  0, 255,   0,    0, 255,  65,    0, 255, 125,    0, 255, 190,
  0, 255, 255,    0, 190, 255,    0, 125, 255,    0,  65, 255,
125, 125, 255,  158, 125, 255,  190, 125, 255,  223, 125, 255,
255, 125, 255,  255, 125, 223,  255, 125, 190,  255, 125, 158,
255, 125, 125,  255, 158, 125,  255, 190, 125,  255, 223, 125,
255, 255, 125,  223, 255, 125,  190, 255, 125,  158, 255, 125,
125, 255, 125,  125, 255, 158,  125, 255, 190,  125, 255, 223,
125, 255, 255,  125, 223, 255,  125, 190, 255,  125, 158, 255,
182, 182, 255,  199, 182, 255,  219, 182, 255,  235, 182, 255,
255, 182, 255,  255, 182, 235,  255, 182, 219,  255, 182, 199,
255, 182, 182,  255, 199, 182,  255, 219, 182,  255, 235, 182,
255, 255, 182,  235, 255, 182,  219, 255, 182,  199, 255, 182,
182, 255, 182,  182, 255, 199,  182, 255, 219,  182, 255, 235,
182, 255, 255,  182, 235, 255,  182, 219, 255,  182, 199, 255,
  0,   0, 113,   28,   0, 113,   56,   0, 113,   85,   0, 113,
113,   0, 113,  113,   0,  85,  113,   0,  56,  113,   0,  28,
113,   0,   0,  113,  28,   0,  113,  56,   0,  113,  85,   0,
113, 113,   0,   85, 113,   0,   56, 113,   0,   28, 113,   0,
  0, 113,   0,    0, 113,  28,    0, 113,  56,    0, 113,  85,
  0, 113, 113,    0,  85, 113,    0,  56, 113,    0,  28, 113,
 56,  56, 113,   69,  56, 113,   85,  56, 113,   97,  56, 113,
113,  56, 113,  113,  56,  97,  113,  56,  85,  113,  56,  69,
113,  56,  56,  113,  69,  56,  113,  85,  56,  113,  97,  56,
113, 113,  56,   97, 113,  56,   85, 113,  56,   69, 113,  56,
 56, 113,  56,   56, 113,  69,   56, 113,  85,   56, 113,  97,
 56, 113, 113,   56,  97, 113,   56,  85, 113,   56,  69, 113,
 81,  81, 113,   89,  81, 113,   97,  81, 113,  105,  81, 113,
113,  81, 113,  113,  81, 105,  113,  81,  97,  113,  81,  89,
113,  81,  81,  113,  89,  81,  113,  97,  81,  113, 105,  81,
113, 113,  81,  105, 113,  81,   97, 113,  81,   89, 113,  81,
 81, 113,  81,   81, 113,  89,   81, 113,  97,   81, 113, 105,
 81, 113, 113,   81, 105, 113,   81,  97, 113,   81,  89, 113,
  0,   0,  65,   16,   0,  65,   32,   0,  65,   48,   0,  65,
 65,   0,  65,   65,   0,  48,   65,   0,  32,   65,   0,  16,
 65,   0,   0,   65,  16,   0,   65,  32,   0,   65,  48,   0,
 65,  65,   0,   48,  65,   0,   32,  65,   0,   16,  65,   0,
  0,  65,   0,    0,  65,  16,    0,  65,  32,    0,  65,  48,
  0,  65,  65,    0,  48,  65,    0,  32,  65,    0,  16,  65,
 32,  32,  65,   40,  32,  65,   48,  32,  65,   56,  32,  65,
 65,  32,  65,   65,  32,  56,   65,  32,  48,   65,  32,  40,
 65,  32,  32,   65,  40,  32,   65,  48,  32,   65,  56,  32,
 65,  65,  32,   56,  65,  32,   48,  65,  32,   40,  65,  32,
 32,  65,  32,   32,  65,  40,   32,  65,  48,   32,  65,  56,
 32,  65,  65,   32,  56,  65,   32,  48,  65,   32,  40,  65,
 44,  44,  65,   48,  44,  65,   52,  44,  65,   60,  44,  65,
 65,  44,  65,   65,  44,  60,   65,  44,  52,   65,  44,  48,
 65,  44,  44,   65,  48,  44,   65,  52,  44,   65,  60,  44,
 65,  65,  44,   60,  65,  44,   52,  65,  44,   48,  65,  44,
 44,  65,  44,   44,  65,  48,   44,  65,  52,   44,  65,  60,
 44,  65,  65,   44,  60,  65,   44,  52,  65,   44,  48,  65,
  0,   0,   0,    0,   0,   0,    0,   0,   0,    0,   0,   0,
  0,   0,   0,    0,   0,   0,    0,   0,   0,    0,   0,   0
};

// =======================================================================

/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

SDL_test_font.c
https://www.libsdl.org/
*/

BYTE VGA_FONT_8X8[8*256] = {

    /*
    * 0 0x00 '^@'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 1 0x01 '^A'
    */
    0x7e,           /* 01111110 */
    0x81,           /* 10000001 */
    0xa5,           /* 10100101 */
    0x81,           /* 10000001 */
    0xbd,           /* 10111101 */
    0x99,           /* 10011001 */
    0x81,           /* 10000001 */
    0x7e,           /* 01111110 */

    /*
    * 2 0x02 '^B'
    */
    0x7e,           /* 01111110 */
    0xff,           /* 11111111 */
    0xdb,           /* 11011011 */
    0xff,           /* 11111111 */
    0xc3,           /* 11000011 */
    0xe7,           /* 11100111 */
    0xff,           /* 11111111 */
    0x7e,           /* 01111110 */

    /*
    * 3 0x03 '^C'
    */
    0x6c,           /* 01101100 */
    0xfe,           /* 11111110 */
    0xfe,           /* 11111110 */
    0xfe,           /* 11111110 */
    0x7c,           /* 01111100 */
    0x38,           /* 00111000 */
    0x10,           /* 00010000 */
    0x00,           /* 00000000 */

    /*
    * 4 0x04 '^D'
    */
    0x10,           /* 00010000 */
    0x38,           /* 00111000 */
    0x7c,           /* 01111100 */
    0xfe,           /* 11111110 */
    0x7c,           /* 01111100 */
    0x38,           /* 00111000 */
    0x10,           /* 00010000 */
    0x00,           /* 00000000 */

    /*
    * 5 0x05 '^E'
    */
    0x38,           /* 00111000 */
    0x7c,           /* 01111100 */
    0x38,           /* 00111000 */
    0xfe,           /* 11111110 */
    0xfe,           /* 11111110 */
    0xd6,           /* 11010110 */
    0x10,           /* 00010000 */
    0x38,           /* 00111000 */

    /*
    * 6 0x06 '^F'
    */
    0x10,           /* 00010000 */
    0x38,           /* 00111000 */
    0x7c,           /* 01111100 */
    0xfe,           /* 11111110 */
    0xfe,           /* 11111110 */
    0x7c,           /* 01111100 */
    0x10,           /* 00010000 */
    0x38,           /* 00111000 */

    /*
    * 7 0x07 '^G'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 8 0x08 '^H'
    */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xe7,           /* 11100111 */
    0xc3,           /* 11000011 */
    0xc3,           /* 11000011 */
    0xe7,           /* 11100111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */

    /*
    * 9 0x09 '^I'
    */
    0x00,           /* 00000000 */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x42,           /* 01000010 */
    0x42,           /* 01000010 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 10 0x0a '^J'
    */
    0xff,           /* 11111111 */
    0xc3,           /* 11000011 */
    0x99,           /* 10011001 */
    0xbd,           /* 10111101 */
    0xbd,           /* 10111101 */
    0x99,           /* 10011001 */
    0xc3,           /* 11000011 */
    0xff,           /* 11111111 */

    /*
    * 11 0x0b '^K'
    */
    0x0f,           /* 00001111 */
    0x07,           /* 00000111 */
    0x0f,           /* 00001111 */
    0x7d,           /* 01111101 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x78,           /* 01111000 */

    /*
    * 12 0x0c '^L'
    */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */

    /*
    * 13 0x0d '^M'
    */
    0x3f,           /* 00111111 */
    0x33,           /* 00110011 */
    0x3f,           /* 00111111 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x70,           /* 01110000 */
    0xf0,           /* 11110000 */
    0xe0,           /* 11100000 */

    /*
    * 14 0x0e '^N'
    */
    0x7f,           /* 01111111 */
    0x63,           /* 01100011 */
    0x7f,           /* 01111111 */
    0x63,           /* 01100011 */
    0x63,           /* 01100011 */
    0x67,           /* 01100111 */
    0xe6,           /* 11100110 */
    0xc0,           /* 11000000 */

    /*
    * 15 0x0f '^O'
    */
    0x18,           /* 00011000 */
    0xdb,           /* 11011011 */
    0x3c,           /* 00111100 */
    0xe7,           /* 11100111 */
    0xe7,           /* 11100111 */
    0x3c,           /* 00111100 */
    0xdb,           /* 11011011 */
    0x18,           /* 00011000 */

    /*
    * 16 0x10 '^P'
    */
    0x80,           /* 10000000 */
    0xe0,           /* 11100000 */
    0xf8,           /* 11111000 */
    0xfe,           /* 11111110 */
    0xf8,           /* 11111000 */
    0xe0,           /* 11100000 */
    0x80,           /* 10000000 */
    0x00,           /* 00000000 */

    /*
    * 17 0x11 '^Q'
    */
    0x02,           /* 00000010 */
    0x0e,           /* 00001110 */
    0x3e,           /* 00111110 */
    0xfe,           /* 11111110 */
    0x3e,           /* 00111110 */
    0x0e,           /* 00001110 */
    0x02,           /* 00000010 */
    0x00,           /* 00000000 */

    /*
    * 18 0x12 '^R'
    */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */

    /*
    * 19 0x13 '^S'
    */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x00,           /* 00000000 */
    0x66,           /* 01100110 */
    0x00,           /* 00000000 */

    /*
    * 20 0x14 '^T'
    */
    0x7f,           /* 01111111 */
    0xdb,           /* 11011011 */
    0xdb,           /* 11011011 */
    0x7b,           /* 01111011 */
    0x1b,           /* 00011011 */
    0x1b,           /* 00011011 */
    0x1b,           /* 00011011 */
    0x00,           /* 00000000 */

    /*
    * 21 0x15 '^U'
    */
    0x3e,           /* 00111110 */
    0x61,           /* 01100001 */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x86,           /* 10000110 */
    0x7c,           /* 01111100 */

    /*
    * 22 0x16 '^V'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x7e,           /* 01111110 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 23 0x17 '^W'
    */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0xff,           /* 11111111 */

    /*
    * 24 0x18 '^X'
    */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 25 0x19 '^Y'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 26 0x1a '^Z'
    */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0xfe,           /* 11111110 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 27 0x1b '^['
    */
    0x00,           /* 00000000 */
    0x30,           /* 00110000 */
    0x60,           /* 01100000 */
    0xfe,           /* 11111110 */
    0x60,           /* 01100000 */
    0x30,           /* 00110000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 28 0x1c '^\'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 29 0x1d '^]'
    */
    0x00,           /* 00000000 */
    0x24,           /* 00100100 */
    0x66,           /* 01100110 */
    0xff,           /* 11111111 */
    0x66,           /* 01100110 */
    0x24,           /* 00100100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 30 0x1e '^^'
    */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x7e,           /* 01111110 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 31 0x1f '^_'
    */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0x7e,           /* 01111110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 32 0x20 ' '
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 33 0x21 '!'
    */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 34 0x22 '"'
    */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x24,           /* 00100100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 35 0x23 '#'
    */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0xfe,           /* 11111110 */
    0x6c,           /* 01101100 */
    0xfe,           /* 11111110 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x00,           /* 00000000 */

    /*
    * 36 0x24 '$'
    */
    0x18,           /* 00011000 */
    0x3e,           /* 00111110 */
    0x60,           /* 01100000 */
    0x3c,           /* 00111100 */
    0x06,           /* 00000110 */
    0x7c,           /* 01111100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 37 0x25 '%'
    */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0xcc,           /* 11001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x66,           /* 01100110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 38 0x26 '&'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 39 0x27 '''
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 40 0x28 '('
    */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x00,           /* 00000000 */

    /*
    * 41 0x29 ')'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x00,           /* 00000000 */

    /*
    * 42 0x2a '*'
    */
    0x00,           /* 00000000 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0xff,           /* 11111111 */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 43 0x2b '+'
    */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 44 0x2c ','
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */

    /*
    * 45 0x2d '-'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 46 0x2e '.'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 47 0x2f '/'
    */
    0x06,           /* 00000110 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x60,           /* 01100000 */
    0xc0,           /* 11000000 */
    0x80,           /* 10000000 */
    0x00,           /* 00000000 */

    /*
    * 48 0x30 '0'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xd6,           /* 11010110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */

    /*
    * 49 0x31 '1'
    */
    0x18,           /* 00011000 */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 50 0x32 '2'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0x06,           /* 00000110 */
    0x1c,           /* 00011100 */
    0x30,           /* 00110000 */
    0x66,           /* 01100110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 51 0x33 '3'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0x06,           /* 00000110 */
    0x3c,           /* 00111100 */
    0x06,           /* 00000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 52 0x34 '4'
    */
    0x1c,           /* 00011100 */
    0x3c,           /* 00111100 */
    0x6c,           /* 01101100 */
    0xcc,           /* 11001100 */
    0xfe,           /* 11111110 */
    0x0c,           /* 00001100 */
    0x1e,           /* 00011110 */
    0x00,           /* 00000000 */

    /*
    * 53 0x35 '5'
    */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xfc,           /* 11111100 */
    0x06,           /* 00000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 54 0x36 '6'
    */
    0x38,           /* 00111000 */
    0x60,           /* 01100000 */
    0xc0,           /* 11000000 */
    0xfc,           /* 11111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 55 0x37 '7'
    */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x00,           /* 00000000 */

    /*
    * 56 0x38 '8'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 57 0x39 '9'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7e,           /* 01111110 */
    0x06,           /* 00000110 */
    0x0c,           /* 00001100 */
    0x78,           /* 01111000 */
    0x00,           /* 00000000 */

    /*
    * 58 0x3a ':'
    */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 59 0x3b ';'
    */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */

    /*
    * 60 0x3c '<'
    */
    0x06,           /* 00000110 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x06,           /* 00000110 */
    0x00,           /* 00000000 */

    /*
    * 61 0x3d '='
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 62 0x3e '>'
    */
    0x60,           /* 01100000 */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x60,           /* 01100000 */
    0x00,           /* 00000000 */

    /*
    * 63 0x3f '?'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 64 0x40 '@'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xde,           /* 11011110 */
    0xde,           /* 11011110 */
    0xde,           /* 11011110 */
    0xc0,           /* 11000000 */
    0x78,           /* 01111000 */
    0x00,           /* 00000000 */

    /*
    * 65 0x41 'A'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 66 0x42 'B'
    */
    0xfc,           /* 11111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x7c,           /* 01111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0xfc,           /* 11111100 */
    0x00,           /* 00000000 */

    /*
    * 67 0x43 'C'
    */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 68 0x44 'D'
    */
    0xf8,           /* 11111000 */
    0x6c,           /* 01101100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x6c,           /* 01101100 */
    0xf8,           /* 11111000 */
    0x00,           /* 00000000 */

    /*
    * 69 0x45 'E'
    */
    0xfe,           /* 11111110 */
    0x62,           /* 01100010 */
    0x68,           /* 01101000 */
    0x78,           /* 01111000 */
    0x68,           /* 01101000 */
    0x62,           /* 01100010 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 70 0x46 'F'
    */
    0xfe,           /* 11111110 */
    0x62,           /* 01100010 */
    0x68,           /* 01101000 */
    0x78,           /* 01111000 */
    0x68,           /* 01101000 */
    0x60,           /* 01100000 */
    0xf0,           /* 11110000 */
    0x00,           /* 00000000 */

    /*
    * 71 0x47 'G'
    */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xce,           /* 11001110 */
    0x66,           /* 01100110 */
    0x3a,           /* 00111010 */
    0x00,           /* 00000000 */

    /*
    * 72 0x48 'H'
    */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 73 0x49 'I'
    */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 74 0x4a 'J'
    */
    0x1e,           /* 00011110 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x78,           /* 01111000 */
    0x00,           /* 00000000 */

    /*
    * 75 0x4b 'K'
    */
    0xe6,           /* 11100110 */
    0x66,           /* 01100110 */
    0x6c,           /* 01101100 */
    0x78,           /* 01111000 */
    0x6c,           /* 01101100 */
    0x66,           /* 01100110 */
    0xe6,           /* 11100110 */
    0x00,           /* 00000000 */

    /*
    * 76 0x4c 'L'
    */
    0xf0,           /* 11110000 */
    0x60,           /* 01100000 */
    0x60,           /* 01100000 */
    0x60,           /* 01100000 */
    0x62,           /* 01100010 */
    0x66,           /* 01100110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 77 0x4d 'M'
    */
    0xc6,           /* 11000110 */
    0xee,           /* 11101110 */
    0xfe,           /* 11111110 */
    0xfe,           /* 11111110 */
    0xd6,           /* 11010110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 78 0x4e 'N'
    */
    0xc6,           /* 11000110 */
    0xe6,           /* 11100110 */
    0xf6,           /* 11110110 */
    0xde,           /* 11011110 */
    0xce,           /* 11001110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 79 0x4f 'O'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 80 0x50 'P'
    */
    0xfc,           /* 11111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x7c,           /* 01111100 */
    0x60,           /* 01100000 */
    0x60,           /* 01100000 */
    0xf0,           /* 11110000 */
    0x00,           /* 00000000 */

    /*
    * 81 0x51 'Q'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xce,           /* 11001110 */
    0x7c,           /* 01111100 */
    0x0e,           /* 00001110 */

    /*
    * 82 0x52 'R'
    */
    0xfc,           /* 11111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x7c,           /* 01111100 */
    0x6c,           /* 01101100 */
    0x66,           /* 01100110 */
    0xe6,           /* 11100110 */
    0x00,           /* 00000000 */

    /*
    * 83 0x53 'S'
    */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 84 0x54 'T'
    */
    0x7e,           /* 01111110 */
    0x7e,           /* 01111110 */
    0x5a,           /* 01011010 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 85 0x55 'U'
    */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 86 0x56 'V'
    */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */

    /*
    * 87 0x57 'W'
    */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xd6,           /* 11010110 */
    0xd6,           /* 11010110 */
    0xfe,           /* 11111110 */
    0x6c,           /* 01101100 */
    0x00,           /* 00000000 */

    /*
    * 88 0x58 'X'
    */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 89 0x59 'Y'
    */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 90 0x5a 'Z'
    */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0x8c,           /* 10001100 */
    0x18,           /* 00011000 */
    0x32,           /* 00110010 */
    0x66,           /* 01100110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 91 0x5b '['
    */
    0x3c,           /* 00111100 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 92 0x5c '\'
    */
    0xc0,           /* 11000000 */
    0x60,           /* 01100000 */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x06,           /* 00000110 */
    0x02,           /* 00000010 */
    0x00,           /* 00000000 */

    /*
    * 93 0x5d ']'
    */
    0x3c,           /* 00111100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 94 0x5e '^'
    */
    0x10,           /* 00010000 */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 95 0x5f '_'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */

    /*
    * 96 0x60 '`'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 97 0x61 'a'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 98 0x62 'b'
    */
    0xe0,           /* 11100000 */
    0x60,           /* 01100000 */
    0x7c,           /* 01111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0xdc,           /* 11011100 */
    0x00,           /* 00000000 */

    /*
    * 99 0x63 'c'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc0,           /* 11000000 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 100 0x64 'd'
    */
    0x1c,           /* 00011100 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 101 0x65 'e'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 102 0x66 'f'
    */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x60,           /* 01100000 */
    0xf8,           /* 11111000 */
    0x60,           /* 01100000 */
    0x60,           /* 01100000 */
    0xf0,           /* 11110000 */
    0x00,           /* 00000000 */

    /*
    * 103 0x67 'g'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x76,           /* 01110110 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x7c,           /* 01111100 */
    0x0c,           /* 00001100 */
    0xf8,           /* 11111000 */

    /*
    * 104 0x68 'h'
    */
    0xe0,           /* 11100000 */
    0x60,           /* 01100000 */
    0x6c,           /* 01101100 */
    0x76,           /* 01110110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0xe6,           /* 11100110 */
    0x00,           /* 00000000 */

    /*
    * 105 0x69 'i'
    */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 106 0x6a 'j'
    */
    0x06,           /* 00000110 */
    0x00,           /* 00000000 */
    0x06,           /* 00000110 */
    0x06,           /* 00000110 */
    0x06,           /* 00000110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */

    /*
    * 107 0x6b 'k'
    */
    0xe0,           /* 11100000 */
    0x60,           /* 01100000 */
    0x66,           /* 01100110 */
    0x6c,           /* 01101100 */
    0x78,           /* 01111000 */
    0x6c,           /* 01101100 */
    0xe6,           /* 11100110 */
    0x00,           /* 00000000 */

    /*
    * 108 0x6c 'l'
    */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 109 0x6d 'm'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xec,           /* 11101100 */
    0xfe,           /* 11111110 */
    0xd6,           /* 11010110 */
    0xd6,           /* 11010110 */
    0xd6,           /* 11010110 */
    0x00,           /* 00000000 */

    /*
    * 110 0x6e 'n'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xdc,           /* 11011100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x00,           /* 00000000 */

    /*
    * 111 0x6f 'o'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 112 0x70 'p'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xdc,           /* 11011100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x7c,           /* 01111100 */
    0x60,           /* 01100000 */
    0xf0,           /* 11110000 */

    /*
    * 113 0x71 'q'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x76,           /* 01110110 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x7c,           /* 01111100 */
    0x0c,           /* 00001100 */
    0x1e,           /* 00011110 */

    /*
    * 114 0x72 'r'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xdc,           /* 11011100 */
    0x76,           /* 01110110 */
    0x60,           /* 01100000 */
    0x60,           /* 01100000 */
    0xf0,           /* 11110000 */
    0x00,           /* 00000000 */

    /*
    * 115 0x73 's'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0xc0,           /* 11000000 */
    0x7c,           /* 01111100 */
    0x06,           /* 00000110 */
    0xfc,           /* 11111100 */
    0x00,           /* 00000000 */

    /*
    * 116 0x74 't'
    */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0xfc,           /* 11111100 */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x36,           /* 00110110 */
    0x1c,           /* 00011100 */
    0x00,           /* 00000000 */

    /*
    * 117 0x75 'u'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 118 0x76 'v'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */

    /*
    * 119 0x77 'w'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0xd6,           /* 11010110 */
    0xd6,           /* 11010110 */
    0xfe,           /* 11111110 */
    0x6c,           /* 01101100 */
    0x00,           /* 00000000 */

    /*
    * 120 0x78 'x'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 121 0x79 'y'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7e,           /* 01111110 */
    0x06,           /* 00000110 */
    0xfc,           /* 11111100 */

    /*
    * 122 0x7a 'z'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x4c,           /* 01001100 */
    0x18,           /* 00011000 */
    0x32,           /* 00110010 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 123 0x7b '{'
    */
    0x0e,           /* 00001110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x70,           /* 01110000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x0e,           /* 00001110 */
    0x00,           /* 00000000 */

    /*
    * 124 0x7c '|'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 125 0x7d '}'
    */
    0x70,           /* 01110000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x0e,           /* 00001110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x70,           /* 01110000 */
    0x00,           /* 00000000 */

    /*
    * 126 0x7e '~'
    */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 127 0x7f ''
    */
    0x00,           /* 00000000 */
    0x10,           /* 00010000 */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 128 0x80 '?'
    */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x0c,           /* 00001100 */
    0x78,           /* 01111000 */

    /*
    * 129 0x81 '?'
    */
    0xcc,           /* 11001100 */
    0x00,           /* 00000000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 130 0x82 '‚'
    */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 131 0x83 '?'
    */
    0x7c,           /* 01111100 */
    0x82,           /* 10000010 */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 132 0x84 '„'
    */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 133 0x85 '…'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 134 0x86 '†'
    */
    0x30,           /* 00110000 */
    0x30,           /* 00110000 */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 135 0x87 '‡'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0x7e,           /* 01111110 */
    0x0c,           /* 00001100 */
    0x38,           /* 00111000 */

    /*
    * 136 0x88 '€'
    */
    0x7c,           /* 01111100 */
    0x82,           /* 10000010 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 137 0x89 '‰'
    */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 138 0x8a '?'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 139 0x8b '‹'
    */
    0x66,           /* 01100110 */
    0x00,           /* 00000000 */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 140 0x8c '?'
    */
    0x7c,           /* 01111100 */
    0x82,           /* 10000010 */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 141 0x8d '?'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 142 0x8e '?'
    */
    0xc6,           /* 11000110 */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 143 0x8f '?'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 144 0x90 '?'
    */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0xf8,           /* 11111000 */
    0xc0,           /* 11000000 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 145 0x91 '‘'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0xd8,           /* 11011000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 146 0x92 '’'
    */
    0x3e,           /* 00111110 */
    0x6c,           /* 01101100 */
    0xcc,           /* 11001100 */
    0xfe,           /* 11111110 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xce,           /* 11001110 */
    0x00,           /* 00000000 */

    /*
    * 147 0x93 '“'
    */
    0x7c,           /* 01111100 */
    0x82,           /* 10000010 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 148 0x94 '”'
    */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 149 0x95 '•'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 150 0x96 '–'
    */
    0x78,           /* 01111000 */
    0x84,           /* 10000100 */
    0x00,           /* 00000000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 151 0x97 '—'
    */
    0x60,           /* 01100000 */
    0x30,           /* 00110000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 152 0x98 '?'
    */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7e,           /* 01111110 */
    0x06,           /* 00000110 */
    0xfc,           /* 11111100 */

    /*
    * 153 0x99 '™'
    */
    0xc6,           /* 11000110 */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */

    /*
    * 154 0x9a '?'
    */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 155 0x9b '›'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 156 0x9c '?'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0x64,           /* 01100100 */
    0xf0,           /* 11110000 */
    0x60,           /* 01100000 */
    0x66,           /* 01100110 */
    0xfc,           /* 11111100 */
    0x00,           /* 00000000 */

    /*
    * 157 0x9d '?'
    */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 158 0x9e '?'
    */
    0xf8,           /* 11111000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xfa,           /* 11111010 */
    0xc6,           /* 11000110 */
    0xcf,           /* 11001111 */
    0xc6,           /* 11000110 */
    0xc7,           /* 11000111 */

    /*
    * 159 0x9f '?'
    */
    0x0e,           /* 00001110 */
    0x1b,           /* 00011011 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0xd8,           /* 11011000 */
    0x70,           /* 01110000 */
    0x00,           /* 00000000 */

    /*
    * 160 0xa0 ' '
    */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x7c,           /* 01111100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 161 0xa1 '?'
    */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x38,           /* 00111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 162 0xa2 '?'
    */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */

    /*
    * 163 0xa3 '?'
    */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 164 0xa4 '¤'
    */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0x00,           /* 00000000 */
    0xdc,           /* 11011100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x00,           /* 00000000 */

    /*
    * 165 0xa5 '?'
    */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0x00,           /* 00000000 */
    0xe6,           /* 11100110 */
    0xf6,           /* 11110110 */
    0xde,           /* 11011110 */
    0xce,           /* 11001110 */
    0x00,           /* 00000000 */

    /*
    * 166 0xa6 '¦'
    */
    0x3c,           /* 00111100 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x3e,           /* 00111110 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 167 0xa7 '§'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 168 0xa8 '?'
    */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x63,           /* 01100011 */
    0x3e,           /* 00111110 */
    0x00,           /* 00000000 */

    /*
    * 169 0xa9 '©'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 170 0xaa '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x06,           /* 00000110 */
    0x06,           /* 00000110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 171 0xab '«'
    */
    0x63,           /* 01100011 */
    0xe6,           /* 11100110 */
    0x6c,           /* 01101100 */
    0x7e,           /* 01111110 */
    0x33,           /* 00110011 */
    0x66,           /* 01100110 */
    0xcc,           /* 11001100 */
    0x0f,           /* 00001111 */

    /*
    * 172 0xac '¬'
    */
    0x63,           /* 01100011 */
    0xe6,           /* 11100110 */
    0x6c,           /* 01101100 */
    0x7a,           /* 01111010 */
    0x36,           /* 00110110 */
    0x6a,           /* 01101010 */
    0xdf,           /* 11011111 */
    0x06,           /* 00000110 */

    /*
    * 173 0xad '­'
    */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 174 0xae '®'
    */
    0x00,           /* 00000000 */
    0x33,           /* 00110011 */
    0x66,           /* 01100110 */
    0xcc,           /* 11001100 */
    0x66,           /* 01100110 */
    0x33,           /* 00110011 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 175 0xaf '?'
    */
    0x00,           /* 00000000 */
    0xcc,           /* 11001100 */
    0x66,           /* 01100110 */
    0x33,           /* 00110011 */
    0x66,           /* 01100110 */
    0xcc,           /* 11001100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 176 0xb0 '°'
    */
    0x22,           /* 00100010 */
    0x88,           /* 10001000 */
    0x22,           /* 00100010 */
    0x88,           /* 10001000 */
    0x22,           /* 00100010 */
    0x88,           /* 10001000 */
    0x22,           /* 00100010 */
    0x88,           /* 10001000 */

    /*
    * 177 0xb1 '±'
    */
    0x55,           /* 01010101 */
    0xaa,           /* 10101010 */
    0x55,           /* 01010101 */
    0xaa,           /* 10101010 */
    0x55,           /* 01010101 */
    0xaa,           /* 10101010 */
    0x55,           /* 01010101 */
    0xaa,           /* 10101010 */

    /*
    * 178 0xb2 '?'
    */
    0x77,           /* 01110111 */
    0xdd,           /* 11011101 */
    0x77,           /* 01110111 */
    0xdd,           /* 11011101 */
    0x77,           /* 01110111 */
    0xdd,           /* 11011101 */
    0x77,           /* 01110111 */
    0xdd,           /* 11011101 */

    /*
    * 179 0xb3 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 180 0xb4 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 181 0xb5 'µ'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 182 0xb6 '¶'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xf6,           /* 11110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 183 0xb7 '·'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 184 0xb8 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 185 0xb9 '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xf6,           /* 11110110 */
    0x06,           /* 00000110 */
    0xf6,           /* 11110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 186 0xba '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 187 0xbb '»'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x06,           /* 00000110 */
    0xf6,           /* 11110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 188 0xbc '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xf6,           /* 11110110 */
    0x06,           /* 00000110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 189 0xbd '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 190 0xbe '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 191 0xbf '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xf8,           /* 11111000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 192 0xc0 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 193 0xc1 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 194 0xc2 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 195 0xc3 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 196 0xc4 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 197 0xc5 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xff,           /* 11111111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 198 0xc6 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 199 0xc7 '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x37,           /* 00110111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 200 0xc8 '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x37,           /* 00110111 */
    0x30,           /* 00110000 */
    0x3f,           /* 00111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 201 0xc9 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x3f,           /* 00111111 */
    0x30,           /* 00110000 */
    0x37,           /* 00110111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 202 0xca '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xf7,           /* 11110111 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 203 0xcb '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0xf7,           /* 11110111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 204 0xcc '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x37,           /* 00110111 */
    0x30,           /* 00110000 */
    0x37,           /* 00110111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 205 0xcd '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 206 0xce '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xf7,           /* 11110111 */
    0x00,           /* 00000000 */
    0xf7,           /* 11110111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 207 0xcf '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 208 0xd0 '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 209 0xd1 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 210 0xd2 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 211 0xd3 '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x3f,           /* 00111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 212 0xd4 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 213 0xd5 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 214 0xd6 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x3f,           /* 00111111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 215 0xd7 '?'
    */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0xff,           /* 11111111 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */

    /*
    * 216 0xd8 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xff,           /* 11111111 */
    0x18,           /* 00011000 */
    0xff,           /* 11111111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 217 0xd9 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xf8,           /* 11111000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 218 0xda '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x1f,           /* 00011111 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 219 0xdb '?'
    */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */

    /*
    * 220 0xdc '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */

    /*
    * 221 0xdd '?'
    */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */
    0xf0,           /* 11110000 */

    /*
    * 222 0xde '?'
    */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */
    0x0f,           /* 00001111 */

    /*
    * 223 0xdf '?'
    */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0xff,           /* 11111111 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 224 0xe0 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0xc8,           /* 11001000 */
    0xdc,           /* 11011100 */
    0x76,           /* 01110110 */
    0x00,           /* 00000000 */

    /*
    * 225 0xe1 '?'
    */
    0x78,           /* 01111000 */
    0xcc,           /* 11001100 */
    0xcc,           /* 11001100 */
    0xd8,           /* 11011000 */
    0xcc,           /* 11001100 */
    0xc6,           /* 11000110 */
    0xcc,           /* 11001100 */
    0x00,           /* 00000000 */

    /*
    * 226 0xe2 '?'
    */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0xc0,           /* 11000000 */
    0x00,           /* 00000000 */

    /*
    * 227 0xe3 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x00,           /* 00000000 */

    /*
    * 228 0xe4 '?'
    */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0x60,           /* 01100000 */
    0x30,           /* 00110000 */
    0x60,           /* 01100000 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */

    /*
    * 229 0xe5 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0xd8,           /* 11011000 */
    0xd8,           /* 11011000 */
    0xd8,           /* 11011000 */
    0x70,           /* 01110000 */
    0x00,           /* 00000000 */

    /*
    * 230 0xe6 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x7c,           /* 01111100 */
    0xc0,           /* 11000000 */

    /*
    * 231 0xe7 '?'
    */
    0x00,           /* 00000000 */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */

    /*
    * 232 0xe8 '?'
    */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x3c,           /* 00111100 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */

    /*
    * 233 0xe9 '?'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xfe,           /* 11111110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */

    /*
    * 234 0xea '?'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0xee,           /* 11101110 */
    0x00,           /* 00000000 */

    /*
    * 235 0xeb '?'
    */
    0x0e,           /* 00001110 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x3e,           /* 00111110 */
    0x66,           /* 01100110 */
    0x66,           /* 01100110 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */

    /*
    * 236 0xec '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0xdb,           /* 11011011 */
    0xdb,           /* 11011011 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 237 0xed '?'
    */
    0x06,           /* 00000110 */
    0x0c,           /* 00001100 */
    0x7e,           /* 01111110 */
    0xdb,           /* 11011011 */
    0xdb,           /* 11011011 */
    0x7e,           /* 01111110 */
    0x60,           /* 01100000 */
    0xc0,           /* 11000000 */

    /*
    * 238 0xee '?'
    */
    0x1e,           /* 00011110 */
    0x30,           /* 00110000 */
    0x60,           /* 01100000 */
    0x7e,           /* 01111110 */
    0x60,           /* 01100000 */
    0x30,           /* 00110000 */
    0x1e,           /* 00011110 */
    0x00,           /* 00000000 */

    /*
    * 239 0xef '?'
    */
    0x00,           /* 00000000 */
    0x7c,           /* 01111100 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0xc6,           /* 11000110 */
    0x00,           /* 00000000 */

    /*
    * 240 0xf0 '?'
    */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */
    0xfe,           /* 11111110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 241 0xf1 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x7e,           /* 01111110 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 242 0xf2 '?'
    */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 243 0xf3 '?'
    */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x18,           /* 00011000 */
    0x0c,           /* 00001100 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */

    /*
    * 244 0xf4 '?'
    */
    0x0e,           /* 00001110 */
    0x1b,           /* 00011011 */
    0x1b,           /* 00011011 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */

    /*
    * 245 0xf5 '?'
    */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0xd8,           /* 11011000 */
    0xd8,           /* 11011000 */
    0x70,           /* 01110000 */

    /*
    * 246 0xf6 '?'
    */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x7e,           /* 01111110 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 247 0xf7 '?'
    */
    0x00,           /* 00000000 */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0x00,           /* 00000000 */
    0x76,           /* 01110110 */
    0xdc,           /* 11011100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 248 0xf8 '?'
    */
    0x38,           /* 00111000 */
    0x6c,           /* 01101100 */
    0x6c,           /* 01101100 */
    0x38,           /* 00111000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 249 0xf9 '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 250 0xfa '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x18,           /* 00011000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 251 0xfb '?'
    */
    0x0f,           /* 00001111 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0x0c,           /* 00001100 */
    0xec,           /* 11101100 */
    0x6c,           /* 01101100 */
    0x3c,           /* 00111100 */
    0x1c,           /* 00011100 */

    /*
    * 252 0xfc '?'
    */
    0x6c,           /* 01101100 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x36,           /* 00110110 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 253 0xfd '?'
    */
    0x78,           /* 01111000 */
    0x0c,           /* 00001100 */
    0x18,           /* 00011000 */
    0x30,           /* 00110000 */
    0x7c,           /* 01111100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 254 0xfe '?'
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x3c,           /* 00111100 */
    0x3c,           /* 00111100 */
    0x3c,           /* 00111100 */
    0x3c,           /* 00111100 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

    /*
    * 255 0xff ' '
    */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */
    0x00,           /* 00000000 */

};

BYTE* create_map_winkey_doskey()
{
	BYTE* M = new BYTE[WIN_VK_KEYS_TOTAL];
	memset(M, WIN_VK_UNDEFINED, WIN_VK_KEYS_TOTAL);

	M[WIN_VK_ESCAPE]		=	DOS_VK_ESCAPE;
	M[WIN_VK_1]		=	DOS_VK_1;
	M[WIN_VK_2]		=	DOS_VK_2;
	M[WIN_VK_3]		=	DOS_VK_3;
	M[WIN_VK_4]		=	DOS_VK_4;
	M[WIN_VK_5]		=	DOS_VK_5;
	M[WIN_VK_6]		=	DOS_VK_6;
	M[WIN_VK_7]		=	DOS_VK_7;
	M[WIN_VK_8]		=	DOS_VK_8;
	M[WIN_VK_9]		=	DOS_VK_9;
	M[WIN_VK_0]		=	DOS_VK_0;
	M[0xBD]		=	DOS_VK_SUBTRACT_1;
	M[0xBB]		=	DOS_VK_EQUAL;
	M[WIN_VK_BACK]		=	DOS_VK_BACK;
	M[WIN_VK_TAB]		=	DOS_VK_TAB;
	M[WIN_VK_Q]		=	DOS_VK_Q;
	M[WIN_VK_W]		=	DOS_VK_W;
	M[WIN_VK_E]		=	DOS_VK_E;
	M[WIN_VK_R]		=	DOS_VK_R;
	M[WIN_VK_T]		=	DOS_VK_T;
	M[WIN_VK_Y]		=	DOS_VK_Y;
	M[WIN_VK_U]		=	DOS_VK_U;
	M[WIN_VK_I]		=	DOS_VK_I;
	M[WIN_VK_O]		=	DOS_VK_O;
	M[WIN_VK_P]		=	DOS_VK_P;
	M[WIN_VK_A]		=	DOS_VK_LBRACKET;
	M[WIN_VK_S]		=	DOS_VK_RBRACKET;
	M[WIN_VK_D]		=	DOS_VK_ENTER;
	M[WIN_VK_F]		=	DOS_VK_LCONTROL;
	M[WIN_VK_G]		=	DOS_VK_A;
	M[WIN_VK_H]		=	DOS_VK_S;
	M[WIN_VK_J]		=	DOS_VK_D;
	M[WIN_VK_K]		=	DOS_VK_F;
	M[WIN_VK_L]		=	DOS_VK_G;
	M[WIN_VK_H]		=	DOS_VK_H;
	M[WIN_VK_J]		=	DOS_VK_J;
	M[WIN_VK_K]		=	DOS_VK_K;
	M[WIN_VK_L]		=	DOS_VK_L;
	//M[]		=	DOS_VK_DOTCOMMA;
	//M[]		=	DOS_VK_APOSTROPHE;
	//M[]		=	DOS_VK_QUOTE3;
	M[WIN_VK_LSHIFT]		=	DOS_VK_LSHIFT;
	//M[]		=	DOS_VK_STICK1;
	M[WIN_VK_Z]		=	DOS_VK_Z;
	M[WIN_VK_X]		=	DOS_VK_X;
	M[WIN_VK_C]		=	DOS_VK_C;
	M[WIN_VK_V]		=	DOS_VK_V;
	M[WIN_VK_B]		=	DOS_VK_B;
	M[WIN_VK_N]		=	DOS_VK_N;
	M[WIN_VK_M]		=	DOS_VK_M;
	M[0xBC]		=	DOS_VK_COMMA;
	M[0xBE]		=	DOS_VK_DOT;
	//M[]		=	DOS_VK_STICK2;
	M[WIN_VK_RSHIFT]		=	DOS_VK_RSHIFT;
	M[WIN_VK_MULTIPLY]		=	DOS_VK_MULTIPLY;
	//M[]		=	DOS_VK_LALT;
	M[WIN_VK_SPACE]		=	DOS_VK_SPACE;
	//M[]		=	DOS_VK_CAPS;
	M[WIN_VK_F1]		=	DOS_VK_F1;
	M[WIN_VK_F2]		=	DOS_VK_F2;
	M[WIN_VK_F3]		=	DOS_VK_F3;
	M[WIN_VK_F4]		=	DOS_VK_F4;
	M[WIN_VK_F5]		=	DOS_VK_F5;
	M[WIN_VK_F6]		=	DOS_VK_F6;
	M[WIN_VK_F7]		=	DOS_VK_F7;
	M[WIN_VK_F8]		=	DOS_VK_F8;
	M[WIN_VK_F9]		=	DOS_VK_F9;
	M[WIN_VK_F10]		=	DOS_VK_F10;
	M[WIN_VK_NUMLOCK]		=	DOS_VK_NUMLOCK;
	M[WIN_VK_SCROLL]		=	DOS_VK_SCROLL;

	M[WIN_VK_NUMPAD7]		=	DOS_VK_NUMPAD7;
	M[WIN_VK_NUMPAD8]		=	DOS_VK_NUMPAD8;
	M[WIN_VK_NUMPAD9]		=	DOS_VK_NUMPAD9;
	//M[]		=	DOS_VK_SUBTRACTG;
	M[WIN_VK_NUMPAD4]		=	DOS_VK_NUMPAD4;
	M[WIN_VK_NUMPAD5]		=	DOS_VK_NUMPAD5;
	M[WIN_VK_NUMPAD6]		=	DOS_VK_NUMPAD6;
	//M[]		=	DOS_VK_PLUSG;
	M[WIN_VK_NUMPAD1]		=	DOS_VK_NUMPAD1;
	M[WIN_VK_NUMPAD2]		=	DOS_VK_NUMPAD2;
	M[WIN_VK_NUMPAD3]		=	DOS_VK_NUMPAD3;
	M[WIN_VK_NUMPAD0]		=	DOS_VK_NUMPAD0;
	//M[]		=	DOS_VK_NUMPADDOT;
	M[WIN_VK_PRINT]		=	DOS_VK_PRINT;
	//M[]		=	DOS_VK_0x55;
	//M[]		=	DOS_VK_0x56;
	M[WIN_VK_F11]		=	DOS_VK_F11;
	M[WIN_VK_F12]		=	DOS_VK_F12;
	//M[]		=	DOS_VK_0x59
	//M[]		=	DOS_VK_PAUSE;
	M[WIN_VK_INSERT]		=	DOS_VK_INSERT;
	M[WIN_VK_HOME]		=	DOS_VK_HOME;
	M[WIN_VK_PRIOR]		=	DOS_VK_PRIOR;
	//M[]		=	DOS_VK_STICKG;
	M[WIN_VK_DELETE]		=	DOS_VK_DELETE;
	M[WIN_VK_END]		=	DOS_VK_END;
	M[WIN_VK_NEXT]		=	DOS_VK_NEXT;
	//M[]		=	DOS_VK_RALT;
	M[WIN_VK_RCONTROL]		=	DOS_VK_RCONTROL;
	M[WIN_VK_UP]		=	DOS_VK_UP;
	M[WIN_VK_LEFT]		=	DOS_VK_LEFT;
	M[WIN_VK_DOWN]		=	DOS_VK_DOWN;
	M[WIN_VK_RIGHT]		=	DOS_VK_RIGHT;
	// M[]	=	DOS_VK_ENTERG;

	return M;
}
