/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPostProcessTranslucencies.hpp

#ifndef POSTPROCESS_UPSCALE_HPP_INCLUDED
#define POSTPROCESS_UPSCALE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>

/// \brief
///   Post-processor used internally by the VMobileForwardRenderingSystem to upscale the offscreen buffer contents into the back buffer.
class VPostProcessUpscale : public VPostProcessingBaseComponent
{
public:
  /// \brief Constructor
  VPostProcessUpscale(bool bUseFiltering);

  /// \brief Destructor
  ~VPostProcessUpscale();

  /// \brief Destructor
  EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;

  /// \brief Initialization
  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;

  /// \brief Deinitialization
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;

  /// \brief Returns true if \c pObject is of type \c VMobileForwardRenderingSystem.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;

public:
  // VTypedObject
  V_DECLARE_DYNAMIC_DLLEXP(VPostProcessUpscale, EFFECTS_IMPEXP)

private:
  bool m_bUseFiltering;
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
