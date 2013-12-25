/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file CameraPositionEntity.hpp

#ifndef CAMERAPOSITIONENTITY_HPP_INCLUDED
#define CAMERAPOSITIONENTITY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

/// \brief
///   This is the analogue to the CameraPositionShape inside vForge. It can be used to attach the camera to fixed entities set in vForge
class CameraPositionEntity : public VisBaseEntity_cl
{
public:
  inline CameraPositionEntity()
    : VisBaseEntity_cl()
    , m_fNearClipDistance(0.0f)
    , m_fFarClipDistance(0.0f)
    , m_fFovX(0.0f)
  {
  }

  ///\brief
  /// Takes the camera of the passed context and applies its own transformation and view settings to it
  ///
  /// \param pContext
  ///   The context that should receive a new camera position. When using a renderer node (e.g. deferred), pass pNode->GetReferenceContext()
  EFFECTS_IMPEXP void ApplyToContext(VisRenderContext_cl *pContext);

  ///\brief
  /// Static helper function to find a camera shape in the scene by key and then apply the position and view settings.
  ///
  /// \param pContext
  ///   The context that should receive a new camera position. When using a renderer node (e.g. deferred), pass pNode->GetReferenceContext()
  ///
  /// \param szKey
  ///   The key of the camera shape. Set as the ObjectKey string in vForge.
  EFFECTS_IMPEXP static CameraPositionEntity* ApplyToContext(VisRenderContext_cl *pContext, const char *szKey);

  ///
  /// @name Parameters
  /// @{
  ///

  ///
  /// \brief
  ///   Sets the near clip distance. Set to 0 to use the default value.
  ///
  inline void SetNearClipDistance(float fNearClipDistance)
  {
    m_fNearClipDistance = fNearClipDistance;
  }

  ///
  /// \brief
  ///   Returns the near clip distance.
  ///
  inline float GetNearClipDistance() const
  {
    return m_fNearClipDistance;
  }

  ///
  /// \brief
  ///   Sets the far clip distance. Set to 0 to use the default value.
  ///
  inline void SetFarClipDistance(float fFarClipDistance)
  {
    m_fFarClipDistance = fFarClipDistance;
  }

  ///
  /// \brief
  ///   Returns the far clip distance.
  ///
  inline float GetFarClipDistance() const
  {
    return m_fFarClipDistance;
  }

  ///
  /// \brief
  ///   Sets the horizontal field of view angle. Set to 0 to use the default value.
  ///
  inline void SetFovX(float fFovX)
  {
    m_fFovX = fFovX;
  }

  ///
  /// \brief
  ///   Returns the horizontal field of view angle.
  ///
  inline float GetFovX() const
  {
    return m_fFovX;
  }

  ///
  /// @}
  /// @name Serialization
  /// @{
  ///

  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;
  V_DECLARE_SERIAL_DLLEXP(CameraPositionEntity,  EFFECTS_IMPEXP);
  V_DECLARE_VARTABLE(CameraPositionEntity, EFFECTS_IMPEXP);

  ///
  /// @}
  ///

private:
  // Var-table members
  float m_fNearClipDistance;
  float m_fFarClipDistance;
  float m_fFovX;
};

#endif

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
