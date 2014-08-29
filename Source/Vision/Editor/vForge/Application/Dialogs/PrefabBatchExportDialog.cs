/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using CSharpFramework;
using CSharpFramework.Scene;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Editor.Dialogs
{
  public partial class PrefabBatchExportDialog : Form
  {
    class PrefabEntry
    {
      public string RelFilename = null;
      public FileInfo PrefabFile = null;
      public string VPrefabName = null;
      public FileInfo VPrefabFile = null;
      public ListViewItem ListItem = null;
      public bool Success = false;
    }

    public PrefabBatchExportDialog()
    {
      InitializeComponent();
    }


    public new DialogResult ShowDialog(IWin32Window owner)
    {
      if (_entries == null || _entries.Count == 0)
        return EditorManager.ShowMessageBox(owner, "No prefab files were found in the specified folder", "Batch Export Prefabs", MessageBoxButtons.OK, MessageBoxIcon.Information);
      return base.ShowDialog(owner);
    }


    List<PrefabEntry> Selection
    {
      get
      {
        List<PrefabEntry> _selection = new List<PrefabEntry>();
        foreach (ListViewItem item in listView_Prefabs.Items)
        {
          if (item.Checked)
            _selection.Add(item.Tag as PrefabEntry);
        }
        return _selection;
      }
    }


    public string Folder
    {
      set
      {
        BuildList(value);
      }
    }

    List<PrefabEntry> _entries;


    void BuildList(string folder)
    {
      _entries = null;
      DirectoryInfo dir = new DirectoryInfo(folder);
      FileInfo[] files = dir.GetFiles("*.prefab", SearchOption.AllDirectories);

      listView_Prefabs.BeginUpdate();
      listView_Prefabs.Items.Clear();
      if (files == null || files.Length == 0)
      {
        listView_Prefabs.EndUpdate();
        return;
      }

      _entries = new List<PrefabEntry>();
      int iSub = folder.Length;
      foreach (FileInfo f in files)
      {
        PrefabEntry entry = new PrefabEntry();
        _entries.Add(entry);
        entry.PrefabFile = f;

        entry.RelFilename = f.FullName.Substring(iSub);
        ListViewItem item = listView_Prefabs.Items.Add(entry.RelFilename);
        entry.ListItem = item;

        string status = "<not available>";
        Color statusCol = Color.Gray;

        entry.VPrefabName = Path.ChangeExtension(f.FullName, ".vprefab");
        FileInfo vprefab = new FileInfo(entry.VPrefabName);
        if (vprefab.Exists)
        {
          entry.VPrefabFile = vprefab;
          status = "Last Export: " + vprefab.LastWriteTime.ToString();
          statusCol = Color.Black;
        }
        item.UseItemStyleForSubItems = false;
        ListViewItem.ListViewSubItem subitem = item.SubItems.Add(status);
        subitem.ForeColor = statusCol;

        item.Tag = entry;
        item.Checked = true;
      }

      listView_Prefabs.EndUpdate();
    }



    private void button_Export_Click(object sender, EventArgs e)
    {
      List<PrefabEntry> sel = Selection;
      if (sel.Count == 0)
        return;

      EditorManager.Progress.ShowProgressDialog("Export Prefabs", true);
      float fStep = 100.0f / (float)sel.Count;

      EditorManager.GUI.UIUpdateLock++;

      foreach (PrefabEntry entry in sel)
      {
        ListViewItem.ListViewSubItem subitem = entry.ListItem.SubItems[1];
        ListViewItem.ListViewSubItem coloritem = subitem;
        subitem.Text = "Exporting...";
        EditorManager.Progress.StatusString = "Exporting " + entry.RelFilename;

        coloritem.ForeColor = Color.Black;
        PrefabDesc desc = PrefabManager.CreatePrefab(entry.PrefabFile.FullName);
        if (EditorManager.Progress.WantsAbort)
          break;
        if (desc == null)
        {
          subitem.Text = "Loading failed";
          coloritem.ForeColor = Color.Red;
          continue;
        }
        if (!desc.Loaded)
        {
          subitem.Text = "Loading failed : " + desc.LastError;
          coloritem.ForeColor = Color.Red;
          continue;
        }
        if (entry.VPrefabFile != null && entry.VPrefabFile.IsReadOnly)
        {
          subitem.Text = "Target file is read-only" + desc.LastError;
          coloritem.ForeColor = Color.OrangeRed;
          continue;
        }
        if (!PrefabManager.BINARY_SAVER.SaveToBinaryFile(desc))
        {
          subitem.Text = "Export failed";
          coloritem.ForeColor = Color.Red;
          continue;
        }
        subitem.Text = "Export successful";
        coloritem.ForeColor = Color.Green;
        entry.Success = true;
        entry.VPrefabFile = new FileInfo(entry.VPrefabName);
        listView_Prefabs.Refresh();
        EditorManager.Progress.Percentage += fStep;
        if (EditorManager.Progress.WantsAbort)
          break;
      }
      EditorManager.Progress.HideProgressDialog();
      EditorManager.GUI.UIUpdateLock--;

    }

    private void selectUnselectAllToolStripMenuItem_Click(object sender, EventArgs e)
    {
      List<PrefabEntry> sel = Selection;
      bool bStatus = true; // all on
      if (sel.Count == listView_Prefabs.Items.Count)
        bStatus = false;
      foreach (ListViewItem item in listView_Prefabs.Items)
        item.Checked = bStatus;
    }

    private void invertSelectionToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (ListViewItem item in listView_Prefabs.Items)
        item.Checked = !item.Checked;
    }

    private void unselectToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (ListViewItem item in listView_Prefabs.Items)
        if (((PrefabEntry)item.Tag).Success)
          item.Checked = false;
    }

    private void selectNonexistingExportsToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (ListViewItem item in listView_Prefabs.Items)
        if (((PrefabEntry)item.Tag).VPrefabFile == null)
          item.Checked = true;
        else
          item.Checked = false;
    }

    private void selectExportsOlderThanTodayToolStripMenuItem_Click(object sender, EventArgs e)
    {
      foreach (ListViewItem item in listView_Prefabs.Items)
      {
        FileInfo vprefab = new FileInfo(((PrefabEntry)item.Tag).VPrefabName);
        if (!vprefab.Exists || vprefab.LastWriteTime < DateTime.Today)
          item.Checked = true;
        else
          item.Checked = false;
      }

    }


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
