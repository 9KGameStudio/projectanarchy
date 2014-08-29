/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessTranslucencies.hpp>
#include <Vision/Runtime/Base/Types/VType.hpp>

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=178
#endif

#if defined(__ghs__)
#pragma ghs nowarning 177
#endif

static const int PARTICLEGROUPBASE_VERSION_000      = 0;      // initial version
static const int PARTICLEGROUPBASE_VERSION_001      = 1;      // changed class hierarchy (particles now derived from VisObject3D_cl)
static const int PARTICLEGROUPBASE_VERSION_002      = 2;      // ambient color
static const int PARTICLEGROUPBASE_VERSION_003      = 3;      // visible bitmask
static const int PARTICLEGROUPBASE_VERSION_004      = 4;      // individual wind speed
static const int PARTICLEGROUPBASE_VERSION_005      = 5;      // wind speed in local space
static const int PARTICLEGROUPBASE_VERSION_006      = 6;      // Time-Of-Day Light state
static const int PARTICLEGROUPBASE_VERSION_007      = 7;      // Emitter mesh entity
static const int PARTICLEGROUPBASE_VERSION_008      = 8;      // Emitter intensity
static const int PARTICLEGROUPBASE_VERSION_009      = 9;      // Visibility object flag serialization
static const int PARTICLEGROUPBASE_VERSION_010      = 10;     // Added "alwaysVisible" property
#define PARTICLEGROUPBASE_VERSION_CURRENT   PARTICLEGROUPBASE_VERSION_010

#if defined(__SNC__)
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif

VisParticleLayerEventData_cl::VisParticleLayerEventData_cl(ParticleGroupBase_cl *pLayer, VisAnimEvent_cl *pEvent)
    : IVisCallbackDataObject_cl(&VisParticleGroupManager_cl::OnLayerEvent)
{
  m_pLayer = pLayer;
  m_pEvent = pEvent;
}


V_IMPLEMENT_DYNCREATE( HandleParticlesTask_cl, VThreadedTask, &g_VisionEngineModule );

HandleParticlesTask_cl::HandleParticlesTask_cl(ParticleGroupBase_cl *pGroup) : VThreadedTask()
{
  m_fTimeDelta = 0.f;
  VASSERT(pGroup != NULL);
  m_pParticleGroup = pGroup;
}

// compare function for qsort
static int CompareParticles(const void *arg1, const void *arg2)
{
  ParticleSort_t *pSort1 = (ParticleSort_t *)arg1;
  ParticleSort_t *pSort2 = (ParticleSort_t *)arg2;
  return (int)pSort2->sortkey - (int)pSort1->sortkey;
}

void HandleParticlesTask_cl::Run(VManagedThread *pThread)
{
  // Update the seed value for this particle, this thread, this frame
  m_pParticleGroup->UpdateSeed();
  
  int i;
  float fScaledTime = m_fTimeDelta * m_pParticleGroup->m_fTimeScale;

  int &iHighWaterMark = m_pParticleGroup->m_iHighWaterMark;
  ParticleExt_t *p;
    
  // if the scene-brightness should be used and the time-of-day component is used too, check if it needs to be updated
  if ((m_pParticleGroup->m_bApplyTimeOfDayLight) && (m_pParticleGroup->m_spDescriptor->m_fApplySceneBrightness > 0.0f) && (m_pParticleGroup->m_fLastTimeOfDayUpdate >= 0.0f))
  {
		IVTimeOfDay* tod =  Vision::Renderer.GetTimeOfDayHandler();
		if(tod != NULL)
		{
      if (m_pParticleGroup->m_fLastTimeOfDayUpdate != tod->GetDayTime ())
        m_pParticleGroup->m_bEvaluateBrightnessNextFrame = true;
    }
  }

  // setup global lighting color
  if (m_pParticleGroup->m_bEvaluateBrightnessNextFrame)
    m_pParticleGroup->EvaluateSceneBrightness();

  // setup per frame variables
  m_pParticleGroup->SetPerFrameConstants(fScaledTime); // called after emitter handling (which might change it)

  // now move all particles
  const int iCount = iHighWaterMark; // m_pParticleGroup->GetNumOfParticles();
  iHighWaterMark = 0;
  p = m_pParticleGroup->GetParticlesExt();
  int iValidCount = 0;
  for (i=0;i<iCount;i++,p++) if (p->valid)
  {
    if (!m_pParticleGroup->HandleSingleParticle(p, fScaledTime))
      continue;
    iHighWaterMark = i+1;
    iValidCount++;
  }


  m_pParticleGroup->m_iValidCount = iValidCount;

  // spawn new particles (after handling)
  if (m_pParticleGroup->GetEmitter() && !m_pParticleGroup->m_bPaused && !m_pParticleGroup->IsLifeTimeOver())
    m_pParticleGroup->GetEmitter()->HandleEmitter(m_pParticleGroup,fScaledTime,m_pParticleGroup->m_iRemainingParticleCount);

  // handle constraints
  if (m_pParticleGroup->m_bHandleConstraints)
    m_pParticleGroup->HandleAllConstraints(fScaledTime);

  // sort the particles according to camera distance if requested
  if (m_pParticleGroup->GetSortParticles())
  {
    const VisContextCamera_cl *pCam = VisRenderContext_cl::GetMainRenderContext()->GetCamera();
    const hkvVec3& vCamPos = pCam->GetPosition();
    const hkvVec3& vCamDir = pCam->GetDirection();
    hkvVec3 vTemp;
    float fDist;
    
    ParticleSort_t *pSort;
    pSort = m_pParticleGroup->m_pIndexList;
    VASSERT(pSort);

    ParticleExt_t *particles;
    particles = m_pParticleGroup->GetParticlesExt();
    int iIndexCount = iHighWaterMark;
    if (iIndexCount>65536) iIndexCount=65536;
    for (i=0;i<iHighWaterMark;i++,pSort++)
    {
      p = particles + pSort->index;
      if (p->valid)
      {
        vTemp = p->m_vPosition - vCamPos;
        fDist = vTemp.dot (vCamDir);
        if (fDist>65535.f) fDist=65535.f;
        pSort->sortkey = (unsigned short)fDist;
      }
    }

    qsort(m_pParticleGroup->m_pIndexList, iIndexCount, sizeof(ParticleSort_t), CompareParticles);
  }
  // if this particle group has a child, then its task should be handled in this thread
  if (m_pParticleGroup->m_spOnDestroyCreateGroup) {
    m_pParticleGroup->m_spOnDestroyCreateGroup->m_pHandlingTask->Run(pThread);
  }

  // trail particles -> connect all (needs to be performed after all constraints etc. that might modify position)
  if (m_pParticleGroup->m_eTopology == PARTICLE_TOPOLOGY_TRAIL)
  {
    p = m_pParticleGroup->GetParticlesExt();
    const float fOverlap = m_pParticleGroup->m_spDescriptor->m_fTrailOverlap;
    for (i=0;i<iCount;i++) if (p[i].valid)
    {
      int iPrevIndex = (int)p[i].m_fDistortionMult; // stores the index to the previous...
      VASSERT(iPrevIndex>=0 && iPrevIndex<m_pParticleGroup->GetNumOfParticles());
      const ParticleExt_t *pOther = &p[iPrevIndex];
      if (pOther->valid) // the chain might be interrupted when particles have different lifetimes
      {
        p[i].distortion[0] = (pOther->m_vPosition[0] - p[i].m_vPosition[0]) * fOverlap;
        p[i].distortion[1] = (pOther->m_vPosition[1] - p[i].m_vPosition[1]) * fOverlap;
        p[i].distortion[2] = (pOther->m_vPosition[2] - p[i].m_vPosition[2]) * fOverlap;

        iPrevIndex = (int)pOther->m_fDistortionMult; // stores the index to the previous...
        VASSERT(iPrevIndex>=0 && iPrevIndex<m_pParticleGroup->GetNumOfParticles());
        const ParticleExt_t *pOther2 = &p[iPrevIndex];
        // in order to be able make a continuous trail we need information about the next particle as well
        if (pOther2->valid)
        {
          p[i].m_vNormal = (pOther2->m_vPosition - pOther->m_vPosition) * fOverlap;
        }

      }
    }
  }

  // update internal bounding box:
  m_pParticleGroup->m_fParticleBoundingBoxUpdateTimePos += m_fTimeDelta;
  m_pParticleGroup->UpdateBoundingBoxes(false);
}



