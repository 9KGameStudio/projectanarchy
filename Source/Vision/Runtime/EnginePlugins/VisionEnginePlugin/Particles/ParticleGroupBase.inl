/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



///////////////////////////////////////////////////////////////////////////////////
// IVPhysicsParticleCollection_cl
///////////////////////////////////////////////////////////////////////////////////

inline IVPhysicsParticleCollection_cl::IVPhysicsParticleCollection_cl(unsigned int uiBaseSeed)
{
  m_uiBaseSeed = (uiBaseSeed!=0) ? uiBaseSeed : (Vision::Game.GetRand()+1); 
}

inline void IVPhysicsParticleCollection_cl::UpdateSeed()
{
  // Set seed based on frame, a prime number, and the base seed
 // m_generator.Reset(Vision::Game.GetUpdateSceneCount() * 137 + m_uiBaseSeed);

  // we shouldn't use GetUpdateSceneCount here because SetUpdateSceneCount might be called while this thread is running causing non-deterministic behavior
  // instead, increase the seed every frame manually. This has the advantage, that particles are deterministic even when spawned at distinct frame times.
  m_generator.Reset(m_uiBaseSeed);
  m_uiBaseSeed += 137;
}


///////////////////////////////////////////////////////////////////////////////////
// particle group
///////////////////////////////////////////////////////////////////////////////////

inline VisParticleGroupDescriptor_cl * ParticleGroupBase_cl::GetDescriptor() const
{
  return m_spDescriptor;
}

inline void ParticleGroupBase_cl::SetEmitter(VisParticleEmitter_cl *pEmitter)
{
  EnsureUpdaterTaskFinished();

  m_spEmitter = pEmitter;
  if (pEmitter)
    pEmitter->m_vLastEmitterPos = m_vPosition;
}

inline VisParticleEmitter_cl* ParticleGroupBase_cl::GetEmitter() const
{
  return m_spEmitter;
}

inline bool ParticleGroupBase_cl::RemoveConstraint(VisParticleConstraint_cl *pConstraint)
{
  EnsureUpdaterTaskFinished();

  bool bRes = m_Constraints.RemoveConstraint(pConstraint);

  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->RemoveConstraint(pConstraint);

  return bRes;
}

inline void ParticleGroupBase_cl::RemoveAllConstraints()
{
  EnsureUpdaterTaskFinished();

  m_Constraints.ReleaseAllConstraints();
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->RemoveAllConstraints();
}

inline void ParticleGroupBase_cl::SetSizeMultiplier(const hkvVec4& vSizeMultiplier)
{
  m_vSizeMultiplier = vSizeMultiplier;
}

inline hkvVec4 ParticleGroupBase_cl::GetSizeMultiplier()
{
  return m_vSizeMultiplier;
}

inline void ParticleGroupBase_cl::SetTopology(VIS_PARTICLE_TOPOLOGY_e eTopology)
{
  m_eTopology = eTopology;

  SetUseDistortion(m_eTopology != PARTICLE_TOPOLOGY_MESH && m_eTopology != PARTICLE_TOPOLOGY_BILLBOARDS);

  if (m_eTopology == PARTICLE_TOPOLOGY_TRAIL)
  {
    SetParticleCenter(0.0f, 0.5f);
  }
  else if (m_eTopology == PARTICLE_TOPOLOGY_RINGWAVE)
  {
    SetUseNormals(true);
  }
}

inline VIS_PARTICLE_TOPOLOGY_e ParticleGroupBase_cl::GetTopology() const
{
  return m_eTopology;
}

inline void ParticleGroupBase_cl::SetScaling(float fScale)
{
  if (m_fScaling != fScale)
  {
    m_fScaling = fScale;
    SetInitialTransformation(); // local space update
    UpdateBoundingBoxes();
  }
}

inline const hkvVec4* ParticleGroupBase_cl::GetDynamicLightingShaderConstants()
{
  return m_pParentEffect ? m_pParentEffect->GetDynamicLightingShaderConstants() : NULL;
}

