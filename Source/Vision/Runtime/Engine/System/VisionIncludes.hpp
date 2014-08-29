/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionIncludes.hpp

#ifndef VISION_INCLUDES_HPP
#define VISION_INCLUDES_HPP

//Add all shared/include headers here (e.g. vBase)

#include <Vision/Runtime/Base/VBaseX.hpp>  //VBase + shaders, textures and video classes
#include <Vision/Runtime/Base/Container/DynArray.hpp>
#include <Vision/Runtime/Base/VGL/VGL.hpp>
#include <Vision/Runtime/Base/Action/VAction.hpp>
#include <Vision/Runtime/Base/System/VCallbacks.hpp>
#include <Vision/Runtime/Base/Graphics/VColor.hpp>
#include <Vision/Runtime/Base/System/vEndianSwitch.h>
#include <Vision/Runtime/Base/System/VRefCounter.hpp>
#include <Vision/Runtime/Base/String/VString.hpp>
#include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadManager.hpp>
#include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadedTask.hpp>
#include <Vision/Runtime/Base/System/Threading/Thread/VBackgroundThread.hpp>
#include <Vision/Runtime/Base/System/Threading/StreamProcessor/VStreamProcessingTask.hpp>
#include <Vision/Runtime/Base/System/Threading/StreamProcessor/vstreamprocessor.hpp>
#ifdef SPU
  #include <Vision/Runtime/Base/System/Threading/StreamProcessor/VStreamProcessingHelpers_SPURS_PS3.hpp>
  #include <Vision/Runtime/Base/System/Threading/StreamProcessor/VStreamIterator_SPURS_PS3.hpp>
#else
  #include <Vision/Runtime/Base/System/Threading/StreamProcessor/VStreamProcessingHelpers_ThreadedTask.hpp>
  #include <Vision/Runtime/Base/System/Threading/StreamProcessor/vstreamiterator_threadedtask.hpp>
#endif

#include <Vision/Runtime/Base/System/SenderReceiver/VReceiver.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VSerialX.hpp>
#include <Vision/Runtime/Base/System/VUserDataObj.hpp>


#include <Vision/Runtime/Base/Container/VArray.hpp>
#include <Vision/Runtime/Base/Container/vstrlist.hpp>
#include <Vision/Runtime/Base/Container/VCollection.hpp>
#include <Vision/Runtime/Base/Container/VLinkedList.hpp>
#include <Vision/Runtime/Base/Container/VPListStack.hpp>
#include <Vision/Runtime/Base/Container/VDictionary.hpp>
#include <Vision/Runtime/Base/Container/VBitField.hpp>

#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>

#include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>

#include <Vision/Runtime/Base/System/IO/Stream/IVFileStream.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VDiskFileStream.hpp>

#include <Vision/Runtime/Base/System/Resource/VResource.hpp>
#include <Vision/Runtime/Base/System/Resource/VResourceManager.hpp>
#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>
#include <Vision/Runtime/Base/Graphics/Textures/VTextureManager.hpp>

#include <Vision/Runtime/Base/Math/Primitive/VTriangle.hpp>
#include <Vision/Runtime/Base/Physics/VCollisionData.hpp>
#include <Vision/Runtime/Base/Physics/VCollisionMesh.hpp>

#include <Vision/Runtime/Base/Graphics/VLightmapHelper.hpp>
#include <Vision/Runtime/Base/Graphics/vLightGrid.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderPass.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderManager.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderFXParser.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderEffectLib.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vEffectAssignment.hpp>
#include <Vision/Runtime/Base/String/VStringTokenizerInPlace.hpp>


#include <Vision/Runtime/Base/RemoteComm/VTarget.hpp>
#include <Vision/Runtime/Base/RemoteComm/VConnection.hpp>
#include <Vision/Runtime/Base/RemoteComm/VMessage.hpp>

#include <Vision/Runtime/Base/ReportDialog/VReport.hpp>

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
