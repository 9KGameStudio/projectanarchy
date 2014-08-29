/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/Tools/AutoPlacementTool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VGroupInstance.hpp>
#include <Vision/Runtime/Base/Math/Random/VRandom.hpp>
#include <Vision/Runtime/Base/Physics/VBBoxOctree.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#using <mscorlib.dll>

// TODO:
//  - cant switch to baked mode when empty - should be able to generate unique name in prefab

// Nice to have
//  - instances align to ground non-vertically?
//  - better string display of include/exclude materials
//  - recalc button in decoration UI
//  - decoration grid shape - how to sort into zones?
//  - recalc through marquee in 2D view? Requires grid shape...


// DONE:
//  - prefab support
//  - hotspot add material (picker)
//  - AUX textures
//  - Hotspot size
//  - render display
//  - instances configurable yaw
//  - instances scaling/variation
//  - improve adding-speed for non-baked code path
//  - description for everything
//  - code comment


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VPlacementDensityMap
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// helper class that defines a n*m array of float values interpreted as density
class VPlacementDensityMap
{
public:
  // Constructor that allocates the float array
  VPlacementDensityMap(int iSizeX, int iSizeY, float fInitVal=0.0f)
  {
    m_iSizeX = iSizeX;
    m_iSizeY = iSizeY;
    const int iCount = m_iSizeX*m_iSizeY;
    m_pDensity = new float[iCount];
    for (int i=0;i<iCount;i++)
      m_pDensity[i] = fInitVal;
  }

  ~VPlacementDensityMap()
  {
    V_SAFE_DELETE_ARRAY(m_pDensity);
  }

  // Get a value reference for the passed coordinates. Coordinates must be in valid range.
  float& GetAt(int x, int y)
  {
    VASSERT(x>=0 && x<m_iSizeX && y>=0 && y<m_iSizeY);
    int iIndex = y*m_iSizeX+x;
    return m_pDensity[iIndex];
  }

  int m_iSizeX, m_iSizeY;
  float *m_pDensity;
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VPlacementMaterialInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// additional temporary information that will be attached as user data to engine materials for faster processing
class VPlacementMaterialInfo
{
public:
  VPlacementMaterialInfo(VisSurface_cl *pMaterial)
  {
    m_pMaterial = pMaterial;
    m_iBorderTexel = 0;
    m_fBorderOrDensity = 0.f;
    m_bIsInclude = true;
  }
  bool m_bIsInclude;          // inclusion or eclusion material?
  int m_iBorderTexel;         // amount of border texels in the density map. Ignored for inclusion materials
  float m_fBorderOrDensity;   // density value for inclusion materials, border in units for exclusion materials
  VisSurface_cl *m_pMaterial; // pointer to owner surface

