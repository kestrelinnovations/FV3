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

// nitf.cpp -- Nitf file overlay




#include "stdafx.h"
#define NITF_UTILITY_CLASSES
#include "nitf.h"
using namespace NITFImagery;
#include "err.h"
#include "param.h"
#include "map.h"
#include "file.h"
#include "..\mapview.h"
#include "..\Overlay\ViewInv.h"
#include "histtransdlg.h"
#include "..\fvw.h"
#include "wm_user.h"
#include "OvlFctry.h"
#include "showrmk.h"
#include "FvCore\Include\RichTextFormatter.h"
#include "factory.h"
#include "drag_utils.h"
#include "QueuedAppUtils.h"
#include "..\getobjpr.h"
#include "..\StatusBarManager.h"
#include "NITFTBClass.h"
#include "..\statbar.h"
#include "maps.h"

extern CFVApp theApp;

#ifdef _UNICODE
   #define A2T_FMT L"%S"
#else
   #define A2T_FMT "%s"
#endif


// ****************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************
// ********************************************************************

// --------------------------------------------------------------------------
// --------------------OOOOOOO---VV----------VV--LL--------------------------
// --------------------OO---OO----VV--------VV---LL--------------------------
// --------------------OO---OO-----VV------VV----LL--------------------------
// --------------------OO---OO------VV----VV-----LL--------------------------
// --------------------OO---OO-------VV--VV------LL--------------------------
// --------------------OO---OO--------VVVV-------LL--------------------------
// --------------------OOOOOOO---------VV--------LLLLLLLLLL------------------


map_scale_t                   C_nitf_ovl::m_smallest_scale;
POINT                         C_nitf_ovl::s_ptMenuPoint = { 0, 0 };
BOOL                          C_nitf_ovl::s_bUserAbort;
BOOL                          C_nitf_ovl::s_bEditOn = FALSE;
BOOL                          C_nitf_ovl::s_bParamsUpdate = TRUE;
C_nitf_ovl::EditModeEnum      C_nitf_ovl::s_eEditMode = EDIT_MODE_SELECT;
C_nitf_ovl*                   C_nitf_ovl::s_pNITFOvl = NULL;
CNITF_QueryDlg                C_nitf_ovl::s_QueryDlg;
const CString                 C_nitf_ovl::s_csOverlaySpecification( _T("\\\\Tactical magery Overlay") );
#if defined WARP_TEST
CWARP_QueryDlg                C_nitf_ovl::m_WarpQueryDlg;
#endif
CIconImage*                   C_nitf_ovl::s_piiNITFSystemIcon = NULL;   // Static so can be shared

// **************************************************************************
// **************************************************************************

C_nitf_ovl::C_nitf_ovl() :
   m_fphJPIPProbeHost( this )
{
   init_vars();
}
// end of constructor for C_nitf_ovl

HRESULT C_nitf_ovl::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   if ( open("") != SUCCESS )
   {
      AfxMessageBox( _T("Error Opening Tactical Imagery Overlay.") );
      return E_FAIL;
   }

   return S_OK;
}

// **************************************************************************
// **************************************************************************

C_nitf_ovl::C_nitf_ovl(CString name) :
   m_fphJPIPProbeHost( this )
{
   init_vars();
}
// end of constructor for C_nitf_ovl

// **************************************************************************
// **************************************************************************

C_nitf_ovl::~C_nitf_ovl()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.
}

void C_nitf_ovl::Finalize()
{
	clear_ovl();

   POSITION pos = UTL_get_timer_list()->Find( &m_clsFlashTimer );
   if ( pos != NULL )
      UTL_get_timer_list()->remove_at( pos );

   KillTimer( NULL, m_uiRedrawTimerId );

   EnterCriticalSection( &m_csUtilityProcAccess );
   m_wtdUtilityControl.clear();          // Flush any waiting work
   LeaveCriticalSection( &m_csUtilityProcAccess );

   if ( m_pUtilityThread != NULL )
   {
      BOOL b = PostThreadMessage( m_pUtilityThread->m_nThreadID, WM_QUIT, 0, 0 );
      WaitForSingleObject( m_pUtilityThread->m_hThread, INFINITE );
      delete m_pUtilityThread;
      m_pUtilityThread = NULL;
   }
   DeleteCriticalSection( &m_csUtilityProcAccess );

   if (this == (C_nitf_ovl*) CRemarkDisplay::get_focus_overlay())
      CRemarkDisplay::close_dlg();

   close_nitf_database();
   WSACleanup();
   s_pNITFOvl = NULL;
}


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::init_vars()
{
   s_pNITFOvl = this;      // Should be only copy
   s_QueryDlg.Initialize();      // Load with configuration data

   m_last_ll.lat = 0.0;
   m_last_ll.lon = 0.0;
   m_last_ur.lat = 0.0;
   m_last_ur.lon = 0.0;

   m_scrn_ll.lat = -90.0;
   m_scrn_ll.lon = -180.0;
   m_scrn_ur.lat = 90.0;
   m_scrn_ur.lon = 180.0;

   m_bSelectedObj = FALSE;
   s_bParamsUpdate = TRUE;    // From registry
   m_eNewExplorerCurrentImageDisplay = NEW_EXPLORER_IMAGE_IDLE;
   m_bDrawInProgress = FALSE;
   m_name = "Tactical Imagery"; 
   m_bReload = TRUE;
   m_uiRedrawTimerId = 0;
   m_bImageSourceBusy = FALSE;

   m_csFVWUserDataRootFolder = PRM_get_registry_string( "Main", "USER_DATA" );

   CView* view;
   ViewMapProj* map;

   view = UTL_get_active_non_printing_view();
   if (view == NULL)
   {
      map = NULL;
   }
   else
   {
      map = UTL_get_current_view_map(view);
      if (map != NULL)
      {
         if (!map->is_projection_set())
            map = NULL;
      }
   }

   if (map != NULL)
      map->get_vmap_bounds(&m_scrn_ll, &m_scrn_ur);

   // Load the nitf icons
   m_nitf_icon = CIconImage::load_images("nitf\\nitfovl_nitf.ico");
   m_tiff_icon = CIconImage::load_images("nitf\\nitfovl_tiff.ico");
   m_sid_icon = CIconImage::load_images("nitf\\nitfovl_sid.ico");
   m_j2k_icon = CIconImage::load_images("nitf\\nitfovl_j2k.ico");
   m_pdf_icon = CIconImage::load_images("nitf\\nitfovl_pdf.ico");
   m_misc_icon = CIconImage::load_images("nitf\\nitfovl_misc.ico");
   m_nitf_m_icon = CIconImage::load_images("nitf\\nitfovl_nitf_m.ico");
   m_tiff_m_icon = CIconImage::load_images("nitf\\nitfovl_tiff_m.ico");
   m_sid_m_icon = CIconImage::load_images("nitf\\nitfovl_sid_m.ico");
   m_j2k_m_icon = CIconImage::load_images("nitf\\nitfovl_j2k_m.ico");
   m_pdf_m_icon = CIconImage::load_images("nitf\\nitfovl_pdf_m.ico");
   m_misc_m_icon = CIconImage::load_images("nitf\\nitfovl_misc_m.ico");

   // Stuff the file type string to file type enum lookup map
   static const struct FileTypeCvt
   {
      LPCSTR pszType;
      NITFFileTypeEnum  eType;
   } aFileTypeCvts[] =
   {
      { "NITF",      NITF_NITF_FILE },
      { "GeoTIFF",   NITF_TIFF_FILE },
      { "MrSID",     NITF_MRSID_FILE },
      { "JPIP",      NITF_JPIP_FILE },
      { "JP2",       NITF_JP2_FILE },
      { "PDF",       NITF_PDF_FILE },
      { "GeoPDF",    NITF_PDF_FILE },
      { "GDAL",      NITF_MISC_FILE },
      { "OGR",       NITF_MISC_FILE },
      { "PlugIn",    NITF_MISC_FILE },
      { NULL }
   };
   for ( const FileTypeCvt* pFileTypeCvt = aFileTypeCvts;
         pFileTypeCvt->pszType != NULL; pFileTypeCvt++ )
      m_mpFileTypeLookup.insert(
         FileTypeLookup::value_type( (LPCTSTR) CA2T( pFileTypeCvt->pszType ), pFileTypeCvt->eType ) );


   WSADATA wsaData;
   INT iErr = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
   ASSERT( iErr == 0 && "WSAStartup failed" );


   m_crFrameBusyFrgdColor        = DFLT_FRAME_BUSY_COLOR;
   m_crFrameMonoImageFrgdColor   = DFLT_FRAME_MONO_IMAGE_COLOR;
   m_crFrameColorImageFrgdColor  = DFLT_FRAME_COLOR_IMAGE_COLOR;
   m_crFrameStdBkgdColor         = DFLT_FRAME_STD_BKGD_COLOR;
   m_crFrameSelectedBkgdColor    = DFLT_FRAME_SEL_BKGD_COLOR;
   m_crFrameTempBkgdColor        = DFLT_FRAME_TEMP_BKGD_COLOR;
   m_crFrameTempKeepBkgdColor    = DFLT_FRAME_TEMP_KEEP_BKGD_COLOR;
   m_crQueryRegionFrgdColor      = DFLT_QUERY_REGION_COLOR;
   m_iUnselectedBkgdFrameWidth   = DFLT_UNSELECTED_BKGD_FRAME_WIDTH;
   m_iSelectedBkgdFrameWidth     = DFLT_SELECTED_BKGD_FRAME_WIDTH;

   m_mTempDisplayMask = NITF_TEMP_DISPLAY_NORMAL_MASK;
   ZeroMemory( m_mQueryToolbarStates, sizeof(m_mQueryToolbarStates) );

   m_eImagesOnlyNoEditor = IMAGES_ONLY_NO_EDITOR_UNKNOWN;
   m_eQueryFilterDisplay = QUERY_FILTER_DISPLAY_UNSPECIFIED;   // All images to begin with

   m_bDragging = FALSE;

   m_gbDragQueryRegion.set_draw_mode( XOR_MODE );
   OvlPen& pen = m_gbDragQueryRegion.get_pen();
   COLORREF cr; INT iWidth, iStyle;
   pen.get_foreground_pen( cr, iStyle, iWidth );
   pen.set_foreground_pen( (COLORREF) 0x00FFFFFF, iStyle, 3 );   // Flip all bits in XOR mode

   ZeroMemory( m_mQueryToolbarStates, sizeof(m_mQueryToolbarStates) );
   for ( INT k = 0; k < sizeof(m_csMessageStrings) / sizeof(m_csMessageStrings[0]); k++ )
      m_csMessageStrings[ k ].Empty();

   InitializeCriticalSection( &m_csUtilityProcAccess );
   m_pUtilityThread = NULL;               // No worker thread active
   m_bMultipleExplorerFiles = FALSE;          // Nothing queued
   m_dwMainThreadID = GetCurrentThreadId();
}
// end of init_vars

