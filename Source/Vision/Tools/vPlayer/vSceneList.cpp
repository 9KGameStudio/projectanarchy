/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/vLuaScript.h>
#include <Vision/Tools/vPlayer/vSceneList.hpp>


VSceneList* g_pCurrentList = NULL;
unsigned int g_targetPlatformFilterMask = 0;

int LUA_AddScene(lua_State* state)
{
  vLuaScript s(state);
  if (s.getNumberOfFunctionParameters() < 4)
    return 0;

  const hkUint32 uiPlatforms = (hkUint32)s.getIntParameter(1);

  // if this scene is not supported on this platform, ignore it
  if ((uiPlatforms & g_targetPlatformFilterMask) == 0)
    return 0;

  VSceneListEntry entry;

  entry.uiTargetPlatforms = uiPlatforms;
  entry.sDisplayName = s.getStringParameter(2);
  entry.sScenePath = s.getStringParameter(3);
  
  const char* szSearchPaths = s.getStringParameter(4);
  VNameValueListParser<';', '='> parser(szSearchPaths, false);
  while (parser.next())
  {
    entry.sSearchPaths.Append(parser.name());
  }
  if (entry.sSearchPaths.GetSize() == 0)
    entry.sSearchPaths.Append("");

  if (s.getNumberOfFunctionParameters() >= 5)
    entry.sRoot = s.getStringParameter(5);

  if (s.getNumberOfFunctionParameters() >= 6)
  {
    __int64 tmp;
    sscanf(s.getStringParameter(6), "%lld", &tmp);
    entry.LastUsed = VDateTime::FromSerializable(tmp);
  }
  
  g_pCurrentList->Append(entry);

  return 0;
}

//////////////////////////////////////////////////////////////////////////

bool VSceneListEntry::operator==(const VSceneListEntry& other) const
{
  return sScenePath == other.sScenePath &&
    sSearchPaths[0] == other.sSearchPaths[0];
}

bool VSceneListEntry::operator!=(const VSceneListEntry& other) const
{
  return !(*this == other);
}

bool VSceneListEntry::operator<(const VSceneListEntry& other) const
{
  if (LastUsed != other.LastUsed)
    return LastUsed > other.LastUsed; // newer entries should appear first

  return sDisplayName < other.sDisplayName;
}

//////////////////////////////////////////////////////////////////////////


VSceneList::VSceneList()
{

}

VSceneList::~VSceneList()
{

}

hkvResult VSceneList::LoadFromFile(const char* szFilePath, unsigned int uiTargetPlatformFilterMask)
{
  Reset();

  m_sFilePath = szFilePath;
  m_uiTargetPlatformFilterMask = uiTargetPlatformFilterMask;

  vLuaScript s;

  // register all available platforms as bitmasks
  for (hkInt32 i = 0; i < TARGETDEVICE_COUNT; ++i)
  {
    VString sDeviceName = VTargetDeviceName[i];

    // remove all white spaces
    sDeviceName.ReplaceAll(" ", "");
    sDeviceName.ReplaceAll("\t", "");

    sDeviceName.ToUpper();

    s.setVariable(sDeviceName.AsChar(), (int)V_BIT(i));
  }

#ifdef HK_ANARCHY
  s.setVariable("HK_ANARCHY", true);
#else
  s.setVariable("HK_ANARCHY", false);
#endif

#ifdef _VISION_WINRT // all just DX11 set

  s.setVariable("APOLLO", V_BIT(TARGETDEVICE_COUNT));
  s.setVariable("METRO", V_BIT(TARGETDEVICE_COUNT));

#endif

  // register the callback functions
  s.RegisterCFunction("AddScene", LUA_AddScene);

  g_pCurrentList = this;
  g_targetPlatformFilterMask = uiTargetPlatformFilterMask;

  VString sError;
  bool bResult = s.ExecuteFile(szFilePath, sError);
  if (!bResult)
  {
    hkvLog::Error("Could not parse scene config file: %s", sError.AsChar());
  }

  g_targetPlatformFilterMask = 0;
  g_pCurrentList = NULL;

  return bResult ? HKV_SUCCESS : HKV_FAILURE;
}

hkvResult VSceneList::SaveToFile() const
{
  IVFileOutStream* pFile = Vision::File.Create(m_sFilePath);
  if (pFile == NULL)
    return HKV_FAILURE;

  VString buffer;
  
  for (int i = 0; i < GetSize(); ++i)
  {
    const VSceneListEntry& entry = GetAt(i);
    VString sPlatformMask = PlatformMaskToString(entry.uiTargetPlatforms);
    
    VString sSearchPaths;
    for (int i = 0; i < entry.sSearchPaths.GetSize(); ++i)
    {
      sSearchPaths += entry.sSearchPaths[i];
      if (i < entry.sSearchPaths.GetSize()-1)
        sSearchPaths += ";";
    }

    buffer.Format("AddScene(%s, \"%s\", \"%s\", \"%s\", \"%s\", \"%lld\")\n",
      sPlatformMask.AsChar(), 
      entry.sDisplayName.AsChar(), 
      entry.sScenePath.AsChar(), 
      sSearchPaths.AsChar(), 
      entry.sRoot.AsChar(), 
      entry.LastUsed.ToSerializable());

    pFile->WriteText(buffer);
  }

  pFile->Close();
  return HKV_SUCCESS;
}

void VSceneList::Reload()
{
  LoadFromFile(m_sFilePath, m_uiTargetPlatformFilterMask);
}

void VSceneList::Sort()
{
  struct Helper
  {
    static int Compare(const void *arg1, const void *arg2)
    {
      const VSceneListEntry& a = *static_cast<const VSceneListEntry*>(arg1);
      const VSceneListEntry& b = *static_cast<const VSceneListEntry*>(arg2);

      return (a == b) ? 0 : ((a < b) ? -1 : 1);
    }
  };

  qsort(GetData(), GetSize(), sizeof(VSceneListEntry), Helper::Compare);
}

VString VSceneList::PlatformMaskToString(hkUint32 targetPlatformFilterMask) const
{
  char szPlatforms[1024] = {0};
  for (int i = 0; i < TARGETDEVICE_COUNT; ++i)
  {
    if ((targetPlatformFilterMask & (V_BIT(i))) != 0)
    {
      VString sDeviceName = VTargetDeviceName[i];

      // remove all white spaces
      sDeviceName.ReplaceAll (" ", "");
      sDeviceName.ReplaceAll ("\t", "");

      sDeviceName.ToUpper();

      if (szPlatforms[0] != '\0')
        strcat_s(szPlatforms, " + ");

      strcat_s(szPlatforms, sDeviceName);
    }
  }

  return szPlatforms;
}

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
