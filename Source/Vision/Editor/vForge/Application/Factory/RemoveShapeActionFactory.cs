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
using System.Diagnostics;
using CSharpFramework.Helper;
using CSharpFramework;
using CSharpFramework.Actions;
using CSharpFramework.Shapes;

namespace Editor.Factory
{
  public class EditorRemoveShapeActionFactory : IClassFactory<IAction>
  {
    public IAction CreateInstance(object[] parameters)
    {
      int parameterLength = parameters.GetLength(0);
      Debug.Assert((parameterLength > 0) && (parameterLength < 3), "Wrong parameter count.", "There should be 1 or 2 parameters.");
      if (parameterLength == 1)
      {
        return new RemoveShapeAction((ShapeBase)parameters.GetValue(0));
      }
      else if (parameterLength == 2)
      {
        return new RemoveShapeAction((ShapeBase)parameters.GetValue(0), (bool)parameters.GetValue(1));
      }
      return null;
    }
  }
}

/*
 * Havok SDK - Base file, BUILD(#20130717)
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
