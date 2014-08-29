/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CSharpFramework;
using CSharpFramework.Helper;
using System.Windows.Forms;
using System.IO;
using ManagedBase.LogManaged;
using System.Drawing;

namespace Editor.Help
{
  public class HelpSystem : CSharpFramework.Help.IHelpSystem
  {
    #region Ctor/Dtor

    public HelpSystem()
    {
      // We use EditorManager.Settings as storage as this setting is saved per user.
      // Due to http://support.microsoft.com/kb/267962/en-us we can only set the language on startup
      // and have to restart vForge in order to change the language.
      if (SetHelpLanguage(EditorManager.Settings.HelpLanguage))
      {
        _currentHelpLanguage = EditorManager.Settings.HelpLanguage;
      }
      else
      {
        _currentHelpLanguage = "";
        SetHelpLanguage("");
      }
        

      // Build-in help contexts:
      AddHelpContext("panels", Path.Combine(HelpDataDir, "panels.csv"));
      AddHelpContext("dialogs", Path.Combine(HelpDataDir, "dialogs.csv"));
      AddHelpContext("shapes", Path.Combine(HelpDataDir, "shapes.csv"));
      AddHelpContext("components", Path.Combine(HelpDataDir, "components.csv"));
      AddHelpContext("postprocessors", Path.Combine(HelpDataDir, "postprocessors.csv"));
    }

    #endregion Ctor/Dtor

    #region Helper Functions

    /// <summary>
    /// Adds the sub-folder to the given path if it exists and returns the new path.
    /// If sSubFolderName is empty, the original path is returned instead.
    /// E.g. the input 'folder/file.ext' and 'subfolder' returns 'folder/subfolder/file.ext'.
    /// </summary>
    /// <param name="pathToFile"></param>
    /// <param name="sSubFolderName"></param>
    /// <returns></returns>
    public static string AddSubFolderToPath(string pathToFile, string sSubFolderName)
    {
      try
      {
        if (string.IsNullOrEmpty(sSubFolderName))
          return pathToFile;

        string sPath = Path.GetDirectoryName(pathToFile);
        string sFileName = Path.GetFileName(pathToFile);

        return Path.Combine(sPath, sSubFolderName, sFileName);
      }
      catch { }

      return pathToFile;
    }

    /// <summary>
    /// Tries to find a help file for the currently selected help language.
    /// If non is found, the original path is returned.
    /// E.g. if the input is 'folder/help.chm' and 'GetHelpLanguage' is
    /// 'korean', 'folder/korean/help.chm' is returned if it exists.
    /// </summary>
    /// <param name="pathToHelpFile"></param>
    /// <returns></returns>
    public string GetLanguageSpecificHelpFile(string relativePathToHelpFile, string language, ref bool success)
    {
      if (language == null)
        language = GetHelpLanguage();

      string sNewHelpFile = AddSubFolderToPath(relativePathToHelpFile, language);

      // Test whether we have an SDK root. If not, return the input unchanged.
      string sInstallDir = FileHelper.VisionInstallDir;
      if (sInstallDir == null)
      {
        success = false;
        return relativePathToHelpFile;
      }

      // Test whether the new help file exists on disk.
      string sAbsHelpFilePath = Path.Combine(sInstallDir, sNewHelpFile);
      if (!File.Exists(sAbsHelpFilePath))
      {
        success = false;
        return relativePathToHelpFile;
      }

      success = true;
      return sNewHelpFile;
    }

    public string GetLanguageSpecificHelpFile(string relativePathToHelpFile, string language = null)
    {
      bool bSuccess = false;
      return GetLanguageSpecificHelpFile(relativePathToHelpFile, language, ref bSuccess);
    }

