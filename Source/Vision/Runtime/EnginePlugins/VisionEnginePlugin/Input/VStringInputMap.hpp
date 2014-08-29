/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSTRINGINPUTMAP_HPP_INCLUDED
#define VSTRINGINPUTMAP_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

//forward declarations
typedef struct VInputOptions VInputOptions;

/// \brief 
///   VStringInputMap is an input map that additionally support strings to identify your triggers.
/// 			 
/// \note 
///   If you are also using integer based trigger indices, please setup their mapping before string based
///   triggers or use the iOptTriggerIndex parameter to map the named triggers to your intended integer
/// 	based triggers, otherwise the named trigger will accidentally use another ones (integer based) index.
/// 	You can also initialize your whole map with named triggers and use the integer representation in your
/// 	C++ code and the named trigger just in your script binding. By doing that you will not lose any
/// 	performance in C++ and will be still able to access all triggers without additional code in your
/// 	scripts as well.
///
/// \see VInputMap
class VStringInputMap: public VInputMap
{

public:

  /// \brief 
  ///   Create an input map, which uses an internal hash map to associate trigger names with the internal
  /// 	held trigger indices. So you can use strings to query your triggers. 
  /// 			 
  /// \note 
  ///   If you are also using integer based trigger indices, please setup their mapping before string based
  /// 	triggers or use the iOptTriggerIndex parameter to map the named triggers to your intended integer
  /// 	based triggers, otherwise the named trigger will accidentally use another ones (integer based) index.
  /// 	You can also initialize your whole map with named triggers and use the integer representation in your
  /// 	C++ code and the named trigger just in your script binding. By doing that you will not lose any
  /// 	performance in C++ and will be still able to access all triggers without additional code in your
  /// 	scripts as well.
  ///
  /// \param iNumTriggers     
  ///   [\b optional] The number of mappable trigger, adjust this value to your needs, default
  /// 	value is 32, VStringInputMap::MapTrigger will return -1 if you exceed this limit.
  ///
  /// \param iNumAlternatives
  ///   [\b optional] The number of mappable alternatives, adjust this value to your needs,
  ///   default value is 4, VStringInputMap::MapTrigger will return -1 if you exceed this limit.
  ///
  /// \see VInputMap
  EFFECTS_IMPEXP VStringInputMap(int iNumTriggers=32, int iNumAlternatives=4);

  EFFECTS_IMPEXP virtual ~VStringInputMap();

  /// \brief 
  ///   Maps a trigger using a string. The method behaves like VInputMap::MapTrigger but uses a string instead
  ///   an int for the trigger. Please refer to the VInputMap documentation for further details.
  ///
  /// \param szTriggerName    
  ///   The name of the trigger.
  ///
  /// \param inputDevice      
  ///   The input device for the desired control.
  ///
  /// \param uiControl        
  ///   The control (key, button, touch, ...) to map.
  ///
  /// \param options          
  ///   [\b optional] The options defining the behavior of the mapped control.
  ///
  /// \param iOptTriggerIndex 
  ///   [\b optional] Manually define which trigger index should be used for the string constant.
  ///   Default is -1, so that the named trigger will be mapped to the first free index.
  ///
  /// \return                 
  ///   The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  /// 				
  /// \see VInputMap::MapTrigger(int, IVInputDevice &, unsigned int, const VInputOptions &)
  EFFECTS_IMPEXP int MapTrigger(const char * szTriggerName, IVInputDevice &inputDevice, unsigned int uiControl, const VInputOptions &options = VInputMap::DEFAULT_INPUT_OPTIONS, int iOptTriggerIndex = -1);

