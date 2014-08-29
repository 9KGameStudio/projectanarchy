/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __V_APP_COMPONENT
#define __V_APP_COMPONENT

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppImportExport.hpp>
#include <Vision/Runtime/Framework/VisionApp/Helper/VAppHelper.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

class VAppImpl;

/// \brief
///   Interface defining a VStartupModule.
///
/// Startup modules are executed/processed before the Vision engine gets initialized.
/// After the module gets initialized, the Run() method is executed as long as it
/// returns true. As soon as the Run() method returns false the module execution is
/// finished by a call to DeInit() and the system continues with the next module.
///
/// \example
///   \code
///   class MyStartupModule : public VStartupModule
///   {
///     virtual bool Run()
///     {
///       if (/* exit condition */)
///         return false;
///       
///       // ...
///   
///       return true;
///     }
///   };
///
///   REGISTER_STARTUP_MODULE(MyStartupModule, 0);
///
///   \endcode
///
/// \ingroup VisionAppFramework
class VStartupModule
{
public:
  VStartupModule() {}
  virtual ~VStartupModule() {}

  virtual void Init() {}
  virtual void DeInit() {}

  virtual bool Run() = 0;
};

/// \brief
///   Base class for all kinds of application modules.
///
/// Application modules are meant to be for extending the capabilities of an VApp application.
/// The basic idea is that once a module is initialized it reacts to all kinds of Vision callbacks,
/// and thus is able to perform nearly every task on application level within Vision.
///
/// Furthermore, the base class implementation also provides a couple of helper methods
/// for dealing with dynamically created callbacks from a VAppMenu. An overview of different
/// use cases and how to deal with them can be found in the VDebugOptions class.
///
/// Every derived class must put the macro V_DECLARE_DYNCREATE(class_name) into the header declaration
/// to add RTTI capabilities, and also the counterpart V_IMPLEMENT_DYNCREATE(...) in the cpp, otherwise
/// the VAppImpl is not able to retrieve a module with a given type. See RTTI documentation for sample code.
///
/// \sa VAppImpl::GetAppModule
/// \sa VAppImpl::GetAppModules
/// \ingroup VisionAppFramework
class VAppModule : public VTypedObject, public VRefCounter, public IVisCallbackHandler_cl
{
  V_DECLARE_DYNCREATE_DLLEXP(VAppModule, VAPP_IMPEXP);

public:
  VAppModule() : m_bEnabled(true), m_pParent(NULL) {}
  virtual ~VAppModule() {}

  virtual void Init() {}
  virtual void DeInit() {}

  inline VAppImpl* GetParent() const { return m_pParent; }
  inline void SetParent(VAppImpl* pParent) { m_pParent = pParent; }

  virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE {}

  inline bool IsEnabled() const { return m_bEnabled; }
  inline void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

protected:
  VAPP_IMPEXP int GetCallbackIndex(IVisCallbackDataObject_cl* pData);
  VAPP_IMPEXP int GetCallbackIndex(VArray<VisCallback_cl*>& callbacks, IVisCallbackDataObject_cl* pData);

  VAPP_IMPEXP void RegisterCallbacks();
  VAPP_IMPEXP void RegisterCallbacks(VArray<VisCallback_cl*>& callbacks);
  
  VAPP_IMPEXP void DeRegisterCallbacks();
  VAPP_IMPEXP void DeRegisterCallbacks(VArray<VisCallback_cl*>& callbacks);

  VArray<VisCallback_cl*> m_callbacks;
  bool m_bEnabled;

private:
  VAppImpl* m_pParent;
};

/// \brief
///   Default sorting key values which are used by the default application modules to register items at the application menu.
#define VAPP_DEFAULT_SORTING_0 100
#define VAPP_DEFAULT_SORTING_1 200
#define VAPP_DEFAULT_SORTING_2 300
#define VAPP_DEFAULT_SORTING_3 400
#define VAPP_DEFAULT_SORTING_4 500

