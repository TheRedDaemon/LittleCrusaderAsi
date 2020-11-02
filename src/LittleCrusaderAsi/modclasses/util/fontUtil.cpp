
#include "../bltOverlay.h"

// all font loading and drawing structures were contructed using the DDFontEngine
// https://realmike.org/blog/projects/fast-bitmap-fonts-for-directdraw/
// most of the comments are from there, too
namespace modclasses
{
  bool FontHandler::loadFont(FontTypeEnum fontType, Json &fontData, IDirectDraw7* drawObject, DWORD textcolor)
  {
    auto fntIt{ fonts.find(fontType) };
    if (fntIt == fonts.end())
    {
      FontContainer& font = fonts[fontType];  // create
      //FontContainer font; // test
      // whole bitmap loading was taken from DDFrontEngine
      // needs a lot of error checks

      // TODO: create font configuration (and maybe look, if you can clean this up a little)
      HFONT hfont{ CreateFont(24, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman")) };
      if (hfont == NULL)
      {
        cleanInErrorCase(fontType, "Unable to get font.");
        return false;
      }

      // Create an offscreen device context:
      HDC hdc{ CreateCompatibleDC(0) };
      if (hdc == NULL)
      {
        cleanInErrorCase(fontType, "Failed creating device context.");
        DeleteObject(hfont);
        return false;
      }

      // select font:
      SelectObject(hdc, hfont);

      // get text metrics and char widths:
      TEXTMETRIC tm;              // text metrics, e.g. character height
      // Measure TrueType fonts with GetCharABCWidths:
      GetTextMetrics(hdc, &tm);
      ZeroMemory(font.ABCWidths, 256 * sizeof(ABC));    // set abcA and abcC to 0 (abcB will be overwritten)
      if (!GetCharABCWidths(hdc, 32, 255, &(font.ABCWidths[32]))) // If it's not a TT font, use GetTextExtentPoint32 to fill array abc:  
      {
        SIZE Size;
        // for all characters, beginning with " ":
        for (int i = 32; i < 256; i++)
        {
          // get width of character...
          GetTextExtentPoint32(hdc, (char*)&i, 1, &Size);
          // ...and store it in abcB:
          font.ABCWidths[i].abcB = Size.cx;
        }
      }

      // Create an offscreen bitmap:
      int width = 16 * tm.tmMaxCharWidth;
      int height = 14 * tm.tmHeight;
      HBITMAP OffscrBmp = CreateCompatibleBitmap(hdc, width, height);
      if (!OffscrBmp)
      {
        cleanInErrorCase(fontType, "Unable to create bitmap.");
        DeleteObject(hfont);
        DeleteDC(hdc);
        return false;
      }
      // Select bitmap into DC:
      HBITMAP OldBmp = (HBITMAP)SelectObject(hdc, OffscrBmp);
      if (!OldBmp)
      {
        cleanInErrorCase(fontType, "Unable to get old bitmap.");
        DeleteObject(hfont);
        DeleteObject(OffscrBmp);
        DeleteDC(hdc);
        return false;
      }

      // Clear background to black:
      SelectObject(hdc, GetStockObject(BLACK_BRUSH));
      Rectangle(hdc, 0, 0, width, height);
      SetBkMode(hdc, TRANSPARENT);           // do not fill character background
      SetTextColor(hdc, RGB(255, 255, 255)); // text color white

      // Draw characters:
      unsigned char chr;
      for (int y = 0; y < 14; y++)
      {
        for (int x = 0; x < 16; x++)
        {
          chr = (unsigned char)(y * 16 + x + 32);
          TextOut(hdc, x * tm.tmMaxCharWidth - font.ABCWidths[chr].abcA, y * tm.tmHeight, (const char*)&chr, 1);
        }
      }

      // GetDIBits requires format info about the bitmap. We can have GetDIBits
      // fill a structure with that info if we pass a NULL pointer for lpvBits:
      font.bitmapInfo = std::make_unique<char[]>(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
      LPBITMAPINFO bmInfo{ font.getBitmapInfo() };

      ZeroDDObject(bmInfo->bmiHeader);
      bmInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      // Get info but first de-select OffscrBmp because GetDIBits requires it:
      SelectObject(hdc, OldBmp);
      if (!GetDIBits(hdc, OffscrBmp, 0, height, NULL, bmInfo, DIB_RGB_COLORS))
      {
        cleanInErrorCase(fontType, "Unable to receive bitmap info.");
        DeleteObject(hfont);
        DeleteObject(OffscrBmp);
        DeleteDC(hdc);
        return false;
      }

      // Reserve memory for bitmap bits:
      font.bitmapData = std::make_unique<char[]>(bmInfo->bmiHeader.biSizeImage);

      // Have GetDIBits convert OffscrBmp to a DIB (device-independent bitmap):
      if (!GetDIBits(hdc, OffscrBmp, 0, height, font.bitmapData.get(), bmInfo, DIB_RGB_COLORS))
      {
        cleanInErrorCase(fontType, "Unable to receive bitmap data.");
        DeleteObject(hfont);
        DeleteObject(OffscrBmp);
        DeleteDC(hdc);
        return false;
      }

      // release HDC:
      DeleteObject(hfont);
      DeleteObject(OffscrBmp);
      DeleteDC(hdc);

      // Pre-calculate SrcRects:
      int32_t CellWidth = bmInfo->bmiHeader.biWidth >> 4;
      int32_t CellHeight = bmInfo->bmiHeader.biHeight / 14;

      ZeroDDObject(font.SrcRects);
      ZeroDDObject(font.BPlusC);
      for (int c = 32; c < 256; c++)
      {
        font.SrcRects[c].left = ((c - 32) % 16) * CellWidth;
        font.SrcRects[c].top = ((c - 32) >> 4) * CellHeight;
        font.SrcRects[c].right = font.SrcRects[c].left + font.ABCWidths[c].abcB;
        font.SrcRects[c].bottom = font.SrcRects[c].top + CellHeight;
        font.BPlusC[c] = font.ABCWidths[c].abcB + font.ABCWidths[c].abcC;
      }

      // which object causes problems?;

      fntIt = fonts.find(fontType);
    }

    if (fntIt->second.lpFontSurf == nullptr)
    {
      FontContainer& font = fntIt->second;

      LONG surfWidth{ font.getBitmapInfo()->bmiHeader.biWidth };
      LONG surfHeight{ font.getBitmapInfo()->bmiHeader.biHeight };

      // maybe create in video memory?
      DDSURFACEDESC2 ddsd;
      ZeroDDObjectAndSetSize(ddsd);
      ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
      ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      ddsd.dwWidth = surfWidth;
      ddsd.dwHeight = surfHeight;

      if (drawObject->CreateSurface(&ddsd, &font.lpFontSurf, NULL) != S_OK)
      {
        cleanInErrorCase(fontType, "Failed creating font surface.");
        return false;
      }

      DDCOLORKEY ddck;
      ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = 0;
      font.lpFontSurf->SetColorKey(DDCKEY_SRCBLT, &ddck);

      // From DDFontEngine:
      // Change foreground palette entry to requested text color:
      // (Assumes that entry 1 is foreground and 0 is background)
      DWORD *palentry = (DWORD*)&(font.getBitmapInfo()->bmiColors[1]);
      *palentry = textcolor;

      HDC SurfDC{ nullptr };
      if (font.lpFontSurf->GetDC(&SurfDC) != S_OK)
      {
        cleanInErrorCase(fontType, "Failed getting font surface DC.");
        return false;
      }

      int ret = StretchDIBits(SurfDC, 0, 0, surfWidth, surfHeight, 0, 0, surfWidth, surfHeight,
                              font.bitmapData.get(), font.getBitmapInfo(), DIB_RGB_COLORS, SRCCOPY);

      font.lpFontSurf->ReleaseDC(SurfDC);

      if (ret == 0)
      {
        cleanInErrorCase(fontType, "StretchDIBits failed.");
        return false;
      }
    }
    
    return true;
  }


  bool FontHandler::drawText(LPDIRECTDRAWSURFACE7 destination, FontTypeEnum fontType, const std::string &text, int32_t posX, int32_t posY,
                             int horizontalMaxLength, bool centerHorizontal, bool centerVertical, bool truncate,
                             std::function<std::pair<int32_t, int32_t>(RECT)> *reactToRelSize)
  {
    auto fntIt{ fonts.find(fontType) };
    if (fntIt == fonts.end() || fntIt->second.lpFontSurf == nullptr)
    {
      return false;
    }

    bool allOk{ true };
    FontContainer& font = fntIt->second;

    // precompute positions
    // line separation for now will only happen on a ' ' position
    // trailing whitespace is not removed -> centered stuff could be moved in strange ways
    int32_t heightOfAChar{ font.SrcRects['a'].bottom - font.SrcRects['a'].top };
    int32_t spaceWidth{ font.ABCWidths[' '].abcA + font.BPlusC[' '] };

    // only get number of chars and line size -> compute based on conditions first: number, second: length, third, add to counter, because space ignored
    std::vector<std::tuple<int32_t, int32_t, bool>> lineInfo;
    bool stillTruncate{ false };
    int32_t lastSpace{ 0 };
    int32_t beforeLastSpace{ 0 };

    size_t charNumber{ 0 };
    int32_t relXPos{ 0 };
    for (unsigned char chr : text)
    {
      if (chr == ' ')
      {
        // will contain length and number until then
        lastSpace = charNumber; // at this point it is the char position (and 0 is ignored)
        beforeLastSpace = relXPos;  // includes the space length
      }

      int32_t xPosAdd = font.ABCWidths[chr].abcA + font.BPlusC[chr];
      if (horizontalMaxLength < relXPos + xPosAdd)
      {
        if (truncate) // truncate does not care
        {
          stillTruncate = true;
          break; // we are done if the line is filled
        }

        if (horizontalMaxLength > xPosAdd) // the char is placed if alone to big for line
        {
          if (lastSpace > 0)
          {
            lineInfo.push_back({ lastSpace, beforeLastSpace, true });
            charNumber -= (lastSpace + 1);  // removing last space
            relXPos -= (beforeLastSpace + spaceWidth); // removing last space wdith
          }
          else  // if the line is filled, it cuts
          {
            lineInfo.push_back({ charNumber, relXPos, false });
            charNumber = 0;
            relXPos = 0;
          }
          beforeLastSpace = 0;
          lastSpace = 0;
        }
      }

      ++charNumber;
      relXPos += xPosAdd;
    }
    lineInfo.push_back({ charNumber, relXPos, false }); // if it run through


    // idea changed, these function now updates x and y
    if (reactToRelSize)
    {
      // compute containing box
      RECT textBox{ 0, 0, 0, 0 };
      
      for (auto& numberLengthAndSpace : lineInfo)
      {
        if (std::get<1>(numberLengthAndSpace) > textBox.right)
        {
          textBox.right = std::get<1>(numberLengthAndSpace);
        }
      }
      textBox.bottom = lineInfo.size() * heightOfAChar;

      if (centerVertical)
      {
        textBox.bottom /= 2;
        textBox.top -= textBox.bottom;
      }

      if (centerHorizontal)
      {
        textBox.right /= 2;
        textBox.left -= textBox.right;
      }
      
      textBox.left += posX;
      textBox.right += posX;
      textBox.top += posY;
      textBox.bottom += posY;

      std::pair<int32_t, int32_t> newXYpos = (*reactToRelSize)(textBox);
      posX = newXYpos.first;
      posY = newXYpos.second;
    }


    // print chars
    size_t counter{ 0 };
    for (size_t line{ 0 }; line < lineInfo.size(); line++)
    {
      auto& numberLengthAndSpace = lineInfo[line];
      int32_t x{ posX };
      int32_t y{ posY + static_cast<int32_t>(line) * heightOfAChar };

      if (centerVertical)
      {
        y -= heightOfAChar * lineInfo.size() / 2;
      }

      if (centerHorizontal)
      {
        x -= std::get<1>(numberLengthAndSpace) / 2;
      }

      for (int32_t chrNum{ 0 }; chrNum < std::get<0>(numberLengthAndSpace); chrNum++)
      {
        unsigned char chr = text[counter];
        x += font.ABCWidths[chr].abcA;
        allOk = allOk && destination->BltFast(x, y, font.lpFontSurf, &(font.SrcRects[chr]), DDBLTFAST_SRCCOLORKEY) == S_OK;
        x += font.BPlusC[chr];
        ++counter;
      }

      // ignores everything else, just places it
      if (stillTruncate)
      {
        for (size_t i = 0; i < 3; i++)
        {
          x += font.ABCWidths['.'].abcA;
          allOk = allOk && destination->BltFast(x, y, font.lpFontSurf, &(font.SrcRects['.']), DDBLTFAST_SRCCOLORKEY) == S_OK;
          x += font.BPlusC['.'];
        }
      }
      else if (std::get<2>(numberLengthAndSpace))
      {
        ++counter; // adding one, because removed space
      }
    }

    return allOk;
  }


  void FontHandler::releaseSurfaces()
  {
    for (auto& [fontType, fontContainer] : fonts)
    {
      fontContainer.lpFontSurf->Release();
      fontContainer.lpFontSurf = nullptr;
    }
  }


  void FontHandler::cleanInErrorCase(FontTypeEnum fontType, std::string &&logMsg)
  {
    auto fntIt{ fonts.find(fontType) };
    if (fntIt != fonts.end())
    {
      if (fntIt->second.lpFontSurf != nullptr)
      {
        fntIt->second.lpFontSurf->Release();
      }

      fonts.erase(fontType);
    }

    LOG(WARNING) << logMsg;
  }
}