inline VTextureObject* ParticleGroupBase_cl::GetShadowReceiveShadowMap()
{
  return (m_pParentEffect && GetShadowReceive()) ? m_pParentEffect->GetShadowReceiveShadowMap() : NULL;
}

inline hkvVec3 ParticleGroupBase_cl::GetLightSamplingOffset() const
{
  return m_pParentEffect ? m_pParentEffect->GetLightSamplingOffset() : hkvVec3::ZeroVector();
}

inline float ParticleGroupBase_cl::GetScaling() const
{
  return m_fScaling;
}


inline void ParticleGroupBase_cl::SetOrientation(float fYaw,float fPitch,float fRoll)
{
  hkvMat3 mTemp (hkvNoInitialization);
  mTemp.setFromEulerAngles (fRoll, fPitch, fYaw);
  SetOrientation(mTemp);
}

inline void ParticleGroupBase_cl::SetOrientation(const hkvMat3 &mRotMat)
{
  SetRotationMatrix(mRotMat);
}

inline void ParticleGroupBase_cl::SetOrientation(const hkvVec3& vDir, const hkvVec3& vUp, const hkvVec3& vRight)
{
  hkvMat3 mTemp (hkvNoInitialization);
  mTemp.setAxisXYZ (vDir,vRight,vUp);
  SetOrientation(mTemp);
}


inline void ParticleGroupBase_cl::SetPause(bool bStatus)
{
  m_bPaused = bStatus;
}

inline bool ParticleGroupBase_cl::IsPaused() const
{
  return m_bPaused;
}

inline void ParticleGroupBase_cl::SetHalted(bool bStatus)
{
  m_bHalted = bStatus;
}

inline bool ParticleGroupBase_cl::IsHalted() const
{
  return m_bHalted;
}


inline bool ParticleGroupBase_cl::AddParticleToCache(ParticleExt_t *pParticle)
{
  pParticle->valid = 0; // this will be checked again when returning free particle
  if (m_iCachedParticleCount>=m_iMaxCachedParticleCount)
    return false;

  // cache the index
  int iIndex = (int)(pParticle-GetParticlesExt()); // no division by particle size here :-)
  VASSERT(&GetParticlesExt()[iIndex]==pParticle);
  m_piCachedParticle[m_iCachedParticleCount++] = iIndex;
  return true;
}

inline void ParticleGroupBase_cl::InitSingleParticle(ParticleExt_t *pParticle)
{
  VASSERT(pParticle);
  m_spDescriptor->InitRandomParticle(pParticle, this);
}

inline bool ParticleGroupBase_cl::CreateParticleFromSource(const ParticleExt_t *pSrcParticle, float fTimeDelta, int iCopyFlags)
{
  ParticleExt_t *p = GetFreeParticle();
  if (!p) return false;
  m_spDescriptor->InitRandomParticle(p, this, pSrcParticle,iCopyFlags);
  HandleSingleParticle(p,fTimeDelta);
  return true;
}


inline void ParticleGroupBase_cl::SetParticleAnimFrame(ParticleExt_t *pParticle, float fFrame, bool bWrapAround)
{
  pParticle->animation_frame = (int)(fFrame);

  if(bWrapAround)
  {
    pParticle->animation_next_frame = (pParticle->animation_frame+1) % (m_iMaxAnimFrame + 1);
  }
  else
  {
    pParticle->animation_next_frame = hkvMath::Min (pParticle->animation_frame+1,m_iMaxAnimFrame);
  }
  
  pParticle->blend_weight = hkvMath::float2int ((fFrame - (float)pParticle->animation_frame)*255.99f);
}


