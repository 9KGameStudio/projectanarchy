/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadow.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadowManager.hpp>


////////////////////////////////////////////////////////////////////////////////////////////////
// overridden function to extract the shader registers from the compiled version
void VBlobShadowShader::PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader)
{
  VCompiledShaderPass::PostCompileFunction(pSourceFX, pSourceShader);

  m_iRegPlaneU = GetConstantBuffer(VSS_VertexShader)->GetRegisterByName("TexRefPlaneU");
  m_iRegPlaneV = GetConstantBuffer(VSS_VertexShader)->GetRegisterByName("TexRefPlaneV");
  VASSERT(m_iRegPlaneU>=0 && m_iRegPlaneV>=0);

  m_iRegColor = GetConstantBuffer(VSS_PixelShader)->GetRegisterByName("ShadowColor");
  VASSERT(m_iRegColor>=0);

  m_iRegFadeParams = GetConstantBuffer(VSS_VertexShader)->GetRegisterByName("FadeParams");
  VASSERT(m_iRegFadeParams>=0);

  m_iShadowTexSampler = GetSamplerIndexByName(VSS_PixelShader, "ShadowTexture");
}

void VBlobShadowShader::UpdateShadow(const VBlobShadow *pShadow)
{
  if (m_iShadowTexSampler>=0)
  {
    VStateGroupTexture *pStateGroupTexture = GetStateGroupTexture(VSS_PixelShader, m_iShadowTexSampler);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = pShadow->GetShadowTexture();
  }

  // update the uv projection planes
  const float fSizeX = pShadow->m_ShadowBox.getSizeX();
  const float fSizeY = pShadow->m_ShadowBox.getSizeY();

  hkvPlane planeU;
  planeU.m_vNormal = hkvVec3(1.f/fSizeX, 0, 0);  
  planeU.m_fNegDist = -planeU.m_vNormal.dot (pShadow->m_ShadowBox.m_vMin);

  hkvPlane planeV;
  planeV.m_vNormal = hkvVec3(0, 1.f/fSizeY, 0);  
  planeV.m_fNegDist = -planeV.m_vNormal.dot (pShadow->m_ShadowBox.m_vMin);

  GetConstantBuffer(VSS_VertexShader)->SetSingleRegisterF(m_iRegPlaneU, &planeU.m_vNormal.x);
  GetConstantBuffer(VSS_VertexShader)->SetSingleRegisterF(m_iRegPlaneV, &planeV.m_vNormal.x);

  // fade parameters
  float fMaxFadeStart = pShadow->Height-HKVMATH_HUGE_EPSILON;
  float fClampedFadeStart = hkvMath::Min(pShadow->FadeStart, fMaxFadeStart); // FadeStart must be smaller than Height
  float fPivot = pShadow->m_ShadowBox.m_vMax.z-pShadow->Radius-fClampedFadeStart; // offset pivot with FadeStart
  float fHeight = pShadow->Height-fClampedFadeStart; // adjust Height with FadeStart
  GetConstantBuffer(VSS_VertexShader)->SetSingleRegisterF(m_iRegFadeParams, fPivot, fHeight, 0.0f, 0.0f);

  // shadow color 
  GetConstantBuffer(VSS_PixelShader)->SetSingleRegisterF(m_iRegColor, pShadow->m_vBlendColor.data);

  m_bModified = true;
}


// register the shader class in the engine module
V_IMPLEMENT_DYNCREATE(VBlobShadowShader, VCompiledShaderPass, &g_VisionEngineModule);

/////////////////////////////////////////////////////////////////////////////
// Simple shadow functions
/////////////////////////////////////////////////////////////////////////////
VBlobShadow::VBlobShadow(int iComponentFlags) : 
  IVObjectComponent(0, iComponentFlags),
  Enabled(TRUE), 
  Radius(0.0f),
  Height(0.0f),
  FadeStart(0.0f)
{
  SetColor(V_RGBA_GREY);
}

void VBlobShadow::SetShadowBox(const hkvAlignedBBox &bbox) 
{
  m_ShadowBox = bbox; 

  // In case no Height/ Radius was explicitly specified by user, these values have to be derived from the 
  // specified bounding box. Otherwise the fading will not work correctly.
  float fExtentX = m_ShadowBox.getSizeX();
  float fExtentZ = m_ShadowBox.getSizeZ();
  Radius = hkvMath::Max(fExtentX, fExtentZ)*0.5f;
  Height = m_ShadowBox.getSizeZ()-Radius;
  Height = hkvMath::Max(Height, HKVMATH_HUGE_EPSILON);
}

