/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Actions;
using Editor.Actions;
using CSharpFramework.Shapes;
using Editor.Shapes;
using CSharpFramework.Clipboard;
using CSharpFramework.View;
using CSharpFramework.Docking;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Dialogs;
using System.IO;
using ManagedFramework;
using System.Collections.Generic;
using System.Globalization;
using System.Collections.Specialized;
using CSharpFramework.Controls;

namespace Editor
{
	/// <summary>
	/// ShapeTreePanel. Contains the tree view for shapes and layers/zones.
	/// </summary>
	public partial class ShapeTreePanel : DockableForm
	{
    #region Members

    private System.ComponentModel.IContainer components;
    
    // Background color of the active shape
    private static readonly Color ACTIVESHAPE_BKCOLOR = Color.Yellow;

    // Help buttons
    ToolStripHelpButton _helpbuttonShapes = null;
    ToolStripHelpButton _helpbuttonLayers = null;

    #endregion Members
    
    #region Constructor/Destructor
    
    /// <summary>
    /// Constructor
    /// </summary>
    public ShapeTreePanel(DockingContainer container) : base(container)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

      // Add help buttons
      _helpbuttonShapes = new ToolStripHelpButton("Shapes Panel");
      toolStrip_Shapes.Items.Add(_helpbuttonShapes);
      _helpbuttonLayers = new ToolStripHelpButton("Layer Panel");
      toolStrip1.Items.Add(_helpbuttonLayers);

      //check if in design mode. visual studio 2010 can crash when you try to call any static functions or members from EditorManager when in design mode
      if (System.ComponentModel.LicenseManager.UsageMode != System.ComponentModel.LicenseUsageMode.Designtime)
      {
        // set specific image lists
        shapeTreeView.ImageList = EditorManager.GUI.ShapeTreeImages.ImageList;

        EditorManager.SceneChanged += new SceneChangedEventHandler(shapeTreeView.OnSceneChanged);
        EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);

        EditorScene.ShapeChanged += new ShapeChangedEventHandler(shapeTreeView.OnShapeChanged);
        EditorScene.LayerChanged += new LayerChangedEventHandler(shapeTreeView.OnLayerChanged);
        EditorScene.LayerChanged += new LayerChangedEventHandler(EditorScene_LayerChanged);

        //EditorScene.LayerChanged += new LayerChangedEventHandler(layerListView.OnLayerChanged);
        //EditorManager.SceneChanged += new SceneChangedEventHandler(layerListView.OnSceneChanged);

        CSharpFramework.Layer.ActiveShapeChanged += new CSharpFramework.Layer.ActiveShapeChangedEventHandler(Layer_ActiveShapeChanged);
        shapeTreeView.NodeStateChanged += new CSharpFramework.TreeViewMS.NodeStateChangedEventHandlers(shapeTreeView_NodeStateChanged);

        // make these two context menus available in the control
        treeView_Layers.ContextMenu_Layers = this.contextMenuStrip_Layer;
        treeView_Layers.ContextMenu_Zones = this.contextMenuStrip_Zones;
        treeView_Layers.OnSelectionChanged += new EventHandler(treeView_Layers_OnSelectionChanged);
        UpdateLayerToolbar();
        UpdatedLayerInfoPanelVisible = false;