// **************************************************************************
// **************************************************************************

void C_nitf_ovl::clear_ovl()
{
   // Insure that any overview building in progress will run down
   POSITION pos = m_objlist.GetHeadPosition();
   while ( pos != NULL )
   {
      m_objlist.GetNext( pos )->m_cbImageLibCallback2.m_eCallbackMode
         = ImageLibCallbackInterface::CALLBACK_MODE_ABORTING;
   }

   m_objlist.RemoveAll();
} // end of clear_ovl()

// **************************************************************************
// **************************************************************************

int C_nitf_ovl::draw( ActiveMap* pMap )
{
   #ifdef TIMING_TEST_2
      DWORD dwBeginTicks = GetTickCount();
   #endif

      POSITION next;
      map_scale_t mapscale, smallest_scale;

   LPCSTR psz = __targv[0];

   CFvwUtil *futil = CFvwUtil::get_instance();

   if ( pMap == NULL )
      return FAILURE;

   // get the map bounds to clip symbol drawing 
   if ( pMap->get_vmap_bounds(&m_scrn_ll, &m_scrn_ur) != SUCCESS )
      return FAILURE;

   pMap->get_vsurface_size(&m_scrn_width, &m_scrn_height);

   if ( s_bParamsUpdate )
   {
      s_bParamsUpdate = FALSE;
      m_bReload = TRUE;    // Will force redraw

      CString sdata = PRM_get_registry_string( "NitfFile", "ShowBounds", "Y" );
      m_bShowBounds = sdata.CompareNoCase( "Y" ) == 0;

      sdata = PRM_get_registry_string( "NitfFile", "HideDuringScroll", "Y" );
      m_bHideDuringScroll = sdata.CompareNoCase( "Y" ) == 0;

      sdata = PRM_get_registry_string( "NitfFile", "ShowSecondaryImages", "N" );
      m_bShowSecondaryImages = sdata.CompareNoCase( "Y" ) == 0;

      sdata = PRM_get_registry_string( "NitfFile", "NewExplorerImages", "CenterMap" );
      m_eNewExplorerInitialImageDisplay =
         ( 0 == sdata.CompareNoCase( "CenterMapAlways" ) )
            ? NEW_EXPLORER_IMAGE_CENTER_MAP_ALWAYS
         : ( 0 == sdata.CompareNoCase( "CenterMapAsk" )
               || 0 == sdata.CompareNoCase( "CenterMap" ) )
            ? NEW_EXPLORER_IMAGE_CENTER_MAP_ASK
         : ( 0 == sdata.CompareNoCase( "NewDisplayTabAlways" ) )
            ? NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ALWAYS
         : ( 0 == sdata.CompareNoCase( "NewDisplayTabAsk" )
               || 0 == sdata.CompareNoCase( "NewDisplayTab" ) )
            ? NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ASK
         : NEW_EXPLORER_IMAGE_DISPLAY_ONLY;

   }  // Params update

   // get the map bounds to clip symbol drawing 
   if ( pMap->get_vmap_bounds(&m_scrn_ll, &m_scrn_ur) != SUCCESS )
      return FAILURE;

   if ((m_scrn_ll.lat != m_last_ll.lat) || (m_scrn_ll.lon != m_last_ll.lon) ||
      (m_scrn_ur.lat != m_last_ur.lat) || (m_scrn_ur.lon != m_last_ur.lon))
   {
      m_bReload = TRUE;

      SaveViewCorners( pMap );      // For query editor

      m_last_ll.lat = m_scrn_ll.lat;
      m_last_ll.lon = m_scrn_ll.lon;
      m_last_ur.lat = m_scrn_ur.lat;
      m_last_ur.lon = m_scrn_ur.lon;
   }

   if ( m_bReload )
   {
      m_bReload = FALSE;

      if ( SUCCESS != select_nitf_records( *pMap ) )
         return FAILURE;
   }

   m_last_ll.lat = m_scrn_ll.lat;
   m_last_ll.lon = m_scrn_ll.lon;
   m_last_ur.lat = m_scrn_ur.lat;
   m_last_ur.lon = m_scrn_ur.lon;

   // Prepare for partial rendering
   m_bDrawInProgress = TRUE;
   next = m_objlist.GetHeadPosition();
   while ( next != NULL )
      m_objlist.GetNext( next )->m_bIncompleteDraw = FALSE;

   m_bIncompleteDraw = m_bInvalidateRequested = FALSE;

   if ( m_objlist.GetCount() > 0 )
   {
      
      mapscale = pMap->scale();
      smallest_scale = get_smallest_scale();
      
      // MATT_TODO -- hook up NITFPathCtrl to this
      if (smallest_scale > mapscale)
      {
         if (this == (C_nitf_ovl*) CRemarkDisplay::get_focus_overlay())
            CRemarkDisplay::close_dlg();
         return SUCCESS;
      }
      
      next = m_objlist.GetHeadPosition();
      while ( next != NULL )
         m_objlist.GetNext( next )->draw( pMap );

      // set flag to draw/not-draw labels based up current map scale
      m_bDrawLabels = (get_smallest_labels_scale() <= pMap->scale())  &&  
         (get_smallest_labels_scale() != NULL_SCALE);
      
      if ( m_bDrawLabels )
      {
         CWaitCursor wait;
         next = m_objlist.GetHeadPosition();
         while ( next != NULL )
            m_objlist.GetNext( next )->draw_label( pMap );
      }
   }  // Object count > 0

   set_valid(TRUE);

   // Update any georegions
   if ( s_bEditOn )
   {
      GeoBoundsPtrIter it;
      for ( it = m_agbpQueryRegions.begin(); it != m_agbpQueryRegions.end(); it++ )
      {
         GeoBounds& gb = it->GetGeoBounds();
         gb.prepare_for_redraw( pMap );
         gb.draw( pMap, pMap->get_CDC() );
      }
   }

   // If there was a busy source found, see if can disconnect some other(s)
   m_dwLastDrawTicks = GetTickCount();
   m_iRedrawDelayTicks = 10 * 1000;       // Assume no immediate redraw, 10 seconds
   if ( m_bImageSourceBusy )
   {
      next = m_objlist.GetHeadPosition();
      while ( next != NULL )
      {
         C_nitf_obj* pObj = m_objlist.GetNext( next );
         _variant_t varStatus;
         if ( !pObj->m_bIncompleteDraw && pObj->m_pImageLib != NULL )
         {
            pObj->m_pImageLib->
               SpecialFunction( IMAGELIB_SPEC_FUNC_TRY_CLOSE_IMAGE_SOURCE,
                  VARIANT_TRUE, VARIANT_NULL, &varStatus, NULL, NULL, NULL );
            if ( (LONG) varStatus == (LONG) IMAGELIB_SPEC_FUNC_TRY_CLOSE_IMAGE_SOURCE_NOW_CLOSED )
            {
#if ( 0 && defined _DEBUG ) || defined ALL_TESTS
               ATLTRACE( _T("Post-disconnected \"%s\"\n"), pObj->m_filename );
#endif
               m_iRedrawDelayTicks = 2 * 1000;  // Retry after a short delay
            }
         }  // Can clean up
      }  // Object loop
   }  // Busy source
     
   m_bDrawInProgress = FALSE;

   // Start or stop redraw timer depending upon whether there was an incomplete draw
   if ( !m_bIncompleteDraw )
   {
      if ( m_uiRedrawTimerId != 0 )
      {
         KillTimer( NULL, m_uiRedrawTimerId );
         m_uiRedrawTimerId = 0;
      }
      m_bImageSourceBusy = FALSE;
   }
   else if ( m_uiRedrawTimerId == 0 )
   {
      m_uiRedrawTimerId = SetTimer( NULL, 0, 1000, RedrawTimerProc );
      ATLASSERT( m_uiRedrawTimerId != 0 );
   }

   if ( m_bInvalidateRequested )
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
      {
         map_view->PostMessage(WM_INVALIDATE_FROM_THREAD,
         (WPARAM) new MapViewInvalidateAll(FALSE));
      }
   }


   #if defined TIMING_TEST_2 || defined ALL_TESTS
      ATLTRACE( _T("Objects = %d, total draw time = %d\n"),
         m_objlist.GetCount(), (INT) ( GetTickCount() - dwBeginTicks ) );
   #endif

      return SUCCESS;
}
// End of C_nitf_ovl::draw()


// **************************************************************************
// **************************************************************************

C_icon* C_nitf_ovl::hit_test( map_projection_utils::CMapProjWrapper* pMap, CPoint ptTest )
{
   POSITION pPrevObj = m_objlist.GetTailPosition();
   while ( pPrevObj != NULL )
   {
      C_nitf_obj* pObj = &*m_objlist.GetPrev( pPrevObj );
      if ( pObj->hit_test( ptTest ) )
      {
         // If icon or edge hit
         if ( pObj->m_bSelectHit )
            return pObj;
      }
   }
   return NULL;      // No hits or inside boundary
}  // End of C_nitf_ovl::hit_test()


// ******************************************************************************
// ******************************************************************************

