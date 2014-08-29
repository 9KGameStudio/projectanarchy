/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/ScaleformEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformInternal.hpp>

static struct
{
  WPARAM winKey;
  Scaleform::KeyCode appKey;
}
keyCodeMap[] =
{
  {VK_BACK,    Scaleform::Key::Backspace},
  {VK_TAB,     Scaleform::Key::Tab},
  {VK_CLEAR,   Scaleform::Key::Clear},
  {VK_RETURN,  Scaleform::Key::Return},
  {VK_SHIFT,   Scaleform::Key::Shift},
  {VK_CONTROL, Scaleform::Key::Control},
  {VK_MENU,    Scaleform::Key::Alt},
  {VK_PAUSE,   Scaleform::Key::Pause},
  {VK_CAPITAL, Scaleform::Key::CapsLock},
  {VK_ESCAPE,  Scaleform::Key::Escape},
  {VK_SPACE,   Scaleform::Key::Space},
  {VK_PRIOR,   Scaleform::Key::PageUp},
  {VK_NEXT,    Scaleform::Key::PageDown},
  {VK_END,     Scaleform::Key::End},
  {VK_HOME,    Scaleform::Key::Home},
  {VK_LEFT,    Scaleform::Key::Left},
  {VK_UP,      Scaleform::Key::Up},
  {VK_RIGHT,   Scaleform::Key::Right},
  {VK_DOWN,    Scaleform::Key::Down},
  {VK_INSERT,  Scaleform::Key::Insert},
  {VK_DELETE,  Scaleform::Key::Delete},
  {VK_HELP,    Scaleform::Key::Help},
  {VK_NUMLOCK, Scaleform::Key::NumLock},
  {VK_SCROLL,  Scaleform::Key::ScrollLock},

  {VK_OEM_1,     Scaleform::Key::Semicolon},
  {VK_OEM_PLUS,  Scaleform::Key::Equal},
  {VK_OEM_COMMA, Scaleform::Key::Comma},
  {VK_OEM_MINUS, Scaleform::Key::Minus},
  {VK_OEM_PERIOD,Scaleform::Key::Period},
  {VK_OEM_2,     Scaleform::Key::Slash},
  {VK_OEM_3,     Scaleform::Key::Bar},
  {VK_OEM_4,     Scaleform::Key::BracketLeft},
  {VK_OEM_5,     Scaleform::Key::Backslash},
  {VK_OEM_6,     Scaleform::Key::BracketRight},
  {VK_OEM_7,     Scaleform::Key::Quote},
  {VK_OEM_AX,    Scaleform::Key::OEM_AX},   //  'AX' key on Japanese AX kbd
  {VK_OEM_102,   Scaleform::Key::OEM_102},  //  "<>" or "\|" on RT 102-key kbd.
  {VK_ICO_HELP,  Scaleform::Key::ICO_HELP}, //  Help key on ICO
  {VK_ICO_00,    Scaleform::Key::ICO_00}   //  00 key on ICO
};

