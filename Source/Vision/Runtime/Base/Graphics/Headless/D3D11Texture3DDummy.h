/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <D3D11.h>

#include <Vision/Runtime/Base/Graphics/Headless/D3D11DeviceChildDummy.h>
#include <Vision/Runtime/Base/Graphics/Headless/D3D11ResourceDummy.h>
#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

class D3D11Texture3DDummy : public ID3D11Texture3D, public UnknownDummy
{
public:
  D3D11Texture3DDummy(const D3D11_TEXTURE3D_DESC* pDesc) : m_desc(*pDesc)
  {}

  HKV_NR_IMPLEMENT_IUNKNOWN;
  HKV_NR_IMPLEMENT_ID3D11DEVICECHILD;
  HKV_NR_IMPLEMENT_ID3D11RESOURCE;

  virtual void STDMETHODCALLTYPE GetDesc(D3D11_TEXTURE3D_DESC *pDesc) HKV_OVERRIDE
  {
    *pDesc = m_desc;
  }

private:
  D3D11Texture3DDummy();

  D3D11_TEXTURE3D_DESC m_desc;
};

/*
 * Havok SDK - Base file, BUILD(#20140328)
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