BOOL C_nitf_ovl::image_hit_test(int mx, int my)
{
   CPoint testpt(mx, my);

   // return the first one hit
   POSITION pos = m_objlist.GetTailPosition();
   while ( pos != NULL )
   {
      C_nitf_obj* pObj = &*m_objlist.GetPrev( pos );
      // is the image larger than iconic
      if ( ( pObj->m_rect.Width() >= 20 ) || ( pObj->m_rect.Height() >= 20 ) )
      {
         // is the image being displayed
         if ( pObj->m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
         {
            if ( pObj->hit_test(testpt))
            {
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}
// end of image_hit_test()


//-----------------------------------------------------------------------------
boolean_t C_nitf_ovl::offer_keydown( IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point )
{
   if (nChar == VK_ESCAPE && m_bDragging)
   {
      cancel_drag(OVL_get_overlay_manager()->get_view_map());
      return TRUE;
   }

   // Only interested in CTRL-SHIFT-noALT qualifiers.
   if ( GetKeyState( VK_CONTROL ) < 0
      && GetKeyState( VK_SHIFT ) < 0
      && GetKeyState ( VK_MENU ) >= 0 )
   {
      switch( nChar )
      {
         case VK_UP:
            if ( ( nFlags & ( 1 << 14 ) ) == 0 )   // Only if wasn't down already
               RotateImageStack( point, TRUE );    // Top image to bottom
            return TRUE;

         case VK_DOWN:
            if ( ( nFlags & ( 1 << 14 ) ) == 0 )   // Only if wasn't down already
               RotateImageStack( point, FALSE );   // Bottom image to top
            return TRUE;

      default:;
      }  // switch( nChar )
   }
   return FALSE;
}


//-----------------------------------------------------------------------------
boolean_t C_nitf_ovl::offer_mousewheel( IFvMapView *pMapView, SHORT zDelta,
                                       UINT nFlags, CPoint point )
{
   if ( ( nFlags & ( MK_SHIFT | MK_CONTROL ) ) == ( MK_SHIFT | MK_CONTROL ) )
   {
      RotateImageStack( point, zDelta >= 0 );    // Raise stack if scrolling up
      return TRUE;      // Handled
   }
   return FALSE;
}


//-----------------------------------------------------------------------------
VOID C_nitf_ovl::RotateImageStack( CPoint ptTest, BOOL bRaiseStack )
{
   // Look for the top and bottom filled images at this point
   POSITION pNext;
   C_nitf_obj *pObj, *pTopObj = NULL, *pBottomObj = NULL;

   pNext = m_objlist.GetHeadPosition();
   while ( pNext != NULL )
   {
      pObj = &*m_objlist.GetNext( pNext );
      if ( pObj->hit_test( ptTest )
         && pObj->m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
      {
         pTopObj = pObj;
         if ( pBottomObj == NULL )
            pBottomObj = pObj;
      }
   }  // Object loop

   // If there are at least two images in the stack
   if ( pTopObj != pBottomObj )
   {
      if ( bRaiseStack )
         make_topmost_or_bottommost( pTopObj, FALSE );
      else
         make_topmost_or_bottommost( pBottomObj, TRUE );
   }
}  // End of C_nitf_ovl::RotateImageStack()


// ******************************************************************************
// ******************************************************************************

void C_nitf_ovl::release_focus()
{
   clear_selects();
}
// end of release_focus

// ******************************************************************************
// ******************************************************************************

int C_nitf_ovl::selected( IFvMapView* pMapView, CPoint pt, UINT uiFlags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   // If query tool is active and mode is "RoI Select", start or move a drag rectangle
   if ( s_bEditOn && s_eEditMode == EDIT_MODE_AREA )
   {
      m_eDragMode = DRAG_MODE_NONE; // Not active yet

      GeoBoundsPtrIter it;
      
      // Deactivate any existing region and check for a hit
      BOOL bOnce = FALSE;
      for ( it = m_agbpQueryRegions.begin(); it != m_agbpQueryRegions.end(); it++ )
      {
         GeoBounds& gb = it->GetGeoBounds();
         do
         {
            if ( !bOnce && gb.hit_test( pt ) )  // Hit a new region
            {
               bOnce = TRUE;
               if ( gb.has_edit_focus() ) // If already active
               {

                  // Copy the GeoBounds into a GeoBoundsDragger used just for dragging
                  m_gbdRegionDragger.copy( gb );
                  gb.set_edit_focus( FALSE );
                  gb.invalidate( FALSE );

                  // Set up drag
                  HCURSOR hCursor;
                  if ( m_gbdRegionDragger.on_selected( OVL_get_overlay_manager()->get_view_map(), pt, uiFlags, &m_bDragging, &hCursor ) )
                  {
                     m_eDragMode = DRAG_MODE_DRAGGER;
                     m_itDragRegion = it; // Remember for drop
                  }
                  else
                  {
                     // Not dragging
                  }
                  return SUCCESS;
               }
               else  // Wasn't previously active
                  gb.set_edit_focus( TRUE );
            }     // Not new hit
            else
            {
               if ( !gb.has_edit_focus() )
                  break;      // No fix up needed
                  
               gb.set_edit_focus( FALSE );   // Deactivate
            }

            gb.invalidate( FALSE );    // Need to fix up
         } while ( FALSE );
      }  // Region hit test loop

      // If didn't hit existing region, start a new one
      if ( !bOnce )
      {
         m_bDragging = TRUE;
         m_ptDragOrigin = pt;
         mapProjWrapper.surface_to_geo( pt.x, pt.y, &m_gptDragOrigin.lat, &m_gptDragOrigin.lon );
      }

      return SUCCESS;
   }  // Editor active

   C_nitf_obj* pObj = static_cast< C_nitf_obj* >( hit_test( &mapProjWrapper, pt ) );
   if ( pObj == NULL )
   {
      clear_selects();
      return FAILURE;
   }

   show_info( pObj );      // Always show something if something is hit
   m_bSelectedObj = TRUE;

   return SUCCESS;
}
// end of selected()



//////////////////////////////////////////////////////////////////////////////
// MouseMove event handler
//////////////////////////////////////////////////////////////////////////////
INT C_nitf_ovl::on_mouse_moved( IFvMapView *pMapView, CPoint pt, UINT uiFlags)
{
   // Don't do anything if the editor is not active
   if ( !s_bEditOn )
      return FAILURE;

   if (m_bDragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(OVL_get_overlay_manager()->get_view_map(), pt, uiFlags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   // Check whether the point is on one of our existing regions
   GeoBoundsPtrIter it;
   for ( it = m_agbpQueryRegions.begin(); it != m_agbpQueryRegions.end(); it++ )
   {
      GeoBounds& gb = it->GetGeoBounds();
      if ( gb.hit_test( pt ) )
      {
         // Copy your GeoBounds into a GeoBoundsDragger used just for dragging
         m_gbdRegionDragger.copy( gb );

         // on_selected would take action
         HCURSOR hCursor = NULL;
         CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
         if ( m_gbdRegionDragger.on_test_selected( &mapProjWrapper, pt, uiFlags, &hCursor, m_htHintText ) )
         {
            if (hCursor != NULL)
               pMapView->SetCursor(hCursor);

            // If you do not have the edit focus, you will get it - if selected.
            if ( !gb.has_edit_focus() )
            {
               // Since you do not have edit focus, you do not have edit handles, so
               // the cursor will hit the GeoBounds themselves or nothing at all.
               m_htHintText.set_help_text( "Select / Move Region" );
               m_htHintText.set_tool_tip( "Select / Move Region" );
            }
            else
            {
               CString csHelp = m_htHintText.get_help_text();

               // On a move
               if ( csHelp == "Move" )
               {
                  m_htHintText.set_help_text( "Move Region" );
                  m_htHintText.set_tool_tip( "Move Region" );
               }
               else
               {
                  m_htHintText.set_help_text( "Resize Region" );
                  m_htHintText.set_tool_tip( "Resize Region" );
               }
            }

            pMapView->SetTooltipText(_bstr_t(m_htHintText.get_tool_tip()));
            pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(m_htHintText.get_help_text()));
            return SUCCESS;

         }  // Dragger selected
      }  // Region hit
   }  // Region loop

   // Select the rectangle cursor if in RoI mode
   pMapView->SetCursor(AfxGetApp()->LoadCursor(
      s_eEditMode == EDIT_MODE_AREA ? IDC_RECTANGLE_CURSOR : IDC_CROSSHAIR ));

   return FAILURE;   // No hit
}

int C_nitf_ovl::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bDragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}


//////////////////////////////////////////////////////////////////////////////
// Drag
//
// Called by the overlay architecture when dragging.
//////////////////////////////////////////////////////////////////////////////
VOID C_nitf_ovl::drag( ViewMapProj* pMap, CPoint pt, UINT uiFlags, 
   HCURSOR* phCursor, HintText** ppHint )
{
   // Checks
   ASSERT( pMap != NULL );
   ASSERT( phCursor != NULL );
   ASSERT( ppHint != NULL );
   ASSERT( s_bEditOn );

   static_cast<MapView *>(pMap->get_CView())->ScrollMapIfPointNearEdge(pt.x,
      pt.y);

   // Initialize
   *ppHint  = NULL;
   *phCursor = AfxGetApp()->LoadCursor( IDC_ARROW );

   if ( m_eDragMode == DRAG_MODE_DRAGGER )
   {
      HintText ht;   // Scratch
      CClientDC dc(pMap->get_CView());
      m_gbdRegionDragger.on_drag( pMap, &dc, pt, uiFlags, phCursor, ht );
   }
   else  // Not dragger
   {
      // Get a DC
      CClientDC dc(pMap->get_CView());

      if ( m_eDragMode == DRAG_MODE_NEW )
      {
         m_eDragMode = DRAG_MODE_NONE;
         m_gbDragQueryRegion.draw( pMap, &dc );   // Erase previous region
      }

      // if the cursor is outside of the view, then use the no drop cursor
      CRect rcClient(0, 0, pMap->get_surface_width(), pMap->get_surface_height());
      if ( !rcClient.PtInRect( pt ) )
         *phCursor = AfxGetApp()->LoadCursor( IDC_NODROP );

      else
      {
         DOUBLE dLat, dLon;
         pMap->surface_to_geo( pt.x, pt.y, &dLat, &dLon );

         if ( !drag_utils::in_drag_threshold( m_ptDragOrigin, pt )
            && fabs( dLat - m_gptDragOrigin.lat ) > 1.0e-9
            && fabs( dLon - m_gptDragOrigin.lon ) > 1.0e-9 )   // Must be non-degenerate
         {
            m_eDragMode = DRAG_MODE_NEW;

            // Draw the new region
            d_geo_t gptSW, gptNE;
            if ( dLat > m_gptDragOrigin.lat )
            {
               gptSW.lat = m_gptDragOrigin.lat;
               gptNE.lat= dLat;
            }
            else
            {
               gptSW.lat = dLat;
               gptNE.lat = m_gptDragOrigin.lat;
            }

            do
            {
               gptSW.lon = m_gptDragOrigin.lon;
               gptNE.lon = dLon;     // Assume drag point is east of origin

               if ( dLon > m_gptDragOrigin.lon )   // If apparently so
               {
                  if ( dLon < m_gptDragOrigin.lon + 180.0 ) // No dateline wrap
                     break;
               }
               else                                // If drag point seems west of origin
               {
                  if ( dLon < m_gptDragOrigin.lon - 180.0 ) // Dateline wrap
                     break;
               }
               gptSW.lon = dLon;
               gptNE.lon = m_gptDragOrigin.lon;
            } while ( FALSE );

            m_gbDragQueryRegion.set_bounds_no_error_check( gptSW, gptNE );
            m_gbDragQueryRegion.prepare_for_redraw( pMap );
            m_gbDragQueryRegion.draw( pMap, &dc );
         }  // Draw needed
      }  // Cursor is in the view

   }  // Not dragger

   // if the cursor is outside of the view, then use the no drop cursor
   CRect rcClient(0, 0, pMap->get_surface_width(), pMap->get_surface_height());
   if ( !rcClient.PtInRect( pt ) )
      *phCursor = AfxGetApp()->LoadCursor( IDC_NODROP );

}  // End of drag()


//////////////////////////////////////////////////////////////////////////////
// Cancel Drag
//////////////////////////////////////////////////////////////////////////////
VOID C_nitf_ovl::cancel_drag( ViewMapProj* pMap )
{
   // Checks
   ASSERT( pMap != NULL );
   ASSERT( s_bEditOn );

   m_bDragging = FALSE;

   if ( m_eDragMode == DRAG_MODE_DRAGGER )
   {
      // Abort the drag operation
      CClientDC dc(pMap->get_CView());
      m_gbdRegionDragger.on_cancel_drag( pMap, &dc);
   }

   // Clean up any left over graphics
   m_eDragMode = DRAG_MODE_NONE;
   OVL_get_overlay_manager()->invalidate_all( FALSE );
}  // End of cancel_drag()


//////////////////////////////////////////////////////////////////////////////
// Drop
//////////////////////////////////////////////////////////////////////////////
VOID C_nitf_ovl::drop( ViewMapProj* pMap, CPoint pt, UINT uiFlags )
{
   // Checks
   ASSERT( pMap != NULL );
   ASSERT( s_bEditOn );

   m_bDragging = FALSE;

   if ( m_eDragMode == DRAG_MODE_DRAGGER )
   {
      // Complete the drag operation
      CClientDC dc(pMap->get_CView());
      m_gbdRegionDragger.on_drop( pMap, &dc, pt, uiFlags );

      // Update the region being modified
      GeoBounds& gb = m_itDragRegion->GetGeoBounds();
      gb.invalidate( FALSE );    // So old size gets cleaned up
      gb.set_bounds( m_gbdRegionDragger.get_sw(), m_gbdRegionDragger.get_ne() );

      // Make it active again in case of further adjustments
      gb.set_edit_focus( TRUE );
      OVL_get_overlay_manager()->invalidate_all( FALSE );
   }
   else
   {
      if ( m_eDragMode == DRAG_MODE_NEW )   // If there is a bounds box
      {
         m_agbpQueryRegions.push_front( GeoBoundsPtr( new GeoBounds() ) );

         DOUBLE dN, dE, dS, dW;
         m_gbDragQueryRegion.get_bounds( dS, dW, dN, dE );
         m_agbpQueryRegions.front().GetGeoBounds().set_bounds( dS, dW, dN, dE );
         m_agbpQueryRegions.front().GetGeoBounds().
            get_pen().set_foreground_pen( m_crQueryRegionFrgdColor, UTIL_LINE_SOLID, 1 );
      }

      cancel_drag( pMap ); // Remove the selection rectangle;
   }  // Not dragger

   // Let the editor know the new regions set
   s_QueryDlg.SetQueryGeoRegions();
}

#ifdef JPIP_SUPPORT

//////////////////////////////////////////////////////////////////////////////
// can_drop_data()
//
// Called by the overlay architecture when dragging onto the overlay.
//////////////////////////////////////////////////////////////////////////////
boolean_t C_nitf_ovl::can_drop_data( COleDataObject* pDataObject, CPoint& pt )
{
   STGMEDIUM stg_medium;
   if ( pDataObject->IsDataAvailable( CF_UNICODETEXT ) )
   {
      pDataObject->GetData( (CLIPFORMAT) CF_UNICODETEXT, &stg_medium );
      if ( stg_medium.tymed == TYMED_HGLOBAL )
      {
         HGLOBAL hGlobal = stg_medium.hGlobal;
         LPCWSTR pwsz = (LPCWSTR) GlobalLock( hGlobal );

         boolean_t bResult = TestURLSourceType( pwsz );

         GlobalUnlock( hGlobal );
         return bResult;
      }
   }
   return FALSE;
}  // End of can_drop_data()

#endif

//////////////////////////////////////////////////////////////////////////////
// paste_from_clipboard()
//
// Called by the overlay architecture when pasting a file or files onto the overlay.
/////////////////////////////////////////////////////////////////////////////
void C_nitf_ovl::paste_from_clipboard()
{
   if ( ::OpenClipboard( NULL ) )
   {
      BOOL bNeedNewTempDB = TRUE;
      try
      {
#if 0 && defined _DEBUG
         UINT ui = 0;
         while ( 0 != ( ui = ::EnumClipboardFormats( ui ) ) )
         {
            TCHAR tchTemp[ 256 ];
            ::GetClipboardFormatName( ui, tchTemp, 255 );
            ATLTRACE( _T("Format %d = \"%s\"\n"), ui, tchTemp );
         }
#endif
         HANDLE hGlobal;
#ifdef JPIP_SUPPORT
         hGlobal = ::GetClipboardData( CF_UNICODETEXT );
         if ( hGlobal != NULL )
         {
            LPCWSTR pwsz = (LPCWSTR) ::GlobalLock( hGlobal );
            if ( pwsz != NULL )
            {
               wstring wstrExpandedURL;
               if ( TestURLSourceType( pwsz, &wstrExpandedURL ) )
                  AddExplorerFile( wstrExpandedURL.c_str(), bNeedNewTempDB );
            }
            ::GlobalUnlock( hGlobal );
         }
#endif

         hGlobal = ::GetClipboardData( CF_HDROP );
         if ( hGlobal != NULL )
         {
            // How many files are dropped
            UINT cFiles = DragQueryFileW( (HDROP) hGlobal, 0xFFFFFFFF, 0, 0 );
 
            // Get the names of these files and see if you can use any
            if ( cFiles > 1 || CheckUtilityThreadAlive() )
               m_bMultipleExplorerFiles = TRUE;    // Allow "yes/no to all" options
            for  ( UINT ui = 0; ui < cFiles; ++ui )
            {
               WCHAR wchFilespec[ MAX_PATH ];
               DragQueryFileW( (HDROP) hGlobal, ui, wchFilespec, MAX_PATH );
               if ( NITFValidateImageFileType( wchFilespec, TRUE ) )
               {
#if ( 0 && defined _DEBUG ) || defined ALL_TESTS
                  ATLTRACE( _T("NITF file paste: \"%s\"\n"), CW2T( wchFilespec ) );
#endif
                  AddExplorerFile( wchFilespec, bNeedNewTempDB );
               }
            }
            ::GlobalUnlock( hGlobal );

            AddExplorerFileEnd( TRUE );   // Reset the yes/no/all messages

         }
      }
      catch ( ... )
      {
      }
      if ( !bNeedNewTempDB )
         DropTempDB();
      ::CloseClipboard();
   }
}



//////////////////////////////////////////////////////////////////////////////
// paste_OLE_data_object()
//
// Called by the overlay architecture when dropping a file onto the overlay.
//////////////////////////////////////////////////////////////////////////////
boolean_t C_nitf_ovl::paste_OLE_data_object( ViewMapProj* pMapProj, CPoint* pPt, 
   COleDataObject* pDataObject )
{
   STGMEDIUM stg_medium;
   HGLOBAL hGlobal;
   BOOL bNeedNewTempDB = TRUE;

   // Test if we have dropped a file collection onto the map
   if ( pDataObject->IsDataAvailable( CF_HDROP ) )
   {
      // Get the file name and extension from the COLEDataObject
      pDataObject->GetData( (CLIPFORMAT) CF_HDROP, &stg_medium );
      if ( stg_medium.tymed == TYMED_HGLOBAL )
         hGlobal = stg_medium.hGlobal;

      // How many files are dropped
      UINT cFiles = DragQueryFileW( (HDROP) hGlobal, 0xFFFFFFFF, 0, 0 );
 
      // Get the names of these files and see if you can use any
      if ( cFiles > 1 || CheckUtilityThreadAlive() )
         m_bMultipleExplorerFiles = TRUE;    // Enable "yes/no to all"
      for  ( UINT ui = 0; ui < cFiles; ++ui )
      {
         WCHAR wchFilespec[ MAX_PATH ];
         DragQueryFileW( (HDROP) hGlobal, ui, wchFilespec, MAX_PATH );
         if ( NITFValidateImageFileType( wchFilespec, TRUE ) )
         {
#if ( 0 && defined _DEBUG ) || defined ALL_TESTS
            ATLTRACE( _T("NITF file paste: \"%s\"\n"), CW2T( wchFilespec ) );
#endif
            AddExplorerFile( wchFilespec, bNeedNewTempDB );
         }
      }
      AddExplorerFileEnd( TRUE );   // Reset the yes/no/all messages
   }

#ifdef JPIP_SUPPORT
   // Test if we have dropped a URL onto the map
   else if ( pDataObject->IsDataAvailable( CF_UNICODETEXT ) )
   {
      // Get the URL text from the COLEDataObject
      pDataObject->GetData( (CLIPFORMAT) CF_UNICODETEXT, &stg_medium );
      if ( stg_medium.tymed == TYMED_HGLOBAL )
      {
         hGlobal = stg_medium.hGlobal;

         LPCWSTR pwsz = (LPCWSTR) GlobalLock( hGlobal );

         wstring wstrExpandedURL;
         if ( TestURLSourceType( pwsz, &wstrExpandedURL ) )
            AddExplorerFile( wstrExpandedURL.c_str(), bNeedNewTempDB );

         GlobalUnlock( hGlobal );
      }
   }
#endif

   if ( !bNeedNewTempDB )
      DropTempDB();

   return FALSE;
}


// **************************************************************************
// **************************************************************************

VOID C_nitf_ovl::SaveViewCorners( MapProj* pMap )
{
   // Get view size
   INT cWidth, cHeight;
   pMap->get_surface_size( &cWidth, &cHeight );

   // Convert the four corners to lat/lon
   pMap->surface_to_geo( 0,      0,       &m_gptViewCorners[0].lat, &m_gptViewCorners[0].lon );
   pMap->surface_to_geo( cWidth, 0,       &m_gptViewCorners[1].lat, &m_gptViewCorners[1].lon );
   pMap->surface_to_geo( cWidth, cHeight, &m_gptViewCorners[2].lat, &m_gptViewCorners[2].lon );
   pMap->surface_to_geo( 0,      cHeight, &m_gptViewCorners[3].lat, &m_gptViewCorners[3].lon );

   if ( s_bEditOn )
      s_QueryDlg.SetQueryGeoRegions();
}


// **************************************************************************
// **************************************************************************

// Find the image associated with the screen x,y and display info about it

int C_nitf_ovl::show_info( C_icon* pIconObj )
{
   NITFObjPtr pObj = static_cast< C_nitf_obj* >( pIconObj );
   clear_selects();

   POSITION pos;
   if ( ( pos = m_objlist.Find( pObj ) ) != NULL )
   {
      // Put selected object at tail so draws on top
      m_objlist.RemoveAt( pos );
      m_objlist.AddTail( pObj );
      pObj->m_bSelected = TRUE;
      pObj->invalidate();

      CString csTitle;
      csTitle = _T("NITF File Info --- ");
      csTitle += pObj->m_filename;
      csTitle += " -- ";

      CString csEdit;
      if ( !pObj->get_nitf_info( csEdit ) )
      {
         csEdit = _T("Information for \"");
         csEdit += pObj->m_csFilespec + _T("\" is not available");
      }
      //Display the dialog with the information in it
      CRichTextFormatter richTextFormatter;
      richTextFormatter.add_text( std::string(csEdit), INFO_DATA_COLOR, INFO_DATA_NORM_SIZE, INFO_DATA_NORM_ATTRIB, FALSE );
      CRemarkDisplay::display_dlg( AfxGetApp()->m_pMainWnd, richTextFormatter.GetRichTextString().c_str(), csTitle, this, MAP_INFO_RICH_TEXT );

      return SUCCESS;
   }

   return FAILURE;
}
// End of show_info()


// **************************************************************************
// **************************************************************************

// Add NITF overlay commands to menu
VOID C_nitf_ovl::menu( ViewMapProj* pMap, CPoint ptMenu, CList< CFVMenuNode*, CFVMenuNode* >& list )
{
   s_ptMenuPoint = ptMenu;

   if ( !GetDefaultableDisplayAdjustments() )
      return;

   if ( !m_objlist.IsEmpty() )
   {
      POSITION pNext;
      C_nitf_obj* pObj;

      // Find number of filled images at this point
      INT cSelectedImages = 0, cFilledImages = 0,
         cDefaultableAdjustmentsImages = 0, cExternalViewableImages = 0;
      pNext = m_objlist.GetHeadPosition();
      while ( pNext != NULL )
      {
         pObj = &*m_objlist.GetNext( pNext );
         if ( pObj->hit_test( ptMenu ) )
         {
            cSelectedImages++;

            if ( pObj->m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
               cFilledImages++;

            if ( CanDisplayAdjustmentsBeDefaulted( pObj ) )
               cDefaultableAdjustmentsImages++;

            if ( pObj->m_eSourceLocationType <= SOURCE_LOCATION_TYPE_MAX_DIRECT )
               cExternalViewableImages++;
         }
      }

      // Assume no multiple selections needed
      LPCTSTR pszAllBreaker, pszFilledBreaker, pszHiddenBreaker, pszToTopBreaker,
         pszDoDefaultBreaker, pszExternalViewableBreaker;
      pszAllBreaker = pszFilledBreaker = pszHiddenBreaker = pszToTopBreaker =
         pszDoDefaultBreaker = pszExternalViewableBreaker = _T("  ");

      if ( cSelectedImages >= 2 )
         pszAllBreaker = _T("\\");

      if ( cFilledImages >= 2 )
         pszFilledBreaker = _T("\\");

      if ( cSelectedImages - cFilledImages >= 2 )
         pszHiddenBreaker = _T("\\");

      if ( cFilledImages >= 3 )
         pszToTopBreaker = _T("\\");

      if ( cDefaultableAdjustmentsImages >= 2 )
         pszDoDefaultBreaker = _T("\\");

      if ( cExternalViewableImages >= 2 )
         pszExternalViewableBreaker = _T("\\");

      BOOL  bTopFilledImageDone = FALSE;

      pNext = m_objlist.GetTailPosition();
      while ( pNext != NULL )
      {
         static const INT MAX_MENU_LINE_CHARS = 81;
         TCHAR tchMenuLine[ MAX_MENU_LINE_CHARS ];

         pObj = &*m_objlist.GetPrev( pNext );
         if ( pObj->hit_test( ptMenu ) )
         {
            _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
               _T("Info on image:%s%s"), pszAllBreaker, pObj->m_filename );
            list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &point_info ) );

            if ( pObj->m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
            {
               _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                  _T("Hide image:%s%s"), pszFilledBreaker, pObj->m_filename );
               list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &set_hide_image ) );

               _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                  _T("Adjust levels and transparency:%s%s"), pszFilledBreaker, pObj->m_filename );
               list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &toggle_translucent ) );

               _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                  _T("Go to native image scale:%s%s"), pszFilledBreaker, pObj->m_filename );
               list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &go_to_native_scale ) );

               if ( cFilledImages > 1 )
               {
                  // If topmost filled image
                  if ( !bTopFilledImageDone )
                  {
                     // Send-to-back only for topmost filled image.
                     _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                        _T("Send to back:%s%s"), pszToTopBreaker, pObj->m_filename );
                     list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &send_to_back ) );
                     bTopFilledImageDone = TRUE;
                  }
                  else
                  {
                     // Bring to front only for non-topmost filled images.
                     _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                        _T("Bring to front:%s%s"), pszToTopBreaker, pObj->m_filename );
                     list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &bring_to_front ) );
                  }  // Not top filled image

               }  // Multiple filled images

            }  // Filled image
            
            else  // Not filled
            {
               _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                  _T("Show image:%s%s"), pszHiddenBreaker, pObj->m_filename );
               list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &set_show_image ) );
            }

            if ( pObj->m_eSourceLocationType <= SOURCE_LOCATION_TYPE_MAX_DIRECT )
            {
               _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                  _T("External viewer:%s%s"), pszExternalViewableBreaker, pObj->m_filename );
               list.AddTail( new CCIconMenuItem( tchMenuLine, pObj, &view_image ) );
            }

            if ( CanDisplayAdjustmentsBeDefaulted( pObj ) )
            {
               _sntprintf_s( tchMenuLine, MAX_MENU_LINE_CHARS, _TRUNCATE,
                  _T("Restore default NITF settings:%s%s"),
                  pszDoDefaultBreaker, pObj->m_filename );
               list.AddTail( new CCIconMenuItem( tchMenuLine,
                                                pObj, &ResetImageDisplayAdjustments ) );
            }

         }  // Object hit
      }  // Object list loop

      if ( cSelectedImages > 0 )
         list.AddTail( new CFVMenuNode( _T("SEPARATOR") ) );
   }  // Any objects

   BOOL bSeparatorNeeded = FALSE;

   // Allow user to hide all images
   FilterDfltableDsplyAdjs( L"Fill <> 0" );
   if ( !m_sprsDfltableDsplyAdjs->adoEOF )
   {
      list.AddTail( new CLparamMenuItem( _T("Hide all tactical images"), 0, &hide_all_images ) );
      bSeparatorNeeded = TRUE;
   }

   // If any global display cleanup possible
   FilterDfltableDsplyAdjs( NULL );
   if ( !m_sprsDfltableDsplyAdjs->adoEOF )
   {
      list.AddTail( new CLparamMenuItem( _T("Restore all default tactical images per-image settings"), 0,
         &ResetAllPerImageDisplayAdjustments ) );
      bSeparatorNeeded = TRUE;
   }