  VisBitmapPtr m_spDensityTexture;  // optional density texture (inclusion only). Converted to bitmap for CPU lookups
  hkvVec4 m_DensityMask;      // mask to apply to m_spDensityTexture to generate density scalar (via dot4 with texel color)
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class PlacementMaterialBase
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool PlacementMaterialBase::MatchesMaterialName(const char *szName)
{
  String ^s = gcnew String(szName);
  return MatchesMaterialName(s);
}

bool PlacementMaterialBase::MatchesMaterialName(String ^sName)
{
  bool bResult = false;
  if (_bIsRegEx)
  {
    bResult = System::Text::RegularExpressions::Regex::IsMatch(sName, _materialName, System::Text::RegularExpressions::RegexOptions::IgnoreCase);
  }else
  {
    bResult = String::Compare(sName,_materialName,true)==0;
  }
  
  return bResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class PlacementMaterialInclude
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PlacementMaterialInclude::DesityIsAuxTexture()
{
  if (_densityTexture==AUX0_TEXTURE) return 0;
  if (_densityTexture==AUX1_TEXTURE) return 1;
  if (_densityTexture==AUX2_TEXTURE) return 2;
  if (_densityTexture==AUX3_TEXTURE) return 3;
  if (_densityTexture==AUX4_TEXTURE) return 4;
  if (_densityTexture==AUX5_TEXTURE) return 5;
  if (_densityTexture==AUX6_TEXTURE) return 6;
  if (_densityTexture==AUX7_TEXTURE) return 7;
  return -1;
}

VTextureObject *PlacementMaterialInclude::GetMaterialTexture(VisSurface_cl *pSurface, bool &bIsSurfaceTexture)
{
  bIsSurfaceTexture = false;
  if (String::IsNullOrEmpty(_densityTexture))
    return NULL;
  if (_densityTexture==DIFFUSE_TEXTURE)
  {
    bIsSurfaceTexture = true;
    return pSurface->GetBaseTextureObject();
  }
  int iAux = DesityIsAuxTexture();
  if (iAux>=0)
  {
    bIsSurfaceTexture = true;
    if (iAux<pSurface->GetAuxiliaryTextureCount())
      return pSurface->GetAuxiliaryTexture(iAux);
    return NULL;
  }
  return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class AutoPlacementProperties
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize members in the passed VPlacementMaterialInfo
void AutoPlacementProperties::InitializeMaterial(VPlacementMaterialInfo *pInfo)
{
  // check include materials
  if (MaterialIncludes!=nullptr)
  {
    for (int i=0;i<MaterialIncludes->Length;i++)
    {
      PlacementMaterialInclude ^incl = MaterialIncludes[i];
      if (!incl->MatchesMaterialName(pInfo->m_pMaterial->GetName()))
        continue;
      pInfo->m_bIsInclude = true;
      pInfo->m_fBorderOrDensity = incl->_density;

      // load density texture as bitmap
      if (!String::IsNullOrEmpty(incl->_densityTexture))
      {
        VString sTexFile;
        bool bIsSurfaceTex;
        VTextureObject *pSurfaceTex = incl->GetMaterialTexture(pInfo->m_pMaterial, bIsSurfaceTex);
        if (bIsSurfaceTex)
        {
          if (pSurfaceTex!=NULL)
            sTexFile = pSurfaceTex->GetOriginalFilename();
        } else
        {
          // load texture directly:
          ConversionUtils::StringToVString(incl->_densityTexture,sTexFile);
        }

        // load as bitmap:
        if (!sTexFile.IsEmpty())
          pInfo->m_spDensityTexture = VisBitmap_cl::LoadBitmapFromFile(sTexFile);
        if (pInfo->m_spDensityTexture!=NULL)
        {
          pInfo->m_spDensityTexture->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
          pInfo->m_DensityMask.set(incl->_densityTextureMask.X,incl->_densityTextureMask.Y,incl->_densityTextureMask.Z,incl->_densityTextureMask.W);
        }
      }
      return; // material found -> done with initialization
    }
  }

  // check exclude materials
  if (MaterialExcludes!=nullptr)
  {
    for (int i=0;i<MaterialExcludes->Length;i++)
    {
      PlacementMaterialExclude ^excl = MaterialExcludes[i];
      if (!excl->MatchesMaterialName(pInfo->m_pMaterial->GetName()))
        continue;
      pInfo->m_bIsInclude = false;
      pInfo->m_fBorderOrDensity = excl->_border + m_fGridSpacing*0.999f; // biased border
      pInfo->m_iBorderTexel = (int)(pInfo->m_fBorderOrDensity / m_fGridSpacing);
      return; // material found -> done with initialization
    }
  }

  // default:
  pInfo->m_bIsInclude = true;
  pInfo->m_fBorderOrDensity = 0.f; // if set to > 0.0 then all materials that are not defined in either list will receive decoration
}



#define FOREACH_MATERIAL \
  for (int i=0;i<(int)geometry.GetNumEntries();i++)\
  {\
    VisStaticGeometryInstance_cl *pInst = geometry.GetEntry(i);\
    VisSurface_cl *pMaterial = pInst->GetSurface();\
    if (pMaterial==NULL)\
      continue;


// Create additional material information for all geometry passed in the collection
void CreateMaterialInfo(AutoPlacementProperties ^properties, VisStaticGeometryInstanceCollection_cl &geometry)
{
  // make sure no user data hangs around
  FOREACH_MATERIAL
    if (pMaterial->GetUserData()==NULL)
      continue;
    hkvLog::Warning("Material '%s' already has user data - overwriting it...",pMaterial->GetName());
    pMaterial->SetUserData(NULL);
  }

  FOREACH_MATERIAL
    if (pMaterial->GetUserData()!=NULL) // already assigned
      continue;
    VPlacementMaterialInfo *pInfo = new VPlacementMaterialInfo(pMaterial);
    properties->InitializeMaterial(pInfo);
    pMaterial->SetUserData(pInfo);
  }

}


// destroy additional material information for all geometry passed in the collection (cleanup time)
void DestroyMaterialInfo(VisStaticGeometryInstanceCollection_cl &geometry)
{
  FOREACH_MATERIAL
    if (pMaterial->GetUserData()!=NULL)
    {
      VPlacementMaterialInfo *pInfo = (VPlacementMaterialInfo *)pMaterial->GetUserData();
      delete pInfo;
      pMaterial->SetUserData(NULL);
    }
  }
}


bool Helper_IsConsistentSetting(VisStaticGeometryInstanceCollection_cl &traceGeometry, float &fDensityOrBorder, bool &bIsInclude)
{
  bool bFirst = true;
  for (int i=0;i<traceGeometry.GetNumEntries();i++)
  {
    VisSurface_cl *pSurface = traceGeometry.GetEntry(i)->GetSurface();
    if (pSurface==NULL || pSurface->GetUserData()==NULL)
      continue;
    VPlacementMaterialInfo *pMatInfo = (VPlacementMaterialInfo *)pSurface->GetUserData();
    if (bFirst)
    {
      bFirst = false;
      bIsInclude = pMatInfo->m_bIsInclude;
      fDensityOrBorder = pMatInfo->m_fBorderOrDensity;
    } else
    {
      if (bIsInclude != pMatInfo->m_bIsInclude || fDensityOrBorder != pMatInfo->m_fBorderOrDensity)
        return false;
    }

  }

  return bFirst==false; // found any?
}


bool Helper_GetAverageDensity(const hkvVec3 &vStart, float fBottom, float fGridSpacing, VisStaticGeometryInstanceCollection_cl &traceGeometry, float &fDensityOrBorder, bool &bIsInclude)
{
  if (Helper_IsConsistentSetting(traceGeometry, fDensityOrBorder, bIsInclude))
    return true;

  VisTraceLineInfo_t traceResult;
  const int iSubDiv = 4;
  int iIncludeCount = 0;
  float fSumDensity = 0.f;
  float fMaxExclude = 0.f;
  float fStep = fGridSpacing / (float)iSubDiv;
  bIsInclude = true;
  for (int y=0;y<iSubDiv;y++)
    for (int x=0;x<iSubDiv;x++)
    {
      float fOfsX = (x&1) ? fGridSpacing*0.5f : 0.f; // alternating half offset
      hkvVec3 vRayStart(vStart.x + fStep*(float)x + fOfsX, vStart.y + fStep*(float)y,vStart.z);
      hkvVec3 vRayEnd(vRayStart.x,vRayStart.y, fBottom);
      if (Vision::CollisionToolkit.TraceLineStaticGeometry(vRayStart,vRayEnd, traceGeometry, 1, &traceResult)==0)
        continue;
      if (traceResult.pSurface==NULL || traceResult.pSurface->GetUserData()==NULL)
        continue;
       VPlacementMaterialInfo *pMatInfo = (VPlacementMaterialInfo *)traceResult.pSurface->GetUserData();
       if (pMatInfo->m_bIsInclude)
       {
         iIncludeCount++;
         fSumDensity += pMatInfo->m_fBorderOrDensity;
       }
       else
       {
         bIsInclude = false; // will be false as soon as any material is exclude
         fMaxExclude = hkvMath::Max(fMaxExclude, pMatInfo->m_fBorderOrDensity);
       }
    }

  if (bIsInclude)
  {
    if (iIncludeCount<1)
      return false;
    fDensityOrBorder = fSumDensity / (float)(iSubDiv*iSubDiv); // iIncludeCount;
  } else
  {
    fDensityOrBorder = fMaxExclude;
  }
  return true;
}

// Generate the actual placement
String^ AutoPlacementTool::GeneratePlacement(AutoPlacementProperties ^properties, float x1, float y1, float x2, float y2, float fModelSize, String ^absFilename, bool bAddFileRCS, ProgressStatus ^progress)
{
  // we need surface and UV information
  int iOldSettings = Vision::CollisionToolkit.GetTraceLineSettings();
  Vision::CollisionToolkit.SetTraceLineSettings(iOldSettings | VIS_TRACESETTINGS_STOREBASEUV | VIS_TRACESETTINGS_STORESURFACEINFO);

  VRandom random(0);
  const float fGlobalDensity = properties->Density;
  const float fAverageScale = properties->AverageScale;
  const float fGridSpacing = fModelSize*fAverageScale / fGlobalDensity;
  const float fGridCenter = fGridSpacing*0.5f;
  if (fGridSpacing<=HKVMATH_HUGE_EPSILON)
    return "invalid decoration model or too high density";

  properties->m_fGridSpacing = fGridSpacing;

  VString sFilename;
  ConversionUtils::StringToVString(absFilename, sFilename);

  if (bAddFileRCS)
    ManagedBase::RCS::GetProvider()->EditFile(absFilename);

  IVFileOutStream *pOut = Vision::File.Create(sFilename);
  if (pOut==NULL)
    return "Failed to open output file";


  int iInstanceCount = 0;
  DynObjArray_cl<VGroupInstance> instances(0);

  if (progress!=nullptr)
    progress->StatusString = "Create material information";

  // gather all static meshes relevant for tracing...
  float fLarge = 9999999999.f;
  hkvAlignedBBox overallBox(hkvVec3(x1,y1,-fLarge), hkvVec3(x2,y2,fLarge));
  VisStaticGeometryInstanceCollection_cl relevantGeometry(0);
  VisStaticGeometryInstanceCollection_cl traceGeometry(0);

  Vision::GetSceneManager()->GatherStaticGeometryInBoundingBox(overallBox, relevantGeometry);
  if (relevantGeometry.GetNumEntries()==0)
    return "No geometry to place the decoration";

  // ...and provide all materials in there with additional information
  CreateMaterialInfo(properties, relevantGeometry);

  hkvAlignedBBox tightBBox;
  relevantGeometry.ComputeBoundingBox(tightBBox);
  float fMinZ = tightBBox.m_vMin.z-10.f;
  float fMaxZ = tightBBox.m_vMax.z+10.f;

  // determine # of samples and allocate density map
  const int iCountX = (int)((x2-x1) / fGridSpacing);
  const int iCountY = (int)((y2-y1) / fGridSpacing);
  if (iCountX<2 || iCountY<2)
    return "Area too small";
  if (iCountX*iCountY>64*1024*1024)
    return "Area too large or density too high.\nReduce density or split into smaller groups";
  VPlacementDensityMap densityMap(iCountX,iCountY);
  bool bResult = true;

  // sort geometry into an octree:
  VBBoxOctree octree;
  VPList relevantElements;
  hkvAlignedBBox traceBox;
  const float fBBoxEps = 1.0f; // epsilon for bbox overlapping checks
  {
    const int iCount = relevantGeometry.GetNumEntries();
    VBBoxOctreeNodeElemDesc *pDesc = new VBBoxOctreeNodeElemDesc[iCount];
    
    for (int i=0;i<iCount;i++)
    {
      pDesc[i].m_bBox = relevantGeometry.GetEntry(i)->GetBoundingBox();
      pDesc[i].m_bBox.addBoundary(hkvVec3(fBBoxEps)); // safety margin
      pDesc[i].m_elementID = relevantGeometry.GetEntry(i)->GetNumber();
    }
    octree.Build(pDesc, iCount, 16);
    int iMem = octree.GetMemoryConsumption();
    delete[] pDesc;
  }

  // STEP 1: fill density map with include/exclude materials:
  {
    if (progress!=nullptr)
    {
      progress->StatusString = "Generate density map";
      progress->Percentage = 5.f;
    }

    VisTraceLineInfo_t traceResult;
    for (int y=0;bResult && y<iCountY;y++)
    {
      for (int x=0;x<iCountX;x++)
      {
        float &fThisDensity(densityMap.GetAt(x,y));
        float px = x1 + (float)x*fGridSpacing;
        float py = y1 + (float)y*fGridSpacing;

        // gather relevant geometry from the octree
        traceBox.setWithoutValidityCheck(hkvVec3(px-fBBoxEps,py-fBBoxEps,fMinZ), hkvVec3(px+fGridSpacing+fBBoxEps,py+fGridSpacing+fBBoxEps,fMaxZ));
        relevantElements.Clear();
        int iRelevantCount = octree.GetRelevantElements(traceBox, relevantElements);
        traceGeometry.Clear();
        traceGeometry.EnsureSize(iRelevantCount);
        for (int i=0;i<iRelevantCount;i++)
        {
          VisStaticGeometryInstance_cl *pGI = VisStaticGeometryInstance_cl::ElementManagerGet((unsigned int)relevantElements.Get(i));
          if (!pGI->GetBoundingBox().overlaps(traceBox))
            continue;
          traceGeometry.AppendEntryFast(pGI);
        }
        if (traceGeometry.GetNumEntries()==0)
          continue;
        hkvVec3 vStart(px,py,fMaxZ);
        hkvVec3 vEnd(px,py,fMinZ);
        float fDensityOrBorder = 0.0f;
        bool bIsInclude = false;
        if (!Helper_GetAverageDensity(vStart, fMinZ, fGridSpacing, traceGeometry, fDensityOrBorder, bIsInclude))
          continue;

        if (bIsInclude)
        {
          if (fThisDensity>=0.f) // not tagged for exclude?
            fThisDensity = fDensityOrBorder * fGlobalDensity;
        } else
        {
          // tag environment of pixel for exclude
          int iBorderTexel = (int)(fDensityOrBorder/fGridSpacing);
          for (int yy=y-iBorderTexel;yy<=y+iBorderTexel;yy++)
            for (int xx=x-iBorderTexel;xx<=x+iBorderTexel;xx++)
              if (xx>=0 && yy>=0 && xx<iCountX && yy<iCountY)
              {
                // accurate texel distance check
                hkvVec2 vDist2D((float)(xx-x)*fGridSpacing, (float)(yy-y)*fGridSpacing);
                if (vDist2D.getLength()<=fDensityOrBorder)
                  densityMap.GetAt(xx,yy) = -1.f;
              }
        }

      }

      if (progress!=nullptr)
      {
        progress->Percentage = 5.0f + ((float)y)*40.f/(float)iCountY; // [5%..45%]
        if (progress->WantsAbort)
        {
          bResult = false;
          break;
        }
      }

    }
  }

  // STEP 2: generate instances using density map
  if (bResult)
  {
    if (progress!=nullptr)
      progress->StatusString = "Generate placement";
    VisTraceLineInfo_t traceResult;
    hkvVec4 vBitmapColor;
    float fVariance = fGridCenter * properties->DistributionRandomness;
    for (int y=0;bResult && y<iCountY;y++)
    {
      for (int x=0;x<iCountX;x++)
      {
        float px = x1 + (float)x*fGridSpacing + fVariance*random.GetFloatNeg() + fGridCenter;
        float py = y1 + (float)y*fGridSpacing + fVariance*random.GetFloatNeg() + fGridCenter;
        hkvVec3 vStart(px,py,fMaxZ);
        hkvVec3 vEnd(px,py,fMinZ);

        // gather relevant trace geometry from octree
        traceBox.setWithoutValidityCheck(hkvVec3(px-fBBoxEps,py-fBBoxEps,fMinZ), hkvVec3(px+fBBoxEps,py+fBBoxEps,fMaxZ));
        relevantElements.Clear();
        int iRelevantCount = octree.GetRelevantElements(traceBox, relevantElements);
        traceGeometry.Clear();
        traceGeometry.EnsureSize(iRelevantCount);
        for (int i=0;i<iRelevantCount;i++)
        {
          VisStaticGeometryInstance_cl *pGI = VisStaticGeometryInstance_cl::ElementManagerGet((unsigned int)relevantElements.Get(i));
          if (!pGI->GetBoundingBox().overlaps(traceBox))
            continue;
          traceGeometry.AppendEntryFast(pGI);
        }

        if (traceGeometry.GetNumEntries()==0 || Vision::CollisionToolkit.TraceLineStaticGeometry(vStart,vEnd, traceGeometry, 1, &traceResult)==0)
          continue;

        float fDensity = densityMap.GetAt(x,y);

        // modulate density with density texture, if present
        if (traceResult.pSurface!=NULL && traceResult.pSurface->GetUserData()!=NULL)
        {
          VPlacementMaterialInfo *pMatInfo = (VPlacementMaterialInfo *)traceResult.pSurface->GetUserData();
          if (!pMatInfo->m_bIsInclude)
            continue; // oups? we still hit an exclusion material that we did not detect before

          if (pMatInfo->m_spDensityTexture!=NULL)
          {
            pMatInfo->m_spDensityTexture->LookupTexelColor(vBitmapColor, traceResult.baseUV.x,traceResult.baseUV.y, BITMAPLOOKUPFLAG_REPEAT);
            float fScalar = vBitmapColor.dot(pMatInfo->m_DensityMask) / 255.f;
            fDensity *= fScalar;
          }
        }

        if (random.GetFloat()>fDensity)
          continue;

        if (iInstanceCount>1000000) // sanity
        {
          bResult = false;
          break;
        }

        // create the instance data
        VGroupInstance &inst(instances[iInstanceCount++]);
        float fYaw = properties->MinYaw + random.GetFloat()*(properties->MaxYaw - properties->MinYaw);
        float fScale = fAverageScale * (1.f+random.GetFloatNeg()*properties->ScaleVariation);
        inst.m_vPosition.set(px,py,traceResult.touchPoint.z + properties->HeightOffset*fScale);
        hkvMat3 scaleMat(fScale,0,0, 0,fScale,0, 0,0,fScale);
        hkvMat3 rotation;
        rotation.setFromEulerAngles(0,0,fYaw);
        inst.m_Rotation = scaleMat.multiplyReverse(rotation);
        inst.m_fSeed = random.GetFloat();
      }

      if (progress!=nullptr)
      {
        progress->Percentage = 45.f + ((float)y)*40.f/(float)iCountY; // [45%..85%]
        if (progress->WantsAbort)
        {
          bResult = false;
          break;
        }
      }
    }
  }

  // cleanup material info
  DestroyMaterialInfo(relevantGeometry);

  if (bResult && progress!=nullptr)
    progress->StatusString = "Write out file...";

  // write out instance position file
  VGroupInstanceFile file;
  file.Create(pOut, iInstanceCount, VGroupInstance::INSTANCE_TYPE_DECORATION_WS);
  for (int i=0;i<iInstanceCount;i++)
    file.WriteInstance(instances.GetDataPtr()[i]);

  file.Close(true); // closes pOut

  if (bAddFileRCS && bResult)
    ManagedBase::RCS::GetProvider()->AddFile(absFilename, true);

  if (bResult && progress!=nullptr)
  {
    progress->StatusString = "Write out file...done";
    progress->Percentage = 95.f;
  }

  if (!bResult)
    return "User abort";

  Vision::CollisionToolkit.SetTraceLineSettings(iOldSettings);

  return nullptr;
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