///////////////////////////////////////////////////////////////////////////////////
// Particle group base
///////////////////////////////////////////////////////////////////////////////////


void ParticleGroupBase_cl::InitGroup(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, float fScaling, bool bSpawnParticles, int iGeneration)
{
  VASSERT(pDescr);

  UpdateSeed(); // For Initializing random values using seed when particle created

  // Set various default values
  m_iChildIndex = 0;
  m_bInertiaAffectsGravity = true;
  m_pColorLookup = NULL;
  m_iGeneration = iGeneration;
  m_bPaused = m_bHalted = m_bIsDead = false;
  m_bUpdateLifetimeIfInvisible = false;
  m_fColorBitmapSizeX = m_fColorBitmapSizeY = 0.f;
  m_bHandleConstraints = true;
  m_bRenderConstraints = false;
  m_bRepeatLifetime = false;

  m_fInitialDelay = 0.f;
  m_fTransformationCurveTime = m_fTransformationCurveSpeed = 0.f;
  m_iTrailIndex = 0;
  m_fLastTimeOfDayUpdate = -1.0f;
  m_bApplyTimeOfDayLight = true;

  m_ParticleBoundingBox.setInvalid();
  m_WorldSpaceBoundingBox.setInvalid();
  m_LocalSpaceBoundingBox.setInvalid();
  m_bScheduleVisibilityUpdate = true;

  m_iCachedParticleCount = 0;
  m_iMaxAnimFrame = 0;
  m_iMaxCachedParticleCount = PARTICLECACHESIZE_NOALLOC;
  if (m_iCachedParticleNoAlloc != m_piCachedParticle)
    V_SAFE_DELETE_ARRAY(m_piCachedParticle);
  m_piCachedParticle = m_iCachedParticleNoAlloc;
  SetLocalFactors(0.f, 0.f); // not at all in local space
  m_fParticleBoundingBoxUpdateTimePos = 0.f;
  m_bHandleWhenVisible = false; // always handle
  m_iValidCount = 0;
  m_iConstraintAffectBitMask = 0xffffffff;
  m_bHasTransformationCurves = m_bHasEvents = false;

  m_vGroupMoveDeltaAccum.setZero();
  m_pParentEffect = NULL;
  m_pEmitterMeshEntity = NULL;
  m_AmbientColor.SetRGBA(0, 0, 0, 0);
  SetWindSpeed(hkvVec3::ZeroVector(), false);

  m_vFrameWind.setZero();
  m_vFrameWindNoInertia.setZero();
  m_vSizeMultiplier.setZero();
  m_vGroupMoveDelta.setZero();
  m_spDescriptor = pDescr;

  // Emitter will be overridden by default emitter in OnDescriptorChanged if no emitter was set.
  SetEmitter(pEmitter);

  // Set basic orientation, position and scale options before OnDescriptorChanged so that potential child effects can read valid values.
  m_vOldPos = vSpawnPos;
  m_vPosition = vSpawnPos;
  m_fScaling = fScaling; // Do not use setter because descriptor is not yet initialized.
  SetOrientation(vOrientation.x, vOrientation.y, vOrientation.z);

  // modify properties from descriptor
  OnDescriptorChanged();

  // Could be overridden by default emitter in OnDescriptorChanged
  pEmitter = GetEmitter();
  VASSERT(pEmitter || m_pParentGroup);

  m_iRemainingParticleCount = -1;
  int iParticleCount = 0;
  float fPps; // particles per second
  if (m_pParentGroup) 
  {
    // if this group belongs to a parent group, calc the particle count in a different way:
    // particles per second from parent group * number of particles to respawn * lifetime of particles in this group
    fPps = m_pParentGroup->GetEmitter()->m_ParticlesPerSec.m_fAverage;
    float fRespawn = m_pParentGroup->m_spDescriptor->m_OnDestroyCreateCount.m_fAverage;

    iParticleCount = m_spDescriptor->GetEstimatedMaxParticleCount(-1.f, fPps*fRespawn) + 8; // leave extra margin
    m_iRemainingParticleCount = 0; // do not spawn particles on own
  } 
  else
  {
    VASSERT(pEmitter);
    fPps = pEmitter->m_ParticlesPerSec.m_fAverage;
    iParticleCount = m_spDescriptor->GetEstimatedMaxParticleCount(-1.f,fPps) + 8; // leave extra margin
    if (pEmitter->m_FixParticleCount.IsDefined())
    {
      m_iRemainingParticleCount = (int)pEmitter->m_FixParticleCount.GetRandomValue(GetRandom());  
      if (m_iRemainingParticleCount<1) m_iRemainingParticleCount = 1;
      if (m_iRemainingParticleCount<iParticleCount)
        iParticleCount = m_iRemainingParticleCount;
    }
  }    
  // init the particle array
  VASSERT(iParticleCount>0);
  SetParticleStride(sizeof(ParticleExt_t));
  Init( 0, iParticleCount);
  InitParticleIndexList(GetSortParticles());

  // init cache and estimate how much we need
  // there is no reason to scale it with the overall number particles, as long as we have enough particles per frame
  int iWantedCacheSize = (int)(fPps*0.08f); // (fPps*0.08f) is number of spawned p at low fps
  if (iWantedCacheSize<PARTICLECACHESIZE_NOALLOC)
    iWantedCacheSize = PARTICLECACHESIZE_NOALLOC;
  else if (iWantedCacheSize>512)
    iWantedCacheSize = 512;

  m_iCachedParticleCount = 0;
  if (iWantedCacheSize>m_iMaxCachedParticleCount)
  {
    if (m_iCachedParticleNoAlloc!=m_piCachedParticle)
      V_SAFE_DELETE_ARRAY(m_piCachedParticle);
    if (iWantedCacheSize>PARTICLECACHESIZE_NOALLOC)
      m_piCachedParticle = new unsigned short[iWantedCacheSize];
    else 
      m_piCachedParticle = m_iCachedParticleNoAlloc;
  }
  m_iMaxCachedParticleCount = iWantedCacheSize;


  // texture/transparency properties
  char szTexFile[FS_MAX_PATH];
  if (m_spDescriptor->m_sTextureFilename.IsEmpty())
    SetTextureObject(Vision::TextureManager.GetPlainWhiteTexture());
  else
  {
    // make the texture filename relative to XML filename
    m_spDescriptor->MakeFilenameAbsolute(szTexFile,m_spDescriptor->m_sTextureFilename);
    SetTexture(szTexFile);
  }

  if (!m_spDescriptor->m_sNormalmapFilename.IsEmpty())
  {
    m_spDescriptor->MakeFilenameAbsolute(szTexFile,m_spDescriptor->m_sNormalmapFilename);
    SetNormalmap(Vision::TextureManager.Load2DTexture(szTexFile));
  }
  if (!m_spDescriptor->m_sSpecularmapFilename.IsEmpty())
  {
    m_spDescriptor->MakeFilenameAbsolute(szTexFile,m_spDescriptor->m_sSpecularmapFilename);
    SetNormalmap(Vision::TextureManager.Load2DTexture(szTexFile));
  }

  SetTransparencyType(m_spDescriptor->m_eTransp);
  SetAnimationSubdivisions(m_spDescriptor->m_iAnimSubDiv[0],m_spDescriptor->m_iAnimSubDiv[1]);
  SetUseSmoothAnimationFading(m_spDescriptor->m_bUseSmoothAnimation);
  SetParticleSizeAspectRatio(m_spDescriptor->m_fSizeAspect);
  SetAlwaysInForeGround(m_spDescriptor->m_bAlwaysInForeground);
  SetUseOcclusionCulling(m_spDescriptor->m_bUseOcclusionCulling);
  SetObjectFlag(VObjectFlag_AutoDispose); // so the particle effect layer gets deleted automatically

  ////////////////////////////////////////////////////////
  // fill the group with initial count
  ////////////////////////////////////////////////////////
  if (bSpawnParticles)
    RespawnAllParticles(false);

  if (!m_pHandlingTask)
    m_pHandlingTask = new HandleParticlesTask_cl(this);

  #ifdef _VISION_WIN32
    m_pParticleDesaturationGroup = NULL;
  #endif
  //VASSERT(GetVisibilityObject());
}

ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, unsigned int uiRandomSeed)
  : IVPhysicsParticleCollection_cl(uiRandomSeed)
  , m_pParentGroup(NULL)
  , m_piCachedParticle(NULL)
  , m_pHandlingTask(NULL)

