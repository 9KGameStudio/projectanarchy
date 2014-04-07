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
using CSharpFramework;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework.View;
using CSharpFramework.Docking;
using WeifenLuo.WinFormsUI;
using Editor.Shapes;
using System.IO;
using CSharpFramework.Dialogs;
using CSharpFramework.Controls;
using CSharpFramework.Helper;
using System.Collections.Generic;

namespace Editor
{
  #region Class ShapeCreatorPanel

  /// <summary>
  /// This panel has a tree view that allows to drag creator objects onto the engine view.
  /// The engine view is responsible for creating an instance in the view. By default, all creator
  /// plugins show up in this panel, but the Vision engine view already allows to drag Shape instances
  /// into it.
  /// </summary>
  public partial class ShapeCreatorPanel : DockableForm
  {
    #region Nested Class ShapeCreatorTreeNode

    /// <summary>
    /// Tree node item for the creator tree
    /// </summary>
    public class ShapeCreatorTreeNode : TreeNode
    {
      /// <summary>
      /// Constructor for a category
      /// </summary>
      /// <param name="nodename"></param>
      /// <param name="iIcon"></param>
      public ShapeCreatorTreeNode(string nodename, int iIcon)
        : base(nodename, iIcon, iIcon)
      {
      }

      /// <summary>
      /// Constructor for creator node. The object can be of any type that is supported by the engine view as a drop target.
      /// </summary>
      /// <param name="nodename"></param>
      /// <param name="creator"></param>
      /// <param name="iIcon"></param>
      public ShapeCreatorTreeNode(string nodename, object creator, int iIcon, string helpKey)
        : base(nodename, iIcon, iIcon)
      {
        CreatorObject = creator;
        _sHelpKey = helpKey;

        if (!string.IsNullOrEmpty(_sHelpKey))
        {
          _helpButton = new HelpToolStripMenuItem(_sHelpKey, "shapes");
          if (iIcon < EditorManager.GUI.ShapeTreeImages.ImageList.Images.Count && iIcon >= 0)
            _helpButton.Image = EditorManager.GUI.ShapeTreeImages.ImageList.Images[iIcon];
          
          ContextMenuStrip = new System.Windows.Forms.ContextMenuStrip();

          // This is a workaround for 'ContextMenuStrip.Items.Add' being incredibly slow so
          // we can't call it in every constructor. Therefore, we listen to the opening of the
          // context menu and only add the item if we actually need to show it.
          ContextMenuStrip.Opening += new CancelEventHandler(ContextMenuStrip_Opening);
        }
      }

      void ContextMenuStrip_Opening(object sender, CancelEventArgs e)
      {
        // Empty context menus are always canceled by default, so we need to reset it here or
        // the menu won't show up on the first try.
        e.Cancel = false;
        if (_helpButton != null)
        {
          ContextMenuStrip.Items.Add(_helpButton);
          ContextMenuStrip.Opening -= new CancelEventHandler(ContextMenuStrip_Opening);
        }
      }

      /// <summary>
      /// The help key used for the context menu of this ShapeCreatorTreeNode.
      /// </summary>
      public string HelpKey
      {
        get
        {
          return _sHelpKey;
        }
      }

      /// <summary>
      /// Indicates whether this node is a category or a creator
      /// </summary>
      public bool IsCategory { get { return CreatorObject == null; } }

      /// <summary>
      /// The creator object (passed in the constructor)
      /// </summary>
      public object CreatorObject = null;

      string _sHelpKey = null;
      HelpToolStripMenuItem _helpButton = null;
    }

    #endregion

    #region Member Variables

    private TreeNode[] nodeState = null;
    private IContainer components;
    private bool _startDragDrop = false;
    object _selectedCreatorObject = null;
    static string NO_DESCRIPTION = "<No description available>";
    ShapeCreatorTreeNode _selectedCreatorNode = null;
    ToolStripHelpButton _helpbutton = null;

    #endregion Member Variables

