/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_CUSTOM_PROGRESSBAR_HPP_INCLUDED
#define HKV_CUSTOM_PROGRESSBAR_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QLabel>
ANALYSIS_IGNORE_ALL_END

class hkvCustomProgressBar : public QLabel
{
  Q_OBJECT
public:
  struct Section
  {
    Section() : m_iValue(0) { }
    Section(const QColor& barColor, hkUint32 iValue) : m_barColor(barColor), m_iValue(iValue) {}

    QColor m_barColor;
    hkUint32 m_iValue;
  };
  typedef std::vector<Section> SectionVector;

  ASSETBROWSER_IMPEXP hkvCustomProgressBar(QWidget* parent);
  ASSETBROWSER_IMPEXP ~hkvCustomProgressBar();
    
  ASSETBROWSER_IMPEXP void SetSections(const SectionVector& sections);
  ASSETBROWSER_IMPEXP const SectionVector& GetSections();

signals:
  void doubleClicked(void);

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent* event) HKV_OVERRIDE;
  virtual void paintEvent(QPaintEvent* e) HKV_OVERRIDE;

private:
  SectionVector m_sections;
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
