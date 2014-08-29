/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvUIService.hpp

#ifndef HKVUISERVICE_HPP_INCLUDED
#define HKVUISERVICE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Common/Base/Types/hkBaseTypes.h>

class hkvUiService
{
public:
  enum MessageBoxButton
  {
    Ok      = V_BIT(0),
    Yes     = V_BIT(1),
    No      = V_BIT(2),
    Abort   = V_BIT(3),
    Retry   = V_BIT(4),
    Cancel  = V_BIT(5),
  };

  enum MessageBoxIcon
  {
    Information = V_BIT(0),
    Warning     = V_BIT(1),
    Critical    = V_BIT(2),
    Question    = V_BIT(3),
  };

  enum HelpSystemQuery
  {
    HasHelpPageQuery  = V_BIT(0),
    ShowHelpPageQuery = V_BIT(1),
  };

  /// \brief
  ///   Data object for the OnRunUiServiceCommand callback in hkvUiService.
  class UiMessageBoxData : public hkvCallbackData
  {
  public:
    ASSETFRAMEWORK_IMPEXP UiMessageBoxData(const hkvCallback* sender, const char* title, const char* text,
      hkFlags<MessageBoxButton, hkUint8> availableButtons, MessageBoxButton defaultButton, MessageBoxIcon icon)
        : hkvCallbackData(sender), m_title(title), m_text(text), m_availableButtons(availableButtons), m_defaultButton(defaultButton), m_icon(icon), m_clickedButton(defaultButton)
    {
    }

    hkStringPtr m_title;
    hkStringPtr m_text;
    hkFlags<MessageBoxButton, hkUint8> m_availableButtons;
    hkEnum<MessageBoxButton, hkUint8> m_defaultButton;
    hkEnum<MessageBoxIcon, hkUint8> m_icon;

    hkEnum<MessageBoxButton, hkUint8> m_clickedButton;
  };

  /// \brief
  ///   Data object for the OnQuerySilentMode callback in hkvUiService.
  class UiQueryEditorStatusData : public hkvCallbackData
  {
  public:
    ASSETFRAMEWORK_IMPEXP UiQueryEditorStatusData(const hkvCallback* sender) : hkvCallbackData(sender), m_silentMode(false), m_isDevelopmentMachine(false), m_isPushResourceChangesEnabled(false)
    {
    }

    bool m_silentMode;
    bool m_isDevelopmentMachine;
    bool m_isPushResourceChangesEnabled;
    hkStringPtr m_projectDir;
    hkStringPtr m_projectSearchPath;
  };

  /// \brief
  ///   Data object for the OnQuerySilentMode callback in hkvUiService.
  class QueryHelpSystemData : public hkvCallbackData
  {
  public:
    ASSETFRAMEWORK_IMPEXP QueryHelpSystemData(const hkvCallback* sender, HelpSystemQuery query, const char* key, const char* context)
      : hkvCallbackData(sender), m_query(query), m_key(key), m_context(context), m_success(false)
    {
    }

    // in
    hkEnum<HelpSystemQuery, hkUint8> m_query;
    const char* m_key;
    const char* m_context;

    // out
    bool m_success;
  };

public:
  ASSETFRAMEWORK_IMPEXP static void TriggerOnShowMessageBoxCallback(hkvCallbackData* pData);
  ASSETFRAMEWORK_IMPEXP static void TriggerOnQueryEditorStatusCallback(hkvCallbackData* pData);
  ASSETFRAMEWORK_IMPEXP static void TriggerOnQueryHelpSystemCallback(hkvCallbackData* pData);

public: // Data
  ASSETFRAMEWORK_IMPEXP static hkvCallbackWrapper OnShowMessageBox;
  ASSETFRAMEWORK_IMPEXP static hkvCallbackWrapper OnQueryEditorStatus;
  ASSETFRAMEWORK_IMPEXP static hkvCallbackWrapper OnQueryHelpSystem;

public:
  ASSETFRAMEWORK_IMPEXP static bool isSilentModeEnabled();
  ASSETFRAMEWORK_IMPEXP static bool isDevelopmentMachine();
  ASSETFRAMEWORK_IMPEXP static bool isPushResourceChangesEnabled();
  ASSETFRAMEWORK_IMPEXP static const char* getProjectDir(hkStringBuf& out_buffer);
  ASSETFRAMEWORK_IMPEXP static const char* getProjectSearchPath(hkStringBuf& out_buffer);
  ASSETFRAMEWORK_IMPEXP static MessageBoxButton showMessageBox(const char* title, const char* text, hkFlags<MessageBoxButton, hkUint8> availableButtons = Ok, MessageBoxButton defaultButton = Ok, MessageBoxIcon icon = Information);

  ASSETFRAMEWORK_IMPEXP static bool HasHelpPage(const char* key, const char* contextName = NULL);
  ASSETFRAMEWORK_IMPEXP static bool ShowHelpPage(const char* key, const char* contextName = NULL);
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
