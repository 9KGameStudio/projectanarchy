/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

// This interface file generates RemoteInputModule_wrapper.cpp
// and is represented in Lua as the "RemoteInput" module.

%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

%module RemoteInput
%{
  #include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
%}

%nodefaultctor IVRemoteInput;
%nodefaultdtor IVRemoteInput;

class IVRemoteInput {
public:

  bool StartServer(const char* dataDirectory, int iPort = 8080);

  void StopServer();

  void DebugDrawTouchAreas(VColorRef color);

  void DebugDrawTouchPoints(VColorRef color);

  void InitEmulatedDevices();

  void DeinitEmulatedDevices();

  bool AddVariableDirect(const char* varName, const char* value);  

  bool UpdateVariableDirect(const char* varName, const char* value);

  bool RemoveVariable(const char* varName);

  void SetResizeOnConnect(bool bOn);

  void SetSmoothTouchInput(bool bOn);

  void SetDisableMouseInput(bool bOn);

  %extend
  {
    VSWIG_CREATE_CAST_UNSAFE(IVRemoteInput)
  }
};

#else

/// \brief MODULE: The Lua \b RemoteInput module contains the wrapped class IVRemoteInput, accessible through a global variable of the same name \b RemoteInput.
class IVRemoteInput {
public:
  /// \brief
  ///   Starts the HTTP server that provides the RemoteInput app for HTML5 based devices.
  ///
  /// Before starting the remote input server, make sure that the vRemoteInput plugin has been loaded.
  ///
  /// \param dataDirectory
  ///   The directory the user GUI data is located in.
  ///
  /// \param port
  ///   The port the http server should use.
  ///
  /// \return
  ///   True on success, false otherwise.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:StartServer('RemoteGUI')
  ///   \endcode
  boolean StartServer(string dataDirectory, number port = 8080);

  /// \brief
  ///   Stops the HTTP server if it is running.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:StopServer('RemoteGUI')
  ///   \endcode
  void StopServer();

  /// \brief
  ///  Draws all touch areas on the screen using lines
  ///
  /// \param color
  ///   The color the debugging lines should have.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:DebugDrawTouchAreas(Vision.VColorRef(255,0,0))
  ///   \endcode
  void DebugDrawTouchAreas(VColorRef color);

  /// \brief
  ///  Draws all touch points on the screen using lines
  ///
  /// \param color
  ///   The color the debugging lines should have.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:DebugDrawTouchPoints(Vision.VColorRef(255,0,0))
  ///   \endcode
  void DebugDrawTouchPoints(VColorRef color);

  /// \brief
  ///  Initialized the emulated devices, this is done automatically when the vRemoteInput plugin is loaded
  ///  however as vForge deinitializes the input system after each play game session this method needs to be recalled
  ///  at the start of a play game session.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:InitEmulatedDevices()
  ///   \endcode
  void InitEmulatedDevices();

  /// \brief
  ///  Deinitialize the emulated devices. You need to call this function when manually going back to non-emulated input mode.
  void DeinitEmulatedDevices();

  /// \brief
  ///   Adds a new variable that is send to the remote device.
  ///
  /// \param varName
  ///   The name of the variable.
  /// 
  /// \param value
  ///   The value of the variable. This value is interpreted by the JavaScript engine of the device so you can use any JavaScript values
  ///   e.g. 3.45 or 'hello world'.
  ///
  /// \return
  ///   True on success, false otherwise.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:AddVariableDirect('greeting', 'hello, world')
  ///   \endcode
  boolean AddVariableDirect(string varName, string value);  

  /// \brief
  ///   Updates an existing variable.
  ///
  /// \param varName
  ///   Name of the variable.
  ///
  /// \param value
  ///   New value of the variable.
  ///
  /// \return
  ///   True on success, false otherwise.
  boolean UpdateVariableDirect(string varName, string value);

  /// \brief
  ///   Removes an existing variable.
  ///
  /// \param varName
  ///   Name of the variable.
  ///
  /// \return
  ///   True on success, false otherwise.
  boolean RemoveVariable(string varName);

  /// \brief
  ///  Enables or disables the resizing of the screen to the mobile device resolution.
  ///
  /// \param resize
  ///   True for on, false for off.
  void SetResizeOnConnect(boolean resize);

  /// \brief
  ///  Enables or disables the smoothing of the incoming touch data.
  ///
  ///  Enabled by default.
  /// 
  /// \param smoothTouchInput
  ///   True for on, false for off.
  void SetSmoothTouchInput(boolean smoothTouchInput);

  /// \brief
  ///   If the mouse input should be disabled when the remote device connects.
  ///
  /// On by default (mouse input disabled).
  ///
  /// \param disableMouseInput
  ///   True to disable mouse input, false for enable it.
  void SetDisableMouseInput(boolean disableMouseInput);
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
