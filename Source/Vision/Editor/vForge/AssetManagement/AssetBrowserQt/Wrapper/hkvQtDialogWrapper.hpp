/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVQTDIALOGWRAPPER_HPP
#define HKVQTDIALOGWRAPPER_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
ANALYSIS_IGNORE_ALL_END

class ASSETBROWSER_IMPEXP hkvQtDialogWrapper : public QWidget
{
    Q_OBJECT
public:
    hkvQtDialogWrapper(QWidget* pParent);
    ~hkvQtDialogWrapper();

protected:
    virtual void childEvent(QChildEvent* e) HKV_OVERRIDE;
    virtual bool eventFilter(QObject* pObject, QEvent* e) HKV_OVERRIDE;

private:
    void init();
    void saveFocus();
    void restoreFocus();

    void ensureEnabled();
    void ensureDisabled();

private:
    HWND m_hParent;
    HWND m_hPreviousFocus;
    bool m_bParentDisabled;
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
