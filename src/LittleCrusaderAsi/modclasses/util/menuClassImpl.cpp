
#include "../bltOverlay.h"

namespace modclasses
{
  // NOTE: basically all hardcoded currently


  // MenuBase //


  MenuBase* MenuBase::executeAction(MenuAction actionType, bool repeat)
  {
    switch (actionType)
    {
    case MenuAction::ACTION:
      return repeat ? nullptr : action(); // repeat can not be used for an action
    case MenuAction::BACK:
      return back();
    case MenuAction::UP:
    case MenuAction::DOWN:
    case MenuAction::RIGHT:
    case MenuAction::LEFT:
      move(actionType);
      break;
    default:
      break;
    }

    return nullptr;
  }


  MenuBase& MenuBase::ascend()
  {
    if (lastMenu)
    {
      return *lastMenu;
    }

    LOG(WARNING) << "BltOverlay: Tried to ascend menu without a higher menu. Staying on level.";
    return *this;
  }


  void MenuBase::forceDrawThisAndParent() const
  {
    if (lastMenu)
    {
      lastMenu->draw();
    }
    draw();
  }


  void MenuBase::draw(bool drawParent, bool checkIfParent) const
  {
    const MenuBase* toDraw{ drawParent && lastMenu ? lastMenu : this };
    const MenuBase* current{ bltOver.currentMenu };

    if (toDraw == current)
    {
      toDraw->draw();
    }
    else if (checkIfParent && toDraw == current->lastMenu)
    {
      current->forceDrawThisAndParent();
    }
  }
  
  
  // MainMenu //


  void MainMenu::addChild(std::unique_ptr<MenuBase> &&menu)
  {
    subMenus->push_back(std::move(menu));
    computeStartEndVisible();
  }


  void MainMenu::move(MenuAction direction)
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
      draw();
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


  MenuBase* MainMenu::action()
  {
    if (subMenus->empty())
    {
      return nullptr;
    }
    
    // status of menu can stay
    MenuBase* newBase{ subMenus->at(currentSelected)->access(false) };
    if (!newBase)
    {
      // redraw in case the teyt changed
      draw();
    }

    return newBase;
  }


  MenuBase* MainMenu::back()
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

