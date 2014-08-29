/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationEntityModel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstantHelper.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif

VTerrainDecorationEntityModelFactory VTerrainDecorationEntityModel::g_EntityModelFactory;

IVTerrainDecorationModel* VTerrainDecorationEntityModelFactory::CreateModelWithFilename(const char *szFilename, const IVTerrainDecorationModel::ModelProperties_t *pProperties)
{
  if (!VFileHelper::HasExtension(szFilename,"model") && !VFileHelper::HasExtension(szFilename,"vmesh"))
    return NULL;

  VTerrainDecorationEntityModel *pRes = new VTerrainDecorationEntityModel(szFilename,pProperties);
  pRes->EnsureLoaded();
  return pRes;
}

int VTerrainDecorationEntityModel::GetCollisionSubObjectCount()
{
  if (!IsValid() || m_Properties.m_eCollisionType==VDecorationCollisionPrimitive::VDecorationCollision_None)
    return 0;
  if (m_Properties.m_eCollisionType==VDecorationCollisionPrimitive::VDecorationCollision_Capsule && m_Properties.m_fCollisionCapsuleRadius<=0.f)
    return 0;
  return 1;
}

VDecorationCollisionPrimitive* VTerrainDecorationEntityModel::GetCollisionSubObject(int iIndex)
{
  VASSERT(iIndex==0 && iIndex<GetCollisionSubObjectCount());

  m_CollisionObject.m_eCollisionType = m_Properties.m_eCollisionType;
  if (m_CollisionObject.m_eCollisionType==VDecorationCollisionPrimitive::VDecorationCollision_Capsule)
  {
    // The lower end should stick into the ground, the upper half-sphere should be inside the bounding box - if possible
    float fUpper = m_LocalBBox.m_vMax.z - m_Properties.m_fCollisionCapsuleRadius;
    float fLower = m_LocalBBox.m_vMin.z;
    if (fUpper<fLower) fUpper=fLower;
    m_CollisionObject.Set(hkvVec3(0,0,fLower), hkvVec3(0,0,fUpper), m_Properties.m_fCollisionCapsuleRadius);
  } else
  {
    m_CollisionObject.m_pRenderMesh = m_spMesh;
  }
  return &m_CollisionObject;
}


void VTerrainDecorationEntityModel::ReapplyShaders()
{
  m_spVegetationShaders = NULL;

  // set default material to vegetation (when AUTO mode is used):
  if (m_spMesh!=NULL)
  {

#if defined( HK_ANARCHY )
    const BOOL bUseDeferred = FALSE;
#else
    const BOOL bUseDeferred =  Vision::Renderer.IsRendererNodeOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem));
#endif
    
    int iSubmeshCount = m_spMesh->GetSubmeshCount();
    for (int i=0;i<iSubmeshCount;i++)
    {
      VCompiledEffect *pVegetationFX = NULL;
      VBaseSubmesh *pSubmesh = m_spMesh->GetSubmesh(i);
      VisSurface_cl *pSurf = pSubmesh->GetSurface();
      if (m_spVegetationShaders==NULL)
        m_spVegetationShaders = new VisShaderSet_cl();

      if (pSurf->GetShaderMode()==VisSurface_cl::VSM_Auto)
      {
        char szParamStr[1024];
      
        const hkvAlignedBBox &bbox = m_spMesh->GetBoundingBox();
        float fBaseZ = bbox.isValid() ? -bbox.m_vMin.z : 0.f;
        sprintf(szParamStr,"MaterialParams=%.4f,%.4f,%.4f,%.4f;AlphaThreshold=%.3f;fBaseZ=%.2f", 
          pSurf->GetSpecularMultiplier(),
          pSurf->GetSpecularExponent(),
          pSurf->GetParallaxScale (), pSurf->GetParallaxBias (),
          pSurf->GetAlphaTestThreshold(),fBaseZ
          );

        if (bUseDeferred)
        {
          if (Vision::RenderLoopHelper.GetLightGrid()!=NULL)
          {
            VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("\\Shaders\\DeferredShadingVegetationLG.ShaderLib", SHADERLIBFLAG_HIDDEN);
            pVegetationFX = Vision::Shaders.CreateEffect("DeferredVegetation_Wind_LG",szParamStr,EFFECTCREATEFLAG_NONE,pLib);
          } else
          {
            VShaderEffectLib *pLib = Vision::Shaders.LoadShaderLibrary("\\Shaders\\DeferredShadingVegetation.ShaderLib", SHADERLIBFLAG_HIDDEN);
            pVegetationFX = Vision::Shaders.CreateEffect("DeferredVegetation_Wind",szParamStr,EFFECTCREATEFLAG_NONE,pLib);
          }
        } else
        {
          BOOL bRes = Vision::Shaders.LoadShaderLibrary("\\Shaders\\TerrainVegetation.ShaderLib", SHADERLIBFLAG_HIDDEN) != NULL;
          if (pSurf->m_spNormalMap!=NULL)
          {
            if (pSurf->m_spSpecularMap!=NULL)
              pVegetationFX = Vision::Shaders.CreateEffect("VegetationNrmlSpec_Wind",szParamStr);
            else
              pVegetationFX = Vision::Shaders.CreateEffect("VegetationNrml_Wind",szParamStr);
          } 
          else
            pVegetationFX = Vision::Shaders.CreateEffect("Vegetation_Wind",szParamStr);
        }
      }
      
      if (pVegetationFX==NULL)
        pVegetationFX = pSurf->GetEffect();
      else
      {
        pSurf->SetEffect(pVegetationFX); // sets up depth technique etc.
        pSurf->SetShaderMode(VisSurface_cl::VSM_Auto);
      }
      if (pSurf->GetTechnique())
        m_spVegetationShaders->Add(pSubmesh, pSurf, pSurf->GetTechnique());
    }
  }

  if (m_spVegetationShaders==NULL)
    m_spVegetationShaders = m_spMesh->GetShaderSet();  // no clone


  // supports instancing at all?
