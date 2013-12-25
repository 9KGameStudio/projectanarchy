namespace Editor.Dialogs
{
  partial class WorkspaceDlg
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WorkspaceDlg));
      this.captionBar = new CSharpFramework.Controls.DialogCaptionBar();
      this.laProjectDirDetail = new System.Windows.Forms.Label();
      this.btnProjectDir = new System.Windows.Forms.Button();
      this.laSelectDir = new System.Windows.Forms.Label();
      this.laSelectDirCaption = new System.Windows.Forms.Label();
      this.laSelectedDir = new System.Windows.Forms.Label();
      this.btnSelectFolder = new System.Windows.Forms.Button();
      this.btnCancel = new System.Windows.Forms.Button();
      this.btnOk = new System.Windows.Forms.Button();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.laProjectDirCaption = new System.Windows.Forms.Label();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.tableLayoutPanel1.SuspendLayout();
      this.flowLayoutPanel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // captionBar
      // 
      this.captionBar.BackColor = System.Drawing.SystemColors.Window;
      this.captionBar.Caption = "Workspace Selection";
      this.captionBar.Description = "Every project must reside in a workspace. This can either be the project director" +
    "y itself, or any parent directory of the project.";
      this.captionBar.Dock = System.Windows.Forms.DockStyle.Top;
      this.captionBar.HelpContext = "dialogs";
      this.captionBar.HelpKey = null;
      this.captionBar.HelpManual = null;
      this.captionBar.Image = global::Editor.Properties.Resources.asset_library_32x32;
      this.captionBar.Location = new System.Drawing.Point(0, 0);
      this.captionBar.Name = "captionBar";
      this.captionBar.SetFontColor = System.Drawing.SystemColors.ControlText;
      this.captionBar.ShowBorder = false;
      this.captionBar.ShowBottomLine = true;
      this.captionBar.Size = new System.Drawing.Size(516, 52);
      this.captionBar.TabIndex = 0;
      // 
      // laProjectDirDetail
      // 
      this.laProjectDirDetail.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.tableLayoutPanel1.SetColumnSpan(this.laProjectDirDetail, 2);
      this.laProjectDirDetail.Location = new System.Drawing.Point(27, 19);
      this.laProjectDirDetail.Name = "laProjectDirDetail";
      this.laProjectDirDetail.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
      this.laProjectDirDetail.Size = new System.Drawing.Size(482, 30);
      this.laProjectDirDetail.TabIndex = 2;
      this.laProjectDirDetail.Text = "Select this option if your project is a standalone project. You will be able to u" +
    "se resources contained in the project directory and in the Havok Vision SDK dire" +
    "ctory.";
      // 
      // btnProjectDir
      // 
      this.btnProjectDir.Location = new System.Drawing.Point(27, 52);
      this.btnProjectDir.Name = "btnProjectDir";
      this.btnProjectDir.Size = new System.Drawing.Size(137, 23);
      this.btnProjectDir.TabIndex = 3;
      this.btnProjectDir.Text = "&Use Project Directory";
      this.btnProjectDir.UseVisualStyleBackColor = true;
      this.btnProjectDir.Click += new System.EventHandler(this.btnProjectDir_Click);
      // 
      // laSelectDir
      // 
      this.laSelectDir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.tableLayoutPanel1.SetColumnSpan(this.laSelectDir, 2);
      this.laSelectDir.Location = new System.Drawing.Point(27, 99);
      this.laSelectDir.Name = "laSelectDir";
      this.laSelectDir.Size = new System.Drawing.Size(482, 42);
      this.laSelectDir.TabIndex = 5;
      this.laSelectDir.Text = resources.GetString("laSelectDir.Text");
      // 
      // laSelectDirCaption
      // 
      this.laSelectDirCaption.AutoSize = true;
      this.tableLayoutPanel1.SetColumnSpan(this.laSelectDirCaption, 3);
      this.laSelectDirCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.laSelectDirCaption.Location = new System.Drawing.Point(7, 78);
      this.laSelectDirCaption.Name = "laSelectDirCaption";
      this.laSelectDirCaption.Padding = new System.Windows.Forms.Padding(0, 8, 0, 0);
      this.laSelectDirCaption.Size = new System.Drawing.Size(166, 21);
      this.laSelectDirCaption.TabIndex = 4;
      this.laSelectDirCaption.Text = "Select Workspace Directory";
      // 
      // laSelectedDir
      // 
      this.laSelectedDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
      this.laSelectedDir.AutoEllipsis = true;
      this.laSelectedDir.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.laSelectedDir.Location = new System.Drawing.Point(27, 145);
      this.laSelectedDir.Name = "laSelectedDir";
      this.laSelectedDir.Size = new System.Drawing.Size(445, 23);
      this.laSelectedDir.TabIndex = 6;
      this.laSelectedDir.Text = "< select a directory >";
      this.laSelectedDir.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      this.laSelectedDir.Click += new System.EventHandler(this.btnSelectFolder_Click);
      // 
      // btnSelectFolder
      // 
      this.btnSelectFolder.Anchor = System.Windows.Forms.AnchorStyles.None;
      this.btnSelectFolder.Image = global::Editor.Properties.Resources.toolbar_folder;
      this.btnSelectFolder.Location = new System.Drawing.Point(478, 144);
      this.btnSelectFolder.Name = "btnSelectFolder";
      this.btnSelectFolder.Size = new System.Drawing.Size(31, 25);
      this.btnSelectFolder.TabIndex = 7;
      this.btnSelectFolder.UseVisualStyleBackColor = true;
      this.btnSelectFolder.Click += new System.EventHandler(this.btnSelectFolder_Click);
      // 
      // btnCancel
      // 
      this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.btnCancel.Location = new System.Drawing.Point(424, 7);
      this.btnCancel.Name = "btnCancel";
      this.btnCancel.Size = new System.Drawing.Size(75, 25);
      this.btnCancel.TabIndex = 9;
      this.btnCancel.Text = "&Cancel";
      this.btnCancel.UseVisualStyleBackColor = true;
      // 
      // btnOk
      // 
      this.btnOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.btnOk.Enabled = false;
      this.btnOk.Location = new System.Drawing.Point(343, 7);
      this.btnOk.Name = "btnOk";
      this.btnOk.Size = new System.Drawing.Size(75, 25);
      this.btnOk.TabIndex = 8;
      this.btnOk.Text = "&OK";
      this.btnOk.UseVisualStyleBackColor = true;
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.AutoSize = true;
      this.tableLayoutPanel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.tableLayoutPanel1.ColumnCount = 3;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.Controls.Add(this.laProjectDirCaption, 0, 0);
      this.tableLayoutPanel1.Controls.Add(this.laProjectDirDetail, 1, 1);
      this.tableLayoutPanel1.Controls.Add(this.btnProjectDir, 1, 2);
      this.tableLayoutPanel1.Controls.Add(this.btnSelectFolder, 2, 5);
      this.tableLayoutPanel1.Controls.Add(this.laSelectDirCaption, 0, 3);
      this.tableLayoutPanel1.Controls.Add(this.laSelectedDir, 1, 5);
      this.tableLayoutPanel1.Controls.Add(this.laSelectDir, 1, 4);
      this.tableLayoutPanel1.Controls.Add(this.flowLayoutPanel1, 0, 6);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 52);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.Padding = new System.Windows.Forms.Padding(4);
      this.tableLayoutPanel1.RowCount = 7;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.Size = new System.Drawing.Size(516, 268);
      this.tableLayoutPanel1.TabIndex = 10;
      // 
      // laProjectDirCaption
      // 
      this.laProjectDirCaption.AutoSize = true;
      this.tableLayoutPanel1.SetColumnSpan(this.laProjectDirCaption, 3);
      this.laProjectDirCaption.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.laProjectDirCaption.Location = new System.Drawing.Point(7, 4);
      this.laProjectDirCaption.Name = "laProjectDirCaption";
      this.laProjectDirCaption.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
      this.laProjectDirCaption.Size = new System.Drawing.Size(128, 15);
      this.laProjectDirCaption.TabIndex = 2;
      this.laProjectDirCaption.Text = "Use Project Directory";
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.AutoSize = true;
      this.tableLayoutPanel1.SetColumnSpan(this.flowLayoutPanel1, 3);
      this.flowLayoutPanel1.Controls.Add(this.btnCancel);
      this.flowLayoutPanel1.Controls.Add(this.btnOk);
      this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowLayoutPanel1.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
      this.flowLayoutPanel1.Location = new System.Drawing.Point(7, 175);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Padding = new System.Windows.Forms.Padding(0, 4, 0, 0);
      this.flowLayoutPanel1.Size = new System.Drawing.Size(502, 86);
      this.flowLayoutPanel1.TabIndex = 8;
      // 
      // WorkspaceDlg
      // 
      this.AcceptButton = this.btnOk;
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
      this.AutoSize = true;
      this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.CancelButton = this.btnCancel;
      this.ClientSize = new System.Drawing.Size(516, 320);
      this.ControlBox = false;
      this.Controls.Add(this.tableLayoutPanel1);
      this.Controls.Add(this.captionBar);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "WorkspaceDlg";
      this.ShowInTaskbar = false;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Workspace Selection";
      this.tableLayoutPanel1.ResumeLayout(false);
      this.tableLayoutPanel1.PerformLayout();
      this.flowLayoutPanel1.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private CSharpFramework.Controls.DialogCaptionBar captionBar;
    private System.Windows.Forms.Label laProjectDirDetail;
    private System.Windows.Forms.Button btnProjectDir;
    private System.Windows.Forms.Label laSelectDir;
    private System.Windows.Forms.Label laSelectDirCaption;
    private System.Windows.Forms.Label laSelectedDir;
    private System.Windows.Forms.Button btnSelectFolder;
    private System.Windows.Forms.Button btnCancel;
    private System.Windows.Forms.Button btnOk;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.Label laProjectDirCaption;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
  }
}