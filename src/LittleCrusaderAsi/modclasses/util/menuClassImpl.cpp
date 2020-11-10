
#include "../bltOverlay.h"

namespace modclasses
{
  // NOTE: basically all hardcoded currently
  // MainMenu

  // should only be called during init
  void MainMenu::addMenuStructure(std::unique_ptr<MenuBase> &menu)
  {
    subMenus->push_back(std::move(menu));
    computeStartEndVisible();
  }


  void MainMenu::move(MenuAction direction, BltOverlay &over)
  {
    size_t beforePos{ currentSelected };
    
    switch (direction)
    {
      case modclasses::MenuAction::UP:
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
      case modclasses::MenuAction::DOWN:
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

    size_t maxVisible{ (std::min)(subMenus->size() - 1, bigMenu ? 7u : 13u) };  // hardcoded
    startEndVisible = { 0, maxVisible };
  }


  MenuBase* MainMenu::action(BltOverlay &over)
  {
    if (subMenus->empty())
    {
      return nullptr;
    }
    
    // status of menu can stay
    MenuBase* newBase{ subMenus->at(currentSelected)->access(this, false, over) };
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

    return lastMenu->access(this, true, over);
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
    over.menuOffSurf->BltFast(over.menuPos.first, over.menuPos.second, over.compSurf, &over.menuRects.mainMenu, DDBLTFAST_NOCOLORKEY);

    // header text
    over.fntHandler.drawText(over.menuOffSurf, FontTypeEnum::NORMAL_BOLD, header, 150, 40, 260, true, true, true, nullptr);

    // draw menus
    if (subMenus->empty())
    {
      return;
    }

    int32_t menuXMid{ bigMenu ? 100 : 86 };
    int32_t steps{ bigMenu ? 60 : 32 };

    for (size_t i = startEndVisible.first; i < startEndVisible.second + 1; i++)
    {
      menuBoxDrawHelper(subMenus->at(i)->header, menuXMid, i == currentSelected, over);
      menuXMid += steps;
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

  MenuBase* MainMenu::access(MenuBase* caller, bool callerLeaving, BltOverlay &over)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!callerLeaving)
    {
      lastMenu = caller;
    }

    if (!moveToThis)
    {
      return nullptr;
    }

    draw(over);

    return this;
  }

  MainMenu::MainMenu(const std::string &headerString, const std::function<bool(bool leaving, std::string& header)> accessReactFunc,
                     bool isItBigMenu, std::unique_ptr<std::vector<std::unique_ptr<MenuBase>>> subMenusObj)
                     : MenuBase(headerString, accessReactFunc), bigMenu(isItBigMenu), subMenus(std::move(subMenusObj)) // hope this works
  {
    if (!subMenus)
    {
      subMenus = std::make_unique<std::vector<std::unique_ptr<MenuBase>>>();
    }
    computeStartEndVisible();
    LOG(INFO) << "HI";
  }
}