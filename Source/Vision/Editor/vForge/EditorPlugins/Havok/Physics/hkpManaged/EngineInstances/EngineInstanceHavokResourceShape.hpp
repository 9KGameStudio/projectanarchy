/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;
using namespace VisionManaged;

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokResourceShape.hpp>


namespace HavokManaged
{
  public ref class EngineInstanceHavokResourceShape : public IEngineShapeInstance
  {
  public:
    /// @name Construction/Destruction
    //@{
    EngineInstanceHavokResourceShape();

    virtual void DisposeObject() override;
    //@}

    /// @name Overridden IEngineShapeInstance functions
    //@{
    virtual void SetVisible(bool bStatus) override {}
    virtual void SetPosition(float x,float y,float z) override {}
    virtual void SetOrientation(float yaw,float pitch,float roll) override {}
    virtual void SetScaling(float x,float y, float z) override {}
    virtual void SetUniqueID(unsigned __int64 iID) override;

    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult ^%result) override;
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual IntPtr GetNativeObject() override { return IntPtr(m_pShape); }
    virtual void GetDependencies(ResourceSnapshotInfo ^info) override;

    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual void OnBeforeExport(SceneExportInfo ^info) override {if (m_pShape!=NULL) m_pShape->SetMarkInsideSerializationSession(true);}
    virtual void OnAfterExport(SceneExportInfo ^info) override {if (m_pShape!=NULL) m_pShape->SetMarkInsideSerializationSession(false);}

    //@}

    void LoadFromFile(String ^filename, BoundingBox ^boundingBox);
    void SetAllowAsyncLoading(bool bStatus) {m_pShape->m_bLoadAsynchronously = bStatus;}
  protected:
    vHavokResourceShape *m_pShape;
  };

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
