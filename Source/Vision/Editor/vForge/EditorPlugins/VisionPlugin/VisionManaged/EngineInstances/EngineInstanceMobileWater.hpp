/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;
using namespace CSharpFramework::BaseTypes;

// forward declaration
class VMobileWater;

namespace VisionManaged
{
  public ref class EngineInstanceMobileWater : public IEngineInstanceObject3D
  {
  public:  
    EngineInstanceMobileWater();
    virtual void DisposeObject() override;
    virtual void *GetObject3D() override  {return m_pMobileWater;}

    // overridden IEngineShapeInstance functions
    virtual void SetVisible(bool bStatus) override { m_bVisible=bStatus; UpdateVisibleStatus(); }
    virtual void SetVisibleBitmask(unsigned int uiMask) { m_uiVisibilityBitMask=uiMask; UpdateVisibleStatus(); }

    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual void SetScaling(float x, float y, float z) override;

    // visibility
    void UpdateVisibleStatus();
    void UpdateStaticLighting();

    // setters
    void SetSize(float x, float y);
    void SetGridCellSize(float x, float y);
    void SetUseRadialGrid(bool bValue);
    void SetRenderHook(unsigned int uiRenderHook);
    void SetShaderEffect(ShaderEffectConfig ^fxConfig);
    void SetTextureTiling(float x, float y);
    void SetUseStaticLighting(bool bValue);

  private:
    bool m_bVisible;
    unsigned int m_uiVisibilityBitMask;
    VMobileWater* m_pMobileWater;
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