{
  InitGroup(pDescr, pEmitter, vSpawnPos, hkvVec3(0.f, 0.f, 0.f), 1.0f, true);
}

ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, bool bSpawnParticles, unsigned int uiRandomSeed)
  : IVPhysicsParticleCollection_cl(uiRandomSeed)
  , m_pParentGroup(NULL)
  , m_piCachedParticle(NULL)
  , m_pHandlingTask(NULL)
{
  InitGroup(pDescr, pEmitter, vSpawnPos, vOrientation, 1.0f, bSpawnParticles);
}

ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, float fScaling, bool bSpawnParticles, unsigned int uiRandomSeed)
  : IVPhysicsParticleCollection_cl(uiRandomSeed)
  , m_pParentGroup(NULL)
  , m_piCachedParticle(NULL)
  , m_pHandlingTask(NULL)
{
  InitGroup(pDescr, pEmitter, vSpawnPos, vOrientation, fScaling, bSpawnParticles);
}

ParticleGroupBase_cl::ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, ParticleGroupBase_cl *pParent, int iGeneration, unsigned int uiRandomSeed)
  : IVPhysicsParticleCollection_cl(uiRandomSeed)
  , m_pParentGroup(pParent)
  , m_piCachedParticle(NULL)
  , m_pHandlingTask(NULL)
{
  VASSERT(m_pParentGroup);
  
  // Constructor used for OnDestroy particles, emitter is intentionally left empty, it will automatically use the default descriptor emitter.
  InitGroup(pDescr, NULL, pParent->GetPosition(), pParent->GetOrientation(), pParent->GetScaling(), false, iGeneration);
}

void ParticleGroupBase_cl::RemoveUpdaterTaskRecursive(ParticleGroupBase_cl *pGroup)
{
  if (pGroup==NULL)
    return;
  if (pGroup->GetUpdateTask()==this->m_pHandlingTask)
    pGroup->SetUpdateTask(NULL);
  RemoveUpdaterTaskRecursive(pGroup->m_spOnDestroyCreateGroup);
}

void ParticleGroupBase_cl::UpdateDefaultShaderFlags()
{
  VisParticleGroup_cl::UpdateDefaultShaderFlags();
  ReassignShader(false);
}

ParticleGroupBase_cl::~ParticleGroupBase_cl()
{
  EnsureUpdaterTaskFinished();
  RemoveUpdaterTaskRecursive(m_spOnDestroyCreateGroup); // make sure nothing else references this task
  V_SAFE_DELETE(m_pHandlingTask);
  if (m_iCachedParticleNoAlloc!=m_piCachedParticle)
    V_SAFE_DELETE_ARRAY(m_piCachedParticle);
}

void ParticleGroupBase_cl::ReassignShader(bool bRecreateFX)
{
  VisEffectConfig_cl &fxConfig = m_spDescriptor->m_CustomEffectConfig;
  if (bRecreateFX)
    fxConfig.ReAssignEffect();

  // Get particle shading flags
  VisParticleGroup_cl::ParticleShaderFlags_e iParticleShaderFlags = GetDefaultShaderFlags();
  bool bTrackLightGrid = (iParticleShaderFlags & PARTICLESHADERFLAGS_LIGHTING_STATIC) != 0;

  // Warn if soft particles enabled but not usable depth buffer can't be read.
  if (iParticleShaderFlags & PARTICLESHADERFLAGS_SOFTPARTICLES)
  {
    VRendererNodeCommon* pRenderer = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(0));
    if (pRenderer == NULL || (pRenderer->GetSupportedBufferFlags() & VBUFFERFLAG_DEPTH) == 0)
    {
      hkvLog::Info("Particle layer \"%s\": Soft particles aren't supported by the current renderer node. Option will be deactivated.", m_spDescriptor->GetName());
    }
  }

  // Get Effect from config.
  VCompiledEffect* pFX = fxConfig.GetEffect();

  // No user defined effect
#if !defined(_VISION_MOBILE)
  if (pFX == NULL && m_eTopology == PARTICLE_TOPOLOGY_TRAIL)
  {
    VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("\\Shaders\\TrailParticles.ShaderLib", SHADERLIBFLAG_HIDDEN);
    pFX = Vision::Shaders.CreateEffect("TrailParticles", NULL, EFFECTCREATEFLAG_NONE, pLib);
  }
#endif

  // No effect means use default technique.
  if (pFX == NULL)
  {
    SetTechnique(NULL);
  }
  else
  {
    // create effect config
    VTechniqueConfig config;
    Vision::GetApplication()->GetShaderProvider()->GetParticleEffectConfig(iParticleShaderFlags, config);

    // find technique
    VCompiledTechnique* pTechnique = pFX->FindCompatibleTechnique(&config);
    if (pTechnique == NULL)
      pTechnique = pFX->GetDefaultTechnique();
    if (pTechnique == NULL)
      hkvLog::Error("Could not create technique from user defined particle shader. Will fall back to built-in default shader.");
    SetTechnique(pTechnique);

    // Get lightgrid settings.
    if (pTechnique)
    {
      VCompiledShaderPass* pShader = pTechnique->GetShader(0);
      if (pShader)
      {
        unsigned int iTrackingMask = pShader->GetRenderState()->GetTrackingMask();
        bTrackLightGrid |= (iTrackingMask & (VSHADER_TRACKING_LIGHTGRID_PS | VSHADER_TRACKING_LIGHTGRID_VS | VSHADER_TRACKING_LIGHTGRID_GS)) != 0;
      }
    }
  }

  // Update light color and light tracking settings.
  if (m_pParentEffect)
  {
    SetLightGridColorPtr(bTrackLightGrid ? m_pParentEffect->m_OwnLGColors : NULL);
    m_pParentEffect->UpdateLightTrackingSettings();
  }
}

void ParticleGroupBase_cl::Finalize()
{
  ReassignShader(false);
  SetInitialTransformation();
}


float ParticleGroupBase_cl::GetSortingDistance(const VisObject3D_cl *pCamera)
{
  hkvVec3 vPos(GetParentEffect()==NULL ? GetPosition() : GetParentEffect()->GetPosition());
  hkvVec3 vDiff = (hkvVec3) vPos - pCamera->GetPosition(); // all particle layers should use same pivot for sorting
  float fDist = vDiff.getLength();
  fDist -= (float)m_iChildIndex*Vision::World.GetGlobalUnitScaling(); // there should be a well defined order between particle layers
  return fDist;
}

void ParticleGroupBase_cl::SetInitialTransformation()
{
  if (m_spDescriptor->m_bUseTransformationCurve)
  {
    float x = m_fTransformationCurveTime;
    if (m_spDescriptor->m_spPositionCurve!=NULL)
      m_vLocalPosition = m_spDescriptor->m_spPositionCurve->GetValueFast(x) * m_fScaling;
    if (m_spDescriptor->m_spOrientationCurve!=NULL)
    {
      hkvVec3 vLocalYPR = m_spDescriptor->m_spOrientationCurve->GetValueFast(x);
      vLocalYPR *= 360.f;

      m_localOrientation.setFromEulerAngles (vLocalYPR.z, vLocalYPR.y, vLocalYPR.x);
    }
  } else
  {
    m_vLocalPosition = m_spDescriptor->m_vRelativePosition * m_fScaling;
    m_localOrientation.setFromEulerAngles (m_spDescriptor->m_vRelativeOrientation.z, m_spDescriptor->m_vRelativeOrientation.y, m_spDescriptor->m_vRelativeOrientation.x);
  }
  UpdateBinding();
}

