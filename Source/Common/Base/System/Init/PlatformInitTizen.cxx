/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <FBase.h>
#include <FApp.h>
#include <FMedia.h>
#include <Common/Base/Container/String/hkUtf8.h>

#include <Common/Base/System/Init/PlatformInitTizen.h>

int hkMain(int argc, const char** argv);

inline void PlatformInit()
{

}

inline void PlatformAddDefaultMounts(hkFileSystem* basefs, hkUnionFileSystem* ufs)
{
	hkUtf8::Utf8FromWide resDirW(Tizen::App::App::GetInstance()->GetAppResourcePath().GetPointer());
	ufs->mount(basefs, "", resDirW.cString(), false); // ReadOnly
	hkUtf8::Utf8FromWide dataDirW(Tizen::App::App::GetInstance()->GetAppDataPath().GetPointer());
	ufs->mount(basefs, "", dataDirW.cString(), true); // Read/Write
}


using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using Tizen::Base::Runtime::Timer;

Tizen::Ui::Controls::Form *g_currentApp;
const int TIME_OUT = 10;

#ifndef HK_TIZEN_CUSTOM_MAIN
extern "C" int OspMain(int argc, char* pArgv[])
{
	result r = E_SUCCESS;

	Tizen::Base::Collection::ArrayList* pArgs = new Tizen::Base::Collection::ArrayList();
	pArgs->Construct();

	AppLog("Tizen application started.");
	r = Tizen::App::Application::Execute(TizenApp::CreateInstance, pArgs);
	if (IsFailed(r))
	{
		AppLogExceptionTag("Havok", "Application execution failed-[%s].", GetErrorMessage(r));
		r &= 0x0000FFFF;
	}

	pArgs->RemoveAll(true);
	delete pArgs;
	AppLogTag("Havok", "Tizen application finished.");

	return static_cast<int>(r);
}
#endif

TizenForm::TizenForm(TizenApp* pApp)
		: __pApp(pApp), m_SplashScreen(null)
{
}

TizenForm::~TizenForm(void)
{
}

result TizenForm::OnInitializing(void)
{
	Init();
	return E_SUCCESS;
}

result TizenForm::Init(void)
{
	int x, y;
	SetOrientation(Tizen::Ui::ORIENTATION_LANDSCAPE);
	GetBounds(x, y, width, height);

	Tizen::Media::Image image;
	if (image.Construct() == E_SUCCESS) {
		Tizen::Base::String filepath = TizenApp::GetInstance()->GetAppResourcePath() + L"screen-density-xhigh/SplashScreen.png";
		m_SplashScreen = image.DecodeN(filepath, Tizen::Graphics::BITMAP_PIXEL_FORMAT_ARGB8888);
	}

	AddTouchEventListener(*this);
	return E_SUCCESS;
}
	
result TizenForm::OnDraw(void)
{
	Tizen::Graphics::Canvas* canvas = GetCanvasN();
	if (canvas != null)
	{
		Tizen::Graphics::Rectangle clientRect = GetClientAreaBounds();
		Tizen::Graphics::Rectangle rt(0, 0, clientRect.width, clientRect.height);
		
		int height = rt.height;
		int width = rt.height * m_SplashScreen->GetWidth() / m_SplashScreen->GetHeight();
		return canvas->DrawBitmap(Tizen::Graphics::Rectangle((rt.x + ((rt.width - width) / 2)), rt.y, width, height), *m_SplashScreen);
	}
	
	return E_SUCCESS;
}
	
	
void TizenForm::OnTouchPressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo & touchInfo)
{
}
	
void TizenForm::OnTouchLongPressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}

void TizenForm::OnTouchReleased(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}

void TizenForm::OnTouchMoved(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}

void TizenForm::OnTouchDoublePressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}

void TizenForm::OnTouchFocusIn(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}
	
void TizenForm::OnTouchFocusOut(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}
	
void TizenForm::OnTouchCanceled(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition, const Tizen::Ui::TouchEventInfo& touchInfo)
{
}


TizenApp::TizenApp()
	: __pForm(HK_NULL), __pTimer(HK_NULL) 
{
	
}

TizenApp::~TizenApp()
{
}

Application* TizenApp::CreateInstance()
{
	return new (std::nothrow) TizenApp();
}

TizenForm* TizenApp::createForm()
{
	return new (std::nothrow) TizenForm(this);
}

