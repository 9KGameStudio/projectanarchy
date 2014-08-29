/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/EngineInstances/EngineInstanceHavokResourceShape.hpp>

using namespace System::Diagnostics;

namespace HavokManaged
{

  EngineInstanceHavokResourceShape::EngineInstanceHavokResourceShape()
  {
    m_pShape = new vHavokResourceShape();
    m_pShape->AddRef();
  }

  void EngineInstanceHavokResourceShape::DisposeObject()
  {
    if (m_pShape)
    {
      if (!m_pShape->IsDisposed())
        m_pShape->DisposeObject();
      m_pShape->Release();
      m_pShape = NULL;
    }
  }

  bool EngineInstanceHavokResourceShape::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    float r = 5.0f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-r,-r,-r,r,r,r);
    return false;
  }

  void EngineInstanceHavokResourceShape::SetUniqueID(unsigned __int64 iID)
  {
    if (m_pShape!=NULL)
      m_pShape->SetUniqueID(iID);
  }

  // ----------------------------------------

  // --------------------------------------------------------------------------
  void EngineInstanceHavokResourceShape::TraceShape(
    Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
    //...
  }


  bool EngineInstanceHavokResourceShape::OnExport(SceneExportInfo ^info)
  {
    if (m_pShape == NULL || !m_pShape->IsLoaded())
      return true;
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pShape;
    return true;
  }

  void EngineInstanceHavokResourceShape::GetDependencies(ResourceSnapshotInfo ^info)
  {
    if ( m_pShape==NULL || info->NativeResourceSnapshotPtr.ToPointer()==NULL)
      return;
    VResourceSnapshot *snapshot = (VResourceSnapshot *)info->NativeResourceSnapshotPtr.ToPointer();
    m_pShape->GetDependencies(*snapshot);
  }


  void EngineInstanceHavokResourceShape::LoadFromFile(String ^filename, BoundingBox ^boundingBox)
  {
    VString sFilename;
    ConversionUtils::StringToVString(filename, sFilename);
    if (sFilename.IsEmpty())
      m_pShape->Unload();
    else
    {
      // validate and resolve the path:
      char szCompatibleName[FS_MAX_PATH];
      VFileHelper::CombineDirAndFile(szCompatibleName, "", sFilename);

      m_pShape->LoadFromFile(szCompatibleName, true);
    }

    // also copy invlid bboxes
    const hkvAlignedBBox& b(m_pShape->m_BoundingBox);
    boundingBox->Set(b.m_vMin.x, b.m_vMin.y, b.m_vMin.z,  b.m_vMax.x, b.m_vMax.y, b.m_vMax.z);
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