#if defined FILTER_RESULTS_ONLY_RCLICK_OPTION || defined IMAGE_ONLY_NO_EDITOR_RCLICK_OPTION      
   CFVMenuNode* pNode;

#ifdef FILTER_RESULTS_ONLY_RCLICK_OPTION      
   if ( s_pNITFOvl->m_eQueryFilterDisplay != QUERY_FILTER_DISPLAY_UNSPECIFIED )
   {
      pNode = new CFVMenuNode(
         _T("Show only filtered NITF Query Tool results (else all NITF)"), 
         &ToggleQueryFilterDisplay );
      pNode->SetMFFlags( m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_TRUE
                           ? MF_CHECKED : MF_UNCHECKED );
      list.AddTail( pNode );
      bSeparatorNeeded = TRUE;
   }
#endif

#ifdef IMAGES_ONLY_NO_EDITOR_RCLICK_OPTION
   pNode = new CFVMenuNode(
      _T("No NITF graphics when NITF Query Tool is off"), 
      &ToggleImagesOnlyNoEditor );
   pNode->SetMFFlags( m_eImagesOnlyNoEditor == IMAGES_ONLY_NO_EDITOR_FALSE
                           ? MF_UNCHECKED : MF_CHECKED );
   list.AddTail( pNode );
   bSeparatorNeeded = TRUE;