void ParticleGroupBase_cl::OnDescriptorChanged()
{
  ////////////////////////////////////////////////
  // default emitter
  ////////////////////////////////////////////////
  if (!GetEmitter())
    SetEmitter(m_spDescriptor->GetDefaultEmitter()->Clone(GetRandom()));

  //////////////////////////////////////////////
  // animation
  //////////////////////////////////////////////
  m_fAnimFrameCount = (float)m_spDescriptor->m_iUsedAnimFrames;
  m_iMaxAnimFrame = m_spDescriptor->m_iUsedAnimFrames-1;
  m_eParticleAnimMode = m_spDescriptor->m_eParticleAnimMode;

  //////////////////////////////////////////////
  // geometry
  //////////////////////////////////////////////
  SetTopology(static_cast<VIS_PARTICLE_TOPOLOGY_e>(m_spDescriptor->m_eTopology));
  SetGeometry(m_spDescriptor->m_spGeometry);

  //////////////////////////////////////////////
  // rotation
  //////////////////////////////////////////////
  m_eRotationMode = m_spDescriptor->m_eRotationMode;
  m_spRotationCurve = (m_eRotationMode==PARTICLE_ROTATION_ANGLECURVE) ? m_spDescriptor->m_spRotationCurve : NULL;

  //////////////////////////////////////////////
  // lookup curves from descriptor
  //////////////////////////////////////////////
  m_spColorLookup = m_spDescriptor->m_spColorLookup;
  m_pColorLookup = NULL;
  if (m_spColorLookup)
  {
    m_spColorLookup->RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD); // don't unload while we have m_pColorLookup
    m_fColorBitmapSizeX = (float)m_spColorLookup->GetWidth();
    m_fColorBitmapSizeY = (float)m_spColorLookup->GetHeight();
    m_pColorLookup = m_spColorLookup->GetDataPtr();
  }
  m_spSizeCurve = m_spDescriptor->m_spSizeCurve;
  m_spAnimCurve = m_spDescriptor->m_spAnimCurve;

  //////////////////////////////////////////////
  // other parameters from descriptor
  //////////////////////////////////////////////

  // using lightgrid overrides the simple modulation color
  m_bEvaluateBrightnessNextFrame = (GetLightGridColorPtr()==NULL) && (m_spDescriptor->m_fApplySceneBrightness > 0.0f);

  m_InstanceColor.SetRGBA(255,255,255,255);
  m_iConstraintAffectBitMask = m_spDescriptor->m_iConstraintAffectBitMask;

  m_fLifeTime = m_spDescriptor->GetRandomGroupLifeTime(GetRandom());
  m_fTimeScale = m_spDescriptor->m_TimeScaling.GetRandomValue(GetRandom());
  m_fInitialDelay = m_spDescriptor->m_InitialDelay.GetRandomValue(GetRandom());
  m_bRepeatLifetime = m_spDescriptor->m_bRepeatLifetime;

  m_bInfiniteLifeTime = (m_fLifeTime<0.f) || (m_pParentGroup!=NULL);
  if (!m_pParentGroup && GetEmitter()->m_FixParticleCount.IsDefined())
  {
    m_iRemainingParticleCount = (int)GetEmitter()->m_FixParticleCount.GetRandomValue(GetRandom());  
    if (m_iRemainingParticleCount<1) m_iRemainingParticleCount = 1;
  }
  m_bInertiaAffectsGravity = m_spDescriptor->m_bInertiaAffectsGravity;

  // fading
  SetFadeMode(m_spDescriptor->m_eFadeMode);
  FadeDistancesFromDesc();

  // start at random time position past m_fDynamicInflateInterval to invoke the first update of the bounding box immediately.
  m_fParticleBoundingBoxUpdateTimePos = (1.0f + GetRandom().GetFloat()) * m_spDescriptor->m_fDynamicInflateInterval;

  m_bIsDead = false;
  m_bHandleWhenVisible = m_spDescriptor->m_bHandleWhenVisible;

  // soft particles
  SetRenderSoftParticles(m_spDescriptor->m_bSoftParticles);

  // lighting options
  SetLightingStatic(m_spDescriptor->m_bLightingStatic);
  SetLightingDynamic(m_spDescriptor->m_bLightingDynamic);
  SetTessellationEnabled(m_spDescriptor->m_bDomainFreqSamplingEnabled);
  SetTessellationFactorPixelPerVertex(m_spDescriptor->m_fDomainFreqSamplingPixelPerVertex);
  SetShadowReceive(m_spDescriptor->m_bShadowReceive);
  SetBacklightingScale(m_spDescriptor->m_fBacklightingScale);
  SetUseNormalFromDiffAlpha(m_spDescriptor->m_bUseNormalFromDiffAlpha);
  
  // local positioning:
  // 0% : no local space at all
  // 50% : in local space at particle lifetime start but none at end
  // 100% : fully in local space
  float fAtStart = hkvMath::Min(m_spDescriptor->m_fApplyEmitterSpeed*2.f, 1.f);
  float fAtEnd = hkvMath::Max(m_spDescriptor->m_fApplyEmitterSpeed*2.f-1.f, 0.f);
  SetLocalFactors(fAtStart,fAtEnd);
  SetUseLocalSpaceMatrix(m_spDescriptor->m_bLocalSpace); // real local space?

  // center
  if (m_eTopology != PARTICLE_TOPOLOGY_TRAIL)
  {
    const hkvVec2 &vCenter(m_spDescriptor->m_vParticleCenter);
    SetParticleCenter(vCenter.x,vCenter.y);
  }

  SetDepthOffset(m_spDescriptor->m_fDepthOffset);

  // normal
  SetUseNormals(m_spDescriptor->m_bDistortionPlaneAligned || m_eTopology == PARTICLE_TOPOLOGY_RINGWAVE);

  SetSizeMultiplier(m_spDescriptor->m_vSizeMultiplier);

  // sorting (upon initialization, the index list for sorting is initialized in the InitGroup function)
  SetSortParticles(m_spDescriptor->m_bSortParticles);
  InitParticleIndexList(GetSortParticles());

  // render order - under certain circumstances, force a specific order constant
  #if defined(_VISION_WIN32) || defined(_VR_GLES2)
    const bool bQuarterSizeParticles = false;

  #else
    // On consoles, get quarter size particle mode from the translucency render loop
    bool bQuarterSizeParticles = false;

    if (VRendererNodeCommon* pRendererNode = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(0)))
    {
      if (VPostProcessTranslucencies* pTranslucencies = pRendererNode->Components().GetComponentOfBaseType<VPostProcessTranslucencies>())
      {
        bQuarterSizeParticles = pTranslucencies->IsQuarterSizeParticles();
      }
    }

  #endif

  ANALYSIS_IGNORE_WARNING_ONCE(6237)
  if (bQuarterSizeParticles && m_spDescriptor->m_iRenderOrder==VRH_PARTICLES && m_spDescriptor->m_eTransp==VIS_TRANSP_ADDITIVE)
  {
    SetRenderOrder(VRH_ADDITIVE_PARTICLES);
  }
  else
  {
    SetRenderOrder((unsigned int)m_spDescriptor->m_iRenderOrder);
  }

  // group sorting key
  SetUserSortKey(m_spDescriptor->m_iGroupSortKey);

  ////////////////////////////////////////////////
  // create a new group for destroying particles
  ////////////////////////////////////////////////

  if (m_spDescriptor->m_OnDestroyCreateCount.IsDefined() && m_iGeneration < MAX_ONDESTROY_GROUPDEPTH)
  {
    VisParticleGroupDescriptor_cl *pDestroyDesc = m_spDescriptor->m_spDestroyCreateDesc;
    if (pDestroyDesc)
    {
      m_spOnDestroyCreateGroup = new ParticleGroupBase_cl(pDestroyDesc, this, m_iGeneration + 1, GetBaseSeed());
      m_spOnDestroyCreateGroup->ReassignShader(false);
    }
  }

  // transformation curves
  m_bHasTransformationCurves = m_spDescriptor->m_bUseTransformationCurve && (m_spDescriptor->m_spPositionCurve!=NULL || m_spDescriptor->m_spOrientationCurve!=NULL);
  float fTransformTime = m_spDescriptor->m_fTransformationCurveTime;
  if (fTransformTime<=HKVMATH_LARGE_EPSILON)
  {
    fTransformTime = m_fLifeTime;
    if (fTransformTime<=HKVMATH_LARGE_EPSILON) fTransformTime = 1.f;
  }
  m_fTransformationCurveSpeed = 1.f / fTransformTime;

  // emitter relative positioning
  SetInitialTransformation();

  // events
  m_bHasEvents = m_spDescriptor->m_EventList.GetEventCount()>0;

  m_bUseVisibility = !m_spDescriptor->m_bAlwaysVisible;

  UpdateBoundingBoxes();
}

