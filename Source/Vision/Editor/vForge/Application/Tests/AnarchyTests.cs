/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Core;
using NUnit.Framework;
using CSharpFramework;
using CSharpFramework.Math;
using CSharpFramework.Scene;
using CSharpFramework.Shapes;
using CSharpFramework.DynamicProperties;
using CSharpFramework.Actions;

namespace Editor.Tests
{
  [TestFixture]
  public class AnarchyTests
  {
    [Test]
    public void TestAvailableShapes()
    {
      var allPlugins = EditorManager.ShapeCreatorPlugins.Cast<IEditorPlugin>();

#if HK_ANARCHY
      var shapeCategories = allPlugins.Select(p => p.GetPluginCategory()).Distinct().ToList();
      shapeCategories.Remove("Havok AI");
      shapeCategories.Sort();

      var requiredCategories = new[] { "Constraints", "Effects", "Fmod", "Geometry", "Grouping", "Havok", "Lighting", "Objects", "Visibility" };

      Assert.IsTrue(Enumerable.SequenceEqual(shapeCategories, requiredCategories));
#endif

#if HK_ANARCHY
      bool shouldContain = false;
#else
      bool shouldContain = true;
#endif

      var effectShapes = from p in allPlugins where p.GetPluginCategory() == "Effects" select p.GetPluginName();

      Assert.AreEqual(shouldContain, effectShapes.Contains("Projected Decal"));
      Assert.AreEqual(shouldContain, effectShapes.Contains("Shaped Fog"));
      Assert.AreEqual(shouldContain, effectShapes.Contains("Sun Glare"));
      Assert.AreEqual(shouldContain, effectShapes.Contains("Volumetric Cone"));
      Assert.AreEqual(shouldContain, effectShapes.Contains("Water"));

      var geomShapes = from p in allPlugins where p.GetPluginCategory() == "Geometry" select p.GetPluginName();

      Assert.AreEqual(shouldContain, geomShapes.Contains("Decoration Group"));
    }

    [Test]
    public void TestComponents()
    {
      DynamicPropertyCollectionType[] compTypes = EditorManager.EngineManager.ComponentClassManager.CollectionTypes;

      var componentNames = from t in compTypes select t.UniqueName.ToLowerInvariant();

#if HK_ANARCHY
      foreach (string name in componentNames)
      {
        if (name.Contains("shadow"))
        {
          Assert.IsTrue(name == "vbaseshadowmapcomponentspotdirectional" || name == "vmobileshadowmapcomponentspotdirectional" || name == "vblobshadow");
        }
      }
#endif
    }


    [Test]
    public void RendererTest()
    {
#if HK_ANARCHY
      var rendererNames = EditorManager.RendererNodeManager.NodeClassTypeManager.CollectionNames.ToList();
      rendererNames.Remove("VSimpleRendererNode");
      rendererNames.Sort();

      var requiredRenderers = new[] { "VMobileForwardRenderingSystem" };

      Assert.IsTrue(Enumerable.SequenceEqual(rendererNames, requiredRenderers), string.Format("Expected to find renderers of type(s) {0} but found {1}",
        string.Join(",", requiredRenderers),
        string.Join(",", rendererNames)));
#endif
    }

    [Test]
    public void SkyTest()
    {

#if HK_ANARCHY
      var skyNames = EditorManager.EngineManager.SkyManager.CollectionNames.ToList();
      skyNames.Sort();

      var requiredSkys = new[] { "VisSky_cl", "VSky", "VSkyBase" };

      Assert.IsTrue(Enumerable.SequenceEqual(skyNames, requiredSkys));
#endif
    }
  }
}

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
