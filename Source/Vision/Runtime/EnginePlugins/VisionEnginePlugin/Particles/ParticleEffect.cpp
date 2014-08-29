/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEffect.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenSpotDir.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VBaseShadowMapComponentSpotDirectional.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VTimeOfDayComponent.hpp>
#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>


////////////////////////////////////////////////////////////////////////////
// handle the particle effect instance
////////////////////////////////////////////////////////////////////////////


// macro to iterate through all valid groups
#define FOR_ALL_GROUPS \
  for (unsigned int i=0;i<m_iGroupCount;i++)\
  {\
    ParticleGroupBase_cl *pGroup = m_spGroups[i];\
    if (!pGroup) continue;

VisParticleEffect_cl::VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, unsigned int uiRandomSeed) :
  m_uiRandomBaseSeed(uiRandomSeed)
{
  VASSERT(pEffect);

  ResetValues();

  pEffect->InitParticleEffectInstance(this,vPos,vOri, 1.0f);
}

VisParticleEffect_cl::VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, unsigned int uiRandomSeed) :
  m_uiRandomBaseSeed(uiRandomSeed)
{
  VASSERT(pEffect);

  ResetValues();

  m_vPosition = vPos;
  m_vOrientation = vOri;

  pEffect->InitParticleEffectInstance(this,vPos,vOri, fScaling);
}

VisParticleEffect_cl::VisParticleEffect_cl() :
  m_uiRandomBaseSeed(0)
{
  ResetValues();
}

void VisParticleEffect_cl::ResetValues()
{
  m_bUseLightgridLighting = false;
  m_bUseDynamicLighting = false;
  m_bUseSHForDynamicLighting = false;

  m_spGroups = NULL;
  m_iGroupCount = 0;
  m_bPaused = m_bHalted = false;
  m_bRemoveWhenFinished = true;

  m_vPosition = hkvVec3(0.0f);
  m_vOrientation = hkvVec3(0.0f);

  m_vLightSamplingOffset = hkvVec3(0.0f);

  m_bAnyMeshEmitter = false;
  m_pDynamicLightingShaderConstants = NULL;
  m_sPreferredDirectionalLightSourceKey = "";
  m_uiLightInfluenceBitmask = V_BIT(0);
  m_uiLastDynamicLightListUpdateFrame = 0;
  m_pLastDynamicLightListUpdateContext = NULL;

  m_bNeedsShadowMapSelectionUpdate = false;
  m_spShadowReceiveShadowMap = NULL;

  m_uiDynamicLightingPriorityThreshhold = 64;
}


void VisParticleEffect_cl::CreateFromDescriptors(VisParticleGroupDescriptor_cl **pDescList,int iDescCount)
{
  V_SAFE_DELETE_ARRAY(m_spGroups);
  m_iGroupCount = iDescCount;
  m_spSourceFXFile = NULL;

  if (!iDescCount)
    return;
  
  // allocate the particle group pointers and fill them
  m_spGroups = new ParticleGroupBasePtr[iDescCount];
  for (unsigned int i=0;i<m_iGroupCount;i++)
  {
    ParticleGroupBase_cl *pLayer = new ParticleGroupBase_cl(pDescList[i],NULL, GetPosition(), GetOrientation(), true, GetRandomBaseSeed());
    m_spGroups[i] = pLayer;
    pLayer->m_pParentEffect = this;
    pLayer->m_iChildIndex = i;
    pLayer->AttachToParent(this);
    pLayer->Finalize();
  }
}


VisParticleEffect_cl::~VisParticleEffect_cl()
{
  V_SAFE_DELETE_ARRAY(m_spGroups);
  V_SAFE_DELETE_ARRAY(m_pDynamicLightingShaderConstants);
}


void VisParticleEffect_cl::Tick(float fTimeDiff)
{
  FOR_ALL_GROUPS
    if (pGroup->m_bHasTransformationCurves || pGroup->m_bHasEvents)
    {
      float &x(pGroup->m_fTransformationCurveTime);
      const float fDelta = pGroup->m_fTransformationCurveSpeed*fTimeDiff*pGroup->m_fTimeScale;
      if (pGroup->m_spDescriptor->m_spPositionCurve!=NULL)
      {
        hkvVec3 vPos = pGroup->m_spDescriptor->m_spPositionCurve->GetValueFast(x) * pGroup->m_fScaling;
        pGroup->SetLocalPosition(vPos);
      }
      if (pGroup->m_spDescriptor->m_spOrientationCurve!=NULL)
      {
        hkvVec3 vOri = pGroup->m_spDescriptor->m_spOrientationCurve->GetValueFast(x);
        vOri *= 360.f;
        pGroup->SetLocalOrientation(vOri);
      }
      if (pGroup->m_bHasEvents)
      {
        VisAnimEventList_cl &list(pGroup->m_spDescriptor->m_EventList);
        list.PrepareCurrentEventSet(x,x+fDelta,true); // no fmod here. note this method is not thread safe
        VisAnimEvent_cl *pEvent = list.GetNextEvent();
        while (pEvent!=NULL)
        {
          VisParticleLayerEventData_cl data(pGroup,pEvent);
          data.Trigger();
          pEvent = list.GetNextEvent();
        }
      }
      x = hkvMath::mod (x + fDelta,1.f);
    }
    if (m_bAnyMeshEmitter)
    {
      pGroup->GetEmitter()->UpdateMeshEmitterEntity(); // must be called from the main thread

      //Use this line to debug the bounding box of the emitter
      //Vision::Game.DrawBoundingBox(pGroup->m_BoundingBox, V_RGBA_CYAN);    
    }

    pGroup->HandleParticles(fTimeDiff);
  }
}

bool VisParticleEffect_cl::IsLifeTimeOver() const
{
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    ParticleGroupBase_cl *pLayer = m_spGroups[i];
    if (pLayer == NULL || pLayer->IsDead())
      continue;

    if (!pLayer->IsLifeTimeOver())
      return false;
  }
  return true;
}

float VisParticleEffect_cl::GetRemainingLifeTime() const
{
  float fMax = 0.f;
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    ParticleGroupBase_cl *pLayer = m_spGroups[i];
    if (pLayer==NULL || pLayer->IsDead())
      continue;

    if (pLayer->m_bInfiniteLifeTime)
      return -1.f;

    float fLayerLifeTime = pLayer->GetRemainingLifeTime();
    fMax = hkvMath::Max(fMax,fLayerLifeTime);
  }
  return fMax;
}

void VisParticleEffect_cl::AddGroupsToCollection(VisParticleGroupCollection_cl &dest)
{
  dest.EnsureSize(dest.GetSize()+m_iGroupCount);
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    ParticleGroupBase_cl *pLayer = m_spGroups[i];
    if (pLayer==NULL || pLayer->IsDead())
      continue;

    dest.AppendEntryFast(pLayer);
  }
}

void VisParticleEffect_cl::UpdateLightGrid()
{
  if (!m_bUseLightgridLighting)
    return;
  hkvVec3 vPos = GetPosition();
  VLightGrid_cl *pLightGrid = this->GetRelevantLightGrid();
  if (pLightGrid)
  {
    pLightGrid->GetColorsAtPositionI(vPos + m_vLightSamplingOffset, m_OwnLGColors);
    if (pLightGrid->GetLightGridType() == VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL)
    {
      m_OwnLGColors[0] *= 2.0f;
      m_OwnLGColors[1] *= 2.0f;
    }
  }
}