    /// <summary>
    /// Constructor
    /// </summary>
    public ShapeCreatorPanel(DockingContainer container)
      : base(container)
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      // Add help button
      _helpbutton = new ToolStripHelpButton(Text);
      _helpbutton.Alignment = ToolStripItemAlignment.Right;
      toolStrip.Items.Insert(0, _helpbutton);


      label_Description.Text = null;

      IProject.NewProjectLoaded += new EventHandler(IProject_NewProjectLoaded);
      IProject.ProjectUnloaded += new EventHandler(IProject_ProjectUnloaded);
      PrefabDesc.OnPrefabSaved += new EventHandler(PrefabDesc_OnPrefabSaved);
      PrefabDesc.OnPrefabPropertiesChanged += new EventHandler(PrefabDesc_OnPrefabPropertiesChanged);
      treeView_Creators.ImageList = TreeViewImages.ImageList;

      // Incremental search
      searchPanel.FilterChanged += new EventHandler(searchPanel_FilterChanged);

      // Used for UI updates in case the prefab directory changes (to update view of available prefabs)
      EditorManager.EditorSettingsChanged += new EditorSettingsChangedEventHandler(EditorManager_EditorSettingsChanged);

      // UI updates of ShapeCreatorPanel can be triggered from the EditorManager.GUI as desired (e.g. when ShapeCreators
      // have been added in custom plugins from e.g. some scene callback and they should be available in the list)
      EditorManager.GUI.ShapeCreatorUpdateRequest += new EventHandler(EditorManager_GUI_ShapeCreatorUpdateRequest);
    }


    /// <summary>
    /// Private constructor. Necessary to get this form properly shown in the designer when deriving from it.
    /// See ticket #1067 and http://stackoverflow.com/questions/1216940/
    /// </summary>
    private ShapeCreatorPanel()
      : base(null)
    {
      InitializeComponent();
    }

    #region Build tree functions

    /// <summary>
    /// Clears all nodes in the tree view
    /// </summary>
    public void ClearTree()
    {
      this.treeView_Creators.Nodes.Clear();
      _selectedCreatorNode = null;
      SelectedCreatorObject = null;
    }

    /// <summary>
    /// Wraps around BeginUpdate of the tree view
    /// </summary>
    public void BeginAddCreators()
    {
      treeView_Creators.BeginUpdate();
    }

    /// <summary>
    /// Wraps around EndUpdate of the tree view
    /// </summary>
    public void EndAddCreators()
    {
      treeView_Creators.EndUpdate();
    }


    /// <summary>
    /// Adds a new creator to the tree. The creator object can be any type that is supported in the View OnDrop function
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="creator"></param>
    /// <param name="nodename"></param>
    /// <param name="iIconIndex"></param>
    /// <returns></returns>
    public ShapeCreatorTreeNode AddCreator(ShapeCreatorTreeNode parent, object creator, string nodename, int iIconIndex, string helpKey)
    {
      TreeNodeCollection nodes = (parent == null) ? treeView_Creators.Nodes : parent.Nodes;

      ShapeCreatorTreeNode node = new ShapeCreatorTreeNode(nodename, creator, iIconIndex, helpKey);
      nodes.Add(node);

      return node;
    }

    /// <summary>
    /// Adds a new category to the tree. Tries o find an existing category
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="nodename"></param>
    /// <param name="iIconIndex"></param>
    /// <returns></returns>
    public ShapeCreatorTreeNode AddCategory(ShapeCreatorTreeNode parent, string nodename, int iIconIndex)
    {
      TreeNodeCollection nodes = (parent == null) ? treeView_Creators.Nodes : parent.Nodes;

      if (iIconIndex < 0) // use the folder icon
        iIconIndex = GroupShape.GroupIconIndex;

      // find existing category
      string compare = nodename.ToLower();
      foreach (ShapeCreatorTreeNode child in nodes)
        if (child.Text.ToLower() == compare)
          return child;

      ShapeCreatorTreeNode node = new ShapeCreatorTreeNode(nodename, iIconIndex);
      nodes.Add(node);

      return node;
    }

