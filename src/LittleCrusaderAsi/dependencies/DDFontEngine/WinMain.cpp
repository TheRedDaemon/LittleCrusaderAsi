//---------------------------------------------------------------------------
//
//  DDFontMake - Copyright © 2000 Michael Fötsch.
//      Last Update: June 26, 2000
//
//      This program accompanies the tutorial "Fast Bitmap Fonts for
//      DirectDraw", which is available at www.mr-gamemaker.com.
//      DDFontMake creates bitmaps from TrueType, bitmap, and vector fonts.
//      These bitmaps are stored in the DDF (DirectDraw Font) format, which
//      is used by the DirectDraw font engine from that tutorial.
//      The tutorial along with a compiled version of this utility is available
//      also at www.geocities.com/foetsch.
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//---------------------------------------------------------------------------

#include <windows.h>
#pragma hdrstop
#include "Resfile.h"

#define WINDOWCLASSNAME "DDFontMakeWindow"
#define WINDOWCAPTION "DDFontMake - Copyright © 2000 Michael Fötsch"

//-------------------------- Structures/Classes -----------------------------

// enum TPenTypes - Indices into arrays Pens and PenColors
enum TPenTypes {
    ptGrid,                         // used to draw grid lines
    ptBaseline,                     // used to draw the baseline of the font
    ptAbcA, ptAbcB, ptAbcC          // used to draw ABC widths of characters
};

// array PenColors - COLORREFs for the above pen types
COLORREF PenColors[5] = {
    // dark gray:       // red:         // green:       // blue:
    RGB(128, 128, 128), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
    // magenta:
    RGB(255, 0, 255)
};

// array Pens - array that will store HPENs for the above pen types
HPEN Pens[5];


// CFontInfo
//      This class stores information about the currently loaded font.
//      This includes LOGFONT and CHOOSEFONT structures for use with the
//      ChooseFont dialog, as well as info about character dimensions.
//
class CFontInfo
{
public:
    CFontInfo() { hFont=NULL; }
    ~CFontInfo() { if (hFont) DeleteObject(hFont); }
    void InitChooseFont(HWND hwnd);

    HFONT hFont;                // HFONT used to draw with this font
    LOGFONT lf;                 // font description for CreateFontIndirect
    CHOOSEFONT cf;              // font description for ChooseFont dialog
    TEXTMETRIC tm;              // text metrics, e.g. character height
    ABC abc[224];               // character widths
};

// CFontInfo::InitChooseFont
//      Fill CHOOSEFONT cf so that it can be passed to ChooseFont
void CFontInfo::InitChooseFont(HWND hwnd)
{
    ZeroMemory(&lf, sizeof(lf));
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
}
//---------------------------------------------------------------------------

//-------------------------- Global Variables -------------------------------

HWND hWnd;
HINSTANCE hInst;
CFontInfo fi;                   // currently opened font
OPENFILENAME OpenFileName;      // used for GetSaveFileName
char SaveFileName[MAX_PATH];    // buffer for OpenFileName.lpstrFile

//---------------------------------------------------------------------------

// ErrMsgBox
//      Displays a MsgBox with the error message for GetLastError.
//      Uses FormatMessage to retrieve the message.
//      The code was taken from the FormatMessage help topic.
//
void ErrMsgBox()
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );

    // Display the string.
    MessageBox(NULL, (char*)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION);

    // Free the buffer.
    LocalFree( lpMsgBuf );
}
//---------------------------------------------------------------------------

// MeasureFont
//      Fills CFontInfo fi (global) with text metrics and char widths
//      -> hdc: HDC that the font is currently selected into
//
void MeasureFont(HDC hdc)
{
    GetTextMetrics(hdc, &fi.tm);
    // Measure TrueType fonts with GetCharABCWidths:
    if (!GetCharABCWidths(hdc, 32, 255, fi.abc))
    // If it's not a TT font, use GetTextExtentPoint32 to fill array abc:
    {
        ZeroMemory(fi.abc, 224*sizeof(ABC));    // set abcA and abcC to 0
                                                // (abcB will be overwritten)
        SIZE Size;
        // for all characters, beginning with " ":
        for (int i=32; i < 256; i++)
        {
            // get width of character...
            GetTextExtentPoint32(hdc, (char*)&i, 1, &Size);
            // ...and store it in abcB:
            fi.abc[i-32].abcB = Size.cx;
        }
    }
}
//---------------------------------------------------------------------------