#endif
#endif   // Either RCLICK option

   // See if there are any query geobounds regions to delete
   if ( s_bEditOn && m_agbpQueryRegions.size() > 0 )
   {
      list.AddTail( new CCIconLparamMenuItem( "Remove all NITF query regions",
         &s_pNITFOvl->m_gbpScratch, &RemoveQueryRegion ) );

      GeoBoundsPtrIter it;
      for ( it = m_agbpQueryRegions.begin(); it != m_agbpQueryRegions.end(); it++ )
      {
         GeoBounds& gb = it->GetGeoBounds();
         if ( gb.hit_test( ptMenu ) )
         {
            DOUBLE dS, dW, dN, dE, dX[4], dY[4], dMin = +1e30;
            gb.get_bounds( dS, dW, dN, dE );
            pMap->geo_to_surface( dS, dW, &dX[0], &dY[0] );  // SW corner
            pMap->geo_to_surface( dN, dW, &dX[1], &dY[1] );  // NW corner
            pMap->geo_to_surface( dN, dE, &dX[2], &dY[2] );  // NE corner
            pMap->geo_to_surface( dS, dE, &dX[3], &dY[3] );  // SE corner

            // Looking for which edge is closest
            INT iEdge;
            for ( INT i1 = 0; i1 < 4; i1++ )
            {
               INT i2 = ( i1 + 1 ) % 4;   // Next vertex CW

               DOUBLE d = CFvwUtil::get_instance()->distance_to_line(
                  dX[i1], dY[i1], dX[i2], dY[i2], (DOUBLE) ptMenu.x, (DOUBLE) ptMenu.y );
               if ( d < dMin )
               {
                  iEdge = i1;     // Preliminary
                  dMin = d;
               }
            }
            static LPCTSTR const pszDirections[] =
               { _T("east"), _T("south"), _T("west"), _T("north") };
            CString cs = _T("Remove NITF query region to ");
            cs += pszDirections[ iEdge ];
            list.AddTail( new CCIconLparamMenuItem( cs, &*it, &RemoveQueryRegion ) );
         }  // Region hit test
      }  // Query regions loop
      bSeparatorNeeded = TRUE;
   }  // Edit on and regions exist

   if ( bSeparatorNeeded )
      list.AddTail( new CFVMenuNode( _T("SEPARATOR") ) );

   // Cleanup
   if ( s_eDBServerType != IMAGEDBSERVER_SQLSERVER_SERVER_TYPE )
   {
      static const BSTR bsDropTempTable =
         L"DROP TABLE DefaultableDisplayAdjustments\n";
      s_pNITFDBAgent->ExecuteSQL( bsDropTempTable );
   }
   m_sprsDfltableDsplyAdjs = NULL;

}
// end of menu

