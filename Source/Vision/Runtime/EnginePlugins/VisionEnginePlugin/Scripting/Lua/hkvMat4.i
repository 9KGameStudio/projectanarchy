/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvMat4
{
public:
  %extend 
  {
    hkvMat4()
    {
      hkvMat4 *pNew = new hkvMat4(hkvNoInitialization);
      pNew->setIdentity();
      return pNew;
    }

    hkvMat4(const hkvMat4& rhs)
    {
      return new hkvMat4(rhs);
    } 

    hkvMat4(float c0r0, float c1r0, float c2r0, float c3r0, float c0r1, float c1r1, float c2r1, float c3r1,
            float c0r2, float c1r2, float c2r2, float c3r2, float c0r3, float c1r3, float c2r3, float c3r3, 
            bool bUseColumnMajor = false)
    {
      return new hkvMat4(c0r0, c1r0, c2r0, c3r0, c0r1, c1r1, c2r1, c3r1,
                         c0r2, c1r2, c2r2, c3r2, c0r3, c1r3, c2r3, c3r3,
                         (bUseColumnMajor ? hkvMat4::ColumnMajor : hkvMat4::RowMajor));
    }
  }

  hkvMat4(const hkvMat3& m3x3, const hkvVec3& vTranslation);

  void setZero();
  void setIdentity();
  void set(const hkvMat3& m3x3, const hkvVec3& vTranslation);

  %extend 
  {
    void set(float c0r0, float c1r0, float c2r0, float c3r0, float c0r1, float c1r1, float c2r1, float c3r1,
             float c0r2, float c1r2, float c2r2, float c3r2, float c0r3, float c1r3, float c2r3, float c3r3, 
             bool bUseColumnMajor = false)
    {
      self->set(c0r0, c1r0, c2r0, c3r0, c0r1, c1r1, c2r1, c3r1,
                c0r2, c1r2, c2r2, c3r2, c0r3, c1r3, c2r3, c3r3,
                (bUseColumnMajor ? hkvMat4::ColumnMajor : hkvMat4::RowMajor));
    }
  }

  void setRow(unsigned int iRow, const hkvVec4& values);

  %extend
  {
    void setRow(unsigned int iRow, float r0, float r1, float r2, float r3)
    {
      self->setRow(iRow, hkvVec4(r0, r1, r2, r3));
    }
  }

  const hkvVec4 getRow(unsigned int iRow) const;

  void setColumn(unsigned int iColumn, const hkvVec4& values);

  %extend
  {
    void setColumn(unsigned int iColumn, float c0, float c1, float c2, float c3)
    {
      self->setColumn(iColumn, hkvVec4(c0, c1, c2, c3));
    }
  }

  const hkvVec4 getColumn(unsigned int iColumn) const;

  %extend 
  {   
    void setElement(unsigned int iNthElement, float f, bool bUseColumnMajor = true)
    {
      self->setElement(iNthElement, f, (bUseColumnMajor ? hkvMat4::ColumnMajor : hkvMat4::RowMajor));
    }

    float getElement(unsigned int iNthElement, bool bUseColumnMajor = true)
    {
      return self->getElement(iNthElement, (bUseColumnMajor ? hkvMat4::ColumnMajor : hkvMat4::RowMajor));
    }
  }

  void setDiagonal(const hkvVec4& values);
  const hkvVec4 getDiagonal() const;

  void setAxis(unsigned int iAxisXYZ, const hkvVec3& v);

  %extend
  {    
    void setAxis(unsigned int iAxisXYZ, float x, float y, float z)
    {
      self->setAxis(iAxisXYZ, hkvVec3(x, y, z));
    }
  }

  const hkvVec3 getAxis(unsigned int iAxisXYZ) const;

  void setRotationalPart(const hkvMat3& mRot);
  const hkvMat3 getRotationalPart() const;

  void setFromEulerAngles(float fRoll, float fPitch, float fYaw); 

  %extend
  {    
    hkvVec3 getAsEulerAngles() const
    {
      float fRoll, fPitch, fYaw;
      self->getAsEulerAngles(fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw); 
    }
  }

  void setFromQuaternion(const hkvQuat& q);
  hkvQuat getAsQuaternion() const;

  void setScalingMatrix(const hkvVec3& vScaleXYZ); 

  %extend
  {
    void setScalingMatrix(float x, float y, float z)
    {
      self->setScalingMatrix(hkvVec3(x, y, z));
    }
  }

  void setRotationMatrixX(float fDegree);
  void setRotationMatrixY(float fDegree);
  void setRotationMatrixZ(float fDegree);

  %extend
  {
    void setRotationMatrix (const hkvVec3* pAxis, float fDegree)
    {
      if(pAxis != NULL)
        self->setRotationMatrix(pAxis->getNormalized(), fDegree);
    }
  }

  void setTranslationMatrix(const hkvVec3& vTranslation);

  bool isZero(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;
  bool isIdentity(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;
  bool isIdentical(const hkvMat4& rhs) const;
  bool isEqual(const hkvMat4& rhs, float fEpsilon) const;
  bool isValid() const;

  void transpose();
  const hkvMat4 getTransposed() const;

  %extend 
  {   
    bool invert()
    {
      return self->invert()==HKV_SUCCESS;
    }
  }

  const hkvMat4 getInverse() const;

  %extend 
  {   
    bool invertOrthogonal()
    {
      return self->invertOrthogonal()==HKV_SUCCESS;
    }
  }

  const hkvVec3 transformPosition(const hkvVec3& v) const;
  const hkvVec3 transformDirection(const hkvVec3& v) const;
  const hkvVec4 transform(const hkvVec4& v) const;

  void setScalingFactors(const hkvVec3& vXZY);
  hkvVec3 getScalingFactors() const;

  %extend
  {   
    void setScalingFactors(float x, float y, float z)
    {
      self->setScalingFactors(hkvVec3(x, y, z));
    }
  
    bool normalize(float fEpsilon = HKVMATH_SMALL_EPSILON)
    {
      return self->normalize(fEpsilon)==HKV_SUCCESS;
    }
  }

  const hkvVec3 getTranslation() const;
  void setTranslation(const hkvVec3& vTranslation);

  void setLookAtMatrix(const hkvVec3& vStartPos, const hkvVec3& vTargetPos, const hkvVec3& vUpDir = hkvVec3 (0, 0, 1));

  %extend 
  { 
    void setPerspectiveProjectionMatrix(float fViewWidth, float fViewHeight, float fNearZ, float fFarZ)
    {
      self->setPerspectiveProjectionMatrix(fViewWidth, fViewHeight, fNearZ, fFarZ);
    }

    void setPerspectiveProjectionMatrix(float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ)
    {
      self->setPerspectiveProjectionMatrix(fLeft, fRight, fBottom, fTop, fNearZ, fFarZ);
    }

    void setPerspectiveProjectionMatrixFromFovX(float fFieldOfViewX, float fAspectRatioWidthDivHeight, float fNearZ, float fFarZ)
    {
      self->setPerspectiveProjectionMatrixFromFovX(fFieldOfViewX, fAspectRatioWidthDivHeight, fNearZ, fFarZ);
    }

    void setPerspectiveProjectionMatrixFromFovY(float fFieldOfViewY, float fAspectRatioWidthDivHeight, float fNearZ, float fFarZ)
    {
      self->setPerspectiveProjectionMatrixFromFovY(fFieldOfViewY, fAspectRatioWidthDivHeight, fNearZ, fFarZ);
    }

    void setOrthographicProjectionMatrix(float fViewWidth, float fViewHeight, float fNearZ, float fFarZ)
    {
      self->setOrthographicProjectionMatrix(fViewWidth, fViewHeight, fNearZ, fFarZ);
    }

    void setOrthographicProjectionMatrix(float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ)
    {
      self->setOrthographicProjectionMatrix(fLeft, fRight, fBottom, fTop, fNearZ, fFarZ);
    }     
  }

  %extend 
  { 
    hkvMat4 clone() const
    {
      return *self; //will call the copy constructor
    }
  }

  const hkvMat4 multiply(const hkvMat4& rhs) const;
  const hkvMat4 multiplyReverse(const hkvMat4& rhs) const;

  %extend 
  { 
    bool __eq(const hkvMat4* pRhs) const
    {
      if (pRhs == NULL) 
        return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }
    
    hkvMat4 __add (const hkvMat4* pRhs)
    {
      if (pRhs == NULL)
        return *self;
      return (*self + (*pRhs));
    }

    hkvMat4 __sub(const hkvMat4* pRhs)
    {
      if (pRhs == NULL) 
        return *self;
      return (*self - (*pRhs));
    }
    
    /*
    hkvMat4 __mul(const hkvMat4* pRhs)
    {
      %#ifdef HKVMATH_ENABLE_NEW_OPERATORS
        if (pRhs == NULL) 
          return *self;
        return (*self * (*pRhs));
      %#else
        hkvLog::Info("Warning: Calling 'hkvMat4 operator*(const hkvMat4& lhs, const hkvMat4& rhs)' without enabling 'HKVMATH_ENABLE_NEW_OPERATORS'!");
        return *self;
      %#endif
    }
    */

    hkvVec3 __mul(const hkvVec3* pRhs)
    {
      if (pRhs == NULL) 
        return hkvVec3();
      return (*self * (*pRhs));
    }
    
    hkvVec4 __mul(const hkvVec4* pRhs)
    {
      if (pRhs == NULL) 
        return hkvVec4();
      return (*self * (*pRhs));
    }

    hkvMat4 __mul (float rhs)
    {
      return (*self * rhs);
    }
    
    hkvMat4 __div (float rhs)
    {
      return (*self / rhs);
    }
    
    float __getitem__(int iElem)
    {
      if((iElem >= 0) && (iElem < 16))  
        return self->m_ElementsCM[iElem];
      return 0;
    }
    
    void __setitem__(int iElem, float fValue)
    {
      if((iElem >= 0) && (iElem < 16))      
        self->m_ElementsCM[iElem] = fValue;
    }
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvMat4, 256, "[%1.2f,%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f,%1.2f]", self->m_ElementsCM[0], self->m_ElementsCM[1], self->m_ElementsCM[2], self->m_ElementsCM[3], self->m_ElementsCM[4], self->m_ElementsCM[5], self->m_ElementsCM[6], self->m_ElementsCM[7], self->m_ElementsCM[8], self->m_ElementsCM[9], self->m_ElementsCM[10], self->m_ElementsCM[11], self->m_ElementsCM[12], self->m_ElementsCM[13], self->m_ElementsCM[14], self->m_ElementsCM[15])
VSWIG_CREATE_TOSTRING(hkvMat4, "hkvMat4 CM: c0[%1.2f,%1.2f,%1.2f,%1.2f] c1[%1.2f,%1.2f,%1.2f,%1.2f] c2[%1.2f,%1.2f,%1.2f,%1.2f] c3[%1.2f,%1.2f,%1.2f,%1.2f]", self->m_ElementsCM[0], self->m_ElementsCM[1], self->m_ElementsCM[2], self->m_ElementsCM[3], self->m_ElementsCM[4], self->m_ElementsCM[5], self->m_ElementsCM[6], self->m_ElementsCM[7], self->m_ElementsCM[8], self->m_ElementsCM[9], self->m_ElementsCM[10], self->m_ElementsCM[11], self->m_ElementsCM[12], self->m_ElementsCM[13], self->m_ElementsCM[14], self->m_ElementsCM[15])

#else

/// \brief 
///   Wrapper for the hkvMat4 4x4 matrix that can be used to represent rotations, scalings, translations as well as projections.
///   The elements are stored in column-major order. 
///   That means first the data of column 0 is stored, then column 1, column 2, then column 3.
///
/// \note
///   Since all wrappers classes are handled as pointers in Lua you
///   cannot copy its content by assignment. Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new matrix:
///     local m1 = Vision.hkvMat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1)
///
///     -- this will not copy the content of m1 (just another pointer to it):
///     local m2 = m1
///
///     -- this will affect access to m1 and m2 since the pointers share the same object:
///     m2[5] = 3
///     -- this assertion will not fail
///     Vision.Assert(m1 == m2, "When this assertions fails 'm1' and 'm2' are pointing to different matrix.")
///
///     -- in order to copy the content to another matrix, you have to clone:
///     local m3 = m1:clone()
///
///     -- or create a new matrix using the copy constructor
///     local m4 = Vision.hkvMat4(m1)
///
///   \endcode
/// \see clone
class hkvMat4
{
public:

  /// @name Initialization
  /// @{
  
  /// \brief 
  ///   Default Constructor. Will initialize matrix to identity.
  hkvMat4();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The matrix to copy.
  hkvMat4(hkvMat4 other);

  /// \brief
  ///   Constructor with element initialization.
  ///
  /// \note 
  ///   Internally data is stored in column-major format.
  ///   If you set the columnMajor parameter to false, the other parameters are considered
  ///   to be in row-major order (the expected default). This allows a 'natural' representation in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  ///
  /// \param c0r0
  ///   This is the value for column 0, row 0.
  ///
  /// \param c1r0
  ///   If layout is RowMajor, this is the value for column 1, row 0.
  ///   If layout is ColumnMajor, this is the value for row 1, column 0.
  ///
  /// \param c2r0
  ///   If layout is RowMajor, this is the value for column 2, row 0.
  ///   If layout is ColumnMajor, this is the value for row 2, column 0.
  ///
  /// \param c3r0
  ///   If layout is RowMajor, this is the value for column 3, row 0.
  ///   If layout is ColumnMajor, this is the value for row 3, column 0.
  ///
  /// \param c0r1
  ///   If layout is RowMajor, this is the value for column 0, row 1.
  ///   If layout is ColumnMajor, this is the value for row 0, column 1.
  ///
  /// \param c1r1
  ///   This is the value for column 1, row 1.
  ///
  /// \param c2r1
  ///   If layout is RowMajor, this is the value for column 2, row 1.
  ///   If layout is ColumnMajor, this is the value for row 2, column 1.
  ///
  /// \param c3r1
  ///   If layout is RowMajor, this is the value for column 3, row 1.
  ///   If layout is ColumnMajor, this is the value for row 3, column 1.
  ///
  /// \param c0r2
  ///   If layout is RowMajor, this is the value for column 0, row 2.
  ///   If layout is ColumnMajor, this is the value for row 0, column 2.
  ///
  /// \param c1r2
  ///   If layout is RowMajor, this is the value for column 1, row 2.
  ///   If layout is ColumnMajor, this is the value for row 1, column 2.
  ///
  /// \param c2r2
  ///   This is the value for column 2, row 2.
  ///
  /// \param c3r2
  ///   If layout is RowMajor, this is the value for column 3, row 2.
  ///   If layout is ColumnMajor, this is the value for row 3, column 2.
  ///
  /// \param c0r3
  ///   If layout is RowMajor, this is the value for column 0, row 3.
  ///   If layout is ColumnMajor, this is the value for row 0, column 3.
  ///
  /// \param c1r3
  ///   If layout is RowMajor, this is the value for column 1, row 3.
  ///   If layout is ColumnMajor, this is the value for row 1, column 3.
  ///
  /// \param c2r3
  ///   If layout is RowMajor, this is the value for column 2, row 3.
  ///   If layout is ColumnMajor, this is the value for row 2, column 3.
  ///
  /// \param c3r3
  ///   This is the value for column 3, row 3.
  ///
  /// \param columnMajor (\b optional) 
  ///   Set to true to use column-major, false for row-major (default).
  ///
  /// \par Example
  ///   \code
  ///     -- create a new hkvMat4 looking like this:
  ///     -- 1, 1, 1, 1,
  ///     -- 2, 2, 2, 2,
  ///     -- 3, 3, 3, 3,
  ///     -- 4, 4, 4, 4
  ///     local mat = Vision.hkvMat4(1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4); -- create the matrix using row-major
  ///   \endcode
  hkvMat4(number c0r0, number c1r0, number c2r0, number c3r0, number c0r1, number c1r1, number c2r1, number c3r1,
          number c0r2, number c1r2, number c2r2, number c3r2, number c0r3, number c1r3, number c2r3, number c3r3, 
          boolean columnMajor = false);

  /// \brief
  ///   Creates a matrix by combining a 3x3 matrix and a translation vector.
  hkvMat4(hkvMat3 m3x3, hkvVec3 translation);

  /// \brief
  ///   Sets the matrix to all zero.
  void setZero();

  /// \brief
  ///   Sets the matrix to the identity matrix.
  void setIdentity();

  /// \brief
  ///   Creates a matrix by combining a 3x3 matrix and a translation vector.
  void set(hkvMat3 m3x3, hkvVec3 vTranslation);

  /// \brief
  ///   Set all elements of the matrix.
  ///
  /// \note 
  ///   Internally data is stored in column-major format.
  ///   If you set the columnMajor parameter to false, the other parameters are considered
  ///   to be in row-major order (the expected default). This allows a 'natural' representation in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  ///
  /// \param c0r0
  ///   This is the value for column 0, row 0.
  ///
  /// \param c1r0
  ///   If layout is RowMajor, this is the value for column 1, row 0.
  ///   If layout is ColumnMajor, this is the value for row 1, column 0.
  ///
  /// \param c2r0
  ///   If layout is RowMajor, this is the value for column 2, row 0.
  ///   If layout is ColumnMajor, this is the value for row 2, column 0.
  ///
  /// \param c3r0
  ///   If layout is RowMajor, this is the value for column 3, row 0.
  ///   If layout is ColumnMajor, this is the value for row 3, column 0.
  ///
  /// \param c0r1
  ///   If layout is RowMajor, this is the value for column 0, row 1.
  ///   If layout is ColumnMajor, this is the value for row 0, column 1.
  ///
  /// \param c1r1
  ///   This is the value for column 1, row 1.
  ///
  /// \param c2r1
  ///   If layout is RowMajor, this is the value for column 2, row 1.
  ///   If layout is ColumnMajor, this is the value for row 2, column 1.
  ///
  /// \param c3r1
  ///   If layout is RowMajor, this is the value for column 3, row 1.
  ///   If layout is ColumnMajor, this is the value for row 3, column 1.
  ///
  /// \param c0r2
  ///   If layout is RowMajor, this is the value for column 0, row 2.
  ///   If layout is ColumnMajor, this is the value for row 0, column 2.
  ///
  /// \param c1r2
  ///   If layout is RowMajor, this is the value for column 1, row 2.
  ///   If layout is ColumnMajor, this is the value for row 1, column 2.
  ///
  /// \param c2r2
  ///   This is the value for column 2, row 2.
  ///
  /// \param c3r2
  ///   If layout is RowMajor, this is the value for column 3, row 2.
  ///   If layout is ColumnMajor, this is the value for row 3, column 2.
  ///
  /// \param c0r3
  ///   If layout is RowMajor, this is the value for column 0, row 3.
  ///   If layout is ColumnMajor, this is the value for row 0, column 3.
  ///
  /// \param c1r3
  ///   If layout is RowMajor, this is the value for column 1, row 3.
  ///   If layout is ColumnMajor, this is the value for row 1, column 3.
  ///
  /// \param c2r3
  ///   If layout is RowMajor, this is the value for column 2, row 3.
  ///   If layout is ColumnMajor, this is the value for row 2, column 3.
  ///
  /// \param c3r3
  ///   This is the value for column 3, row 3.
  ///
  /// \param columnMajor (\b optional) 
  ///   Set to true to use column-major, false for row-major (default).
  void set(number c0r0, number c1r0, number c2r0, number c3r0, number c0r1, number c1r1, number c2r1, number c3r1,
           number c0r2, number c1r2, number c2r2, number c3r2, number c0r3, number c1r3, number c2r3, number c3r3, 
           boolean columnMajor = false);

  /// @}
  /// @name Element Manipulation
  /// @{

  /// \brief 
  ///   Sets all the values in the specified row using a vector.
  ///
  /// \param rowIndex 
  ///   The row to set: 0, 1, 2 or 3
  ///
  /// \param row 
  ///   The vector with the row data.
  void setRow(number rowIndex, hkvVec4 row);

  /// \brief 
  ///   Sets all the values in the specified row.
  ///
  /// \param rowIndex 
  ///   The row to set: 0, 1, 2 or 3
  ///
  /// \param c0 
  ///   Data for the specified row and colum 0.
  ///
  /// \param c1 
  ///   Data for the specified row and colum 1.
  ///
  /// \param c2 
  ///   Data for the specified row and colum 2.
  ///
  /// \param c3 
  ///   Data for the specified row and colum 3.
  void setRow(number rowIndex, number c0, number c1, number c2, number c3);

  /// \brief 
  ///   Returns all the values in the specified row.
  ///
  /// \param rowIndex 
  ///   The row to get: 0, 1, 2 or 3
  ///
  /// \return 
  ///   A vector with the row values.
  hkvVec4 getRow(number rowIndex);

  /// \brief 
  ///   Sets all the values in the specified column using a vector.
  ///
  /// \param colIndex 
  ///   The column to set: 0, 1, 2 or 3.
  ///
  /// \param col 
  ///   The vector with the column data.
  void setColumn(number colIndex, hkvVec4 col);

  /// \brief 
  ///   Sets all the values in the specified column.
  /// 
  /// \param colIndex
  ///   The column to set: 0, 1, 2 or 3.
  ///
  /// \param r0 
  ///   Data for the specified column and row 0.
  ///
  /// \param r1 
  ///   Data for the specified column and row 1.
  ///
  /// \param r2 
  ///   Data for the specified column and row 2.
  ///
  /// \param r3
  ///   Data for the specified column and row 3.
  void setColumn(number colIndex, number r0, number r1, number r2, number r3);

  /// \brief 
  ///   Returns all the values in the specified column.
  ///
  /// \param colIndex 
  ///   The column to get: 0, 1, 2 or 3.
  ///
  /// \return 
  ///   A vector with the column values.
  hkvVec4 getColumn(number colIndex);

  /// \brief 
  ///   Sets a specific element of the matrix.
  ///
  /// \note 
  ///   This function can be used to access elements in row-major or column-major fashion.
  ///   That can be very useful when working with other libraries that have other conventions.
  ///   You can keep the indices identical, and just need to specify, that it is meant to be a row-major
  ///   or column-major index. Which element exactly is accessed, will then be handled internally.
  ///
  /// \param element
  ///   The index, which element should be accessed. If layout is ColumnMajor, this is like accessing m_ElementsCM
  ///   which is a direct array lookup.
  ///   If layout is RowMajor, the index is interpreted as if it is meant for a matrix that is stored in row-major order.
  ///   Since this class stores its data in column-major order, the index is recalculated to access the proper element.
  ///
  /// \param value
  ///   The value to be set at the specified element index.
  ///
  /// \param columnMajor (\b optional) 
  ///   Set to true (default) to access the matrix in colum-major, use false for row-major.
  void setElement(number element, number value, boolean columnMajor = true);

  /// \brief 
  ///   Returns a specific element from the matrix.
  ///
  /// \note 
  ///   This function can be used to access elements in row-major or column-major fashion.
  ///   That can be very useful when working with other libraries that have other conventions.
  ///   You can keep the indices identical, and just need to specify, that it is meant to be a row-major
  ///   or column-major index. Which element exactly is accessed, will then be handled internally.
  ///
  /// \param element
  ///   The index, which element should be accessed. If layout is ColumnMajor, this is like an array lookup.
  ///   If layout is RowMajor, the index is interpreted as if it is meant for a matrix that is stored in row-major order.
  ///   Since this class stores its data in column-major order, the index is recalculated to access the proper element.
  ///
  /// \param columnMajor (\b optional) 
  ///   Set to true (default) to access the matrix in colum-major, use false for row-major.
  ///
  /// \return 
  ///   The value of the requested element.
  number getElement(number element, boolean columnMajor = true);

  /// \brief
  ///   Sets all the values on the matrices diagonal.
  void setDiagonal(hkvVec4 values);

  /// \brief
  ///   Returns all the values on the matrices diagonal.
  hkvVec4 getDiagonal();

  /// \brief
  ///   Sets the x,y,z values as hkvVec3 on the specified axis.
  ///
  /// \param axis 
  ///   The axis to set: 0, 1 or 2.
  ///
  /// \param vector 
  ///   The new axis values as vector.
  void setAxis(number axis, hkvVec3 vector);

  /// \brief 
  ///   Sets the x,y,z values on the specified axis.
  ///
  /// \param axis 
  ///   The axis to set: 0, 1 or 2.
  ///
  /// \param x 
  ///   The x axis component.
  ///
  /// \param y 
  ///   The y axis component.
  ///
  /// \param z 
  ///   The z axis component.
  void setAxis(number axis, number x, number y, number z);

  /// \brief
  ///   Returns the x,y,z values on the specified axis as hkvVec3.
  ///
  /// \param axis 
  ///   The axis to get: 0, 1 or 2.
  ///
  /// \return 
  ///   A hkvVec3 representing the requested axis.
  hkvVec3 getAxis(number axis);

  /// \brief
  ///   Returns the 3x3 part of the 4x4 matrix, which represents rotation and scaling.
  hkvMat3 getRotationalPart();

  /// \brief
  ///   Sets the 3x3 part of the 4x4 matrix, which represents rotation and scaling. Leaves all other data unchanged.
  void setRotationalPart(hkvMat3 mRot);

  /// @}
  /// @name Creating Matrices
  /// @{
  
  /// \brief
  ///   Sets this matrix to a rotation matrix defined by the given euler angles (in degrees).
  void setFromEulerAngles(number roll, number pitch, number yaw);

  /// \brief
  ///   Returns the euler angles (in degrees) which represent the rotation which is stored in the 3x3 part of this matrix.
  /// 
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw.
  hkvVec3 getAsEulerAngles();

  /// \brief
  ///   Sets this matrix to a rotation matrix which represents the same rotation as the given quaternion.
  void setFromQuaternion(hkvQuat q);

  /// \brief
  ///   Returns a quaternion which represent the rotation which is stored in the 3x3 part of this matrix.
  hkvQuat getAsQuaternion();

  /// \brief 
  ///   Transforms this matrix into a scaling matrix.
  ///
  /// \param scale 
  ///   A vector holding the scaling values for x,y and z axis.
  void setScalingMatrix(hkvVec3 scale);
  
  /// \brief 
  ///   Transforms this matrix into a scaling matrix.
  ///
  /// \param x 
  ///   Scaling values for x axis.
  ///
  /// \param y 
  ///   Scaling values for y axis.
  ///
  /// \param z 
  ///   Scaling values for z axis.
  void setScalingMatrix(number x, number y, number z);

  /// \brief 
  ///   Sets this matrix to a matrix that rotates objects around the X axis.
  ///
  /// \param degrees 
  ///   The degrees to rotate.
  void setRotationMatrixX(number degrees);
  
  /// \brief 
  ///   Sets this matrix to a matrix that rotates objects around the Y axis.
  ///
  /// \param degrees 
  ///   The degrees to rotate.
  void setRotationMatrixY(number degrees);
  
  /// \brief 
  ///   Sets this matrix to a matrix that rotates objects around the Z axis.
  ///
  /// \param degrees
  ///   The degrees to rotate.
  void setRotationMatrixZ(number degrees);
  
  /// \brief 
  ///   Sets this matrix to a matrix that rotates objects around an arbitrary specified axis.
  ///
  /// \note
  ///   The matrix will be completely reset, so if it had any scaling or translation beforehand, this will be gone.
  ///
  /// \param axis 
  ///   The rotation axis. Axis gets internally normalized (\b always).
  ///
  /// \param degrees 
  ///   The degrees to rotate.
  void setRotationMatrix(hkvVec3 axis, number degrees);

  /// \brief
  ///   Creates a pure translation matrix.
  ///
  /// \param translation
  ///   The translation vector.
  void setTranslationMatrix(hkvVec3 translation);

  /// @}
  /// @name Checks
  /// @{
  
  /// \brief 
  ///   Checks whether this matrix is all zero (within some epsilon range).
  ///
  /// \param epsilon (\b optional)
  ///   Specify an epsilon for this range (default is Vision.MATH_DEFAULT_EPSILON).
  ///
  /// \return
  ///   True if zero within the specified epsilon, otherwise false.
  boolean isZero(number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief 
  ///   Checks whether this matrix is the identity (within some epsilon).
  ///
  /// \param epsilon (\b optional) 
  ///   Specify an epsilon for this range (default is Vision.MATH_DEFAULT_EPSILON).
  ///
  /// \return 
  ///   True if proven as identity matrix (within the epsilon), otherwise false.
  boolean isIdentity(number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief 
  ///   Checks whether the two matrices are identical.
  ///
  /// \param other 
  ///   The matrix to check.
  ///
  /// \return 
  ///   True if identical (without epsilon), otherwise false.
  boolean isIdentical(hkvMat3 other);

  /// \brief 
  ///   Checks whether the two matrices are equal (within some epsilon range).
  ///
  /// \param other 
  ///   The matrix to check.
  ///
  /// \param epsilon
  ///   Specify an epsilon for this range (e.g Vision.MATH_LARGE_EPSILON).
  ///    
  /// \return 
  ///   True if equal within the given epsilon, otherwise false.
  boolean isEqual(hkvMat3 other, number epsilon);

  /// \brief
  ///   Checks that no element is NaN or infinity.
  boolean isValid();  

  /// @} 
  /// @name Utility Methods
  /// @{

  /// \brief 
  ///   Transposes the matrix.
  void transpose();

  /// \brief 
  ///   Gets a copy of a transposed matrix.
  ///
  /// \return 
  ///   The transposed matrix.
  hkvMat4 getTransposed();

  /// \brief 
  ///   Inverts this matrix. Works for all (mathematically) invertible matrices.
  ///
  /// \note 
  ///   Will do a full inversion of the whole matrix. Can invert all types of matrices, that
  ///   are mathematically invertible. A matrix multiplied with its inverse will always yield the
  ///   identity (within some epsilon range). If the matrix could not be inverted the values of
  ///   the matrix are undefined.
  ///
  /// \return 
  ///   True if operation was successful, otherwise false (in this case the matrix values are undefined!).
  boolean invert(); 

  /// \brief
  ///   Returns an inverted copy of this matrix.
  ///
  /// Internally this creates a copy of the matrix and then calls hkvMat3::invert on that.
  /// As such it has the same limitations when the inversion fails.
  hkvMat4 getInverse();

  /// \brief
  ///   Inverts the matrix. Only works on orthogonal and uniformly scaled matrices. Faster than 'invert'.
  ///
  /// \return
  ///   True if operation was successful, otherwise false.
  boolean invertOrthogonal();

  /// \brief
  ///   Returns a transformed copy of v, assuming v to be a point.
  ///
  /// This function uses 1 as the w-component of v and then multiplies v with this matrix.
  /// The result is that the translation part of this matrix is added to the resulting vector,
  /// which means the point is not only rotated but also moved around.
  /// This is the default method of transformation.
  /// 
  /// \param v
  ///   The position vector to be transformed.
  hkvVec3 transformPosition(hkvVec3 v);

  /// \brief
  ///   Returns a transformed copy of v, assuming v to be a direction vector.
  ///
  /// This function uses 0 as the w-component of v and then multiplies v with this matrix.
  /// The result is that the translation part of this matrix is ignored,
  /// which means that v is only rotated and scaled but not moved around.
  ///
  /// \param v
  ///   The direction vector to be transformed.
  hkvVec3 transformDirection(hkvVec3 v) const;

  /// \brief
  ///   Transforms a 4 component vector.
  hkvVec4 transform(hkvVec4 v);

  /// \brief 
  ///   Sets the scaling of the x,y,z axis.
  ///
  /// \param scaling 
  ///   The scaling values of all axes as vector.
  ///
  /// \param epsilon
  ///   Epsilon range to test against zero matrix.
  ///
  ///  \return
  ///    False if the scaling could not be set, because the matrix is too close to a zero matrix.
  boolean setScalingFactors(hkvVec3 scaling, number epsilon = MATH_SMALL_EPSILON);

  /// \brief
  ///   Sets the scaling of the x,y,z axis.
  ///
  /// \param x 
  ///   Scaling along the x axis.
  ///
  /// \param y 
  ///   Scaling along the y axis.
  ///
  /// \param z
  ///   Scaling along the z axis.
  ///
  /// \param epsilon
  ///   Epsilon range to test against zero matrix.
  ///
  ///  \return
  ///    False if the scaling could not be set, because the matrix is too close to a zero matrix.
  boolean setScalingFactors(number x, number y, number z, number epsilon = MATH_SMALL_EPSILON);

  /// \brief
  ///   Returns the scaling of the x,y,z axis.
  hkvVec3 getScalingFactors();

  /// \brief
  ///   Normalizes each axis (ie. the 3x3 rotational part of the matrix).
  ///
  /// Returns false if the matrix is too close to a zero matrix and could not be normalized
  /// or if any value is invalid (ie. NaN or +/- infinity).
  /// Does not modify the matrix in case of failure.
  ///
  /// \param epsilon
  ///   If the length of an axis is below this value, normalization fails.
  ///
  /// \return
  ///   True if normalization succeeds, false otherwise.
  boolean normalize(number epsilon = MATH_SMALL_EPSILON);

  /// \brief
  ///   Returns the translational part of the matrix.
  hkvVec3 getTranslation();

  /// \brief
  ///   Sets the translational part of the matrix.
  void setTranslation(hkvVec3 translation);

  /// \brief
  ///   Creates a 'look-at' matrix, which will transform objects such that they are located at sartPos 'looking at' targetPos.
  ///
  /// The Vision Engine uses by default the X-axis as the 'forward' direction of objects, +Y as 'left' and +Z as 'up'.
  /// In case that lookDir and upDir are nearly equal an arbitrary up vector is used to create a valid matrix.
  ///
  /// \param startPos
  ///   The position at which the object should be located.
  ///
  /// \param targetPos
  ///   The position at which the object should 'look'.
  ///
  /// \param upDir
  ///   An optional vector to describe where 'up' is. By default (0,0,1) is used. If the look direction coincides with upDir,
  ///   an arbitrary other up direction is used, meaning the matrix will be valid, but it will be arbitrary rotated around the look direction.
  void setLookAtMatrix(hkvVec3 startPos, hkvVec3 targetPos, hkvVec3 upDir = hkvVec3 (0, 0, 1));

  /// \brief
  ///   Creates a symmetric perspective projection matrix. See the off-center variant of this function for more detailed documentation.
  void setPerspectiveProjectionMatrix(number viewWidth, number viewHeight, number nearZ, number farZ);

  /// \brief 
  ///   Creates an off-center perspective projection matrix.
  ///
  /// \param left
  ///   Specifies the coordinate for the left vertical clipping plane. For symmetric projection matrices 'left == -right'
  ///
  /// \param right
  ///   Specifies the coordinate for the right vertical clipping plane. For symmetric projection matrices 'left == -right'
  ///
  /// \param bottom
  ///   Specifies the coordinate for the bottom horizontal clipping plane. For symmetric projection matrices 'bottom == -top'
  ///
  /// \param top
  ///   Specifies the coordinate for the top horizontal clipping plane. For symmetric projection matrices 'bottom == -top'
  ///
  /// \param nearZ
  ///   The distance of the camera to the near clipping plane. Please note that for good depth buffer precision the ratio
  ///   of the (farZ / nearZ) should be as low as possible. This is best achieved by pushing the near plane out as much as possible,
  ///   but also make sure to not set the far clipping plane too far out.
  ///   Since the Vision Engine by default uses 1 unit == 1 centimeter, values for nearZ should always be 10 or larger.
  ///
  /// \param farZ
  ///   The distance of the camera to the far clipping plane.
  void setPerspectiveProjectionMatrix(number left, number right, number bottom, number top, number nearZ, number farZ);

  /// \brief
  ///   Creates a symmetric perspective projection matrix. 
  ///
  /// \param fieldOfViewX
  ///   The opening angle of the camera along the X axis, given in degrees.
  ///   For example 45. The opening angle along the Y axis is computed from this value and aspectRatioWidthDivHeight.
  ///
  /// \param aspectRatioWidthDivHeight
  ///   The aspect ratio should match the aspect ratio of the viewport.
  ///   For example, aspectRatioWidthDivHeight = 2.0 means the camera's angle of view is twice as wide in X as it is in Y.
  ///
  /// \param nearZ
  ///   The distance of the camera to the near clipping plane. Please note that for good depth buffer precision the ratio
  ///   of the (farZ / nearZ) should be as low as possible. This is best achieved by pushing the near plane out as much as possible,
  ///   but also make sure to not set the far clipping plane too far out.
  ///   Since the Vision Engine by default uses 1 unit == 1 centimeter, values for nearZ should always be 10 or larger.
  ///
  /// \param farZ
  ///   The distance of the camera to the far clipping plane.
  /// \see hkvMat4::setPerspectiveProjectionMatrix
  void setPerspectiveProjectionMatrixFromFovX(number fieldOfViewX, number aspectRatioWidthDivHeight, number nearZ, number farZ);

  /// \brief
  ///   Creates a symmetric perspective projection matrix. 
  ///
  /// \param fieldOfViewY
  ///   The opening angle of the camera along the Y axis, given in degrees.
  ///   For example 45. The opening angle along the X axis is computed from this value and aspectRatioWidthDivHeight.
  ///
  /// \param aspectRatioWidthDivHeight
  ///   The aspect ratio should match the aspect ratio of the viewport.
  ///   For example, aspectRatioWidthDivHeight = 2.0 means the camera's angle of view is twice as wide in X as it is in Y.
  ///
  /// \param nearZ
  ///   The distance of the camera to the near clipping plane. Please note that for good depth buffer precision the ratio
  ///   of the (farZ / nearZ) should be as low as possible. This is best achieved by pushing the near plane out as much as possible,
  ///   but also make sure to not set the far clipping plane too far out.
  ///   Since the Vision Engine by default uses 1 unit == 1 centimeter, values for nearZ should always be 10 or larger.
  ///
  /// \param farZ
  ///   The distance of the camera to the far clipping plane.
  ///
  /// \see hkvMat4::setPerspectiveProjectionMatrix
  void setPerspectiveProjectionMatrixFromFovY(number fieldOfViewY, number aspectRatioWidthDivHeight, number nearZ, number farZ);

  /// \brief
  ///   Creates a symmetric orthographic projection matrix. See the off-center variant of this function for more detailed documentation.
  void setOrthographicProjectionMatrix(number viewWidth, number viewHeight, number nearZ, number farZ);

  /// \brief 
  ///   Creates an off-center orthographic projection matrix.
  ///
  /// \param left
  ///   Specifies the coordinate for the left vertical clipping plane. For symmetric projection matrices 'left == -right'
  ///
  /// \param right
  ///   Specifies the coordinate for the right vertical clipping plane. For symmetric projection matrices 'left == -right'
  ///
  /// \param bottom
  ///   Specifies the coordinate for the bottom horizontal clipping plane. For symmetric projection matrices 'bottom == -top'
  ///
  /// \param top
  ///   Specifies the coordinate for the top horizontal clipping plane. For symmetric projection matrices 'bottom == -top'
  ///
  /// \param nearZ
  ///   The distance of the camera to the near clipping plane.
  ///   For orthographic projections it is possible to set nearZ to a negative value to position the near plane behind the camera.
  ///   For example in an editor it is common to position the camera somewhere around the origin, but set the clipping planes to
  ///   -1000000 and +1000000 such that everything inside this slab will be rendered.
  ///
  /// \param farZ
  ///   The distance of the camera to the far clipping plane.
  ///
  /// \see hkvMat4::setPerspectiveProjectionMatrix
  VBASE_IMPEXP void setOrthographicProjectionMatrix(number left, number right, number bottom, number top, number nearZ, number farZ);

  /// \brief 
  ///   Clones the current matrix.
  ///
  /// \return 
  ///   An identical hkvMat4.
  ///
  /// \note 
  ///   Since all wrappers classes are handled as pointers in Lua you
  ///   cannot copy its content by assignment. Please take a look at the code samples.
  ///
  /// \par Example
  ///   \code
  ///     -- create a new matrix:
  ///     local m1 = Vision.hkvMat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1)
  ///
  ///     -- this will not copy the content of m1 (just another pointer to it):
  ///     local m2 = m1
  ///
  ///     -- this will affect access to m1 and m2 since the pointers share the same object:
  ///     m2[5] = 3
  ///     -- this assertion will not fail
  ///     Vision.Assert(m1 == m2, "When this assertions fails 'm1' and 'm2' are pointing to different matrices.")
  ///
  ///     -- in order to copy the content to another matrix, you have to clone:
  ///     local m3 = m1:clone()
  ///
  ///     -- this will affect only m3:
  ///     m3[5] = 6
  ///     -- this assertion will not fail
  ///     Vision.Assert(m1 ~= m3, "When this assertions fails 'm1' and 'm3' are pointing to same matrix.")
  ///
  ///     -- instead of clone you can also create a new matrix using the copy constructor:
  ///     local m4 = Vision.hkvMat4(m1)
  ///   \endcode
  hkvMat4 clone();

  /// @}
  /// @name Operators and Math Methods
  /// @{

  /// \brief 
  ///   Returns a matrix that is the result of multiplying this matrix with another matrix (right-hand side).
  ///
  /// \note
  ///   Prefer this function for matrix multiplications. Try not to use the * operator.
  ///   The * operator currently acts like multiplyReverse, since this is the old math classes.
  ///   At some point that operator will become deprecated. You can then use 'multiplyReverse' instead or rewrite formulas to use 'multiply'.
  ///   Therefore when writing new code it is recommended to use function calls instead of overloaded operators, at least for now.
  ///
  /// \param other 
  ///   The right-hand side matrix.
  ///
  /// \return 
  ///   The result of the multiplication.
  ///
  /// \see hkvMat4::multiplyReverse
  hkvMat4 multiply(hkvMat4 other);
  
  /// \brief 
  ///   Returns a matrix that is the result of multiplying another (right-hand side) matrix with this matrix.
  ///
  /// \note 
  ///   Try not to use the * operator.The * operator currently acts like multiplyReverse, since this is the
  ///   old math classes. At some point that operator will become deprecated. You can then use 'multiplyReverse'
  ///   instead or rewrite formulas to use 'multiply'. Therefore when writing new code it is recommended to use
  ///   function calls instead of overloaded operators, at least for now.
  ///
  /// \param other 
  ///   The right-hand side matrix.
  ///
  /// \return 
  ///   The result of the 'reverse' multiplication.
  ///
  /// \see hkvMat4::multiply
  hkvMat4 multiplyReverse(hkvMat4 other);

  /// \brief
  ///   Operator == for hkvMat4 comparison.
  ///
  /// \param other 
  ///   The other matrix to compare with.
  ///
  /// \return 
  ///   True if the matrices are equal within Vision.MATH_DEFAULT_EPSILON, otherwise false.
  boolean __eq(hkvMat4 other);

  /// \brief 
  ///   Operator + for hkvMat4, which adds the two matrices component-wise.
  /// 
  /// \param other 
  ///   The matrix to add.
  ///
  /// \return 
  ///   The result matrix.
  hkvMat4 __add(hkvMat4 other);

  /// \brief 
  ///   Operator - for hkvMat4, which subtracts the two matrices component-wise.
  ///
  /// \param other 
  ///   The matrix to subtract.
  ///
  /// \return 
  ///   The result matrix.
  hkvMat4 __sub(hkvMat4 other);

  /// \brief 
  ///   Operator * to multiply the matrix with a hkvVec3 vector. 
  ///
  /// \param vec 
  ///   The hkvVec3 vector to multiply with.
  ///
  /// \return 
  ///   The result of the multiplication.
  hkvVec3 __mul(hkvVec3 vec);

  /// \brief 
  ///   Operator * to multiply the matrix with a hkvVec4 vector. 
  ///
  /// \param vec 
  ///   The hkvVec4 vector to multiply with.
  ///
  /// \return 
  ///   The result of the multiplication.
  hkvVec4 __mul(hkvVec4 vec);

  /// \brief
  ///   Operator * to multiply each element with a scalar value.
  ///
  /// \param value 
  ///   The scalar value to multiply with.
  ///
  /// \return 
  ///   The result matrix.
  hkvMat4 __mul(number value);

  /// \brief
  ///   Operator / to devide each element by a scalar value.
  ///
  /// \param value
  ///   The scalar value used for the division.
  ///
  /// \return 
  ///   The result matrix.
  hkvMat4 __div(number value);

  /// \brief 
  ///   Returns a specific element from the matrix in column-major (local elem6 = mat[6]).
  ///
  /// \param element 
  ///   The index, which element should be accessed (0 to 15).
  ///
  /// \return 
  ///   The value of the requested element.
  number __getitem(number element);
  
  /// \brief 
  ///   Sets a specific element of the matrix in column-major (mat[6] = 5.343).
  ///
  /// \param element 
  ///   The index, which element should be accessed (0 to 15).
  ///
  /// \param value 
  ///   The value to be set at the specified element index.
  void __setitem(number element, number value);

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
