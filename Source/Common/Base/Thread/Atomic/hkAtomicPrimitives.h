/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_ATOMIC_PRIMITIVES_H
#define HK_ATOMIC_PRIMITIVES_H

namespace hkAtomic
{
	/// Compare and swap. Performs the following operation atomically:
	///		if ( *address == oldValue )
	///		{
	///			*address = newValue;
	///			return true;
	///		}
	///		return false;
	template <typename T>
	HK_FORCE_INLINE bool HK_CALL compareAndSwap(T volatile* address, const T& oldValue, const T& newValue);
}

#ifndef __HAVOK_PARSER__
#	if		defined(HK_PLATFORM_WIN32) || \
			defined(HK_PLATFORM_XBOX360)
#		include <Common/Base/Thread/Atomic/hkAtomicPrimitives_Win32.inl>
#	elif	defined(HK_PLATFORM_PSVITA) || \
			defined(HK_PLATFORM_PS3)	|| \
			defined(HK_PLATFORM_ANDROID)|| \
			defined(HK_PLATFORM_TIZEN)	|| \
			defined(HK_PLATFORM_PS4)	|| \
			defined(HK_PLATFORM_LINUX)
#		include <Common/Base/Thread/Atomic/hkAtomicPrimitives_Gcc.inl>
#	elif	defined(HK_PLATFORM_RVL)
#		include <Common/Base/Thread/Atomic/hkAtomicPrimitives_Wii.inl>
#	elif	defined(HK_PLATFORM_WIIU)
#		include <Common/Base/Thread/Atomic/hkAtomicPrimitives_WiiU.inl>
#	elif	defined(HK_PLATFORM_IOS)	|| \
			defined(HK_PLATFORM_MACPPC)	|| \
			defined(HK_PLATFORM_MAC386)
#		include <Common/Base/Thread/Atomic/hkAtomicPrimitives_Mac.inl>			
#	else
#		error Missing implementation of atomic primitives!!
#	endif
#endif

#endif	// HK_ATOMIC_PRIMITIVES_H

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