#if defined (_VISION_WIN32)
  m_spInstancingTech = NULL;
  m_spInstancingTechShadow = NULL;
  m_spInstancingTechIRPrePass = NULL;
  m_spInstancingTechIRMainPass = NULL;
  m_pShaderSetIRPrePass = NULL;
  m_pShaderSetIRMainPass = NULL;

  if (Vision::Renderer.SupportsInstancing() && m_bValidState && m_spVegetationShaders->Count()>=1)
  {
    const VisDrawCallInfo_t &drawCall(m_spVegetationShaders->GetDrawCallList()[0]);
    VisSurface_cl *pMat = drawCall.GetSurface();

	// The auto shaders do not have the ALPHATEST tag, but they use alpha test by default
	const bool alphaTest = (pMat->GetTransparencyType () == VIS_TRANSP_ALPHATEST || pMat->GetTransparencyType () == VIS_TRANSP_ALPHA) && (pMat->GetShaderMode() != VisSurface_cl::VSM_Auto);

    bool bAnyInstancingShader = false;
    VCompiledEffect *pFX = pMat->m_spCurrentEffect;
    if (pFX) // checks whether the assigned shader effect holds an instancing version
    {
      VTechniqueConfig cfg;

#ifdef _VR_DX10
      if ((Vision::Renderer.GetRendererNode ()) && (Vision::Renderer.GetRendererNode ()->GetMultisampleMode() != VVIDEO_MULTISAMPLE_OFF))
        cfg.SetInclusionTags("Instancing;MSAA");
      else
#endif
      cfg.SetInclusionTags("Instancing");
      if (alphaTest)
      {
        cfg.AddInclusionTag ("ALPHATEST");
      }

      m_spInstancingTech = pFX->FindCompatibleTechnique(&cfg, Vision::Shaders.GetGlobalTechniqueConfig());
      if (m_spInstancingTech!=NULL && m_spInstancingTech->GetShaderCount()==1)
        bAnyInstancingShader = true;

      // shadowmap instancing?
      VTechniqueConfig cfg_shadow("Instancing;SpecificShadowmapFill",NULL);
      if (alphaTest)
      {
        cfg_shadow.AddInclusionTag ("ALPHATEST");
      }
      m_spInstancingTechShadow = pFX->FindCompatibleTechnique(&cfg_shadow, Vision::Shaders.GetGlobalTechniqueConfig());
      if (m_spInstancingTechShadow!=NULL && m_spInstancingTechShadow->GetShaderCount()==1)
        bAnyInstancingShader = true;
    }


    if (bAnyInstancingShader)
    {
      VisMBVertexDescriptor_t desc;
      int iIndexCount = m_spMesh->GetMeshBuffer()->GetIndexCount();
      VVertexBuffer* pVB = m_spMesh->GetMeshBuffer()->GetVertexBuffer();
      m_spMesh->GetMeshBuffer()->GetVertexDescriptor(desc);

      VIndexBuffer* pIB = m_spMesh->GetMeshBuffer()->GetIndexBuffer();

      m_spModelMesh = new VisMeshBuffer_cl();
      m_spModelMesh->SetVertexBuffer(pVB,desc,m_spMesh->GetNumOfVertices(),VIS_MEMUSAGE_STATIC, 0);
      m_spModelMesh->SetIndexBuffer(pIB,iIndexCount,pIB->GetUsageFlags(), 0);
      m_spModelMesh->SetPrimitiveCount(iIndexCount/3);
      m_spModelMesh->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
      #ifdef HK_DEBUG
      m_spModelMesh->SetFilename("<TerrainDecorationEntity>");
      #endif

      m_iModelStreams = m_spModelMesh->GetStreamMask();
    }
  }
#endif
}


BOOL VTerrainDecorationEntityModel::Reload()
{
#ifndef _VR_DX11
  m_PerInstanceData.m_iFirstRegister = 50; // hardcoded in the shader
#endif
  m_spMesh = Vision::Game.LoadDynamicMesh(GetFilename(),true);
  m_spMesh->SetAllowShaderAssignment(false); // never update shader assignment on terrain entities

  m_bValidState = m_spMesh!=NULL && m_spMesh->IsLoaded();
  VASSERT(m_bValidState);


  if (Vision::Editor.IsInEditor())
  {
    // get additional properties from editor
    VisCallback_cl &cb = ((VTerrainDecorationModelManager *)GetParentManager())->OnGatherModelProperties;
    VTerrainDecorationModelDataObject data(&cb, this);
    cb.TriggerCallbacks(&data);
  }


  ReapplyShaders(); // after UpdateParameter because bbox must be valid
  UpdateParameter();

  return TRUE;
}


BOOL VTerrainDecorationEntityModel::Unload()
{
  m_spMesh = NULL;
  m_spModelMesh = NULL;
  m_spInstancingTech = NULL;
  m_spInstancingTechShadow = NULL;
  m_spInstancingTechIRPrePass = NULL;
  m_spInstancingTechIRMainPass = NULL;
  m_spVegetationShaders = NULL;
  m_pShaderSetIRPrePass = NULL;
  m_pShaderSetIRMainPass = NULL;
  m_bValidState = false;

  return TRUE;
}

void VTerrainDecorationEntityModel::UpdateParameter()
{
  VBaseMesh *pOldMesh = m_spMesh;
  m_spMesh = Vision::Game.LoadDynamicMesh(GetFilename(),true);

  m_bNeedsLightmap = false;
  m_iLightmapSampler = -1;
  bool bSupportsDepthRendering = true;
  bool bSupportsShadowmapRendering = true;
  if (m_spMesh)
  {
    m_spMesh->GetVisibilityBoundingBox(m_LocalBBox);

    // check shader requirements
    for (int i=0;i<(int)m_spMesh->GetSurfaceCount();i++)
    {
      VisSurface_cl *pSrf = m_spMesh->GetSurface(i);
      if (pSrf->m_spDepthFill==NULL)
        bSupportsDepthRendering = false;
      if (pSrf->m_spShadowmapFill==NULL)
        bSupportsShadowmapRendering = false;

      VCompiledTechnique *pSrfTech = pSrf->GetTechnique();
      
      if (!pSrfTech)
        continue;
      for (unsigned int j=0;j<pSrfTech->GetShaderCount();j++)
      {
        const VCompiledShaderPass *pShader = pSrfTech->GetShader(j);
        const int iActiveSamplerCount = (int)pShader->GetActiveSamplerCount(VSS_PixelShader);
        for (int k=0;k<iActiveSamplerCount;k++)
        {
          const VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, k);
          VASSERT(pStateGroupTexture != NULL);
          if (pStateGroupTexture->m_cTextureType == TEXTURETYPE_LIGHTMAP)
          {
            m_iLightmapSampler = k;
            m_bNeedsLightmap = true;
          }
        }
      }
    }
  }

  IVTerrainDecorationModel::UpdateParameter();

  if (pOldMesh != m_spMesh)
    ReapplyShaders();

  if (!bSupportsShadowmapRendering)
    m_iSupportedContextTypes &= ~(1<<VIS_CONTEXTUSAGE_DEPTHSHADOW);
  if (bSupportsDepthRendering)
    m_iSupportedContextTypes |= (1<<VIS_CONTEXTUSAGE_DEPTHFILL);

}


void VTerrainDecorationEntityModel::GatherLightmapInfo(VLightmapSceneInfo &info, VTerrainDecorationInstance **pInst, int iCount)
{
#ifdef _VISION_WIN32
  if (m_spCustomLightmapMesh!=NULL)
    IVTerrainDecorationModel::GatherLightmapInfo(m_spCustomLightmapMesh, info,pInst,iCount);
  else
    IVTerrainDecorationModel::GatherLightmapInfo(m_spMesh, info,pInst,iCount);
#endif
}


