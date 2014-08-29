/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMapLookupControl.hpp

#ifndef VTREECONTROL_HPP_INCLUDED
#define VTREECONTROL_HPP_INCLUDED

class VTreeViewItem;
class VTreeViewControl;
class VTreeViewCollapseButton;

/// \brief
///   Collection class for items in a tree control
class VTreeViewItemCollection : public VRefCountedCollection<VTreeViewItem>
{
public:
  VTreeViewItemCollection() {m_pParentItem=NULL;}
  GUI_IMPEXP void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  GUI_IMPEXP void MeasureItems(const VRectanglef &area, hkvVec2 &vCurrentPos);
  GUI_IMPEXP VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse);

  GUI_IMPEXP void Add(VTreeViewItem *pItem);
  GUI_IMPEXP void Remove(VTreeViewItem *pItem);
  GUI_IMPEXP void RemoveAt(int iIndex);
  GUI_IMPEXP void Clear();
  GUI_IMPEXP void SetCollapsed(bool bStatus, bool bRecursive=false);

  GUI_IMPEXP void SerializeX( VArchive &ar, VTreeViewControl *pOwner);
  GUI_IMPEXP bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite, VTreeViewControl *pOwner);
  VTreeViewItem *m_pParentItem;
};

/// \brief
///   Represents a single tree view item. It is not a control, but instead holds an arbitrary item to render
class VTreeViewItem : public VRefCounter, public VUserDataObj
{
public:
  /// \brief
  ///   Constructs a new tree view item and associates it with a control to render (e.g. a text label). Does not add it to any child list
  GUI_IMPEXP VTreeViewItem(VTreeViewControl *pOwner, VDlgControlBase *pControl);
  GUI_IMPEXP virtual ~VTreeViewItem();

  /// \brief
  ///   Indicates whether this item is collapsed
  inline bool IsCollapsed() const {return m_bCollapsed;}

  /// \brief
  ///   Sets the collapsed state of this item
  GUI_IMPEXP void SetCollapsed(bool bStatus, bool bRecursive=false);

  // overridden functions
  GUI_IMPEXP void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  GUI_IMPEXP void MeasureItem(const VRectanglef &area, hkvVec2 &vCurrentPos);
  GUI_IMPEXP VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse);

  VTreeViewControl *m_pOwner;
  VTreeViewItem *m_pParentItem;
  VSmartPtr<VTreeViewCollapseButton> m_spCollapseButton;
  VDlgControlBasePtr m_spControl;
  VTreeViewItemCollection m_Items;
  bool m_bCollapsed, m_bClipped;
};


/// \brief
///   Represents a tree view control. Use m_Items.Add to add new items in root level
class VTreeViewControl : public VDlgControlBase
{
public:
  GUI_IMPEXP VTreeViewControl();
  GUI_IMPEXP virtual ~VTreeViewControl();

  /// \brief
  ///   Collapse all items at root level or optionally recursively
  inline void CollapseAll(bool bRecursive)   {m_Items.SetCollapsed(true, bRecursive);}

  /// \brief
  ///   Uncollapse all items at root level or optionally recursively
  inline void UnCollapseAll(bool bRecursive) {m_Items.SetCollapsed(false, bRecursive);}

  /// \brief
  ///   Jump to vertical offset (in pixels)
  GUI_IMPEXP void SetScrollPosition(float fPos);

  /// \brief
  ///   Internal function to notify that item layout has to be re-evaluated
  inline void FlagForMeasure() 
  {
    m_bNeedsMeasure = true;
  }

// overridden functions
  GUI_IMPEXP virtual void DisposeObject() HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnSizeChanged() HKV_OVERRIDE;

// serialization
  V_DECLARE_SERIAL_DLLEXP( VTreeViewControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

public:
  VColorRef m_iBackgroundCol;
  VImageStates m_CollapsedButtonStyle, m_UnCollapsedButtonStyle; // States that are used to render the collapse button
  VTreeViewItemCollection m_Items;  ///< List of root level items
  float m_fIndentChildren;          ///< Amount of pixels to indent between parent/child
  float m_fIndentCollapseButton;    ///< Amount of pixels to indent between collapse button and item

protected:
 
  void TestBuild();
  void MeasureItems();

  bool m_bNeedsMeasure;
  VSliderControlPtr m_spVScrollbar;
  float m_fCurrentScrollPos;

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
