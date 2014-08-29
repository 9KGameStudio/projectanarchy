/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VOrbitCamera.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessDepthOfField.hpp>
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VZoomDetector.hpp>

enum CAMERA_CONTROL 
{
  CAMERA_HORIZONTAL_MOVEMENT,
  CAMERA_VERTICAL_MOVEMENT,
  CAMERA_ZOOM,

  CAMERA_NUM_ELEMENTS
};

#define INPUTMAP_CAMERA_ALTERNATIVES   4

VOrbitCamera::VOrbitCamera(int iComponentFlags) 
  : IVObjectComponent(0, iComponentFlags)
  , Enabled(TRUE)
  , Collides(FALSE)
  , Follow(TRUE)
  , Zoom(TRUE)
  , DepthOfField(FALSE)
  , FollowFixed(FALSE)
  , InitialYaw(0.0f)
  , InitialPitch(30.0f)
  , MinimumPitch(-89.0f)
  , MaximumPitch(89.0f)
  , CameraDistance(500.0f)
  , MinimalDistance(200.0f)
  , MaximalDistance(1000.0f)
  , RelativeLookAtHeight(0.5f)
  , CameraSensitivity(200.0f)
  , MoveSmoothness(0.3f)
  , m_spCameraProxy(NULL)
  , m_vOldLookAt(hkvVec3::ZeroVector())
  , m_vOldBaseOri(hkvVec3::ZeroVector())
  , m_vOffsetOri(hkvVec3::ZeroVector())
  , m_fCurrentDistance(0.0f)
#if defined(SUPPORTS_MULTITOUCH)
  , m_pZoomDetector(NULL)