void VTerrainDecorationEntityModel::OnEditorEvent(EditorEvent_e action, VTerrain *pTerrain, void *pDataA, void *pDataB) 
{
  IVTerrainDecorationModel::OnEditorEvent(action, pTerrain, pDataA, pDataB);
#ifdef _VISION_WIN32
  if (action==GatherLightmapFinished)
  {
  }
  if (action==SurfaceShaderChanged)
  {

    VisSurface_cl *pSurface = (VisSurface_cl *)pDataA;
    
    // is the sent surface part of this model?
    if (m_spMesh!=NULL && m_spMesh->GetSurfaceIndex(pSurface)>=0)
    {
      ReapplyShaders();
      UpdateParameter();
    }
    return;
  }
  if (action==RendererNodeChanged)
  {
    if (m_spMesh!=NULL)
    {
      ReapplyShaders();
      UpdateParameter();
    }
    return;
  }

#endif
}

#define RENDER_INSTANCES(_iCount) \
  Vision::RenderLoopHelper.ResetMeshStreams(); \
  Vision::RenderLoopHelper.AddMeshStreams(m_spModelMesh, m_iModelStreams & (pShader->GetStreamMask() | VERTEX_STREAM_INDEXBUFFER));\
  Vision::RenderLoopHelper.AddMeshStreamsEx(pInstanceMesh, iInstanceStreamMask, 0, 1);\
  Vision::RenderLoopHelper.SetMeshInstanceCount(_iCount,iVertexCount);\
  Vision::RenderLoopHelper.RenderMeshes(pShader, ePrimType, iFirstPrimitive, iPrimitiveCount, iVertexCount);


void VTerrainDecorationEntityModel::RenderBatch(VisMeshBuffer_cl *pInstanceMesh, int iInstanceCount, int iInstanceStreamMask, const hkvVec3 &vTranslate, RenderMode_e eRenderMode)
{
  VASSERT(pInstanceMesh!=NULL);
  pInstanceMesh->EnsureLoaded();
  #if defined(_VR_DX11)
    VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_DECOMODEL,&m_PerModelConstantBuffer);
  #else
    VisRenderStates_cl::SetVertexShaderConstant(48,m_vDecorationParams.data,1);
  #endif
  
  VCompiledTechnique *pTech = NULL;

  // render into specific contexts ?
  const VisContextUsageHint_e eContextType = Vision::Contexts.GetCurrentContext()->GetUsageHint();
  if ((eContextType & VIS_CONTEXTUSAGE_TYPEENUMMASK) == VIS_CONTEXTUSAGE_DEPTHFILL)
    return;
  else if ((eContextType & VIS_CONTEXTUSAGE_TYPEENUMMASK) == VIS_CONTEXTUSAGE_DEPTHSHADOW)
    pTech = m_spInstancingTechShadow;    
  else if (eRenderMode == IVTerrainDecorationModel::RENDER_MODE_OTW)
    pTech = m_spInstancingTech;
  else if (eRenderMode == IVTerrainDecorationModel::RENDER_MODE_IR_PREPASS)
  {
    if (m_spInstancingTechIRPrePass == NULL)
    {
      // lazy IR shader init
      VisSurface_cl* pSurface = m_spMesh->GetSurface(0);
      VTerrainDecorationModelManager* pManager = (VTerrainDecorationModelManager *)GetParentManager();
      VShaderEffectLib* pEffectLib = pManager->GetInfraredShaderLib();
      RecreateIRShaders(pSurface, pEffectLib);
    } 
    pTech = m_spInstancingTechIRPrePass;
  }
  else if (eRenderMode == IVTerrainDecorationModel::RENDER_MODE_IR_MAINPASS)
  {
    if (m_spInstancingTechIRMainPass == NULL)
    {
      // lazy IR shader init
      VisSurface_cl* pSurface = m_spMesh->GetSurface(0);
      VTerrainDecorationModelManager* pManager = (VTerrainDecorationModelManager *)GetParentManager();
      VShaderEffectLib* pEffectLib = pManager->GetInfraredShaderLib();
      RecreateIRShaders(pSurface, pEffectLib);
    } 
    pTech = m_spInstancingTechIRMainPass;
  }
  if (pTech==NULL)
    return;
  VCompiledShaderPass *pShader = pTech->GetShader(0);

  VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType = m_spModelMesh->GetPrimitiveType();
  int iVertexCount = m_spModelMesh->GetVertexCount();

  Vision::RenderLoopHelper.BeginMeshRendering();
  hkvMat4 transform(hkvMat3::IdentityMatrix(), vTranslate);
  Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);
  const int iSubmeshCount = m_spMesh->GetSubmeshCount();
  int iFirstIndex, iIndexCount;

  for (int i=0;i<iSubmeshCount;i++)
  {
    VBaseSubmesh *pSubmesh = m_spMesh->GetBaseSubmesh(i);
    pSubmesh->GetRenderRange(iFirstIndex, iIndexCount);
    int iFirstPrimitive = iFirstIndex/3;
    int iPrimitiveCount = iIndexCount/3;

    VisSurface_cl *pSurface = pSubmesh->GetSurface();
    Vision::RenderLoopHelper.BindSurfaceTextures(pSurface,pShader,NULL);
    Vision::RenderLoopHelper.BindDefaultStateGroups(pSurface,pShader);

    RENDER_INSTANCES(iInstanceCount);
  }

  Vision::RenderLoopHelper.EndMeshRendering();
#if defined(_VR_DX11)
  VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_DECOMODEL, NULL);
#endif
}


void VTerrainDecorationEntityModel::RenderBatch(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, const hkvVec3 &vTranslate, RenderMode_e eRenderMode)
{
  /*
  // DEBUGGING:
  static bool bAllowInstancing = false;
  if (Vision::Key.IsPressed(VGLK_SPACE))
  {
    Vision::Key.SetSingleHit(VGLK_SPACE);
    bAllowInstancing = !bAllowInstancing;
    Vision::Message.Add(1,"Instancing : %s",bAllowInstancing?"On":"Off");
  }
  */

  #if defined(_VR_DX11)
    VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_DECOMODEL,&m_PerModelConstantBuffer);
  #else
    VisRenderStates_cl::SetVertexShaderConstant(48,m_vDecorationParams.data,1);
  #endif

  // render into specific contexts ?
  const VisContextUsageHint_e eContextType = Vision::Contexts.GetCurrentContext()->GetUsageHint();
  if ((eContextType & VIS_CONTEXTUSAGE_TYPEENUMMASK) == VIS_CONTEXTUSAGE_DEPTHFILL) 
    RenderBatchDepthFill(pInfoComp, pInstList, iCount,vTranslate);
  else if ((eContextType & VIS_CONTEXTUSAGE_TYPEENUMMASK) == VIS_CONTEXTUSAGE_DEPTHSHADOW) 
    RenderBatchDepthShadow(pInfoComp, pInstList, iCount,vTranslate);
  else if (eRenderMode == IVTerrainDecorationModel::RENDER_MODE_OTW)
    RenderBatchOTW(pInfoComp, pInstList, iCount,vTranslate);
  else
    RenderBatchIR(pInfoComp, pInstList, iCount,vTranslate, eRenderMode);
}

