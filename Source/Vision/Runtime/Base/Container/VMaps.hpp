/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMaps.hpp

#ifndef DC_VISION_VMAPS_HPP
#define DC_VISION_VMAPS_HPP

#include <Vision/Runtime/Base/Container/VLink.hpp>

/// \brief Functor class computing the hash for storing items in a VMap.
///
/// The default implementation hashes the memory contents of the object.
template<typename T> struct VHash
{
  UINT operator()(const T& key)
  {
    // compute hash (key must have multiple of 4 size)
    V_COMPILE_ASSERT(sizeof(key)>=sizeof(UINT) && (sizeof(key)%sizeof(UINT)==0));
    const UINT *p = (const UINT *)&key;
    UINT res = 0;
    const int iCount = sizeof(key)/sizeof(UINT);
    for (int i=0;i<iCount;i++)
      res ^= p[i];
    return res;
  }
};

/// \brief VHash overload for pointer types.
template<typename T> struct VHash<T*>
{
  UINT operator()(const T* key)
  {
    // The lowest bits of a pointer are usually zero, so throw them away.
    return static_cast<UINT>(reinterpret_cast<UINT_PTR>(key) >> 4);
  }
};

/// \brief Base class for hashing string types.
///
/// Provides implementations for multiple string types to allow
/// searching the hash map without the overhead of creating a temporary
/// key object.
struct VHashForStringTypes
{
  template<int N>
  UINT operator()(const VStaticString<N>& key)
  {
    return VHashString::GetHash(key.AsChar());
  }

  UINT operator()(const VString& key)
  {
    return VHashString::GetHash(key.AsChar());
  }

  UINT operator()(const char* key)
  {
    return VHashString::GetHash(key);
  }

  UINT operator()(const VHashString& key)
  {
    return key.GetStoredHash();
  }
};

/// \brief
///   Templated hash struct.
template<> struct VHash<VString> : public VHashForStringTypes
{
};

/// \brief
///   Templated hash struct.
template<> struct VHash<VHashString> : public VHashForStringTypes
{
};

/// \brief
///   Templated hash map class.
template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH = VHash<KEY_TYPE> >
class VMap
{
protected:
  /// \brief
  ///   KeyValuePair helper struct
  struct VPair
  {
    VPair* pNext;

    KEY_TYPE key;
    VALUE_TYPE value;
  };

public:
  /// \brief Constructor
  VMap(int nBlockSize = 10);

  /// \brief Copy Constructor
  VMap(const VMap& other);

  /// \brief Destructor
  ~VMap();

  /// \brief Assignment operator
  VMap& operator=(const VMap& other);

  /// \brief Returns the number of elements in the hash map.
  int GetCount() const;

  /// \brief Returns true if there are no elements in the hash map.
  bool IsEmpty() const;

  /// \brief Retrieves an element with the specified key. Returns true if found.
  template<typename LOOKUP_TYPE>
  bool Lookup(const LOOKUP_TYPE& key, VALUE_TYPE& rValue) const;

  /// \brief Returns true if an element matching the specified key was found.
  template<typename LOOKUP_TYPE>
  bool KeyExists(const LOOKUP_TYPE& key) const;

  /// \brief Returns a reference to an object stored with the specified key.
  ///   If no such element exists in the hash map, it is added first.
  template<typename LOOKUP_TYPE>
  VALUE_TYPE& operator[](const LOOKUP_TYPE& key);

  /// \brief Sets the value at the specified key.
  template<typename LOOKUP_TYPE>
  void SetAt(const LOOKUP_TYPE& key, const VALUE_TYPE& newValue) { (*this)[key] = newValue; }

  /// \brief Removes the element that matches the specified key. Returns true if an element was deleted.
  template<typename LOOKUP_TYPE>
  bool RemoveKey(const LOOKUP_TYPE& key);

  /// \brief Removes all elements that map to the given value. Returns the number of removed elements.
  int RemoveValue(const VALUE_TYPE& value);

  /// \brief Removes all elements and deallocates the internal storage. Same as Reset.
  void RemoveAll();

  /// \brief Removes all elements and deallocates the internal storage. Same as RemoveAll.
  void Reset() {RemoveAll();}

  /// \brief Removes all elements, but does not deallocate the internal storage.
  void Clear();

  /// \brief Returns an iterator that can be used to iterate over the entire map.
  VPOSITION GetStartPosition() const { return (m_nCount == 0) ? NULL : VBEFORE_START_POSITION; }  

  /// \brief Advances the iterator and retrieves the next pair of key and value.
  void GetNextPair(VPOSITION& rNextPosition, KEY_TYPE& rKey, VALUE_TYPE& rValue) const;

  // advanced features for derived classes
  UINT GetHashTableSize() const;
  void InitHashTable(UINT hashSize, bool bAllocNow = true);

  template<typename LOOKUP_TYPE>
  UINT HashKey(const LOOKUP_TYPE& key) const;

  // Implementation
protected:

  VPair** m_pHashTable;
  UINT m_nHashTableSize;
  int m_nCount;
  VPair* m_pFreeList;
  struct VLink* m_pBlocks;
  int m_nBlockSize;

  VPair* NewPair();
  void FreePair(VPair* , bool bDeallocOnEmpty=true);

  template<typename LOOKUP_TYPE>
  VPair* GetPairAt(const LOOKUP_TYPE& key, UINT&) const;
};

// common map types
typedef VMap<void*, void*> VMapPtrToPtr;
typedef VMap<const void*, void*> VMapCPtrToPtr;
typedef VMap<const void*, const void*> VMapCPtrToCPtr;
typedef VMap<void*, int> VMapPtrToInt;
typedef VMap<const void*, int> VMapCPtrToInt;
typedef VMap<int, void*> VMapIntToPtr;
typedef VMap<int, int> VMapIntToInt;
typedef VMap<void*, unsigned int> VMapPtrToUInt;
typedef VMap<__int64, void*> VMapUniqueIDToPtr;

#include <Vision/Runtime/Base/Container/VMaps.inl>

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
