/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef vPlayerApp_h__
#define vPlayerApp_h__

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Tools/vPlayer/vPlayerAppConfig.hpp>

class VPlayerApp : public VAppImpl
{
public:
  VPlayerApp();
  virtual ~VPlayerApp();

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE;
  virtual void PreloadPlugins() HKV_OVERRIDE;
  virtual void AfterEngineInit() HKV_OVERRIDE;

  virtual void Init() HKV_OVERRIDE;
  virtual void DeInit() HKV_OVERRIDE;

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  
  virtual bool Run() HKV_OVERRIDE;

private:
  void ParseCommandLine();
  void PreloadHavokPlugins();
  void PreloadThirdPartyPlugins();

  void SetWindowIcon();

  VPlayerAppConfig m_Config;
};

#endif // vPlayerApp_h__

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