        // associate drop-to-floor directions
        directionPosXToolStripMenuItem.Tag = Vector3F.XAxis;
        directionNegXToolStripMenuItem.Tag = -Vector3F.XAxis;
        directionPosYToolStripMenuItem.Tag = Vector3F.YAxis;
        directionNegYToolStripMenuItem.Tag = -Vector3F.YAxis;
        directionCeilingToolStripMenuItem.Tag = Vector3F.ZAxis;
        directionFloorToolStripMenuItem.Tag = -Vector3F.ZAxis;

      }

    }


		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		

    /// <summary>
    /// Private constructor. Necessary to get this form properly shown in the designer when deriving from it.
    /// See ticket #1067 and http://stackoverflow.com/questions/1216940/
    /// </summary>
    private ShapeTreePanel()
      : base(null)
    {
      InitializeComponent();
    }

    #endregion Constructor/Destructor

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		
		#endregion

    #region Event Listeners

    /// <summary>
    /// Updates the treenode background color for the active shape
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Layer_ActiveShapeChanged(object sender, CSharpFramework.Layer.ActiveShapeChangedArgs e)
    {
      if (e.OldActiveShape != null)
      {
        // Restore the treenode background of the previously active shape
        ShapeTreeNode treeNode = shapeTreeView.FindShapeInTree(e.OldActiveShape, null);
        if (treeNode != null)
          treeNode.BackColor = Color.Empty;
      }

      if (e.NewActiveShape != null)
      {
        // Highlight the treenode for the new active shape
        ShapeTreeNode treeNode = shapeTreeView.FindShapeInTree(e.NewActiveShape, null);
        if (treeNode != null)
          treeNode.BackColor = ACTIVESHAPE_BKCOLOR;
      }
    }

    private void shapeTreeView_NodeStateChanged(object sender, CSharpFramework.TreeViewMS.NodeStateChangedArgs e)
    {
      ShapeTreeNode newNode = e.NewNode as ShapeTreeNode;
      if (e.Reason == TreeViewMS.NodeStateChangedArgs.Reasons.NodeCreated || e.Reason == TreeViewMS.NodeStateChangedArgs.Reasons.NodeUnSelected)
        if (newNode != null && EditorManager.Scene != null && newNode.shape == EditorManager.Scene.ActiveLayer.ActiveShape)
          newNode.BackColor = ACTIVESHAPE_BKCOLOR;      
    }


    void treeView_Layers_OnSelectionChanged(object sender, EventArgs e)
    {
      UpdateLayerToolbar();
    }

    #endregion

    #region Shape Context Menu


    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="order"></param>
    private void SortShapes(SortOrder order)
    {
      if (EditorManager.Scene == null || EditorManager.Scene.ActiveLayer == null)
        return;

      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;

      if (shapes != null && shapes.Count > 0)
      {
        EditorManager.Actions.StartGroup("Sort shapes");
        foreach (ShapeBase shape in shapes)
        {
          if (shape.HasChildren())
          {
            EditorManager.Actions.Add(new SortShapeChildrenAlphabeticallyAction(shape, order));
          }
          else
          {
            ShapeBase parent = shape.Parent;

            if (parent == null)
              parent = EditorManager.Scene.ActiveLayer.Root;

            if (parent != null)
              EditorManager.Actions.Add(new SortShapeChildrenAlphabeticallyAction(parent, order));
          }
        }
        EditorManager.Actions.EndGroup();
      }
      else
      {
        ShapeBase parent = EditorManager.Scene.ActiveLayer.Root;

        if (parent != null)
          EditorManager.Actions.Add(new SortShapeChildrenAlphabeticallyAction(parent, order));
      }
    }

    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="mode"></param>
    private void DoDropToFloor(Shape3D.DropToFloorMode mode, Vector3F axis)
    {
      Cursor = Cursors.WaitCursor;
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      EditorManager.Actions.Add(new DropToFloorAction(shapes, mode, axis, true));
      EditorApp.ActiveView.UpdateView(false);
      Cursor = Cursors.Default;
    }

    private void contextMenuStrip_Shapes_Opening(object sender, CancelEventArgs e)
    {
      ContextMenuStrip menu = (ContextMenuStrip)sender;

      ShapeCollection shapes = shapeTreeView.SelectedShapes;

      // find the entry in the context menu and remeber its index
      ToolStripMenuItem operationItem = EditorManager.FindSubMenuItem(menu.Items, "Relevant Operations");
      int index = (operationItem != null) ? menu.Items.IndexOf(operationItem) : -1;
      int iSelCount = (shapes != null) ? shapes.Count : 0;

      // remove the old entry 
      menu.Items.Remove(operationItem);

      // and add the current one
      operationItem = shapes.GetRelevantOperationsMenuItem();
      if (index < 0)
        menu.Items.Add(operationItem);
      else
        menu.Items.Insert(index, operationItem);
      operationItem.Image = global::Editor.Properties.Resources.wrench; // removing seems to reset this

      // fill the Create items
      EditorManager.BuildCreateMenu(createToolStripMenuItem);

      // enable/disable all
      bool bHasScene = EditorManager.Scene != null;
      foreach (ToolStripItem item in menu.Items)
        item.Enabled = bHasScene;

      bool bAnyMovable = false;
      if (shapes!=null)
        foreach (ShapeBase shape in shapes)
        {
          if (!(shape is Shape3D) || !shape.Modifiable || shape.IsHintSet(ShapeBase.HintFlags_e.NoUserTransform))
            continue;
          bAnyMovable = true;
        }
      dropToFloorToolStripMenuItem.Visible = bAnyMovable;

      createPrefabToolStripMenuItem.Enabled = bHasScene && iSelCount > 0;

      this.lockShapesToolStripMenuItem.Enabled = iSelCount > 0;
      if (lockShapesToolStripMenuItem.Enabled)
      {
        string text = shapes.Count == 1 ? "shape" : "shapes";
        if (shapes.EvaluateUseShapeUnLockingPreference())
        {
          lockShapesToolStripMenuItem.Text = "Un-freeze " + text;
          this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_open;
        }
        else
        {
          lockShapesToolStripMenuItem.Text = "Freeze " + text;
          this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
        }
      }

      // Layers shown in "Move to Layer" item
      if (bHasScene && shapes.Count>0)
      {
        moveToLayerToolStripMenuItem.DropDownItems.Clear();
        foreach (Layer layer in EditorManager.Scene.Layers)
          if (layer.Modifiable && layer.Loaded && layer != EditorManager.Scene.ActiveLayer)
          {
            if (!SetShapeParentAction.TestShapesLinkToDifferentZone(shapes, layer.Root, false))
              continue;

            ToolStripItem item = moveToLayerToolStripMenuItem.DropDownItems.Add(layer.LayerName);
            item.Tag = layer;
            item.Click += new EventHandler(ShapeMoveToLayerItem_Click);
          }
        moveToLayerToolStripMenuItem.Enabled = moveToLayerToolStripMenuItem.DropDownItems.Count > 0;
      }
      else
      {
        moveToLayerToolStripMenuItem.Enabled = false;
      }
    }

    void ShapeMoveToLayerItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = shapeTreeView.SelectedShapes;
      if (shapes.Count == 0)
        return;
      ShapeBase root = ((Layer)((ToolStripItem)sender).Tag).Root;
      if (!SetShapeParentAction.TestShapesLinkToDifferentZone(shapes, root, true))
        return;

      EditorManager.Actions.StartGroup("Move shapes to layer ");
      foreach (ShapeBase shape in shapes)
        EditorManager.Actions.Add(new SetShapeParentAction(shape, root));
      EditorManager.Actions.EndGroup();

      // todo: check if any shapes have actually moved...(doesn't happen if every shape you try to move is 'frozen')
      EditorManager.Scene.ActiveLayer = (Layer)(sender as ToolStripMenuItem).Tag;
    }

    private void setShapeActiveToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeBase shape = shapeTreeView.SelectedShape;
      EditorApp.Scene.ActiveLayer.ActiveShape = shape;
    }

    private void selectChildNodesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // set the new scene selection
      EditorManager.SelectedShapes = shapeTreeView.ChildShapes; 
    }

    private void selectRecursivelyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // set the new scene selection
      EditorManager.SelectedShapes = shapeTreeView.ChildShapesRecursively; 
    }

    private void cutShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnCut();
    }

    private void copyShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnCopy();
    }

    private void pasteShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnPaste();
    }

    private void deleteShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnDelete();
    }

    private void renameShapeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      shapeTreeView.RenameShape(shapeTreeView.SelectedShape);
    }


    private void pivotToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.Pivot, EnginePanel.CurrentDropToFloorAxis);
    }

    private void boundingBoxToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.BoundingBox, EnginePanel.CurrentDropToFloorAxis);
    }

    private void bottomCenterToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.BottomCenter, EnginePanel.CurrentDropToFloorAxis);
    }

    private void directionFloorToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item == null || !(item.Tag is Vector3F))
        return;
      EnginePanel.CurrentDropToFloorAxis = (Vector3F)item.Tag;
    }

    private void dropToFloorToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
    {
      directionPosXToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionPosXToolStripMenuItem.Tag;
      directionNegXToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionNegXToolStripMenuItem.Tag;
      directionPosYToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionPosYToolStripMenuItem.Tag;
      directionNegYToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionNegYToolStripMenuItem.Tag;
      directionCeilingToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionCeilingToolStripMenuItem.Tag;
      directionFloorToolStripMenuItem.Checked = EnginePanel.CurrentDropToFloorAxis == (Vector3F)directionFloorToolStripMenuItem.Tag;
    }

    private void jumpToToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //Ensure that all shapes are visible:
      BoundingBox mergedBox = EditorManager.SelectedShapes.BoundingBox;
      if (!mergedBox.Valid)
        return;

      // make it local again
      Vector3F center = mergedBox.Center;
      mergedBox.Translate(-center);

      EditorManager.ActiveView.LookAt(center, mergedBox);
    }

    private void alphabeticallyUpToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Ascending);
    }

    private void alphabeticallyDownToolStripMenuItem_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Descending);
    }

    private void createPrefabToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = shapeTreeView.SelectedShapes;
      PrefabDesc prefab = new PrefabDesc(null);

      // try to use/create the prefabs directory of the project
      string proj = EditorManager.Project.ProjectDir;
      string prefabDir = Path.Combine(proj, EditorManager.Settings.PrefabDirectory);
      if (!Directory.Exists(prefabDir))
      {
        try
        {
          Directory.CreateDirectory(prefabDir);
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
          prefabDir = proj; // OK, fallback to project directory
        }
      }

      // first select the prefab file to save
      CreateFileDlg fileDlg = new CreateFileDlg();
      fileDlg.Caption = "Save a prefab";
      fileDlg.Description = "Enter the name of the prefab file and select the directory to save it in. Then click OK to proceed.";
      fileDlg.InitialDirectory = prefabDir;
      fileDlg.Ext = ".prefab";
      fileDlg.Filter = new string[] { ".prefab" };
      fileDlg.AllowOverwrite = true;

      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      prefab.Filename = fileDlg.FileName;

      // if the file exists, load it to get the properties
      if (File.Exists(prefab.Filename))
      {
        prefab.Load();
      }

      // set this before the dialog opens
      prefab.SourceShapesForExposedParameters = shapes;

      // edit the prefab properties
      using (PropertyGridDlg propDlg = new PropertyGridDlg("Edit prefab properties", "Edit additional prefab properties. Click OK to proceed with saving the prefab file."))
      {
        propDlg.DataObjectNoClone = prefab;
        if (propDlg.ShowDialog() != DialogResult.OK)
          return;

        // fill with the shapes
        if (!prefab.CreateFromInstances(shapes, shapes.Center, true, true))
        {
          EditorManager.ShowMessageBox("Failed to create prefab. Detailed message:\n\n" + prefab.LastError, "Creation Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }

        if (!prefab.SaveToFile(fileDlg.FileName))
        {
          EditorManager.ShowMessageBox("Failed to save prefab. Detailed message:\n\n" + prefab.LastError, "Saving Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
          return;
        }
      }
    }

    private void lockShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = shapeTreeView.SelectedShapes;
      if (shapes == null)
        return;
      bool bUseUnlocking = shapes.EvaluateUseShapeUnLockingPreference();
      EditorManager.Actions.Add(new FreezeShapesAction(shapes, !bUseUnlocking));
    }



    #endregion

    #region Layer Context Menu

    private void contextMenuStrip_Layer_Opening(object sender, CancelEventArgs e)
    {
      treeView_Layers.UpdateSelectionStatus();

      bool bHasScene = EditorManager.Scene != null;
      bool bEditable = treeView_Layers.Selection_AllLayersEditable;
      bool bRemovable = treeView_Layers.Selection_AllLayersRemovable;
      deleteToolStripMenuItem.Enabled = bRemovable;
      renameToolStripMenuItem.Enabled = bEditable;

      if (treeView_Layers.Selection_Layers.Count == 1)
      {
        lockStatusToolStripMenuItem.Visible = true;
        lockStatusToolStripMenuItem.Text = "LockStatus: " + treeView_Layers.Selection_Layers[0].LockStatus;
      }
      else
      {
        lockStatusToolStripMenuItem.Visible = false;
      }
      
      lockLayerToolStripMenuItem.Enabled = treeView_Layers.Selection_AnyLayerNotLocked;
      updateLockStatusToolStripMenuItem.Enabled = !bEditable && !treeView_Layers.Selection_AnyLayerReference;
      unlockLayerToolStripMenuItem.Enabled = bEditable;

      unfreezeAllShapesToolStripMenuItem.Enabled = treeView_Layers.Selection_AnyLayerHasFrozenShapes;
      showLayerAssetsToolStripMenuItem.Enabled = treeView_Layers.Selection_Layers.Count > 0;

      string errorMsg = null;
      bool bAllowRename = true;
      bool bAnyNonMatching = false;
      bool bCanSortLayersIntoZones = treeView_Layers.Selection_LayersOnly;
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (!layer.Loaded || layer.ParentZone != null || layer.IsReference)
          bCanSortLayersIntoZones = false;
        if (layer.FileNameMatchesName())
          continue;
        bAnyNonMatching = true;
        if (!layer.SetLayerNameAsFilename(true, ref errorMsg))
        {
          bAllowRename = false;
          break;
        }
      }
      bAllowRename &= bAnyNonMatching;
      toolStripMenuItem_LastSeparator.Visible = bAllowRename;
      renameFileToLayerNameToolStripMenuItem.Visible = bAllowRename;
      sortShapesIntoZonesToolStripMenuItem.Enabled = bCanSortLayersIntoZones;
    }

    private void lockLayerToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        layer.TryLock(null, true);
      }
    }

    private void unlockLayerToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (!layer.Modifiable)
          continue;
        if (!layer.Dirty || layer.SaveToFile())
        {
          if (!layer.ReleaseLock(true, true))
            EditorManager.ShowMessageBox("The layer \"" + layer.LayerName + "\" cannot be unlocked since it has been locked by the current user and has already been edited. You will need to either submit or revert the changes with your revision control system in order to unlock it again.", "Layer cannot be unlocked", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
      }
    }

    private void updateLockStatusToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
        layer.UpdateLockStatus(true, true);
    }

    private void cutToolStripMenuItem_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCut();
    }

    private void copyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCopy();
    }

    private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnDelete();
    }

    private void renameToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Layer layer = EditorApp.Scene.ActiveLayer;
      treeView_Layers.BeginRenameLayer(layer);
    }

    private void unfreezeAllShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (Layer layer in treeView_Layers.Selection_Layers)
        EditorManager.Actions.Add(new FreezeShapesAction(layer.Root.ChildCollection, false));
    }

    private void renameFileToLayerNameToolStripMenuItem_Click(object sender, EventArgs e)
    {
      string errorMsg = null;
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (!layer.SetLayerNameAsFilename(false, ref errorMsg))
        {
          EditorManager.ShowMessageBox(string.Format("Failed to rename layer file for Layer '{0}'.\n\nDetailed message:\n{1}", layer.LayerName, errorMsg), "Failed to rename layer file", MessageBoxButtons.OK, MessageBoxIcon.Error);
          break;
        }
      }

    }

    private void sortShapesIntoZonesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ZoneCollection zones = new ZoneCollection();
      foreach (Zone zone in EditorManager.Scene.Zones)
        if (zone.ConsiderForSortIntoZones)
          zones.Add(zone);
      if (zones.Count==0)
        return;

      ShapeCollection shapes = new ShapeCollection();
      foreach (Layer layer in treeView_Layers.Selection_Layers)
      {
        if (layer.ParentZone != null || !layer.Modifiable)
          continue;
        foreach (ShapeBase shape in layer.Root.ChildCollection)
          shapes.Add(shape);

      }

      if (shapes.Count > 0)
      {
        SortShapesIntoZonesAction action = new SortShapesIntoZonesAction(shapes, zones);
        EditorManager.Actions.Add(action);
      }
    }


    #endregion

    #region Zone Context Menu

    private void contextMenuStrip_Zones_Opening(object sender, CancelEventArgs e)
    {
      bool bCanLoadUnload = treeView_Layers.Selection_ZonesOnly;
      bool bFirst = true;
      bool bLastStatus = false;

      foreach (Zone zone in treeView_Layers.Selection_Zones)
      {
        if (bFirst)
        {
          bLastStatus = zone.Loaded;
          bFirst = false;
        }
        if (bLastStatus != zone.Loaded)
        {
          bCanLoadUnload = false;
          break;
        }
        bLastStatus = zone.Loaded;
      }

      loadUnloadToolStripMenuItem.Enabled = bCanLoadUnload;
      string txt;
      if (bLastStatus)
        txt = "Unload zone";
      else
        txt = "Reload zone";
      if (treeView_Layers.Selection_Zones.Count > 1)
        txt += "s";

      loadUnloadToolStripMenuItem.Text = txt;

      relightToolStripMenuItem.Enabled = treeView_Layers.Selection_AllZonesLoaded;
      showZoneAssetsToolStripMenuItem.Enabled = treeView_Layers.Selection_Zones.Count > 0;
      moveShapesToolStripMenuItem.Enabled = treeView_Layers.Selection_Zones.Count > 0;
    }

    private void toolStripMenuItem_CutZone_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCut();
    }

    private void toolStripMenuItem_CopyZone_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnCopy();

    }


    
    private void toolStripMenuItem_DeleteZone_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnDelete();
    }

    private void toolStripMenuItem_RenameZone_Click(object sender, EventArgs e)
    {
      Zone zone = EditorApp.Scene.ActiveZone;
      treeView_Layers.BeginRenameZone(zone);
    }

    private void loadUnloadToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!treeView_Layers.Selection_ZonesOnly || treeView_Layers.Selection_Zones.Count < 1)
        return;

      bool bNewState = !treeView_Layers.Selection_Zones[0].Loaded;

      ZoneCollection zones = treeView_Layers.Selection_Zones.ShallowClone();
      if (zones.Count > 1)
        EditorManager.Actions.StartGroup(bNewState ? "Reloading zones" : "Unloading zones");

      foreach (Zone zone in zones)
        EditorManager.Actions.Add(new SetZoneLoadedStatusAction(zone,bNewState));

      if (zones.Count > 1)
        EditorManager.Actions.EndGroup();
    }

    private void relightToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (treeView_Layers.Selection_Zones.Count < 1)
        return;
      EditorApp.Scene.UpdateStaticLighting(false, treeView_Layers.Selection_Zones, false);
    }


    void ShowAssetDialog(ZoneCollection zones, LayerCollection layers)
    {
      ShowAssetListDlg dlg = new ShowAssetListDlg();

      ResourceSnapshotInfo info = new ResourceSnapshotInfo();
      ExportHelper.StartResourceSnapshot(info);

      string zoneNames = null;
      string layerNames = null;

      if (zones != null)
        foreach (Zone zone in zones)
        {
          zone.GetDependencies(info);
          if (zoneNames != null)
            zoneNames += ", ";
          zoneNames += zone.ZoneName;
        }

      if (layers != null)
        foreach (Layer layer in layers)
        {
          layer.GetDependencies(info);
          if (layerNames != null)
            layerNames += ", ";
          layerNames += layer.LayerName;
        }

      string desc = "This list shows the native assets used by :";
      if (zoneNames != null)
        desc += "\n  Zones : " + zoneNames;
      if (layerNames != null)
        desc += "\n  Layers : " + layerNames;
    
      dlg.Description = desc;
      dlg.AssetList = EditorManager.AssetManager.ExtractAssetInfo(info);
      ExportHelper.EndResourceSnapshot(info); // after extracting

      dlg.ShowDialog();
    }

    private void showZoneAssetsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ZoneCollection zones = treeView_Layers.Selection_Zones;
      if (zones == null || zones.Count == 0)
        return;
      ShowAssetDialog(zones, null);
    }

    private void showLayerAssetsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      LayerCollection layer = treeView_Layers.Selection_Layers;
      if (layer == null || layer.Count == 0)
        return;
      ShowAssetDialog(null, layer);

    }


    private void moveShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!treeView_Layers.Selection_ZonesOnly)
        return;
      ShapeCollection shapes = new ShapeCollection();
      foreach (Zone zone in treeView_Layers.Selection_Zones)
      {
        Shape3D shape = zone.GetZoneMoveProxyShape();
        if (shape != null)
          shapes.Add(shape);
      }
      EditorManager.SelectedShapes = shapes;
    }


    #endregion

    #region Layer Toolbar

    /// <summary>
    /// Updates the layer toolbar button states
    /// </summary>
    private void UpdateLayerToolbar()
    {
      bool bHasScene = (EditorManager.Scene != null);
      if (!bHasScene)
      {
        toolStripButton_AddLayer.Enabled = false;
        toolStripButton_AddZone.Enabled = false;
        toolStripButton_DeleteLayer.Enabled = false;
        toolStripButton_LayerMoveUp.Enabled = false;
        toolStripButton_LayerMoveDown.Enabled = false;
        toolStripButton_LockUpdate.Enabled = false;
        toolStripButton_FileRefresh.Enabled = false;
        toolStripButton_SortAZUp.Enabled = false;
        toolStripButton_SortAZDown.Enabled = false;
        toolStripButton_AddGroup.Enabled = false;
        toolStripButton_DeleteShape.Enabled = false;
        toolStripButton_SortShapesAZDown.Enabled = false;
        toolStripButton_SortShapesAZUp.Enabled = false;
        toolStripSplitButton_MoveToLayer.Enabled = false;
        toolStripButton_ShapeMoveDown.Enabled = false;
        toolStripButton_ShapeMoveUp.Enabled = false;
        toolStripSplitButton_ToolsOptions.Enabled = false;
        toolStripButton_ImportReference.Enabled = false;
        return;
      }

      Zone activeZone = EditorManager.Scene.ActiveZone;
      toolStripButton_AddLayer.Enabled = bHasScene;
      toolStripButton_AddZone.Enabled = bHasScene;
      toolStripButton_DeleteLayer.Enabled = treeView_Layers.Selection_AllLayersRemovable || treeView_Layers.Selection_AllZonesModifiable;
      toolStripButton_LayerMoveUp.Enabled = treeView_Layers.Selection_SingleLayer || treeView_Layers.Selection_SingleZone;
      toolStripButton_LayerMoveDown.Enabled = toolStripButton_LayerMoveUp.Enabled;
      toolStripButton_LockUpdate.Enabled = bHasScene;
      toolStripButton_FileRefresh.Enabled = bHasScene;
      toolStripButton_AddGroup.Enabled = bHasScene;
      toolStripButton_DeleteShape.Enabled = bHasScene;
      toolStripButton_SortShapesAZDown.Enabled = bHasScene;
      toolStripButton_SortShapesAZUp.Enabled = bHasScene;
      toolStripSplitButton_MoveToLayer.Enabled = bHasScene;
      toolStripButton_ShapeMoveDown.Enabled = bHasScene;
      toolStripButton_ShapeMoveUp.Enabled = bHasScene;
      toolStripSplitButton_ToolsOptions.Enabled = bHasScene;
      toolStripButton_ImportReference.Enabled = bHasScene;

      Zone parentZone = (treeView_Layers.Selection_Layers.Count > 0 && treeView_Layers.Selection_SingleLayer) ? treeView_Layers.Selection_Layers[0].ParentZone : null;
      int iLayerCount = (parentZone == activeZone) ? EditorManager.Scene.GetZoneLayerCount(activeZone) : EditorManager.Scene.Layers.Count;
      int iZoneCount = EditorManager.Scene.Zones.Count;
      toolStripButton_SortAZUp.Enabled = (treeView_Layers.Selection_SingleLayer && iLayerCount > 1) || (treeView_Layers.Selection_SingleZone && iZoneCount > 1);
      toolStripButton_SortAZDown.Enabled = toolStripButton_SortAZUp.Enabled;
    }

    private void toolStripButton_AddLayer_Click(object sender, EventArgs e)
    {
      Layer layer = Layer.CreateLayer(treeView_Layers.Selection_Layers);
    }

    private void toolStripButton_AddZone_Click(object sender, EventArgs e)
    {
      Zone zone = Zone.CreateZone();
      treeView_Layers.SelectedNode = null; //clear selection
    }

    private void toolStripButton_DeleteLayer_Click(object sender, EventArgs e)
    {
      treeView_Layers.OnDelete();
    }

    private void toolStripButton_LayerMoveDown_Click(object sender, EventArgs e)
    {
      IScene scene = EditorManager.Scene;
      if (scene == null)
        return;

      if (treeView_Layers.Selection_SingleLayer)
      {
        Layer layer = treeView_Layers.Selection_Layers[0];
        EditorManager.Actions.Add(new MoveLayerAction(layer, MoveLayerAction.MoveDir.Down));
      }
      if (treeView_Layers.Selection_SingleZone)
      {
        Zone zone = treeView_Layers.Selection_Zones[0];
        EditorManager.Actions.Add(new MoveZoneAction(zone, 1));
      }
    }


    private void toolStripButton_LayerMoveUp_Click(object sender, EventArgs e)
    {
      IScene scene = EditorManager.Scene;
      if (scene==null)
        return;

      if (treeView_Layers.Selection_SingleLayer)
      {
        Layer layer = treeView_Layers.Selection_Layers[0];
        EditorManager.Actions.Add(new MoveLayerAction(layer, MoveLayerAction.MoveDir.Up));
      }
      if (treeView_Layers.Selection_SingleZone)
      {
        Zone zone = treeView_Layers.Selection_Zones[0];
        EditorManager.Actions.Add(new MoveZoneAction(zone, -1));
      }
    }

    private LayerCollection GetSortingRelevantLayers()
    {
      if (EditorManager.Scene == null)
        return null;
      Zone zone = null;

      // Get layers from parent zone or from scene
      if (treeView_Layers.Selection_SingleLayer)
        zone = treeView_Layers.Selection_Layers[0].ParentZone;
      LayerCollection layers = zone != null ? zone.Layers : EditorManager.Scene.Layers;
      return layers;
    }

    private void toolStripButton_SortAZUp_Click(object sender, EventArgs e)
    {
      if (treeView_Layers.Selection_SingleLayer)
      {
        LayerCollection layers = GetSortingRelevantLayers();
        ChangeLayerOrderAction.SortAlphabetically(layers, SortOrder.Ascending);
      }
      else if (treeView_Layers.Selection_SingleZone)
      {
        ZoneCollection zones = EditorManager.Scene.Zones;
        ChangeZoneOrderAction.SortAlphabetically(zones, SortOrder.Ascending);
      }
    }

    private void toolStripButton_SortAZDown_Click(object sender, EventArgs e)
    {
      if (treeView_Layers.Selection_SingleLayer)
      {
        LayerCollection layers = GetSortingRelevantLayers();
        ChangeLayerOrderAction.SortAlphabetically(layers, SortOrder.Descending);
      }
      else if (treeView_Layers.Selection_SingleZone)
      {
        ZoneCollection zones = EditorManager.Scene.Zones;
        ChangeZoneOrderAction.SortAlphabetically(zones, SortOrder.Descending);
      }
    }

    private void toolStripButton_LockUpdate_Click(object sender, EventArgs e)
    {
      Cursor.Current = Cursors.WaitCursor;
      EditorManager.Scene.UpdateLayerLocks();
      Cursor.Current = Cursors.Default;
    }

    private void toolStripButton_FileRefresh_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene != null)
        EditorApp.Scene.UpdateLayers(); // create engine instances
    }

    private void toolStripButton_ImportReference_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;

      OpenFileDlg dlg = new OpenFileDlg("Import Layer from external scene", "Import a .layer file from another scene in the same project", null, true, new string[1] { ".layer" });
      if (dlg.ShowDialog() != DialogResult.OK)
        return;
      string[] absNames = dlg.FileNames;
      StringCollection nameList = new StringCollection();
      foreach (string name in absNames)
        nameList.Add(name);
      StringCollection errorList = new StringCollection();
      EditorApp.Scene.ImportLayerReferences(nameList, errorList);

      if (errorList.Count > 0)
      {
        string error = "Error while importing the following layers as a refernces:\n\n";
        foreach (string s in errorList)
          error += "  - " + s + "\n";
        EditorManager.ShowMessageBox(error, "Error loading references", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }


    }

    bool _layerInfoPanelVisible = true;

    /// <summary>
    /// gets or sets thre status of the update panel. This panel contains the the "Update" and "Ignore"
    /// button
    /// </summary>
    public bool UpdatedLayerInfoPanelVisible
    {
      get {return _layerInfoPanelVisible;}
      set
      {
        if (_layerInfoPanelVisible==value)
          return;
        _layerInfoPanelVisible = value;
        panel_UpdateLayer.Visible = value;
      }
    }

    private void button_LayerUpdate_Click(object sender, System.EventArgs e)
    {
      EditorApp.Scene.UpdateLayers();
      UpdatedLayerInfoPanelVisible = false;
    }

    private void button_UpdateIgnore_Click(object sender, System.EventArgs e)
    {
      UpdatedLayerInfoPanelVisible = false;
    }

    #endregion

    #region Event Listeners
    
    /// <summary>
    /// Listener for the OnSceneChanged event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      UpdateLayerToolbar();
      UpdatedLayerInfoPanelVisible = false;
    }

    /// <summary>
    /// Listens for the OnLayerChanged event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      if (EditorManager.GUI.UIUpdateLock > 0)
        return;

      UpdateLayerToolbar();

      bool needInvoke = this.InvokeRequired;

      if (e.action == LayerChangedArgs.Action.ContentChanged)
        UpdatedLayerInfoPanelVisible = true;
    }

    #endregion

    private void toolStripButton_DeleteShape_Click(object sender, EventArgs e)
    {
      shapeTreeView.OnDelete();
    }

    private void toolStripButton_AddGroup_Click(object sender, EventArgs e)
    {
      Layer layer = EditorManager.Scene.ActiveLayer;
      if (layer != null)
      {
        Group3DShape group = new Group3DShape("Group");
        EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(group, layer.ActiveShape, layer, true));
      } 
    }

    private void toolStripButton_SortShapesAZUp_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Ascending);
    }

    private void toolStripButton_SortShapesAZDown_Click(object sender, EventArgs e)
    {
      SortShapes(SortOrder.Descending);
    }

    private void toolStripSplitButton_MoveToLayer_ButtonClick(object sender, EventArgs e)
    {
      toolStripSplitButton_MoveToLayer.ShowDropDown();
    }

    private void toolStripSplitButton_MoveToLayer_DropDownOpening(object sender, EventArgs e)
    {
      bool bHasScene = EditorManager.Scene != null;
      ShapeCollection shapes = shapeTreeView.SelectedShapes;

      toolStripSplitButton_MoveToLayer.DropDownItems.Clear();
      // add all layers to the dropdownlist
      if (bHasScene && shapes.Count > 0)
      {
        foreach (Layer layer in EditorManager.Scene.Layers)
          if (layer.Modifiable && layer.Loaded && layer != EditorManager.Scene.ActiveLayer)
          {
            ToolStripItem item = toolStripSplitButton_MoveToLayer.DropDownItems.Add(layer.LayerName);
            item.Tag = layer;
            item.Click += new EventHandler(ShapeMoveToLayerItem_Click);
          }
      }
    }

    private void toolStripButton_ShapeMoveDown_Click(object sender, EventArgs e)
    {
      SortSelectedShapes(MoveShapesDirection.Down);
    }

    private void toolStripButton_ShapeMoveUp_Click(object sender, EventArgs e)
    {
      SortSelectedShapes(MoveShapesDirection.Up);
    }

    private void toolStripSplitButton_ToolsOptions_DropDownOpening(object sender, EventArgs e)
    {
      toolStripSplitButton_AutomaticFocus.Checked = EditorManager.Settings.FocusOnSelection;
      toolStripSplitButton_PropertiesOnSelection.Checked = EditorManager.Settings.ShowPropertiesPanelOnSelection;
    }

    private void toolStripSplitButton_AutomaticFocus_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.FocusOnSelection = toolStripSplitButton_AutomaticFocus.Checked;
    }

    private void toolStripSplitButton_PropertiesOnSelection_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.ShowPropertiesPanelOnSelection = toolStripSplitButton_PropertiesOnSelection.Checked;
    }

    private void SortSelectedShapes(MoveShapesDirection moveDirection)
    {
      ShapeCollection selected = shapeTreeView.SelectedShapes;

      // return if any of the indices is already on the maximum.
      foreach (ShapeBase shape in selected)
      {
        if (moveDirection == MoveShapesDirection.Up)
        {
          if (shape.Parent.ChildCollection.FindIndex(i => i == shape) <= 0)
            return;
        }
        else
          if (shape.Parent.ChildCollection.FindIndex(i => i == shape) >= shape.Parent.ChildCollection.Count - 1)
            return;
      }

      // get all parents to share modified collections between their children.
      ShapeCollection parents = new ShapeCollection();
      foreach (ShapeBase shape in selected)
        if (!parents.Contains(shape.Parent))
          parents.Add(shape.Parent);

      EditorManager.Actions.StartGroup("Sort Shapes");
      foreach (ShapeBase parent in parents)
      {
        // create copy of the original collection before sorting
        ShapeCollection copyOfChildren = new ShapeCollection();
        copyOfChildren.AddRange(parent.ChildCollection);

        if (moveDirection == MoveShapesDirection.Up)
        {
          for (int i = 0; i < selected.Count; i++)
          {
            ShapeBase child = selected[i];
            if (child.Parent == parent)
            {
              int index = copyOfChildren.FindIndex(c => c == child);
              copyOfChildren.Remove(child);
              copyOfChildren.Insert(index - 1, child);
              EditorManager.Actions.Add(new SortShapeChildrenAction(parent, copyOfChildren));
            }
          }
        }
        else
          for (int i = selected.Count - 1; i > -1; i--)
          {
            ShapeBase child = selected[i];
            if (child.Parent == parent)
            {
              int index = copyOfChildren.FindIndex(c => c == child);
              copyOfChildren.Remove(child);
              copyOfChildren.Insert(index + 1, child);
              EditorManager.Actions.Add(new SortShapeChildrenAction(parent, copyOfChildren));
            }
          }
      }
      EditorManager.Actions.EndGroup();

      // recover selection
      ArrayList newSelection = new ArrayList();
      foreach (ShapeTreeNode node in shapeTreeView.Nodes)
      {
        if (selected.Contains(node.shape)) // root
          newSelection.Add(node);
        foreach (ShapeTreeNode subNode in shapeTreeView.GetChildNodes(node))
        {
          if (selected.Contains(subNode.shape)) // all children
            newSelection.Add(subNode);
        }
      }
      shapeTreeView.SelectedNodes = newSelection;
    }

    private enum MoveShapesDirection
    {
      Up,
      Down
    }
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
