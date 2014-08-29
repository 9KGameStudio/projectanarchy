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
using CSharpFramework.Dialogs;
using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Controls;
using ManagedFramework;
using CSharpFramework.Helper;
using System.IO;
using System.Reflection;
using System.Collections.Generic;
using CSharpFramework.AssetManagement;
using CSharpFramework.BaseTypes;

namespace Editor.Dialogs
{
	/// <summary>
	/// Export Dialog class.
	/// </summary>
	public class ExportDialog : System.Windows.Forms.Form
	{
    private System.Windows.Forms.Button button_ExportActiveProfile;
    private System.Windows.Forms.Button button_CANCEL;
    private CSharpFramework.Controls.DialogCaptionBar captionBar;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Button button_Browse;
    private System.Windows.Forms.TextBox text_Pathname;
    private System.Windows.Forms.GroupBox settingsBox;
    private System.Windows.Forms.CheckBox checkBox_RunAfterExport;
    private System.Windows.Forms.GroupBox groupBox2;
    private CheckBox checkBox_IncludeVisibilityInfo;
    private GroupBox groupBox3;
    private Button button_CreateProfile;
    private ComboBox comboBox_Profile;
    private Button button_SaveProfile;
    private Label label1;
    private Label label2;
    private CheckBox checkBox_InvisibleLayersExportInvisibleObjects;
    private Label label3;
    private CheckBox checkBox_updateAssetTransformations;
    private Button button_ExportSelectedProfiles;
    private Label label_PathExtension;
    private CheckBox checkBox_SaveProfileOnExport;
    private ProfileDropDownControl dropDown_AssetProfiles;
    private TableLayoutPanel tableLayoutPanel1;
    private Button button_ShowAssets;
    private Button button_AllLayers;
    private Button button_InvertLayers;
    private CheckBox checkBox_LoadedZonesOnly;
    private ListView listView_Layers;
    private ColumnHeader nameHeader;
    private ColumnHeader descriptionHeader;
    private TableLayoutPanel checkBoxTableLayout;
    private Button button_DeleteProfile;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

    /// <summary>
    /// Constructor
    /// </summary>
		public ExportDialog()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

      // re-use this image list
      listView_Layers.SmallImageList = LayerTreeViewNodeBase.ImageList.ImageList;

      FillProfileCombobox();

      // Set up a ToolTip
      ToolTip toolTip = new ToolTip();
      toolTip.AutoPopDelay = 5000;
      toolTip.InitialDelay = 1000;
      toolTip.ReshowDelay = 500;
      toolTip.SetToolTip(this.checkBox_RunAfterExport, "Uses the vPlayer.exe in the same directory as the running vForge.exe");
		}

    private void ExportDialog_Load(object sender, EventArgs e)
    {
      //force the dialog loading the as "default" selected configuration
      comboBox_Profile_SelectionChangeCommitted(null, null);
    }

    void ExportDialog_ResizeEnd(object sender, EventArgs e)
    {
      dropDown_AssetProfiles.Refresh();
    }

