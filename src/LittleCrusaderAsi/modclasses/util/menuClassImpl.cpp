
#include "../bltOverlay.h"

namespace modclasses
{
  // NOTE: basically all hardcoded currently
  
  
  // MainMenu //


  void MainMenu::addChild(std::unique_ptr<MenuBase> &&menu)
  {
    subMenus->push_back(std::move(menu));
    computeStartEndVisible();
  }


  void MainMenu::move(MenuAction direction, BltOverlay &over)
  {
    size_t beforePos{ currentSelected };
    
    switch (direction)
    {
      case MenuAction::UP:
      {
        if (currentSelected > 0)
        {
          --currentSelected;
          if (currentSelected < startEndVisible.first)
          {
            --startEndVisible.first;
            --startEndVisible.second;
          }
        }
        break;
      }
      case MenuAction::DOWN:
      {
        if (currentSelected < subMenus->size() - 1)
        {
          ++currentSelected;
          if (currentSelected > startEndVisible.second)
          {
            ++startEndVisible.first;
            ++startEndVisible.second;
          }
        }
        break;
      }
      default:
        break;
    }

    if (beforePos != currentSelected)
    {
      draw(over);
    }
  }


  void MainMenu::computeStartEndVisible()
  {
    if (subMenus->size() <= 1)
    {
      startEndVisible = { 0, 0 };
      return;
    }

    size_t maxVisible{ (std::min)(subMenus->size() - 1, bigMenu ? 6u : 12u) };  // hardcoded ( maxNum + 1 (0 index) )
    startEndVisible = { 0, maxVisible };
  }


  MenuBase* MainMenu::action(BltOverlay &over)
  {
    if (subMenus->empty())
    {
      return nullptr;
    }
    
    // status of menu can stay
    MenuBase* newBase{ subMenus->at(currentSelected)->access(false, over) };
    if (!newBase)
    {
      // redraw in case the teyt changed
      draw(over);
    }

    return newBase;
  }


  MenuBase* MainMenu::back(BltOverlay &over)
  {
    if (!lastMenu)
    {
      return nullptr;
    }

    // setting status back -> should be enough
    currentSelected = 0;
    computeStartEndVisible();
    
    if (accessReact)
    {
      // ignore return
      accessReact(true, header);
    }

    return lastMenu->access(true, over);
  }


  void MainMenu::menuBoxDrawHelper(std::string& text, int32_t yPos, bool active, BltOverlay &over)
  {
    FontTypeEnum type;
    RECT* rect;
    std::pair<int32_t, int32_t> boxPos{ 10, 0 };
    
    if (bigMenu)
    {
      type = FontTypeEnum::NORMAL;
      rect = active ? &over.menuRects.bigButtonPressed : &over.menuRects.bigButton;
      boxPos.second = yPos - 30;
    }
    else
    {
      type = FontTypeEnum::SMALL_BOLD;
      rect = active ? &over.menuRects.smallButtonPressed : &over.menuRects.smallButton;
      boxPos.second = yPos - 16;
    }

    over.menuOffSurf->BltFast(boxPos.first, boxPos.second, over.compSurf, rect, DDBLTFAST_NOCOLORKEY);
    over.fntHandler.drawText(over.menuOffSurf, type, text, 150, yPos, 260, true, true, true, nullptr);
  }


  void MainMenu::draw(BltOverlay &over)
  {
    // initial background
    over.menuOffSurf->BltFast(0, 0, over.compSurf, &over.menuRects.mainMenu, DDBLTFAST_NOCOLORKEY);

    // header text
    over.fntHandler.drawText(over.menuOffSurf, FontTypeEnum::NORMAL_BOLD, header, 150, 40, 260, true, true, true, nullptr);

    // draw menus
    if (subMenus->empty())
    {
      return;
    }

    int32_t menuYMid{ bigMenu ? 100 : 86 };
    int32_t steps{ bigMenu ? 60 : 32 };

    for (size_t i = startEndVisible.first; i < startEndVisible.second + 1; i++)
    {
      menuBoxDrawHelper(subMenus->at(i)->header, menuYMid, i == currentSelected, over);
      menuYMid += steps;
    }

    if (startEndVisible.first > 0)
    {
      over.menuOffSurf->BltFast(0, 70, over.compSurf, &over.menuRects.goldArrowUp, DDBLTFAST_SRCCOLORKEY);
    }

    if (startEndVisible.second < subMenus->size() - 1)
    {
      over.menuOffSurf->BltFast(0, 430, over.compSurf, &over.menuRects.goldArrowDown, DDBLTFAST_SRCCOLORKEY);
    }
  }