    /// <summary>
    /// This helper function looks for a specific TreeNode with the given text
    /// in the passed TreeNode or its child nodes.
    /// </summary>
    /// <param name="parent"> Parent TreeNode </param>
    /// <param name="nodename"> Node Text to look for</param>
    /// <returns> The TreeNode if found, otherwise NULL </returns>
    private TreeNode FindCategoryTreeNode(TreeNode parent, string nodetext)
    {
      if (parent != null && parent.Text.Equals(nodetext))
        return parent;

      foreach (TreeNode node in parent.Nodes)
      {
        if (node.Nodes != null && node.Nodes.Count > 0)
        {
          if (node.Text.Equals(nodetext))
            return node;
          else
            FindCategoryTreeNode(node, nodetext);
        }
      }

      return null;
    }

    /// <summary>
    /// Stores the collapsed state of each node in the tree view. Basically a
    /// copy of the tree is kept to compare against when rebuilding the tree.
    /// </summary>
    private void StoreCollapsedState()
    {
      if (this.treeView_Creators.Nodes == null || searchPanel.IsActive)
        return;

      // Save Collapse State of TreeView
      nodeState = new TreeNode[this.treeView_Creators.Nodes.Count];
      this.treeView_Creators.Nodes.CopyTo(nodeState, 0);
    }

    /// <summary>
    /// This helper function restores the collapsed state of the given TreeNodeCollection 
    /// recursively. We must do this after the complete Tree has been build as we can not
    /// expand/collapse parent nodes when they have no children.
    /// </summary>
    /// <param name="nodes"> Collection of TreeNodes </param>
    private void RestoreCollapsedState(TreeNodeCollection nodes)
    {
      if (nodes == null || nodes.Count == 0)
        return;

      foreach (TreeNode n1 in nodes)
      {
        if (n1.Nodes != null && n1.Nodes.Count > 0)
        {
          // Restore children recursively
          RestoreCollapsedState(n1.Nodes);

          // Check this node against stored nodes
          foreach (TreeNode n2 in nodeState)
          {
            TreeNode found = FindCategoryTreeNode(n2, n1.Text);
            if (found != null)
            {
              if (found.IsExpanded)
                n1.Expand();
              else
                n1.Collapse();
              break;
            }
          }
        }
      }
    }

    /// <summary>
    /// This version adds multiple nodes into the tree id the specified path contains back slashes
    /// </summary>
    /// <param name="parent"></param>
    /// <param name="path"></param>
    /// <param name="separator"></param>
    /// <param name="iIconIndex"></param>
    /// <returns></returns>
    public ShapeCreatorTreeNode AddCategoryPath(ShapeCreatorTreeNode parent, string path, string separator, int iIconIndex)
    {
      string[] folders = path.Split(separator.ToCharArray());
      foreach (string folder in folders)
        parent = AddCategory(parent, folder, iIconIndex);
      return parent;
    }

    /// <summary>
    /// Returns the image list that is used for this tree view
    /// </summary>
    public GUI.DynamicImageList TreeViewImages
    {
      get { return EditorManager.GUI.ShapeTreeImages; }
    }

