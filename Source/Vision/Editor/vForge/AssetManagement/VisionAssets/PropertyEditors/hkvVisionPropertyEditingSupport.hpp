/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_VISION_PROPERTY_EDITING_SUPPORT_HPP_INCLUDED
#define HKV_VISION_PROPERTY_EDITING_SUPPORT_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/PropertyBrowser/hkvPropertyEditingSupport.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/PropertyBrowser/hkvPropertyWidget.hpp>

/// \brief
///   Provides support for displaying and editing asset properties specific to Vision assets.
class hkvVisionPropertyEditingProvider : public hkvPropertyEditingProvider
{
public:
  hkvVisionPropertyEditingProvider() {}
private:
  hkvVisionPropertyEditingProvider(const hkvVisionPropertyEditingProvider&);
  hkvVisionPropertyEditingProvider& operator=(const hkvVisionPropertyEditingProvider&);

public:
  virtual bool canHandleProperty(const hkvProperty& prop) const HKV_OVERRIDE;
  virtual void makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString) const HKV_OVERRIDE;
  virtual hkvPropertyWidget* createEditor(QWidget* parent, const hkvProperty& prop) const HKV_OVERRIDE;

private:
  hkvPropertyWidget* createEditorLOD(QWidget* parent, const hkvProperty& prop) const;
  hkvPropertyWidget* createEditorTagfileFilterSettings(QWidget* parent, const hkvProperty& prop) const;
};


/// \brief
///   Property widget class handling LOD distances properties.
class hkvLODDistancesPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvLODDistancesPropertyWidget(QWidget* parent);
  ~hkvLODDistancesPropertyWidget();

public:
  static void makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString);

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

private slots:
  void OnButtonClicked();

private:
  QLabel* m_label;
  QToolButton* m_button;
  QHBoxLayout* m_layout;
};


/// \brief
///   Custom property widget class for handling of tagfile filter settings.
class hkvTagfileFilterSettingsPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvTagfileFilterSettingsPropertyWidget(QWidget* parent);
  ~hkvTagfileFilterSettingsPropertyWidget();

public:
  static void makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString);

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

  private slots:
    void OnButtonClicked();

private:
  QLabel* m_label;
  QToolButton* m_button;
  QHBoxLayout* m_layout;
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