#endif
  , m_pInputMap(NULL)
{
  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnEditorModeChanged += this;

  // Create input map
  m_pInputMap = new VInputMap(CAMERA_NUM_ELEMENTS, INPUTMAP_CAMERA_ALTERNATIVES);
  
  // Map mouse and touch input depending on physical touch/cursor distance.
#if defined(SUPPORTS_MOUSE)

  m_pInputMap->MapTrigger(CAMERA_HORIZONTAL_MOVEMENT, V_MOUSE, CT_MOUSE_ABS_DELTA_X,   VInputOptions::Sensitivity(0.18f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CAMERA_VERTICAL_MOVEMENT,   V_MOUSE, CT_MOUSE_ABS_DELTA_Y,   VInputOptions::Sensitivity(0.18f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CAMERA_ZOOM,                V_MOUSE, CT_MOUSE_WHEEL,         VInputOptions::Sensitivity(-15.0f));

#endif

#if defined(_VISION_MOBILE) && defined(SUPPORTS_MULTITOUCH)

  VTouchArea* pTouchArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(), -2000.0f);
  m_pInputMap->MapTrigger(CAMERA_HORIZONTAL_MOVEMENT, pTouchArea, CT_TOUCH_ABS_DELTA_X, VInputOptions::Sensitivity(0.4f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CAMERA_VERTICAL_MOVEMENT,   pTouchArea, CT_TOUCH_ABS_DELTA_Y, VInputOptions::Sensitivity(0.4f / Vision::Video.GetDeviceDpi()));

  m_pZoomDetector = new VZoomDetector(VRectanglef(), -2000.0f);
  m_pInputMap->MapTrigger(CAMERA_ZOOM, *m_pZoomDetector, CT_TOUCH_ZOOM, VInputOptions::Sensitivity(-30.0f / Vision::Video.GetDeviceDpi()));

#endif

  // Map button/key/joystick based input - since we accumulate the values every frame, we need to timescale these inputs to achieve a frame rate independent behavior.

  VInputOptions camMoveOptions;
  camMoveOptions.m_bTimeScaled = true;
  camMoveOptions.m_fSensitivity = 0.5f;
  camMoveOptions.m_fDeadZone = 0.2f;

  VInputOptions camZoomOptions;
  camZoomOptions.m_bTimeScaled = true;
  camZoomOptions.m_fSensitivity = 3.0f;

#if defined(SUPPORTS_KEYBOARD)

  m_pInputMap->MapTriggerAxis(CAMERA_VERTICAL_MOVEMENT,   V_KEYBOARD, CT_KB_KP_8, CT_KB_KP_2, camMoveOptions);
  m_pInputMap->MapTriggerAxis(CAMERA_HORIZONTAL_MOVEMENT, V_KEYBOARD, CT_KB_KP_4, CT_KB_KP_6, camMoveOptions);
  m_pInputMap->MapTriggerAxis(CAMERA_ZOOM,                V_KEYBOARD, CT_KB_PGDN, CT_KB_PGUP, camZoomOptions);

#endif

  // Map the same input for all pad-like devices
  IVInputDevice* padDevices[] =
  {
#if defined (_VISION_XENON) || defined (_VISION_PS3) || defined (_VISION_PSP2) || ( defined(_VISION_WIN32) && !defined( _VISION_APOLLO ) )
    VInputManager::IsInitialized() ? &V_PAD(0) : NULL,
#elif defined(_VISION_WIIU)
    &VInputManagerWiiU::GetDRC(V_DRC_FIRST),
    &VInputManagerWiiU::GetRemote(0),
#endif
    NULL
  };

  for(int iDeviceIdx = 0; iDeviceIdx < V_ARRAY_SIZE(padDevices) - 1; iDeviceIdx++)
  {
    if(padDevices[iDeviceIdx] == NULL)
    {
      continue;
    }

    m_pInputMap->MapTriggerAxis(CAMERA_VERTICAL_MOVEMENT,   *padDevices[iDeviceIdx], CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  camMoveOptions);
    m_pInputMap->MapTriggerAxis(CAMERA_HORIZONTAL_MOVEMENT, *padDevices[iDeviceIdx], CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, camMoveOptions);
    m_pInputMap->MapTriggerAxis(CAMERA_ZOOM,                *padDevices[iDeviceIdx], CT_PAD_LEFT_TRIGGER,           CT_PAD_RIGHT_TRIGGER,           camZoomOptions);
  }
}

VOrbitCamera::~VOrbitCamera()
{
  Vision::Callbacks.OnEditorModeChanged -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;

  V_SAFE_DELETE(m_pInputMap);

#if defined(SUPPORTS_MULTITOUCH)
  V_SAFE_DELETE(m_pZoomDetector);
#endif

  Enabled = FALSE;
  UpdateAttachment();
}

void VOrbitCamera::SetEnabled(bool bStatus) 
{
	Enabled = bStatus ? TRUE: FALSE;
	UpdateAttachment(); 
}

void VOrbitCamera::UpdateAttachment()
{
  VisContextCamera_cl* pMainCamera = Vision::Camera.GetMainCamera();
  if (Enabled && Vision::Editor.IsPlayingTheGame() && GetOwner() != NULL)
  {
    if (!m_spCameraProxy)
    {
      m_spCameraProxy = Vision::Game.CreateEntity<VisBaseEntity_cl>(hkvVec3::ZeroVector());
      m_spCameraProxy->SetEntityKey("<VOrbitCamera:CameraProxy>"); 
    }
    
    // (Re)attach main camera.
    pMainCamera->AttachToEntity(m_spCameraProxy, hkvVec3::ZeroVector()); 

    // Re-set the initial camera transformation.
    SetupCamera();
  }
  else
  {
    if (m_spCameraProxy)
    {
      if (pMainCamera->GetParent() == m_spCameraProxy)
      {
        pMainCamera->AttachToEntity(NULL, hkvVec3::ZeroVector());
      }
      m_spCameraProxy = NULL;
    }
  }
}

BOOL VOrbitCamera::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
  {
    return false;
  }

  if (!pObject->IsOfType(VisBaseEntity_cl::GetClassTypeId()))
  {
    sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VOrbitCamera::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  UpdateAttachment();
}

void VOrbitCamera::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  if (strcmp(pVar->GetName(), "Enabled") == 0 ||
    strcmp(pVar->GetName(), "Follow") == 0 ||
    strcmp(pVar->GetName(), "FollowFixed") == 0 ||
    strcmp(pVar->GetName(), "Zoom") == 0)
  {
    UpdateAttachment();
  }
  else if (strcmp(pVar->GetName(), "MinimumPitch") == 0)
  {
    MinimumPitch = hkvMath::Min(MinimumPitch, MaximumPitch);
  }
  else if (strcmp(pVar->GetName(), "MaximumPitch") == 0)
  {
    MaximumPitch = hkvMath::Max(MinimumPitch, MaximumPitch);
  }
  else if (strcmp(pVar->GetName(), "MinimalDistance") == 0)
  {
    MinimalDistance = hkvMath::Min(MinimalDistance, MaximalDistance);
  }
  else if (strcmp(pVar->GetName(), "MaximalDistance") == 0)
  {
    MaximalDistance = hkvMath::Max(MinimalDistance, MaximalDistance);
  }
}

V_IMPLEMENT_SERIAL(VOrbitCamera, IVObjectComponent, 0, &g_VisionEngineModule);

// Versions
#define VORBITCAMERACOMPONENT_VERSION_0          0     // Initial version
#define VORBITCAMERACOMPONENT_VERSION_2          2     // Updated version
#define VORBITCAMERACOMPONENT_VERSION_3          3     // Added support for fixed camera orientation (not linked to parent object)
#define VORBITCAMERACOMPONENT_VERSION_4          4     // MinimumPitch / MaximumPitch / DepthOfField
#define VORBITCAMERACOMPONENT_VERSION_CURRENT    VORBITCAMERACOMPONENT_VERSION_4

void VOrbitCamera::Serialize(VArchive &ar)
{
  char iLocalVersion = VORBITCAMERACOMPONENT_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= VORBITCAMERACOMPONENT_VERSION_CURRENT && iLocalVersion >= VORBITCAMERACOMPONENT_VERSION_0, "Invalid local version. Please re-export");

    ar >> Enabled;
    ar >> Collides;
    ar >> Follow;
    ar >> Zoom;

    ar >> InitialYaw;
    ar >> InitialPitch;
    ar >> CameraDistance;
    ar >> MinimalDistance;
    ar >> MaximalDistance;
    ar >> RelativeLookAtHeight;

    ar >> CameraSensitivity;
    ar >> MoveSmoothness;

    if (iLocalVersion >= VORBITCAMERACOMPONENT_VERSION_3)
      ar >> FollowFixed;

    if (iLocalVersion >= VORBITCAMERACOMPONENT_VERSION_4)
    {
      ar >> MinimumPitch;
      ar >> MaximumPitch;
      ar >> DepthOfField;
    }
    else
    {
      // Old default settings, prior to version 4.
      MinimumPitch = 0.0f;
      MaximumPitch = 89.0f;
    }
  } 
  else
  {
    ar << iLocalVersion;

    ar << Enabled;
    ar << Collides;
    ar << Follow;
    ar << Zoom;

    ar << InitialYaw;
    ar << InitialPitch;
    ar << CameraDistance;
    ar << MinimalDistance;
    ar << MaximalDistance;
    ar << RelativeLookAtHeight;

    ar << CameraSensitivity;
    ar << MoveSmoothness;
    ar << FollowFixed;

    ar << MinimumPitch;
    ar << MaximumPitch;
    ar << DepthOfField;
  }
}

