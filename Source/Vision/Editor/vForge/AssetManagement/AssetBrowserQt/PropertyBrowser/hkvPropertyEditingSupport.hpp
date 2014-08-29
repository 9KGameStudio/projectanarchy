/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_PROPERTY_EDITING_SUPPORT_INCLUDED
#define HKV_PROPERTY_EDITING_SUPPORT_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

class hkvPropertyWidget;

/// \brief
///   Abstract base class for providers of display/editing support of hkvProperty objects.
class hkvPropertyEditingProvider
{
protected:
  hkvPropertyEditingProvider() {}
private:
  hkvPropertyEditingProvider(const hkvPropertyEditingProvider&);
  hkvPropertyEditingProvider& operator=(const hkvPropertyEditingProvider&);
public:
  virtual ~hkvPropertyEditingProvider() {}

public:
  /// \brief
  ///   Returns whether this editing provider can provide display/editing support for
  ///   the passed property.
  /// \param prop
  ///   the property to display or edit
  /// \return
  ///   \c true if this provider supports the passed property; \c false if not.
  virtual bool canHandleProperty(const hkvProperty& prop) const = 0;

  /// \brief
  ///   Builds the string for displaying the passed property in a display-only context, and
  ///   writes the display string to a string buffer.
  /// \param prop
  ///   the property to display
  /// \param [out] out_displayString
  ///   the string buffer to write the display string to
  virtual void makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString) const = 0;

  /// \brief
  ///   Creates an editor widget for editing the passed property in a property grid.
  /// \param parent
  ///   the Qt parent widget for the property editor to be created
  /// \param prop
  ///   the property to be edited
  /// \return
  ///   the property editor that was created, or \c NULL in case no editor could be created for
  ///   the passed property.
  virtual hkvPropertyWidget* createEditor(QWidget* parent, const hkvProperty& prop) const = 0;
};


/// \brief
///   Default implementation of property display/editing support.
class hkvDefaultPropertyEditingProvider : public hkvPropertyEditingProvider
{
public:
  hkvDefaultPropertyEditingProvider() {}
private:
  hkvDefaultPropertyEditingProvider(const hkvDefaultPropertyEditingProvider&);
  hkvDefaultPropertyEditingProvider& operator=(const hkvDefaultPropertyEditingProvider&);

public:
  virtual bool canHandleProperty(const hkvProperty& prop) const HKV_OVERRIDE;
  virtual void makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString) const HKV_OVERRIDE;
  virtual hkvPropertyWidget* createEditor(QWidget* parent, const hkvProperty& prop) const HKV_OVERRIDE;
};


/// \brief
///   Allows retrieving objects providing editing support for hkvProperty objects.
class hkvPropertyEditingProviderSupport
{
private:
  hkvPropertyEditingProviderSupport() {}
  hkvPropertyEditingProviderSupport(const hkvPropertyEditingProviderSupport&);
  hkvPropertyEditingProviderSupport& operator=(const hkvPropertyEditingProviderSupport&);

public:
  /// \brief
  ///   Initializes the global instance of this class.
  /// \sa getGlobalInstance()
  ASSETBROWSER_IMPEXP static void init();

  /// \brief
  ///   De-initializes the global instance of this class. 
  ASSETBROWSER_IMPEXP static void deInit();

  /// \brief
  ///   Returns the global instance of this class.
  /// \return
  ///   the global instance of this registry, or \c NULL, if the global instance has not been
  ///   initialized.
  ASSETBROWSER_IMPEXP static hkvPropertyEditingProviderSupport* getGlobalInstance();

public:
  /// \brief
  ///   Searches for an editing provider suitable for displaying and editing the passed property.
  ///
  /// If the property does not have a custom type set, the default editing provider will always be 
  /// returned. The editing providers implemented in asset plugins are only searched when 
  /// hkvProperty::getCustomType() returns a non-empty string.
  ///
  /// \param prop
  ///   the property to display/edit
  /// \return
  ///   the provider suitable for editing the passed property; \c NULL if no suitable provider
  ///   could be found.
  ASSETBROWSER_IMPEXP const hkvPropertyEditingProvider* findSuitableProvider(const hkvProperty& prop) const;

private:
  static hkvPropertyEditingProviderSupport* s_globalInstance;

private:
  hkvDefaultPropertyEditingProvider m_defaultProvider;
};

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