void VBlobShadow::SetBoundingBoxFromOwnerProperties()
{
  const VisTypedEngineObject_cl *pTypedEngineObject = GetOwner();
  VASSERT(pTypedEngineObject != NULL);

  if(pTypedEngineObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
  {
    const VisObject3D_cl *pOwner = vstatic_cast<const VisObject3D_cl*>(GetOwner());
    pOwner->GetPosition((hkvVec3&) m_ShadowBox.m_vMin);
  }
  else if(pTypedEngineObject->IsOfType(V_RUNTIME_CLASS(VisStaticMeshInstance_cl)))
  {
    const VisStaticMeshInstance_cl *pOwner = vstatic_cast<const VisStaticMeshInstance_cl*>(GetOwner());
    m_ShadowBox.m_vMin = pOwner->GetTransform().getTranslation();
  }

  // build absolute bounding box
  m_ShadowBox.m_vMax = m_ShadowBox.m_vMin;
  m_ShadowBox.addBoundary(hkvVec3(Radius));
  m_ShadowBox.m_vMin.z -= Height-Radius;
}

void VBlobShadow::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    if (m_spShadowTex==NULL)
      m_spShadowTex = VBlobShadowManager::GlobalManager().GetDefaultShadowTexture();
    VBlobShadowManager::GlobalManager().Instances().AddUnique(this);
  }
  else
  {
    m_spShadowTex = NULL;
    VBlobShadowManager::GlobalManager().Instances().SafeRemove(this);
  }
}

BOOL VBlobShadow::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if ((!pObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl))) && (!pObject->IsOfType(V_RUNTIME_CLASS(VisStaticMeshInstance_cl))))
  {
    sErrorMsgOut = "Component can only be added to instances of VisObject3D_cl and VisStaticMeshInstance_cl or derived classes.";
    return FALSE;
  }
  
  return TRUE;
}

void VBlobShadow::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  SetColor(ShadowColor); // sets the float dependency

  if (!strcmp(pVar->name, "TextureFilename"))
  {
    m_spShadowTex = (value && value[0]) ? Vision::TextureManager.Load2DTexture(value) : VBlobShadowManager::GlobalManager().GetDefaultShadowTexture();
  }
}

V_IMPLEMENT_SERIAL(VBlobShadow,IVObjectComponent, 0, &g_VisionEngineModule);
void VBlobShadow::Serialize(VArchive &ar)
{
  char iLocalVersion = VBLOB_SHADOW_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG((iLocalVersion >= VBLOB_SHADOW_VERSION_2) && (iLocalVersion <= VBLOB_SHADOW_VERSION_CURRENT), "Invalid local version. Please re-export");

    ar >> Enabled;
    
    if (iLocalVersion < VBLOB_SHADOW_VERSION_3)
    {
      bool m_bNormalFalloff;
      ar >> m_bNormalFalloff;
    }

    m_ShadowBox.SerializeAs_VisBoundingBox(ar);
    ar >> Radius >> Height;
    ar >> ShadowColor >> m_vBlendColor;
    m_spShadowTex = (VTextureObject *)ar.ReadProxyObject();

    if (iLocalVersion >= VBLOB_SHADOW_VERSION_3)
    {
      ar >> FadeStart;
    }
  } 
  else
  {
    ar << iLocalVersion;
    ar << Enabled;
    m_ShadowBox.SerializeAs_VisBoundingBox (ar);
    ar << Radius << Height;
    ar << ShadowColor << m_vBlendColor;
    ar.WriteProxyObject(m_spShadowTex);
    ar << FadeStart;
  }
}

START_VAR_TABLE(VBlobShadow,IVObjectComponent,"Blob shadow component",VVARIABLELIST_FLAGS_NONE, "Blob Shadow" )
  DEFINE_VAR_COLORREF(VBlobShadow, ShadowColor, "Color of the shadow", "127,127,127,255", 0, NULL);
  DEFINE_VAR_FLOAT(VBlobShadow, Radius, "Radius of the blob shadow", "80.0", 0, "Min(0.0)");
  DEFINE_VAR_FLOAT(VBlobShadow, Height, "Max. height of the pivot over ground", "200.0", 0, "Min(0.0)");
  DEFINE_VAR_FLOAT(VBlobShadow, FadeStart, "Pivot height at which fading of shadow starts (must be smaller than Height)", "0.0", 0, "Min(0.0)");
  DEFINE_VAR_BOOL(VBlobShadow, Enabled, "En-/Disable component", "TRUE", 0, NULL);
  DEFINE_VAR_STRING_CALLBACK(VBlobShadow, TextureFilename, "Filename of the texture", "\\Textures\\blobShadow.dds", DISPLAY_HINT_TEXTUREFILE, NULL);
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20140717)
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