void VOrbitCamera::OnDeserializationCallback(const VSerializationContext &context)
{
  UpdateAttachment();
}

void VOrbitCamera::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if(pData->m_pSender == &Vision::Callbacks.OnEditorModeChanged)
  {
    UpdateAttachment();
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    ProcessInput();
  }
}

void VOrbitCamera::ProcessInput()
{
  if (m_spCameraProxy == NULL)
    return;

  float fStepX = CameraSensitivity * m_pInputMap->GetTrigger(CAMERA_HORIZONTAL_MOVEMENT);
  float fStepY = CameraSensitivity * m_pInputMap->GetTrigger(CAMERA_VERTICAL_MOVEMENT);
  float fStepZoom = (Zoom ? CameraSensitivity * m_pInputMap->GetTrigger(CAMERA_ZOOM) : 0.0f);

#if defined(_VISION_MOBILE)
  // Disable rotation on mobile if there is zoom, otherwise the camera is zooming and rotating at the same time.
  if (m_pZoomDetector->IsZooming())
  {
    fStepX = fStepY = 0.0f;
  }
#endif

  // Calculate current blending weight.
  float fWeight;
  if (MoveSmoothness < HKVMATH_SMALL_EPSILON)
  {
    fWeight = 1.0f;
  }
  else
  {
    fWeight = Vision::GetTimer()->GetTimeDifference() / MoveSmoothness;
    fWeight = hkvMath::clamp(fWeight, HKVMATH_SMALL_EPSILON, 1.0f);
  }

  // Step camera move
  UpdateCamera(fStepX, fStepY, fStepZoom, fWeight); 
}

