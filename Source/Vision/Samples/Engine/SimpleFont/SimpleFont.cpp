/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Simple Font Demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// The demo shows how to use the font class provided in the engine.
// A font resource can be generated using the FontGrabber tool from the SDK.
// ***********************************************************************************************

#include <Vision/Samples/Engine/SimpleFont/SimpleFontPCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

class SimpleFontApp : public VAppImpl, public IVisCallbackHandler_cl
{
public:
  SimpleFontApp() : spFont_CN18(NULL), spFont_Arial22(NULL), spMeshObj(NULL), filtered(0), spCustomShaderPass(NULL), fAngle(270.0f) {}
  virtual ~SimpleFontApp() {}

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE
  {
#if defined(_VISION_PSP2)
    config.m_videoConfig.m_iXRes *= 2;
    config.m_videoConfig.m_iYRes *= 2;
#endif
  }

  virtual void Init() HKV_OVERRIDE
  {
    VisFont_cl::OnPrintCallback += this;
    LoadScene(VisAppLoadSettings("ViewerMap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap"));
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  virtual bool Run() HKV_OVERRIDE;

  virtual void DeInit() HKV_OVERRIDE
  {
    spFont_CN18 = NULL;
    spFont_Arial22 = NULL;
    spMeshObj = NULL;
    spCustomShaderPass = NULL;

    VisFont_cl::OnPrintCallback -= this;
  }

private:
  void LoadCustomShaderPass();

  VisFontPtr spFont_CN18;
  VisFontPtr spFont_Arial22;

  VisMeshBufferObjectPtr spMeshObj;
  VSimpleRenderState_t filtered;
  VCompiledShaderPassPtr spCustomShaderPass;

  float fAngle;
};

VAPP_IMPLEMENT_SAMPLE(SimpleFontApp);

void SimpleFontApp::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &VisFont_cl::OnPrintCallback)
  {
    VisPrintFontCallbackDataObject_cl &modifier((VisPrintFontCallbackDataObject_cl &)*pData);

    // Next line with half height
    if (modifier.m_szRemainingString[0]=='*')
    {
      modifier.m_fNextYAdvance *= 0.5f;
      modifier.m_szRemainingString++; // advance the text so these chars are not printed
      return;
    }

    // go on printing with new font...
    if (modifier.m_szRemainingString[0]=='$')
    {
      modifier.m_pFont = &Vision::Fonts.DebugFont();
      modifier.m_szRemainingString++; // advance the text so these chars are not printed
      return;
    }

    // convert the following 8 bytes into a 32bit color (RGBA hex). We know that we operate on char*, otherwise use the m_szRemainingStringW member
    unsigned int iHexVal;
    if (sscanf(modifier.m_szRemainingString,"%x",&iHexVal)==1)
    {
      modifier.m_iColor.SetRGBA((iHexVal>>24)&255,(iHexVal>>16)&255,(iHexVal>>8)&255,(iHexVal)&255);
      modifier.m_szRemainingString += 8; // advance the text so these chars are not printed
    }
  }
}

void SimpleFontApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());

  // load 2 font resources
  spFont_CN18 = Vision::Fonts.LoadFont("Fonts\\CourierNew_18.fnt");
  spFont_Arial22 = Vision::Fonts.LoadFont("Fonts\\Arial_22.fnt");
  VASSERT(spFont_CN18 && spFont_Arial22) ;

  // as a special feature, manually associate a character with a custom bitmap.
  // In a GUI you can use yet unused font characters (e.g. #254), pre-parse all GUI labels via the 
  // VGUIManager::OnTextLabelCallback callback and return patched strings-
  VTextureObject *pTex = Vision::TextureManager.Load2DTexture(":havok_sdk/Data/Vision/Samples/Engine/Textures/warning.dds");
  VRectanglef tc(0.f,0.f,1.f,1.f);    // texture coordinate on the custom texture
  VRectanglef sc(0.f,0.f,24.f,24.f);  // on-screen size of the character
  spFont_Arial22->AddCustomCharacter('!',pTex, tc, sc, sc.GetSizeX(), 28.f); // force a larger line stepping if a line contains this character

  // convert text into a font mesh where each character consists of two triangles
  VisMeshBuffer_cl *pFontMesh = spFont_Arial22->CreateTextMesh("This is 3D text\n(1 quad per character)",V_RGBA_RED);
  spMeshObj = new VisMeshBufferObject_cl(pFontMesh);
  spMeshObj->SetPosition(hkvVec3(200,0,0));
  spMeshObj->SetUseEulerAngles(TRUE);
  // set the correct render order for the text mesh, for both forward and deferred renderers
  spMeshObj->SetOrder( VRH_CORONAS_AND_FLARES, 0 );

  // retrieve callbacks every time we print a '#'. This is optional of course
  VisFont_cl::SetPrintCallbackCharacter('#');

  filtered = VisFont_cl::DEFAULT_STATE;
  filtered.SetFlag(RENDERSTATEFLAG_FILTERING); // rotated text looks better with filtering

  LoadCustomShaderPass();
}