inline bool ParticleGroupBase_cl::HandleSingleParticle(ParticleExt_t *pParticle, float fDeltaTime)
{
  // handle lifetime
  pParticle->m_fLifeTimeCounter += fDeltaTime*pParticle->m_fLifeTimeInc;
  if (VISION_UNLIKELY(pParticle->m_fLifeTimeCounter>=1.f)) // lifetime is over
  {
    if (m_bRepeatLifetime)
      pParticle->m_fLifeTimeCounter = hkvMath::mod (pParticle->m_fLifeTimeCounter,1.f);
    else
    {
      DestroyParticle(pParticle,fDeltaTime);
      return false;
    }
  }


  // assign new color
  if (VISION_LIKELY(m_pColorLookup!=NULL))
  {
    // do the color lookup at y=per particle random value, x=lifetime
    // use the non-interpolating version of LookupTexelColor because of performance considerations

    pParticle->color = m_InstanceColor * pParticle->m_ModColor * 
     m_pColorLookup[(int)(pParticle->m_fLifeTimeCounter*m_fColorBitmapSizeX)];
  }
  else
  {
    pParticle->color = m_InstanceColor * pParticle->m_ModColor;
  }

  // particle size : either from size curve of lookup or just grow size
  if (VISION_LIKELY(m_spSizeCurve!=NULL))
  {
    pParticle->size = m_spSizeCurve->GetValueFastInterpolated(pParticle->m_fLifeTimeCounter) * pParticle->m_fSizeGrowth;
  } 
  else
  {
    pParticle->size += pParticle->m_fSizeGrowth * fDeltaTime;
    if (pParticle->size<=0.f)
    {
      DestroyParticle(pParticle,fDeltaTime);
      return false;
    }
  }

  pParticle->m_vPosition += pParticle->m_vVelocity*fDeltaTime;

  if (m_bMovesWithEmitter)
  {
    float fWeight = m_fLocalFactorStart + m_fLocalFactorDiff * pParticle->m_fLifeTimeCounter;
    pParticle->m_vPosition += m_vGroupMoveDelta * fWeight;
  }

  pParticle->m_vVelocity = (pParticle->m_vVelocity + m_vFrameWind * pParticle->m_fInertiaFactor + m_vFrameWindNoInertia) * m_fFrameFriction;

  float fAnimFrame;
  // animation
  switch (m_eParticleAnimMode)
  {
  case PARTICLE_ANIMATION_REPEAT:
  case PARTICLE_ANIMATION_LIFETIME:
    pParticle->m_fAnimPos = hkvMath::mod (pParticle->m_fAnimPos+pParticle->m_fAnimInc*fDeltaTime, 1.f);
    if (m_spAnimCurve)
    {
      fAnimFrame = m_spAnimCurve->GetValueFast(pParticle->m_fAnimPos);
      SetParticleAnimFrame(pParticle,fAnimFrame, m_eParticleAnimMode == PARTICLE_ANIMATION_REPEAT);
    } 
    else
      SetParticleAnimFrame(pParticle,pParticle->m_fAnimPos*m_fAnimFrameCount, m_eParticleAnimMode == PARTICLE_ANIMATION_REPEAT);
    break;
  }
  // verify animation frame is in valid range
  VASSERT(pParticle->animation_frame < m_spDescriptor->m_iUsedAnimFrames);

  // rotation
  if (m_spRotationCurve)
  {
    pParticle->m_fRotationParam0 = hkvMath::mod (pParticle->m_fRotationParam0+pParticle->m_fRotationParam1*fDeltaTime, 1.f);
    pParticle->angle = m_spRotationCurve->GetValueFast(pParticle->m_fRotationParam0);
  } else
  {
    // constant angle speed
    if (pParticle->m_fRotationParam0 != 0.0f)
      pParticle->angle = hkvMath::mod (pParticle->angle+pParticle->m_fRotationParam0*fDeltaTime, (hkvMath::pi () * 2.0f));
  }

  if (!GetUseDistortion())
    return true;


  // particle distortion
  switch (m_eTopology)
  {
  case PARTICLE_TOPOLOGY_RINGWAVE:
    {
      hkvVec3& distort = (hkvVec3&)pParticle->distortion;

      distort.x = m_vSizeMultiplier.x * pParticle->size;
      distort.y = m_vSizeMultiplier.y * pParticle->size;
      distort.z = m_vSizeMultiplier.z * pParticle->size;

      // still unrotated, so rotate vector:
      distort = m_cachedRotMatrix.transformDirection(distort);
    }
    break;

  case PARTICLE_TOPOLOGY_STRETCH_VELOCITY: // speed multiplier
    {
      pParticle->distortion[0] = pParticle->m_fDistortionMult * pParticle->m_vVelocity[0];
      pParticle->distortion[1] = pParticle->m_fDistortionMult * pParticle->m_vVelocity[1];
      pParticle->distortion[2] = pParticle->m_fDistortionMult * pParticle->m_vVelocity[2];
    }
    break;

  case PARTICLE_TOPOLOGY_STRETCH_FIXLENGTH:
    {
      hkvVec3 dist = pParticle->m_vVelocity;

      if (!dist.isZero())
      {
        dist.setLength(pParticle->m_fDistortionMult);
        pParticle->distortion[0] = dist.x;
        pParticle->distortion[1] = dist.y;
        pParticle->distortion[2] = dist.z;
      }
    }
    break;

  case PARTICLE_TOPOLOGY_TRAIL:
    {
      // handled outside this loop
      pParticle->angle = 0.f;
    }
    break;

  case PARTICLE_TOPOLOGY_CUSTOM:
    break;

  default:
    VASSERT_MSG(FALSE, "Unsupported distortion mode");
  }

  if (m_spDescriptor->m_bDistortionPlaneAligned) // plane align the particle to always point up
  {
    hkvVec3 vRight(hkvNoInitialization);

    if (hkvMath::Abs(pParticle->m_vVelocity[0]) > hkvMath::Abs(pParticle->m_vVelocity[1]))
      vRight.set(0, 1, 0);
    else
      vRight.set(1, 0, 0);

    vRight = vRight.cross((hkvVec3&)pParticle->distortion[0]);
    vRight.normalize();
    pParticle->m_vNormal = vRight;
  }

  return true;
}

