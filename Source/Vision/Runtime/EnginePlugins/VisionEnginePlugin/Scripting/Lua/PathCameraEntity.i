/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor PathCameraEntity;
%nodefaultdtor PathCameraEntity;

class PathCameraEntity : public VisBaseEntity_cl
{
public:
  bool LoadScriptFile(const char *szFilename);

  void FreeScriptFile();

  bool Start();

  void Stop();

  void SetPause(bool bStatus);

  void SetTimeScaling(float fScale);

  float GetTimeScaling() const;

  bool IsPlaying() const;

  bool IsPaused() const;

  void AttachToRendererNode(int iRendererNodeIndex);
};

#else

/// \brief Lua wrapper class for PathCameraEntity.
class PathCameraEntity : public VisBaseEntity_cl
{
public:

  /// @name Common Path Functions
  /// @{
  
  ///\brief
  ///  Load an XML camera script file from file and start it
  ///
  ///\param filename
  ///  Filename of the XML file
  ///
  ///\returns
  ///  true if loading was successful
  boolean LoadScriptFile(string filename);

  ///\brief
  ///  Unload the XML file
  void FreeScriptFile();

  ///\brief
  ///  Start the playback (i.e. attach camera and process the camera script)
  boolean Start();

  ///\brief
  ///  Stop the playback
  void Stop();

  ///\brief
  ///  Pauses or resumes the playback
  void SetPause(boolean status);

  ///\brief
  ///  Sets the relative playback speed (1.0f = normal speed as defined in the XML, 2.0f is twice the speed)
  void SetTimeScaling(number scale);

  ///\brief
  ///  Return the time scaling of this camera. The default is 1.0
  number GetTimeScaling();

  ///\brief
  ///  Indicates whether playback is running or not
  boolean IsPlaying();

  ///\brief
  ///  Indicates whether playback is currently paused
  boolean IsPaused();

  ///\brief
  ///  Attaches this camera to the renderer node of the given index.
  void AttachToRendererNode(number rendererNodeIndex);

  /// @}
};

#endif

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