void VOrbitCamera::SetupCamera()
{
  if (GetOwner() == NULL || !Enabled)
    return;

  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(GetOwner());

  // Set initial orientation offset
  if (Follow)
  {
    m_vOffsetOri.setZero();
  }
  // Use initial orientation from owner as a fixed offset (if not following).
  else
  {
    m_vOffsetOri = pOwnerEntity->GetOrientation();
  }

  m_vOldLookAt = ComputeCenterPosition();
  m_vOldBaseOri = ComputeBaseOrientation();

  CameraDistance = m_fCurrentDistance = hkvMath::clamp(CameraDistance, MinimalDistance, MaximalDistance);
  
  UpdateCamera(0.0f, 0.0f, 0.0f, 1.0f);
}

void VOrbitCamera::UpdateCamera(float fYawStep, float fPitchStep, float fZoomStep, float fWeight)
{
  if (GetOwner() == NULL)
    return;

  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(GetOwner());

  // Get position of entity
  hkvVec3 vNewLookAt = ComputeCenterPosition();

  // Set blended position.
  vNewLookAt = hkvMath::interpolate(m_vOldLookAt, vNewLookAt, fWeight);

  // Smoothen look-at position and base orientation.
  hkvVec3 vNewBaseOri = ComputeBaseOrientation();

  // Set blended orientation (YAW).
  float fDiffAngle = vNewBaseOri.x - m_vOldBaseOri.x;

  // Make sure angle is in between [-180, 180] degrees.
  while (fDiffAngle > 180.0f)
    fDiffAngle -= 360.0f;
  while (fDiffAngle < -180.0f)
    fDiffAngle += 360.0f;

  vNewBaseOri.x = m_vOldBaseOri.x + fDiffAngle * fWeight;

  // Set blended orientation (PITCH).
  vNewBaseOri.y = hkvMath::interpolate(m_vOldBaseOri.y, vNewBaseOri.y, fWeight);

  // Store user orientation offset separately. It will not be smoothed.
  if (!FollowFixed)
  {
    m_vOffsetOri.x += fYawStep;
    m_vOffsetOri.y -= fPitchStep;
  }

  hkvVec3 vNewOri = m_vOffsetOri + vNewBaseOri;
  vNewOri.y = hkvMath::clamp(vNewOri.y, MinimumPitch, MaximumPitch);

  // Rectify user pitch after clamping.
  m_vOffsetOri.y = vNewOri.y - vNewBaseOri.y;

  // Set final camera orientation.
  m_spCameraProxy->SetOrientation(vNewOri); 

  // Store position and base orientation for next update without taking any collisions into account.
  m_vOldLookAt = vNewLookAt;
  m_vOldBaseOri = vNewBaseOri;

  // Update distance, zoom is taken into account quadratically.
  float fCurrentZoom = sqrt(CameraDistance) + fZoomStep;
  CameraDistance = hkvMath::clamp(fCurrentZoom * fCurrentZoom, MinimalDistance, MaximalDistance);

#if defined(_VISION_WIN32) && defined(SUPPORTS_MOUSE)
  // Smooth zooming on PC.
  const float fZoomWeight = hkvMath::saturate(Vision::GetTimer()->GetTimeDifference() * 15.0f);
  m_fCurrentDistance = hkvMath::interpolate(m_fCurrentDistance, CameraDistance, fZoomWeight);
#else
  m_fCurrentDistance = CameraDistance;
#endif

  // Store local copy for collision detection.
  float fCurrentDistance = m_fCurrentDistance; 

  // Check for collision (camera with geometry).
  if (Collides)
  {
    // Set new camera position now in order to get the corresponding near plane corners.
    m_spCameraProxy->SetPosition(vNewLookAt - m_spCameraProxy->GetDirection() * fCurrentDistance); 

    fCurrentDistance = ComputeCollisionDistance(vNewLookAt, fCurrentDistance);
  }

  // Set final camera position.
  m_spCameraProxy->SetPosition(vNewLookAt - m_spCameraProxy->GetDirection() * fCurrentDistance); 

#if defined(SUPPORTS_POSTPROCESSING) && !defined(HK_ANARCHY)

  // Update Depth of Field focus
  if (DepthOfField && Vision::Renderer.GetRendererNode(0) != NULL)
  {
    VPostProcessDepthOfField* pDepthOfField = Vision::Renderer.GetRendererNode(0)->Components().GetComponentOfType<VPostProcessDepthOfField>();
    if (pDepthOfField != NULL)
    {
      float fNearDistance, fFarDistance;
      VisRenderContext_cl::GetMainRenderContext()->GetClipPlanes(fNearDistance, fFarDistance);

      // DoF starts at near clip distance.
      const hkvVec3 vCenterVector = ComputeCenterPosition() - m_spCameraProxy->GetPosition();
      const float fActualDistance = vCenterVector.dot(m_spCameraProxy->GetDirection()) - fNearDistance;
      pDepthOfField->SetParams(fActualDistance, pDepthOfField->FocusDepth, pDepthOfField->BlurRadius);
    }
  }

#endif
}