inline void ParticleGroupBase_cl::SetVisible(bool bStatus)
{
  if (bStatus) 
    SetVisibleBitmask(0xFFFFFFFF); 
  else 
    SetVisibleBitmask(0);

  // also activate/deactivate the visibility object.
  // Make sure this only happens when visible status changes, because SetActivate resets the test counters
  // currently the particle effect takes care for this
  VisVisibilityObject_cl *pVisObj = GetVisibilityObject();
  if (pVisObj)
    pVisObj->SetActivate(bStatus);

  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->SetVisible(bStatus);
}

inline const hkvAlignedBBox& ParticleGroupBase_cl::BoundingBox() const
{
  return GetUseLocalSpaceMatrix() ? GetLocalSpaceBoundingBox() : GetWorldSpaceBoundingBox();
}

inline const hkvAlignedBBox& ParticleGroupBase_cl::GetLocalSpaceBoundingBox() const
{
	return m_LocalSpaceBoundingBox;
}

inline const hkvAlignedBBox& ParticleGroupBase_cl::GetWorldSpaceBoundingBox() const
{
	return m_WorldSpaceBoundingBox;
}

inline void ParticleGroupBase_cl::RenderBoundingBox(VColorRef iColor)
{
	UpdateBoundingBoxes();
	if (GetUseLocalSpaceMatrix())
		Vision::Game.DrawBoundingBox(GetLocalSpaceBoundingBox(), iColor, 1.f);
	else
		Vision::Game.DrawBoundingBox(GetWorldSpaceBoundingBox(), iColor, 1.f);
}

inline const hkvAlignedBBox *ParticleGroupBase_cl::CalcCurrentBoundingBox()
{
	UpdateBoundingBoxes();
	return GetUseLocalSpaceMatrix() ? &GetLocalSpaceBoundingBox() : &GetWorldSpaceBoundingBox();
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
