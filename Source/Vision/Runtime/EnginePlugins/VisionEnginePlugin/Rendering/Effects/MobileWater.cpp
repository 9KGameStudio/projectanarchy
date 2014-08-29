/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/MobileWater.hpp>

enum MOBILE_WATER_VERSION
{
  MOBILE_WATER_VERSION_1 = 1,
  MOBILE_WATER_VERSION_2 = 2,
  MOBILE_WATER_VERSION_CURRENT = MOBILE_WATER_VERSION_2,
};

VMobileWater::VMobileWater(Deserialize_e deserialize) :
  m_gridCellSize(10.0f, 10.0f),
  m_bUseRadialGrid(false),
  m_bUseStaticLighting(true),
  m_textureTiling(100.0f, 100.0f),
  m_size(100.0f, 100.0f),
  m_eRenderHook(VRH_PRE_TRANSPARENT_PASS_ENTITIES)
{
  if(deserialize == DeserializeNo)
  {
    CommonInit();
    SetVisibilityBitMask(VIS_ENTITY_VISIBLE);
  }  
}

void VMobileWater::CommonInit()
{
  VMobileWaterManager::GlobalManager().Add(this);
  m_localPlaneCorners[0].set(-0.5f, -0.5f, 0.0f );
  m_localPlaneCorners[1].set( 0.5f, -0.5f, 0.0f );
  m_localPlaneCorners[2].set( 0.5f,  0.5f, 0.0f );
  m_localPlaneCorners[3].set(-0.5f,  0.5f, 0.0f );

  CreateMesh();
}

VMobileWater::~VMobileWater()
{
}

VTypedObject* VMobileWater::CreateObject()
{
  return new VMobileWater(DeserializeYes);
}

V_IMPLEMENT_RUNTIMECLASS( VMobileWater, VisObject3D_cl, 0, VMobileWater::CreateObject, &g_VisionEngineModule );

void VMobileWater::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);

  if(ar.IsLoading())
  {
    int version;
    ar >> version;
    VASSERT(version > 0 && version <= MOBILE_WATER_VERSION_CURRENT);

    int m_iGridSubdivisionsX, m_iGridSubdivisionsY;
    if(version == MOBILE_WATER_VERSION_1)
    {
      ar >> m_iGridSubdivisionsX;
      ar >> m_iGridSubdivisionsY;
    }
    else
    {
      ar >> m_gridCellSize;
    }

    ar >> m_bUseRadialGrid;
    ar >> m_size;
    ar >> *reinterpret_cast<unsigned int*>(&m_eRenderHook);
    ar >> m_EffectConfig;
    ar >> m_uiVisibilityBitMask;
    ar >> m_textureTiling;
    ar >> m_bUseStaticLighting;

    if(version == MOBILE_WATER_VERSION_1)
    {
      m_gridCellSize = hkvVec2(m_size.x / (float)m_iGridSubdivisionsX, m_size.y / (float)m_iGridSubdivisionsY);
      // Texture tiling was in object space now its in world space
      m_textureTiling = m_size.compDiv(m_textureTiling);
    }
  }
  else
  {
    ar << static_cast<int>(MOBILE_WATER_VERSION_CURRENT);
    ar << m_gridCellSize;
    ar << m_bUseRadialGrid;
    ar << m_size;
    ar << static_cast<unsigned int>(m_eRenderHook);
    ar << m_EffectConfig;
    ar << m_uiVisibilityBitMask;
    ar << m_textureTiling;
    ar << m_bUseStaticLighting;
  }
}

void VMobileWater::OnDeserializationCallback(const VSerializationContext &context)
{
  CommonInit();
  SetEffect(m_EffectConfig.GetEffect());
  UpdateMeshTransformation();
  SetVisibilityBitMask(m_uiVisibilityBitMask);
}

VBool VMobileWater::WantsDeserializationCallback(const VSerializationContext &context)
{
  return TRUE;
}