void VisParticleEffect_cl::UpdateLightTrackingSettings()
{
  m_bUseLightgridLighting = false;
  m_bUseDynamicLighting = false;
  m_bUseSHForDynamicLighting = false;
  m_bNeedsShadowMapSelectionUpdate = false;

  for (unsigned int i=0; i<m_iGroupCount; i++)
  {
    ParticleGroupBase_cl *pGroup = m_spGroups[i];
    if (!pGroup)
      continue;

    m_bUseDynamicLighting = m_bUseDynamicLighting || pGroup->GetLightingDynamic();
    m_bUseLightgridLighting = m_bUseLightgridLighting || (pGroup->GetLightGridColorPtr() == m_OwnLGColors);
    m_bNeedsShadowMapSelectionUpdate = m_bNeedsShadowMapSelectionUpdate || pGroup->GetShadowReceive();

    bool bMobileShader = Vision::GetApplication()->GetShaderProvider()->GetUseMobileParticleShader();
    if(m_bUseDynamicLighting && bMobileShader)
      m_bUseSHForDynamicLighting = true; 

    VASSERT_MSG(!m_bUseDynamicLighting || (m_bUseSHForDynamicLighting == bMobileShader), "In a single particle effect there are groups both with mobile lighting and with non-mobile lighting. This is not allowed!");
  }

  if(m_bUseDynamicLighting)
  {
    if(m_pDynamicLightingShaderConstants == NULL)
      m_pDynamicLightingShaderConstants = new hkvVec4[VisParticleGroup_cl::s_uiNumDynamicLightShaderConstantRegisters];
  }
  else
  {
    V_SAFE_DELETE_ARRAY(m_pDynamicLightingShaderConstants);
  }

  if (!m_bNeedsShadowMapSelectionUpdate)
  {
    m_spShadowReceiveShadowMap = NULL;
  }

  // Force single lightgrid update if needed since the object could be moved since the last time being statically lit.
  if(m_bUseLightgridLighting)
  {
    UpdateLightGrid();
  }
}

// Virtual function that gets called whenever the position or orientation of the Object3D is changed
void VisParticleEffect_cl::OnObject3DChanged(int iO3DFlags)
{
  // the binding takes care of moving the child emitter (with local space positions)
  VisObject3D_cl::OnObject3DChanged(iO3DFlags);

  if ( iO3DFlags & VIS_OBJECT3D_POSCHANGED )
  {
    UpdateLightGrid();

    // Update scene brightness as this also can involve a form of lightgrid sampling
    EvaluateSceneBrightness();
  }

  // remove only the "change" flags
  ClearO3DChangeFlags();
}


static int PARTICLETRIGGER_ID_PAUSE = -1;
static int PARTICLETRIGGER_ID_RESUME = -1;
static int PARTICLETRIGGER_ID_RESTART = -1;

void VisParticleEffect_cl::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisObject3D_cl::MessageFunction(iID,iParamA,iParamB);
  if (iID == VIS_MSG_TRIGGER)
  {
    EnsureComponentIDsRegistered();

    // the trigger components are added by vForge and only serialized if used
    // See EngineInstanceParticleGroup::c'tor
    VisTriggerTargetComponent_cl *pTarget = reinterpret_cast<VisTriggerTargetComponent_cl*>(iParamB);
    if (pTarget->m_iComponentID == PARTICLETRIGGER_ID_PAUSE)
      SetPause(true);
    else if (pTarget->m_iComponentID == PARTICLETRIGGER_ID_RESUME)
      SetPause(false);
    else if (pTarget->m_iComponentID == PARTICLETRIGGER_ID_RESTART)
      Restart();
    
    return;
  }
}

void VisParticleEffect_cl::EnsureComponentIDsRegistered()
{
  // cache IDs to avoid string comparisons
  if (VISION_UNLIKELY(PARTICLETRIGGER_ID_PAUSE == -1))
  {
    PARTICLETRIGGER_ID_PAUSE = IVObjectComponent::RegisterStringID(PARTICLETRIGGER_PAUSE);
    PARTICLETRIGGER_ID_RESUME = IVObjectComponent::RegisterStringID(PARTICLETRIGGER_RESUME);
    PARTICLETRIGGER_ID_RESTART = IVObjectComponent::RegisterStringID(PARTICLETRIGGER_RESTART);
  }
}

void VisParticleEffect_cl::IncPosition(const hkvVec3& vDelta, bool bMoveParticles)
{
  VisObject3D_cl::IncPosition(vDelta);

  if ( bMoveParticles)
  {
    // do not manually update particles in vForge edit mode
    // (happens automatically in OnObject3DChanged function)
    if ( Vision::Editor.IsInEditor() && !Vision::Editor.IsAnimatingOrPlaying() )
      return;

//    m_vOldPos = GetPosition();
    FOR_ALL_GROUPS
      if (!pGroup->GetUseLocalSpaceMatrix())
        pGroup->MoveParticles( vDelta );
    }
  }
}

void VisParticleEffect_cl::SetScaling(float fScale)
{
  FOR_ALL_GROUPS
    pGroup->SetScaling(fScale);
  }
}

void VisParticleEffect_cl::SetVisible(bool bStatus)
{
  FOR_ALL_GROUPS
    pGroup->SetVisible(bStatus);
  }
}

void VisParticleEffect_cl::SetAmbientColor(VColorRef iColor)
{
  FOR_ALL_GROUPS
    pGroup->SetAmbientColor(iColor);
  }
}

void VisParticleEffect_cl::EvaluateSceneBrightness()
{
  FOR_ALL_GROUPS
    pGroup->EvaluateSceneBrightness();
  }

  // Particle color values are not automatically updated if we are either in editor and not running or the particle is paused.
  // Doing this update twice a frame is not only very wasteful but also causes bugs with trail particles.
  if (IsPaused() || (Vision::Editor.IsInEditor() && !Vision::Editor.IsPlaying()))
  {
    FOR_ALL_GROUPS
      pGroup->HandleParticles(0.0f);
    }
  }
}

void VisParticleEffect_cl::SetVisibleBitmask(unsigned int iMask)
{
  FOR_ALL_GROUPS
    pGroup->SetVisibleBitmask(iMask & pGroup->m_spDescriptor->m_iVisibleBitmask);
  }
}

bool VisParticleEffect_cl::IsVisible() const 
{    
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    if (m_spGroups[i] != NULL && m_spGroups[i]->GetVisibleBitmask() != 0)
      return true;
  }

  return false;
}

void VisParticleEffect_cl::UpdateVisibilityBoundingBox()
{
  UpdateBoundingBoxes();
}

void VisParticleEffect_cl::UpdateBoundingBoxes(bool bImmediateVisibilityBoundingBoxUpdate)
{
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    if (m_spGroups[i] != NULL)
      m_spGroups[i]->UpdateBoundingBoxes(bImmediateVisibilityBoundingBoxUpdate);
  }
}

void VisParticleEffect_cl::SetPause(bool bStatus)
{
  m_bPaused = bStatus;
  FOR_ALL_GROUPS
    pGroup->SetPause(bStatus);
  }
}

void VisParticleEffect_cl::SetHalted(bool bStatus)
{
  m_bHalted = bStatus;
  FOR_ALL_GROUPS
    pGroup->SetHalted(bStatus);
  }
}

void VisParticleEffect_cl::SetApplyTimeOfDayLight(bool bApply)
{
  FOR_ALL_GROUPS
    pGroup->SetApplyTimeOfDayLight(bApply);
  }
}

bool VisParticleEffect_cl::GetApplyTimeOfDayLight(void) const
{
  FOR_ALL_GROUPS
    if (pGroup->GetApplyTimeOfDayLight ())
      return (true);
  }

  return (false);
}

void VisParticleEffect_cl::SetFinished()
{
  FOR_ALL_GROUPS
    pGroup->SetFinished();
  }
}

void VisParticleEffect_cl::SetHandleWhenVisible(bool bStatus)
{
  FOR_ALL_GROUPS
    pGroup->SetHandleWhenVisible(bStatus);
  }
}

