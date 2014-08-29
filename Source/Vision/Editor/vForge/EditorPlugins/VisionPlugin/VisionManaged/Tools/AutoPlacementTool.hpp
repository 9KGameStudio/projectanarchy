/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;

// The base class for include and exclude material definition
[Serializable]
public ref class PlacementMaterialBase : public ISerializable, public ICloneable, public PropertySorter
{
public:
  PlacementMaterialBase()
  {
    _bIsRegEx = false;
  }

  PlacementMaterialBase(SerializationInfo ^info, StreamingContext context)
  {
    _materialName = info->GetString("_materialName");
    _bIsRegEx = info->GetBoolean("_bIsRegEx");
  }

  virtual void GetObjectData(SerializationInfo ^info, StreamingContext context) 
  {
    info->AddValue("_materialName", _materialName);
    info->AddValue("_bIsRegEx",_bIsRegEx);
  }

  /// ICloneable implementation
  virtual Object ^Clone()
  {
    return MemberwiseClone();
  }

  // helper function for name matching
  bool MatchesMaterialName(const char *szName);
  bool MatchesMaterialName(String ^sName);

  String ^_materialName;
  bool _bIsRegEx;
};


// The base class for a include material definition. The class used by vForge is PlacementMaterialIncludeDisplay
[Serializable]
public ref class PlacementMaterialInclude : public PlacementMaterialBase
{
public:
  PlacementMaterialInclude()
  {
    _density = 1.0f;
    _densityTextureMask = Vector4F(1,0,0,0);
  }

  PlacementMaterialInclude(SerializationInfo ^info, StreamingContext context)
    : PlacementMaterialBase(info, context)
  {
    _density = info->GetSingle("_density");
    _densityTexture = info->GetString("_densityTexture");
    _densityTextureMask = (Vector4F)info->GetValue("_densityTextureMask",Vector4F::typeid);
  }

  virtual void GetObjectData(SerializationInfo ^info, StreamingContext context) override
  {
    PlacementMaterialBase::GetObjectData(info, context);
    info->AddValue("_density", _density);
    info->AddValue("_densityTexture", _densityTexture);
    info->AddValue("_densityTextureMask", _densityTextureMask);
  }

  static String^ DIFFUSE_TEXTURE = "<Diffuse Texture>";
  static String^ AUX0_TEXTURE = "<Auxiliary Texture #0>";
  static String^ AUX1_TEXTURE = "<Auxiliary Texture #1>";
  static String^ AUX2_TEXTURE = "<Auxiliary Texture #2>";
  static String^ AUX3_TEXTURE = "<Auxiliary Texture #3>";
  static String^ AUX4_TEXTURE = "<Auxiliary Texture #4>";
  static String^ AUX5_TEXTURE = "<Auxiliary Texture #5>";
  static String^ AUX6_TEXTURE = "<Auxiliary Texture #6>";
  static String^ AUX7_TEXTURE = "<Auxiliary Texture #7>";

  int DesityIsAuxTexture();
  VTextureObject *GetMaterialTexture(VisSurface_cl *pSurface, bool &bIsSurfaceTexture);

  String ^_densityTexture;
  Vector4F _densityTextureMask;
  float _density;
};


// The base class for a exclude material definition. The class used by vForge is PlacementMaterialExcludeDisplay
[Serializable]
public ref class PlacementMaterialExclude : public PlacementMaterialBase
{
public:
  PlacementMaterialExclude()
  {
    _border = 200.0f * EditorManager::Settings->GlobalUnitScaling;
  }

  PlacementMaterialExclude(SerializationInfo ^info, StreamingContext context)
    : PlacementMaterialBase(info, context)
  {
    _border = info->GetSingle("_border");
  }

  virtual void GetObjectData(SerializationInfo ^info, StreamingContext context) override
  {
    PlacementMaterialBase::GetObjectData(info, context);
    info->AddValue("_border", _border);
  }

  float _border;
};


// This class holds all properties to define the placement rules, including the material lists for inclusion/exclusion
[Serializable]
public ref class AutoPlacementProperties : public ISerializable, public ICloneable
{
public:
  AutoPlacementProperties()
  {
    Density = 1.0f;
    DistributionRandomness = 1.0f;
    MinYaw = 0.0f;
    MaxYaw = 360.f;
    AverageScale = 1.0f;
    ScaleVariation = 0.2f;
    HeightOffset = 0.0f;
  }

  AutoPlacementProperties(SerializationInfo ^info, StreamingContext context)
  {
    Density = info->GetSingle("Density");
    DistributionRandomness = info->GetSingle("DistributionRandomness");

    MinYaw = info->GetSingle("MinYaw");
    MaxYaw = info->GetSingle("MaxYaw");
    AverageScale = info->GetSingle("AverageScale");
    ScaleVariation = info->GetSingle("ScaleVariation");
    HeightOffset = info->GetSingle("HeightOffset");
    
    MaterialIncludes = (array<PlacementMaterialInclude ^>^)info->GetValue("MaterialIncludes",array<PlacementMaterialInclude ^>::typeid);
    MaterialExcludes = (array<PlacementMaterialExclude ^>^)info->GetValue("MaterialExcludes",array<PlacementMaterialExclude ^>::typeid);
  }

  VOVERRIDE void GetObjectData(SerializationInfo ^info, StreamingContext context) 
  {
    info->AddValue("Density", Density);
    info->AddValue("DistributionRandomness", DistributionRandomness);
    info->AddValue("MinYaw", MinYaw);
    info->AddValue("MaxYaw", MaxYaw);
    info->AddValue("AverageScale", AverageScale);
    info->AddValue("ScaleVariation", ScaleVariation);
    info->AddValue("HeightOffset", HeightOffset);

    info->AddValue("MaterialIncludes", MaterialIncludes);
    info->AddValue("MaterialExcludes", MaterialExcludes);
  }

  /// ICloneable implementation
  virtual Object ^ Clone()
  {
    return MemberwiseClone();
  }

  void InitializeMaterial(class VPlacementMaterialInfo *pInfo);

  float Density, DistributionRandomness, MinYaw, MaxYaw, AverageScale, ScaleVariation, HeightOffset;

  array<PlacementMaterialInclude ^> ^MaterialIncludes;
  array<PlacementMaterialExclude ^> ^MaterialExcludes;

  // temp data:
  float m_fGridSpacing;
};




// This class has static functions to run the actual tool
public ref class AutoPlacementTool
{
public:
  // key function: Run the actual placement with the passed properties. Output is a .vgroup file. Return value is an error string (or null for no error)
  static String^ GeneratePlacement(AutoPlacementProperties ^properties, float x1, float y1, float x2, float y2, float fModelSize, String ^absFilename, bool bAddFileRCS, ProgressStatus ^progress);
};

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