// **************************************************************************
// **************************************************************************

void C_nitf_ovl::view_image(ViewMapProj *map, C_icon *pt)
{
   C_nitf_obj *obj;
   CString viewer, path, latstr, lonstr, geostr, tstr;
   double lat, lon;
   CFvwUtil *futil = CFvwUtil::get_instance();
   int errnum;

   obj =  (C_nitf_obj *) pt;

   // viewer = PRM_get_registry_string("NitfFile", "ExtViewer", "");

   path = obj->m_csFilespec;

   // check to see if GeoRect is the NITF app
   DWORD type = REG_SZ;
   char value[301];
   unsigned long size = 300;
   PRM_read_registry(HKEY_CLASSES_ROOT,".ntf", "", &type, (unsigned char*) value, &size);
   tstr = value;
   if ( tstr.CompareNoCase("georect") == 0 )
   {
      // get the location of the cursor
      map->surface_to_geo( s_ptMenuPoint.x, s_ptMenuPoint.y, &lat, &lon );
      latstr.Format("%09.6f", lat);
      lonstr.Format("%010.6f", lon);
      geostr.Format("-g%s,%s", latstr, lonstr);

      futil->shell_execute(path.GetBuffer(250), geostr.GetBuffer(50), &errnum);
   }
   else
      futil->shell_execute(path.GetBuffer(250), &errnum);
}
// end of view_image


//-----------------------------------------------------------------------------
VOID C_nitf_ovl::ToggleQueryFilterDisplay( CString strMenuItemName )
{
   s_pNITFOvl->m_eQueryFilterDisplay =
      s_pNITFOvl->m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_TRUE
      ? QUERY_FILTER_DISPLAY_FALSE : QUERY_FILTER_DISPLAY_TRUE;
   
   s_pNITFOvl->m_bReload = TRUE;
   OVL_get_overlay_manager()->invalidate_all( FALSE );

}  // End of C_nitf_ovl::ToggleQueryFilterDisplay()


//-----------------------------------------------------------------------------
VOID C_nitf_ovl::ToggleImagesOnlyNoEditor( CString strMenuItemName )
{
   s_pNITFOvl->m_eImagesOnlyNoEditor =
      s_pNITFOvl->m_eImagesOnlyNoEditor == IMAGES_ONLY_NO_EDITOR_FALSE
      ? IMAGES_ONLY_NO_EDITOR_TRUE : IMAGES_ONLY_NO_EDITOR_FALSE;
   
   PRM_set_registry_string( "NitfFile", "ImagesOnlyNoEditor",
      s_pNITFOvl->m_eImagesOnlyNoEditor == IMAGES_ONLY_NO_EDITOR_FALSE ? "N" : "Y");

   s_pNITFOvl->m_bReload = TRUE;
   OVL_get_overlay_manager()->invalidate_all( FALSE );

}  // End of C_nitf_ovl::ToggleImagesOnlyNoEditor()


// **************************************************************************
// **************************************************************************

// temp test code -- REMOVE !!!
//#include "..\fvw.h"
//#include "..\getobjpr.h"

void C_nitf_ovl::point_info(ViewMapProj *map, C_icon *pt)
{
   C_nitf_ovl *thisovl;

   // call info display routine
   thisovl =  (C_nitf_ovl *) pt->m_overlay;
   thisovl->show_info(pt);
}
// end of point_info


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::set_show_image( ViewMapProj* pMap, C_icon* pIcon )
{
   C_nitf_obj* pObj = static_cast< C_nitf_obj* >( pIcon );
   
   pObj->m_eFillControl = FILL_CONTROL_FILLED_APP_DFLT_FRAME;
   pObj->set_display_mode();
   pObj->invalidate();

}  // end of set_show_image


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::set_hide_image( ViewMapProj* pMap, C_icon* pt )
{
   C_nitf_obj* pObj = static_cast< C_nitf_obj* >( pt );
   
   pObj->m_eFillControl = FILL_CONTROL_OUTLINE_FRAME;
   pObj->set_display_mode();
   pObj->invalidate();

}  // end of set_hide_image


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::send_to_back( ViewMapProj* pMap, C_icon* pt )
{

   s_pNITFOvl->make_topmost_or_bottommost( static_cast< C_nitf_obj* >( pt ), FALSE );

}  // End of C_nitf_ovl::send_to_back()


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::bring_to_front( ViewMapProj* pMap, C_icon* pt )
{

   s_pNITFOvl->make_topmost_or_bottommost( static_cast< C_nitf_obj* >( pt ), TRUE );

}  // End of C_nitf_ovl::bring_to_front()


// **************************************************************************
// **************************************************************************

VOID C_nitf_ovl::hide_all_images( CString strMenuItemName, LPARAM )
{
   POSITION pNext = s_pNITFOvl->m_objlist.GetHeadPosition();
   while ( pNext != NULL )
      s_pNITFOvl->m_objlist.GetNext( pNext )->
         m_eFillControl = FILL_CONTROL_OUTLINE_FRAME;

   wostringstream wssUpdateList;

   // Hide all images
   wssUpdateList <<
      L"  FillControl = " << FILL_CONTROL_OUTLINE_FRAME << L"\n";
   
   s_pNITFDBAgent->UpdateImageDisplayInfoRecord(
      L"", 0,              // Wildcard images
      L"Generic", L"Generic",
      _bstr_t( wssUpdateList.str().c_str() ),
      (LONG) FALSE );   // Don't create any unnecessary records

   OVL_get_overlay_manager()->invalidate_all(FALSE);
}
// End of C_nitf_ovl::hide_all_images()


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::toggle_translucent(ViewMapProj *map, C_icon *pt)
{
   NITFObjPtr spObj( static_cast< C_nitf_obj* >( pt ) );
   CHistTransDlg dlg( spObj );
   dlg.DoModal();
   s_pNITFOvl->m_bReload = TRUE;
   OVL_get_overlay_manager()->invalidate_all( FALSE );
}
// End of C_nitf_ovl::toggle_translucent()


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::go_to_native_scale( ViewMapProj* pMap, C_icon* pObj )
{
   double lat, lon;
   ProjectionEnum proj_type; 
   int zoom_percent;
   double rotation;
   MapScale mscale;

   pMap->surface_to_geo( s_ptMenuPoint.x, s_ptMenuPoint.y, &lat, &lon );
   // lat = map->actual_center_lat();
   // lon = map->actual_center_lon();
   proj_type = pMap->projection_type();
   zoom_percent = pMap->actual_zoom_percent();
   rotation = pMap->actual_rotation();

   CView* view = UTL_get_active_non_printing_view();

   // CList<MapType *, MapType *> map_list;
   // MapType *map_type;
   // int map_handle_index = 0;
   
   // MAP_get_map_list(BLANK_CATEGORY, map_list); 

   int imgscale = (int) (static_cast< C_nitf_obj* >( pObj )->m_pixel_size_in_meters);
   if (imgscale < 1)
      imgscale = 1;

   const MapSource curr_source = BLANK_MAP;
   MapScale curr_scale(imgscale, MapScale::METERS);
   const MapSeries curr_series = "";

   MapCategory category = "BLANK";

   int status = UTL_change_view_map_to_closest(view, category,
      curr_source, curr_scale, curr_series, lat, lon, 
      rotation, zoom_percent, proj_type);

      // put up meaningful dialog and leave dialog box up
      if (status != SUCCESS)
      {
         return;
      }

      OVL_get_overlay_manager()->invalidate_all(FALSE);
}
// End of C_nitf_ovl::go_to_native_scale()


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::RemoveQueryRegion( ViewMapProj* pMap, C_icon* pIcon, LPARAM lParam )
{
   GeoBoundsPtrIter it;
   for ( it = s_pNITFOvl->m_agbpQueryRegions.begin(); it != s_pNITFOvl->m_agbpQueryRegions.end(); )
   {
      if ( pIcon == &s_pNITFOvl->m_gbpScratch || pIcon == &*it )
      {
         GeoBoundsPtr gbp( *it );   // Copy the GeoBoundsPtr
         it = s_pNITFOvl->m_agbpQueryRegions.erase( it );
         gbp.GetGeoBounds().prepare_for_redraw( pMap );
         gbp.GetGeoBounds().invalidate();
      }
      else
         it++;
   }
   s_QueryDlg.SetQueryGeoRegions();
}

