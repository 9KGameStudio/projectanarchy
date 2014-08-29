/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VDynamicMesh;
%nodefaultdtor VDynamicMesh;

class VDynamicMesh //as far as possible we hide this relation : public VBaseMesh
{
public:

  inline int GetSubmeshCount() const;
  inline int GetSurfaceCount() const;

  %extend{
    VSWIG_RETURN_BY_VALUE_CONST(hkvAlignedBBox, GetBoundingBox);
  
    bool ReloadShaderAssignment(const char * szFilename)
    {
      return self->ReloadShaderAssignmentFile(szFilename)==TRUE;
    }
  }
  
  %rename(GetSurface) GetSurfaceByName(const char* szSurfaceName) const;
  inline VisSurface_cl* GetSurfaceByName(const char* szSurfaceName) const;
  inline VisSurface_cl* GetSurface(int iIndex) const;

  inline int GetSurfaceIndex(VisSurface_cl* pSurface);
  
  %rename(GetName) GetFilename() const;
  const char * GetFilename() const;
  
  %extend{
    bool operator == (const VDynamicMesh& other)
    {
      return self==&other;
    }
  }

};

//Implement GetAllSurfaces native because it returns an array of surfaces
%native(VDynamicMesh_GetAllSurfaces) int VDynamicMesh_GetAllSurfaces(lua_State *L);
%{
  SWIGINTERN int VDynamicMesh_GetAllSurfaces(lua_State *L)
  {
    IS_MEMBER_OF(VDynamicMesh) //this will move this function to the method table of the specified class
      
    SWIG_CONVERT_POINTER(L, 1, VDynamicMesh, pMesh)

    lua_newtable(L);                                        //stack: ..., table
    
    int iCount = pMesh->GetSurfaceCount();
    VisSurface_cl** ppSurfaces = pMesh->GetSurfaceArray();

    for(int i=0;i<iCount;++i)
    {
      VSWIG_PUSH_PROXY(L, VisSurface_cl, ppSurfaces[i]);    //stack: ..., table, surface
      lua_rawseti(L, -2, i+1);                         //stack: ..., table
    } 
    
    return 1; //return the table as stack param
  }
%}

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VDynamicMesh, 128, "[%s]", self->GetFilename())
VSWIG_CREATE_TOSTRING(VDynamicMesh, "VDynamicMesh: '%s' (%d surfaces, %d sub meshes)", self->GetFilename(), self->GetSurfaceCount(), self->GetSubmeshCount())


#else

/// \brief Dynamic mesh wrapper class.
/// \see VisBaseEntity_cl::GetMesh
/// \see VisGame_cl::CreateDynamicMesh
/// \see VisGame_cl::GetDynamicMesh
class VDynamicMesh {
public:

  /// @name Common Dynamic Mesh Functions
  /// @{

  /// \brief Returns the name of the mesh (= the file name)
  /// \return The mesh name.
  string GetName();
      
  /// \brief Get the number of submeshes.
  /// \return The number of submeshes (1 = there is only a single mesh).
  number GetSubmeshCount();
  
  /// \brief Returns the bounding box for this mesh.
  /// \return The bounding box.
  hkvAlignedBBox GetBoundingBox();

  /// @}
  /// @name Surfaces
  /// @{
  
  /// \brief Get the number of surfaces.
  /// \return The number of defined surfaces for this mesh.  
  number GetSurfaceCount();

  /// \brief Get a specific surfaces.
  /// \param identifier A \b number, indicating the index of the surface or a \b string specifying the name of the surface.
  /// \return The requested surface or nil if not present.
  VisSurface_cl GetSurface(mixed identifier);

  /// \brief Returns the index of the surface or -1 for alien surfaces.
  /// \note: Can be used to determine whether the passed surface is used by this mesh.
  /// \param surface The surface to look up.
  /// \return The index of the surface or -1 if the specified surface does not belong to this mesh.
  number GetSurfaceIndex(VisSurface_cl surface);
 
  /// \brief Returns all surfaces of the mesh.
  /// \return A table containing all surfaces from index 1 to LEN.
  /// \par Example
  ///   \code
  ///   local surfaces = self.GetMesh():GetAllSurfaces()
  ///   for i=1,#surfaces do
  ///     Debug:PrintLine("Surface '" .. surfaces:GetName() .. "' : " .. surfaces:GetIndex());
  ///   end
  ///   \endcode
  table GetAllSurfaces();
  
  /// @}
  /// @name Shader Assignment
  /// @{

  /// \brief Reloads the associated materials.xml file and reassigns the shaders in it.
  /// \param filename Path and file name of the XML file.
  /// \return Returns true if the assignment was successful, otherwise false.
  boolean ReloadShaderAssignment(string filename);
  
  /// \brief Returns the param string of a given surface.
  /// \param surface The surface to add the effect specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \return Key/value dictionary of all parameters of the surface's effect.
  /// \par Example
  ///   \code
  ///     ...
  ///     local effectParams = self:GetEffectParams(0)
  ///     ...
  ///   \endcode
  /// \see VisBaseEntity_cl::SetEffect
  table GetEffectParams(mixed surface);

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