bool VisParticleEffect_cl::IsUpdatedOnlyWhenVisible()
{
  FOR_ALL_GROUPS
    if (!pGroup->GetHandleWhenVisible() || pGroup->m_bUpdateLifetimeIfInvisible) return false;
  }
  return true;
}

bool VisParticleEffect_cl::WasRecentlyRendered()
{ 
  FOR_ALL_GROUPS
    if (pGroup->WasRecentlyRendered()) return true;
  }
  return false;
}

void VisParticleEffect_cl::Restart()
{
  FOR_ALL_GROUPS
    pGroup->m_bIsDead = false;
    pGroup->SetBaseSeed(m_uiRandomBaseSeed);
    pGroup->InitGroup(pGroup->m_spDescriptor, pGroup->m_spEmitter, pGroup->GetPosition(), pGroup->GetOrientation(), pGroup->GetScaling(), true);
    pGroup->m_pParentEffect = this;
    pGroup->m_iChildIndex = i;
    pGroup->AttachToParent(this);
    pGroup->Finalize();
  }
}

void VisParticleEffect_cl::ModSysNotifyFunctionCommand(int command, void *param)
{
  if (command==VIS_MODSYSCMD_FINISHPLAYBACKONDISPOSE)
  {
    VASSERT_MSG(param!=NULL,"Command VIS_MODSYSCMD_FINISHPLAYBACKONDISPOSE needs the parameter pointer to be set.");
    if (!param) return;
    if ((*(bool*)param)==false)
    {
      RemoveObjectFlag(VObjectFlag_FinishPlaybackOnDispose);
    }
    else
    {
      SetObjectFlag(VObjectFlag_FinishPlaybackOnDispose);
    }
  }
}

void VisParticleEffect_cl::ModSysNotifyFunctionParentAltered(int iFlags)
{
  // If this call was generated by a Reposition call, Offset particles
  if (iFlags & VIS_MODSYSFLAG_REPOSITION)
  {
    FOR_ALL_GROUPS
      if (!pGroup->GetUseLocalSpaceMatrix())
        pGroup->EnsureUpdaterTaskFinished();
    }

    hkvVec3 vOriginalPos = GetPosition();
    VisObject3D_cl::ModSysNotifyFunctionParentAltered(iFlags);

    hkvVec3 vDelta = GetPosition()-vOriginalPos;
    FOR_ALL_GROUPS
      if (pGroup->GetUseLocalSpaceMatrix())
        continue;
       pGroup->MoveParticles( vDelta );
       pGroup->m_vGroupMoveDeltaAccum.setZero();// -= vDelta;
       pGroup->m_spEmitter->m_vLastEmitterPos = pGroup->GetPosition();
       pGroup->m_vOldPos = pGroup->GetPosition();
    }
  }
  else
  {
    VisObject3D_cl::ModSysNotifyFunctionParentAltered(iFlags);
  }
}


void VisParticleEffect_cl::DisposeObject()
{
  FOR_ALL_GROUPS
    pGroup->EnsureUpdaterTaskFinished();
  }

  // If the particle effect is still alive and marked as "FinishPlaybackOnDispose",
  // we do not dispose it immediately. Instead, we set it as finished so that it
  // is automatically disposed when all its layers die.
  if (!IsDead() && IsObjectFlagSet(VObjectFlag_FinishPlaybackOnDispose))
  {
    SetRemoveWhenFinished(true);
    SetFinished();
    SetHandleWhenVisible(false); // Lifetime needs to be updated even when not visible
  }
  else
  {
    // dispose all layers as well
    FOR_ALL_GROUPS
      pGroup->DisposeObject();
    }
    V_SAFE_DELETE_ARRAY(m_spGroups);
    m_iGroupCount = 0;

    VisObject3D_cl::DisposeObject();
    if (!m_spSourceFXFile || !m_spSourceFXFile->GetParentManager())
      return;
    m_spSourceFXFile->GetParentManager()->Instances().SafeRemove(this);
  }
}

void VisParticleEffect_cl::AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence)
{
  FOR_ALL_GROUPS
    pGroup->AddRelevantConstraints(pSrcList,bCheckInfluence);
  }
}

void VisParticleEffect_cl::AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence)
{
  FOR_ALL_GROUPS
    pGroup->AddConstraint(pConstraint,bCheckInfluence);
  }
}

void VisParticleEffect_cl::SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace)
{
  FOR_ALL_GROUPS
    pGroup->SetWindSpeed(vWind, bApplyInLocalSpace);
  }
}

void VisParticleEffect_cl::SetMeshEmitterEntity(VisBaseEntity_cl *pEntity)
{
  m_bAnyMeshEmitter = false;
  FOR_ALL_GROUPS
	  m_bAnyMeshEmitter |= pGroup->SetMeshEmitterEntity(pEntity);
  }
}

PARTICLE_IMPEXP void VisParticleEffect_cl::SetPreferredDirLightKey(const char* szPreferredDirectionalLightSourceKey)
{
  m_sPreferredDirectionalLightSourceKey = szPreferredDirectionalLightSourceKey;
}

void VisParticleEffect_cl::RemoveConstraint(VisParticleConstraint_cl *pConstraint)
{
  FOR_ALL_GROUPS
    pGroup->RemoveConstraint(pConstraint);
  }
}

void VisParticleEffect_cl::RemoveAllConstraints()
{
  FOR_ALL_GROUPS
    pGroup->RemoveAllConstraints();
  }
}

void VisParticleEffect_cl::SetIntensity(float fIntensity)
{
  FOR_ALL_GROUPS
    pGroup->GetEmitter()->SetIntensity(fIntensity);
  }
}


void VisParticleEffect_cl::RespawnAllParticles(bool bUseOldCount)
{
  FOR_ALL_GROUPS
    pGroup->RespawnAllParticles(bUseOldCount);
  }
}

void VisParticleEffect_cl::TeleportSpawnPosition()
{
  FOR_ALL_GROUPS
    pGroup->TeleportSpawnPosition();
  }
}

void VisParticleEffect_cl::RenderParticleBoundingBoxes()
{
  FOR_ALL_GROUPS
    pGroup->RenderParticleBoundingBoxes();
  }
}

bool VisParticleEffect_cl::GetLocalBoundingBox(hkvAlignedBBox &destBox) const
{
  hkvAlignedBBox tempBox;
  destBox.setInvalid();
  FOR_ALL_GROUPS
    if (!pGroup->GetDescriptor()->GetBoundingBox(tempBox)) continue;
    destBox.expandToInclude(tempBox);
  }
  return destBox.isValid();
}


bool VisParticleEffect_cl::GetCurrentBoundingBox(hkvAlignedBBox &destBox) const
{
  destBox.setInvalid();
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    if (m_spGroups[i] != NULL)
      destBox.expandToInclude(m_spGroups[i]->GetUseLocalSpaceMatrix() ? m_spGroups[i]->GetLocalSpaceBoundingBox() : m_spGroups[i]->GetWorldSpaceBoundingBox());
  }
  return destBox.isValid();
}

void VisParticleEffect_cl::GetLocalSpaceBoundingBox(hkvAlignedBBox &destBox) const
{
  destBox.setInvalid();
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    if (m_spGroups[i] != NULL)
      destBox.expandToInclude(m_spGroups[i]->GetLocalSpaceBoundingBox());
  }
}

void VisParticleEffect_cl::GetWorldSpaceBoundingBox(hkvAlignedBBox &destBox) const
{
  destBox.setInvalid();
  for (unsigned int i = 0; i < m_iGroupCount; ++i)
  {
    if (m_spGroups[i] != NULL)
      destBox.expandToInclude(m_spGroups[i]->GetWorldSpaceBoundingBox());
  }
}

