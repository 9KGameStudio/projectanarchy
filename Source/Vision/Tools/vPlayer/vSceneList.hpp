/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef vSceneList_h__
#define vSceneList_h__

#include <Vision/Tools/vPlayer/IDataProvider.hpp>

#include <Vision/Runtime/Base/System/VDateTime.hpp>

struct VSceneListEntry
{
  inline VSceneListEntry() 
  {
    uiTargetPlatforms = 0;
    bAnyCached = false;
  }
  
  bool operator==(const VSceneListEntry& other) const;
  bool operator!=(const VSceneListEntry& other) const;
  bool operator<(const VSceneListEntry& other) const;

  hkUint32 uiTargetPlatforms;

  VString sDisplayName;
  VDateTime LastUsed;

  VString sScenePath;
  VArray<VString> sSearchPaths;

  VString sRoot;

  VTextureObjectPtr spThumbnail;
  bool bAnyCached;
};

class VSceneList : public VArray<VSceneListEntry*>
{
public:
  VSceneList();
  ~VSceneList();

  hkvResult LoadFromFile(const char* szFilePath, hkUint32 uiTargetPlatformFilterMask);
  hkvResult SaveToFile() const;

  void Reset();
  void Reload();

  void Sort();

private:
  VString PlatformMaskToString(hkUint32 targetPlatformFilterMask) const;

  VString m_sFilePath;
  hkUint32 m_uiTargetPlatformFilterMask;
};

/// \brief
///   Callback Data Object send by the UI implementation once a scene should be loaded, removed from the list etc.
class VSceneListEvent : public IVisCallbackDataObject_cl
{
public:
  enum Action
  {
    LOAD_SCENE,
    CLEAR_CACHE,
    REMOVE_FROM_LIST,
    DATA_CHANGED
  };

  inline VSceneListEvent(VCallback *pSender, int iListIndex, int iEntryIndex, Action eAction) : 
    IVisCallbackDataObject_cl(pSender),
    m_iListIndex(iListIndex),
    m_iEntryIndex(iEntryIndex),
    m_eAction(eAction)
  {
  }

  inline int GetListIndex() const { return m_iListIndex; }
  inline int GetEntryIndex() const { return m_iEntryIndex; }
  inline Action GetAction() const { return m_eAction; }

private:
  int m_iListIndex;
  int m_iEntryIndex;
  Action m_eAction;
};

class ISceneListDataProvider : public IDataProvider
{
public:
  struct SpecificFlags
  {
    enum Enum
    {
      CACHE_ENABLED = V_BIT(1)
    };
  };

  ISceneListDataProvider(hkUint32 uiFlags) : IDataProvider(uiFlags) {}

  virtual int GetSize() const = 0;
  virtual VSceneListEntry& GetAt(int iIndex) = 0;
  virtual const VSceneListEntry& GetAt(int iIndex) const = 0;

  virtual int Find(const VSceneListEntry& entry) const = 0;

  inline bool IsCacheEnabled() const { return (GetFlags() & SpecificFlags::CACHE_ENABLED) != 0; }
};

#endif // vSceneList_h__

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
