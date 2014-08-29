/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;
using namespace System::Collections;
using namespace System;

#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>

class VCustomVolumeObject;

namespace VisionManaged
{

  class EarClippingCallbacks
  {
  public:
    virtual ~EarClippingCallbacks() {}
    virtual void Init(int numNeededVertices) = 0;
    virtual void AddVertex(float x, float y, float z) = 0;
    virtual void AddTriangle(int iIndex1, int iIndex2, int iIndex3) = 0;
    virtual void Finish() = 0;
  };

  class EarClippingToVMeshFile : public EarClippingCallbacks
  {
  public:
    EarClippingToVMeshFile(VString filename);

    virtual void Init(int numNeededVertices) override;
    virtual void AddVertex(float x, float y, float z) override;
    virtual void AddTriangle(int iIndex1, int iIndex2, int iIndex3) override;
    virtual void Finish() override;

  private:
    VGScene m_scene;
    VGMesh& m_mesh;
    VString m_sFilename;
  };

  class EarClippingToStaticMesh : public EarClippingCallbacks
  {
  public:
    EarClippingToStaticMesh(VisStaticMesh_cl& staticMesh);

    virtual void Init(int numNeededVertices) override;
    virtual void AddVertex(float x, float y, float z) override;
    virtual void AddTriangle(int iIndex1, int iIndex2, int iIndex3) override;
    virtual void Finish() override;
  private:
    struct Triangle 
    {
      Triangle() {}
      Triangle(int i1, int i2, int i3)
      {
        VASSERT_MSG(i1 >= 0 && i1 < 0xFFFF,"index overflow");
        VASSERT_MSG(i2 >= 0 && i2 < 0xFFFF,"index overflow");
        VASSERT_MSG(i3 >= 0 && i3 < 0xFFFF,"index overflow");
        this->i1 = (unsigned short)i1;
        this->i2 = (unsigned short)i2;
        this->i3 = (unsigned short)i3;
      }
      unsigned short i1,i2,i3;
    };

    VisStaticMesh_cl& m_staticMesh;
    hkvAlignedBBox m_bbox;
    hkvVec3* m_vertices;
    VArray<Triangle> m_triangles;
    int m_iCurVertexIndex;
    #ifdef HK_DEBUG
    int m_iMaxvertexIndex;
    #endif
  };

  //forward declaration
  ref class EngineInstanceCustomVolumeVertex;

  public ref class EngineInstanceCustomVolumeObject : public IEngineInstanceObject3D
  {
  public:     

    enum class VUpdateType_e 
    {
      VUT_UPDATE_LOCAL_VERTEX_POSITIONS,
      VUT_UPDATE_RETRIANGULATE
    };

    EngineInstanceCustomVolumeObject(Shape3D ^ownerShape);
    virtual void DisposeObject() override;

    // overridden IEngineShapeInstance functions
    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual void OnBeforeExport(SceneExportInfo ^info) override; 
    virtual bool OnExport(SceneExportInfo ^info) override;
    virtual void *GetObject3D() override { return m_pCustomVolumeEntity; }
    virtual void OnRenderHook(ShapeBase ^owner, int iConstant) override;

    virtual void SetScaling(float x, float y, float z) override;
    virtual void SetVisible(bool value) override {}

    // Setter getter for native class
    void SetCustomStaticMesh(bool bValue);
    void SetStaticMeshPath(String^ path);

    // special
    void RunEarClipping(EarClippingCallbacks& callbacks);
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void UpdateStaticMesh(VUpdateType_e updateType);
    void OnCreationFinished();

    //checks if the custom static mesh is valid or not, returns null if yes, error message otherwise
    String^ CheckIsStaticMeshValid();

    inline float GetHeight() { return m_fHeight; }
    inline void SetHeight(float fValue) { m_fHeight = fValue; UpdateStaticMesh(VUpdateType_e::VUT_UPDATE_LOCAL_VERTEX_POSITIONS); }

  private:
    // Helper initializing the runtime created static mesh
    void InitStaticMesh();

    float m_fHeight;
    Shape3D ^m_pOwnerShape;

    VCustomVolumeObject* m_pCustomVolumeEntity;
    VisStaticMeshPtr* m_pspEditorStaticMesh; // Holds the runtime created static mesh.
    bool m_bHasCustomMesh;  // This flag is needed because the m_pspEditorStaticMesh can only be created 
                            // after the creation of the shape has finished. We thus need to store this setting.

    bool m_bReverseWinding;
    bool m_bValid;

    VCompiledTechniquePtr* m_pspLightClippingVolumeDisplayMesh;
  };

  public ref class EngineInstanceCustomVolumeVertex : public IEngineInstanceObject3D
  {
  public:
    EngineInstanceCustomVolumeVertex(Shape3D ^owner, Shape3D ^shape);
    virtual void DisposeObject() override;

    // vertex should not return a value here as it does not inherit from VisTypedEngineObject_cl
    virtual IntPtr GetNativeObject() override  {return System::IntPtr::Zero;}
    virtual void OnPostEngineInstanceCreation() override;

    // overridden IEngineShapeInstance functions
    virtual void SetVisible(bool bStatus) override  {}
    virtual void SetOrientation(float yaw,float pitch,float roll) override  {}
    virtual void SetScaling(float x,float y, float z) override  {}
    virtual void SetObjectKey(String ^key) override {}
    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result) override;
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual bool OnExport(SceneExportInfo ^info) override { return true; }
    virtual void *GetObject3D() override  {return nullptr;}

    virtual void SetPosition(float x, float y, float z) override;
    virtual bool GetPosition(Vector3F %enginePosition) override;

    virtual bool GetOrientation(Vector3F %engineOrientation) override;

    // special
    void RenderShape(VisionViewBase ^view, ShapeRenderMode mode);
    void OnRemoveFromScene();

  private:
    VTextureObject* m_pIcon;
    Shape3D ^m_pOwner;
    Shape3D ^m_pShape;
    Vector3F m_previousLocalPosition;
  };

}

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