bool SimpleFontApp::Run()
{
  // simple render state to render the font (use alpha blending)
  VSimpleRenderState_t state = VisFont_cl::DEFAULT_STATE;

  // Render our text. Usually, the PrintText function needs a IVRender2DInterface where it can render to (first parameter)
  // This however is not very practical, since a render interface is only available inside a render loop.
  // To be able to print text inside the Run function (or game code function), the VisFont_cl class has
  // some special code to batch printed text strings and render them later inside the render loop. To batch the text (instead of
  // rendering directly) just pass NULL as the first parameter
  spFont_CN18->PrintText(
    NULL,                    // render interface (NULL to batch the text since we are not inside the render loop)
    hkvVec2(100,100),      // position on screen
    "Small Font (size 18)",  // text to print
    VColorRef(255,255,255,160), // text color
    state,                   // render state (alpha blending,...)
    1.f                      // text scaling
    );

  // print another text in Arial 22
  spFont_Arial22->PrintText(NULL,hkvVec2(100,120), "Arial Font (size 22)", VColorRef(100,160,255,255), state, 1.f);
  spFont_Arial22->PrintText(NULL,hkvVec2(100,160), "Custom character: !,\nwhich forces larger\nline heights", V_RGBA_WHITE, state, 1.f);
  spFont_Arial22->PrintText(NULL,hkvVec2(100,250), "This is #FFFF00FFtext #FF00FFFFprinted #00FF00FFin #0000FFFFdifferent #00FFFF77colors #FFFFFFFFusing a print callback", V_RGBA_WHITE, state, 1.f);
  spFont_Arial22->PrintText(NULL,hkvVec2(100,300), "A print callback can also be used to #$change font\nor use#*\ndifferent line height", V_RGBA_WHITE, state, 1.f);

  // emulate italic text
  spFont_Arial22->PrintText(NULL,hkvVec2(100,350),hkvVec2(1.f,0.f),hkvVec2(0.25f,-1.f),"This emulates italic text",V_RGBA_WHITE,filtered);

  // print text using a shader (Tint in this example - note that passed 
  // green color is ignored since shader uses a constant color)
  if(spCustomShaderPass)
  {
    float fNear;
    float fFar;
    VisRenderContext_cl::GetMainRenderContext()->GetClipPlanes(fNear, fFar);
    spFont_Arial22->PrintText(NULL,hkvVec2(100,400),hkvVec2(1.0f,0.0f),hkvVec2(0.0f,-1.0f),"Shader-Based Text A - Normal",V_RGBA_GREEN,*spCustomShaderPass.GetPtr(), fNear + 2.0f);
    spFont_Arial22->PrintText(NULL,hkvVec2(100,420),hkvVec2(1.0f,0.0f),hkvVec2(1.0f,-1.0f),"Shader-Based Text B - Italic",V_RGBA_GREEN,*spCustomShaderPass.GetPtr(), fNear + 2.0f);
  }

  // 2D text rotation
  hkvVec2 vDir (hkvMath::cosDeg (fAngle), hkvMath::sinDeg (fAngle));
  hkvVec2 vUp(vDir.y,-vDir.x); // orthogonal
  spFont_Arial22->PrintText(NULL,hkvVec2(400,480),vDir,vUp,"This is rotated 2D text\nincluding line breaks",V_RGBA_WHITE,filtered);

  // rotate the 3d font
  fAngle = hkvMath::mod (fAngle+Vision::GetTimer()->GetTimeDifference()*100.f,360.f);
  spMeshObj->SetOrientation(fAngle,0,0);

  return true;
}

void SimpleFontApp::LoadCustomShaderPass()
{
  // load TintColor shader to be used with PrintText and assign yellow color
  VShaderEffectLib* pFXLib = Vision::Shaders.LoadShaderLibrary("Shaders\\TextShader.ShaderLib", SHADERLIBFLAG_HIDDEN);
  if(pFXLib)
  {
    VCompiledEffect* pFX = Vision::Shaders.CreateEffect("TextShader", NULL, EFFECTCREATEFLAG_FORCEUNIQUE, pFXLib);
    if(pFX)
    {
      VCompiledShaderPass** ppPasses = pFX->GetDefaultTechnique()->GetShaderList();
      if (ppPasses && *ppPasses)
        spCustomShaderPass = *ppPasses;
    }
  }
}

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
