/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceParticleGroup.hpp>
//#include "VisionView.h"
#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEmitter.hpp>

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif


namespace VisionManaged
{
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // single particle group
  ///////////////////////////////////////////////////////////////////////////////////////////////

  EngineInstanceParticleGroup::EngineInstanceParticleGroup()
  {
    m_iUpdateLockCtr = 0;
    m_fYaw = m_fPitch = m_fRoll = 0.f;
    m_fPosX = m_fPosY = m_fPosZ = 0.f;
    m_pGroup = nullptr;
    m_pFXFile = nullptr;
    m_iVisibleBitmask = 0xffffffff;
    m_bVisible = true;
    m_bRemoveWhenFinished = true;
    m_uiRandomBaseSeed = 0;
    m_pMeshEmitterEntity = NULL;
  }

  void EngineInstanceParticleGroup::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pGroup)
    V_SAFE_RELEASE(m_pFXFile);
    VisParticleGroupManager_cl::GlobalManager().PurgeUnusedResources();
  }

  void *EngineInstanceParticleGroup::GetObject3D()
  {
    VisObject3D_cl *pObj3D = (VisObject3D_cl *) m_pGroup;
    return pObj3D;
  }

  void EngineInstanceParticleGroup::RespawnAllParticles()
  {
    if (m_pGroup && m_iUpdateLockCtr==0)
    {
      m_pGroup->RespawnAllParticles(true);
      m_pGroup->UpdateVisibilityBoundingBox();
      m_pGroup->ReComputeVisibility();
      UpdateParticleColors();
    }
  }

  void EngineInstanceParticleGroup::UpdateParticleColors()
  {
    VLightGrid_cl *pLG = Vision::RenderLoopHelper.GetLightGrid();
    if (m_pGroup && m_iUpdateLockCtr==0)
    {
      for (int i=0;i<m_pGroup->GetParticleGroupCount();i++)
      {
        ParticleGroupBase_cl *pLayer = m_pGroup->GetParticleGroup(i);
        if (pLayer==nullptr || pLayer->GetDescriptor()->m_fApplySceneBrightness == 0.0f)
          continue;
        pLayer->EvaluateSceneBrightness();
        pLayer->SetPerFrameConstants(0.f);

        const int iCount = pLayer->GetNumOfParticles();
        ParticleExt_t *p = pLayer->GetParticlesExt();
        for (int j=0;j<iCount;j++,p++) 
          if (p->valid)
            pLayer->HandleSingleParticle(p,0.f);
      }
    }
  }

  void EngineInstanceParticleGroup::OnLinksChanged()
  {
    if (m_pGroup == nullptr)
      return;

    // Check whether the restart target component is connected to any source
    VisTriggerTargetComponent_cl* pTriggerTargetComponent = 
      m_pGroup->Components().GetComponentOfBaseTypeAndName<VisTriggerTargetComponent_cl>(PARTICLETRIGGER_RESTART);

    if (pTriggerTargetComponent == NULL)
    {
      m_pGroup->SetRemoveWhenFinished(m_bRemoveWhenFinished);
      return;
    }

    // If that is the case, it shouldn't be removed when it has finished, so override the setting.
    if (pTriggerTargetComponent->m_Sources.Count() > 0)
      m_pGroup->SetRemoveWhenFinished(false);
    else
      m_pGroup->SetRemoveWhenFinished(m_bRemoveWhenFinished); // Set actual value.
  }

  bool EngineInstanceParticleGroup::SetEffectFile(String ^effect)
  {
    VString sFilename;
    ConversionUtils::StringToVString(effect,sFilename);

    // check whether we have to load a new effect
    if (m_pFXFile!=nullptr && !sFilename.IsEmpty())
    {
      const char *szOldFilname = m_pFXFile->GetFilename();
      if (szOldFilname && !stricmp(szOldFilname,sFilename))
        return true;
    }

    // remember old parent
    VisObject3D_cl *pOldParent = ( m_pGroup ) ? m_pGroup->GetParent() : NULL;

    V_SAFE_DISPOSEANDRELEASE(m_pGroup);
    V_SAFE_RELEASE(m_pFXFile);
    m_pFXFile = VisParticleGroupManager_cl::GlobalManager().LoadFromFile(sFilename);
    if (!m_pFXFile)
      return false;
    m_pFXFile->AddRef();

    Vector3F vPos(m_fPosX,m_fPosY,m_fPosZ);
    Vector3F vOri(m_fYaw,m_fPitch,m_fRoll);
    m_pGroup = (VisParticleEffect_cl *)CreateParticleEffectInstance((void*)m_pFXFile,vPos,vOri,m_uiRandomBaseSeed);
    if (!m_pGroup)
      return false;
    m_pGroup->AddRef();

    // restore parent
    if ( pOldParent )
      m_pGroup->AttachToParent(pOldParent);
    
    // add trigger target components - only inside vForge. They are only serialized when used.
    VisTriggerTargetComponent_cl *pComp;

    // pause target
    pComp = new VisTriggerTargetComponent_cl(PARTICLETRIGGER_PAUSE, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pGroup->AddComponent(pComp);

    // unpause target
    pComp = new VisTriggerTargetComponent_cl(PARTICLETRIGGER_RESUME, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pGroup->AddComponent(pComp);

    // restart target
    pComp = new VisTriggerTargetComponent_cl(PARTICLETRIGGER_RESTART, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pGroup->AddComponent(pComp);

    return true;
  }

  void EngineInstanceParticleGroup::SetAmbientColor(unsigned int iColor)
  {
    VColorRef color;
    color.SetRGBA(iColor);
    if (m_pGroup)
      m_pGroup->SetAmbientColor(color);
    UpdateParticleColors();
  }

  void EngineInstanceParticleGroup::SetPosition(float x,float y,float z)
  {
    hkvVec3 vPos(x,y,z);
    m_fPosX = x;
    m_fPosY = y;
    m_fPosZ = z;

    if ( !m_pGroup )
      return;

    hkvVec3 vOldPos = m_pGroup->GetPosition();
    m_pGroup->IncPosition(vPos-vOldPos); // move all particles
    m_pGroup->UpdateVisibilityBoundingBox();
    m_pGroup->ReComputeVisibility(); // new center of the bounding box
  }

  void EngineInstanceParticleGroup::SetOrientation(float yaw,float pitch,float roll)
  {
    m_fYaw = yaw;
    m_fPitch = pitch;
    m_fRoll = roll;
    if ( !m_pGroup )
      return;

    IEngineInstanceObject3D::SetOrientation(yaw, pitch, roll);
    if (IsHalted())
    {
      m_pGroup->RespawnAllParticles(true);
      m_pGroup->UpdateVisibilityBoundingBox();
    }
  }

  void EngineInstanceParticleGroup::SetScaling(float x,float y, float z)
  {
    if (!m_pGroup) return;
    m_pGroup->SetScaling((x+y+z)/3.f);
    if (IsHalted())
    {
      m_pGroup->RespawnAllParticles(true);
      m_pGroup->UpdateVisibilityBoundingBox();
    }
  }

  void EngineInstanceParticleGroup::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (!m_pGroup)
      return;
    if (mode==ShapeRenderMode::Selected)
    {
      IVRenderInterface *pRI = (static_cast<VisionView ^>(view))->GetRenderInterface();
      
      // render every particle layer as a preview
      for (int i=0;i<m_pGroup->GetParticleGroupCount();i++)
      {
        ParticleGroupBase_cl *pGroup = m_pGroup->GetParticleGroup(i);

        if( !pGroup )
          continue;
        
        switch (g_CurrentPreviewType)
        {
          case ParticlePreviewType_e::LayerProperties:      RenderPreview_LayerProperties(i, view, pRI, pGroup);break;
          case ParticlePreviewType_e::ParticleProperties:   RenderPreview_ParticleProperties(i, view, pRI, pGroup);break;
          case ParticlePreviewType_e::EmitterProperties:    RenderPreview_EmitterProperties(i, view, pRI, pGroup);break;
          case ParticlePreviewType_e::VisibilityProperties: RenderPreview_VisibilityProperties(i, view, pRI, pGroup);break;
          case ParticlePreviewType_e::PhysicsProperties:    RenderPreview_PhysicsProperties(i, view, pRI, pGroup);break;
          default: break;
        }

      }
    }
  }

  void EngineInstanceParticleGroup::RenderPreview_LayerProperties(int iLayer, VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup)
  {
  }

  void EngineInstanceParticleGroup::RenderPreview_ParticleProperties(int iLayer, VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup)
  {
  }

  void EngineInstanceParticleGroup::RenderPreview_EmitterProperties(int iLayer, VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup)
  {
    if( iLayer!=g_CurrentPreviewLayer )
      return;

    VisParticleEmitter_cl *pEmitter =  pGroup->GetEmitter();
    VIS_EMITTER_TYPE_e eType = pEmitter->GetType();
    VisParticleEffect_cl *pParentEffect = pGroup->GetParentEffect();


    hkvMat3 mRelRotMat;
    hkvVec3 relativeOrientation = pGroup->GetDescriptor()->m_vRelativeOrientation;
    mRelRotMat.setFromEulerAngles(relativeOrientation.z, relativeOrientation.y, relativeOrientation.x);
    hkvMat3 mShapeRotMat = (pParentEffect != NULL) ? pParentEffect->GetRotationMatrix() : m_pGroup->GetRotationMatrix();
    hkvMat3 mRotMat = mShapeRotMat * mRelRotMat;

    hkvVec3 vParam = pEmitter->m_vParam.getAsVec3();
    vParam *= pGroup->GetScaling(); // scale the emitter extent

    float fConeAngle = pEmitter->m_fConeAngle * 2;

    hkvVec3 vPos(m_fPosX, m_fPosY, m_fPosZ);
    vPos += mShapeRotMat * pGroup->GetDescriptor()->m_vRelativePosition;

    hkvVec3 vDir = mRotMat.getColumn(0);

    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
    const VColorRef coneColor(255, 150, 0, 50);
    const VColorRef shapeColor(255, 255, 0, 50);
    const float fConeLength = 100.0f;

    switch (eType)
    {
      case EMITTER_TYPE_UNKNOWN: break;
      case EMITTER_TYPE_POINT:
        pRI->RenderSphere(vPos, 1, shapeColor, state);
        if (fConeAngle >= 0.f)
          pRI->RenderCone(vPos, vDir, fConeAngle, fConeLength, coneColor, state);
        
        break; 
      case EMITTER_TYPE_SPHERE: 
        pRI->RenderSphere(vPos, vParam.x, shapeColor, state);
        
        if (fConeAngle >= 0.f)
        {
          const hkvVec3 dirs[6] = { hkvVec3(0, 0, 1), hkvVec3(0, 0, -1), hkvVec3(1, 0, 0),
                                    hkvVec3(-1, 0, 0), hkvVec3(0, 1, 0), hkvVec3(0, -1, 0) };
          if(pEmitter->m_bEmitFromSurface)
          {
            for (int i = 0; i < 6; ++i)
            {
              hkvVec3 up = mRotMat * dirs[i];
              pRI->RenderCone(vPos + up * vParam.x, up, fConeAngle, fConeLength, coneColor, state);
            }
          }
          else
          {
            for (int i = 0; i < 6; ++i)
              pRI->RenderCone(vPos + dirs[i] * vParam.x, vDir, fConeAngle, fConeLength, coneColor, state);
            pRI->RenderCone(vPos, vDir, fConeAngle, 100.f, coneColor, state);
          }
        }

        break;
      case EMITTER_TYPE_BOX:
      {
        hkvVec3 corners[8];
        hkvAlignedBBox localBox(-vParam,vParam);

        localBox.getCorners (corners);
        const hkvMat4 mTransformation (mRotMat,vPos);
        mTransformation.transformPositions (corners, 8);

        pRI->RenderBox(corners, sizeof(hkvVec3), shapeColor, state);

        if (fConeAngle>=0.f)
        {
          if(pEmitter->m_bEmitFromSurface)
          {
            const hkvVec3 directions[6] = { hkvVec3(0, 0, 1), hkvVec3(0, 0, -1), hkvVec3(1, 0, 0), hkvVec3(-1, 0, 0), hkvVec3(0, -1, 0), hkvVec3(0, 1, 0) };
            for (int i = 0; i < 6; ++i)
            {
              pRI->RenderCone(vPos + mRotMat * directions[i].compMul(vParam), mRotMat.transformDirection(directions[i]), fConeAngle, fConeLength, coneColor, state);
            }
          }
          else
          {
            for (int i = 0; i < 8; i++)
              pRI->RenderCone(corners[i], vDir, fConeAngle, 100.f, coneColor, state);
            pRI->RenderCone(vPos, vDir, fConeAngle, 100.f, coneColor, state);
          }
        }
        break;
      }
      case EMITTER_TYPE_PLANE:
      {
        hkvVec3 vRight = mRotMat.getAxis(1);
        hkvVec3 vUp = mRotMat.getAxis(2);
        vRight.setLength(vParam.x);
        vUp.setLength(vParam.y);
        pRI->RenderPlane(vRight, vUp, vPos, shapeColor, state);

        if (fConeAngle>=0.f)
        {
          pRI->RenderCone(vPos, vDir, fConeAngle, 100.f, coneColor, state);

          const hkvVec3 corners[4] = { hkvVec3(0, vParam.x, vParam.y), hkvVec3(0, vParam.x, -vParam.y),
                                      hkvVec3(0, -vParam.x, vParam.y), hkvVec3(0, -vParam.x, -vParam.y) };
          for (int i = 0; i < 4; ++i)
            pRI->RenderCone(vPos + mRotMat * (corners[i] * 0.7f), vDir, fConeAngle, fConeLength, coneColor, state);
        }
        break;
      }
      case EMITTER_TYPE_RAY:
        pRI->RenderCylinder(vPos, vDir*vParam.x, 10.0f, shapeColor, state, RENDERSHAPEFLAGS_LINES | RENDERSHAPEFLAGS_SOLID);

        if (fConeAngle>=0.f)
        {
          if(pEmitter->m_bEmitFromSurface)
          {
            const hkvVec3 normals[8] = { hkvVec3(0, 0, 1), hkvVec3(0, 0, -1), hkvVec3(0,  1, 0), hkvVec3(0, -1,  0),
                                         hkvVec3(0, 1, 1), hkvVec3(0, 1, -1), hkvVec3(0, -1, 1), hkvVec3(0, -1, -1) };

            // we reduce the number of cones when overlapping
            const int numOfCones = fConeAngle>22.5f ? (fConeAngle>45 ? 2 : 4) : 8;
            for(int i=0;i<numOfCones;i++)
            {
              hkvVec3 normal = mRotMat.transformDirection(normals[i]);
              pRI->RenderCone(vPos, normal, fConeAngle, 100.f, coneColor, state);
              pRI->RenderCone(vPos + vDir*vParam.x, normal, fConeAngle, fConeLength, coneColor, state);
              pRI->RenderCone(vPos + vDir*vParam.x*0.5f, normal, fConeAngle, fConeLength, coneColor, state);
            }
          }
          else
          {
            pRI->RenderCone(vPos, vDir, fConeAngle, 100.f, coneColor, state);
            pRI->RenderCone(vPos + vDir*vParam.x, vDir, fConeAngle, fConeLength, coneColor, state);
            pRI->RenderCone(vPos + vDir*vParam.x*0.5f, vDir, fConeAngle, fConeLength, coneColor, state);
          }
        }
        break;

      case EMITTER_TYPE_MESH:
        if(m_pMeshEmitterEntity!=NULL)
        {
          m_pMeshEmitterEntity->DebugRenderMesh(shapeColor);
          hkvAlignedBBox bbox;
          m_pMeshEmitterEntity->GetCollisionBoundingBox(bbox, true);
          vPos = bbox.getCenter();
          vDir = m_pMeshEmitterEntity->GetDirection();
        }
        if (fConeAngle >= 0.f)
        {
          pRI->RenderCone(vPos, vDir, fConeAngle, fConeLength, coneColor, state);
        }
        break;
      
      default:
        break;
    }
  }

  void EngineInstanceParticleGroup::RenderPreview_VisibilityProperties(int iLayer, VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup)
  {
    VisParticleGroupDescriptor_cl *pDesc = pGroup->GetDescriptor();
    VisParticleEffect_cl * pParentEffect = pGroup->GetParentEffect(); //could be null

    hkvMat3 rotMat = (pParentEffect!=NULL) ? pParentEffect->GetRotationMatrix() : pGroup->GetRotationMatrix();

    hkvAlignedBBox orientedBBox(pDesc->m_BoundingBox.m_vMin, pDesc->m_BoundingBox.m_vMax);

    hkvMat4 transform(rotMat, (pParentEffect!=NULL) ? pParentEffect->GetPosition() :  pGroup->GetPosition() );
    transform.setScalingFactors( hkvVec3( pGroup->GetScaling() ) );
    orientedBBox.transformFromOrigin( transform );

    hkvAlignedBBox bBox( hkvNoInitialization );
    bBox.setInvalid();
    bBox.expandToInclude( orientedBBox );

    pRI->DrawLineBox(bBox, (iLayer == g_CurrentPreviewLayer) ? V_RGBA_RED : VColorRef(255, 150, 0, 50), 1.f);
  }

  void EngineInstanceParticleGroup::RenderPreview_PhysicsProperties(int iLayer, VisionViewBase ^view, IVRenderInterface *pRI, ParticleGroupBase_cl *pGroup)
  {
    if( iLayer!=g_CurrentPreviewLayer )
      return;

    //render the gravity vector
    hkvVec3 vGravity = pGroup->GetDescriptor()->m_vGravity;
    hkvVec3 vPos(m_fPosX,m_fPosY,m_fPosZ);
    
    pRI->DrawLine(vPos, vPos+hkvVec3(vGravity.x,0,0), V_RGBA_RED, 1.f);
    pRI->DrawLine(vPos+hkvVec3(vGravity.x,0,0), vPos+hkvVec3(vGravity.x,vGravity.y,0), V_RGBA_GREEN, 1.f);
    pRI->DrawLine(vPos+vGravity, vPos+hkvVec3(vGravity.x,vGravity.y,0), V_RGBA_BLUE, 1.f);
    pRI->DrawLine(vPos, vPos+vGravity, V_RGBA_PURPLE, 1.f);
    VString sGravity;
    sGravity.Format("Gravity %2.2f", vGravity.getLength());
    pRI->DrawText3D(vPos+vGravity+hkvVec3(15,15,15), sGravity.AsChar(), V_RGBA_PURPLE);
  }

  void* EngineInstanceParticleGroup::CreateParticleEffectInstance(void* pFXFile, Vector3F pos, Vector3F ori, unsigned int uiRandomSeed)
  {
    if (pFXFile==NULL)
      return NULL;
    
    hkvVec3 vPos(pos.X, pos.Y, pos.Z);
    hkvVec3 vOri(ori.X, ori.Y, ori.Z);
    return ((VisParticleEffectFile_cl*)pFXFile)->CreateParticleEffectInstance(vPos,vOri,uiRandomSeed);
  }

  void EngineInstanceParticleGroup::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vDir(rayEnd.X-rayStart.X,rayEnd.Y-rayStart.Y,rayEnd.Z-rayStart.Z);
  
    hkvAlignedBBox bbox;
    if (!GetLocalBoundingBox(bbox))
      return;
    
    float fT;
    hkvVec3 vTouch;

    if (!bbox.getRayIntersection (vStart,vDir, &fT, &vTouch))
      return;

    fT *= vDir.getLength();
    pResult->SetShapeHit_IfCloser(ownerShape,fT);
  }

    
  bool EngineInstanceParticleGroup::GetLocalBoundingBox(hkvAlignedBBox &bbox)
  {
    if (!m_pGroup)
      return false;

    hkvAlignedBBox mergedBox;
    mergedBox.setInvalid();

    // merge the initial visibility bounding boxes of all particle layers
    for (int i=0;i<m_pGroup->GetParticleGroupCount();i++)
    {
      ParticleGroupBase_cl *pGroup = m_pGroup->GetParticleGroup(i);
      if (!pGroup) continue;
      mergedBox.expandToInclude(pGroup->GetDescriptor()->m_BoundingBox);
    }
    if (mergedBox.isValid())
      bbox = mergedBox;
    else
    {
      float fSize = 30.f;
      bbox.set(hkvVec3 (-fSize), hkvVec3 (fSize));
    }
    return true;
  }

  bool EngineInstanceParticleGroup::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    hkvAlignedBBox box;
    if (!GetLocalBoundingBox(box))
      return false;
    (*bbox).Set(box.m_vMin.x,box.m_vMin.y,box.m_vMin.z, box.m_vMax.x,box.m_vMax.y,box.m_vMax.z);
    return true;
  }

  void EngineInstanceParticleGroup::AddConstraint(EngineInstanceConstraint ^pConstraint)
  {
    if ( !m_pGroup )
      return;

    m_pGroup->RemoveConstraint(pConstraint->GetConstraint());
    m_pGroup->AddConstraint(pConstraint->GetConstraint(),false);
  }

  void EngineInstanceParticleGroup::RemoveConstraint(EngineInstanceConstraint ^pConstraint)
  {
    if ( !m_pGroup )
      return;

    m_pGroup->RemoveConstraint(pConstraint->GetConstraint());
    if (IsHalted()) m_pGroup->RespawnAllParticles(true);
  }

  void EngineInstanceParticleGroup::RemoveAllConstraints()
  {
    if ( !m_pGroup )
      return;

    m_pGroup->RemoveAllConstraints();
    if (IsHalted()) m_pGroup->RespawnAllParticles(true);
  }

  bool EngineInstanceParticleGroup::OnExport(SceneExportInfo ^info)
  {
    if ( !m_pGroup )
      return true;

    Debug::Assert( m_pGroup != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pGroup);
    return true;
  }


  void EngineInstanceParticleGroup::SetWindSpeed(Vector3F wind, bool bLocalSpace)
  {
    if ( !m_pGroup )
      return;
    m_pGroup->SetWindSpeed(hkvVec3(wind.X,wind.Y,wind.Z), bLocalSpace);		
  }

  void EngineInstanceParticleGroup::SetRemoveWhenFinished(bool value)
  {
    if ( !m_pGroup )
      return;

    m_bRemoveWhenFinished = value;

    OnLinksChanged(); // Decide if we need to override the value.		
  }

  void EngineInstanceParticleGroup::Restart()
  {
    if ( !m_pGroup )
      return;
    m_pGroup->Restart();
  }


  void EngineInstanceParticleGroup::SetMeshEmitterEntity(EngineInstanceEntity ^entityInst)
  {
    if ( !m_pGroup )
      return;
    VisBaseEntity_cl *pEntity = NULL;
    if (entityInst!=nullptr)
      pEntity = entityInst->GetEntity();
    m_pGroup->SetMeshEmitterEntity(pEntity);

    if (m_pMeshEmitterEntity!=pEntity)
    {
      m_pMeshEmitterEntity = pEntity;
      if (pEntity!=NULL && pEntity->GetAnimConfig())
        pEntity->GetAnimConfig()->GetCurrentVertexResult(); // enforce update of the collision mesh
      m_pGroup->RespawnAllParticles(true);
    }
  }

  void EngineInstanceParticleGroup::SetPreferredDirLightKey(String ^dirLightKey)
  {
    if (!m_pGroup)
      return;

    VString sDirLightKeyVString;
    ConversionUtils::StringToVString(dirLightKey, sDirLightKeyVString);
    m_pGroup->SetPreferredDirLightKey(sDirLightKeyVString);
  }

  void EngineInstanceParticleGroup::SetLightSamplingOffset(Vector3F lightSamplingOffset)
  {
    if (m_pGroup)
      m_pGroup->SetLightSamplingOffset(hkvVec3(lightSamplingOffset.X, lightSamplingOffset.Y, lightSamplingOffset.Z));
  }

  void EngineInstanceParticleGroup::SetRandomBaseSeed(unsigned int uiBaseSeed)
  { 
    m_uiRandomBaseSeed = uiBaseSeed; 
    if (m_pGroup) 
    {
      m_pGroup->SetRandomBaseSeed(uiBaseSeed); 
      m_pGroup->Restart();
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Constraint
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////

  EngineInstanceConstraint::EngineInstanceConstraint()
  {
    m_pConstraint = nullptr;

    m_bInfinite = true;
    m_fPosX = m_fPosY = m_fPosZ = 0.f;
    m_fYaw = m_fPitch = m_fRoll = 0.f;
    m_fBoxX = m_fBoxY = m_fBoxZ = 0.f;
  }

  void EngineInstanceConstraint::DisposeObject()
  {
    if (m_pConstraint) 
      m_pConstraint->FlagForRemoval();
    V_SAFE_DISPOSEANDRELEASE(m_pConstraint);
  }

  void *EngineInstanceConstraint::GetObject3D()
  {
    VisObject3D_cl *pObj3D = (VisObject3D_cl *) m_pConstraint;
    return pObj3D;
  }

  void EngineInstanceConstraint::SetPosition(float x,float y,float z)
  {
    m_fPosX = x;
    m_fPosY = y;
    m_fPosZ = z;

    IEngineInstanceObject3D::SetPosition(x, y, z);
  }


  void EngineInstanceConstraint::SetOrientation(float yaw,float pitch,float roll)
  {
    m_fYaw = yaw;
    m_fPitch = pitch;
    m_fRoll = roll;

    IEngineInstanceObject3D::SetOrientation(yaw, pitch, roll);
  }

    
  void EngineInstanceConstraint::SetAffectBitmask(unsigned int iMask)
  {
    if (m_pConstraint)
      m_pConstraint->m_iAffectBitMask = iMask;
  }

  void EngineInstanceConstraint::SetRadius(float fRadius)
  {
    if (m_eType==ConstraintType_e::Sphere) ((VisParticleConstraintSphere_cl *)m_pConstraint)->SetRadius(fRadius);
    if (m_eType==ConstraintType_e::Point) ((VisParticleConstraintPoint_cl *)m_pConstraint)->SetRadius(fRadius);
    if (m_eType==ConstraintType_e::XAxis||m_eType==ConstraintType_e::YAxis||m_eType==ConstraintType_e::ZAxis)
      ((VisParticleConstraintInfCylinder_cl *)m_pConstraint)->SetRadius(fRadius);
    if (m_eType==ConstraintType_e::Cyclone) ((VisParticleAffectorCyclone_cl *)m_pConstraint)->SetRadius(fRadius);
    if (m_eType==ConstraintType_e::GravityPoint) ((VisParticleAffectorGravityPoint_cl *)m_pConstraint)->SetRadius(fRadius);
  }

  void EngineInstanceConstraint::SetInside(bool bStatus)
  {
    if (m_eType==ConstraintType_e::Sphere) ((VisParticleConstraintSphere_cl *)m_pConstraint)->SetInside(bStatus);
    if (m_eType==ConstraintType_e::XAxis||m_eType==ConstraintType_e::YAxis||m_eType==ConstraintType_e::ZAxis)
      ((VisParticleConstraintInfCylinder_cl *)m_pConstraint)->SetInside(bStatus);
    if (m_eType==ConstraintType_e::AABox) ((VisParticleConstraintAABox_cl *)m_pConstraint)->SetInside(bStatus);

  }

  void EngineInstanceConstraint::SetInfiniteExtent(bool bStatus)
  {
    m_bInfinite = bStatus;
    if (m_eType==ConstraintType_e::Plane) ((VisParticleConstraintPlane_cl *)m_pConstraint)->SetPlaneExtent(m_bInfinite,m_fBoxX,m_fBoxY,m_fBoxZ);
  }


  void EngineInstanceConstraint::SetBoxSize(float x,float y,float z)
  {
    m_fBoxX = x;
    m_fBoxY = y;
    m_fBoxZ = z;
    if (m_eType==ConstraintType_e::AABox) ((VisParticleConstraintAABox_cl *)m_pConstraint)->SetBoxSize(x,y,z);
    if (m_eType==ConstraintType_e::Plane) ((VisParticleConstraintPlane_cl *)m_pConstraint)->SetPlaneExtent(m_bInfinite,m_fBoxX,m_fBoxY,m_fBoxZ);
    if (m_eType==ConstraintType_e::CameraBox) ((VisParticleConstraintCamBox_cl *)m_pConstraint)->SetBoxRadius(hkvVec3(x*0.5f,y*0.5f,z*0.5f));
}

  void EngineInstanceConstraint::SetIntensity(float fIntensity)
  {
    if (m_eType==ConstraintType_e::Fan) ((VisParticleAffectorFan_cl *)m_pConstraint)->SetIntensity(fIntensity);
    if (m_eType==ConstraintType_e::Cyclone) ((VisParticleAffectorCyclone_cl *)m_pConstraint)->SetIntensity(fIntensity);
    if (m_eType==ConstraintType_e::GravityPoint) ((VisParticleAffectorGravityPoint_cl *)m_pConstraint)->SetIntensity(fIntensity);
  }

  void EngineInstanceConstraint::SetConeAngle(float fAngle)
  {
    if (m_eType==ConstraintType_e::Fan) ((VisParticleAffectorFan_cl *)m_pConstraint)->SetConeAngle(fAngle);
  }

  void EngineInstanceConstraint::SetLength(float fLength)
  {
    if (m_eType==ConstraintType_e::Cyclone) ((VisParticleAffectorCyclone_cl *)m_pConstraint)->SetLength(fLength);
  }

  void EngineInstanceConstraint::SetRandomness(float fVariation)
  {
    if (m_eType==ConstraintType_e::Fan) ((VisParticleAffectorFan_cl *)m_pConstraint)->SetRandomness(fVariation);
  }

  void EngineInstanceConstraint::SetReflectionNoise(float fNoise)
  {
    m_pConstraint->SetReflectionNoise(fNoise);
  }


  void EngineInstanceConstraint::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  }

  bool EngineInstanceConstraint::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    float fSize = 30.f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

  void EngineInstanceConstraint::HighlightShape(VisionViewBase ^view)
  {
    if (!m_pConstraint) return;
    IVRenderInterface *pRI = (static_cast<VisionView ^>(view))->GetRenderInterface();
    m_pConstraint->DebugRender(pRI, hkvVec3(m_fPosX,m_fPosY,m_fPosZ));
  }


  void EngineInstanceConstraint::SetType(ConstraintType_e eType)
  {
    if (m_pConstraint)
    {
      if (m_eType == eType) return;
      m_pConstraint->FlagForRemoval();
    }
    V_SAFE_DISPOSEANDRELEASE(m_pConstraint);

    hkvAlignedBBox bbox (hkvVec3 (-50,-50,-30), hkvVec3 (50,50,30));
    m_eType = eType;

    switch (eType)
    {
      case ConstraintType_e::GroundPlane:
        m_pConstraint = new VisParticleConstraintGroundPlane_cl(0.f);break;
      case ConstraintType_e::Plane:
        m_pConstraint = new VisParticleConstraintPlane_cl();break;
      case ConstraintType_e::Point:
        m_pConstraint = new VisParticleConstraintPoint_cl(hkvVec3(0,0,0),4.f);break;
      case ConstraintType_e::Sphere:
        m_pConstraint = new VisParticleConstraintSphere_cl(hkvVec3(0,0,0),100.f,false);break;
      case ConstraintType_e::XAxis:
        m_pConstraint = new VisParticleConstraintInfCylinder_cl(hkvVec3(0,0,0),40.f,false,AXIS_X); break;
      case ConstraintType_e::YAxis:
        m_pConstraint = new VisParticleConstraintInfCylinder_cl(hkvVec3(0,0,0),40.f,false,AXIS_Y); break;
      case ConstraintType_e::ZAxis:
        m_pConstraint = new VisParticleConstraintInfCylinder_cl(hkvVec3(0,0,0),40.f,false,AXIS_Z); break;
      case ConstraintType_e::AABox:
        m_pConstraint = new VisParticleConstraintAABox_cl(bbox,false);break;
      case ConstraintType_e::Fan:
        m_pConstraint = new VisParticleAffectorFan_cl(hkvVec3(0,0,0),hkvVec3(1,0,0),45.f,600.f);break;
      case ConstraintType_e::Cyclone:
        m_pConstraint = new VisParticleAffectorCyclone_cl(hkvVec3(0,0,0),hkvVec3(1,0,0),45.f,600.f);break;
      case ConstraintType_e::GravityPoint:
        m_pConstraint = new VisParticleAffectorGravityPoint_cl(hkvVec3(0,0,0),30.f,600.f);break;
      case ConstraintType_e::Terrain:
        m_pConstraint = new VisParticleConstraintTerrain_cl();break;
      case ConstraintType_e::CameraBox:
        m_pConstraint = new VisParticleConstraintCamBox_cl(hkvVec3(m_fBoxX*0.5f,m_fBoxY*0.5f,m_fBoxZ*0.5f));break;
      default:break;
    }    
    if (!m_pConstraint)
      return;
    m_pConstraint->AddRef();
    //m_pConstraint->SetDebugColor(VColorRef(100,180,255,100));
  }

  void EngineInstanceConstraint::SetReflection(ConstraintReflectionType_e eMode, float fPersistance)
  {
    if (!m_pConstraint) return;
    m_pConstraint->SetReflection((VIS_CONSTRAINT_REFLECT_BEHAVIOR_e)eMode,fPersistance);
  }
  
  bool EngineInstanceConstraint::OnExport(SceneExportInfo ^info)
  {
    Debug::Assert( m_pConstraint != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pConstraint);
    return true;
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
