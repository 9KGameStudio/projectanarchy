/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_RESOURCESHAPE_HPP_INCLUDED
#define V_HAVOK_RESOURCESHAPE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokElementManager.hpp>

#pragma managed(push, off)
#include <Common/Base/hkBase.h>
#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Base/System/Io/Writer/Buffered/hkBufferedStreamWriter.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Base/Reflection/hkClassMemberAccessor.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#include <Physics2012/Utilities/Serialize/hkpPhysicsData.h>
#pragma managed(pop)

class vHavokResourceShape;

/// \brief
///   Internal class to load a Havok resource asynchronously
class vHavokResourceLoadingTask : public VThreadedTask
{
public:
  vHavokResourceLoadingTask() {m_pShape=NULL;m_bInstantiate=true;}
  virtual ~vHavokResourceLoadingTask() {}
  VHAVOK_IMPEXP virtual void Run(VManagedThread *pThread) HKV_OVERRIDE;
  V_DECLARE_DYNCREATE(vHavokResourceLoadingTask); // just for the resource viewer tool

  vHavokResourceShape *m_pShape;
  IVFileInStream* m_pStream;
  bool m_bInstantiate;
};


/// \brief
///   Represents a Havok resource file (e.g. hkt) that is loaded and (typically) instantiated in the scene
class vHavokResourceShape : public VisTypedEngineObject_cl, public VRefCounter
{
public:
  VHAVOK_IMPEXP vHavokResourceShape();
  VHAVOK_IMPEXP virtual ~vHavokResourceShape();
  VHAVOK_IMPEXP virtual void DisposeObject() HK_OVERRIDE;

  /// \brief
  ///   Load the Havok resource with the passed filename. If bInstantiate=true is passed, content is actually added to the world (e.g. rigid bodies to the physics world)
  VHAVOK_IMPEXP bool LoadFromFile(const char *szFilename, bool bInstantiate);

  /// \brief
  ///   Releases the resource. If objects have been instantiated, then this function removes them again from the world
  VHAVOK_IMPEXP void Unload();

  /// \brief
  ///   Indicates whether the file has been loaded properly
  inline bool IsLoaded() const {return m_bIsLoaded;}

  /// \brief
  ///   Wait for the loading/instantiation task to finish 
  inline void EnsureTasksFinished() 
  {
    Vision::GetThreadManager()->WaitForTask(&m_LoadingTask, true);
  }

  V_DECLARE_SERIAL_DLLEXP(vHavokResourceShape, VHAVOK_IMPEXP);
  VHAVOK_IMPEXP virtual void Serialize(VArchive& ar ) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void OnSerialized(VArchive& ar) HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  VHAVOK_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  VHAVOK_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

protected:
  void Internal_LoadResource(const char *szFilename);

public: // properties:
  VString m_sFilename;
  bool m_bLoadAsynchronously, m_bIsLoaded, m_bInstantiated, m_bActiveAtStartup, m_bQueued;
  hkvAlignedBBox m_BoundingBox;

  hkRefPtr<hkObjectResource> m_spHavokResource;
  vHavokResourceLoadingTask m_LoadingTask;
};


/// \brief
///   Helper collection class for instances of type vHavokResourceShape
class vHavokResourceShapeCollection : public VRefCountedCollection<vHavokResourceShape>
{
public:
  /// \brief
  ///   Call the DisposeObject function on each element and clear the list afterwards
  inline void DisposeAll()
  {
    for (int i=Count()-1;i>=0;i--)
      GetAt(i)->DisposeObject();
    Clear();
  }

};


#endif // V_HAVOK_RESOURCESHAPE_HPP_INCLUDED

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
