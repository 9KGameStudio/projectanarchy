/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_CARVER_H
#define HKAI_CARVER_H

#include <Common/Base/Types/hkRefPtr.h>
#include <Ai/Pathfinding/Common/hkaiVolume.h>

extern HK_EXPORT_AI const class hkClass hkaiCarverClass;


class HK_EXPORT_AI hkaiCarver : public hkReferencedObject
{
		//+version(1)
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		enum FlagBits
		{
			CARVER_ERODE_EDGES = 1
		};


		hkaiCarver(hkFinishLoadedObjectFlag f);

		hkaiCarver(const hkaiVolume* volume);
		virtual ~hkaiCarver();

		HK_FORCE_INLINE const hkaiVolume* getVolume() const { return m_volume; }

		void setVolume(const hkaiVolume* volume);

		inline bool shouldErodeEdges() const { return m_flags.anyIsSet(CARVER_ERODE_EDGES); }
		void setErodeEdges( bool erode );

	private:

		hkRefPtr<const hkaiVolume> m_volume;
		hkFlags< FlagBits, hkUint32 > m_flags; //+default(hkaiCarver::CARVER_ERODE_EDGES)
};

#endif // HKAI_CARVER_H

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
