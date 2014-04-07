/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceMobileWater.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/MobileWater.hpp>

namespace VisionManaged
{

  EngineInstanceMobileWater::EngineInstanceMobileWater()
  {
    m_uiVisibilityBitMask = VIS_ENTITY_VISIBLE;
    m_bVisible = true;
    m_pMobileWater = new VMobileWater();
    m_pMobileWater->AddRef();
  }

  void EngineInstanceMobileWater::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pMobileWater);
  }

  void EngineInstanceMobileWater::UpdateVisibleStatus()
  {
    unsigned int uiMask = (m_bVisible) ? m_uiVisibilityBitMask : 0;
    m_pMobileWater->SetVisibilityBitMask(uiMask);
  }

  void EngineInstanceMobileWater::UpdateStaticLighting()
  {
    m_pMobileWater->UpdateStaticLighting();
  }

  bool EngineInstanceMobileWater::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    float x = m_pMobileWater->GetSize().x * 0.5f;
    float y = m_pMobileWater->GetSize().y * 0.5f;
    (*bbox).Set(-x,-y,-1.f,x,y,1.f);

    return true;
  }

  void EngineInstanceMobileWater::TraceShape(Shape3D ^ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    float fDist = m_pMobileWater->TracePlane(
      hkvVec3(rayStart.X,rayStart.Y,rayStart.Z),
      hkvVec3(rayEnd.X,rayEnd.Y,rayEnd.Z), 
      true);

    if (fDist<0.f)
      return;

    (*result).SetShapeHit_IfCloser(ownerShape, fDist);
  }

  bool EngineInstanceMobileWater::OnExport(SceneExportInfo ^info) 
  {
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pMobileWater;
    return true;
  }

  void EngineInstanceMobileWater::SetScaling(float x,float y, float z)
  {
    // do nothing
  }

  void EngineInstanceMobileWater::SetSize(float x, float y)
  {
    m_pMobileWater->SetSize(hkvVec2(x,y));
  }

  void EngineInstanceMobileWater::SetNumGridSubdivisions(int x, int y)
  {
    m_pMobileWater->SetNumGridSubdivisions(x, y);
  }

  void EngineInstanceMobileWater::SetUseRadialGrid(bool bValue)
  {
    m_pMobileWater->SetUseRadialGrid(bValue);
  }

  void EngineInstanceMobileWater::SetRenderHook(unsigned int uiRenderHook)
  {
    VASSERT(uiRenderHook > VRH_INVALID);
    m_pMobileWater->SetRenderHook(static_cast<VRenderHook_e>(uiRenderHook));
  }

  void EngineInstanceMobileWater::SetShaderEffect(ShaderEffectConfig ^fxConfig)
  {
    VCompiledEffect *pEffect = ConversionUtils::GetShaderEffect(fxConfig, true, true);
    m_pMobileWater->SetEffect(pEffect); 
  }

  void EngineInstanceMobileWater::SetTextureTiling(float x, float y)
  {
    m_pMobileWater->SetTextureTiling(hkvVec2(x, y));
  }

  void EngineInstanceMobileWater::SetUseStaticLighting(bool bValue)
  {
    m_pMobileWater->SetUseStaticLighting(bValue);
  }
}

/*
 * Havok SDK - Base file, BUILD(#20140328)
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