void VScaleformMovieInstance::HandleWindowsInputMessage(VWindowsMessageDataObject* pObj)
{
  m_pAdvanceTask->WaitUntilFinished();

  // mouse buttons are not handled here because of the non uniform input handling across all platforms
  switch(pObj->m_Msg.message)
  {

#if defined(USE_SF_IME)
  case WM_IME_STARTCOMPOSITION:
  case WM_IME_KEYDOWN:
  case WM_IME_COMPOSITION:   
  case WM_IME_ENDCOMPOSITION:
  case WM_IME_NOTIFY:
  case WM_INPUTLANGCHANGE:
  case WM_IME_CHAR:
    {
      //hkvLog::Warning("WM_IME_CHAR (%u) W: 0x%x L: 0x%x", pObj->m_Msg.message, pObj->m_Msg.wParam, pObj->m_Msg.lParam);
      Scaleform::GFx::IMEWin32Event ev(Scaleform::GFx::IMEWin32Event::IME_Default,
        (Scaleform::UPInt)pObj->m_Msg.hwnd, pObj->m_Msg.message, pObj->m_Msg.wParam, pObj->m_Msg.lParam);

      Scaleform::UInt32 handleEvtRetVal = m_pMovieInst->HandleEvent(ev);
      pObj->m_bProcessed = (handleEvtRetVal & Scaleform::GFx::Movie::HE_NoDefaultAction) != 0;
    }
    break;
#endif

  case WM_SETFOCUS:
    m_pMovieInst->HandleEvent(Scaleform::GFx::Event::SetFocus);
    break;
  case WM_KILLFOCUS:
    m_pMovieInst->HandleEvent(Scaleform::GFx::Event::KillFocus);
    break;

  case WM_KEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
    {
      unsigned int uiMessage = pObj->m_Msg.message;
      WPARAM wParam = pObj->m_Msg.wParam;

      // we have to convert the char to SF KeyCode enum
      const Scaleform::GFx::Event::EventType eventType = (uiMessage == WM_KEYDOWN || uiMessage == WM_SYSKEYDOWN) ? 
        Scaleform::GFx::Event::KeyDown : Scaleform::GFx::Event::KeyUp;

      Scaleform::KeyCode keyCode = Scaleform::Key::None;

      if (wParam >= 'A' && wParam <= 'Z')                   keyCode = (Scaleform::KeyCode) ((wParam - 'A') + Scaleform::Key::A);
      else if (wParam >= VK_F1 && wParam <= VK_F15)         keyCode = (Scaleform::KeyCode) ((wParam - VK_F1) + Scaleform::Key::F1);
      else if (wParam >= '0' && wParam <= '9')              keyCode = (Scaleform::KeyCode) ((wParam - '0') + Scaleform::Key::Num0);
      else if (wParam >= VK_NUMPAD0 && wParam <= VK_DIVIDE) keyCode = (Scaleform::KeyCode) ((wParam - VK_NUMPAD0) + Scaleform::Key::KP_0);
      else
      {
        for (int i = 0; i < V_ARRAY_SIZE(keyCodeMap); i++)
        {
          if (wParam == keyCodeMap[i].winKey)
          {
            keyCode = keyCodeMap[i].appKey;
            break;
          }
        }
      }

      // forward the event
      if (keyCode!=Scaleform::Key::None)
      { 
        Scaleform::UInt8 scanCode = (pObj->m_Msg.lParam >> 16) & 0xff;
        bool  extendedKeyFlag = (pObj->m_Msg.lParam & (1 << 24)) != 0;
        // we need a scanCode to distinguish between left and right shifts
        if (wParam == VK_SHIFT && scanCode == 54) // right shift code
        {
          extendedKeyFlag = true;
        }

        m_pMovieInst->HandleEvent(Scaleform::GFx::KeyEvent(eventType, keyCode, 0, 0, *UpdateModifiers(extendedKeyFlag)));
      }
      break; // do not mark msg as handled, otherwise WM_CHAR will miss the character
    }

  case WM_CHAR:
    m_pMovieInst->HandleEvent(Scaleform::GFx::CharEvent((Scaleform::UInt32)pObj->m_Msg.wParam));
    break;

  default:
    break;
  }
}

// Keep track of the pressed modifiers like CTRL, SHIFT, etc..
Scaleform::KeyModifiers* VScaleformMovieInstance::UpdateModifiers(bool extendedKeyFlag)
{
  Scaleform::KeyModifiers mods;
  if (::GetKeyState(VK_SHIFT) & 0x8000)
    mods.SetShiftPressed(true);
  if (::GetKeyState(VK_CONTROL) & 0x8000)
    mods.SetCtrlPressed(true);
  if (::GetKeyState(VK_MENU) & 0x8000)
    mods.SetAltPressed(true);
  if (::GetKeyState(VK_NUMLOCK) & 0x1)
    mods.SetNumToggled(true);
  if (::GetKeyState(VK_CAPITAL) & 0x1)
    mods.SetCapsToggled(true);
  if (::GetKeyState(VK_SCROLL) & 0x1)
    mods.SetScrollToggled(true);
  if (extendedKeyFlag)
    mods.SetExtendedKey(true);
  *m_pKeyModifiers = mods;
  return m_pKeyModifiers;
}

/*
 * Havok SDK - Base file, BUILD(#20140618)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
