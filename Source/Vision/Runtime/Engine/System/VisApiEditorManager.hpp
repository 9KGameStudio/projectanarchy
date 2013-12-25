/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiEditorManager.hpp

#ifndef VIS_API_EDITOR_MANAGER_HPP
#define VIS_API_EDITOR_MANAGER_HPP

#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>

/// \brief
///   Enum that defines bitflags for the IVisSceneExporter_cl::StartVSceneExport function
enum VSceneExportFlags_e
{
  VExport_Plugins         = V_BIT(0), ///< write out the required plugins
  VExport_SceneScript     = V_BIT(1), ///< export the scene script reference
  VExport_Sky             = V_BIT(2), ///< export the sky object
  VExport_RendererNode    = V_BIT(3), ///< export the renderer node
  VExport_TimeOfDay       = V_BIT(4), ///< export time of day handler
  VExport_Fog             = V_BIT(5), ///< export fog settings
  VExport_View            = V_BIT(6), ///< export view related settings (FOV)
  VExport_ZoneRefs        = V_BIT(7), ///< export zone references
  VExport_TimeStepping    = V_BIT(8), ///< export time stepping settings

  VExport_All = 0xffffffff            ///< export everything
};


/// \brief
///   Internal data object class used by the OnTriggerSetVariable callback
class VisEditorVariableDataObject_cl : public IVisCallbackDataObject_cl 
{
public:
  inline VisEditorVariableDataObject_cl(VisCallback_cl *pSender, VisTypedEngineObject_cl *pNativeObj, const char *szVariableName, const char *szNewValue, bool bIsSubProperty, bool bSetViaAction)
    : IVisCallbackDataObject_cl(pSender), m_pNativeObj(pNativeObj), m_szVariableName(szVariableName), m_szNewValue(szNewValue),
      m_bIsSubProperty(bIsSubProperty), m_bSetViaAction(bSetViaAction)
  {
    VASSERT(pNativeObj!=NULL);
    m_bSuccess = false;
  }

  VisTypedEngineObject_cl *m_pNativeObj;
  const char *m_szVariableName;
  const char *m_szNewValue;
  bool m_bIsSubProperty, m_bSetViaAction, m_bSuccess;
};

/// \brief
///   Internal data object class used by the OnSet3DCanvasSize
class V3DCanvasSizeDataObject : public IVisCallbackDataObject_cl
{
public:
  inline V3DCanvasSizeDataObject(VisCallback_cl *pSender, int iWidth, int iHeight)
    : IVisCallbackDataObject_cl(pSender), m_iWidth(iWidth), m_iHeight(iHeight)
  { }

  int m_iWidth,m_iHeight;
};

/// \brief
///   Archive class with some convenience overrides for loading exported vscene/vzone files.
/// 
/// For instance, it can count the number of shapes that go into the archive by overriding the
/// ReadObject/WriteObject function.
/// 
/// Used by VSceneLoader class and zone loading.
class VShapesArchive : public VArchive
{
public:

  /// \brief
  ///   Constructor for reading 
  VISION_APIFUNC VShapesArchive(IVFileInStream* pStream, VProgressStatus *pProgess);

  /// \brief
  ///   Constructor for writing 
  VISION_APIFUNC VShapesArchive(IVFileOutStream* pStream);

  /// \brief
  ///   Read object of the required type from the archive. 
  ///
  /// This function reads the type of the object as well as its data by calling the object's 
  /// Serialize() method. Additionally, the implementation in this class updates the loading
  /// progress bar.
  ///
  /// \param pClassRefRequested 
  ///   Optional parameter; to be used to validate the type of the object being read.
  ///   If this is non-NULL, the function will check whether the passed type matches the loaded object's type or not.
  /// \param pObjectLength
  ///   pointer to a variable the object size should be written to, can be NULL
  /// \return
  ///   the object that has been read
  VISION_APIFUNC virtual VTypedObject* ReadObject( const VType* pClassRefRequested, unsigned int* pObjectLength = NULL );

