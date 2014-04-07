/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
using System;
using System.IO;
using System.Drawing.Design;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using CSharpFramework;
using ManagedBase;
using ManagedFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using CSharpFramework.PropertyEditors;
using CSharpFramework.DynamicProperties;
using CSharpFramework.UndoRedo;
using CSharpFramework.Actions;
using CSharpFramework.Scene;
using CSharpFramework.View;
using CSharpFramework.Serialization;
using VisionManaged;
using System.Drawing;
using CSharpFramework.BaseTypes;

namespace VisionEditorPlugin.Shapes
{
  #region Mobile Water Shape
  [Serializable, ShapeTypeNiceName("Mobile Water")]
  class MobileWaterShape : ShapeObject3D
  {
    #region Category Sorting Definitions

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_GEOMETRY = "Geometry";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_GEOMETRY = Shape3D.LAST_CATEGORY_ORDER_ID + 1;

    /// <summary>
    /// Category string
    /// </summary>
    protected const string CAT_RENDERING = "Rendering";

    /// <summary>
    /// Category ID
    /// </summary>
    protected const int CATORDER_RENDERING = Shape3D.LAST_CATEGORY_ORDER_ID + 2;


    /// <summary>
    /// last used category ID
    /// </summary>
    public new const int LAST_CATEGORY_ORDER_ID = CATORDER_RENDERING;

    #endregion

    #region Constructor
    public MobileWaterShape(string name) : base(name)
    {
      AddHint(HintFlags_e.NoScale);
      AddHint(HintFlags_e.OnlyYawRotation);
      _shaderFX = new ShaderEffectConfig(this);

      // remember to add the same stuff in the serialization constructor
      _shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
    }

    ~MobileWaterShape()
    {
      if (_shaderFX != null)
        _shaderFX.OnShaderEffectChanged -= new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
    }
    #endregion

    #region Engine Instance
    /// <summary>
    /// Create the engine instance
    /// </summary>
    /// <param name="bCreateChildren"></param>
    public override void CreateEngineInstance(bool bCreateChildren)
    {
      this._engineInstance = new EngineInstanceMobileWater();
      SetEngineInstanceBaseProperties(); // sets the position etc.

      base.CreateEngineInstance(bCreateChildren);
    }

    public override void SetEngineInstanceBaseProperties()
    {
      base.SetEngineInstanceBaseProperties();

      EngineMobileWater.SetUseRadialGrid(_useRadialGrid);
      EngineMobileWater.SetSize(_size.X, _size.Y);
      EngineMobileWater.SetTextureTiling(_textureTiling.X, _textureTiling.Y);
      EngineMobileWater.SetNumGridSubdivisions(_gridSubdivisions.X, _gridSubdivisions.Y);
      if(_shaderFX != null)
        EngineMobileWater.SetShaderEffect(_shaderFX);
      EngineMobileWater.SetUseStaticLighting(_useStaticLighting);
    }
    #endregion

    #region GUI

