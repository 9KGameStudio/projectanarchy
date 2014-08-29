/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSCALEFORMVALUECONTAINER_HPP_INCLUDED
#define VSCALEFORMVALUECONTAINER_HPP_INCLUDED

#include <GFx/GFx_Player.h>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariable.hpp>

class VScaleformMovieInstance;
struct VScaleformVariableInfo;

/// \brief
///   Helper class for caching VScaleformValue objects.
class VScaleformVariableManager
{
  // Non-copyable
  VScaleformVariableManager(const VScaleformVariableManager&);
  VScaleformVariableManager& operator=(const VScaleformVariableManager&);

public:
  // Constructor / Destructor
  VScaleformVariableManager();
  ~VScaleformVariableManager();

  /// \brief
  ///   Creates a handle to a Scaleform ActionScript variable.
  const VScaleformVariable CreateVariable(const Scaleform::GFx::Value& gfxValue, const char* szVarName,
    const VScaleformMovieInstance* pMovieInstance, const Scaleform::GFx::Value& parentGFxValue);

  /// \brief
  ///   Clears the container.
  void Reset();

  /// \brief 
  ///   Reads all variable values from their respective ActionScript instances.
  void SynchronizeRead();

  /// \brief
  ///   Returns whether the container is empty.
  inline bool IsEmpty() const
  {
    return (m_infos.GetSize() <= 0);
  }

  static void ReadSingleValueFromAS(VScaleformVariableInfo& info);

  static void WriteSingleValueToAS(VScaleformVariableInfo& info);

private:
  // Private functions
  VScaleformVariableInfo* FindInfo(const char* szName, const Scaleform::GFx::Value& parentGFxValue);

  // Member variables
  VArray<VSmartPtr<VScaleformVariableInfo> > m_infos;
};

#if !defined(_VISION_DOC)

struct VScaleformVariableInfo : public VRefCounter
{
  VScaleformVariableInfo() : m_pMovieInstance(NULL) {}

  VScaleformValue m_value;
  VString m_sVarName;
  Scaleform::GFx::Value m_parentGFxValue;
  const VScaleformMovieInstance* m_pMovieInstance;

private:
  // Non-copyable
  VScaleformVariableInfo(const VScaleformVariableInfo&);
  VScaleformVariableInfo& operator=(const VScaleformVariableInfo&);
};

#endif

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