  VISION_APIFUNC virtual void WriteObject( const VTypedObject* pObj, const VType* pForceClass=NULL);

  /// \brief
  ///   Custom close function (non-virtual)
  VISION_APIFUNC void Close();

  int m_iObjectCount, m_iNonNullCount, m_iRootObjectCount;
  int m_iRecursionDepth, m_iNonNullCountRead;

  float m_fStartPecentage;
  float m_fEndPercentage;
  VProgressStatus *m_pProgress;
  bool m_bRetainRange, m_bRangeSet;
  bool m_bHasPerObjectRangeInfo;
  float m_fProgressStep;
};


#if defined (WIN32) || defined _VISION_DOC

/// \brief
///   VForge export related helper class
class VExportShapesArchive : public VShapesArchive
{
public:
  VISION_APIFUNC VExportShapesArchive(IVFileOutStream *pOut);
  VISION_APIFUNC ~VExportShapesArchive();


  VISION_APIFUNC VOVERRIDE void WriteObject( const VTypedObject* pObj, const VType* pForceClass=NULL);
  

  /// \brief
  ///   Internal function to add a plugin to the m_UniquePluginNames list
  VISION_APIFUNC void AddPlugin(IVisPlugin_cl *pPlugin);

  DynArray_cl<unsigned int> m_iObjectsBytesCount; ///< for every !=NULL shape it stores the file size of all resource dependencies
  DynArray_cl<unsigned short> m_iObjectRecursionDepth; ///< for every !=NULL shape it stores the file size of all resource dependencies
  DynArray_cl<unsigned int> m_iObjectPatchPos; ///< for every !=NULL shape it stores the position for progress value patching
  DynArray_cl<float> m_iObjectRangeLen; ///< for every !=NULL shape it stores the file size of all resource dependencies

  unsigned int m_iSumNewBytes;
  int m_iObjectsWithRangeCount;
  VStrList m_LoadedFilesSoFar;
  VStrList m_UniquePluginNames;

};

/// \brief
///   VForge exporter interface for vscene/vzone/vprefab export
///
/// vForge uses this interface to write out binary export files of type vscene, vzone and vprefab. Custom exporters can be used by installing an exporter factory through
/// Vision::Editor.SetExporterFactory. Every export step (vscene, every single vzone) creates a new exporter instance through Vision::Editor.CreateExporter and
/// destroys the exporter instance through smart pointer de-referencing, so no custom cleanup code is available.
/// For every export, vForge calls the StartVSceneExport function (or its vzone counterpart), serializes relevant shapes into the return archive and ends the export
/// by calling EndExport. For actual implementation, please refer to class VSceneExporter in the Vision engine plugin.
/// vscene files can be read by class VSceneLoader, vzone files are read by class VisZoneResource_cl and vprefab files are read by class VPrefab.
/// The exporter interface can not only be used to modify the vscene file format, it can also be used to trigger custom vscene exports from custom aplication code.
/// However these interfaces are only available on PC platforms.
class IVisSceneExporter_cl : public VRefCounter
{
public:
  /// \brief
  ///   Holds zone properties for exporting
  struct VZoneInfo_t
  {
    inline VZoneInfo_t()
    {
      m_fLoadedRadius = -1.f;
      m_fCacheInMargin = 1000.f;
      m_fCacheOutMargin = 2000.f;
      m_dPivot[0] = m_dPivot[1] = m_dPivot[2] = 0.0;
    }
    VZoneInfo_t& operator = (const VZoneInfo_t& other)
    {
      m_sZoneName = other.m_sZoneName;
      m_sLightgridFilename = other.m_sLightgridFilename;
      m_ZoneBox = other.m_ZoneBox;
      m_fLoadedRadius = other.m_fLoadedRadius;
      m_fCacheInMargin = other.m_fCacheInMargin;
      m_fCacheOutMargin = other.m_fCacheOutMargin;
      memcpy(m_dPivot,other.m_dPivot,sizeof(m_dPivot));
      return *this;
    }