void VisParticleEffect_cl::OnSingleGroupFinished(ParticleGroupBase_cl *pFinishedGroup)
{

#ifdef _VISION_WIN32
  if (VisParticleGroupManager_cl::g_bLoopAllEffects) // this feature is only relevant inside the editor
  {

    bool bAllDead = true;
    FOR_ALL_GROUPS
      if (!pGroup->m_bIsDead)
      {
        bAllDead = false;
        break;
      }
    }
    if (bAllDead)
    {
      Restart();
    }
    return;
  }
#endif

  if (!m_bRemoveWhenFinished)
    return;

  int iMaxValid = 0;
  FOR_ALL_GROUPS
    if (pGroup==pFinishedGroup)
    {
      pGroup->AttachToParent(NULL);
      m_spGroups[i] = NULL; // remove from list, pFinishedGroup might not be valid anymore afterwards
      continue;
    }
    iMaxValid = i+1;
  }
  m_iGroupCount = iMaxValid; // new upper limit. If 0, group is "dead"
}


const char SERIALMODE_FILENAME = 0; // serializes the file name of the effect file and goes through the manager (XML parsing)
const char SERIALMODE_FULL     = 1; // binary serializes the effects into the file

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=178
#endif

#if defined(__ghs__)
#pragma ghs nowarning 177
#endif

const int PARTICLEEFFECT_VERSION_0 = 0;
const int PARTICLEEFFECT_VERSION_1 = 1; // effect key, serialize mode
const int PARTICLEEFFECT_VERSION_2 = 2; // group count bugfix
const int PARTICLEEFFECT_VERSION_3 = 3; // serialize paused & halted
const int PARTICLEEFFECT_VERSION_4 = 4; // add m_uiRandomBaseSeed
const int PARTICLEEFFECT_VERSION_5 = 5; // add m_uiLightInfluenceBitmask, m_szPreferredDirectionalLightSourceKey, m_vLightSamplingOffset
const int PARTICLEEFFECT_VERSION_6 = 6; // add m_bRemoveWhenFinished
#define PARTICLEEFFECT_CURRENTVERSION  PARTICLEEFFECT_VERSION_6

#if defined(__SNC__)
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif


V_IMPLEMENT_SERIAL( VisParticleEffect_cl, VisObject3D_cl, 0, &g_VisionEngineModule );
void VisParticleEffect_cl::Serialize(VArchive &ar)
{
  VisObject3D_cl::Serialize(ar);
  char iSerializeMode = SERIALMODE_FILENAME;
  int iVersion = PARTICLEEFFECT_CURRENTVERSION;

  if (ar.IsLoading())
  {
    ar >> iVersion; VASSERT(iVersion>=0 && iVersion<=PARTICLEEFFECT_CURRENTVERSION);
    if (iVersion>=PARTICLEEFFECT_VERSION_1)
    {
      ar >> iSerializeMode;
      VASSERT(iSerializeMode==SERIALMODE_FILENAME || iSerializeMode==SERIALMODE_FULL);
    }

    if (iVersion>=PARTICLEEFFECT_VERSION_4)
      ar >> m_uiRandomBaseSeed;

    if (iSerializeMode==SERIALMODE_FILENAME)
    {
      // read effect filename
      char szFXFilename[FS_MAX_PATH+1];
      ar.ReadStringBinary(szFXFilename,FS_MAX_PATH);
      m_spSourceFXFile = VisParticleGroupManager_cl::GlobalManager().LoadFromFile(szFXFilename);
      VASSERT(m_spSourceFXFile);
    }
    else 
    if (iSerializeMode==SERIALMODE_FULL)
    {
      VASSERT(!"Not supported yet");
      VisParticleEffectFile_cl *pFX = NULL;
      ar >> pFX;
      m_spSourceFXFile = pFX;
    }
    m_spSourceFXFile->InitParticleEffectInstance(this);

    if (iVersion>=PARTICLEEFFECT_VERSION_1)
    {
      // OLD key, now in object3d
      if (ar.GetLoadingVersion()<VISION_ARCHIVE_VERSION_025)
        ar >> m_sObjectKey;
    }

    if (iVersion>=PARTICLEEFFECT_VERSION_3)
    {
      ar >> m_bPaused >> m_bHalted;
    }

    // read properties for each particle group. Read count must match actual count from the effect!
    unsigned int iGroupCount;
    ar >> iGroupCount;

    m_bAnyMeshEmitter = false;
    VASSERT(iGroupCount <= m_iGroupCount);
    for (unsigned int i = 0; i < iGroupCount; i++)
    {
      bool bPresent = true;
      if (iVersion>=PARTICLEEFFECT_VERSION_2)
        ar >> bPresent;
      if (bPresent)
      {
        m_spGroups[i]->SerializeX(ar);
        m_bAnyMeshEmitter |= m_spGroups[i]->GetEmitter()->m_spEmitterMesh!=NULL;
      }
      else
        m_spGroups[i] = NULL;
    }
//    m_vOldPos = GetPosition();

    if(iVersion >= PARTICLEEFFECT_VERSION_5)
    {
      ar >> m_uiLightInfluenceBitmask;
      ar >> m_sPreferredDirectionalLightSourceKey;
      ar >> m_vLightSamplingOffset;
    }
    if (iVersion >= PARTICLEEFFECT_VERSION_6)
    {
      ar >> m_bRemoveWhenFinished;
    }
  }
  else
  {
    ar << iVersion;
    ar << iSerializeMode;
    ar << m_uiRandomBaseSeed;
    VASSERT(m_spSourceFXFile);
    if (iSerializeMode==SERIALMODE_FILENAME)
    {
      ar << m_spSourceFXFile->GetFilename();
    }
    else
    if (iSerializeMode==SERIALMODE_FULL)
    {
      ar << m_spSourceFXFile;
    }

    //ar << m_sEffectKey; now in object 3d
    ar << m_bPaused << m_bHalted;

    // save the properties (position, constraints,..) of each particle group separately
    ar << m_iGroupCount;
    for (unsigned int i = 0; i < m_iGroupCount; i++)
    {
      if (m_spGroups[i])
      {
        ar << (bool)true;
        m_spGroups[i]->SerializeX(ar);
      }
      else
        ar << (bool)false;
    }

    // Version 5
    ar << m_uiLightInfluenceBitmask;
    ar << m_sPreferredDirectionalLightSourceKey;
    ar << m_vLightSamplingOffset;

    // Version 6
    ar << m_bRemoveWhenFinished;
  }
}


void VisParticleEffect_cl::OnDeserializationCallback(const VSerializationContext &context)
{
  VisObject3D_cl::OnDeserializationCallback(context);

  for (unsigned int i = 0; i < m_iGroupCount; i++)
  {
    if (m_spGroups[i] != NULL )
    {
      m_spGroups[i]->OnDeserializationCallback(context);
      m_bAnyMeshEmitter |= m_spGroups[i]->GetEmitter()->m_spEmitterMesh!=NULL;
    }
  }
  UpdateLightGrid();
  SetPause(m_bPaused);
  SetHalted(m_bHalted);
}