void C_nitf_ovl::clear_selects() 
{ 
   POSITION pPrev = m_objlist.GetTailPosition();
   while ( pPrev != NULL )
   {
      C_nitf_obj& obj = *m_objlist.GetPrev( pPrev );
      if ( obj.m_bSelected )
      {
         // clear the selected icon if item is selected
         obj.m_bSelected = FALSE;
         obj.invalidate( TRUE );
      }
   }
   m_bSelectedObj = FALSE;
}
// end of clear_selects


// ******************************************************************************
// ******************************************************************************

int C_nitf_ovl::open( const CString& csFilespec )
{
   CWaitCursor wait;

   int iRslt = open_nitf_database();
   if ( iRslt != SUCCESS )
      return FAILURE;

   // If started with a file to open, add it to the DB if necessary
   if ( !csFilespec.IsEmpty() )
   {
      BOOL bNeedNewTempDB = TRUE;
      AddExplorerFile( (LPCWSTR) _bstr_t( csFilespec ), bNeedNewTempDB );
      if ( !bNeedNewTempDB )
         DropTempDB();
   }
 
   set_valid(TRUE);
   return SUCCESS;
}
// end of open


// ******************************************************************************
// *****************************************************************

map_scale_t C_nitf_ovl::get_smallest_scale()
{ 
   CString display_above = PRM_get_registry_string("NitfFile", 
      "NitfDisplayAbove", "1:5 M");
   set_smallest_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_scale; 
}


// *****************************************************************
// *****************************************************************

void C_nitf_ovl::set_smallest_scale(map_scale_t scale) 
{ 
   m_smallest_scale = scale; 
}

// *****************************************************************
// *****************************************************************

map_scale_t C_nitf_ovl::get_smallest_labels_scale()
{ 
   CString display_above = PRM_get_registry_string("NitfFile", 
      "NITFLabelThreshold", "1:250 K");

   set_smallest_labels_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_labels_scale; 
}
// end of get_smallest_labels_scale

// *****************************************************************
// *****************************************************************

void C_nitf_ovl::set_smallest_labels_scale(map_scale_t scale) 
{ 
   m_smallest_labels_scale = scale; 
}


//
// RedrawTimerProc()
//
VOID C_nitf_ovl::RedrawTimerProc( HWND hWnd, UINT uiMsg, UINT_PTR idEvent, DWORD dwTime )
{
   if ( !s_pNITFOvl->m_bDrawInProgress )
   {
      ATLASSERT( idEvent == s_pNITFOvl->m_uiRedrawTimerId );

      // If time to retry any busy server
      if ( (INT) ( GetTickCount() - s_pNITFOvl->m_dwLastDrawTicks ) >= s_pNITFOvl->m_iRedrawDelayTicks )
      {
         s_pNITFOvl->m_dwLastDrawTicks += 1000 * 1000;      // Push this away, draw will readjust

         // Start a redraw
         MapView* map_view = fvw_get_view();
         if (map_view)
         {
            map_view->PostMessage(WM_INVALIDATE_FROM_THREAD,
               (WPARAM) new MapViewInvalidateAll(FALSE));
         }
      }
   }
}


// **************************************************************************
// **************************************************************************

STDMETHODIMP ImageLibCallbackInterface::raw_imagelib_progress(double percent, BSTR label)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState())
   m_dProgressPercent = percent;       // For tracking

   // If working blind, check whether to abort
   if ( m_eCallbackMode != CALLBACK_MODE_NORMAL )
      return ( m_eCallbackMode == CALLBACK_MODE_ABORTING ) ? S_FALSE : S_OK;

   CMainFrame *frame = (CMainFrame *)UTL_get_frame();
   if (!frame)
      return S_FALSE;

   CString slabel;

   AfxBSTR2CString(&slabel, label);

   if (percent < -900.0)
   {
      int rslt;
      slabel += "\r\n\r\n";
      slabel += "Do you want to Abort?";
      rslt = AfxMessageBox(slabel, MB_YESNO|MB_ICONSTOP);
      if (rslt == IDYES)
         return S_FALSE;
      else
         return S_OK;
   }

   if ((percent >= 0.0) && (percent <= 100.0))
      frame->m_statusBarMgr->m_arrStatusBars[0]->set_progressbar_percent(percent, slabel);
   else
      frame->m_statusBarMgr->m_arrStatusBars[0]->clear_progressbar();

   return S_OK;
}
// end of raw_imagelib_progress


// **************************************************************************
// **************************************************************************

#ifdef NO_SQL_SERVER_DB   // In nitf_sql.cpp if using SQL_Server database

// **************************************************************************
// **************************************************************************

void C_nitf_ovl::add_single_file(CString filename)
{
   C_nitf_db db;
   CNitfUtil util;
   C_nitf_file_set set;
   int rslt;

   rslt = db.open_file_set();
   if (rslt != SUCCESS)
      return;

   rslt = util.add_file(filename, &db);
   if (rslt != SUCCESS)
      return;

   rslt = util.get_file_set_info(filename, &set);
   if (rslt != SUCCESS)
      return;

   add_obj(&set);
}

// **************************************************************************
// **************************************************************************
#endif

// **************************************************************************
// **************************************************************************
// The NITF overlay is being turned off
int C_nitf_ovl::pre_close( boolean_t* pbCancel )
{
   // If the query tool is on, turn it off
   if ( OVL_get_overlay_manager()->GetCurrentEditor() == FVWID_Overlay_NitfFiles )
      OVL_get_overlay_manager()->set_mode( GUID_NULL, FALSE );

   EnterCriticalSection( &m_csUtilityProcAccess );
   m_wtdUtilityControl.clear();          // Flush any waiting work
   LeaveCriticalSection( &m_csUtilityProcAccess );

   if ( m_pUtilityThread != NULL )
   {
      BOOL b = PostThreadMessage( m_pUtilityThread->m_nThreadID, WM_QUIT, 0, 0 );
      WaitForSingleObject( m_pUtilityThread->m_hThread, INFINITE );
      delete m_pUtilityThread;
      m_pUtilityThread = NULL;
   }

   if ( pbCancel != NULL )
      *pbCancel = FALSE;

   return SUCCESS;
}


// **************************************************************************
// **************************************************************************
// Turn the NITF Data Manager on or off.
int C_nitf_ovl::set_edit_on( boolean_t edit_on )
{
   if ( OVL_get_overlay_manager()->get_first_of_type( FVWID_Overlay_NitfFiles ) == NULL )
   {
      C_overlay* pOverlay;
      OVL_get_overlay_manager()->create( FVWID_Overlay_NitfFiles, &pOverlay );
   }

   // no change - do nothing
   if ( s_bEditOn == edit_on )
      return SUCCESS;

   #if defined TOOL_TRACE
   ATLTRACE( _T("C_nitf_ovl::set_edit_on( %d )\n"), edit_on );
   #endif

   // Make sure any flash timer is off
   POSITION pos = UTL_get_timer_list()->Find( &s_pNITFOvl->m_clsFlashTimer );
   if ( pos != NULL )         // Make sure it's not connected
      UTL_get_timer_list()->remove_at( pos );

   if ( !s_QueryDlg.GetSafeHwnd() )
      s_QueryDlg.Create( IDD_NITF_QUERY, theApp.m_pMainWnd );

   if ( s_QueryDlg.m_bInitializeDone )
      s_QueryDlg.EnableCtrl( edit_on );

   if ( edit_on )
   {
      // Set up the flash completion timer
      UTL_get_timer_list()->add( &s_pNITFOvl->m_clsFlashTimer );

      CWnd *pFrame = UTL_get_frame();
      if (pFrame)
         pFrame->SetFocus();  // Set the focus to the main window after MDM is created.  (allows use of intellimouse)
   }

#ifdef WARP_TEST
   // MATT_TODO ---------------------------------------------------
   // This is only for testing purposes. The final query results
   // will be pulled into a control that Russ is developing using 
   // the metadata retrieval functions.
   if ( !C_nitf_ovl::m_WarpQueryDlg.GetSafeHwnd() )
      m_WarpQueryDlg.Create( IDD_WARP_QUERY );
   m_WarpQueryDlg.ShowWindow( edit_on ? SW_SHOW : SW_HIDE );
   // -------------------------------------------------------------
#endif

   // Closing the NITF query tool
   if ( !edit_on )
   {
      // if the coverage overlay exists
      //if (overlay)
      //{
      //   // remove the overlay from the list and the display, and delete object
      //   OVL_get_overlay_manager()->delete_overlay(overlay);
      //   delete overlay;
      //}
   }
   else
   {
      // Initialize
      // JIM_TODO ???
   }

   s_bEditOn = edit_on;

   return SUCCESS;
}