    VString m_sZoneName;          ///< Name of the zone as shown in vForge
    VString m_sLightgridFilename; ///< Optional filename of the light grid
    hkvAlignedBBox m_ZoneBox;  ///< Zone bounding box
    float m_fLoadedRadius, m_fCacheInMargin, m_fCacheOutMargin; ///< cache distances (as exposed in vForge)
    double m_dPivot[3]; ///< pivot of the zone, should be 0,0,0
  };

  IVisSceneExporter_cl() {}
  virtual ~IVisSceneExporter_cl() {}

  /// \brief
  ///   Starts the export of scene elements as a fully featured vscene file.
  ///
  /// \param pOut
  ///   The file stream to write the vscene file content to. Must be !=NULL
  ///
  /// \param bCloseFile
  ///   If true, the pOut file will be closed in the EndExport step.
  ///
  /// \param pEmbeddedZones
  ///   An array that holds at least iEmbeddedZoneCount structures of type VZoneInfo_t. This array describes the vzone references that should be embedded into
  ///   the vscene file. Note that it does not embed the vzone data itself, but rather just the data that it needs to trigger streaming (filename reference, box size, loading distances).
  ///
  /// \param iEmbeddedZoneCount
  ///   The number of zones to embed
  ///
  /// \param eFlags
  ///   Bitflags that define what is included into the vscene. See enum VSceneExportFlags_e for bitflag values
  ///
  /// \return
  ///   An archive pointer (base class: VArchive) that is used to write out all native shapes that should go into the vscene. vForge uses it to pass it to every engine
  ///   instance in the scene during the export step.
  VISION_APIFUNC virtual VExportShapesArchive* StartVSceneExport(IVFileOutStream *pOut, bool bCloseFile, const VZoneInfo_t *pEmbeddedZones, int iEmbeddedZoneCount, VSceneExportFlags_e eFlags=VExport_All) = 0;

  /// \brief
  ///   Starts the export of scene elements as a vzone file. The parameters and return value is explained in StartVSceneExport
  VISION_APIFUNC virtual VExportShapesArchive* StartVZoneExport(IVFileOutStream *pOut, bool bCloseFile, VZoneInfo_t &info) = 0;

  /// \brief
  ///   Starts the export of scene elements as a vprefab file. The parameters and return value is explained in StartVSceneExport
  VISION_APIFUNC virtual VExportShapesArchive* StartVPrefabExport(IVFileOutStream *pOut, bool bCloseFile) = 0;

  /// \brief
  ///   Ends the export step for all variants (StartVSceneExport, StartVZoneExport, StartVPrefabExport) and writes the file.
  VISION_APIFUNC virtual void EndExport() = 0;
};


/// \brief
///   Interface that is used to allow for installing custom vscene exporters. One global factory can be installed via Vision::Editor.SetExporterFactory
class IVisSceneExporterFactory_cl
{
public:
  IVisSceneExporterFactory_cl() {}
  virtual ~IVisSceneExporterFactory_cl() {}

  /// \brief
  ///   Key function of this interface that must return a new exporter instance (IVisSceneExporter_cl). Since that class is refcounted, the code outside takes care of releasing/destroying exporter instances.
  virtual IVisSceneExporter_cl *CreateExporter() = 0;
};


typedef VSmartPtr<IVisSceneExporter_cl> IVisSceneExporterPtr;

#endif // win32


/// \brief
///   Class for accessing the editor state
/// 
/// The editor manager provides functionality for for accessing the current vForge state.
/// 
/// It can be accessed via Vision::Editor. 
/// 
/// The information can be used to implement editor specific behaviors in a game application.
/// 
/// The default OnUpdateScene implementation has the following behavior: In the
/// EDITORMODE_PLAYING_IN_EDITOR and EDITORMODE_PLAYING_IN_GAME mode everything is still executed.
/// The EDITORMODE_ANIMATING skips the Think/PreThink functions and the physics update. The
/// EDITORMODE_NONE mode additionally skips the shader and particle updates.
class VisEditorManager_cl
{
public:

