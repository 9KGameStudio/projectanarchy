/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef IDataProvider_h__
#define IDataProvider_h__

class IDataProvider
{
public:
  struct Flags
  {
    enum Enum
    {
      READ_ONLY = V_BIT(0),

      DEFAULT = 0
    };
  };

  IDataProvider(hkUint32 uiFlags) : m_uiFlags(uiFlags) {}
  virtual ~IDataProvider() {}

  inline bool IsReadOnly() const { return (m_uiFlags & Flags::READ_ONLY) != 0; }
  inline hkUint32 GetFlags() const { return m_uiFlags; }

  VisCallback_cl m_OnDataChanged;

private:
  hkUint32 m_uiFlags;
};

class ITextDataProvider : public IDataProvider
{
public:
  ITextDataProvider(hkUint32 uiFlags) : IDataProvider(uiFlags) {}

  virtual const VString& GetData() const = 0;

  virtual VColorRef GetColor() const = 0;
};

#endif // IDataProvider_h__

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
