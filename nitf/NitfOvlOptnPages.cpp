// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// NitfOvlOptnPages.cpp - Implementation of the CNitfOvlOptn<General,Files,Paths>Page classes.



#include "stdafx.h"
#include "..\resource.h"
#include "NITF.h"
#include "param.h"
#include "NitfOvlOptnPages.h"
#include "factory.h"
#include "fid.h"
#include "..\getobjpr.h"



/////////////////////////////////////////////////////////////////////////////
// CNITFNoEditorWarningDlg

IMPLEMENT_DYNCREATE( CNITFNoEditorWarningDlg, CDialog )

CNITFNoEditorWarningDlg::CNITFNoEditorWarningDlg() :
         CDialog( IDD_OVL_NITF_NO_EDITOR_WARNING ),
         m_iDontShowMsgAgain( BST_UNCHECKED )
{
}

void CNITFNoEditorWarningDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP( CNITFNoEditorWarningDlg )
	DDX_Check( pDX, IDC_NOSHOWMSG_CHK, m_iDontShowMsgAgain );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CNITFNoEditorWarningDlg, CDialog )
	//{{AFX_MSG_MAP( CNITFNoEditorWarningDlg )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Message handlers

BOOL CNITFNoEditorWarningDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData( FALSE );
	return TRUE;
}
// End of CNITFNoEditorWarningDlg::OnInitDialog()

void CNITFNoEditorWarningDlg::OnOK() 
{
	UpdateData( TRUE );

	CDialog::OnOK();
}
// End of CNITFNoEditorWarningDlg::OnOK



/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnGeneralPage property page

IMPLEMENT_DYNCREATE( CNitfOvlOptnGeneralPage, COverlayPropertyPage )

CNitfOvlOptnGeneralPage::CNitfOvlOptnGeneralPage() :
   COverlayPropertyPage( FVWID_Overlay_NitfFiles, CNitfOvlOptnGeneralPage::IDD )
{
	//{{AFX_DATA_INIT(CNitfOvlOptnGeneralPage)
	//}}AFX_DATA_INIT
}

CNitfOvlOptnGeneralPage::~CNitfOvlOptnGeneralPage()
{
}

void CNitfOvlOptnGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	COverlayPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNitfOvlOptnGeneralPage)
//	DDX_Check( pDX, IDC_TOGGLE, m_toggle_overlay );
	DDX_Check( pDX, IDC_NITF_BOUNDING_AREAS, m_iShowBounds );
	DDX_Check( pDX, IDC_NITF_HIDE_DURING_SCROLL, m_iHideDuringScroll );
   DDX_Check( pDX, IDC_NITF_SECONDARY_IMAGES, m_iSecondaryImages );
   DDX_Check( pDX, IDC_OVL_NITF_GEN_NEW_DISPLAY_ONLY, m_iNewExplorerDisplayOnly );
   DDX_Check( pDX, IDC_OVL_NITF_GEN_NEW_CENTER_MAP_ALWAYS, m_iNewExplorerCenterMapAlways );
   DDX_Check( pDX, IDC_OVL_NITF_GEN_NEW_CENTER_MAP_ASK, m_iNewExplorerCenterMapAsk );
   DDX_Check( pDX, IDC_OVL_NITF_GEN_NEW_DISPLAY_TAB_ALWAYS, m_iNewExplorerNewDisplayTabAlways );
   DDX_Check( pDX, IDC_OVL_NITF_GEN_NEW_DISPLAY_TAB_ASK, m_iNewExplorerNewDisplayTabAsk );
   DDX_Check( pDX, IDC_NITF_FILLED_ONLY_NO_EDITOR, m_iImagesOnlyNoEditor );
   DDX_Check( pDX, IDC_NITF_ONLY_FILTERED_OBJECTS, m_iOnlyFilteredObjects );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNitfOvlOptnGeneralPage, COverlayPropertyPage)
	//{{AFX_MSG_MAP(CNitfOvlOptnGeneralPage)
	ON_BN_CLICKED( IDC_NITF_BOUNDING_AREAS, OnModified )
	ON_BN_CLICKED( IDC_TOGGLE, OnToggleOverlay )
   ON_BN_CLICKED( IDC_NITF_HIDE_DURING_SCROLL, OnModified )
   ON_BN_CLICKED( IDC_NITF_SECONDARY_IMAGES, OnModified )
   ON_BN_CLICKED( IDC_OVL_NITF_GEN_NEW_DISPLAY_ONLY, OnModified )
   ON_BN_CLICKED( IDC_OVL_NITF_GEN_NEW_CENTER_MAP_ALWAYS, OnModified )
   ON_BN_CLICKED( IDC_OVL_NITF_GEN_NEW_CENTER_MAP_ASK, OnModified )
   ON_BN_CLICKED( IDC_OVL_NITF_GEN_NEW_DISPLAY_TAB_ALWAYS, OnModified )
   ON_BN_CLICKED( IDC_OVL_NITF_GEN_NEW_DISPLAY_TAB_ASK, OnModified )
   ON_BN_CLICKED( IDC_NITF_FILLED_ONLY_NO_EDITOR, OnClickedImagesOnlyNoEditor )
   ON_BN_CLICKED( IDC_NITF_ONLY_FILTERED_OBJECTS, OnClickedOnlyFilteredObjects )
   ON_BN_CLICKED( IDC_NITF_RESET_ALL_DISPLAY_ADJ, OnClickedResetAllDisplayAdjustments )
	ON_CBN_SELCHANGE( IDC_LABEL_THRESHOLD, OnModified )
	ON_CBN_SELCHANGE( IDC_THRESHOLD, OnModified )
   ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Message handlers