  /// \brief
  ///   Enumeration of editor modes
  enum EditorMode_e
  {
    EDITORMODE_NONE = 0,                ///< plain editing mode active, no updates performed;
    EDITORMODE_ANIMATING = 1,           ///< perform shader and particle animations
    EDITORMODE_PLAYING_IN_EDITOR = 2,   ///< full playing mode including Think/PreThink and physics updates. Editor has input control.
    EDITORMODE_PLAYING_IN_GAME = 3      ///< full playing mode including Think/PreThink and physics updates. Game has input control.
  };

  VisEditorManager_cl();
  ~VisEditorManager_cl();

  ///
  /// @name Status Functions
  /// @{
  ///

  /// \brief
  ///   Indicates whether the engine is currently running inside the editor. Returns false by
  ///   default (stand-alone game applications)
  inline bool IsInEditor() const 
  {
    return m_bIsInEditor;
  }

  /// \brief
  ///   Indicates whether the engine is currently running in editor and is in silent mode. Returns false by
  ///   default (stand-alone game applications)
  inline bool IsInEditorSilentMode() const 
  {
    return m_bIsInEditorSilentMode;
  }

  /// \brief
  ///   Whether script caching is currently used (affects instantiation of script components)
  inline bool UseEditorScriptCaching() const 
  {
    return m_bEditorScriptCaching;
  }

  /// \brief
  ///   Returns the current editor mode. This function returns EDITORMODE_NONE if the engine is not
  ///   running inside the editor (IsInEditor()==false).
  inline EditorMode_e GetMode() const 
  {
    return m_editorMode;
  }

  /// \brief
  ///   Indicates whether the editor is currently active either in the animating or playing mode.
  ///   For convenience it returns true if not inside the editor.
  inline bool IsAnimatingOrPlaying() const
  {
    if ( !IsInEditor() )
      return true;
    if ( m_editorMode == EDITORMODE_ANIMATING || IsPlaying() )
      return true;
    return false;
  }


  /// \brief
  ///   Indicates whether the editor is currently either in the EDITORMODE_PLAYING_IN_EDITOR or the EDITORMODE_PLAYING_IN_GAME mode. For convenience it returns true if not inside the editor.
  ///
  inline bool IsPlaying() const
  {
    if ( !IsInEditor() )
      return true;
    if ( m_editorMode == EDITORMODE_PLAYING_IN_EDITOR || m_editorMode == EDITORMODE_PLAYING_IN_GAME )
      return true;
    return false;
  }

  /// \brief
  ///   Indicates whether the editor is currently active in the EDITORMODE_PLAYING_IN_GAME mode.
  ///   For convenience it returns true if not inside the editor.
  inline bool IsPlayingTheGame() const
  {
    if ( !IsInEditor() )
      return true;
    if ( m_editorMode == EDITORMODE_PLAYING_IN_GAME )
      return true;
    return false;
  }

  /// \brief
  ///   Returns the base data path used by the editor.
  /// \return
  ///   \c NULL if no project is loaded; the base data path otherwise
  inline VString& GetBaseDataPath()
  {
    return m_BaseDataPath;
  }

  /// \brief
  ///   Returns the path used by the editor to load internal.
  /// \return
  ///   \c NULL if no project is loaded; the edtor data path otherwise
  inline VString& GetEditorDataPath()
  {
    return m_EditorDataPath;
  }

  ///
  /// \brief  Gets the project path of the currently loaded project in the editor. This value is only valid (non-NULL) when InEditor() is true. (E.g. a project loaded and in play-the-game mode)
  ///
  /// \return null if no project is loaded, else the project path. 
  ///
  inline VString& GetProjectPath()
  {
    return m_ProjectPath;
  }

  ///
  /// \brief  Gets the project path of the currently loaded project in the editor. This value is only valid (non-NULL) when InEditor() is true. (E.g. a project loaded and in play-the-game mode)
  ///
  /// \return null if no project is loaded, else the project path. 
  ///
  inline VString& GetSceneName()
  {
    return m_SceneName;
  }