void VTerrainDecorationEntityModel::RenderBatchDepthFill(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, const hkvVec3 &vTranslate)
{
  // depth fill rendering
  INSERT_PERF_MARKER_SCOPE("VTerrainDecorationEntityModel::RenderBatchDepthFill");

  VisDrawCallInfo_t surfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];
  const int iNumSubmeshes = m_spMesh->GetSubmeshCount();
  int iAsmCount = 0;
  for (int i=0;i<iNumSubmeshes;i++)
  {
    VDynamicSubmesh *pSubmesh = m_spMesh->GetSubmesh(i);
    VisSurface_cl *pSurface = pSubmesh->GetSurface();
    if (pSurface->m_spDepthFill==NULL)
      continue;
    VisDrawCallInfo_t &assignment(surfaceShaderList[iAsmCount]);
    assignment.Set(pSubmesh, pSurface, pSurface->m_spDepthFill->GetShader(0));
    iAsmCount++;
  }
  Vision::RenderLoopHelper.BeginEntityRendering();
  for (int i=0;i<iCount;i++)
  {
    hkvMat4 transform(pInstList[i]->m_Orientation,pInstList[i]->m_vPosition + vTranslate);
    Vision::RenderLoopHelper.RenderModelWithSurfaceShaderList(m_spMesh, transform.getPointer (),iAsmCount,surfaceShaderList);
  }
  Vision::RenderLoopHelper.EndEntityRendering();
}

void VTerrainDecorationEntityModel::RenderBatchDepthShadow(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, const hkvVec3 &vTranslate)
{
  // depth shader rendering
  INSERT_PERF_MARKER_SCOPE("VTerrainDecorationEntityModel::RenderBatchDepthShadow");
  VTerrainDecorationModelManager *pManager = (VTerrainDecorationModelManager *)GetParentManager();

  if (m_spInstancingTechShadow!=NULL && iCount>4 && pManager->m_iInstancingBatchCount>0 /* && bAllowInstancing*/)
  {
    VCompiledShaderPass *pShader = m_spInstancingTechShadow->GetShader(0);

    Vision::RenderLoopHelper.BeginMeshRendering();
    hkvMat4 transform(hkvMat3::IdentityMatrix(), vTranslate);
    Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);
    VisSurface_cl *pSurface = m_spMesh->GetSurface(0);
    Vision::RenderLoopHelper.BindSurfaceTextures(pSurface,pShader,NULL);
    Vision::RenderLoopHelper.BindDefaultStateGroups(pSurface,pShader);
    VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType = m_spModelMesh->GetPrimitiveType();
    int iFirstPrimitive = 0;
    int iPrimitiveCount = m_spModelMesh->GetCurrentPrimitiveCount();
    int iVertexCount = m_spModelMesh->GetVertexCount();
    int iMaxInstanceCount, iInstanceStreamMask;
    VisMeshBuffer_cl *pInstanceMesh = ((VTerrainDecorationModelManager *)GetParentManager())->GetInstanceBuffer(iMaxInstanceCount,iInstanceStreamMask);

    while (iCount>0)
    {
      int iRenderCount = hkvMath::Min(iCount,iMaxInstanceCount);

      // fill the instance buffer:
      {
        VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_RENDERDECORARION_INSTANCE_SETUP);
        VModelInstanceData_t *pDest = (VModelInstanceData_t *)pInstanceMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE,0,iRenderCount);
        for (int i=0;i<iRenderCount;i++,pInstList++,pDest++)
          pDest->Set(*pInstList[0]);
        pInstanceMesh->UnLockVertices();
      }

      iCount-=iRenderCount;
      RENDER_INSTANCES(iRenderCount);
    }
    Vision::RenderLoopHelper.EndMeshRendering();
  } 
  else
  {
    // non-instancing version
    VisDrawCallInfo_t surfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];
    const int iNumSubmeshes = m_spMesh->GetSubmeshCount();
    int iAsmCount = 0;
    for (int i=0;i<iNumSubmeshes;i++)
    {
      VDynamicSubmesh *pSubmesh = m_spMesh->GetSubmesh(i);
      VisDrawCallInfo_t &assignment(surfaceShaderList[iAsmCount]);
      if (pSubmesh->GetSurface()->m_spShadowmapFill==NULL)
        continue;
      assignment.Set(pSubmesh, pSubmesh->GetSurface(), pSubmesh->GetSurface()->m_spShadowmapFill->GetShader(0));
      iAsmCount++;
    }
    Vision::RenderLoopHelper.BeginEntityRendering();
    for (int i=0;i<iCount;i++)
    {
      hkvMat4 transform(pInstList[i]->m_Orientation,pInstList[i]->m_vPosition + vTranslate);
      Vision::RenderLoopHelper.RenderModelWithSurfaceShaderList(m_spMesh, transform.getPointer (),iAsmCount,surfaceShaderList);
    }
    Vision::RenderLoopHelper.EndEntityRendering();
  }
}

