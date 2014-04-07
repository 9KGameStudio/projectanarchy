/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRingBuffer.hpp

#ifndef VRINGBUFFER_HPP
#define VRINGBUFFER_HPP

  /// \class  VRingBuffer
  ///
  /// \brief  A templated generic ring buffer.
  ///
  template<typename T, unsigned int Size> class VRingBuffer
  {
  public:

    /// \brief  
    ///   Default constructor
    VRingBuffer()
      : m_uiTail(0), m_uiAvailable(0)
    {
    }

    /// \brief  Reads entries from the buffer. 
    ///
    /// \param [out] pTarget The target of the read operation - the values read from the buffer will be stored there.
    /// \param uiCount The maximum number of elements to read.
    ///
    /// \return The number of elements actually read.
    ///
    unsigned int Read(T* pTarget, unsigned int uiCount = 1)
    {
      if (uiCount > 0)
      {
        // Don't read past the end of the array or past the amount of available bytes
        unsigned int uiElementsToRead = hkvMath::Min(hkvMath::Min(uiCount, Size - m_uiTail), m_uiAvailable);

        memcpy(pTarget, m_Buffer + m_uiTail, uiElementsToRead * sizeof(T));

        AdvanceTail(uiElementsToRead);

        // Continue reading in case we need to wrap around the end of the buffer
        return uiElementsToRead + Read(pTarget + uiElementsToRead, uiCount - uiElementsToRead);
      }
      else
      {
        return 0;
      }
    }

    /// \brief  Writes entries to the buffer. 
    ///
    /// \param [in] pSource The source values which will be written into the buffer.
    /// \param uiCount The number of elements to write.
    ///
    /// \return true if it the values could all be written, false if the buffer is full.
    ///
    bool Write(const T* pSource, unsigned int uiCount = 1)
    {
      if (uiCount > 0)
      {
        if (uiCount + m_uiAvailable > Size)
        {
          return false;
        }

        // Don't write past the end of the array
        int uiElementsToWrite = hkvMath::Min(uiCount, Size - GetHead());

        memcpy(m_Buffer + GetHead(), pSource, uiElementsToWrite * sizeof(T));
        AdvanceHead(uiElementsToWrite);

        return Write(pSource + uiElementsToWrite, uiCount - uiElementsToWrite);
      }

      return true;
    }

    /// \brief  Returns the number of unread elements currently in the buffer. 
    ///
    /// \return The count of unread elements currently in the buffer. 
    ///
    unsigned int GetCount()
    {
      return m_uiAvailable;
    }

    /// \brief Returns the size of the buffer
    ///
    /// \return The size of the buffer
    ///
    unsigned int GetSize()
    {
      return Size;
    }

  private:

    void AdvanceTail(unsigned int uiOffset)
    {
      VASSERT(uiOffset <= m_uiAvailable);

      m_uiTail = (m_uiTail + uiOffset) % Size;
      m_uiAvailable -= uiOffset;
    }

    void AdvanceHead(unsigned int uiOffset)
    {
      VASSERT(uiOffset + m_uiAvailable <= Size);

      m_uiAvailable += uiOffset;
    }

    unsigned int GetHead()
    {
      return (m_uiTail + m_uiAvailable) % Size;
    }


    //! The data buffer. 
    T m_Buffer[Size];

    unsigned int m_uiTail;
    unsigned int m_uiAvailable;
  };

  /// \class  VThreadSafeRingBuffer
  ///
  /// \brief  Implementation of a thread safe ring buffer which uses locks to protect the buffer accesses.
  ///
  template<typename T, unsigned int Size> class VThreadSafeRingBuffer : public VRingBuffer<T, Size>
  {
  public:

    /// \brief  Default constructor. 
    VThreadSafeRingBuffer()
      : VRingBuffer<T, Size>(), m_LockMutex()
    {
    }

    /// \brief See: VRingBuffer::Read
    bool Read(T* pTarget)
    {
      VMutexLocker lock(m_LockMutex);
      return VRingBuffer<T, Size>::Read(pTarget);
    }

    /// \brief See: VRingBuffer::Write
    bool Write(T* pSource)
    {
      VMutexLocker lock(m_LockMutex);
      return VRingBuffer<T, Size>::Write(pSource);
    }

  private:

    VMutex m_LockMutex;
  };


  /// \brief
  ///   A simple ring buffer template for accumulation of values.  
  ///
  /// This buffer uses no separate read pointer; Elements can be accessed by index via the array index operator, where the most recently written element is to be found
  /// at the highest index (\c Size - 1).
  ///
  /// \param T
  ///   the type of the elements stored in the buffer
  /// 
  /// \param Size
  ///   the size of the buffer
  template<typename T>
  class VSimpleRingBuffer
  {
  public:
    /// The element type
    typedef T ElementType;

  private:
    /// The elements managed by this buffer
    T *m_pElements;
    /// The size of the buffer
    unsigned int m_iSize;
    /// The pointer denoting the next element to be overwritten
    unsigned int m_iWritePointer;

  public:

    /// \brief
    ///   Performs a zero-initialization of all elements in the buffer.
    VSimpleRingBuffer(unsigned int iSize)
    {
      m_iSize = iSize;
      m_pElements = new T[iSize];
      memset(m_pElements, 0, sizeof(T) * iSize);
      m_iWritePointer = 0;
    }

    /// \brief
    ///   Destroys the buffer.
    ~VSimpleRingBuffer()
    {
      delete[] m_pElements;
    }

    /// \brief
    ///   Adds an element to the buffer.
    void Add(T element)
    {
      m_pElements[m_iWritePointer] = element;
      m_iWritePointer = (++m_iWritePointer % m_iSize);
    }

    /// \brief
    ///   Returns the last element of the buffer.
    ///
    /// \return
    ///   the last element of the buffer. If no elements have been added so far,
    ///   this returns 0.
    T GetLast() const
    {
      return m_pElements[m_iWritePointer ? m_iWritePointer - 1 : m_iSize - 1];
    }

    /// \brief
    ///   Returns the size of this buffer.
    ///
    /// \return
    ///   the size of this buffer
    unsigned int GetSize() const
    {
      return m_iSize;
    }

    /// \brief
    ///   Returns an element from the buffer.
    ///
    /// \param iIndex
    ///   index of the element to get. \c 0 is the oldest element in the
    ///   buffer, Size - 1 is the youngest.
    ///
    /// \return
    ///   The requested element
    T operator[](unsigned int iIndex) const
    {
      iIndex = (m_iWritePointer + iIndex) % m_iSize;
      return m_pElements[iIndex];
    }
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