//
// InitRegistry() - set up FalconView image hooks
//
VOID C_nitf_ovl::InitRegistry( BOOL bUnregister )
{
   static const size_t APP_COMMAND_SIZE = MAX_PATH + 20;
   WCHAR wchAppCommand[ APP_COMMAND_SIZE ];
   CQueuedAppRegistration qar;   // Utility for registering queued application handlers

#ifdef JPIP_SUPPORT
   HKEY hk;

   // Find the GeoRect application
   do
   {
      if ( bUnregister )
         break;         // Don't worry about GeoRect when unregistering

      if ( RegOpenKeyExW( HKEY_CLASSES_ROOT, L"Applications\\GeoRect.exe\\Shell\\Open\\Command",
            0, KEY_READ, &hk ) != ERROR_SUCCESS )
         break;

      // Get the GeoRect open command syntax
      DWORD
         dwType,
         dwSize = sizeof(wchAppCommand);
      if ( RegQueryValueExW( hk, L"", 0, &dwType, (LPBYTE) wchAppCommand, &dwSize ) == ERROR_SUCCESS )
      {
         if ( dwType == REG_SZ )
         {
            // First register GeoRect as the JPIP protocol handler.  It will be the first alternate
            qar.RegisterQueuedApp( FALSE, HKEY_CLASSES_ROOT, L"jpip",
               L"Shell\\Open\\Command", wchAppCommand );
         }
      }
      RegCloseKey( hk );
   } while ( FALSE );
#endif

   // Construct the FalconView open command
   try
   {
      GetModuleFileNameW( NULL, wchAppCommand, sizeof(wchAppCommand) / sizeof(wchAppCommand[0]) );
      wcscat_s( wchAppCommand, APP_COMMAND_SIZE, L" /O \"%1\"" );

#ifdef JPIP_SUPPORT
      // Register or unregister FalconView as the primary handler
      qar.RegisterQueuedApp( bUnregister, HKEY_CLASSES_ROOT, L"jpip",
         L"Shell\\Open\\Command", wchAppCommand );

      // Make sure that the class info is set up
      if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, _T("jpip"), 0, KEY_WRITE, &hk ) == ERROR_SUCCESS )
      {
         LPCTSTR psz = _T("JPIP Interactive Imaging Protocol");
         RegSetValueEx( hk, _T(""), 0, REG_SZ,
            (LPBYTE) psz, ( _tcslen( psz ) + 1 ) * sizeof(TCHAR) );

         psz = _T("");
         RegSetValueEx( hk, _T("URL Protocol"), 0, REG_SZ,
            (LPBYTE) psz, ( _tcslen( psz ) + 1 ) * sizeof(TCHAR) );

         RegCloseKey( hk );
      }

#else
      // Only unregister FalconView if no JPIP support
      if ( bUnregister )
         qar.RegisterQueuedApp( bUnregister, HKEY_CLASSES_ROOT, L"jpip",
            L"Shell\\Open\\Command", wchAppCommand );
#endif
   }
   catch( ... ) {}
}  // End of InitRegistry()


////////////////////////////////////////////
//
// Query Tool toolbar handling
//
////////////////////////////////////////////

//
// UpdateQueryToolToolbar() - set button enables, checked states, and tooltips
//
VOID C_nitf_ovl::UpdateQueryToolToolbar()
{
   CMainFrame* pMainFrame = (CMainFrame*) UTL_get_frame();

   const CNITFTBClass::ButtonLookup* pbl = CNITFTBClass::s_blButtonLookup;
   do
   {
      _variant_t varResult1, varResult2;

      if ( s_QueryDlg.m_ctlNITFQueryDlg.SpecialFunction(
         QUERY_DLG_SPECFUNC_UPDATE_TOOLBAR_BUTTON,
         _variant_t( pbl->iButtonNum + QUERY_DLG_TOOLBAR_MIN_BUTTON ),
         _variant_t( VARIANT_FALSE ), VARIANT_NULL,        // FALSE = not clicked
         &varResult1, &varResult2 ) )
      {
         // If button state specified
         if ( varResult1.vt != VT_NULL && varResult1.vt != VT_EMPTY )
            m_mQueryToolbarStates[ pbl->iButtonNum ] = varResult1;
         
         // If tooltip text specified
         if ( varResult2.vt == VT_BSTR )
         {
            // Get the combined status message and tool tip text
            CString* pcsMessageString = m_csMessageStrings + pbl->iButtonNum;
            *pcsMessageString = CW2T( varResult2.bstrVal );
            INT iPos = pcsMessageString->ReverseFind( _T('\n') );
            if ( iPos != -1 )
            {
               /* Update the button text on the toolbar
               pMainFrame->m_nitf_data_manager_toolbar.set_button_text(
                  pbl->uiButtonID, pcsMessageString->Mid( iPos + 1 ) );
                  */

               pcsMessageString->Truncate( iPos );
            }
         }
      }
   } while ( (++pbl)->iButtonNum != -1 );
}


VOID C_nitf_ovl::OnToolbarCmd( UINT nID )
{
   INT iButtonNum;
   _variant_t varResult1, varResult2;

   switch( nID )
   {
      case ID_NITF_SELECT:       // Normal FVW pointer mode
         SetEditMode( EDIT_MODE_SELECT );
         break;

      case ID_NITF_AREA_SELECT:  // Region-of-Interest drawing mode
         SetEditMode( EDIT_MODE_AREA );
         break;

      case ID_NITF_ERASE_AREAS:  // Erase all regions-of-interest
         s_pNITFOvl->m_agbpQueryRegions.clear();
         s_QueryDlg.SetQueryGeoRegions(); // Tell query tool
         OVL_get_overlay_manager()->invalidate_all( FALSE );
         break;

      case ID_NITF_QUERY_EDITOR: // Toggle query editor visibility
         s_QueryDlg.m_lEnableQueryEditor =
            s_QueryDlg.m_lEnableQueryEditor == SW_HIDE ? SW_SHOW : SW_HIDE;
         s_QueryDlg.EnableCtrl();
         break;

      case ID_NITF_METADATA_LIST:   // Toggle metadata list visibility
         s_QueryDlg.m_lEnableMetadataList =
            s_QueryDlg.m_lEnableMetadataList == SW_HIDE ? SW_SHOWNA : SW_HIDE;
         s_QueryDlg.EnableCtrl();
         break;

#if 0
      case ID_NITF_OPEN_CONFIG_1:
      case ID_NITF_OPEN_CONFIG_2:
      case ID_NITF_OPEN_CONFIG_3:
      case ID_NITF_OPEN_CONFIG_4:
#endif
      case ID_NITF_LOCAL_SEARCH:
#if 0
      case ID_NITF_NETWORK_SEARCH:
#endif
      case ID_NITF_WEB_SEARCH:
         if ( ( iButtonNum = CNITFTBClass::GetToolbarButtonNum( nID ) ) < 0 )
            break;               // Couldn't translate
             
         if ( s_QueryDlg.m_ctlNITFQueryDlg.SpecialFunction(
            QUERY_DLG_SPECFUNC_UPDATE_TOOLBAR_BUTTON,
            _variant_t( iButtonNum + QUERY_DLG_TOOLBAR_MIN_BUTTON ),
            _variant_t( VARIANT_TRUE ), VARIANT_NULL,  // TRUE = clicked
            &varResult1, &varResult2 ) )
         {
            // If button state specified
            if ( varResult1.vt != VT_NULL && varResult1.vt != VT_EMPTY )
               m_mQueryToolbarStates[ iButtonNum ] =
                  _variant_t( varResult1 );
         }
         break;

      case ID_NITF_DISPLAY_FILTER:
         m_eQueryFilterDisplay = m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_TRUE
            ? QUERY_FILTER_DISPLAY_FALSE : QUERY_FILTER_DISPLAY_TRUE;
         m_bReload = TRUE;
         OVL_get_overlay_manager()->invalidate_all( FALSE );
         break;

   }  // switch( nID )
}  // End of OnToolbarCmd()


VOID C_nitf_ovl::OnToolbarUpdateCmdUI( CCmdUI* pCmdUI )
{
   INT iButtonNum;
   switch( pCmdUI->m_nID )
   {
      case ID_NITF_SELECT:
         pCmdUI->SetCheck( GetEditMode() == EDIT_MODE_SELECT );
         break;

      case ID_NITF_AREA_SELECT:
         pCmdUI->SetCheck( GetEditMode() == EDIT_MODE_AREA );
         break;

      case ID_NITF_ERASE_AREAS:
         pCmdUI->Enable( m_agbpQueryRegions.size() > 0 );
         break;

      case ID_NITF_QUERY_EDITOR:
         pCmdUI->SetCheck( s_QueryDlg.m_lEnableQueryEditor != SW_HIDE );
         break;

      case ID_NITF_METADATA_LIST:
         pCmdUI->SetCheck( s_QueryDlg.m_lEnableMetadataList == SW_SHOWNA );
         break;

#if 0
      case ID_NITF_OPEN_CONFIG_1:
      case ID_NITF_OPEN_CONFIG_2:
      case ID_NITF_OPEN_CONFIG_3:
      case ID_NITF_OPEN_CONFIG_4:
#endif
      case ID_NITF_LOCAL_SEARCH:
#if 0
      case ID_NITF_NETWORK_SEARCH:
#endif
      case ID_NITF_WEB_SEARCH:
         iButtonNum = CNITFTBClass::GetToolbarButtonNum( pCmdUI->m_nID );
         pCmdUI->Enable( ( m_mQueryToolbarStates[ iButtonNum ]
            & QUERY_DLG_TOOLBAR_BUTTON_ENABLE_MASK ) != 0 );
         pCmdUI->SetCheck(( m_mQueryToolbarStates[ iButtonNum ]
            & QUERY_DLG_TOOLBAR_BUTTON_CHECKED_MASK ) != 0 );
         break;

      case ID_NITF_DISPLAY_FILTER:
         pCmdUI->SetCheck( m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_TRUE );
         break;

   }  // switch( pCmdUI->m_nID )
}

// End of nitf.cpp