bool TizenApp::OnAppInitializing(AppRegistry& appRegistry)
{
	// TODO:
	// Initialize Frame and App specific data.
	// The App's permanent data and context can be obtained from the appRegistry.
	//
	// If this method is successful, return true; otherwise, return false.
	// If this method returns false, the App will be terminated.

	result r = E_SUCCESS;

	// Create a Frame
	Frame* pTizenAppFrame = new (std::nothrow) Frame();
	r = pTizenAppFrame->Construct();
	if ( IsFailed(r) ) {
		AppLogExceptionTag("Havok", "TizenAppFrame Construct failed.");
	}
	pTizenAppFrame->SetOrientation(ORIENTATION_LANDSCAPE);
	
	this->AddFrame(*pTizenAppFrame);
	
	__pForm = createForm();
	if (__pForm == NULL) 
	{
		AppLogExceptionTag("Havok", "Form creation failed.");
	}
	
	g_currentApp = __pForm;
	
	r = __pForm->Construct(FORM_STYLE_NORMAL);
	if ( IsFailed(r) ) {
		AppLogExceptionTag("Havok", "Construct failed.");
	}

	__pForm->AddKeyEventListener(*__pForm);

	r = GetAppFrame()->GetFrame()->AddControl(*__pForm);
	if ( IsFailed(r) ) {
		AppLogExceptionTag("Havok", "AddControl failed.");
	}

	Timer* pTempTimer = new (std::nothrow) Timer;
	if (pTempTimer == NULL) AppLogExceptionTag("Havok", "Failed to allocate timer.");

	r = pTempTimer->Construct(*this);
	if (IsFailed(r)) AppLogExceptionTag("Havok", "pTempTimer->Construct(*this) failed.");

	AppLog("OnAppInitializing done.\n");

	__pTimer  = pTempTimer;

	return true;
}

bool TizenApp::OnAppInitialized()
{
	mAppThread.Construct();
	mAppThread.Start();
	return true;
}

bool TizenApp::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	// Deallocate resources allocated by this App for termination.
	// The App's permanent data and context can be saved via appRegistry.
	
	if (__pTimer != null)
	{
		__pTimer->Cancel();
		delete __pTimer;
		__pTimer = null;
	}

	return true;
}

void
TizenApp::OnForeground()
{
	//~ Tizen::System::PowerManager::KeepScreenOnState(true, false);
	// Start or resume drawing when the application is moved to the foreground.
	if (__pTimer != null)
	{
		__pTimer->Start(TIME_OUT);
	}
}

void
TizenApp::OnBackground()
{
	// TODO:
	// Stop drawing when the application is moved to the background.
	if (__pTimer != null)
	{
		__pTimer->Cancel();
	}
}

void
TizenApp::OnLowMemory()
{
	// TODO:
	// Free unused resources or close the application.
}

void
TizenApp::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	// TODO:
	// Handle any changes in battery level here.
	// Stop using multimedia features(camera, mp3 etc.) if the battery level is CRITICAL.
}

void
TizenApp::OnScreenOn()
{
	// TODO:
	// Get the released resources or resume the operations that were paused or stopped in OnScreenOff().
	OnForeground();
}

void
TizenApp::OnScreenOff()
{
	// TODO:
	// Unless there is a strong reason to do otherwise, release resources (such as 3D, media, and sensors) to allow the device
	// to enter the sleep mode to save the battery.s
	// Invoking a lengthy asynchronous method within this listener method can be risky, because it is not guaranteed to invoke a
	// callback before the device enters the sleep mode.
	// Similarly, do not perform lengthy operations in this listener method. Any operation must be a quick one.
	OnBackground();
}

void TizenApp::OnTimerExpired(Timer& timer)
{
	if (__pTimer == NULL) return;

	__pTimer->Start(TIME_OUT);
	__pForm->OnDraw();
}

void TizenForm::OnKeyPressed(const Control& source, KeyCode keyCode)
{
}

void TizenForm::OnKeyReleased(const Control& source, KeyCode keyCode)
{
	if ( (keyCode == Tizen::Ui::KEY_BACK) || (keyCode == Tizen::Ui::KEY_MENU) )
	{
		__pApp->Terminate();
	}
}

void TizenForm::OnKeyLongPressed(const Control& source, KeyCode keyCode)
{
}

Object* AppThread::Run() {
	hkMain(0, HK_NULL);
	TizenApp::GetInstance()->Terminate();
	return null;
}

/*
 * Havok SDK - Base file, BUILD(#20140625)
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