unsigned int VisParticleEffect_cl::GetLightScreenSpaceOverlapArea(const VisLightSource_cl& lightSource, const hkvAlignedBBox& particleEffectBoundingBox, const hkvMat4& viewProjectionMatrix) const
{
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  if(!pContext)
    return 0;

  int iScreenWidth,iScreenHeight;
  pContext->GetSize(iScreenWidth, iScreenHeight);

  if(lightSource.GetType() == VIS_LIGHT_DIRECTED)
  {
    // Directional lights influence the whole particle effect.
    // Since computing its number of screen pixels is unnecessarily complicated, just output screen pixel count.
    return iScreenWidth * iScreenHeight; 
  }

  hkvAlignedBBox intersectionBoundingBox;
  lightSource.GetBoundingBox(intersectionBoundingBox);
  intersectionBoundingBox.reduceToIntersection(particleEffectBoundingBox);
  
  // No intersection?
  if(!intersectionBoundingBox.isValid())
    return 0;

  // Estimate by max rect in view 
  hkvVec3 corners[8];
  intersectionBoundingBox.getCorners(corners);
  hkvVec2 screenMin(1);
  hkvVec2 screenMax(-1);
  for(int i = 0; i < 8; ++i)
  {
    hkvVec4 projectedPos = viewProjectionMatrix.transform(corners[i].getAsVec4(1.0f));
    hkvVec2 screenPos = projectedPos.getAsVec2() / projectedPos.w;
    screenMin.setMin(screenPos);
    screenMax.setMax(screenPos);
  }
  
  // clip corners of bounding rectangle
  screenMin.clampTo(hkvVec2(-1.0f, -1.0f), hkvVec2(1.0f, 1.0f));
  screenMax.clampTo(hkvVec2(-1.0f, -1.0f), hkvVec2(1.0f, 1.0f));

  // calculate influence area 
  unsigned int iWidth = static_cast<unsigned int>((screenMax.x - screenMin.x) * 0.5f * iScreenWidth);
  unsigned int iHeight = static_cast<unsigned int>((screenMax.y - screenMin.y) * 0.5f * iScreenHeight);

  return iWidth * iHeight;
}

void VisParticleEffect_cl::UpdateDynamicLightSH()
{
  // Get all visible light sources.
  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisColl == NULL)
    return;
  const VisLightSrcCollection_cl *pLightSourceCollection = pVisColl->GetVisibleLights();
  if (pLightSourceCollection == NULL)
    return;

  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  m_uiLastDynamicLightListUpdateFrame = pContext->GetLastRenderedFrame();
  m_pLastDynamicLightListUpdateContext = pContext;
  const hkvVec3& vCamPos = pContext->GetCamera()->GetPosition();

  hkvVec3 shColorCoefficient00(0.0f); // band zero (ambient)
  hkvVec3 shColorCoefficient10(0.0f); // band one
  hkvVec3 shColorCoefficient11(0.0f);
  hkvVec3 shColorCoefficient12(0.0f);

  // Position at which the sh will be evaluated.
  hkvVec3 spherePosition = GetPosition() + m_vLightSamplingOffset;

  for (unsigned int iLightIdx = 0; iLightIdx < pLightSourceCollection->GetNumEntries(); iLightIdx++)
  {
    VisLightSource_cl* pLight = pLightSourceCollection->GetEntry(iLightIdx);
    if (!pLight->IsDynamic())
      continue;

    // Compute attenuation and light direction - using same model as used in dynamic lighting shader
    float lightAttenuation = 1.0f;
    hkvVec3 lightDirection;
    if(pLight->GetType() != VIS_LIGHT_DIRECTED)
    {
      lightDirection = pLight->GetPosition() - spherePosition;
      float lightDistance = lightDirection.getLength();
      if(lightDistance != 0.0f) // Following computations can handle lightDirection==0 properly.
        lightDirection /= lightDistance;

      // Distance attenuation (linear)
      lightAttenuation = hkvMath::saturate(1.0f - lightDistance / pLight->GetRadius());

      // Spot attenuation (quadratic)
      if(pLight->GetType() == VIS_LIGHT_SPOTLIGHT)
      {
        float alpha = lightDirection.dot(-pLight->GetDirection());
        float halfCosAngle = hkvMath::cosDeg(pLight->GetProjectionAngle() * 0.5f);
        lightAttenuation *= hkvMath::saturate((alpha - halfCosAngle) / (1.0f - halfCosAngle));
      }

      if(lightAttenuation == 0.0f)
        continue;
    }
    else
      lightDirection = -pLight->GetDirection();
    
    // scaled color
    hkvVec3 lightColor = pLight->GetColor().ToFloat() * (lightAttenuation * pLight->GetMultiplier() * pLight->GetFadeWeight(vCamPos));

    
    // Compute integral for wrapped diffuse defined as:
    // max(0, cos(theta) + X) / (1+X)
    // theta: angle between normal and light dir ("NdotL")
    // X: Backlight amount (needs to be within 0-1!)
    // 
    // Assume LightDir as fixed (0,0,1) and integrate over all normals to obtain SH - only middle coef of every band non-zero!
    // (These are sphere surface integrals - notation is WolframAlpha.com compatible ;)
    // Band0: int (1/(2*sqrt(pi)) * max(0,cos(p) + X) / (1+X) * sin(p)) dp dt, t=0...2pi, p=0...pi
    // -> sqrt(PI) / 2 * (X+1)
    // Band1: int (sqrt(3)/(2*sqrt(pi)) * max(0,cos(p) + X) / (1+X) * sin(p) * cos(p)) dp dt, t=0...2pi, p=0...pi
    // -> 0.5 * sqrt(PI/3) (-X*X+X+2)
    //
    // Rotation such a zonal harmonic is easy! see http://blog.blackhc.net/2010/06/rotation-of-low-order-spherical-harmonics/
    // 
    // However backlight is defined per group rather than per effect.
    // That means, that the factors
    // float x0 = backlight + 1.0f;
    // float x1 = (-backlight*backlight + backlight + 2.0f);
    // .. have to be applied to the corresponding bands later - this is done on the fly by ParticleGroupBase_cl::GetLightingShaderConstants
    // 
    // To sample the light normally one would convert the normal to a sampling function. That basically means projecting into the SPH basis.
    // This is done by: float4(1, normal.yzx) * float4(0.2820947918f, -0.4886025119f, 0.4886025119f, -0.4886025119f)
    // (Which is just evaluating the first two SH bands with cartesian coordinates - see http://www.ppsloan.org/publications/StupidSH36.pdf - Appendix A2
    //
    // These factors can premultiplied... and yield exactly 0.25 for EACH coefficient!
    // This means that we can apply this factor to the resulting coefficients instead of every single sh coefficient.

    // Add to sh color coefs.
    shColorCoefficient00 += lightColor;
    shColorCoefficient10 += lightColor * lightDirection.y;
    shColorCoefficient11 += lightColor * lightDirection.z;
    shColorCoefficient12 += lightColor * lightDirection.x;
  }

  // Pack into constants and apply earlier mentioned factors.
  m_pDynamicLightingShaderConstants[0] = hkvVec4(shColorCoefficient00.r, shColorCoefficient10.r, shColorCoefficient11.r, shColorCoefficient12.r) * 0.25f;
  m_pDynamicLightingShaderConstants[1] = hkvVec4(shColorCoefficient00.g, shColorCoefficient10.g, shColorCoefficient11.g, shColorCoefficient12.g) * 0.25f;
  m_pDynamicLightingShaderConstants[2] = hkvVec4(shColorCoefficient00.b, shColorCoefficient10.b, shColorCoefficient11.b, shColorCoefficient12.b) * 0.25f;
}