/// \brief
///   GUI context wrapper.
///
/// This wrapper is used to provide a application wide context for all kind of menu stuff.
/// Note that the context can't be activated or deactivated, this is handled transparently
/// when showing or closing a dialog.
///
/// Note that the modal versions of ShowDialog() always use a NullApp.
///
/// \ingroup VisionAppFramework
class VAppMenuContext : public VGUIMainContext
{
public:
  VAppMenuContext() : VGUIMainContext(NULL) {}
  virtual ~VAppMenuContext() {}

  VAPP_IMPEXP virtual VDialog* ShowDialog(const char *szDialogResource) HKV_OVERRIDE;
  VAPP_IMPEXP virtual void ShowDialog(VDialog* pDialog) HKV_OVERRIDE;
  VAPP_IMPEXP virtual void CloseDialog(VDialog* pDialog) HKV_OVERRIDE;

  VAPP_IMPEXP virtual void OnRender() HKV_OVERRIDE;

private:
  virtual void SetActivate(bool bStatus) HKV_OVERRIDE;
};
typedef VSmartPtr<VAppMenuContext> VAppMenuContextPtr;

/// \brief
///   Helper class providing various menu item colors.
class VAppMenuColors 
{
public:
  enum VDialogColors
  {
    COLOR_DIALOG_BG = 0,
    COLOR_TEXT,
    COLOR_LIST_CONTROL_BG,
    COLOR_LIST_ITEM_BG,
    COLOR_LIST_GROUP_BG,
    COLOR_LIST_ITEM_TEXT_DISABLED,
    COLOR_LIST_ITEM_TEXT_NORMAL,
    COLOR_LIST_ITEM_TEXT_OVER,
    COLOR_LIST_ITEM_TEXT_SELECTED,

    COLOR_COUNT
  };

  VAPP_IMPEXP static void SetColor(VDialogColors item, const VColorRef& color);
  VAPP_IMPEXP static const VColorRef& GetColor(VDialogColors item);

private:
  static VColorRef m_colors[COLOR_COUNT];
};

/// \brief
///   Menu item used by the default menu.
struct VAppMenuItem
{
public:
  VAppMenuItem()
    : m_sItemName()
    , m_uiSortingKey(0)
    , m_bCheckable(true)
    , m_bInitCheckState(false)
    , m_iAction(-1) {}
  
  VAppMenuItem(const char* szItemName, int iAction, unsigned int uiSortingKey, bool bCheckable, bool bInitCheckState = false)
    : m_sItemName(szItemName)
    , m_uiSortingKey(uiSortingKey)
    , m_bCheckable(bCheckable)
    , m_bInitCheckState(bInitCheckState)
    , m_iAction(iAction) {}
  
  VString      m_sItemName;       ///< Display Name
  unsigned int m_uiSortingKey;    ///< Sorting key by which all items with the same parent should be sorted.
  bool         m_bCheckable;      ///< true: item can be turned on and off, false: item represents a one-shot action
  bool         m_bInitCheckState; ///< Defines the inital state of the item in case m_bCheckable is true
  int          m_iAction;         ///< Value which is returned via the callback when item is clicked (see VAppMenuDataObject)
};

typedef VArray<VAppMenuItem> VAppMenuItems;

/// \brief
///   Interface describing an application menu.
///
/// The only condition custom implementations must meet is that there should be some sort of mechanism that determines
/// if an item is clicked, activated, etc. and trigger the corresponding callback which is created in RegisterItem(...)
/// and RegisterGroup(...) respectively.
///
/// \sa VDefaultMenu
/// \ingroup VisionAppFramework
class VAppMenu : public VAppModule
{
  V_DECLARE_DYNCREATE_DLLEXP(VAppMenu, VAPP_IMPEXP);

public:
  /// \brief
  ///   Registers the given item and adds it to the item with the given name.
  ///
  /// When not specifying any parent name the item is added to the ROOT.
  ///
  /// \param item
  ///   Item which is added/registered to the menu.
  ///
  /// \param szParentName
  ///   Name of the item to which the new item should be attached to.
  ///
  /// \return
  ///   Callback which is triggered by the VAppMenu once the item is activated/clicked.
  virtual VisCallback_cl* RegisterItem(const VAppMenuItem& item, const char* szParentName = NULL) = 0;