    protected override void Dispose(bool disposing)
		{
			if(disposing)
			{
				if(components != null)
				{
					components.Dispose();
				}
        comboBox_Profile.Dispose();
			}
			base.Dispose(disposing);
		}


		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExportDialog));
      System.Windows.Forms.ListViewGroup listViewGroup5 = new System.Windows.Forms.ListViewGroup("Layers to export", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup6 = new System.Windows.Forms.ListViewGroup("Zones to export", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup7 = new System.Windows.Forms.ListViewGroup("Scene Components to export", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewGroup listViewGroup8 = new System.Windows.Forms.ListViewGroup("Optional Plugins to embed", System.Windows.Forms.HorizontalAlignment.Left);
      System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem("xxx");
      System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem("yyy");
      System.Windows.Forms.ListViewItem listViewItem7 = new System.Windows.Forms.ListViewItem("zzz");
      System.Windows.Forms.ListViewItem listViewItem8 = new System.Windows.Forms.ListViewItem("www");
      this.button_ExportActiveProfile = new System.Windows.Forms.Button();
      this.button_CANCEL = new System.Windows.Forms.Button();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.dropDown_AssetProfiles = new CSharpFramework.Controls.ProfileDropDownControl();
      this.label_PathExtension = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.button_Browse = new System.Windows.Forms.Button();
      this.text_Pathname = new System.Windows.Forms.TextBox();
      this.settingsBox = new System.Windows.Forms.GroupBox();
      this.checkBoxTableLayout = new System.Windows.Forms.TableLayoutPanel();
      this.checkBox_IncludeVisibilityInfo = new System.Windows.Forms.CheckBox();
      this.checkBox_RunAfterExport = new System.Windows.Forms.CheckBox();
      this.checkBox_InvisibleLayersExportInvisibleObjects = new System.Windows.Forms.CheckBox();
      this.checkBox_updateAssetTransformations = new System.Windows.Forms.CheckBox();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.button_ShowAssets = new System.Windows.Forms.Button();
      this.button_AllLayers = new System.Windows.Forms.Button();
      this.button_InvertLayers = new System.Windows.Forms.Button();
      this.checkBox_LoadedZonesOnly = new System.Windows.Forms.CheckBox();
      this.listView_Layers = new System.Windows.Forms.ListView();
      this.nameHeader = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.descriptionHeader = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.button_DeleteProfile = new System.Windows.Forms.Button();
      this.checkBox_SaveProfileOnExport = new System.Windows.Forms.CheckBox();
      this.label2 = new System.Windows.Forms.Label();
      this.button_SaveProfile = new System.Windows.Forms.Button();
      this.button_CreateProfile = new System.Windows.Forms.Button();
      this.comboBox_Profile = new System.Windows.Forms.ComboBox();
      this.button_ExportSelectedProfiles = new System.Windows.Forms.Button();
      this.captionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.groupBox1.SuspendLayout();
      this.settingsBox.SuspendLayout();
      this.checkBoxTableLayout.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.tableLayoutPanel1.SuspendLayout();
      this.groupBox3.SuspendLayout();
      this.SuspendLayout();
      // 
      // button_ExportActiveProfile
      // 
      this.button_ExportActiveProfile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportActiveProfile.Image = ((System.Drawing.Image)(resources.GetObject("button_ExportActiveProfile.Image")));
      this.button_ExportActiveProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_ExportActiveProfile.Location = new System.Drawing.Point(304, 666);
      this.button_ExportActiveProfile.Name = "button_ExportActiveProfile";
      this.button_ExportActiveProfile.Size = new System.Drawing.Size(135, 28);
      this.button_ExportActiveProfile.TabIndex = 6;
      this.button_ExportActiveProfile.Text = "Export Active Profile";
      this.button_ExportActiveProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_ExportActiveProfile.Click += new System.EventHandler(this.button_ExportActiveProfile_Click);
      // 
      // button_CANCEL
      // 
      this.button_CANCEL.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.button_CANCEL.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_CANCEL.Location = new System.Drawing.Point(444, 666);
      this.button_CANCEL.Name = "button_CANCEL";
      this.button_CANCEL.Size = new System.Drawing.Size(97, 28);
      this.button_CANCEL.TabIndex = 7;
      this.button_CANCEL.Text = "Cancel";
      // 
      // groupBox1
      // 
      this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox1.Controls.Add(this.dropDown_AssetProfiles);
      this.groupBox1.Controls.Add(this.label_PathExtension);
      this.groupBox1.Controls.Add(this.label3);
      this.groupBox1.Controls.Add(this.label1);
      this.groupBox1.Controls.Add(this.button_Browse);
      this.groupBox1.Controls.Add(this.text_Pathname);
      this.groupBox1.Location = new System.Drawing.Point(12, 138);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(529, 68);
      this.groupBox1.TabIndex = 2;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Target";
      // 
      // dropDown_AssetProfiles
      // 
      this.dropDown_AssetProfiles.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.dropDown_AssetProfiles.AnchorSize = new System.Drawing.Size(417, 20);
      this.dropDown_AssetProfiles.BackColor = System.Drawing.Color.White;
      this.dropDown_AssetProfiles.DockSide = CSharpFramework.Controls.DropDownControl.eDockSide.Left;
      this.dropDown_AssetProfiles.Location = new System.Drawing.Point(104, 42);
      this.dropDown_AssetProfiles.Name = "dropDown_AssetProfiles";
      this.dropDown_AssetProfiles.Size = new System.Drawing.Size(417, 20);
      this.dropDown_AssetProfiles.TabIndex = 5;
      this.dropDown_AssetProfiles.ProfileSelectionChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // label_PathExtension
      // 
      this.label_PathExtension.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label_PathExtension.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label_PathExtension.Location = new System.Drawing.Point(374, 15);
      this.label_PathExtension.Name = "label_PathExtension";
      this.label_PathExtension.Size = new System.Drawing.Size(120, 19);
      this.label_PathExtension.TabIndex = 3;
      this.label_PathExtension.Text = ".[profile].vscene";
      this.label_PathExtension.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(13, 45);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(73, 13);
      this.label3.TabIndex = 4;
      this.label3.Text = "Asset Profiles:";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(13, 19);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(57, 13);
      this.label1.TabIndex = 0;
      this.label1.Text = "File Name:";
      // 
      // button_Browse
      // 
      this.button_Browse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_Browse.Location = new System.Drawing.Point(497, 13);
      this.button_Browse.Name = "button_Browse";
      this.button_Browse.Size = new System.Drawing.Size(24, 23);
      this.button_Browse.TabIndex = 2;
      this.button_Browse.Text = "...";
      this.button_Browse.Click += new System.EventHandler(this.button_Browse_Click);
      // 
      // text_Pathname
      // 
      this.text_Pathname.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.text_Pathname.Location = new System.Drawing.Point(104, 16);
      this.text_Pathname.Name = "text_Pathname";
      this.text_Pathname.Size = new System.Drawing.Size(265, 20);
      this.text_Pathname.TabIndex = 1;
      this.text_Pathname.Text = "textBox1";
      this.text_Pathname.TextChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // settingsBox
      // 
      this.settingsBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.settingsBox.Controls.Add(this.checkBoxTableLayout);
      this.settingsBox.Location = new System.Drawing.Point(12, 589);
      this.settingsBox.Name = "settingsBox";
      this.settingsBox.Size = new System.Drawing.Size(529, 69);
      this.settingsBox.TabIndex = 4;
      this.settingsBox.TabStop = false;
      this.settingsBox.Text = "Options";
      // 
      // checkBoxTableLayout
      // 
      this.checkBoxTableLayout.ColumnCount = 2;
      this.checkBoxTableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.checkBoxTableLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.checkBoxTableLayout.Controls.Add(this.checkBox_IncludeVisibilityInfo, 0, 0);
      this.checkBoxTableLayout.Controls.Add(this.checkBox_RunAfterExport, 1, 1);
      this.checkBoxTableLayout.Controls.Add(this.checkBox_InvisibleLayersExportInvisibleObjects, 0, 1);
      this.checkBoxTableLayout.Controls.Add(this.checkBox_updateAssetTransformations, 1, 0);
      this.checkBoxTableLayout.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBoxTableLayout.Location = new System.Drawing.Point(3, 16);
      this.checkBoxTableLayout.Name = "checkBoxTableLayout";
      this.checkBoxTableLayout.RowCount = 2;
      this.checkBoxTableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.checkBoxTableLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.checkBoxTableLayout.Size = new System.Drawing.Size(523, 50);
      this.checkBoxTableLayout.TabIndex = 8;
      // 
      // checkBox_IncludeVisibilityInfo
      // 
      this.checkBox_IncludeVisibilityInfo.AutoSize = true;
      this.checkBox_IncludeVisibilityInfo.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBox_IncludeVisibilityInfo.Location = new System.Drawing.Point(3, 3);
      this.checkBox_IncludeVisibilityInfo.Name = "checkBox_IncludeVisibilityInfo";
      this.checkBox_IncludeVisibilityInfo.Size = new System.Drawing.Size(255, 19);
      this.checkBox_IncludeVisibilityInfo.TabIndex = 0;
      this.checkBox_IncludeVisibilityInfo.Text = "Build and include Visibility Info";
      this.checkBox_IncludeVisibilityInfo.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // checkBox_RunAfterExport
      // 
      this.checkBox_RunAfterExport.AutoSize = true;
      this.checkBox_RunAfterExport.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBox_RunAfterExport.Location = new System.Drawing.Point(264, 28);
      this.checkBox_RunAfterExport.Name = "checkBox_RunAfterExport";
      this.checkBox_RunAfterExport.Size = new System.Drawing.Size(256, 19);
      this.checkBox_RunAfterExport.TabIndex = 1;
      this.checkBox_RunAfterExport.Text = "Run after Export";
      this.checkBox_RunAfterExport.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // checkBox_InvisibleLayersExportInvisibleObjects
      // 
      this.checkBox_InvisibleLayersExportInvisibleObjects.AutoSize = true;
      this.checkBox_InvisibleLayersExportInvisibleObjects.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBox_InvisibleLayersExportInvisibleObjects.Location = new System.Drawing.Point(3, 28);
      this.checkBox_InvisibleLayersExportInvisibleObjects.Name = "checkBox_InvisibleLayersExportInvisibleObjects";
      this.checkBox_InvisibleLayersExportInvisibleObjects.Size = new System.Drawing.Size(255, 19);
      this.checkBox_InvisibleLayersExportInvisibleObjects.TabIndex = 2;
      this.checkBox_InvisibleLayersExportInvisibleObjects.Text = "Invisible Layers export invisible Objects";
      this.checkBox_InvisibleLayersExportInvisibleObjects.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // checkBox_updateAssetTransformations
      // 
      this.checkBox_updateAssetTransformations.AutoSize = true;
      this.checkBox_updateAssetTransformations.Checked = true;
      this.checkBox_updateAssetTransformations.CheckState = System.Windows.Forms.CheckState.Checked;
      this.checkBox_updateAssetTransformations.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBox_updateAssetTransformations.Location = new System.Drawing.Point(264, 3);
      this.checkBox_updateAssetTransformations.Name = "checkBox_updateAssetTransformations";
      this.checkBox_updateAssetTransformations.Size = new System.Drawing.Size(256, 19);
      this.checkBox_updateAssetTransformations.TabIndex = 7;
      this.checkBox_updateAssetTransformations.Text = "Execute Asset Transformation Pipeline";
      this.checkBox_updateAssetTransformations.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // groupBox2
      // 
      this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox2.Controls.Add(this.tableLayoutPanel1);
      this.groupBox2.Location = new System.Drawing.Point(12, 212);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(529, 372);
      this.groupBox2.TabIndex = 3;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Layers and Zones to export";
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 4;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.Controls.Add(this.button_ShowAssets, 3, 1);
      this.tableLayoutPanel1.Controls.Add(this.button_AllLayers, 1, 1);
      this.tableLayoutPanel1.Controls.Add(this.button_InvertLayers, 2, 1);
      this.tableLayoutPanel1.Controls.Add(this.checkBox_LoadedZonesOnly, 0, 1);
      this.tableLayoutPanel1.Controls.Add(this.listView_Layers, 0, 0);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 16);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 2;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 19F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(523, 353);
      this.tableLayoutPanel1.TabIndex = 5;
      // 
      // button_ShowAssets
      // 
      this.button_ShowAssets.Dock = System.Windows.Forms.DockStyle.Fill;
      this.button_ShowAssets.Location = new System.Drawing.Point(430, 324);
      this.button_ShowAssets.Name = "button_ShowAssets";
      this.button_ShowAssets.Size = new System.Drawing.Size(90, 26);
      this.button_ShowAssets.TabIndex = 7;
      this.button_ShowAssets.Text = "Show Assets";
      this.button_ShowAssets.Click += new System.EventHandler(this.button_ShowAssets_Click);
      // 
      // button_AllLayers
      // 
      this.button_AllLayers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.button_AllLayers.Location = new System.Drawing.Point(238, 324);
      this.button_AllLayers.Name = "button_AllLayers";
      this.button_AllLayers.Size = new System.Drawing.Size(90, 26);
      this.button_AllLayers.TabIndex = 6;
      this.button_AllLayers.Text = "Select All";
      this.button_AllLayers.Click += new System.EventHandler(this.button_AllLayers_Click);
      // 
      // button_InvertLayers
      // 
      this.button_InvertLayers.Dock = System.Windows.Forms.DockStyle.Fill;
      this.button_InvertLayers.Location = new System.Drawing.Point(334, 324);
      this.button_InvertLayers.Name = "button_InvertLayers";
      this.button_InvertLayers.Size = new System.Drawing.Size(90, 26);
      this.button_InvertLayers.TabIndex = 5;
      this.button_InvertLayers.Text = "Invert Selection";
      this.button_InvertLayers.Click += new System.EventHandler(this.button_InvertLayers_Click);
      // 
      // checkBox_LoadedZonesOnly
      // 
      this.checkBox_LoadedZonesOnly.AutoSize = true;
      this.checkBox_LoadedZonesOnly.Dock = System.Windows.Forms.DockStyle.Fill;
      this.checkBox_LoadedZonesOnly.Location = new System.Drawing.Point(3, 329);
      this.checkBox_LoadedZonesOnly.Margin = new System.Windows.Forms.Padding(3, 8, 3, 7);
      this.checkBox_LoadedZonesOnly.Name = "checkBox_LoadedZonesOnly";
      this.checkBox_LoadedZonesOnly.Size = new System.Drawing.Size(229, 17);
      this.checkBox_LoadedZonesOnly.TabIndex = 2;
      this.checkBox_LoadedZonesOnly.Text = "Export loaded Zones only";
      this.checkBox_LoadedZonesOnly.CheckedChanged += new System.EventHandler(this.ProfileControlChanged);
      // 
      // listView_Layers
      // 
      this.listView_Layers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.listView_Layers.AutoArrange = false;
      this.listView_Layers.CheckBoxes = true;
      this.listView_Layers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.nameHeader,
            this.descriptionHeader});
      this.tableLayoutPanel1.SetColumnSpan(this.listView_Layers, 4);
      this.listView_Layers.FullRowSelect = true;
      this.listView_Layers.GridLines = true;
      listViewGroup5.Header = "Layers to export";
      listViewGroup5.Name = "Layers";
      listViewGroup6.Header = "Zones to export";
      listViewGroup6.Name = "Zones";
      listViewGroup7.Header = "Scene Components to export";
      listViewGroup7.Name = "SceneComponents";
      listViewGroup8.Header = "Optional Plugins to embed";
      listViewGroup8.Name = "Plugins";
      this.listView_Layers.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup5,
            listViewGroup6,
            listViewGroup7,
            listViewGroup8});
      listViewItem5.Group = listViewGroup5;
      listViewItem5.StateImageIndex = 0;
      listViewItem6.Group = listViewGroup6;
      listViewItem6.StateImageIndex = 0;
      listViewItem7.Group = listViewGroup7;
      listViewItem7.StateImageIndex = 0;
      listViewItem8.Group = listViewGroup8;
      listViewItem8.StateImageIndex = 0;
      this.listView_Layers.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem5,
            listViewItem6,
            listViewItem7,
            listViewItem8});
      this.listView_Layers.Location = new System.Drawing.Point(3, 3);
      this.listView_Layers.MultiSelect = false;
      this.listView_Layers.Name = "listView_Layers";
      this.listView_Layers.ShowItemToolTips = true;
      this.listView_Layers.Size = new System.Drawing.Size(517, 315);
      this.listView_Layers.TabIndex = 1;
      this.listView_Layers.UseCompatibleStateImageBehavior = false;
      this.listView_Layers.View = System.Windows.Forms.View.Details;
      this.listView_Layers.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.listView_Layers_ItemChecked);
      // 
      // nameHeader
      // 
      this.nameHeader.Text = "Name";
      this.nameHeader.Width = 200;
      // 
      // descriptionHeader
      // 
      this.descriptionHeader.Text = "Description";
      this.descriptionHeader.Width = 250;
      // 
      // groupBox3
      // 
      this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox3.Controls.Add(this.button_DeleteProfile);
      this.groupBox3.Controls.Add(this.checkBox_SaveProfileOnExport);
      this.groupBox3.Controls.Add(this.label2);
      this.groupBox3.Controls.Add(this.button_SaveProfile);
      this.groupBox3.Controls.Add(this.button_CreateProfile);
      this.groupBox3.Controls.Add(this.comboBox_Profile);
      this.groupBox3.Location = new System.Drawing.Point(12, 59);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(529, 74);
      this.groupBox3.TabIndex = 1;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "Export Presets";
      // 
      // button_DeleteProfile
      // 
      this.button_DeleteProfile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_DeleteProfile.Image = global::Editor.Properties.Resources.delete2;
      this.button_DeleteProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_DeleteProfile.Location = new System.Drawing.Point(291, 43);
      this.button_DeleteProfile.Name = "button_DeleteProfile";
      this.button_DeleteProfile.Size = new System.Drawing.Size(107, 24);
      this.button_DeleteProfile.TabIndex = 5;
      this.button_DeleteProfile.Text = "Delete Preset";
      this.button_DeleteProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_DeleteProfile.UseVisualStyleBackColor = true;
      this.button_DeleteProfile.Click += new System.EventHandler(this.button_DeleteProfile_Click);
      // 
      // checkBox_SaveProfileOnExport
      // 
      this.checkBox_SaveProfileOnExport.AutoSize = true;
      this.checkBox_SaveProfileOnExport.Location = new System.Drawing.Point(15, 48);
      this.checkBox_SaveProfileOnExport.Name = "checkBox_SaveProfileOnExport";
      this.checkBox_SaveProfileOnExport.Size = new System.Drawing.Size(132, 17);
      this.checkBox_SaveProfileOnExport.TabIndex = 4;
      this.checkBox_SaveProfileOnExport.Text = "Save Preset on Export";
      this.checkBox_SaveProfileOnExport.UseVisualStyleBackColor = true;
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(13, 22);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(40, 13);
      this.label2.TabIndex = 0;
      this.label2.Text = "Preset:";
      // 
      // button_SaveProfile
      // 
      this.button_SaveProfile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_SaveProfile.Image = ((System.Drawing.Image)(resources.GetObject("button_SaveProfile.Image")));
      this.button_SaveProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_SaveProfile.Location = new System.Drawing.Point(404, 43);
      this.button_SaveProfile.Name = "button_SaveProfile";
      this.button_SaveProfile.Size = new System.Drawing.Size(117, 24);
      this.button_SaveProfile.TabIndex = 2;
      this.button_SaveProfile.Text = "Save Preset";
      this.button_SaveProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_SaveProfile.UseVisualStyleBackColor = true;
      this.button_SaveProfile.Click += new System.EventHandler(this.button_SaveProfile_Click);
      // 
      // button_CreateProfile
      // 
      this.button_CreateProfile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.button_CreateProfile.Image = ((System.Drawing.Image)(resources.GetObject("button_CreateProfile.Image")));
      this.button_CreateProfile.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_CreateProfile.Location = new System.Drawing.Point(168, 43);
      this.button_CreateProfile.Name = "button_CreateProfile";
      this.button_CreateProfile.Size = new System.Drawing.Size(117, 24);
      this.button_CreateProfile.TabIndex = 3;
      this.button_CreateProfile.Text = "Create Preset";
      this.button_CreateProfile.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_CreateProfile.UseVisualStyleBackColor = true;
      this.button_CreateProfile.Click += new System.EventHandler(this.button_CreateProfile_Click);
      // 
      // comboBox_Profile
      // 
      this.comboBox_Profile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.comboBox_Profile.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      this.comboBox_Profile.FormattingEnabled = true;
      this.comboBox_Profile.Location = new System.Drawing.Point(104, 19);
      this.comboBox_Profile.Name = "comboBox_Profile";
      this.comboBox_Profile.Size = new System.Drawing.Size(417, 21);
      this.comboBox_Profile.TabIndex = 1;
      this.comboBox_Profile.SelectionChangeCommitted += new System.EventHandler(this.comboBox_Profile_SelectionChangeCommitted);
      // 
      // button_ExportSelectedProfiles
      // 
      this.button_ExportSelectedProfiles.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.button_ExportSelectedProfiles.Image = ((System.Drawing.Image)(resources.GetObject("button_ExportSelectedProfiles.Image")));
      this.button_ExportSelectedProfiles.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
      this.button_ExportSelectedProfiles.Location = new System.Drawing.Point(12, 666);
      this.button_ExportSelectedProfiles.Name = "button_ExportSelectedProfiles";
      this.button_ExportSelectedProfiles.Size = new System.Drawing.Size(285, 28);
      this.button_ExportSelectedProfiles.TabIndex = 6;
      this.button_ExportSelectedProfiles.Text = "Export Selected Profiles";
      this.button_ExportSelectedProfiles.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
      this.button_ExportSelectedProfiles.Click += new System.EventHandler(this.button_ExportSelectedProfiles_Click);
      // 
      // captionBar
      // 
      this.captionBar.BackColor = System.Drawing.SystemColors.Window;
      this.captionBar.Caption = "Exporting the Scene";
      this.captionBar.Description = "All settings in this dialog can be saved in export presets for later reuse. You c" +
    "an export the scene either for the active Asset Profile, or for a selection of m" +
    "ultiple Asset Profiles.";
      this.captionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.captionBar.HelpContext = "dialogs";
      this.captionBar.HelpKey = "Export Dialog";
      this.captionBar.HelpManual = null;
      this.captionBar.Image = ((System.Drawing.Image)(resources.GetObject("captionBar.Image")));
      this.captionBar.Location = new System.Drawing.Point(0, 0);
      this.captionBar.Name = "captionBar";
      this.captionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.captionBar.ShowBorder = false;
      this.captionBar.ShowBottomLine = true;
      this.captionBar.Size = new System.Drawing.Size(554, 52);
      this.captionBar.TabIndex = 0;
      // 
      // ExportDialog
      // 
      this.AcceptButton = this.button_ExportSelectedProfiles;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.CancelButton = this.button_CANCEL;
      this.ClientSize = new System.Drawing.Size(554, 705);
      this.Controls.Add(this.groupBox3);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.settingsBox);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this.captionBar);
      this.Controls.Add(this.button_CANCEL);
      this.Controls.Add(this.button_ExportSelectedProfiles);
      this.Controls.Add(this.button_ExportActiveProfile);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.MinimumSize = new System.Drawing.Size(570, 520);
      this.Name = "ExportDialog";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
      this.Text = "Export Scene";
      this.Load += new System.EventHandler(this.ExportDialog_Load);
      this.ResizeEnd += new System.EventHandler(this.ExportDialog_ResizeEnd);
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.settingsBox.ResumeLayout(false);
      this.checkBoxTableLayout.ResumeLayout(false);
      this.checkBoxTableLayout.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.tableLayoutPanel1.ResumeLayout(false);
      this.tableLayoutPanel1.PerformLayout();
      this.groupBox3.ResumeLayout(false);
      this.groupBox3.PerformLayout();
      this.ResumeLayout(false);

    }
		#endregion

    #region Get and set settings

    SceneExportProfile _settings = null;
    SceneExportProfile _settingsUnmodified = null;    

    /// <summary>
    /// Gets or sets the settings from which the export options are taken. The set function clones to local copy.
    /// The get function applies the export flags to each layer
    /// </summary>
    public SceneExportProfile Settings
    {
      get 
      {
        GetSettingsFromControls(); // read back from UI
        return _settings;
      }
      set 
      {
        if (value == null)
        {
          return;
        }

        _bForceReturnNotModified = true;

        // If a profile with the name of the settings to be set already exists, try to load it and use
        // it as the unmodified baseline for our settings.
        SceneExportProfile existingProfile = SceneExportProfile.LoadProfile(EditorManager.Scene, value.ProfileName, true);
        _settingsUnmodified = existingProfile != null ? existingProfile : (SceneExportProfile)value.Clone();

        // If a combo box entry for the settings doesn't yet exist, create one.
        if (FindProfileNameInComboBox(value.ProfileName) < 0)
        {
          comboBox_Profile.Items.Add(value.ProfileName);
        }

        _settings = (SceneExportProfile)value.Clone();

        // set in the controls:
        text_Pathname.Text = MakeExportBaseName(_settings.ExportPath, false);
        dropDown_AssetProfiles.Profiles = (ProfileEnum)_settings.SelectedAssetProfiles.Clone();
        checkBox_RunAfterExport.Checked = _settings.RunAfterExport;
        checkBox_IncludeVisibilityInfo.Checked = _settings.IncludeVisibilityInfo;
        checkBox_InvisibleLayersExportInvisibleObjects.Checked = _settings.InvisibleLayersExportInvisibleObjects;
        checkBox_LoadedZonesOnly.Checked = _settings.LoadedZonesOnly;
        checkBox_updateAssetTransformations.Checked = _settings.UpdateAssetTransformations;

        UIExportFlags = (SceneExportFlags_e)_settings.ExportFlags;

        UpdateListBox();
        UpdateButtonStates();

        _bForceReturnNotModified = false;
      }
    }

    SceneExportFlags_e UIExportFlags
    {
      get
      {
        uint iFlags = 0xffffffff; // all other flags should be set
        foreach (ListViewItem item in listView_Layers.Items)
        {
          if (!(item.Tag is SceneExportFlags_e))
            continue;
          SceneExportFlags_e iValue = (SceneExportFlags_e)item.Tag;
          if (!item.Checked)
            iFlags &= ~((uint)iValue);
        }
        
        return (SceneExportFlags_e)iFlags;
      }
      set
      {
        foreach (ListViewItem item in listView_Layers.Items)
        {
          if (!(item.Tag is SceneExportFlags_e))
            continue;
          SceneExportFlags_e iValue = (SceneExportFlags_e)item.Tag;
          item.Checked = (value & iValue) != 0;
        }
      }
    }

    bool _bForceReturnModified = false;
    bool _bForceReturnNotModified = false;

    bool CurrentSettingModified
    {
      get
      {
        if (_settingsUnmodified == null || _settings == null || !_bDlgShown)
          return false;
        if (_bForceReturnModified)
          return true;
        if (_bForceReturnNotModified)
          return false;
        SceneExportProfile current = Settings; // get from UI;
        return !current.Equals(_settingsUnmodified);
      }
    }

    void GetSettingsFromControls()
    {
      if (!_bDlgShown) // controls are not valid yet
        return;

      _settings.ExportPath = text_Pathname.Text.Trim() + FILE_EXTENSION_VSCENE;
      _settings.SelectedAssetProfiles = (ProfileEnum)dropDown_AssetProfiles.Profiles.Clone();
      _settings.RunAfterExport = checkBox_RunAfterExport.Checked;
      _settings.IncludeVisibilityInfo = checkBox_IncludeVisibilityInfo.Checked;
      _settings.InvisibleLayersExportInvisibleObjects = checkBox_InvisibleLayersExportInvisibleObjects.Checked;
      _settings.LoadedZonesOnly = checkBox_LoadedZonesOnly.Checked;
      _settings.ExportFlags = (uint)UIExportFlags;
      _settings.UpdateAssetTransformations = checkBox_updateAssetTransformations.Checked;

      _settings.ExportedLayers.Clear();
      _settings.ExportedZones.Clear();
      _settings.PluginsUserSelection.Clear();

      foreach (ListViewItem item in listView_Layers.Items)
      {
        // plugins:
        if (item.Tag is string)
          _settings.PluginsUserSelection[(string)item.Tag] = item.Checked;

        if (!item.Checked)
          continue;
        if (item.Tag is Layer)
          _settings.ExportedLayers.Add(item.Tag);
        if (item.Tag is Zone)
          _settings.ExportedZones.Add(item.Tag);
      }
      // fill up layer collection with layers that are not in the UI. This way the profiles are comparable
      foreach (Zone zone in _settings.ExportedZones)
      {
        foreach (Layer layer in zone.Layers)
          if (layer.Export && !_settings.ExportedLayers.Contains(layer))
            _settings.ExportedLayers.Add(layer);
      }
    }

    private bool ValidateSettings()
    {
      GetSettingsFromControls();
      char[] otherInvalidChars = new char[] { '*', '?' };
      if (Path.IsPathRooted(_settings.ExportPath)
        || _settings.ExportPath.Equals(FILE_EXTENSION_VSCENE, StringComparison.InvariantCultureIgnoreCase)
        || _settings.ExportPath.IndexOfAny(Path.GetInvalidPathChars()) >= 0
        || _settings.ExportPath.IndexOfAny(otherInvalidChars) >= 0)
      {
        EditorManager.ShowMessageBox(
          "The export path is not valid.\nIt must be a file name or a relative path, and it must\nonly contain characters that are allowed in file names.",
          "Export path invalid", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        text_Pathname.Focus();
        return false;
      }
      
      return true;
    }

    public bool ExportActiveProfileOnly = false;

    /// <summary>
    /// Confirm this dialog
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void button_ExportActiveProfile_Click(object sender, System.EventArgs e)
    {
      if (!ValidateSettings() || !CheckSaveExportProfile())
        return;

      Settings.ExportedLayersToScene();
      ExportActiveProfileOnly = true;

      DialogResult = DialogResult.OK;
    }

    /// <summary>
    /// Confirm this dialog
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void button_ExportSelectedProfiles_Click(object sender, System.EventArgs e)
    {
      if (!ValidateSettings() || !CheckSaveExportProfile())
        return;

      Settings.ExportedLayersToScene();
      ExportActiveProfileOnly = false;

      DialogResult = DialogResult.OK;
    }

    #endregion

    #region Export filename / path

    static readonly string FILE_EXTENSION_VSCENE = ".vscene";

    private string MakeExportBaseName(string path, bool removeProfile)
    {
      string pathOut = path;
      if (pathOut.EndsWith(FILE_EXTENSION_VSCENE, StringComparison.InvariantCultureIgnoreCase))
        pathOut = pathOut.Remove(pathOut.Length - FILE_EXTENSION_VSCENE.Length);

      if (!removeProfile)
        return pathOut;

      foreach (IProfileManager.Profile profile in EditorManager.ProfileManager.GetProfiles())
      {
        string profileExtension = "." + profile.ToString();
        if (pathOut.EndsWith(profileExtension, StringComparison.InvariantCultureIgnoreCase))
        {
          pathOut = pathOut.Remove(pathOut.Length - profileExtension.Length);
          break;
        }
      }

      return pathOut;
    }

    private void button_Browse_Click(object sender, System.EventArgs e)
    {
      OpenFileDlg fileDlg = new OpenFileDlg();
      fileDlg.ShowBaseData = false;
      string exportDir = "";
      if (_settings.ExportPath!=null)
        exportDir = System.IO.Path.GetDirectoryName(_settings.ExportPath);

      if (System.IO.Directory.Exists(exportDir))
        fileDlg.InitialDirectory = exportDir;
      else
        fileDlg.InitialDirectory = EditorManager.Project.ProjectDir;
      fileDlg.Filter = new string[] { "." + SceneExportProfile.FILE_EXTENSION_EXPORT };
      if (fileDlg.ShowDialog() != DialogResult.OK)
        return;

      string pathBase = MakeExportBaseName(EditorManager.Project.MakeRelative(fileDlg.FileName), true);

      text_Pathname.Text = pathBase;

      OnCheckProfileModified();
    }

    private void checkBox_LoadedZonesOnly_CheckedChanged(object sender, EventArgs e)
    {
      _settings.LoadedZonesOnly = checkBox_LoadedZonesOnly.Checked;
      UpdateButtonStates();
      UpdateListBox();
    }

    #endregion

    #region Layer listview

    private void listView_Layers_ItemChecked(object sender, ItemCheckedEventArgs e)
    {
      if (bListBuildInProgress)
        return;

      this.OnCheckProfileModified();
    }

    bool bListBuildInProgress = false;

    void UpdateListBox()
    {
      System.Diagnostics.Debug.Assert(bListBuildInProgress == false);
      bListBuildInProgress = true;
      listView_Layers.BeginUpdate(); // Prevents Repaint from happening, but not Checked events
      listView_Layers.Items.Clear();
      
      IScene scene = EditorManager.Scene;

      // layers that can be exported
      foreach (Layer layer in scene.Layers)
      {
        if (layer.ParentZone != null)
          continue;

        string itemName = layer.LayerName;
        Color textColor = Color.Black;
        ListViewItem item = new ListViewItem(itemName);
        item.ForeColor = textColor;
        item.ImageIndex = LayerTreeViewNodeBase.GetLayerIcon(layer);
        item.Tag = layer;
        item.Checked = _settings.ExportedLayers.Contains(layer); // layer.Export;
        item.Group = listView_Layers.Groups["Layers"];

        item.SubItems.Add(layer.Description);
        item.ToolTipText = layer.AbsoluteLayerFilename;
        listView_Layers.Items.Add(item);
      }

      ListViewGroup group = listView_Layers.Groups["Zones"];
      // zones that can be exported
      foreach (Zone zone in scene.Zones)
      {
        // [#24564] Not modifiable zones can still be exported, otherwise this would not be consistent with unparented layers
        if (_settings.LoadedZonesOnly && !zone.Loaded /* || !zone.Modifiable*/)
          continue;
        //create item and set properties like Checked BEFORE we add it to the Items list to prevent triggering events and rebuilding the list every time
        ListViewItem item = new ListViewItem(zone.ZoneName);  
        item.ImageIndex = LayerTreeViewNodeBase.GetZoneIcon(zone);
        item.Tag = zone;
        item.Checked = _settings.ExportedZones.Contains(zone); // zone.Export;
        item.Group = group;

        item.SubItems.Add(zone.Description);
        item.ToolTipText = zone.AbsoluteZoneFilename;
        listView_Layers.Items.Add(item);
      }

      // scene components
      // fill flags box
      Type enumType = typeof(SceneExportFlags_e);
      string[] flags = Enum.GetNames(enumType);
      Array values = Enum.GetValues(enumType);
      for (int i = 0; i < flags.Length; i++)
      {        
        ListViewItem item = new ListViewItem(flags[i]);
        item.Tag = values.GetValue(i);
        item.Group = listView_Layers.Groups["SceneComponents"];

        FieldInfo fi = enumType.GetField(flags[i]);
        DescriptionAttribute[] descAttr = (DescriptionAttribute[])fi.GetCustomAttributes(typeof(DescriptionAttribute), true);
        if (descAttr.Length == 1)
          item.ToolTipText = descAttr[0].Description;
          //item.SubItems.Add(descAttr[0].Description);
        listView_Layers.Items.Add(item);
      }
      UIExportFlags = (SceneExportFlags_e)_settings.ExportFlags;

      // custom plugins state
      Dictionary<string, bool> relevantPlugins = new Dictionary<string, bool>();
      ExportHelper.GetPluginsForUserSelection(relevantPlugins);
      foreach (KeyValuePair<string, bool> plugin in relevantPlugins)
      {
        string name = plugin.Key;
        ListViewItem item = new ListViewItem(name);
        bool bState = plugin.Value;
        // sync with the settings:
        _settings.PluginsUserSelection.TryGetValue(name, out bState);

        item.Tag = name;
        item.Group = listView_Layers.Groups["Plugins"];
        item.ToolTipText = "If enabled, this plugin will be loaded by the vPlayer";
        item.Checked = bState;
        listView_Layers.Items.Add(item);
      }

      listView_Layers.EndUpdate();
      bListBuildInProgress = false;
    }

    private void button_AllLayers_Click(object sender, System.EventArgs e)
    {
      listView_Layers.BeginUpdate();
      bListBuildInProgress = true;
      foreach (ListViewItem item in listView_Layers.Items)
        item.Checked = true;
      listView_Layers.EndUpdate();
      bListBuildInProgress = false;
    }

    private void button_InvertLayers_Click(object sender, System.EventArgs e)
    {
      listView_Layers.BeginUpdate();
      bListBuildInProgress = true;
      foreach (ListViewItem item in listView_Layers.Items)
        item.Checked = !item.Checked;
      listView_Layers.EndUpdate();
      bListBuildInProgress = false;
    }

    private void button_ShowAssets_Click(object sender, EventArgs e)
    {
      ShowAssetListDlg dlg = new ShowAssetListDlg();

      ResourceSnapshotInfo info = new ResourceSnapshotInfo();
      ExportHelper.StartResourceSnapshot(info);

      foreach (ListViewItem item in listView_Layers.Items)
      {
        if (!item.Checked)
          continue;
        if (item.Tag is Layer)
          ((Layer)item.Tag).GetDependencies(info);
        else if (item.Tag is Zone)
          ((Zone)item.Tag).GetDependencies(info);
      }
      string desc = "This list shows the native assets used by the current export preset";
      dlg.Description = desc;
      dlg.AssetList = EditorManager.AssetManager.ExtractAssetInfo(info);
      ExportHelper.EndResourceSnapshot(info); // after extracting

      dlg.ShowDialog();
    }

    #endregion

    #region Export profile

    public bool AutoSaveExportProfile
    {
      get { return checkBox_SaveProfileOnExport.Checked; }
      set { checkBox_SaveProfileOnExport.Checked = value; }
    }

    bool PromptProfileModified()
    {
      if (!CurrentSettingModified)
        return true;

      DialogResult res = EditorManager.ShowMessageBox("The current preset '" + _settings.ProfileName + "' has been modified since\nlast saving.\n\n" +
        "Select 'Yes' to save the preset\n" +
        "Select 'No' to discard the changes\n" +
        "Select 'Cancel' to abort preset switching\n"
        , "Save preset changes?", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);

      if (res == DialogResult.Cancel)
        return false;
      if (res == DialogResult.Yes)
      {
        if (!SaveProfile(_settings))
          return false;
        Settings = _settings; // create unmodified copy
        return true;
      }

      return true;
    }


    int FindProfileNameInComboBox(string name)
    {
      for (int i = 0; i < comboBox_Profile.Items.Count; i++)
        if (string.Compare(comboBox_Profile.Items[i].ToString(), name, true) == 0)
          return i;
      return -1;
    }

    void FillProfileCombobox()
    {
      comboBox_Profile.Items.Clear();

      try
      {
        DirectoryInfo layerFolder = new DirectoryInfo(EditorManager.Scene.LayerDirectoryName);
        FileInfo[] files = layerFolder.GetFiles("*." + SceneExportProfile.FILE_EXTENSION_EXPORTPROFILE);
        foreach (FileInfo file in files)
        {
          string name = Path.GetFileNameWithoutExtension(file.Name);
          comboBox_Profile.Items.Add(name);
        }
      }
      catch (Exception ex)
      {
        EditorManager.DumpException(ex);
      }

    }

    bool CheckSaveExportProfile()
    {
      if (!checkBox_SaveProfileOnExport.Checked || !CurrentSettingModified)
        return true;

      return SaveProfile(Settings);
    }

    bool SaveProfile(SceneExportProfile settings)
    {
      if (!settings.SaveToFile())
      {
        EditorManager.ShowMessageBox("Failed to save export preset '" + settings.ProfileName + "'", "Export Preset Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }
      Settings = settings; // clone and update UI
      _settingsUnmodified = (SceneExportProfile)settings.Clone();

      return true;
    }

    bool DeleteProfile(SceneExportProfile settings)
    {
      if (comboBox_Profile.Items.Count <= 1)
      {
        EditorManager.ShowMessageBox("Can't delete the last export preset '" + settings.ProfileName + "'", "Delete Export Preset Failed", MessageBoxButtons.OK, MessageBoxIcon.Information);
        return false;
      }

      if (EditorManager.ShowMessageBox("Do you want to delete the export preset '" + settings.ProfileName + "'?", "Delete Export Preset?", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
      {
        return false;
      }

      if (!settings.DeleteFile())
      {
        EditorManager.ShowMessageBox("Failed to delete preset '" + settings.ProfileName + "'.\nMake sure the file '" + settings.FileName + "' is writable.", "Delete Export Preset Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
        return false;
      }

      // We do not want that popup that the export preset we just deleted is modified.
      _bForceReturnNotModified = true;
      // Rebuild the export preset combobox and select its new first index.
      FillProfileCombobox();
      comboBox_Profile.SelectedIndex = 0;
      // Changing SelectedIndex does not invoke selection changed, so we do so manually.
      comboBox_Profile_SelectionChangeCommitted(null, null);
      _bForceReturnNotModified = false;
      // The Settings property has now been modified and we need to push it back to the global settings as it may still be the just deleted one.
      EditorManager.Scene.CurrentExportProfile = Settings;
      return true;
    }

    private void button_CreateProfile_Click(object sender, EventArgs e)
    {
      if (!PromptProfileModified())
        return;

      CreateFileDlg dlg = new CreateFileDlg();
      dlg.InitialDirectory = EditorManager.Scene.LayerDirectoryName;
      dlg.Caption = "Creating a new Export Preset";
      dlg.Description = "Enter the name of the new preset file.";
      dlg.Ext = "." + SceneExportProfile.FILE_EXTENSION_EXPORTPROFILE;
      dlg.Filter = new string[] { dlg.Ext.ToLower() };
      dlg.AllowOverwrite = false;
      dlg.SupportCustomDirectories = false;
      dlg.AllowFolderCreation = false;

      if (dlg.ShowDialog(this) != DialogResult.OK)
        return;
      SceneExportProfile settings = Settings;
      string oldName = settings.ProfileName;
      settings.ProfileName = Path.GetFileNameWithoutExtension(dlg.FileName);
      if (!SaveProfile(settings))
      {
        settings.ProfileName = oldName;
        return;
      }
      Settings = settings; // create unmodified copy
    }

    private void comboBox_Profile_SelectionChangeCommitted(object sender, EventArgs e)
    {
      if (comboBox_Profile.SelectedItem == null)
        return;
      string newName = comboBox_Profile.SelectedItem.ToString();
      if (_settings != null && string.Compare(_settings.ProfileName, newName, true) == 0)
        return;
      if (!PromptProfileModified())
      {
        // switch back to old profile
        UpdateButtonStates();
        return;
      }
      SceneExportProfile newProfile = SceneExportProfile.LoadProfile(EditorManager.Scene, newName, true);
      if (newProfile == null)
      {
        EditorManager.ShowMessageBox("Failed to load '" + newName + "'", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        UpdateButtonStates();
        return;
      }
      Settings = newProfile; // clone and apply to UI
      _settingsUnmodified = (SceneExportProfile)newProfile.Clone();
    }

    private void button_DeleteProfile_Click(object sender, EventArgs e)
    {
      DeleteProfile(Settings);
    }

    private void button_SaveProfile_Click(object sender, EventArgs e)
    {
      if (!ValidateSettings())
        return;

      SaveProfile(Settings);

      // Transfer settings to scene, so even if we cancel the export, we get the saved settings
      // when opening the export dialog again.
      if (EditorManager.Scene != null)
        EditorManager.Scene.CurrentExportProfile = Settings;
    }

    #endregion

    #region UI callbacks

    bool _bDlgShown = false;

    protected override void OnVisibleChanged(EventArgs e)
    {
      base.OnVisibleChanged(e);
      if (Visible)
      {
        _bDlgShown = true;
        UpdateButtonStates();
      }
    }

    void UpdateButtonStates()
    {
      //textBox_ReplacementPath.Enabled = checkBox_BakeReplacementResources.Checked;
      //button_BrowseReplacementPath.Enabled = checkBox_BakeReplacementResources.Checked;
      button_SaveProfile.Enabled = this.CurrentSettingModified;

      // find selected index
      if (_settings != null)
      {
        comboBox_Profile.SelectedIndex = FindProfileNameInComboBox(_settings.ProfileName);
      }
    }
    
    void OnCheckProfileModified()
    {
      button_SaveProfile.Enabled = this.CurrentSettingModified;
    }

    private void ProfileControlChanged(object sender, EventArgs e)
    {
      this.OnCheckProfileModified();
      UpdateListBox();
    }

    #endregion

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