void ParticleGroupBase_cl::FadeDistancesFromDesc()
{
  SetCustomFadeRange(m_spDescriptor->m_fFadeStart * VisParticleGroupManager_cl::g_fGlobalFadeScaling, m_spDescriptor->m_fFadeEnd * VisParticleGroupManager_cl::g_fGlobalFadeScaling);
}


#ifdef SUPPORTS_SNAPSHOT_CREATION

void ParticleGroupBase_cl::GetDependencies(VResourceSnapshot &snapshot)
{
  VisParticleGroup_cl::GetDependencies(snapshot);

  // TODO: textures from the descriptor...
}

#endif

void ParticleGroupBase_cl::ModSysNotifyFunctionCommand(int command, void *param)
{
  if (command==VIS_MODSYSCMD_RECOMPUTEVISIBILITY)
  {
    if (GetVisibilityObject())
      GetVisibilityObject()->ReComputeVisibility();
  }
}


int ParticleGroupBase_cl::CountActiveParticles() const
{
  const int iParticleCount = m_iHighWaterMark;
  const ParticleExt_t *p = GetParticlesExt();
  int iFound = 0;
  for (int i=0;i<iParticleCount;i++,p++)
    if (p->valid) iFound++;

  return iFound;
}

void ParticleGroupBase_cl::TeleportSpawnPosition()
{
  if (!m_spEmitter)
    return;

  // Force emitter to current particle group position, without any interpolation
  m_spEmitter->m_vLastEmitterPos = m_vPosition;
  m_spEmitter->m_fParticleAge = 1.0f;
}

void ParticleGroupBase_cl::RespawnAllParticles(bool bUseOldCount)
{
  int iParticleCount = GetNumOfParticles();
  if (iParticleCount<1) return;

  if (!m_spEmitter) return;

  // no interpolation of position
  m_spEmitter->m_vLastEmitterPos = m_vPosition;
  m_spEmitter->m_fParticleAge = 1.0f;

  int iStartupCount = 0;

  // either use same number as currently active or determine startup count
  if (bUseOldCount)
    iStartupCount = CountActiveParticles();
  else if (hkvMath::isZero(m_fInitialDelay))
  {
    float fPps = m_spEmitter->m_ParticlesPerSec.m_fAverage;
    int iAllParticles = m_spDescriptor->GetEstimatedMaxParticleCount(-1.f,fPps);
    iStartupCount = (int)(m_spEmitter->m_StartupFillPercentage.GetRandomValue(GetRandom())*(float)iAllParticles*(1.f/99.9f)); // avoid rounding issues
  }

  if (iStartupCount>iParticleCount)
    iStartupCount = iParticleCount;
  else if (iStartupCount<0) iStartupCount = 0;

  m_iCachedParticleCount = 0;
  ParticleExt_t *pParticle = GetParticlesExt(); // particle array is still empty!
  ParticleExt_t *pArray = pParticle;
  memset(pParticle,0,iParticleCount*sizeof(ParticleExt_t));

  m_iHighWaterMark = m_iValidCount = iStartupCount;
  m_vGroupMoveDelta.setZero();
  m_vGroupMoveDeltaAccum.setZero(); // all particles start at right position...

  for (int i=0;i<iStartupCount;i++,pParticle++)
  {
    VASSERT(!pParticle->valid);
    InitSingleParticle(pParticle); // asserts when index>=m_iHighWaterMark 

    float fLifeTimeInc = pParticle->m_fLifeTimeInc;
    if (i>0 && fLifeTimeInc>0.f)
    {
      const int iIterations = 8;
      float dtime = ((float)i)/(fLifeTimeInc*(float)iIterations * (float)iStartupCount);
      // setup per frame variables
      SetPerFrameConstants(dtime);
      for (int j=0;j<iIterations;j++)
        HandleSingleParticle(pParticle,dtime);
    }
    else
    {
      SetPerFrameConstants(0.f);
      HandleSingleParticle(pParticle,0.f);
    }
  }
  
  HandleAllConstraints(0.f);

  if (m_eTopology == PARTICLE_TOPOLOGY_TRAIL)
  {
    pParticle = pArray;
    for (int i=0;i<iStartupCount;i++,pParticle++)
    {
      int iPrevIndex = hkvMath::Max(i-1,0);
      pParticle->m_fDistortionMult = (float)iPrevIndex + 0.5f; // stores the index to the previous...
      const ParticleExt_t *pOther = &pArray[iPrevIndex];
      pParticle->distortion[0] = pOther->m_vPosition[0] - pParticle->m_vPosition[0];
      pParticle->distortion[1] = pOther->m_vPosition[1] - pParticle->m_vPosition[1];
      pParticle->distortion[2] = pOther->m_vPosition[2] - pParticle->m_vPosition[2];
      // in order to be able make a continuous trail we need information about the next particle as well
      iPrevIndex = hkvMath::Max(i-2,0);
      const ParticleExt_t *pOther2 = &pArray[iPrevIndex];
      pParticle->m_vNormal = pOther2->m_vPosition - pOther->m_vPosition;

    }
    m_iTrailIndex = iStartupCount-1;
  }

  m_fParticleBoundingBoxUpdateTimePos = m_spDescriptor->m_fDynamicInflateInterval; // Force inflate.
  UpdateBoundingBoxes();
}


void ParticleGroupBase_cl::IncPosition(const hkvVec3& vDelta)
{
  SetPosition(GetPosition()+vDelta);
}

//TODO
hkvVec3 ParticleGroupBase_cl::GetDirection()
{
  return m_vDirection;
}

void ParticleGroupBase_cl::MoveParticles(const hkvVec3& vDelta)
{
  EnsureUpdaterTaskFinished();

  const int iParticleCount = m_iHighWaterMark;
  int i;
  ParticleExt_t *p = GetParticlesExt();

  for (i=0;i<iParticleCount;i++,p++) if (p->valid)
    p->m_vPosition += vDelta;

  if (m_spEmitter!=NULL)
    m_spEmitter->m_vLastEmitterPos += vDelta;
  m_vOldPos += vDelta;
}

void ParticleGroupBase_cl::SetLocalFactors(float fAtLifetimeStart, float fAtLifetimeEnd)
{
  m_fLocalFactorStart = fAtLifetimeStart;
  m_fLocalFactorDiff = fAtLifetimeEnd-fAtLifetimeStart;
  m_bMovesWithEmitter = fAtLifetimeStart>0.f || fAtLifetimeEnd>0.f;
}


 
void ParticleGroupBase_cl::SetPerFrameConstants(float dtime)
{
  if (m_spDescriptor->m_fFriction>0.f)
    m_fFrameFriction = hkvMath::pow(1.f-m_spDescriptor->m_fFriction, dtime);
  else 
    m_fFrameFriction = 1.f;

  m_vFrameWind = m_vWindSpeed*dtime;
  if (m_bWindInLocalSpace)
  {
    const hkvMat3 &mRot = GetRotationMatrix();
    m_vFrameWind = mRot.transformDirection(m_vFrameWind);
  }

  if (g_ParticleGroupManager.m_pWindHandler!=NULL)
    m_vFrameWind += g_ParticleGroupManager.m_pWindHandler->GetWindVelocityAtPosition(GetPosition()) * dtime;

  if (m_bInertiaAffectsGravity)
  {
    m_vFrameWind += m_spDescriptor->m_vGravity * dtime;
    m_vFrameWindNoInertia.setZero();
  }
  else
    m_vFrameWindNoInertia = m_spDescriptor->m_vGravity * dtime;

  // transform per-frame speeds back into local space
  if (GetUseLocalSpaceMatrix())
  {
    hkvMat3 transposedRot = GetRotationMatrix();
    transposedRot.transpose();
    m_vFrameWind = transposedRot.transformDirection(m_vFrameWind);
    m_vFrameWindNoInertia = transposedRot.transformDirection(m_vFrameWindNoInertia);
  }
}


 
void ParticleGroupBase_cl::UpdateVisibilityObject()
{
  UpdateBoundingBoxes();
}


void ParticleGroupBase_cl::CopyParentPosition()
{
  VASSERT(m_pParentGroup);
  SetPosition(m_pParentGroup->GetPosition());
  SetOrientation(m_pParentGroup->GetRotationMatrix());
}