void VMobileWater::CreateMesh()
{
  VisMeshBuffer_cl *pMesh = NULL;
  hkvVec2 numGridSubdivisions = m_size.compDiv(m_gridCellSize);
  int iNumGridSubdivisions[2] = { hkvMath::Max(static_cast<int>(hkvMath::round(numGridSubdivisions.x)), 1),
                                  hkvMath::Max(static_cast<int>(hkvMath::round(numGridSubdivisions.y)), (m_bUseRadialGrid) ? 3 : 1) };
  // ensure that the indices will fit into an 16-bit index buffer (mobile limitation)
  if((iNumGridSubdivisions[0] + 1) * (iNumGridSubdivisions[1] + 1) > 65535)
  {
    float fAspectRatio = m_gridCellSize.x / m_gridCellSize.y;
    float newSubdivX = hkvMath::sqrt(fAspectRatio * 65535.0f);
    float newSubdivY = newSubdivX / fAspectRatio;
    iNumGridSubdivisions[0] = static_cast<int>(hkvMath::floor(newSubdivX) - 1.0f);
    iNumGridSubdivisions[1] = static_cast<int>(hkvMath::floor(newSubdivY) - 1.0f);
    VASSERT((iNumGridSubdivisions[0] + 1) * (iNumGridSubdivisions[1] + 1) <= 65535);
  }
  hkvVec2 textureTiling = m_size.compDiv(m_textureTiling);
  if(m_bUseStaticLighting)
  {
    VWaterPlaneGeneratorLightgrid generator(GetPosition(), GetRotationMatrix(), 
      hkvVec3(m_size.x, m_size.y, 1.0f), GetRelevantLightGrid());
    pMesh = generator.Generate(m_bUseRadialGrid, iNumGridSubdivisions, m_localPlaneCorners, textureTiling);
  }
  else
  {
    VWaterPlaneGenerator generator;
    pMesh = generator.Generate<VWaterPlaneGenerator::VertexLayout>(m_bUseRadialGrid, iNumGridSubdivisions, m_localPlaneCorners, textureTiling);
  }

#ifdef HK_DEBUG
  pMesh->SetFilename("<MobileWaterMesh>");
#endif
  m_spMeshBufferObject = new VisMeshBufferObject_cl(pMesh);
  m_spMeshBufferObject->SetVisible(true);
  m_spMeshBufferObject->SetObjectFlag(VObjectFlag_AutoDispose);
}

void VMobileWater::ReInitMeshBuffer()
{
  CreateMesh();
  UpdateMeshTransformation();
  SetEffect(m_EffectConfig.GetEffect());
}

void VMobileWater::UpdateStaticLighting()
{
  if( m_spMeshBufferObject && m_bUseStaticLighting )
  {
    VWaterPlaneGeneratorLightgrid generator(GetPosition(), GetRotationMatrix(), hkvVec3(m_size.x, m_size.y, 1.0f), 
      GetRelevantLightGrid(), sizeof(VWaterPlaneGeneratorLightgrid::VertexLayoutLightgrid));
    VisMeshBuffer_cl* pMesh = m_spMeshBufferObject->GetCurrentMeshBuffer();
    generator.FillCustomData(pMesh->LockVertices(0), pMesh->GetVertexCount());
    pMesh->UnLockVertices();
  }
}

void VMobileWater::SetTechnique(VCompiledTechnique *pTechnique)
{
  m_spWaterTechnique = pTechnique;
  if(m_spMeshBufferObject)
    m_spMeshBufferObject->SetTechnique(pTechnique);
}

void VMobileWater::SetEffect(VCompiledEffect *pEffect)
{
  m_EffectConfig.SetEffect(pEffect);
  if(pEffect)
  {
    VTechniqueConfig config;
    VTechniqueConfig* configs = &config;
    pEffect->GetSourceEffect()->AddEffectParameterInclusionTags(&configs, 1, pEffect->GetParameterString());
    if(m_bUseStaticLighting)
    {
      config.AddInclusionTag("USE_STATIC_LIGHTING");
    }
    if(Vision::World.GetFogParameters().depthMode != VFogParameters::Off)
    {
      config.AddInclusionTag("DEPTH_FOG");
    }
    VCompiledTechnique* pTechnique = pEffect->FindCompatibleTechnique(&config);
    SetTechnique(pTechnique);
  }
  else
  {
    SetTechnique(NULL);
  }
}

VCompiledEffect* VMobileWater::GetEffect()
{
  return m_EffectConfig.GetEffect();
}

void VMobileWater::GetWorldSpaceVertices(hkvVec3 (&vertices)[4]) const
{ 
  for (int i=0; i < V_ARRAY_SIZE(vertices); i++)
  {
    vertices[i] = m_localPlaneCorners[i];
    vertices[i].x *= m_size.x;
    vertices[i].y *= m_size.y;

    vertices[i] = m_cachedRotMatrix * vertices[i];
    vertices[i] += m_vPosition;
  }
}

