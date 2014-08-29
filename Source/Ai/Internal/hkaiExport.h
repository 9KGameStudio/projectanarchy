/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_AI_EXPORT__H
#define HKAI_AI_EXPORT__H

#ifndef HK_EXPORT_AI
	#if defined(HK_DYNAMIC_DLL)
	#	if defined(HK_AI_BUILD) // DLL export for Ai product
	#		define HK_EXPORT_AI __declspec(dllexport)
	#		define HK_EXPORT_AITEMPLATE_SPECIALIZATION 
	#	else // Being used in a non Ai dll build, but still a dll use, so import
	#		define HK_EXPORT_AI __declspec(dllimport)
	#		define HK_EXPORT_AI_TEMPLATE_SPECIALIZATION extern // see http://support.microsoft.com/kb/168958
	#	endif
	#else
	#	define HK_EXPORT_AI
	#	define HK_EXPORT_AI_TEMPLATE_SPECIALIZATION
	#endif
#endif

#endif //HKAI_AI_EXPORT__H

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
