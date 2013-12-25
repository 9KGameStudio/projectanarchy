/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline VMap<KEY_TYPE, VALUE_TYPE, HASH>::VMap(int nBlockSize)
  : m_pHashTable(NULL)
  , m_nHashTableSize(17)  // default size
  , m_nCount(0)
  , m_pFreeList(NULL)
  , m_pBlocks(NULL)
{
  ASSERT(nBlockSize > 0);
  m_nBlockSize = nBlockSize;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline VMap<KEY_TYPE, VALUE_TYPE, HASH>::VMap(const VMap& other)
  : m_pHashTable(NULL)
  , m_nHashTableSize(17)  // default size
  , m_nCount(0)
  , m_pFreeList(NULL)
  , m_pBlocks(NULL)
  , m_nBlockSize(other.m_nBlockSize)
{
  operator=(other);
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline int VMap<KEY_TYPE, VALUE_TYPE, HASH>::GetCount() const
{
  return m_nCount;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline bool VMap<KEY_TYPE, VALUE_TYPE, HASH>::IsEmpty() const
{
  return m_nCount == 0;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
template<typename LOOKUP_TYPE>
inline UINT VMap<KEY_TYPE, VALUE_TYPE, HASH>::HashKey(const LOOKUP_TYPE& key) const
{
  return HASH()(key);
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline void VMap<KEY_TYPE, VALUE_TYPE, HASH>::InitHashTable(UINT nHashSize, bool bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(m_nCount == 0);
  VASSERT(nHashSize > 0);

  if (m_pHashTable != NULL)
  {
    RemoveAll();
  }

  if (bAllocNow)
  {
    m_pHashTable = new VPair*[nHashSize];
    memset(m_pHashTable, 0, sizeof(VPair*) * nHashSize);
  }
  m_nHashTableSize = nHashSize;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline void VMap<KEY_TYPE, VALUE_TYPE, HASH>::RemoveAll()
{
  VASSERT(V_IS_VALID_PTR(this));

  if (m_pHashTable != NULL)
  {
    // Destroy all existing key,value pairs
    for(int unsigned i = 0; i < m_nHashTableSize; i++)
    {
      VPair* pPair = m_pHashTable[i];
      while(pPair)
      {
        pPair->key.~KEY_TYPE();
        pPair->value.~VALUE_TYPE();
        pPair = pPair->pNext;
      }
    }

    // free hash table
    delete[] m_pHashTable;
    m_pHashTable = NULL;
  }

  m_nCount = 0;
  m_pFreeList = NULL;
  m_pBlocks->FreeChain();
  m_pBlocks = NULL;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline void VMap<KEY_TYPE, VALUE_TYPE, HASH>::Clear() // does not de-allocate
{
  VASSERT(V_IS_VALID_PTR(this));
  for (int iBucket=0;iBucket<(int)m_nHashTableSize && m_pHashTable!=NULL;iBucket++)
  {
    VPair* pPair = m_pHashTable[iBucket];
    while(pPair)
    {
      pPair->key.~KEY_TYPE();
      pPair->value.~VALUE_TYPE();
      pPair = pPair->pNext;
    }

    if (m_pHashTable[iBucket]==NULL)
      continue;
    if (m_pFreeList==NULL)
    {
      m_pFreeList = m_pHashTable[iBucket];
      m_pHashTable[iBucket] = NULL;
    }
    else
    {
      // append the whole bucket list to the free list
      VPair *pLast = m_pFreeList;
      while (pLast->pNext!=NULL)
        pLast = pLast->pNext;
      pLast->pNext = m_pHashTable[iBucket];
      m_pHashTable[iBucket] = NULL;
    }
  }
  m_nCount = 0;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline VMap<KEY_TYPE, VALUE_TYPE, HASH>::~VMap()
{
  RemoveAll();
  ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Pair helpers
// same as CList implementation except we store VPair's not CNode's
//    and VPair's are singly linked all the time

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline typename VMap<KEY_TYPE, VALUE_TYPE, HASH>::VPair* VMap<KEY_TYPE, VALUE_TYPE, HASH>::NewPair()
{
  if (m_pFreeList == NULL)
  {
    // add another block
    VLink* newBlock = VLink::Create(m_pBlocks, m_nBlockSize, sizeof(VPair));
    // chain them into free list
    VPair* pPair = (VPair*) newBlock->data();
    // free in reverse order to make it easier to debug
    pPair += m_nBlockSize - 1;
    for (int i = m_nBlockSize-1; i >= 0; i--, pPair--)
    {
      pPair->pNext = m_pFreeList;
      m_pFreeList = pPair;
    }
  }
  ASSERT(m_pFreeList != NULL);  // we must have something

  VPair* pPair = m_pFreeList;
  m_pFreeList = m_pFreeList->pNext;
  m_nCount++;
  ASSERT(m_nCount > 0);  // make sure we don't overflow

  return pPair;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline void VMap<KEY_TYPE, VALUE_TYPE, HASH>::FreePair(VPair* pPair, bool bDeallocOnEmpty)
{
  pPair->key.~KEY_TYPE();
  pPair->value.~VALUE_TYPE();
  pPair->pNext = m_pFreeList;
  m_pFreeList = pPair;
  m_nCount--;
  VASSERT(m_nCount >= 0);  // make sure we don't underflow

  // if no more elements, cleanup completely
  if (m_nCount == 0 && bDeallocOnEmpty)
    RemoveAll();
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
template<typename LOOKUP_TYPE>
inline typename VMap<KEY_TYPE, VALUE_TYPE, HASH>::VPair* VMap<KEY_TYPE, VALUE_TYPE, HASH>::GetPairAt(const LOOKUP_TYPE& key, UINT& nHash) const
// find Pair (or return NULL)
{
  nHash = HashKey(key) % m_nHashTableSize;

  if (m_pHashTable == NULL)
    return NULL;

  // see if it exists
  VPair* pPair;
  for (pPair = m_pHashTable[nHash]; pPair != NULL; pPair = pPair->pNext)
  {
    if (pPair->key == key)
      return pPair;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////

template<typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
template<typename LOOKUP_TYPE>
inline bool VMap<KEY_TYPE, VALUE_TYPE, HASH>::Lookup(const LOOKUP_TYPE& key, VALUE_TYPE& rValue) const
{
  VASSERT(V_IS_VALID_PTR(this));

  UINT nHash;
  VPair* pPair = GetPairAt(key, nHash);
  if (pPair == NULL)
    return false;  // not in map

  rValue = pPair->value;
  return true;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
template<typename LOOKUP_TYPE>
inline bool VMap<KEY_TYPE, VALUE_TYPE, HASH>::KeyExists(const LOOKUP_TYPE& key) const
{
  VASSERT(V_IS_VALID_PTR(this));

  UINT nHash;
  VPair* pPair = GetPairAt(key, nHash);
  if (pPair == NULL)
    return false;  // not in map

  return true;
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
template<typename LOOKUP_TYPE>
inline VALUE_TYPE& VMap<KEY_TYPE, VALUE_TYPE, HASH>::operator[](const LOOKUP_TYPE& key)
{
  VASSERT(V_IS_VALID_PTR(this));

  UINT nHash;
  VPair* pPair;
  if ((pPair = GetPairAt(key, nHash)) == NULL)
  {
    if (m_pHashTable == NULL)
      InitHashTable(m_nHashTableSize);

    // it doesn't exist, add a new Pair
    pPair = NewPair();

    new (&pPair->key) KEY_TYPE(key);
    new (&pPair->value) VALUE_TYPE();

    // put into hash table
    pPair->pNext = m_pHashTable[nHash];
    m_pHashTable[nHash] = pPair;
  }
  return pPair->value;  // return new reference
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
template<typename LOOKUP_TYPE>
inline bool VMap<KEY_TYPE, VALUE_TYPE, HASH>::RemoveKey(const LOOKUP_TYPE& key)
{
  VASSERT(V_IS_VALID_PTR(this));

  if (m_pHashTable == NULL)
    return false;  // nothing in the table

  VPair** ppPairPrev;
  ppPairPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

  VPair* pPair;
  for (pPair = *ppPairPrev; pPair != NULL; pPair = pPair->pNext)
  {
    if (pPair->key == key)
    {
      // remove it
      *ppPairPrev = pPair->pNext;  // remove from list
      FreePair(pPair);
      return true;
    }
    ppPairPrev = &pPair->pNext;
  }
  return false;  // not found
}

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline int VMap<KEY_TYPE, VALUE_TYPE, HASH>::RemoveValue(const VALUE_TYPE& value)
{
  VASSERT(V_IS_VALID_PTR(this));

  int iFound = 0;
  for (int iBucket=0;iBucket<(int)m_nHashTableSize && m_pHashTable!=NULL;iBucket++)
  {
    VPair** ppPairPrev;
    ppPairPrev = &m_pHashTable[iBucket];

    VPair* pPair;
    for (pPair = *ppPairPrev; pPair != NULL; pPair = pPair->pNext)
    {
      if (pPair->value == value)
      {
        // remove it
        *ppPairPrev = pPair->pNext;  // remove from list
        FreePair(pPair);
        iFound++;
        iBucket--;break; // restart this bucket
      } 
      else
      {
        ppPairPrev = &pPair->pNext;
      }
    }
  }
  
  return iFound;
}


/////////////////////////////////////////////////////////////////////////////
// Iterating

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline void VMap<KEY_TYPE, VALUE_TYPE, HASH>::GetNextPair(VPOSITION& rNextPosition, KEY_TYPE& rKey, VALUE_TYPE& rValue) const
{
  VASSERT(V_IS_VALID_PTR(this));
  VASSERT(m_pHashTable != NULL);  // never call on empty map

  VPair* pPairRet = (VPair*)rNextPosition;
  VASSERT(pPairRet != NULL);

  if (pPairRet == (VPair*) VBEFORE_START_POSITION)
  {
    // find the first Pair
    for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
    {
      if ((pPairRet = m_pHashTable[nBucket]) != NULL)
        break;
    }
    ASSERT(pPairRet != NULL);  // must find something
  }

  // find next Pair
  VASSERT(V_IS_VALID_READ_RANGE(pPairRet, sizeof(VPair))); // [gerd]
  VPair* pPairNext;
  if ((pPairNext = pPairRet->pNext) == NULL)
  {
    // go to next bucket

    for (UINT nBucket = (HashKey(pPairRet->key) % m_nHashTableSize) + 1; nBucket < m_nHashTableSize; nBucket++)
    {
      if ((pPairNext = m_pHashTable[nBucket]) != NULL)
        break;
    }
  }

  rNextPosition = (VPOSITION) pPairNext;

  // fill in return data
  rKey = pPairRet->key;
  rValue = pPairRet->value;
}


#ifdef _VISION_PS3
#pragma diag_push
#pragma diag_suppress=1669
#endif

template <typename KEY_TYPE, typename VALUE_TYPE, typename HASH>
inline VMap<KEY_TYPE, VALUE_TYPE, HASH>& VMap<KEY_TYPE, VALUE_TYPE, HASH>::operator=(const VMap& other)
{
  // Could be optimized and copied directly
  RemoveAll();
  VPOSITION p = other.GetStartPosition();
  KEY_TYPE key;
  VALUE_TYPE val;
  while(p)
  {
    other.GetNextPair(p, key, val);
    operator[](key) = val;
  }
  
  return *this;
}  

#ifdef _VISION_PS3
#pragma diag_pop
#endif

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