void VisParticleEffect_cl::UpdateDynamicLightLists()
{
  DynamicLightListEntry aDynamicLightListDirectional[VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsDirectional];
  DynamicLightListEntry aDynamicLightListPoint[VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsPoint];
  DynamicLightListEntry aDynamicLightListSpot[VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsSpot];

  // Get all visible light sources.
  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisColl == NULL)
    return;
  const VisLightSrcCollection_cl *pLightSourceCollection = pVisColl->GetVisibleLights();
  if (pLightSourceCollection == NULL)
    return;

  hkvAlignedBBox effectBoundingBox;
  GetWorldSpaceBoundingBox(effectBoundingBox);

  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  m_uiLastDynamicLightListUpdateFrame = pContext->GetLastRenderedFrame();
  m_pLastDynamicLightListUpdateContext = pContext;
  const hkvVec3& vCamPos = pContext->GetCamera()->GetPosition();
  hkvMat4 mViewProjection = pContext->getProjectionMatrix() * pContext->GetCamera()->GetWorldToCameraTransformation();

  // Insert into dynamic light lists
  for (unsigned int iLightIdx = 0; iLightIdx < pLightSourceCollection->GetNumEntries(); iLightIdx++)
  {
    DynamicLightListEntry lightEntry;
    lightEntry.pLight = pLightSourceCollection->GetEntry(iLightIdx);

    // We are only interested in dynamic lights that are not faded out.
    if (!lightEntry.pLight->IsDynamic())
      continue;
    const float fFade = lightEntry.pLight->GetMultiplier() * lightEntry.pLight->GetFadeWeight(vCamPos);
    if (fFade <= HKVMATH_LARGE_EPSILON)
      continue;

    // Check light influence mask.
    if((lightEntry.pLight->GetLightInfluenceBitMaskEntity() & m_uiLightInfluenceBitmask) == 0)
      continue;

    // Compute priority and ignore if no pixels affected
    lightEntry.uiPriority = GetLightScreenSpaceOverlapArea(*lightEntry.pLight, effectBoundingBox, mViewProjection);
    if(lightEntry.uiPriority < m_uiDynamicLightingPriorityThreshhold)
      continue;

    // Insert into one of the lists. Always overwrite the one with the smallest priority
    // Since the lists are very small sorting sounds like overkill.
    
    // Determine right list and size.
    DynamicLightListEntry* pLightList = NULL;
    unsigned int uiLightListSizeMax = 0;
    switch(lightEntry.pLight->GetType())
    {
    case VIS_LIGHT_DIRECTED:
    {
      // If key equals the preferred key, set priority to max.
      if(!m_sPreferredDirectionalLightSourceKey.IsEmpty() &&  m_sPreferredDirectionalLightSourceKey.CompareI(lightEntry.pLight->GetObjectKey()) == 0)
        lightEntry.uiPriority = UINT_MAX;
      else
      {
        // Skip if it is a time of day backlight!
        VTimeOfDayComponent* pTimeOfDayComponent = lightEntry.pLight->Components().GetComponentOfType<VTimeOfDayComponent>();
        if(pTimeOfDayComponent != NULL)
        {
          if(pTimeOfDayComponent->GetAttachementType() == TIMEOFDAY_ATTACHMENT_SUNBACKLIGHTSOURCE)
            continue;
          // Prefer time of day lights!
          else
            lightEntry.uiPriority *= 2;
        }

        // Shadow component doubles priority
        VBaseShadowMapComponentSpotDirectional* pShadowMapComponent = lightEntry.pLight->Components().GetComponentOfBaseType<VBaseShadowMapComponentSpotDirectional>();
        if(pShadowMapComponent != NULL)
          lightEntry.uiPriority *= 4;
      }

      pLightList = aDynamicLightListDirectional;
      uiLightListSizeMax = VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsDirectional;
      break;
    }

    case VIS_LIGHT_POINT:
      pLightList = aDynamicLightListPoint;
      uiLightListSizeMax = VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsPoint;
      break;

    case VIS_LIGHT_SPOTLIGHT:
      pLightList = aDynamicLightListSpot;
      uiLightListSizeMax = VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsSpot;
      break;

    default:
      VASSERT_MSG(false, "Unsupported light type.");
      return;
    }

    // Find light with lowest priority and overwrite if lower than current light.
    int iNewEntryIdx = -1;
    unsigned int uiLowestPriorityFound = UINT_MAX;
    for(unsigned int iDynamicLightListIdx = 0; iDynamicLightListIdx < uiLightListSizeMax; ++iDynamicLightListIdx)
    {
      if(pLightList[iDynamicLightListIdx].uiPriority < uiLowestPriorityFound)
      {
        uiLowestPriorityFound = pLightList[iDynamicLightListIdx].uiPriority;
        iNewEntryIdx = iDynamicLightListIdx;
        if(uiLowestPriorityFound == 0)
          break;
      }
    }
    if(iNewEntryIdx >= 0 && uiLowestPriorityFound < lightEntry.uiPriority)
      pLightList[iNewEntryIdx] = lightEntry;
  }


  // Update corresponding constants with new light lists.
  UpdateDynamicLightListConstants(aDynamicLightListDirectional, aDynamicLightListPoint, aDynamicLightListSpot);
  UpdateShadowMapSelection(aDynamicLightListDirectional[0].pLight);
}

void VisParticleEffect_cl::UpdateDynamicLightListConstants(DynamicLightListEntry aDynamicLightListDirectional[],
                                                       DynamicLightListEntry aDynamicLightListPoint[],
                                                       DynamicLightListEntry aDynamicLightListSpot[])
{
  VASSERT_MSG(m_pDynamicLightingShaderConstants != NULL, "Intern DynamicLightingShader Constant buffer of VisParticleEffect_cl is NULL!");

  // Reset light count
  m_pDynamicLightingShaderConstants[0].setZero();

  static const unsigned int constantOffsetDirectional = 6;
  static const unsigned int constantStrideDirectional = 2;
  static const unsigned int constantOffsetPoint = VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsDirectional * constantStrideDirectional + constantOffsetDirectional;
  static const unsigned int constantStridePoint = 2;
  static const unsigned int constantOffsetSpot = VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsPoint * constantStridePoint + constantOffsetPoint;
  static const unsigned int constantStrideSpot = 3;

  unsigned int actualLightCountDirectional = 0;
  unsigned int uiCurrentConstant = constantOffsetDirectional;
  for(unsigned int iDynamicLightListIdx = 0; iDynamicLightListIdx < VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsDirectional; ++iDynamicLightListIdx)
  {
    if(aDynamicLightListDirectional[iDynamicLightListIdx].pLight == NULL)
      break;
    VisLightSource_cl* pLight = aDynamicLightListDirectional[iDynamicLightListIdx].pLight;

    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetDirection().getAsVec4(0);
    ++uiCurrentConstant;
    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetColor().getAsVec4() * pLight->GetMultiplier();
    ++uiCurrentConstant;

    ++actualLightCountDirectional;
  }

  // Point lights
  int actualLightCountPoint = 0;
  uiCurrentConstant = constantOffsetPoint;
  for(unsigned int iDynamicLightListIdx = 0; iDynamicLightListIdx < VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsPoint; ++iDynamicLightListIdx)
  {
    if(aDynamicLightListPoint[iDynamicLightListIdx].pLight == NULL)
      break;
    VisLightSource_cl* pLight = aDynamicLightListPoint[iDynamicLightListIdx].pLight;

    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetPosition().getAsVec4(1.0f / pLight->GetRadius());
    ++uiCurrentConstant;
    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetColor().getAsVec4() * pLight->GetMultiplier();
    ++uiCurrentConstant;

    ++actualLightCountPoint;
  }

  // Spot lights
  int actualLightCountSpot = 0;
  uiCurrentConstant = constantOffsetSpot;
  for(unsigned int iDynamicLightListIdx = 0; iDynamicLightListIdx < VisParticleGroup_cl::s_uiMaxNumSupportedDynamicLightsSpot; ++iDynamicLightListIdx)
  {
    if(aDynamicLightListSpot[iDynamicLightListIdx].pLight == NULL)
      break;
    VisLightSource_cl* pLight = aDynamicLightListSpot[iDynamicLightListIdx].pLight;

    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetPosition().getAsVec4(1.0f / pLight->GetRadius());
    ++uiCurrentConstant;
    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetDirection().getAsVec4(hkvMath::cosDeg(pLight->GetProjectionAngle() * 0.5f));
    ++uiCurrentConstant;
    hkvVec4 lightColor;
    VColorRef::RGBA_To_Float(pLight->GetColor(), lightColor);
    m_pDynamicLightingShaderConstants[uiCurrentConstant] = pLight->GetColor().getAsVec4() * pLight->GetMultiplier();
    m_pDynamicLightingShaderConstants[uiCurrentConstant].w =  1.0f / (1.0f - hkvMath::cosDeg(pLight->GetProjectionAngle() * 0.5f));
    ++uiCurrentConstant;

    ++actualLightCountSpot;
  }

  // Store lightcounts into floats
  m_pDynamicLightingShaderConstants[0].x = static_cast<float>(actualLightCountDirectional); //*reinterpret_cast<float*>(&actualLightCountDirectional);
  m_pDynamicLightingShaderConstants[0].y = static_cast<float>(actualLightCountPoint); //*reinterpret_cast<float*>(&actualLightCountPoint);
  m_pDynamicLightingShaderConstants[0].z = static_cast<float>(actualLightCountSpot); //*reinterpret_cast<float*>(&actualLightCountSpot);
}