  /// \brief 
  ///   Maps a trigger using a string. The method behaves like VInputMap::MapTrigger but uses a string instead
  /// 	an int for the trigger. Please refer to the VInputMap documentation for further details.
  ///
  /// \param szTriggerName    
  ///   The name of the trigger.
  /// 
  /// \param pArea            
  ///   The touch area for the desired control.
  /// 
  /// \param uiControl        
  ///   The control (key, button, touch, ...) to map.
  /// 
  /// \param options         
  ///   [\b optional] The options defining the behavior of the mapped control.
  /// 
  /// \param iOptTriggerIndex 
  ///   [\b optional] Manually define which trigger index should be used for the string constant.
  ///   Default is -1, so that the named trigger will be mapped to the first free index.
  /// 
  /// \return                 
  ///   The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success
  /// 				
  /// \see VInputMap::MapTrigger(int, VTouchArea*, unsigned int, const VInputOptions &)
  EFFECTS_IMPEXP int MapTrigger(const char * szTriggerName, VTouchArea* pArea, unsigned int uiControl, const VInputOptions &options = VInputMap::DEFAULT_INPUT_OPTIONS, int iOptTriggerIndex = -1);

  /// \brief 
  ///   Maps a trigger axis using a string. The method behaves like VInputMap::MapTriggerAxis but uses a string instead
  /// 	an int for the trigger. Please refer to the VInputMap documentation for further details.
  ///
  /// \param szTriggerName     
  ///   The name of the trigger.
  /// 
  /// \param inputDevice      
  ///   The touch area for the desired control.
  /// 
  /// \param uiControlNegative 
  ///   The control (key, button, touch, ...) to map.
  /// 
  /// \param uiControlPositive 
  ///   The control (key, button, touch, ...) to map.
  /// 
  /// \param options           
  ///   [\b optional] The options defining the behavior of the mapped control.
  /// 
  /// \param iOptTriggerIndex 
  ///   [\b optional] Manually define which trigger index should be used for the string constant.
  /// 	Default is -1, so that the named trigger will be mapped to the first free index.
  ///
  /// \return                  
  ///   The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  /// 				
  /// \see VInputMap::MapTriggerAxis(int, IVInputDevice &, unsigned int, unsigned int, const VInputOptions &)
  EFFECTS_IMPEXP int MapTriggerAxis(const char * szTriggerName, IVInputDevice &inputDevice, unsigned int uiControlNegative, unsigned int uiControlPositive, const VInputOptions &options = VInputMap::DEFAULT_INPUT_OPTIONS, int iOptTriggerIndex = -1);
  using VInputMap::MapTriggerAxis; // Do not hide function in base class.
  
#if defined(_VISION_PS3) || defined(_VISION_WIIU)

  // this additional method is required because the PS3 compiler complains about
  // an ambitious method when using float GetTrigger(const char *szTriggerName)
  inline float GetTrigger(int iTriggerIndex)
  {
    return VInputMap::GetTrigger(iTriggerIndex);
  }

#endif

  /// \brief 
  ///   Gets a trigger value, where the trigger is identified by a string. The method behaves like VInputMap::GetTrigger but
  ///   performs a lookup of the trigger (to get the internally assigned integer) and calls VInputMap::GetTrigger afterwards.
  /// 	So this operation is slightly more expensive because of the additional lookup.
  ///
  /// \param szTriggerName 
  ///   The name of the trigger.
  ///
  /// \return 
  ///   The current value of the trigger, which usually ranges from 0 to 1 (or -1 to 1 for axes).
  inline float GetTrigger(const char *szTriggerName)
  {
    int iTriggerIndex = 0;

    if(!m_hashMap.Lookup(szTriggerName, iTriggerIndex))
    {
      //hkvLog::Warning("Input trigger '%s' is unknown!", szTriggerName);
      return 0;
    }

    return VInputMap::GetTrigger(iTriggerIndex);
  }
  using VInputMap::GetTrigger; // Do not hide function in base class.

  /// \brief 
  ///   Returns the index of this instance into its manager.  
  inline unsigned int GetIndex() const 
  {
    return m_iManagerIndex;
  }

protected:
  //to find the next free trigger for not yet assigned named triggers
  EFFECTS_IMPEXP int GetNextFreeTriggerIndex() const;

  VMap<VString, int> m_hashMap;
  unsigned int m_iManagerIndex;

};

#endif //VSTRINGINPUTMAP_HPP_INCLUDED

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