BOOL CNitfOvlOptnGeneralPage::OnInitDialog() 
{

	// Bug 1556
	// An hourglass cursor should be added to the NITF options page in OnInitDialog
	//	since attaching to the database, when necessary, can be lengthy.
	CWaitCursor wait;

   InitVars();

   COverlayPropertyPage::OnInitDialog();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}  // End of CNitfOvlOptnGeneralPage::OnInitDialog

afx_msg VOID CNitfOvlOptnGeneralPage::OnClickedImagesOnlyNoEditor()
{
   m_bImagesOnlyNoEditorChanged = TRUE;
   OnModified();
}

afx_msg VOID CNitfOvlOptnGeneralPage::OnClickedOnlyFilteredObjects()
{
   UpdateData( TRUE );     // Get current check state

   // If checking, see if allowed
   if ( m_iOnlyFilteredObjects != BST_UNCHECKED )
   {
      do
      {
         // If was checked (now indeterminate), now unchecked
         if ( m_iOnlyFilteredObjects == BST_INDETERMINATE )
            m_iOnlyFilteredObjects = BST_UNCHECKED;

         else     // Was unchecked, now checked
         {
            if ( C_nitf_ovl::s_pNITFOvl != NULL )
            {
               if ( C_nitf_ovl::s_pNITFOvl->m_eQueryFilterDisplay
                  != C_nitf_ovl::QUERY_FILTER_DISPLAY_UNSPECIFIED )
                  break;
            }
            m_iOnlyFilteredObjects = BST_INDETERMINATE;
         }
         UpdateData( FALSE );
      } while ( FALSE );
   }

   m_bOnlyFilteredObjectsChanged = TRUE;
   OnModified();
}

static inline ULONGLONG ULL( FILETIME& ft )
{
   return (* (ULARGE_INTEGER*) &ft ).QuadPart;
}

