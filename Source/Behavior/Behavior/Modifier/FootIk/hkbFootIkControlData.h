/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_CONTROL_DATA_H
#define HKB_FOOT_IK_CONTROL_DATA_H

#include <Behavior/Behavior/Modifier/FootIk/hkbFootIkGains.h>

extern const class hkClass hkbFootIkControlDataClass;

#define HKB_FOOT_IK_MAX_FEET 8

/// The controls that influence the behavior of an hkbFootIkModifier.
struct hkbFootIkControlData
{
	// +version(1)
	public:
		
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbFootIkControlData );
		HK_DECLARE_REFLECTION();

		hkbFootIkControlData() {}

		// We align this because by doing so, the compiler always pads the object to fit into a 16-byte aligned block,
		// which is helpful when we put these into tracks.

			/// foot Ik gains.
		HK_ALIGN16(struct hkbFootIkGains m_gains); //+hk.Description("The gains that smooth out the foot IK.")

		hkBinaryReal getEnabled(int footIndex)
		{
			HK_ASSERT2(0xb8cd3ec, footIndex < HKB_FOOT_IK_MAX_FEET, "Foot index out of range for control data enabled access." );
			return m_enabled[footIndex];
		}

		void setEnabled(int footIndex, hkReal val)
		{
			HK_ASSERT2(0x77ded37a, footIndex < HKB_FOOT_IK_MAX_FEET, "Foot index out of range for control data enabled access." );
			m_enabled[footIndex] = val;
		}

	protected:

		hkReal m_enabled[HKB_FOOT_IK_MAX_FEET];	//+hk.Description("Whether or not each foot is enabled.")
												//+hkb.RoleAttribute("ROLE_DEFAULT", "FLAG_HIDDEN")

	public:
		
		hkbFootIkControlData( hkFinishLoadedObjectFlag flag ) : m_gains(flag) {}
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