  MenuBase* MainMenu::access(bool callerLeaving, BltOverlay &over)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!moveToThis)
    {
      return nullptr;
    }

    draw(over);

    return this;
  }


  MainMenu::MainMenu(std::string &&headerString, HeaderReact &&accessReactFunc, bool &&isItBigMenu)
                    : MenuBase(std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)), 
                      bigMenu(std::move(isItBigMenu)), subMenus(std::make_unique<std::vector<std::unique_ptr<MenuBase>>>()) {}


  // ChoiceInputMenu //


  void ChoiceInputMenu::move(MenuAction direction, BltOverlay &over)
  {
    size_t beforePos{ currentSelected };

    switch (direction)
    {
      case MenuAction::UP:
      {
        if (currentSelected > 0)
        {
          --currentSelected;
          if (currentSelected < startEndVisible.first)
          {
            --startEndVisible.first;
            --startEndVisible.second;
          }
        }
        break;
      }
      case MenuAction::DOWN:
      {
        if (currentSelected < choicePairs.size() - 1)
        {
          ++currentSelected;
          if (currentSelected > startEndVisible.second)
          {
            ++startEndVisible.first;
            ++startEndVisible.second;
          }
        }
        break;
      }
      default:
        break;
    }

    if (beforePos != currentSelected)
    {
      draw(over);
    }
  }


  void ChoiceInputMenu::computeStartEndVisible()
  {
    if (choicePairs.size() <= 1)
    {
      startEndVisible = { 0, 0 };
      return;
    }

    size_t maxVisible{ (std::min)(choicePairs.size() - 1, 2u) };  // hardcoded ( maxNum + 1 (0 index) )
    startEndVisible = { 0, maxVisible };
  }


  // only apply (ok) button reacts to "push"
  MenuBase* ChoiceInputMenu::action(BltOverlay &over)
  {
    if (!selectReact || choicePairs.empty())
    {
      return nullptr;
    }

    if (selectReact(choicePairs.at(currentSelected).second, resultOfEnter, false, currentValue))
    {
      currentValue = choicePairs.at(currentSelected).first;
    }

    draw(over);
    return nullptr;
  }


  MenuBase* ChoiceInputMenu::back(BltOverlay &over)
  {
    if (!lastMenu)
    {
      return nullptr;
    }

    // setting status back -> should be enough
    currentSelected = 0;
    computeStartEndVisible();
    resultOfEnter = "";
    over.inputActive = false;

    if (accessReact)
    {
      // ignore return
      accessReact(true, header);
    }

    return lastMenu->access(true, over);
  }


  void ChoiceInputMenu::menuBoxDrawHelper(std::string& text, int32_t yPos, bool active, BltOverlay &over)
  {
    FontTypeEnum type{ FontTypeEnum::SMALL };
    RECT* rect{ active ? &over.menuRects.inputFieldSelected : &over.menuRects.inputField };
    std::pair<int32_t, int32_t> boxPos{ 25, yPos - 16 };

    over.inputOffSurf->BltFast(boxPos.first, boxPos.second, over.compSurf, rect, DDBLTFAST_NOCOLORKEY);
    over.fntHandler.drawText(over.inputOffSurf, type, text, 150, yPos, 140, true, true, true, nullptr);
  }


  void ChoiceInputMenu::draw(BltOverlay &over)
  {
    // initial background
    over.inputOffSurf->BltFast(0, 0, over.compSurf, &over.menuRects.bigInputBox, DDBLTFAST_NOCOLORKEY);

    // header text
    over.fntHandler.drawText(over.inputOffSurf, FontTypeEnum::SMALL_BOLD, header, 150, 25, 280, true, true, true, nullptr);

    // default
    over.fntHandler.drawText(over.inputOffSurf, FontTypeEnum::SMALL, "Default:", 16, 55, 65, false, true, true, nullptr);
    over.fntHandler.drawText(over.inputOffSurf, FontTypeEnum::SMALL, defaultValue, 192, 55, 184, true, true, true, nullptr);

    // current
    over.fntHandler.drawText(over.inputOffSurf, FontTypeEnum::SMALL, "Current:", 16, 80, 65, false, true, true, nullptr);
    over.fntHandler.drawText(over.inputOffSurf, FontTypeEnum::SMALL, currentValue, 192, 80, 184, true, true, true, nullptr);

    // draw menus
    if (choicePairs.empty())
    {
      return;
    }

    int32_t menuYMid{ 124 };
    for (size_t i = startEndVisible.first; i < startEndVisible.second + 1; i++)
    {
      menuBoxDrawHelper(choicePairs.at(i).first, menuYMid, i == currentSelected, over);
      menuYMid += 32;
    }

    // assuming three visible max
    if (startEndVisible.first > 0)
    {
      over.inputOffSurf->BltFast(5, 118, over.compSurf, &over.menuRects.silverArrowUp, DDBLTFAST_SRCCOLORKEY);
      over.inputOffSurf->BltFast(275, 118, over.compSurf, &over.menuRects.silverArrowUp, DDBLTFAST_SRCCOLORKEY);
    }

    if (startEndVisible.second < choicePairs.size() - 1)
    {
      over.inputOffSurf->BltFast(5, 162, over.compSurf, &over.menuRects.silverArrowDown, DDBLTFAST_SRCCOLORKEY);
      over.inputOffSurf->BltFast(275, 162, over.compSurf, &over.menuRects.silverArrowDown, DDBLTFAST_SRCCOLORKEY);
    }

    // apply text
    over.fntHandler.drawText(over.inputOffSurf, FontTypeEnum::SMALL, resultOfEnter, 150, 226, 200, true, true, true, nullptr);
  }


  MenuBase* ChoiceInputMenu::access(bool callerLeaving, BltOverlay &over)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!moveToThis)
    {
      return nullptr;
    }

    if (selectReact)
    {
      selectReact(0, resultOfEnter, true, currentValue);  // update after access
    }
    else
    {
      resultOfEnter = "Missing react function.";
    }

    draw(over);
    over.inputActive = true;

    return this;
  }


  ChoiceInputMenu::ChoiceInputMenu(std::string &&headerString, HeaderReact &&accessReactFunc, std::string &&defaultValueStr,
                                   std::vector<std::pair<std::string, int32_t>> &&choicePairCon, SelectReact &&selectReactFunc)
                                  : MenuBase(std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                                    defaultValue(std::move(defaultValueStr)), choicePairs(std::move(choicePairCon)),
                                    selectReact(std::move(selectReactFunc))
  {
    computeStartEndVisible();
    if (selectReact)
    {
      selectReact(0, resultOfEnter, true, currentValue);  // update after access
    }
  }
}