void VTerrainDecorationEntityModel::RenderBatchOTW(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, const hkvVec3 &vTranslate)
{
  // OTW rendering
  VTerrainDecorationModelManager *pManager = (VTerrainDecorationModelManager *)GetParentManager();
  if (m_spInstancingTech!=NULL && iCount>4 && pManager->m_iInstancingBatchCount>0 /* && bAllowInstancing*/)
  {
    VCompiledShaderPass *pShader = m_spInstancingTech->GetShader(0);

    Vision::RenderLoopHelper.BeginMeshRendering();
    hkvMat4 transform(hkvMat3::IdentityMatrix(), vTranslate);
    Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);
    VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType = m_spModelMesh->GetPrimitiveType();
    int iVertexCount = m_spModelMesh->GetVertexCount();
    int iMaxInstanceCount, iInstanceStreamMask;
    VisMeshBuffer_cl *pInstanceMesh = ((VTerrainDecorationModelManager *)GetParentManager())->GetInstanceBuffer(iMaxInstanceCount,iInstanceStreamMask);
    iInstanceStreamMask &= pShader->GetStreamMask();

    // lightmap version
    if (m_iLightmapSampler>=0 && pInfoComp!=NULL)
    {
#ifdef HK_DEBUG
      const VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, m_iLightmapSampler);
      VASSERT(pStateGroupTexture!=NULL && pStateGroupTexture->m_cTextureType==TEXTURETYPE_LIGHTMAP);
#endif

      VStateGroupSampler *pLMSampler = pShader->GetStateGroupSampler(VSS_PixelShader, m_iLightmapSampler);
      VTerrainSector *pLastSector = NULL;
      VTextureObject *pTerrainLightmap = NULL;
      const VTerrainConfig& config(pInfoComp->m_pTerrain->m_Config);
      VTerrainSectorManager &sectormanager(pInfoComp->m_pTerrain->m_SectorManager);
      while (iCount>0)
      {
        int iWantedRenderCount = hkvMath::Min(iCount,iMaxInstanceCount);
        int iRenderCount = 0;

        // fill the instance buffer:
        {
          VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_RENDERDECORARION_INSTANCE_SETUP);
          VModelInstanceData_t *pDest = (VModelInstanceData_t *)pInstanceMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE,0,iWantedRenderCount);
          // fill buffer up to lightmap change
          for (int i=0;i<iWantedRenderCount;i++,iRenderCount++,pDest++)
          {
            if (pLastSector!=pInstList[i]->m_pOwnerSector)
            {
              pLastSector = pInstList[i]->m_pOwnerSector;
              VTextureObject *pNewLightmap = pLastSector->m_pMeshPage[0].GetSurfaceSafe().m_spModelLightmaps[0];
              if (pNewLightmap!=pTerrainLightmap)
              {
                if (iRenderCount>0) // start a new batch so break here
                  break;
              }
            }
            pDest->Set(*pInstList[i]);
          }
          pInstanceMesh->UnLockVertices();
        }

        // bind sector specific properties
        VTerrainSector *pStateSetupSector = pInstList[0]->m_pOwnerSector; // this is where the batch starts
        hkvVec4 vWorld2Sector(false);
        // transforms worldspace to sector 0..1 range
        config.GetWorldSpaceToSectorTransform(pStateSetupSector->m_iIndexX,pStateSetupSector->m_iIndexY,vWorld2Sector);
        sectormanager.SetWorld2SectorTransform(vWorld2Sector);
          // standard range -> lightmap
        const hkvVec4 vSector2LM = pStateSetupSector->GetLightmapScaleOffset();
        VisRenderStates_cl::VSSetModelUVToLightmap(vSector2LM.data);
        Vision::RenderLoopHelper.BindMeshTexture(pStateSetupSector->m_pMeshPage[0].GetSurfaceSafe().m_spModelLightmaps[0],m_iLightmapSampler,pLMSampler);

        // advance by actual render counts
        pInstList += iRenderCount;
        iCount-=iRenderCount;

        VisSurface_cl *pSurface = m_spMesh->GetSurface(0);
        Vision::RenderLoopHelper.BindSurfaceTextures(pSurface,pShader,NULL);
        Vision::RenderLoopHelper.BindDefaultStateGroups(pSurface,pShader);
        int iFirstPrimitive = 0;
        int iPrimitiveCount = m_spModelMesh->GetCurrentPrimitiveCount();

        RENDER_INSTANCES(iRenderCount);
      }

    }
    else // non-lightmapped version
    {
      while (iCount>0)
      {
        int iRenderCount = hkvMath::Min(iCount,iMaxInstanceCount);

        // fill the instance buffer:
        {
          VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_RENDERDECORARION_INSTANCE_SETUP);
          VModelInstanceData_t *pDest = (VModelInstanceData_t *)pInstanceMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE,0,iRenderCount);
          for (int i=0;i<iRenderCount;i++,pInstList++,pDest++)
            pDest->Set(*pInstList[0]);
          pInstanceMesh->UnLockVertices();
        }

        iCount-=iRenderCount;
        const int iSubmeshCount = m_spMesh->GetSubmeshCount();
        int iFirstIndex, iIndexCount;

        for (int i=0;i<iSubmeshCount;i++)
        {
          VBaseSubmesh *pSubmesh = m_spMesh->GetBaseSubmesh(i);
          pSubmesh->GetRenderRange(iFirstIndex, iIndexCount);
          int iFirstPrimitive = iFirstIndex/3;
          int iPrimitiveCount = iIndexCount/3;
          VisSurface_cl *pSurface = pSubmesh->GetSurface();
          Vision::RenderLoopHelper.BindSurfaceTextures(pSurface,pShader,NULL);
          Vision::RenderLoopHelper.BindDefaultStateGroups(pSurface,pShader);

          RENDER_INSTANCES(iRenderCount);
        }
      }
    }

    Vision::RenderLoopHelper.EndMeshRendering();
    return;
  }

  //////////////////////////////////////////////////////////
  // Non-instancing  version

  const VisDrawCallInfo_t *pSurfaceShaderList;
  VDynamicMesh *pMesh = m_spMesh;
  VASSERT(m_spVegetationShaders!=NULL);
  VisShaderSet_cl *pSet = m_spVegetationShaders;
  int iAsmCount = pSet->GetShaderAssignmentList(&pSurfaceShaderList);
  VColorRef iLastColor;

  #ifdef _VR_DX11
    VisRenderStates_cl::SetVSConstantBuffer(7,&m_PerInstanceData);
  #endif

  if (m_bNeedsLightmap && pInfoComp!=NULL)
  {
    VTerrainSector *pLastSector = NULL;
    VTextureObject *pTerrainLightmap = NULL;
    const VTerrainConfig& config(pInfoComp->m_pTerrain->m_Config);
    VTerrainSectorManager &sectormanager(pInfoComp->m_pTerrain->m_SectorManager);

    Vision::RenderLoopHelper.BeginEntityRendering();
    for (int i=0;i<iCount;i++,pInstList++)
    {
      if (pLastSector!=(*pInstList)->m_pOwnerSector)
      {
        pLastSector = (*pInstList)->m_pOwnerSector;
        VTextureObject *pNewLightmap = pLastSector->m_pMeshPage[0].GetSurfaceSafe().m_spModelLightmaps[0];
        if (pNewLightmap!=pTerrainLightmap)
        {
          // assign new model lightmaps
          const int iSrfCount = m_spMesh->GetSurfaceCount();
          for (int j=0;j<iSrfCount;j++)
            m_spMesh->GetSurface(j)->m_spModelLightmaps[0] = pNewLightmap;

          // and also force a shader re-binding
          for (int j=0;j<iAsmCount;j++)
            pSurfaceShaderList[j].GetShader()->m_bModified=true;

          pTerrainLightmap = pNewLightmap;
        }

        hkvVec4 vWorld2Sector(false);
        // transforms worldspace to sector 0..1 range
        config.GetWorldSpaceToSectorTransform(pLastSector->m_iIndexX,pLastSector->m_iIndexY,vWorld2Sector);
        sectormanager.SetWorld2SectorTransform(vWorld2Sector);
          // standard range -> lightmap
        const hkvVec4 vSector2LM = pLastSector->GetLightmapScaleOffset();
        VisRenderStates_cl::VSSetModelUVToLightmap(vSector2LM.data);
      }

      // per instance tint color
      if (i==0 || iLastColor!=(*pInstList)->m_InstanceColor)
      {
        iLastColor = (*pInstList)->m_InstanceColor;
        VPerInstanceData_t &data(m_PerInstanceData.BeginUpdate());
          VColorRef::RGBA_To_Float((*pInstList)->m_InstanceColor, data.vPerInstanceColor);
        m_PerInstanceData.EndUpdate();
      #ifndef _VR_DX11
        VisRenderStates_cl::SetVSConstantBuffer(7,&m_PerInstanceData);
      #endif
      }

      // finally render object
      hkvMat4 transform((*pInstList)->m_Orientation,(*pInstList)->m_vPosition + vTranslate);
      Vision::RenderLoopHelper.RenderModelWithSurfaceShaderList(m_spMesh,transform.getPointer (),iAsmCount,pSurfaceShaderList);
    }
    VisRenderStates_cl::SetVSConstantBuffer(7,NULL);
    Vision::RenderLoopHelper.EndEntityRendering();
  }
  else
  {
    Vision::RenderLoopHelper.BeginEntityRendering();

    // no lightmaps -> simple loop 
    for (int i=0;i<iCount;i++,pInstList++)
    {
      // per instance tint color
      if (i==0 || iLastColor!=(*pInstList)->m_InstanceColor)
      {
        iLastColor = (*pInstList)->m_InstanceColor;
        VPerInstanceData_t &data(m_PerInstanceData.BeginUpdate());
          VColorRef::RGBA_To_Float((*pInstList)->m_InstanceColor, data.vPerInstanceColor);
        m_PerInstanceData.EndUpdate();
      #ifndef _VR_DX11
        VisRenderStates_cl::SetVSConstantBuffer(7,&m_PerInstanceData);
      #endif
      }
      hkvMat4 transform((*pInstList)->m_Orientation,(*pInstList)->m_vPosition + vTranslate);
      Vision::RenderLoopHelper.RenderModelWithSurfaceShaderList(m_spMesh,transform.getPointer(),iAsmCount,pSurfaceShaderList);
    }

    VisRenderStates_cl::SetVSConstantBuffer(7,NULL);

    Vision::RenderLoopHelper.EndEntityRendering();
  }
}