  /// \brief
  ///  Gets the sdk path of the editor. This value is only valid (non-empty) when InEditor() is true.
  ///
  /// \return an empty string if not in editor, else the sdk path. (E.g. C:/Havok/VisionSDK/)
  ///
  inline VString& GetSDKPath()
  {
    return m_SDKPath;
  }

  /// \brief
  ///   Makes the specified path absolute based on the current editor project path
  /// \param szPath
  ///   the path to make absolute
  /// \param sResult
  ///   the VString in which the modified path is returned if successful
  /// \return
  ///   true if the function succeeded and sResult contains a valid absolute path; false
  ///   if the function failed. In case of failure, the contents of sResult is undefined.
  VISION_APIFUNC bool MakePathAbsoluteForProject(const char *szPath, VString& sResult) const;

  /// \brief
  ///   Gets the executable path of the editor. This value is only valid (non-empty) when InEditor() is true.
  ///
  /// \return an empty string if not in editor, else the executable path.
  ///
  inline VString& GetExecutablePath()
  {
    return m_ExecutablePath;
  }


  ///
  /// @}
  ///

  ///
  /// @name Editor Management Functions
  /// @{
  ///

  /// \brief
  ///   Called by the editor to set the IsInEditor flag
  VISION_APIFUNC void SetInEditor(bool bState);

  /// \brief
  ///   Called by the editor to set the script caching flag
  VISION_APIFUNC void SetUseEditorScriptCaching(bool bUseCaching);

  /// \brief
  ///   Called by the editor to set the IsInEditor flag
  VISION_APIFUNC void SetInEditorSilentMode(bool bState);

  /// \brief
  ///   Called by the editor to set the current editor mode
  VISION_APIFUNC void SetMode(EditorMode_e state);

  /// \brief
  ///   Called by the editor to set the current base data path
  VISION_APIFUNC void SetBaseDataPath(const char* szBaseDataPath);

  /// \brief
  ///   Called by the editor to set the current editor data path
  VISION_APIFUNC void SetEditorDataPath(const char* szEditorDataPath);

  /// \brief
  ///   Called by the editor to set the current project path
  VISION_APIFUNC void SetProjectPath(const char* szProjectPath);

  /// \brief
  ///   Called by the editor to set the current scene name
  VISION_APIFUNC void SetSceneName(const char* szProjectPath);

  /// \brief
  ///   Called by the editor to set the current sdk path
  VISION_APIFUNC void SetSDKPath(const char* szSDKPath);

  /// \brief
  ///   Called by the editor to set the current executable path
  VISION_APIFUNC void SetExecutablePath(const char* szExecutablePath);

  /// \brief
  ///   vForge sets this to true when switching to orthographic views because these views do not work properly with many special effects
  inline void SetIgnoreAdvancedEffects(bool bStatus)
  {
    m_bIgnoreAdvancedEffects = bStatus;
  }

  /// \brief
  ///   Special effect implementations should query this flag and not render the effect if they do not work properly with orthographic modes for instance. 
  ///   The default value is false and may only be true inside vForge.
  inline bool GetIgnoreAdvancedEffects() const
  {
    return m_bIgnoreAdvancedEffects;
  }

#if defined (WIN32) || defined _VISION_DOC

  /// \brief
  ///   Called by the editor if the editor is running the tests
  VISION_APIFUNC void SetEditorIsTesting();

  ///
  /// @}
  ///

  ///
  /// @name vForge Rendering
  /// @{
  ///

  /// \brief
  ///   This callback is triggered from inside vForge's shading mode renderloop, e.g. render
  /// worldspace normals etc.
  /// 
  /// Hook into this callback to render custom geometry in any of the shading modes.
  /// 
  /// This callback does not have any specific parameters, instead the renderloop can be retrieved
  /// via  Vision::RenderLoopHelper.GetReplacementRenderLoop and cast to class
  /// VisDebugShadingRenderLoop_cl. VisDebugShadingRenderLoop_cl provides useful functionality (i.e. the current
  /// technique used for rendering)
  VisCallback_cl OnRenderShadingModeGeometry;

  /// \brief
  ///   This callback can be triggered by custom code to trigger a repaint of the vForge engine view
  VisCallback_cl OnTriggerViewUpdate;

