/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VThreadVariable.hpp

#pragma once

// Helper class for VISION_THREADLOCAL_INST using TlsAlloc/TlsFree for DLLs
// because __declspec(thread) doesn't work if the DLL is dynamically loaded 
//
// Original class by Edgar 't Hart based on:
// Using Thread Local Storage in a Dynamic-Link Library
// http://msdn2.microsoft.com/en-us/library/ms686997(VS.85).aspx


/// \brief
///  Helper class for VISION_THREADLOCAL_INST using TlsAlloc/TlsFree for DLLs
///  because __declspec(thread) doesn't work if the DLL is dynamically loaded.
template <class TYPE > class VThreadVariable
{
public:
  /// \brief
  ///   Default constructor
  VThreadVariable ();

  /// \brief
  /// Destructor
  ~VThreadVariable ();
  
  /// \brief
  /// Operator for getting contained type
  operator TYPE();

  /// \brief
  /// Operator for getting contained type
  TYPE operator->();

  /// \brief
  /// Operator for setting contained type
  void operator = ( const TYPE& from );

  bool operator == (const TYPE& other) const;

  bool operator != (const TYPE& other) const;

protected:
  TYPE GetValue() const;
  void SetValue(const TYPE& value);

#ifdef _VISION_POSIX
  pthread_key_t m_key;
  pthread_once_t key_once;
#elif defined(_VISION_WIIU)
  unsigned int m_uiTLSIndex;
#else
  // TLS specific
  DWORD   m_TLSindex;
#endif
};

template <class TYPE>
VThreadVariable<TYPE>::VThreadVariable ()
{
  V_COMPILE_ASSERT(sizeof(TYPE) <= sizeof(void*));

#ifdef _VISION_POSIX
  if (pthread_key_create(&m_key, NULL))
    exit(1);
#elif defined(_VISION_WIIU)
  m_uiTLSIndex = 0xCDCDCDCD;
  if(!VWiiUTLSAlloc(m_uiTLSIndex))
    OSPanic(__FILE__, __LINE__, "Ran out of tls indices!");
#else
  if ((m_TLSindex = TlsAlloc()) == TLS_OUT_OF_INDEXES) 
      exit(1);
#endif
}

template <class TYPE>
VThreadVariable<TYPE>::~VThreadVariable ()
{
#ifdef _VISION_POSIX
  pthread_key_delete(m_key);
#elif defined(_VISION_WIIU)
  VWiiUTLSFree(m_uiTLSIndex);
#else
  TlsFree(m_TLSindex);
  m_TLSindex = -1;
#endif
}

template <class TYPE>
TYPE VThreadVariable<TYPE>::GetValue() const
{
  // Retrieve a data pointer for the current thread
#ifdef _VISION_POSIX
  void* lpvData = pthread_getspecific(m_key);
#elif defined(_VISION_WIIU)
  void* lpvData = VWiiUTLSGet(m_uiTLSIndex);
#else
  void* lpvData = TlsGetValue(m_TLSindex); 
#endif

  return *reinterpret_cast<TYPE*>(&lpvData);
}

template <class TYPE>
void VThreadVariable<TYPE>::SetValue(const TYPE& value)
{
  void* lpvData = reinterpret_cast<void*>(value);

#ifdef _VISION_POSIX
  pthread_setspecific(m_key, lpvData);
#elif defined(_VISION_WIIU)
  VWiiUTLSSet(m_uiTLSIndex, lpvData);
#else
  TlsSetValue(m_TLSindex, lpvData);
#endif
}

template <class TYPE>
inline VThreadVariable<TYPE>::operator TYPE()
{
  return GetValue();
}

template <class TYPE>
inline TYPE VThreadVariable<TYPE>::operator->()
{
  return GetValue();
}

template <class TYPE>
inline void VThreadVariable<TYPE>::operator = ( const TYPE& from )
{
  SetValue(from);
}

template <class TYPE>
inline bool VThreadVariable<TYPE>::operator == (const TYPE& other) const
{
  return other == GetValue();
}

template <class TYPE>
inline bool VThreadVariable<TYPE>::operator != (const TYPE& other) const
{
  return !operator == (other);
}


/// \brief
///  Helper class for using TlsAlloc/TlsFree for DLLs because __declspec(thread) doesn't work if the DLL is dynamically loaded.
///  This class is for buffers of native types.
template<typename TYPE,int N> class VThreadBuffer
{
public:
  /// \brief Constructor. Allocates entry in TLS.
  VThreadBuffer()
  {
    if ( !TAllocate() )
    {
      assert(!"Failed to allocate slot for thread-local value. This may result in unpredictable behavior.");
    }
  }

  /// \brief Destructor. Deallocates entry from TLS.
  ~VThreadBuffer()
  {
    FreeThreadData();
    TDeallocate();
  }
  /// \brief Cast to type operator
  operator TYPE* ()
  {
    return CheckForAllocation();
  }

protected:
  /// \brief Called to release data and set 0 to TLS entry.
  void FreeThreadData()
  {
    // Retrieve a data pointer for the current thread
    TYPE* lpvData = TGetValue();
    if ( lpvData != NULL )
    {
      delete [] lpvData;
      lpvData = NULL;
    }
    TSetValue( lpvData );
  }
  /// \brief Check if the buffer has been created. Creates it if not.
  TYPE* CheckForAllocation()
  {
    // Retrieve a data pointer for the current thread
    TYPE* lpvData = TGetValue();

    // If NULL, allocate memory for the TLS slot for this thread
    if (lpvData == NULL)
    {
      lpvData = new TYPE[N]; 
      if (lpvData == NULL || ! TSetValue(lpvData) )
        return NULL;
    }
    return lpvData;
  }

protected:
  // Depending on if we uses POSIX TLS functions
#ifdef _VISION_POSIX
  pthread_key_t m_key;
  pthread_once_t key_once;

  inline bool  TAllocate()             { return pthread_key_create(&m_key, NULL) == 0; }
  inline void  TDeallocate()           { pthread_key_delete(m_key); }
  inline TYPE* TGetValue()             { return (TYPE*)pthread_getspecific(m_key); }
  inline bool  TSetValue( TYPE* data ) { return pthread_setspecific(m_key, data) == 0; }

#elif defined(_VISION_WIIU)

  unsigned int m_uiTLSIndex;
  inline bool  TAllocate()             { return VWiiUTLSAlloc(m_uiTLSIndex); }
  inline void  TDeallocate()           { VWiiUTLSFree(m_uiTLSIndex); }
  inline TYPE* TGetValue()             { return (TYPE*)VWiiUTLSGet(m_uiTLSIndex); }
  inline bool  TSetValue( TYPE* data ) { return VWiiUTLSSet(m_uiTLSIndex, data); }
#else
  DWORD   m_TLSindex;

  inline bool  TAllocate()             { return (m_TLSindex = TlsAlloc()) != TLS_OUT_OF_INDEXES; }   
  inline void  TDeallocate()           { TlsFree(m_TLSindex); m_TLSindex = -1; }
  inline TYPE* TGetValue()             { return (TYPE*)TlsGetValue(m_TLSindex); }
  inline bool  TSetValue( TYPE* data ) { return TlsSetValue(m_TLSindex, data) != 0; }
#endif
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