void ParticleGroupBase_cl::SetAmbientColor(VColorRef iColor)
{
  if (m_spOnDestroyCreateGroup) 
    m_spOnDestroyCreateGroup->SetAmbientColor(iColor);

  m_AmbientColor = iColor;

  // update the final color
  EvaluateSceneBrightness();
}


void ParticleGroupBase_cl::EvaluateSceneBrightness()
{
  // reset this state
  m_bEvaluateBrightnessNextFrame = false;

  if (m_spDescriptor->m_fApplySceneBrightness <= 0.0f || GetLightingStatic())
  {
    m_InstanceColor.SetRGB(255, 255, 255);
  }
  else if (GetParentEffect() != NULL)
  {
    VColorRef sceneBrightnessColor;

    // Use lightgrid for scene brightness if available
    VLightGrid_cl* pLightGrid = GetParentEffect()->GetRelevantLightGrid();
    if (pLightGrid)
    {
      hkvVec3 destColor;
      pLightGrid->GetAverageColorAtPositionI(GetPosition() + GetLightSamplingOffset(), destColor);
      if (pLightGrid->GetLightGridType() == VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL)
        destColor *= 2.0f;

      sceneBrightnessColor.FromFloat(destColor);
    }
    else
    {
      // Otherwise use time of day if available.
      IVTimeOfDay* pTimeOfDay = Vision::Renderer.GetTimeOfDayHandler();
      if (m_bApplyTimeOfDayLight && pTimeOfDay != NULL)
      {
        // store when we last updated the time-of-day stuff
        m_fLastTimeOfDayUpdate = pTimeOfDay->GetDayTime();
        sceneBrightnessColor = pTimeOfDay->GetAmbientColor() + pTimeOfDay->GetSunColor(); // heuristic to determine ToD contribution
      }
      else
      {
        sceneBrightnessColor = Vision::Renderer.GetDefaultLightingColor();
      }
    }

    // Add instance ambient color on top of that.
    sceneBrightnessColor += m_AmbientColor;

    // Interpolate between white (= no influence) and scene brightness
    VColorRef newInstanceColor = VColorRef(255, 255, 255) * (1.0f - m_spDescriptor->m_fApplySceneBrightness) +
                                     sceneBrightnessColor * m_spDescriptor->m_fApplySceneBrightness;
    m_InstanceColor.SetRGB(newInstanceColor.r, newInstanceColor.g, newInstanceColor.b);
  }
}


bool ParticleGroupBase_cl::FillFreeParticleCache()
{
  // reset the cache
  m_iCachedParticleCount = 0;
  const int iParticleCount = GetNumOfParticles();
  int i;
  ParticleExt_t *p = GetParticlesExt();
  for (i=0;i<iParticleCount;i++,p++)
    if (!p->valid)
    {
	  // Fill backwards so we get the lowest indices first in the cache
      m_piCachedParticle[m_iMaxCachedParticleCount - ++m_iCachedParticleCount] = i;

      if (m_iMaxCachedParticleCount == m_iCachedParticleCount)
        break; // cache is full
    }
  if (!m_iCachedParticleCount)
    return false; // no free particles at all
  if (m_iMaxCachedParticleCount > m_iCachedParticleCount)
  {
    // Need to shift value down since we weren't able to fill it
    memmove(&m_piCachedParticle[0], 
            &m_piCachedParticle[m_iMaxCachedParticleCount - m_iCachedParticleCount], 
            m_iCachedParticleCount*sizeof(unsigned short));
  }

  return true;
}


ParticleExt_t* ParticleGroupBase_cl::GetFreeParticle()
{
  if (m_eTopology == PARTICLE_TOPOLOGY_TRAIL) // trail is handled differently
  {
    int iPrevIndex = hkvMath::Max (m_iTrailIndex,0);// the first one point to itself
    m_iTrailIndex = (m_iTrailIndex+1) % GetNumOfParticles();
    m_iHighWaterMark = hkvMath::Max(m_iHighWaterMark,m_iTrailIndex+1);
    ParticleExt_t *p = &GetParticlesExt()[m_iTrailIndex];
    p->m_fDistortionMult = (float)iPrevIndex + 0.1f; // in this mode, we use this variable to store the index to the previous particle

    // make sure the end of the trail does not reference the beginning
    int iNext = (m_iTrailIndex+1) % GetNumOfParticles();
    ParticleExt_t *pNext = &GetParticlesExt()[iNext];
    pNext->m_fDistortionMult = (float)iNext + 0.1f; // reference itself
    return p;
  }
  if (m_iCachedParticleCount>0) // take particle from cache
  {
    m_iCachedParticleCount--;
    int iIndex = m_piCachedParticle[m_iCachedParticleCount];
    ParticleExt_t *p = &GetParticlesExt()[iIndex];
    if (!p->valid)
    {
      iIndex++;
      m_iHighWaterMark = hkvMath::Max(m_iHighWaterMark,iIndex);
      return p;
    }
    // particle has been reused although it was in cache, try the next one
    return GetFreeParticle();
  }

  // cache was empty, so fill it up again
  if (!FillFreeParticleCache())
    return NULL; // no free particle at all

  return GetFreeParticle();
}

void ParticleGroupBase_cl::DestroyParticle(Particle_t *pParticle,float fTimeDelta)
{
  if (m_spOnDestroyCreateGroup)
  {
    int i;
    int iCount = (int)m_spDescriptor->m_OnDestroyCreateCount.GetRandomValue(GetRandom());
    // create iCount new particles from other group
    if (iCount)
    {
      int iCopyFlags = m_spDescriptor->m_iOnDestroyCopyFlags;
      float fStep = fTimeDelta/(float)iCount;
      float fTimePos = 0.f;
      for (i=0;i<iCount;i++,fTimePos+=fStep)
        if (!m_spOnDestroyCreateGroup->CreateParticleFromSource((ParticleExt_t *)pParticle,fTimePos,iCopyFlags))
          break;
    }
  }
  AddParticleToCache((ParticleExt_t *)pParticle);
}

void ParticleGroupBase_cl::RenderParticleBoundingBoxes()
{
  ParticleExt_t *p = GetParticlesExt();
  for (int i = 0; i < m_iHighWaterMark; ++i, ++p)
  {
    if (p->valid)
    {
    Vision::Game.DrawCube(p->m_vPosition, p->size, p->color);
}
  }
}

void ParticleGroupBase_cl::HandleParticles(float dtime)
{
  EnsureUpdaterTaskFinished();

  VISION_PROFILE_FUNCTION(VIS_PROFILE_PARTICLES_HANDLE);

  if (m_bScheduleVisibilityUpdate)
  {
    if (GetUseLocalSpaceMatrix())
      SetVisibilityBoundingBox(true, m_LocalSpaceBoundingBox);
    else
      SetVisibilityBoundingBox(false, m_WorldSpaceBoundingBox);
  
    m_bScheduleVisibilityUpdate = false;
  }

  bool bNeedLifetimeUpdateOnly = false;

  if (m_bHandleWhenVisible && !WasRecentlyRendered()) // actual vischeck of particle group
  {
    if(!m_bUpdateLifetimeIfInvisible)
    {
      return;
    }
    bNeedLifetimeUpdateOnly = true;
  }

  if (m_bHalted) 
    return; // paused mode

  if (m_fInitialDelay>0.f)
  {
    float fScaledTime = dtime * m_fTimeScale;
    m_fInitialDelay -= fScaledTime;
    return;
  }

  // also handle child particle group
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->HandleParticles(dtime);

  bool bDead = m_bIsDead;

  // handle group lifetime
  if (!m_bInfiniteLifeTime && !m_bPaused) // pause will also save lifetime
  {
    float fScaledTime = dtime * m_fTimeScale;
    m_fLifeTime -= fScaledTime;
    if (m_fLifeTime <= 0.0f)
    {
      // Make sure that we get full particle simulation again once the lifetime is zero,
      // even if we only update lifetime and do not simulate. This ensures that the particles
      // get correctly faded out and destroyed (which happens in simulation).
      bNeedLifetimeUpdateOnly = false;
      if(m_iValidCount == 0)
      {
        bDead = true;
      }
    }
  } 
  else if (m_iRemainingParticleCount==0 && m_iValidCount==0 && !m_pParentGroup) // also die when all particles are used up
    bDead = true;

  if (bDead)
  {
    m_bIsDead = true;
    if (m_pParentEffect)
      m_pParentEffect->OnSingleGroupFinished(this); // detach from parent
    // from now on, group might not be valid anymore, so return
    return;
  }

  if(bNeedLifetimeUpdateOnly)
  {
    // Early out if we only care about life update
    return; 
  }

  if (VISION_UNLIKELY(m_bRenderConstraints))
    m_Constraints.RenderConstraints(VisRenderContext_cl::GetCurrentContext()->GetRenderInterface());

  m_pHandlingTask->m_fTimeDelta = dtime;
  
  if (!m_pParentGroup) 
  {
    m_vGroupMoveDelta = m_vGroupMoveDeltaAccum;
    m_vGroupMoveDeltaAccum.setZero();
    Vision::GetThreadManager()->ScheduleTask(m_pHandlingTask, 5);
    SetUpdateTask(m_pHandlingTask);
  }
  else
  {
    // if this is a child particle group, copy position and orientation from parent
    CopyParentPosition();
    ParticleGroupBase_cl *pRootParent = m_pParentGroup;
    while (pRootParent->m_pParentGroup) 
    {
      pRootParent = pRootParent->m_pParentGroup;
    }
    SetUpdateTask(pRootParent->m_pHandlingTask);
  }
}


