/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CSharpFramework.Shapes;
using CSharpFramework;
using CSharpFrameworkTests;
using System.IO;
using System.Drawing;
using System.Runtime.Serialization;
using HavokManaged;
using System.ComponentModel;
using CSharpFramework.PropertyEditors;
using System.Drawing.Design;
using CSharpFramework.Math;

namespace HavokEditorPlugin.Shapes
{
  #region class HavokResourceShape

  [Serializable, ShapeTypeNiceName("Havok Resource")]
  public class HavokResourceShape : ShapeBase
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_RESOURCE = "Havok Resource";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_RESOURCE = Shape3D.LAST_CATEGORY_ORDER_ID + 1; 

    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_RESOURCE;

    #endregion

    #region Constructor

    public HavokResourceShape(string name)
      : base(name)
    {
    }

    #endregion

    #region ShapeBase Members

    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceHavokResourceShape();

      SetEngineInstanceBaseProperties(); // sets the position etc.

      base.CreateEngineInstance(bCreateChildren);
    }


    // Accesses the engine instance as an object of type EngineInstanceHavokConstraint
    EngineInstanceHavokResourceShape EngineResource
    {
      get { return (EngineInstanceHavokResourceShape)_engineInstance; }
    }

    /// <summary>
    /// overridden function to set/update all specific properties on the engine instance
    /// </summary>
    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties();
      if (_engineInstance == null)
        return;
      EngineResource.SetAllowAsyncLoading(_allowAsyncLoading);
      EngineResource.LoadFromFile(_filename, _boundingBox);
    }


    BoundingBox _boundingBox = new BoundingBox();

    public override BoundingBox AbsoluteBoundingBox
    {
      get
      {
        if (_boundingBox.Valid)
          return _boundingBox;
        return base.AbsoluteBoundingBox;
      }
    }


    public override bool GetLocalBoundingBox(ref BoundingBox bbox)
    {
      return false;
    }


    #endregion

    #region Properties

    string _filename;
    bool _allowAsyncLoading = true;

    [Description("Filename of the Havok resource")]
    [SortedCategory(CAT_RESOURCE, CATORDER_RESOURCE), PropertyOrder(1)]
    [EditorAttribute(typeof(FilenameEditor), typeof(UITypeEditor)), FileDialogFilter(new string[] { ".hkt" })]
    public string Filename
    {
      get { return _filename; }
      set 
      {
        if (string.Compare(_filename, value, true) == 0)
          return;
        _filename = value;
        if (HasEngineInstance())
          EngineResource.LoadFromFile(_filename, _boundingBox);
      }
    }

    [Description("If enabled, the resource may be loaded asynchronously by the runtime plugin")]
    [SortedCategory(CAT_RESOURCE, CATORDER_RESOURCE), PropertyOrder(2)]
    public bool AllowAsynchronousLoading
    {
      get { return _allowAsyncLoading; }
      set 
      { 
        _allowAsyncLoading = value;
        if (HasEngineInstance())
          EngineResource.SetAllowAsyncLoading(_allowAsyncLoading);
      }
    }

    #endregion

    #region GUI

    public static int ICON_INDEX = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\folder_cubes.bmp"), Color.Magenta);

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return ICON_INDEX;
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected HavokResourceShape(SerializationInfo info, StreamingContext context)
      : base(info, context)
    {
      _filename = info.GetString("_filename");
      _allowAsyncLoading = info.GetBoolean("_allowAsyncLoading");
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context)
    {
      base.GetObjectData(info, context);

      info.AddValue("_filename", _filename);
      info.AddValue("_allowAsyncLoading", _allowAsyncLoading);
    }

    #endregion

    #region Logic Graph

    public string LogicGraph_GetSupportedActions()
    {
      return "Collision";
    }

    public IntPtr[] LogicGraph_GetNativeObjects()
    {
      if (!HasEngineInstance())
        return null;
      return new IntPtr[1] { EngineResource.GetNativeObject() };
    }

    #endregion
  }

  #endregion

  #region HavokResourceShape Creator Plugin

  [CSharpFrameworkTests.Common.PrefabTest.IgnoreForPrefabTests]
  class HavokResourceShapeCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public HavokResourceShapeCreator()
    {
      IconIndex = HavokResourceShape.ICON_INDEX;
    }

    public override string GetPluginName()
    {
      return "Generic Havok Resource";
    }

    public override string GetPluginCategory()
    {
      return "Havok";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Loads a Havok file (e.g. hkt) and puts its content into the respective world.";
    }


    public override ShapeBase CreateShapeInstance()
    {
      HavokResourceShape shape = new HavokResourceShape("Havok Resource");
      return shape;
    }
  }

  #endregion

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
