/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor IVObjectComponent;
%nodefaultdtor IVObjectComponent;

class IVObjectComponent : public VisTypedEngineObject_cl
{
public:

  virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  int GetComponentID() const;

  const char* GetComponentName();

  void SetComponentID(int iValue);

  void SetComponentName(const char* szName);

  %extend{
    bool CanAttachToObject(VisTypedEngineObject_cl *typedObject)
    {
      VString sError;
      bool bPossible = self->CanAttachToObject(typedObject, sError) == TRUE;
      if(!bPossible) hkvLog::Warning("%s", sError.AsChar());
      return bPossible;
    }
  }
};

//Implement this method native in order to return the concrete type instance
//instead of the base class VisTypedEngineObject_cl
%native(IVObjectComponent_GetOwner) int IVObjectComponent_GetOwner(lua_State *L);
%{
  SWIGINTERN int IVObjectComponent_GetOwner(lua_State *L)
  {
    IS_MEMBER_OF(IVObjectComponent) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, IVObjectComponent, pSelf)

    lua_settop(L, 0);
    LUA_PushObjectProxy(L, pSelf->GetOwner()); //will handle NULL as well
 
    return 1;
  }
%} 

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(IVObjectComponent, 128, "%s", self->GetComponentName()==NULL?self->GetClassTypeId()->m_lpszClassName:self->GetComponentName() )
VSWIG_CREATE_TOSTRING(IVObjectComponent, "%s: %s", self->GetClassTypeId()->m_lpszClassName, self->GetComponentName())

#else

/// \brief Base class of any object component.
class IVObjectComponent : public VisTypedEngineObject_cl {
public:

  /// @name Common Object Component Functions
  /// @{
  
  /// \brief Set a new component owner.
  /// \param owner The new owner of the component.
  void SetOwner(VisTypedEngineObject_cl owner);
  
  /// \brief Get the current owner of the component.
  /// \return The owner or nil.
  mixed GetOwner();

  /// \brief Get the compoent ID.
  /// \return The component ID as number.
  number GetComponentID();

  /// \brief Set the compoent ID.
  /// \param id The new ID of the component.
  void SetComponentID(number id);
  
  /// \brief Get the name of the component.
  /// \return The name or nil if not set. 
  string GetComponentName();

  /// \brief Set the name of the component.
  /// \param name The new name of the component. 
  void SetComponentName(string name);
  
  /// \brief Check wheter the component can be attached to an object or not.
  /// \param object The new intended parent object. 
  /// \return true = component attachable to object, false = it is not possible to attach.
  boolean CanAttachToObject(VisTypedEngineObject_cl object)
  
  /// @}
};

#endif

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