const hkvVec3 VOrbitCamera::ComputeBaseOrientation() const
{
  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(GetOwner());

  hkvVec3 vBaseOri;
  if (Follow)
  {
    vBaseOri = pOwnerEntity->GetOrientation();
  }
  else
  {
    vBaseOri = hkvVec3::ZeroVector();
  }

  vBaseOri.x += InitialYaw;
  vBaseOri.y += InitialPitch;

  vBaseOri.y = hkvMath::clamp(vBaseOri.y, MinimumPitch, MaximumPitch);

  return vBaseOri;
}

const hkvVec3 VOrbitCamera::ComputeCenterPosition() const
{
  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(GetOwner());

  // Get bounding box size of owner.
  const VDynamicMesh* pMesh = pOwnerEntity->GetMesh();
  if (pMesh != NULL)
  {
    hkvAlignedBBox bbox;
    pMesh->GetCollisionBoundingBox(bbox);

    // Some models have an invalid collision bbox
    if (!bbox.isValid())
    {
      pMesh->GetVisibilityBoundingBox(bbox);
    }

    const float fLookAtHeight = bbox.getSizeZ() * hkvMath::saturate(RelativeLookAtHeight);
    
    hkvVec3 vPos = pOwnerEntity->GetPosition();
    vPos.z += bbox.m_vMin.z + fLookAtHeight;
    return vPos;
  }
  else
  {
    return pOwnerEntity->GetPosition();
  }
}

float VOrbitCamera::ComputeCollisionDistance(const hkvVec3& vLookAtPos, float fCameraDistance) const
{
  IVisPhysicsModule_cl* pPhysicsModule = Vision::GetApplication()->GetPhysicsModule();

  VisRenderContext_cl* pMainRenderContext = VisRenderContext_cl::GetMainRenderContext();

  float fNearDistance, fFarDistance;
  pMainRenderContext->GetClipPlanes(fNearDistance, fFarDistance);

  // Compute near plane corners and place them at the look-at position (The trace has to start from there).  
  const float fNearToLookAtDistance = hkvMath::Max(0.0f, fCameraDistance - fNearDistance);
  const hkvVec3 vLookAtOffset = m_spCameraProxy->GetDirection() * fNearToLookAtDistance;
  const hkvVec3 vNearCorners[] = 
  {
    pMainRenderContext->GetWorldPosFromClipSpacePos(hkvVec3(-1.0f, -1.0f, 0.0f)),
    pMainRenderContext->GetWorldPosFromClipSpacePos(hkvVec3(1.0f, -1.0f, 0.0f)),
    pMainRenderContext->GetWorldPosFromClipSpacePos(hkvVec3(-1.0f, 1.0f, 0.0f)),
    pMainRenderContext->GetWorldPosFromClipSpacePos(hkvVec3(1.0f, 1.0f, 0.0f)),
    pMainRenderContext->GetWorldPosFromClipSpacePos(hkvVec3(0.0f, 0.0f, 0.0f))
  };

  // Perform one ray cast for each corner.
  VisPhysicsHit_t hitInfos[16];

  for (int i = 0; i < V_ARRAY_SIZE(vNearCorners); i++)
  {
    float fHitDistance = hkvMath::FloatMaxPos();

    // Use physics module for tracing lines.
    if (pPhysicsModule != NULL)
    {
      const int iNumHits = pPhysicsModule->Raycast(vNearCorners[i] + vLookAtOffset, vNearCorners[i], V_ARRAY_SIZE(hitInfos), hitInfos);
      for (int j = 0; j < iNumHits; j++)
      {
        const bool bTouchedEntity = (hitInfos[j].eHitType == VIS_TRACETYPE_ENTITYBOX || hitInfos[j].eHitType == VIS_TRACETYPE_ENTITYPOLY);
        if (!bTouchedEntity)
          fHitDistance = hitInfos[j].fHitFraction * fNearToLookAtDistance;
      }
    }
    // Use the Vision collision toolkit for tracing lines.
    else
    {
      VisTraceLineInfo_t traceLineInfo;
      if (!Vision::CollisionToolkit.TraceLine(vNearCorners[i] + vLookAtOffset, vNearCorners[i], 
        (ULONG)VIS_TRACE_STOREPRIMITIVES, VIS_TRACE_NONE, NULL, NULL, &traceLineInfo))
      {
        fHitDistance = traceLineInfo.distance;
      }
    }

    // Don't use full near clip distance since we're only tracing a ray instead of performing a sweep.
    fCameraDistance = hkvMath::Min(fCameraDistance, fHitDistance + fNearDistance * 0.9f);
  }

  return fCameraDistance;
}

