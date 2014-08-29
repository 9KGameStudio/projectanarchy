/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using CSharpFramework;
using ManagedFramework;
using Editor.Actions;
using CSharpFramework.Math;
using CSharpFramework.Actions;
using CSharpFramework.Shapes;
using CSharpFramework.Clipboard;
using CSharpFramework.Contexts;
using CSharpFramework.Dialogs;
using CSharpFramework.Docking;
using CSharpFramework.ShortCuts;
using ManagedBase;
using Editor.Contexts;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Controls;
using System.Diagnostics;
using System.Collections.Generic;


namespace Editor
{
  /// <summary>
  /// Panel for engine view.
  /// </summary>
  public partial class EnginePanel : DockableForm
  {
    /// <summary>
    /// get the view
    /// </summary>
    [BrowsableAttribute(false)]
    public VisionView View { get { return visionView; } }

    #region Constructor

    /// <summary>
    /// Constructor
    /// </summary>
    public EnginePanel(DockingContainer container) : base(container)
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();
      _helpbutton = new ToolStripHelpButton(Text);
      toolStrip_EnginePanel.Items.Add(_helpbutton);

      // I put this into a separate function because VS messed it up too often!
      this.visionView = new ManagedFramework.VisionView();
      GizmoBase.GizmoChanged += new ShapeDragModeEventHandler(View.OnGizmoChanged);

      this.visionView.BackColor = System.Drawing.Color.DarkGray;
      this.visionView.CurrentContext = null;
      this.visionView.Dirty = true;
      this.visionView.Location = new System.Drawing.Point(0, 0);
      this.visionView.Name = "visionView";
      this.visionView.Size = new System.Drawing.Size(640, 480);
      this.visionView.TabIndex = 9;
      this.visionView.Dock = DockStyle.Fill;
      this.visionView.AllowDrop = true; // shape creators

      DockHandler.AllowRedocking = true;

      this.panel_VisionView.Controls.Add(this.visionView);

      // register vision view listeners
      EditorManager.SceneChanged += new SceneChangedEventHandler(visionView.OnSceneChanged);
      EditorScene.ShapeChanged += new ShapeChangedEventHandler(visionView.OnShapeChanged);
      EditorScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(visionView.OnPropertyChanged);
      EditorScene.LayerChanged += new LayerChangedEventHandler(EditorScene_LayerChanged);
      EditorManager.EditorModeChanged += new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      VisionViewBase.MouseContextChanged += new ContextChangedEventHandler(this.OnMouseContextChanged);
      GizmoBase.GizmoChanged += new ShapeDragModeEventHandler(this.OnGizmoChanged);
      EditorManager.SceneChanged += new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.SceneEvent += new SceneEventHandler(enginePanel_SceneEvent);
      ViewIconSettings.OnViewIconSettingsChanged += new EventHandler(ViewIconSettings_OnViewIconSettingsChanged);
      EditorApp.ActiveView = visionView;

      // register gizmo listeners
      EditorScene.ShapeChanged += new ShapeChangedEventHandler(visionView.Gizmo.OnShapeChanged);
      EditorScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(visionView.Gizmo.OnPropertyChanged);
      EditorScene.PropertyChanged += new CSharpFramework.PropertyChangedEventHandler(this.OnPropertyChanged);
      EditorScene.LayerChanged += new CSharpFramework.LayerChangedEventHandler(visionView.Gizmo.OnLayerChanged);
      EditorManager.SceneChanged += new CSharpFramework.SceneChangedEventHandler(visionView.Gizmo.OnSceneChanged);
      EditorManager.ShapeSelectionChanged += new ShapeSelectionChangedEventHandler(visionView.Gizmo.OnSelectionChanged);
      EditorManager.ShapeSelectionChanged += new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);
      //EditorScene.V3DChanged += new V3DChangedEventHandler(visionView.Gizmo.OnV3DChanged);
      VisionViewBase.ProjectionModeChanged += new EventHandler(VisionViewBase_ProjectionModeChanged);
      // set the default move context
      visionView.SetDefaultContext();
      visionView.Gizmo.DragMode = ShapeDragMode.MOVE;

      // setup the context menus
      // the item's implementation is in this class because the View doesn't know about actions
      visionView.ContextMenu_SelShapes = contextMenuStrip_SelShapes;
      visionView.ContextMenu_View = contextMenuStrip_View;