    /// <summary>
    /// Overridden function to get the icon index
    /// </summary>
    /// <returns></returns>
    public override int GetIconIndex()
    {
      return EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"bitmaps\Shapes\shapes_water.bmp"), Color.Magenta);
    }

    #endregion

    #region ISerializable

    /// <summary>
    /// Called when deserializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    protected MobileWaterShape(SerializationInfo info, StreamingContext context) : base(info, context)
    {
      AddHint(HintFlags_e.NoScale);
      AddHint(HintFlags_e.OnlyYawRotation);
      _size = (Vector2F)info.GetValue("_size", typeof(Vector2F));
      _useRadialGrid = info.GetBoolean("_useRadialGrid");
      _textureTiling = (Vector2F)info.GetValue("_textureTiling", typeof(Vector2F));
      _gridSubdivisions = (Vector2I)info.GetValue("_gridSubdivisions", typeof(Vector2I));
      _renderHook = (RenderOrderBits_e)info.GetValue("_renderHook", typeof(RenderOrderBits_e));
      _shaderFX = (ShaderEffectConfig)info.GetValue("_shaderFX", typeof(ShaderEffectConfig));
      if (SerializationHelper.HasElement(info, "_useStaticLighting"))
        _useStaticLighting = info.GetBoolean("_useStaticLighting");
      _shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
      _shaderFX.Owner = this;
    }

    /// <summary>
    /// Called when serializing
    /// </summary>
    /// <param name="info"></param>
    /// <param name="context"></param>
    public override void GetObjectData(SerializationInfo info, StreamingContext context) 
    {
      base.GetObjectData(info,context);
      info.AddValue("_size", _size);
      info.AddValue("_useRadialGrid", _useRadialGrid);
      info.AddValue("_gridSubdivisions",_gridSubdivisions);
      info.AddValue("_renderHook", _renderHook);
      info.AddValue("_shaderFX", _shaderFX);
      info.AddValue("_textureTiling", _textureTiling);
      info.AddValue("_useStaticLighting", _useStaticLighting);
    }

    #endregion

    #region internal methods
    private void _shaderFX_OnShaderEffectChanged(object sender, CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedArgs e)
    {
      if (EngineMobileWater != null)
        EngineMobileWater.SetShaderEffect(_shaderFX);
    }
    #endregion

    #region members
    [BrowsableAttribute(false)]
    public EngineInstanceMobileWater EngineMobileWater { get { return (EngineInstanceMobileWater)_engineInstance; } }

    Vector2F _size = new Vector2F(1024.0f, 1024.0f);
    Vector2I _gridSubdivisions = new Vector2I(10, 10);
    bool _useRadialGrid = false;
    bool _useStaticLighting = true;
    RenderOrderBits_e _renderHook = RenderOrderBits_e.PreTransparentPassEntities;
    ShaderEffectConfig _shaderFX;
    Vector2F _textureTiling = new Vector2F(1.0f, 1.0f);
    #endregion

    #region properties
    [Description("The size of the water plane")]
    [SortedCategory(CAT_GEOMETRY, CATORDER_GEOMETRY), PropertyOrder(1)]
    public Vector2F Size
    {
      get { return _size; }
      set
      {
        if(value.X < 0.1 || value.Y < 0.1)
          return;
        _size = value;
        if (HasEngineInstance())
          EngineMobileWater.SetSize(value.X, value.Y);
      }
    }

    [Description("Number of subdivisions for the water plane")]
    [SortedCategory(CAT_GEOMETRY, CATORDER_GEOMETRY), PropertyOrder(2)]
    public Vector2I GridSubdivisions
    {
      get { return _gridSubdivisions; }
      set
      {
        if (value.X < 1 || (!UseRadialGrid && value.Y < 1) // for a regular grid there should be at least 1 subdivision
           || (UseRadialGrid && value.Y < 3) || // for a radial grid there need to be at least 3 y-subdivisions => triangle
           (value.X + 1) * (value.Y + 1) > 0xFFFF) // ensure that the amount of generated vertices fits into a 16-bit index buffer (mobile limitation)
          return;
        _gridSubdivisions = value;
        if (HasEngineInstance())
          EngineMobileWater.SetNumGridSubdivisions(value.X, value.Y);
      }
    }

    [Description("If a radial grid should be used instead of a rectangular grid")]
    [SortedCategory(CAT_GEOMETRY, CATORDER_GEOMETRY), PropertyOrder(3)]
    public bool UseRadialGrid
    {
      get { return _useRadialGrid; }
      set
      {
        _useRadialGrid = value;
        if (value && _gridSubdivisions.Y < 3)
          this.GridSubdivisions = new Vector2I(this.GridSubdivisions.X, 3);
        if (HasEngineInstance())
          EngineMobileWater.SetUseRadialGrid(value);
      }
    }

    [Description("How many times the textures should be repeated on the water plane")]
    [SortedCategory(CAT_GEOMETRY, CATORDER_GEOMETRY), PropertyOrder(4)]
    public Vector2F TextureTiling
    {
      get { return _textureTiling; }
      set
      {
        if ((value.X > -0.01 && value.X < 0.01) || (value.Y > 0.01 && value.Y < 0.01))
          return;
        _textureTiling = value;
        if (HasEngineInstance())
          EngineMobileWater.SetTextureTiling(value.X, value.Y);
      }
    }

    [Description("If static lighting (Lightgrid) should be applied to the mesh used for water rendering")]
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(1)]
    public bool UseStaticLighting
    {
      get { return _useStaticLighting; }
      set
      {
        if (value != _useStaticLighting)
        {
          _useStaticLighting = value;
          if (HasEngineInstance())
            EngineMobileWater.SetUseStaticLighting(value);
        }
      }
    }

    [Description("At which point in the render loop the water should be rendered")]
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(2)]
    public RenderOrderBits_e RenderHook
    {
      get { return _renderHook; }
      set
      {
        if (_renderHook == value)
          return;

        _renderHook = value;
        if (HasEngineInstance())
          EngineMobileWater.SetRenderHook((uint)_renderHook);
      }
    }

    [TypeConverter(typeof(UndoableObjectConverter)),
    Description("The shader effect used for the water")]
    [SortedCategory(CAT_RENDERING, CATORDER_RENDERING), PropertyOrder(3)]
    public ShaderEffectConfig ShaderEffect
    {
      get {return _shaderFX;}
      set 
      {
        if (_shaderFX == value)
          return;

        if (_shaderFX != null)
        {
          _shaderFX.OnShaderEffectChanged -= new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
          _shaderFX.Dispose();
        }
        _shaderFX = value;
        if (_shaderFX != null)
        {
          _shaderFX.OnShaderEffectChanged += new CSharpFramework.Scene.ShaderEffectConfig.ShaderEffectChangedEventHandler(_shaderFX_OnShaderEffectChanged);
          _shaderFX.Owner = this;
          _shaderFX.UpdateEffect();
        }
      }
    }

    #endregion

    #region Plugin information

    /// <summary>
    /// Overridden function
    /// </summary>
    /// <returns></returns>
    public override EditorPluginInfo GetPluginInformation()
    {
      return EditorPlugin.EDITOR_PLUGIN_INFO;
    }

    #endregion

    #region Shape3D overrides

    public override void OnSceneEvent(SceneEventArgs args)
    {
      base.OnSceneEvent(args);

      if (args.action == SceneEventArgs.Action.AfterStaticLighting || args.action == SceneEventArgs.Action.StaticLightingReset)
      {
        if (_useStaticLighting && HasEngineInstance())
          EngineMobileWater.UpdateStaticLighting();
      }
    }

    /// <summary>
    /// Overridden OnSelected function
    /// </summary>
    public override void OnSelected()
    {
      base.OnSelected();
      CreateHotSpots();
    }

    /// <summary>
    /// Overridden OnUnSelected function
    /// </summary>
    public override void OnUnSelected()
    {
      base.OnUnSelected();
      RemoveHotSpots();
    }

    /// <summary>
    /// Overridden OnTransformationChanged function
    /// </summary>
    public override void OnTransformationChanged()
    {
      base.OnTransformationChanged();
      if(_useStaticLighting && HasEngineInstance())
        EngineMobileWater.UpdateStaticLighting();
    }
    #endregion

    #region Hotspots
    HotSpotDistance _hotSpotX = null;
    HotSpotDistance _hotSpotY = null;
    HotSpotDistance _hotSpotXY = null;

    public void CreateHotSpots()
    {
      if (_hotSpotX == null)
      {
        _hotSpotX = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 10.0f);
        _hotSpotX.StartDistance = this.Size.X;
        _hotSpotX.DisplayScaling = 0.5f;
        _hotSpotX.Axis = this.XAxis;
        _hotSpotX.ToolTipText = "size in x direction";
        EditorManager.ActiveView.HotSpots.Add(_hotSpotX);
      }

      if (_hotSpotY == null)
      {
        _hotSpotY = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 10.0f);
        _hotSpotY.StartDistance = this.Size.Y;
        _hotSpotY.DisplayScaling = 0.5f;
        _hotSpotY.Axis = this.YAxis;
        _hotSpotY.ToolTipText = "size in y direction";
        EditorManager.ActiveView.HotSpots.Add(_hotSpotY);
      }

      if (_hotSpotXY == null)
      {
        _hotSpotXY = new HotSpotDistance(this, @"textures\Hotspot_Rect.dds", VisionColors.Yellow, HotSpotBase.PickType.Square, 10.0f);
        _hotSpotXY.StartDistance = this.Size.GetLength();
        _hotSpotXY.DisplayScaling = 0.5f;
        Vector3F axis = (this.YAxis * Size.Y + this.XAxis * Size.X);
        axis.Normalize();
        _hotSpotXY.Axis = axis;
        _hotSpotXY.ToolTipText = "size";
        EditorManager.ActiveView.HotSpots.Add(_hotSpotXY);
      }
    }

    public void RemoveHotSpots()
    {
      if (_hotSpotX != null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotX);
        _hotSpotX.Remove();
        _hotSpotX = null;
      }

      if (_hotSpotY != null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotY);
        _hotSpotY.Remove();
        _hotSpotY = null;
      }

      if (_hotSpotXY != null)
      {
        EditorManager.ActiveView.HotSpots.Remove(_hotSpotXY);
        _hotSpotXY.Remove();
        _hotSpotXY = null;
      }
    }

    /// <summary>
    /// Called when user starts dragging a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragBegin(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot == _hotSpotX)
        _hotSpotX.StartDistance = Size.X;

      if (hotSpot == _hotSpotY)
        _hotSpotY.StartDistance = Size.Y;

      if (hotSpot == _hotSpotXY)
        _hotSpotXY.StartDistance = Size.GetLength();
    }

    /// <summary>
    /// Called while the user drags a hotspot that belongs to this shape
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    /// <param name="fDeltaX"></param>
    /// <param name="fDeltaY"></param>
    public override void OnHotSpotDrag(HotSpotBase hotSpot, VisionViewBase view, float fDeltaX, float fDeltaY)
    {
      if (hotSpot == _hotSpotX)
        Size = new Vector2F(_hotSpotX.CurrentDistance, Size.Y); // update for the view
      if (hotSpot == _hotSpotY)
        Size = new Vector2F(Size.X, _hotSpotY.CurrentDistance); // update for the view
      if(hotSpot == _hotSpotXY)
      {
        float ratio = Size.Y / Size.X;
        float newX = _hotSpotXY.CurrentDistance / (float)System.Math.Sqrt(ratio * ratio + 1.0f);
        float newY = ratio * newX;
        Size = new Vector2F(newX, newY);
      }
    }

    /// <summary>
    /// Called when the user releases the mouse button on a hotspot
    /// </summary>
    /// <param name="hotSpot"></param>
    /// <param name="view"></param>
    public override void OnHotSpotDragEnd(HotSpotBase hotSpot, VisionViewBase view)
    {
      if (hotSpot == _hotSpotX)
      {
        if (_hotSpotX.HasChanged)
        {
          float fNewX = _hotSpotX.CurrentDistance;
          Size = new Vector2F(_hotSpotX.StartDistance, Size.Y); // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "Size", new Vector2F(fNewX, Size.Y))); // send an action which sets the property from old value to new one
        }
      }

      if (hotSpot == _hotSpotY)
      {
        if (_hotSpotY.HasChanged)
        {
          float fNewY = _hotSpotY.CurrentDistance;
          Size = new Vector2F(Size.X, _hotSpotY.StartDistance); // set old value for the action
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "Size", new Vector2F(Size.X, fNewY))); // send an action which sets the property from old value to new one
        }
      }

      if (hotSpot == _hotSpotXY)
      {
        if (_hotSpotXY.HasChanged)
        {
          float ratio = Size.Y / Size.X;
          //compute old value
          float oldX = _hotSpotXY.StartDistance / (float)System.Math.Sqrt(ratio * ratio + 1.0f);
          float oldY = oldX * ratio;
          Size = new Vector2F(oldX, oldY); // set old value for the action

          //compute new value
          float newX = _hotSpotXY.CurrentDistance / (float)System.Math.Sqrt(ratio * ratio + 1.0f);
          float newY = ratio * newX;
          EditorManager.Actions.Add(SetPropertyAction.CreateSetPropertyAction(this, "Size", new Vector2F(newX, newY))); // send an action which sets the property from old value to new one
        }
      }
    }

    /// <summary>
    /// Called every frame and per hotspot that belongs to the shape
    /// </summary>
    /// <param name="hotSpot"></param>
    public override void OnHotSpotEvaluatePosition(HotSpotBase hotSpot)
    {
      if (hotSpot == _hotSpotX)
      {
        if (!_hotSpotX.IsDragging)
        {
          _hotSpotX.StartDistance = Size.X;
          _hotSpotX.Axis = this.XAxis;
        }
      }

      if (hotSpot == _hotSpotY)
      {
        if (!_hotSpotY.IsDragging)
        {
          _hotSpotY.StartDistance = Size.Y;
          _hotSpotY.Axis = this.YAxis;
        }
      }

      if (hotSpot == _hotSpotXY)
      {
        if (!_hotSpotXY.IsDragging)
        {
          _hotSpotXY.StartDistance = Size.GetLength();
          Vector3F axis = (this.YAxis * Size.Y + this.XAxis * Size.X);
          axis.Normalize();
          _hotSpotXY.Axis = axis;
        }
      }
    }
    #endregion
  }
  #endregion

  #region Mobile Water creator plugin
  class MobileWaterCreator : CSharpFramework.IShapeCreatorPlugin
  {
    public MobileWaterCreator()
    {
      IconIndex = EditorManager.GUI.ShapeTreeImages.AddBitmap(Path.Combine(EditorManager.AppDataDir, @"Bitmaps\Shapes\shapes_water.bmp"), Color.Magenta);
    }

    public override string GetPluginName()
    {
      return "MobileWater";
    }

    public override string GetPluginCategory()
    {
      return "Effects";
    }

    /// <summary>
    /// Returns a short description text
    /// </summary>
    /// <returns></returns>
    public override string GetPluginDescription()
    {
      return "Adds a simple water effect to the scene.\n"
           + "This simple water effect was specially designed to be used on mobile devices";
    }

    public override ShapeBase CreateShapeInstance()
    {
      MobileWaterShape shape = new MobileWaterShape("MobileWater");
      shape.Position = EditorManager.Scene.CurrentShapeSpawnPosition;
      //assign a default effect:
      if (EditorManager.EngineManager.IsInitialized())
      {
        shape.ShaderEffect.EffectLibrary = @"\Shaders\MobileWater.Shaderlib";
        shape.ShaderEffect.EffectName = "MobileWater";
      }
      return shape;
    }
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