    /// <summary>
    /// Updates all project related creators, this function is called by the project update event    
    /// </summary>    
    protected void ProjectUpdate()
    {
      BeginAddCreators();
      StoreCollapsedState();
      ClearTree();
      EditorProject project = EditorApp.Project;
      if (project == null)
      {
        EndAddCreators();
        return;
      }

      int iCategoryIcon = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\folder_new.png"), Color.Magenta);

      // add all creator plugins in this project
      ShapeCreatorTreeNode catCreators = this.AddCategory(null, "Shape Creators", iCategoryIcon);
      IEditorPluginList list = EditorManager.SortedShapeCreatorPlugins;
      foreach (IShapeCreatorPlugin plugin in list)
      {
        if (!plugin.IsVisible())
          continue;

        // Apply the search filter
        if (!searchPanel.MatchesFilter(plugin.GetPluginName()))
          continue;

        ShapeCreatorTreeNode catPlugin = catCreators;
        string catName = plugin.GetPluginCategory();
        if (catName != null && catName.Length > 0)
          catPlugin = AddCategoryPath(catCreators, catName, "\\", -1);

        string sShapeTypeName = plugin.GetShapeType() == null ? null : plugin.GetShapeType().Name;
        AddCreator(catPlugin, plugin, plugin.GetPluginName(), plugin.IconIndex, sShapeTypeName);
      }

      string prefabDir = project.MakeAbsolute(EditorManager.Settings.PrefabDirectory);
      if (Directory.Exists(prefabDir))
      {

        // Add prefabs from project directory:
        try
        {
          AddPrefabCreators(prefabDir, null);
        }
        catch (Exception ex)
        {
          EditorManager.DumpException(ex);
        }
      }

      // Expand all if no collapsed state was restored before, otherwise restore old one.
      // We can not expand/collapse the TreeNodes at creation time as they have no children
      // assigned when they are created.
      if (nodeState == null || nodeState.Length == 0 || searchPanel.IsActive)
        treeView_Creators.ExpandAll();
      else
        RestoreCollapsedState(treeView_Creators.Nodes);

      EndAddCreators();
    }


    private void IProject_NewProjectLoaded(object sender, EventArgs e)
    {
      this.ProjectUpdate();
      toolStrip.Enabled = true;
      ClearRecentShapesContextMenu();
    }

    private void IProject_ProjectUnloaded(object sender, EventArgs e)
    {
      this.ClearTree();
      toolStrip.Enabled = false;
      ClearRecentShapesContextMenu();
    }

    void EditorManager_EditorSettingsChanged(object sender, EditorSettingsChangedArgs e)
    {
      // Update UI because prefab directory might have changed
      if (e.OldSettings.PrefabDirectory != e.NewSettings.PrefabDirectory)
        ProjectUpdate();
    }

    void EditorManager_GUI_ShapeCreatorUpdateRequest(object sender, EventArgs e)
    {
      ProjectUpdate();
    }

    void PrefabDesc_OnPrefabSaved(object sender, EventArgs e)
    {
      ProjectUpdate();
    }

    void PrefabDesc_OnPrefabPropertiesChanged(object sender, EventArgs e)
    {
      ProjectUpdate();
    }

    #endregion

    #region Drag & Drop

    private void treeView_Creators_ItemDrag(object sender, System.Windows.Forms.ItemDragEventArgs e)
    {
      ShapeCreatorTreeNode node = (ShapeCreatorTreeNode)e.Item;
      if (node.CreatorObject == null)
        return;

      // start dragging the creator object
      DoDragDrop(node.CreatorObject, DragDropEffects.Copy | DragDropEffects.Scroll);

      UpdateRecentShapesList(node.CreatorObject); // create item, even if dragdrop was cancelled.
    }

    #endregion

    #region Tree View callbacks

    private void treeView_Creators_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
    {
      if (_helpbutton.DynamicHelpKeys == null)
        _helpbutton.DynamicHelpKeys = new List<string>();
      else
        _helpbutton.DynamicHelpKeys.Clear();

      _selectedCreatorNode = e.Node as ShapeCreatorTreeNode;
      if (_selectedCreatorNode == null)
        return;

      _helpbutton.DynamicHelpKeys.Add(_selectedCreatorNode.HelpKey);
      SelectedCreatorObject = _selectedCreatorNode.CreatorObject;
    }

    private void treeView_Creators_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
    {
      PrefabDesc prefab = SelectedCreatorObject as PrefabDesc;
      if (prefab != null)
      {
        PrefabManager.EditPrefabDescriptor(prefab);
      }
    }

    private void toolButton_CollapseAll_Click(object sender, EventArgs e)
    {
      treeView_Creators.CollapseAll();
    }