afx_msg VOID CNitfOvlOptnGeneralPage::OnClickedResetAllDisplayAdjustments()
{
   CComBSTR ccbsMsg, ccbsTitle;
   ccbsTitle.LoadString( IDS_NITF_RESET_WARNING_TITLE );
   ccbsMsg.LoadString( IDS_NITF_RESET_WARNING );
   if ( ::MessageBoxW( NULL, (LPCWSTR) ccbsMsg, (LPCWSTR) ccbsTitle,
      MB_YESNO | MB_ICONWARNING ) == IDYES )
   {
      C_nitf_ovl::ResetAllImageDisplayAdjustments();

      // Remove any GeoRect presets and any .fid files more than one week old
      FILETIME ftNow;
      GetSystemTimeAsFileTime( &ftNow );

      CString csFIDFolder = PRM_get_registry_string( "Main", "USER_DATA" ) + "\\GeoRect\\fid";

      WIN32_FIND_DATA ffd;             // Where file info gets put

      // Search for .fid files in this folder
      HANDLE hSearch = FindFirstFileEx(
         csFIDFolder + _T("\\*.fid"),
         FindExInfoStandard,           // Information level
         &ffd,                         // Information buffer
         FindExSearchNameMatch,        // Filtering type
         NULL,                         // Search criteria
         0 );                          // Additional search control

      // Look up the file info
      if ( hSearch != INVALID_HANDLE_VALUE )
      {  
         do    // For all .fid files in the folder
         {
            do // Once
            {
               if ( ( ffd.dwFileAttributes &  // Reject hidden, system, etc. files
                  ( FILE_ATTRIBUTE_ENCRYPTED
                     | FILE_ATTRIBUTE_HIDDEN
                     | FILE_ATTRIBUTE_OFFLINE
                     | FILE_ATTRIBUTE_SYSTEM
                     | FILE_ATTRIBUTE_DIRECTORY    // Reject directories with that extension (??)
                     | FILE_ATTRIBUTE_TEMPORARY ) ) != 0 )
                  break;

               CString csFIDFilespec = csFIDFolder + _T("\\") + ffd.cFileName;

               if ( ( ULL( ftNow ) - ULL( ffd.ftLastAccessTime ) )
                  < ( 7 * 24 * 60 * 60 * 10000000ULL ) ) // Less than one week since accessed
               {

                  // Remove the display parameters
                  CFid fid;
                  if ( SUCCESS == fid.open( csFIDFilespec ) )
                  {
                     static const LPCSTR KeyList[] =
                     {
                        FID_CS_TYPE,
                        FID_CS_TYPE_STR,
                        FID_CONTRAST,
                        FID_POSITION,
                        NULL
                     };
                     const LPCSTR* pKeys = KeyList;

                     BOOL bChanged = FALSE;
                     do
                     {
                        if ( SUCCESS ==  fid.remove_key( *pKeys++ ) )
                           bChanged = TRUE;
                     } while ( *pKeys != NULL );

                     // If changed and not empty, save it
                     if ( !fid.m_list.empty() )
                     {
                        if ( bChanged )
                           fid.save();
                        break;
                     }
                     // If there is nothing left, delete the file

                  }  // Couldn't read it (corrupted?) or now empty.  Kill it.
               }

               DeleteFile( csFIDFilespec );     // Discard .fid file completely
            } while ( FALSE );

         } while ( FindNextFile( hSearch, &ffd ) != 0 );

         FindClose( hSearch );
         hSearch = INVALID_HANDLE_VALUE;
      }  // End of .fid file cleanup

      InitVars();
      UpdateData( FALSE );
      OnModified();
   }
}  // End of OnClickedResetAllDisplayAdjustments()