void VTerrainDecorationEntityModel::RenderBatchIR(VTerrainVisibilityCollectorComponent *pInfoComp, VTerrainDecorationInstance **pInstList, int iCount, const hkvVec3 &vTranslate, RenderMode_e eRenderMode)
{
  // IR Rendering
  VTerrainDecorationModelManager* pManager = (VTerrainDecorationModelManager *)GetParentManager();
  VShaderEffectLib* pEffectLib = pManager->GetInfraredShaderLib();
  bool bPrePass = (eRenderMode == RENDER_MODE_IR_PREPASS);

  if (m_spInstancingTech!=NULL && iCount > 4 && pManager->m_iInstancingBatchCount > 0 && pEffectLib != NULL)
  {
    //////////////////////////////////////////////////////////
    // Instancing  version
    int iMaxInstanceCount, iInstanceStreamMask;
    VisSurface_cl* pSurface = m_spMesh->GetSurface(0);

    if (m_spInstancingTechIRPrePass == NULL)
    {
      // lazy IR shader init
      RecreateIRShaders(pSurface, pEffectLib);
    }
    if (m_spInstancingTechIRPrePass == NULL || m_spInstancingTechIRMainPass == NULL)
      return;

    const int iFirstPrimitive = 0;
    const int iPrimitiveCount = m_spModelMesh->GetCurrentPrimitiveCount();
    const int iVertexCount = m_spModelMesh->GetVertexCount();
    const VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType = m_spModelMesh->GetPrimitiveType();

    VCompiledShaderPass* pShader = bPrePass? m_spInstancingTechIRPrePass->GetShader(0) : m_spInstancingTechIRMainPass->GetShader(0);
    
    VisMeshBuffer_cl* pInstanceMesh = pManager->GetInstanceBuffer(iMaxInstanceCount, iInstanceStreamMask);
    iInstanceStreamMask &= pShader->GetStreamMask();

    // perform the rendering
    Vision::RenderLoopHelper.BeginMeshRendering();
    hkvMat4 transform(hkvMat3::IdentityMatrix(), vTranslate);
    Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);
    Vision::RenderLoopHelper.BindDefaultStateGroups(pSurface, pShader);

    if (!bPrePass || pSurface->GetTransparencyType() != VIS_TRANSP_NONE)
      Vision::RenderLoopHelper.BindMeshTexture(pSurface->GetBaseTextureObject(), 0, NULL);

    if (!bPrePass)
      Vision::RenderLoopHelper.BindSurfaceTextures(pSurface, pShader);

    while (iCount > 0)
    {
      int iRenderCount = hkvMath::Min(iCount, iMaxInstanceCount);

      // fill the instance buffer:
      {
        VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_RENDERDECORARION_INSTANCE_SETUP);

        VModelInstanceData_t* pDest = (VModelInstanceData_t *)pInstanceMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE, 0, iRenderCount);
        for (int i = 0; i < iRenderCount; i++, pInstList++, pDest++)
          pDest->Set(*pInstList[0]);
        pInstanceMesh->UnLockVertices();
      }

      iCount -= iRenderCount;

      RENDER_INSTANCES(iRenderCount);
    }

    Vision::RenderLoopHelper.EndMeshRendering();

    return;
  }

  //////////////////////////////////////////////////////////
  // Non-instancing  version

  VDynamicMesh* pMesh = m_spMesh;
  VisSurface_cl** ppSurfaces = pMesh->GetSurfaceArray();

  if (m_pShaderSetIRPrePass == NULL)
  {
    // create shader set for pre IR rendering
    VisShaderSet_cl* pShaderSet = new VisShaderSet_cl();
    pShaderSet->BeginUpdate();

    for (int i = 0; i < pMesh->GetSubmeshCount(); i++)
    {
      VDynamicSubmesh* pSubmesh = pMesh->GetSubmesh(i);
      VisSurface_cl* pSurface = ppSurfaces[pSubmesh->m_iMaterialIndex];

      VCompiledTechnique* pTech = RecreateIRShaderPre(pSurface, pEffectLib);
      VASSERT(pTech != NULL);

      pShaderSet->Add(pSubmesh, pSurface, pTech);
    }

    pShaderSet->EndUpdate();
    m_pShaderSetIRPrePass = pShaderSet;

    // create shader set for main IR rendering
    pShaderSet = new VisShaderSet_cl();
    pShaderSet->BeginUpdate();

    for (int i = 0; i < pMesh->GetSubmeshCount(); i++)
    {
      VDynamicSubmesh* pSubmesh = pMesh->GetSubmesh(i);
      VisSurface_cl* pSurface = ppSurfaces[pSubmesh->m_iMaterialIndex];

      VCompiledTechnique* pTech = RecreateIRShaderMain(pSurface, pEffectLib);
      VASSERT(pTech != NULL);

      pShaderSet->Add(pSubmesh, pSurface, pTech);
    }

    pShaderSet->EndUpdate();
    m_pShaderSetIRMainPass = pShaderSet;
  }

  VisShaderSet_cl* pSet = bPrePass? m_pShaderSetIRPrePass : m_pShaderSetIRMainPass;
  const VisDrawCallInfo_t* pSurfaceShaderList;
  int iAsmCount = pSet->GetShaderAssignmentList(&pSurfaceShaderList);

  Vision::RenderLoopHelper.BeginEntityRendering();

  for (int i = 0; i < iCount; i++, pInstList++)
  {
    // render object
    hkvMat4 transform((*pInstList)->m_Orientation,(*pInstList)->m_vPosition + vTranslate);
    Vision::RenderLoopHelper.RenderModelWithSurfaceShaderList(m_spMesh ,transform.getPointer(), iAsmCount, pSurfaceShaderList);
  }

  Vision::RenderLoopHelper.EndEntityRendering();
}