// CMLoadFont
//      Menu item "File | LoadFont"
void CMLoadFont()
{
    // invoke ChooseFont common dialog:
    if (ChooseFont(&fi.cf))
    {
        // create an HFONT:
        if (fi.hFont) { DeleteObject(fi.hFont); fi.hFont = NULL; }
        fi.hFont = CreateFontIndirect(&fi.lf);

        // get HDC:
        HDC hdc = GetDC(hWnd);

        // select font:
        SelectObject(hdc, fi.hFont);

        // get text metrics and char widths:
        MeasureFont(hdc);

        // release HDC:
        ReleaseDC(hWnd, hdc);

        // redraw window:
        RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    }
}
//---------------------------------------------------------------------------

    #define GetFilePointer(x) SetFilePointer(x, 0, 0, FILE_CURRENT)
    #define ERROR_BREAK(x) { ErrMsgBox(); throw (int)(x); }

// CMSaveFont
//      Menu item "File | Save Font"
void CMSaveFont()
{
    HDC OffscrDC=NULL;
    HBITMAP OffscrBmp=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;
    HANDLE BmpFile=INVALID_HANDLE_VALUE;

try
{
    // Use fontname as filename by default:
    strcpy(OpenFileName.lpstrFile, fi.lf.lfFaceName);
    // Let user specify file to save to:
    if (!GetSaveFileName(&OpenFileName)) return;

    // Redraw window now so that the preview will not be erased later:
    RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    UpdateWindow(hWnd);

    // Create an offscreen device context:
    OffscrDC = CreateCompatibleDC(0);

    if (fi.hFont) SelectObject(OffscrDC, fi.hFont);

    // Create an offscreen bitmap:
    int width=16*fi.tm.tmMaxCharWidth;
    int height=14*fi.tm.tmHeight;
    HBITMAP OffscrBmp = CreateCompatibleBitmap(OffscrDC, width, height);
    if (!OffscrBmp) ERROR_BREAK(0);
    // Select bitmap into DC:
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    if (!OldBmp) ERROR_BREAK(1);

    // Clear background to black:
    SelectObject(OffscrDC, GetStockObject(BLACK_BRUSH));
    Rectangle(OffscrDC, 0, 0, width, height);
    SetBkMode(OffscrDC, TRANSPARENT);     // do not fill character background
    SetTextColor(OffscrDC, RGB(255, 255, 255)); // text color white

    // Draw characters:
    unsigned char c;
    for (int y=0; y < 14; y++)
    for (int x=0; x < 16; x++)
    {
        c = (unsigned char)(y*16+x+32);
        TextOut(OffscrDC, x*fi.tm.tmMaxCharWidth-fi.abc[c-32].abcA,
            y*fi.tm.tmHeight, (const char*)&c, 1);
            // *) See remark in WM_PAINT handler as to why abcA is subtracted.
    }

// Following code to store an HDC to a BMP file was taken from the tutorial
// "Using the GDI to Take DirectDraw Screenshots", which can be found at
// www.mr-gamemaker.com.
// The code is ready to store other formats than monochrome, although we
// do not make use of that yet.

    // GetDIBits requires format info about the bitmap. We can have GetDIBits
    // fill a structure with that info if we pass a NULL pointer for lpvBits:
    // Reserve memory for bitmap info (BITMAPINFOHEADER + largest possible
    // palette):
    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) +
        256 * sizeof(RGBQUAD)])) == NULL) ERROR_BREAK(4);
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // Get info but first de-select OffscrBmp because GetDIBits requires it:
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        ERROR_BREAK(5);

    // Reserve memory for bitmap bits:
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
        ERROR_BREAK(6);

    // Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi,
        DIB_RGB_COLORS)) ERROR_BREAK(7);

    // Create a file to save the DIB to:
    if ((BmpFile = CreateFile(OpenFileName.lpstrFile,
                              GENERIC_WRITE,
                              0, NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL)) == INVALID_HANDLE_VALUE) ERROR_BREAK(8);

    DWORD Written;    // number of bytes written by WriteFile

    // Write a file header to the file:
    BITMAPFILEHEADER bmfh;
    bmfh.bfType = 19778;        // 'BM'
    // bmfh.bfSize = ???        // we'll write that later
    // The bitmap format requires the Reserved fields to be 0.
    // Our format is DDF. We store ID values into these fields:
    bmfh.bfReserved1 = 'DD';    // "DirectDraw...
    bmfh.bfReserved2 = 'FF';    // "...Font File"
    // bmfh.bfOffBits = ???     // we'll write that later
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        ERROR_BREAK(9);
    if (Written < sizeof(bmfh)) ERROR_BREAK(9);

    // Write BITMAPINFOHEADER to the file:
    if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER),
        &Written, NULL)) ERROR_BREAK(10);
    if (Written < sizeof(BITMAPINFOHEADER)) ERROR_BREAK(10);

    // Calculate size of palette:
    int PalEntries;
    // 16-bit or 32-bit bitmaps require bit masks:
    if (lpbi->bmiHeader.biCompression == BI_BITFIELDS) PalEntries = 3;
    else
        // bitmap is palettized?
        PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
            // 2^biBitCount palette entries max.:
            1 << lpbi->bmiHeader.biBitCount
        // bitmap is TrueColor -> no palette:
        : 0;
    // If biClrsUsed use only biClrUsed palette entries:
    if (lpbi->bmiHeader.biClrUsed) PalEntries = lpbi->bmiHeader.biClrUsed;

    // Write palette to the file:
    if (PalEntries)
    {
        if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD),
            &Written, NULL)) ERROR_BREAK(11);
        if (Written < PalEntries * sizeof(RGBQUAD)) ERROR_BREAK(11);
    }

    // The current position in the file (at the beginning of the bitmap bits)
    // will be saved to the BITMAPFILEHEADER:
    bmfh.bfOffBits = GetFilePointer(BmpFile);

    // Write bitmap bits to the file:
    if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage,
        &Written, NULL)) ERROR_BREAK(12);
    if (Written < lpbi->bmiHeader.biSizeImage) ERROR_BREAK(12);

    // The current pos. in the file is the final file size and will be saved:
    bmfh.bfSize = GetFilePointer(BmpFile);

    // Store the font info and character widths to the file:
    if (WriteFile(BmpFile, &fi.tm, sizeof(fi.tm), &Written, NULL) &&
    WriteFile(BmpFile, fi.abc, 224*sizeof(ABC), &Written, NULL) &&
    WriteFile(BmpFile, &fi.lf, sizeof(fi.lf), &Written, NULL)
        == NULL) ERROR_BREAK(13);

    // We have all the info for the file header. Save the updated version:
    SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        ERROR_BREAK(14);
    if (Written < sizeof(bmfh)) ERROR_BREAK(14);

    // Blt preview to screen:
    HDC hdc = GetDC(hWnd);
    SelectObject(OffscrDC, OffscrBmp);
    BitBlt(hdc, 0, 0, width, height, OffscrDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hdc);
}
catch (int &errorcode)
{
    char Buf[100];
    wsprintf(Buf, "Screenshot error #%i", errorcode);
    OutputDebugString(Buf);
}
catch (...)
{
    OutputDebugString("Screenshot error");
}

    if (OffscrDC) DeleteDC(OffscrDC);
    if (OffscrBmp) DeleteObject(OffscrBmp);
    if (lpbi) delete[] lpbi;
    if (lpvBits) delete[] lpvBits;
    if (BmpFile != INVALID_HANDLE_VALUE) CloseHandle(BmpFile);
}
//---------------------------------------------------------------------------

    #undef ERROR_BREAK

