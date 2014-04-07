/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <d3d9.h>

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

class Direct3DVertexDeclaration9Dummy : public IDirect3DVertexDeclaration9, public UnknownDummy
{
public:
  HKV_NR_IMPLEMENT_IUNKNOWN;

  STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetDeclaration)(THIS_ D3DVERTEXELEMENT9* pElement,UINT* pNumElements) HKV_NR_UNIMPLEMENTED

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