void CNitfOvlOptnGeneralPage::OnOK() 
{
	UpdateData( TRUE );

   // Check for new images-only if no editor option
   if ( GetRegistryValue( "ImagesOnlyNoEditorWarning", TRUE )
      && m_iImagesOnlyNoEditor != BST_UNCHECKED
      && m_iImagesOnlyNoEditorOriginal == BST_UNCHECKED )
   {
      CNITFNoEditorWarningDlg dlg;
      if ( IDOK != dlg.DoModal() )
         m_iImagesOnlyNoEditor = BST_UNCHECKED;

      SetRegistryValue( "ImagesOnlyNoEditorWarning", dlg.m_iDontShowMsgAgain == BST_UNCHECKED );
   }

	// Icon/frame display threshold
	CString cs = GetRegistryValue( "NITFDisplayAbove", "1:5 M" );
	if ( m_display_threshold.Compare( cs ) != 0 )
		SetRegistryValue( "NITFDisplayAbove", m_display_threshold );
	
	// Label display threshold
	cs = GetRegistryValue( "NITFLabelThreshold", "1:250 K" );
	if ( m_label_threshold.Compare( cs ) != 0 )
		SetRegistryValue( "NITFLabelThreshold", m_label_threshold );
	
   // Image "showing"
   SetRegistryCheckValue( "ShowBounds", m_iShowBounds );
	SetRegistryCheckValue( "HideDuringScroll", m_iHideDuringScroll );
   SetRegistryCheckValue( "ShowSecondaryImages", m_iSecondaryImages );

   // Query Tool
   SetRegistryCheckValue( "ImagesOnlyNoEditor", m_iImagesOnlyNoEditor );
   SetRegistryCheckValue( "OnlyFilteredObjects", m_iOnlyFilteredObjects );

   // New explorer drag-drop or command-line images
   if ( m_iNewExplorerCenterMapAlways != BST_UNCHECKED )
      SetRegistryValue( "NewExplorerImages", "CenterMapAlways" );
   else if ( m_iNewExplorerCenterMapAsk != BST_UNCHECKED )
      SetRegistryValue( "NewExplorerImages", "CenterMapAsk" );
   else if ( m_iNewExplorerNewDisplayTabAlways != BST_UNCHECKED )
      SetRegistryValue( "NewExplorerImages", "NewDisplayTabAlways" );
   else if ( m_iNewExplorerNewDisplayTabAsk != BST_UNCHECKED )
      SetRegistryValue( "NewExplorerImages", "NewDisplayTabAsk" );
   else
      SetRegistryValue( "NewExplorerImages", "DisplayOnly" );

   // Update NITF overlay
   if ( C_nitf_ovl::s_pNITFOvl != NULL )
   {
      if ( m_bImagesOnlyNoEditorChanged )
         C_nitf_ovl::s_pNITFOvl->m_eImagesOnlyNoEditor =
            m_iImagesOnlyNoEditor != BST_UNCHECKED
               ? C_nitf_ovl::IMAGES_ONLY_NO_EDITOR_TRUE
               : C_nitf_ovl::IMAGES_ONLY_NO_EDITOR_FALSE;

      if ( m_bOnlyFilteredObjectsChanged
         && C_nitf_ovl::s_pNITFOvl->m_eQueryFilterDisplay
            != C_nitf_ovl::QUERY_FILTER_DISPLAY_UNSPECIFIED )
         C_nitf_ovl::s_pNITFOvl->m_eQueryFilterDisplay =
            m_iOnlyFilteredObjects == BST_UNCHECKED
               ? C_nitf_ovl::QUERY_FILTER_DISPLAY_FALSE
               : C_nitf_ovl::QUERY_FILTER_DISPLAY_TRUE;

      C_nitf_ovl::s_pNITFOvl->m_bShowSecondaryImages =
         m_iSecondaryImages != BST_UNCHECKED;

      C_nitf_ovl::s_pNITFOvl->m_eNewExplorerInitialImageDisplay =
         ( m_iNewExplorerCenterMapAlways != BST_UNCHECKED )
            ? C_nitf_ovl::NEW_EXPLORER_IMAGE_CENTER_MAP_ALWAYS
         :( m_iNewExplorerCenterMapAsk != BST_UNCHECKED )
            ? C_nitf_ovl::NEW_EXPLORER_IMAGE_CENTER_MAP_ASK
         : ( m_iNewExplorerNewDisplayTabAlways != BST_UNCHECKED )
            ? C_nitf_ovl::NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ALWAYS
         : ( m_iNewExplorerNewDisplayTabAsk != BST_UNCHECKED )
            ? C_nitf_ovl::NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ASK
         : C_nitf_ovl::NEW_EXPLORER_IMAGE_DISPLAY_ONLY;
   }


   C_nitf_ovl::s_bParamsUpdate = TRUE;

	COverlayPropertyPage::OnOK();
}  // End of OnOK()