// WMPaint
//      WM_PAINT handler
//      Arranges the characters in a 16x14 grid. Draws grid lines as well as
//      the baseline of the font and the ABC widths of each character.
//
void WMPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    int y;
    if (fi.hFont) SelectObject(ps.hdc, fi.hFont);

    SetBkMode(ps.hdc, TRANSPARENT);     // do not fill character background

    SelectObject(ps.hdc, Pens[ptGrid]);         // use dark gray pen for grid
    int width=16*fi.tm.tmMaxCharWidth;
    int height=14*fi.tm.tmHeight;
    for (y=0; y < 15; y++)                      // draw horizontal lines
    {
        MoveToEx(ps.hdc, 0, y*fi.tm.tmHeight, NULL);
        LineTo(ps.hdc, width, y*fi.tm.tmHeight);
    }
    for (int x=0; x < 17; x++)                  // draw vertical lines
    {
        MoveToEx(ps.hdc, x*fi.tm.tmMaxCharWidth, 0, NULL);
        LineTo(ps.hdc, x*fi.tm.tmMaxCharWidth, height);
    }

    unsigned char c;
    int left, top, bottom;

    // arrange characters in 14 rows:
    for (y=0; y < 14; y++)
    {
        top = y*fi.tm.tmHeight;
        bottom = top + fi.tm.tmHeight;

        // draw baseline with red pen:
        SelectObject(ps.hdc, Pens[ptBaseline]);
        MoveToEx(ps.hdc, 0, bottom-fi.tm.tmDescent, NULL);
        LineTo(ps.hdc, width, bottom-fi.tm.tmDescent);

        // arrange characters in 16 columns per row:
        for (int x=0; x < 16; x++)
        {
            left = x*fi.tm.tmMaxCharWidth;
            c = (unsigned char)(y*16+x+32);
            // draw char
            TextOut(ps.hdc, left-fi.abc[c-32].abcA, top,(const char*)&c, 1);
                // *) abcA is subtracted because we want our character to start
                // at the left edge of its cell regardless of the value of abcA.
                // Otherwise, when abcA was negative, part of the character
                // would be visible in the preceding cell. A similar problem
                // could occur at the right edge if abcA was too big.

            // draw A width (see docs, "ABC" and "GetCharABCWidth") using green:
            SelectObject(ps.hdc, Pens[ptAbcA]);
            MoveToEx(ps.hdc, left+fi.abc[c-32].abcA, top, NULL);
            LineTo(ps.hdc, left+fi.abc[c-32].abcA, bottom);
            // draw B width using blue:
            left += fi.abc[c-32].abcB; SelectObject(ps.hdc, Pens[ptAbcB]);
            MoveToEx(ps.hdc, left, top, NULL); LineTo(ps.hdc, left, bottom);
            // draw C width using magenta:
            left += fi.abc[c-32].abcC; SelectObject(ps.hdc, Pens[ptAbcC]);
            MoveToEx(ps.hdc, left, top, NULL); LineTo(ps.hdc, left, bottom);
        }
    }

    EndPaint(hWnd, &ps);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//-------------------------- Window Procedure -------------------------------
