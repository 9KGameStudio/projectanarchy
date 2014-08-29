/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPROPERTYMODELENTRY_HPP_INCLUDED
#define HKVPROPERTYMODELENTRY_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowser.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
ANALYSIS_IGNORE_ALL_START
#include <deque>
ANALYSIS_IGNORE_ALL_END

class hkvPropertyModelEntry
{
public:
  ASSETBROWSER_IMPEXP hkvPropertyModelEntry(hkvProperty* pProperty);
  ASSETBROWSER_IMPEXP ~hkvPropertyModelEntry();

  ASSETBROWSER_IMPEXP void ClearEntries();

  ASSETBROWSER_IMPEXP hkUint32 GetNumSubEntries() const;
  ASSETBROWSER_IMPEXP hkvPropertyModelEntry* GetSubEntry(hkUint32 uiIndex) const;
  ASSETBROWSER_IMPEXP void AddSubEntry(hkvPropertyModelEntry* pEntry);

public:
  ASSETBROWSER_IMPEXP hkvPropertyModelEntry* GetParentEntry() const { return m_pParentEntry; }
  ASSETBROWSER_IMPEXP hkvProperty* GetProperty() const { return m_pProperty; }
  ASSETBROWSER_IMPEXP hkUint32 GetIndexInParent() const { return m_uiIndexInParent; }

private:
  hkvPropertyModelEntry* m_pParentEntry;
  hkvProperty* m_pProperty;

  hkUint32 m_uiIndexInParent;
  std::deque<hkvPropertyModelEntry*> m_SubEntries;
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
