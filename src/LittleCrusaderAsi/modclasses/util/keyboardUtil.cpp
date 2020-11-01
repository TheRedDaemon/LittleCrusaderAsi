
#include "../enumheaders/keyboardEnumsAndUtil.h"

namespace modclasses
{
  void KChange::doAction(const HWND window, const bool keyUp, const bool repeat) const
  {
    // for now ignoring lparam, also doesn't send letters at the moment
    // adding restructure to use char modifiers would require additional structure changes
    // at the moment, I rather use the switch, or later, a event notification that an text input is required
    // could also be, that at this point, keyboard changes are done in the crusader keyboard handler and
    // this here will only serv for func calls (and this might make this class useless)

    if (keyUp)
    {
      SendMessage(window, WM_KEYUP, keyToUse, 1);
    }
    else
    {
      // no repeat used currently
      if (!repeat)
      {
        SendMessage(window, WM_KEYDOWN, keyToUse, 0);
      }

      //if (letter)
      //{
      //  SendMessage(window, WM_CHAR, letter, 1);
      //}
    }
  }

  void KFunction::doAction(const HWND window, const bool keyUp, const bool repeat) const
  {
    funcToUse(window, keyUp, repeat);
  }
}