float VMobileWater::TracePlane(const hkvVec3& vStart, const hkvVec3& vEnd, bool bDoubleSided) const
{
  // Make a quad out of two triangles and trace it
  hkvVec3 vWorldPos[4];
  GetWorldSpaceVertices(vWorldPos);
  VTriangle tri;

  hkvVec3 vDiff = vEnd - vStart;
  hkvVec3 vDir = vDiff;
  float fDist;
  tri.SetPoints((hkvVec3* )&vWorldPos[2],(hkvVec3* )&vWorldPos[1],(hkvVec3* )&vWorldPos[0]);
  if (tri.GetTraceIntersection((hkvVec3& )vStart,vDir,bDoubleSided, fDist))
    return fDist*vDir.getLength();

  tri.SetPoints((hkvVec3* )&vWorldPos[3],(hkvVec3* )&vWorldPos[2],(hkvVec3* )&vWorldPos[0]);
  if (tri.GetTraceIntersection((hkvVec3& )vStart,vDir,bDoubleSided, fDist))
    return fDist*vDir.getLength();

  return -1.f;
}

void VMobileWater::OnObject3DChanged(int iO3DFlags)
{
  VisObject3D_cl::OnObject3DChanged(iO3DFlags);
  UpdateMeshTransformation();
}

void VMobileWater::UpdateMeshTransformation()
{
  const hkvMat3 &rotMat(m_cachedRotMatrix);

  hkvVec3 corners[4];
  memcpy(corners, m_localPlaneCorners, sizeof(corners));

  hkvAlignedBBox aabb;

  if (m_spMeshBufferObject)
  {
    // Update the position
    m_spMeshBufferObject->SetPosition(m_vPosition);

    // Compute the rotation & scale matrix
    hkvMat3 mat;

    mat.set(m_size.x, 0       , 0, 
            0       , m_size.y, 0, 
            0       , 0       , 1);

    mat = rotMat.multiply (mat);
    m_spMeshBufferObject->SetRotationMatrix(mat);

    // Compute the new bounding box
    for(int i=0; i< V_ARRAY_SIZE(corners); i++)
    {
      corners[i] = mat.transformDirection(corners[i]);
    }
    aabb.setFromPoints(corners, V_ARRAY_SIZE(corners));
    aabb.addBoundary(hkvVec3(Vision::World.GetGlobalUnitScaling()));
    VASSERT(aabb.isValid());
    m_spMeshBufferObject->SetVisibilityBoundingBox(&aabb);

    // Compute the new render order
    int iSubOrder = (int)(GetPosition().z * 100.f); // Sort bottom to top
    m_spMeshBufferObject->SetOrder(m_eRenderHook, -iSubOrder);
  }
}

void VMobileWater::SetRenderHook(VRenderHook_e eRenderHook)
{
  if(m_eRenderHook != eRenderHook)
  {
    m_eRenderHook = eRenderHook;
    if(m_spMeshBufferObject)
      m_spMeshBufferObject->SetOrder(eRenderHook, m_spMeshBufferObject->GetSubOrder());
  }
}

void VMobileWater::SetVisibilityBitMask(unsigned int uiVisibilityBitMask)
{
  m_uiVisibilityBitMask = uiVisibilityBitMask;
  if(m_spMeshBufferObject->GetVisibilityObject())
    m_spMeshBufferObject->GetVisibilityObject()->SetVisibleBitmask(uiVisibilityBitMask);
}

void VMobileWater::SetUseStaticLighting(bool bValue)
{
  if(bValue != m_bUseStaticLighting)
  {
    m_bUseStaticLighting = bValue;
    ReInitMeshBuffer();
  }
}

void VMobileWater::DisposeObject()
{
  if (IsObjectFlagSet(VObjectFlag_Disposing))
    return;
  m_spMeshBufferObject = NULL;
  m_spWaterTechnique = NULL;
  m_EffectConfig.Clear();
  VisTypedEngineObject_cl::DisposeObject();
  VMobileWaterManager::GlobalManager().Remove(this);
}

const hkvAlignedBBox& VMobileWater::GetBoundingBox()
{
  m_BoundingBox.setInvalid();
  hkvVec3 vWorldPos[4];
  GetWorldSpaceVertices(vWorldPos);
  for (int i=0; i < V_ARRAY_SIZE(vWorldPos); i++)
    m_BoundingBox.expandToInclude(vWorldPos[i]);
  return m_BoundingBox;
}

