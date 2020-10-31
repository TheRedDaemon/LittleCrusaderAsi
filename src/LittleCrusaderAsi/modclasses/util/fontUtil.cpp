
#include "../bltOverlay.h"

// all font loading and drawing structures were contructed using the DDFontEngine
// https://realmike.org/blog/projects/fast-bitmap-fonts-for-directdraw/
namespace modclasses
{
  bool FontHandler::loadFont(FontTypeEnum fontType, Json &fontData, const IDirectDraw7* drawObject)
  {

  }

  bool FontHandler::drawText(LPDIRECTDRAWSURFACE7 destination, FontTypeEnum fontType, const std::string &text, int32_t posX, int32_t posY,
                             int horizontalMaxLength, bool centerBoxHorizontal, bool centerVertical, bool truncate, int32_t &retUsedHorizontalSpace)
  {
    auto fntIt{ fonts.find(fontType) };
    if (fntIt == fonts.end() && fntIt->second.lpFontSurf == nullptr)
    {
      return false;
    }

    bool allOk{ true };
    FontContainer& font = fntIt->second;

    // precompute position for centering...  not the best solution to be fair
    // TODO: think of better way if possible
    int32_t writeCharMax{0};
    std::pair<int32_t, int32_t> relPos{ 0, 0 };
    if (centerBoxHorizontal || centerVertical || truncate)
    {

      if (centerVertical)
      {
        relPos.second -= font.SrcRects['a'].bottom - font.SrcRects['a'].top; // init with one line
      }

      int32_t relXPos{ 0 };
      for (unsigned char chr : text)
      {
        int32_t xPosAdd = font.ABCWidths[chr].abcA + font.BPlusC[chr];
        if (horizontalMaxLength < relXPos + xPosAdd)
        {
          if (relPos.first != -horizontalMaxLength / 2)
          {
            if (centerBoxHorizontal)
            {
              relPos.first = -horizontalMaxLength / 2;
            }

            if (truncate)
            {
              break; // we are done if the line is filled
            }
          }

          if (centerVertical)
          {
            relPos.second -= font.SrcRects['a'].bottom - font.SrcRects['a'].top; // another line
          }

          ++writeCharMax;
          relXPos = xPosAdd;
        }
      }

      if (centerVertical)
      {
        relPos.second /= 2; // init with one line
      }
    }

    posX = relPos.first;
    posY = relPos.second;

    int32_t drawnChars{ 0 };
    for (unsigned char chr : text)
    {
      if (font.ABCWidths[chr].abcA + font.BPlusC[chr] + posX >= horizontalMaxLength)
      {
        posX = relPos.first;
        posY += font.SrcRects['a'].bottom - font.SrcRects['a'].top; // another line
      }
      
      posX += font.ABCWidths[chr].abcA;
      allOk = allOk && destination->BltFast(posX, posY, font.lpFontSurf, &(font.SrcRects[chr]), DDBLTFAST_SRCCOLORKEY) == S_OK;
      posX += font.BPlusC[chr];

      ++drawnChars;
      if (drawnChars >= writeCharMax)
      {
        break;
      }
    }

    // add three points
    if (truncate)
    {
      for (size_t i = 0; i < 3; i++)
      {
        posX += font.ABCWidths['.'].abcA;
        allOk = allOk && destination->BltFast(posX, posY, font.lpFontSurf, &(font.SrcRects['.']), DDBLTFAST_SRCCOLORKEY) == S_OK;
        posX += font.BPlusC['.'];
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
}