VOID CNitfOvlOptnGeneralPage::InitVars() 
{
  // Icon/frame display threshold
   m_display_threshold = GetRegistryValue( "NITFDisplayAbove", "1:5 M" );

   // Label threshold
   m_label_threshold = GetRegistryValue( "NITFLabelThreshold", "1:250 K" );

   // Image "showing"
	m_iShowBounds = GetRegistryCheckValue( "ShowBounds", BST_CHECKED );
	m_iHideDuringScroll = GetRegistryCheckValue( "HideDuringScroll", BST_CHECKED );
   m_iSecondaryImages = GetRegistryCheckValue( "ShowSecondaryImages", BST_UNCHECKED );

   CString cs = GetRegistryValue( "NewExplorerImages", "CenterMap" );
   m_iNewExplorerCenterMapAlways =
      ( 0 == cs.CompareNoCase( "CenterMapAlways" ) )
      ? BST_CHECKED : BST_UNCHECKED;
   m_iNewExplorerCenterMapAsk =
      ( 0 == cs.CompareNoCase( "CenterMapAsk" ) || 0 == cs.CompareNoCase( "CenterMap" ) )
      ? BST_CHECKED : BST_UNCHECKED;
   m_iNewExplorerNewDisplayTabAlways =
      ( 0 == cs.CompareNoCase( "NewDisplayTabAlways" ) )
      ? BST_CHECKED : BST_UNCHECKED;
   m_iNewExplorerNewDisplayTabAsk =
      ( 0 == cs.CompareNoCase( "NewDisplayTabAsk" ) || 0 == cs.CompareNoCase( "NewDisplayTab" ) )
      ? BST_CHECKED : BST_UNCHECKED;
   m_iNewExplorerDisplayOnly =
      ( m_iNewExplorerCenterMapAlways == BST_UNCHECKED
         && m_iNewExplorerCenterMapAsk == BST_UNCHECKED
         && m_iNewExplorerNewDisplayTabAlways == BST_UNCHECKED
         && m_iNewExplorerNewDisplayTabAsk == BST_UNCHECKED )
      ? BST_CHECKED : BST_UNCHECKED;

   // Query Tool
   m_iImagesOnlyNoEditorOriginal = m_iImagesOnlyNoEditor =
      GetRegistryCheckValue( "ImagesOnlyNoEditor", BST_UNCHECKED );
   m_bImagesOnlyNoEditorChanged = FALSE;

   m_iOnlyFilteredObjects = GetRegistryCheckValue( "OnlyFilteredObjects", BST_UNCHECKED );
   do
   {
      if ( m_iOnlyFilteredObjects == BST_UNCHECKED )
         break;

      if ( C_nitf_ovl::s_pNITFOvl != 0 )
      {
         if ( C_nitf_ovl::s_pNITFOvl->m_eQueryFilterDisplay !=
            C_nitf_ovl::QUERY_FILTER_DISPLAY_UNSPECIFIED )
            break;
      }
      m_iOnlyFilteredObjects = BST_INDETERMINATE;
   } while ( FALSE );
   m_bOnlyFilteredObjectsChanged = FALSE;

}  // End of CNitfOvlOptnGeneralPage::OnInitDialog


CString CNitfOvlOptnGeneralPage::GetRegistryValue( LPCSTR pszKeyName, LPCSTR pszDefault )
{
   return PRM_get_registry_string( "NitfFile", pszKeyName, pszDefault );
}

BOOL CNitfOvlOptnGeneralPage::GetRegistryValue( LPCSTR pszKeyName, BOOL bDefault )
{
   CString cs = GetRegistryValue( pszKeyName, bDefault ? "Y" : "N" );
   return cs.CompareNoCase( "Y" ) == 0;
}

BOOL CNitfOvlOptnGeneralPage::GetRegistryCheckValue( LPCSTR pszKeyName, INT iDefault )
{
   return GetRegistryValue( pszKeyName, iDefault != BST_UNCHECKED )
                              ? BST_CHECKED : BST_UNCHECKED;
}

VOID CNitfOvlOptnGeneralPage::SetRegistryValue( LPCSTR pszKeyName, LPCSTR pszValue )
{
   PRM_set_registry_string( "NitfFile", pszKeyName, pszValue );
}


VOID CNitfOvlOptnGeneralPage::SetRegistryValue( LPCSTR pszKeyName, BOOL bValue )
{
   SetRegistryValue( pszKeyName, bValue ? "Y" : "N" );
}

VOID CNitfOvlOptnGeneralPage::SetRegistryCheckValue( LPCSTR pszKeyName, INT iValue )
{
   SetRegistryValue( pszKeyName, iValue != BST_UNCHECKED ? "Y" : "N" );
}