void VMobileWater::SetUseRadialGrid(bool bValue)
{
  if(bValue != m_bUseRadialGrid)
  {
    m_bUseRadialGrid = bValue;
    ReInitMeshBuffer();
  }
}

void VMobileWater::SetSize(hkvVec2 size)
{ 
  if(m_size != size)
  {
    m_size = size; 
    ReInitMeshBuffer();
  }
}

void VMobileWater::SetGridCellSize(float x, float y)
{
  VASSERT(x >= 0 && y >= 0);
  if(m_gridCellSize.x != x || m_gridCellSize.y != y)
  {
    m_gridCellSize.set(x, y);

    ReInitMeshBuffer();
  }
}

void VMobileWater::SetTextureTiling(hkvVec2 tiling)
{
  if(m_textureTiling != tiling)
  {
    m_textureTiling = tiling;

    ReInitMeshBuffer();
  }
}

void VMobileWater::ReassignEffect()
{  
  m_EffectConfig.ReAssignEffect();
  SetEffect(m_EffectConfig.GetEffect()); // Re-apply the technique
}

///////////////////////////////////////////////////////////////////////////////////
/// VMobileWaterManager
///////////////////////////////////////////////////////////////////////////////////

VMobileWaterManager VMobileWaterManager::g_MobileWaterManager;

void VMobileWaterManager::OneTimeInit()
{
  Vision::Callbacks.OnBeforeSceneUnloaded += this;
  Vision::Callbacks.OnReassignShaders += this;
  Vision::Callbacks.OnGlobalRenderSettingsChanged += this;
  Vision::Callbacks.OnRendererNodeChanged += this;
}

void VMobileWaterManager::OneTimeDeInit()
{
  Vision::Callbacks.OnBeforeSceneUnloaded -= this;
  Vision::Callbacks.OnReassignShaders -= this;
  Vision::Callbacks.OnGlobalRenderSettingsChanged -= this;
  Vision::Callbacks.OnRendererNodeChanged -= this;
}

void VMobileWaterManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{  
  if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneUnloaded)
  {
    const int iCount = m_Instances.Count();
    for (int i=iCount - 1; i>=0; --i) // Backwards to keep collection intact
    {
      VMobileWater *pWater = m_Instances.GetAt(i);
      pWater->DisposeObject();
    }
    m_Instances.Clear();
    return;
  }

  // We only need to respond to this callback outside the editor, because vForge has its own re-assignment callback.
  // also note that Vision::Callbacks.OnReassignShaders is not triggered during runtime
  if ((pData->m_pSender == &Vision::Callbacks.OnGlobalRenderSettingsChanged && 
        (static_cast<VisGlobalRendererSettingsDataObject_cl*>(pData)->m_eChangedFlags & VGRP_FOG_TOGGLE) != 0 ) || 
      (pData->m_pSender == &Vision::Callbacks.OnReassignShaders && !Vision::Editor.IsInEditor()))
  {
    const int iCount = m_Instances.Count();
    for (int i=0; i<iCount; ++i)
    {
      m_Instances.GetAt(i)->ReassignEffect();
    }
    return;
  }

  if(pData->m_pSender == &Vision::Callbacks.OnRendererNodeChanged)
  {
    const int iCount = m_Instances.Count();
    for (int i=0; i<iCount; ++i)
    {
      m_Instances.GetAt(i)->UpdateStaticLighting();
    }
    return;
  }
}

void VMobileWaterManager::Add(VMobileWater* pElement)
{
  m_Instances.Add(pElement);
}

void VMobileWaterManager::Remove(VMobileWater* pElement)
{
  m_Instances.Remove(pElement);
}

VMobileWaterManager& VMobileWaterManager::GlobalManager()
{
  return g_MobileWaterManager;
}

///////////////////////////////////////////////////////////////////////////////////
/// VMobileWaterShader
///////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_DYNCREATE(VMobileWaterShader, VCompiledShaderPass, &g_VisionEngineModule)

namespace
{
  void InitShaderRegisterHelper(VCompiledShaderPass* pPass, VConstantBufferRegister& reg, const char* constantName)
  {
    if(!reg.Init(pPass, constantName))
    {
      hkvLog::Warning("Couldn't find shader constant '%s' in shader %s", constantName, pPass->GetSourceResource()->GetName().AsChar());
    }
  }
}