void VisParticleEffect_cl::UpdateShadowMapSelection(VisLightSource_cl* pDirectionalLightSource)
{
  VTextureObjectPtr spShadowMapBefore = m_spShadowReceiveShadowMap;
  m_spShadowReceiveShadowMap = NULL;

  if (pDirectionalLightSource != NULL && m_bNeedsShadowMapSelectionUpdate)
  {
    VBaseShadowMapComponentSpotDirectional* pShadowMapComponent = pDirectionalLightSource->Components().GetComponentOfBaseType<VBaseShadowMapComponentSpotDirectional>();
    if (pShadowMapComponent != NULL && pShadowMapComponent->IsEnabled())
    {
#if defined(_CPPRTTI)
      VASSERT_MSG(dynamic_cast<VShadowMapGenSpotDir*>(pShadowMapComponent->GetShadowMapGenerator()) != NULL,
        "Expected directional light's shadowmap generator to be of type VShadowMapGenSpotDir! Light object key: \"%s\"", pDirectionalLightSource->GetKey());
#endif
      VShadowMapGenSpotDir* pShadowMapGenerator = static_cast<VShadowMapGenSpotDir*>(pShadowMapComponent->GetShadowMapGenerator());

      hkvMat4 mLightProjectionTex[4];
      hkvVec4 vCascadeScale, vCascadeEnd;
      pShadowMapGenerator->ComputeCascadeParams(mLightProjectionTex, vCascadeScale, vCascadeEnd);
      m_spShadowReceiveShadowMap = pShadowMapGenerator->GetShadowMap();

      // Use nearest cascade by default.
      int iUsedCascade = 0;

      // Check if its frustum contains the particle bounding box and the area around it and choose higher cascade if necessary.
      // This basically resembles the code used in ShadowMapHelpers.inc shader.
      VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
      switch (pShadowMapComponent->GetCascadeSelection())
      {
      case VBaseShadowMapComponentSpotDirectional::CSM_SELECT_BY_INTERVAL:
      {
        VisFrustum_cl viewFrustum;
        pContext->GetViewFrustum(viewFrustum);
        float fDepth = -viewFrustum.GetNearPlane()->getDistanceTo(GetPosition());
        iUsedCascade = (fDepth > vCascadeEnd.x) + (fDepth > vCascadeEnd.y) + (fDepth > vCascadeEnd.z) + (fDepth > vCascadeEnd.w);
        break;
      }

      case VBaseShadowMapComponentSpotDirectional::CSM_SELECT_BY_INTERVAL_DEPTH:
      {
        float fDepth = GetPosition().getDistanceTo(pContext->GetCamera()->GetPosition());
        iUsedCascade = (fDepth > vCascadeEnd.x) + (fDepth > vCascadeEnd.y) + (fDepth > vCascadeEnd.z) + (fDepth > vCascadeEnd.w);
        break;
      }

      case VBaseShadowMapComponentSpotDirectional::CSM_SELECT_BY_BOUNDINGBOX:
      {
        const float fBorderBias = 1.05f;
        hkvVec4 scaleBias[3] =
        {
          hkvVec4(4, 4, -1, -1) * fBorderBias,
          hkvVec4(4, 4, -3, -1) * fBorderBias,
          hkvVec4(4, 4, -1, -3) * fBorderBias
        };
        if (pShadowMapGenerator->GetCascadeCount() <= 2)
          scaleBias[0].y = 2 * fBorderBias;

        for (; iUsedCascade < pShadowMapGenerator->GetCascadeCount() - 1; ++iUsedCascade)
        {
          // Transform from atlas coordinates so that valid x and y coordinates are in [-1,1] and valid z is < 1
          hkvVec3 vProjCoords = mLightProjectionTex[iUsedCascade].transformPosition(GetPosition());
          hkvVec3 vPositionInBBox = hkvVec3(vProjCoords.x * scaleBias[iUsedCascade].x + scaleBias[iUsedCascade].z,
            vProjCoords.y * scaleBias[iUsedCascade].y + scaleBias[iUsedCascade].w,
            vProjCoords.z);
          // Find the outermost coordinate
          vPositionInBBox.setAbs(vPositionInBBox);
          float fMaxCoord = hkvMath::Max(vPositionInBBox.x, vPositionInBBox.y, vPositionInBBox.z);

          if (fMaxCoord < 1.0f)
            break;
        }
      }
        break;
      }

      if (iUsedCascade >= pShadowMapGenerator->GetCascadeCount())
        iUsedCascade = pShadowMapGenerator->GetCascadeCount() - 1;

      // Setup light projection matrix.
      m_pDynamicLightingShaderConstants[1] = mLightProjectionTex[iUsedCascade].getColumn(0);
      m_pDynamicLightingShaderConstants[2] = mLightProjectionTex[iUsedCascade].getColumn(1);
      m_pDynamicLightingShaderConstants[3] = mLightProjectionTex[iUsedCascade].getColumn(2);
      m_pDynamicLightingShaderConstants[4] = mLightProjectionTex[iUsedCascade].getColumn(3);

      // Fade parameters
      float fFadeStart, fFadeEnd;
      pShadowMapGenerator->ComputeFadeOutParams(fFadeStart, fFadeEnd);
      m_pDynamicLightingShaderConstants[5].x = fFadeEnd;
      m_pDynamicLightingShaderConstants[5].y = 1.0f / (fFadeEnd - fFadeStart);
      // Inverse Shadowsize
      int iTextureWidth, iTextureHeight;
      pShadowMapGenerator->GetTextureSize(iTextureWidth, iTextureHeight);
      m_pDynamicLightingShaderConstants[5].z = 1.0f / iTextureWidth;
      m_pDynamicLightingShaderConstants[5].w = 1.0f / iTextureHeight;
    }
  }

  // Update all shader flags if the shadow map changed from NULL to something or backwards.
  if (spShadowMapBefore != m_spShadowReceiveShadowMap && (spShadowMapBefore == NULL || m_spShadowReceiveShadowMap == NULL))
  {
    for (unsigned int i = 0; i < m_iGroupCount; ++i)
    {
      if (m_spGroups[i] != NULL)
        m_spGroups[i]->UpdateDefaultShaderFlags();
    }
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void VisParticleEffect_cl::GetDependencies(VResourceSnapshot &snapshot)
{
  VisObject3D_cl::GetDependencies(snapshot);

  if (m_spSourceFXFile)
    m_spSourceFXFile->GetDependencies(snapshot);

  FOR_ALL_GROUPS
    pGroup->GetDependencies(snapshot);
  }
}
#endif


int VisParticleEffect_cl::GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList)
{
  int iCount = VisObject3D_cl::GetSynchronizationGroupList(context, pDestList);
  if (context.m_bSupportsInterpolation)
    pDestList[iCount++].Set(this, &VNetworkParticleEffectGroup::g_Instance);
  else
    pDestList[iCount++].Set(this, &VNetworkParticleEffectGroup::g_Instance);
  return iCount;
}