  /// \brief
  ///   This function can be called by client code to change an editor property of the shape that corresponds to the passed native object
  ///
  /// \param pNativeObj
  ///   Pointer to the native object (e.g. entity)
  /// \param szVariableName
  ///   Display name in vForge of the variable to update. If this is a variable from the entity variable list, then bIsSubProperty must be set to true
  /// \param szNewValue
  ///   String representation of the new value (e.g. "1.2" for float value)
  /// \param bIsSubProperty
  ///   true if the variable is a nested property (e.g. from var table for entity), false if it is a first level shape property. This parameter has no effect if the native object is a component.
  /// \param bSetViaAction
  ///   true if an editor action should be generated so the change appears in the UNDO/REDO history of the editor and thus is undoable.
  /// \returns
  ///   true if the operation was successful on the editor side
  VISION_APIFUNC bool SetVariableInEditor(VisTypedEngineObject_cl *pNativeObj, const char *szVariableName, const char *szNewValue, bool bIsSubProperty, bool bSetViaAction);

  /// \brief
  ///   Internal callback used by vForge to listen to SetVariableInEditor function calls
  VisCallback_cl OnTriggerSetVariable;

  /// \brief
  ///  This function can be called by client code to change the size of the 3d display inside the editor engine panel
  ///
  /// \param iWidth
  ///   new width of the 3d display
  ///
  /// \param iHeight
  ///   new height of the 3d display
  VISION_APIFUNC void Set3DCanvasSize(int iWidth, int iHeight);

  /// \brief
  ///   Internal callback used by vForge to listen to Set3DCanvasSize function calls
  VisCallback_cl OnSet3DCanvasSize;

  ///
  /// @}
  ///

  ///
  /// @name Vforge Export interface
  /// @{
  ///


  /// \brief
  ///   Installs a custom factory that provides exporter objects for vForge scene exports. This factory remains set for the lifetime of the engine
  inline void SetExporterFactory(IVisSceneExporterFactory_cl *pFactory)
  {
    m_pExporterFactory = pFactory;
  }

  /// \brief
  ///   Returns the current factory
  inline IVisSceneExporterFactory_cl* GetExporterFactory() const
  {
    return m_pExporterFactory;
  }

  /// \brief
  ///   Creates a new exporter object using the exporter factory (if set). The return value can be NULL. vForge calls this function for every export operation.
  inline IVisSceneExporter_cl *CreateExporter()
  {
    if (m_pExporterFactory==NULL)
      return NULL;
    return m_pExporterFactory->CreateExporter();
  }

  ///
  /// @}
  ///

#endif

protected:
friend class Vision;
  void Init();
  void DeInit();

  bool m_bIsInEditor, m_bIgnoreAdvancedEffects, m_bIsInEditorSilentMode, m_bEditorScriptCaching;
  EditorMode_e m_editorMode;
  VString m_BaseDataPath;
  VString m_EditorDataPath;
  VString m_ProjectPath;
  VString m_SceneName;
  VString m_SDKPath;
  VString m_ExecutablePath;
#ifdef WIN32
  IVisSceneExporterFactory_cl *m_pExporterFactory;
#endif
};



/// \brief
///   Derived callback data object class that is used for the OnEditorModeChanged callback function
/// 
/// This callback is useful when integrating a custom game application into vForge to react to
/// editor mode changes.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnEditorModeChanged
/// \sa VisCallbackManager_cl::OnEditorModeChanging
class VisEditorModeChangedDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and the old and new editor mode
  VisEditorModeChangedDataObject_cl(VisCallback_cl *pSender, VisEditorManager_cl::EditorMode_e eOldMode, VisEditorManager_cl::EditorMode_e eNewMode) : IVisCallbackDataObject_cl(pSender)
  {
    m_eOldMode = eOldMode;
    m_eNewMode = eNewMode;
  }
  

  VisEditorManager_cl::EditorMode_e m_eOldMode;
  VisEditorManager_cl::EditorMode_e m_eNewMode;
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