VCallbackRetVal_e VISION_FASTCALL UpdateMobileWaterShaderProperties(VCallbackGeometryType_e sendertype, void *pElement, VCompiledShaderPass *shader)
{
  VMobileWaterShader *pShader = vstatic_cast<VMobileWaterShader*>(shader);

  // only update the shader once per frame & renderer node
  IVRendererNode *pRendererNode = Vision::Renderer.GetCurrentRendererNode();
  if(pRendererNode == NULL)
    return CALLBACK_DONTCALL; //prevent rendering if there is no renderer node
  if (pShader->NeedsUpdate(pRendererNode))
  {
    pShader->UpdateConstants(pRendererNode);
  }
  return CALLBACK_CONTINUE;
}

bool VMobileWaterShader::NeedsUpdate(IVRendererNode* pRendererNode) const
{
#if defined(_VR_GLES2)
  return true;
#else
  return m_uiLastUpdate != VisRenderContext_cl::GetGlobalTickCount() || pRendererNode != m_pLastRendererNode;
#endif
}

void VMobileWaterShader::UpdateConstants(IVRendererNode* pRendererNode)
{
  m_uiLastUpdate = VisRenderContext_cl::GetGlobalTickCount();
  m_pLastRendererNode = pRendererNode;

  hkvVec3 vTopLeft, vBottomLeft, vRightDir, vDownDir;
  VisRenderLoopHelper_cl::ComputeFrustumFarCorners (pRendererNode, &vTopLeft, &vBottomLeft, NULL, NULL, &vRightDir, &vDownDir);

#if defined(_VR_GLES2)
  // Flip down vector if not rendering to the back buffer.
  if (!VisRenderContext_cl::GetCurrentContext()->RendersIntoBackBuffer())
  {
    vDownDir *= -1.0f;
    vTopLeft = vBottomLeft;
  }
#endif
  
  hkvVec4 v4TopLeft(vTopLeft.getAsVec4(1.0f));
  hkvVec4 v4RightDir(vRightDir.getAsVec4(1.0f));
  hkvVec4 v4DownDir(vDownDir.getAsVec4(1.0f));
  m_regDown.SetRegisterValueSafeF(this, v4DownDir.data);
  m_regRight.SetRegisterValueSafeF(this, v4RightDir.data);
  m_regUpperLeftCorner.SetRegisterValueSafeF(this, v4TopLeft.data);
}

void VMobileWaterShader::PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader)
{
  VCompiledShaderPass::PostCompileFunction(pSourceFX, pSourceShader);

  InitShaderRegisterHelper(this, m_regDown, "DownDir");
  InitShaderRegisterHelper(this, m_regRight, "RightDir");
  InitShaderRegisterHelper(this, m_regUpperLeftCorner, "UpperLeftCorner");

  m_uiLastUpdate = 0;
  m_pCallback = &UpdateMobileWaterShaderProperties;
}

namespace {
  /// \brief iterator for indices of a vertex buffer
  struct VIndexIterator
  {
  public:
    VIndexIterator(void* pMem, VIndexFormat indexFormat)
    {
      m_pCur = (char*)pMem;
      m_uiStride = (indexFormat == VIS_INDEXFORMAT_32) ? 4 : 2;
    }

    void operator++(int dummy) // postfix operator
    {
      m_pCur += m_uiStride;
    }

    void operator = (unsigned int uiValue)
    {
      VASSERT_MSG(m_uiStride == 4 || uiValue <= 0xFFFF, "index does not fit into 16-bit");
      if(m_uiStride == 2)
      {
        *((unsigned short*)m_pCur) = uiValue;
      }
      else
      {
        *((unsigned int*)m_pCur) = uiValue;
      }
    }

  private:
    char* m_pCur;
    size_t m_uiStride;
  };
}

