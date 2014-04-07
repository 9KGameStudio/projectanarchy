/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ===========================================================
// Remote Input Lua Module - SWIG Interface
// ===========================================================
// This interface file generates RemoteInputModule_wrapper.cpp
// and is represented in Lua as the "RemoteInput" module.

%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

#ifndef VLUA_APIDOC
/// \brief 
///   Additional functionality provided by the RemoteInput plugin
%module RemoteInput
%{
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
%}
#endif

%nodefaultctor IVRemoteInput;
%nodefaultdtor IVRemoteInput;

/// \brief MODULE: The Lua \b IVRemoteInput module contains all generated wrapper classes of the RemoteInputEnginePlugin.
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
  /// \param iPort
  ///   The port the http server should use.
  ///
  /// \return
  ///   True on success, false otherwise.
  ///
  /// \par Example
  ///   \code
  ///     RemoteInput:StartServer('RemoteGUI')
  ///   \endcode
  bool StartServer(const char* dataDirectory, int iPort = 8080);

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
  bool AddVariableDirect(const char* varName, const char* value);  

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
  bool UpdateVariableDirect(const char* varName, const char* value);

  /// \brief
  ///   Removes an existing variable.
  ///
  /// \param varName
  ///   Name of the variable.
  ///
  /// \return
  ///   True on success, false otherwise.
  bool RemoveVariable(const char* varName);

  /// \brief
  ///  Enables or disables the resizing of the screen to the mobile device resolution.
  ///
  /// \param bOn
  ///   True for on, false for off.
  void SetResizeOnConnect(bool bOn);

  /// \brief
  ///  Enables or disables the smoothing of the incoming touch data.
  ///
  ///  Enabled by default.
  /// 
  /// \param bOn
  ///   True for on, false for off.
  void SetSmoothTouchInput(bool bOn);

  /// \brief
  ///   If the mouse input should be disabled when the remote device connects.
  ///
  /// On by default (mouse input disabled).
  ///
  /// \param bOn
  ///   True to disable mouse input, false for enable it.
  void SetDisableMouseInput(bool bOn);

  %extend
  {
    VSWIG_CREATE_CAST_UNSAFE(IVRemoteInput)
  }
};

/*
 * Havok SDK - Base file, BUILD(#20140328)
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