void VOrbitCamera::Teleport()
{ 
  m_vOldLookAt = ComputeCenterPosition(); 
  m_vOldBaseOri = ComputeBaseOrientation();
  m_fCurrentDistance = CameraDistance;
}

START_VAR_TABLE(VOrbitCamera, IVObjectComponent, "Can be attached to entities to use them as the look-at position.", VVARIABLELIST_FLAGS_NONE, "Orbit Camera" )
  DEFINE_VAR_BOOL(VOrbitCamera, Enabled, "If enabled, camera is active when running the game", "TRUE", 0, NULL);
  DEFINE_VAR_BOOL(VOrbitCamera, Collides, "If enabled, camera collides with the geometry", "FALSE", 0, NULL);
  DEFINE_VAR_BOOL(VOrbitCamera, Follow, "If enabled, camera is aligned to the orientation of the parent entity", "FALSE", 0, NULL);
  DEFINE_VAR_BOOL(VOrbitCamera, FollowFixed, "If enabled, the camera orientation cannot be controlled by the user", "FALSE", 0, NULL);
  DEFINE_VAR_BOOL(VOrbitCamera, Zoom, "If enabled, camera can be zoomed with the mouse wheel", "TRUE", 0, NULL);
  DEFINE_VAR_BOOL(VOrbitCamera, DepthOfField, "If enabled and there is a depth of field post processor, the focus point will automatically be set to focus the parent entity.", "FALSE", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, InitialYaw, "Initial yaw angle of the camera", "0.0", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, InitialPitch, "Initial pitch angle of the camera", "30.0", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, MinimumPitch, "Minimum pitch angle", "-89.0", 0, "Clamp(-89.0, 89.0)");
  DEFINE_VAR_FLOAT(VOrbitCamera, MaximumPitch, "Minimum pitch angle", "89.0", 0, "Clamp(-89.0, 89.0)");
  DEFINE_VAR_FLOAT(VOrbitCamera, CameraDistance, "Initial distance of the camera to the parent entity", "500.0", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, MinimalDistance, "Minimum distance of the camera to the parent entity", "100.0", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, MaximalDistance, "Maximum distance of the camera to the parent entity", "1000.0", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, RelativeLookAtHeight, "Look-at height relative to the parent entity's height", "0.5", 0, "Clamp(0.0, 1.0)");
  DEFINE_VAR_FLOAT(VOrbitCamera, CameraSensitivity, "Camera orientation control sensitivity", "200.0", 0, NULL);
  DEFINE_VAR_FLOAT(VOrbitCamera, MoveSmoothness, "Camera movement smoothness when following the entity's position / orientation", "0.3", 0, "Clamp(0.0, 1e6)");
END_VAR_TABLE

// VPlayerCamera serialization fall back
V_IMPLEMENT_SERIAL(VPlayerCamera, VOrbitCamera, 0, &g_VisionEngineModule);

START_VAR_TABLE(VPlayerCamera, VOrbitCamera, "Can be attached to entities to use them as the look-at position.", VFORGE_HIDECLASS, "Player Camera")
END_VAR_TABLE

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
