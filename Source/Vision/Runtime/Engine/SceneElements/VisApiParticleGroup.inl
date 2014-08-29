/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiParticleGroup.inl

extern "C" bool g_bSupportsInstancing;


inline void VisParticleGroup_cl::SetParticleStride(short iStructStride, short iStructOfs)
{
  VASSERT(iStructStride>=sizeof(Particle_t));
  m_iStructStride = iStructStride;
  m_iStructOfs = iStructOfs;
}

inline void VisParticleGroup_cl::SetAnimationSubdivisions(unsigned int iSubDivX, unsigned int iSubDivY)
{
  m_iAnimSubDivX = iSubDivX;
  m_iAnimSubDivY = iSubDivY;
  UpdateDefaultShaderFlags();
}

inline int VisParticleGroup_cl::GetAnimationSubdivisionsX() const
{
  return m_iAnimSubDivX;
}

inline int VisParticleGroup_cl::GetAnimationSubdivisionsY() const
{
  return m_iAnimSubDivY;
}

inline void VisParticleGroup_cl::SetUseSmoothAnimationFading(bool bStatus)
{
  m_bUseSmoothAnimation = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetUseSmoothAnimationFading() const
{
  return m_bUseSmoothAnimation;
}

inline void VisParticleGroup_cl::SetDepthOffset(float fScale)
{
  VASSERT(fScale>-1.f && fScale<1.f);
  m_fDepthOffset = fScale;
}

inline float VisParticleGroup_cl::GetDepthOffset() const
{
  return m_fDepthOffset;
}

inline void VisParticleGroup_cl::SetUseLocalSpaceMatrix(bool bStatus)
{
  m_bUseLocalSpaceMatrix = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetUseLocalSpaceMatrix() const
{
  return m_bUseLocalSpaceMatrix;
}


inline void VisParticleGroup_cl::SetRenderSoftParticles(bool bStatus)
{
  m_bSoftParticles = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetRenderSoftParticles() const
{
  return m_bSoftParticles;
}

inline void VisParticleGroup_cl::SetLightingStatic(bool bStatus)
{
  m_bLightingStatic = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetLightingStatic() const
{
  return m_bLightingStatic;
}

inline void VisParticleGroup_cl::SetLightingDynamic(bool bStatus)
{
  m_bLightingDynamic = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetLightingDynamic() const
{
   return m_bLightingDynamic;
}

inline void VisParticleGroup_cl::SetParticleSizeAspectRatio(float fAspectRatio)
{
  m_fSizeAspect = fAspectRatio;
}

inline float VisParticleGroup_cl::GetParticleSizeAspectRatio() const
{
  return m_fSizeAspect;
}

inline void VisParticleGroup_cl::SetBacklightingScale(float fBacklightingScale)
{
  m_fBacklightingScale = fBacklightingScale;
}

inline float VisParticleGroup_cl::GetBacklightingScale() const
{
  return m_fBacklightingScale;
}

inline VisVisibilityObject_cl* VisParticleGroup_cl::GetVisibilityObject() const
{
  return m_spVisObject;
}

inline void VisParticleGroup_cl::SetShadowReceive(bool bStatus)
{
  m_bShadowReceive = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetShadowReceive() const
{
  return m_bShadowReceive;
}

inline void VisParticleGroup_cl::SetUseNormalFromDiffAlpha(bool bStatus)
{
  m_bUseNormalFromDiffAlpha = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetUseNormalFromDiffAlpha() const
{
  return m_bUseNormalFromDiffAlpha;
}

inline void VisParticleGroup_cl::SetVisibilityObject(VisVisibilityObject_cl* pVisObj)
{
  if (pVisObj == m_spVisObject)
    return;
  if (m_spVisObject != NULL)
    m_spVisObject->RemoveObject3D(this);
  m_spVisObject=pVisObj;
  if (pVisObj != NULL)
    m_spVisObject->AddObject3D(this);
}

inline const char *VisParticleGroup_cl::GetTexture() const
{
  if (!spTexture) return NULL;
  return spTexture->GetFilename();
}

inline void VisParticleGroup_cl::TriggerTextureAnim()
{
  if (texanim_id)
    texanim_id->SetTriggerStatus(TRUE);
}


inline void VisParticleGroup_cl::ResetTextureAnim()
{
  if (texanim_id)
    texanim_id->Reset();
}


inline float VisParticleGroup_cl::GetTextureAnimPhase()
{
  if (texanim_id)
    return texanim_id->GetCurrentPhase();
  return 0.f;
}


inline int VisParticleGroup_cl::GetTextureAnimNumOfPhases()
{
  if (texanim_id)
    texanim_id->GetNumberOfFrames();
  return 0;
}


inline void VisParticleGroup_cl::SetTextureAnimPhase(float newPhase)
{  
  if (texanim_id)
    texanim_id->SetAnimPhase(newPhase);
}


inline void VisParticleGroup_cl::IncTextureAnimPhase(float phaseDelta)
{  
  if (texanim_id)
    texanim_id->IncAnimPhase(phaseDelta);
}

inline VisTextureAnimInstance_cl* VisParticleGroup_cl::GetTextureAnimationInstance() const
{
  return texanim_id;
}


inline void VisParticleGroup_cl::SetAlwaysInForeGround(BOOL status)
{
  if (status)
  {
    m_iRenderState.SetFlag(RENDERSTATEFLAG_ALWAYSVISIBLE);
  }
  else
  {
    m_iRenderState.RemoveFlag(RENDERSTATEFLAG_ALWAYSVISIBLE);
  }

  UpdateOcclusionCullingSettings();
}

inline void VisParticleGroup_cl::SetUseOcclusionCulling(bool bStatus)
{
  m_bUseOcclusionCulling = bStatus;

  UpdateOcclusionCullingSettings();
}

inline bool VisParticleGroup_cl::GetUseOcclusionCulling() const
{
  return m_bUseOcclusionCulling;
}

inline void VisParticleGroup_cl::SetTransparencyType(VIS_TransparencyType newTransparencyType)
{
  m_iRenderState.SetTransparency(newTransparencyType);
  int iOpaqueFlags = RENDERSTATEFLAG_WRITETOZBUFFER | RENDERSTATEFLAG_ALPHATEST | RENDERSTATEFLAG_USEFOG;
  if (newTransparencyType == VIS_TRANSP_ALPHATEST || newTransparencyType == VIS_TRANSP_NONE)
    m_iRenderState.SetFlag(iOpaqueFlags);
  else
    m_iRenderState.RemoveFlag(iOpaqueFlags);
}


inline VIS_TransparencyType VisParticleGroup_cl::GetTransparencyType() const
{
  return (VIS_TransparencyType)m_iRenderState.GetTransparency();
}

inline int VisParticleGroup_cl::GetNumOfParticles() const 
{ 
  return numParticles; 
}


inline Particle_t *VisParticleGroup_cl::GetParticles() const
{ 
  return (Particle_t *)particles;
} 


inline void VisParticleGroup_cl::SetTextureRepetitions(float numOfRepetitions)
{
  m_fNumTexRepetitions = numOfRepetitions;
}

inline float VisParticleGroup_cl::GetTextureRepetitions() const
{
  return m_fNumTexRepetitions;
}

inline void VisParticleGroup_cl::SetBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
  SetVisibilityBoundingBox(GetUseLocalSpaceMatrix(), hkvAlignedBBox(hkvVec3(minX, minY, minZ), hkvVec3(maxX, maxY, maxZ)));
}

inline BOOL VisParticleGroup_cl::GetCheckVisibility() const
{
  return m_bUseVisibility ? TRUE : FALSE;
}

inline void VisParticleGroup_cl::SetBoundingBox(const hkvAlignedBBox & bbox)
{
  SetVisibilityBoundingBox(GetUseLocalSpaceMatrix(), bbox);
}

inline void VisParticleGroup_cl::SetTechnique(VCompiledTechnique *pTechnique)
{
  m_spCustomTechnique = pTechnique;
  m_spLastShader = NULL;
}

inline VCompiledTechnique *VisParticleGroup_cl::GetTechnique() const
{
  return m_spCustomTechnique;
}

inline void VisParticleGroup_cl::SetUseNormals(bool bStatus)
{
  m_bUseNormals = bStatus;
}

inline bool VisParticleGroup_cl::GetUseNormals() const
{
  return m_bUseNormals;
}

inline void VisParticleGroup_cl::SetUseDistortion(bool bStatus)
{
  m_bUseDistortion = bStatus;
}

inline bool VisParticleGroup_cl::GetUseDistortion() const
{
  return m_bUseDistortion;
}

inline void VisParticleGroup_cl::SetTessellationEnabled(bool bStatus)
{
  m_bUseTesselation = bStatus;
  UpdateDefaultShaderFlags();
}

inline bool VisParticleGroup_cl::GetTessellationEnabled() const
{
  return m_bUseTesselation;
}

inline void VisParticleGroup_cl::SetTessellationFactorPixelPerVertex(float fTessellationFactorPixelPerVertex)
{
  m_fTessellationFactorPixelPerVertex = fTessellationFactorPixelPerVertex;
}

inline float VisParticleGroup_cl::GetTessellationFactorPixelPerVertex() const
{
  return m_fTessellationFactorPixelPerVertex;
}

inline hkvVec3 VisParticleGroup_cl::GetLightSamplingOffset() const
{
  return hkvVec3::ZeroVector();
}

inline void VisParticleGroup_cl::SetParticleCenter(float x, float y)
{
  m_vParticleCenter.set(x,y);
}

inline void VisParticleGroup_cl::SetParticleCenter(const hkvVec2& center)
{
  m_vParticleCenter = center;
}

inline void VisParticleGroup_cl::GetParticleCenter(float &x, float &y) const
{
  x = m_vParticleCenter.x;
  y = m_vParticleCenter.y;
}

inline hkvVec2 VisParticleGroup_cl::GetParticleCenter() const
{
  return m_vParticleCenter;
}

inline bool VisParticleGroup_cl::GetHardwareSpanning() const
{
  return m_bUseHardwareSpanning;
}

inline ParticleSort_t *VisParticleGroup_cl::GetParticleIndexList() const
{ 
  return m_pIndexList;
} 

inline void VisParticleGroup_cl::SetSortParticles(bool bStatus)
{
  m_bSortParticles = bStatus;
}

inline bool VisParticleGroup_cl::GetSortParticles() const
{
  return m_bSortParticles;
}


inline void VisParticleGroup_cl::SetUserSortKey(int iUserSortKey)
{
  m_iUserSortKey = iUserSortKey;
}

inline int VisParticleGroup_cl::GetUserSortKey()
{
  return m_iUserSortKey;
}


inline unsigned int VisParticleGroup_cl::GetRenderOrder() const
{
  return m_iRenderOrder;
}

inline void VisParticleGroup_cl::SetVisibleBitmask(unsigned int iBitmask)
{
  m_iVisibleBitmask=iBitmask;
}

inline unsigned int VisParticleGroup_cl::GetVisibleBitmask() const
{
  return m_iVisibleBitmask;
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
