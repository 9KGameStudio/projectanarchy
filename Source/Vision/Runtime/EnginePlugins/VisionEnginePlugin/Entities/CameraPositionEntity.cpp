/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/CameraPositionEntity.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_IMPLEMENT_SERIAL(CameraPositionEntity, VisBaseEntity_cl, 0, &g_VisionEngineModule);

#define CAMERAPOSITIONENTITY_VERSION_0       0 // Initial version
#define CAMERAPOSITIONENTITY_VERSION_1       1 // Start Camera Parameter
#define CAMERAPOSITIONENTITY_VERSION_2       2 // Removed Start Camera Parameter
#define CAMERAPOSITIONENTITY_VERSION_CURRENT CAMERAPOSITIONENTITY_VERSION_2

void CameraPositionEntity::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);

  char iLocalVersion = CAMERAPOSITIONENTITY_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; 
    VASSERT_MSG(iLocalVersion >= CAMERAPOSITIONENTITY_VERSION_0 && iLocalVersion <= CAMERAPOSITIONENTITY_VERSION_CURRENT,
      "Unsupported CameraPositionEntity version number, please re-serialize");

    ar >> m_fNearClipDistance >> m_fFarClipDistance >> m_fFovX;

    if (iLocalVersion == CAMERAPOSITIONENTITY_VERSION_1)
    {
      BOOL unusedIsStartCamera;
      ar >> unusedIsStartCamera;
    }
  } 
  else
  {
    ar << iLocalVersion;
    ar << m_fNearClipDistance << m_fFarClipDistance << m_fFovX;
  }
}

void CameraPositionEntity::ApplyToContext(VisRenderContext_cl* pContext)
{
  VASSERT(pContext != NULL && pContext->GetCamera() != NULL);

  // Apply position and transformation.
  VisContextCamera_cl* pCamera = pContext->GetCamera();
  pCamera->AttachToEntity(this, hkvVec3::ZeroVector());

  // Apply clip distances.
  float fNear, fFar;
  pContext->GetClipPlanes(fNear, fFar);

  if (m_fNearClipDistance > 0.0f)
    fNear = m_fNearClipDistance;
  if (m_fFarClipDistance > 0.0f)
    fFar = m_fFarClipDistance;

  fFar = hkvMath::Max(fFar, fNear + 0.001f);

  pContext->SetClipPlanes(fNear, fFar);

  // Apply FOV.
  if (m_fFovX > 0.0f)
  {
    float fFovY = 0.0f;
    int iWidth, iHeight;
    pContext->GetSize(iWidth, iHeight);

    hkvMathHelpers::adjustFovsForAspectRatio(m_fFovX, fFovY, static_cast<float>(iWidth) / static_cast<float>(iHeight));

    pContext->SetFOV(m_fFovX, fFovY);
  }
}

/*static*/ CameraPositionEntity* CameraPositionEntity::ApplyToContext(VisRenderContext_cl* pContext, const char* szKey)
{
  VisBaseEntity_cl* pEnt = Vision::Game.SearchEntity(szKey);
  CameraPositionEntity* pCam =  vdynamic_cast<CameraPositionEntity*>(pEnt);

  if (pCam == NULL)
    return NULL;

  pCam->ApplyToContext(pContext);
  return pCam;
}

// Hide this entity class in vForge because there is a dedicated C# shape type.
START_VAR_TABLE(CameraPositionEntity, VisBaseEntity_cl, "CameraPositionEntity", VFORGE_HIDECLASS, "")  
  DEFINE_VAR_FLOAT_AND_NAME(CameraPositionEntity, m_fNearClipDistance, "NearClipDistance", "Custom near clip distance (or 0)", "0.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(CameraPositionEntity, m_fFarClipDistance, "FarClipDistance", "Custom far clip distance (or 0)", "0.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(CameraPositionEntity, m_fFovX, "FovX", "Custom FOV (or 0)", "0.0", 0, 0);
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
