/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/HavokAi/HavokAiPCH.h>
#include <Vision/Samples/Engine/HavokAi/PathPickDlg.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>

PathPickDlg::PathPickDlg()
  : m_spCursor(NULL)
  , m_vStartPoint(-500.0f, -500.0f, 0.0f)
  , m_vEndPoint(500.0f, 500.0f, 0.0f)
{
}

void PathPickDlg::OnSetFocus(bool bStatus)
{
  m_spCursor = GetMenuManager()->LoadCursorResource("GUI/mouseNorm.TGA");
}

VCursor* PathPickDlg::GetCurrentCursor(VGUIUserInfo_t &user)
{
  return m_spCursor;
}

void PathPickDlg::OnClick(VMenuEventDataObject *pEvent)
{
  VDialog::OnClick(pEvent);

  hkvVec2 vMousePos = GetContext()->GetCurrentMousePos();
  hkvVec3 vDir;
  Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(vMousePos.x, vMousePos.y, vDir, 10000.f);
  hkvVec3 vFrom = Vision::Camera.GetCurrentCameraPosition();
  hkvVec3 vTo = vFrom + vDir;

  const float v2h = vHavokConversionUtils::GetVision2HavokScale();
  hkVector4 from; from.set(vFrom.x*v2h, vFrom.y*v2h, vFrom.z*v2h);
  hkVector4 to; to.set(vTo.x*v2h, vTo.y*v2h, vTo.z*v2h);

  hkaiNavMeshQueryMediator::HitDetails hit;
  hit.m_hitFaceKey = HKAI_INVALID_PACKED_KEY;
  hit.m_hitFraction = -1.0f;
  if (vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->castRay(from, to, hit))
  {
    hkVector4 hitPoint; hitPoint.setInterpolate(from, to, hit.m_hitFraction);
    hitPoint.mul4(vHavokConversionUtils::GetHavok2VisionScale());
    if (pEvent->m_iButtons & BUTTON_LMOUSE)
    {
      m_vStartPoint = hkvVec3(hitPoint(0), hitPoint(1), hitPoint(2));
    }
    else
    {
      m_vEndPoint = hkvVec3(hitPoint(0), hitPoint(1), hitPoint(2));
    }
  }
}

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