VisMeshBuffer_cl* VWaterPlaneGenerator::DoGenerate(bool bUseRadialGrid, const int (&iNumGridSubdivisions)[2], const hkvVec3 (&planeCorners)[4], hkvVec2 numTextureTiles)
{
  VASSERT_MSG(m_uiStride > 0, "SetLayout has to be called first");
  VisMeshBuffer_cl *pMesh = new VisMeshBuffer_cl();

  VisMBVertexDescriptor_t desc;
  VASSERT(m_uiStride < 0xFFFF);
  desc.m_iStride = (short)m_uiStride;
  desc.m_iPosOfs = offsetof(VertexLayout, pos);
  desc.m_iTexCoordOfs[0] = offsetof(VertexLayout, tex) | VERTEXDESC_FORMAT_FLOAT2;
  desc.m_iNormalOfs = offsetof(VertexLayout, normal);
  FillVertexDescriptor(desc);
  desc.ComputeHash();

  int iNumTotalVertices = (iNumGridSubdivisions[0] + 1) * (iNumGridSubdivisions[1] + 1);
  pMesh->AllocateVertices(desc, iNumTotalVertices);
  void* pVertexData = pMesh->LockVertices(0);
  VertexLayout *pVert = (VertexLayout *)pVertexData;

  hkvVec3 vDiagonal =  planeCorners[3] - planeCorners[1];
  hkvVec3 vStartVertex = planeCorners[1];

  if (bUseRadialGrid)
  {
    float fSubdivFrac[2] = { 1.0f/(float)iNumGridSubdivisions[0], 1.0f/(float)iNumGridSubdivisions[1] };

    for (int y = iNumGridSubdivisions[1] - 1; y >= 0; y--)
    {
      float fUnitPos[2];
      fUnitPos[0] = hkvMath::sinRad ((float)y * fSubdivFrac[1] * hkvMath::pi () * 2.0f);
      fUnitPos[1] = hkvMath::cosRad ((float)y * fSubdivFrac[1] * hkvMath::pi () * 2.0f);

      for (int x = 0; x <= iNumGridSubdivisions[0]; x++)
      {
        float fDistance = hkvMath::pow ((float)x * fSubdivFrac[0], 4.0f) * 0.5f;

        pVert->pos[0] = fUnitPos[0] * fDistance;
        pVert->pos[1] = fUnitPos[1] * fDistance;
        pVert->pos[2] = 0.0f;
        pVert->tex[0] = (pVert->pos[0] - vStartVertex.x) / vDiagonal.x * numTextureTiles.x;
        pVert->tex[1] = (pVert->pos[1] - vStartVertex.y) / vDiagonal.y * numTextureTiles.y;
        pVert->normal[0] = 0.0f;
        pVert->normal[1] = 0.0f;
        pVert->normal[2] = 1.0f;

        // Advance to the next vertex
        pVert = (VertexLayout*)(((char*)pVert) + m_uiStride);
      }
    }
  }
  else
  {
    hkvVec3 vDiffU = (planeCorners[2] - planeCorners[1]) * (1.0f / (float)iNumGridSubdivisions[0]);
    hkvVec3 vDiffV = (planeCorners[0] - planeCorners[1]) * (1.0f / (float)iNumGridSubdivisions[1]);

    for (int y = 0; y <= iNumGridSubdivisions[1]; y++)
    {
      for (int x = 0; x <= iNumGridSubdivisions[0]; x++)
      {
        hkvVec3 vPos = vStartVertex + vDiffU * (float)x + vDiffV * (float)y;

        pVert->pos[0] = vPos.x;
        pVert->pos[1] = vPos.y;
        pVert->pos[2] = vPos.z;
        pVert->tex[0] = (vPos.x - vStartVertex.x) / vDiagonal.x * numTextureTiles.x;
        pVert->tex[1] = (vPos.y - vStartVertex.y) / vDiagonal.y * numTextureTiles.y;
        pVert->normal[0] = 0.0f;
        pVert->normal[1] = 0.0f;
        pVert->normal[2] = 1.0f;

        // Advance to the next vertex
        pVert = (VertexLayout*)(((char*)pVert) + m_uiStride);
      }
    }
  }

  FillCustomData(pVertexData, iNumTotalVertices);

  pMesh->UnLockVertices();

  pMesh->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
  int iNumTotalIndices = iNumGridSubdivisions[0] * iNumGridSubdivisions[1] * 2 * 3;

  VIndexFormat indexFormat = (iNumTotalVertices > 0xFFFF) ? VIS_INDEXFORMAT_32 : VIS_INDEXFORMAT_16;
  pMesh->AllocateIndexList(iNumTotalIndices, VIS_MEMUSAGE_STATIC, indexFormat);

  VIndexIterator indexIt(pMesh->LockIndices(0), indexFormat);

  int maxY = iNumGridSubdivisions[1];
  if(bUseRadialGrid)
    maxY--; // When using a radial grid we leave out the last row to close the circle later

  for (int y = 0; y < maxY; y++)
  {
    for (int x = 0; x < iNumGridSubdivisions[0]; x++)
    {
      int iStartVertex = y * (iNumGridSubdivisions[0] + 1) + x;
      // First triangle
      indexIt = iStartVertex; indexIt++;
      indexIt = iStartVertex + 1; indexIt++;
      indexIt = iStartVertex + (iNumGridSubdivisions[0] + 1); indexIt++;     

      // Second triangle
      indexIt = iStartVertex + 1; indexIt++;
      indexIt = iStartVertex + (iNumGridSubdivisions[0] + 1) + 1; indexIt++;
      indexIt = iStartVertex + (iNumGridSubdivisions[0] + 1); indexIt++;

    }
  }

  if (bUseRadialGrid)
  {
    int y = iNumGridSubdivisions[1]-1;
    for (int x = 0; x < iNumGridSubdivisions[0]; x++)
    {
      int iStartVertex = y * (iNumGridSubdivisions[0] + 1) + x;
      int iWrapVertex = x;
      // First triangle
      indexIt = iStartVertex; indexIt++;
      indexIt = iStartVertex + 1; indexIt++;
      indexIt = iWrapVertex; indexIt++;          

      // Second triangle
      indexIt = iStartVertex + 1; indexIt++;
      indexIt = iWrapVertex + 1; indexIt++;
      indexIt = iWrapVertex; indexIt++;    
    }
  }

  pMesh->UnLockIndices();
  return pMesh;
}

