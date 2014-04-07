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
using CSharpFramework;
using CSharpFramework.Docking;
using WeifenLuo.WinFormsUI;
using CSharpFramework.Shapes;
using CSharpFramework.Scene;
using ManagedFramework;

namespace Editor
{
	/// <summary>
  /// Panel that lets you view the scripts that are available in the scene
	/// </summary>
	public class ScriptListPanel : DockableForm
  {
    #region pirvate variables
    
    private ListView listView1;
    private ImageList imageList1;
    private ColumnHeader columnHeader1;
    private ContextMenuStrip scriptListContextMenu;
    private ToolStripMenuItem openMenuItem;
    private ToolStripMenuItem toolStripMenuItem1;
    private ToolStrip toolStrip_Shapes;
    private ToolStripDropDownButton toolStripDropDownButton1;
    private ToolStripMenuItem displayAllToolStripMenuItem;
    private ToolStripButton refreshButton;
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
      listView1.MouseDoubleClick += new MouseEventHandler(_listBox_MouseDoubleClick);
		}

    /// <summary>
    /// On scene changed
    /// </summary>
    private void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      if(e.newscene == null)
        listView1.Items.Clear();
    }

    /// <summary>
    /// Mouse double click handling
    /// </summary>
    private void _listBox_MouseDoubleClick(object sender, MouseEventArgs e)
    {
      EditorManager.OnScriptSelectionChanged(new ScriptSelectionChangedArgs(listView1.SelectedItems[0].Text));
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
          listView1.MouseDoubleClick -= new MouseEventHandler(_listBox_MouseDoubleClick);

					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

    void EditorScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      if (e.action == LayerChangedArgs.Action.Selected)
      {
        if (!displayAllToolStripMenuItem.Checked)
        {
          listView1.Items.Clear();
          AddScriptsRecursively(e.layer.Root.ChildCollection);
        }
      }
    }

    /// <summary>
    /// Add all the unique scripts from shape list to the lisbtox
    /// </summary>
    private void AddScriptsRecursively(ShapeCollection shapes)
    {
      ShapeComponentType scriptComponentType = (ShapeComponentType)EditorManager.EngineManager.ComponentClassManager.GetCollectionType("VScriptComponent");

      if (EditorManager.Scene != null && EditorManager.Scene.ActiveLayer == EditorManager.Scene.MainLayer)
      {
        string sceneScript = "";
        bool found = ScriptManager.GetSceneScriptFile(ref sceneScript);

        if (found && listView1.FindItemWithText(sceneScript) == null)
          listView1.Items.Add(new ListViewItem { ImageIndex = 0, Text = sceneScript });          
      }

      foreach (ShapeBase shape in shapes)
      {
        if (shape.HasChildren())
          AddScriptsRecursively(shape.ChildCollection);

        if (shape.Components == null)
          continue;

        foreach (ShapeComponent comp in shape.Components)
        {
          if (comp.CollectionType == scriptComponentType)
          {
            string script = comp.GetPropertyByName("ScriptFile").Value.ToString();

            if (string.IsNullOrEmpty(script))
              continue;

            if (listView1.FindItemWithText(script) == null)
              listView1.Items.Add(new ListViewItem { ImageIndex = 0, Text = script });          
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
      this.listView1 = new System.Windows.Forms.ListView();
      this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.imageList1 = new System.Windows.Forms.ImageList(this.components);
      this.scriptListContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.openMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip_Shapes = new System.Windows.Forms.ToolStrip();
      this.toolStripDropDownButton1 = new System.Windows.Forms.ToolStripDropDownButton();
      this.displayAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.refreshButton = new System.Windows.Forms.ToolStripButton();
      this.scriptListContextMenu.SuspendLayout();
      this.toolStrip_Shapes.SuspendLayout();
      this.SuspendLayout();
      // 
      // listView1
      // 
      this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
      this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView1.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
      this.listView1.Location = new System.Drawing.Point(0, 25);
      this.listView1.MultiSelect = false;
      this.listView1.Name = "listView1";
      this.listView1.Size = new System.Drawing.Size(292, 364);
      this.listView1.SmallImageList = this.imageList1;
      this.listView1.TabIndex = 2;
      this.listView1.UseCompatibleStateImageBehavior = false;
      this.listView1.View = System.Windows.Forms.View.Details;
      this.listView1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.OnMouseClick);
      // 
      // columnHeader1
      // 
      this.columnHeader1.Width = 286;
      // 
      // imageList1
      // 
      this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
      this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
      this.imageList1.Images.SetKeyName(0, "scroll.ico");
      // 
      // scriptListContextMenu
      // 
      this.scriptListContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openMenuItem,
            this.toolStripMenuItem1});
      this.scriptListContextMenu.Name = "scriptListContextMenu";
      this.scriptListContextMenu.Size = new System.Drawing.Size(117, 48);
      // 
      // openMenuItem
      // 
      this.openMenuItem.Name = "openMenuItem";
      this.openMenuItem.Size = new System.Drawing.Size(116, 22);
      this.openMenuItem.Text = "Open";
      this.openMenuItem.Click += new System.EventHandler(this.openMenuItem_Click);
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(116, 22);
      this.toolStripMenuItem1.Text = "Used by";
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
            this.displayAllToolStripMenuItem});
      this.toolStripDropDownButton1.Image = global::Editor.Properties.Resources.toolbar_list;
      this.toolStripDropDownButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripDropDownButton1.Name = "toolStripDropDownButton1";
      this.toolStripDropDownButton1.Size = new System.Drawing.Size(29, 22);
      this.toolStripDropDownButton1.Text = "toolStripDropDownButton1";
      this.toolStripDropDownButton1.ToolTipText = "Display scripts from all layers";
      // 
      // displayAllToolStripMenuItem
      // 
      this.displayAllToolStripMenuItem.AutoToolTip = true;
      this.displayAllToolStripMenuItem.CheckOnClick = true;
      this.displayAllToolStripMenuItem.Name = "displayAllToolStripMenuItem";
      this.displayAllToolStripMenuItem.Size = new System.Drawing.Size(164, 22);
      this.displayAllToolStripMenuItem.Text = "Display all scripts";
      this.displayAllToolStripMenuItem.ToolTipText = "Display all scripts";
      this.displayAllToolStripMenuItem.CheckedChanged += new System.EventHandler(this.displayAllToolStripMenuItem_CheckedChanged);
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
      this.Controls.Add(this.listView1);
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


    private void ScriptListSubmenuItem_Click(object sender, EventArgs e)
    {
      var item = sender as ToolStripMenuItem;

      ShapeBase shape = EditorManager.Scene.FindShapeByID((ulong)item.Tag);

      if (shape != null) 
      {
        ShapeCollection shapeColl = new ShapeCollection();
        shapeColl.Add(shape);
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
        (scriptListContextMenu.Items[1] as ToolStripMenuItem).DropDownItems.Clear();

        ShapeCollection shapes = EditorManager.Scene.GetShapesByScript(listView1.SelectedItems[0].Text);

        foreach (ShapeBase shape in shapes)
        {
          ToolStripMenuItem item = new ToolStripMenuItem(shape.ShapeName, null, new EventHandler(ScriptListSubmenuItem_Click));
          item.Tag = shape.UniqueID;

          (scriptListContextMenu.Items[1] as ToolStripMenuItem).DropDownItems.Add(item);
        }

        scriptListContextMenu.Show(listView1, e.Location);
      }
    }

    private void openMenuItem_Click(object sender, EventArgs e)
    {
      EditorManager.OnScriptSelectionChanged(new ScriptSelectionChangedArgs(listView1.SelectedItems[0].Text));
    }


    private void FillScriptList()
    {
      listView1.Items.Clear();

      if (displayAllToolStripMenuItem.Checked) // display all scripts
      {
        foreach (Layer layer in EditorManager.Scene.Layers)
        {
          AddScriptsRecursively(layer.Root.ChildCollection);
        }
      }
      else // display scripts for current layer
      {
        AddScriptsRecursively(EditorManager.Scene.ActiveLayer.Root.ChildCollection);
      }
    }

    /// <summary>
    /// Handle display mode change for script list (all or per layer)
    /// </summary>
    private void displayAllToolStripMenuItem_CheckedChanged(object sender, EventArgs e)
    {
      FillScriptList();
    }

    private void RefreshButtonClick(object sender, EventArgs e)
    {
      FillScriptList();
    }

	}
}

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