void VTerrainDecorationEntityModel::RecreateIRShaders(VisSurface_cl* pSurface, VShaderEffectLib* pEffectLib)
{
  if (pSurface == NULL)
    return;

  // check for supported transparency types
  if (pSurface->GetTransparencyType() != VIS_TRANSP_NONE &&
      pSurface->GetTransparencyType() != VIS_TRANSP_ALPHATEST &&
      pSurface->GetTransparencyType() != VIS_TRANSP_ALPHA)
    return;

  char szParam[1024];
  VTechniqueConfig baseCfg("INSTANCING;WIND", NULL);

//   if (Vision::Renderer.GetCurrentRendererNode() != NULL && Vision::Renderer.GetRendererNode()->GetMultisampleMode() != VVIDEO_MULTISAMPLE_OFF)
//     baseCfg.AddInclusionTag("MSAA");

  switch (pSurface->GetGeometryTopology())
  {
    case VisSurface_cl::VGT_3DMesh:
      baseCfg.AddInclusionTag("GEO_3D");
      break;

    default:
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaders: Only 3DMesh geometry topology is supported");
      return;
  }

  hkvVec4 vWindPhaseParams(0.0071f, 0.0092f, 0.0f, 0.0f);
  const hkvAlignedBBox& bbox = m_spMesh->GetBoundingBox();
  if (bbox.isValid())
    vWindPhaseParams.w = -bbox.m_vMin.z;

  {
    // 1. Assign G-Pass pass technique
    ////////////////////////////////////
    szParam[0] = '\0';

    // Get params needed for IR shader creation from IR renderer
    char* pszParamPos = GetParamStringForIRSurface(pSurface, szParam);
    pszParamPos += sprintf(pszParamPos, "WindPhaseParams=%g,%g,%g,%g;", vWindPhaseParams.x, vWindPhaseParams.y, vWindPhaseParams.z, vWindPhaseParams.w);

    VTechniqueConfig cfg(baseCfg);

    bool bHasMaterialTex = pSurface->GetAuxiliaryTextureCount() >= 1 && pSurface->GetAuxiliaryTexture(0) != NULL;
    bool bHasHotSpotTex = pSurface->GetAuxiliaryTextureCount() >= 2 && pSurface->GetAuxiliaryTexture(1) != NULL;

    if (bHasMaterialTex)
    {
      // --- Thermal params are taken from auxiliary texture
      cfg.AddInclusionTag("AUX_TEX");
    }
    else
    {
      int iMaterialID = 0;
      if (pSurface->GetMesh() != NULL)
        iMaterialID = (int)pSurface->GetMesh()->GetBaseSubmesh(0)->GetGeometryInfo().m_sUserFlags;
      
      // --- Thermal params are taken from submesh user flags
      pszParamPos += sprintf(pszParamPos, "ThermalMaterialID=%i;", iMaterialID);
    }

    if (bHasHotSpotTex)
      cfg.AddInclusionTag("HEATING_POWER");

    VCompiledEffect* pFX = Vision::Shaders.CreateEffect("InitialPass", szParam, EFFECTCREATEFLAG_NONE, pEffectLib);
    if (pFX == NULL)
    {
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaders: Failed to create InitialPass effect");
      return;
    }
  
    VCompiledTechnique* pTechnique = pFX->FindCompatibleTechnique(&cfg);
    if (pTechnique == NULL)
    {
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaders: No compatible technique found; using default technique");
      pTechnique = pFX->GetDefaultTechnique(); // find default technique
    }

    VASSERT(pTechnique != NULL && pTechnique->GetShaderCount() == 1);
    pTechnique->GetShader(0)->m_bModified = true;

    m_spInstancingTechIRMainPass = pTechnique;
  }

  {
    // 2. Assign Pre-Pass pass technique
    /////////////////////////////////////
    szParam[0] = '\0';
    char* pszParamPos = szParam;
    pszParamPos += sprintf(pszParamPos, "WindPhaseParams=%g,%g,%g,%g;", vWindPhaseParams.x, vWindPhaseParams.y, vWindPhaseParams.z, vWindPhaseParams.w);

    VTechniqueConfig cfg(baseCfg);

    if (pSurface->GetTransparencyType() == VIS_TRANSP_ALPHATEST || pSurface->GetTransparencyType() == VIS_TRANSP_ALPHA)
    {
      cfg.AddInclusionTag("ALPHATEST");
      pszParamPos += sprintf(pszParamPos, "AlphaTestThreshold=%g;", pSurface->GetAlphaTestThreshold());
    }

    VCompiledEffect* pFX = Vision::Shaders.CreateEffect("PrePass", szParam, EFFECTCREATEFLAG_NONE, pEffectLib);
    if (pFX == NULL)
    {
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaders: Failed to create PrePass effect");
      return;
    }

    VCompiledTechnique* pTechnique = pFX->FindCompatibleTechnique(&cfg);
    if (pTechnique == NULL)
    {
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaders: No compatible technique found; using default technique");
      pTechnique = pFX->GetDefaultTechnique(); // find default technique
    }

    VASSERT(pTechnique != NULL && pTechnique->GetShaderCount() == 1);
    pTechnique->GetShader(0)->m_bModified = true;

    m_spInstancingTechIRPrePass = pTechnique;
  }
}

