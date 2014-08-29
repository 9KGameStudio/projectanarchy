/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using CSharpFramework;
using CSharpFramework.Docking;
using WeifenLuo.WinFormsUI;
using CSharpFramework.Shapes;
using CSharpFramework.Scene;
using CSharpFramework.Helper;
using ManagedFramework;

namespace Editor
{
	/// <summary>
  /// Panel that lets you view the scripts that are available in the scene
	/// </summary>
	public class ScriptListPanel : DockableForm
  {
    #region pirvate variables
    
    private ListView scriptsListControl;
    private ImageList imageList1;
    private ColumnHeader headerDirectory;
    private ContextMenuStrip scriptListContextMenu;
    private ToolStripMenuItem openMenuItem;
    private ToolStripMenuItem usedByMenuItem;
    private ToolStrip toolStrip_Shapes;
    private ToolStripDropDownButton toolStripDropDownButton1;
    private ToolStripMenuItem selectedLayerOnlyToolStripMenuItem;
    private ToolStripButton refreshButton;
    private ColumnHeader headerReferenceCount;
    private ToolStripMenuItem searchProjectDirectoryToolStripMenuItem;
    private ToolStripMenuItem allLayersStripMenuItem;
    private IContainer components;

    #endregion

    /// <summary>
    /// Constructor
    /// </summary>
		public ScriptListPanel(DockingContainer container) : base(container)
		{
      InitializeComponent();

      EditorScene.LayerChanged += new LayerChangedEventHandler(EditorScene_LayerChanged);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.SceneEvent += new SceneEventHandler(EditorManager_SceneEvent);
      scriptsListControl.MouseDoubleClick += new MouseEventHandler(_listBox_MouseDoubleClick);
		}