  /// \brief
  ///   Registers the given items as a group with the given group name.
  ///
  /// Same as for RegisterItem(...), when not specifying a parent name the group is added to the ROOT.
  ///
  /// \param szGroupName
  ///   Group's name which is used to display the item, but can also be used as a parent name for other items.
  ///
  /// \param items
  ///   Array of items included in the group.
  ///
  /// \param szParentName
  ///   Name of the item to which the new group should be attached to.
  ///
  /// \param uiSortingKey
  ///   Key used to sort items on the same level (with the same parent).
  ///
  /// \param bMultiSelect
  ///   If True multiple items of the group can be active at the same time, otherwise only one item can be active at a time.
  ///
  /// \return
  ///   One callback for each item of the group. Each callback is triggered once the item in question is activated/clicked.
  virtual VArray<VisCallback_cl*> RegisterGroup(const char* szGroupName, const VArray<VAppMenuItem>& items, const char* szParentName = NULL, unsigned int uiSortingKey = 0, bool bMultiSelect = true) = 0;

  /// \brief
  ///   Remove the item with the given name from the menu.
  virtual void RemoveItem(const char* szItemName) = 0;

  /// \brief
  ///   Remove the group and all its children with the given name from the menu.
  virtual void RemoveGroup(const char* szGroupName) = 0;

  /// \brief
  ///   Sets the state of the item with the given name.
  virtual void SetItemCheckState(const char* szItemName, bool bActive) = 0;

  /// \brief
  ///   Returns whether the item with the given name is active or not.
  virtual bool GetItemCheckState(const char* szItemName) const = 0;

  /// \brief
  ///   Returns whether the menu is visible or not.
  virtual bool IsVisible() const = 0;

  /// \brief
  ///   Sets the visibility state
  virtual void SetVisible(bool bVisible) = 0;

  /// \brief
  ///   Start the open or close animation.
  virtual void ToggleMainMenu() = 0;

  /// \brief
  ///   Reset the menu to its initial state without any animation applied.
  virtual void Reset() = 0;
};

/// \brief
///   Callback Data Object send by a menu implementation once a menu item is clicked, activated, etc.
class VAppMenuDataObject : public IVisCallbackDataObject_cl
{
public:
  /// \brief
  ///   Helper describing the type of the menu callback object.
  enum VAppMenuDataObjectType
  {
    TYPE_TRIGGER = 0, ///< Normal action (clicked, activated, etc.)
    TYPE_DELETE       ///< !!! Callback which sends this data object is about to being deleted. This type gives registered 
                      ///      listeners the chance to de-register themself before deletion of the callback object.
  };

  inline VAppMenuDataObject(VCallback *pSender, unsigned int uiType, int iAction) : IVisCallbackDataObject_cl(pSender), m_uiType(uiType), m_iAction(iAction) {}
  unsigned int m_uiType;
  int m_iAction;
};

/// \brief
///   Styled button class extending a simple VPushButton.
class VStyledButton : public VPushButton
{
public:
  VAPP_IMPEXP VStyledButton(const char* szText, bool bUseGradient, VisFontPtr spFont, float fScale, float fBorderWidth, float fFontScale);
  VAPP_IMPEXP virtual void OnPaint(VGraphicsInfo& Graphics, const VItemRenderInfo& parentState) HKV_OVERRIDE;

private:
  float m_fBorderWidth;
};

#endif //__V_APP_COMPONENT

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
