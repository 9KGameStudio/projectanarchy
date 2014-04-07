/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisPathNode_cl;
%nodefaultdtor VisPathNode_cl;

class VisPathNode_cl : public VisObjectKey_cl
{
public:

  void SetPosition(const hkvVec3& pos);

  %extend{
    VSWIG_RETURN_BY_VALUE_CONST(hkvVec3, GetPosition);
    
    void SetInTypeBezier(bool bBezier) 
    {
      self->SetTypeIn(bBezier ? VisPathNode_cl::BEZIER : VisPathNode_cl::LINEAR);
    }
    
    void SetOutTypeBezier(bool bBezier) 
    {
      self->SetTypeOut(bBezier ? VisPathNode_cl::BEZIER : VisPathNode_cl::LINEAR);
    }
    
    void MakeInColinearToOut() 
    {
      self->MakeControlVerticesColinear(IN_VECTOR);
    }
    
    void MakeOutColinearToIn() 
    {
      self->MakeControlVerticesColinear(OUT_VECTOR);
    }
  }
  

  void SetControlVertices(const hkvVec3& cv1, const hkvVec3& cv2);
  //inline void GetControlVertices(hkvVec3& cv1, hkvVec3& cv2) const;
  
  float GetLen(const VisPathNode_cl &next) const;

};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisPathNode_cl, 128, "[%1.2f,%1.2f,%1.2f]", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)
VSWIG_CREATE_TOSTRING(VisPathNode_cl, "VisPathNode_cl '%s' [%1.2f,%1.2f,%1.2f]", self->GetObjectKey(), self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)


#else

/// \brief Lua wrapper for path nodes.
class VisPathNode_cl : public VisObjectKey_cl {
public:

  /// @name Common Path Node Functions
  /// @{
  
  /// \brief set the position of the path node.
  /// \param pos The new position of the path node.
  void SetPosition(const hkvVec3 pos);

  /// \brief Get the position of the path node
  /// \return The position as vector.
  hkvVec3 GetPosition();
  
  /// \brief Sets the tangent generation type for the IN tangent vector.
  /// \param bezier Set to true for bezier interpolation or false for linear interpolation (will ignore the in-control vertex).
  /// \see SetControlVertices
  /// \see GetControlVertices
  void SetInTypeBezier(bool bezier);
  
  /// \brief Sets the tangent generation type for the OUT tangent vector.
  /// \param bezier Set to true for bezier interpolation or false for linear interpolation (will ignore the out-control vertex).
  /// \see SetControlVertices
  /// \see GetControlVertices
  void SetOutTypeBezier(bool bezier); 
  
  /// \brief Makes the IN control vertex of this node colinear to the OUT control vertex by mirroring the relative position.
  void MakeInColinearToOut();
  
  /// \brief Makes the OUT control vertex of this node colinear to the IN control vertex by mirroring the relative position.
  void MakeOutColinearToIn();

  /// \brief Sets the position of the control vertices in world space. 
  /// Internally, this function calculates the local space position relatively to the owner path so moving the path updates the node control vertices properly.
  /// \param inVertexControl Absolute position of the IN control vertex.
  /// \param outVertexControl Absolute position of the OUT control vertex.
  void SetControlVertices(hkvVec3 inVertexControl, hkvVec3 outVertexControl);
  
  /// \brief Evaluates the length between this node and the passed node. 
  /// \note
  ///   The length is approximated by subdividing the path into segments.
  ///   The number of segments is defined by the PATH_STEPS constant, which is currently set to 100.
  ///   The length is not cached. Thus this operation is performace critical.
  /// \param next The path node used for distance evaluation.
  /// \return The length in between the two path nodes.
  number GetLen(VisPathNode_cl next);
  
  /// @}
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