void ParticleGroupBase_cl::InflateBoundingBox()
{
  m_ParticleBoundingBox.setInvalid();

  ParticleExt_t *p = GetParticlesExt();
  
  for (int i = 0; i < m_iHighWaterMark; i++, p++)
  {
    if (!p->valid)
      continue;

    hkvVec3 pos = p->m_vPosition;
    hkvVec3 rad(p->size,p->size,p->size);

    hkvAlignedBBox transformedMeshBoundingBox = m_spDescriptor->m_MeshBoundingBox;
    transformedMeshBoundingBox.scaleFromOrigin(rad);
    transformedMeshBoundingBox.translate(pos);

    m_ParticleBoundingBox.expandToInclude(transformedMeshBoundingBox);

	  rad *= 0.5f;

    m_ParticleBoundingBox.expandToInclude(pos + rad);
    m_ParticleBoundingBox.expandToInclude(pos - rad);

    // make sure that this box remains valid until the next update
    float dt = m_spDescriptor->m_fDynamicInflateInterval;
    hkvVec3 lastPos = pos + p->m_vVelocity * dt;
    m_ParticleBoundingBox.expandToInclude(lastPos + rad);
    m_ParticleBoundingBox.expandToInclude(lastPos - rad);

    // Makes no sense for ringwaves and distortion off.
    if (GetUseDistortion() && m_eTopology != PARTICLE_TOPOLOGY_RINGWAVE)
    {
      pos.x += p->distortion[0];
      pos.y += p->distortion[1];
      pos.z += p->distortion[2];
      m_ParticleBoundingBox.expandToInclude(pos + rad);
      m_ParticleBoundingBox.expandToInclude(pos - rad);
    }
    }
  }


int ParticleGroupBase_cl::AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence)
  {
  // Constraints don't work with local space particles.
  if (m_spDescriptor->m_bLocalSpace)
    {
    hkvLog::Warning("Constraints won't work with local space particles! (Occured in %s)", m_spDescriptor->GetName());
    return 0;
  }

  EnsureUpdaterTaskFinished();
  VASSERT(pSrcList);
  int i;
  const int iCount = pSrcList->GetConstraintCount();
  int iFound = 0;

  for (i=0;i<iCount;i++)
  {
    VisParticleConstraint_cl *pConstraint = pSrcList->GetConstraint(i);
    if (!pConstraint)
      continue;
    if (bCheckInfluence && !pConstraint->Influences(GetWorldSpaceBoundingBox()))
      continue;
    if (!AddConstraint(pConstraint, false))
      continue; // influence already checked
    
    iFound++;
  }
  return iFound;
}


bool ParticleGroupBase_cl::AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence)
{
  // Constraints don't work with local space particles.
  if (m_spDescriptor->m_bLocalSpace)
  {
    hkvLog::Warning("Constraints won't work with local space particles! (Occured in %s)", m_spDescriptor->GetName());
    return 0;
  }


  EnsureUpdaterTaskFinished();
  VASSERT(pConstraint);
  if (bCheckInfluence && !pConstraint->Influences(GetWorldSpaceBoundingBox()))
    return false;

  m_Constraints.AddConstraint(pConstraint);

  // add also to the child group (without checking)
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->AddConstraint(pConstraint,false);

  return true;
}


void ParticleGroupBase_cl::HandleAllConstraints(float dtime) 
{
  // local list of constraints
  m_Constraints.HandleParticles(this,dtime,m_iConstraintAffectBitMask);

  // handle global list. Note that HandleParticles causes problems in multithreaded mode. Removing dead constraints is not necessary here
  VisParticleGroupManager_cl::GlobalManager().GlobalConstraints().HandleParticlesNoRemove(this, dtime, m_iConstraintAffectBitMask);
}



V_IMPLEMENT_SERIALX( ParticleGroupBase_cl);
void ParticleGroupBase_cl::SerializeX( VArchive &ar )
{
  // serialize base class
  VisParticleGroup_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    int iVersion;
    float fScaling;

    // version
    ar >> iVersion;

    // particle group data
    ar >> fScaling; SetScaling(fScaling);
    if (iVersion>=PARTICLEGROUPBASE_VERSION_002)
    {
      ar >> m_AmbientColor; // version 2
    }
    if (iVersion>=PARTICLEGROUPBASE_VERSION_003)
    {
      unsigned int iMask = 0xffffffff;
      ar >> iMask; // version 3
      SetVisibleBitmask(iMask);
    }

    if (iVersion>=PARTICLEGROUPBASE_VERSION_004)
      m_vWindSpeed.SerializeAsVisVector (ar);// version 4
    if (iVersion>=PARTICLEGROUPBASE_VERSION_005)
      ar >> m_bWindInLocalSpace; // version 5
    if (iVersion>=PARTICLEGROUPBASE_VERSION_006)
      ar >> m_bApplyTimeOfDayLight; // version 6
    if (iVersion>=PARTICLEGROUPBASE_VERSION_008)
      ar >> GetEmitter()->m_fIntensity; // version 8

    if (iVersion>=PARTICLEGROUPBASE_VERSION_007)
      ar >> m_pEmitterMeshEntity;

    if (iVersion>=PARTICLEGROUPBASE_VERSION_009)
    {
      int iVisFlags;
      ar >> iVisFlags;
      VisVisibilityObject_cl *pVisObj = GetVisibilityObject();
      if (pVisObj!=NULL)
      {
        pVisObj->SetVisibilityTestFlags(iVisFlags);
        if (pVisObj->GetVisData()!=NULL)
        {
          pVisObj->GetVisData()->SetAutomaticUpdate((iVisFlags&VIS_NO_VISZONE_ASSIGNMENT)==0);
        }
      }
    }

    if (iVersion>=PARTICLEGROUPBASE_VERSION_010)
    {
      BOOL checkVis = FALSE;
      ar >> checkVis;
      SetCheckVisibility(checkVis);
    }

    // constraints
    m_Constraints.SerializeX(ar);

    ParticleGroupBase_cl * pOnDestroyGroup = m_spOnDestroyCreateGroup;

    while(pOnDestroyGroup!=NULL && pOnDestroyGroup!=this /*avoid loops*/)
    {
      pOnDestroyGroup->m_Constraints.AddFrom(&m_Constraints);
      pOnDestroyGroup = pOnDestroyGroup->m_spOnDestroyCreateGroup; //continue with next group
    }

    // use white as a default value, because one cannot sample the light-grid during loading
    m_InstanceColor.SetRGB (255, 255, 255);

    // Evaluate the real color next frame, see EvaluateSceneBrightness
    m_bEvaluateBrightnessNextFrame = true;
  } 
  else
  {
    // version number
    ar << (int)PARTICLEGROUPBASE_VERSION_CURRENT; 

    // particle group data
    ar << GetScaling();
    ar << m_AmbientColor; // version 2
    ar << GetVisibleBitmask(); // version 3
    m_vWindSpeed.SerializeAsVisVector (ar);// version 4
    ar << m_bWindInLocalSpace; // version 5
    ar << m_bApplyTimeOfDayLight; // version 6
    ar << GetEmitter()->m_fIntensity; // version 8

    VisBaseEntity_cl *pEmitterMesh = NULL;
    if (GetEmitter()->m_wpEntityMeshEmitter!=NULL)
      pEmitterMesh = GetEmitter()->m_wpEntityMeshEmitter.GetPtr();
    ar << pEmitterMesh; // version 7
    int iVisFlags = (this->GetVisibilityObject() != NULL)? this->GetVisibilityObject()->GetVisibilityTestFlags() : 0;
    ar << iVisFlags; // version 9
    ar << GetCheckVisibility(); // version 10

    // constraints
    m_Constraints.SerializeX(ar);
  }
}

