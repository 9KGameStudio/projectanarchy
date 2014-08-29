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
using System.Text;
using System.Windows.Forms;
using CSharpFramework;
using CSharpFramework.Docking;
using CSharpFramework.View;
using ManagedBase.LogManaged;
using vResourceViewerBase80.Controls;
using CSharpFramework.Controls;

namespace Editor
{
  public partial class ResourceViewerPanel : DockableForm
  {
    #region Member Variables

    // Help button
    ToolStripHelpButton _helpbutton = null;

    #endregion

    public ResourceViewerPanel(DockingContainer container) : base(container)
    {
      InitializeComponent();

      // Add help button
      _helpbutton = new ToolStripHelpButton(Text);
      resourceMasterPanel.ToolStrip.Items.Add(_helpbutton);

      // show auto thumbnail button to enable auto thumbnail feature
      resourceMasterPanel.ShowAutoThumbnailButton();

      EditorManager.SceneClosing += new CancelEventHandler(EditorManager_SceneClosing);
    }

    public void ShowLogPane()
    {
      resourceMasterPanel.ShowLogPane();
    }

    public void AddOnClearLogMessagesHandler(vResourceViewerBase80.Controls.MasterPanel.ClearLogMessagesDelegate MyDelegate)
    {
      resourceMasterPanel.OnClearLogMessages += MyDelegate;
    }

    public void RemoveOnClearLogMessagesHandler(vResourceViewerBase80.Controls.MasterPanel.ClearLogMessagesDelegate MyDelegate)
    {
      resourceMasterPanel.OnClearLogMessages -= MyDelegate;
    }

    /// <summary>
    /// Clears the asset list in the vResViewer every time a scene is closed
    /// </summary>
    void EditorManager_SceneClosing(object sender, CancelEventArgs e)
    {
      resourceMasterPanel.assetTabPage1.Clear();
    }

    /// <summary>
    /// Returns the control that is relevant for log messages etc.
    /// </summary>
    public MasterPanel ResourceViewWindow
    {
      get
      {
        return resourceMasterPanel;
      }
    }

    protected override void WndProc(ref Message m)
    {
      base.WndProc(ref m);
      resourceMasterPanel.ProcessWndProc(ref m);
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
