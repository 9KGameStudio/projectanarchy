/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisPath_cl;
%nodefaultdtor VisPath_cl;

class VisPath_cl : public VisObject3D_cl
{
  
public:
  %rename(Remove) DisposeObject();
  void DisposeObject();

  void AddPathNode(VisPathNode_cl *node);

  void DeleteAllPathNodes();

  int GetPathNodeCount() const;
  
  void DrawPath(VColorRef color = V_RGBA_GREEN, float width = 2.0f);

  %extend{
     VisPathNode_cl* AddPathNode(const hkvVec3* pPos, const char * szKey = NULL)
     {
        VisPathNode_cl * pNode = new VisPathNode_cl(*pPos);
        if(szKey) pNode->SetObjectKey(szKey);
        
        self->AddPathNode(pNode);
        return pNode;
     }
     
     VisPathNode_cl* GetPathNode(int iIndex)
     {
        return self->GetPathNode(iIndex, TRUE);
     }
     
     VisPathNode_cl* GetPathNode(const char *szKey)
     {
        return self->FindPathNode(szKey, TRUE);
     }
     
     VSWIG_CONVERT_BOOL_SETTER(SetClosed);
     VSWIG_CONVERT_BOOL_GETTER_CONST(IsClosed);


     %apply hkvVec3* OUTPUT { hkvVec3 * pos };
     %apply hkvVec3* OUTPUT { hkvVec3 * ori };
     void Evaluate(hkvVec3* pos, hkvVec3* ori, float fPositionInPath, bool bSmooth = false)
     {
       if(bSmooth)
         self->EvalPointSmooth(fPositionInPath, *pos, ori);
       else
         self->EvalPoint(fPositionInPath, *pos, ori);
     }
  }

  float GetLen();
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisPath_cl, 128, "[%s : %1.2f,%1.2f,%1.2f]", self->IsClosed()?"C":"-", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)
VSWIG_CREATE_TOSTRING(VisPath_cl, "VisPath_cl '%s' [%s : %1.2f,%1.2f,%1.2f]", self->GetObjectKey(), self->IsClosed()?"C":"-", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)


#else

/// \brief Lua wrapper class for VisPath_cl.
class VisPath_cl : public VisObject3D_cl {
public:

  /// @name Common Path Functions
  /// @{
  
  /// \brief Remove this path and it's node from the scene.
  void Remove();

  /// \brief Remove all path nodes from this path.
  void DeleteAllPathNodes();

  /// \brief Returns the number of path nodes present in this path.
  /// \return The number of path nodes.
  number GetPathNodeCount();
 
  /// \brief Adds one path node to this path.
  /// \param node The path node to add.
  void AddPathNode(VisPathNode_cl node);
 
  /// \brief Create and add a new path node to this path.
  /// \param pos The position, where the path node should be created.
  /// \param key (\b optional) The object key of the new path node.
  VisPathNode_cl AddPathNode(hkvVec3 pos, string key = nil);
     
  /// \brief Gets the path node at the given index. 
  /// \param node The desired path node as index from 0 to GetPathNodeCount()-1 or as string with the key.
  /// \return The requested path node or nil.
  VisPathNode_cl GetPathNode(mixed node);

  /// \brief Sets the closed status of the path. 
  /// Closed paths wrap around the path evaluation parameter, whereas non-closed path clamp the parameter to [0..1]
  /// \param closed Set to true to close the path otherwise false.
  void SetClosed(boolean closed);
  
  /// \brief Check whether the path is closed or not.
  /// \return true = path closed; false = path is not closed;
  boolean IsClosed();
  
  /// \brief Returns the length of the path.
  /// The length of a path is cached until path properties are changed that influence the length.
  /// Recalculating the length is a performance critical operation. 
  /// \return The length of this path in world units. 
  number GetLen();

  /// \brief Evaluates a point on the path. 
  /// \param position The position along the path from 0 (the first node) to 1 (the last node in this path).
  /// \param smooth (\b optional) Use a smoothed path for evaluation.
  /// \return Two hkvVec3 holding the position and direction of the request point. Please note that this direction is not an orientation in Euler angles. In order to set this direction on an object, please use VisObject3D_cl::SetDirection instead of VisObject3D_cl::SetOrientation.
  /// \par Example
  ///   \code
  ///     local path = Game:GetPath("MyPath")
  ///     local pos, ori = path:Evaluate(0.75, true)
  ///   \endcode
  multiple Evaluate(number position, boolean smooth = false);
  
  /// @}
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