void CNitfOvlOptnGeneralPage::OnDestroy()
{
   if ( C_nitf_ovl::s_pNITFOvl != NULL )
      C_nitf_ovl::s_pNITFOvl->m_bReload = TRUE;

	OVL_get_overlay_manager()->invalidate_all( FALSE );
	
}  // End of CNitfOvlOptnGeneralPage::OnDestory()



/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnFilesPage property page

IMPLEMENT_DYNCREATE (CNitfOvlOptnFilesPage, COverlayPropertyPage )

CNitfOvlOptnFilesPage::CNitfOvlOptnFilesPage() :
   COverlayPropertyPage( FVWID_Overlay_NitfFiles, CNitfOvlOptnFilesPage::IDD )
{
  m_dwSourcesDlgEventsCookie = 0;
  EnableAutomation();
	//{{AFX_DATA_INIT(CNitfOvlOptnFilesPage)
	//}}AFX_DATA_INIT
}

CNitfOvlOptnFilesPage::~CNitfOvlOptnFilesPage()
{
}

void CNitfOvlOptnFilesPage::DoDataExchange( CDataExchange* pDX )
{
	COverlayPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNitfOvlOptnFilesPage)
	DDX_Control( pDX, IDC_NITF_SOURCES_FILES_DLG, m_NITF_files );
	//}}AFX_DATA_MAP
}

BEGIN_DISPATCH_MAP( CNitfOvlOptnFilesPage, CDialog )
	DISP_FUNCTION( CNitfOvlOptnFilesPage, "OnHelp", OnHelp, VT_EMPTY, VTS_I4 )
   DISP_FUNCTION( CNitfOvlOptnFilesPage, "OnNotifyChange", OnNotifyChange, VT_EMPTY, VTS_NONE )
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP( CNitfOvlOptnFilesPage, CDialog )
	INTERFACE_PART( CNitfOvlOptnFilesPage, DIID__INITFSourcesDlgEvents, Dispatch )
END_INTERFACE_MAP()

BEGIN_MESSAGE_MAP( CNitfOvlOptnFilesPage, COverlayPropertyPage )
	//{{AFX_MSG_MAP(CNitfOvlOptnFilesPage)
   ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Message handlers

BOOL CNitfOvlOptnFilesPage::OnInitDialog() 
{
	COverlayPropertyPage::OnInitDialog();

	// Bug 1556
	// An hourglass cursor should be added to the NITF options page in OnInitDialog
	//	since attaching to the database, when necessary, can be lengthy.
	CWaitCursor wait;


	CString sdata = PRM_get_registry_string( "Main", "USER_DATA" ) + "\\NITF";

#if 1
   BOOL b = AfxConnectionAdvise(
      m_NITF_files.GetControlUnknown(),
      DIID__INITFSourcesDlgEvents,
      GetIDispatch( FALSE ),
      FALSE,
      &m_dwSourcesDlgEventsCookie );
   ASSERT( b );
#endif

   m_NITF_files.SetControlMode( SOURCES_CTRL_CONTROL_MODE_SINGLE_FILES );
	m_NITF_files.Initialize( NULL, NULL, sdata ); 

	UpdateData( FALSE );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}  // End of CNitfOvlOptnFilesPage::OnInitDialog

void CNitfOvlOptnFilesPage::OnOK() 
{
	UpdateData(TRUE);

	COverlayPropertyPage::OnOK();
}  // End of CNitfOvlOptnFilesPage::OnOK()


void CNitfOvlOptnFilesPage::OnDestroy()
{
   if ( C_nitf_ovl::s_pNITFOvl != NULL )
      C_nitf_ovl::s_pNITFOvl->m_bReload = TRUE;

  if ( m_dwSourcesDlgEventsCookie != 0 )
   {
      BOOL b = AfxConnectionUnadvise(
         m_NITF_files.GetControlUnknown(),
         DIID__INITFSourcesDlgEvents,
         GetIDispatch( FALSE ),
         FALSE,
         m_dwSourcesDlgEventsCookie );
      ASSERT( b );
      m_dwSourcesDlgEventsCookie = 0;
   }

	OVL_get_overlay_manager()->invalidate_all( FALSE );
	
}  // End of CNitfOvlOptnFilesPage::OnDestory()