//---------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(
    HWND hwnd,	// handle of window
    UINT uMsg,	// message identifier
    WPARAM wParam,	// first message parameter
    LPARAM lParam 	// second message parameter
)
{
    LRESULT Result=0;
    int i;
    HDC hdc;    // used in case WM_CREATE

    switch (uMsg)
    {
    case WM_COMMAND:            // Main menu
        switch (wParam)
        {
        case CM_LOADFONT: CMLoadFont(); break;
        case CM_SAVEFONT: CMSaveFont(); break;
        case CM_EXIT: DestroyWindow(hWnd); break;
        }
        break;

    case WM_CREATE:
        // Initialize CFontInfo fi:
        fi.InitChooseFont(hWnd);
        hdc = GetDC(hWnd);
        GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(LOGFONT), &fi.lf);
        MeasureFont(hdc);
        ReleaseDC(hWnd, hdc);
        // create HPENs for the constants in TPenTypes:
        for (i=0; i < 5; i++)
            Pens[i] = CreatePen(PS_DOT, 1, PenColors[i]);
        // prepare OPENFILENAME structure for GetSaveFileName:
        ZeroMemory(&OpenFileName, sizeof(OpenFileName));
        OpenFileName.lStructSize = OPENFILENAME_SIZE_VERSION_400;   // = 76
            // (See remark at the end of this file!)
        OpenFileName.hwndOwner = hWnd;
        OpenFileName.lpstrFilter = "DirectDraw Font files (*.ddf;*.bmp)\0"
            "*.ddf;*.bmp\0";
        OpenFileName.nFilterIndex = 1;
        OpenFileName.lpstrFile = SaveFileName;
        OpenFileName.nMaxFile = MAX_PATH;
        OpenFileName.lpstrTitle = "Save as DirectDraw Font";
        OpenFileName.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
        OpenFileName.lpstrDefExt = "ddf";
        break;

    case WM_DESTROY:
        // destroy the HPENs in array Pens:
        for (i=0; i < 5; i++)
            DeleteObject(Pens[i]);
        PostQuitMessage(true); break;

    case WM_PAINT:
        WMPaint(); break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return Result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//-------------------------- WinMain ----------------------------------------
//---------------------------------------------------------------------------

#pragma argsused
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // Save params to global vars:
    hInst = hInstance;

    // Register window class:
    WNDCLASS WndClass;
    ZeroMemory(&WndClass, sizeof(WndClass));
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WindowProc;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    WndClass.lpszMenuName = "MAINMENU";
    WndClass.lpszClassName = WINDOWCLASSNAME;
    if (!RegisterClass(&WndClass)) {ErrMsgBox(); return false;}

    // Create window:
    hWnd = CreateWindow(WINDOWCLASSNAME,
                        WINDOWCAPTION,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        (HWND)NULL,
                        (HMENU)NULL,
                        hInstance,
                        NULL);
    if (!hWnd) { ErrMsgBox(); return false; }

    // Show the window immediately:
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop:
    MSG Msg;
    while (GetMessage(&Msg, (HWND)NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}
//---------------------------------------------------------------------------


/*

OPENFILENAME_SIZE_VERSION_400 Remark:
"With Windows 2000, the OPENFILENAME structure has increased to include some
additional members. However, this causes problems for applications on
previous operation systems. To use the current header files for applications
on Windows 95/98 and Windows NT4.0, either use the
#define "/D_WIN32_WINNT=0x0400" or use OPENFILENAME_SIZE_VERSION_400 for the
lStructSize member of OPENFILENAME."
(Taken from http://msdn.microsoft.com/library/default.asp?URL=/library/psdk/winui/commdlg3_7zqd.htm)

(BTW, the #define should be "#define _WIN32_WINNT 0x0400" in source code...)

In case the "File | Save" dialog does not work, try different values for
OpenFileName.lStructSize (in the WM_CREATE handler):
    OpenFileName.lStructSize = sizeof(OPENFILENAME);
    or
    OpenFileName.lStructSize = OPENFILENAME_VERSION_400;    // = 76
*/
