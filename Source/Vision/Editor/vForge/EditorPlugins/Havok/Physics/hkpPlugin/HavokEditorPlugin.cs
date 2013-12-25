/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;
using NUnit.Core;
using CSharpFramework;
using ManagedFramework;
using CSharpFramework.Scene;
using CSharpFramework.Serialization;
using HavokEditorPlugin.Shapes;
using VisionEditorPlugin.Shapes;
using CSharpFramework.Shapes;
using CSharpFramework.Actions;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Math;


namespace HavokEditorPlugin
{

  /// <summary>
  /// Main plugin class
  /// </summary>
  public class EditorPlugin : IEditorPluginModule
  {
    /// <summary>
    /// Constructor of EditorPlugin that sets up plugin information
    /// </summary>
    public EditorPlugin()
    {
      _version = 0;                   //version used for serialization
      _name = "HavokEditorPlugin";
    }

    /// <summary>
    /// This scope holds one global instance of the plugin info. It is returned by all shape's overridden GetPluginInformation function.
    /// This global info is filled inside InitPluginModule
    /// </summary>
    public static EditorPluginInfo EDITOR_PLUGIN_INFO = new EditorPluginInfo();

    #region InitPluginModule : register the creator plugins

    HavokPhysicsPanel _panel = null; 

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool InitPluginModule()
    {
      EDITOR_PLUGIN_INFO.NativePluginNames = new string[] { "Havok" };
      HavokManaged.ManagedModule.InitManagedModule();

      StaticMeshShape.UsesCollisionGroups = true; // switch to displaying collision groups
      
      // register shape classes
      creators = new IShapeCreatorPlugin[]
                 {
                   new HavokConstraintShapeCreator(),
                   new HavokConstraintChainShapeCreator()
                 };

      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Add( plugin );

      // create and activate panel
      _panel = new HavokPhysicsPanel(EditorManager.ApplicationLayout.DockingArea);
      _panel.ShowDockable();

      // Register automated tests
      TestSuiteBuilder testBuilder = new TestSuiteBuilder();
      TestSuite testSuite = testBuilder.Build(typeof(EditorPlugin).Assembly.FullName);
      TestManager.AddTestSuite(testSuite);

      return true;
    }


    #endregion

    #region DeInitPluginModule

    /// <summary>
    /// Overridden function that gets called when the plugin is loaded. Registers all creator plugins
    /// </summary>
    /// <returns></returns>
    public override bool DeInitPluginModule()
    {
      HavokManaged.ManagedModule.DeInitManagedModule();
      foreach (IShapeCreatorPlugin plugin in creators)
        EditorManager.ShapeCreatorPlugins.Remove( plugin );

      _panel.Close();
      _panel.Dispose();
      _panel = null;

      return true;
    }

    IShapeCreatorPlugin[] creators;

    #endregion
  }


}

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
