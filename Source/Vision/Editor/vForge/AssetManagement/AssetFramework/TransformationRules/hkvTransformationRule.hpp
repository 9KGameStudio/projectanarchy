/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTransformationRule.hpp

#ifndef HKVTRANSFORMATIONRULE_HPP_INCLUDED
#define HKVTRANSFORMATIONRULE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>

class hkvAsset;
class hkvAssetLibrary;

class hkvITransformationControl
{
public:
  ASSETFRAMEWORK_IMPEXP hkvITransformationControl();
private:
  hkvITransformationControl(const hkvITransformationControl&);
  hkvITransformationControl& operator=(const hkvITransformationControl&);

public:
  ASSETFRAMEWORK_IMPEXP virtual void cancelTransformation() = 0;
};


class hkvITransformationControlHost
{
public:
  ASSETFRAMEWORK_IMPEXP hkvITransformationControlHost();
private:
  hkvITransformationControlHost(const hkvITransformationControlHost&);
  hkvITransformationControlHost& operator=(const hkvITransformationControlHost&);

public:
  ASSETFRAMEWORK_IMPEXP void setTransformationControl(hkvITransformationControl* control);
  ASSETFRAMEWORK_IMPEXP void cancelTransformation();

  ASSETFRAMEWORK_IMPEXP void incNumOutputsWritten();
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumOutputsWritten() const;

protected:
  virtual hkvCriticalSectionLock acquireLock() = 0;

private:
  hkvITransformationControl* m_transformationControl;
  hkUint32 m_numOutputsWritten;
};


class hkvTransformationRule : public hkReferencedObject, public hkvIProperties
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvTransformationRule> RefNew;
  typedef hkRefPtr<hkvTransformationRule> RefPtr;
  typedef hkRefPtr<const hkvTransformationRule> RefCPtr;

public:
  class Context;

public:
  ASSETFRAMEWORK_IMPEXP hkvTransformationRule (hkvTargetPlatform platform);

public: // Interface
  ASSETFRAMEWORK_IMPEXP virtual hkvTransformationRule* clone() const;
  ASSETFRAMEWORK_IMPEXP virtual hkUint32 getTypeIndex() const = 0;
  
  ASSETFRAMEWORK_IMPEXP virtual bool hasStaticTarget() const { return false; }
  ASSETFRAMEWORK_IMPEXP virtual bool getRelativeStaticTargetPath(const hkvAsset& asset, hkStringBuf& out_path) const { return false; }
  ASSETFRAMEWORK_IMPEXP bool getAbsoluteStaticTargetPath(const hkvAsset& asset, hkStringBuf& out_path) const;

  ASSETFRAMEWORK_IMPEXP virtual bool executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const = 0;

  /// \brief 
  ///   Fills output with a list of files that are generated for the given input.
  ///
  /// \param input
  ///   The input.
  /// \param output
  ///   The output.
  /// \param forLookUpTable
  ///   Whether this list is used for generating the lookup table used by the engine or for other purposes like stale asset cleanup.
  ///   If an output is not used by the engine it should be be written to the lookup table (e.g. cache files, intermediate output etc).
  /// \return
  ///   Whether any files were added to the output.
  ASSETFRAMEWORK_IMPEXP virtual bool queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output, bool forLookUpTable) const = 0;

  ASSETFRAMEWORK_IMPEXP virtual bool assetNeedsTransformation(const hkvAsset& asset) const;

public:
  inline hkvTargetPlatform getPlatform() const { return m_platform; }

  ASSETFRAMEWORK_IMPEXP hkUint32 computeHash() const;

protected:
  ASSETFRAMEWORK_IMPEXP void cleanUpTempFiles(Context& context) const;
  ASSETFRAMEWORK_IMPEXP hkResult copySourceFileToTemp(Context& context) const;
  ASSETFRAMEWORK_IMPEXP void deleteTargetFile(Context& context) const;
  ASSETFRAMEWORK_IMPEXP const char* makeAbsoluteTargetFilePath(Context& context, const char* fileNameAddition, hkStringBuf& out_filePath) const;
  ASSETFRAMEWORK_IMPEXP hkResult makeIntermediateBaseName(Context& context) const;
  ASSETFRAMEWORK_IMPEXP const char* makeIntermediateFileName(Context& context, const char* extension, hkStringBuf& out_fileName) const;
  ASSETFRAMEWORK_IMPEXP const char* makeTargetFileName(Context& context, const char* fileNameAddition, hkStringBuf& out_fileName) const;
  ASSETFRAMEWORK_IMPEXP hkResult moveTempFileToTarget(Context& context) const;
  ASSETFRAMEWORK_IMPEXP void notifyOutputWritten(Context& context, const char* variantKey) const;
  ASSETFRAMEWORK_IMPEXP const char* replaceVariables(Context& context, const char* inputString, hkStringBuf& out_str) const;

private:
  hkResult getVariableValue(Context& context, const char* varName, hkStringBuf& out_varValue) const;

protected:
  hkvTargetPlatform m_platform;
};

class hkvTransformationRule::Context
{
public:
  ASSETFRAMEWORK_IMPEXP Context(const hkvTransformationInput& input, hkvTransformationOutput& output);

public:
  ASSETFRAMEWORK_IMPEXP void clearExtraMetadata();
  ASSETFRAMEWORK_IMPEXP void setExtraMetadataValue(const char* name, bool value);
  ASSETFRAMEWORK_IMPEXP void setExtraMetadataValue(const char* name, const char* value);
  ASSETFRAMEWORK_IMPEXP void setExtraMetadataValue(const char* name, double value);
  ASSETFRAMEWORK_IMPEXP void setExtraMetadataValue(const char* name, int value);
  ASSETFRAMEWORK_IMPEXP const char* makeExtraMetadataString(hkStringBuf& buffer) const;

private:
  void setExtraMetadataProperty(const hkvProperty& prop);

public:
  const hkvTransformationInput& m_input;
  hkvTransformationOutput& m_output;

  hkStringPtr m_tempPath;
  hkStringPtr m_tempFileNameBase;
  hkUint32 m_tempFileCounter;
  hkArray<hkStringPtr> m_tempFileNames;

  hkStringPtr m_tempOriginalFile;

  hkStringPtr m_stepSourceFile;
  hkStringPtr m_stepTargetFile;

  hkStringPtr m_targetFile;

private:
  hkvPropertyList m_extraMetadata;
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
