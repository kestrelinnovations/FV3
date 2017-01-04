// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// NITF_QueryDlg.cpp : implementation file




#include "stdafx.h"
#include "nitf.h"
#include "NITF_QueryDlg.h"
#include "..\MapView.h"
#include "factory.h"
#include "param.h"
#include "..\fvw.h"
#include "..\getobjpr.h"
#include "..\MapEngineCOM.h"

extern CFVApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CNITF_QueryDlg dialog


BEGIN_DISPATCH_MAP( CNITF_QueryDlg, CDialog )
   DISP_FUNCTION( CNITF_QueryDlg, "OnHelp", OnHelp, VT_EMPTY, VTS_I4 VTS_I4 )
   DISP_FUNCTION( CNITF_QueryDlg, "OnNotifyChange", OnNotifyChange, VT_EMPTY, VTS_NONE )
   DISP_FUNCTION( CNITF_QueryDlg, "OnUpdateMapDisplay", OnUpdateMapDisplay, VT_EMPTY, VTS_UI4 VTS_VARIANT )
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP( CNITF_QueryDlg, CDialog )
   INTERFACE_PART( CNITF_QueryDlg, DIID__INITFQueryDlgEvents, Dispatch )
END_INTERFACE_MAP()

BEGIN_MESSAGE_MAP( CNITF_QueryDlg, CDialog )
   //{{AFX_MSG_MAP(CNITF_QueryDlg)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_ACTIVATE()
   ON_WM_NCACTIVATE()
   ON_WM_MOUSEACTIVATE()
   ON_WM_SYSCOMMAND()
   ON_WM_DESTROY()
   // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
   ON_MESSAGE( WM_EXITSIZEMOVE, OnExitSizeMove )
END_MESSAGE_MAP()


////////////////////////////////
// Construct/destruct
////////////////////////////////

CNITF_QueryDlg::CNITF_QueryDlg( CWnd* pParent /*=NULL*/ ) :
   CDialog( CNITF_QueryDlg::IDD, pParent )
{
   //{{AFX_DATA_INIT(CNITF_QueryDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   m_bInitializeDone = FALSE;
}

BOOL CNITF_QueryDlg::Initialize()
{
   static BOOL bOnce = FALSE;
   HRESULT hr = S_OK;

   if ( !bOnce )
   {
      bOnce = TRUE;

      m_dwQueryDlgEventsCookie = 0;
      EnableAutomation();

      hr = m_pxdQueryToolConfiguration.CreateInstance(
         __uuidof( MSXML2::DOMDocument60 ), NULL, CLSCTX_INPROC_SERVER );
      if ( SUCCEEDED( hr ) )
      {
         // Make a temporary query tool to read the configuration
         INITFQueryDlgPtr pQueryDlg;
         pQueryDlg.CreateInstance( CLSID_NITFQueryDlg, NULL, CLSCTX_INPROC_SERVER );

         m_lEnableQueryEditor = SW_SHOW;  // In case can't find
         m_lEnableMetadataList = SW_SHOWNA;

         _variant_t var;
         hr = pQueryDlg->SpecialFunction( QUERY_DLG_SPECFUNC_GET_CONFIGURATION,
            VARIANT_NULL, VARIANT_NULL, VARIANT_NULL, &var, NULL );
         if ( SUCCEEDED( hr ) && var.vt == VT_BSTR )
         {
            try
            {
               m_pxdQueryToolConfiguration->loadXML( var.bstrVal );

               // Set Query Tool startup conditions
               MSXML2::IXMLDOMElementPtr pxe = m_pxdQueryToolConfiguration->
                  selectSingleNode( L"/NITFQueryToolConfiguration/UIWindowInitialVisibility" );
               if ( pxe != NULL )
               {
                  var.Clear();
                  if ( S_OK == pxe->raw_getAttribute( L"QueryEditor", &var ) )
                     m_lEnableQueryEditor = (INT) var != 0 ? SW_SHOW : SW_HIDE;

                  var.Clear();
                  if ( S_OK == pxe->raw_getAttribute( L"MetadataList", &var ) )
                  {
                     if ( (INT) var == 0 )
                        m_lEnableMetadataList = SW_HIDE;
                     else if ( (INT) var == 2 )
                        m_lEnableMetadataList = SW_SHOW_ON_FIRST_AOI;
                  }
               
                  // Default edit mode is EDIT_MODE_SELECT
                  var.Clear();
                  if ( S_OK == pxe->raw_getAttribute( L"CursorMode", &var ) )
                  {
                     if ( (INT) var == 1 )
                        C_nitf_ovl::s_eEditMode = C_nitf_ovl::EDIT_MODE_AREA;
                  }
               }  // pxe != NULL
            }
            catch ( ... )
            {
               ATLASSERT( FALSE );
               hr = E_FAIL;
            }
         }
      }
      ATLASSERT( SUCCEEDED( hr ) );
   }
   return SUCCEEDED( hr );
}


