namespace Editor.Dialogs
{
  partial class PrefabBatchExportDialog
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PrefabBatchExportDialog));
      this.panel1 = new System.Windows.Forms.Panel();
      this.button_Export = new System.Windows.Forms.Button();
      this.button_Close = new System.Windows.Forms.Button();
      this.dialogCaptionBar1 = new CSharpFramework.Controls.DialogCaptionBar();
      this.listView_Prefabs = new System.Windows.Forms.ListView();
      this.columnHeader_File = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.columnHeader_Export = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
      this.menuStrip1 = new System.Windows.Forms.MenuStrip();
      this.selectionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectUnselectAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.invertSelectionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.unselectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectNonexistingExportsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.selectExportsOlderThanTodayToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.panel1.SuspendLayout();
      this.menuStrip1.SuspendLayout();
      this.SuspendLayout();
      // 
      // panel1
      // 
      this.panel1.Controls.Add(this.button_Export);
      this.panel1.Controls.Add(this.button_Close);
      this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.panel1.Location = new System.Drawing.Point(0, 392);
      this.panel1.Name = "panel1";
      this.panel1.Size = new System.Drawing.Size(548, 63);
      this.panel1.TabIndex = 0;
      // 
      // button_Export
      // 
      this.button_Export.Location = new System.Drawing.Point(181, 20);
      this.button_Export.Name = "button_Export";
      this.button_Export.Size = new System.Drawing.Size(78, 31);
      this.button_Export.TabIndex = 1;
      this.button_Export.Text = "Export";
      this.button_Export.UseVisualStyleBackColor = true;
      this.button_Export.Click += new System.EventHandler(this.button_Export_Click);
      // 
      // button_Close
      // 
      this.button_Close.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.button_Close.Location = new System.Drawing.Point(279, 20);
      this.button_Close.Name = "button_Close";
      this.button_Close.Size = new System.Drawing.Size(78, 31);
      this.button_Close.TabIndex = 0;
      this.button_Close.Text = "Close";
      this.button_Close.UseVisualStyleBackColor = true;
      // 
      // dialogCaptionBar1
      // 
      this.dialogCaptionBar1.BackColor = System.Drawing.SystemColors.Window;
      this.dialogCaptionBar1.Caption = "Batch export Prefabs";
      this.dialogCaptionBar1.Description = "Select prefabs in this project that should be re-exported to their runtime vprefa" +
    "b counterpart";
      this.dialogCaptionBar1.Dock = System.Windows.Forms.DockStyle.Top;
      this.dialogCaptionBar1.HelpContext = "dialogs";
      this.dialogCaptionBar1.HelpKey = "";
      this.dialogCaptionBar1.HelpManual = null;
      this.dialogCaptionBar1.Image = ((System.Drawing.Image)(resources.GetObject("dialogCaptionBar1.Image")));
      this.dialogCaptionBar1.Location = new System.Drawing.Point(0, 24);
      this.dialogCaptionBar1.Name = "dialogCaptionBar1";
      this.dialogCaptionBar1.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.dialogCaptionBar1.ShowBorder = false;
      this.dialogCaptionBar1.ShowBottomLine = true;
      this.dialogCaptionBar1.Size = new System.Drawing.Size(548, 52);
      this.dialogCaptionBar1.TabIndex = 2;
      // 
      // listView_Prefabs
      // 
      this.listView_Prefabs.CheckBoxes = true;
      this.listView_Prefabs.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader_File,
            this.columnHeader_Export});
      this.listView_Prefabs.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView_Prefabs.Location = new System.Drawing.Point(0, 76);
      this.listView_Prefabs.Name = "listView_Prefabs";
      this.listView_Prefabs.Size = new System.Drawing.Size(548, 316);
      this.listView_Prefabs.TabIndex = 3;
      this.listView_Prefabs.UseCompatibleStateImageBehavior = false;
      this.listView_Prefabs.View = System.Windows.Forms.View.Details;
      // 
      // columnHeader_File
      // 
      this.columnHeader_File.Text = "Prefab Filename";
      this.columnHeader_File.Width = 360;
      // 
      // columnHeader_Export
      // 
      this.columnHeader_Export.Text = "Export Status";
      this.columnHeader_Export.Width = 179;
      // 
      // menuStrip1
      // 
      this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectionToolStripMenuItem});
      this.menuStrip1.Location = new System.Drawing.Point(0, 0);
      this.menuStrip1.Name = "menuStrip1";
      this.menuStrip1.Size = new System.Drawing.Size(548, 24);
      this.menuStrip1.TabIndex = 4;
      this.menuStrip1.Text = "menuStrip1";
      // 
      // selectionToolStripMenuItem
      // 
      this.selectionToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectUnselectAllToolStripMenuItem,
            this.invertSelectionToolStripMenuItem,
            this.unselectToolStripMenuItem,
            this.selectNonexistingExportsToolStripMenuItem,
            this.selectExportsOlderThanTodayToolStripMenuItem});
      this.selectionToolStripMenuItem.Name = "selectionToolStripMenuItem";
      this.selectionToolStripMenuItem.Size = new System.Drawing.Size(67, 20);
      this.selectionToolStripMenuItem.Text = "Selection";
      // 
      // selectUnselectAllToolStripMenuItem
      // 
      this.selectUnselectAllToolStripMenuItem.Name = "selectUnselectAllToolStripMenuItem";
      this.selectUnselectAllToolStripMenuItem.Size = new System.Drawing.Size(218, 22);
      this.selectUnselectAllToolStripMenuItem.Text = "Select/Unselect all";
      this.selectUnselectAllToolStripMenuItem.Click += new System.EventHandler(this.selectUnselectAllToolStripMenuItem_Click);
      // 
      // invertSelectionToolStripMenuItem
      // 
      this.invertSelectionToolStripMenuItem.Name = "invertSelectionToolStripMenuItem";
      this.invertSelectionToolStripMenuItem.Size = new System.Drawing.Size(218, 22);
      this.invertSelectionToolStripMenuItem.Text = "Invert Selection";
      this.invertSelectionToolStripMenuItem.Click += new System.EventHandler(this.invertSelectionToolStripMenuItem_Click);
      // 
      // unselectToolStripMenuItem
      // 
      this.unselectToolStripMenuItem.Name = "unselectToolStripMenuItem";
      this.unselectToolStripMenuItem.Size = new System.Drawing.Size(217, 22);
      this.unselectToolStripMenuItem.Text = "Unselect successful Exports";
      this.unselectToolStripMenuItem.Click += new System.EventHandler(this.unselectToolStripMenuItem_Click);
      // 
      // selectNonexistingExportsToolStripMenuItem
      // 
      this.selectNonexistingExportsToolStripMenuItem.Name = "selectNonexistingExportsToolStripMenuItem";
      this.selectNonexistingExportsToolStripMenuItem.Size = new System.Drawing.Size(217, 22);
      this.selectNonexistingExportsToolStripMenuItem.Text = "Select non-existing Exports";
      this.selectNonexistingExportsToolStripMenuItem.Click += new System.EventHandler(this.selectNonexistingExportsToolStripMenuItem_Click);
      // 
      // selectExportsOlderThanTodayToolStripMenuItem
      // 
      this.selectExportsOlderThanTodayToolStripMenuItem.Name = "selectExportsOlderThanTodayToolStripMenuItem";
      this.selectExportsOlderThanTodayToolStripMenuItem.Size = new System.Drawing.Size(236, 22);
      this.selectExportsOlderThanTodayToolStripMenuItem.Text = "Select Exports older than today";
      this.selectExportsOlderThanTodayToolStripMenuItem.Click += new System.EventHandler(this.selectExportsOlderThanTodayToolStripMenuItem_Click);
      // 
      // PrefabBatchExportDialog
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.button_Close;
      this.ClientSize = new System.Drawing.Size(548, 455);
      this.Controls.Add(this.listView_Prefabs);
      this.Controls.Add(this.dialogCaptionBar1);
      this.Controls.Add(this.panel1);
      this.Controls.Add(this.menuStrip1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
      this.MainMenuStrip = this.menuStrip1;
      this.Name = "PrefabBatchExportDialog";
      this.Text = "Batch Export Prefabs";
      this.panel1.ResumeLayout(false);
      this.menuStrip1.ResumeLayout(false);
      this.menuStrip1.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Button button_Export;
    private System.Windows.Forms.Button button_Close;
    private CSharpFramework.Controls.DialogCaptionBar dialogCaptionBar1;
    private System.Windows.Forms.ListView listView_Prefabs;
    private System.Windows.Forms.ColumnHeader columnHeader_File;
    private System.Windows.Forms.ColumnHeader columnHeader_Export;
    private System.Windows.Forms.MenuStrip menuStrip1;
    private System.Windows.Forms.ToolStripMenuItem selectionToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem selectUnselectAllToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem invertSelectionToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem unselectToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem selectNonexistingExportsToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem selectExportsOlderThanTodayToolStripMenuItem;
  }
}