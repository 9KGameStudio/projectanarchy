/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiError.hpp

#ifndef _VISIONAPIERROR_
#define _VISIONAPIERROR_


class VisReportCollector_cl;

/// \brief
///   Class for error messages, warnings and system messages. Can be accessed via Vision::Error
/// 
/// Custom error messages can be triggered via this class.
/// 
/// Furthermore, internal error messages sent by the engine can be redirected via callbacks.
class VisError_cl
{
public:
  void Init();
  void DeInit();

  ///
  /// @name Debug Reporting System
  /// @{
  ///


  /// \brief
  ///   Sets the status for the error/warning report dialog
  /// 
  /// The bug/warning report dialog appears after Vision::InitWorld() and after Vision::DeInitWorld()  (If
  /// any warning or error occurred)
  /// 
  /// The errors are grouped by error type such as missing textures, missing entity classes etc.
  /// 
  /// \param bEnabled
  ///   The new status: If always set to FALSE, the dialog will never be shown
  /// 
  /// \note
  ///   The default status is TRUE
  /// 
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC void SetReportStatus(bool bEnabled) { m_Report.SetReportStatus(bEnabled); }

  /// \brief
  ///   Returns whether status reports are currently enabled.
  ///
  /// \sa SetReportStatus
  VISION_APIFUNC bool GetReportStatus() const { return m_Report.GetReportStatus(); }

  /// \brief
  ///   Deletes all report entries that accumulated since the last time the report dialog has been
  ///   displayed.
  /// 
  /// \note
  ///   This function will be called each time after the dialog has been shown.
  /// 
  /// \note
  ///   However, this function should not be called by the user.
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC void ResetReportEntries();

  /// \brief
  ///   Adds a single, un-grouped entry to the report dialog
  /// 
  /// This entry will be listed in the report dialog the next time it is shown.
  /// 
  /// \param reportType
  ///   Type of the entry; can be VIS_REPORTENTRY_INFO, VIS_REPORTENTRY_WARNING or
  ///   VIS_REPORTENTRY_ERROR
  /// 
  /// \param szInfo
  ///   Info string that appears in the list
  /// 
  /// \param szDescr
  ///   Info string that appears in the description box when the error is selected in the dialog
  /// 
  /// \param szDetailed
  ///   Info string that appears in the detailed box when the error is selected
  /// 
  /// \note
  ///   This Report will not be added to an error/warning group, but appear as a single list
  ///   element
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  /// 
  /// \example
  ///   \code
  ///   char detailed[256];
  ///   sprintf(detailed,"The following file is missing : '%s'", szFilename);
  ///   Vision::Error.AddReportEntry(VIS_REPORTENTRY_ERROR,  "File(s) not found", "At least one file could not be opened or does not exist!", detailed);
  ///   \endcode
  VISION_APIFUNC void AddReportEntry(VisReportEntryType_e reportType, const char *szInfo, const char *szDescr, const char *szDetailed);

  /// \brief
  ///   Adds a report entry to a predefined group (missing texture,...)
  /// 
  /// In the report list, each group only appears once and lists all members added in the detailed
  /// box.
  /// 
  /// \param type
  ///   The error/warning group
  /// 
  /// \param szSetName
  ///   The member string (e.g. single filename) that should be added to the group list
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  /// 
  /// \example
  ///   \code
  ///   Vision::Error.AddReportGroupEntry( VIS_REPORTGROUPTYPE_FILE_NOT_FOUND, "myFile.dat" );
  ///   \endcode
  VISION_APIFUNC void AddReportGroupEntry(VisReportGroupType_e type, const char *szSetName);

  /// \brief
  ///   Shows the error/warning report dialog
  /// 
  /// (only if it has any entries).
  /// 
  /// This dialog will automatically be shown after Vision::InitWorld() and after Vision::DeInitWorld() (if
  /// enabled by SetReportStatus, which is default)
  /// 
  /// \param bCheckStatus
  ///   If TRUE, the dialog will only be opened if it is activated by the SetReportStatus flag. If
  ///   FALSE, the dialog will be shown regardless of the status flag.
  /// 
  /// \return
  ///   \c TRUE, if the dialog has been shown, FALSE if the report status flag is disabled or
  ///   the dialog is empty 
  /// 
  /// \note
  ///   If the dialog is empty, it will not be shown and the function returns FALSE
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC BOOL ShowReportDialog(BOOL bCheckStatus = FALSE);

  ///
  /// @}
  ///

  

  

private:

  VReport m_Report;

  void AddEntriesToReport(VReport &report);

  static int CompareEntries(const void* entr1, const void *entr2);
  VPList m_reportList;
};

#endif

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
