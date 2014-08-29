/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_OPACITYLOOKUP_HPP_INCLUDED
#define V_HAVOK_OPACITYLOOKUP_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>



/// \brief
///   Resource class that loads a texture file and stores the opacity in a 1-bit per pixel bitfield
///
/// This class is used for raycasts through alpha-tested materials, such as foliage. A texture can be loaded through
/// the global vHavokOpacityMapManager::GetManager() instance
class vHavokOpacityMap : public VManagedResource
{
public:
  VHAVOK_IMPEXP vHavokOpacityMap();
  VHAVOK_IMPEXP virtual ~vHavokOpacityMap();

  /// \brief
  ///   Lookup function to test opacity at given u,v coordinates. This function performs automatic wrapping for coordinates outside the [0..1] range
  inline bool IsOpaqueAtUV(float u, float v) 
  {
    return IsOpaqueAtTexel((int)(u*m_fSizeX), (int)(v*m_fSizeY));
  }
  
  /// \brief
  ///   Lookup function to test opacity at given texel coordinates. This function performs automatic wrapping for coordinates outside the [0..m_iSizeX-1, 0..m_iSizeY-1] range
  VHAVOK_IMPEXP bool IsOpaqueAtTexel(int x, int y);

  // resource overloads
  VHAVOK_IMPEXP virtual BOOL Reload() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual BOOL Unload() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) HKV_OVERRIDE {return sprintf(szDestBuffer,"%i*%i", m_iSizeX,m_iSizeY);}
#ifdef _VISION_WIN32
	VHAVOK_IMPEXP virtual IVResourcePreview *CreateResourcePreview() HKV_OVERRIDE;
#endif

public:
  int m_iSizeX,m_iSizeY;      ///< Size of the texture
  float m_fSizeX, m_fSizeY;   ///< Internal values for faster access
  VTBitfield<16> m_Bits;      ///< The actual bitfield
};

#ifdef _VISION_WIN32

/// \brief
///   Internal class to show a preview of the opacity map
class vHavokOpacityMapPreview : public IVResourcePreview
{
public:
  vHavokOpacityMapPreview(vHavokOpacityMap *pOwner) : IVResourcePreview(pOwner) {}
	virtual void OnRender(int iFlags) HKV_OVERRIDE;
	virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnDeActivate() HKV_OVERRIDE;

  VTextureObjectPtr m_spTexture;
};

#endif

/// \brief
///   Manager class that holds all resources of type vHavokOpacityMap. A singleton instance can be accessed via vHavokOpacityMapManager::GetManager
class vHavokOpacityMapManager : public VisResourceManager_cl
{
public:
  vHavokOpacityMapManager()
    : VisResourceManager_cl("RaycastOpacityMaps", VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER)
  {
  }

  /// \brief
  ///   Creates an actual new instance. To load a resource, use the LoadResource function.
  VHAVOK_IMPEXP virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo) HKV_OVERRIDE;

  /// \brief
  ///   Static function to access the manager instance
  static VHAVOK_IMPEXP vHavokOpacityMapManager &GetManager();

private:
  static vHavokOpacityMapManager g_GlobalManager;
};


#endif // V_HAVOK_OPACITYLOOKUP_HPP_INCLUDED

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