///////////////////////////////////////////
// MFC
///////////////////////////////////////////

//
// DoDataExchange()
//
VOID CNITF_QueryDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CNITF_QueryDlg)
   // NOTE: the ClassWizard will add DDX and DDV calls here
   DDX_Control(pDX, IDC_NITF_QUERY_DLG, m_ctlNITFQueryDlg );
   //}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// CNITF_QueryDlg message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CNITF_QueryDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   if ( C_nitf_ovl::s_piiNITFSystemIcon == NULL )
      C_nitf_ovl::s_piiNITFSystemIcon = CIconImage::load_images( "ovlmgr\\nitf.ico" );
   SetIcon( C_nitf_ovl::s_piiNITFSystemIcon->get_icon( 16 ), FALSE );

   RECT rc;
   GetWindowRect( &rc );
   m_ptMinDlgSize.x = rc.right - rc.left;
   m_ptMinDlgSize.y = rc.bottom - rc.top;

   GetClientRect( &rc );
   CWnd* pWnd = GetDlgItem( IDC_NITF_QUERY_DLG );
   if ( pWnd != NULL )
      pWnd->MoveWindow( rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top );

   m_ctlNITFQueryDlg.SetControlMode( SOURCES_CTRL_CONTROL_MODE_QUERY_TOOL );
   m_ctlNITFQueryDlg.SpecialFunction( QUERY_DLG_SPECFUNC_SET_PARENT_WINDOW_HANDLES,
      _variant_t( (ULONG_PTR) theApp.m_pMainWnd->m_hWnd ), _variant_t( (ULONG_PTR) m_hWnd ),
      VARIANT_NULL, NULL, NULL );

   BOOL b = AfxConnectionAdvise(
      m_ctlNITFQueryDlg.GetControlUnknown(),
      DIID__INITFQueryDlgEvents,
      GetIDispatch( FALSE ),
      FALSE,
      &m_dwQueryDlgEventsCookie );
   ASSERT( b );

   m_ctlNITFQueryDlg.Initialize( NULL, NULL,
      PRM_get_registry_string( "Main", "USER_DATA" ) + L"\\NITF" );

   // Set the size and position from last time
   UpdateQueryToolConfiguration();
   MSXML2::IXMLDOMNodePtr pNode;
   if ( S_OK == m_pxdQueryToolConfiguration->raw_selectSingleNode(
      L"./NITFQueryToolConfiguration/QueryDialogWindow", &pNode ) )
   {
      if ( 4 == swscanf_s( (LPCWSTR) pNode->text, L"%d,%d,%d,%d",
         &rc.left, &rc.top, &rc.right, &rc.bottom ) )
         SetWindowPos( NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE );
   }

   m_bInitializeDone = TRUE;
   SetQueryGeoRegions();   // Current
#if 0
   m_ctlNITFQueryDlg.GetInitialEnableCtrl( &m_lEnableQueryEditor, &m_lEnableMetadataList );
#endif
   ShowWindow( C_nitf_ovl::s_bEditOn ? m_lEnableQueryEditor : SW_HIDE );

   return TRUE;
}

