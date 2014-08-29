/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvCollectionPanelDlg.hpp

#ifndef HKVCOLLECTIONPANELDLG_HPP_INCLUDED
#define HKVCOLLECTIONPANELDLG_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/Wrapper/hkvQtWindowContainer.hpp>

class private_hkvCollectionPanel;

/// \brief
///   This is the interface for the managed layer. This allows us to not include all of Qt
///   on the managed side.
class hkvCollectionPanel : public hkvQtWindowContainer
{
public: // public functions
  ASSETBROWSER_IMPEXP hkvCollectionPanel();
  ASSETBROWSER_IMPEXP ~hkvCollectionPanel();

protected:
  virtual void* QtWindowCreate_private() HK_OVERRIDE;

private: // private data
  private_hkvCollectionPanel* pData;
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