    private void toolButton_ExpandAll_Click(object sender, EventArgs e)
    {
      treeView_Creators.ExpandAll();
    }

    private void treeView_Creators_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
    {
      _selectedCreatorNode = e.Node as ShapeCreatorTreeNode;
      if (_selectedCreatorNode == null)
        return;
      SelectedCreatorObject = _selectedCreatorNode.CreatorObject;
      treeView_Creators.SelectedNode = e.Node;
    }

    #endregion

    #region Selected Creator Object

    /// <summary>
    /// get or set the creator object
    /// </summary>
    public object SelectedCreatorObject
    {
      get { return _selectedCreatorObject; }
      set
      {
        if (_selectedCreatorObject == value)
          return;
        _selectedCreatorObject = value;

        // fire event
        if (SelectedCreatorObjectChanged != null)
          SelectedCreatorObjectChanged(this, EventArgs.Empty);

        // update description text
        string _desc = null;
        IEditorPlugin plugin = _selectedCreatorObject as IEditorPlugin;
        if (plugin != null)
        {
          _desc = plugin.GetPluginDescription();
          if (string.IsNullOrEmpty(_desc))
            _desc = NO_DESCRIPTION;
        }

        PrefabDesc prefab = _selectedCreatorObject as PrefabDesc;
        if (prefab != null)
        {
          _desc = prefab.Description;
          if (string.IsNullOrEmpty(_desc))
            _desc = NO_DESCRIPTION;
        }

        this.label_Description.Text = _desc;
      }
    }

    /// <summary>
    /// This event gets fired when the object in SelectedCreatorObject changed
    /// </summary>
    public event EventHandler SelectedCreatorObjectChanged = null;

    #endregion

    #region Incremental Search

    void searchPanel_FilterChanged(object sender, EventArgs e)
    {
      ProjectUpdate();
      treeView_Creators.ExpandAll();
      toolStripButtonClearSearch.Enabled = !string.IsNullOrEmpty(searchPanel.SearchText);
    }

    private void toolStripButtonClearSearch_Click(object sender, EventArgs e)
    {
      searchPanel.ClearSearch();
    }

    /// <summary>
    /// Implements natural sorting for filenames. This class is used in AddPrefabCreators
    /// </summary>
    private class NaturalFileNameComparer : IComparer
    {
      public int Compare(object objA, object objB)
      {
        string filenameA = Path.GetFileNameWithoutExtension(objA.ToString());
        string filenameB = Path.GetFileNameWithoutExtension(objB.ToString());
        return StringHelper.CompareStringsNaturally(filenameA, filenameB);
      }
    }

    void AddPrefabCreators(string dir, ShapeCreatorTreeNode parent)
    {
      EditorProject project = EditorApp.Project;
      int iIcon = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\lock_ok.png"), Color.Magenta);
      int iCategoryIcon = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\folder_new.png"), Color.Magenta);

      // Create the prefab category, if still missing
      ShapeCreatorTreeNode catParent = null;
      if (parent == null)
        catParent = this.AddCategory(null, "Prefabs", iCategoryIcon);
      else
        catParent = this.AddCategory(parent, dir.Substring(dir.LastIndexOf('\\')), iCategoryIcon);

      // Iterate all subdirectories
      string[] directories = Directory.GetDirectories(dir);
      foreach (string directory in directories)
        AddPrefabCreators(directory, catParent);

      // Iterate all files        
      string[] files = Directory.GetFiles(dir, "*.prefab");
      Array.Sort(files, new NaturalFileNameComparer());

      foreach (string filename in files)
      {
        string relname = project.MakeRelative(filename);
        PrefabDesc desc = PrefabManager.CreatePrefab(relname);
        if (!desc.Loaded)
          continue;

        // Get the name of the prefab
        string _name = desc.Name;
        if (_name == null || _name == "")
          _name = relname;

        // Apply the search filter
        if (!searchPanel.MatchesFilter(_name))
          continue;

        // Add the category path to the tree
        ShapeCreatorTreeNode cat = catParent;
        string catName = desc.Category;
        if (catName != null && catName != "")
          cat = AddCategoryPath(catParent, catName, "\\", -1);

        AddCreator(cat, desc, _name, iIcon, "PrefabShape");
      }

      // Check whether any prefab creators has been added
      if (catParent.Nodes.Count == 0)
        catParent.Remove();
    }