// This event gets fired when the user clicks the Help button in the 
// NITF IAS/JPIP single-files dialog.  0 is for general help
afx_msg VOID CNitfOvlOptnFilesPage::OnHelp( LONG lHelpType )
{
   TRACE( _T("CNitfOvlOptnFilesPage:OnHelp( %d )\n"),
      lHelpType );

#if 1
   switch ( lHelpType )
   {
      case 98:    // Temp for JPIP only
      case 99:
	      AfxGetApp()->WinHelp( HID_BASE_RESOURCE + IDD_NITF_JPIP_SOURCES );
   }
#else
   DWORD dwData;
   switch ( lCurrentTab )
   {
      case 1:
         dwData = HID_BASE_RESOURCE + IDD_NITF_QUERY_SOURCES_TAB;
         break;
      case 2:
         dwData = HID_BASE_RESOURCE + IDD_NITF_QUERY_QUERY_TAB;
         break;
      case 3:
         dwData = HID_BASE_RESOURCE + IDD_NITF_QUERY_SAVE_RSTR_TAB;
         break;
      case 4:
         dwData = HID_BASE_RESOURCE + IDD_NITF_QUERY_OPTIONS_TAB;
         break;
      case 0:
      default:
         dwData = HID_BASE_RESOURCE + IDD_NITF_QUERY;
         break;
   }
	AfxGetApp()->WinHelp( dwData );
#endif
}  // End of OnHelp()


//
// OnNotifyChange() - query dialog UI conditions change
//
VOID CNitfOvlOptnFilesPage::OnNotifyChange()
{
}  // End of OnNotifyChange()


/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnPathsPage property page

IMPLEMENT_DYNCREATE( CNitfOvlOptnPathsPage, COverlayPropertyPage )

CNitfOvlOptnPathsPage::CNitfOvlOptnPathsPage() :
   COverlayPropertyPage( FVWID_Overlay_NitfFiles, CNitfOvlOptnPathsPage::IDD )
{
	//{{AFX_DATA_INIT(CNitfOvlOptnPathsPage)
	//}}AFX_DATA_INIT
}

CNitfOvlOptnPathsPage::~CNitfOvlOptnPathsPage()
{
}

void CNitfOvlOptnPathsPage::DoDataExchange( CDataExchange* pDX )
{
	COverlayPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNitfOvlOptnPathsPage)
	DDX_Control( pDX, IDC_NITF_SOURCES_PATHS_DLG, m_NITF_paths );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CNitfOvlOptnPathsPage, COverlayPropertyPage )
	//{{AFX_MSG_MAP(CNitfOvlOptnPathsPage)
   ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Messageessage handlers

BOOL CNitfOvlOptnPathsPage::OnInitDialog() 
{
	COverlayPropertyPage::OnInitDialog();

	// Bug 1556
	// An hourglass cursor should be added to the NITF options page in OnInitDialog
	//	since attaching to the database, when necessary, can be lengthy.
	CWaitCursor wait;

	CString sdata = PRM_get_registry_string( "Main", "USER_DATA" );
	sdata += "\\NITF";

   m_NITF_paths.SetControlMode( SOURCES_CTRL_CONTROL_MODE_PATHS );
	m_NITF_paths.Initialize( NULL, NULL, sdata ); 

	UpdateData( FALSE );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}  // End of CNitfOvlOptnPathsPage::OnInitDialog


void CNitfOvlOptnPathsPage::OnOK() 
{
	UpdateData(TRUE);

 	COverlayPropertyPage::OnOK();
}  // End of CNitfOvlOptnPathsPage::OnOK()


void CNitfOvlOptnPathsPage::OnDestroy()
{
   if ( C_nitf_ovl::s_pNITFOvl != NULL )
      C_nitf_ovl::s_pNITFOvl->m_bReload = TRUE;

	OVL_get_overlay_manager()->invalidate_all( FALSE );
	
}  // End of CNitfOvlOptnPathsPage::OnDestory()

// End of CNitfOvlOptnPages.cpp
LRESULT CNITFNoEditorWarningDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

