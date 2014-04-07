/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef HAVOK_AI_ENGINEPLUGIN_HPP
#define HAVOK_AI_ENGINEPLUGIN_HPP

#if !defined(AFX_STDAFX_H__16731FC2_4597_4F18_95B2_B105710FA38F__INCLUDED_)
#define AFX_STDAFX_H__16731FC2_4597_4F18_95B2_B105710FA38F__INCLUDED_

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(WIN32)

	#ifndef WIN32_LEAN_AND_MEAN		
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#endif

	#include <crtdbg.h>

	//TODO REmove references to vHavokAiUtil when moved functionality to module.
	#ifdef VHAVOKAIUTIL_PLUGIN_EXPORTS
		#define VHAVOKAIUTIL_IMPEXP __declspec(dllexport)
	#else
		#define VHAVOKAIUTIL_IMPEXP __declspec(dllimport)
	#endif 

#else
	#define VHAVOKAIUTIL_IMPEXP
#endif




#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>


#undef swap16
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__06731FC2_4597_4F18_95B2_B105710FA38E__INCLUDED_)

#endif

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
