/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceDecorationGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VGroupInstance.hpp>

using System::Diagnostics::Debug;

#using <mscorlib.dll>

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

#define FOREACH_GROUP \
  if (m_pGroups)\
    for (int i=0;i<m_pGroups->Count();i++)\
    {\
      VTerrainDecorationGroup *pGroup = m_pGroups->GetAt(i);


namespace VisionManaged
{
  EngineInstanceDecorationGroup::EngineInstanceDecorationGroup() 
  {
    _x = _y = _z = _yaw = _pitch = _roll = 0.f;
    _lgx = _lgy = _lgz = 0.f;
    m_bVisible = true;
    m_bUseLightgrid = true;
    m_bCastDynamicShadows = true;
    m_iVisibleBitMask = 0xffffffff;
    m_iAmbientColor = 0;
    m_fFarClip = 0.f;
    m_fCollisionCapsuleRadius = 0.f;

    m_eCollisionType = DecorationCollisionType_e::None;
    m_bPerInstanceVisibility = true;

    m_fScaling = 1.f;
    m_pInstancesSortedByModel = nullptr;
    m_pGroups = new VTerrainDecorationGroupCollection();
  }

  void EngineInstanceDecorationGroup::DisposeObject() 
  {
    if (m_pGroups)
    {
      m_pGroups->DisposeObjects();
      delete m_pGroups;
      m_pGroups = NULL;
    }
  }

  void EngineInstanceDecorationGroup::SetUniqueID(unsigned __int64 iID)
  {
    FOREACH_GROUP
      pGroup->SetUniqueID(iID);
    }
  }

  void EngineInstanceDecorationGroup::SetPosition(float x,float y,float z)
  {
    _x=x; _y=y; _z=z;
    UpdateLightgridColors();
  }

  void EngineInstanceDecorationGroup::SetOrientation(float yaw,float pitch,float roll)
  {
    _yaw = yaw; _pitch=pitch; _roll=roll;
  }

  void EngineInstanceDecorationGroup::SetScaling(float x,float y, float z) 
  {
    m_fScaling = (x+y+z)*(1.f/3.f); // only uniform scaling
    //m_fScaling = 1.f; // !=1 causes problems with the local boundingbox of the group
  }

  void EngineInstanceDecorationGroup::UpdateLightgridColors()
  {
    VColorRef iAmbColor;
    iAmbColor.SetRGBA(m_iAmbientColor);

    hkvVec3 vPos(_x+_lgx,_y+_lgy,_z+_lgz);
    FOREACH_GROUP
      pGroup->SetUseLightgrid(m_bUseLightgrid);
      pGroup->SetLightgridSamplePosition(vPos);
      pGroup->SetAmbientColor(iAmbColor);
      pGroup->UpdateLightgridColors();
    }
  }


  void EngineInstanceDecorationGroup::SetCollisionType(DecorationCollisionType_e eType, float fCapsuleRadius)
  {
    if (m_fCollisionCapsuleRadius==fCapsuleRadius && m_eCollisionType==eType)
      return;
    m_eCollisionType = eType;
    m_fCollisionCapsuleRadius = fCapsuleRadius;
    UpdateInstances();
  }


  void EngineInstanceDecorationGroup::SetCastLightmapShadows(bool bStatus)
  {
    FOREACH_GROUP
      pGroup->m_bCastLightmapShadows = bStatus;
    }
  }

  void EngineInstanceDecorationGroup::SetCastDynamicShadows(bool bStatus)
  {
    m_bCastDynamicShadows = bStatus;
    UpdateVisibleStatus();
  }


  void EngineInstanceDecorationGroup::GetDependencies(ResourceSnapshotInfo ^info)
  {
    VResourceSnapshot *snapshot = (VResourceSnapshot *)info->NativeResourceSnapshotPtr.ToPointer();
    FOREACH_GROUP
      if (pGroup->m_Instances.m_iCount>0)
        pGroup->GetDependencies(*snapshot);
    }
  }
    
  void EngineInstanceDecorationGroup::OnPostEngineInstanceCreation() 
  {
    FOREACH_GROUP
      ConversionUtils::CallOnDeserializationFunction(pGroup);
    }
  }

  void EngineInstanceDecorationGroup::OnBeforeExport(SceneExportInfo ^info)
  {
    FOREACH_GROUP
      pGroup->SetMarkInsideSerializationSession(true);
    }
  }
  void EngineInstanceDecorationGroup::OnAfterExport(SceneExportInfo ^info)
  {
    FOREACH_GROUP
      pGroup->SetMarkInsideSerializationSession(false);
    }
  }


  bool EngineInstanceDecorationGroup::OnExport(SceneExportInfo ^info) 
  {
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    FOREACH_GROUP
      if (pGroup->m_Instances.m_iCount>0)
        ar << pGroup;
    }
    return true;
  }


  void EngineInstanceDecorationGroup::UpdateVisibleStatus()
  {
    unsigned int iMask = (m_bVisible) ? m_iVisibleBitMask : 0;
    FOREACH_GROUP
      pGroup->SetVisibleBitmask(pGroup->m_Instances.m_iCount>0 ? iMask : 0);
      pGroup->m_bCastDynamicShadows = m_bCastDynamicShadows;
    }
    SetFarClipDistance(m_fFarClip);
  }

  void EngineInstanceDecorationGroup::SetFarClipDistance(float fDistance)
  {
    m_fFarClip = fDistance;
    FOREACH_GROUP
      pGroup->SetFarClipDistance(fDistance);
    }
  }

  float EngineInstanceDecorationGroup::GetAverageModelRadius(array<ModelGroup::GroupEntry ^> ^models)
  {
    if (models==nullptr || models->Length==0)
      return 0.f;

    float fRadius = 0.f;

    for (int i=0;i<models->Length;i++)
    {
      IVTerrainDecorationModel *pNativeModel = Helper_LoadNativeModel(models[i]);
      // update average radius
      if (pNativeModel!=NULL && pNativeModel->m_LocalBBox.isValid())
      {
        float x = hkvMath::Max(-pNativeModel->m_LocalBBox.m_vMin.x, pNativeModel->m_LocalBBox.m_vMax.x);
        float y = hkvMath::Max(-pNativeModel->m_LocalBBox.m_vMin.y, pNativeModel->m_LocalBBox.m_vMax.y);
        float xy = hkvMath::Max(x,y);
        fRadius = hkvMath::Max(fRadius,xy);
      }
    }
    return fRadius;
  }



  IVTerrainDecorationModel* EngineInstanceDecorationGroup::Helper_LoadNativeModel(ModelGroup::GroupEntry ^pGroup)
  {
    if (pGroup==nullptr)
      return NULL;

    // optimize this routine through cache
    VString sFilename;
    ConversionUtils::StringToVString(pGroup->Filename,sFilename);
    IVTerrainDecorationModel::ModelType_e eForcedType = IVTerrainDecorationModel::Model; // for now, force it to .model
    IVTerrainDecorationModel::ModelProperties_t props;
    props.m_iModelID = -1; // check for exact match of props, see VTerrainDecorationModelManager::CreateModel
    props.m_fCollisionCapsuleRadius = m_fCollisionCapsuleRadius;
    props.m_eCollisionType = (VDecorationCollisionPrimitive::VDecorationCollisionType_e)m_eCollisionType;
    props.m_fFarClip = m_fFarClip;
    IVTerrainDecorationModel *pNativeModel = VTerrainDecorationModelManager::GlobalManager().CreateModel(sFilename, props,true, eForcedType);
    return pNativeModel;
  }

  void EngineInstanceDecorationGroup::UpdateInstances()
  {
    FOREACH_GROUP
      pGroup->ClearInstances();
    }

    if (m_pInstancesSortedByModel==nullptr)
      return;

    for (int i=0;i<m_pInstancesSortedByModel->Length;i++)
    {
      VTerrainDecorationGroup *pNativeGroup = NULL;
      if (i<m_pGroups->Count())
        pNativeGroup = m_pGroups->GetAt(i);
      else
      {
        pNativeGroup = new VTerrainDecorationGroup();
        m_pGroups->Add(pNativeGroup);
      }

      ArrayList ^pModelList = m_pInstancesSortedByModel[i];
      if (pModelList==nullptr || pModelList->Count==0)
        continue;

      ModelGroup::GroupEntry ^pGroup = static_cast<DecorationInstanceShape ^>(pModelList[0])->m_pGroupEntry;
      IVTerrainDecorationModel *pNativeModel = Helper_LoadNativeModel(pGroup);
      if (pNativeModel==NULL)
        continue;

      const hkvAlignedBBox &bbox(pNativeModel->m_LocalBBox);
      float fModelRadius = hkvMath::Max(bbox.getSizeX(),bbox.getSizeY()) * 0.5f;
      pNativeGroup->SetUsePerInstanceVisibility(m_bPerInstanceVisibility);
      VTerrainDecorationInstanceCollection &coll = pNativeGroup->BeginUpdateInstances();
        coll.Allocate(NULL,pModelList->Count);
        VTerrainDecorationInstance *pDest = coll.m_pInstances;
        for (int j=0;j<pModelList->Count;j++,pDest++)
        {
          DecorationInstanceShape ^src = static_cast<DecorationInstanceShape ^>(pModelList[j]);
          pDest->m_spModel = pNativeModel;
          Vector3F pos = src->RenderPosition;
          hkvVec3 wsPos(pos.X,pos.Y,pos.Z);
          float fExtraScaling = pGroup->GetRandomScale(src->RandomSeed);
          float fScale = src->UniformScaling * m_fScaling * fExtraScaling; // combine scalings

          // update shape's cached properties:
          src->_fCachedRadius = fModelRadius * fScale;
          src->_fCachedLocalMinX = bbox.m_vMin.x * fScale;
          src->_fCachedLocalMinY = bbox.m_vMin.y * fScale;
          src->_fCachedLocalMinZ = bbox.m_vMin.z * fScale;
          src->_fCachedLocalMaxX = bbox.m_vMax.x * fScale;
          src->_fCachedLocalMaxY = bbox.m_vMax.y * fScale;
          src->_fCachedLocalMaxZ = bbox.m_vMax.z * fScale;

          hkvMat3 rotMat;
          ConversionUtils::CreateRotationMatrix(rotMat, src->Yaw, src->Pitch, src->Roll);
          rotMat *= fScale; // the scaled matrix must be passed anyway
          pDest->SetTransformation(wsPos, rotMat, fScale);
          pDest->m_InstanceColor.SetRGBA(src->_iTintColor);
        }
      pNativeGroup->EndUpdateInstances();
    }

    // since we have fresh instances, re-assign other properties:
    UpdateLightgridColors();
    UpdateVisibleStatus();
    //SetEnableCollisions(m_bUseCollisions);
  }

  void EngineInstanceDecorationGroup::RenderCollisionCapsules()
  {
    if (m_fCollisionCapsuleRadius<=0.f || m_eCollisionType!=DecorationCollisionType_e::Capsule)
      return;
    IVRenderInterface* pRI = Vision::Game.GetDebugRenderInterface();
    FOREACH_GROUP
      VTerrainDecorationInstance *pInst = pGroup->m_Instances.m_pInstances;
      for (int j=0;j<pGroup->m_Instances.m_iCount;j++,pInst++)
      {
        int iNumCapsules = pInst->GetModel()->GetCollisionSubObjectCount();
        for (int k=0;k<iNumCapsules;k++)
        {
          VDecorationCollisionPrimitive* pCapsule = pInst->GetModel()->GetCollisionSubObject(k);
          pCapsule->DebugRender(pRI, pInst->GetTransformation(),pInst->GetScaling());
        }
      }

    }
  }


  bool EngineInstanceDecorationGroup::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    return false;
  }

  void EngineInstanceDecorationGroup::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
    static VisTraceLineInfo_t hitHinfo;
    FOREACH_GROUP
      if (pGroup->TraceTest(vStart,vEnd,1,&hitHinfo)==0)
        continue;
      (*result).SetShapeHit_IfCloser(ownerShape,hitHinfo.distance);
    }
  }

  DecorationInstanceShape ^ EngineInstanceDecorationGroup::GetBestTraceHit(Shape3D ^parentShape, array<DecorationInstanceShape ^>^ pData, Vector3F rayStart, Vector3F rayEnd)
  {

    return nullptr; // TODO
  }

  void EngineInstanceDecorationGroup::GetDropToFloorHeights(Shape3D ^parentShape, array<DecorationInstanceShape ^>^ pData, Shape3D::DropToFloorMode mode, Vector3F axis, ShapeCollection ^colliderShapes)
  {
    float fPickDist = EditorManager::Settings->MaxPickingDistance;
    VisTraceLineInfo_t* pTraceInfo = new VisTraceLineInfo_t();

    for (int i=0;i<pData->Length;i++)
    {
      DecorationInstanceShape ^inst = pData[i];
      if (mode == Shape3D::DropToFloorMode::Pivot)
      {
        // perform a traceline
        Vector3F wsPos(inst->x,inst->y,inst->z);
        hkvVec3 vStart(wsPos.X,wsPos.Y,wsPos.Z);
        hkvVec3 vEnd = vStart + hkvVec3 (0,0,-fPickDist);

        Vision::CollisionToolkit.TraceLine(vStart,vEnd,VIS_TRACE_ALL,VIS_TRACE_NONE,NULL,NULL,pTraceInfo);
        // ignore custom shapes here as this is a very expensive instruction - traceline against static geom should be enough
        //EditorManager::EngineManager->TriggerTracelineEvent(Vector3F(vStart.x,vStart.y,vStart.z), Vector3F(vEnd.x,vEnd.y,vEnd.z));
        //VisionEngineManager::TraceCustomMeshes(pTraceInfo,vStart,vEnd,colliderShapes);
        if (pTraceInfo->detected)
        {
          inst->x += axis.X * pTraceInfo->distance;
          inst->y += axis.Y * pTraceInfo->distance;
          inst->z += axis.Z * pTraceInfo->distance;
        }
      }
    }
    delete pTraceInfo;
  }



  void EngineInstanceDecorationGroup::RelaxPositions(array<DecorationInstanceShape ^>^ pData, array<Vector3F >^ pNewPos, int iIterations)
  {
    int iCount = pData->Length;
    hkvVec3* pForces = new hkvVec3[iCount];
    float fEps = 0.1f;
    float fInvDistScale = 1.f;

    for (int i = 0; i < iCount; ++i)
      pForces[i].setZero();

    // copy over initial positions
    for (int i=0;i<iCount;i++)
      pNewPos[i] = pData[i]->Position;

    for (int iter=0;iter<iIterations;iter++)
    {
      for (int a=0;a<iCount;a++)
        for (int b=a+1;b<iCount;b++)
        {
          DecorationInstanceShape ^instA = pData[a];
          DecorationInstanceShape ^instB = pData[b];
          hkvVec3 vA(pNewPos[a].X,pNewPos[a].Y,pNewPos[a].Z);
          hkvVec3 vB(pNewPos[b].X,pNewPos[b].Y,pNewPos[b].Z);
          hkvVec3 vDiff = vB-vA;
          float rA = instA->_fCachedRadius;
          float rB = instB->_fCachedRadius;
          float fAverage = hkvMath::sqrt(rA*rB);

          float fDistSqr = vDiff.getLengthSquared ()*fInvDistScale;
          float fDist = hkvMath::sqrt(fDistSqr);
          if (fDist>(rA+rB))
            continue;

          if (fDist<fEps)
          {
            vDiff.set(Vision::Game.GetFloatRandNeg(),Vision::Game.GetFloatRandNeg(),0.001f);
            vDiff.setLength(fAverage);
          }
          else
          {
            if (fDistSqr<fAverage)
              fDistSqr = fAverage;
            vDiff *= (fAverage/fDistSqr); // 1/r falloff
          }

          pForces[a] -= vDiff;
          pForces[b] += vDiff;
        }

      // apply forces and reset
      for (int j=0;j<iCount;j++)
      {
        hkvVec3& force(pForces[j]);
        pNewPos[j].X += force.x;
        pNewPos[j].Y += force.y;
        //inst->_z += force.z; // do not move up or down
        force.setZero();
      }
    }
    delete[] pForces;
  }


  String^ EngineInstanceDecorationGroup::BakeInstances(Shape3D ^owner, String ^filename)
  {
    VString sFilename;
    String ^absFile = EditorManager::Project->MakeAbsolute(filename);
    ConversionUtils::StringToVString(absFile,sFilename);

    ManagedBase::RCS::GetProvider()->EditFile(absFile);

    IVFileOutStream *pOut = Vision::File.Create(sFilename);
    if (pOut==NULL)
      return "failed to create output file";

    int iInstanceCount = m_pInstances->Length;
    VGroupInstanceFile file;
    file.Create(pOut, iInstanceCount, VGroupInstance::INSTANCE_TYPE_DECORATION_LS);

    // loop through the sorted-by-model list to set the group index
    for (int iSplit=0;iSplit<m_pInstancesSortedByModel->Length;iSplit++)
    {
      ArrayList ^group = m_pInstancesSortedByModel[iSplit];
      int iSubCount = group->Count;
      for (int i=0;i<iSubCount;i++)
      {
        VGroupInstance instance;
        // fill out instance info
        DecorationInstanceShape ^src = static_cast<DecorationInstanceShape ^>(group[i]);

        // transform to local space:
        instance.m_vPosition.set(src->LocalSpacePosition.X,src->LocalSpacePosition.Y,src->LocalSpacePosition.Z);
        ConversionUtils::Matrix3FToVisMatrix3x3(instance.m_Rotation, src->LocalSpaceRotation);
        instance.m_fScale = src->UniformScaling;
        instance.m_iColor.SetRGBA(src->_iTintColor);
        instance.m_fSeed = src->RandomSeed;
        instance.m_iSplitKey = iSplit;
        file.WriteInstance(instance);
      }
    }

    file.Close();
    ManagedBase::RCS::GetProvider()->AddFile(absFile, true /* Binary file */);

    return nullptr;
  }


  static inline int CompareSplitKey( const void *arg1, const void *arg2)
  {
    VGroupInstance *pElem1 = (VGroupInstance *)arg1;
    VGroupInstance *pElem2 = (VGroupInstance *)arg2;
    return (pElem2->m_iSplitKey - pElem1->m_iSplitKey);
  }


  void EngineInstanceDecorationGroup::Helper_Assign(VTerrainDecorationInstance &dest, const VGroupInstance &src, float fExternalScale, IVTerrainDecorationModel* pModel, const hkvVec3& apply_ofs, const hkvMat3 &apply_rotation)
  {
    float fScale = src.m_fScale * fExternalScale;
    hkvMat3 rotation = src.m_Rotation * fScale;
    dest.SetTransformation(
      apply_rotation.transformDirection(src.m_vPosition) + apply_ofs, 
      apply_rotation.multiply(rotation), fScale);

    dest.m_spModel = pModel;
    dest.m_pOwnerSector = NULL;
    dest.m_InstanceColor = src.m_iColor;
  }

  String^ EngineInstanceDecorationGroup::CreateFromBinaryFile(Shape3D ^owner, String ^filename, IGroupHelper ^atlas, BoundingBox ^localBBox)
  {
    VString sFilename;
    ConversionUtils::StringToVString(EditorManager::Project->MakeAbsolute(filename),sFilename);

    IVFileInStream *pIn = Vision::File.Open(sFilename);
    if (pIn==NULL)
      return "failed to open file";

    FOREACH_GROUP
      pGroup->ClearInstances();
    }

    // read binary file into one block
    int iInstanceCount;
    VGroupInstance::AvailableFlags_e eFlags;
    VGroupInstanceFile file;
    file.Open(pIn,iInstanceCount,eFlags);
    if (iInstanceCount<1)
    {
      file.Close();
      return nullptr;
    }

    hkvMat3 applyRotation = hkvMat3::IdentityMatrix();
    hkvVec3 applyOffset(0,0,0);
    bool bIsInLocalSpace = (eFlags & VGroupInstance::INSTANCES_IN_LOCALSPACE)!=0;
    if (bIsInLocalSpace)
    {
      ConversionUtils::Matrix3FToVisMatrix3x3(applyRotation, owner->RotationMatrix);
      applyOffset.set(owner->x,owner->y,owner->z);
    }

    VGroupInstance *pSrcArray = new VGroupInstance[iInstanceCount];
    for (int i=0;i<iInstanceCount;i++)
      file.ReadInstance(pSrcArray[i]);

    // sort the block - at least by by model but also other split criteria
    qsort(pSrcArray,iInstanceCount,sizeof(VGroupInstance),CompareSplitKey);
    hkvAlignedBBox combinedBBox;
    combinedBBox.setInvalid();

    // create a group - for each distinct model
    int iStartIndex = 0;
    int iGroupCount = 0;
    for (int i=1;i<=iInstanceCount;i++)
    {
      if (i<iInstanceCount && pSrcArray[iStartIndex].m_iSplitKey==pSrcArray[i].m_iSplitKey)
        continue;

      // lookup model only once. Not correct if atlas has changed in the meanwhile, but much faster
      float fFirstSeed = pSrcArray[iStartIndex].m_fSeed;
      ModelGroup::GroupEntry ^entry = atlas->GetModel(fFirstSeed);
      IVTerrainDecorationModel* pNativeModel = this->Helper_LoadNativeModel(entry);
      if (pNativeModel==NULL)
      {
        iStartIndex = i;
        continue;
      }

      // start new group:
      VTerrainDecorationGroup *pNewGroup;
      if (iGroupCount<m_pGroups->Count())
        pNewGroup = m_pGroups->GetAt(iGroupCount);
      else
      {
        pNewGroup = new VTerrainDecorationGroup(true);
        m_pGroups->Add(pNewGroup);
      }

      // fill the group with instances
      int iCount = i-iStartIndex;
      VGroupInstance *pSrcInst = &pSrcArray[iStartIndex];
      pNewGroup->SetUsePerInstanceVisibility(m_bPerInstanceVisibility);
      VTerrainDecorationInstanceCollection &coll = pNewGroup->BeginUpdateInstances();
        coll.Allocate(NULL,iCount);
        for (int j=0;j<iCount;j++,pSrcInst++)
          Helper_Assign(coll.m_pInstances[j], *pSrcInst, entry->GetRandomScale(pSrcInst->m_fSeed), pNativeModel, applyOffset, applyRotation);
      pNewGroup->EndUpdateInstances();
      hkvAlignedBBox bbox = pNewGroup->GetBoundingBox();
      if (bbox.isValid())
        combinedBBox.expandToInclude(bbox);

      iGroupCount++;
      iStartIndex = i;
    }

    
    file.Close();
    delete[] pSrcArray;

    // since we have fresh instances, re-assign other properties:
    UpdateLightgridColors();
    UpdateVisibleStatus();
    if (combinedBBox.isValid())
      localBBox->Set(combinedBBox.m_vMin.x, combinedBBox.m_vMin.y, combinedBBox.m_vMin.z, combinedBBox.m_vMax.x, combinedBBox.m_vMax.y, combinedBBox.m_vMax.z);
    return nullptr;
  }

  String^ EngineInstanceDecorationGroup::UnbakeInstances(Shape3D ^owner, String ^filename, IGroupHelper ^creator, ShapeCollection ^instances)
  {
    VString sFilename;
    ConversionUtils::StringToVString(EditorManager::Project->MakeAbsolute(filename),sFilename);

    IVFileInStream *pIn = Vision::File.Open(sFilename);
    if (pIn==NULL)
      return "failed to open file";

    // read binary file into one block
    int iInstanceCount;
    VGroupInstance::AvailableFlags_e eFlags;
    VGroupInstanceFile file;
    file.Open(pIn,iInstanceCount,eFlags);
    VGroupInstance instance;

    Matrix3F applyRotation = owner->RotationMatrix;
    Vector3F applyOffset = owner->Position;
    bool bInLocalSpace = (eFlags & VGroupInstance::INSTANCES_IN_LOCALSPACE)!=0;

    for (int i=0;i<iInstanceCount;i++)
    {
      char szName[128];
      sprintf(szName,"Decoration %i",i);
      file.ReadInstance(instance);
      DecorationInstanceShape ^dest = creator->CreateNewInstanceShape(gcnew String(szName));
      instances->Add(dest);

      dest->_fRandomSeed = instance.m_fSeed;
      dest->UniformScaling = instance.m_fScale;
      dest->_iTintColor = instance.m_iColor.GetNative();
      Vector3F pos(instance.m_vPosition.x, instance.m_vPosition.y, instance.m_vPosition.z);

      if (bInLocalSpace)
      {
        Matrix3F instRot;
        dest->Position = Matrix3F::Transform(owner->RotationMatrix, pos) + applyOffset;
        instRot = ConversionUtils::VisMatrix3x3ToMatrix3F(instance.m_Rotation);
        dest->RotationMatrix = instRot * applyRotation;
      }
      else
      {
        float fRoll,fPitch,fYaw;
        instance.m_Rotation.getAsEulerAngles(fRoll,fPitch,fYaw);
        dest->SetPositionOrientation(pos, Vector3F(fYaw, fPitch, fRoll));
      }
    }
        
    file.Close();

    return nullptr;
  }
  
}

/*
 * Havok SDK - Base file, BUILD(#20140619)
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
