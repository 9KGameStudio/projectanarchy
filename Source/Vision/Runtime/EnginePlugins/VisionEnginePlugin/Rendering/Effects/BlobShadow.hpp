/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file BlobShadow.hpp

#ifndef BLOBSHADOW_HPP_INCLUDED
#define BLOBSHADOW_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VBlobShadowManager;
class VBlobShadow;
typedef VSmartPtr<VBlobShadow> BlobShadowPtr;

// Versions
#define VBLOB_SHADOW_VERSION_1         1                                 
#define VBLOB_SHADOW_VERSION_2         2                                 
#define VBLOB_SHADOW_VERSION_3         3 // Added fading, removed m_bNormalFalloff
#define VBLOB_SHADOW_VERSION_CURRENT   VBLOB_SHADOW_VERSION_3 

/// \brief
///   Shader class with shadow update capabilities.
class VBlobShadowShader : public VCompiledShaderPass
{
public:
  VBlobShadowShader():
    m_iShadowTexSampler(-1),
    m_iRegPlaneU(-1),
    m_iRegPlaneV(-1),
    m_iRegColor(-1),
    m_iRegFadeParams(-1)
  {
  }

  EFFECTS_IMPEXP virtual void PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader) HKV_OVERRIDE; 
  
  void UpdateShadow(const VBlobShadow *pShadow);

protected:
  int m_iShadowTexSampler;
  int m_iRegPlaneU, m_iRegPlaneV, m_iRegColor, m_iRegFadeParams;

  /// \brief
  ///   RTTI macro
  V_DECLARE_DYNCREATE_DLLEXP(VBlobShadowShader, EFFECTS_IMPEXP)

};


/// \brief
///   An instance of this class defines a single blob shadow instance. It is a component that can be attached to
///   objects of class VisObject3D_cl and VisStaticMeshInstance_cl.
class VBlobShadow : public IVObjectComponent
{
public: 
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Component constructor
  EFFECTS_IMPEXP VBlobShadow(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  EFFECTS_IMPEXP virtual ~VBlobShadow() {}

  ///
  /// @}
  ///

  ///
  /// @name Shadow Properties
  /// @{
  ///

  /// \brief
  ///   Enables/ disables the blob shadow.
  inline void SetEnabled(bool bStatus=true)
  {
    Enabled = bStatus;
  }

  /// \brief
  ///   Indicates whether the blob shadow is currently enabled.
  inline bool IsEnabled() const 
  {
    return Enabled==TRUE;
  }

  /// \brief
  ///   Sets the world space bounding box for the shadow. The bounding box is used for determining relevant primitives.
  EFFECTS_IMPEXP void SetShadowBox(const hkvAlignedBBox &bbox);

  /// \brief
  ///   Retrieves the current world space bounding box.
  inline void GetShadowBox(hkvAlignedBBox &bbox) const 
  {
    bbox = m_ShadowBox;
  }

  /// \brief
  ///   Defines radius and height of the shadow box.
  /// 
  /// \param fRadius
  ///   Radius of the shadow box.
  ///
  /// \param fHeight
  ///   Height of the shadow box.
  ///
  /// \note
  ///   Height must be larger than FadeStart.
  inline void SetSize(float fRadius, float fHeight)
  { 
    Height = fHeight;
    Radius = fRadius;
  }

  /// \brief
  ///   Gets radius and height of the shadow box.
  inline void GetSize(float &fRadius, float &fHeight) const
  {
    fRadius = Radius;
    fHeight = Height;
  }

  /// \brief
  ///   Defines the height at which fading of shadow starts. 
  /// 
  /// \param fFadeStart
  ///   Height at which fading of shadow starts.
  ///
  /// \note
  ///   FadeStart must be smaller than Height.
  inline void SetFadeStart(float fFadeStart)
  { 
    FadeStart = fFadeStart; 
  }

  /// \brief
  ///   Gets the height at which fading of shadow starts.
  inline float GetFadeStart() const
  {
    return FadeStart;
  }

  /// \brief
  ///   Defines radius and height of the shadow box if the center is provided through the owner object.
  void SetBoundingBoxFromOwnerProperties();

  /// \brief
  ///   Sets the shadow color. The default color is gray.
  /// 
  /// \param iColor
  ///   Color of the blob shadow.
  ///
  /// \note
  ///   The alpha value is used to control the shadow intensity.
  inline void SetColor(VColorRef iColor) 
  {
    ShadowColor = iColor;
    m_vBlendColor = iColor.ToFloat4();

    // pre-multiply color with alpha
    m_vBlendColor.x = (1.0f-m_vBlendColor.x)*m_vBlendColor.w;
    m_vBlendColor.y = (1.0f-m_vBlendColor.y)*m_vBlendColor.w;
    m_vBlendColor.z = (1.0f-m_vBlendColor.z)*m_vBlendColor.w;
  }

  /// \brief
  ///   Returns the current shadow color. The default color is gray.
  inline VColorRef GetColor() const 
  {
    return ShadowColor;
  }

  /// \brief
  ///   Sets a custom shadow texture. Can be set to NULL to use the default texture.
  inline void SetShadowTexture(VTextureObject *pTex) 
  {
    m_spShadowTex = pTex;
  }

  /// \brief
  ///   Loads a custom texture by filename and sets it
  inline void SetShadowTexture(const char *szFilename) 
  {
    SetShadowTexture(Vision::TextureManager.Load2DTexture(szFilename));
  }

  /// \brief
  ///   Returns the custom shadow texture. Returns NULL if no custom texture is set.
  inline VTextureObject* GetShadowTexture() const 
  {
    return m_spShadowTex;
  }

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes.
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl and VisStaticMeshInstance_cl instances.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function to respond to variable changes.
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VBlobShadow, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table.
  V_DECLARE_VARTABLE(VBlobShadow, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar);

  ///
  /// @}
  ///

public:  
  BOOL Enabled;
  hkvAlignedBBox m_ShadowBox;
  VColorRef ShadowColor;
  hkvVec4 m_vBlendColor;
  VTextureObjectPtr m_spShadowTex;
  float Radius, Height, FadeStart;

};


/// \brief
///   Collection for handling shadow instances.
class VBlobShadowCollection : public VRefCountedCollection<VBlobShadow>
{
};

#endif

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
