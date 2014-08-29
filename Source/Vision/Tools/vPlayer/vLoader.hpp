/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef vLoader_h__
#define vLoader_h__

#include <Vision/Tools/vPlayer/vPlayerAppModule.hpp>

class SceneLoader : public ILoader
{
public:
  SceneLoader() : ILoader("vscene") {}
  ~SceneLoader() {}

  virtual bool Load(const VSceneListEntry& entry) HKV_OVERRIDE;
  virtual VString GetLastError() const HKV_OVERRIDE;
  virtual bool NeedsManualCallbackAfterLoading() const HKV_OVERRIDE { return false; }

private:
  VSceneListEntry m_sceneListEntry;
};

class MeshLoader : public ILoader
{
public:
  MeshLoader() : ILoader("vmesh") {}
  ~MeshLoader() {}

  virtual bool Load(const VSceneListEntry& entry) HKV_OVERRIDE;
  virtual VString GetLastError() const HKV_OVERRIDE;
  virtual bool NeedsManualCallbackAfterLoading() const HKV_OVERRIDE { return true; }
};

class PrefabLoader : public ILoader
{
public:
  PrefabLoader() : ILoader("vprefab") {}
  ~PrefabLoader() {}

  virtual bool Load(const VSceneListEntry& entry) HKV_OVERRIDE;
  virtual VString GetLastError() const HKV_OVERRIDE;
  virtual bool NeedsManualCallbackAfterLoading() const HKV_OVERRIDE { return true; }
};

class ZoneLoader : public ILoader
{
public:
  ZoneLoader() : ILoader("vzone") {}
  ~ZoneLoader() {}

  virtual bool Load(const VSceneListEntry& entry) HKV_OVERRIDE;
  virtual VString GetLastError() const HKV_OVERRIDE;
  virtual bool NeedsManualCallbackAfterLoading() const HKV_OVERRIDE { return true; }
};

class LitLoader : public ILoader
{
public:
  LitLoader() : ILoader("lit") {}
  ~LitLoader() {}

  virtual bool Load(const VSceneListEntry& entry) HKV_OVERRIDE;
  virtual VString GetLastError() const HKV_OVERRIDE;
  virtual bool NeedsManualCallbackAfterLoading() const HKV_OVERRIDE { return true; }
};

#endif // vLoader_h__

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
