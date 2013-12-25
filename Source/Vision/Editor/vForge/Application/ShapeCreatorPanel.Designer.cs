using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using System.Collections;
using CSharpFramework.Controls;
using CSharpFramework;
using CSharpFramework.Scene;

namespace Editor
{
  public partial class ShapeCreatorPanel
  {
    #region Windows Form Designer generated code
    private System.Windows.Forms.Label label_Description;
    private System.Windows.Forms.Splitter splitter1;
    private TreeView treeView_Creators;
    private ToolTip toolTip1;
    private ToolStrip toolStrip;
    private ToolStripDropDownButton recentShapesToolButton;
    private ToolStripButton toolButton_ExpandAll;
    private ToolStripButton toolButton_CollapseAll;
    private ToolStripSeparator toolStripSeparator2;
    private IncrementalSearchPanel searchPanel;
    private ToolStripSeparator toolStripSeparator1;
    private ToolStripButton toolStripButtonClearSearch;
    private ToolStripLabel toolStripLabelSearch;

    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ShapeCreatorPanel));
      this.label_Description = new System.Windows.Forms.Label();
      this.splitter1 = new System.Windows.Forms.Splitter();
      this.treeView_Creators = new System.Windows.Forms.TreeView();
      this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
      this.toolStrip = new System.Windows.Forms.ToolStrip();
      this.recentShapesToolButton = new System.Windows.Forms.ToolStripDropDownButton();
      this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
      this.toolButton_CollapseAll = new System.Windows.Forms.ToolStripButton();
      this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
      this.toolButton_ExpandAll = new System.Windows.Forms.ToolStripButton();
      this.toolStripLabelSearch = new System.Windows.Forms.ToolStripLabel();
      this.searchPanel = new CSharpFramework.Controls.IncrementalSearchPanel();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.toolStripButtonClearSearch = new System.Windows.Forms.ToolStripButton();
      this.toolStrip.SuspendLayout();
      this.SuspendLayout();
      // 
      // label_Description
      // 
      this.label_Description.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.label_Description.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.label_Description.Location = new System.Drawing.Point(0, 506);
      this.label_Description.Name = "label_Description";
      this.label_Description.Size = new System.Drawing.Size(324, 44);
      this.label_Description.TabIndex = 1;
      this.label_Description.Text = "Description goes here";
      // 
      // splitter1
      // 
      this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.splitter1.Location = new System.Drawing.Point(0, 504);
      this.splitter1.Name = "splitter1";
      this.splitter1.Size = new System.Drawing.Size(324, 2);
      this.splitter1.TabIndex = 2;
      this.splitter1.TabStop = false;
      // 
      // treeView_Creators
      // 
      this.treeView_Creators.Dock = System.Windows.Forms.DockStyle.Fill;
      this.treeView_Creators.Location = new System.Drawing.Point(0, 25);
      this.treeView_Creators.Name = "treeView_Creators";
      this.treeView_Creators.Size = new System.Drawing.Size(324, 479);
      this.treeView_Creators.TabIndex = 1;
      this.treeView_Creators.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeView_Creators_ItemDrag);
      this.treeView_Creators.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_Creators_AfterSelect);
      this.treeView_Creators.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_Creators_NodeMouseClick);
      this.treeView_Creators.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_Creators_NodeMouseDoubleClick);
      this.treeView_Creators.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView_Creators_KeyDown);
      // 
      // toolStrip
      // 
      this.toolStrip.Enabled = false;
      this.toolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
      this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.recentShapesToolButton,
            this.toolStripSeparator3,
            this.toolButton_CollapseAll,
            this.toolStripSeparator2,
            this.toolButton_ExpandAll,
            this.toolStripLabelSearch,
            this.searchPanel,
            this.toolStripSeparator1,
            this.toolStripButtonClearSearch});
      this.toolStrip.Location = new System.Drawing.Point(0, 0);
      this.toolStrip.Name = "toolStrip";
      this.toolStrip.Size = new System.Drawing.Size(324, 25);
      this.toolStrip.TabIndex = 4;
      this.toolStrip.Text = "toolStrip";
      this.toolStrip.Layout += new System.Windows.Forms.LayoutEventHandler(this.toolStrip_Layout);
      // 
      // recentShapesToolButton
      // 
      this.recentShapesToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.recentShapesToolButton.Image = global::Editor.Properties.Resources.pawn_new;
      this.recentShapesToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.recentShapesToolButton.Name = "recentShapesToolButton";
      this.recentShapesToolButton.Size = new System.Drawing.Size(29, 22);
      this.recentShapesToolButton.ToolTipText = "Recently Used Shapes";
      // 
      // toolStripSeparator3
      // 
      this.toolStripSeparator3.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
      this.toolStripSeparator3.Name = "toolStripSeparator3";
      this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
      // 
      // toolButton_CollapseAll
      // 
      this.toolButton_CollapseAll.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
      this.toolButton_CollapseAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolButton_CollapseAll.Image = global::Editor.Properties.Resources.collapse_all;
      this.toolButton_CollapseAll.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolButton_CollapseAll.Name = "toolButton_CollapseAll";
      this.toolButton_CollapseAll.Size = new System.Drawing.Size(23, 22);
      this.toolButton_CollapseAll.Click += new System.EventHandler(this.toolButton_CollapseAll_Click);
      // 
      // toolStripSeparator2
      // 
      this.toolStripSeparator2.Name = "toolStripSeparator2";
      this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
      // 
      // toolButton_ExpandAll
      // 
      this.toolButton_ExpandAll.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
      this.toolButton_ExpandAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolButton_ExpandAll.Image = global::Editor.Properties.Resources.expand_all;
      this.toolButton_ExpandAll.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolButton_ExpandAll.Name = "toolButton_ExpandAll";
      this.toolButton_ExpandAll.Size = new System.Drawing.Size(23, 22);
      this.toolButton_ExpandAll.ToolTipText = "Expand All";
      this.toolButton_ExpandAll.Click += new System.EventHandler(this.toolButton_ExpandAll_Click);
      // 
      // toolStripLabelSearch
      // 
      this.toolStripLabelSearch.Image = ((System.Drawing.Image)(resources.GetObject("toolStripLabelSearch.Image")));
      this.toolStripLabelSearch.Name = "toolStripLabelSearch";
      this.toolStripLabelSearch.Size = new System.Drawing.Size(61, 22);
      this.toolStripLabelSearch.Text = "Search:";
      // 
      // searchPanel
      // 
      this.searchPanel.AutoSize = false;
      this.searchPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.searchPanel.ForeColor = System.Drawing.Color.Silver;
      this.searchPanel.Name = "searchPanel";
      this.searchPanel.Size = new System.Drawing.Size(100, 23);
      this.searchPanel.Text = "Enter your search token here...";
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
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
      // ShapeCreatorPanel
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(324, 550);
      this.Controls.Add(this.treeView_Creators);
      this.Controls.Add(this.toolStrip);
      this.Controls.Add(this.splitter1);
      this.Controls.Add(this.label_Description);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "ShapeCreatorPanel";
      this.TabText = "Shape Creators";
      this.Text = "Shape Creators";
      this.toolStrip.ResumeLayout(false);
      this.toolStrip.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
    #endregion

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        IProject.NewProjectLoaded -= new EventHandler(IProject_NewProjectLoaded);
        IProject.ProjectUnloaded -= new EventHandler(IProject_ProjectUnloaded);
        PrefabDesc.OnPrefabSaved -= new EventHandler(PrefabDesc_OnPrefabSaved);

        searchPanel.FilterChanged -= new EventHandler(searchPanel_FilterChanged);
        EditorManager.EditorSettingsChanged -= new EditorSettingsChangedEventHandler(EditorManager_EditorSettingsChanged);
        EditorManager.GUI.ShapeCreatorUpdateRequest -= new EventHandler(EditorManager_GUI_ShapeCreatorUpdateRequest);
        if (components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }

    private ToolStripSeparator toolStripSeparator3;
  }
}