void VWaterPlaneGenerator::FillVertexDescriptor(VisMBVertexDescriptor_t& desc)
{

}

void VWaterPlaneGenerator::FillCustomData(void* pVertexData, unsigned int uiNumVertices)
{

}

VisMeshBuffer_cl* VWaterPlaneGeneratorLightgrid::Generate(bool bUseRadialGrid, const int (&iNumGridSubdivisions)[2], const hkvVec3 (&planeCorners)[4], hkvVec2 numTextureTiles)
{
  return VWaterPlaneGenerator::Generate<VertexLayoutLightgrid>(bUseRadialGrid, iNumGridSubdivisions, planeCorners, numTextureTiles);
}

void VWaterPlaneGeneratorLightgrid::FillVertexDescriptor(VisMBVertexDescriptor_t& desc)
{
  VWaterPlaneGenerator::FillVertexDescriptor(desc);
  desc.m_iTexCoordOfs[1] = offsetof(VertexLayoutLightgrid, lightGridColor) | VERTEXDESC_FORMAT_FLOAT3;
}

void VWaterPlaneGeneratorLightgrid::FillCustomData(void* pVertexData, unsigned int uiNumVertices)
{
  VASSERT(m_uiStride > 0 && uiNumVertices > 0);

  VWaterPlaneGenerator::FillCustomData(pVertexData, uiNumVertices);


  if(m_pLightGrid == NULL)
  {
    hkvLog::Warning("Static lit water plane: No light grid present. Using global ambient as a fall back.");
    hkvVec4 globalAmbient = Vision::Renderer.GetGlobalAmbientColor();

    VertexLayoutLightgrid* pVert = (VertexLayoutLightgrid*)pVertexData;
    for(unsigned int i=0; i<uiNumVertices; i++)
    {
      pVert->lightGridColor.set(globalAmbient.x, globalAmbient.y, globalAmbient.z);
      pVert = (VertexLayoutLightgrid*)(((char*)pVert) + m_uiStride);
    }
  }
  else
  {
    VertexLayoutLightgrid* pVert = (VertexLayoutLightgrid*)pVertexData;
    for(unsigned int i=0; i<uiNumVertices; i++)
    {
      hkvVec3 vVertexWorldPos = m_vWorldPosition + m_rotation.transformDirection(pVert->pos.compMul(m_vScale));
      pVert->lightGridColor.set(0.0f);
      m_pLightGrid->EvaluateColorAtPosition(vVertexWorldPos, m_rotation.transformDirection(pVert->normal), pVert->lightGridColor);
      pVert = (VertexLayoutLightgrid*)(((char*)pVert) + m_uiStride);
    }
  }
}

/*
 * Havok SDK - Base file, BUILD(#20140710)
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
