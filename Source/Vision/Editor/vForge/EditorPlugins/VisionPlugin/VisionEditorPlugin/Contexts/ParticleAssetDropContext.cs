/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System.Diagnostics;
using CSharpFramework.Contexts;
using VisionEditorPlugin.Shapes;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Shapes;
using System.Drawing;
using CSharpFramework.Actions;
using VisionManaged;
using ManagedFramework;
using CSharpFramework.AssetManagement;

namespace VisionEditorPlugin.Contexts
{
  #region IDropContext

  /// <summary>
  /// Base class of a drag and drop context
  /// </summary>
  public class ParticleAssetDropContext : IDropContext
  {
    #region Dragging functions

    private void createInstance(System.Windows.Forms.DragEventArgs e)
    {
      EngineInstanceParticleGroup instance = new EngineInstanceParticleGroup();
      instance.SetEffectFile(System.IO.Path.ChangeExtension(_assetPaths[0], ".xml"));
      _instance = instance;
      _instance.SetRemoveWhenFinished(false); // Needed to be able to correctly restart the particle effect.
    }

    private void createShape(System.Windows.Forms.DragEventArgs e)
    {
      try
      {
        ParticleGroupShape particle = new ParticleGroupShape(System.IO.Path.GetFileNameWithoutExtension(_assetNames[0]));
        particle.EffectFile = System.IO.Path.ChangeExtension(_assetPaths[0], ".xml");
        particle.Position = _cachedPosition;
        _dummyShape = particle;
      }
      catch
      {
      }
    }

    public override void DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
    {
      _assetPaths = CSharpFramework.Contexts.IDropContext.GetAssetPaths(e);
      _assetTypes = CSharpFramework.Contexts.IDropContext.GetAssetTypes(e);
      _assetNames = CSharpFramework.Contexts.IDropContext.GetAssetNames(e);

      Debug.Assert(_assetPaths.Length == _assetTypes.Length && _assetPaths.Length == _assetNames.Length);
      if (_assetPaths.Length == 0)
        return;

      if (_assetTypes[0] == "Particle")
      {
        createInstance(e);
        DragOver(sender, e);
      }
      else
      {
        Debug.Assert(false, "The asset type " + _assetTypes[0] + " is not supported by ParticleAsssetDropContext!");
      }
    }

    public override void DragLeave()
    {
      if (_instance != null)
      {
        _instance.Dispose();
        _instance = null;
      }

      if (_dummyShape != null)
      {
        _dummyShape.Dispose();
        _dummyShape = null;
      }
      EditorManager.ActiveView.UpdateView(false);
    }

    public override void DragOver(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if (_instance == null)
        return;

      BoundingBox aabb = new BoundingBox();
      _instance.GetLocalBoundingBox(ref aabb);
      _cachedPosition = GetObjectPosition(new Point(e.X, e.Y), aabb, false);
      _instance.SetPosition(_cachedPosition);

      if (_instance.IsLifeTimeOver())
        _instance.Restart();

      EditorManager.ActiveView.UpdateView(true);
    }

    public override void DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
    {
      createShape(e);
      // Clones the dummy and places it correctly into the scene.
      EditorManager.ActiveView.DropObject(_dummyShape, e);

      _dummyShape.OnEditorUpdateUI();

      EditorManager.ActiveView.Focus();

      // Cleanup
      DragLeave();
    }

    #endregion

    #region Member variables

    private string[] _assetPaths;
    private string[] _assetTypes;
    private string[] _assetNames;
    private ParticleGroupShape _dummyShape;
    private EngineInstanceParticleGroup _instance;
    private Vector3F _cachedPosition = new Vector3F();
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
