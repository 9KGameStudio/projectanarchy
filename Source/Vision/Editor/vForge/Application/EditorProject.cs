/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.IO;
using CSharpFramework;
using System.Windows.Forms;
using System.Reflection;
using CSharpFramework.Helper;
using CSharpFramework.Dialogs;
using AssetManagementManaged;
using System.Diagnostics;
using Editor.Dialogs;
using System.Collections.Generic;

namespace Editor
{
	/// <summary>
  /// EditorProject implements the IProject interface. The active project can be accessed via EditorManager.Project.
  /// See also its EditorScene counterpart
	/// </summary>
  public class EditorProject : IProject
  {
    #region IProject Members

    /// <summary>
    /// Constructor
    /// </summary>
    public EditorProject()
    {
    }

    /// <summary>
    /// Overridden save function of the project
    /// </summary>
    /// <returns></returns>
    public override bool Save()
    {
      bool bRes = true;

      if (Scene != null)
      {
        if (!Scene.Save())
          bRes = false;
      }

      Dirty = false;  //clear all the dirty flags
      return bRes;
    }

    /// <summary>
    /// Overridden OpenScene function
    /// </summary>
    /// <param name="relFilename"></param>
    /// <returns></returns>
    public override bool OpenScene(string relFilename)
    {
      // project must not have an active scene
      if ( Scene != null )
        return false;

      object[] parameters = { this, relFilename };
      EditorScene scene = EditorManager.SceneFactory.CreateInstance(parameters) as EditorScene;
      if (scene == null)
      {
        EditorManager.ShowMessageBox("Scene must be derived from class 'EditorScene'", "Failed to create scene", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      // installer assembly resolver for editor plugin assemblies
      AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(SceneLoader_AssemblyResolve);

      // We set the scene here to make scene information available to de-serialization code that needs to load custom data.
      // I.e. profile specific settings that need to know the scene name to get the correct path.
      _currentlyLoadingScene = scene;
      if (!scene.Load(relFilename))
      {
        _currentlyLoadingScene = null;
        return false;
      }
      _currentlyLoadingScene = null;

      AppDomain.CurrentDomain.AssemblyResolve -= new ResolveEventHandler(SceneLoader_AssemblyResolve);

      EditorManager.Progress.StatusString = "Create Shapes";
      Scene = scene; // this creates the engine instances since it calls EditorApp.Scene

      if (scene.SceneVersion != EditorScene.SCENE_VERSION_CURRENT)
      {
        // output info that the scene version is out of data
        EditorManager.ShowMessageBox("Scene file has outdated version number.\nPlease re-save scene.", "Information", MessageBoxButtons.OK,MessageBoxIcon.Information);
        scene.Dirty = true;
      }

      return true;
    }

    /// <summary>
    /// Resolve missing plugins errors when loading the scene files.
    /// (Try to find them from our list of loaded plugins instead)
    /// </summary>
    private static System.Reflection.Assembly SceneLoader_AssemblyResolve(object sender, ResolveEventArgs args)
    {
      // extract simple plugin name from the passed "qualified full name"
      string[] splitSet = args.Name.Split(",".ToCharArray() );
      string pluginModuleName = splitSet[0];

      // try to find the plugin in the list of global editor plugins
      IEditorPluginModule pluginModule = EditorManager.GetPluginByName(pluginModuleName);
      if ( pluginModule != null )
        return pluginModule.GetType().Assembly;

      return null;
    }

    /// <summary>
    /// Overridden NewScene function
    /// </summary>
    /// <param name="relFileName"></param>
    /// <param name="bPlainEmpty"></param>
    /// <returns></returns>
    public override bool NewScene(string relFileName, bool bPlainEmpty)
    {
      object[] parameters = { this, relFileName };
      EditorScene editorScene = EditorManager.SceneFactory.CreateInstance(parameters) as EditorScene;
      if (editorScene == null)
      {
        EditorManager.ShowMessageBox("Scene must be derived from class 'EditorScene'", "Failed to create scene", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      EditorManager.ActiveView.SetCameraPosition(editorScene.Settings.CameraPosition);
      EditorManager.ActiveView.SetCameraRotation(editorScene.Settings.CameraAngles);

      _scene = editorScene;
      
      // save the scene (empty) such that all layers can be safely saved
      editorScene.SaveAs(relFileName);

      if (!bPlainEmpty)
      {
        V3DLayer MainLayer = new V3DLayer("Main layer");
        editorScene.AddLayer(MainLayer, true, false);

        // mark the layer as dirty, to ensure that it will be saved now
        MainLayer.Dirty = true;
      }

      // this will fire an event that is often used to set things up, so only now the scene will be set up completely
      Scene = editorScene; // set it afterwards so the OnSceneChanged callback has the correct name

      // save the scene again, now with all layers, because NOW all scene elements will be really setup
      editorScene.SaveAs(relFileName);
      
      return true;
    }


    /// <summary>
    /// Overridden setter. The setter assigns the scene as well to EditorApp.Scene which creates the engine instances
    /// </summary>
    public override IScene Scene
    {
      get
      {
        return base.Scene;
      }
      set
      {
        base.Scene = value;
        EditorApp.Scene = (EditorScene)value;
      }
    }

    /// <summary>
    /// Overridden project Close function
    /// </summary>
    /// <returns></returns>
    public override bool Close()
    {
      if (Scene != null)
        Scene.Close();

      SendProjectClosingEvent();

      EditorApp.Project = null;
      EditorApp.Scene = null;

      while (_deinitStack.Count > 0)
      {
        Action deinitAction = _deinitStack.Pop();
        deinitAction();
      }

      return true;
    }

    #endregion

    #region Create and Load projects

    /// <summary>
    /// Create a new project
    /// </summary>
    /// <param name="dir"></param>
    /// <param name="projname"></param>
    /// <returns></returns>
    public static EditorProject CreateProject(string dir, string projname)
    {
      char slash = Path.DirectorySeparatorChar;
      string projdir  = dir;                              // x:\dir
      string projextname = projname +".project";
      string projfile = Path.Combine(projdir, projextname);    // x:\dir\projname.project

      //create the project directory
      if (Directory.CreateDirectory(projdir) == null)
      {
        EditorManager.ShowMessageBox("Failed to create project directory", "Failed to create project", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return null;
      }

      //create the project file
      FileStream fs = File.Create(projfile);
      if (fs == null)
        return null;

      //We don't store anything in the project file
      fs.Close();

      //Open the project and return it
      //EditorProject proj = new EditorProject();
      EditorProject proj = EditorManager.ProjectFactory.CreateInstance(null) as EditorProject;
      if (proj == null)
      {
        EditorManager.ShowMessageBox("Project must be derived from class 'EditorProject'", "Failed to create project", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return null;
      }

      proj.FileName = projextname;

      try
      {
        proj.Load(projfile);

        //Add the project file
        ManagedBase.RCS.GetProvider().AddFile(projfile, true /* Binary file - but files are until now empty, should we place text in there this needs to be changed */);
      }
      catch(Exception ex)
      {
        EditorManager.DumpException(ex);
        proj = null;
      }

      return proj;
    }

    /// <summary>
    /// Actually load the project
    /// </summary>
    /// <param name="filename"></param>
    public void Load(string filename)
    {
      if (!File.Exists(filename))
      {
        throw new FileNotFoundException("Project file not found.");
      }

      try
      {
        SendProjectLoadingEvent();

        EditorManager.EngineManager.InitFileHandling();
        EditorManager.EngineManager.SetBaseDataPath(EditorManager.BaseDataDir);
        EditorManager.EngineManager.SetEditorDataPath(EditorManager.AppDataDir);

        _deinitStack.Push(() =>
        {
          //Remove the project directory from the engine base data directory
          EditorApp.EngineManager.File_RemoveSearchPath(ProjectSearchPath);
          RemoveAllCustomDataDirectories();

          EditorApp.EngineManager.File_RemoveFileSystem("workspace");
        });

        m_filename = filename;
        string projectDirNoEndingSlash = Path.GetDirectoryName(filename).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
        ProjectDir = projectDirNoEndingSlash + Path.DirectorySeparatorChar;

        //Nothing to read at the moment...
        //We don't want to put user specific data in the project file anyway
        //Handle file string earlier to use RCS.
        this.FileName = Path.GetFileName(filename);

        _deinitStack.Push(() =>
        {
          ProjectDir = "";
          FileName = "";
        });

        EditorManager.EngineManager.SetProjectPath(ProjectDir);

        // Open MultiUser Editing Dialog
        if (!TestManager.IsRunning && !EditorManager.SilentMode && EditorManager.Settings.OpenMultiUserEditingDialogOnProjectLoad)
        {
          if (DetectFolderType() != FolderType_e.Local || EditorManager.Settings.OpenMultiUserEditingDialogForLocalProjects)
          {
            MultiUserEditingDlg dlg = new MultiUserEditingDlg();
            dlg.ShowInTaskbar = false;
            dlg.Project = this;
            if (dlg.ShowDialog() != DialogResult.OK)
            {
              throw new Exception("No multi-user editing option selected.");
            }
          }
        }

        // Select and activate the right RCS system. User cannot abort this operation.
        // If updating the RCS status fails we break up loading the project.
        if (!UpdateRCSStatus(true))
        {
          throw new Exception("Failed to update RCS status.");
        }

        // Setup RCS changelist
        if (ManagedBase.RCS.GetProvider() != null)
        {
          string sChangeListDescription = "vForge";
          if (EditorManager.Settings.CreateSeparateChangelists)
          {
            string sDirName = Path.GetFileName(projectDirNoEndingSlash);
            sChangeListDescription += " - " + sDirName;
          }
          string sChangelist = ManagedBase.RCS.GetProvider().GetChangelistByDescription(sChangeListDescription, true);
          if (sChangelist != null)
            ManagedBase.RCS.GetProvider().SetChangelist(sChangelist);
        }

        _deinitStack.Push(() =>
        {
          // Deactivate RCS system
          UpdateRCSStatus(false);
        });

        if (!EnsureValidWorkspace())
        {
          throw new Exception("No valid workspace found.");
        }

        // optionally load plugins from sub directory
        string additionalPluginDir = AdditionalPluginDir;

        EditorManager.ProfileManager.InitNewProject(ProjectDir);

        // Notify the asset manager of automatically added data directories
        EditorManager.AssetManager.AddDataDirectory(
          EditorManager.IsCustomBaseDataDir ? ":base_data" : EditorManager.DefaultBaseDataSearchPath, "Base", false, false, true);

        // If a custom simulation data path has been set, add it as a file system.
        if (EditorManager.IsCustomSimulationDataDir)
        {
          EditorManager.EngineManager.File_AddFileSystem("simulation_data", EditorManager.SimulationDataDir, FileSystemFlags.Writable);
        }
        // Add the search path for simulation data
        this.AddCustomDataDirectory(
          EditorManager.IsCustomSimulationDataDir ? ":simulation_data" : EditorManager.DefaultSimulationDataSearchPath, "Simulation", true, true);

        // Load the project specific editor plugins (install an assembly resolver to load the
        // assemblies from the project directory)
        AppDomain.CurrentDomain.AssemblyResolve += new ResolveEventHandler(ProjectPlugins_AssemblyResolve);
        EditorPlugins = new EditorPluginCollection();
        PluginManager.LoadPlugins(ProjectDir, EditorPlugins);
        if (additionalPluginDir != null)
          PluginManager.LoadPlugins(additionalPluginDir, EditorPlugins);
        AppDomain.CurrentDomain.AssemblyResolve -= new ResolveEventHandler(ProjectPlugins_AssemblyResolve);

        //Load the entity plugins (so they can register their application) before engine init
        EditorManager.EngineManager.Plugins_LoadAllEnginePlugins(projectDirNoEndingSlash);
        if (additionalPluginDir != null)
          EditorManager.EngineManager.Plugins_LoadAllEnginePlugins(additionalPluginDir);

        // load the manifest file which may contain additional engine plugins:
        EditorManager.LoadManifestFile(this, Path.Combine(projectDirNoEndingSlash, "vForgeManifest.txt"));

        // Find out whether the project directory is already added as a search path; i.e., whether we open 
        // a project in one of the previously added directories. If yes, set the project search path 
        // accordingly and don't add another search path. Otherwise, add a search path for the project.
        String searchPathOfProjectDir = EditorManager.EngineManager.File_GetSearchPathForPath(
          filename, FileSystemAccessMode.Read, FileSystemElementType.File);
        if (searchPathOfProjectDir != null)
        {
          ProjectSearchPath = searchPathOfProjectDir;
        }
        else
        {
          EditorManager.EngineManager.File_AddSearchPath(ProjectSearchPath, SearchPathFlags.Writable | SearchPathFlags.PathMustExist);
          EditorManager.AssetManager.AddDataDirectory(ProjectSearchPath, new DirectoryInfo(ProjectDir).Name, false, true, true);
        }

        _deinitStack.Push(() =>
        {
          // Unload plugins after DeInit (when all references to plugin components/entities are gone)
          EditorManager.EngineManager.Plugins_UnloadAllEnginePlugins();

          // Unload the project specific editor plugins
          PluginManager.UnloadPlugins(EditorPlugins);
          EditorManager.DeInitializeNonStandardPlugins();

          EditorPlugins = null;
        });

        EditorManager.EngineManager.RefreshEntityClassManager();
        EditorManager.EngineManager.RefreshShaderEffectManager();
        EditorManager.EngineManager.RefreshComponentClassManager();

        //Initialize the engine
        if (!EditorApp.EngineManager.InitEngine())
        {
          throw new Exception("Failed to initialize engine. Please check the engine log for more details.");
        }

        _deinitStack.Push(() =>
        {
          // empty all class managers (before engine.DeInit so that native code can be executed)
          EditorManager.EngineManager.RefreshEntityClassManager();
          EditorManager.EngineManager.RefreshShaderEffectManager();
          EditorManager.EngineManager.RefreshComponentClassManager();

          EditorApp.EngineManager.DeInitEngine();
        });
      }
      catch
      {
        Close();
        throw;
      }
    }


    /// <summary>
    /// Tries to find a workspace for the current project directory. Failing this, either prompts
    /// the user where to set the workspace (if permitted by the application state), or assumes the
    /// project directory to be the workspace directory (if running silently or if executing tests).
    /// </summary>
    /// <returns><code>true</code> if a workspaces was either found or has been created; <code>false</code> otherwise.</returns>
    private bool EnsureValidWorkspace()
    {
      // Find the vForge workspace directory. This may be any directory above the
      // project directory.
      // If no workspace could be found, either show a dialog (if allowed) or assume the project directory.
      if (!EditorManager.EngineManager.FindWorkspaceDirectory(ProjectDir, "workspace", out WorkspaceDir, out ProjectSearchPath))
      {
        if (!TestManager.IsRunning && !EditorManager.SilentMode)
        {
          WorkspaceDlg dlg = new WorkspaceDlg(ProjectDir);
          if (dlg.ShowDialog() != DialogResult.OK)
          {
            return false;
          }

          WorkspaceDir = dlg.UseProjectDir ? ProjectDir : dlg.SelectedDir.FullName;
          if (!CreateWorkspaceFile())
          {
            return false;
          }
          ProjectSearchPath = ":workspace/" + ProjectDir.Substring(WorkspaceDir.Length).Replace('\\', '/');
          ProjectSearchPath = ProjectSearchPath.Trim('/');
        }
        else
        {
          WorkspaceDir = ProjectDir;
          ProjectSearchPath = ":workspace";
        }
      }

      // We have a workspace. Add it as a file system to the Engine.
      if (!EditorManager.EngineManager.File_AddFileSystem("workspace", WorkspaceDir, FileSystemFlags.Writable))
      {
        return false;
      }

      return true;
    }


    /// <summary>
    /// Attempts to create a workspace marker file in the folder <code>WorkspaceDir</code>.
    /// </summary>
    /// <returns><code>true</code> if the file was successfully created</returns>
    private bool CreateWorkspaceFile()
    {
      String workspaceFile = String.Empty;
      try
      {
        workspaceFile = Path.Combine(WorkspaceDir, WorkspaceFileName);
        using (File.Create(workspaceFile))
        {
          // No action; we just want to create the file.
        }
      }
      catch (System.Exception ex)
      {
        EditorManager.ShowMessageBox(String.Format("Failed to create the workspace file at path \"{0}\". Error: {1}", workspaceFile, ex.Message), "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return false;
      }

      // Try to add to RCS. Not fatal if this fails.
      ManagedBase.RCS.GetProvider().AddFile(workspaceFile, false);

      return true;
    }

    /// <summary>
    /// Resolves failures when loading project specific editor plugins.
    /// 
    /// .NET by default seems to load assemblies from the executable directory and the assembly cache only.
    /// This resolver forces .NET to load the assemblies from the current project directory. Typically
    /// just the managed DLLs fail to load and get handled by this resolver.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    /// <returns></returns>
    private System.Reflection.Assembly ProjectPlugins_AssemblyResolve(object sender, ResolveEventArgs args)
    {
      // extract the plugin file name from the passed "qualified full name"
      string[] splitSet = args.Name.Split(",".ToCharArray() );
      string pluginFileName = splitSet[0];

      // First try to load the plugin from the project directory
      try 
      {
        Assembly asm = Assembly.LoadFrom(ProjectDir + pluginFileName + ".dll");
        return asm;
      }
      catch
      {
      }

      // If the assembly is not in the project dir then check the additional plugin directory 
      try
      {
        string sAdditionalPluginDir = AdditionalPluginDir;
        if (sAdditionalPluginDir != null)
        {
          Assembly asm = Assembly.LoadFrom(sAdditionalPluginDir + pluginFileName + ".dll");
          return asm;
        }
      }
      catch
      {
      }

      return null;
    }

    /// <summary>
    /// return the full pathname of the project
    /// </summary>
    public string PathName
    {
      get {return m_filename;}
    }

    private string m_filename;

    /// <summary>
    /// Gets the absolute path to the additional plugin directory, if such a directory
    /// is specified in the application settings. The returned path has a trailing
    /// backslash.
    /// 
    /// If no additional plugin directory is defined then NULL is returned
    /// </summary>
    public string AdditionalPluginDir
    {
      get
      {
        // Build the absolute path to the plugin directory
        string additionalPluginDir = null;
        if (!string.IsNullOrEmpty(EditorManager.Settings.AdditionalPluginDirectory))
        {
          additionalPluginDir = EditorManager.Settings.AdditionalPluginDirectory;
          if (!FileHelper.IsAbsolute(additionalPluginDir))
            additionalPluginDir = Path.Combine(this.ProjectDir, additionalPluginDir);
          if (!Directory.Exists(additionalPluginDir))
            additionalPluginDir = null;

          // Make sure it has a trailing backslash
          if (additionalPluginDir != null)
            if (!additionalPluginDir.EndsWith(Path.DirectorySeparatorChar.ToString()))
              additionalPluginDir += Path.DirectorySeparatorChar;
        }

        if (additionalPluginDir == null)
          return null;

        return Path.GetFullPath(additionalPluginDir);
      }
    }

    /// <summary>
    /// Find the project for an absolute scene filename. Searches in all folders above.
    /// </summary>
    /// <param name="filename">absolute scene filename</param>
    /// <returns>absolute project filename</returns>
    public static string FindProjectFor(string filename)
    {
      string dir = Path.GetDirectoryName(filename);
      if (!Directory.Exists(dir))
        return null;

      while (true)
      {
        //Get all the project files in the directory
        string[] files = Directory.GetFiles(dir, "*.project");

        //Did we find one?
        if (files.Length>0)
          return files[0]; //Return first match. (There shouldn't be multiple anyway)

        //Move one directory up
        dir = Path.GetDirectoryName(dir);
        if (dir.Length == 0) break;
      }

      return null;
    }

    #endregion

    #region Dirty Flag

    /// <summary>
    /// Overridden dirty flag
    /// </summary>
    public override bool Dirty
    {
      get
      {
        //Check the local dirty flag
        if (m_bDirty)
          return true;

        //Is the scene dirty?
        if (Scene != null && Scene.Dirty)
          return true;

        //Add anything else that could also be dirty here:

        return false;
      }

      set
      {
        if (value)
        { //Set the local dirty flag
          m_bDirty = true;
        } 
        else
        {
          //Clear the local dirty flag 
          m_bDirty = false;

          //Clear the scene dirty flag
          if (Scene != null)
            Scene.Dirty = false;

          //Add anything else that could also be dirty here:
        }
      }

    }

    private bool m_bDirty;

    // Actions to be performed during closing of the project, matching the calls during loading in reverse order.
    private Stack<Action> _deinitStack = new Stack<Action>();

    #endregion

    /// <summary>
    /// Return our scene (casted)
    /// </summary>
    public EditorScene EditorScene
    {
      get { return (EditorScene) base.Scene; }
      set { base.Scene = value; }
    }
  }
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
