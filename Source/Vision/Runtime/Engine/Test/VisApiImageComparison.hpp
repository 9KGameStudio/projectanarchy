/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VISAPIIMAGECOMPARISON_HPP_INCLUDED
#define VISAPIIMAGECOMPARISON_HPP_INCLUDED

/// \brief
/// TODO: image comparison not finished yet, part of 2013.3 roadmap (see HS ID#1322)
class VisImageComparison_cl  
{
public:
  VISION_APIFUNC VisImageComparison_cl();
  VISION_APIFUNC virtual ~VisImageComparison_cl();

  /// \brief
  ///   Save a reference image of the current framebuffer content.
  /// 
  /// \param szFileName
  ///   The filename the reference should be saved at.
  ///
  /// \param uiScaleDown
  ///   Scale down factor by which the current framebuffer content is scaled. Using 2 for instance results
  ///   in a resolution change from e.g. 1280x720 to 640x360. Valid range is 1 to 9.
  /// 
  /// \return
  ///   Returns true if successful.
  VISION_APIFUNC bool SaveReferenceImage(const char* szFileName, unsigned int uiScaleDown=1);

  /// \brief
  ///   Calculates a difference image and saves it to file.
  ///   The differeence image is the absulute difference of a reference image and the current framebuffer content.
  /// 
  /// \param szFileName
  ///   The filename of a reference image which should be used.
  ///
  /// \param szOutputFileName
  ///   The filename the difference image should be saved at.
  ///
  /// \param bIncludeSourceImages
  ///   If true, the image stored to file also includes the reference image and the current framebuffer content, otherwise
  ///   the image contains the difference image only.
  ///
  /// \param uiScaleDown
  ///   Scale down factor by which the current framebuffer content is scaled. Using 2 for instance results
  ///   in a resolution change from e.g. 1280x720 to 640x360. Valid range is 1 to 9.
  ///
  /// \return
  ///   Returns true if successful.
  VISION_APIFUNC bool SaveDifferenceImage(const char* szFileName, const char* szOutputFileName, bool bIncludeSourceImages=true, unsigned int uiScaleDown=1);

  /// \brief
  ///   Calculate a difference image and return it as base64 encoded string.
  /// 
  /// \param szFileName
  ///   The filename of a reference image which should be used.
  ///
  /// \param pImageData
  ///   String which is filled with the base64 encoded data.
  ///
  /// \param bIncludeSourceImages
  ///   If ture, the image stored to file also includes the reference image and the current framebuffer content, otherwise
  ///   the image contains the difference image only.
  ///
  /// \param uiScaleDown
  ///   Scale down factor by which the current framebuffer content is scaled. Using 2 for instance results
  ///   in a resolution change from e.g. 1280x720 to 640x360. Valid range is 1 to 9.
  ///
  /// \return
  ///   Returns true if successful.
  VISION_APIFUNC bool GetBase64EncodedDifferenceImage(const char* szFileName, char*& pImageData, bool bIncludeSourceImages=true, unsigned int uiScaleDown=1);

  /// \brief
  ///   Compares a reference image against the current framebuffer content.
  /// 
  /// \param szFileName
  ///   The filename of a reference image which should be used.
  ///
  /// \param iError
  ///   Mean square error of the comparison.
  ///
  /// \param iThreshold
  ///   Error threshold.
  ///
  /// \param uiScaleDown
  ///   Scale down factor by which the current framebuffer content is scaled. Using 2 for instance results
  ///   in a resolution change from e.g. 1280x720 to 640x360. Valid range is 1 to 9.
  ///
  /// \return
  ///   Returns true if mean square error is less equal given threshold.
  VISION_APIFUNC bool CompareImage(const char* szFileName, unsigned int& iError, unsigned int iThreshold=0, unsigned int uiScaleDown=1);

private:
  void ValidateScreenBuffer();
  void GrabScreenBuffer(Image_cl* pImage, unsigned int uiScaleDown);
  bool GetImages(const char* szFileName, Image_cl* pScreenImage, Image_cl* pReferenceImage, unsigned int uiScaleDown);

  bool DifferenceImageInternal(const char* szFileName, Image_cl* pImage, bool bIncludeSourceImages, unsigned int uiScaleDown);
  bool MakeDifferenceImage(const ImageMap_cl* pScreenImage, const ImageMap_cl* pReferenceImage, ImageMap_cl* pDifferenceImage, bool bIncludeSourceImages);

  unsigned int CalculateMeanSquareError(const UBYTE* pRefData, const UBYTE* pData, int iOriginX, int iOriginY, int iWidth, int iHeight, int iStride);
  void EncodeBase64String(VMemoryOutStreamLocal& imageDataStream, char*& pImageData);

  unsigned int m_uiScreenBufferWidth;
  unsigned int m_uiScreenBufferHeight;

  UBYTE* m_pScreenBuffer;
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
