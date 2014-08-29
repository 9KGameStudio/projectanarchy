/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline void VisParticleEffect_cl::SetRemoveWhenFinished(bool bStatus)
{
 m_bRemoveWhenFinished = bStatus;
}

inline bool VisParticleEffect_cl::GetRemoveWhenFinished() const
{
  return m_bRemoveWhenFinished;
}

inline void VisParticleEffect_cl::Dispose()
{
  DisposeObject();
}

inline bool VisParticleEffect_cl::IsDead() const
{
  return m_bRemoveWhenFinished && (m_iGroupCount==0);
}

inline int VisParticleEffect_cl::GetParticleGroupCount() const
{
  return m_iGroupCount;
}

inline ParticleGroupBase_cl *VisParticleEffect_cl::GetParticleGroup(int iIndex) const
{
  return m_spGroups[iIndex];
}



inline unsigned int VisParticleEffect_cl::GetLightInfluenceBitMask() const
{
  return m_uiLightInfluenceBitmask;
}

inline void VisParticleEffect_cl::SetLightInfluenceBitMask(unsigned int uiLightMask)
{
  m_uiLightInfluenceBitmask = uiLightMask;
}

inline void VisParticleEffect_cl::SetLightSamplingOffset(const hkvVec3& vLightSamplingOffset)
{
  m_vLightSamplingOffset = vLightSamplingOffset;
  EvaluateSceneBrightness();
  UpdateLightGrid();
}

inline hkvVec3 VisParticleEffect_cl::GetLightSamplingOffset() const
{
  return m_vLightSamplingOffset;
}

inline void VisParticleEffect_cl::SetDynamicLightingPriorityThreshhold(unsigned int uiDynamicLightingPriorityThreshhold)
{
  m_uiDynamicLightingPriorityThreshhold = uiDynamicLightingPriorityThreshhold;
}

inline unsigned int VisParticleEffect_cl::GetDynamicLightingPriorityThreshhold() const
{
  return m_uiDynamicLightingPriorityThreshhold;
}

inline const hkvVec3* VisParticleEffect_cl::GetCachedLightgridColors() const
{
  return m_OwnLGColors;
}

inline bool VisParticleEffect_cl::GetUseLightgrid() const
{
  return m_bUseLightgridLighting;
}

inline bool VisParticleEffect_cl::GetUseDynamicLighting() const
{
  return m_bUseDynamicLighting;
}

inline const hkvVec4* VisParticleEffect_cl::GetDynamicLightingShaderConstants()
{
  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  if(m_pLastDynamicLightListUpdateContext != pContext || pContext->GetLastRenderedFrame() != m_uiLastDynamicLightListUpdateFrame)
  {
    if(m_bUseSHForDynamicLighting)
      UpdateDynamicLightSH();
    else
      UpdateDynamicLightLists();
  }

  return m_pDynamicLightingShaderConstants;
}

inline VTextureObject* VisParticleEffect_cl::GetShadowReceiveShadowMap()
{
  return m_spShadowReceiveShadowMap;
}

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