afx_msg VOID CNITF_QueryDlg::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
   CWnd::OnActivate( nState, pWndOther, bMinimized );
   OnActivate1( (WPARAM) nState,
      (LPARAM) ( pWndOther != NULL ? pWndOther->GetSafeHwnd() : 0 ) );
}


afx_msg BOOL CNITF_QueryDlg::OnNcActivate( BOOL bActive )
{
   CWnd::OnNcActivate( bActive );
   OnActivate1( (WPARAM) bActive, (LPARAM) 0 );
   return TRUE;
}


afx_msg INT CNITF_QueryDlg::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message )
{
   CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
   OnActivate2( (WPARAM) 0, (LPARAM) 0 );
   return MA_ACTIVATE;
}


afx_msg VOID CNITF_QueryDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
   OnActivate2( (WPARAM) 0, lParam );
   CWnd::OnSysCommand( nID, lParam );
}


afx_msg LRESULT CNITF_QueryDlg::OnExitSizeMove( WPARAM wParam, LPARAM lParam )
{
   OnActivate2( wParam, lParam );
   return 1;
}


afx_msg VOID CNITF_QueryDlg::OnCancel()
{
#ifdef CLOSE_QUERY_TOOL_ON_CANCEL
   if ( C_nitf_ovl_factory::get_edit_mode() == C_ovl_mgr::NITF_DATA_MANAGER )
      OVL_get_overlay_manager()->set_mode( C_ovl_mgr::NO_EDIT );
#else
   m_lEnableQueryEditor = SW_HIDE;
   EnableCtrl();
#endif
}


afx_msg VOID CNITF_QueryDlg::OnSize( UINT nType, int cX, int cY )
{
   CDialog::OnSize( nType, cX, cY );
   CWnd* pWnd = GetDlgItem( IDC_NITF_QUERY_DLG );
   if ( pWnd != NULL )
      pWnd->MoveWindow( 0, 0, cX, cY, TRUE );
}


afx_msg VOID CNITF_QueryDlg::OnGetMinMaxInfo( MINMAXINFO* pMMI ) 
{
   if ( m_bInitializeDone )
      pMMI->ptMinTrackSize = m_ptMinDlgSize;
}  // End of CNITF_QueryDlg::OnGetMinMaxInfo()


// This event gets fired when the user clicks the Help button in the 
// NITF Query Tool dialog.  Tab number are 1-based, 0 is for general help
afx_msg VOID CNITF_QueryDlg::OnHelp( LONG lHelpType, LONG lCurrentTab )
{
   TRACE( _T("CNITF_QueryDlg::OnHelp( %d, %d )\n"),
      lHelpType, lCurrentTab );

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
}


//
// OnNotifyChange() - query dialog UI conditions change
//
VOID CNITF_QueryDlg::OnNotifyChange()
{
   if ( fvw_get_view() != NULL )
   {
      C_nitf_ovl::s_pNITFOvl->UpdateQueryToolToolbar();
      UpdateQueryToolConfiguration();

      // Don't disturb metadata list if waiting for first AoI
      if ( m_lEnableMetadataList != SW_SHOW_ON_FIRST_AOI )
      {
         m_lEnableMetadataList =
            ( C_nitf_ovl::s_pNITFOvl->m_mQueryToolbarStates
               [ QUERY_DLG_TOOLBAR_BUTTON_SHOW_METADATA_LIST - QUERY_DLG_TOOLBAR_MIN_BUTTON ]
               & QUERY_DLG_TOOLBAR_BUTTON_CHECKED_MASK ) != 0
            ? SW_SHOWNA : SW_HIDE;
      }
   }
}