VCompiledTechnique* VTerrainDecorationEntityModel::RecreateIRShaderMain(VisSurface_cl* pSurface, VShaderEffectLib* pEffectLib)
{
  if (pSurface == NULL)
    return NULL;

  // check for supported transparency types
  if (pSurface->GetTransparencyType() != VIS_TRANSP_NONE &&
      pSurface->GetTransparencyType() != VIS_TRANSP_ALPHATEST &&
      pSurface->GetTransparencyType() != VIS_TRANSP_ALPHA)
    return NULL;

  char szParam[1024];
  VTechniqueConfig baseCfg("WIND", NULL);

  switch (pSurface->GetGeometryTopology())
  {
    case VisSurface_cl::VGT_3DMesh:
      baseCfg.AddInclusionTag("GEO_3D");
      break;

    default:
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaderMain: Only 3DMesh geometry topology is supported");
      return NULL;
  }

  hkvVec4 vWindPhaseParams(0.0071f, 0.0092f, 0.0f, 0.0f);
  const hkvAlignedBBox& bbox = m_spMesh->GetBoundingBox();
  if (bbox.isValid())
    vWindPhaseParams.w = -bbox.m_vMin.z;

  szParam[0] = '\0';

  // Get params needed for IR shader creation from IR renderer
  char* pszParamPos = GetParamStringForIRSurface(pSurface, szParam);
  pszParamPos += sprintf(pszParamPos, "WindPhaseParams=%g,%g,%g,%g;", vWindPhaseParams.x, vWindPhaseParams.y, vWindPhaseParams.z, vWindPhaseParams.w);

  VTechniqueConfig cfg(baseCfg);

  bool bHasMaterialTex = pSurface->GetAuxiliaryTextureCount() >= 1 && pSurface->GetAuxiliaryTexture(0) != NULL;
  bool bHasHotSpotTex = pSurface->GetAuxiliaryTextureCount() >= 2 && pSurface->GetAuxiliaryTexture(1) != NULL;

  if (bHasMaterialTex)
  {
    // --- Thermal params are taken from auxiliary texture
    cfg.AddInclusionTag("AUX_TEX");
  }
  else
  {
    int iMaterialID = 0;
    if (pSurface->GetMesh() != NULL)
      iMaterialID = (int)pSurface->GetMesh()->GetBaseSubmesh(0)->GetGeometryInfo().m_sUserFlags;
      
    // --- Thermal params are taken from submesh user flags
    pszParamPos += sprintf(pszParamPos, "ThermalMaterialID=%i;", iMaterialID);
  }

  if (bHasHotSpotTex)
    cfg.AddInclusionTag("HEATING_POWER");

  VCompiledEffect* pFX = Vision::Shaders.CreateEffect("InitialPass", szParam, EFFECTCREATEFLAG_NONE, pEffectLib);
  if (pFX == NULL)
  {
    hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaderMain: Failed to create InitialPass effect");
    return NULL;
  }
  
  VCompiledTechnique* pTechnique = pFX->FindCompatibleTechnique(&cfg);
  if (pTechnique == NULL)
  {
    hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaderMain: No compatible technique found; using default technique");
    pTechnique = pFX->GetDefaultTechnique(); // find default technique
  }

  VASSERT(pTechnique != NULL && pTechnique->GetShaderCount() == 1);
  pTechnique->GetShader(0)->m_bModified = true;

  return pTechnique;
}

VCompiledTechnique* VTerrainDecorationEntityModel::RecreateIRShaderPre(VisSurface_cl* pSurface, VShaderEffectLib* pEffectLib)
{
  if (pSurface == NULL)
    return NULL;

  // check for supported transparency types
  if (pSurface->GetTransparencyType() != VIS_TRANSP_NONE &&
      pSurface->GetTransparencyType() != VIS_TRANSP_ALPHATEST &&
      pSurface->GetTransparencyType() != VIS_TRANSP_ALPHA)
    return NULL;

  char szParam[1024];
  VTechniqueConfig baseCfg("WIND", NULL);

  switch (pSurface->GetGeometryTopology())
  {
    case VisSurface_cl::VGT_3DMesh:
      baseCfg.AddInclusionTag("GEO_3D");
      break;

    default:
      hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaderPre: Only 3DMesh geometry topology is supported");
      return NULL;
  }

  hkvVec4 vWindPhaseParams(0.0071f, 0.0092f, 0.0f, 0.0f);
  const hkvAlignedBBox& bbox = m_spMesh->GetBoundingBox();
  if (bbox.isValid())
    vWindPhaseParams.w = -bbox.m_vMin.z;

  szParam[0] = '\0';
  char* pszParamPos = szParam;
  pszParamPos += sprintf(pszParamPos, "WindPhaseParams=%g,%g,%g,%g;", vWindPhaseParams.x, vWindPhaseParams.y, vWindPhaseParams.z, vWindPhaseParams.w);

  VTechniqueConfig cfg(baseCfg);

  if (pSurface->GetTransparencyType() == VIS_TRANSP_ALPHATEST || pSurface->GetTransparencyType() == VIS_TRANSP_ALPHA)
  {
    cfg.AddInclusionTag("ALPHATEST");
    pszParamPos += sprintf(pszParamPos, "AlphaTestThreshold=%g;", pSurface->GetAlphaTestThreshold());
  }

  VCompiledEffect* pFX = Vision::Shaders.CreateEffect("PrePass", szParam, EFFECTCREATEFLAG_NONE, pEffectLib);
  if (pFX == NULL)
  {
    hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaderPre: Failed to create PrePass effect");
    return NULL;
  }

  VCompiledTechnique* pTechnique = pFX->FindCompatibleTechnique(&cfg);
  if (pTechnique == NULL)
  {
    hkvLog::Warning("VTerrainDecorationEntityModel::RecreateIRShaderPre: No compatible technique found; using default technique");
    pTechnique = pFX->GetDefaultTechnique(); // find default technique
  }

  VASSERT(pTechnique != NULL && pTechnique->GetShaderCount() == 1);
  pTechnique->GetShader(0)->m_bModified = true;

  return pTechnique;
}

char* VTerrainDecorationEntityModel::GetParamStringForIRSurface(VisSurface_cl* pSurface, char* pszParam)
{
  char szTexName[FS_MAX_PATH];
  szTexName[0] = '0';
  const char* pTexFilename = pSurface->GetTextureFile(VisSurfaceTextures_cl::VTT_Diffuse);
  if (pTexFilename != NULL)
  {
    // Remove extension and directory from the filename
    VPathHelper::GetFilenameNoExt(szTexName, VPathHelper::GetFilename(pTexFilename));
  }

  char* pszParamPos = pszParam;

  pszParamPos += sprintf(pszParam, "MixingStretchTex=%s;", szTexName);

  pszParamPos += sprintf(pszParamPos, "CullMode=%s;DepthWrite=%s;PassType=%s;",
                         pSurface->IsDoubleSided() ? "none" : "back",
                         pSurface->IsDepthWriteEnabled() ? "true" : "false",
                         VPassTypeToString(VPT_AUTO));

  if (pSurface->GetDepthBias() != 0.0f)
    pszParamPos += sprintf(pszParamPos, "DepthBias=%g;", pSurface->GetDepthBias());
  if (pSurface->GetDepthBiasClamp() != 0.0f)
    pszParamPos += sprintf(pszParamPos, "DepthBiasClamp=%g;", pSurface->GetDepthBiasClamp());
  if (pSurface->GetSlopeScaledDepthBias() != 0.0f)
    pszParamPos += sprintf(pszParamPos, "SlopeScaledDepthBias=%g;", pSurface->GetSlopeScaledDepthBias());

  return pszParamPos;
}

/*
 * Havok SDK - Base file, BUILD(#20140711)
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
