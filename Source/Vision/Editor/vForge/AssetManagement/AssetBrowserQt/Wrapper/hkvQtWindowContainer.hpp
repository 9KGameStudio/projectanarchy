/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvQtWindowContainer.hpp

#ifndef VASSETBROWSER_HPP_INCLUDED
#define VASSETBROWSER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>

/// \brief
///   Data object for the OnRequestDialogClose callback in the hkvAssetBrowserQt.
class hkvRequestDialogCloseData : public hkvCallbackData
{
public:
  inline hkvRequestDialogCloseData(hkvCallback* pSender, int returnCode, const char* uids) : 
  hkvCallbackData(pSender), m_iReturnCode(returnCode), m_returnValue(uids)
  {
  }
  int m_iReturnCode;
  hkStringPtr m_returnValue;
};


class hkvEnableWindowData : public hkvCallbackData
{
public:
  inline hkvEnableWindowData(hkvCallback* pSender, bool bEnableWindow) : 
  hkvCallbackData(pSender), m_bEnableWindow(bEnableWindow)
  {
  }
  bool m_bEnableWindow;
};


class hkvQtWindowContainer
{
public:
  ASSETBROWSER_IMPEXP hkvQtWindowContainer(bool useWrapper = false, bool isModal = false, const char* caption = "", const char* description = "", const char* icon = "");

  // calls private virtual method that creates the window and does embedding etc.
  ASSETBROWSER_IMPEXP void Create(void* windowHandle);
  ASSETBROWSER_IMPEXP void Delete();
  ASSETBROWSER_IMPEXP void Resize(int iWidth, int iHeight);

public: // static functions
  ASSETBROWSER_IMPEXP static void EnableAppWindows(bool bEnable);

private:
  static int s_iWindowEnableRefCount;

public:
  ASSETBROWSER_IMPEXP static hkvCallbackWrapper OnEnableWindowChanged;

protected:
  virtual void* QtWindowCreate_private() = 0;

private:
  bool m_useWrapper;
  bool m_isModal;
  hkStringPtr m_caption;
  hkStringPtr m_description;
  hkStringPtr m_icon;
  void* m_pQtWindow;
};



#endif

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