void ParticleGroupBase_cl::OnDeserializationCallback(const VSerializationContext &context)
{
  VisParticleGroup_cl::OnDeserializationCallback(context);

  GetEmitter()->SetMeshEmitterEntity(m_pEmitterMeshEntity);
  m_pEmitterMeshEntity = NULL;
  OnObject3DChanged( VIS_OBJECT3D_POSCHANGED | VIS_OBJECT3D_ORICHANGED);
  RespawnAllParticles(false);
}

bool ParticleGroupBase_cl::SetMeshEmitterEntity(VisBaseEntity_cl* pEntity)
{
	// allows changing the mesh emitter before 
	m_pEmitterMeshEntity = pEntity;
	VisParticleEmitter_cl *pEmitter = GetEmitter();
	if (pEmitter->GetType()==EMITTER_TYPE_MESH)
	{
		pEmitter->SetMeshEmitterEntity(pEntity);
		return pEmitter->m_spEmitterMesh!=NULL;
	} 
	else
	{
		pEmitter->SetMeshEmitterEntity(NULL);
		return false;
	}
}

// Virtual function that gets called whenever the position or orientation of the Object3D is changed
void ParticleGroupBase_cl::OnObject3DChanged(int iO3DFlags)
{
  VisParticleGroup_cl::OnObject3DChanged(iO3DFlags);

  if ( iO3DFlags & VIS_OBJECT3D_POSCHANGED )
  {
    EnsureUpdaterTaskFinished();

    m_bEvaluateBrightnessNextFrame |= m_spDescriptor->m_fApplySceneBrightness > 0.0f;

    hkvVec3 vDelta = GetPosition() - m_vOldPos;

  #ifdef _VISION_WIN32
    if ( Vision::Editor.IsInEditor() && !Vision::Editor.IsAnimatingOrPlaying() && !GetUseLocalSpaceMatrix())
    {
      MoveParticles( vDelta );

      // also move child group
      if (m_spOnDestroyCreateGroup)
        m_spOnDestroyCreateGroup->IncPosition(vDelta);
      vDelta.setZero(); // already handled
    }
  #endif
    m_vGroupMoveDeltaAccum += vDelta;

    UpdateBoundingBoxes();
  }
  if ( iO3DFlags & VIS_OBJECT3D_ORICHANGED )
  {
    EnsureUpdaterTaskFinished();

    hkvMat3 rotMat = GetRotationMatrix();
    hkvVec3 t1, t2, t3;
    rotMat.getAxisXYZ(&t1, &t2, &t3);
    m_vDirection = t1; 
    m_vRightDir = t2; 
    m_vUpDir = t3;

    UpdateBoundingBoxes();
  }

  m_vOldPos = GetPosition();

  // remove only the "change" flags
  ClearO3DChangeFlags();
}


void ParticleGroupBase_cl::SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace)
{
  m_vWindSpeed = vWind;
  m_bWindInLocalSpace = bApplyInLocalSpace;
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->SetWindSpeed(vWind);
}


void ParticleGroupBase_cl::SetVisibleBitmask(unsigned int iMask)
{
  VisParticleGroup_cl::SetVisibleBitmask(iMask);
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->SetVisibleBitmask(iMask);
}

void ParticleGroupBase_cl::UpdateBoundingBoxes(bool bImmediateVisibilityBoundingBoxUpdate)
{
  if (!m_spEmitter || !m_bUseVisibility)
    return;

  // Update ParticleBoundingBox only if necessary.
  float fParticleBoundingBoxUpdateInterval = m_spDescriptor->m_fDynamicInflateInterval;
  if (!m_ParticleBoundingBox.isValid() ||
      (fParticleBoundingBoxUpdateInterval >= 0.0f && m_fParticleBoundingBoxUpdateTimePos >= fParticleBoundingBoxUpdateInterval) ||
      !Vision::Editor.IsAnimatingOrPlaying())
  {
    m_fParticleBoundingBoxUpdateTimePos = hkvMath::mod(m_fParticleBoundingBoxUpdateTimePos, fParticleBoundingBoxUpdateInterval);
    InflateBoundingBox();
  }

  m_WorldSpaceBoundingBox.setInvalid();
  m_LocalSpaceBoundingBox.setInvalid();

  hkvMat4 transform;
  if (m_pParentEffect != NULL)
    transform.set(m_pParentEffect->GetRotationMatrix(), m_pParentEffect->GetPosition());
  else
    transform.set(GetRotationMatrix(), GetPosition());
  transform.setScalingFactors(hkvVec3(GetScaling()));


  // Determines if the m_WorldSpaceBoundingBox box is computed from m_LocalSpaceBoundingBox (true) or the other way round (false). 
  bool worldBoundingBoxFromLocal = false;

  if (fParticleBoundingBoxUpdateInterval >= 0.0f && m_ParticleBoundingBox.isValid())
  {
    // The space of m_ParticleBoundingBox depends on GetUseLocalSpaceMatrix()
    // Either way world or local bounding box must be computed from it.
    // Note that it is also possible that m_ParticleBoundingBox is invalid!

    if (GetUseLocalSpaceMatrix())
    {
      m_LocalSpaceBoundingBox = m_ParticleBoundingBox;
      worldBoundingBoxFromLocal = true;
    }
    else
    {
      m_WorldSpaceBoundingBox = m_ParticleBoundingBox;
      worldBoundingBoxFromLocal = false;
    }
  }
  else
  {
    // For mesh emitters use the mesh world bounding box and apply the descriptor bounding box as offset.
    if (m_spEmitter->GetType() == EMITTER_TYPE_MESH && m_pEmitterMeshEntity != NULL)
    {
      m_WorldSpaceBoundingBox.expandToInclude(m_pEmitterMeshEntity->GetBoundingBox());

      hkvVec3 vBoundary((m_spDescriptor->m_BoundingBox.m_vMax - m_spDescriptor->m_BoundingBox.m_vMin) * 0.5f * GetScaling());
      m_WorldSpaceBoundingBox.addBoundary(vBoundary);
    }
    // For all other emitter type use the rotated descriptor bounding box.
    else
    {
      hkvAlignedBBox orientedBBox = m_spDescriptor->m_BoundingBox;
      orientedBBox.transformFromOrigin(transform);

      m_WorldSpaceBoundingBox.expandToInclude(orientedBBox);
    }

    worldBoundingBoxFromLocal = false;
  }

  // Depending on which box is "native" compute the other (local/world) box.
  if (worldBoundingBoxFromLocal)
  {
    // Ensure that emitter position is included - important for fast moving particle effects with low fParticleBoundingBoxUpdateInterval!
    m_LocalSpaceBoundingBox.expandToInclude(m_spDescriptor->m_vRelativePosition);

    if (bImmediateVisibilityBoundingBoxUpdate)
      SetVisibilityBoundingBox(true, m_LocalSpaceBoundingBox);

    // Compute world space bounding box from local space.
    m_WorldSpaceBoundingBox = m_LocalSpaceBoundingBox;
    m_WorldSpaceBoundingBox.transformFromOrigin(transform);
  }
  else
  {
    // Ensure that emitter position is included - important for fast moving particle effects with low fParticleBoundingBoxUpdateInterval!
    m_WorldSpaceBoundingBox.expandToInclude(GetPosition());

    if (bImmediateVisibilityBoundingBoxUpdate)
      SetVisibilityBoundingBox(false, m_WorldSpaceBoundingBox);

    // Compute local space bounding box from world space.
    m_LocalSpaceBoundingBox = m_WorldSpaceBoundingBox;
    m_LocalSpaceBoundingBox.transformFromOrigin(transform.getInverse());
  }

  m_bScheduleVisibilityUpdate = !bImmediateVisibilityBoundingBoxUpdate;
}

/*
 * Havok SDK - Base file, BUILD(#20140621)
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
