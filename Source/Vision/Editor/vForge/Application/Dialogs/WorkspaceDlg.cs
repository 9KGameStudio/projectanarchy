/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Editor.Dialogs
{
  public partial class WorkspaceDlg : Form
  {
    public WorkspaceDlg(String projectDir)
    {
      InitializeComponent();
      _projectDir = new DirectoryInfo(projectDir);
    }

    private void btnProjectDir_Click(object sender, EventArgs e)
    {
      _useProjectDir = true;
      DialogResult = DialogResult.OK;
    }

    private void btnSelectFolder_Click(object sender, EventArgs e)
    {
      FolderBrowserDialog fbd = new FolderBrowserDialog();
      fbd.RootFolder = Environment.SpecialFolder.MyComputer;
      fbd.ShowNewFolderButton = false;
      fbd.SelectedPath = (_selectedDir != null) ? _selectedDir.FullName : _projectDir.FullName;
      if (fbd.ShowDialog(this) == DialogResult.OK)
      {
        DirectoryInfo selectedDir = new DirectoryInfo(fbd.SelectedPath);
        if (!_projectDir.FullName.ToLower().StartsWith(selectedDir.FullName.ToLower()))
        {
          MessageBox.Show("The selected directory is not a parent directory of the project directory. Please select a directory above the project directory as the new workspace directory.", "Directory not usable", MessageBoxButtons.OK, MessageBoxIcon.Warning);
          return;
        }

        _selectedDir = selectedDir;
        laSelectedDir.Text = _selectedDir.FullName;
        btnOk.Enabled = true;
      }
    }

    public DirectoryInfo SelectedDir
    {
      get { return _selectedDir; }
    }

    public bool UseProjectDir
    {
      get { return _useProjectDir; }
    }

    private DirectoryInfo _projectDir;
    private DirectoryInfo _selectedDir;
    private bool _useProjectDir;
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
