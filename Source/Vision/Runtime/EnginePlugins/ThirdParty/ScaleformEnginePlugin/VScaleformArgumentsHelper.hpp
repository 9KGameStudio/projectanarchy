/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSCALEFORMARGUMENTSHELPER_HPP_INCLUDED
#define VSCALEFORMARGUMENTSHELPER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformValue.hpp>
#include <GFx/GFx_Player.h>

/// \brief
///   Helper class which allows to store an array of Scaleform::GFX::Value arguments on the stack in most cases.
///   Dynamic allocation is only performed for larger arrays.
class VScaleformArgumentsHelper
{
public:
  VScaleformArgumentsHelper(const VScaleformValue* pArgs, unsigned int uiNumArgs)
    : m_pArguments(m_stackArguments)
  {
    // Only allocate dynamically if necessary.
    if (uiNumArgs > NumStackArguments)
    {
      m_pArguments = new Scaleform::GFx::Value[uiNumArgs];
    }

    for (unsigned int i = 0; i < uiNumArgs; i++)
    {
      m_pArguments[i] = pArgs[i].GetGFxValue();
    }
  }

  inline ~VScaleformArgumentsHelper()
  {
    if (m_pArguments != m_stackArguments)
      delete [] m_pArguments;
  }

  inline const Scaleform::GFx::Value* GetGFxArguments() const
  {
    return m_pArguments;
  }

private:
  Scaleform::GFx::Value* m_pArguments;

  static const unsigned int NumStackArguments = 8;
  Scaleform::GFx::Value m_stackArguments[NumStackArguments];
};

#endif

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
