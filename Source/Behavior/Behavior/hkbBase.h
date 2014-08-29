/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef hkbBase_H
#define hkbBase_H

#define HK_PTR_TO_hkUint64( ptr ) (hkUint64)( (hkUlong)( ptr ) )

#define HKB_COPY_MEMBERS( dest, source, TYPE, m_firstMember, m_lastMember ) \
{ \
	int startOffset = HK_OFFSET_OF( TYPE, m_firstMember ); \
	int endOffset = HK_OFFSET_OF( TYPE, m_lastMember ) + sizeof(m_lastMember); \
	int numBytes = endOffset - startOffset; \
	hkString::memCpy( &(dest).m_firstMember, &(source).m_firstMember, numBytes ); \
}

// internalClassVersion should be the version of the InternalClass that will be generated
// This should be the latest version as defined by the patches
#define HKB_BEGIN_INTERNAL_STATE( internalClassVersion ) \
	public: \
	virtual hkReferencedObject* createInternalState() HK_OVERRIDE; \
	virtual void getInternalState( hkReferencedObject& internalState ) const HK_OVERRIDE; \
	virtual void setInternalState( const hkReferencedObject& internalState ) HK_OVERRIDE; \
	protected:

#define HKB_END_INTERNAL_STATE()

#define HKB_TOOL_ASSERT( id, condition, str ) if(!(condition)) { HK_WARN_ALWAYS( (id | 0xabe00000), str ); HK_ASSERT( (id | 0xabe00000), false ); }

/// It can sometimes be helpful to represent a boolean as an hkReal for blending.
/// This struct encapsulates such bools to unify our use/access of such bools across the SDK.

struct hkBinaryReal
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkBinaryReal );

		enum RoundMode
		{
			ROUND,
			CEILING,
			FLOOR,
		};

		// Constructors

		HK_FORCE_INLINE hkBinaryReal() : m_storage() {}
		HK_FORCE_INLINE hkBinaryReal(bool b) : m_storage((b) ? 1.f : 0.f) {}
		HK_FORCE_INLINE hkBinaryReal(hkReal r) : m_storage(r) {}

		// Operators

		HK_FORCE_INLINE hkBinaryReal& operator=(bool b) { m_storage = (b) ? 1.f : 0.f; return *this; }
		HK_FORCE_INLINE hkBinaryReal& operator=(hkReal r) { m_storage = r; return *this; }
		HK_FORCE_INLINE operator hkReal&() { return m_storage; }
		HK_FORCE_INLINE operator hkReal() const { return m_storage; }
		HK_FORCE_INLINE operator hkSimdFloat32() const { hkSimdFloat32 r; r.setFromFloat(m_storage); return r; }

		template <RoundMode R> HK_FORCE_INLINE hkBool32 getAsBool() const;

	private:

		hkReal m_storage;

	public:

		hkBinaryReal( hkFinishLoadedObjectFlag flag ) {}
};

template <> HK_FORCE_INLINE hkBool32 hkBinaryReal::getAsBool<hkBinaryReal::ROUND>() const { return (m_storage >= 0.5f); }
template <> HK_FORCE_INLINE hkBool32 hkBinaryReal::getAsBool<hkBinaryReal::CEILING>() const { return (m_storage >= 0.0001f); }
template <> HK_FORCE_INLINE hkBool32 hkBinaryReal::getAsBool<hkBinaryReal::FLOOR>() const { return (m_storage > 0.9999f); }

// Some defines that are used to offset various behavior determinism check values to ensure they don't falsly match-up
// and we catch the diversion as quickly as possible

#define HKB_DETERMINISTIC_EVENT_COUNT_BASE			1000
#define HKB_DETERMINISTIC_EVENT_EXTERNAL_ID_BASE	2000
#define HKB_DETERMINISTIC_EVENT_INTERNAL_ID_BASE	0
#define HKB_DETERMINISTIC_NODE_COUNT_BASE			300
#define HKB_DETERMINISTIC_NODE_ID_BASE				0

#endif // hkbBase_H

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
