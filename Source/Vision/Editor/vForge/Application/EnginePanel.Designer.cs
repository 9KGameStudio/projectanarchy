using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using System.Collections;
using CSharpFramework;
using CSharpFramework.Contexts;
using CSharpFramework.View;
using CSharpFramework.Controls;

namespace Editor
{
	public partial class EnginePanel
	{
		#region Windows Form Designer generated code
		private ManagedFramework.VisionView visionView;
private System.Windows.Forms.MenuItem Profiling;
private System.Windows.Forms.MenuItem Profiling_Next;
private System.Windows.Forms.MenuItem Profiling_Previous;
private System.Windows.Forms.Panel panel_VisionView;
private ToolStrip toolStrip_EnginePanel;
private ToolStripSeparator toolStripSeparator3;
private ToolStripSeparator toolStripSeparator4;
private ToolStripSplitButton toolStripDropDownButton_PlayMode;
private ToolStripMenuItem animateToolStripMenuItem_Animate;
private ToolStripMenuItem runInEditorToolStripMenuItem_RunInEditor;
private ToolStripMenuItem playTheGameToolStripMenuItem_PlayTheGame;
private ContextMenuStrip contextMenuStrip_SelShapes;
private ToolStripSeparator toolStripMenuItem2;
private ToolStripMenuItem dropToFloorToolStripMenuItem;
private ToolStripMenuItem linksToolStripMenuItem;
private ToolStripMenuItem moveToToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem3;
private ToolStripMenuItem cutToolStripMenuItem;
private ToolStripMenuItem copyToolStripMenuItem;
private ToolStripMenuItem deleteToolStripMenuItem;
private ToolStripMenuItem pivotToolStripMenuItem;
private ToolStripMenuItem boundingBoxToolStripMenuItem;
private ToolStripMenuItem bottomCenterToolStripMenuItem;
private ToolStripMenuItem unlinkFromSourcesToolStripMenuItem;
private ToolStripMenuItem unlinkAllTargetsToolStripMenuItem;
private ToolStripMenuItem cameraPositionToolStripMenuItem;
private ToolStripMenuItem shapeOriginToolStripMenuItem;
private ContextMenuStrip contextMenuStrip_View;
private ToolStripMenuItem pasteAtCursorToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem4;
private ToolStripMenuItem gridToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem5;
private ToolStripMenuItem renderingToolStripMenuItem;
private ToolStripMenuItem shadingToolStripMenuItem;
private ToolStripMenuItem debugFlagsToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem6;
private ToolStripMenuItem shapeOriginToolStripMenuItem1;
private ToolStripMenuItem createToolStripMenuItem;
private ToolStripMenuItem insertHereToolStripMenuItem;
private ToolStripMenuItem snapPositionToolStripMenuItem;
private ToolStripMenuItem snapRotationToolStripMenuItem;
private ToolStripMenuItem showGridToolStripMenuItem;
private ToolStripMenuItem relativeGridToolStripMenuItem;
private ToolStripMenuItem gridLinesToolStripMenuItem;
private ToolStripMenuItem volumeGridToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem7;
private ToolStripMenuItem gridSize10ToolStripMenuItem;
private ToolStripMenuItem gridSize25ToolStripMenuItem;
private ToolStripMenuItem gridSize100ToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem15;
private ToolStripMenuItem units2ToolStripMenuItem;
private ToolStripMenuItem units5ToolStripMenuItem;
private ToolStripMenuItem units10ToolStripMenuItem;
private ToolStripMenuItem units25ToolStripMenuItem;
private ToolStripMenuItem units50ToolStripMenuItem;
private ToolStripMenuItem units100ToolStripMenuItem;
private ToolStripMenuItem solidToolStripMenuItem;
private ToolStripMenuItem wireframeToolStripMenuItem;
private ToolStripMenuItem dummyToolStripMenuItem1;
private ToolStripMenuItem dummyToolStripMenuItem;
private ToolStripMenuItem sceneOriginToolStripMenuItem;
private ToolStripMenuItem cameraPositionToolStripMenuItem1;
private ToolStripMenuItem selectInTreeViewToolStripMenuItem;
private ToolStripMenuItem lockShapesToolStripMenuItem;
private ToolStripMenuItem pickToolStripMenuItem;
private ToolStripMenuItem Pick_UV0_ToolStripMenuItem;
private ToolStripMenuItem Pick_UV1_ToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem8;
private ToolStripMenuItem directionFloorToolStripMenuItem;
private ToolStripMenuItem directionCeilingToolStripMenuItem;
private ToolStripMenuItem directionPosXToolStripMenuItem;
private ToolStripMenuItem directionNegXToolStripMenuItem;
private ToolStripMenuItem directionPosYToolStripMenuItem;
private ToolStripMenuItem directionNegYToolStripMenuItem;
private ToolStripSplitButton toolStripDropDownButton_PerspectiveMode;
private ToolStripMenuItem ToolStripMenuItem_Perspective;
private ToolStripMenuItem ToolStripMenuItem_PerspectiveTop;
private ToolStripMenuItem ToolStripMenuItem_PerspectiveFront;
private ToolStripMenuItem ToolStripMenuItem_PerspectiveRight;
private ToolStripSeparator toolStripMenuItem19;
private ToolStripMenuItem ToolStripMenuItem_Top;
private ToolStripMenuItem ToolStripMenuItem_Front;
private ToolStripMenuItem ToolStripMenuItem_Right;
private ToolStripSeparator toolStripMenuItem1;
private ToolStripMenuItem ToolStripMenuItem_View_EditClipRange;
private ToolStripMenuItem ToolStripMenuItem_View_ZoomIn;
private ToolStripMenuItem ToolStripMenuItem_View_ZoomOut;
private ToolStripMenuItem ToolStripMenuItem_View_ZoomDefault;
private ToolStripSplitButton ToolStripSplitButton_Rendering;
private ToolStripMenuItem ToolStrip_Rendering_Solid;
private ToolStripMenuItem ToolStrip_Rendering_Wireframe;
private ToolStripSeparator ToolStrip_Rendering_Sep1;
private ToolStripSeparator ToolStrip_Rendering_Sep2;
private ToolStripMenuItem ToolStrip_Rendering_AffectsScene;
private ToolStripMenuItem ToolStrip_Rendering_AffectsSelection;
private ToolStripSplitButton ToolStripSplitButton_DebugFlags;
private ToolStripMenuItem zonesToolStripMenuItem;
private ToolStripMenuItem loadZoneAtCameraPositionToolStripMenuItem;
private ToolStripMenuItem selectZoneAtCameraPositionToolStripMenuItem;
private ToolStripMenuItem loadZonesCloseToCameraToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem9;
private ToolStripMenuItem ToolStripMenuItem_View_Properties;
private ToolStripMenuItem setupGridToolStripMenuItem;
private ToolStripSeparator toolStripMenuItem10;
private ToolStripMenuItem AlignmentToolStripMenuItem;
private ToolStripMenuItem AlignToGridToolStripMenuItem;
private ToolStripMenuItem AlignToObjectToolStripMenuItem;
private ToolStripMenuItem AlignToHitPointNormalToolStripMenuItem;
private ToolStripMenuItem AlignToHitPointToolStripMenuItem;
private ToolStripSplitButton toolStripSplitButton_ViewIcons;
private ToolStripMenuItem showViewIconsToolStripMenuItem;
private ToolStripMenuItem showHiddenShapesToolStripMenuItem;
private ToolStripMenuItem viewIconActionToolStripMenuItem;
private ToolStripMenuItem ToolStripMenuItem_IconAction_None;
private ToolStripMenuItem ToolStripMenuItem_IconAction_Select;
private ToolStripSeparator toolStripMenuItem11;
private ToolStripMenuItem ToolStripMenuItem_IconAction_Delete;
private ToolStripMenuItem ToolStripMenuItem_IconAction_Drop;
private ToolStripMenuItem ToolStripMenuItem_IconAction_Drop_BB;
private ToolStripMenuItem ToolStripMenuItem_IconAction_Drop_BC;
private ToolStripSplitButton ToolStripSplitButton_MoveSensitivity;
private ImageList UnitScaleImageList;
private ToolStripMenuItem ToolStripMenuItem_View_Size;
private ToolStripMenuItem ToolStripMenuItem_View_SetSize;
private ToolStripSeparator toolStripMenuItem12;
private ToolStripMenuItem ToolStripMenuItem_View_SafeMode;
private ToolStripMenuItem ToolStripMenuItem_View_SetCustomSafeMode;
private ToolStripSeparator toolStripMenuItem13;
private ToolStripSeparator toolStripMenuItem14;
private ToolStripMenuItem ToolStripMenuItem_View_DisableSafeMode;
private ToolStripMenuItem ToolStripMenuItem_IconAction_ToggleFreeze;
private ToolStripMenuItem uniformScaleToolStripMenuItem;
private ToolStripMenuItem nonuniformScaleToolStripMenuItem;
private ToolStripMenuItem transformGizmoToolStripMenuItem;
private ToolStripMenuItem moveToolStripMenuItem;
private ToolStripMenuItem rotateToolStripMenuItem;
private ToolStripMenuItem scaleToolStripMenuItem;
private ToolStripSplitButton ToolStripSplitButton_Camera;
private ToolStripMenuItem toolStripButton_MoveContext;
private ToolStripMenuItem toolStripButton_MoveCameraWithKeys;
private ToolStripSeparator ToolStripSplitButton_CameraSep1;
private ToolStripMenuItem ToolStripSplitButton_CameraStyle_Pan;
private ToolStripMenuItem ToolStripSplitButton_CameraStyle_PanHoriz;
private ToolStripMenuItem ToolStripSplitButton_CameraStyle_Orbit;
private ToolStripMenuItem ToolStripSplitButton_CameraStyle_Max;
private ToolStripMenuItem ToolStripSplitButton_CameraStyle_Maya;
private ToolStripMenuItem toolStripButton_StickToGround;
private ToolStripMenuItem toolStripButton_EnableMoveSnap;
private ToolStripMenuItem toolStripButton_EnableAngleSnap;
private ToolStripSplitButton ToolStripSplitButton_Tools;
private ToolStripMenuItem toolStripButton_MeasureTool;
private ToolStripMenuItem toolStripButton_localPos;
private ToolStripMenuItem toolStripButton_localOri;
private ToolStripMenuItem toolStripButton_localScale;
private ToolStripMenuItem toolStripButton_Link_ShowAll;
private ToolStripMenuItem toolStripButton_Link_ShowSelected;
private ToolStripMenuItem toolStripButton_Link_ShowNone;
private ToolStripMenuItem toolStripButton_Link_ShowActiveLayer;
private ToolStripSeparator ToolStripSplitButton_Tools_Sep1;
private ToolStripMenuItem ToolStripButton_HighlightSelection;
private ToolStripMenuItem ToolStripButton_IsolateSelection;
private ToolStripMenuItem ToolStripButton_AutomaticZoomFit;
private ToolStripSplitButton ToolStripSplitButton_SelectionFilter;
private ToolStripMenuItem ToolStripSplitButton_SelectionFilterUse;
private ToolStripMenuItem ToolStripSplitButton_SelectionFilterOtherShapes;
private ToolStripSeparator toolStripMenuItem16;
private ToolStripSplitButton ToolStripButton_Select;
private ToolStripMenuItem ToolStripButton_OneClickSelection;
private ToolStripSeparator ToolStripSplitButton_CameraSep2;
private ToolStripMenuItem ToolStripSplitButton_Camera_MoveOrigin;
private ToolStripMenuItem ToolStripSplitButton_Camera_MoveShapeOrigin;
private ToolStripMenuItem ToolStripSplitButton_Camera_MoveSelection;
private ToolStripMenuItem traceHitToolStripMenuItem;
private ToolStripSplitButtonCheckable ToolStripButton_Scale;
private ToolStripSplitButtonCheckable ToolStripButton_Move;
private ToolStripSplitButtonCheckable ToolStripButton_Rotate;
private ToolStripSplitButtonCheckable ToolStripButton_Link;
private ToolStripSplitButtonCheckable ToolStripButton_ZoomFit;
ToolStripDropDown _play_Dropdown = null;

