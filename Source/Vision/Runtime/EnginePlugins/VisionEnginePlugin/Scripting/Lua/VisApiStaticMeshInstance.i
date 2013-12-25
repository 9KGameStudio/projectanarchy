/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisStaticMeshInstance_cl;
%nodefaultdtor VisStaticMeshInstance_cl;

class VisStaticMeshInstance_cl : public VisTypedEngineObject_cl, public VisObjectKey_cl
{
public:

  %extend{
  
    VSWIG_RETURN_BY_VALUE_CONST(hkvAlignedBBox, GetBoundingBox);
  
    hkvVec3 GetPosition()
    {
      return self->GetTransform().getTranslation(); //return by value
    }
    
    hkvMat3 GetRotation()
    {
      return self->GetTransform().getRotationalPart(); //return by value
    }
    
    VSWIG_BITMASK_SETTER(SetVisibleBitmask);
    
    VSWIG_BITMASK_SETTER(SetCollisionBitmask);
    VSWIG_BITMASK_GETTER_CONST(GetCollisionBitmask);
  }
  
  %rename(Remove) DisposeObject();
  void DisposeObject();

  bool IsLightmapped() const;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisStaticMeshInstance_cl, 128, "%s", self->GetObjectKey()==NULL?self->GetClassTypeId()->m_lpszClassName:self->GetObjectKey() )
VSWIG_CREATE_TOSTRING_EX(VisStaticMeshInstance_cl, hkvVec3 vPos = self->GetTransform().getTranslation();, "%s: %s [%1.2f,%1.2f,%1.2f]", self->GetClassTypeId()->m_lpszClassName, self->GetObjectKey(), vPos.x, vPos.y, vPos.z)
		
#else

/// \brief Wrapper class for static mesh instances.
class VisStaticMeshInstance_cl : public VisTypedEngineObject_cl, public VisObjectKey_cl {
public:

  /// @name Common Static Mesh Instatnce Functions
  /// @{
  
  /// \brief Get the current position of the static mesh instance.
  /// \return A vector with the position.
  hkvVec3 GetPosition();
  
  /// \brief The roation of the static mesh instance.
  /// \return A matrix with the rotation data
  hkvMat3 GetRotation();

  /// \brief Get the bounding box of the static mesh instance.
  /// \return The bounding box of the static mesh instance.
  hkvAlignedBBox GetBoundingBox();

  /// \brief Makes the static mesh instance visible in certain rendering contexts only.
  /// \details
  ///    This static mesh instance will only be rendered in render contexts, if the logical AND result of this bitmask and the context's bitmask is not zero.
  /// \param mask The new visible bitmask.
  /// \see VBitmask
  void SetVisibleBitmask(VBitmask mask);

  /// \brief Specifies the collision bitmask for the static mesh instance. 
  /// \param mask The new collision bitmask.
  /// \param collisionBehavior The new collision behavior
  /// \see VBitmask
  void SetCollisionBitmask(VBitmask mask, int collisionBehavior=1);

  /// \brief Gets the collision bitmask for the static mesh instance. 
  /// \return The collision bitmask as VBitmask.
  /// \see VBitmask
  VBitmask GetCollisionBitmask();
    
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
