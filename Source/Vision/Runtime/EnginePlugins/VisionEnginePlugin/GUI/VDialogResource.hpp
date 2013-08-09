/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDialogResource.hpp

#ifndef VDIALOGRESOURCE_HPP_INCLUDED
#define VDIALOGRESOURCE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/IVMenuSystemResource.hpp>
class VGUIManager;


/// \brief
///   Class for storing a dialog resource. Can create a dialog instance
class VDialogResource : public IVMenuSystemResource
{
public:

  /// \brief
  ///   Resource constructor
  GUI_IMPEXP VDialogResource(VGUIManager *pManager, const char *szFilename);

  /// \brief
  ///   Create a dialog instance using this resource
  GUI_IMPEXP VDialog *CreateInstance(IVGUIContext *pContext, VDialog *pParent, int iFlags=DIALOGFLAGS_NONE);
public:
  GUI_IMPEXP virtual BOOL Reload();
  GUI_IMPEXP virtual BOOL Unload();
  GUI_IMPEXP virtual bool Parse(TiXmlElement *pNode, const char *szPath);
private:
  TiXmlElement *m_pXMLNode;
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20130723)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
