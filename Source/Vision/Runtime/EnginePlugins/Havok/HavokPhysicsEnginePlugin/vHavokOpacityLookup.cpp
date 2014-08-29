/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokOpacityLookup.hpp>


vHavokOpacityMapManager vHavokOpacityMapManager::g_GlobalManager;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// class vHavokOpacityMap
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vHavokOpacityMap::vHavokOpacityMap()
  : VManagedResource(&vHavokOpacityMapManager::GetManager())
{
  m_fSizeX = m_fSizeY = 0.f;
  m_iSizeX = m_iSizeY = 1;
  m_Bits.AllocateBitfield(1); // does not allocate
  m_Bits.SetAll(); // opaque by default
}

vHavokOpacityMap::~vHavokOpacityMap()
{
}

  
bool vHavokOpacityMap::IsOpaqueAtTexel(int x, int y)
{
  EnsureLoaded();
  x %= m_iSizeX;
  y %= m_iSizeY;
  if (x<0)
    x += m_iSizeX;
  if (y<0)
    y += m_iSizeY;

  int iIndex = y*m_iSizeX + x;
  bool bResult = m_Bits.IsBitSet(iIndex);
  return bResult;
}



BOOL vHavokOpacityMap::Reload()
{
  IVFileInStream *pIn = vHavokOpacityMapManager::GetManager().CreateFileInStream(GetOriginalFilename(), this);
  VTextureLoader loader;
  if (pIn==NULL || !loader.Open(pIn,true,0))
  {
    Vision::Error.AddReportGroupEntry(VIS_REPORTGROUPTYPE_MISSING_BITMAP, GetOriginalFilename());
    return TRUE; // flag as loaded anyway
  }

  // allocate bitfield
  m_iSizeX = (int)loader.m_Header.dwWidth;
  m_iSizeY = (int)loader.m_Header.dwHeight;
  m_fSizeX = (float)m_iSizeX - HKVMATH_LARGE_EPSILON;
  m_fSizeY = (float)m_iSizeY - HKVMATH_LARGE_EPSILON;
  int iBits = m_iSizeX*m_iSizeY;
  m_Bits.AllocateBitfield(iBits);
  m_Bits.Clear();

  int iBit = 0;
  const UBYTE iAlphaThreshold = 64; // 25% threshold
  VColorRef *pImg = loader.NextRGBAImage();
  for (int y=0;y<m_iSizeY;y++)
    for (int x=0;x<m_iSizeX;x++, pImg++, iBit++)
    {
      if (pImg->a>iAlphaThreshold)
        m_Bits.SetBit(iBit);
    }

  loader.Close();

  return TRUE;
}

BOOL vHavokOpacityMap::Unload()
{
  m_fSizeX = m_fSizeY = 0.f;
  m_iSizeX = m_iSizeY = 1;

  m_Bits.FreeBitfield();
  m_Bits.AllocateBitfield(1); // does not allocate
  m_Bits.SetAll(); // opaque by default

  return TRUE;
}

  
void vHavokOpacityMap::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
  VManagedResource::AccumulateMemoryFootprint(iUniqueSys, iUniqueGPU, iDependentSys, iDependentGPU);
  iUniqueSys += m_Bits.GetIntCount()*sizeof(int);
}


#ifdef _VISION_WIN32

IVResourcePreview *vHavokOpacityMap::CreateResourcePreview()
{
  return new vHavokOpacityMapPreview(this);
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// class vHavokOpacityMapPreview
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _VISION_WIN32

void vHavokOpacityMapPreview::OnActivate()
{
  vHavokOpacityMap *pMap = (vHavokOpacityMap *)GetOwner();
  m_spTexture = Vision::TextureManager.Create2DTextureObject("<OpacityPreview>", pMap->m_iSizeX, pMap->m_iSizeY, 1, VTextureLoader::L8);
  m_spTexture->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  // convert into luminance texture
  for (int y=0;y<pMap->m_iSizeY;y++)
  {
    UBYTE row[8192];
    for (int x=0;x<pMap->m_iSizeX;x++)
      row[x] = pMap->IsOpaqueAtTexel(x,y) ? 255 : 0;
    m_spTexture->UpdateRect(0,0,y,pMap->m_iSizeX,1,-1, row, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  }
}

void vHavokOpacityMapPreview::OnDeActivate()
{
  m_spTexture = NULL;
}

void vHavokOpacityMapPreview::OnRender(int iFlags)
{
  if (m_spTexture==NULL)
    return;

  VSimpleRenderState_t iProperties = VisFont_cl::DEFAULT_STATE;
  hkvVec2 vPos1(32.f,32.f);
  hkvVec2 vSize((float)m_spTexture->GetTextureWidth(),(float)m_spTexture->GetTextureHeight());

  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  pRI->DrawTexturedQuad(vPos1,vPos1+vSize,m_spTexture,hkvVec2(0,0), hkvVec2(1,1), V_RGBA_WHITE, iProperties);
  Vision::RenderLoopHelper.EndOverlayRendering();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// class vHavokOpacityMapManager
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vHavokOpacityMapManager &vHavokOpacityMapManager::GetManager()
{
  return g_GlobalManager;
}

VManagedResource *vHavokOpacityMapManager::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
  vHavokOpacityMap *pRes = new vHavokOpacityMap();
  pRes->SetFilename(szFilename);
  pRes->EnsureLoaded();
  return pRes;
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
