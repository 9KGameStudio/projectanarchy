using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using System.Collections;
using CSharpFramework;
using CSharpFramework.Controls;

namespace Editor
{
  public partial class ShapeTreePanel
  {
    #region Windows Form Designer generated code
    private CSharpFramework.ShapeTreeView shapeTreeView;
    private System.Windows.Forms.Splitter splitter1;
    private ContextMenuStrip contextMenuStrip_Layer;
    private ToolStripMenuItem lockLayerToolStripMenuItem;
    private ToolStripMenuItem unlockLayerToolStripMenuItem;
    private ToolStripMenuItem updateLockStatusToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem1;
    private ToolStripMenuItem deleteToolStripMenuItem;
    private ToolStripMenuItem renameToolStripMenuItem;
    private ContextMenuStrip contextMenuStrip_Shapes;
    private ToolStripMenuItem setShapeActiveToolStripMenuItem;
    private ToolStripMenuItem createToolStripMenuItem;
    private ToolStripMenuItem insertHereToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem3;
    private ToolStripMenuItem cutShapeToolStripMenuItem;
    private ToolStripMenuItem copyShapeToolStripMenuItem;
    private ToolStripMenuItem pasteShapeToolStripMenuItem;
    private ToolStripMenuItem deleteShapeToolStripMenuItem;
    private ToolStripMenuItem renameShapeToolStripMenuItem;
    private ToolStripMenuItem relevantOperationsToolStripMenuItem;
    private ToolStripMenuItem goHereToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem6;
    private ToolStripMenuItem dropToFloorToolStripMenuItem;
    private ToolStripMenuItem jumpToToolStripMenuItem;
    private ToolStripMenuItem pivotToolStripMenuItem;
    private ToolStripMenuItem boundingBoxToolStripMenuItem;
    private ToolStripMenuItem bottomCenterToolStripMenuItem;
    private ToolStripMenuItem sortShapesToolStripMenuItem;
    private ToolStripMenuItem alphabeticallyUpToolStripMenuItem;
    private ToolStripMenuItem alphabeticallyDownToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem7;
    private ToolStripMenuItem createPrefabToolStripMenuItem;
    private ToolStrip toolStrip1;
    private ToolStripButton toolStripButton_AddLayer;
    private ToolStripButton toolStripButton_DeleteLayer;
    private ToolStripButton toolStripButton_LayerMoveUp;
    private ToolStripButton toolStripButton_LayerMoveDown;
    private ToolStripButton toolStripButton_LockUpdate;
    private ToolStripButton toolStripButton_FileRefresh;
    private ToolStripSeparator toolStripSeparator1;
    private ToolStripButton toolStripButton_SortAZUp;
    private ToolStripButton toolStripButton_SortAZDown;
    private CSharpFramework.Controls.LayerTreeView treeView_Layers;
    private ContextMenuStrip contextMenuStrip_Zones;
    private ToolStripSeparator toolStripSeparator2;
    private ToolStripMenuItem toolStripMenuItem_DeleteZone;
    private ToolStripMenuItem toolStripMenuItem_RenameZone;
    private ToolStripMenuItem loadUnloadToolStripMenuItem;
    private ToolStripMenuItem relightToolStripMenuItem;
    private ToolStripMenuItem childNodesToolStripMenuItem;
    private ToolStripMenuItem selectChildNodesToolStripMenuItem;
    private ToolStripMenuItem selectRecursivelyToolStripMenuItem;
    private ToolStripMenuItem lockShapesToolStripMenuItem;
    private ToolStripMenuItem unfreezeAllShapesToolStripMenuItem;
    private ToolStripMenuItem showZoneAssetsToolStripMenuItem;
    private ToolStripMenuItem showLayerAssetsToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem_LastSeparator;
    private ToolStripMenuItem renameFileToLayerNameToolStripMenuItem;
    private ToolStripMenuItem moveToLayerToolStripMenuItem;
    private ToolStripMenuItem layersGoHereToolStripMenuItem;
    private ToolStripSeparator toolStripMenuItem8;
    private ToolStripMenuItem directionFloorToolStripMenuItem;
    private ToolStripMenuItem directionCeilingToolStripMenuItem;
    private ToolStripMenuItem directionPosXToolStripMenuItem;
    private ToolStripMenuItem directionNegXToolStripMenuItem;
    private ToolStripMenuItem directionPosYToolStripMenuItem;
    private ToolStripMenuItem directionNegYToolStripMenuItem;
    private ToolStripMenuItem sortShapesIntoZonesToolStripMenuItem;
    private ToolStripMenuItem moveShapesToolStripMenuItem;
    private ToolStripButton toolStripButton_AddZone;
    private ToolStrip toolStrip_Shapes;
    private ToolStripButton toolStripButton_AddGroup;
    private ToolStripButton toolStripButton_DeleteShape;
    private ToolStripButton toolStripButton_ShapeMoveDown;
    private ToolStripButton toolStripButton_ShapeMoveUp;
    private ToolStripButton toolStripButton_SortShapesAZUp;
    private ToolStripButton toolStripButton_SortShapesAZDown;
    private ToolStripSplitButton toolStripSplitButton_MoveToLayer;
    private ToolStripMenuItem layersGoHereToolStripMenuItem1;
    private ToolStripSeparator toolStripSeparator5;
    private ToolStripSeparator toolStripSeparator3;
    private ToolStripSeparator toolStripSeparator4;
    private ToolStripSplitButton toolStripSplitButton_ToolsOptions;
    private ToolStripMenuItem toolStripSplitButton_AutomaticFocus;
    private ToolStripMenuItem toolStripSplitButton_PropertiesOnSelection;
    private ToolStripButton toolStripButton_ImportReference;
    private TableLayoutPanel panel_UpdateLayer;
    private PictureBox pictureBox_UpdateIcon;
    private FlowLayoutPanel flowLayoutPanel1;
    private Button button_UpdateIgnore;
    private Button button_LayerUpdate;
    private Label label1;
    private ToolStripMenuItem lockStatusToolStripMenuItem;

    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ShapeTreePanel));
      this.shapeTreeView = new CSharpFramework.ShapeTreeView();
      this.contextMenuStrip_Shapes = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.relevantOperationsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.goHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
      this.cutShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.copyShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.pasteShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.deleteShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.renameShapeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripSeparator();
      this.createToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.insertHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
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
      this.sortShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.alphabeticallyUpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.alphabeticallyDownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.childNodesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectChildNodesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectRecursivelyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.moveToLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.layersGoHereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
      this.cameraPositionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.shapeOriginToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripSeparator();
      this.setShapeActiveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.jumpToToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.lockShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.createPrefabToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.contextMenuStrip_Layer = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.lockStatusToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.lockLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unlockLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.updateLockStatusToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unfreezeAllShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.sortShapesIntoZonesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.showLayerAssetsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
      this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.renameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem_LastSeparator = new System.Windows.Forms.ToolStripSeparator();
      this.renameFileToLayerNameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_AddLayer = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_AddZone = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_ImportReference = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DeleteLayer = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_LayerMoveDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_LayerMoveUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortAZUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortAZDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_LockUpdate = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_FileRefresh = new System.Windows.Forms.ToolStripButton();
      this.treeView_Layers = new CSharpFramework.Controls.LayerTreeView();
      this.contextMenuStrip_Zones = new System.Windows.Forms.ContextMenuStrip(this.components);
      this.moveShapesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.loadUnloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.relightToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.showZoneAssetsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripMenuItem_DeleteZone = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem_RenameZone = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip_Shapes = new System.Windows.Forms.ToolStrip();
      this.toolStripButton_AddGroup = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_DeleteShape = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButton_ShapeMoveDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_ShapeMoveUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortShapesAZUp = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton_SortShapesAZDown = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripSplitButton_MoveToLayer = new System.Windows.Forms.ToolStripSplitButton();
      this.layersGoHereToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSplitButton_ToolsOptions = new System.Windows.Forms.ToolStripSplitButton();
      this.toolStripSplitButton_AutomaticFocus = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSplitButton_PropertiesOnSelection = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripLabelSearch = new System.Windows.Forms.ToolStripLabel();
      this.searchPanel = new CSharpFramework.Controls.IncrementalSearchPanel();
      this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
      this.panel_UpdateLayer = new System.Windows.Forms.TableLayoutPanel();
      this.pictureBox_UpdateIcon = new System.Windows.Forms.PictureBox();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.button_UpdateIgnore = new System.Windows.Forms.Button();
      this.button_LayerUpdate = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.propertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip_Search = new System.Windows.Forms.ToolStrip();
      this.toolStripButtonClearSearch = new System.Windows.Forms.ToolStripButton();
      this.contextMenuStrip_Shapes.SuspendLayout();
      this.contextMenuStrip_Layer.SuspendLayout();
      this.toolStrip1.SuspendLayout();
      this.contextMenuStrip_Zones.SuspendLayout();
      this.toolStrip_Shapes.SuspendLayout();
      this.panel_UpdateLayer.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_UpdateIcon)).BeginInit();
      this.flowLayoutPanel1.SuspendLayout();
      this.toolStrip_Search.SuspendLayout();
      this.SuspendLayout();
      // 
      // shapeTreeView
      // 
      this.shapeTreeView.AllowDrop = true;
      this.shapeTreeView.AllowMultiSelect = true;
      this.shapeTreeView.ContextMenuStrip = this.contextMenuStrip_Shapes;
      this.shapeTreeView.Dock = System.Windows.Forms.DockStyle.Top;
      this.shapeTreeView.DrawMode = System.Windows.Forms.TreeViewDrawMode.OwnerDrawText;
      this.shapeTreeView.HideSelection = false;
      this.shapeTreeView.LabelEdit = true;
      this.shapeTreeView.Location = new System.Drawing.Point(0, 50);
      this.shapeTreeView.Name = "shapeTreeView";
      this.shapeTreeView.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("shapeTreeView.SelectedNodes")));
      this.shapeTreeView.ShapeFilter = "";
      this.shapeTreeView.Size = new System.Drawing.Size(397, 257);
      this.shapeTreeView.TabIndex = 1;
      this.shapeTreeView.UseShapeFilter = false;
      // 
      // contextMenuStrip_Shapes
      // 
      this.contextMenuStrip_Shapes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.relevantOperationsToolStripMenuItem,
            this.toolStripMenuItem3,
            this.cutShapeToolStripMenuItem,
            this.copyShapeToolStripMenuItem,
            this.pasteShapeToolStripMenuItem,
            this.deleteShapeToolStripMenuItem,
            this.renameShapeToolStripMenuItem,
            this.toolStripMenuItem6,
            this.createToolStripMenuItem,
            this.dropToFloorToolStripMenuItem,
            this.sortShapesToolStripMenuItem,
            this.childNodesToolStripMenuItem,
            this.moveToLayerToolStripMenuItem,
            this.toolStripMenuItem2,
            this.toolStripMenuItem7,
            this.setShapeActiveToolStripMenuItem,
            this.jumpToToolStripMenuItem,
            this.lockShapesToolStripMenuItem,
            this.createPrefabToolStripMenuItem,
            this.propertiesToolStripMenuItem});
      this.contextMenuStrip_Shapes.Name = "contextMenuStrip_Shapes";
      this.contextMenuStrip_Shapes.Size = new System.Drawing.Size(181, 418);
      this.contextMenuStrip_Shapes.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Shapes_Opening);
      // 
      // relevantOperationsToolStripMenuItem
      // 
      this.relevantOperationsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.goHereToolStripMenuItem});
      this.relevantOperationsToolStripMenuItem.Image = global::Editor.Properties.Resources.wrench;
      this.relevantOperationsToolStripMenuItem.Name = "relevantOperationsToolStripMenuItem";
      this.relevantOperationsToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.relevantOperationsToolStripMenuItem.Text = "Relevant Operations";
      this.relevantOperationsToolStripMenuItem.ToolTipText = "Performs relevant operations on the shapes";
      // 
      // goHereToolStripMenuItem
      // 
      this.goHereToolStripMenuItem.Name = "goHereToolStripMenuItem";
      this.goHereToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
      this.goHereToolStripMenuItem.Text = "<go here>";
      // 
      // toolStripMenuItem3
      // 
      this.toolStripMenuItem3.Name = "toolStripMenuItem3";
      this.toolStripMenuItem3.Size = new System.Drawing.Size(177, 6);
      // 
      // cutShapeToolStripMenuItem
      // 
      this.cutShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.cut;
      this.cutShapeToolStripMenuItem.Name = "cutShapeToolStripMenuItem";
      this.cutShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.cutShapeToolStripMenuItem.Text = "Cut";
      this.cutShapeToolStripMenuItem.ToolTipText = "Cuts the selected shape(s)";
      this.cutShapeToolStripMenuItem.Click += new System.EventHandler(this.cutShapeToolStripMenuItem_Click);
      // 
      // copyShapeToolStripMenuItem
      // 
      this.copyShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.copy;
      this.copyShapeToolStripMenuItem.Name = "copyShapeToolStripMenuItem";
      this.copyShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.copyShapeToolStripMenuItem.Text = "Copy";
      this.copyShapeToolStripMenuItem.ToolTipText = "Copies the selected shape(s)";
      this.copyShapeToolStripMenuItem.Click += new System.EventHandler(this.copyShapeToolStripMenuItem_Click);
      // 
      // pasteShapeToolStripMenuItem
      // 
      this.pasteShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.paste;
      this.pasteShapeToolStripMenuItem.Name = "pasteShapeToolStripMenuItem";
      this.pasteShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.pasteShapeToolStripMenuItem.Text = "Paste";
      this.pasteShapeToolStripMenuItem.ToolTipText = "Pastes the clipboard content";
      this.pasteShapeToolStripMenuItem.Click += new System.EventHandler(this.pasteShapeToolStripMenuItem_Click);
      // 
      // deleteShapeToolStripMenuItem
      // 
      this.deleteShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.delete2;
      this.deleteShapeToolStripMenuItem.Name = "deleteShapeToolStripMenuItem";
      this.deleteShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.deleteShapeToolStripMenuItem.Text = "Delete";
      this.deleteShapeToolStripMenuItem.ToolTipText = "Deletes the selected shape(s)";
      this.deleteShapeToolStripMenuItem.Click += new System.EventHandler(this.deleteShapeToolStripMenuItem_Click);
      // 
      // renameShapeToolStripMenuItem
      // 
      this.renameShapeToolStripMenuItem.Image = global::Editor.Properties.Resources.rename;
      this.renameShapeToolStripMenuItem.Name = "renameShapeToolStripMenuItem";
      this.renameShapeToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.renameShapeToolStripMenuItem.Text = "Rename";
      this.renameShapeToolStripMenuItem.ToolTipText = "Renames the selected shape";
      this.renameShapeToolStripMenuItem.Click += new System.EventHandler(this.renameShapeToolStripMenuItem_Click);
      // 
      // toolStripMenuItem6
      // 
      this.toolStripMenuItem6.Name = "toolStripMenuItem6";
      this.toolStripMenuItem6.Size = new System.Drawing.Size(177, 6);
      // 
      // createToolStripMenuItem
      // 
      this.createToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.insertHereToolStripMenuItem});
      this.createToolStripMenuItem.Image = global::Editor.Properties.Resources.pawn_new;
      this.createToolStripMenuItem.Name = "createToolStripMenuItem";
      this.createToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.createToolStripMenuItem.Text = "Create";
      this.createToolStripMenuItem.ToolTipText = "All creator plugins";
      // 
      // insertHereToolStripMenuItem
      // 
      this.insertHereToolStripMenuItem.Name = "insertHereToolStripMenuItem";
      this.insertHereToolStripMenuItem.Size = new System.Drawing.Size(130, 22);
      this.insertHereToolStripMenuItem.Text = "<go here>";
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
      this.dropToFloorToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.dropToFloorToolStripMenuItem.Name = "dropToFloorToolStripMenuItem";
      this.dropToFloorToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.dropToFloorToolStripMenuItem.Text = "Drop to Floor";
      this.dropToFloorToolStripMenuItem.ToolTipText = "Drops selected shapes until they collide with the floor geometry";
      this.dropToFloorToolStripMenuItem.DropDownOpening += new System.EventHandler(this.dropToFloorToolStripMenuItem_DropDownOpening);
      // 
      // pivotToolStripMenuItem
      // 
      this.pivotToolStripMenuItem.Name = "pivotToolStripMenuItem";
      this.pivotToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.pivotToolStripMenuItem.Text = "Pivot";
      this.pivotToolStripMenuItem.ToolTipText = "Takes the pivot point of the shape";
      this.pivotToolStripMenuItem.Click += new System.EventHandler(this.pivotToolStripMenuItem_Click);
      // 
      // boundingBoxToolStripMenuItem
      // 
      this.boundingBoxToolStripMenuItem.Name = "boundingBoxToolStripMenuItem";
      this.boundingBoxToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.boundingBoxToolStripMenuItem.Text = "Bounding Box";
      this.boundingBoxToolStripMenuItem.ToolTipText = "Collides the full bounding box with the geometry";
      this.boundingBoxToolStripMenuItem.Click += new System.EventHandler(this.boundingBoxToolStripMenuItem_Click);
      // 
      // bottomCenterToolStripMenuItem
      // 
      this.bottomCenterToolStripMenuItem.Name = "bottomCenterToolStripMenuItem";
      this.bottomCenterToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
      this.bottomCenterToolStripMenuItem.Text = "Bottom Center";
      this.bottomCenterToolStripMenuItem.ToolTipText = "Takes the bottom center of the bounding box to collide";
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
      // sortShapesToolStripMenuItem
      // 
      this.sortShapesToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.alphabeticallyUpToolStripMenuItem,
            this.alphabeticallyDownToolStripMenuItem});
      this.sortShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.sort_descending;
      this.sortShapesToolStripMenuItem.Name = "sortShapesToolStripMenuItem";
      this.sortShapesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.sortShapesToolStripMenuItem.Text = "Sort Shapes";
      this.sortShapesToolStripMenuItem.ToolTipText = "Sorts the children of the selected shape (or root)";
      // 
      // alphabeticallyUpToolStripMenuItem
      // 
      this.alphabeticallyUpToolStripMenuItem.Image = global::Editor.Properties.Resources.sort_az_descending;
      this.alphabeticallyUpToolStripMenuItem.Name = "alphabeticallyUpToolStripMenuItem";
      this.alphabeticallyUpToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.alphabeticallyUpToolStripMenuItem.Text = "Alphabetically Up";
      this.alphabeticallyUpToolStripMenuItem.ToolTipText = "Sorts the shapes by shape name (A-Z)";
      this.alphabeticallyUpToolStripMenuItem.Click += new System.EventHandler(this.alphabeticallyUpToolStripMenuItem_Click);
      // 
      // alphabeticallyDownToolStripMenuItem
      // 
      this.alphabeticallyDownToolStripMenuItem.Image = global::Editor.Properties.Resources.sort_az_ascending;
      this.alphabeticallyDownToolStripMenuItem.Name = "alphabeticallyDownToolStripMenuItem";
      this.alphabeticallyDownToolStripMenuItem.Size = new System.Drawing.Size(183, 22);
      this.alphabeticallyDownToolStripMenuItem.Text = "Alphabetically Down";
      this.alphabeticallyDownToolStripMenuItem.ToolTipText = "Sorts the shapes by shape name (Z-A)";
      this.alphabeticallyDownToolStripMenuItem.Click += new System.EventHandler(this.alphabeticallyDownToolStripMenuItem_Click);
      // 
      // childNodesToolStripMenuItem
      // 
      this.childNodesToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectChildNodesToolStripMenuItem,
            this.selectRecursivelyToolStripMenuItem});
      this.childNodesToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes_icon;
      this.childNodesToolStripMenuItem.Name = "childNodesToolStripMenuItem";
      this.childNodesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.childNodesToolStripMenuItem.Text = "Child Nodes";
      this.childNodesToolStripMenuItem.ToolTipText = "Select child nodes of this shape";
      // 
      // selectChildNodesToolStripMenuItem
      // 
      this.selectChildNodesToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes;
      this.selectChildNodesToolStripMenuItem.Name = "selectChildNodesToolStripMenuItem";
      this.selectChildNodesToolStripMenuItem.Size = new System.Drawing.Size(173, 22);
      this.selectChildNodesToolStripMenuItem.Text = "Select Child Nodes";
      this.selectChildNodesToolStripMenuItem.ToolTipText = "Select the child nodes of the currently selected shapes";
      this.selectChildNodesToolStripMenuItem.Click += new System.EventHandler(this.selectChildNodesToolStripMenuItem_Click);
      // 
      // selectRecursivelyToolStripMenuItem
      // 
      this.selectRecursivelyToolStripMenuItem.Image = global::Editor.Properties.Resources.child_nodes_rec;
      this.selectRecursivelyToolStripMenuItem.Name = "selectRecursivelyToolStripMenuItem";
      this.selectRecursivelyToolStripMenuItem.Size = new System.Drawing.Size(173, 22);
      this.selectRecursivelyToolStripMenuItem.Text = "Select Recursively";
      this.selectRecursivelyToolStripMenuItem.ToolTipText = "Select all the child nodes of the currently selected shapes recursively";
      this.selectRecursivelyToolStripMenuItem.Click += new System.EventHandler(this.selectRecursivelyToolStripMenuItem_Click);
      // 
      // moveToLayerToolStripMenuItem
      // 
      this.moveToLayerToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.layersGoHereToolStripMenuItem});
      this.moveToLayerToolStripMenuItem.Image = global::Editor.Properties.Resources.breakpoint_into;
      this.moveToLayerToolStripMenuItem.Name = "moveToLayerToolStripMenuItem";
      this.moveToLayerToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.moveToLayerToolStripMenuItem.Text = "Move to Layer";
      // 
      // layersGoHereToolStripMenuItem
      // 
      this.layersGoHereToolStripMenuItem.Name = "layersGoHereToolStripMenuItem";
      this.layersGoHereToolStripMenuItem.Size = new System.Drawing.Size(166, 22);
      this.layersGoHereToolStripMenuItem.Text = "<Layers go here>";
      // 
      // toolStripMenuItem2
      // 
      this.toolStripMenuItem2.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cameraPositionToolStripMenuItem,
            this.shapeOriginToolStripMenuItem});
      this.toolStripMenuItem2.Name = "toolStripMenuItem2";
      this.toolStripMenuItem2.Size = new System.Drawing.Size(180, 22);
      this.toolStripMenuItem2.Text = "Move To";
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
      // toolStripMenuItem7
      // 
      this.toolStripMenuItem7.Name = "toolStripMenuItem7";
      this.toolStripMenuItem7.Size = new System.Drawing.Size(177, 6);
      // 
      // setShapeActiveToolStripMenuItem
      // 
      this.setShapeActiveToolStripMenuItem.Image = global::Editor.Properties.Resources.lightbulb_on;
      this.setShapeActiveToolStripMenuItem.Name = "setShapeActiveToolStripMenuItem";
      this.setShapeActiveToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.setShapeActiveToolStripMenuItem.Text = "Set Shape Active";
      this.setShapeActiveToolStripMenuItem.ToolTipText = "Marks the shape as active so added shapes will use it as parent";
      this.setShapeActiveToolStripMenuItem.Click += new System.EventHandler(this.setShapeActiveToolStripMenuItem_Click);
      // 
      // jumpToToolStripMenuItem
      // 
      this.jumpToToolStripMenuItem.Image = global::Editor.Properties.Resources.toolbar_camera_small;
      this.jumpToToolStripMenuItem.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
      this.jumpToToolStripMenuItem.Name = "jumpToToolStripMenuItem";
      this.jumpToToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.jumpToToolStripMenuItem.Text = "Jump to Object";
      this.jumpToToolStripMenuItem.ToolTipText = "Move the camera to the selected shapes(s)";
      this.jumpToToolStripMenuItem.Click += new System.EventHandler(this.jumpToToolStripMenuItem_Click);
      // 
      // lockShapesToolStripMenuItem
      // 
      this.lockShapesToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
      this.lockShapesToolStripMenuItem.Name = "lockShapesToolStripMenuItem";
      this.lockShapesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.lockShapesToolStripMenuItem.Text = "Freeze Shapes";
      this.lockShapesToolStripMenuItem.Click += new System.EventHandler(this.lockShapesToolStripMenuItem_Click);
      // 
      // createPrefabToolStripMenuItem
      // 
      this.createPrefabToolStripMenuItem.Image = global::Editor.Properties.Resources.folder_refresh;
      this.createPrefabToolStripMenuItem.Name = "createPrefabToolStripMenuItem";
      this.createPrefabToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.createPrefabToolStripMenuItem.Text = "Create Prefab";
      this.createPrefabToolStripMenuItem.Click += new System.EventHandler(this.createPrefabToolStripMenuItem_Click);
      // 
      // splitter1
      // 
      this.splitter1.Dock = System.Windows.Forms.DockStyle.Top;
      this.splitter1.Location = new System.Drawing.Point(0, 307);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(397, 3);
      this.splitter1.TabIndex = 2;
      this.splitter1.TabStop = false;
      // 
      // contextMenuStrip_Layer
      // 
      this.contextMenuStrip_Layer.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lockStatusToolStripMenuItem,
            this.lockLayerToolStripMenuItem,
            this.unlockLayerToolStripMenuItem,
            this.updateLockStatusToolStripMenuItem,
            this.unfreezeAllShapesToolStripMenuItem,
            this.sortShapesIntoZonesToolStripMenuItem,
            this.showLayerAssetsToolStripMenuItem,
            this.toolStripMenuItem1,
            this.deleteToolStripMenuItem,
            this.renameToolStripMenuItem,
            this.toolStripMenuItem_LastSeparator,
            this.renameFileToLayerNameToolStripMenuItem});
      this.contextMenuStrip_Layer.Name = "contextMenuStrip_Layer";
      this.contextMenuStrip_Layer.Size = new System.Drawing.Size(215, 236);
      this.contextMenuStrip_Layer.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Layer_Opening);
      // 
      // lockStatusToolStripMenuItem
      // 
      this.lockStatusToolStripMenuItem.BackColor = System.Drawing.SystemColors.ControlLight;
      this.lockStatusToolStripMenuItem.Enabled = false;
      this.lockStatusToolStripMenuItem.Name = "lockStatusToolStripMenuItem";
      this.lockStatusToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.lockStatusToolStripMenuItem.Text = "LockStatus: Unknown";
      // 
      // lockLayerToolStripMenuItem
      // 
      this.lockLayerToolStripMenuItem.Image = global::Editor.Properties.Resources._lock;
      this.lockLayerToolStripMenuItem.Name = "lockLayerToolStripMenuItem";
      this.lockLayerToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.lockLayerToolStripMenuItem.Text = "Lock Layer";
      this.lockLayerToolStripMenuItem.ToolTipText = "Tries to lock the layer so it can be edited";
      this.lockLayerToolStripMenuItem.Click += new System.EventHandler(this.lockLayerToolStripMenuItem_Click);
      // 
      // unlockLayerToolStripMenuItem
      // 
      this.unlockLayerToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_open;
      this.unlockLayerToolStripMenuItem.Name = "unlockLayerToolStripMenuItem";
      this.unlockLayerToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.unlockLayerToolStripMenuItem.Text = "Unlock Layer";
      this.unlockLayerToolStripMenuItem.ToolTipText = "Unlocks the layer so other users can edit it over the network";
      this.unlockLayerToolStripMenuItem.Click += new System.EventHandler(this.unlockLayerToolStripMenuItem_Click);
      // 
      // updateLockStatusToolStripMenuItem
      // 
      this.updateLockStatusToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_view;
      this.updateLockStatusToolStripMenuItem.Name = "updateLockStatusToolStripMenuItem";
      this.updateLockStatusToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.updateLockStatusToolStripMenuItem.Text = "Update Lock Status";
      this.updateLockStatusToolStripMenuItem.ToolTipText = "Refreshes the lock status of the layer";
      this.updateLockStatusToolStripMenuItem.Click += new System.EventHandler(this.updateLockStatusToolStripMenuItem_Click);
      // 
      // unfreezeAllShapesToolStripMenuItem
      // 
      this.unfreezeAllShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.lock_open;
      this.unfreezeAllShapesToolStripMenuItem.Name = "unfreezeAllShapesToolStripMenuItem";
      this.unfreezeAllShapesToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.unfreezeAllShapesToolStripMenuItem.Text = "Un-freeze All Shapes";
      this.unfreezeAllShapesToolStripMenuItem.ToolTipText = "If this layer contains any frozen shape, this operation releases the frozen state" +
    " on all";
      this.unfreezeAllShapesToolStripMenuItem.Click += new System.EventHandler(this.unfreezeAllShapesToolStripMenuItem_Click);
      // 
      // sortShapesIntoZonesToolStripMenuItem
      // 
      this.sortShapesIntoZonesToolStripMenuItem.Image = global::Editor.Properties.Resources.unload_zone;
      this.sortShapesIntoZonesToolStripMenuItem.Name = "sortShapesIntoZonesToolStripMenuItem";
      this.sortShapesIntoZonesToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.sortShapesIntoZonesToolStripMenuItem.Text = "Sort Shapes into Zones";
      this.sortShapesIntoZonesToolStripMenuItem.ToolTipText = "Sort all shapes in this layer into best matching zones";
      this.sortShapesIntoZonesToolStripMenuItem.Click += new System.EventHandler(this.sortShapesIntoZonesToolStripMenuItem_Click);
      // 
      // showLayerAssetsToolStripMenuItem
      // 
      this.showLayerAssetsToolStripMenuItem.Image = global::Editor.Properties.Resources.memory;
      this.showLayerAssetsToolStripMenuItem.Name = "showLayerAssetsToolStripMenuItem";
      this.showLayerAssetsToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.showLayerAssetsToolStripMenuItem.Text = "Show Layer Assets";
      this.showLayerAssetsToolStripMenuItem.Click += new System.EventHandler(this.showLayerAssetsToolStripMenuItem_Click);
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(211, 6);
      // 
      // deleteToolStripMenuItem
      // 
      this.deleteToolStripMenuItem.Image = global::Editor.Properties.Resources.delete2;
      this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
      this.deleteToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.deleteToolStripMenuItem.Text = "Delete";
      this.deleteToolStripMenuItem.ToolTipText = "Deletes the selected layer";
      this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
      // 
      // renameToolStripMenuItem
      // 
      this.renameToolStripMenuItem.Image = global::Editor.Properties.Resources.rename;
      this.renameToolStripMenuItem.Name = "renameToolStripMenuItem";
      this.renameToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.renameToolStripMenuItem.Text = "Rename";
      this.renameToolStripMenuItem.ToolTipText = "Renames the selected layer";
      this.renameToolStripMenuItem.Click += new System.EventHandler(this.renameToolStripMenuItem_Click);
      // 
      // toolStripMenuItem_LastSeparator
      // 
      this.toolStripMenuItem_LastSeparator.Name = "toolStripMenuItem_LastSeparator";
      this.toolStripMenuItem_LastSeparator.Size = new System.Drawing.Size(211, 6);
      // 
      // renameFileToLayerNameToolStripMenuItem
      // 
      this.renameFileToLayerNameToolStripMenuItem.Image = global::Editor.Properties.Resources.save_as;
      this.renameFileToLayerNameToolStripMenuItem.Name = "renameFileToLayerNameToolStripMenuItem";
      this.renameFileToLayerNameToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
      this.renameFileToLayerNameToolStripMenuItem.Text = "Rename file to Layer name";
      this.renameFileToLayerNameToolStripMenuItem.ToolTipText = "Synchronizes the layer\'s filename with the layer\'s name. Tries to rename file(s) " +
    "on disk";
      this.renameFileToLayerNameToolStripMenuItem.Click += new System.EventHandler(this.renameFileToLayerNameToolStripMenuItem_Click);
      // 
      // toolStrip1
      // 
      this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_AddLayer,
            this.toolStripButton_AddZone,
            this.toolStripButton_ImportReference,
            this.toolStripButton_DeleteLayer,
            this.toolStripSeparator5,
            this.toolStripButton_LayerMoveDown,
            this.toolStripButton_LayerMoveUp,
            this.toolStripButton_SortAZUp,
            this.toolStripButton_SortAZDown,
            this.toolStripSeparator1,
            this.toolStripButton_LockUpdate,
            this.toolStripButton_FileRefresh});
      this.toolStrip1.Location = new System.Drawing.Point(0, 310);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(397, 25);
      this.toolStrip1.TabIndex = 3;
      this.toolStrip1.Text = "toolStrip_Layers";
      // 
      // toolStripButton_AddLayer
      // 
      this.toolStripButton_AddLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddLayer.Image = global::Editor.Properties.Resources.breakpoint_new;
      this.toolStripButton_AddLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddLayer.Name = "toolStripButton_AddLayer";
      this.toolStripButton_AddLayer.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddLayer.Text = "toolStripButton1";
      this.toolStripButton_AddLayer.ToolTipText = "Adds a new layer";
      this.toolStripButton_AddLayer.Click += new System.EventHandler(this.toolStripButton_AddLayer_Click);
      // 
      // toolStripButton_AddZone
      // 
      this.toolStripButton_AddZone.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddZone.Image = global::Editor.Properties.Resources.add_zone;
      this.toolStripButton_AddZone.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddZone.Name = "toolStripButton_AddZone";
      this.toolStripButton_AddZone.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddZone.Text = "toolStripButton1";
      this.toolStripButton_AddZone.ToolTipText = "Adds a new zone";
      this.toolStripButton_AddZone.Click += new System.EventHandler(this.toolStripButton_AddZone_Click);
      // 
      // toolStripButton_ImportReference
      // 
      this.toolStripButton_ImportReference.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ImportReference.Image = global::Editor.Properties.Resources.toolbar_folder_in;
      this.toolStripButton_ImportReference.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ImportReference.Name = "toolStripButton_ImportReference";
      this.toolStripButton_ImportReference.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ImportReference.Text = "toolStripButton1";
      this.toolStripButton_ImportReference.ToolTipText = "Add a layer reference from another scene in this project. These layers cannot be " +
    "edited";
      this.toolStripButton_ImportReference.Click += new System.EventHandler(this.toolStripButton_ImportReference_Click);
      // 
      // toolStripButton_DeleteLayer
      // 
      this.toolStripButton_DeleteLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DeleteLayer.Image = global::Editor.Properties.Resources.delete2;
      this.toolStripButton_DeleteLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DeleteLayer.Name = "toolStripButton_DeleteLayer";
      this.toolStripButton_DeleteLayer.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_DeleteLayer.Text = "toolStripButton1";
      this.toolStripButton_DeleteLayer.ToolTipText = "Delete the selected layer or zone";
      this.toolStripButton_DeleteLayer.Click += new System.EventHandler(this.toolStripButton_DeleteLayer_Click);
      // 
      // toolStripSeparator5
      // 
      this.toolStripSeparator5.Name = "toolStripSeparator5";
      this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_LayerMoveDown
      // 
      this.toolStripButton_LayerMoveDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_LayerMoveDown.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.toolStripButton_LayerMoveDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_LayerMoveDown.Name = "toolStripButton_LayerMoveDown";
      this.toolStripButton_LayerMoveDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_LayerMoveDown.ToolTipText = "Move the layer one item down in the layer list";
      this.toolStripButton_LayerMoveDown.Click += new System.EventHandler(this.toolStripButton_LayerMoveDown_Click);
      // 
      // toolStripButton_LayerMoveUp
      // 
      this.toolStripButton_LayerMoveUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_LayerMoveUp.Image = global::Editor.Properties.Resources.arrow_up_blue;
      this.toolStripButton_LayerMoveUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_LayerMoveUp.Name = "toolStripButton_LayerMoveUp";
      this.toolStripButton_LayerMoveUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_LayerMoveUp.ToolTipText = "Move the layer up one item in the layer list";
      this.toolStripButton_LayerMoveUp.Click += new System.EventHandler(this.toolStripButton_LayerMoveUp_Click);
      // 
      // toolStripButton_SortAZUp
      // 
      this.toolStripButton_SortAZUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortAZUp.Image = global::Editor.Properties.Resources.sort_az_descending;
      this.toolStripButton_SortAZUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortAZUp.Name = "toolStripButton_SortAZUp";
      this.toolStripButton_SortAZUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortAZUp.Text = "toolStripButton1";
      this.toolStripButton_SortAZUp.ToolTipText = "Sort the layers alphabetically by name";
      this.toolStripButton_SortAZUp.Click += new System.EventHandler(this.toolStripButton_SortAZUp_Click);
      // 
      // toolStripButton_SortAZDown
      // 
      this.toolStripButton_SortAZDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortAZDown.Image = global::Editor.Properties.Resources.sort_az_ascending;
      this.toolStripButton_SortAZDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortAZDown.Name = "toolStripButton_SortAZDown";
      this.toolStripButton_SortAZDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortAZDown.Text = "toolStripButton1";
      this.toolStripButton_SortAZDown.ToolTipText = "Sort the layers alphabetically by name";
      this.toolStripButton_SortAZDown.Click += new System.EventHandler(this.toolStripButton_SortAZDown_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_LockUpdate
      // 
      this.toolStripButton_LockUpdate.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_LockUpdate.Image = global::Editor.Properties.Resources.lock_view;
      this.toolStripButton_LockUpdate.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_LockUpdate.Name = "toolStripButton_LockUpdate";
      this.toolStripButton_LockUpdate.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_LockUpdate.ToolTipText = "Update the layer lock status over the network";
      this.toolStripButton_LockUpdate.Click += new System.EventHandler(this.toolStripButton_LockUpdate_Click);
      // 
      // toolStripButton_FileRefresh
      // 
      this.toolStripButton_FileRefresh.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_FileRefresh.Image = global::Editor.Properties.Resources.folder_refresh;
      this.toolStripButton_FileRefresh.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_FileRefresh.Name = "toolStripButton_FileRefresh";
      this.toolStripButton_FileRefresh.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_FileRefresh.Text = "toolStripButton1";
      this.toolStripButton_FileRefresh.ToolTipText = "Check for updated layer files that are modified in the network";
      this.toolStripButton_FileRefresh.Click += new System.EventHandler(this.toolStripButton_FileRefresh_Click);
      // 
      // treeView_Layers
      // 
      this.treeView_Layers.AllowDrop = true;
      this.treeView_Layers.AllowMultiSelect = true;
      this.treeView_Layers.CheckBoxes = true;
      this.treeView_Layers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.treeView_Layers.DrawMode = System.Windows.Forms.TreeViewDrawMode.OwnerDrawText;
      this.treeView_Layers.HideSelection = false;
      this.treeView_Layers.LabelEdit = true;
      this.treeView_Layers.Location = new System.Drawing.Point(0, 335);
      this.treeView_Layers.Name = "treeView_Layers";
      this.treeView_Layers.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("treeView_Layers.SelectedNodes")));
      this.treeView_Layers.ShowNodeToolTips = true;
      this.treeView_Layers.Size = new System.Drawing.Size(397, 317);
      this.treeView_Layers.TabIndex = 4;
      // 
      // contextMenuStrip_Zones
      // 
      this.contextMenuStrip_Zones.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.moveShapesToolStripMenuItem,
            this.loadUnloadToolStripMenuItem,
            this.relightToolStripMenuItem,
            this.showZoneAssetsToolStripMenuItem,
            this.toolStripSeparator2,
            this.toolStripMenuItem_DeleteZone,
            this.toolStripMenuItem_RenameZone});
      this.contextMenuStrip_Zones.Name = "contextMenuStrip_Layer";
      this.contextMenuStrip_Zones.Size = new System.Drawing.Size(170, 142);
      this.contextMenuStrip_Zones.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Zones_Opening);
      // 
      // moveShapesToolStripMenuItem
      // 
      this.moveShapesToolStripMenuItem.Image = global::Editor.Properties.Resources.gizmo;
      this.moveShapesToolStripMenuItem.Name = "moveShapesToolStripMenuItem";
      this.moveShapesToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.moveShapesToolStripMenuItem.Text = "Move Shapes";
      this.moveShapesToolStripMenuItem.ToolTipText = "Move the content of the selected zone(s) using a move gizmo";
      this.moveShapesToolStripMenuItem.Click += new System.EventHandler(this.moveShapesToolStripMenuItem_Click);
      // 
      // loadUnloadToolStripMenuItem
      // 
      this.loadUnloadToolStripMenuItem.Image = global::Editor.Properties.Resources.unload_zone;
      this.loadUnloadToolStripMenuItem.Name = "loadUnloadToolStripMenuItem";
      this.loadUnloadToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.loadUnloadToolStripMenuItem.Text = "Load/Unload";
      this.loadUnloadToolStripMenuItem.Click += new System.EventHandler(this.loadUnloadToolStripMenuItem_Click);
      // 
      // relightToolStripMenuItem
      // 
      this.relightToolStripMenuItem.Image = global::Editor.Properties.Resources.lightbulb_on;
      this.relightToolStripMenuItem.Name = "relightToolStripMenuItem";
      this.relightToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.relightToolStripMenuItem.Text = "Relight";
      this.relightToolStripMenuItem.Click += new System.EventHandler(this.relightToolStripMenuItem_Click);
      // 
      // showZoneAssetsToolStripMenuItem
      // 
      this.showZoneAssetsToolStripMenuItem.Image = global::Editor.Properties.Resources.memory;
      this.showZoneAssetsToolStripMenuItem.Name = "showZoneAssetsToolStripMenuItem";
      this.showZoneAssetsToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
      this.showZoneAssetsToolStripMenuItem.Text = "Show Zone Assets";
      this.showZoneAssetsToolStripMenuItem.Click += new System.EventHandler(this.showZoneAssetsToolStripMenuItem_Click);
      // 
      // toolStripSeparator2
      // 
      this.toolStripSeparator2.Name = "toolStripSeparator2";
      this.toolStripSeparator2.Size = new System.Drawing.Size(166, 6);
      // 
      // toolStripMenuItem_DeleteZone
      // 
      this.toolStripMenuItem_DeleteZone.Image = global::Editor.Properties.Resources.delete2;
      this.toolStripMenuItem_DeleteZone.Name = "toolStripMenuItem_DeleteZone";
      this.toolStripMenuItem_DeleteZone.Size = new System.Drawing.Size(169, 22);
      this.toolStripMenuItem_DeleteZone.Text = "Delete";
      this.toolStripMenuItem_DeleteZone.ToolTipText = "Deletes the selected zone";
      this.toolStripMenuItem_DeleteZone.Click += new System.EventHandler(this.toolStripMenuItem_DeleteZone_Click);
      // 
      // toolStripMenuItem_RenameZone
      // 
      this.toolStripMenuItem_RenameZone.Image = global::Editor.Properties.Resources.rename;
      this.toolStripMenuItem_RenameZone.Name = "toolStripMenuItem_RenameZone";
      this.toolStripMenuItem_RenameZone.Size = new System.Drawing.Size(169, 22);
      this.toolStripMenuItem_RenameZone.Text = "Rename";
      this.toolStripMenuItem_RenameZone.ToolTipText = "Renames the selected zone";
      this.toolStripMenuItem_RenameZone.Click += new System.EventHandler(this.toolStripMenuItem_RenameZone_Click);
      // 
      // toolStrip_Shapes
      // 
      this.toolStrip_Shapes.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Shapes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_AddGroup,
            this.toolStripButton_DeleteShape,
            this.toolStripSeparator3,
            this.toolStripButton_ShapeMoveDown,
            this.toolStripButton_ShapeMoveUp,
            this.toolStripButton_SortShapesAZUp,
            this.toolStripButton_SortShapesAZDown,
            this.toolStripSeparator4,
            this.toolStripSplitButton_MoveToLayer,
            this.toolStripSplitButton_ToolsOptions});
      this.toolStrip_Shapes.Location = new System.Drawing.Point(0, 0);
      this.toolStrip_Shapes.Name = "toolStrip_Shapes";
      this.toolStrip_Shapes.Size = new System.Drawing.Size(397, 25);
      this.toolStrip_Shapes.TabIndex = 0;
      this.toolStrip_Shapes.Text = "toolStrip2";
      // 
      // toolStripButton_AddGroup
      // 
      this.toolStripButton_AddGroup.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_AddGroup.Image = global::Editor.Properties.Resources.folder_new;
      this.toolStripButton_AddGroup.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_AddGroup.Name = "toolStripButton_AddGroup";
      this.toolStripButton_AddGroup.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_AddGroup.Text = "toolStripButton1";
      this.toolStripButton_AddGroup.ToolTipText = "Adds new group to layer";
      this.toolStripButton_AddGroup.Click += new System.EventHandler(this.toolStripButton_AddGroup_Click);
      // 
      // toolStripButton_DeleteShape
      // 
      this.toolStripButton_DeleteShape.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_DeleteShape.Image = global::Editor.Properties.Resources.delete2;
      this.toolStripButton_DeleteShape.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_DeleteShape.Name = "toolStripButton_DeleteShape";
      this.toolStripButton_DeleteShape.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_DeleteShape.Text = "toolStripButton2";
      this.toolStripButton_DeleteShape.ToolTipText = "Delete the selected shape";
      this.toolStripButton_DeleteShape.Click += new System.EventHandler(this.toolStripButton_DeleteShape_Click);
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripButton_ShapeMoveDown
      // 
      this.toolStripButton_ShapeMoveDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ShapeMoveDown.Image = global::Editor.Properties.Resources.arrow_down_blue;
      this.toolStripButton_ShapeMoveDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ShapeMoveDown.Name = "toolStripButton_ShapeMoveDown";
      this.toolStripButton_ShapeMoveDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ShapeMoveDown.Text = "toolStripButton3";
      this.toolStripButton_ShapeMoveDown.ToolTipText = "Move the shape one item down in the shape list";
      this.toolStripButton_ShapeMoveDown.Click += new System.EventHandler(this.toolStripButton_ShapeMoveDown_Click);
      // 
      // toolStripButton_ShapeMoveUp
      // 
      this.toolStripButton_ShapeMoveUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_ShapeMoveUp.Image = global::Editor.Properties.Resources.arrow_up_blue;
      this.toolStripButton_ShapeMoveUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_ShapeMoveUp.Name = "toolStripButton_ShapeMoveUp";
      this.toolStripButton_ShapeMoveUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_ShapeMoveUp.Text = "toolStripButton4";
      this.toolStripButton_ShapeMoveUp.ToolTipText = "Move the shape one layer up in the shape list";
      this.toolStripButton_ShapeMoveUp.Click += new System.EventHandler(this.toolStripButton_ShapeMoveUp_Click);
      // 
      // toolStripButton_SortShapesAZUp
      // 
      this.toolStripButton_SortShapesAZUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortShapesAZUp.Image = global::Editor.Properties.Resources.sort_az_descending;
      this.toolStripButton_SortShapesAZUp.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortShapesAZUp.Name = "toolStripButton_SortShapesAZUp";
      this.toolStripButton_SortShapesAZUp.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortShapesAZUp.Text = "toolStripButton5";
      this.toolStripButton_SortShapesAZUp.ToolTipText = "Sort the shapes alphabetically by name";
      this.toolStripButton_SortShapesAZUp.Click += new System.EventHandler(this.toolStripButton_SortShapesAZUp_Click);
      // 
      // toolStripButton_SortShapesAZDown
      // 
      this.toolStripButton_SortShapesAZDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton_SortShapesAZDown.Image = global::Editor.Properties.Resources.sort_az_ascending;
      this.toolStripButton_SortShapesAZDown.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton_SortShapesAZDown.Name = "toolStripButton_SortShapesAZDown";
      this.toolStripButton_SortShapesAZDown.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton_SortShapesAZDown.Text = "toolStripButton6";
      this.toolStripButton_SortShapesAZDown.ToolTipText = "Sort the shapes alphabetically by name";
      this.toolStripButton_SortShapesAZDown.Click += new System.EventHandler(this.toolStripButton_SortShapesAZDown_Click);
      // 
      // toolStripSeparator4
      // 
      this.toolStripSeparator4.Name = "toolStripSeparator4";
      this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripSplitButton_MoveToLayer
      // 
      this.toolStripSplitButton_MoveToLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripSplitButton_MoveToLayer.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.layersGoHereToolStripMenuItem1});
      this.toolStripSplitButton_MoveToLayer.Image = global::Editor.Properties.Resources.breakpoint_into;
      this.toolStripSplitButton_MoveToLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripSplitButton_MoveToLayer.Name = "toolStripSplitButton_MoveToLayer";
      this.toolStripSplitButton_MoveToLayer.Size = new System.Drawing.Size(32, 22);
      this.toolStripSplitButton_MoveToLayer.Text = "toolStripSplitButton1";
      this.toolStripSplitButton_MoveToLayer.ToolTipText = "Move selected shape to a different layer";
      this.toolStripSplitButton_MoveToLayer.ButtonClick += new System.EventHandler(this.toolStripSplitButton_MoveToLayer_ButtonClick);
      this.toolStripSplitButton_MoveToLayer.DropDownOpening += new System.EventHandler(this.toolStripSplitButton_MoveToLayer_DropDownOpening);
      // 
      // layersGoHereToolStripMenuItem1
      // 
      this.layersGoHereToolStripMenuItem1.Name = "layersGoHereToolStripMenuItem1";
      this.layersGoHereToolStripMenuItem1.Size = new System.Drawing.Size(166, 22);
      this.layersGoHereToolStripMenuItem1.Text = "<Layers go here>";
      // 
      // toolStripSplitButton_ToolsOptions
      // 
      this.toolStripSplitButton_ToolsOptions.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripSplitButton_ToolsOptions.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripSplitButton_AutomaticFocus,
            this.toolStripSplitButton_PropertiesOnSelection});
      this.toolStripSplitButton_ToolsOptions.Image = global::Editor.Properties.Resources.toolbar_selection_tools;
      this.toolStripSplitButton_ToolsOptions.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripSplitButton_ToolsOptions.Name = "toolStripSplitButton_ToolsOptions";
      this.toolStripSplitButton_ToolsOptions.Size = new System.Drawing.Size(32, 22);
      this.toolStripSplitButton_ToolsOptions.ToolTipText = "Options for the Shapes Tree View";
      this.toolStripSplitButton_ToolsOptions.DropDownOpening += new System.EventHandler(this.toolStripSplitButton_ToolsOptions_DropDownOpening);
      // 
      // toolStripSplitButton_AutomaticFocus
      // 
      this.toolStripSplitButton_AutomaticFocus.CheckOnClick = true;
      this.toolStripSplitButton_AutomaticFocus.Image = global::Editor.Properties.Resources.toolbar_focus_selection;
      this.toolStripSplitButton_AutomaticFocus.Name = "toolStripSplitButton_AutomaticFocus";
      this.toolStripSplitButton_AutomaticFocus.Size = new System.Drawing.Size(261, 22);
      this.toolStripSplitButton_AutomaticFocus.Text = "Automatic Focus On Selection";
      this.toolStripSplitButton_AutomaticFocus.Click += new System.EventHandler(this.toolStripSplitButton_AutomaticFocus_Click);
      // 
      // toolStripSplitButton_PropertiesOnSelection
      // 
      this.toolStripSplitButton_PropertiesOnSelection.CheckOnClick = true;
      this.toolStripSplitButton_PropertiesOnSelection.Image = global::Editor.Properties.Resources.toolbar_properties_selection;
      this.toolStripSplitButton_PropertiesOnSelection.Name = "toolStripSplitButton_PropertiesOnSelection";
      this.toolStripSplitButton_PropertiesOnSelection.Size = new System.Drawing.Size(261, 22);
      this.toolStripSplitButton_PropertiesOnSelection.Text = "Show Properties Panel On Selection";
      this.toolStripSplitButton_PropertiesOnSelection.Visible = false;
      this.toolStripSplitButton_PropertiesOnSelection.Click += new System.EventHandler(this.toolStripSplitButton_PropertiesOnSelection_Click);
      // 
      // toolStripSeparator6
      // 
      this.toolStripSeparator6.Name = "toolStripSeparator6";
      this.toolStripSeparator6.Size = new System.Drawing.Size(6, 25);
      // 
      // toolStripLabelSearch
      // 
      this.toolStripLabelSearch.Image = ((System.Drawing.Image)(resources.GetObject("toolStripLabelSearch.Image")));
      this.toolStripLabelSearch.Name = "toolStripLabelSearch";
      this.toolStripLabelSearch.Size = new System.Drawing.Size(16, 22);
      this.toolStripLabelSearch.ToolTipText = "Filter shapes according to search term";
      // 
      // searchPanel
      // 
      this.searchPanel.AutoSize = false;
      this.searchPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.searchPanel.ForeColor = System.Drawing.Color.Silver;
      this.searchPanel.Name = "searchPanel";
      this.searchPanel.Size = new System.Drawing.Size(100, 23);
      this.searchPanel.Text = "Search";
      // 
      // toolStripSeparator7
      // 
      this.toolStripSeparator7.Name = "toolStripSeparator7";
      this.toolStripSeparator7.Size = new System.Drawing.Size(6, 25);
      // 
      // panel_UpdateLayer
      // 
      this.panel_UpdateLayer.AutoSize = true;
      this.panel_UpdateLayer.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.panel_UpdateLayer.ColumnCount = 2;
      this.panel_UpdateLayer.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40F));
      this.panel_UpdateLayer.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.panel_UpdateLayer.Controls.Add(this.pictureBox_UpdateIcon, 0, 0);
      this.panel_UpdateLayer.Controls.Add(this.flowLayoutPanel1, 1, 1);
      this.panel_UpdateLayer.Controls.Add(this.label1, 1, 0);
      this.panel_UpdateLayer.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel_UpdateLayer.Location = new System.Drawing.Point(0, 596);
      this.panel_UpdateLayer.Name = "panel_UpdateLayer";
      this.panel_UpdateLayer.RowCount = 2;
      this.panel_UpdateLayer.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.panel_UpdateLayer.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.panel_UpdateLayer.Size = new System.Drawing.Size(397, 56);
      this.panel_UpdateLayer.TabIndex = 6;
      // 
      // pictureBox_UpdateIcon
      // 
      this.pictureBox_UpdateIcon.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.pictureBox_UpdateIcon.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox_UpdateIcon.Image")));
      this.pictureBox_UpdateIcon.Location = new System.Drawing.Point(6, 6);
      this.pictureBox_UpdateIcon.Margin = new System.Windows.Forms.Padding(6, 6, 3, 3);
      this.pictureBox_UpdateIcon.Name = "pictureBox_UpdateIcon";
      this.panel_UpdateLayer.SetRowSpan(this.pictureBox_UpdateIcon, 2);
      this.pictureBox_UpdateIcon.Size = new System.Drawing.Size(31, 47);
      this.pictureBox_UpdateIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
      this.pictureBox_UpdateIcon.TabIndex = 0;
      this.pictureBox_UpdateIcon.TabStop = false;
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.AutoSize = true;
      this.flowLayoutPanel1.Controls.Add(this.button_UpdateIgnore);
      this.flowLayoutPanel1.Controls.Add(this.button_LayerUpdate);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(43, 24);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(351, 29);
      this.flowLayoutPanel1.TabIndex = 1;
      // 
      // button_UpdateIgnore
      // 
      this.button_UpdateIgnore.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_UpdateIgnore.Location = new System.Drawing.Point(288, 3);
      this.button_UpdateIgnore.Name = "button_UpdateIgnore";
      this.button_UpdateIgnore.Size = new System.Drawing.Size(60, 23);
      this.button_UpdateIgnore.TabIndex = 2;
      this.button_UpdateIgnore.Text = "Ignore";
      this.button_UpdateIgnore.Click += new System.EventHandler(this.button_UpdateIgnore_Click);
      // 
      // button_LayerUpdate
      // 
      this.button_LayerUpdate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_LayerUpdate.Location = new System.Drawing.Point(222, 3);
      this.button_LayerUpdate.Name = "button_LayerUpdate";
      this.button_LayerUpdate.Size = new System.Drawing.Size(60, 23);
      this.button_LayerUpdate.TabIndex = 1;
      this.button_LayerUpdate.Text = "Update";
      this.button_LayerUpdate.Click += new System.EventHandler(this.button_LayerUpdate_Click);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(43, 4);
      this.label1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(191, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "One or more layers have been modified";
      // 
      // propertiesToolStripMenuItem
      // 
      this.propertiesToolStripMenuItem.Image = global::Editor.Properties.Resources.wrench;
      this.propertiesToolStripMenuItem.Name = "propertiesToolStripMenuItem";
      this.propertiesToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
      this.propertiesToolStripMenuItem.Text = "Properties";
      this.propertiesToolStripMenuItem.Click += new System.EventHandler(this.propertiesToolStripMenuItem_Click);
      // 
      // toolStrip_Search
      // 
      this.toolStrip_Search.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip_Search.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabelSearch,
            this.searchPanel,
            this.toolStripButtonClearSearch});
      this.toolStrip_Search.Location = new System.Drawing.Point(0, 25);
      this.toolStrip_Search.Name = "toolStrip_Search";
      this.toolStrip_Search.Size = new System.Drawing.Size(397, 25);
      this.toolStrip_Search.TabIndex = 7;
      this.toolStrip_Search.Text = "toolStrip2";
      this.toolStrip_Search.Layout += new System.Windows.Forms.LayoutEventHandler(this.toolStrip_Search_Layout);
      // 
      // toolStripButtonClearSearch
      // 
      this.toolStripButtonClearSearch.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButtonClearSearch.Enabled = false;
      this.toolStripButtonClearSearch.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonClearSearch.Image")));
      this.toolStripButtonClearSearch.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButtonClearSearch.Name = "toolStripButtonClearSearch";
      this.toolStripButtonClearSearch.Size = new System.Drawing.Size(23, 22);
      this.toolStripButtonClearSearch.ToolTipText = "Clear Search";
      this.toolStripButtonClearSearch.Click += new System.EventHandler(this.toolStripButtonClearSearch_Click);
      // 
      // ShapeTreePanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(397, 652);
      this.Controls.Add(this.panel_UpdateLayer);
      this.Controls.Add(this.treeView_Layers);
      this.Controls.Add(this.toolStrip1);
      this.Controls.Add(this.splitter1);
      this.Controls.Add(this.shapeTreeView);
      this.Controls.Add(this.toolStrip_Search);
      this.Controls.Add(this.toolStrip_Shapes);
      this.DockableAreas = ((WeifenLuo.WinFormsUI.DockAreas)(((WeifenLuo.WinFormsUI.DockAreas.Float | WeifenLuo.WinFormsUI.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.DockAreas.DockRight)));
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ShapeTreePanel";
      this.ShowHint = WeifenLuo.WinFormsUI.DockState.DockLeft;
      this.TabText = "Shapes";
      this.Text = "Shapes and Layers";
      this.contextMenuStrip_Shapes.ResumeLayout(false);
      this.contextMenuStrip_Layer.ResumeLayout(false);
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      this.contextMenuStrip_Zones.ResumeLayout(false);
      this.toolStrip_Shapes.ResumeLayout(false);
      this.toolStrip_Shapes.PerformLayout();
      this.panel_UpdateLayer.ResumeLayout(false);
      this.panel_UpdateLayer.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox_UpdateIcon)).EndInit();
      this.flowLayoutPanel1.ResumeLayout(false);
      this.toolStrip_Search.ResumeLayout(false);
      this.toolStrip_Search.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
    #endregion

    protected override void Dispose(bool disposing)
    {
      EditorManager.SceneChanged -= new SceneChangedEventHandler(shapeTreeView.OnSceneChanged);
      EditorManager.SceneChanged -= new SceneChangedEventHandler(EditorManager_SceneChanged);

      EditorScene.ShapeChanged -= new ShapeChangedEventHandler(shapeTreeView.OnShapeChanged);
      EditorScene.LayerChanged -= new LayerChangedEventHandler(shapeTreeView.OnLayerChanged);
      EditorScene.LayerChanged -= new LayerChangedEventHandler(EditorScene_LayerChanged);

      //EditorScene.LayerChanged -= new LayerChangedEventHandler(layerListView.OnLayerChanged);
      //EditorManager.SceneChanged -= new SceneChangedEventHandler(layerListView.OnSceneChanged);

      CSharpFramework.Layer.ActiveShapeChanged -= new CSharpFramework.Layer.ActiveShapeChangedEventHandler(Layer_ActiveShapeChanged);
      searchPanel.FilterChanged -= new EventHandler(searchPanel_FilterChanged);

      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }

    private ToolStripSeparator toolStripSeparator6;
    private ToolStripLabel toolStripLabelSearch;
    private IncrementalSearchPanel searchPanel;
    private ToolStripSeparator toolStripSeparator7;
    private ToolStripMenuItem toolStripMenuItem2;
    private ToolStripMenuItem cameraPositionToolStripMenuItem;
    private ToolStripMenuItem shapeOriginToolStripMenuItem;
    private ToolStripMenuItem propertiesToolStripMenuItem;    
    private ToolStrip toolStrip_Search;
    private ToolStripButton toolStripButtonClearSearch;    
  }
}