		private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EnginePanel));
      this.Profiling = new System.Windows.Forms.MenuItem();
      this.Profiling_Previous = new System.Windows.Forms.MenuItem();
      this.Profiling_Next = new System.Windows.Forms.MenuItem();
      this.panel_VisionView = new System.Windows.Forms.Panel();
      this.toolStrip_EnginePanel = new System.Windows.Forms.ToolStrip();
      this.ToolStripButton_Select = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolStripButton_Move = new CSharpFramework.Controls.ToolStripSplitButtonCheckable();
      this.toolStripButton_EnableMoveSnap = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_StickToGround = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_localPos = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripButton_Rotate = new CSharpFramework.Controls.ToolStripSplitButtonCheckable();
      this.toolStripButton_EnableAngleSnap = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_localOri = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripButton_Scale = new CSharpFramework.Controls.ToolStripSplitButtonCheckable();
      this.uniformScaleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.nonuniformScaleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_localScale = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripButton_Link = new CSharpFramework.Controls.ToolStripSplitButtonCheckable();
      this.toolStripButton_Link_ShowNone = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_Link_ShowAll = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_Link_ShowSelected = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_Link_ShowActiveLayer = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripDropDownButton_PlayMode = new System.Windows.Forms.ToolStripSplitButton();
      this.animateToolStripMenuItem_Animate = new System.Windows.Forms.ToolStripMenuItem();
      this.runInEditorToolStripMenuItem_RunInEditor = new System.Windows.Forms.ToolStripMenuItem();
      this.playTheGameToolStripMenuItem_PlayTheGame = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_Camera = new System.Windows.Forms.ToolStripSplitButton();
      this.toolStripButton_MoveContext = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripButton_MoveCameraWithKeys = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_CameraSep1 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripSplitButton_CameraStyle_Pan = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_CameraStyle_PanHoriz = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_CameraStyle_Orbit = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_CameraStyle_Max = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_CameraStyle_Maya = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_CameraSep2 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripSplitButton_Camera_MoveOrigin = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_Camera_MoveShapeOrigin = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_Camera_MoveSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_MoveSensitivity = new System.Windows.Forms.ToolStripSplitButton();
      this.toolStripSplitButton_ViewIcons = new System.Windows.Forms.ToolStripSplitButton();
      this.showViewIconsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.showHiddenShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.viewIconActionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_IconAction_None = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_IconAction_Select = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem11 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_IconAction_Delete = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_IconAction_ToggleFreeze = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_IconAction_Drop = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_IconAction_Drop_BB = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_IconAction_Drop_BC = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripButton_ZoomFit = new CSharpFramework.Controls.ToolStripSplitButtonCheckable();
      this.ToolStripButton_IsolateSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripButton_AutomaticZoomFit = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_SelectionFilter = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolStripSplitButton_SelectionFilterUse = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_SelectionFilterOtherShapes = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem16 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripSplitButton_Rendering = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolStrip_Rendering_Solid = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStrip_Rendering_Wireframe = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStrip_Rendering_Sep1 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStrip_Rendering_Sep2 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStrip_Rendering_AffectsScene = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStrip_Rendering_AffectsSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_DebugFlags = new System.Windows.Forms.ToolStripSplitButton();
      this.toolStripDropDownButton_PerspectiveMode = new System.Windows.Forms.ToolStripSplitButton();
      this.ToolStripMenuItem_Perspective = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_PerspectiveTop = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_PerspectiveFront = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_PerspectiveRight = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem19 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_Top = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_Front = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_Right = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_View_EditClipRange = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_View_ZoomIn = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_View_ZoomOut = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_View_ZoomDefault = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem9 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_View_Size = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_View_SetSize = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem12 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_View_SafeMode = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_View_SetCustomSafeMode = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripMenuItem_View_DisableSafeMode = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem13 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripMenuItem14 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripMenuItem_View_Properties = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_Tools = new System.Windows.Forms.ToolStripSplitButton();
      this.toolStripButton_MeasureTool = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripSplitButton_Tools_Sep1 = new System.Windows.Forms.ToolStripSeparator();
      this.ToolStripButton_HighlightSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.ToolStripButton_OneClickSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
      this.contextMenuStrip_SelShapes = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.transformGizmoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.moveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.rotateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.scaleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.AlignmentToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.AlignToGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.AlignToObjectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.AlignToHitPointToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.AlignToHitPointNormalToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dropToFloorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.pivotToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.boundingBoxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.bottomCenterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem8 = new System.Windows.Forms.ToolStripSeparator();
      this.directionFloorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionCeilingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionPosXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionNegXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionPosYToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.directionNegYToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.linksToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unlinkFromSourcesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unlinkAllTargetsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.moveToToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.cameraPositionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.shapeOriginToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
      this.selectInTreeViewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.lockShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
      this.cutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.copyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.contextMenuStrip_View = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.pasteAtCursorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
      this.gridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.setupGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem10 = new System.Windows.Forms.ToolStripSeparator();
      this.snapPositionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.snapRotationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.showGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.relativeGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.gridLinesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.volumeGridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripSeparator();
      this.gridSize10ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.gridSize25ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.gridSize100ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem15 = new System.Windows.Forms.ToolStripSeparator();
      this.units2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.units5ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.units10ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.units25ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.units50ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.units100ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
      this.renderingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.solidToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.wireframeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.shadingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dummyToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.debugFlagsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.dummyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.pickToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.Pick_UV0_ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.Pick_UV1_ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
      this.createToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.insertHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.shapeOriginToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.sceneOriginToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.cameraPositionToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.traceHitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.zonesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectZoneAtCameraPositionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.loadZoneAtCameraPositionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.loadZonesCloseToCameraToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.UnitScaleImageList = new System.Windows.Forms.ImageList(this.components);
      this.toolStrip_EnginePanel.SuspendLayout();
      this.contextMenuStrip_SelShapes.SuspendLayout();
      this.contextMenuStrip_View.SuspendLayout();
      this.SuspendLayout();
      // 
      // Profiling
      // 
      this.Profiling.Index = -1;
      this.Profiling.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.Profiling_Previous,
            this.Profiling_Next});
      this.Profiling.Text = "Profiling";
      // 
      // Profiling_Previous
      // 
      this.Profiling_Previous.Index = 0;
      this.Profiling_Previous.Text = "Previous Chart";
      this.Profiling_Previous.Click += new System.EventHandler(this.Profiling_Previous_Click);
      // 
      // Profiling_Next
      // 
      this.Profiling_Next.Index = 1;
      this.Profiling_Next.Text = "Next Chart";
      this.Profiling_Next.Click += new System.EventHandler(this.Profiling_Next_Click);
      // 
      // panel_VisionView
      // 
      this.panel_VisionView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.panel_VisionView.Location = new System.Drawing.Point(0, 31);
      this.panel_VisionView.MinimumSize = new System.Drawing.Size(16, 16);
      this.panel_VisionView.Name = "panel_VisionView";
      this.panel_VisionView.Size = new System.Drawing.Size(769, 413);
      this.panel_VisionView.TabIndex = 1;
      // 
      // toolStrip_EnginePanel
      // 
      this.toolStrip_EnginePanel.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_EnginePanel.ImageScalingSize = new System.Drawing.Size(24, 24);
      this.toolStrip_EnginePanel.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripButton_Select,
            this.ToolStripButton_Move,
            this.ToolStripButton_Rotate,
            this.ToolStripButton_Scale,
            this.ToolStripButton_Link,
            this.toolStripSeparator3,
            this.toolStripDropDownButton_PlayMode,
            this.ToolStripSplitButton_Camera,
            this.ToolStripSplitButton_MoveSensitivity,
            this.toolStripSplitButton_ViewIcons,
            this.ToolStripButton_ZoomFit,
            this.ToolStripSplitButton_SelectionFilter,
            this.ToolStripSplitButton_Rendering,
            this.ToolStripSplitButton_DebugFlags,
            this.toolStripDropDownButton_PerspectiveMode,
            this.ToolStripSplitButton_Tools,
            this.toolStripSeparator4});
      this.toolStrip_EnginePanel.Location = new System.Drawing.Point(0, 0);
      this.toolStrip_EnginePanel.Name = "toolStrip_EnginePanel";
      this.toolStrip_EnginePanel.Size = new System.Drawing.Size(769, 31);
      this.toolStrip_EnginePanel.TabIndex = 0;
      this.toolStrip_EnginePanel.Text = "Engine View Toolbar";
      // 
      // ToolStripButton_Select
      // 
      this.ToolStripButton_Select.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButton_Select.Image = global::Editor.Properties.Resources.toolbar_select;
      this.ToolStripButton_Select.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_Select.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButton_Select.Name = "ToolStripButton_Select";
      this.ToolStripButton_Select.Size = new System.Drawing.Size(40, 28);
      this.ToolStripButton_Select.Text = "Select";
      this.ToolStripButton_Select.Visible = false;
      // 
      // ToolStripButton_Move
      // 
      this.ToolStripButton_Move.Checked = System.Windows.Forms.CheckState.Unchecked;
      this.ToolStripButton_Move.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButton_Move.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_EnableMoveSnap,
            this.toolStripButton_StickToGround,
            this.toolStripButton_localPos});
      this.ToolStripButton_Move.Image = global::Editor.Properties.Resources.toolbar_move;
      this.ToolStripButton_Move.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_Move.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButton_Move.Name = "ToolStripButton_Move";
      this.ToolStripButton_Move.Size = new System.Drawing.Size(40, 28);
      this.ToolStripButton_Move.Text = "Move";
      this.ToolStripButton_Move.ToolTipText = "Move Selected Shapes";
      this.ToolStripButton_Move.Click += new System.EventHandler(this.toolStripButton_Move_Click);
      this.ToolStripButton_Move.DropDownOpening += new System.EventHandler(this.toolStripButton_Move_DropDownOpening);
      // 
      // toolStripButton_EnableMoveSnap
      // 
      this.toolStripButton_EnableMoveSnap.CheckOnClick = true;
      this.toolStripButton_EnableMoveSnap.Image = global::Editor.Properties.Resources.magnet;
      this.toolStripButton_EnableMoveSnap.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_EnableMoveSnap.Name = "toolStripButton_EnableMoveSnap";
      this.toolStripButton_EnableMoveSnap.Size = new System.Drawing.Size(171, 22);
      this.toolStripButton_EnableMoveSnap.Text = "Enable Move Snap";
      this.toolStripButton_EnableMoveSnap.Click += new System.EventHandler(this.toolStripButton_EnableMoveSnap_Click);
      // 
      // toolStripButton_StickToGround
      // 
      this.toolStripButton_StickToGround.CheckOnClick = true;
      this.toolStripButton_StickToGround.Image = global::Editor.Properties.Resources.arrow_green_down;
      this.toolStripButton_StickToGround.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_StickToGround.Name = "toolStripButton_StickToGround";
      this.toolStripButton_StickToGround.Size = new System.Drawing.Size(171, 22);
      this.toolStripButton_StickToGround.Text = "Stick To Ground";
      this.toolStripButton_StickToGround.ToolTipText = "If enabled, objects stick to the ground surface while moving the shapes";
      this.toolStripButton_StickToGround.Click += new System.EventHandler(this.toolStripButton_StickToGround_Click);
      // 
      // toolStripButton_localPos
      // 
      this.toolStripButton_localPos.CheckOnClick = true;
      this.toolStripButton_localPos.Image = global::Editor.Properties.Resources.toolbar_gizmo;
      this.toolStripButton_localPos.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_localPos.Name = "toolStripButton_localPos";
      this.toolStripButton_localPos.Size = new System.Drawing.Size(171, 22);
      this.toolStripButton_localPos.Text = "Local Transform";
      this.toolStripButton_localPos.Click += new System.EventHandler(this.toolStripButton_localPos_Click);
      // 
      // ToolStripButton_Rotate
      // 
      this.ToolStripButton_Rotate.Checked = System.Windows.Forms.CheckState.Unchecked;
      this.ToolStripButton_Rotate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButton_Rotate.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_EnableAngleSnap,
            this.toolStripButton_localOri});
      this.ToolStripButton_Rotate.Image = global::Editor.Properties.Resources.toolbar_rotate;
      this.ToolStripButton_Rotate.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_Rotate.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButton_Rotate.Name = "ToolStripButton_Rotate";
      this.ToolStripButton_Rotate.Size = new System.Drawing.Size(40, 28);
      this.ToolStripButton_Rotate.Text = "Rotate";
      this.ToolStripButton_Rotate.ToolTipText = "Rotate Selected Shapes";
      this.ToolStripButton_Rotate.Click += new System.EventHandler(this.toolStripButton_Rotate_Click);
      this.ToolStripButton_Rotate.DropDownOpening += new System.EventHandler(this.toolStripButton_Rotate_DropDownOpening);
      // 
      // toolStripButton_EnableAngleSnap
      // 
      this.toolStripButton_EnableAngleSnap.CheckOnClick = true;
      this.toolStripButton_EnableAngleSnap.Image = global::Editor.Properties.Resources.magnet;
      this.toolStripButton_EnableAngleSnap.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_EnableAngleSnap.Name = "toolStripButton_EnableAngleSnap";
      this.toolStripButton_EnableAngleSnap.Size = new System.Drawing.Size(172, 22);
      this.toolStripButton_EnableAngleSnap.Text = "Enable Angle Snap";
      this.toolStripButton_EnableAngleSnap.Click += new System.EventHandler(this.toolStripButton_EnableAngleSnap_Click);
      // 
      // toolStripButton_localOri
      // 
      this.toolStripButton_localOri.CheckOnClick = true;
      this.toolStripButton_localOri.Image = global::Editor.Properties.Resources.toolbar_gizmo;
      this.toolStripButton_localOri.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_localOri.Name = "toolStripButton_localOri";
      this.toolStripButton_localOri.Size = new System.Drawing.Size(172, 22);
      this.toolStripButton_localOri.Text = "Local Transform";
      this.toolStripButton_localOri.Click += new System.EventHandler(this.toolStripButton_localOri_Click);
      // 
      // ToolStripButton_Scale
      // 
      this.ToolStripButton_Scale.Checked = System.Windows.Forms.CheckState.Unchecked;
      this.ToolStripButton_Scale.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButton_Scale.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.uniformScaleToolStripMenuItem,
            this.nonuniformScaleToolStripMenuItem,
            this.toolStripButton_localScale});
      this.ToolStripButton_Scale.Image = global::Editor.Properties.Resources.toolbar_scale_uniform;
      this.ToolStripButton_Scale.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_Scale.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButton_Scale.Name = "ToolStripButton_Scale";
      this.ToolStripButton_Scale.Size = new System.Drawing.Size(40, 28);
      this.ToolStripButton_Scale.Text = "Scale";
      this.ToolStripButton_Scale.ToolTipText = "Scale Selected Shapes";
      this.ToolStripButton_Scale.ButtonClick += new System.EventHandler(this.toolStripSplitButton_Scale_ButtonClick);
      this.ToolStripButton_Scale.DropDownOpening += new System.EventHandler(this.toolStripSplitButton_Scale_DropDownOpening);
      // 
      // uniformScaleToolStripMenuItem
      // 
      this.uniformScaleToolStripMenuItem.Image = global::Editor.Properties.Resources.toolbar_scale_uniform_small;
      this.uniformScaleToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.uniformScaleToolStripMenuItem.Name = "uniformScaleToolStripMenuItem";
      this.uniformScaleToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
      this.uniformScaleToolStripMenuItem.Text = "Uniform Scale";
      this.uniformScaleToolStripMenuItem.Click += new System.EventHandler(this.uniformScaleToolStripMenuItem1_Click);
      // 
      // nonuniformScaleToolStripMenuItem
      // 
      this.nonuniformScaleToolStripMenuItem.Image = global::Editor.Properties.Resources.toolbar_scale_nonuniform_small;
      this.nonuniformScaleToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.nonuniformScaleToolStripMenuItem.Name = "nonuniformScaleToolStripMenuItem";
      this.nonuniformScaleToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
      this.nonuniformScaleToolStripMenuItem.Text = "Non-uniform Scale";
      this.nonuniformScaleToolStripMenuItem.Click += new System.EventHandler(this.nonuniformScaleToolStripMenuItem1_Click);
      // 
      // toolStripButton_localScale
      // 
      this.toolStripButton_localScale.CheckOnClick = true;
      this.toolStripButton_localScale.Image = global::Editor.Properties.Resources.toolbar_gizmo;
      this.toolStripButton_localScale.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_localScale.Name = "toolStripButton_localScale";
      this.toolStripButton_localScale.Size = new System.Drawing.Size(175, 22);
      this.toolStripButton_localScale.Text = "Local Transform";
      this.toolStripButton_localScale.Click += new System.EventHandler(this.toolStripButton_localScale_Click);
      // 
      // ToolStripButton_Link
      // 
      this.ToolStripButton_Link.Checked = System.Windows.Forms.CheckState.Unchecked;
      this.ToolStripButton_Link.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButton_Link.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_Link_ShowNone,
            this.toolStripButton_Link_ShowAll,
            this.toolStripButton_Link_ShowSelected,
            this.toolStripButton_Link_ShowActiveLayer});
      this.ToolStripButton_Link.Image = global::Editor.Properties.Resources.toolbar_link;
      this.ToolStripButton_Link.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButton_Link.Name = "ToolStripButton_Link";
      this.ToolStripButton_Link.Size = new System.Drawing.Size(40, 28);
      this.ToolStripButton_Link.Text = "Link";
      this.ToolStripButton_Link.ToolTipText = "Shape Linking Context";
      this.ToolStripButton_Link.Click += new System.EventHandler(this.toolStripButton_Link_Click);
      this.ToolStripButton_Link.DropDownOpening += new System.EventHandler(this.toolStripButton_Link_DropDownOpening);
      // 
      // toolStripButton_Link_ShowNone
      // 
      this.toolStripButton_Link_ShowNone.CheckOnClick = true;
      this.toolStripButton_Link_ShowNone.Image = global::Editor.Properties.Resources.toolbar_link_small;
      this.toolStripButton_Link_ShowNone.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_Link_ShowNone.Name = "toolStripButton_Link_ShowNone";
      this.toolStripButton_Link_ShowNone.Size = new System.Drawing.Size(200, 22);
      this.toolStripButton_Link_ShowNone.Text = "Hide All Links";
      this.toolStripButton_Link_ShowNone.Click += new System.EventHandler(this.toolStripButton_Link_Show_Click);
      // 
      // toolStripButton_Link_ShowAll
      // 
      this.toolStripButton_Link_ShowAll.CheckOnClick = true;
      this.toolStripButton_Link_ShowAll.Image = global::Editor.Properties.Resources.toolbar_link_small;
      this.toolStripButton_Link_ShowAll.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_Link_ShowAll.Name = "toolStripButton_Link_ShowAll";
      this.toolStripButton_Link_ShowAll.Size = new System.Drawing.Size(200, 22);
      this.toolStripButton_Link_ShowAll.Text = "Show All Links";
      this.toolStripButton_Link_ShowAll.Click += new System.EventHandler(this.toolStripButton_Link_Show_Click);
      // 
      // toolStripButton_Link_ShowSelected
      // 
      this.toolStripButton_Link_ShowSelected.CheckOnClick = true;
      this.toolStripButton_Link_ShowSelected.Image = global::Editor.Properties.Resources.toolbar_link_small;
      this.toolStripButton_Link_ShowSelected.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_Link_ShowSelected.Name = "toolStripButton_Link_ShowSelected";
      this.toolStripButton_Link_ShowSelected.Size = new System.Drawing.Size(200, 22);
      this.toolStripButton_Link_ShowSelected.Text = "Show Links Selected";
      this.toolStripButton_Link_ShowSelected.Click += new System.EventHandler(this.toolStripButton_Link_Show_Click);
      // 
      // toolStripButton_Link_ShowActiveLayer
      // 
      this.toolStripButton_Link_ShowActiveLayer.CheckOnClick = true;
      this.toolStripButton_Link_ShowActiveLayer.Image = global::Editor.Properties.Resources.toolbar_link_small;
      this.toolStripButton_Link_ShowActiveLayer.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_Link_ShowActiveLayer.Name = "toolStripButton_Link_ShowActiveLayer";
      this.toolStripButton_Link_ShowActiveLayer.Size = new System.Drawing.Size(200, 22);
      this.toolStripButton_Link_ShowActiveLayer.Text = "Show Links Active Layer";
      this.toolStripButton_Link_ShowActiveLayer.Click += new System.EventHandler(this.toolStripButton_Link_Show_Click);
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(6, 31);
      // 
      // toolStripDropDownButton_PlayMode
      // 
      this.toolStripDropDownButton_PlayMode.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripDropDownButton_PlayMode.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.animateToolStripMenuItem_Animate,
            this.runInEditorToolStripMenuItem_RunInEditor,
            this.playTheGameToolStripMenuItem_PlayTheGame});
      this.toolStripDropDownButton_PlayMode.Image = global::Editor.Properties.Resources.toolbar_play_editor;
      this.toolStripDropDownButton_PlayMode.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripDropDownButton_PlayMode.Name = "toolStripDropDownButton_PlayMode";
      this.toolStripDropDownButton_PlayMode.Size = new System.Drawing.Size(40, 28);
      //this.toolStripDropDownButton_PlayMode.ToolTipText = "Animates or Plays Engine View";
      this.toolStripDropDownButton_PlayMode.ButtonClick += new System.EventHandler(this.toolStripDropDownButton_PlayMode_ButtonClick);
      // 
      // animateToolStripMenuItem_Animate
      // 
      this.animateToolStripMenuItem_Animate.Image = global::Editor.Properties.Resources.toolbar_timeofday;
      this.animateToolStripMenuItem_Animate.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.animateToolStripMenuItem_Animate.Name = "animateToolStripMenuItem_Animate";
      this.animateToolStripMenuItem_Animate.Size = new System.Drawing.Size(150, 22);
      this.animateToolStripMenuItem_Animate.Text = "Animate";
      //this.animateToolStripMenuItem_Animate.ToolTipText = "Play Application in \"Animate\" mode";
      this.animateToolStripMenuItem_Animate.Click += new System.EventHandler(this.animateToolStripMenuItem_Animate_Click);
      // 
      // runInEditorToolStripMenuItem_RunInEditor
      // 
      this.runInEditorToolStripMenuItem_RunInEditor.Image = global::Editor.Properties.Resources.toolbar_editor;
      this.runInEditorToolStripMenuItem_RunInEditor.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.runInEditorToolStripMenuItem_RunInEditor.Name = "runInEditorToolStripMenuItem_RunInEditor";
      this.runInEditorToolStripMenuItem_RunInEditor.Size = new System.Drawing.Size(150, 22);
      this.runInEditorToolStripMenuItem_RunInEditor.Text = "Run in Editor";
      //this.runInEditorToolStripMenuItem_RunInEditor.ToolTipText = "Play application in \"Play in Editor\" mode";
      this.runInEditorToolStripMenuItem_RunInEditor.Click += new System.EventHandler(this.runInEditorToolStripMenuItem_RunInEditor_Click);
      // 
      // playTheGameToolStripMenuItem_PlayTheGame
      // 
      this.playTheGameToolStripMenuItem_PlayTheGame.Image = global::Editor.Properties.Resources.toolbar_gamepad;
      this.playTheGameToolStripMenuItem_PlayTheGame.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.playTheGameToolStripMenuItem_PlayTheGame.Name = "playTheGameToolStripMenuItem_PlayTheGame";
      this.playTheGameToolStripMenuItem_PlayTheGame.Size = new System.Drawing.Size(150, 22);
      this.playTheGameToolStripMenuItem_PlayTheGame.Text = "Play the Game";
      //this.playTheGameToolStripMenuItem_PlayTheGame.ToolTipText = "Play application in \"Play the Game\" mode";
      this.playTheGameToolStripMenuItem_PlayTheGame.Click += new System.EventHandler(this.playTheGameToolStripMenuItem_PlayTheGame_Click);
      // 
      // ToolStripSplitButton_Camera
      // 
      this.ToolStripSplitButton_Camera.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripSplitButton_Camera.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_MoveContext,
            this.toolStripButton_MoveCameraWithKeys,
            this.ToolStripSplitButton_CameraSep1,
            this.ToolStripSplitButton_CameraStyle_Pan,
            this.ToolStripSplitButton_CameraStyle_PanHoriz,
            this.ToolStripSplitButton_CameraStyle_Orbit,
            this.ToolStripSplitButton_CameraStyle_Max,
            this.ToolStripSplitButton_CameraStyle_Maya,
            this.ToolStripSplitButton_CameraSep2,
            this.ToolStripSplitButton_Camera_MoveOrigin,
            this.ToolStripSplitButton_Camera_MoveShapeOrigin,
            this.ToolStripSplitButton_Camera_MoveSelection});
      this.ToolStripSplitButton_Camera.Image = global::Editor.Properties.Resources.toolbar_camera;
      this.ToolStripSplitButton_Camera.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_Camera.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripSplitButton_Camera.Name = "ToolStripSplitButton_Camera";
      this.ToolStripSplitButton_Camera.Size = new System.Drawing.Size(40, 28);
      this.ToolStripSplitButton_Camera.Text = "Camera Control Options";
      this.ToolStripSplitButton_Camera.Click += new System.EventHandler(this.toolStripButton_MoveContext_Click);
      this.ToolStripSplitButton_Camera.DropDownOpening += new System.EventHandler(this.ToolStripSplitButton_Camera_DropDownOpening);
      // 
      // toolStripButton_MoveContext
      // 
      this.toolStripButton_MoveContext.Image = global::Editor.Properties.Resources.toolbar_camera_move;
      this.toolStripButton_MoveContext.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_MoveContext.Name = "toolStripButton_MoveContext";
      this.toolStripButton_MoveContext.Size = new System.Drawing.Size(262, 22);
      this.toolStripButton_MoveContext.Text = "Enable Standard Move Context";
      this.toolStripButton_MoveContext.ToolTipText = "Enables the Standard Move Context";
      this.toolStripButton_MoveContext.Click += new System.EventHandler(this.toolStripButton_MoveContext_Click);
      // 
      // toolStripButton_MoveCameraWithKeys
      // 
      this.toolStripButton_MoveCameraWithKeys.Image = global::Editor.Properties.Resources.toolbar_camera_keys;
      this.toolStripButton_MoveCameraWithKeys.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_MoveCameraWithKeys.Name = "toolStripButton_MoveCameraWithKeys";
      this.toolStripButton_MoveCameraWithKeys.Size = new System.Drawing.Size(262, 22);
      this.toolStripButton_MoveCameraWithKeys.Text = "Control Camera with WASD Keys";
      this.toolStripButton_MoveCameraWithKeys.ToolTipText = "Camera Control with [W,A,S,D] Keys (Disables Hotkeys)";
      this.toolStripButton_MoveCameraWithKeys.Click += new System.EventHandler(this.toolStripButton_MoveCameraWithKeys_Click);
      // 
      // ToolStripSplitButton_CameraSep1
      // 
      this.ToolStripSplitButton_CameraSep1.Name = "ToolStripSplitButton_CameraSep1";
      this.ToolStripSplitButton_CameraSep1.Size = new System.Drawing.Size(259, 6);
      // 
      // ToolStripSplitButton_CameraStyle_Pan
      // 
      this.ToolStripSplitButton_CameraStyle_Pan.Image = global::Editor.Properties.Resources.toolbar_camera_move;
      this.ToolStripSplitButton_CameraStyle_Pan.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_CameraStyle_Pan.Name = "ToolStripSplitButton_CameraStyle_Pan";
      this.ToolStripSplitButton_CameraStyle_Pan.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_CameraStyle_Pan.Text = "Style: Middle Mouse Pan";
      this.ToolStripSplitButton_CameraStyle_Pan.Click += new System.EventHandler(this.ToolStripSplitButton_CameraStyle_Click);
      // 
      // ToolStripSplitButton_CameraStyle_PanHoriz
      // 
      this.ToolStripSplitButton_CameraStyle_PanHoriz.Image = global::Editor.Properties.Resources.toolbar_camera_move;
      this.ToolStripSplitButton_CameraStyle_PanHoriz.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_CameraStyle_PanHoriz.Name = "ToolStripSplitButton_CameraStyle_PanHoriz";
      this.ToolStripSplitButton_CameraStyle_PanHoriz.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_CameraStyle_PanHoriz.Text = "Style: Middle Mouse Pan Horizontal";
      this.ToolStripSplitButton_CameraStyle_PanHoriz.Click += new System.EventHandler(this.ToolStripSplitButton_CameraStyle_Click);
      // 
      // ToolStripSplitButton_CameraStyle_Orbit
      // 
      this.ToolStripSplitButton_CameraStyle_Orbit.Image = global::Editor.Properties.Resources.toolbar_camera_orbit;
      this.ToolStripSplitButton_CameraStyle_Orbit.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_CameraStyle_Orbit.Name = "ToolStripSplitButton_CameraStyle_Orbit";
      this.ToolStripSplitButton_CameraStyle_Orbit.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_CameraStyle_Orbit.Text = "Style: Middle Mouse Orbit Selection";
      this.ToolStripSplitButton_CameraStyle_Orbit.Click += new System.EventHandler(this.ToolStripSplitButton_CameraStyle_Click);
      // 
      // ToolStripSplitButton_CameraStyle_Max
      // 
      this.ToolStripSplitButton_CameraStyle_Max.Image = ((System.Drawing.Image)(resources.GetObject("ToolStripSplitButton_CameraStyle_Max.Image")));
      this.ToolStripSplitButton_CameraStyle_Max.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_CameraStyle_Max.Name = "ToolStripSplitButton_CameraStyle_Max";
      this.ToolStripSplitButton_CameraStyle_Max.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_CameraStyle_Max.Text = "Style: 3dsMax Camera";
      this.ToolStripSplitButton_CameraStyle_Max.Click += new System.EventHandler(this.ToolStripSplitButton_CameraStyle_Click);
      // 
      // ToolStripSplitButton_CameraStyle_Maya
      // 
      this.ToolStripSplitButton_CameraStyle_Maya.Image = ((System.Drawing.Image)(resources.GetObject("ToolStripSplitButton_CameraStyle_Maya.Image")));
      this.ToolStripSplitButton_CameraStyle_Maya.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_CameraStyle_Maya.Name = "ToolStripSplitButton_CameraStyle_Maya";
      this.ToolStripSplitButton_CameraStyle_Maya.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_CameraStyle_Maya.Text = "Style: Maya Camera";
      this.ToolStripSplitButton_CameraStyle_Maya.Click += new System.EventHandler(this.ToolStripSplitButton_CameraStyle_Click);
      // 
      // ToolStripSplitButton_CameraSep2
      // 
      this.ToolStripSplitButton_CameraSep2.Name = "ToolStripSplitButton_CameraSep2";
      this.ToolStripSplitButton_CameraSep2.Size = new System.Drawing.Size(259, 6);
      // 
      // ToolStripSplitButton_Camera_MoveOrigin
      // 
      this.ToolStripSplitButton_Camera_MoveOrigin.Image = global::Editor.Properties.Resources.gizmo;
      this.ToolStripSplitButton_Camera_MoveOrigin.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_Camera_MoveOrigin.Name = "ToolStripSplitButton_Camera_MoveOrigin";
      this.ToolStripSplitButton_Camera_MoveOrigin.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_Camera_MoveOrigin.Text = "Move Camera to Origin";
      this.ToolStripSplitButton_Camera_MoveOrigin.Click += new System.EventHandler(this.ToolStripSplitButton_Camera_MoveOrigin_Click);
      // 
      // ToolStripSplitButton_Camera_MoveShapeOrigin
      // 
      this.ToolStripSplitButton_Camera_MoveShapeOrigin.Image = global::Editor.Properties.Resources.node;
      this.ToolStripSplitButton_Camera_MoveShapeOrigin.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_Camera_MoveShapeOrigin.Name = "ToolStripSplitButton_Camera_MoveShapeOrigin";
      this.ToolStripSplitButton_Camera_MoveShapeOrigin.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_Camera_MoveShapeOrigin.Text = "Move Camera to Shape Origin";
      this.ToolStripSplitButton_Camera_MoveShapeOrigin.Click += new System.EventHandler(this.ToolStripSplitButton_Camera_MoveShapeOrigin_Click);
      // 
      // ToolStripSplitButton_Camera_MoveSelection
      // 
      this.ToolStripSplitButton_Camera_MoveSelection.Image = global::Editor.Properties.Resources.pick;
      this.ToolStripSplitButton_Camera_MoveSelection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_Camera_MoveSelection.Name = "ToolStripSplitButton_Camera_MoveSelection";
      this.ToolStripSplitButton_Camera_MoveSelection.Size = new System.Drawing.Size(262, 22);
      this.ToolStripSplitButton_Camera_MoveSelection.Text = "Move Camera to Selection";
      this.ToolStripSplitButton_Camera_MoveSelection.Click += new System.EventHandler(this.ToolStripSplitButton_Camera_MoveSelection_Click);
      // 
      // ToolStripSplitButton_MoveSensitivity
      // 
      this.ToolStripSplitButton_MoveSensitivity.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripSplitButton_MoveSensitivity.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_MoveSensitivity.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripSplitButton_MoveSensitivity.Name = "ToolStripSplitButton_MoveSensitivity";
      this.ToolStripSplitButton_MoveSensitivity.Size = new System.Drawing.Size(40, 28);
      this.ToolStripSplitButton_MoveSensitivity.Text = "Camera Move and Rotate Sensitivity";
      this.ToolStripSplitButton_MoveSensitivity.ButtonClick += new System.EventHandler(this.ToolStripSplitButton_UIUnitScale_ButtonClick);
      // 
      // toolStripSplitButton_ViewIcons
      // 
      this.toolStripSplitButton_ViewIcons.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripSplitButton_ViewIcons.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showViewIconsToolStripMenuItem,
            this.showHiddenShapesToolStripMenuItem,
            this.viewIconActionToolStripMenuItem});
      this.toolStripSplitButton_ViewIcons.Image = global::Editor.Properties.Resources.icons;
      this.toolStripSplitButton_ViewIcons.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripSplitButton_ViewIcons.Name = "toolStripSplitButton_ViewIcons";
      this.toolStripSplitButton_ViewIcons.Size = new System.Drawing.Size(40, 28);
      this.toolStripSplitButton_ViewIcons.Text = "Shape View Icons";
      this.toolStripSplitButton_ViewIcons.ToolTipText = "Shape View Icons";
      this.toolStripSplitButton_ViewIcons.ButtonClick += new System.EventHandler(this.toolStripSplitButton_ViewIcons_ButtonClick);
      this.toolStripSplitButton_ViewIcons.DropDownOpening += new System.EventHandler(this.toolStripSplitButton_ViewIcons_DropDownOpening);
      // 
      // showViewIconsToolStripMenuItem
      // 
      this.showViewIconsToolStripMenuItem.Image = global::Editor.Properties.Resources.ball_glass_blue_16;
      this.showViewIconsToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.showViewIconsToolStripMenuItem.Name = "showViewIconsToolStripMenuItem";
      this.showViewIconsToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
      this.showViewIconsToolStripMenuItem.Text = "Show Shape Icons";
      this.showViewIconsToolStripMenuItem.ToolTipText = "Indicates whether per-shape view icons are displayed in the first place";
      this.showViewIconsToolStripMenuItem.Click += new System.EventHandler(this.showViewIconsToolStripMenuItem_Click);
      // 
      // showHiddenShapesToolStripMenuItem
      // 
      this.showHiddenShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.ball_glass_grey_16;
      this.showHiddenShapesToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.showHiddenShapesToolStripMenuItem.Name = "showHiddenShapesToolStripMenuItem";
      this.showHiddenShapesToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
      this.showHiddenShapesToolStripMenuItem.Text = "Show Hidden Shapes";
      this.showHiddenShapesToolStripMenuItem.ToolTipText = "If enabled, also hidden shapes are shown";
      this.showHiddenShapesToolStripMenuItem.Click += new System.EventHandler(this.showHiddenShapesToolStripMenuItem_Click);
      // 
      // viewIconActionToolStripMenuItem
      // 
      this.viewIconActionToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_IconAction_None,
            this.ToolStripMenuItem_IconAction_Select,
            this.toolStripMenuItem11,
            this.ToolStripMenuItem_IconAction_Delete,
            this.ToolStripMenuItem_IconAction_ToggleFreeze,
            this.ToolStripMenuItem_IconAction_Drop,
            this.ToolStripMenuItem_IconAction_Drop_BB,
            this.ToolStripMenuItem_IconAction_Drop_BC});
      this.viewIconActionToolStripMenuItem.Image = global::Editor.Properties.Resources.wrench;
      this.viewIconActionToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.viewIconActionToolStripMenuItem.Name = "viewIconActionToolStripMenuItem";
      this.viewIconActionToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
      this.viewIconActionToolStripMenuItem.Text = "On Icon Click";
      // 
      // ToolStripMenuItem_IconAction_None
      // 
      this.ToolStripMenuItem_IconAction_None.Name = "ToolStripMenuItem_IconAction_None";
      this.ToolStripMenuItem_IconAction_None.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_None.Text = "Nothing";
      this.ToolStripMenuItem_IconAction_None.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_None_Click);
      // 
      // ToolStripMenuItem_IconAction_Select
      // 
      this.ToolStripMenuItem_IconAction_Select.Image = global::Editor.Properties.Resources.pick;
      this.ToolStripMenuItem_IconAction_Select.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_IconAction_Select.Name = "ToolStripMenuItem_IconAction_Select";
      this.ToolStripMenuItem_IconAction_Select.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_Select.Text = "Select Shape";
      this.ToolStripMenuItem_IconAction_Select.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_Select_Click);
      // 
      // toolStripMenuItem11
      // 
      this.toolStripMenuItem11.Name = "toolStripMenuItem11";
      this.toolStripMenuItem11.Size = new System.Drawing.Size(226, 6);
      // 
      // ToolStripMenuItem_IconAction_Delete
      // 
      this.ToolStripMenuItem_IconAction_Delete.Image = global::Editor.Properties.Resources.delete2;
      this.ToolStripMenuItem_IconAction_Delete.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_IconAction_Delete.Name = "ToolStripMenuItem_IconAction_Delete";
      this.ToolStripMenuItem_IconAction_Delete.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_Delete.Text = "Delete Shape";
      this.ToolStripMenuItem_IconAction_Delete.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_Delete_Click);
      // 
      // ToolStripMenuItem_IconAction_ToggleFreeze
      // 
      this.ToolStripMenuItem_IconAction_ToggleFreeze.Image = ((System.Drawing.Image)(resources.GetObject("ToolStripMenuItem_IconAction_ToggleFreeze.Image")));
      this.ToolStripMenuItem_IconAction_ToggleFreeze.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_IconAction_ToggleFreeze.Name = "ToolStripMenuItem_IconAction_ToggleFreeze";
      this.ToolStripMenuItem_IconAction_ToggleFreeze.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_ToggleFreeze.Text = "Freeze/Un-freeze Shape";
      this.ToolStripMenuItem_IconAction_ToggleFreeze.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_ToggleFreeze_Click);
      // 
      // ToolStripMenuItem_IconAction_Drop
      // 
      this.ToolStripMenuItem_IconAction_Drop.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.ToolStripMenuItem_IconAction_Drop.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_IconAction_Drop.Name = "ToolStripMenuItem_IconAction_Drop";
      this.ToolStripMenuItem_IconAction_Drop.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_Drop.Text = "Drop to Floor : Pivot";
      this.ToolStripMenuItem_IconAction_Drop.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_Drop_Click);
      // 
      // ToolStripMenuItem_IconAction_Drop_BB
      // 
      this.ToolStripMenuItem_IconAction_Drop_BB.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.ToolStripMenuItem_IconAction_Drop_BB.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_IconAction_Drop_BB.Name = "ToolStripMenuItem_IconAction_Drop_BB";
      this.ToolStripMenuItem_IconAction_Drop_BB.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_Drop_BB.Text = "Drop to Floor : Bounding box";
      this.ToolStripMenuItem_IconAction_Drop_BB.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_Drop_BB_Click);
      // 
      // ToolStripMenuItem_IconAction_Drop_BC
      // 
      this.ToolStripMenuItem_IconAction_Drop_BC.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.ToolStripMenuItem_IconAction_Drop_BC.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_IconAction_Drop_BC.Name = "ToolStripMenuItem_IconAction_Drop_BC";
      this.ToolStripMenuItem_IconAction_Drop_BC.Size = new System.Drawing.Size(229, 22);
      this.ToolStripMenuItem_IconAction_Drop_BC.Text = "Drop to Floor : Bottom center";
      this.ToolStripMenuItem_IconAction_Drop_BC.Click += new System.EventHandler(this.ToolStripMenuItem_IconAction_Drop_BC_Click);
      // 
      // ToolStripButton_ZoomFit
      // 
      this.ToolStripButton_ZoomFit.Checked = System.Windows.Forms.CheckState.Unchecked;
      this.ToolStripButton_ZoomFit.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripButton_ZoomFit.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripButton_IsolateSelection,
            this.ToolStripButton_AutomaticZoomFit});
      this.ToolStripButton_ZoomFit.Image = global::Editor.Properties.Resources.frame_all;
      this.ToolStripButton_ZoomFit.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripButton_ZoomFit.Name = "ToolStripButton_ZoomFit";
      this.ToolStripButton_ZoomFit.Size = new System.Drawing.Size(40, 28);
      this.ToolStripButton_ZoomFit.Text = "Focus On Selection";
      this.ToolStripButton_ZoomFit.ToolTipText = "Focus Scene";
      this.ToolStripButton_ZoomFit.Click += new System.EventHandler(this.toolStripButton_ZoomFit_Click);
      this.ToolStripButton_ZoomFit.DropDownOpening += new System.EventHandler(this.ToolStripButton_ZoomFit_DropDownOpening);
      // 
      // ToolStripButton_IsolateSelection
      // 
      this.ToolStripButton_IsolateSelection.CheckOnClick = true;
      this.ToolStripButton_IsolateSelection.Image = global::Editor.Properties.Resources.isolate_selection;
      this.ToolStripButton_IsolateSelection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_IsolateSelection.Name = "ToolStripButton_IsolateSelection";
      this.ToolStripButton_IsolateSelection.Size = new System.Drawing.Size(242, 30);
      this.ToolStripButton_IsolateSelection.Text = "Isolate Selection";
      this.ToolStripButton_IsolateSelection.CheckedChanged += new System.EventHandler(this.ToolStripButton_IsolateSelection_CheckedChanged);
      // 
      // ToolStripButton_AutomaticZoomFit
      // 
      this.ToolStripButton_AutomaticZoomFit.CheckOnClick = true;
      this.ToolStripButton_AutomaticZoomFit.Image = global::Editor.Properties.Resources.frame_autoselect;
      this.ToolStripButton_AutomaticZoomFit.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_AutomaticZoomFit.Name = "ToolStripButton_AutomaticZoomFit";
      this.ToolStripButton_AutomaticZoomFit.Size = new System.Drawing.Size(242, 30);
      this.ToolStripButton_AutomaticZoomFit.Text = "Automatic Focus On Selection";
      this.ToolStripButton_AutomaticZoomFit.ToolTipText = "Toggle Automatic Focus On Selection";
      this.ToolStripButton_AutomaticZoomFit.Click += new System.EventHandler(this.ToolStripButton_AutomaticZoomFit_Click);
      // 
      // ToolStripSplitButton_SelectionFilter
      // 
      this.ToolStripSplitButton_SelectionFilter.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripSplitButton_SelectionFilter.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripSplitButton_SelectionFilterUse,
            this.ToolStripSplitButton_SelectionFilterOtherShapes,
            this.toolStripMenuItem16});
      this.ToolStripSplitButton_SelectionFilter.Image = global::Editor.Properties.Resources.toolbar_filter;
      this.ToolStripSplitButton_SelectionFilter.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_SelectionFilter.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripSplitButton_SelectionFilter.Name = "ToolStripSplitButton_SelectionFilter";
      this.ToolStripSplitButton_SelectionFilter.Size = new System.Drawing.Size(40, 28);
      this.ToolStripSplitButton_SelectionFilter.Text = "Configure Shape Selection Filter";
      this.ToolStripSplitButton_SelectionFilter.ToolTipText = "Configures the Shape Selection Filter";
      this.ToolStripSplitButton_SelectionFilter.ButtonClick += new System.EventHandler(this.ToolStripSplitButton_SelectionFilter_Click);
      this.ToolStripSplitButton_SelectionFilter.DropDownOpening += new System.EventHandler(this.ToolStripSplitButton_SelectionFilter_DropDownOpening);
      // 
      // ToolStripSplitButton_SelectionFilterUse
      // 
      this.ToolStripSplitButton_SelectionFilterUse.CheckOnClick = true;
      this.ToolStripSplitButton_SelectionFilterUse.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_SelectionFilterUse.Name = "ToolStripSplitButton_SelectionFilterUse";
      this.ToolStripSplitButton_SelectionFilterUse.Size = new System.Drawing.Size(191, 22);
      this.ToolStripSplitButton_SelectionFilterUse.Text = "Enable Selection Filter";
      this.ToolStripSplitButton_SelectionFilterUse.ToolTipText = "Enables the Shape Selection Filter";
      this.ToolStripSplitButton_SelectionFilterUse.Click += new System.EventHandler(this.ToolStripSplitButton_SelectionFilterUse_Click);
      // 
      // ToolStripSplitButton_SelectionFilterOtherShapes
      // 
      this.ToolStripSplitButton_SelectionFilterOtherShapes.CheckOnClick = true;
      this.ToolStripSplitButton_SelectionFilterOtherShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_SelectionFilterOtherShapes.Name = "ToolStripSplitButton_SelectionFilterOtherShapes";
      this.ToolStripSplitButton_SelectionFilterOtherShapes.Size = new System.Drawing.Size(191, 22);
      this.ToolStripSplitButton_SelectionFilterOtherShapes.Text = "Other Shapes Pickable";
      this.ToolStripSplitButton_SelectionFilterOtherShapes.ToolTipText = "Determines whether other shapes type not in the list are still pickable";
      this.ToolStripSplitButton_SelectionFilterOtherShapes.Click += new System.EventHandler(this.ToolStripSplitButton_SelectionFilterOtherShapes_Click);
      // 
      // toolStripMenuItem16
      // 
      this.toolStripMenuItem16.Name = "toolStripMenuItem16";
      this.toolStripMenuItem16.Size = new System.Drawing.Size(188, 6);
      // 
      // ToolStripSplitButton_Rendering
      // 
      this.ToolStripSplitButton_Rendering.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripSplitButton_Rendering.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStrip_Rendering_Solid,
            this.ToolStrip_Rendering_Wireframe,
            this.ToolStrip_Rendering_Sep1,
            this.ToolStrip_Rendering_Sep2,
            this.ToolStrip_Rendering_AffectsScene,
            this.ToolStrip_Rendering_AffectsSelection});
      this.ToolStripSplitButton_Rendering.Image = global::Editor.Properties.Resources.view_mode;
      this.ToolStripSplitButton_Rendering.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripSplitButton_Rendering.Name = "ToolStripSplitButton_Rendering";
      this.ToolStripSplitButton_Rendering.Size = new System.Drawing.Size(40, 28);
      this.ToolStripSplitButton_Rendering.ToolTipText = "Rendering & Shading Modes";
      this.ToolStripSplitButton_Rendering.ButtonClick += new System.EventHandler(this.ToolStripSplitButton_Rendering_ButtonClick);
      this.ToolStripSplitButton_Rendering.DropDownOpening += new System.EventHandler(this.ToolStripSplitButton_Rendering_DropDownOpening);
      // 
      // ToolStrip_Rendering_Solid
      // 
      this.ToolStrip_Rendering_Solid.Image = global::Editor.Properties.Resources.view_solid;
      this.ToolStrip_Rendering_Solid.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStrip_Rendering_Solid.Name = "ToolStrip_Rendering_Solid";
      this.ToolStrip_Rendering_Solid.Size = new System.Drawing.Size(208, 22);
      this.ToolStrip_Rendering_Solid.Text = "Solid";
      this.ToolStrip_Rendering_Solid.Click += new System.EventHandler(this.solidToolStripMenuItem_Click);
      // 
      // ToolStrip_Rendering_Wireframe
      // 
      this.ToolStrip_Rendering_Wireframe.Image = global::Editor.Properties.Resources.view_wireframe;
      this.ToolStrip_Rendering_Wireframe.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStrip_Rendering_Wireframe.Name = "ToolStrip_Rendering_Wireframe";
      this.ToolStrip_Rendering_Wireframe.Size = new System.Drawing.Size(208, 22);
      this.ToolStrip_Rendering_Wireframe.Text = "Wireframe";
      this.ToolStrip_Rendering_Wireframe.Click += new System.EventHandler(this.wireframeToolStripMenuItem_Click);
      // 
      // ToolStrip_Rendering_Sep1
      // 
      this.ToolStrip_Rendering_Sep1.Name = "ToolStrip_Rendering_Sep1";
      this.ToolStrip_Rendering_Sep1.Size = new System.Drawing.Size(205, 6);
      // 
      // ToolStrip_Rendering_Sep2
      // 
      this.ToolStrip_Rendering_Sep2.Name = "ToolStrip_Rendering_Sep2";
      this.ToolStrip_Rendering_Sep2.Size = new System.Drawing.Size(205, 6);
      this.ToolStrip_Rendering_Sep2.Visible = false;
      // 
      // ToolStrip_Rendering_AffectsScene
      // 
      this.ToolStrip_Rendering_AffectsScene.Name = "ToolStrip_Rendering_AffectsScene";
      this.ToolStrip_Rendering_AffectsScene.Size = new System.Drawing.Size(208, 22);
      this.ToolStrip_Rendering_AffectsScene.Text = "Shading Affects Scene";
      this.ToolStrip_Rendering_AffectsScene.Visible = false;
      // 
      // ToolStrip_Rendering_AffectsSelection
      // 
      this.ToolStrip_Rendering_AffectsSelection.Name = "ToolStrip_Rendering_AffectsSelection";
      this.ToolStrip_Rendering_AffectsSelection.Size = new System.Drawing.Size(208, 22);
      this.ToolStrip_Rendering_AffectsSelection.Text = "Shading Affects Selection";
      this.ToolStrip_Rendering_AffectsSelection.Visible = false;
      // 
      // ToolStripSplitButton_DebugFlags
      // 
      this.ToolStripSplitButton_DebugFlags.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripSplitButton_DebugFlags.Image = global::Editor.Properties.Resources.flags;
      this.ToolStripSplitButton_DebugFlags.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripSplitButton_DebugFlags.Name = "ToolStripSplitButton_DebugFlags";
      this.ToolStripSplitButton_DebugFlags.Size = new System.Drawing.Size(40, 28);
      this.ToolStripSplitButton_DebugFlags.ToolTipText = "Engine Debug Flags";
      this.ToolStripSplitButton_DebugFlags.ButtonClick += new System.EventHandler(this.ToolStripSplitButton_DebugFlags_ButtonClick);
      this.ToolStripSplitButton_DebugFlags.DropDownOpening += new System.EventHandler(this.ToolStripSplitButton_DebugFlags_DropDownOpening);
      // 
      // toolStripDropDownButton_PerspectiveMode
      // 
      this.toolStripDropDownButton_PerspectiveMode.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripDropDownButton_PerspectiveMode.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_Perspective,
            this.ToolStripMenuItem_PerspectiveTop,
            this.ToolStripMenuItem_PerspectiveFront,
            this.ToolStripMenuItem_PerspectiveRight,
            this.toolStripMenuItem19,
            this.ToolStripMenuItem_Top,
            this.ToolStripMenuItem_Front,
            this.ToolStripMenuItem_Right,
            this.toolStripMenuItem1,
            this.ToolStripMenuItem_View_EditClipRange,
            this.ToolStripMenuItem_View_ZoomIn,
            this.ToolStripMenuItem_View_ZoomOut,
            this.ToolStripMenuItem_View_ZoomDefault,
            this.toolStripMenuItem9,
            this.ToolStripMenuItem_View_Size,
            this.ToolStripMenuItem_View_SafeMode,
            this.toolStripMenuItem14,
            this.ToolStripMenuItem_View_Properties});
      this.toolStripDropDownButton_PerspectiveMode.Image = global::Editor.Properties.Resources.view_properties;
      this.toolStripDropDownButton_PerspectiveMode.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripDropDownButton_PerspectiveMode.Name = "toolStripDropDownButton_PerspectiveMode";
      this.toolStripDropDownButton_PerspectiveMode.Size = new System.Drawing.Size(40, 28);
      this.toolStripDropDownButton_PerspectiveMode.Text = "3D View";
      this.toolStripDropDownButton_PerspectiveMode.ToolTipText = "View Mode";
      this.toolStripDropDownButton_PerspectiveMode.ButtonClick += new System.EventHandler(this.toolStripDropDownButton_PerspectiveMode_ButtonClick);
      this.toolStripDropDownButton_PerspectiveMode.DropDownOpening += new System.EventHandler(this.toolStripDropDownButton_PerspectiveMode_DropDownOpening);
      // 
      // ToolStripMenuItem_Perspective
      // 
      this.ToolStripMenuItem_Perspective.Image = global::Editor.Properties.Resources.view_perspective;
      this.ToolStripMenuItem_Perspective.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_Perspective.Name = "ToolStripMenuItem_Perspective";
      this.ToolStripMenuItem_Perspective.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_Perspective.Text = "Perspective";
      this.ToolStripMenuItem_Perspective.ToolTipText = "Switches View to Perspective Mode";
      this.ToolStripMenuItem_Perspective.Click += new System.EventHandler(this.perspectiveToolStripMenuItem_Perspective_Click);
      // 
      // ToolStripMenuItem_PerspectiveTop
      // 
      this.ToolStripMenuItem_PerspectiveTop.Image = global::Editor.Properties.Resources.toolbar_zgizmo;
      this.ToolStripMenuItem_PerspectiveTop.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_PerspectiveTop.Name = "ToolStripMenuItem_Perspective";
      this.ToolStripMenuItem_PerspectiveTop.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_PerspectiveTop.Text = "  Orient Top";
      this.ToolStripMenuItem_PerspectiveTop.ToolTipText = "Reorients camera to a top-down perspective view";
      this.ToolStripMenuItem_PerspectiveTop.Click += new System.EventHandler(this.perspectiveToolStripMenuItem_PerspectiveTop_Click);
      // 
      // ToolStripMenuItem_PerspectiveFront
      // 
      this.ToolStripMenuItem_PerspectiveFront.Image = global::Editor.Properties.Resources.toolbar_xgizmo;
      this.ToolStripMenuItem_PerspectiveFront.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_PerspectiveFront.Name = "ToolStripMenuItem_Perspective";
      this.ToolStripMenuItem_PerspectiveFront.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_PerspectiveFront.Text = "  Orient Front";
      this.ToolStripMenuItem_PerspectiveFront.ToolTipText = "Reorients camera to a front-facing perspective view";
      this.ToolStripMenuItem_PerspectiveFront.Click += new System.EventHandler(this.perspectiveToolStripMenuItem_PerspectiveFront_Click);
      // 
      // ToolStripMenuItem_PerspectiveRightB
      // 
      this.ToolStripMenuItem_PerspectiveRight.Image = global::Editor.Properties.Resources.toolbar_ygizmo;
      this.ToolStripMenuItem_PerspectiveRight.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_PerspectiveRight.Name = "ToolStripMenuItem_Perspective";
      this.ToolStripMenuItem_PerspectiveRight.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_PerspectiveRight.Text = "  Orient Right";
      this.ToolStripMenuItem_PerspectiveRight.ToolTipText = "Reorients camera to a right-facing perspective view";
      this.ToolStripMenuItem_PerspectiveRight.Click += new System.EventHandler(this.perspectiveToolStripMenuItem_PerspectiveRight_Click);
      // 
      // ToolStripMenuItem_Top
      // 
      this.ToolStripMenuItem_Top.Image = ((System.Drawing.Image)(resources.GetObject("ToolStripMenuItem_Top.Image")));
      this.ToolStripMenuItem_Top.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_Top.Name = "ToolStripMenuItem_Top";
      this.ToolStripMenuItem_Top.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_Top.Text = "Orthogonal Top";
      this.ToolStripMenuItem_Top.ToolTipText = "Switches View to Orthographic Top View";
      this.ToolStripMenuItem_Top.Click += new System.EventHandler(this.topToolStripMenuItem_Top_Click);
      // 
      // ToolStripMenuItem_Front
      // 
      this.ToolStripMenuItem_Front.Image = global::Editor.Properties.Resources.view_front;
      this.ToolStripMenuItem_Front.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_Front.Name = "ToolStripMenuItem_Front";
      this.ToolStripMenuItem_Front.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_Front.Text = "Orthogonal Front";
      this.ToolStripMenuItem_Front.ToolTipText = "Switches View to Orthographic Front View";
      this.ToolStripMenuItem_Front.Click += new System.EventHandler(this.frontToolStripMenuItem_Front_Click);
      // 
      // ToolStripMenuItem_Right
      // 
      this.ToolStripMenuItem_Right.Image = global::Editor.Properties.Resources.view_right;
      this.ToolStripMenuItem_Right.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_Right.Name = "ToolStripMenuItem_Right";
      this.ToolStripMenuItem_Right.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_Right.Text = "Orthogonal Right";
      this.ToolStripMenuItem_Right.ToolTipText = "Switches View to Orthographic Right View";
      this.ToolStripMenuItem_Right.Click += new System.EventHandler(this.rightToolStripMenuItem_Right_Click);
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(251, 6);
      // 
      // ToolStripMenuItem_View_EditClipRange
      // 
      this.ToolStripMenuItem_View_EditClipRange.Image = global::Editor.Properties.Resources.fit_to_size;
      this.ToolStripMenuItem_View_EditClipRange.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_EditClipRange.Name = "ToolStripMenuItem_View_EditClipRange";
      this.ToolStripMenuItem_View_EditClipRange.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_EditClipRange.Text = "Edit Clip Range (Orthogonal View)";
      this.ToolStripMenuItem_View_EditClipRange.ToolTipText = "Edit Orthographic Clip Range";
      this.ToolStripMenuItem_View_EditClipRange.Click += new System.EventHandler(this.toolStripButton_SelectViewBox_Click);
      // 
      // ToolStripMenuItem_View_ZoomIn
      // 
      this.ToolStripMenuItem_View_ZoomIn.Image = global::Editor.Properties.Resources.zoom_in;
      this.ToolStripMenuItem_View_ZoomIn.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_ZoomIn.Name = "ToolStripMenuItem_View_ZoomIn";
      this.ToolStripMenuItem_View_ZoomIn.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_ZoomIn.Text = "Zoom In (Orthogonal View)";
      this.ToolStripMenuItem_View_ZoomIn.Click += new System.EventHandler(this.toolStripButtonZoomIn_Click);
      // 
      // ToolStripMenuItem_View_ZoomOut
      // 
      this.ToolStripMenuItem_View_ZoomOut.Image = global::Editor.Properties.Resources.zoom_out;
      this.ToolStripMenuItem_View_ZoomOut.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_ZoomOut.Name = "ToolStripMenuItem_View_ZoomOut";
      this.ToolStripMenuItem_View_ZoomOut.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_ZoomOut.Text = "Zoom Out (Orthogonal View)";
      this.ToolStripMenuItem_View_ZoomOut.Click += new System.EventHandler(this.toolStripButton_ZoomOut_Click);
      // 
      // ToolStripMenuItem_View_ZoomDefault
      // 
      this.ToolStripMenuItem_View_ZoomDefault.Image = global::Editor.Properties.Resources.view_1_1;
      this.ToolStripMenuItem_View_ZoomDefault.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_ZoomDefault.Name = "ToolStripMenuItem_View_ZoomDefault";
      this.ToolStripMenuItem_View_ZoomDefault.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_ZoomDefault.Text = "Default Zoom (Orthogonal View)";
      this.ToolStripMenuItem_View_ZoomDefault.Click += new System.EventHandler(this.toolStripButton_Zoom100_Click);
      // 
      // toolStripMenuItem9
      // 
      this.toolStripMenuItem9.Name = "toolStripMenuItem9";
      this.toolStripMenuItem9.Size = new System.Drawing.Size(251, 6);
      // 
      // ToolStripMenuItem_View_Size
      // 
      this.ToolStripMenuItem_View_Size.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_View_SetSize,
            this.toolStripMenuItem12});
      this.ToolStripMenuItem_View_Size.Image = global::Editor.Properties.Resources.fit_to_size;
      this.ToolStripMenuItem_View_Size.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_Size.Name = "ToolStripMenuItem_View_Size";
      this.ToolStripMenuItem_View_Size.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_Size.Text = "Engine View Size";
      this.ToolStripMenuItem_View_Size.ToolTipText = "Set the Engine View Size (Only when panel is undocked)";
      // 
      // ToolStripMenuItem_View_SetSize
      // 
      this.ToolStripMenuItem_View_SetSize.Image = global::Editor.Properties.Resources.fit_to_size;
      this.ToolStripMenuItem_View_SetSize.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_SetSize.Name = "ToolStripMenuItem_View_SetSize";
      this.ToolStripMenuItem_View_SetSize.Size = new System.Drawing.Size(186, 22);
      this.ToolStripMenuItem_View_SetSize.Text = "Set Custom View Size";
      this.ToolStripMenuItem_View_SetSize.Click += new System.EventHandler(this.ToolStripMenuItem_View_SetSize_Click);
      // 
      // toolStripMenuItem12
      // 
      this.toolStripMenuItem12.Name = "toolStripMenuItem12";
      this.toolStripMenuItem12.Size = new System.Drawing.Size(183, 6);
      // 
      // ToolStripMenuItem_View_SafeMode
      // 
      this.ToolStripMenuItem_View_SafeMode.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_View_SetCustomSafeMode,
            this.ToolStripMenuItem_View_DisableSafeMode,
            this.toolStripMenuItem13});
      this.ToolStripMenuItem_View_SafeMode.Image = global::Editor.Properties.Resources.fit_to_size;
      this.ToolStripMenuItem_View_SafeMode.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_SafeMode.Name = "ToolStripMenuItem_View_SafeMode";
      this.ToolStripMenuItem_View_SafeMode.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_SafeMode.Text = "Engine View Safe Mode";
      this.ToolStripMenuItem_View_SafeMode.ToolTipText = "Set the Engine View Safe Mode";
      // 
      // ToolStripMenuItem_View_SetCustomSafeMode
      // 
      this.ToolStripMenuItem_View_SetCustomSafeMode.Image = global::Editor.Properties.Resources.fit_to_size;
      this.ToolStripMenuItem_View_SetCustomSafeMode.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_SetCustomSafeMode.Name = "ToolStripMenuItem_View_SetCustomSafeMode";
      this.ToolStripMenuItem_View_SetCustomSafeMode.Size = new System.Drawing.Size(194, 22);
      this.ToolStripMenuItem_View_SetCustomSafeMode.Text = "Set Custom Safe Mode";
      this.ToolStripMenuItem_View_SetCustomSafeMode.Click += new System.EventHandler(this.ToolStripMenuItem_View_SetSafeMode_Click);
      // 
      // ToolStripMenuItem_View_DisableSafeMode
      // 
      this.ToolStripMenuItem_View_DisableSafeMode.Image = global::Editor.Properties.Resources.fit_to_size;
      this.ToolStripMenuItem_View_DisableSafeMode.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_DisableSafeMode.Name = "ToolStripMenuItem_View_DisableSafeMode";
      this.ToolStripMenuItem_View_DisableSafeMode.Size = new System.Drawing.Size(194, 22);
      this.ToolStripMenuItem_View_DisableSafeMode.Text = "Disable";
      this.ToolStripMenuItem_View_DisableSafeMode.Click += new System.EventHandler(this.disableToolStripMenuItem_Click);
      // 
      // toolStripMenuItem13
      // 
      this.toolStripMenuItem13.Name = "toolStripMenuItem13";
      this.toolStripMenuItem13.Size = new System.Drawing.Size(191, 6);
      // 
      // toolStripMenuItem14
      // 
      this.toolStripMenuItem14.Name = "toolStripMenuItem14";
      this.toolStripMenuItem14.Size = new System.Drawing.Size(251, 6);
      // 
      // ToolStripMenuItem_View_Properties
      // 
      this.ToolStripMenuItem_View_Properties.Image = global::Editor.Properties.Resources.camera2;
      this.ToolStripMenuItem_View_Properties.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripMenuItem_View_Properties.Name = "ToolStripMenuItem_View_Properties";
      this.ToolStripMenuItem_View_Properties.Size = new System.Drawing.Size(254, 22);
      this.ToolStripMenuItem_View_Properties.Text = "Viewport Preview Settings";
      this.ToolStripMenuItem_View_Properties.ToolTipText = "Configure Viewport Preview Settings (FOV or Filtering)";
      this.ToolStripMenuItem_View_Properties.Click += new System.EventHandler(this.viewPropertiesToolStripMenuItem_Click);
      // 
      // ToolStripSplitButton_Tools
      // 
      this.ToolStripSplitButton_Tools.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.ToolStripSplitButton_Tools.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_MeasureTool,
            this.ToolStripSplitButton_Tools_Sep1,
            this.ToolStripButton_HighlightSelection,
            this.ToolStripButton_OneClickSelection});
      this.ToolStripSplitButton_Tools.Image = global::Editor.Properties.Resources.toolbar_tool;
      this.ToolStripSplitButton_Tools.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripSplitButton_Tools.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.ToolStripSplitButton_Tools.Name = "ToolStripSplitButton_Tools";
      this.ToolStripSplitButton_Tools.Size = new System.Drawing.Size(40, 28);
      this.ToolStripSplitButton_Tools.Text = "Tools";
      this.ToolStripSplitButton_Tools.DropDownOpening += new System.EventHandler(this.ToolStripSplitButton_Tools_DropDownOpening);
      // 
      // toolStripButton_MeasureTool
      // 
      this.toolStripButton_MeasureTool.Image = global::Editor.Properties.Resources.toolbar_measure;
      this.toolStripButton_MeasureTool.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.toolStripButton_MeasureTool.Name = "toolStripButton_MeasureTool";
      this.toolStripButton_MeasureTool.Size = new System.Drawing.Size(176, 22);
      this.toolStripButton_MeasureTool.Text = "Measure Tool";
      this.toolStripButton_MeasureTool.ToolTipText = "Simple picker tool to measure the distance between two positions";
      this.toolStripButton_MeasureTool.Click += new System.EventHandler(this.toolStripButton_MeasureTool_Click);
      // 
      // ToolStripSplitButton_Tools_Sep1
      // 
      this.ToolStripSplitButton_Tools_Sep1.Name = "ToolStripSplitButton_Tools_Sep1";
      this.ToolStripSplitButton_Tools_Sep1.Size = new System.Drawing.Size(173, 6);
      // 
      // ToolStripButton_HighlightSelection
      // 
      this.ToolStripButton_HighlightSelection.CheckOnClick = true;
      this.ToolStripButton_HighlightSelection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_HighlightSelection.Name = "ToolStripButton_HighlightSelection";
      this.ToolStripButton_HighlightSelection.Size = new System.Drawing.Size(176, 22);
      this.ToolStripButton_HighlightSelection.Text = "Highlight Selection";
      this.ToolStripButton_HighlightSelection.Click += new System.EventHandler(this.ToolStripSplitButton_HighlightSelection_Click);
      // 
      // ToolStripButton_OneClickSelection
      // 
      this.ToolStripButton_OneClickSelection.CheckOnClick = true;
      this.ToolStripButton_OneClickSelection.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.ToolStripButton_OneClickSelection.Name = "ToolStripButton_OneClickSelection";
      this.ToolStripButton_OneClickSelection.Size = new System.Drawing.Size(176, 22);
      this.ToolStripButton_OneClickSelection.Text = "One Click Selection";
      this.ToolStripButton_OneClickSelection.ToolTipText = "If enabled, one click selection is also possible in the vForge camera modes.";
      this.ToolStripButton_OneClickSelection.Click += new System.EventHandler(this.ToolStripButton_OneClickSelection_Click);
      // 
      // toolStripSeparator4
      // 
      this.toolStripSeparator4.Name = "toolStripSeparator4";
      this.toolStripSeparator4.Size = new System.Drawing.Size(6, 31);
      // 
      // contextMenuStrip_SelShapes
      // 
      this.contextMenuStrip_SelShapes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.transformGizmoToolStripMenuItem,
            this.AlignmentToolStripMenuItem,
            this.dropToFloorToolStripMenuItem,
            this.linksToolStripMenuItem,
            this.moveToToolStripMenuItem,
            this.toolStripMenuItem2,
            this.selectInTreeViewToolStripMenuItem,
            this.lockShapesToolStripMenuItem,
            this.toolStripMenuItem3,
            this.cutToolStripMenuItem,
            this.copyToolStripMenuItem,
            this.deleteToolStripMenuItem});
      this.contextMenuStrip_SelShapes.Name = "contextMenuStrip1";
      this.contextMenuStrip_SelShapes.Size = new System.Drawing.Size(173, 236);
      this.contextMenuStrip_SelShapes.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_SelShapes_Opening);
      // 
      // transformGizmoToolStripMenuItem
      // 
      this.transformGizmoToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.moveToolStripMenuItem,
            this.rotateToolStripMenuItem,
            this.scaleToolStripMenuItem});
      this.transformGizmoToolStripMenuItem.Image = global::Editor.Properties.Resources.gizmo;
      this.transformGizmoToolStripMenuItem.Name = "transformGizmoToolStripMenuItem";
      this.transformGizmoToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.transformGizmoToolStripMenuItem.Text = "Transform Gizmo";
      // 
      // moveToolStripMenuItem
      // 
      this.moveToolStripMenuItem.Image = global::Editor.Properties.Resources.MoveShape;
      this.moveToolStripMenuItem.Name = "moveToolStripMenuItem";
      this.moveToolStripMenuItem.Size = new System.Drawing.Size(108, 22);
      this.moveToolStripMenuItem.Text = "Move";
      this.moveToolStripMenuItem.Click += new System.EventHandler(this.toolStripButton_Move_Click);
      // 
      // rotateToolStripMenuItem
      // 
      this.rotateToolStripMenuItem.Image = global::Editor.Properties.Resources.RotateShape;
      this.rotateToolStripMenuItem.Name = "rotateToolStripMenuItem";
      this.rotateToolStripMenuItem.Size = new System.Drawing.Size(108, 22);
      this.rotateToolStripMenuItem.Text = "Rotate";
      this.rotateToolStripMenuItem.Click += new System.EventHandler(this.toolStripButton_Rotate_Click);
      // 
      // scaleToolStripMenuItem
      // 
      this.scaleToolStripMenuItem.Image = global::Editor.Properties.Resources.ScaleShape;
      this.scaleToolStripMenuItem.Name = "scaleToolStripMenuItem";
      this.scaleToolStripMenuItem.Size = new System.Drawing.Size(108, 22);
      this.scaleToolStripMenuItem.Text = "Scale";
      this.scaleToolStripMenuItem.Click += new System.EventHandler(this.toolStripButton_Scale_Click);
      // 
      // AlignmentToolStripMenuItem
      // 
      this.AlignmentToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.AlignToGridToolStripMenuItem,
            this.AlignToObjectToolStripMenuItem,
            this.AlignToHitPointToolStripMenuItem,
            this.AlignToHitPointNormalToolStripMenuItem});
      this.AlignmentToolStripMenuItem.Image = global::Editor.Properties.Resources.align_16;
      this.AlignmentToolStripMenuItem.Name = "AlignmentToolStripMenuItem";
      this.AlignmentToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.AlignmentToolStripMenuItem.Text = "Alignment";
      // 
      // AlignToGridToolStripMenuItem
      // 
      this.AlignToGridToolStripMenuItem.Image = global::Editor.Properties.Resources.setup_grid;
      this.AlignToGridToolStripMenuItem.Name = "AlignToGridToolStripMenuItem";
      this.AlignToGridToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
      this.AlignToGridToolStripMenuItem.Text = "Align to Grid";
      this.AlignToGridToolStripMenuItem.ToolTipText = "Snaps Selected Shape(s) to the Grid";
      this.AlignToGridToolStripMenuItem.Click += new System.EventHandler(this.AlignToGridToolStripMenuItem_Click);
      // 
      // AlignToObjectToolStripMenuItem
      // 
      this.AlignToObjectToolStripMenuItem.Image = global::Editor.Properties.Resources.align_16;
      this.AlignToObjectToolStripMenuItem.Name = "AlignToObjectToolStripMenuItem";
      this.AlignToObjectToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
      this.AlignToObjectToolStripMenuItem.Text = "Align to Object";
      this.AlignToObjectToolStripMenuItem.ToolTipText = "Aligns Selected Shape(s) to another Shape";
      this.AlignToObjectToolStripMenuItem.Click += new System.EventHandler(this.AlignToObjectToolStripMenuItem_Click);
      // 
      // AlignToHitPointToolStripMenuItem
      // 
      this.AlignToHitPointToolStripMenuItem.Image = global::Editor.Properties.Resources.node;
      this.AlignToHitPointToolStripMenuItem.Name = "AlignToHitPointToolStripMenuItem";
      this.AlignToHitPointToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
      this.AlignToHitPointToolStripMenuItem.Text = "Align to Hit Point";
      this.AlignToHitPointToolStripMenuItem.ToolTipText = "Aligns selected Shape(s) to the Hit Point traced in the Engine View";
      this.AlignToHitPointToolStripMenuItem.Click += new System.EventHandler(this.AlignToHitPointToolStripMenuItem_Click);
      // 
      // AlignToHitPointNormalToolStripMenuItem
      // 
      this.AlignToHitPointNormalToolStripMenuItem.Image = global::Editor.Properties.Resources.align_normal;
      this.AlignToHitPointNormalToolStripMenuItem.Name = "AlignToHitPointNormalToolStripMenuItem";
      this.AlignToHitPointNormalToolStripMenuItem.Size = new System.Drawing.Size(209, 22);
      this.AlignToHitPointNormalToolStripMenuItem.Text = "Align to Hit Point Normal";
      this.AlignToHitPointNormalToolStripMenuItem.ToolTipText = "Aligns selected Shape(s) to the Hit Point traced in the Engine View (Takes Normal" +
    " Vector into account)";
      this.AlignToHitPointNormalToolStripMenuItem.Click += new System.EventHandler(this.AlignToHitPointNormalToolStripMenuItem_Click);
      // 
      // dropToFloorToolStripMenuItem
      // 
      this.dropToFloorToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.pivotToolStripMenuItem,
            this.boundingBoxToolStripMenuItem,
            this.bottomCenterToolStripMenuItem,
            this.toolStripMenuItem8,
            this.directionFloorToolStripMenuItem,
            this.directionCeilingToolStripMenuItem,
            this.directionPosXToolStripMenuItem,
            this.directionNegXToolStripMenuItem,
            this.directionPosYToolStripMenuItem,
            this.directionNegYToolStripMenuItem});
      this.dropToFloorToolStripMenuItem.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.dropToFloorToolStripMenuItem.Name = "dropToFloorToolStripMenuItem";
      this.dropToFloorToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.dropToFloorToolStripMenuItem.Text = "Drop to Floor";
      this.dropToFloorToolStripMenuItem.ToolTipText = "Drop the shape(s) until they collide with floor geometry";
      this.dropToFloorToolStripMenuItem.DropDownOpening += new System.EventHandler(this.dropToFloorToolStripMenuItem_DropDownOpening);
      // 
      // pivotToolStripMenuItem
      // 
      this.pivotToolStripMenuItem.Name = "pivotToolStripMenuItem";
      this.pivotToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.pivotToolStripMenuItem.Text = "Pivot";
      this.pivotToolStripMenuItem.ToolTipText = "Uses the shape\'s pivot";
      this.pivotToolStripMenuItem.Click += new System.EventHandler(this.pivotToolStripMenuItem_Click);
      // 
      // boundingBoxToolStripMenuItem
      // 
      this.boundingBoxToolStripMenuItem.Name = "boundingBoxToolStripMenuItem";
      this.boundingBoxToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.boundingBoxToolStripMenuItem.Text = "Bounding box";
      this.boundingBoxToolStripMenuItem.ToolTipText = "Uses the full bounding box for collision";
      this.boundingBoxToolStripMenuItem.Click += new System.EventHandler(this.boundingBoxToolStripMenuItem_Click);
      // 
      // bottomCenterToolStripMenuItem
      // 
      this.bottomCenterToolStripMenuItem.Name = "bottomCenterToolStripMenuItem";
      this.bottomCenterToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.bottomCenterToolStripMenuItem.Text = "Bottom center";
      this.bottomCenterToolStripMenuItem.ToolTipText = "Uses the bottom center for collision";
      this.bottomCenterToolStripMenuItem.Click += new System.EventHandler(this.bottomCenterToolStripMenuItem_Click);
      // 
      // toolStripMenuItem8
      // 
      this.toolStripMenuItem8.Name = "toolStripMenuItem8";
      this.toolStripMenuItem8.Size = new System.Drawing.Size(165, 6);
      // 
      // directionFloorToolStripMenuItem
      // 
      this.directionFloorToolStripMenuItem.Name = "directionFloorToolStripMenuItem";
      this.directionFloorToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionFloorToolStripMenuItem.Text = "Direction : Floor";
      this.directionFloorToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionCeilingToolStripMenuItem
      // 
      this.directionCeilingToolStripMenuItem.Name = "directionCeilingToolStripMenuItem";
      this.directionCeilingToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionCeilingToolStripMenuItem.Text = "Direction : Ceiling";
      this.directionCeilingToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionPosXToolStripMenuItem
      // 
      this.directionPosXToolStripMenuItem.Name = "directionPosXToolStripMenuItem";
      this.directionPosXToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionPosXToolStripMenuItem.Text = "Direction : Pos X";
      this.directionPosXToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionNegXToolStripMenuItem
      // 
      this.directionNegXToolStripMenuItem.Name = "directionNegXToolStripMenuItem";
      this.directionNegXToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionNegXToolStripMenuItem.Text = "Direction : Neg X";
      this.directionNegXToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionPosYToolStripMenuItem
      // 
      this.directionPosYToolStripMenuItem.Name = "directionPosYToolStripMenuItem";
      this.directionPosYToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionPosYToolStripMenuItem.Text = "Direction : Pos Y";
      this.directionPosYToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // directionNegYToolStripMenuItem
      // 
      this.directionNegYToolStripMenuItem.Name = "directionNegYToolStripMenuItem";
      this.directionNegYToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.directionNegYToolStripMenuItem.Text = "Direction : Neg Y";
      this.directionNegYToolStripMenuItem.Click += new System.EventHandler(this.directionFloorToolStripMenuItem_Click);
      // 
      // linksToolStripMenuItem
      // 
      this.linksToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.unlinkFromSourcesToolStripMenuItem,
            this.unlinkAllTargetsToolStripMenuItem});
      this.linksToolStripMenuItem.Image = global::Editor.Properties.Resources.links;
      this.linksToolStripMenuItem.Name = "linksToolStripMenuItem";
      this.linksToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.linksToolStripMenuItem.Text = "Links";
      // 
      // unlinkFromSourcesToolStripMenuItem
      // 
      this.unlinkFromSourcesToolStripMenuItem.Name = "unlinkFromSourcesToolStripMenuItem";
      this.unlinkFromSourcesToolStripMenuItem.Size = new System.Drawing.Size(181, 22);
      this.unlinkFromSourcesToolStripMenuItem.Text = "Unlink from Sources";
      this.unlinkFromSourcesToolStripMenuItem.ToolTipText = "Remove all link sources";
      this.unlinkFromSourcesToolStripMenuItem.Click += new System.EventHandler(this.unlinkFromSourcesToolStripMenuItem_Click);
      // 
      // unlinkAllTargetsToolStripMenuItem
      // 
      this.unlinkAllTargetsToolStripMenuItem.Name = "unlinkAllTargetsToolStripMenuItem";
      this.unlinkAllTargetsToolStripMenuItem.Size = new System.Drawing.Size(181, 22);
      this.unlinkAllTargetsToolStripMenuItem.Text = "Unlink all Targets";
      this.unlinkAllTargetsToolStripMenuItem.ToolTipText = "Remove all targets";
      this.unlinkAllTargetsToolStripMenuItem.Click += new System.EventHandler(this.unlinkAllTargetsToolStripMenuItem_Click);
      // 
      // moveToToolStripMenuItem
      // 
      this.moveToToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cameraPositionToolStripMenuItem,
            this.shapeOriginToolStripMenuItem});
      this.moveToToolStripMenuItem.Image = global::Editor.Properties.Resources.gizmo;
      this.moveToToolStripMenuItem.Name = "moveToToolStripMenuItem";
      this.moveToToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.moveToToolStripMenuItem.Text = "Move to";
      // 
      // cameraPositionToolStripMenuItem
      // 
      this.cameraPositionToolStripMenuItem.Name = "cameraPositionToolStripMenuItem";
      this.cameraPositionToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
      this.cameraPositionToolStripMenuItem.Text = "Camera Position";
      this.cameraPositionToolStripMenuItem.Click += new System.EventHandler(this.cameraPositionToolStripMenuItem_Click);
      // 
      // shapeOriginToolStripMenuItem
      // 
      this.shapeOriginToolStripMenuItem.Name = "shapeOriginToolStripMenuItem";
      this.shapeOriginToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
      this.shapeOriginToolStripMenuItem.Text = "Shape Origin";
      this.shapeOriginToolStripMenuItem.Click += new System.EventHandler(this.shapeOriginToolStripMenuItem_Click);
      // 
      // toolStripMenuItem2
      // 
      this.toolStripMenuItem2.Name = "toolStripMenuItem2";
      this.toolStripMenuItem2.Size = new System.Drawing.Size(169, 6);
      // 
      // selectInTreeViewToolStripMenuItem
      // 
      this.selectInTreeViewToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes_icon;
      this.selectInTreeViewToolStripMenuItem.Name = "selectInTreeViewToolStripMenuItem";
      this.selectInTreeViewToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.selectInTreeViewToolStripMenuItem.Text = "Select in Tree View";
      this.selectInTreeViewToolStripMenuItem.Click += new System.EventHandler(this.selectInTreeViewToolStripMenuItem_Click);
      // 
      // lockShapesToolStripMenuItem
      // 
      this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
      this.lockShapesToolStripMenuItem.Name = "lockShapesToolStripMenuItem";
      this.lockShapesToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.lockShapesToolStripMenuItem.Text = "Lock Shapes";
      this.lockShapesToolStripMenuItem.Click += new System.EventHandler(this.lockShapesToolStripMenuItem_Click);
      // 
      // toolStripMenuItem3
      // 
      this.toolStripMenuItem3.Name = "toolStripMenuItem3";
      this.toolStripMenuItem3.Size = new System.Drawing.Size(169, 6);
      // 
      // cutToolStripMenuItem
      // 
      this.cutToolStripMenuItem.Image = global::Editor.Properties.Resources.cut;
      this.cutToolStripMenuItem.Name = "cutToolStripMenuItem";
      this.cutToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.cutToolStripMenuItem.Text = "Cut";
      this.cutToolStripMenuItem.ToolTipText = "Cut the selected shape(s)";
      this.cutToolStripMenuItem.Click += new System.EventHandler(this.cutToolStripMenuItem_Click);
      // 
      // copyToolStripMenuItem
      // 
      this.copyToolStripMenuItem.Image = global::Editor.Properties.Resources.copy;
      this.copyToolStripMenuItem.Name = "copyToolStripMenuItem";
      this.copyToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.copyToolStripMenuItem.Text = "Copy";
      this.copyToolStripMenuItem.ToolTipText = "Copy the selected shape(s)";
      this.copyToolStripMenuItem.Click += new System.EventHandler(this.copyToolStripMenuItem_Click);
      // 
      // deleteToolStripMenuItem
      // 
      this.deleteToolStripMenuItem.Image = global::Editor.Properties.Resources.delete2;
      this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
      this.deleteToolStripMenuItem.Size = new System.Drawing.Size(172, 22);
      this.deleteToolStripMenuItem.Text = "Delete";
      this.deleteToolStripMenuItem.ToolTipText = "Delete the selected shape(s)";
      this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
      // 
      // contextMenuStrip_View
      // 
      this.contextMenuStrip_View.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.pasteAtCursorToolStripMenuItem,
            this.toolStripMenuItem4,
            this.gridToolStripMenuItem,
            this.toolStripMenuItem5,
            this.renderingToolStripMenuItem,
            this.shadingToolStripMenuItem,
            this.debugFlagsToolStripMenuItem,
            this.pickToolStripMenuItem,
            this.toolStripMenuItem6,
            this.createToolStripMenuItem,
            this.shapeOriginToolStripMenuItem1,
            this.zonesToolStripMenuItem});
      this.contextMenuStrip_View.Name = "contextMenuStrip_View";
      this.contextMenuStrip_View.Size = new System.Drawing.Size(175, 242);
      this.contextMenuStrip_View.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_View_Opening);
      // 
      // pasteAtCursorToolStripMenuItem
      // 
      this.pasteAtCursorToolStripMenuItem.Image = global::Editor.Properties.Resources.paste;
      this.pasteAtCursorToolStripMenuItem.Name = "pasteAtCursorToolStripMenuItem";
      this.pasteAtCursorToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.pasteAtCursorToolStripMenuItem.Text = "Paste at Cursor";
      this.pasteAtCursorToolStripMenuItem.ToolTipText = "Pastes the current clipboard content to the scene position under the cursor";
      this.pasteAtCursorToolStripMenuItem.Click += new System.EventHandler(this.pasteAtCursorToolStripMenuItem_Click);
      // 
      // toolStripMenuItem4
      // 
      this.toolStripMenuItem4.Name = "toolStripMenuItem4";
      this.toolStripMenuItem4.Size = new System.Drawing.Size(171, 6);
      // 
      // gridToolStripMenuItem
      // 
      this.gridToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.setupGridToolStripMenuItem,
            this.toolStripMenuItem10,
            this.showGridToolStripMenuItem,
            this.snapPositionToolStripMenuItem,
            this.snapRotationToolStripMenuItem,
            this.relativeGridToolStripMenuItem,
            this.gridLinesToolStripMenuItem,
            this.volumeGridToolStripMenuItem,
            this.toolStripMenuItem7,
            this.gridSize10ToolStripMenuItem,
            this.gridSize25ToolStripMenuItem,
            this.gridSize100ToolStripMenuItem,
            this.toolStripMenuItem15,
            this.units2ToolStripMenuItem,
            this.units5ToolStripMenuItem,
            this.units10ToolStripMenuItem,
            this.units25ToolStripMenuItem,
            this.units50ToolStripMenuItem,
            this.units100ToolStripMenuItem});
      this.gridToolStripMenuItem.Image = global::Editor.Properties.Resources.setup_grid;
      this.gridToolStripMenuItem.Name = "gridToolStripMenuItem";
      this.gridToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.gridToolStripMenuItem.Text = "Grid";
      // 
      // setupGridToolStripMenuItem
      // 
      this.setupGridToolStripMenuItem.Image = global::Editor.Properties.Resources.setup_grid;
      this.setupGridToolStripMenuItem.Name = "setupGridToolStripMenuItem";
      this.setupGridToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.setupGridToolStripMenuItem.Text = "Setup Grid";
      this.setupGridToolStripMenuItem.ToolTipText = "Setup the Grid Snapping Options for Move and Rotation Grids";
      this.setupGridToolStripMenuItem.Click += new System.EventHandler(this.setupGridToolStripMenuItem_Click);
      // 
      // toolStripMenuItem10
      // 
      this.toolStripMenuItem10.Name = "toolStripMenuItem10";
      this.toolStripMenuItem10.Size = new System.Drawing.Size(126, 6);
      // 
      // snapPositionToolStripMenuItem
      // 
      this.snapPositionToolStripMenuItem.Name = "snapPositionToolStripMenuItem";
      this.snapPositionToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.snapPositionToolStripMenuItem.Text = "Snap Position";
      this.snapPositionToolStripMenuItem.ToolTipText = "Enables/disables position snapping";
      this.snapPositionToolStripMenuItem.Click += new System.EventHandler(this.snapPositionToolStripMenuItem_Click);
      // 
      // snapRotationToolStripMenuItem
      // 
      this.snapRotationToolStripMenuItem.Name = "snapRotationToolStripMenuItem";
      this.snapRotationToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.snapRotationToolStripMenuItem.Text = "Snap Rotation";
      this.snapRotationToolStripMenuItem.ToolTipText = "Enables/disables rotation snapping";
      this.snapRotationToolStripMenuItem.Click += new System.EventHandler(this.snapRotationToolStripMenuItem_Click);
      // 
      // showGridToolStripMenuItem
      // 
      this.showGridToolStripMenuItem.Name = "showGridToolStripMenuItem";
      this.showGridToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.showGridToolStripMenuItem.Text = "Show Grid";
      this.showGridToolStripMenuItem.ToolTipText = "Display the grid around the selection";
      this.showGridToolStripMenuItem.Click += new System.EventHandler(this.showGridToolStripMenuItem_Click);
      // 
      // relativeGridToolStripMenuItem
      // 
      this.relativeGridToolStripMenuItem.Name = "relativeGridToolStripMenuItem";
      this.relativeGridToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.relativeGridToolStripMenuItem.Text = "Relative Grid";
      this.relativeGridToolStripMenuItem.ToolTipText = "Whether to draw the grid relative to the gizmo, or centered at the origin";
      this.relativeGridToolStripMenuItem.Click += new System.EventHandler(this.relativeGridToolStripMenuItem_Click);
      // 
      // gridLinesToolStripMenuItem
      // 
      this.gridLinesToolStripMenuItem.Name = "gridLinesToolStripMenuItem";
      this.gridLinesToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.gridLinesToolStripMenuItem.Text = "Draw Lines";
      this.gridLinesToolStripMenuItem.ToolTipText = "Whether to draw the grid as lines or points";
      this.gridLinesToolStripMenuItem.Click += new System.EventHandler(this.gridLinesToolStripMenuItem_Click);
      // 
      // volumeGridToolStripMenuItem
      // 
      this.volumeGridToolStripMenuItem.Name = "volumeGridToolStripMenuItem";
      this.volumeGridToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.volumeGridToolStripMenuItem.Text = "Volume (3D) Grid";
      this.volumeGridToolStripMenuItem.ToolTipText = "Whether to draw and snap against a volumetric or planar grid";
      this.volumeGridToolStripMenuItem.Click += new System.EventHandler(this.volumeGridToolStripMenuItem_Click);
      // 
      // toolStripMenuItem7
      // 
      this.toolStripMenuItem7.Name = "toolStripMenuItem7";
      this.toolStripMenuItem7.Size = new System.Drawing.Size(126, 6);

      // 
      // gridSize10ToolStripMenuItem
      // 
      this.gridSize10ToolStripMenuItem.Name = "units2ToolStripMenuItem";
      this.gridSize10ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.gridSize10ToolStripMenuItem.Text = "Grid Size: 10 x 10";
      this.gridSize10ToolStripMenuItem.ToolTipText = "Sets grid size to 10 x 10 cells";
      this.gridSize10ToolStripMenuItem.Click += new System.EventHandler(this.gridSizeToolStripMenuItem_Click);
      // 
      // gridSize25ToolStripMenuItem
      // 
      this.gridSize25ToolStripMenuItem.Name = "units10ToolStripMenuItem";
      this.gridSize25ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.gridSize25ToolStripMenuItem.Text = "Grid Size: 25 x 25";
      this.gridSize25ToolStripMenuItem.ToolTipText = "Sets grid size to 25 x 25 cells";
      this.gridSize25ToolStripMenuItem.Click += new System.EventHandler(this.gridSizeToolStripMenuItem1_Click);
      // 
      // gridSize100ToolStripMenuItem
      // 
      this.gridSize100ToolStripMenuItem.Name = "units10ToolStripMenuItem";
      this.gridSize100ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.gridSize100ToolStripMenuItem.Text = "Grid Size: 100 x 100";
      this.gridSize100ToolStripMenuItem.ToolTipText = "Sets grid size to 100 x 100 cells)";
      this.gridSize100ToolStripMenuItem.Click += new System.EventHandler(this.gridSizeToolStripMenuItem2_Click);
      // 
      // toolStripMenuItem15
      // 
      this.toolStripMenuItem15.Name = "toolStripMenuItem15";
      this.toolStripMenuItem15.Size = new System.Drawing.Size(126, 6);
      // 
      // units2ToolStripMenuItem
      // 
      this.units2ToolStripMenuItem.Name = "units2ToolStripMenuItem";
      this.units2ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.units2ToolStripMenuItem.Text = "2 Units";
      this.units2ToolStripMenuItem.ToolTipText = "Pre-defined grid of 2 units";
      this.units2ToolStripMenuItem.Click += new System.EventHandler(this.unitsToolStripMenuItem_Click);
      // 
      // units5ToolStripMenuItem
      // 
      this.units5ToolStripMenuItem.Name = "units5ToolStripMenuItem";
      this.units5ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.units5ToolStripMenuItem.Text = "5 Units";
      this.units5ToolStripMenuItem.ToolTipText = "Pre-defined grid of 5 units";
      this.units5ToolStripMenuItem.Click += new System.EventHandler(this.unitsToolStripMenuItem1_Click);
      // 
      // units10ToolStripMenuItem
      // 
      this.units10ToolStripMenuItem.Name = "units10ToolStripMenuItem";
      this.units10ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.units10ToolStripMenuItem.Text = "10 Units";
      this.units10ToolStripMenuItem.ToolTipText = "Pre-defined grid of 10 units";
      this.units10ToolStripMenuItem.Click += new System.EventHandler(this.unitsToolStripMenuItem2_Click);
      // 
      // units25ToolStripMenuItem
      // 
      this.units25ToolStripMenuItem.Name = "units25ToolStripMenuItem";
      this.units25ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.units25ToolStripMenuItem.Text = "25 Units";
      this.units25ToolStripMenuItem.ToolTipText = "Pre-defined grid of 25 units";
      this.units25ToolStripMenuItem.Click += new System.EventHandler(this.unitsToolStripMenuItem3_Click);
      // 
      // units50ToolStripMenuItem
      // 
      this.units50ToolStripMenuItem.Name = "units50ToolStripMenuItem";
      this.units50ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.units50ToolStripMenuItem.Text = "50 Units";
      this.units50ToolStripMenuItem.ToolTipText = "Pre-defined grid of 50 units";
      this.units50ToolStripMenuItem.Click += new System.EventHandler(this.unitsToolStripMenuItem4_Click);
      // 
      // units100ToolStripMenuItem
      // 
      this.units100ToolStripMenuItem.Name = "units100ToolStripMenuItem";
      this.units100ToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.units100ToolStripMenuItem.Text = "100 Units";
      this.units100ToolStripMenuItem.ToolTipText = "Pre-defined grid of 100 units";
      this.units100ToolStripMenuItem.Click += new System.EventHandler(this.unitsToolStripMenuItem5_Click);
      // 
      // toolStripMenuItem5
      // 
      this.toolStripMenuItem5.Name = "toolStripMenuItem5";
      this.toolStripMenuItem5.Size = new System.Drawing.Size(171, 6);
      // 
      // renderingToolStripMenuItem
      // 
      this.renderingToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.solidToolStripMenuItem,
            this.wireframeToolStripMenuItem});
      this.renderingToolStripMenuItem.Image = global::Editor.Properties.Resources.view_default;
      this.renderingToolStripMenuItem.Name = "renderingToolStripMenuItem";
      this.renderingToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.renderingToolStripMenuItem.Text = "Rendering";
      this.renderingToolStripMenuItem.ToolTipText = "Rasterizer options fotr the view";
      // 
      // solidToolStripMenuItem
      // 
      this.solidToolStripMenuItem.Image = global::Editor.Properties.Resources.view_solid;
      this.solidToolStripMenuItem.Name = "solidToolStripMenuItem";
      this.solidToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.solidToolStripMenuItem.Text = "Solid";
      this.solidToolStripMenuItem.Click += new System.EventHandler(this.solidToolStripMenuItem_Click);
      // 
      // wireframeToolStripMenuItem
      // 
      this.wireframeToolStripMenuItem.Image = global::Editor.Properties.Resources.view_wireframe;
      this.wireframeToolStripMenuItem.Name = "wireframeToolStripMenuItem";
      this.wireframeToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
      this.wireframeToolStripMenuItem.Text = "Wireframe";
      this.wireframeToolStripMenuItem.Click += new System.EventHandler(this.wireframeToolStripMenuItem_Click);
      // 
      // shadingToolStripMenuItem
      // 
      this.shadingToolStripMenuItem.Image = global::Editor.Properties.Resources.view_solid;
      this.shadingToolStripMenuItem.Name = "shadingToolStripMenuItem";
      this.shadingToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.shadingToolStripMenuItem.Text = "Shading";
      this.shadingToolStripMenuItem.ToolTipText = "Replaces the rendering with a renderloop that applies shading options to the rend" +
    "ered geometry";

      // 
      // debugFlagsToolStripMenuItem
      // 
      this.debugFlagsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.dummyToolStripMenuItem});
      this.debugFlagsToolStripMenuItem.Image = global::Editor.Properties.Resources.flags;
      this.debugFlagsToolStripMenuItem.Name = "debugFlagsToolStripMenuItem";
      this.debugFlagsToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.debugFlagsToolStripMenuItem.Text = "Debug Flags";
      this.debugFlagsToolStripMenuItem.ToolTipText = "Displays debug information using the engine\'s profiling API";
      // 
      // dummyToolStripMenuItem
      // 
      this.dummyToolStripMenuItem.Name = "dummyToolStripMenuItem";
      this.dummyToolStripMenuItem.Size = new System.Drawing.Size(116, 22);
      this.dummyToolStripMenuItem.Text = "dummy";
      // 
      // pickToolStripMenuItem
      // 
      this.pickToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.Pick_UV0_ToolStripMenuItem,
            this.Pick_UV1_ToolStripMenuItem});
      this.pickToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("pickToolStripMenuItem.Image")));
      this.pickToolStripMenuItem.Name = "pickToolStripMenuItem";
      this.pickToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.pickToolStripMenuItem.Text = "Pick";
      // 
      // Pick_UV0_ToolStripMenuItem
      // 
      this.Pick_UV0_ToolStripMenuItem.Name = "Pick_UV0_ToolStripMenuItem";
      this.Pick_UV0_ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
      this.Pick_UV0_ToolStripMenuItem.Text = "UV0";
      this.Pick_UV0_ToolStripMenuItem.Click += new System.EventHandler(this.Pick_UV0_ToolStripMenuItem_Click);
      // 
      // Pick_UV1_ToolStripMenuItem
      // 
      this.Pick_UV1_ToolStripMenuItem.Name = "Pick_UV1_ToolStripMenuItem";
      this.Pick_UV1_ToolStripMenuItem.Size = new System.Drawing.Size(162, 22);
      this.Pick_UV1_ToolStripMenuItem.Text = "UV1 (Lightmaps)";
      this.Pick_UV1_ToolStripMenuItem.Click += new System.EventHandler(this.Pick_UV1_ToolStripMenuItem_Click);
      // 
      // toolStripMenuItem6
      // 
      this.toolStripMenuItem6.Name = "toolStripMenuItem6";
      this.toolStripMenuItem6.Size = new System.Drawing.Size(171, 6);
      // 
      // createToolStripMenuItem
      // 
      this.createToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.insertHereToolStripMenuItem});
      this.createToolStripMenuItem.Image = global::Editor.Properties.Resources.pawn_new;
      this.createToolStripMenuItem.Name = "createToolStripMenuItem";
      this.createToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.createToolStripMenuItem.Text = "Create";
      this.createToolStripMenuItem.ToolTipText = "All shape creator plugins";
      // 
      // insertHereToolStripMenuItem
      // 
      this.insertHereToolStripMenuItem.Name = "insertHereToolStripMenuItem";
      this.insertHereToolStripMenuItem.Size = new System.Drawing.Size(145, 22);
      this.insertHereToolStripMenuItem.Text = "<insert here>";
      // 
      // shapeOriginToolStripMenuItem1
      // 
      this.shapeOriginToolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.sceneOriginToolStripMenuItem,
            this.cameraPositionToolStripMenuItem1,
            this.traceHitToolStripMenuItem});
      this.shapeOriginToolStripMenuItem1.Image = global::Editor.Properties.Resources.node;
      this.shapeOriginToolStripMenuItem1.Name = "shapeOriginToolStripMenuItem1";
      this.shapeOriginToolStripMenuItem1.Size = new System.Drawing.Size(174, 22);
      this.shapeOriginToolStripMenuItem1.Text = "Shape Origin";
      this.shapeOriginToolStripMenuItem1.ToolTipText = "Sets a new shape spawn origin";
      // 
      // sceneOriginToolStripMenuItem
      // 
      this.sceneOriginToolStripMenuItem.Name = "sceneOriginToolStripMenuItem";
      this.sceneOriginToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
      this.sceneOriginToolStripMenuItem.Text = "Scene Origin";
      this.sceneOriginToolStripMenuItem.ToolTipText = "Uses (0,0,0) as a new origin";
      this.sceneOriginToolStripMenuItem.Click += new System.EventHandler(this.sceneOriginToolStripMenuItem_Click);
      // 
      // cameraPositionToolStripMenuItem1
      // 
      this.cameraPositionToolStripMenuItem1.Name = "cameraPositionToolStripMenuItem1";
      this.cameraPositionToolStripMenuItem1.Size = new System.Drawing.Size(161, 22);
      this.cameraPositionToolStripMenuItem1.Text = "Camera Position";
      this.cameraPositionToolStripMenuItem1.ToolTipText = "Uses the current camera position";
      this.cameraPositionToolStripMenuItem1.Click += new System.EventHandler(this.cameraPositionToolStripMenuItem1_Click);
      // 
      // traceHitToolStripMenuItem
      // 
      this.traceHitToolStripMenuItem.Name = "traceHitToolStripMenuItem";
      this.traceHitToolStripMenuItem.Size = new System.Drawing.Size(161, 22);
      this.traceHitToolStripMenuItem.Text = "Trace Hit";
      this.traceHitToolStripMenuItem.ToolTipText = "Uses the traced position under the cursor";
      this.traceHitToolStripMenuItem.Click += new System.EventHandler(this.traceHitToolStripMenuItem_Click);
      // 
      // zonesToolStripMenuItem
      // 
      this.zonesToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectZoneAtCameraPositionToolStripMenuItem,
            this.loadZoneAtCameraPositionToolStripMenuItem,
            this.loadZonesCloseToCameraToolStripMenuItem});
      this.zonesToolStripMenuItem.Image = global::Editor.Properties.Resources.unload_zone;
      this.zonesToolStripMenuItem.Name = "zonesToolStripMenuItem";
      this.zonesToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
      this.zonesToolStripMenuItem.Text = "Zones";
      // 
      // selectZoneAtCameraPositionToolStripMenuItem
      // 
      this.selectZoneAtCameraPositionToolStripMenuItem.Name = "selectZoneAtCameraPositionToolStripMenuItem";
      this.selectZoneAtCameraPositionToolStripMenuItem.Size = new System.Drawing.Size(269, 22);
      this.selectZoneAtCameraPositionToolStripMenuItem.Text = "Select Zone at Camera position";
      this.selectZoneAtCameraPositionToolStripMenuItem.Click += new System.EventHandler(this.selectZoneAtCameraPositionToolStripMenuItem_Click);
      // 
      // loadZoneAtCameraPositionToolStripMenuItem
      // 
      this.loadZoneAtCameraPositionToolStripMenuItem.Name = "loadZoneAtCameraPositionToolStripMenuItem";
      this.loadZoneAtCameraPositionToolStripMenuItem.Size = new System.Drawing.Size(269, 22);
      this.loadZoneAtCameraPositionToolStripMenuItem.Text = "Load Zone at Camera position";
      this.loadZoneAtCameraPositionToolStripMenuItem.Click += new System.EventHandler(this.loadZoneAtCameraPositionToolStripMenuItem_Click);
      // 
      // loadZonesCloseToCameraToolStripMenuItem
      // 
      this.loadZonesCloseToCameraToolStripMenuItem.Name = "loadZonesCloseToCameraToolStripMenuItem";
      this.loadZonesCloseToCameraToolStripMenuItem.Size = new System.Drawing.Size(269, 22);
      this.loadZonesCloseToCameraToolStripMenuItem.Text = "Load Zones close to Camera position";
      this.loadZonesCloseToCameraToolStripMenuItem.Click += new System.EventHandler(this.loadZonesCloseToCameraToolStripMenuItem_Click);
      // 
      // UnitScaleImageList
      // 
      this.UnitScaleImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("UnitScaleImageList.ImageStream")));
      this.UnitScaleImageList.TransparentColor = System.Drawing.Color.Transparent;
      this.UnitScaleImageList.Images.SetKeyName(0, "uinit_scale_1.png");
      this.UnitScaleImageList.Images.SetKeyName(1, "uinit_scale_2.png");
      this.UnitScaleImageList.Images.SetKeyName(2, "uinit_scale_3.png");
      this.UnitScaleImageList.Images.SetKeyName(3, "uinit_scale_4.png");
      this.UnitScaleImageList.Images.SetKeyName(4, "uinit_scale_5.png");
      this.UnitScaleImageList.Images.SetKeyName(5, "uinit_scale_6.png");
      this.UnitScaleImageList.Images.SetKeyName(6, "uinit_scale_7.png");
      this.UnitScaleImageList.Images.SetKeyName(7, "uinit_scale_8.png");
      this.UnitScaleImageList.Images.SetKeyName(8, "uinit_scale_9.png");
      this.UnitScaleImageList.Images.SetKeyName(9, "uinit_scale_10.png");
      // 
      // EnginePanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(769, 444);
      this.CloseButton = false;
      this.Controls.Add(this.panel_VisionView);
      this.Controls.Add(this.toolStrip_EnginePanel);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.Document)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "EnginePanel";
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.Document;
      this.TabText = "Engine View";
      this.Text = "Engine View";
      this.Load += new System.EventHandler(this.EnginePanel_Load);
      this.toolStrip_EnginePanel.ResumeLayout(false);
      this.toolStrip_EnginePanel.PerformLayout();
      this.contextMenuStrip_SelShapes.ResumeLayout(false);
      this.contextMenuStrip_View.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();
    }
		#endregion

		protected override void Dispose(bool disposing)
    {
      EditorManager.SceneChanged -= new SceneChangedEventHandler(visionView.OnSceneChanged);
      EditorScene.ShapeChanged -= new ShapeChangedEventHandler(visionView.OnShapeChanged);
      EditorScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(visionView.OnPropertyChanged);
      EditorScene.LayerChanged -= new LayerChangedEventHandler(EditorScene_LayerChanged);
      EditorManager.EditorModeChanged -= new EditorModeChangedEventHandler(EditorManager_EditorModeChanged);
      VisionViewBase.MouseContextChanged -= new ContextChangedEventHandler(this.OnMouseContextChanged);
      GizmoBase.GizmoChanged -= new ShapeDragModeEventHandler(this.OnGizmoChanged);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);
      EditorManager.SceneEvent -= new SceneEventHandler(enginePanel_SceneEvent);
      ViewIconSettings.OnViewIconSettingsChanged -= new EventHandler(ViewIconSettings_OnViewIconSettingsChanged);

      EditorScene.ShapeChanged -= new ShapeChangedEventHandler(visionView.Gizmo.OnShapeChanged);
      EditorScene.PropertyChanged -= new CSharpFramework.PropertyChangedEventHandler(visionView.Gizmo.OnPropertyChanged);
      EditorScene.LayerChanged -= new CSharpFramework.LayerChangedEventHandler(visionView.Gizmo.OnLayerChanged);
      EditorManager.SceneChanged -= new CSharpFramework.SceneChangedEventHandler(visionView.Gizmo.OnSceneChanged);
      EditorManager.ShapeSelectionChanged -= new ShapeSelectionChangedEventHandler(visionView.Gizmo.OnSelectionChanged);
      EditorManager.ShapeSelectionChanged -= new ShapeSelectionChangedEventHandler(EditorManager_ShapeSelectionChanged);

      VisionViewBase.ProjectionModeChanged -= new EventHandler(VisionViewBase_ProjectionModeChanged);
      if (disposing)
      {

      }
      base.Dispose(disposing);
    }
	}
}