    /// <summary>
    /// Tries to find a context file for the currently selected help language.
    /// If non is found, the original path is returned.
    /// E.g. if the input is 'folder/help.csv' and 'GetHelpLanguage' is
    /// 'korean', 'folder/korean/help.csv' is returned if it exists and the help manual for
    /// korean exist as well.
    /// </summary>
    /// <param name="pathToContextFile"></param>
    /// <returns></returns>
    public string GetLanguageSpecificContextFile(string pathToContextFile)
    {
      string sNewHelpFile = GetLanguageSpecificHelpFile(VISION_MANUAL_FILE);
      if (sNewHelpFile == VISION_MANUAL_FILE)
        return pathToContextFile;

      string sNewContext = AddSubFolderToPath(pathToContextFile, GetHelpLanguage());
      return File.Exists(sNewContext) ? sNewContext : pathToContextFile;
    }

    #endregion Helper Functions

    #region IHelpSystem Implementation

    public override bool SetHelpLanguage(string language)
    {
      if (String.IsNullOrEmpty(language))
      {
        // Revert to default (English)
        EditorManager.Settings.HelpLanguage = "";
      }
      else
      {
        bool bSuccess = false;
        string sNewManualPath = GetLanguageSpecificHelpFile(VISION_MANUAL_FILE, language, ref bSuccess);
        if (!bSuccess)
          return false;

        EditorManager.Settings.HelpLanguage = language;
      }

      OnHelpLanguageChanged();
      return true;
    }

    public override string GetHelpLanguage()
    {
      // We return the cached value instead of EditorManager.Settings.HelpLanguage
      // as we need to restart in order for the language change to work.
      // See (http://support.microsoft.com/kb/267962/en-us).
      return _currentHelpLanguage;
    }

    public override bool AddHelpContext(string contextName, string pathToContextFile)
    {
      if (_contextToDictionary.ContainsKey(contextName))
        return false;

      try
      {
        bool bExists = System.IO.File.Exists(pathToContextFile);
        if (bExists == false)
        {
          Log.Error("The CSV file '{0}' does not exist and thus cannot be added as a help context!", pathToContextFile);
          return false;
        }

        HelpContext hc = new HelpContext(pathToContextFile, this);
        if (!hc.Reload())
          return false;

        _contextToDictionary.Add(contextName, hc);
      }
      catch
      {
        return false;
      }

      return true;
    }

    public override bool RemoveHelpContext(string contextName)
    {
      return _contextToDictionary.Remove(contextName);
    }

    public override bool ReloadAllHelpContexts()
    {
      bool bSuccess = true;
      foreach (HelpContext context in _contextToDictionary.Values)
      {
        if (!context.Reload())
          bSuccess = false;
      }
      return bSuccess;
    }

    public override bool HasHelpPage(string key, string contextName = null)
    {
      return !String.IsNullOrEmpty(GetHelpPage(key, contextName));
    }

    public override bool ShowHelpPage(Control control, string key, string contextName = null, string manual = null)
    {
      string sHelpPage = GetHelpPage(key, contextName);
      if (String.IsNullOrEmpty(sHelpPage))
        return false;

      if (String.IsNullOrEmpty(manual))
        return ShowVisionManualHelpPage(control, sHelpPage);
      else
        return ShowManualHelpPage(control, manual, sHelpPage);
    }

    public override string GetHelpPage(string key, string contextName = null)
    {
      if (string.IsNullOrEmpty(key))
        return null;

      // If a context is given, try that one first to prevent key collisions in different contexts
      if (!string.IsNullOrEmpty(contextName) && _contextToDictionary.ContainsKey(contextName))
      {
        HelpContext hc = null;
        if (_contextToDictionary.TryGetValue(contextName, out hc))
        {
          string sPage = null;
          if (hc._dict.TryGetValue(key, out sPage))
          {
            return sPage;
          }
        }
      }

      foreach (HelpContext context in _contextToDictionary.Values)
      {
        string sPage = null;
        if (context._dict.TryGetValue(key, out sPage))
        {
          return sPage;
        }
      }

      return null;
    }