//
// OnUpdateMapDisplay()
//
VOID CNITF_QueryDlg::OnUpdateMapDisplay( ULONG mTempDisplayMask, const VARIANT& varIn1 )
{
   MapView* pView = fvw_get_view();
   if ( pView != NULL )
   {
      if ( pView->IsKindOf( RUNTIME_CLASS( MapView ) ) )
      {
         // If there are display adjustments to make
         do
         {
            if ( varIn1.vt != VT_BSTR )
               break;

            MSXML2::IXMLDOMDocument2Ptr pxdParamsDoc;
            if ( S_OK !=
               pxdParamsDoc.CreateInstance( __uuidof( MSXML2::DOMDocument60 ), NULL, CLSCTX_INPROC_SERVER ) )
               break;

            if ( !pxdParamsDoc->loadXML( varIn1.bstrVal ) )
               break;

            MSXML2::IXMLDOMElementPtr pxeCenter =
               pxdParamsDoc->selectSingleNode( L"/UpdateMapDisplay/MappingParams/Center" );
            if ( pxeCenter == NULL )
               break;

            _variant_t
               varLat = pxeCenter->getAttribute( L"Lat" ),
               varLon = pxeCenter->getAttribute( L"Lon" );

            if ( varLat.vt != VT_BSTR || varLon.vt != VT_BSTR )
               break;

            // Center the map on the new lat/lon
            pView->get_map_engine()->change_center( (DOUBLE) varLat, (DOUBLE) varLon );

         } while ( FALSE );

         C_nitf_ovl::s_pNITFOvl->SetTempDisplayMask( mTempDisplayMask );
         C_nitf_ovl::s_pNITFOvl->m_bReload = TRUE;
         C_nitf_ovl::s_pNITFOvl->invalidate();
         OVL_get_overlay_manager()->invalidate_from_thread( FALSE );
      }
   }
}


//
// OnDestroy()
//
VOID CNITF_QueryDlg::OnDestroy()
{
   C_nitf_ovl::s_bEditOn = FALSE;

   if ( m_dwQueryDlgEventsCookie != 0 )
   {
      BOOL b = AfxConnectionUnadvise(
         m_ctlNITFQueryDlg.GetControlUnknown(),
         DIID__INITFQueryDlgEvents,
         GetIDispatch( FALSE ),
         FALSE,
         m_dwQueryDlgEventsCookie );
      ASSERT( b );
      m_dwQueryDlgEventsCookie = 0;
   }
}


///////////////////////////////////////////////
// Public methods
///////////////////////////////////////////////

//
// SetQueryGeoBounds() - send rubber-band region definitions to the query tool
//
VOID CNITF_QueryDlg::SetQueryGeoRegions()
{
   if ( !m_bInitializeDone || C_nitf_ovl::s_pNITFOvl == NULL )
      return;

   DWORD dwRegionsDims[] = { 4, 0-0 }; // Number of entries TBD
   dwRegionsDims[1] = C_nitf_ovl::s_pNITFOvl->m_agbpQueryRegions.size();

   _variant_t varRegions, varViewCorners;
   COleSafeArray sa;

   if ( dwRegionsDims[1] <= 0 )  // If no geobounds regions
      varRegions.vt = VT_EMPTY;

   else
   {
      // Create a safe-array for the regions data
      sa.Create( VT_R8, 2, dwRegionsDims );

      DOUBLE (*pd)[4];
      sa.AccessData( (PVOID*) &pd );
      C_nitf_ovl::GeoBoundsPtrIter it;
      for ( it = C_nitf_ovl::s_pNITFOvl->m_agbpQueryRegions.begin(); 
         it != C_nitf_ovl::s_pNITFOvl->m_agbpQueryRegions.end(); it++, pd++ )
      {
         it->GetGeoBounds().get_bounds( (*pd)[0], (*pd)[1], (*pd)[2], (*pd)[3] );

      }

      // Deaccess the destination data
      sa.UnaccessData();

      // Detach the safe array and attach it to the regions argument
      varRegions = sa.Detach();

      // If waiting for the first area-of-interest, wake up the metadata list
      if ( m_lEnableMetadataList == SW_SHOW_ON_FIRST_AOI )
      {
         m_lEnableMetadataList = SW_SHOWNA;
         EnableCtrl();
      }
   }
   // Create a safe-array for the view corners data
   static const DWORD dwViewCornersDims[] = { 2, 4 };
   sa.Create( VT_R8, 2, (PDWORD) dwViewCornersDims );

   d_geo_t* pgpt = C_nitf_ovl::s_pNITFOvl->m_gptViewCorners;
   DOUBLE (*pd)[2];
   sa.AccessData( (PVOID*) &pd );
   for ( INT i = 0; i < 4; pd++, pgpt++, i++ )
   {
      (*pd)[0] = pgpt->lat;
      (*pd)[1] = pgpt->lon;
   }

   // Deaccess the data
   sa.UnaccessData();

   // Detach the safe array and attach it to the view corners argument
   varViewCorners = sa.Detach();

   m_ctlNITFQueryDlg.SpecialFunction( QUERY_DLG_SPECFUNC_SET_QUERY_GEOBOUNDS,
     _variant_t( (LONG) dwRegionsDims[1] ), varRegions, varViewCorners, NULL, NULL );
}


