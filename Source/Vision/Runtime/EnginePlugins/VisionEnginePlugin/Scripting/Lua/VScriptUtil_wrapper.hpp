/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptRenderer_wrapper.hpp

#ifndef __VSCRIPTUTIL_WRAPPER_HPP
#define __VSCRIPTUTIL_WRAPPER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptConstants.hpp>

/// 
/// \brief
///   Small wrapper to unify utility functions inside the scripting binding
/// 
class VScriptUtil_wrapper
{
public:
  VScriptUtil_wrapper() {}
  ~VScriptUtil_wrapper() {}

  inline int GetRandInt(int iMaxRand = VRAND_MAX) 
  { 
    VASSERT_MSG(iMaxRand>=-VRAND_MAX && iMaxRand<=VRAND_MAX, "You exceeded RAND_MAX, random number will be invalid!");

    if(iMaxRand>0)      return Vision::Game.GetRand()%iMaxRand;
    else if(iMaxRand<0) return -Vision::Game.GetRand()%iMaxRand;
 
    return 0;
  }

  inline int GetRandIntSym(int iMaxRand = VRAND_MAX_2) 
  { 
    VASSERT_MSG(iMaxRand>=-VRAND_MAX_2 && iMaxRand<=VRAND_MAX_2, "Your number range exceeded RAND_MAX, random number will be invalid!");
    int iRange = iMaxRand<<1; //multiply by 2

    if(iMaxRand>0)      return Vision::Game.GetRand()%iRange-iMaxRand;
    else if(iMaxRand<0) return -Vision::Game.GetRand()%iRange-iMaxRand;

    return 0;
  }

  inline float GetRandFloat(float fMaxRand = 1.0f) 
  {
    return ((float)Vision::Game.GetRand())*fMaxRand*VRAND_MAX_INV;
  }

  inline float GetRandFloatSym(float fMaxRand = 1.0f) 
  { 
    return ((float)Vision::Game.GetRand())*fMaxRand*2.f*VRAND_MAX_INV - fMaxRand;
  }

  inline hkvVec3 GetRandVector(float fMaxX, float fMaxY, float fMaxZ)
  {
    return hkvVec3(GetRandFloat(fMaxX),GetRandFloat(fMaxY),GetRandFloat(fMaxZ));
  }

  inline hkvVec3 GetRandVectorSym(float fMaxX, float fMaxY, float fMaxZ)
  {
    return hkvVec3(GetRandFloatSym(fMaxX),GetRandFloatSym(fMaxY),GetRandFloatSym(fMaxZ));
  }

  inline float AngleBetweenVectors(const hkvVec3* pVec1, const hkvVec3* pVec2)
  {
    return ((pVec1 != NULL) && (pVec2 != NULL)) ? pVec1->getAngleBetween(*pVec2) : 0.0f;
  }

  inline hkvMat3 CreateLookAtMatrix(const hkvVec3* pPos, const hkvVec3* pTarget, const hkvVec3* pUp = NULL)
  {
    if(pPos && pTarget)
    {
      hkvMat3 matrix (hkvNoInitialization);

      if(pUp) 
        matrix.setLookInDirectionMatrix (*pTarget - *pPos, *pUp);
      else    
        matrix.setLookInDirectionMatrix (*pTarget - *pPos);

      return hkvMat3(matrix);
    }

    return hkvMat3();
  }

  inline float GetFovX(float fFovY, float fAspectRatio)
  {
    return hkvMathHelpers::getFovX(fFovY, fAspectRatio);
  }

  inline float GetFovY(float fFovX, float fAspectRatio)
  {
    return hkvMathHelpers::getFovY(fFovX, fAspectRatio);
  }

  inline hkvMat3 EulerToMat3_Rad(float fRoll, float fPitch, float fYaw)
  {
    return hkvEulerUtil::ConvertEulerToMat3_Rad(fRoll, fPitch, fYaw);
  }

  inline hkvMat3 EulerToMat3_Deg(float fRoll, float fPitch, float fYaw)
  {
    return hkvEulerUtil::ConvertEulerToMat3_Deg(fRoll, fPitch, fYaw);
  }

  inline hkvMat4 EulerToMat4_Rad(float fRoll, float fPitch, float fYaw)
  {
    return hkvEulerUtil::ConvertEulerToMat4_Rad(fRoll, fPitch, fYaw);
  }

  inline hkvMat4 EulerToMat4_Deg(float fRoll, float fPitch, float fYaw)
  {
    return hkvEulerUtil::ConvertEulerToMat4_Deg(fRoll, fPitch, fYaw);
  }

  inline hkvQuat EulerToQuat_Rad(float fRoll, float fPitch, float fYaw)
  {
    return hkvEulerUtil::ConvertEulerToQuaternion_Rad(fRoll, fPitch, fYaw);
  }

  inline hkvQuat EulerToQuat_Deg(float fRoll, float fPitch, float fYaw)
  {
    return hkvEulerUtil::ConvertEulerToQuaternion_Deg(fRoll, fPitch, fYaw);
  }