    public override bool ShowVisionManualHelpPage(System.Windows.Forms.Control control, string page)
    {
      return ShowManualHelpPage(control, GetLanguageSpecificHelpFile(VISION_MANUAL_FILE), page);
    }

    public override bool ShowManualHelpPage(System.Windows.Forms.Control control, string manual, string page)
    {
      string installDir = FileHelper.VisionInstallDir;
      if (installDir == null)
      {
        EditorManager.ShowMessageBox("The Vision SDK installation path could not be found. Make sure the Vision SDK is installed correctly.", "Show Help Page", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return false;
      }

      string helpFilePath = Path.Combine(installDir, manual);
      if (!File.Exists(helpFilePath))
      {
        EditorManager.ShowMessageBox("The help document could not be found at its location: " + helpFilePath, "Show Help Page", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        return false;
      }

      if (string.IsNullOrEmpty(page))
      {
        try
        {
          System.Windows.Forms.Help.ShowHelp(control, helpFilePath);
        }
        catch (Exception ex)
        {
          EditorManager.ShowMessageBox("Opening the manual failed with the following exception:\n\n" + ex.Message, "Show Help Page", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return false;
        }
      }
      else
      {
        try
        {
          System.Windows.Forms.Help.ShowHelp(control, helpFilePath, HelpNavigator.Topic, page);
        }
        catch (Exception ex)
        {
          EditorManager.ShowMessageBox("Opening the manual help page: '" + page + "' failed with the following exception:\n\n" + ex.Message, "Show Help Page", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return false;
        }
      }
      

      return true;
    }

    #endregion IHelpSystem Implementation

    #region Private Functions

    

    #endregion Private Functions

    #region Member Variables

    #region Internal HelpContext Class
    /// <summary>
    /// Helper class to encapsulate the help context's dictionary
    /// </summary>
    protected internal class HelpContext
    {
      public HelpContext(string absCVSFilePath, HelpSystem parentSystem)
      {
        _sAbsCVSPath = absCVSFilePath;
        _parentSystem = parentSystem;
      }

      #region HelpContext: Member Variables

      public Dictionary<string, string> _dict = new Dictionary<string, string>();
      public string _sAbsCVSPath = null;
      public HelpSystem _parentSystem = null;

      #endregion HelpContext: Member Variables

      private bool AddContextFile(string absCVSPath)
      {
        try
        {
          string[] str = System.IO.File.ReadAllLines(absCVSPath);
          foreach (string line in str)
          {
            try
            {
              List<string> parts = StringHelper.CSVSplit(line);
              if (parts != null && parts.Count == 2)
              {
                _dict[parts[0]] = parts[1];
              }
              else
              {
                Log.Error("The CSV file '{0}' contains an invalid line: {1}", absCVSPath, line);
              }

            }
            catch { }
          }
        }
        catch (Exception ex)
        {
          EditorManager.ShowMessageBox("Loading the help CSV file '" + absCVSPath + "' failed with the following exception:\n\n" + ex.Message, "Load help CSV", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return false;
        }

        return true;
      }

      public bool Reload()
      {
        _dict.Clear();

        // Load default (English version) CVS
        bool bRes = AddContextFile(_sAbsCVSPath);

        // Override entries with language specific CVS if it exists
        string sContextFile = _parentSystem.GetLanguageSpecificContextFile(_sAbsCVSPath);
        if (_sAbsCVSPath != sContextFile)
        {
          bRes = AddContextFile(sContextFile) && bRes;
        }
        return bRes;
      }
    }

    #endregion Internal HelpContext Class

    Dictionary<string, HelpContext> _contextToDictionary = new Dictionary<string, HelpContext>();
    string _currentHelpLanguage = "";
    const string VISION_MANUAL_FILE = @"Docs\Vision\windows\VisionEngine_Documentation_windows.CHM";
    
    #endregion Member Variables
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