//
// EnableCtrl()
//
VOID CNITF_QueryDlg::EnableCtrl( BOOL bEnable )
{
   if ( bEnable )
   {
      ShowWindow( m_lEnableQueryEditor );
      m_ctlNITFQueryDlg.EnableCtrl(
         m_lEnableQueryEditor,
         m_lEnableMetadataList == SW_SHOW_ON_FIRST_AOI
         ? SW_HIDE : m_lEnableMetadataList );
   }
   else
   {
      ShowWindow( SW_HIDE );
      m_ctlNITFQueryDlg.EnableCtrl( SW_HIDE, SW_HIDE );
   }
}  // End of C_NITF_QueryDlg::EnableCtrl()

 
//
// UpdateQueryToolConfiguration() - get query tool XML string
//
VOID CNITF_QueryDlg::UpdateQueryToolConfiguration()
{
   _variant_t var;
   if ( S_OK == m_ctlNITFQueryDlg.SpecialFunction( QUERY_DLG_SPECFUNC_GET_CONFIGURATION,
      VARIANT_NULL, VARIANT_NULL, VARIANT_NULL, &var, NULL ) )
   {
      ATLASSERT( var.vt == VT_BSTR );
      if ( var.vt == VT_BSTR )
      {
         BOOL b;
         b = m_pxdQueryToolConfiguration->loadXML( var.bstrVal );
         ATLASSERT( b );
      }
   }
}


/////////////////////////////
// Private methods
/////////////////////////////

VOID CNITF_QueryDlg::OnActivate1( WPARAM wParam, LPARAM lParam )
{
   if ( m_ctlNITFQueryDlg.GetSafeHwnd() != NULL )
   {
#if 0 && defined _DEBUG
      ATLTRACE( _T("CNITF_QueryDlg::OnActivate1: wParam = 0x%08x, hWndOther = 0x%08x\n"), wParam, lParam );
#endif
      
      VARIANT varHWndCur;
      varHWndCur.vt = VT_BYREF;
      varHWndCur.byref = m_hWnd;
      
      VARIANT varHWndOther;
      varHWndOther.vt = VT_BYREF;
      varHWndOther.byref = (PVOID) lParam;
      
      m_ctlNITFQueryDlg.SpecialFunction( QUERY_DLG_SPECFUNC_WINDOW_ACTIVATION_CHANGE,
         _variant_t( (LONG) wParam ), varHWndCur, varHWndOther, NULL, NULL );
   }
}


VOID CNITF_QueryDlg::OnActivate2( WPARAM wParam, LPARAM lParam )
{
#if 0 && defined _DEBUG
   ATLTRACE( _T("CNITF_QueryDlg::OnActivate2: wParam = 0x%08x, hWndOther = 0x%08x\n"), wParam, lParam );
#endif
   
   VARIANT varHWndCur;
   varHWndCur.vt = VT_BYREF;
   varHWndCur.byref = m_hWnd;
   
   m_ctlNITFQueryDlg.SpecialFunction( QUERY_DLG_SPECFUNC_WINDOW_ACTIVATION_CHANGE,
      _variant_t( (LONG) WA_CLICKACTIVE ), varHWndCur, _variant_t( (LONG) lParam ), NULL, NULL );
}


// End of NITF_QueryDlg.cpp
LRESULT CNITF_QueryDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