void VisParticleEffect_cl::OnReposition(const VisZoneRepositionInfo_t &info, const hkvVec3 &vLocalPos)
{
  // don't call base implementation here
  hkvVec3 vAbsPos(hkvNoInitialization);
  info.Helper_MakeAbsolute(vAbsPos, vLocalPos, GetParentZone());
  IncPosition(vAbsPos - GetPosition(), true);
}



////////////////////////////////////////////////////////////////////////////
// handle the particle effect instance collection
////////////////////////////////////////////////////////////////////////////

#define FOR_ALL_INSTANCES \
  int iCount = Count();\
  for (int i=0;i<iCount;i++)\
  {\
    VisParticleEffect_cl *pInstance = GetAt(i);\
    VASSERT(pInstance!=NULL);

#define FOR_ALL_INSTANCES_REVERSE \
  int iCount = Count();\
  for (int i=iCount-1;i>=0;i--)\
  {\
  VisParticleEffect_cl *pInstance = GetAt(i);\
  VASSERT(pInstance!=NULL);

void VisParticleEffectCollection_cl::Tick(float fTimeDelta, bool bPurgeDead)
{
  FOR_ALL_INSTANCES_REVERSE
    if (pInstance->IsUpdatedOnlyWhenVisible() && !pInstance->WasRecentlyRendered()) continue;
    pInstance->Tick(fTimeDelta);
    if (bPurgeDead && pInstance->IsDead())
    {
      pInstance->DisposeObject(); // this removes it from this collection
    }
  }
}

void VisParticleEffectCollection_cl::EvaluateSceneBrightness()
{
  FOR_ALL_INSTANCES
    pInstance->EvaluateSceneBrightness();
  }
}

void VisParticleEffectCollection_cl::Purge(bool bDeadOnly)
{
  FOR_ALL_INSTANCES_REVERSE
    if ((!bDeadOnly && pInstance->GetRefCount()==1) || pInstance->IsDead())
    {
      // Remember not to go through the Dispose function without removing the flag VObjectFlag_FinishPlaybackOnDispose.
      // In this case we want to remove the particle immediately.
      RemoveAt(i);
    }
  }
}

void VisParticleEffectCollection_cl::ReassignShader(bool bRecreateFX)
{
  FOR_ALL_INSTANCES
    const int iGroupCount = pInstance->GetParticleGroupCount();
    for(int group=0;group<iGroupCount;++group)
    {
      ParticleGroupBase_cl *pGroup = pInstance->GetParticleGroup(group);
      if (pGroup)
        pGroup->ReassignShader(bRecreateFX);
    }
  }
}

VisParticleEffect_cl *VisParticleEffectCollection_cl::FindByKey(const char *szKey, DynArray_cl<VisParticleEffect_cl *> *pStoreArray) const
{
  VisParticleEffect_cl *pFound = NULL;
  int iFound = 0;
  FOR_ALL_INSTANCES
    const char *szOtherKey = pInstance->GetEffectKey();
    if (szKey==szOtherKey || (szKey != NULL && szOtherKey != NULL && !_stricmp(szKey,szOtherKey))) // szKey can be NULL to find all without a key
    {
      if (!pStoreArray)
        return pInstance;
      if (!pFound) pFound = pInstance;
      (*pStoreArray)[iFound++] = pInstance;
    }
  }
  return pFound;
}



////////////////////////////////////////////////////////////////////////////////////
// class VNetworkParticleEffectGroupI
////////////////////////////////////////////////////////////////////////////////////

VNetworkParticleEffectGroupI VNetworkParticleEffectGroupI::g_Instance;

bool VNetworkParticleEffectGroupI::QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  VHistoryDataParticleEffect* pData = (VHistoryDataParticleEffect*) instanceInfo.m_pCustomData;
  
  BYTE iMask = 0;
  iMask |= pParticleEffect->IsPaused() * VCF_PAUSED;
  iMask |= pParticleEffect->IsHalted() * VCF_HALTED;

  BYTE iInterpolatedMask;
  pData->m_particleFlagHistory.Interpolate (&iInterpolatedMask, context.m_iCurrentServerTimeMS);
  // Every change in the flags is reliably transmitted
  if (iInterpolatedMask != iMask)
  {
    out_paketSettings.SetReliability (VMR_Reliable_Ordered);
    out_paketSettings.SetPriority (VMP_HighPriority);
    out_paketSettings.SetOrderingChannel (0);
    return true;
  }
  return false;
}

void VNetworkParticleEffectGroupI::Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  VHistoryDataParticleEffect* pData = (VHistoryDataParticleEffect*) instanceInfo.m_pCustomData;
  BYTE iMask = 0;

  if (ar.IsLoading())
  {
    __int64 iTimeMS;
    ar >> iTimeMS;
    ar >> iMask;
    pData->m_particleFlagHistory.Write (iMask, iTimeMS);
  }
  else
  {
    __int64 iTimeMS = context.m_iCurrentServerTimeMS;
    ar << iTimeMS;
    iMask |= pParticleEffect->IsPaused() * VCF_PAUSED;
    iMask |= pParticleEffect->IsHalted() * VCF_HALTED;
    ar << iMask;
    pData->m_particleFlagHistory.Write (iMask, iTimeMS);
  }
}

void VNetworkParticleEffectGroupI::TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta)
{
  // The server does not use the tick function, calling the base implementation will disable the tick function altogether.
  if (context.m_eNetType == VNT_Server)
  {
    IVNetworkSynchronizationGroup::TickFunction (context, instanceInfo, fTimeDelta);
    return;
  }

  VSystemGUID iOwnerID = instanceInfo.m_pComponent->GetNetworkOwnerID();
  // For clients and peers the group should only interpolate and set the values, if the owner is not set to themselves.
  if (iOwnerID != context.m_iUserID)
  {
    VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
    VHistoryDataParticleEffect* pData = (VHistoryDataParticleEffect*) instanceInfo.m_pCustomData;

    BYTE iMask = 0;
    VReadResult_e res = pData->m_particleFlagHistory.Interpolate (&iMask, context.m_iCurrentServerTimeMS - context.m_iInterpolationDelayMS);
    if (res != VRR_None)
    {
      pParticleEffect->SetPause  ((iMask & VCF_PAUSED) != 0);
      pParticleEffect->SetHalted ((iMask & VCF_HALTED) != 0);
    }
  }
}



////////////////////////////////////////////////////////////////////////////////////
// class VNetworkParticleEffectGroup
////////////////////////////////////////////////////////////////////////////////////

VNetworkParticleEffectGroup VNetworkParticleEffectGroup::g_Instance;

bool VNetworkParticleEffectGroup::QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  __int64 iNewHash = (__int64(1) << 32);
  if (pParticleEffect->IsPaused()) iNewHash |= VCF_PAUSED;
  if (pParticleEffect->IsHalted()) iNewHash |= VCF_HALTED;

  // same transformation as last frame?
  if (iNewHash==instanceInfo.m_iLastUpdateDataHash)
    return false;

  instanceInfo.m_iLastUpdateDataHash = iNewHash;
  return true;
}

void VNetworkParticleEffectGroup::Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
{
  VisParticleEffect_cl *pParticleEffect = (VisParticleEffect_cl *)instanceInfo.m_pInstance;
  BYTE iMask = 0;

  if (ar.IsLoading())
  {
    ar >> iMask;
    pParticleEffect->SetPause  ((iMask & VCF_PAUSED) != 0);
    pParticleEffect->SetHalted ((iMask & VCF_HALTED) != 0);
  }
  else
  {
    if (pParticleEffect->IsPaused()) iMask |= VCF_PAUSED;
    if (pParticleEffect->IsHalted()) iMask |= VCF_HALTED;
    ar << iMask;
  }
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