    return lastMenu->access(true);
  }


  void MainMenu::menuBoxDrawHelper(const std::string& text, int32_t yPos, bool active) const
  {
    FontTypeEnum type;
    RECT* rect;
    std::pair<int32_t, int32_t> boxPos{ 10, 0 };
    
    if (bigMenu)
    {
      type = FontTypeEnum::NORMAL;
      rect = active ? &bltOver.menuRects.bigButtonPressed : &bltOver.menuRects.bigButton;
      boxPos.second = yPos - 30;
    }
    else
    {
      type = FontTypeEnum::SMALL_BOLD;
      rect = active ? &bltOver.menuRects.smallButtonPressed : &bltOver.menuRects.smallButton;
      boxPos.second = yPos - 16;
    }

    bltOver.menuOffSurf->BltFast(boxPos.first, boxPos.second, bltOver.compSurf, rect, DDBLTFAST_NOCOLORKEY);
    bltOver.fntHandler.drawText(bltOver.menuOffSurf, type, text, 150, yPos, 260, true, true, true, nullptr);
  }


  void MainMenu::draw() const
  {
    // initial background
    bltOver.menuOffSurf->BltFast(0, 0, bltOver.compSurf, &bltOver.menuRects.mainMenu, DDBLTFAST_NOCOLORKEY);

    // header text
    bltOver.fntHandler.drawText(bltOver.menuOffSurf, FontTypeEnum::NORMAL_BOLD, header, 150, 40, 260, true, true, true, nullptr);

    // draw menus
    if (subMenus->empty())
    {
      return;
    }

    int32_t menuYMid{ bigMenu ? 100 : 86 };
    int32_t steps{ bigMenu ? 60 : 32 };

    for (size_t i = startEndVisible.first; i < startEndVisible.second + 1; i++)
    {
      menuBoxDrawHelper(subMenus->at(i)->header, menuYMid, i == currentSelected);
      menuYMid += steps;
    }

    if (startEndVisible.first > 0)
    {
      bltOver.menuOffSurf->BltFast(0, 70, bltOver.compSurf, &bltOver.menuRects.goldArrowUp, DDBLTFAST_SRCCOLORKEY);
    }

    if (startEndVisible.second < subMenus->size() - 1)
    {
      bltOver.menuOffSurf->BltFast(0, 430, bltOver.compSurf, &bltOver.menuRects.goldArrowDown, DDBLTFAST_SRCCOLORKEY);
    }
  }


  MenuBase* MainMenu::access(bool callerLeaving)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!moveToThis)
    {
      return nullptr;
    }

    draw();

    return this;
  }


  MainMenu::MainMenu(BltOverlay& overlay, std::string &&headerString, HeaderReact &&accessReactFunc, bool &&isItBigMenu)
                    : MenuBase(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                      bigMenu(std::move(isItBigMenu)), subMenus(std::make_unique<std::vector<std::unique_ptr<MenuBase>>>()) {}

  MainMenu::MainMenu(BltOverlay& overlay, std::string &&headerString, HeaderReact &&accessReactFunc,
                     bool &&isItBigMenu, MainMenuPointer* ptrCon)
                    : MainMenu(overlay, std::forward<std::string>(headerString),
                      std::forward<HeaderReact>(accessReactFunc), std::forward<bool>(isItBigMenu))
  {
    if (ptrCon)
    {
      ptrCon->thisMenu = this;
      ptrCon->header = &header;
      ptrCon->currentSelected = &currentSelected;
      ptrCon->startEndVisible = &startEndVisible;
      ptrCon->bigMenu = &bigMenu;
    }
  }


  // ChoiceInputMenu //


  void ChoiceInputMenu::move(MenuAction direction)
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
      draw();
    }
  }


  void ChoiceInputMenu::computeStartEndVisible()
  {
    if (choicePairs.size() <= 1)
    {
      startEndVisible = { 0, 0 };
      return;
    }

    size_t maxVisible{
      (std::min)(
        choicePairs.size() - 1,
        selectReact ? 4u : 2u
      )
    };  // hardcoded ( maxNum + 1 (0 index) )
    startEndVisible = { 0, maxVisible };
  }


  // only apply (ok) button reacts to "push"
  MenuBase* ChoiceInputMenu::action()
  {
    if (choicePairs.empty() || !(selectReact || selectValueReact))
    {
      return nullptr;
    }

    if (selectReact)
    {
      selectReact(choicePairs.at(currentSelected).second, resultOfEnter);
    }
    else if (selectValueReact && selectValueReact(choicePairs.at(currentSelected).second, resultOfEnter, false, currentValue))
    {
      currentValue = choicePairs.at(currentSelected).first;
    }

    draw();
    return nullptr;
  }


  MenuBase* ChoiceInputMenu::back()
  {
    if (!lastMenu)
    {
      return nullptr;
    }

    bltOver.inputActive = false;

    if (accessReact)
    {
      // ignore return
      accessReact(true, header);
    }

    return lastMenu->access(true);
  }


  void ChoiceInputMenu::menuBoxDrawHelper(const std::string& text, int32_t yPos, bool active) const
  {
    FontTypeEnum type{ FontTypeEnum::SMALL };
    RECT* rect{ active ? &bltOver.menuRects.inputFieldSelected : &bltOver.menuRects.inputField };
    std::pair<int32_t, int32_t> boxPos{ 25, yPos - 16 };

    bltOver.inputOffSurf->BltFast(boxPos.first, boxPos.second, bltOver.compSurf, rect, DDBLTFAST_NOCOLORKEY);
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, type, text, 150, yPos, 230, true, true, true, nullptr);
  }


  void ChoiceInputMenu::draw() const
  {
    // initial background
    bltOver.inputOffSurf->BltFast(0, 0, bltOver.compSurf, &bltOver.menuRects.bigInputBox, DDBLTFAST_NOCOLORKEY);

    // header text
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL_BOLD, header, 150, 25, 280, true, true, true, nullptr);

    if (selectValueReact)
    {
      // default
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, "Default:", 16, 55, 65, false, true, true, nullptr);
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, defaultValue, 192, 55, 184, true, true, true, nullptr);

      // current
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, "Current:", 16, 80, 65, false, true, true, nullptr);
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, currentValue, 192, 80, 184, true, true, true, nullptr);
    }

    // draw menus
    if (choicePairs.empty())
    {
      return;
    }

    int32_t menuYMid{ selectReact ? 60 : 124 };
    for (size_t i = startEndVisible.first; i < startEndVisible.second + 1; i++)
    {
      menuBoxDrawHelper(choicePairs.at(i).first, menuYMid, i == currentSelected);
      menuYMid += 32;
    }

    // assuming three visible max
    if (startEndVisible.first > 0)
    {
      bltOver.inputOffSurf->BltFast(5, selectReact ? 54 : 118, bltOver.compSurf, &bltOver.menuRects.silverArrowUp, DDBLTFAST_SRCCOLORKEY);
      bltOver.inputOffSurf->BltFast(275, selectReact ? 54 : 118, bltOver.compSurf, &bltOver.menuRects.silverArrowUp, DDBLTFAST_SRCCOLORKEY);
    }

    if (startEndVisible.second < choicePairs.size() - 1)
    {
      bltOver.inputOffSurf->BltFast(5, 162, bltOver.compSurf, &bltOver.menuRects.silverArrowDown, DDBLTFAST_SRCCOLORKEY);
      bltOver.inputOffSurf->BltFast(275, 162, bltOver.compSurf, &bltOver.menuRects.silverArrowDown, DDBLTFAST_SRCCOLORKEY);
    }

    // apply text
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, resultOfEnter, 150, 226, 200, true, true, true, nullptr);
  }


  MenuBase* ChoiceInputMenu::access(bool callerLeaving)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!moveToThis)
    {
      return nullptr;
    }

    if (selectValueReact)
    {
      selectValueReact(0, resultOfEnter, true, currentValue);  // update after access
    }
    else if (!selectReact)
    {
      resultOfEnter = "Missing react function.";
    }

    // setting status during access
    currentSelected = 0;
    computeStartEndVisible();
    resultOfEnter = "";

    draw();
    bltOver.inputActive = true;

    return this;
  }


  ChoiceInputMenu::ChoiceInputMenu(BltOverlay& overlay, std::string &&headerString, HeaderReact &&accessReactFunc, std::string &&defaultValueStr,
                                   std::vector<std::pair<std::string, int32_t>> &&choicePairCon, SelectValueReact &&selectReactFunc)
                                  : MenuBase(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                                    defaultValue(std::move(defaultValueStr)), choicePairs(std::move(choicePairCon)),
                                    selectValueReact(std::move(selectReactFunc))
  {
    computeStartEndVisible();
    if (selectValueReact)
    {
      selectValueReact(0, resultOfEnter, true, currentValue);  // update after access
    }
  }

  ChoiceInputMenu::ChoiceInputMenu(BltOverlay& overlay, std::string &&headerString, HeaderReact &&accessReactFunc,
                                   std::string &&defaultValueStr, std::vector<std::pair<std::string, int32_t>> &&choicePairCon,
                                   SelectValueReact &&selectReactFunc, ChoiceInputMenuPointer* ptrCon)
                                  : ChoiceInputMenu(overlay, std::forward<std::string>(headerString),
                                    std::forward<HeaderReact>(accessReactFunc), std::forward<std::string>(defaultValueStr),
                                    std::forward<std::vector<std::pair<std::string, int32_t>>>(choicePairCon),
                                    std::forward<SelectValueReact>(selectReactFunc))
  {
    if (ptrCon)
    {
      ptrCon->thisMenu = this;
      ptrCon->header = &header;
      ptrCon->choicePairs = &choicePairs;
      ptrCon->currentSelected = &currentSelected;
      ptrCon->startEndVisible = &startEndVisible;
      ptrCon->defaultValue = &defaultValue;
      ptrCon->currentValue = &currentValue;
      ptrCon->resultOfEnter = &resultOfEnter;
    }
  }

  ChoiceInputMenu::ChoiceInputMenu(BltOverlay& overlay, std::string &&headerString, HeaderReact &&accessReactFunc,
                                   std::vector<std::pair<std::string, int32_t>> &&choicePairCon, SelectReact &&selectReactFunc)
                                  : MenuBase(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                                    choicePairs(std::move(choicePairCon)), selectReact(std::move(selectReactFunc))
  {
    computeStartEndVisible();
  }

  ChoiceInputMenu::ChoiceInputMenu(BltOverlay& overlay, std::string &&headerString, HeaderReact &&accessReactFunc,
                                   std::vector<std::pair<std::string, int32_t>> &&choicePairCon, SelectReact &&selectReactFunc,
                                   ChoiceInputMenuPointer* ptrCon)
                                  : ChoiceInputMenu(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc),
                                    std::forward<std::vector<std::pair<std::string, int32_t>>>(choicePairCon),
                                    std::forward<SelectReact>(selectReactFunc))
  {
    if (ptrCon)
    {
      ptrCon->thisMenu = this;
      ptrCon->header = &header;
      ptrCon->choicePairs = &choicePairs;
      ptrCon->currentSelected = &currentSelected;
      ptrCon->startEndVisible = &startEndVisible;
      ptrCon->resultOfEnter = &resultOfEnter;
    }
  }


  // FreeInputMenu


  void FreeInputMenu::move(MenuAction direction)
  {
    bool updateDraw{ false };

    if (direction == MenuAction::UP || direction == MenuAction::DOWN)
    {
      if (!bltOver.enableCharReceive(!bltOver.editing))
      {
        resultOfEnter = "Char receiver fail.";
      }

      updateDraw = true;
    }
    else if (bltOver.editing)
    {
      switch (direction)
      {
        case MenuAction::LEFT:
        {
          if (cursorPos > 0)
          {
            --cursorPos;
            updateDraw = true;
          }
          break;
        }
        case MenuAction::RIGHT:
        {
          if (cursorPos < currentInput.size())
          {
            ++cursorPos;
            updateDraw = true;
          }
          break;
        }
        default:
          break;
      }
    }

    if (updateDraw)
    {
      draw();
    }
  }


  MenuBase* FreeInputMenu::action()
  {
    // action will get into menu
    if (!bltOver.editing)
    {
      if (!bltOver.enableCharReceive(!bltOver.editing))
      {
        resultOfEnter = "Char receiver fail.";
      }
      draw();
      return nullptr;
    }

    if (inputReact)
    {
      inputReact(currentInput, resultOfEnter);
    }
    else if (inputValueReact)
    {
      inputValueReact(currentInput, resultOfEnter, false, currentValue);
    }

    // deselect after apply
    if (!bltOver.enableCharReceive(!bltOver.editing))
    {
      resultOfEnter = "Char receiver fail.";
    }
    draw();
    return nullptr;
  }


  MenuBase* FreeInputMenu::back()
  {
    if (bltOver.editing)
    {
      if (cursorPos > 0)
      {
        --cursorPos;
        currentInput.erase(cursorPos, 1);
        draw();
      }
      return nullptr;
    }

    if (!lastMenu)
    {
      return nullptr;
    }

    // setting status back -> should be enough
    currentInput = "";
    cursorPos = 0;
    resultOfEnter = "";
    bltOver.inputActive = false;

    if (accessReact)
    {
      // ignore return
      accessReact(true, header);
    }

    return lastMenu->access(true);
  }


  MenuBase* FreeInputMenu::access(bool callerLeaving)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!moveToThis)
    {
      return nullptr;
    }

    if (inputValueReact)
    {
      inputValueReact(currentInput, resultOfEnter, true, currentValue);  // update after access
    }
    else if (!inputReact)
    {
      resultOfEnter = "Missing react function.";
    }

    draw();
    bltOver.inputActive = true;

    return this;
  }

  // TODO?: maybe take an unsigned char in th efirst place?
  // altough, if string uses char intern, it would not make much of a difference
  MenuBase* FreeInputMenu::executeAction(char newChar)
  {
    if (onlyNumber && std::find(numberChars.begin(), numberChars.end(), newChar) == numberChars.end())
    {
      return nullptr;
    }
    else if (newChar > -1 && newChar < 32)  // reject non-printable chars (this input menu will likely never use them)
    {
      return nullptr;
    }

    if (currentInput.size() < maxInputLength)
    {
      currentInput.insert(cursorPos, 1, newChar);
      ++cursorPos;
      draw();
    }
    return nullptr;
  }


  void FreeInputMenu::draw() const
  {
    // NOTE: box in inputReact case is 100px smaller (top is not 0, but 50)
    // change positions of menu parts if not value input
    const int32_t adapt{ inputReact ? 25 : 0 };

    if (inputReact)
    {
      // fill black for colorkey
      DDBLTFX fx;
      ZeroDDObjectAndSetSize<DDBLTFX>(fx);
      bltOver.inputOffSurf->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &fx);

      // initial background
      bltOver.inputOffSurf->BltFast(0, 50, bltOver.compSurf, &bltOver.menuRects.smallInputBox, DDBLTFAST_NOCOLORKEY);
    }
    else
    {
      // initial background
      bltOver.inputOffSurf->BltFast(0, 0, bltOver.compSurf, &bltOver.menuRects.bigInputBox, DDBLTFAST_NOCOLORKEY);
    }

    // header text
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL_BOLD, header, 150, 25 + adapt * 2,
      280, true, true, true, nullptr);

    if (inputValueReact)
    {
      // default
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, "Default:", 16, 70, 65, false, true, true, nullptr);
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, defaultValue, 192, 70, 184, true, true, true, nullptr);

      // current
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, "Current:", 16, 100, 65, false, true, true, nullptr);
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, currentValue, 192, 100, 184, true, true, true, nullptr);
    }

    // draw input box
    RECT* rect{ bltOver.editing ? &bltOver.menuRects.inputFieldSelected : &bltOver.menuRects.inputField };
    bltOver.inputOffSurf->BltFast(25, 139 - adapt, bltOver.compSurf, rect, DDBLTFAST_NOCOLORKEY);

    // draw input text
    if (bltOver.editing)
    {
      std::string cursorInsert{ currentInput };
      cursorInsert.insert(cursorPos, 1, '|');
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, cursorInsert, 150, 155 - adapt, 230, true, true, true, nullptr);
    }
    else
    {
      bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, currentInput, 150, 155 - adapt, 230, true, true, true, nullptr);
    }
    
    // apply text
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, resultOfEnter, 150, 225 - adapt * 2,
      200, true, true, true, nullptr);
  }


  FreeInputMenu::FreeInputMenu(BltOverlay& overlay, std::string&& headerString, HeaderReact&& accessReactFunc, bool onlyNumbers,
                               std::string&& defaultValueStr, InputValueReact&& inputValueReactFunc)
                              : MenuBase(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                                onlyNumber(onlyNumbers), defaultValue(std::move(defaultValueStr)),
                                inputValueReact(std::move(inputValueReactFunc))
  {
    if (inputValueReact)
    {
      inputValueReact(currentInput, resultOfEnter, true, currentValue);  // update after access
    }
  }

  FreeInputMenu::FreeInputMenu(BltOverlay& overlay, std::string&& headerString, HeaderReact&& accessReactFunc,
                               bool onlyNumbers, InputReact&& inputReactFunc)
                              : MenuBase(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                                onlyNumber(onlyNumbers), inputReact(std::move(inputReactFunc)) { }

  FreeInputMenu::FreeInputMenu(BltOverlay& overlay, std::string&& headerString, HeaderReact&& accessReactFunc, bool onlyNumbers,
                               std::string&& defaultValueStr, InputValueReact&& inputValueReactFunc, FreeInputMenuPointer* ptrCon)
                              : FreeInputMenu(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc),
                                onlyNumbers, std::forward<std::string>(defaultValueStr), std::forward<InputValueReact>(inputValueReactFunc))
  {
    if (ptrCon)
    {
      ptrCon->thisMenu = this;
      ptrCon->header = &header;
      ptrCon->defaultValue = &defaultValue;
      ptrCon->currentValue = &currentValue;
      ptrCon->currentInput = &currentInput;
      ptrCon->cursorPos = &cursorPos;
      ptrCon->resultOfEnter = &resultOfEnter;
    }
  }

  FreeInputMenu::FreeInputMenu(BltOverlay& overlay, std::string&& headerString, HeaderReact&& accessReactFunc,
                               bool onlyNumbers, InputReact&& inputReactFunc, FreeInputMenuPointer* ptrCon)
                              : FreeInputMenu(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc),
                                onlyNumbers, std::forward<InputReact>(inputReactFunc))
  {
    if (ptrCon)
    {
      ptrCon->thisMenu = this;
      ptrCon->header = &header;
      ptrCon->currentInput = &currentInput;
      ptrCon->cursorPos = &cursorPos;
      ptrCon->resultOfEnter = &resultOfEnter;
    }
  }


  // SortableListMenu //


  void SortableListMenu::menuBoxDrawHelper(const std::string& text, bool enabled, int32_t yPos, bool active, size_t number) const
  {
    RECT* rect{ active ? &bltOver.menuRects.inputFieldSelected : &bltOver.menuRects.inputField };
    std::pair<int32_t, int32_t> boxPos{ 40, yPos - 16 };

    bltOver.inputOffSurf->BltFast(boxPos.first, boxPos.second, bltOver.compSurf, rect, DDBLTFAST_NOCOLORKEY);
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, text, 165, yPos, 186, true, true, true, nullptr);

    if (enabled)
    {
      bltOver.inputOffSurf->BltFast(boxPos.first + 218, boxPos.second, bltOver.compSurf, &bltOver.menuRects.okBox, DDBLTFAST_NOCOLORKEY);
    }
    else
    {
      bltOver.inputOffSurf->BltFast(boxPos.first + 218, boxPos.second, bltOver.compSurf, &bltOver.menuRects.cancelBox, DDBLTFAST_NOCOLORKEY);
    }

    if (moving && active)
    {
      bltOver.inputOffSurf->BltFast(boxPos.first, boxPos.second, bltOver.compSurf, &bltOver.menuRects.silverArrowUp, DDBLTFAST_SRCCOLORKEY);
      bltOver.inputOffSurf->BltFast(boxPos.first + 15, boxPos.second, bltOver.compSurf, &bltOver.menuRects.silverArrowDown, DDBLTFAST_SRCCOLORKEY);
    }

    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL_BOLD, std::to_string(number) + ".",
      20, yPos, 30, true, true, true, nullptr);
  }


  void SortableListMenu::draw() const
  {
    // initial background
    bltOver.inputOffSurf->BltFast(0, 0, bltOver.compSurf, &bltOver.menuRects.bigInputBox, DDBLTFAST_NOCOLORKEY);

    // header text
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL_BOLD, header, 165, 25, 260, true, true, true, nullptr);

    // draw menus
    if (menuCon.empty())
    {
      return;
    }

    int32_t menuYMid{ 64 };
    size_t counter{ 0 };
    for(auto& tup : menuCon)
    {
      if (counter < startEndVisible.first || counter > startEndVisible.second)
      {
        ++counter;
        continue; // only handle visible
      }

      menuBoxDrawHelper(std::get<0>(tup), std::get<1>(tup), menuYMid, counter == currentPos, counter + 1);
      menuYMid += 32;
      ++counter;
    }

    // assuming three visible max
    if (startEndVisible.first > 0)
    {
      bltOver.inputOffSurf->BltFast(10, 16, bltOver.compSurf, &bltOver.menuRects.silverArrowUp, DDBLTFAST_SRCCOLORKEY);
    }

    if (startEndVisible.second < menuCon.size() - 1)
    {
      bltOver.inputOffSurf->BltFast(10, 208, bltOver.compSurf, &bltOver.menuRects.silverArrowDown, DDBLTFAST_SRCCOLORKEY);
    }

    // apply text
    bltOver.fntHandler.drawText(bltOver.inputOffSurf, FontTypeEnum::SMALL, resultOfEnter, 165, 226, 200, true, true, true, nullptr);
  }


  MenuBase* SortableListMenu::access(bool callerLeaving)
  {
    bool moveToThis{ accessReact != nullptr ? accessReact(callerLeaving, header) : true };

    if (!moveToThis)
    {
      return nullptr;
    }

    // setting status during access
    currentElement = menuCon.begin();
    currentPos = 0;
    resultOfEnter = "";
    computeStartEndVisible();

    draw();
    bltOver.inputActive = true;

    return this;
  }


  void SortableListMenu::correctStartEndVisible()
  {
    if (currentPos != originalPos)
    {
      // should never be big enough for issues
      int32_t startEndJump{ static_cast<int32_t>(originalPos) - static_cast<int32_t>(currentPos) };
      int32_t toAdd{ 0 };
      if (startEndJump > 0)
      {
        toAdd = (std::min)(startEndJump, static_cast<int32_t>(menuCon.size()) - 1 - static_cast<int32_t>(startEndVisible.second));
      }
      else
      {
        toAdd = (std::max)(startEndJump, -static_cast<int32_t>(startEndVisible.first));
      }
      startEndVisible = { startEndVisible.first + toAdd, startEndVisible.second + toAdd };

      currentPos = originalPos;
    }
  }


  MenuBase* SortableListMenu::action()
  {
    if (menuCon.empty())
    {
      return nullptr;
    }

    if (moving)
    {
      if (sortChangeReact && !sortChangeReact(menuCon, resultOfEnter))
      {
        menuCon.splice(originalElementPos, menuCon, currentElement);
        correctStartEndVisible();
      }

      moving = false;
    }
    else
    {
      originalElementPos = std::next(currentElement);
      originalPos = currentPos;
      moving = true;
    }

    draw();
    return nullptr;
  }


  MenuBase* SortableListMenu::back()
  {
    if (moving)
    {
      // TODO?: will not jump the menu to original element pos -> menu will stay -> change this?
      menuCon.splice(originalElementPos, menuCon, currentElement);
      correctStartEndVisible();
      moving = false;
      draw();
      return nullptr;
    }

    if (!lastMenu)
    {
      return nullptr;
    }

    bltOver.inputActive = false;

    if (accessReact)
    {
      // ignore return
      accessReact(true, header);
    }

    return lastMenu->access(true);
  }


  void SortableListMenu::computeStartEndVisible()
  {
    if (menuCon.size() <= 1)
    {
      startEndVisible = { 0, 0 };
      return;
    }

    size_t maxVisible{ (std::min)(menuCon.size() - 1, 4u) };  // hardcodedy
    startEndVisible = { 0, maxVisible };
  }


  void SortableListMenu::move(MenuAction direction)
  {
    bool change{ false };

    switch (direction)
    {
    case MenuAction::UP:
    {
      if (currentPos > 0)
      {
        --currentPos;
        if (currentPos < startEndVisible.first)
        {
          --startEndVisible.first;
          --startEndVisible.second;
        }

        if (moving)
        {
          menuCon.splice(std::prev(currentElement), menuCon, currentElement);
        }
        else
        {
          currentElement = std::prev(currentElement);
        }

        change = true;
      }
      break;
    }
    case MenuAction::DOWN:
    {
      if (currentPos < menuCon.size() - 1)
      {
        ++currentPos;
        if (currentPos > startEndVisible.second)
        {
          ++startEndVisible.first;
          ++startEndVisible.second;
        }

        if (moving)
        {
          // needs to move two iterators up to get element to be placed before
          menuCon.splice(std::next(currentElement, 2), menuCon, currentElement);
        }
        else
        {
          currentElement = std::next(currentElement);
        }

        change = true;
      }
      break;
    }
    case MenuAction::LEFT:
    case MenuAction::RIGHT:
    {
      std::get<1>(*currentElement) = !std::get<1>(*currentElement);
      if (actiReact && !actiReact(*currentElement, resultOfEnter))
      {
        std::get<1>(*currentElement) = !std::get<1>(*currentElement);
      }
      else
      {
        change = true;
      }

      break;
    }

    default:
      break;
    }

    if (change)
    {
      draw();
    }
  }


  SortableListMenu::SortableListMenu(BltOverlay& overlay, std::string&& headerString, HeaderReact&& accessReactFunc,
                                     SortMenuContainer&& sortMenuCon, SortChangeReact&& sortChangeReactFunc, ActivationReact&& actiReactFunc)
                                    : MenuBase(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc)),
                                      menuCon(std::move(sortMenuCon)), sortChangeReact(std::move(sortChangeReactFunc)),
                                      actiReact(std::move(actiReactFunc))
  {
    computeStartEndVisible();
    currentElement = menuCon.begin();
  }

  SortableListMenu::SortableListMenu(BltOverlay& overlay, std::string&& headerString, HeaderReact&& accessReactFunc,
                                     SortMenuContainer&& sortMenuCon, SortChangeReact&& sortChangeReactFunc,
                                     ActivationReact&& actiReactFunc, SortableListMenuPointer* ptrCon)
                                    : SortableListMenu(overlay, std::forward<std::string>(headerString), std::forward<HeaderReact>(accessReactFunc),
                                      std::forward<SortMenuContainer>(sortMenuCon), std::forward<SortChangeReact>(sortChangeReactFunc),
                                      std::forward<ActivationReact>(actiReactFunc))
  {
    if (ptrCon)
    {
      ptrCon->thisMenu = this;
      ptrCon->header = &header;
      ptrCon->currentPos = &currentPos;
      ptrCon->currentElement = &currentElement;
      ptrCon->originalElementPos = &originalElementPos;
      ptrCon->originalPos = &originalPos;
      ptrCon->moving = &moving;
      ptrCon->startEndVisible = &startEndVisible;
      ptrCon->menuCon = &menuCon;
      ptrCon->resultOfEnter = &resultOfEnter;
    }
  }
}