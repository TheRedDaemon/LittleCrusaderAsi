//---------------------------------------------------------------------------
//  DDFontEngine - Copyright © 2000 Michael Fötsch.
//
//      The DirectDraw Bitmap Font Engine from "Fast Bitmap Fonts
//      for DirectDraw", which can be found at www.mr-gamemaker.com.
//      The tutorial and a TrueType-to-DDF converter are also available at
//      http://www.geocities.com/foetsch
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

#ifndef DDFontEngineH
#define DDFontEngineH

#include <ddraw.h>
//---------------------------------------------------------------------------

// TEXTCOLOR macro
//      Used to pass textcolor to TDDFont constructor and TDDFont::SetTextColor
#define TEXTCOLOR(r, g, b) ((DWORD) (((BYTE) (b) | \
    ((WORD) (g) << 8)) | \
    (((DWORD) (BYTE) (r)) << 16)))


class TDDFontEngine;

class TDDFont
{
public:
    TDDFont(TDDFontEngine *ddfe, LPCSTR fontfile,
        DWORD textcolor=TEXTCOLOR(255, 255, 255));
    ~TDDFont();

    TDDFontEngine *Ddfe;
    LPDIRECTDRAWSURFACE7 lpFontSurf;
    char *FontFile;
    LPBITMAPINFO lpbi;
    LPVOID lpBits;
    TEXTMETRIC TextMetrics;
    ABC ABCWidths[256];
    LOGFONT LogFont;
    int SurfWidth, SurfHeight;
    int CellWidth, CellHeight;
    RECT SrcRects[256];     // Pre-calculated SrcRects for Blt
    int BPlusC[256];
    DWORD TextColor;


    bool LoadFont();
    void SetTextColor(DWORD textcolor);
};
//---------------------------------------------------------------------------

class TDDFontEngine
{
public:
    TDDFontEngine(LPDIRECTDRAW7 lpdd);
    ~TDDFontEngine();

    LPDIRECTDRAW7 lpDD;
    TDDFont *Ddf;

    void SelectFont(TDDFont *ddf);
    HRESULT DrawText(LPDIRECTDRAWSURFACE7 lpDDS, int x, int y, char *Text);
};
//---------------------------------------------------------------------------

#endif