    private void toolStrip_Layout(object sender, LayoutEventArgs e)
    {
      int width = toolStrip.DisplayRectangle.Width;

      foreach (ToolStripItem tsi in toolStrip.Items)
      {
        if (tsi != searchPanel)
        {
          width -= tsi.Width + tsi.Margin.Horizontal;
        }
      }

      searchPanel.Width = Math.Max(100, width - searchPanel.Margin.Horizontal);
    }

    #endregion

    #region Recent Shapes Button

    private void UpdateRecentShapesList(object creatorObj)
    {
      const int maximum = 8;

      //Prefabs cannot be casted to IShapeCreatorPlugin. So we have to check first if the cast is possible to avoid an exception. [#4743]
      IShapeCreatorPlugin shapeCreatorPlugin = creatorObj as IShapeCreatorPlugin;
      if (shapeCreatorPlugin == null)
      {
        return;
      }
      CSharpFramework.EditorManager.PluginToolStripMenuItem shapeItem = new CSharpFramework.EditorManager.PluginToolStripMenuItem(shapeCreatorPlugin.Name, (IEditorPlugin)creatorObj);
      // return if item exists
      foreach (CSharpFramework.EditorManager.PluginToolStripMenuItem item in recentShapesToolButton.DropDownItems)
        if (item.m_plugin.Name == shapeItem.m_plugin.Name)
        {
          // add to top
          recentShapesToolButton.DropDownItems.Insert(0, item);
          return;
        }
      // remove first if maximum is reached
      if (recentShapesToolButton.DropDownItems.Count >= maximum)
      {
        ToolStripItem toRemove = recentShapesToolButton.DropDownItems[maximum - 1];
        toRemove.MouseDown -= shapeItem_MouseDown;
        toRemove.MouseUp -= shapeItem_MouseUp;
        toRemove.MouseLeave -= shapeItem_MouseLeave;
        recentShapesToolButton.DropDownItems.Remove(toRemove);
      }

      // add new
      recentShapesToolButton.DropDownItems.Insert(0, shapeItem);
      shapeItem.MouseDown += new MouseEventHandler(shapeItem_MouseDown);
      shapeItem.MouseLeave += new EventHandler(shapeItem_MouseLeave);
      shapeItem.MouseUp += new MouseEventHandler(shapeItem_MouseUp);
    }

    private void shapeItem_MouseLeave(object sender, EventArgs e)
    {
      if (_startDragDrop)
      {
        _startDragDrop = false;
        DoDragDrop((sender as CSharpFramework.EditorManager.PluginToolStripMenuItem).m_plugin, DragDropEffects.Copy | DragDropEffects.Scroll);
        recentShapesToolButton.HideDropDown();
      }
    }

    private void shapeItem_MouseDown(object sender, MouseEventArgs e)
    {
      _startDragDrop = true;
    }

    private void shapeItem_MouseUp(object sender, MouseEventArgs e)
    {
      _startDragDrop = false;
    }

    private void ClearRecentShapesContextMenu()
    {
      foreach (ToolStripItem item in recentShapesToolButton.DropDownItems)
      {
        item.MouseDown -= shapeItem_MouseDown;
        item.MouseUp -= shapeItem_MouseUp;
        item.MouseLeave -= shapeItem_MouseLeave;
      }
      recentShapesToolButton.DropDownItems.Clear();
    }

    #endregion

    #region Key Handling

    private void treeView_Creators_KeyDown(object sender, KeyEventArgs e)
    {
      EditorManager.EditorComponentKeyDown(null, e);
    }

    #endregion
  }

  #endregion
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