    /// <summary>
    /// On scene event
    /// </summary>
    void EditorManager_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action == SceneEventArgs.Action.AfterLoading)
        FillScriptList();
    }

    /// <summary>
    /// On scene changed
    /// </summary>
    private void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if(e.newscene == null)
        scriptsListControl.Items.Clear();
      else
        FillScriptList();
    }

    /// <summary>
    /// Mouse double click handling
    /// </summary>
    private void _listBox_MouseDoubleClick(object sender, MouseEventArgs e)
    {
      EditorManager.OnScriptSelectionChanged(new ScriptSelectionChangedArgs(scriptsListControl.SelectedItems[0].Text));
    }

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
          EditorScene.LayerChanged -= new LayerChangedEventHandler(EditorScene_LayerChanged);
          EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
          EditorManager.SceneEvent -= new SceneEventHandler(EditorManager_SceneEvent);
          scriptsListControl.MouseDoubleClick -= new MouseEventHandler(_listBox_MouseDoubleClick);

					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

    void EditorScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      if (e.action == LayerChangedArgs.Action.Selected)
      {
         FillScriptList();
      }
    }

    static void DirSearch(string sDir, string sFilter, ref List<string> sFilesOut)
    {
        try
        {
          foreach (string files in Directory.GetFiles(sDir, sFilter))
          {
            sFilesOut.Add(files);
          }
          foreach (string directory in Directory.GetDirectories(sDir))
          {
            if(directory.ToLower().Contains(".shaderbin"))
              continue;
            DirSearch(directory, sFilter, ref sFilesOut);
          }
        }
        catch (System.Exception)
        {
            // simply ignore the error and exit out...
        }
    }

    /// <summary>
    /// Add all the unique scripts from shape list to the dictionary
    /// </summary>
    private void AddScriptsRecursively(ShapeCollection shapes, ref Dictionary<string, int> scriptsDictionaryOut)
    {
      ShapeComponentType scriptComponentType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VScriptComponent");

      foreach (ShapeBase shape in shapes)
      {
        if (shape.HasChildren())
          AddScriptsRecursively(shape.ChildCollection, ref scriptsDictionaryOut);

        if (shape.Components == null)
          continue;

        foreach (ShapeComponent comp in shape.Components)
        {
          if (comp.CollectionType == scriptComponentType)
          {
            string script = comp.GetPropertyByName("ScriptFile").Value.ToString();

            if (string.IsNullOrEmpty(script))
              continue;

            int references = 0;
            scriptsDictionaryOut.TryGetValue(script, out references);
            scriptsDictionaryOut[script] = references + 1;
          }
        }  
      }
    }


		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ScriptListPanel));
      this.scriptsListControl = new System.Windows.Forms.ListView();
      this.headerDirectory = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.headerReferenceCount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.imageList1 = new System.Windows.Forms.ImageList(this.components);
      this.scriptListContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.openMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.usedByMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip_Shapes = new System.Windows.Forms.ToolStrip();
      this.toolStripDropDownButton1 = new System.Windows.Forms.ToolStripDropDownButton();
      this.allLayersStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectedLayerOnlyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.searchProjectDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.refreshButton = new System.Windows.Forms.ToolStripButton();
      this.scriptListContextMenu.SuspendLayout();
      this.toolStrip_Shapes.SuspendLayout();
      this.SuspendLayout();
      // 
      // scriptsListControl
      // 
      this.scriptsListControl.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.headerDirectory,
            this.headerReferenceCount});
      this.scriptsListControl.Dock = System.Windows.Forms.DockStyle.Fill;
      this.scriptsListControl.FullRowSelect = true;
      this.scriptsListControl.GridLines = true;
      this.scriptsListControl.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.scriptsListControl.Location = new System.Drawing.Point(0, 25);
      this.scriptsListControl.MultiSelect = false;
      this.scriptsListControl.Name = "scriptsListControl";
      this.scriptsListControl.Size = new System.Drawing.Size(292, 364);
      this.scriptsListControl.SmallImageList = this.imageList1;
      this.scriptsListControl.TabIndex = 2;
      this.scriptsListControl.UseCompatibleStateImageBehavior = false;
      this.scriptsListControl.View = System.Windows.Forms.View.Details;
      this.scriptsListControl.MouseClick += new System.Windows.Forms.MouseEventHandler(this.OnMouseClick);
      // 
      // headerDirectory
      // 
      this.headerDirectory.Text = "Directory";
      this.headerDirectory.Width = 180;
      // 
      // headerReferenceCount
      // 
      this.headerReferenceCount.Text = "References";
      this.headerReferenceCount.Width = 80;
      // 
      // imageList1
      // 
      this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
      this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
      this.imageList1.Images.SetKeyName(0, "scrollList_unused.png");
      this.imageList1.Images.SetKeyName(1, "scrollList_used.png");
      // 
      // scriptListContextMenu
      // 
      this.scriptListContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openMenuItem,
            this.usedByMenuItem});
      this.scriptListContextMenu.Name = "scriptListContextMenu";
      this.scriptListContextMenu.Size = new System.Drawing.Size(126, 48);
      // 
      // openMenuItem
      // 
      this.openMenuItem.Name = "openMenuItem";
      this.openMenuItem.Size = new System.Drawing.Size(125, 22);
      this.openMenuItem.Text = "Open";
      this.openMenuItem.Click += new System.EventHandler(this.openMenuItem_Click);
      // 
      // usedByMenuItem
      // 
      this.usedByMenuItem.Name = "usedByMenuItem";
      this.usedByMenuItem.Size = new System.Drawing.Size(125, 22);
      this.usedByMenuItem.Text = "Used by...";
      // 
      // toolStrip_Shapes
      // 
      this.toolStrip_Shapes.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Shapes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripDropDownButton1,
            this.refreshButton});
      this.toolStrip_Shapes.Location = new System.Drawing.Point(0, 0);
      this.toolStrip_Shapes.Name = "toolStrip_Shapes";
      this.toolStrip_Shapes.Size = new System.Drawing.Size(292, 25);
      this.toolStrip_Shapes.TabIndex = 0;
      this.toolStrip_Shapes.Text = "toolStrip2";
      // 
      // toolStripDropDownButton1
      // 
      this.toolStripDropDownButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripDropDownButton1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.allLayersStripMenuItem,
            this.selectedLayerOnlyToolStripMenuItem,
            this.searchProjectDirectoryToolStripMenuItem});
      this.toolStripDropDownButton1.Image = global::Editor.Properties.Resources.toolbar_list;
      this.toolStripDropDownButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripDropDownButton1.Name = "toolStripDropDownButton1";
      this.toolStripDropDownButton1.Size = new System.Drawing.Size(29, 22);
      this.toolStripDropDownButton1.Text = "toolStripDropDownButton1";
      this.toolStripDropDownButton1.ToolTipText = "Select which scripts to be filtered out";
      // 
      // allLayersStripMenuItem
      // 
      this.allLayersStripMenuItem.AutoToolTip = true;
      this.allLayersStripMenuItem.Checked = true;
      this.allLayersStripMenuItem.CheckOnClick = true;
      this.allLayersStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
      this.allLayersStripMenuItem.Name = "allLayersStripMenuItem";
      this.allLayersStripMenuItem.Size = new System.Drawing.Size(177, 22);
      this.allLayersStripMenuItem.Text = "All Layers";
      this.allLayersStripMenuItem.ToolTipText = "Show the scripts used by any layer.";
      this.allLayersStripMenuItem.Click += new System.EventHandler(this.allLayersStripMenu_Clicked);
      // 
      // selectedLayerOnlyToolStripMenuItem
      // 
      this.selectedLayerOnlyToolStripMenuItem.AutoToolTip = true;
      this.selectedLayerOnlyToolStripMenuItem.CheckOnClick = true;
      this.selectedLayerOnlyToolStripMenuItem.Name = "selectedLayerOnlyToolStripMenuItem";
      this.selectedLayerOnlyToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
      this.selectedLayerOnlyToolStripMenuItem.Text = "Selected Layer Only";
      this.selectedLayerOnlyToolStripMenuItem.ToolTipText = "Show only the scripts used in the selected layer";
      this.selectedLayerOnlyToolStripMenuItem.Click += new System.EventHandler(this.selectedLayerOnlyMenuItem_Clicked);
      // 
      // searchProjectDirectoryToolStripMenuItem
      // 
      this.searchProjectDirectoryToolStripMenuItem.AutoToolTip = true;
      this.searchProjectDirectoryToolStripMenuItem.CheckOnClick = true;
      this.searchProjectDirectoryToolStripMenuItem.Name = "searchProjectDirectoryToolStripMenuItem";
      this.searchProjectDirectoryToolStripMenuItem.Size = new System.Drawing.Size(177, 22);
      this.searchProjectDirectoryToolStripMenuItem.Text = "Project Directory";
      this.searchProjectDirectoryToolStripMenuItem.ToolTipText = "Search the entire project directory for scripts";
      this.searchProjectDirectoryToolStripMenuItem.Click += new System.EventHandler(this.searchProjectDirectoryToolStrip_Clicked);
      // 
      // refreshButton
      // 
      this.refreshButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.refreshButton.Image = global::Editor.Properties.Resources.toolbar_refresh;
      this.refreshButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.refreshButton.Name = "refreshButton";
      this.refreshButton.Size = new System.Drawing.Size(23, 22);
      this.refreshButton.Text = "toolStripButton1";
      this.refreshButton.ToolTipText = "Refresh script list";
      this.refreshButton.Click += new System.EventHandler(this.RefreshButtonClick);
      // 
      // ScriptListPanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(292, 389);
      this.Controls.Add(this.scriptsListControl);
      this.Controls.Add(this.toolStrip_Shapes);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)(((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.DockAreas.DockRight)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ScriptListPanel";
      this.TabText = "Scripts List";
      this.Text = "Script List (current layer)";
      this.scriptListContextMenu.ResumeLayout(false);
      this.toolStrip_Shapes.ResumeLayout(false);
      this.toolStrip_Shapes.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
		#endregion


    private void ScriptListSubmenuItemMainLayer_Click(object sender, EventArgs e)
    {
      IScene.SendLayerChangedEvent(new LayerChangedArgs(EditorManager.Scene.MainLayer, null, LayerChangedArgs.Action.PropertyChanged));
    }

    private void ScriptListSubmenuItem_Click(object sender, EventArgs e)
    {
      var item = sender as ToolStripMenuItem;

      ShapeBase shape = EditorManager.Scene.FindShapeByID((ulong)item.Tag);

      if (shape != null) 
      {
        ShapeCollection shapeColl = new ShapeCollection();
        shapeColl.Add(shape);
        EditorManager.Scene.ActiveLayer = shape.ParentLayer;
        EditorManager.SelectedShapes = shapeColl;
      }
    }

    /// <summary>
    /// Handle mouse double click on script list
    /// </summary>
    private void OnMouseClick(object sender, MouseEventArgs e)
    {
      if (e.Button == System.Windows.Forms.MouseButtons.Right)
      {
        usedByMenuItem.DropDownItems.Clear();

        string selectedScript = scriptsListControl.SelectedItems[0].Text;

        string sceneScript = "";
        bool found = ScriptManager.GetSceneScriptFile(ref sceneScript);

        if (found && selectedScript == sceneScript)
        {
            ToolStripMenuItem item = new ToolStripMenuItem("Scene Script", null, new EventHandler(ScriptListSubmenuItemMainLayer_Click));
            usedByMenuItem.DropDownItems.Add(item);
        }
        else
        {
            ShapeCollection shapes = EditorManager.Scene.GetShapesByScript(selectedScript);

            foreach (ShapeBase shape in shapes)
            {
                ToolStripMenuItem item = new ToolStripMenuItem(shape.ShapeName, null, new EventHandler(ScriptListSubmenuItem_Click));
                item.Tag = shape.UniqueID;

                usedByMenuItem.DropDownItems.Add(item);
            }
        }

        usedByMenuItem.Enabled = (usedByMenuItem.DropDownItems.Count > 0);
        scriptListContextMenu.Show(scriptsListControl, e.Location);
      }
    }

    private void openMenuItem_Click(object sender, EventArgs e)
    {
      EditorManager.OnScriptSelectionChanged(new ScriptSelectionChangedArgs(scriptsListControl.SelectedItems[0].Text));
    }


    private void FillScriptList()
    {
      if (EditorManager.Scene == null)
        return;

      scriptsListControl.Items.Clear();

      Dictionary<string, int> scripts = new Dictionary<string, int>();

      if (searchProjectDirectoryToolStripMenuItem.Checked)
      {
        List<string> files = new List<string>();

        DirSearch(EditorManager.Project.ProjectDir, "*.lua", ref files);
        foreach (string file in files)
        {
          string path = FileHelper.MakePathRelative(EditorManager.Project.ProjectDir, file);
          scripts[path] = 0;
        }

        foreach (var dataDir in EditorManager.Project.CustomDataDirectories)
        {
          files.Clear();
          DirSearch(dataDir.NativePath, "*.lua", ref files);
          foreach (string file in files)
          {
            string path = FileHelper.MakePathRelative(dataDir.NativePath, file);
            scripts[path] = 0;
          }
        }
      }

      // add scene's script file to the list
      if ((!selectedLayerOnlyToolStripMenuItem.Checked || EditorManager.Scene.ActiveLayer == EditorManager.Scene.MainLayer))
      {
        string sceneScript = "";
        bool found = ScriptManager.GetSceneScriptFile(ref sceneScript);
        if (found)
        {
          int references = 0;
          scripts.TryGetValue(sceneScript, out references);
          scripts[sceneScript] = references + 1;
        }
      }

      if (selectedLayerOnlyToolStripMenuItem.Checked) // display scripts for current layer
      {
        AddScriptsRecursively(EditorManager.Scene.ActiveLayer.Root.ChildCollection, ref scripts);
      }
      else
      {
        foreach (Layer layer in EditorManager.Scene.Layers)
        {
          AddScriptsRecursively(layer.Root.ChildCollection, ref scripts);
        }
      }

      foreach (KeyValuePair<string, int> pair in scripts)
      {
          ListViewItem item = new ListViewItem(pair.Key);
          
          item.SubItems.Add(pair.Value.ToString());
          item.ImageIndex = 0;

          if (pair.Value == 0)
          { 
              item.ForeColor = Color.Gray;
              item.ToolTipText = "unused";
          }

          scriptsListControl.Items.Add(item);
      }
    }

    private void RefreshButtonClick(object sender, EventArgs e)
    {
      FillScriptList();
    }

    /// <summary>
    /// Handle display mode change for script list (all or per layer)
    /// </summary>
    private void selectedLayerOnlyMenuItem_Clicked(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item.Checked == false)
      { 
        item.Checked = true;
        return;
      }

      allLayersStripMenuItem.Checked = false;
      searchProjectDirectoryToolStripMenuItem.Checked = false;

      FillScriptList();
    }

    private void searchProjectDirectoryToolStrip_Clicked(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item.Checked == false)
      {
        item.Checked = true;
        return;
      }


      allLayersStripMenuItem.Checked = false;
      selectedLayerOnlyToolStripMenuItem.Checked = false;
      FillScriptList();
    }

    private void allLayersStripMenu_Clicked(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item.Checked == false)
      {
        item.Checked = true;
        return;
      }

      selectedLayerOnlyToolStripMenuItem.Checked = false;
      searchProjectDirectoryToolStripMenuItem.Checked = false;
      FillScriptList();
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