  inline hkvVec3 Mat3ToEuler_Rad(const hkvMat3* pMatrix)
  {
    if(pMatrix != NULL)
    {
      float fRoll, fPitch, fYaw;
      hkvEulerUtil::ConvertMat3ToEuler_Rad(*pMatrix, fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
    else
    {
      return hkvVec3(0.0f);
    }
  }

  inline hkvVec3 Mat3ToEuler_Deg(const hkvMat3* pMatrix)
  {
    if(pMatrix != NULL)
    {
      float fRoll, fPitch, fYaw;
      hkvEulerUtil::ConvertMat3ToEuler_Deg(*pMatrix, fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
    else
    {
      return hkvVec3(0.0f);
    }
  }

  inline hkvVec3 Mat4ToEuler_Rad(const hkvMat4* pMatrix)
  {
    if(pMatrix != NULL)
    {
      float fRoll, fPitch, fYaw;
      hkvEulerUtil::ConvertMat4ToEuler_Rad(*pMatrix, fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
    else
    {
      return hkvVec3(0.0f);
    }
  }

  inline hkvVec3 Mat4ToEuler_Deg(const hkvMat4* pMatrix)
  {
    if(pMatrix != NULL)
    {
      float fRoll, fPitch, fYaw;
      hkvEulerUtil::ConvertMat4ToEuler_Deg(*pMatrix, fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
    else
    {
      return hkvVec3(0.0f);
    }
  }

  inline hkvVec3 QuatToEuler_Rad(const hkvQuat* pQuat)
  {
    if(pQuat != NULL)
    {
      float fRoll, fPitch, fYaw;
      hkvEulerUtil::ConvertQuaternionToEuler_Rad(*pQuat, fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
    else
    {
      return hkvVec3(0.0f);
    }
  }

  inline hkvVec3 QuatToEuler_Deg(const hkvQuat* pQuat)
  {
    if(pQuat != NULL)
    {
      float fRoll, fPitch, fYaw;
      hkvEulerUtil::ConvertQuaternionToEuler_Deg(*pQuat, fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
    else
    {
      return hkvVec3(0.0f);
    }
  }

  inline float RadToDeg(float radValue)
  {
    return hkvMath::Rad2Deg(radValue);
  }

  inline float DegToRad(float degValue)
  {
    return hkvMath::Deg2Rad(degValue);
  }

  inline bool IsAngleEqualDeg(float fAngle1, float fAngle2, float fEpsilon = HKVMATH_DEFAULT_EPSILON)
  {
    return hkvMath::isAngleEqualDeg(fAngle1, fAngle2, fEpsilon);
  }

  inline float SqrtInverse(float f)
  {
    return hkvMath::sqrtInverse(f);
  }

  inline float Root(float f, float fNthRoot)
  {
    return hkvMath::root(f, fNthRoot);
  }

  inline bool IsPowerOf(unsigned int x, unsigned int base)
  {
    return hkvMath::isPowerOf(x, base);
  }

  inline bool IsPowerOf2(unsigned int x)
  {
    return hkvMath::isPowerOf2(x);
  }

  inline float Log2(float f)
  {
    return hkvMath::log2(f);
  }

  inline float LogB(float f, float base)
  {
    return hkvMath::logB(f, base);
  }

  inline float Sign(float f)
  {
    return hkvMath::sign<float>(f);
  }

  inline unsigned int PowerOf2_ceil(unsigned int x)
  {
    return hkvMath::powerOf2_ceil(x);
  }

  inline unsigned int PowerOf2_floor(unsigned int x)
  {
    return hkvMath::powerOf2_floor(x);
  }

  inline float Round(float f)
  {
    return hkvMath::round(f);
  }

  inline float Round(float f, unsigned int iDecimalPlaces)
  {
    if (iDecimalPlaces < 1) 
      return hkvMath::round(f);
    
    //otherwise we will exceed our number limits
    iDecimalPlaces = (iDecimalPlaces > 9) ? 9 : iDecimalPlaces;

    double dValue = (double)f;
    double dMultiplier = (double)hkvMath::pow(10.0f, (float)iDecimalPlaces);

    float result = (float)(floor(dValue*dMultiplier + 0.5)/ dMultiplier);
    return result;
  }

  inline float RoundTo(float f, float fRoundTo)
  {
    return hkvMath::roundTo(f, fRoundTo);
  }

  inline float Clamp(float f, float fMin, float fMax)
  {
    return hkvMath::clamp<float>(f, fMin, fMax);
  }

  inline float Saturate(float f)
  {
    return hkvMath::saturate<float>(f);
  }

  inline bool IsInRange(float f, float fMinVal, float fMaxVal)
  {
    return hkvMath::isInRange<float>(f, fMinVal, fMaxVal);
  }

  inline bool IsZero(float f, float fEpsilon = HKVMATH_LARGE_EPSILON)
  {
    return hkvMath::isZero(f, fEpsilon);
  }

  inline bool IsEqual(float f1, float f2, float fEpsilon = HKVMATH_LARGE_EPSILON)
  {
    return hkvMath::isFloatEqual(f1, f2, fEpsilon);
  }

  inline bool IsFiniteNumber(float f)
  {
    return hkvMath::isFiniteNumber(f);
  }

  inline bool IsNaN(float f)
  {
    return hkvMath::isNaN(f);
  }

  inline float Interpolate(float f1, float f2, float fInterpolation)
  {
    return hkvMath::interpolate(f1, f2, fInterpolation);
  }

};

#endif // __VSCRIPTUTIL_WRAPPER_HPP

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