      // register some shortcuts
      ShortCutConfig shortcuts = EditorManager.ShortCuts;
      shortcuts.Add(new MenuItemShortCut(pasteAtCursorToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(showGridToolStripMenuItem, Keys.G));
      shortcuts.Add(new MenuItemShortCut(toolStripButton_EnableMoveSnap, Keys.None));
      shortcuts.Add(new MenuItemShortCut(toolStripButton_EnableAngleSnap, Keys.None));
      shortcuts.Add(new MenuItemShortCut(solidToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(wireframeToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(sceneOriginToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(cameraPositionToolStripMenuItem1, Keys.None));
      shortcuts.Add(new MenuItemShortCut(traceHitToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_View_Properties, Keys.None));
      shortcuts.Add(new MenuItemShortCut(cameraPositionToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(shapeOriginToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(pivotToolStripMenuItem, Keys.None)); // drop to floor
      shortcuts.Add(new MenuItemShortCut(boundingBoxToolStripMenuItem, Keys.None)); // drop to floor
      shortcuts.Add(new MenuItemShortCut(bottomCenterToolStripMenuItem, Keys.None)); // drop to floor
      shortcuts.Add(new MenuItemShortCut(selectInTreeViewToolStripMenuItem, Keys.Control | Keys.T));
      shortcuts.Add(new MenuItemShortCut(Profiling_Next, Keys.None));
      shortcuts.Add(new MenuItemShortCut(Profiling_Previous, Keys.None));

      shortcuts.Add(new MenuItemShortCut(directionPosXToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(directionNegXToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(directionPosYToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(directionNegYToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(directionCeilingToolStripMenuItem, Keys.None));
      shortcuts.Add(new MenuItemShortCut(directionFloorToolStripMenuItem, Keys.None));

      // Register some tool strip shortcuts
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_Perspective, Keys.Alt | Keys.Control | Keys.P));
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_PerspectiveTop, Keys.Control | Keys.Shift | Keys.T));
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_PerspectiveFront, Keys.Control | Keys.Shift | Keys.F));
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_PerspectiveRight, Keys.Control | Keys.Shift | Keys.R));

      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_Top, Keys.Alt | Keys.Control | Keys.T));
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_Front, Keys.Alt | Keys.Control | Keys.F));
      shortcuts.Add(new MenuItemShortCut(ToolStripMenuItem_Right, Keys.Alt | Keys.Control | Keys.R));
      shortcuts.Add(new ToolStripSplitButtonShortCut(ToolStripButton_ZoomFit, Keys.F));
      shortcuts.Add(new ToolStripItemShortCut(ToolStripButton_IsolateSelection, Keys.I));

      shortcuts.Add(new ToolStripSplitButtonShortCut(ToolStripSplitButton_MoveSensitivity, Keys.Alt | Keys.Control | Keys.U));

      // Alignment shortcuts
      shortcuts.Add(new MenuItemShortCut(AlignToGridToolStripMenuItem, Keys.Alt | Keys.Control | Keys.G));
      shortcuts.Add(new MenuItemShortCut(AlignToObjectToolStripMenuItem, Keys.Alt | Keys.Control | Keys.O));
      shortcuts.Add(new MenuItemShortCut(AlignToHitPointToolStripMenuItem, Keys.Alt | Keys.Control | Keys.H));
      shortcuts.Add(new MenuItemShortCut(AlignToHitPointNormalToolStripMenuItem, Keys.Alt | Keys.Control | Keys.N));

      // associate drop-to-floor directions
      directionPosXToolStripMenuItem.Tag = Vector3F.XAxis;
      directionNegXToolStripMenuItem.Tag = -Vector3F.XAxis;
      directionPosYToolStripMenuItem.Tag = Vector3F.YAxis;
      directionNegYToolStripMenuItem.Tag = -Vector3F.YAxis;
      directionCeilingToolStripMenuItem.Tag = Vector3F.ZAxis;
      directionFloorToolStripMenuItem.Tag = -Vector3F.ZAxis;

      // get the drop down and cache for changing playmode
      _play_Dropdown = toolStripDropDownButton_PlayMode.DropDown;

      this.toolStripButton_MoveCameraWithKeys.Checked = EditorManager.Settings.MoveCameraWithKeys; //use settings from EditorSettingsBase

      if (!DesignMode)
      {
        this.SetMoveSensitivity(EditorManager.Settings.MoveSensitivity);
      }
      UpdateToolbarStatus();
    }

    /// Clean up any resources being used.
    /// </summary>
    

    #endregion

    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    
    #endregion

    #region Selected Shapes Context Menu


    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="mode"></param>
    private void DoDropToFloor(Shape3D.DropToFloorMode mode, Vector3F axis)
    {
      Cursor = Cursors.WaitCursor;
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      EditorManager.Actions.Add(new DropToFloorAction(shapes, mode, axis, true));
      EditorManager.ActiveView.UpdateView(false);
      Cursor = Cursors.Default;
    }

    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="targetPos"></param>
    static public void MoveSelectionToPosition(Vector3F targetPos)
    {
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      if (shapes == null || shapes.Count == 0)
        return;

      Vector3F deltaPos = targetPos - EditorApp.ActiveView.Gizmo.Position;
      if (shapes.Count > 1)
        EditorManager.Actions.StartGroup("Move Selection");

      foreach (Shape3D shape in shapes)
        EditorManager.Actions.Add(new MoveShapeAction(shape, shape.Position, shape.Position + deltaPos));

      if (shapes.Count > 1)
        EditorManager.Actions.EndGroup();
      EditorApp.ActiveView.Gizmo.RecalcGizmoPos();
    }


    private void contextMenuStrip_SelShapes_Opening(object sender, CancelEventArgs e)
    {
      GizmoBase gizmo = EditorApp.ActiveView.Gizmo;
      AlignToGridToolStripMenuItem.Enabled = /*gizmo.MoveGrid.Enabled &&*/ (gizmo.Shapes.Count > 0);

      Layer commonLayer = null;
      ShapeCollection shapes = gizmo.Shapes;
      foreach (ShapeBase shape in shapes)
      {
        if (commonLayer == null)
          commonLayer = shape.ParentLayer;
        else if (shape.ParentLayer != commonLayer)
        {
          commonLayer = null;
          break;
        }
      }

      bool bAnyMovable = false;
      bool bAnyRotatable = false;
      foreach (ShapeBase shape in shapes)
      {
        if (!(shape is Shape3D) || !shape.Modifiable || shape.IsHintSet(ShapeBase.HintFlags_e.NoUserTransform))
          continue;
        bAnyMovable = true;
        if (!shape.IsHintSet(ShapeBase.HintFlags_e.NoRotation))
          bAnyRotatable = true;
      }

      // the following items depend on the shape to be movable (discarded through action later nayway):
      AlignmentToolStripMenuItem.Visible = bAnyMovable;
      AlignToObjectToolStripMenuItem.Visible = bAnyMovable;
      dropToFloorToolStripMenuItem.Visible = bAnyMovable;
      moveToToolStripMenuItem.Visible = bAnyMovable;
      AlignToHitPointNormalToolStripMenuItem.Enabled = bAnyRotatable;

      lockShapesToolStripMenuItem.Enabled = shapes.Count > 0;
      string text = shapes.Count == 1 ? "shape" : "shapes";
      if (shapes.EvaluateUseShapeUnLockingPreference())
        lockShapesToolStripMenuItem.Text = "Un-freeze " + text;
      else
        lockShapesToolStripMenuItem.Text = "Freeze " + text;

      this.selectInTreeViewToolStripMenuItem.Enabled = commonLayer != null;
    }

    private void AlignToGridToolStripMenuItem_Click(object sender, EventArgs e)
    {
      GizmoBase gizmo = EditorApp.ActiveView.Gizmo;

      if (gizmo.Shapes.Count < 1)
        return;

      // single shape move action
      if (gizmo.Shapes.Count == 1)
      {
        Shape3D shape = (Shape3D)gizmo.Shapes[0];
        EditorManager.Actions.Add(new MoveShapeAction(shape, shape.Position, gizmo.MoveGrid.GetAlignedPos(shape.Position, true)));
        return;
      }

      //group action
      Vector3F[] oldPos = gizmo.Shapes.StoreShapePositionArray();
      Vector3F[] newPos = gizmo.Shapes.StoreShapePositionArray();
      for (int i = 0; i < gizmo.Shapes.Count; i++)
        newPos[i] = gizmo.MoveGrid.GetAlignedPos(oldPos[i], true);

      EditorManager.Actions.Add(new MoveShapesAction(gizmo.Shapes, oldPos, newPos));
    }

    private void AlignToObjectToolStripMenuItem_Click(object sender, EventArgs e)
    {
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;
      if (context == null)
        return;

      context.AlignMode = CameraMoveContext.AlignMode_e.AlignToObject;
    }

    private void AlignToHitPointToolStripMenuItem_Click(object sender, EventArgs e)
    {
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;
      if (context == null)
        return;

      context.AlignMode = CameraMoveContext.AlignMode_e.AlignToHitPoint;
    }

    private void AlignToHitPointNormalToolStripMenuItem_Click(object sender, EventArgs e)
    {
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;
      if (context == null)
        return;

      context.AlignMode = CameraMoveContext.AlignMode_e.AlignToHitPointNormal;
    }

    /// <summary>
    /// Defines the current global direction for drop-to-floor
    /// </summary>
    public static Vector3F CurrentDropToFloorAxis = -Vector3F.ZAxis;

    private void pivotToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.Pivot, CurrentDropToFloorAxis);
    }

    private void boundingBoxToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.BoundingBox, CurrentDropToFloorAxis);
    }

    private void bottomCenterToolStripMenuItem_Click(object sender, EventArgs e)
    {
      DoDropToFloor(Shape3D.DropToFloorMode.BottomCenter, CurrentDropToFloorAxis);
    }


    private void directionFloorToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item == null || !(item.Tag is Vector3F))
        return;
      CurrentDropToFloorAxis = (Vector3F)item.Tag;
    }

    private void dropToFloorToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
    {
      directionPosXToolStripMenuItem.Checked = CurrentDropToFloorAxis == (Vector3F)directionPosXToolStripMenuItem.Tag;
      directionNegXToolStripMenuItem.Checked = CurrentDropToFloorAxis == (Vector3F)directionNegXToolStripMenuItem.Tag;
      directionPosYToolStripMenuItem.Checked = CurrentDropToFloorAxis == (Vector3F)directionPosYToolStripMenuItem.Tag;
      directionNegYToolStripMenuItem.Checked = CurrentDropToFloorAxis == (Vector3F)directionNegYToolStripMenuItem.Tag;
      directionCeilingToolStripMenuItem.Checked = CurrentDropToFloorAxis == (Vector3F)directionCeilingToolStripMenuItem.Tag;
      directionFloorToolStripMenuItem.Checked = CurrentDropToFloorAxis == (Vector3F)directionFloorToolStripMenuItem.Tag;

    }

    private void unlinkFromSourcesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      foreach (ShapeBase shape in shapes)
        EditorManager.Actions.Add(new UnlinkShapeSourcesAction(shape));
      EditorApp.ActiveView.UpdateView(false);
    }

    private void unlinkAllTargetsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      foreach (ShapeBase shape in shapes)
        EditorManager.Actions.Add(new UnlinkShapeTargetsAction(shape));
      EditorApp.ActiveView.UpdateView(false);
    }

    private void cameraPositionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      MoveSelectionToPosition(EditorManager.ActiveView.CameraPosition);
    }

    private void shapeOriginToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene != null)
        MoveSelectionToPosition(EditorManager.Scene.CurrentShapeSpawnPosition);
    }

    private void cutToolStripMenuItem_Click(object sender, EventArgs e)
    {
      visionView.OnCut();
    }

    private void copyToolStripMenuItem_Click(object sender, EventArgs e)
    {
      visionView.OnCopy();
    }

    private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
    {
      visionView.OnDelete();
    }

    private void selectInTreeViewToolStripMenuItem_Click(object sender, EventArgs e)
    {
      GizmoBase gizmo = EditorApp.ActiveView.Gizmo;
      if (gizmo.Shapes.Count < 1)
        return;

      ShapeCollection oldColl = gizmo.Shapes.ShallowCopy();
      gizmo.Clear();

      Layer layer = oldColl[0].ParentLayer;
      EditorManager.Scene.ActiveLayer = layer;

      gizmo.AddShapes(oldColl, false);
    }

    private void lockShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ShapeCollection shapes = EditorApp.ActiveView.Gizmo.Shapes;
      bool bUseUnlocking = shapes.EvaluateUseShapeUnLockingPreference();
      EditorManager.Actions.Add(new FreezeShapesAction(shapes, !bUseUnlocking));
    }

    #endregion

    #region View Context Menu

    #region Nested Class : DebugFlagMenuItem

    class DebugFlagMenuItem : ToolStripMenuItem
    {
      const DebugRenderFlags_e NeedsVisibilityInfoFlags =
        (DebugRenderFlags_e.VisibilityZones | DebugRenderFlags_e.Portals);

      public DebugFlagMenuItem(DebugRenderFlags_e flag, ToolStripDropDownItem owner)
      {
        _flag = flag;
        _owner = owner;
        _needsVisibility = (flag & NeedsVisibilityInfoFlags) != 0;

        this.Text = flag.ToString();
        this.ImageScaling = ToolStripItemImageScaling.None;
        this.Click += new EventHandler(DebugFlagMenuItem_Click);
        if (_needsVisibility)
          IVisibilityBuilder.OnVisibilityUseInEngineChanged += new EventHandler(IVisibilityBuilder_OnVisibilityUseInEngineChanged);
      }

      private void DebugFlagMenuItem_Click(object sender, EventArgs e)
      {
        DebugRenderFlags_e result = EditorManager.EngineManager.DebugRenderFlags ^ _flag;

        if (_needsVisibility)
        {
          // If we're about to enable displaying visibility information, 
          // it must be enabled in the engine.
          if (!this.Checked && !EditorManager.VisibilityBuilder.UseInEngine)
          {
            if (EditorManager.ShowMessageBox(EditorManager.MainForm,
              "\"Use Visibility\" must be enabled in order to display visibility information.\n" +
              "Do you want to enable it now?", "Use Visibility?", MessageBoxButtons.YesNo,
              MessageBoxIcon.Question, DialogResult.Yes) == DialogResult.No)
            {
              return; // Do nothing
            }
            else
            {
              // Enable "Use Visibility"
              EditorManager.VisibilityBuilder.UseInEngine = true;
            }
          }
          // If we're about to disable displaying visibility information, 
          // ask if it should be disabled in the engine too.
          // (Only if this is the last debug flag using visibility info)
          else if (this.Checked && (result & NeedsVisibilityInfoFlags) == 0 &&
            EditorManager.VisibilityBuilder.UseInEngine)
          {
            if (EditorManager.ShowMessageBox(EditorManager.MainForm,
              "You're about to disable displaying visibility information.\n" +
              "Do you want to disable \"Use Visibility\" too?", "Use Visibility?", MessageBoxButtons.YesNo,
              MessageBoxIcon.Question, DialogResult.Yes) == DialogResult.Yes)
            {
              EditorManager.VisibilityBuilder.UseInEngine = false;
            }
          }
        }

        EditorManager.EngineManager.DebugRenderFlags = result;
        EvaluateCheckedStatus();

        // If owner debug flag toolbar show drop down
        if (_owner.GetType() == typeof(ToolStripSplitButton))
        {
          EditorManager.ActiveView.UpdateView(true);
          _owner.ShowDropDown();
        }
      }

      public void EvaluateCheckedStatus()
      {
        this.Checked = (EditorManager.EngineManager.DebugRenderFlags & _flag) != 0;
      }

      public void IVisibilityBuilder_OnVisibilityUseInEngineChanged(object sender, EventArgs e)
      {
        if (!this.Checked || EditorManager.VisibilityBuilder.UseInEngine || !_needsVisibility)
          return;

        // disable displaying visibility information if there is none
        EditorManager.EngineManager.DebugRenderFlags &= ~_flag;
        EvaluateCheckedStatus();
      }

      DebugRenderFlags_e _flag = 0;
      private ToolStripDropDownItem _owner;
      private bool _needsVisibility;
    }

    class DebugFlagResetMenuItem : ToolStripMenuItem
    {
      public DebugFlagResetMenuItem(ToolStripDropDownItem owner)
      {
        _owner = owner;
        this.Text = "Disable All";
        this.Image = Properties.Resources.delete2;
        this.ImageScaling = ToolStripItemImageScaling.None;
        this.Click += new EventHandler(DebugFlagResetMenuItem_Click);
      }

      void DebugFlagResetMenuItem_Click(object sender, EventArgs e)
      {
        EditorManager.EngineManager.DebugRenderFlags = 0;
        DebugFlags = 0;

        // re-evaluate all checked states
        ToolStripItemCollection debugItems = _owner.DropDownItems;
        foreach (ToolStripItem item in debugItems)
        {
          if (item is DebugFlagMenuItem)
            (item as DebugFlagMenuItem).EvaluateCheckedStatus();
        }

        // If owner debug flag toolbar show drop down
        if (_owner.GetType() == typeof(ToolStripSplitButton))
        {
          EditorManager.ActiveView.UpdateView(true);
          _owner.ShowDropDown();
        }
      }

      public DebugRenderFlags_e DebugFlags = 0;
      private ToolStripDropDownItem _owner;
    }

    #endregion


    /// <summary>
    /// Open the view context; prepare some items
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void contextMenuStrip_View_Opening(object sender, CancelEventArgs e)
    {
      bool bHasScene = EditorManager.Scene != null;

      ContextMenuStrip menu = (ContextMenuStrip)sender;

      // grid
      showGridToolStripMenuItem.Checked = GizmoBase.GridSettings.GridMode != GridSettings.GridRenderMode.Disabled;

      // render mode
      solidToolStripMenuItem.Checked = visionView.EngineManager.RenderMode == RenderMode_e.Solid;
      wireframeToolStripMenuItem.Checked = visionView.EngineManager.RenderMode == RenderMode_e.Wireframe;

      // for some reason this has to be done, otherwise it does not pop up the second time
      EditorManager.BuildCreateMenu(createToolStripMenuItem);

      // Debug render flags
      ToolStripItemCollection debugItems = debugFlagsToolStripMenuItem.DropDownItems;
      if (debugItems.Count <= 1) // first time, generate the items
      {
        debugItems.Clear();

        // add new entry to clear all debug flags
        debugItems.Add(new DebugFlagResetMenuItem(debugFlagsToolStripMenuItem));
        debugItems.Add(new ToolStripSeparator());

        System.Array flagvalues = Enum.GetValues(typeof(DebugRenderFlags_e));
        foreach (DebugRenderFlags_e flag in flagvalues)
          debugItems.Add(new DebugFlagMenuItem(flag, debugFlagsToolStripMenuItem));
      }

      foreach (ToolStripItem item in debugItems)
      {
        if (item is DebugFlagMenuItem)
          (item as DebugFlagMenuItem).EvaluateCheckedStatus();
      }

      // paste at cursor
      if (bHasScene && visionView.Clipboard.HasData())
        pasteAtCursorToolStripMenuItem.Enabled = (visionView.Clipboard.DataObject is Shape3D) || (visionView.Clipboard.DataObject is ShapeCollection);
      else
        pasteAtCursorToolStripMenuItem.Enabled = false;

      // view effects
      shadingModes_ensureToolStripItems(shadingToolStripMenuItem);
      shadingModes_evaluateCheckMark(shadingToolStripMenuItem.DropDownItems);


      Pick_UV0_ToolStripMenuItem.Enabled = bHasScene;
      Pick_UV1_ToolStripMenuItem.Enabled = bHasScene;
      Pick_UV0_ToolStripMenuItem.Checked = EditorManager.ActiveView.CurrentContext is UV0PickerContext;
      Pick_UV1_ToolStripMenuItem.Checked = EditorManager.ActiveView.CurrentContext is UV1PickerContext;

      // zones
      zonesToolStripMenuItem.Visible = bHasScene && EditorManager.Scene.Zones.Count > 0;
    }

    ZoneCollection GetRelevantZones(Vector3F pos, bool bInsideCachedRadius)
    {
      float fFarClip = EditorManager.ActiveView.FarClipPlane;
      ZoneCollection zones = new ZoneCollection();
      Vector3D pivot = EditorManager.Scene.GlobalPivot;
      foreach (Zone zone in EditorManager.Scene.Zones)
      {
        BoundingBox zoneBox = bInsideCachedRadius ? zone.AbsoluteBoundingBox : zone.AssignmentBoundingBox;
        if (zoneBox == null || !zoneBox.Valid)
          zoneBox = zone.AssignmentBoundingBox;
        if (!zoneBox.Valid)
          continue;
        Vector3F offset = new Vector3F((float)(pivot.X - zone.ZonePivot.X), (float)(pivot.Y - zone.ZonePivot.Y), (float)(pivot.Z - zone.ZonePivot.Z)); 
        float fDist = zoneBox.GetDistanceXY(pos + offset);
        if (fDist <= 0.0f || (bInsideCachedRadius && fDist <= zone.GetFinalLoadedDistance(fFarClip)))
          zones.Add(zone);
      }
      return zones;
    }


    void LoadZonesAtCameraPosition(bool bInsideCachedRadius)
    {
      ZoneCollection zones = GetRelevantZones(EditorManager.ActiveView.CameraPosition, bInsideCachedRadius);
      GroupAction action = new GroupAction("Reload zones");
      foreach (Zone zone in zones)
        if (!zone.Loaded)
          action.Add(new SetZoneLoadedStatusAction(zone, true));
      EditorManager.Actions.Add(action);
    }

    private void selectZoneAtCameraPositionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      ZoneCollection zones = GetRelevantZones(EditorManager.ActiveView.CameraPosition, false);
      if (zones.Count == 1)
        EditorManager.Scene.ActiveZone = zones[0];
    }

    private void loadZoneAtCameraPositionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      LoadZonesAtCameraPosition(false);
    }

    private void loadZonesCloseToCameraToolStripMenuItem_Click(object sender, EventArgs e)
    {
      LoadZonesAtCameraPosition(true);
    }

    #region Context PasteAtCursor

    private void pasteAtCursorToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Layer layer = EditorManager.Scene.ActiveLayer;
      if (layer == null || !visionView.Clipboard.HasData())
        return;
      Vector3F hitPoint = new Vector3F();
      if (!GetMouseTraceHitPoint(ref hitPoint))
        return;

      // single shape in clipboard
      Shape3D shape = visionView.Clipboard.DataObject as Shape3D;
      if (shape != null)
      {
        shape = (Shape3D)shape.Clone();
        EditorManager.Actions.Add(AddShapeAction.CreateAddShapeAction(shape, layer.ActiveShape, layer, true));
        shape.Position = hitPoint;
      }

      // selection collection in clipboard? (->default)
      ShapeCollection shapes = visionView.Clipboard.DataObject as ShapeCollection;
      if (shapes != null)
      {
        ShapeCollection added = shapes.CloneForClipboard();
        EditorManager.Actions.Add(AddShapesAction.CreateAddShapesAction(added, layer.ActiveShape, layer, true, "paste"));
        foreach (ShapeBase s in added)
        {
          if (s is Shape3D)
            ((Shape3D)s).Position = hitPoint;
        }
        EditorManager.SelectedShapes = added;
      }
    }

    #endregion

    #region Context Grid

    private void setupGridToolStripMenuItem_Click(object sender, EventArgs e)
    {
      GridPanel.Instance.Show();
    }

    private void showGridToolStripMenuItem_Click(object sender, EventArgs e)
    {
      showGridToolStripMenuItem.Checked = !showGridToolStripMenuItem.Checked;
      GizmoBase.GridSettings.GridMode = showGridToolStripMenuItem.Checked ? GridSettings.GridRenderMode.XYPlane : GridSettings.GridRenderMode.Disabled;
      EditorApp.ActiveView.UpdateView(false);
    }

    private void toolStripButton_EnableMoveSnap_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.MoveGrid.Enabled = toolStripButton_EnableMoveSnap.Checked;
    }

    private void toolStripButton_EnableAngleSnap_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.RotateGrid.Enabled = toolStripButton_EnableAngleSnap.Checked;
    }

    private void toolStripButton_Move_DropDownOpening(object sender, EventArgs e)
    {
      toolStripButton_EnableMoveSnap.Checked = EditorApp.ActiveView.Gizmo.MoveGrid.Enabled;
      toolStripButton_localPos.Checked = EditorApp.ActiveView.Gizmo.LocalTranslation;
    }

    private void toolStripButton_Rotate_DropDownOpening(object sender, EventArgs e)
    {
      toolStripButton_EnableAngleSnap.Checked = EditorApp.ActiveView.Gizmo.RotateGrid.Enabled;
      toolStripButton_localOri.Checked = EditorApp.ActiveView.Gizmo.LocalOrientation;
    }

    #endregion

    #region Context Rendering

    private void solidToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;
      visionView.EngineManager.RenderMode = RenderMode_e.Solid;
      EditorApp.ActiveView.UpdateView(false);

      // If sender is menu item from toolbar leave dropdown open
      if (sender == ToolStrip_Rendering_Solid)
        ToolStripSplitButton_Rendering.ShowDropDown();
    }

    private void wireframeToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;
      visionView.EngineManager.RenderMode = RenderMode_e.Wireframe;
      EditorApp.ActiveView.UpdateView(false);

      // If sender is menu item from toolbar leave dropdown open
      if (sender == ToolStrip_Rendering_Wireframe)
        ToolStripSplitButton_Rendering.ShowDropDown();
    }

    private void viewPropertiesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;

      FieldOfViewDlg dlg = new FieldOfViewDlg();
      dlg.Settings = visionView.DefaultViewSettings;

      if (dlg.ShowDialog(this) == DialogResult.OK)
      {
        V3DLayer layer = EditorApp.Scene.V3DLayer;
        if (layer != null)
        {
          // Commit the changes made by the dialog to the main layer as well via an undo-able action.
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(layer, "FOV", dlg.Settings.FOV.X));
        }
      }
    }

    /// <summary>
    /// Still old menu?
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Profiling_Next_Click(object sender, System.EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;

      EditorManager.EngineManager.ProfilingNext();
    }

    /// <summary>
    /// Still old menu?
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void Profiling_Previous_Click(object sender, System.EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;

      EditorManager.EngineManager.ProfilingPrevious();
    }

    #endregion

    #region Context Spawn position


    /// <summary>
    /// Helper function
    /// </summary>
    /// <param name="hitPoint"></param>
    /// <returns></returns>
    bool GetMouseTraceHitPoint(ref Vector3F hitPoint)
    {
      if (!visionView.EngineManager.IsInitialized())
        return false;
      Vector3F startRay, endRay;
      Vector3F hitAngles = new Vector3F();
      int x = EditorApp.ActiveView.CurrentContext.MouseX;
      int y = EditorApp.ActiveView.CurrentContext.MouseY;
      EditorManager.EngineManager.GetRayAtScreenPos(out startRay, out endRay, x, y, EditorManager.Settings.MaxPickingDistance);
      if (!EditorManager.EngineManager.GetTraceHitWithAngles(startRay, endRay, ref hitPoint, ref hitAngles))
        return false;

      return true;
    }

    private void sceneOriginToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;
      EditorManager.Scene.CurrentShapeSpawnPosition = new Vector3F(0, 0, 0);
      EditorApp.ActiveView.UpdateView(false);
    }

    private void cameraPositionToolStripMenuItem1_Click(object sender, EventArgs e)
    {
      if (!visionView.EngineManager.IsInitialized())
        return;
      EditorManager.Scene.CurrentShapeSpawnPosition = EditorManager.ActiveView.CameraPosition;
      EditorApp.ActiveView.UpdateView(false);
    }

    private void traceHitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Vector3F hitPoint = new Vector3F();
      if (GetMouseTraceHitPoint(ref hitPoint))
        EditorManager.Scene.CurrentShapeSpawnPosition = hitPoint;
      EditorApp.ActiveView.UpdateView(false);
    }

    #endregion

    #region Context Shading

    #region Nested Class : ShadingEffectMenuItem

    /// <summary>
    /// Event arguments when the shading mode gets changed
    /// </summary>
    class ShadingModeChangedEventArgs : EventArgs
    {
      public string NewShadingModeName;
      public int NewShadingModeIndex;

      public ShadingModeChangedEventArgs(string modeName, int modeIndex)
        : base()
      {
        NewShadingModeName = modeName;
        NewShadingModeIndex = modeIndex;
      }
    }

    /// <summary>
    /// Tool strip menu item for shading modes
    /// </summary>
    class ShadingEffectMenuItem : ToolStripMenuItem
    {
      //Normal shading name (e.g. no debug rendering like Lightmaps, MaterialIndex, etc...)
      public static string NORMAL_SHADING_STRING = "Normal";

      //Normal shading index, all other render options follow after this index
      public static int NORMAL_SHADING_INDEX = -1;

      /// <summary>
      /// Invoked when the shading mode changes via one of the tool strip buttons
      /// </summary>
      public delegate void ShadingModeChanged(ShadingModeChangedEventArgs e);

      /// <summary>
      /// Event fired before the shading mode will be changed
      /// </summary>
      public event ShadingModeChanged BeforeChangingShadingMode;

      /// <summary>
      /// Event fired after the shading mode has been changed
      /// </summary>
      public event ShadingModeChanged AfterChangingShadingMode;

      public ShadingEffectMenuItem(string name, int iIndex, ToolStripDropDownItem owner)
      {
        _owner = owner;
        Text = name;
        _iEffectIndex = iIndex;
        Image = GetShadingModeIcon(name);
        ImageScaling = ToolStripItemImageScaling.None;
        Click += new EventHandler(ShadingEffectMenuItem_Click);
      }

      private void ShadingEffectMenuItem_Click(object sender, EventArgs e)
      {
        ShadingModeChangedEventArgs newMode = new ShadingModeChangedEventArgs(this.Text, _iEffectIndex);

        //notify listeners
        if (BeforeChangingShadingMode != null)
          BeforeChangingShadingMode(newMode);

        VisionEngineManager engineManager = (VisionEngineManager)EditorManager.EngineManager;
        engineManager.SetReplacementRenderLoopEffect(_iEffectIndex);

        // If owner is toolbar button leave dropdown open
        if (_owner.GetType() == typeof(ToolStripSplitButton))
        {
          EditorManager.ActiveView.UpdateView(true);
          _owner.ShowDropDown();
        }

        //notify listeners
        if (AfterChangingShadingMode != null)
          AfterChangingShadingMode(newMode);
      }

      public void EvaluateCheckedStatus()
      {
        VisionEngineManager em = (VisionEngineManager)EditorManager.EngineManager;
        this.Checked = _iEffectIndex == em.GetCurrentReplacementRenderLoopEffect();
      }

      private Image GetShadingModeIcon(string name)
      {
        switch (name.ToLower())
        {
          case "normal": return Properties.Resources.view_default;
          case "fullbright": return Properties.Resources.view_fullbright;
          case "checkerboard": return Properties.Resources.view_checker;
          case "texture sizes": return Properties.Resources.view_checker;
          case "lighting only": return Properties.Resources.view_shaded;
          case "shaded lighting": return Properties.Resources.view_shaded;
          case "directional lighting": return Properties.Resources.view_shaded;
          case "overdraw": return Properties.Resources.view_overdraw;
          case "display normalmaps": return Properties.Resources.view_normalmap;
          case "display worldspace normals": return Properties.Resources.view_normals;
          case "display worldspace tangents": return Properties.Resources.view_tangents;
          case "display uv0": return Properties.Resources.view_checker;
          case "display uv1": return Properties.Resources.view_checker;
          case "display vertex colors": return Properties.Resources.view_normals;
          case "visibility zone assignment": return Properties.Resources.view_plainwhite;
          case "streaming zone assignment": return Properties.Resources.view_plainwhite;
          case "display texture miplevels": return Properties.Resources.view_checker;
          case "texture density": return Properties.Resources.view_checker;
          default: return Properties.Resources.view_plainwhite;
        }
      }

      public int _iEffectIndex = -1;
      private ToolStripDropDownItem _owner;
    }

    #endregion

    #endregion

    #region Context Pick...

    private void Pick_UV0_ToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (EditorManager.ActiveView.CurrentContext is UV0PickerContext)
        EditorManager.ActiveView.SetDefaultContext();
      else
        EditorManager.ActiveView.CurrentContext = new UV0PickerContext();
    }

    private void Pick_UV1_ToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (EditorManager.ActiveView.CurrentContext is UV1PickerContext)
        EditorManager.ActiveView.SetDefaultContext();
      else
        EditorManager.ActiveView.CurrentContext = new UV1PickerContext();
    }

    #endregion

    #endregion

    #region Engine Toolbar

    /// <summary>
    /// Gets the engine panel toolbar
    /// </summary>
    public ToolStrip Toolbar
    {
      get { return toolStrip_EnginePanel; }
    }

    void UpdateToolbarStatus()
    {
      bool bHasScene = EditorManager.Scene != null;
      bool bPerspective = EditorManager.ActiveView.IsPerspective;

      toolStripButton_MoveContext.Enabled = bHasScene;
      toolStripButton_MoveCameraWithKeys.Enabled = bHasScene;
      ToolStripButton_Move.Enabled = bHasScene;
      ToolStripButton_Rotate.Enabled = bHasScene;
      toolStripButton_StickToGround.Enabled = bHasScene;
      ToolStripButton_Link.Enabled = bHasScene;
      toolStripButton_MeasureTool.Enabled = bHasScene;
      toolStripDropDownButton_PlayMode.Enabled = bHasScene;
      toolStripDropDownButton_PerspectiveMode.Enabled = bHasScene;
      ToolStripButton_Scale.Enabled = bHasScene;
      ToolStripButton_ZoomFit.Enabled = bHasScene;
      ToolStripButton_AutomaticZoomFit.Enabled = bHasScene;
      ToolStripSplitButton_MoveSensitivity.Enabled = bHasScene;
      ToolStripSplitButton_SelectionFilter.Enabled = bHasScene;
      ToolStripSplitButton_Rendering.Enabled = bHasScene;
      ToolStripSplitButton_DebugFlags.Enabled = bHasScene;
      ToolStripSplitButton_Tools.Enabled = bHasScene;
      ToolStripButton_IsolateSelection.Enabled = bHasScene && (ToolStripButton_IsolateSelection.Checked || EditorManager.SelectedShapes.Count > 0);

      ToolStripSplitButton_Camera.Enabled = bHasScene;
      ToolStripMenuItem_View_EditClipRange.Enabled = bHasScene;
      toolStripSplitButton_ViewIcons.Enabled = bHasScene;
    }

    /// <summary>
    /// PlayMode which the user selected last (used as default when clicking the Play button)
    /// </summary>
    EditorManager.Mode _lastActivePlayMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;

    /// <summary>
    /// The drop down store the icons for the play resp stop button
    /// </summary>
    

    private void toolStripButton_MoveContext_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.SetDefaultContext();
    }

    private void toolStripButton_MoveCameraWithKeys_Click(object sender, EventArgs e)
    {
      CameraMoveContext context = EditorApp.ActiveView.CurrentContext as CameraMoveContext;
      if (context != null)
      {
        toolStripButton_MoveCameraWithKeys.Checked = !toolStripButton_MoveCameraWithKeys.Checked;
        EditorManager.Settings.MoveCameraWithKeys = toolStripButton_MoveCameraWithKeys.Checked;
      }
    }

    private void ToolStripSplitButton_Camera_MoveOrigin_Click(object sender, EventArgs e)
    {
      if (EditorApp.Scene == null)
        return;
      EditorApp.ActiveView.SetCameraPosition(new Vector3F(0, 0, 0));
      EditorApp.ActiveView.SetCameraRotation(new Vector3F(0, 0, 0));
      EditorApp.ActiveView.UpdateView(false);
    }

    private void ToolStripSplitButton_Camera_MoveShapeOrigin_Click(object sender, System.EventArgs e)
    {
      if (EditorApp.Scene == null)
        return;
      EditorApp.ActiveView.SetCameraPosition(EditorApp.Scene.CurrentShapeSpawnPosition);
      EditorApp.ActiveView.SetCameraRotation(new Vector3F(0, 0, 0));
      EditorApp.ActiveView.UpdateView(false);
    }

    private void ToolStripSplitButton_Camera_MoveSelection_Click(object sender, System.EventArgs e)
    {
      if (EditorApp.Scene == null && !EditorApp.ActiveView.Gizmo.Visible)
        return;
      Vector3F newPos = EditorApp.ActiveView.Gizmo.Position;
      Vector3F camDir = EditorApp.ActiveView.CameraDirection;
      EditorApp.ActiveView.SetCameraPosition(newPos - camDir * 50.0f);
      EditorApp.ActiveView.UpdateView(false);
    }

    private void toolStripButton_Move_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.DragMode = toolStripButton_VertexSnapping.Checked ? ShapeDragMode.NONE : ShapeDragMode.MOVE;
      EditorApp.ActiveView.Gizmo.LocalTranslation = toolStripButton_localPos.Checked;
    }

    private void toolStripButton_Rotate_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.ROTATE;
      EditorApp.ActiveView.Gizmo.LocalOrientation = toolStripButton_localOri.Checked;
    }

    private void toolStripButton_localPos_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.LocalTranslation = toolStripButton_localPos.Checked;
    }

    private void toolStripButton_localOri_Click(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.LocalOrientation = toolStripButton_localOri.Checked;
    }


    private void toolStripButton_TranslationMode_Click(object sender, EventArgs e)
    {
      if (sender == toolStripButton_StickToGround && toolStripButton_StickToGround.Checked)
      {
        EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.MOVE;
        EditorApp.ActiveView.Gizmo.GizmoTranslationMode = CSharpFramework.GizmoBase.TranslationMode.ObjectsStickToGround;
      }
      else if (sender == toolStripButton_VertexSnapping && toolStripButton_VertexSnapping.Checked)
      {
        EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.NONE;
        EditorApp.ActiveView.Gizmo.GizmoTranslationMode = CSharpFramework.GizmoBase.TranslationMode.SnapToVertices;
      }
      else
      {
        EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.MOVE;
        EditorApp.ActiveView.Gizmo.GizmoTranslationMode = CSharpFramework.GizmoBase.TranslationMode.StandardTranslation;
      }
    }

    private void toolStripButton_Link_Click(object sender, EventArgs e)
    {
      if (EditorApp.ActiveView.Gizmo.DragMode == ShapeDragMode.LINK)
        EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.MOVE;
      else
        EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.LINK;
    }

    private void toolStripButton_Link_Show_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem button = sender as ToolStripMenuItem;
      if (button == null)
        return;

      toolStripButton_Link_ShowNone.Checked = false;
      toolStripButton_Link_ShowAll.Checked = false;
      toolStripButton_Link_ShowSelected.Checked = false;
      toolStripButton_Link_ShowActiveLayer.Checked = false;
      button.Checked = true;

      if (toolStripButton_Link_ShowNone.Checked)
        EditorManager.Settings.LinkDisplayMode = EditorSettingsBase.LinkDisplayMode_e.None;
      else if (toolStripButton_Link_ShowAll.Checked)
        EditorManager.Settings.LinkDisplayMode = EditorSettingsBase.LinkDisplayMode_e.All;
      else if (toolStripButton_Link_ShowSelected.Checked)
        EditorManager.Settings.LinkDisplayMode = EditorSettingsBase.LinkDisplayMode_e.SelectedShapes;
      else if (toolStripButton_Link_ShowActiveLayer.Checked)
        EditorManager.Settings.LinkDisplayMode = EditorSettingsBase.LinkDisplayMode_e.ActiveLayer;
    }

    private void toolStripButton_Link_DropDownOpening(object sender, EventArgs e)
    {
      toolStripButton_Link_ShowNone.Checked = EditorManager.Settings.LinkDisplayMode == EditorSettingsBase.LinkDisplayMode_e.None;
      toolStripButton_Link_ShowAll.Checked = EditorManager.Settings.LinkDisplayMode == EditorSettingsBase.LinkDisplayMode_e.All;
      toolStripButton_Link_ShowSelected.Checked = EditorManager.Settings.LinkDisplayMode == EditorSettingsBase.LinkDisplayMode_e.SelectedShapes;
      toolStripButton_Link_ShowActiveLayer.Checked = EditorManager.Settings.LinkDisplayMode == EditorSettingsBase.LinkDisplayMode_e.ActiveLayer;
    }

    private void toolStripDropDownButton_PerspectiveMode_DropDownOpening(object sender, EventArgs e)
    {
      VisionViewBase.ProjectionMode_e mode = EditorManager.ActiveView.ProjectionMode;
      ToolStripMenuItem_Perspective.Checked = mode == VisionViewBase.ProjectionMode_e.Perspective;
      ToolStripMenuItem_PerspectiveTop.Enabled = true;
      ToolStripMenuItem_PerspectiveFront.Enabled = true;
      ToolStripMenuItem_PerspectiveRight.Enabled = true;
      ToolStripMenuItem_Top.Checked = mode == VisionViewBase.ProjectionMode_e.Top;
      ToolStripMenuItem_Front.Checked = mode == VisionViewBase.ProjectionMode_e.Front;
      ToolStripMenuItem_Right.Checked = mode == VisionViewBase.ProjectionMode_e.Right;

      // Check whether engine view panel is docked / undocked
      ToolStripMenuItem_View_Size.Enabled = ((WeifenLuo.WinFormsUI.DockPane)Parent).DockState == WeifenLuo.WinFormsUI.DockState.Float;
    }

    private void perspectiveToolStripMenuItem_Perspective_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.ProjectionMode = VisionViewBase.ProjectionMode_e.Perspective;
    }

    private void perspectiveToolStripMenuItem_PerspectiveTop_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.ProjectionMode = VisionViewBase.ProjectionMode_e.Perspective;

      // Calculate distance between camera and focusPoint
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;
      float distance = (context.FocusPoint - View.CameraPosition).GetLength();

      // Reorient perspective camera to a top-down view
      EditorManager.ActiveView.SetCameraRotation(new Vector3F(90, 90, 0));

      // Position focus point relative to camera
      Vector3F cameraPosition = View.CameraPosition;
      context.FocusPoint = View.CameraPosition + View.CameraDirection * distance;
    }

    private void perspectiveToolStripMenuItem_PerspectiveFront_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.ProjectionMode = VisionViewBase.ProjectionMode_e.Perspective;

      // Calculate distance between camera and focusPoint
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;
      float distance = (context.FocusPoint - View.CameraPosition).GetLength();

      // Reorient perspective camera to a top-down view
      EditorManager.ActiveView.SetCameraRotation(new Vector3F(90, 0, 0));

      // Position focus point relative to camera
      Vector3F cameraPosition = View.CameraPosition;
      context.FocusPoint = View.CameraPosition + View.CameraDirection * distance;
    }

    private void perspectiveToolStripMenuItem_PerspectiveRight_Click(object sender, EventArgs e)
    {
      EditorManager.ActiveView.ProjectionMode = VisionViewBase.ProjectionMode_e.Perspective;

      // Calculate distance between camera and focusPoint
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;
      float distance = (context.FocusPoint - View.CameraPosition).GetLength();

      // Reorient perspective camera to a top-down view
      EditorManager.ActiveView.SetCameraRotation(new Vector3F(180, 0, 0));

      // Position focus point relative to camera
      Vector3F cameraPosition = View.CameraPosition;
      context.FocusPoint = View.CameraPosition + View.CameraDirection * distance;
    }

    private void enginePanel_SceneEvent(object sender, SceneEventArgs e)
    {
      if (e.action == SceneEventArgs.Action.AfterRendererNodeChanged)
      {
        //the render node changed...
        if (EditorManager.Scene!=null && EditorManager.Scene.RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_DEFERRED)
        {
          if (EditorManager.ActiveView.ProjectionMode != VisionViewBase.ProjectionMode_e.Perspective)
          {
            //switch back to perspective because we do not support deferred + orthogonal
            EditorManager.ActiveView.ProjectionMode = VisionViewBase.ProjectionMode_e.Perspective;
          }

          //switch back to normal shading
          VisionEngineManager engineManager = (VisionEngineManager)EditorManager.EngineManager;
          engineManager.SetReplacementRenderLoopEffect(ShadingEffectMenuItem.NORMAL_SHADING_INDEX);
          EditorManager.ActiveView.UpdateView(true);
        }
      }
    }

    private void topToolStripMenuItem_Top_Click(object sender, EventArgs e)
    {
      othogonalProjection_enable(VisionViewBase.ProjectionMode_e.Top);
    }

    private void othogonalProjection_enable(VisionViewBase.ProjectionMode_e mode)
    {
      // we do not need to perform any extra operations if we are already in orthogonal projection mode
      if ( (EditorManager.Scene != null && EditorManager.Scene.RendererNodeClass != IRendererNodeManager.RENDERERNODECLASS_DEFERRED) ||
           EditorManager.ActiveView.ProjectionMode != VisionViewBase.ProjectionMode_e.Perspective)
      {
        EditorManager.ActiveView.ProjectionMode = mode;
        return;
      }

      VisionEngineManager engineManager = (VisionEngineManager)EditorManager.EngineManager;

      //normal shading mode and deferred render does not work - inform user and ask what to do
      if (ShadingEffectMenuItem.NORMAL_SHADING_INDEX == engineManager.GetCurrentReplacementRenderLoopEffect())
      {
        //deferred render node + switch to orthogonal projection
        if (!EditorManager.Settings.ShadedLightingForDeferredOrthogonal)
        {
          DialogResult result = MessageBox.Show("Orthogonal Projection is not supported for the Deferred Renderer Node. " +
              "You can however use a different 'Shading Mode' and then use the Orthogonal " +
              "Projection. Do you want to switch to 'Shaded Lighting' and " +
              "then enable the Orthogonal Projection when using this menu option? The shading mode will "+
              "be set back after switching back to Perspective Projection.",
              "Orthogonal Projection", MessageBoxButtons.YesNo, MessageBoxIcon.Information);

          if (result == DialogResult.Yes)
            EditorManager.Settings.ShadedLightingForDeferredOrthogonal = true;
        }
      }

      //switch to the desired mode (always needs to be performed - maybe the user alreadymade a permanent decision in the previous dialog
      othogonalProjection_switchTo(mode);
    }

    private bool othogonalProjection_switchTo(VisionViewBase.ProjectionMode_e mode)
    {
      VisionEngineManager engineManager = (VisionEngineManager)EditorManager.EngineManager;

      if (EditorManager.Settings.ShadedLightingForDeferredOrthogonal &&
          ShadingEffectMenuItem.NORMAL_SHADING_INDEX == engineManager.GetCurrentReplacementRenderLoopEffect())
      {
        //only change if we are in the 'normal' shading mode

        //get all shading modes
        StringCollection shadingModes = new StringCollection();
        engineManager.GetReplacementRenderLoopEffects(shadingModes);

        //find the desired shading mode
        for (int i = 0; i < shadingModes.Count; i++)
        {
          if (shadingModes[i] == "Shaded Lighting")
          {
            engineManager.SetReplacementRenderLoopEffect(i);
            EditorManager.ActiveView.ProjectionMode = mode;
            EditorManager.ActiveView.UpdateView(true);

            // Mark that we want to restore normal shading mode when changing back
            _restorePerspectiveNormalShadingMode = true;
            return true;
          }
        }

      }
      else if (ShadingEffectMenuItem.NORMAL_SHADING_INDEX != engineManager.GetCurrentReplacementRenderLoopEffect())
      {
        //currently not using 'normal' shading therefore we can switch to orthogonal
        EditorManager.ActiveView.ProjectionMode = mode;
        return true;
      }

      return false;
    }

    private void frontToolStripMenuItem_Front_Click(object sender, EventArgs e)
    {
      othogonalProjection_enable(VisionViewBase.ProjectionMode_e.Front);
    }

    private void rightToolStripMenuItem_Right_Click(object sender, EventArgs e)
    {
      othogonalProjection_enable(VisionViewBase.ProjectionMode_e.Right);
    }

    private void toolStripButton_ZoomFit_Click(object sender, EventArgs e)
    {
      FocusOnSelection();
    }

    private void ToolStripButton_ZoomFit_DropDownOpening(object sender, EventArgs e)
    {
      ToolStripButton_AutomaticZoomFit.Checked = EditorManager.Settings.FocusOnSelection;
    }

    private void ToolStripButton_AutomaticZoomFit_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.FocusOnSelection = ToolStripButton_AutomaticZoomFit.Checked;
    }

    private void FocusOnSelection()
    {
      // Either get bounding box from selected shapes or from scene
      BoundingBox mergedBox;
      if (EditorManager.SelectedShapes.Count > 0)
        mergedBox = EditorManager.SelectedShapes.BoundingBox;
      else if (!EditorManager.Settings.FocusOnSelection)
        mergedBox = EditorManager.Scene.CalculateSceneExtents;
      else
        return;

      if (mergedBox.Valid)
      {
        // Make it local again
        Vector3F center = mergedBox.Center;
        mergedBox.Translate(-center);
        EditorManager.ActiveView.LookAt(center, mergedBox);

        // When viewing orthographically, set OrthographicScale to frame selection
        switch (EditorManager.ActiveView.ProjectionMode)
        {
          case VisionViewBase.ProjectionMode_e.Top:
            // Adjust scale to fit maximum dimension of mergedBox in x/y plane
            View.OrthographicScale = Math.Max(mergedBox.SizeX / 1000, mergedBox.SizeY / 1000);
            break;
          case VisionViewBase.ProjectionMode_e.Front:
            // Adjust scale to fit maximum dimension of mergedBox in y/z plane
            View.OrthographicScale = Math.Max(mergedBox.SizeY / 1000, mergedBox.SizeZ / 1000);
            break;
          case VisionViewBase.ProjectionMode_e.Right:
            // Adjust scale to fit maximum dimension of mergedBox in x/z plane
            View.OrthographicScale = Math.Max(mergedBox.SizeX / 1000, mergedBox.SizeZ / 1000);
            break;
          default:
            break;
        }
      }
    }

    private void ToolStripButton_IsolateSelection_CheckedChanged(object sender, EventArgs e)
    {
      // Update view
      visionView.IsolateSelection(ToolStripButton_IsolateSelection.Checked, false);

      // Update tooltip text
      if (ToolStripButton_IsolateSelection.Checked)
      {
        ToolStripButton_IsolateSelection.ToolTipText = "Exit Isolate Selection";
        ToolStripButton_IsolateSelection.Image = Properties.Resources.isolate_exit;
      }
      else
      {
        ToolStripButton_IsolateSelection.ToolTipText = "Isolate Selection";
        ToolStripButton_IsolateSelection.Image = Properties.Resources.isolate_selection;
      }

      // Still in isolate selection mode
      ToolStripButton_IsolateSelection.Enabled = (ToolStripButton_IsolateSelection.Checked || EditorManager.SelectedShapes.Count > 0);
    }

    private void toolStripDropDownButton_PlayMode_Click(object sender, EventArgs e)
    {
      // toggle play mode (button image responds to it)
      if (EditorManager.EditorMode == EditorManager.Mode.EM_NONE)
        EditorManager.EditorMode = _lastActivePlayMode;
      else
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
    }

    private void toolStripDropDownButton_PlayMode_ButtonClick(object sender, EventArgs e)
    {
      // toggle play mode (button image responds to it)
      if (EditorManager.EditorMode == EditorManager.Mode.EM_NONE)
        EditorManager.EditorMode = _lastActivePlayMode;
      else
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
    }

    public void animateToolStripMenuItem_Animate_Click(object sender, EventArgs e)
    {
      if (EditorManager.EditorMode == EditorManager.Mode.EM_NONE)
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_ANIMATING;
        _lastActivePlayMode = EditorManager.Mode.EM_ANIMATING;
      }
      else
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      }
    }

    public void runInEditorToolStripMenuItem_RunInEditor_Click(object sender, EventArgs e)
    {
      if (EditorManager.EditorMode == EditorManager.Mode.EM_NONE)
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;
        _lastActivePlayMode = EditorManager.Mode.EM_PLAYING_IN_EDITOR;
      }
      else
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      }
    }

    public void playTheGameToolStripMenuItem_PlayTheGame_Click(object sender, EventArgs e)
    {
      if (EditorManager.EditorMode == EditorManager.Mode.EM_NONE)
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_PLAYING_IN_GAME;
        _lastActivePlayMode = EditorManager.Mode.EM_PLAYING_IN_GAME;
      }
      else
      {
        EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
        View.Dock = System.Windows.Forms.DockStyle.Fill;
      }
    }

    private void toolStripButton_SelectViewBox_Click(object sender, EventArgs e)
    {
      V3DLayer layer = EditorApp.Scene.V3DLayer;
      EditorManager.ActiveView.Gizmo.SetSingleShape(layer.OrthographicViewBox, false);
    }

    private void toolStripButton_MeasureTool_Click(object sender, EventArgs e)
    {
      if (EditorManager.Scene == null)
        return;
      if (EditorManager.ActiveView.CurrentContext is MeasureToolCamera)
        EditorManager.ActiveView.SetDefaultContext();
      else
        EditorManager.ActiveView.CurrentContext = new MeasureToolCamera();
    }



    private void toolStripSplitButton_ViewIcons_DropDownOpening(object sender, EventArgs e)
    {
      ViewIconSettings settings = VisionViewBase.IconSettings;
      showViewIconsToolStripMenuItem.Checked = settings.ShowIcons;
      showHiddenShapesToolStripMenuItem.Checked = !settings.VisibleShapesOnly;

      ToolStripMenuItem_IconAction_None.Checked = settings.Action == ViewIconSettings.Action_e.None;
      ToolStripMenuItem_IconAction_Select.Checked = settings.Action == ViewIconSettings.Action_e.Select;
      ToolStripMenuItem_IconAction_ToggleFreeze.Checked = settings.Action == ViewIconSettings.Action_e.ToggleFreezeState;
      ToolStripMenuItem_IconAction_Delete.Checked = settings.Action == ViewIconSettings.Action_e.CustomAction && settings.ActionClass == typeof(RemoveShapeAction);
      ToolStripMenuItem_IconAction_Drop.Checked = settings.Action == ViewIconSettings.Action_e.CustomAction && settings.ActionClass == typeof(DropToFloorPivotAction);
      ToolStripMenuItem_IconAction_Drop_BB.Checked = settings.Action == ViewIconSettings.Action_e.CustomAction && settings.ActionClass == typeof(DropToFloorBoxAction);
      ToolStripMenuItem_IconAction_Drop_BC.Checked = settings.Action == ViewIconSettings.Action_e.CustomAction && settings.ActionClass == typeof(DropToFloorBottomCenterAction);
    }

    private void toolStripSplitButton_ViewIcons_ButtonClick(object sender, EventArgs e)
    {
      VisionViewBase.IconSettings.ShowIcons = !VisionViewBase.IconSettings.ShowIcons;
    }

    private void showViewIconsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      VisionViewBase.IconSettings.ShowIcons = !VisionViewBase.IconSettings.ShowIcons;
    }

    private void showHiddenShapesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      VisionViewBase.IconSettings.VisibleShapesOnly = !VisionViewBase.IconSettings.VisibleShapesOnly;
    }

    private void selectOnClickToolStripMenuItem_Click(object sender, EventArgs e)
    {
      //VisionViewBase.IconSettings.Selectable = !VisionViewBase.IconSettings.Selectable;
    }

    void ViewIconSettings_OnViewIconSettingsChanged(object sender, EventArgs e)
    {
      //      if (sender == VisionViewBase.IconSettings)
      //        toolStripSplitButton_ViewIcons.Pressed = VisionViewBase.IconSettings.ShowIcons;
    }

    private void ToolStripMenuItem_IconAction_None_Click(object sender, EventArgs e)
    {
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.None, null, null, null);
    }

    private void ToolStripMenuItem_IconAction_Select_Click(object sender, EventArgs e)
    {
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.Select, null, null, null);

    }

    private void ToolStripMenuItem_IconAction_Delete_Click(object sender, EventArgs e)
    {
      string filename = null;
      if (RemoveShapeAction.iIconIndex >= 0)
        filename = EditorManager.GUI.ActionImages.FileNames[RemoveShapeAction.iIconIndex];
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.CustomAction, filename, typeof(RemoveShapeAction), null);
    }

    private void ToolStripMenuItem_IconAction_ToggleFreeze_Click(object sender, EventArgs e)
    {
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.ToggleFreezeState, null, null, null);
    }

    private void ToolStripMenuItem_IconAction_Drop_Click(object sender, EventArgs e)
    {
      string filename = null;
      if (RemoveShapeAction.iIconIndex >= 0)
        filename = EditorManager.GUI.ActionImages.FileNames[DropToFloorAction.iIconIndex];
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.CustomAction, filename, typeof(DropToFloorPivotAction),
        new object[3] { null, CurrentDropToFloorAxis, true });
    }

    private void ToolStripMenuItem_IconAction_Drop_BB_Click(object sender, EventArgs e)
    {
      string filename = null;
      if (RemoveShapeAction.iIconIndex >= 0)
        filename = EditorManager.GUI.ActionImages.FileNames[DropToFloorAction.iIconIndex];
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.CustomAction, filename, typeof(DropToFloorBoxAction),
        new object[3] { null, CurrentDropToFloorAxis, true });
    }

    private void ToolStripMenuItem_IconAction_Drop_BC_Click(object sender, EventArgs e)
    {
      string filename = null;
      if (RemoveShapeAction.iIconIndex >= 0)
        filename = EditorManager.GUI.ActionImages.FileNames[DropToFloorAction.iIconIndex];
      VisionViewBase.IconSettings.SetAction(ViewIconSettings.Action_e.CustomAction, filename, typeof(DropToFloorBottomCenterAction),
        new object[3] { null, CurrentDropToFloorAxis, true });
    }

    private void toolStripSplitButton_Scale_ButtonClick(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.SCALE;
    }

    private void toolStripDropDownButton_PerspectiveMode_ButtonClick(object sender, EventArgs e)
    {
      toolStripDropDownButton_PerspectiveMode.ShowDropDown();
    }


    #region Engine View Toolbar - Shading Menu

    private void ToolStripSplitButton_Rendering_ButtonClick(object sender, EventArgs e)
    {
      if (ToolStrip_Rendering_Solid.Checked)
      {
        visionView.EngineManager.RenderMode = RenderMode_e.Wireframe;
        ToolStrip_Rendering_Solid.Checked = false;
        ToolStrip_Rendering_Wireframe.Checked = true;
      }
      else if (ToolStrip_Rendering_Wireframe.Checked)
      {
        visionView.EngineManager.RenderMode = RenderMode_e.Solid;
        ToolStrip_Rendering_Solid.Checked = true;
        ToolStrip_Rendering_Wireframe.Checked = false;
      }
      else
      {
        visionView.EngineManager.RenderMode = (visionView.EngineManager.RenderMode == RenderMode_e.Solid) ? RenderMode_e.Wireframe : RenderMode_e.Solid;
        ToolStrip_Rendering_Solid.Checked = visionView.EngineManager.RenderMode == RenderMode_e.Solid;
        ToolStrip_Rendering_Wireframe.Checked = visionView.EngineManager.RenderMode == RenderMode_e.Wireframe;
      }

      visionView.UpdateView(true);
    }

    private static int NUM_OF_SHADING_MODES_MIN = 8;

    private void ToolStripSplitButton_Rendering_DropDownOpening(object sender, EventArgs e)
    {
      // Render mode
      {
        ToolStrip_Rendering_Solid.Checked = visionView.EngineManager.RenderMode == RenderMode_e.Solid;
        ToolStrip_Rendering_Wireframe.Checked = visionView.EngineManager.RenderMode == RenderMode_e.Wireframe;
      }

      // Add view effects to engine toolbar
      shadingModes_ensureToolStripItems(ToolStripSplitButton_Rendering, NUM_OF_SHADING_MODES_MIN, 3);
      shadingModes_evaluateCheckMark(ToolStripSplitButton_Rendering.DropDownItems);
    }

    private void shadingModes_evaluateCheckMark(ToolStripItemCollection items)
    {
      Debug.Assert(items != null);

      // Check status
      foreach (ToolStripItem i in items)
      {
        if (i is ShadingEffectMenuItem)
          ((ShadingEffectMenuItem)i).EvaluateCheckedStatus();
      }
    }

    private void shadingModes_ensureToolStripItems(ToolStripDropDownItem targetToolStrip, int minCount = 1, int toolStipInsertionIndex = 0)
    {
      Debug.Assert(targetToolStrip.DropDownItems != null);

      // First time - Third Item is separator
      if (targetToolStrip.DropDownItems.Count < minCount)
      {
        // Insert normal shading mode
        ShadingEffectMenuItem item = new ShadingEffectMenuItem(
          ShadingEffectMenuItem.NORMAL_SHADING_STRING, ShadingEffectMenuItem.NORMAL_SHADING_INDEX, targetToolStrip);

        item.BeforeChangingShadingMode += normalShadingItem_BeforeChangingShadingMode;
        item.AfterChangingShadingMode += new ShadingEffectMenuItem.ShadingModeChanged(item_AfterChangingShadingMode);
        targetToolStrip.DropDownItems.Insert(toolStipInsertionIndex, item);

        // Add all rendering effects that are loaded by the engine manager for this purpose:
        VisionEngineManager em = (VisionEngineManager)EditorManager.EngineManager;
        StringCollection names = new StringCollection();
        em.GetReplacementRenderLoopEffects(names);

        // Insert shading items
        for (int i = 0; i < names.Count; i++)
        {
          ShadingEffectMenuItem shadingMenuItem = new ShadingEffectMenuItem(
            names[i], ShadingEffectMenuItem.NORMAL_SHADING_INDEX + 1 + i, targetToolStrip);
          shadingMenuItem.AfterChangingShadingMode += new ShadingEffectMenuItem.ShadingModeChanged(item_AfterChangingShadingMode);
          targetToolStrip.DropDownItems.Insert(i + toolStipInsertionIndex + 1, shadingMenuItem);
        }

        // Add a check box for particle debug rendering on/off.
        targetToolStrip.DropDownItems.Add(new ToolStripSeparator());
        VisionEngineManager visionEngineManager = (VisionEngineManager)EditorManager.EngineManager;
        ToolStripMenuItem particleDebugRendering = new ToolStripMenuItem("Particle Debug Rendering", global::Editor.Properties.Resources.view_particle_debug);
        particleDebugRendering.Click += (System.EventHandler)((sender, arg) =>
          {
            visionEngineManager.ParticleDebugRendering = !visionEngineManager.ParticleDebugRendering;
            ((ToolStripMenuItem)sender).Checked = visionEngineManager.ParticleDebugRendering;
            // Keep menu open.
            if (targetToolStrip.GetType() == typeof(ToolStripSplitButton))
            {
              EditorManager.ActiveView.UpdateView(true);
              targetToolStrip.ShowDropDown();
            }
          });
        particleDebugRendering.ImageScaling = ToolStripItemImageScaling.None;
        particleDebugRendering.Checked = visionEngineManager.ParticleDebugRendering;
        targetToolStrip.DropDownItems.Add(particleDebugRendering);
      }
    }

    void item_AfterChangingShadingMode(EnginePanel.ShadingModeChangedEventArgs e)
    {
      // After manually changing shading mode, revert possibly set flag that a shading mode should be restored
      // after a manual shading mode change
      _restorePerspectiveNormalShadingMode = false;
    }

    void normalShadingItem_BeforeChangingShadingMode(EnginePanel.ShadingModeChangedEventArgs e)
    {
      if ( (EditorManager.Scene != null &&  EditorManager.Scene.RendererNodeClass == IRendererNodeManager.RENDERERNODECLASS_DEFERRED) &&
           (EditorManager.ActiveView.ProjectionMode != VisionViewBase.ProjectionMode_e.Perspective))
      {
        //switch back to perspective because we do not support deferred + orthogonal
        EditorManager.ActiveView.ProjectionMode = VisionViewBase.ProjectionMode_e.Perspective;
      }
    }

    #endregion

    #region Engine View Toolbar - Debug Flags Menu

    private void ToolStripSplitButton_DebugFlags_ButtonClick(object sender, EventArgs e)
    {
      // Fill list in case it is empty
      if (ToolStripSplitButton_DebugFlags.DropDownItems.Count <= 1)
        ToolStripSplitButton_DebugFlags_DropDownOpening(sender, e);

      // Get reset item
      DebugFlagResetMenuItem item = (DebugFlagResetMenuItem)ToolStripSplitButton_DebugFlags.DropDownItems[0];

      // Nothing set, then enable all
      if (EditorManager.EngineManager.DebugRenderFlags == 0 && item.DebugFlags == 0)
      {
        System.Array flagvalues = Enum.GetValues(typeof(DebugRenderFlags_e));
        foreach (DebugRenderFlags_e flag in flagvalues)
          EditorManager.EngineManager.DebugRenderFlags ^= flag;
        item.DebugFlags = EditorManager.EngineManager.DebugRenderFlags;
      }
      else if (EditorManager.EngineManager.DebugRenderFlags != 0)
      {
        item.DebugFlags = EditorManager.EngineManager.DebugRenderFlags;
        EditorManager.EngineManager.DebugRenderFlags = 0;
      }
      else if (item.DebugFlags != 0)
      {
        EditorManager.EngineManager.DebugRenderFlags = item.DebugFlags;
      }

      visionView.UpdateView(true);
    }

    private void ToolStripSplitButton_DebugFlags_DropDownOpening(object sender, EventArgs e)
    {
      // Add Debug render flags
      {
        ToolStripItemCollection debugItemsToolbar = ToolStripSplitButton_DebugFlags.DropDownItems;

        // First time - generate the items
        if (debugItemsToolbar.Count <= 1)
        {
          // Add new entry to clear all debug flags
          debugItemsToolbar.Clear();
          debugItemsToolbar.Add(new DebugFlagResetMenuItem(ToolStripSplitButton_DebugFlags));
          debugItemsToolbar.Add(new ToolStripSeparator());

          // Add all engine debug flags
          System.Array flagvalues = Enum.GetValues(typeof(DebugRenderFlags_e));
          foreach (DebugRenderFlags_e flag in flagvalues)
          {
            DebugFlagMenuItem debugFlagMenuItem = new DebugFlagMenuItem(flag, ToolStripSplitButton_DebugFlags);
            debugItemsToolbar.Add(debugFlagMenuItem);
          }
        }

        // Check status
        foreach (ToolStripItem item in debugItemsToolbar)
        {
          if (item is DebugFlagMenuItem)
            (item as DebugFlagMenuItem).EvaluateCheckedStatus();
        }
      }
    }

    #endregion

    #region Engine View Toolbar - UI Unit Scale Mode

    /// <summary>
    /// Custom menu item that stores the UI scale value
    /// </summary>
    internal class UIUnitScaleMenuItem : ToolStripMenuItem
    {
      public float UIScaleValue;
    }

    private void EnginePanel_Load(object sender, EventArgs e)
    {
      // Fill UI unit scale menu
      BuildUIUnitScaleMenu();

      // Fill Engine View size mode menu
      BuildEngineViewSizeMenu();

      // File the Engine View safe mode menu
      BuildEngineViewSafeModeMenu();
    }

    // Set camera movement rate to an arbitrary value, and set UI indicators to match
    void SetMoveSensitivity(float sensitivty)
    {
      // Set the UI unit scale 
      EditorManager.Settings.MoveSensitivity = sensitivty;

      // Mark item as checked
      UIUnitScaleMenuItem bestScaleMenuItem = null;

      // Find and add checkmark to the movement icon in the tool strip whose rate is 
      // closest to the current movement scale
      foreach (ToolStripItem i in ToolStripSplitButton_MoveSensitivity.DropDownItems)
      {
        UIUnitScaleMenuItem scaleItem = i as UIUnitScaleMenuItem;
        if (scaleItem != null)
        {
          scaleItem.Checked = false;

          if (bestScaleMenuItem == null
           || (bestScaleMenuItem.UIScaleValue < scaleItem.UIScaleValue
            && scaleItem.UIScaleValue <= sensitivty))
          {
            bestScaleMenuItem = scaleItem;
          }
        }
      }

      if (bestScaleMenuItem != null)
        bestScaleMenuItem.Checked = true;

      // Select move sensitivity icon that most closely matches the new move sensititity
      if (sensitivty <= 5.0f)
      {
        ToolStripSplitButton_MoveSensitivity.Image = global::Editor.Properties.Resources.toolbar_very_slow_camera;
      }
      else if (EditorManager.Settings.MoveSensitivity <= 50.0f)
      {
        ToolStripSplitButton_MoveSensitivity.Image = global::Editor.Properties.Resources.toolbar_slow_camera;
      }
      else if (EditorManager.Settings.MoveSensitivity <= 100.0f)
      {
        ToolStripSplitButton_MoveSensitivity.Image = global::Editor.Properties.Resources.toolbar_normal_camera;
      }
      else if (EditorManager.Settings.MoveSensitivity <= 1000.0f)
      {
        ToolStripSplitButton_MoveSensitivity.Image = global::Editor.Properties.Resources.toolbar_fast_camera;
      }
      else //if (EditorManager.Settings.MoveSensitivity > 1000.0f)
      {
        ToolStripSplitButton_MoveSensitivity.Image = global::Editor.Properties.Resources.toolbar_very_fast_camera;
      }
    }

    void MoveSensitivityModeItem_Click(object sender, EventArgs e)
    {
      UIUnitScaleMenuItem item = sender as UIUnitScaleMenuItem;
      if (item == null)
        return;

      SetMoveSensitivity(item.UIScaleValue);
    }

    void RotateSensitivityModeItem_Click(object sender, EventArgs e)
    {
      UIUnitScaleMenuItem item = sender as UIUnitScaleMenuItem;
      if (item == null)
        return;

      // Mark item as checked
      foreach (ToolStripItem i in ToolStripSplitButton_MoveSensitivity.DropDownItems)
        if ((i is ToolStripMenuItem) && (i as ToolStripMenuItem).Text.Contains("Rotate"))
          (i as ToolStripMenuItem).Checked = false;
      item.Checked = true;

      // Set the UI unit scale 
      EditorManager.Settings.RotateSensitivity = item.UIScaleValue;
    }

    private void ToolStripSplitButton_UIUnitScale_ButtonClick(object sender, EventArgs e)
    {
      ToolStripSplitButton item = sender as ToolStripSplitButton;
      if (item == null)
        return;

      // Increment/decrement move sensitivity (with looping)
      float newSensitivity;
      if (EditorManager.Settings.MoveSensitivity <= 5.0f)
      {
        if (ModifierKeys == Keys.Shift)
          newSensitivity = 5000.0f;
        else newSensitivity = 50.0f;
      }
      else if (EditorManager.Settings.MoveSensitivity <= 50.0f)
      {
        if (ModifierKeys == Keys.Shift)
          newSensitivity = 5.0f;
        else newSensitivity = 100.0f;
      }
      else if (EditorManager.Settings.MoveSensitivity <= 100.0f)
      {
        if (ModifierKeys == Keys.Shift)
          newSensitivity = 50.0f;
        else newSensitivity = 1000.0f;
      }
      else if (EditorManager.Settings.MoveSensitivity <= 1000.0f)
      {
        if (ModifierKeys == Keys.Shift)
          newSensitivity = 100.0f;
        else newSensitivity = 5000.0f;
      }
      else //if (EditorManager.Settings.MoveSensitivity > 1000.0f)
      {
        if (ModifierKeys == Keys.Shift)
          newSensitivity = 1000.0f;
        else newSensitivity = 5.0f;
      }
      SetMoveSensitivity(newSensitivity);
    }

    private void BuildUIUnitScaleMenu()
    {
      // Add camera move sensitivity mode buttons
      EditorSettingsBase.UIUnitScaleMode[] modes = EditorManager.Settings.UIUnitScaleModes;
      for (int i = 0; i < modes.Length; i++)
      {
        EditorSettingsBase.UIUnitScaleMode mode = modes[i];
        UIUnitScaleMenuItem item = new UIUnitScaleMenuItem();
        item.Text = "Move: " + mode.ModeName;
        item.UIScaleValue = mode.ModeValue;
        item.Image = UnitScaleImageList.Images[i];
        item.Size = new Size(16, 16);
        item.ImageScaling = ToolStripItemImageScaling.None;
        item.Click += new EventHandler(MoveSensitivityModeItem_Click);
        ToolStripSplitButton_MoveSensitivity.DropDownItems.Add(item);
      }

      // Add camera rotate sensitivity mode buttons
      ToolStripSplitButton_MoveSensitivity.DropDownItems.Add(new ToolStripSeparator());
      for (int i = 0; i < modes.Length; i++)
      {
        EditorSettingsBase.UIUnitScaleMode mode = modes[i];
        UIUnitScaleMenuItem item = new UIUnitScaleMenuItem();
        item.Text = "Rotate: " + mode.ModeName;
        item.UIScaleValue = mode.ModeValue;
        item.Image = UnitScaleImageList.Images[i];
        item.Size = new Size(16, 16);
        item.ImageScaling = ToolStripItemImageScaling.None;
        item.Click += new EventHandler(RotateSensitivityModeItem_Click);
        ToolStripSplitButton_MoveSensitivity.DropDownItems.Add(item);
      }
      SetMoveSensitivity(EditorManager.Settings.MoveSensitivity);
    }

    #endregion

    #region Engine View Toolbar - EngineView Size

    /// <summary>
    /// Custom menu item that stores the UI scale value
    /// </summary>
    internal class EngineViewSizeMenuItem : ToolStripMenuItem
    {
      public Size EngineViewSize;
    }

    private void BuildEngineViewSizeMenu()
    {
      EditorSettingsBase.EngineViewSizeMode[] modes = EditorManager.Settings.EngineViewSizeModes;
      for (int i = 0; i < modes.Length; i++)
      {
        EditorSettingsBase.EngineViewSizeMode mode = modes[i];
        EngineViewSizeMenuItem item = new EngineViewSizeMenuItem();
        item.Text = mode.ModeName;
        item.EngineViewSize = mode.ModeValue;
        item.Size = new Size(16, 16);
        item.ImageScaling = ToolStripItemImageScaling.None;
        item.Click += new EventHandler(EngineViewSizeModeItem_Click);
        ToolStripMenuItem_View_Size.DropDownItems.Add(item);
      }
    }

    void EngineViewSizeModeItem_Click(object sender, EventArgs e)
    {
      EngineViewSizeMenuItem item = sender as EngineViewSizeMenuItem;
      if (item == null)
        return;

      // Update Engine View resolution
      Size viewSize = item.EngineViewSize;
      viewSize.Width += 16;
      viewSize.Height += 65;
      Parent.Parent.Size = viewSize;
    }

    private void ToolStripMenuItem_View_SetSize_Click(object sender, EventArgs e)
    {
      // Open dialog
      EngineViewSizeDlg dlg = new EngineViewSizeDlg();
      Size viewSize = Parent.Parent.Size;
      viewSize.Width -= 16;
      viewSize.Height -= 65;
      dlg.EngineViewSize = viewSize;

      if (dlg.ShowDialog(this) == DialogResult.OK)
      {
        viewSize = dlg.EngineViewSize;
        viewSize.Width += 16;
        viewSize.Height += 65;
        Parent.Parent.Size = viewSize;
      }
    }

    #endregion

    #region Engine View Toolbar - EngineView Safe Modes

    /// <summary>
    /// Custom menu item that stores the UI scale value
    /// </summary>
    internal class EngineViewSafeModeMenuItem : ToolStripMenuItem
    {
      public SizeF EngineViewSafeMode;
    }

    private void BuildEngineViewSafeModeMenu()
    {
      EditorSettingsBase.EngineViewSafeMode[] modes = EditorManager.Settings.EngineViewSafeModes;
      for (int i = 0; i < modes.Length; i++)
      {
        EditorSettingsBase.EngineViewSafeMode mode = modes[i];
        EngineViewSafeModeMenuItem item = new EngineViewSafeModeMenuItem();
        item.Text = mode.ModeName;
        item.EngineViewSafeMode = mode.ModeValue;
        item.Size = new Size(16, 16);
        item.ImageScaling = ToolStripItemImageScaling.None;
        item.Click += new EventHandler(EngineViewSafeModeItem_Click);
        ToolStripMenuItem_View_SafeMode.DropDownItems.Add(item);
      }
    }

    void EngineViewSafeModeItem_Click(object sender, EventArgs e)
    {
      EngineViewSafeModeMenuItem item = sender as EngineViewSafeModeMenuItem;
      if (item == null)
        return;

      // Update Engine View Safe Mode
      SizeF safeMode = item.EngineViewSafeMode;
      EditorManager.EngineManager.SetEngineViewSafeMode(safeMode.Width, safeMode.Height, true);
      EditorManager.ActiveView.UpdateView(false);
    }

    private void ToolStripMenuItem_View_SetSafeMode_Click(object sender, EventArgs e)
    {
      // Open dialog
      EngineViewSafeModeDlg dlg = new EngineViewSafeModeDlg();
      dlg.EngineViewSafeModeSize = new SizeF(1.0f, 1.0f);

      if (dlg.ShowDialog(this) == DialogResult.OK)
      {
        // Update Engine View Safe Mode
        SizeF safeMode = dlg.EngineViewSafeModeSize;
        EditorManager.EngineManager.SetEngineViewSafeMode(safeMode.Width, safeMode.Height, true);
        EditorManager.ActiveView.UpdateView(false);
      }
    }

    private void disableToolStripMenuItem_Click(object sender, EventArgs e)
    {
      EditorManager.EngineManager.SetEngineViewSafeMode(0.0f, 0.0f, false);
      EditorManager.ActiveView.UpdateView(false);
    }

    #endregion

    #region Engine View Toolbar - Camera

    private void ToolStripSplitButton_Camera_DropDownOpening(object sender, EventArgs e)
    {
      ToolStripSplitButton_CameraStyle_Pan.Checked = false;
      ToolStripSplitButton_CameraStyle_PanHoriz.Checked = false;
      ToolStripSplitButton_CameraStyle_Orbit.Checked = false;
      ToolStripSplitButton_CameraStyle_Max.Checked = false;
      ToolStripSplitButton_CameraStyle_Maya.Checked = false;

      switch (EditorManager.Settings.CameraStyle)
      {
        case EditorSettingsBase.CameraStyle_e.MiddleMousePan: ToolStripSplitButton_CameraStyle_Pan.Checked = true;
          break;
        case EditorSettingsBase.CameraStyle_e.MiddleMousePanHorizontal: ToolStripSplitButton_CameraStyle_PanHoriz.Checked = true;
          break;
        case EditorSettingsBase.CameraStyle_e.MiddleMouseOrbitSelection: ToolStripSplitButton_CameraStyle_Orbit.Checked = true;
          break;
        case EditorSettingsBase.CameraStyle_e.MaxStyle: ToolStripSplitButton_CameraStyle_Max.Checked = true;
          break;
        case EditorSettingsBase.CameraStyle_e.MayaStyle: ToolStripSplitButton_CameraStyle_Maya.Checked = true;
          break;
        default: ToolStripSplitButton_CameraStyle_Pan.Checked = true;
          break;
      }
    }

    private void ToolStripSplitButton_CameraStyle_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem button = sender as ToolStripMenuItem;
      if (button == null)
        return;

      ToolStripSplitButton_CameraStyle_Pan.Checked = false;
      ToolStripSplitButton_CameraStyle_PanHoriz.Checked = false;
      ToolStripSplitButton_CameraStyle_Orbit.Checked = false;
      ToolStripSplitButton_CameraStyle_Max.Checked = false;
      ToolStripSplitButton_CameraStyle_Maya.Checked = false;
      button.Checked = true;

      if (button == ToolStripSplitButton_CameraStyle_Pan)
        EditorManager.Settings.CameraStyle = EditorSettingsBase.CameraStyle_e.MiddleMousePan;
      else if (button == ToolStripSplitButton_CameraStyle_PanHoriz)
        EditorManager.Settings.CameraStyle = EditorSettingsBase.CameraStyle_e.MiddleMousePanHorizontal;
      else if (button == ToolStripSplitButton_CameraStyle_Orbit)
        EditorManager.Settings.CameraStyle = EditorSettingsBase.CameraStyle_e.MiddleMouseOrbitSelection;
      else if (button == ToolStripSplitButton_CameraStyle_Max)
        EditorManager.Settings.CameraStyle = EditorSettingsBase.CameraStyle_e.MaxStyle;
      else if (button == ToolStripSplitButton_CameraStyle_Maya)
        EditorManager.Settings.CameraStyle = EditorSettingsBase.CameraStyle_e.MayaStyle;
    }

    #endregion

    #region Engine View Toolbar - Tools

    private void ToolStripSplitButton_Tools_DropDownOpening(object sender, EventArgs e)
    {
      ToolStripButton_HighlightSelection.Checked = EditorManager.Settings.SelectionHighlight;
      ToolStripButton_OneClickSelection.Checked = EditorManager.Settings.OneClickSelection;
    }

    private void ToolStripSplitButton_HighlightSelection_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.SelectionHighlight = ToolStripButton_HighlightSelection.Checked;
      EditorManager.ActiveView.UpdateView(false);
    }

    private void ToolStripButton_OneClickSelection_Click(object sender, EventArgs e)
    {
      EditorManager.Settings.OneClickSelection = ToolStripButton_OneClickSelection.Checked;
    }

    #endregion

    #region Engine View Toolbar - Selection Filter

    private void ToolStripSplitButton_SelectionFilter_DropDownOpening(object sender, EventArgs e)
    {
      // Check settings
      ToolStripSplitButton_SelectionFilterUse.Checked = EditorManager.ShapeSelectionFilter.Enabled;
      ToolStripSplitButton_SelectionFilterOtherShapes.Checked = EditorManager.ShapeSelectionFilter.OtherShapesPickable;

      // Get shape type filters and their state
      while (ToolStripSplitButton_SelectionFilter.DropDownItems.Count > 3)
        ToolStripSplitButton_SelectionFilter.DropDownItems.RemoveAt(3);

      foreach (IShapeFilter filter in EditorManager.ShapeSelectionFilter)
      {
        ToolStripMenuItem item = new ToolStripMenuItem(filter.DisplayName);
        item.Tag = filter;
        item.Checked = filter.Active;
        item.CheckOnClick = true;
        item.ImageScaling = ToolStripItemImageScaling.None;
        item.Click += new EventHandler(ToolStripSplitButtonItem_Click);
        ToolStripSplitButton_SelectionFilter.DropDownItems.Add(item);
      }
    }

    void ToolStripSplitButtonItem_Click(object sender, EventArgs e)
    {
      ToolStripMenuItem item = sender as ToolStripMenuItem;
      if (item == null)
        return;

      IShapeFilter changedFilter = item.Tag as IShapeFilter;
      if (changedFilter == null)
        return;

      changedFilter.Active = item.Checked;

      // Save selection filter setup
      EditorManager.SaveShapeSelectionFilterSetup();
    }

    private void ToolStripSplitButton_SelectionFilter_Click(object sender, EventArgs e)
    {
      ShapeSelectionFilterDlg dlg = new ShapeSelectionFilterDlg();
      dlg.FilterSetup = (ShapeSelectionFilterSetup)EditorManager.ShapeSelectionFilter.Clone();
      if (dlg.ShowDialog() != DialogResult.OK)
        return;

      EditorManager.ShapeSelectionFilter = dlg.FilterSetup;
    }

    private void ToolStripSplitButton_SelectionFilterUse_Click(object sender, EventArgs e)
    {
      EditorManager.ShapeSelectionFilter.Enabled = ToolStripSplitButton_SelectionFilterUse.Checked;
    }

    private void ToolStripSplitButton_SelectionFilterOtherShapes_Click(object sender, EventArgs e)
    {
      EditorManager.ShapeSelectionFilter.OtherShapesPickable = ToolStripSplitButton_SelectionFilterOtherShapes.Checked;
    }

    #endregion

    #endregion

    #region Listeners

    /// <summary>
    /// Respond to play mode changed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_EditorModeChanged(object sender, EditorModeChangedArgs e)
    {
      // update the play button (play/stop button image)
      if (e.action == EditorModeChangedArgs.Action.ModeChanged)
      {
        switch (e._newMode)
        {
          case EditorManager.Mode.EM_NONE:
            {
              if (_lastActivePlayMode == EditorManager.Mode.EM_ANIMATING)
                toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_play_animate;
              else if (_lastActivePlayMode == EditorManager.Mode.EM_PLAYING_IN_EDITOR)
                toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_play_editor;
              else if (_lastActivePlayMode == EditorManager.Mode.EM_PLAYING_IN_GAME)
                toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_play_game;

              toolStripDropDownButton_PlayMode.DropDown = _play_Dropdown;
              View.Dock = System.Windows.Forms.DockStyle.Fill;
            }
            break;
          case EditorManager.Mode.EM_ANIMATING:
            {
              toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_stop_animate;
              toolStripDropDownButton_PlayMode.DropDown = null;
            }
            break;
          case EditorManager.Mode.EM_PLAYING_IN_EDITOR:
            {
              toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_stop_editor;
              toolStripDropDownButton_PlayMode.DropDown = null;
            }
            break;
          case EditorManager.Mode.EM_PLAYING_IN_GAME:
            {
              toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_stop_game;
              toolStripDropDownButton_PlayMode.DropDown = null;
            }
            break;
        }
      }
    }

    private void OnMouseContextChanged(object sender, ContextChangedArgs e)
    {
      Type t = e.newContext.GetType();
      // we have to do exact match (the 'is' statement is not enough)
      toolStripButton_MoveContext.CheckState = t.Equals(typeof(CameraMoveContext)) ? CheckState.Checked : CheckState.Unchecked;
      toolStripButton_MeasureTool.CheckState = (e.newContext is MeasureToolCamera) ? CheckState.Checked : CheckState.Unchecked;
    }

    private void OnGizmoChanged(object sender, GizmoChangedArgs e)
    {
      switch (e.action)
      {
        case GizmoChangedArgs.Action.ModeChanged:
          ToolStripButton_Move.Checked = (e.newMode == ShapeDragMode.MOVE) ? CheckState.Checked : CheckState.Unchecked;
          ToolStripButton_Rotate.Checked = (e.newMode == ShapeDragMode.ROTATE) ? CheckState.Checked : CheckState.Unchecked;
          ToolStripButton_Scale.Checked = e.newMode == ShapeDragMode.SCALE;
          ToolStripButton_Link.Checked = (e.newMode == ShapeDragMode.LINK) ? CheckState.Checked : CheckState.Unchecked;
          break;
        case GizmoChangedArgs.Action.LocalOrientationChanged:
          toolStripButton_localOri.CheckState = e.bNewLocalOrientation ? CheckState.Checked : CheckState.Unchecked;
          break;
        case GizmoChangedArgs.Action.LocalTranslationChanged:
          toolStripButton_localPos.CheckState = e.bNewLocalTranslation ? CheckState.Checked : CheckState.Unchecked;
          break;
        case GizmoChangedArgs.Action.TranslationModeChanged:
          toolStripButton_StickToGround.Checked = e.newTranslationMode == CSharpFramework.GizmoBase.TranslationMode.ObjectsStickToGround;
          toolStripButton_VertexSnapping.Checked = e.newTranslationMode == CSharpFramework.GizmoBase.TranslationMode.SnapToVertices;
          break;
      }

    }

    /// <summary>
    /// Respond to the scene change event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void EditorManager_SceneChanged(object sender, SceneChangedArgs e)
    {
      // Disable isolate selection
      ToolStripButton_IsolateSelection.Checked = false;
      //stop playmode
      EditorManager.EditorMode = EditorManager.Mode.EM_NONE;
      UpdateToolbarStatus();
    }

    void EditorScene_LayerChanged(object sender, LayerChangedArgs e)
    {
      // Exit isolate selection mode when changing the visibility status of a layer
      if (e.action == LayerChangedArgs.Action.VisibleChanged)
      {
        ToolStripButton_IsolateSelection.Checked = false;
        visionView.IsolateSelection(false, false);
      }

      // Process to engine view
      visionView.OnLayerChanged(sender, e);
    }

    void VisionViewBase_ProjectionModeChanged(object sender, EventArgs e)
    {
      // If we change to perspective mode, check if flag was set to restore normal shading mode
      if (EditorManager.ActiveView.ProjectionMode == VisionViewBase.ProjectionMode_e.Perspective &&
        _restorePerspectiveNormalShadingMode)
      {
        VisionEngineManager engineManager = (VisionEngineManager)EditorManager.EngineManager;
        engineManager.SetReplacementRenderLoopEffect(ShadingEffectMenuItem.NORMAL_SHADING_INDEX);
        _restorePerspectiveNormalShadingMode = false;
      }
      UpdateToolbarStatus();
    }


    private void EditorManager_ShapeSelectionChanged(object sender, ShapeSelectionChangedArgs e)
    {
      ToolStripButton_IsolateSelection.Enabled = (ToolStripButton_IsolateSelection.Checked || EditorManager.SelectedShapes.Count > 0);
      ToolStripButton_ZoomFit.Image = (EditorManager.SelectedShapes.Count > 0) ? Properties.Resources.frame_selected : Properties.Resources.frame_all;
      ToolStripButton_ZoomFit.ToolTipText = (EditorManager.SelectedShapes.Count > 0) ? "Focus On Selection" : "Focus Scene";

      updateTransformStrip();

      // Update focus if enabled
      if (EditorManager.Settings.FocusOnSelection)
        FocusOnSelection();

      EditorManager.ActiveView.UpdateView(false);
    }

    public void OnPropertyChanged(object sender, PropertyChangedArgs e)
    {
      updateTransformStrip();
    }

    #endregion

    private void ToolStripButton_Scale_Click_1(object sender, EventArgs e)
    {
      EditorApp.ActiveView.Gizmo.DragMode = ShapeDragMode.SCALE;
    }

    private void propertiesToolStripMenuItem_Click(object sender, EventArgs e)
    {
      PropertyPanel pp = (EditorManager.MainForm as Form1).PropertyEditorPanel;
      pp.Show();
      pp.Focus();
    }

    #region Private Members

    private IContainer components;
    ToolStripHelpButton _helpbutton = null;

    // Flag to mark if the normal shading mode should be restored when changing
    // to perspective mode (again). This is used after switching to orthogonal
    // mode in deferred rendering, which changes the shading mode.
    private bool _restorePerspectiveNormalShadingMode = false;

    // Toggle whether the transform strip opperates in relative or absolute mode.
    bool relativeTransform = false;
    private void transformStrip_toggleTransformMode_Click(object sender, EventArgs e)
    {
      relativeTransform = ((ToolStripButton)sender).Checked;

      ToolStripButton senderButton = (ToolStripButton)sender;
      if (relativeTransform)
      {
        senderButton.Image = Editor.Properties.Resources.toolbar_transform_relative;
      }
      else
      {
        senderButton.Image = Editor.Properties.Resources.toolbar_transform_absolute;
      }

      updateTransformStrip();
    }

    private void transformStrip_KeyDown(object sender, KeyEventArgs e)
    {
      // Do not send Escape/Delete to editor event key manager, as they cause incompatible behavior
      if (e.KeyCode != Keys.Delete
       && e.KeyCode != Keys.Escape)
      {
        EditorManager.EditorComponentKeyDown(null, e);
      }

      // Enter/Space/Tab commits the current entry
      if (e.KeyCode == Keys.Enter 
       || e.KeyCode == Keys.Space 
       || e.KeyCode == Keys.Tab)
      {
        if (relativeTransform && e.KeyCode == Keys.Enter)
        {
          this.transformStrip_Leave(sender, e);

          ToolStripTextBox textBox = sender as ToolStripTextBox;

          if (textBox != null)
            textBox.SelectAll();
        }
        else
        {
          this.SelectNextControl(this.ActiveControl, true, true, true, true);
        }
      }
    }

    // Filters out letter & symbol keys that are not used in numbers
    private void transformStripTextBox_KeyPress(object sender, KeyPressEventArgs e)
    {
      if (!char.IsDigit(e.KeyChar) && e.KeyChar != '.' && e.KeyChar != '-' && e.KeyChar != ',' && e.KeyChar != '\b')
      {
        e.Handled = true;
      }
    }

	// Select all textbox text when clicked
    private void transformStripTextBox_Click(object sender, EventArgs e)
    {
      ToolStripTextBox textBox = sender as ToolStripTextBox;

      if (textBox != null)
        textBox.SelectAll();
    }

    private void transformStripTextBox_Cancel(object sender, EventArgs e)
    {
      updateTransformStrip();

      ToolStripTextBox textBox = sender as ToolStripTextBox;

      if (textBox != null)
        textBox.SelectAll();
    }

    // When focus leaves a transform strip element, apply the modification to all selected shapes
    private void transformStrip_Leave(object sender, EventArgs e)
    {
      
      ToolStripTextBox textBox = sender as ToolStripTextBox;

      if (textBox != null)
      {
        try
        {
          // Fetch text from current text box and determine field based on textbox id
          float value = Single.Parse(textBox.Text);
          String property = "";
          if (textBox == transformStrip_TranslateX)
            property = "x";
          else if (textBox == transformStrip_TranslateY)
            property = "y";
          else if (textBox == transformStrip_TranslateZ)
            property = "z";
          else if (textBox == transformStrip_Yaw)
            property = "Yaw";
          else if (textBox == transformStrip_Pitch)
            property = "Pitch";
          else if (textBox == transformStrip_Roll)
            property = "Roll";
          else if (textBox == transformStrip_ScaleX)
            property = "ScaleX";
          else if (textBox == transformStrip_ScaleY)
            property = "ScaleY";
          else if (textBox == transformStrip_ScaleZ)
            property = "ScaleZ";
          else if (textBox == transformStrip_ScaleXYZ)
            property = "UniformScaling";

          // Create a group action to allow for group undo/redo
          GroupAction groupAction = new GroupAction("Transform Selected");

          // Create property actions for each selected shape
          foreach (ShapeBase shape in EditorManager.SelectedShapes)
          {
            Shape3D shape3D = shape as Shape3D;

            if (shape3D != null)
            {
              float newValue = value;
              float orgValue = 0;
              bool bAdd = true;

              if (textBox == transformStrip_TranslateX)
                orgValue = shape3D.Position.X;
              else if (textBox == transformStrip_TranslateY)
                orgValue = shape3D.Position.Y;
              else if (textBox == transformStrip_TranslateZ)
                orgValue = shape3D.Position.Z;
              else if (textBox == transformStrip_Yaw)
                orgValue = shape3D.Orientation.X;
              else if (textBox == transformStrip_Pitch)
                orgValue = shape3D.Orientation.Y;
              else if (textBox == transformStrip_Roll)
                orgValue = shape3D.Orientation.Z;
              else if (textBox == transformStrip_ScaleX)
              {
                bAdd = false;
                orgValue = shape3D.ScaleX;
              }
              else if (textBox == transformStrip_ScaleY)
              {
                bAdd = false;
                orgValue = shape3D.ScaleY;
              }
              else if (textBox == transformStrip_ScaleZ)
              {
                bAdd = false;
                orgValue = shape3D.ScaleZ;
              }
              else if (textBox == transformStrip_ScaleXYZ)
              {
                bAdd = false;
                orgValue = shape3D.UniformScaling;
              }

              // When using relative transforms, apply increment to current value
              if (relativeTransform)
              {
                if (bAdd)
                  newValue += orgValue;
                else
                  newValue *= orgValue;
              }

              // do not create an undo step, unless the property actually changed
              if (newValue != orgValue)
                groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, property, newValue));
            }
          }

          EditorManager.Actions.Add(groupAction);
        }
        catch (System.FormatException)
        {
          updateTransformStrip();
          return;
        }
        catch (System.OverflowException)
        {
          updateTransformStrip();
          return;
        }
      }
    }

    // Updates the visibility and enabled fields of transform strip elements based on current state and selection
    public void updateTransformStrip()
    {
      // Base visibility on editor settings
      // changing this setting is expensive, as WinForms does not make an early out, so we have to check this ourselves
      if (transformStrip_EnginePanel.Visible != EditorManager.Settings.ShowTransformStrip)
        transformStrip_EnginePanel.Visible = EditorManager.Settings.ShowTransformStrip;

      if (!transformStrip_EnginePanel.Visible)
        return;

	    // Early-out when a clone operation is detected, as clone operations perform incremental add operations for
	    // each instance, leading to O(n^2) performance
      CameraMoveContext context = EditorManager.ActiveView.CurrentContext as CameraMoveContext;

      if ( context == null )
          return;

      if (context.CloneSize > 0 && EditorManager.SelectedShapes.Count < context.CloneSize)
        return;

      // Enable transform strip and update if there are selected shapes
      if (EditorManager.SelectedShapes.Count > 0)
      {
        transformStrip_EnginePanel.Enabled = true;

        bool bMultiSelect = (EditorManager.SelectedShapes.Count > 1);

        // Copying transforms is poorly defined for multiple selections, so disable this option
        toolStripMenuItem_CopyTranslation.Enabled = !bMultiSelect;
        toolStripMenuItem_CopyRotation.Enabled = !bMultiSelect;
        toolStripMenuItem_CopyScale.Enabled = !bMultiSelect;


        Shape3D firstSelectedShape = (Shape3D)EditorManager.SelectedShapes[0];
        if (firstSelectedShape != null)
        {
          // For relative transforms, clear strip to "identity" values
          if (relativeTransform)
          {
            transformStrip_TranslateX.Text = "0";
            transformStrip_TranslateY.Text = "0";
            transformStrip_TranslateZ.Text = "0";

            transformStrip_Yaw.Text = "0";
            transformStrip_Pitch.Text = "0";
            transformStrip_Roll.Text = "0";

            transformStrip_ScaleX.Text = "1";
            transformStrip_ScaleY.Text = "1";
            transformStrip_ScaleZ.Text = "1";
            transformStrip_ScaleXYZ.Text = "1";
          }
          // For absolute transforms, update with selected shape transforms
          else
          {
            transformStrip_TranslateX.Text = firstSelectedShape.Position.X.ToString();
            transformStrip_TranslateY.Text = firstSelectedShape.Position.Y.ToString();
            transformStrip_TranslateZ.Text = firstSelectedShape.Position.Z.ToString();

            transformStrip_Yaw.Text = firstSelectedShape.Orientation.X.ToString();
            transformStrip_Pitch.Text = firstSelectedShape.Orientation.Y.ToString();
            transformStrip_Roll.Text = firstSelectedShape.Orientation.Z.ToString();

            transformStrip_ScaleX.Text = firstSelectedShape.ScaleX.ToString();
            transformStrip_ScaleY.Text = firstSelectedShape.ScaleY.ToString();
            transformStrip_ScaleZ.Text = firstSelectedShape.ScaleZ.ToString();
            transformStrip_ScaleXYZ.Text = firstSelectedShape.UniformScaling.ToString();

            // When multiple shapes are selected, clear all transforms fields that aren't identical for all selected objects
            if (EditorManager.SelectedShapes.Count > 1)
            {
              Vector3F position = firstSelectedShape.Position;
              Vector3F orientation = firstSelectedShape.Orientation;
              Vector3F scale = new Vector3F(firstSelectedShape.ScaleX, firstSelectedShape.ScaleY, firstSelectedShape.ScaleZ);
              float uniformScale = firstSelectedShape.UniformScaling;

              foreach (ShapeBase shape in EditorManager.SelectedShapes)
              {
                Shape3D shape3D = shape as Shape3D;
                if (shape3D != null)
                {
                  if (shape3D.Position.X != position.X)
                    transformStrip_TranslateX.Text = "";
                  if (shape3D.Position.Y != position.Y)
                    transformStrip_TranslateY.Text = "";
                  if (shape3D.Position.Z != position.Z)
                    transformStrip_TranslateZ.Text = "";
                  if (shape3D.Orientation.X != orientation.X)
                    transformStrip_Yaw.Text = "";
                  if (shape3D.Orientation.Y != orientation.Y)
                    transformStrip_Pitch.Text = "";
                  if (shape3D.Orientation.Z != orientation.Z)
                    transformStrip_Roll.Text = "";
                  if (shape3D.ScaleX != scale.X)
                    transformStrip_ScaleX.Text = "";
                  if (shape3D.ScaleY != scale.Y)
                    transformStrip_ScaleY.Text = "";
                  if (shape3D.ScaleZ != scale.Z)
                    transformStrip_ScaleZ.Text = "";
                  if (shape3D.UniformScaling != uniformScale)
                    transformStrip_ScaleXYZ.Text = "";
                }
                else
                {
                  transformStrip_TranslateX.Text = "";
                  transformStrip_TranslateY.Text = "";
                  transformStrip_TranslateZ.Text = "";

                  transformStrip_Yaw.Text = "";
                  transformStrip_Pitch.Text = "";
                  transformStrip_Roll.Text = "";

                  transformStrip_ScaleX.Text = "";
                  transformStrip_ScaleY.Text = "";
                  transformStrip_ScaleZ.Text = "";
                  transformStrip_ScaleXYZ.Text = "";
                  break;
                }
              }
            }
          }
        }
      }
      // Otherwise clear all text boxes and disable the strip
      else
      {
        transformStrip_EnginePanel.Enabled = false;

        transformStrip_TranslateX.Text = "";
        transformStrip_TranslateY.Text = "";
        transformStrip_TranslateZ.Text = "";
        transformStrip_Yaw.Text = "";
        transformStrip_Pitch.Text = "";
        transformStrip_Roll.Text = "";
        transformStrip_ScaleX.Text = "";
        transformStrip_ScaleY.Text = "";
        transformStrip_ScaleZ.Text = "";
        transformStrip_ScaleXYZ.Text = "";
      }
    }

    // Copies selected shape's translation field to a special clipboard. Always uses absolute translation
    private Vector3F translationClipboard = new Vector3F(0, 0, 0);
    private void copyTranslationStripMenuItem1_Click(object sender, EventArgs e)
    {
      if (EditorManager.SelectedShapes.Count > 0)
      {
        Shape3D selectedShape = EditorManager.SelectedShapes[0] as Shape3D;
        if (selectedShape != null)
        {
          translationClipboard.X = selectedShape.Position.X;
          translationClipboard.Y = selectedShape.Position.Y;
          translationClipboard.Z = selectedShape.Position.Z;
        }
      }
    }

    // Sets the absolute translation of all selected shapes to the value of the translation clipboard
    private void pasteTranslationToolStripMenuItem_Click(object sender, EventArgs e)
    {
      transformStrip_TranslateX.Text = translationClipboard.X.ToString();
      transformStrip_TranslateY.Text = translationClipboard.Y.ToString();
      transformStrip_TranslateZ.Text = translationClipboard.Z.ToString();
      
      GroupAction groupAction = new GroupAction("Transform Selected");
      foreach (ShapeBase shape in EditorManager.SelectedShapes)
      {
        Shape3D shape3D = shape as Shape3D;
        if (shape3D != null)
        {
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "x", translationClipboard.X));
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "y", translationClipboard.Y));
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "z", translationClipboard.Z));
        }
      }
      EditorManager.Actions.Add(groupAction);
    }

    // Copies selected shape's rotation field to a special clipboard. Always uses absolute rotation
    private Vector3F rotationClipboard = new Vector3F(0, 0, 0);
    private void copyRotationToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (EditorManager.SelectedShapes.Count > 0)
      {
        Shape3D selectedShape = EditorManager.SelectedShapes[0] as Shape3D;
        if (selectedShape != null)
        {
          rotationClipboard.X = selectedShape.Yaw;
          rotationClipboard.Y = selectedShape.Pitch;
          rotationClipboard.Z = selectedShape.Roll;
        }
      }
    }

    // Sets the absolute rotation of all selected shapes to the value of the rotation clipboard
    private void pasteRotationToolStripMenuItem_Click(object sender, EventArgs e)
    {
      transformStrip_Yaw.Text = rotationClipboard.X.ToString();
      transformStrip_Pitch.Text = rotationClipboard.Y.ToString();
      transformStrip_Roll.Text = rotationClipboard.Z.ToString();

      GroupAction groupAction = new GroupAction("Transform Selected");
      foreach (ShapeBase shape in EditorManager.SelectedShapes)
      {
        Shape3D shape3D = shape as Shape3D;
        if (shape3D != null)
        {
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "Yaw", rotationClipboard.X));
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "Pitch", rotationClipboard.Y));
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "Roll", rotationClipboard.Z));
        }
      }
      EditorManager.Actions.Add(groupAction);
    }

    // Copies selected shape's scale field to a special clipboard. Always uses absolute scale
    private Vector3F scaleClipboard = new Vector3F(1, 1, 1);
    private void copyScaleToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if (EditorManager.SelectedShapes.Count > 0)
      {
        Shape3D selectedShape = EditorManager.SelectedShapes[0] as Shape3D;
        if (selectedShape != null)
        {
          scaleClipboard.X = selectedShape.ScaleX;
          scaleClipboard.Y = selectedShape.ScaleY;
          scaleClipboard.Z = selectedShape.ScaleZ;
        }
      }
    }

    // Sets the absolute scale of all selected shapes to the value of the scale clipboard
    private void pasteScaleToolStripMenuItem_Click(object sender, EventArgs e)
    {
      transformStrip_ScaleX.Text = scaleClipboard.X.ToString();
      transformStrip_ScaleY.Text = scaleClipboard.Y.ToString();
      transformStrip_ScaleZ.Text = scaleClipboard.Z.ToString();

      GroupAction groupAction = new GroupAction("Transform Selected");
      foreach (ShapeBase shape in EditorManager.SelectedShapes)
      {
        Shape3D shape3D = shape as Shape3D;
        if (shape3D != null)
        {
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "ScaleX", scaleClipboard.X));
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "ScaleY", scaleClipboard.Y));
          groupAction.Add(SetPropertyAction.CreateSetPropertyAction(shape3D, "ScaleZ", scaleClipboard.Z));
        }
      }
      EditorManager.Actions.Add(groupAction);
    }
    #endregion

    private void ToolStripButton_ZoomFit_ButtonClick(object sender, EventArgs e)
    {
      FocusOnSelection();
    }
  }
}

/*
 * Havok SDK - Base file, BUILD(#20140624)
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
