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

// NITFObj.cpp -- Implementation of C_nitf_obj class




#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "NITF.h"
#include "param.h"
#include "map.h"
#include "dib.h"
#include "PolygonClipper.h"
#include "..\mapview.h"
#include "nitf_util.h"
#include "ComErrorObject.h"
#include "GeometryUtils.h"
#define NITF_CONVERSIONS_ONLY
#include "NITFUtilities.h"
#undef NITF_CONVERSIONS_ONLY
#include "GeometryUtils.h"
#include "fid.h"

#ifndef NO_USE_GETMAPPEDIMAGE
   #define USE_GETMAPPEDIMAGE
#endif

#ifndef NO_ALWAYS_ROTATED
   #define ALWAYS_ROTATED
#endif

#ifdef USE_GETMAPPEDIMAGE
   #ifdef NO_RPC00B
      static const LONG MAPPING_OPTIONS = 0 * ( (1<<MAPPING_OPT_RPC00B) | (1<<MAPPING_OPT_DTED) );
   #else
      static const LONG MAPPING_OPTIONS = 1 * ( (1<<MAPPING_OPT_RPC00B) | (1<<MAPPING_OPT_DTED) );
   #endif
#endif

#if 1
   static const INT BOUNDS_LINE_TYPE = UTIL_LINE_TYPE_RHUMB;   // Fits better on curved map projections
#else
   static const INT BOUNDS_LINE_TYPE = UTIL_LINE_TYPE_SIMPLE;
#endif


// **************************************************************************
// **************************************************************************
//       CJPIPCallback
// **************************************************************************
// **************************************************************************

STDMETHODIMP CJPIPCallback::raw_ImageLibCallbackSpecialFunction(
            /*[in]*/ EnumImageLibCallbackSpecFuncCode eFunctionCode,
            /*[in]*/ VARIANT varInParam1,
            /*[in]*/ VARIANT varInParam2,
            /*[out]*/ VARIANT* pvarOutParam )
{
   _variant_t vartInParam2 = varInParam2;

   switch ( eFunctionCode )
   {
      case IMAGELIB_CALLBACK_SPEC_FUNC_JPIP_DOWNLOAD_PROGRESS:
#if ( 0 && defined _DEBUG ) || defined ALL_TESTS
            ATLTRACE( _T("Progress callback \"%s\", time = %u, status = %d\n"),
               m_pNITFObj->m_filename, GetTickCount(), vartInParam2.lVal );
#endif
         if ( vartInParam2.lVal == (LONG) IMAGELIB_CALLBACK_SPEC_FUNC_PROGRESS_COMPLETE_SOURCE_BUSY )
            C_nitf_ovl::s_pNITFOvl->m_bImageSourceBusy = TRUE;  // Server was busy

         else if ( m_pNITFObj->m_bIncompleteDraw )       // If last drawing was done with incomplete data
         {
            if ( C_nitf_ovl::s_pNITFOvl->m_bDrawInProgress )
               C_nitf_ovl::s_pNITFOvl->m_bInvalidateRequested = TRUE;   // Redo the draw later
            else
               OVL_get_overlay_manager()->invalidate_from_thread( FALSE ); // Redo now
         }
         break;

      case IMAGELIB_CALLBACK_SPEC_FUNC_JPIP_COMPLETE_RENDER:
      {
         try
         {
            vartInParam2.ChangeType( VT_BOOL );
            C_nitf_ovl::s_pNITFOvl->m_bIncompleteDraw |= m_pNITFObj->m_bIncompleteDraw =
                  vartInParam2.boolVal == VARIANT_FALSE; // Drawing is being done with incomplete data?
#if ( 0 && defined _DEBUG ) || defined ALL_TESTS
            ATLTRACE( _T("Render complete callback \"%s\", time = %u, incomplete = %d\n"),
                  m_pNITFObj->m_filename, GetTickCount(), m_pNITFObj->m_bIncompleteDraw );
#endif
         }
         catch ( ... )
         {
         }
         break;
      }

      case IMAGELIB_CALLBACK_SPEC_FUNC_NO_OP:
      default:
         break;
   
   }  // switch ( eFunctionCode )
   return S_OK;
}


// **************************************************************************
// **************************************************************************
//       C_nitf_obj
// **************************************************************************
// **************************************************************************

C_nitf_obj::C_nitf_obj( C_overlay* pNITFOverlay ) :
      C_icon( pNITFOverlay ),
         m_pxdDisplayParams( __uuidof( MSXML2::DOMDocument60 ) )
      {
         m_bnd_ll.lat = 0.0;
         m_bnd_ll.lon = 0.0;
         m_bnd_ur.lat = 0.0;
         m_bnd_ur.lon = 0.0;
         m_rect.SetRectEmpty();
         m_eFileType = NITF_UNDEFINED_FILE;
         m_width = 0;
         m_height = 0;
         m_cDisplayImagePixels = 0;
         m_has_geo = FALSE;
         m_bSelected = FALSE;
         m_eFillControl = FILL_CONTROL_OUTLINE_FRAME;
         m_eFileAccessStatus = FILE_ACCESS_NOT_CHECKED;  // Need to check for file existance
         m_eq_mode = 2;  
         m_translucent = TRUE;
         m_trans_value = 255;
         m_is_color = FALSE;
         m_mTempDisplayMap = NITF_TEMP_DISPLAY_NORMAL_MASK;
         m_dwBeginFlashTicks = 0;

         m_contr_minval = 1;
         m_contr_ctrval = 128;
         m_contr_maxval = 255;
         m_freq_lum = NULL;
         //   CO_CREATE( m_pxdDisplayParams, __uuidof( MSXML2::DOMDocument60 ) );

         m_use_icon = FALSE;
         m_image_loaded = FALSE;
         m_is_multispectral = FALSE;
         m_eImageLibLoadStatus = IMAGELIB_NOT_LOADED;  // Need to call ImageLib::Load()
         m_bBackgroundBuildThreadFailed = FALSE;
         m_hBackgroundBuildThread = INVALID_HANDLE_VALUE;

         ZeroMemory( m_pt, sizeof(m_pt) );

         m_bTileCacheServiceInitialized = FALSE;

         m_cbJPIPCallback.m_pNITFObj = this;
         m_cbJPIPCallback.AddRef();
         m_bIncompleteDraw = FALSE;

         m_old_ul_lat = m_old_ul_lon = m_old_lr_lat = m_old_lr_lon == 0.0;
         m_old_width = m_old_height = 0;

         m_cRefCount = 0;
      }

// **************************************************************************
// **************************************************************************

C_nitf_obj::~C_nitf_obj()
{
   m_cbImageLibCallback2.m_eCallbackMode =
      ImageLibCallbackInterface::CALLBACK_MODE_ABORTING;  // Make sure will try to quit
     
   // Wait if still building an overview file
   time_t tmStart;
   time( &tmStart );
   while ( m_hBackgroundBuildThread != INVALID_HANDLE_VALUE
      && time( NULL ) - tmStart < 120 )   // 2 minute timeout
   {
      DWORD dw = MsgWaitForMultipleObjects( 1, &m_hBackgroundBuildThread, FALSE, 60000, QS_ALLEVENTS );
      switch ( dw )
      {
         case WAIT_OBJECT_0:     // Thread ended
            CloseHandle( m_hBackgroundBuildThread );
            goto BackgroundBuildDone;
         
         case WAIT_OBJECT_0 + 1: // Message arrived
            {
               MSG msg;
               while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
               {
                  TranslateMessage( &msg );
                  DispatchMessage( &msg );
               }
            }
      }
   }
BackgroundBuildDone:

   m_pImageLib = NULL;     // Make sure any callback process is shut down

   delete m_freq_lum;
// clear_image_buffer();

}  // End of ~C_nitf_obj()

// **************************************************************************
// **************************************************************************

VOID C_nitf_obj::InitImageLib()
{
   if ( m_pImageLib == NULL )
   {
      CO_CREATE( m_pImageLib, __uuidof( ImageLib ) );
      m_cbImageLibCallback1.m_eCallbackMode = ImageLibCallbackInterface::CALLBACK_MODE_NORMAL;
      m_pImageLib->set_callback( &m_cbImageLibCallback1 );
      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_MULTISPECTRAL, VARIANT_TRUE,
         VARIANT_NULL, NULL, NULL, NULL, NULL );
#if 1 || defined ALL_TESTS     // Set to 0 to disable cache for debugging
      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_SET_SOURCE_CACHE_ROOT_FOLDER,
         _bstr_t( C_nitf_ovl::s_pNITFOvl->m_csFVWUserDataRootFolder ) + L"\\NITF",
         _variant_t( MAX_SOURCE_CACHE_BYTES ), NULL, NULL, NULL, NULL );
#endif
   }
   // Make sure that any display parameters are  up-to-date
   m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_LOAD_DISPLAY_PARAMS_XML,
      _variant_t( m_pxdDisplayParams->xml ), VARIANT_NULL, NULL, NULL, NULL, NULL );
}


// **************************************************************************
// **************************************************************************

CString C_nitf_obj::get_help_text()
{
   CString tstr;

   tstr = m_img_date;
   if (tstr.GetLength() < 6)
      tstr = "<unknown>";
   m_help_text = tstr + ", " + m_csFilespec;

   return m_help_text;
}
// end of get_help_text

// **************************************************************************
// **************************************************************************

CString C_nitf_obj::get_tool_tip()
{
   return m_bSelectHit ? m_filename : "";   // Tool tip only on icon or edge
}

// **************************************************************************
// **************************************************************************

BOOL C_nitf_obj::hit_test( CPoint ptTest )
{
   // Quick check 
   if ( m_rect.PtInRect( ptTest ) )
   {
      // Is it iconic?
      if ( m_rect.Width() < 20 )
         return m_bSelectHit = TRUE;  // Anywhere on icon is ok, direct hit

      if ( CFvwUtil::get_instance()->point_in_polygon( ptTest, m_pt, 4 ) )
      {
         // Selection hit if hit edge
         m_bSelectHit = CFvwUtil::get_instance()->point_on_polygon( ptTest, 6, m_pt, 4 );
         return TRUE;
      }
   }
   return FALSE;
}

#if 0

// **************************************************************************
// **************************************************************************

BOOL C_nitf_obj::hit_test_edge( CPoint ptTest )
{
   // Must be in rectangle 
   if ( !m_rect.PtInRect( ptTest ) )
      return FALSE;

   return CFvwUtil::get_instance()->point_on_polygon( ptTest, 6, m_pt, 4 );
}


// **************************************************************************
// **************************************************************************

boolean_t C_nitf_obj::hit_test( MapProj* pMap, CPoint ptTest )
{
   return hit_test( ptTest );
}
// End of C_nitf_obj::hit_test()
#endif

// **************************************************************************
// **************************************************************************

BOOL C_nitf_obj::IsBusy()
{
   if ( m_hBackgroundBuildThread != INVALID_HANDLE_VALUE )
   {
      if ( WaitForSingleObject( m_hBackgroundBuildThread, 0 ) != WAIT_OBJECT_0 )
         return TRUE;         // Still busy

      CloseHandle( m_hBackgroundBuildThread );
      m_hBackgroundBuildThread = INVALID_HANDLE_VALUE;
   }
   return FALSE;        // Idle
}


// **************************************************************************
// **************************************************************************

INT C_nitf_obj::StartBackgroundBuildThread()
{
   if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_TILE_CACHE )
   {
      CComBSTR ccbsOverviewName, ccbsErrorMsg;
      INT iErr;
      InitImageLib();
      m_pImageLib->get_overview_name( _bstr_t( m_csFilespec ), &ccbsOverviewName, &iErr, &ccbsErrorMsg );
      DeleteFileW( (LPCWSTR) ccbsOverviewName );         // Make sure there's no old (probably corrupt) file

      m_cbImageLibCallback2.m_eCallbackMode = ImageLibCallbackInterface::CALLBACK_MODE_BACKGROUND;
   }

   CWinThread* pThread = AfxBeginThread(
         (AFX_THREADPROC) BackgroundBuildProc1,
                                       // Application-defined function of type
                                       // UINT __cdecl fnc( LPVOID )
         static_cast<LPVOID>( this ),  // 32-bit parameter value passed to the thread
         THREAD_PRIORITY_BELOW_NORMAL, // In case single CPU
         0,                            // Default stack size 
         CREATE_SUSPENDED,             // Need to copy handle
         NULL );                       // Security
   if ( pThread != NULL )
   {
      DuplicateHandle(
         GetCurrentProcess(), pThread->m_hThread,
         GetCurrentProcess(), &m_hBackgroundBuildThread,
         0, FALSE, DUPLICATE_SAME_ACCESS );

#if ( defined THREAD_TRACE && defined _DEBUG) || defined ALL_TESTS
      TRACE( _T("BackgroundBuild thread started, h = 0x%08x, id = 0x%08x\n"),
         m_hBackgroundBuildThread, pThread->m_nThreadID );
#endif

      pThread->ResumeThread();
      return SUCCESS;
   }

   m_hBackgroundBuildThread = INVALID_HANDLE_VALUE;
   m_bBackgroundBuildThreadFailed = TRUE;
   return FAILURE;
}


// **************************************************************************
// **************************************************************************

UINT C_nitf_obj::BackgroundBuildProc1( LPVOID pvThis )
{
   C_nitf_obj* pThis = static_cast< C_nitf_obj* >( pvThis );

   UINT uiResult = pThis->BackgroundBuildProc2();
   if ( uiResult != SUCCESS )
      pThis->m_bBackgroundBuildThreadFailed = TRUE;
   
   OVL_get_overlay_manager()->invalidate_from_thread( FALSE );
   
   return uiResult;
}


// **************************************************************************
// **************************************************************************

UINT C_nitf_obj::BackgroundBuildProc2()
{
   // If waiting for tile cache server to prepare tile data
   if ( m_eImageLibLoadStatus == IMAGELIB_LOADED_TILE_CACHE )
   {
      DWORD dwDelay;
      // Try to convert param text to DWORD
      do
      {
         if ( m_bstrBackgroundBuildParam.length() > 0 )
         {
            if ( 1 == swscanf_s( (LPCWSTR) m_bstrBackgroundBuildParam, L"%ul", &dwDelay ) )
               break;
         }
         dwDelay = 10 * 1000;    // Failed, use default
      } while ( FALSE );
      Sleep( dwDelay );    // Delay
      return S_OK;         // Then retry
   }

   CoInitialize( NULL );

   IImageLibPtr pImageLib;
   INT iErr = -999;     // Force build
   CComBSTR ccbsErrorMsg;
   HRESULT hr;
   
#if ( defined THREAD_TRACE && defined _DEBUG ) || defined ALL_TESTS
   time_t tm;
   time( &tm );
   TRACE( _T("Start of BackgroundBuild for \"%s\" at %s"), m_csFilespec, ctime( &tm ) );
#endif

   do
   {
      hr = pImageLib.CreateInstance( __uuidof( ImageLib ) );
      if ( hr != S_OK )
         break;

      hr = pImageLib->set_callback( &m_cbImageLibCallback2 );
      if ( hr != S_OK )
         break;

      INT iWidth, iHeight;
      hr = pImageLib->load( _bstr_t( m_csFilespec ), &iWidth, &iHeight, &iErr, &ccbsErrorMsg );
      if ( hr != S_OK )
         break;

      hr = pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_OVERVIEW_BUILD, VARIANT_TRUE,
         VARIANT_NULL, NULL, NULL, NULL, NULL );
      if ( hr != S_OK )
         break;

      ccbsErrorMsg.Empty();
      hr = pImageLib->create_thumbnail( &iErr, &ccbsErrorMsg );
      if ( hr == S_FALSE && iErr == IMAGELIB_ERROR_USER_ABORT )
         hr = S_OK;        // Abandoned is not an error
   } while ( FALSE );

#if ( defined THREAD_TRACE && defined _DEBUG ) || defined ALL_TESTS
   time( &tm );
   TRACE( _T("End of BackgroundBuild for \"%s\" at %s"), m_csFilespec, ctime( &tm ) );
   TRACE(_T("  pImageLib = 0x%08x, iErr = %d, ErrMsg = \"%S\"\n"),
      pImageLib, iErr, (LPCWSTR) ccbsErrorMsg );
#endif

   CoUninitialize();
   return hr == S_OK ? SUCCESS : FAILURE;
}

// **************************************************************************
// **************************************************************************

void C_nitf_obj::SetRGBBuffers( PBYTE pbRGBData, INT cPixels )
{
   m_apbRGBBuffer = std::auto_ptr< BYTE >( new BYTE[ 3 * cPixels ] );
   m_red_buffer = m_apbRGBBuffer.get() + 0;
   m_grn_buffer = m_red_buffer + cPixels;
   m_blu_buffer = m_grn_buffer + cPixels;

   for ( INT k=0; k < cPixels; k++ )
   {
      m_red_buffer[k] = pbRGBData[k*3+0];
      m_grn_buffer[k] = pbRGBData[k*3+1];
      m_blu_buffer[k] = pbRGBData[k*3+2];
   }
}  // End of SetRGBBuffers()


// **************************************************************************
// **************************************************************************

void C_nitf_ovl::get_label(int recnum, CString & label)
{
   CString tstr, tstr2;

   label = "";
}
// end of get_label

// **************************************************************************
// **************************************************************************

boolean_t C_nitf_obj::is_kind_of(const char *class_name) 
{
   if (!strcmp(class_name, "C_nitf_obj"))
      return TRUE;

   return C_icon::is_kind_of(class_name);
}

// **************************************************************************
// **************************************************************************

void C_nitf_obj::draw_nitf_icon( ActiveMap* pMap, CDC* pDC, int x, int y, boolean_t hilighted)
{
   if ( m_dwBeginFlashTicks != 0 )
   {
      if ( m_dwBeginFlashTicks == ULONG_MAX )
      {
         C_nitf_ovl::s_pNITFOvl->m_clsFlashTimer.reset();
         C_nitf_ovl::s_pNITFOvl->m_clsFlashTimer.start();
         m_dwBeginFlashTicks = GetTickCount();  // When first flashed
      }

      if ( GetTickCount() > m_dwBeginFlashTicks + SELECT_FLASH_TICKS - ( 2 * TICK_PERIOD ) )
         m_dwBeginFlashTicks = 0;      // End of flash
      else
      {
         INT cWidth, cHeight, cDimMax, iXOffset, iYOffset;
         pMap->get_surface_size( &cWidth, &cHeight );
         iXOffset = x - ( cWidth / 2 ); iYOffset = y - ( cHeight / 2 );

         // Only display arrow if icon is more than 10% of the screen size away from the center
         cDimMax = cWidth > cHeight ? cWidth : cHeight;
         if ( 100 * ( ( iXOffset * iXOffset ) + ( iYOffset * iYOffset ) )
               > cDimMax * cDimMax )
            DrawLocatorArrow( pMap, cWidth, cHeight,
               atan2( (DOUBLE) iYOffset, (DOUBLE) iXOffset ), UTIL_COLOR_GREEN );
      }
   }  // m_dwBeginFlashTicks > 0

   if ( ( m_mTempDisplayMap & C_nitf_ovl::s_pNITFOvl->m_mTempDisplayMask ) == 0
      && C_nitf_ovl::s_bEditOn )
      return;     // Only here because of flash

   CIconImage* draw_image;
   int icon_adjusted_size = -1;
   boolean_t is_printing = pMap->get_CDC()->IsPrinting();

   if (is_printing)
   {
      double adjust_percentage = (double)PRM_get_registry_int("Printing", 
         "IconAdjustSizePercentage", 0);
      icon_adjusted_size = (int)(32.0 + 32.0*adjust_percentage/100.0);
   }

   if (m_is_color)
   {
      switch( m_eFileType )
      {
      case NITF_JPIP_FILE:          // Temp
      case NITF_JP2_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_j2k_icon;
         break;
      case NITF_NITF_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_nitf_icon;
         break;
      case NITF_TIFF_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_tiff_icon;
         break;
      case NITF_MRSID_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_sid_icon;
         break;
      case NITF_MISC_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_misc_icon;
         break;
      default:
         ASSERT( FALSE && "No icon for this file type" );
         return;
      }
   }
   else
   {
      switch( m_eFileType )
      {
      case NITF_JPIP_FILE:          // Temp
      case NITF_JP2_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_j2k_m_icon;
         break;
      case NITF_NITF_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_nitf_m_icon;
         break;
      case NITF_TIFF_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_tiff_m_icon;
         break;
      case NITF_MRSID_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_sid_m_icon;
         break;
      case NITF_PDF_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_pdf_m_icon;
         break;
      case NITF_MISC_FILE:
         draw_image = C_nitf_ovl::s_pNITFOvl->m_misc_m_icon;
         break;
      default:
         ASSERT( FALSE && "No icon for this file type" );
         return;
      }
   }

   if (hilighted)
      draw_image->draw_hilighted( pDC, x, y, 32);
   else
   {
      draw_image->draw_icon( pDC, x, y, 32, icon_adjusted_size );
      if ( 0 != ( m_mTempDisplayMap & NITF_TEMP_DISPLAY_SELECTED_MASK ) )
      {
         // Since NITF icons are only ~20x20, reduce the size a bit (by 6)
         INT iOffset = ( ( icon_adjusted_size > 0 ? icon_adjusted_size : 32 ) - 6
            + C_nitf_ovl::s_pNITFOvl->m_iSelectedBkgdFrameWidth ) / 2;
         POINT pts[5] =
         {
            { x - iOffset, y - iOffset },
            { x + iOffset, y - iOffset },
            { x + iOffset, y + iOffset },
            { x - iOffset, y + iOffset },
            { x - iOffset, y - iOffset }
         };
         CFvwUtil::get_instance()->polyline( pDC, pts, 5, UTIL_COLOR_GREEN,
            C_nitf_ovl::s_pNITFOvl->m_iSelectedBkgdFrameWidth );
      }
   }
}
// end of draw_nitf_icon

#if 0
// **************************************************************************
// **************************************************************************

void C_nitf_obj::draw_missing_icon(CDC *dc, int x, int y)
{
   CIconImage* draw_image;
   int icon_adjusted_size = -1;
   boolean_t is_printing = dc->IsPrinting();

   C_nitf_ovl *ovl = (C_nitf_ovl*) m_overlay;

   if (is_printing)
   {
      double adjust_percentage = (double)PRM_get_registry_int("Printing", 
         "IconAdjustSizePercentage", 0);
      icon_adjusted_size = (int)(32.0 + 32.0*adjust_percentage/100.0);
   }

   draw_image = ovl->m_missing_icon;

   draw_image->draw_icon(dc, x, y, 32, icon_adjusted_size);
}
// end of draw_missing_icon

// **************************************************************************
// ********************************************************************

BOOL C_nitf_obj::image_is_dirty()
{
   int k;
   BOOL dirty = FALSE;

   for (k=0; k<4; k++)
   {
      if ((m_pt[k].x != m_old_pt[k].x) || (m_pt[k].y != m_old_pt[k].y))
         dirty = TRUE;
      m_old_pt[k].x = m_pt[k].x;
      m_old_pt[k].y = m_pt[k].y;
   }

   return dirty;
}
#endif

// **************************************************************************
// **************************************************************************

BOOL C_nitf_obj::show_fill()
{
   // right now return FALSE bacause currently fill is more annoying than useful
   return FALSE;

   BOOL enclose;

   if ( m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
      return FALSE;

   enclose = GEO_enclose_degrees( m_geo_rect_ll, m_geo_rect_ur,
      C_nitf_ovl::s_pNITFOvl->m_scrn_ll, C_nitf_ovl::s_pNITFOvl->m_scrn_ur );

   return enclose;
}

// **************************************************************************
// **************************************************************************


void C_nitf_obj::draw_bounds( ActiveMap* pMap )
{
   int style;
   COLORREF crFrgdColor, crBkgdColor;
   CFvwUtil* pFUtil = CFvwUtil::get_instance();
   C_nitf_ovl* pOvl = C_nitf_ovl::s_pNITFOvl;

   CDC* dc = pMap->get_CDC();

   const ImageFrameLineStyle* pifls = ImageFrameLineStyles;
   while ( m_pixel_size_in_meters < pifls->fMinPixelSize )
      pifls++;
   style = pifls->iLineStyle;

   if ( IsBusy() )            // Probably building an overview file
      crFrgdColor = pOvl->m_crFrameBusyFrgdColor;        // Default = bright yellow
   else if ( m_is_color )
      crFrgdColor = pOvl->m_crFrameColorImageFrgdColor;  // Default = red
   else
      crFrgdColor = pOvl->m_crFrameMonoImageFrgdColor;   // Default = off white

   INT iBkgdWidth = pOvl->m_iUnselectedBkgdFrameWidth;
   if ( 0 != ( m_mTempDisplayMap & NITF_TEMP_DISPLAY_SELECTED_MASK ) || m_bSelected )
   {
      crBkgdColor = pOvl->m_crFrameSelectedBkgdColor;
      iBkgdWidth = pOvl->m_iSelectedBkgdFrameWidth;
   }
   else if ( 0 != ( m_mTempDisplayMap & NITF_TEMP_DISPLAY_TEMP_MASK ) )
      crBkgdColor = pOvl->m_crFrameTempBkgdColor;
   else if ( 0 != ( m_mTempDisplayMap & NITF_TEMP_DISPLAY_KEEP_TEMP_MASK ) )
      crBkgdColor = pOvl->m_crFrameTempKeepBkgdColor;
   else
      crBkgdColor = pOvl->m_crFrameStdBkgdColor;

   INT cWidth, cHeight;
   pMap->get_surface_size( &cWidth, &cHeight );

   if ( m_dwBeginFlashTicks != 0 )
   {
      if ( m_dwBeginFlashTicks == ULONG_MAX )
      {
         pOvl->m_clsFlashTimer.reset();
         pOvl->m_clsFlashTimer.start();
         m_dwBeginFlashTicks = GetTickCount();  // When first flashed
      }

      // Check whether any part of the bounds is visible.  Use geo coordinates to make sure
      // there are no problems if the image is very far away
      DOUBLE dImagePoly[4][2] =  // XY pairs
      {
         { m_bnd_ul.lon, m_bnd_ul.lat },  // These bounds have been normalized in the database
         { m_bnd_ur.lon, m_bnd_ur.lat },
         { m_bnd_lr.lon, m_bnd_lr.lat },
         { m_bnd_ll.lon, m_bnd_ll.lat }
      };
 
      DOUBLE
         dViewCorners[2][4],
         dLimits[2][2];
      for ( INT k = 0; k < 4; k++ )
      {
         dViewCorners[ 0 ][ k ] = pOvl->m_gptViewCorners[ k ].lat;
         dViewCorners[ 1 ][ k ] = pOvl->m_gptViewCorners[ k ].lon;
      }
      // Normalize to remove any dateline effects
      NITFNormalizeGeoBounds( dViewCorners, dLimits );

      for ( INT iLonOffset = -360; iLonOffset <= +360; iLonOffset += 360 )
      {
         DOUBLE dViewPoly[ 4 ][ 2 ]; // XY pairs
         for ( INT k = 0; k < 4; k++ )
         {
            dViewPoly[ k ][ 0 ] = dViewCorners[ 1 ][ k ] + iLonOffset;   // Lon -> x
            dViewPoly[ k ][ 1 ] = dViewCorners[ 0 ][ k ];   // Lat ->y
         }

         // If image bounds intersect view, fill the bounds polygon
         if ( GeometryUtils::PolygonIntersection( 4, dImagePoly, 4, dViewPoly ) )
         {
            if ( GetTickCount() > m_dwBeginFlashTicks + SELECT_FLASH_TICKS - ( 2 * TICK_PERIOD ) )
               m_dwBeginFlashTicks = 0;      // End of the flash
            else
            {
               pFUtil->fill_polygon( dc, m_pt, 4, UTIL_COLOR_GREEN, UTIL_FILL_FDIAG,
                                     cWidth, cHeight );

               // If image doesn't overlap the center 10%, draw an arrow
               for ( INT k = 0; k < 4; k++ )
               {
                  dImagePoly[ k ][ 0 ] = m_pt[ k ].x;
                  dImagePoly[ k ][ 1 ] = -m_pt[ k ].y;   // Y is increasing down
               }
               dViewPoly[ 0 ][ 0 ] = dViewPoly[ 3 ][ 0 ] = 0.05 * ( 11 + ( 9 * cWidth ) );
               dViewPoly[ 1 ][ 0 ] = dViewPoly[ 2 ][ 0 ] = 0.05 * ( 9 + ( 11 * cWidth ) );
               dViewPoly[ 0 ][ 1 ] = dViewPoly[ 1 ][ 1 ] = -0.05 * ( 11 + ( 9 * cHeight ) );
               dViewPoly[ 2 ][ 1 ] = dViewPoly[ 3 ][ 1 ] = -0.05 * ( 9 + ( 11 * cHeight ) );
               if ( !GeometryUtils::PolygonIntersection( 4, dImagePoly, 4, dViewPoly ) )
               {
                  // Find the angle from the view center to the image center
                  DOUBLE
                     dAngle = atan2( -dImagePoly[ 0 ][ 1 ] - dImagePoly[ 2 ][ 1 ] - cHeight,
                        dImagePoly[ 0 ][ 0 ] + dImagePoly[ 2 ][ 0 ] - cWidth );
                  DrawLocatorArrow( pMap, cWidth, cHeight, dAngle, UTIL_COLOR_GREEN );
               }
            }
            goto DrawIt;      // Now draw the bounds
         }
      }
      // No intersection.  Flash an arrow
      if ( GetTickCount() > m_dwBeginFlashTicks + SELECT_FLASH_TICKS - ( 2 * TICK_PERIOD ) )
         m_dwBeginFlashTicks = 0;
      else
      {
         // Draw an arrow to the offscreen image.  This is done in geo coordinates and may
         // not line up exactly if the projection is not equal arc
         DOUBLE dDLonX2 = m_bnd_ul.lon + m_bnd_lr.lon - dViewCorners[ 1 ][ 0 ] - dViewCorners[ 1 ][ 2 ];
         if ( dDLonX2 > +2 * 180.0 )   // Remove any dateline wrap
            dDLonX2 -= 2 * 360.0;
         if ( dDLonX2 < -2 * 180.0 )
            dDLonX2 += 2 * 360.0;
         DOUBLE dAngle =
            atan2(      // North-up angle from view center to image center
               dViewCorners[ 0 ][ 0 ] + dViewCorners[ 0 ][ 2 ] - m_bnd_ul.lat - m_bnd_lr.lat,
               dDLonX2 )
            + atan2(    // North-up angle of view top edge
               dViewCorners[ 0 ][ 1 ] - dViewCorners[ 0 ][ 0 ],
               dViewCorners[ 1 ][ 1 ] - dViewCorners[ 1 ][ 0 ] );
         DrawLocatorArrow( pMap, cWidth, cHeight, dAngle, UTIL_COLOR_RED );
      }
      return;
   }
DrawIt:;

   if ( m_bIncompleteDraw )      // Progressive rendering
   {
      if ( m_cViewIntersectionVertices >= 3 )
      {
         UtilDraw util( dc );
         util.set_pen( pOvl->m_crFrameBusyFrgdColor, UTIL_LINE_SOLID, iBkgdWidth, TRUE );
         util.draw_polygon( UTIL_FILL_NONE, m_apptViewIntersectionVertices.get(),
                                    m_cViewIntersectionVertices );
         util.set_pen( crBkgdColor, UTIL_LINE_SOLID, 2, FALSE );
         util.draw_polygon( UTIL_FILL_NONE, m_apptViewIntersectionVertices.get(),
                                    m_cViewIntersectionVertices );
      }
      return;
   }

   // Draw the background, 4 pix wide
   DOUBLE
      dULLon = NITFUnwrapLon( m_bnd_ul.lon ),
      dURLon = NITFUnwrapLon( m_bnd_ur.lon ),
      dLRLon = NITFUnwrapLon( m_bnd_lr.lon ),
      dLLLon = NITFUnwrapLon( m_bnd_ll.lon );

   const struct
   {
      INT      iWidth;
      INT      iStyle;
      COLORREF crColor;
      BOOL     bBorderPart;
   } LineDrawInfo[] =
   {
      { iBkgdWidth,     UTIL_LINE_SOLID,        crBkgdColor,   FALSE }, // Background broad line
      { 2,              UTIL_LINE_BORDER_END,   crFrgdColor,   TRUE },  // Inside marker
      { 2,              UTIL_LINE_SOLID,        crBkgdColor,   TRUE },  // Pattern prep
      { 2,              style,                  crFrgdColor,   FALSE }, // Foreground thin line
      { -1 }
   }, *pldi = LineDrawInfo;

   do
   {
      // Don't draw boundary marks for very small outline or filled image
      if ( ( 6 * m_rect.Width() < cWidth
            || 6 * m_rect.Height() < cHeight
            || m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
         && pldi->bBorderPart )
         continue;      // Don't draw boundary marks for very small outlines or filled images

      // Draw the line component
      pFUtil->draw_geo_line( pMap, dc, m_bnd_ul.lat, dULLon, m_bnd_ur.lat, dURLon,
         pldi->crColor, pldi->iStyle, pldi->iWidth, BOUNDS_LINE_TYPE, TRUE, FALSE );
      pFUtil->draw_geo_line( pMap, dc, m_bnd_ur.lat, dURLon, m_bnd_lr.lat, dLRLon,
         pldi->crColor, pldi->iStyle, pldi->iWidth, BOUNDS_LINE_TYPE, TRUE, FALSE );
      pFUtil->draw_geo_line( pMap, dc, m_bnd_lr.lat, dLRLon, m_bnd_ll.lat, dLLLon,
         pldi->crColor, pldi->iStyle, pldi->iWidth, BOUNDS_LINE_TYPE, TRUE, FALSE );
      pFUtil->draw_geo_line( pMap, dc, m_bnd_ll.lat, dLLLon, m_bnd_ul.lat, dULLon,
         pldi->crColor, pldi->iStyle, pldi->iWidth, BOUNDS_LINE_TYPE, TRUE, FALSE );
   } while ( (++pldi)->iWidth >= 0 );

}
// End of draw_bounds()


// **************************************************************************
// **************************************************************************

int C_nitf_obj::draw( ActiveMap* pMap)
{
   int rslt, k;
   double dpp_lat, dpp_lon;
   CFvwUtil *futil = CFvwUtil::get_instance();
   C_nitf_ovl* pOvl = C_nitf_ovl::s_pNITFOvl;
   m_pActiveMap = pMap;
   double rot;
   BOOL is_visible = FALSE;   // Until proven otherwise

   // Clean up if were building an overview file
   if ( m_hBackgroundBuildThread != INVALID_HANDLE_VALUE )
   {
      // Check whether thread has terminated
      if ( WaitForSingleObject( m_hBackgroundBuildThread, 0 ) == WAIT_OBJECT_0 )
      {
#if ( defined THREAD_TRACE && defined _DEBUG ) || defined ALL_TESTS
         ATLTRACE( _T("C_nitf_obj::draw() - cleanup for thread h = 0x%08x, current thread id = 0x%08x\n"),
            m_hBackgroundBuildThread, GetCurrentThreadId() );
#endif
         CloseHandle( m_hBackgroundBuildThread );
         m_hBackgroundBuildThread = INVALID_HANDLE_VALUE;
      }
   }

   rslt = SUCCESS;

   pMap->get_vmap_degrees_per_pixel(&dpp_lat, &dpp_lon);

   rot = pMap->actual_rotation();
   m_map_is_rotated = rot != 0.0;

   CDC* pDC = pMap->get_CDC();

   if (m_has_geo)
   {
      int minx, maxx, miny, maxy;

#if 1 // Handles images that go out of bounds better
     if ( m_geo_rect_ll.lat <= pOvl->m_scrn_ur.lat
         && m_geo_rect_ur.lat >= pOvl->m_scrn_ll.lat )
     {
         DOUBLE dWest = pOvl->m_scrn_ll.lon;
         if ( dWest > pOvl->m_scrn_ur.lon )
         dWest -= 360.0;

          for ( DOUBLE dLonOffset = -360.0; dLonOffset <= +360.0; dLonOffset += 360.0 )
         {
            if ( m_geo_rect_ll.lon + dLonOffset <= pOvl->m_scrn_ur.lon
               && m_geo_rect_ur.lon + dLonOffset >= dWest )
            {
               is_visible = TRUE;
               break;
            }
         }
      }
#else
      is_visible = GEO_intersect_degrees(pOvl->m_scrn_ll, pOvl->m_scrn_ur, m_geo_rect_ll, m_geo_rect_ur) ||
         GEO_enclose_degrees  (pOvl->m_scrn_ll, pOvl->m_scrn_ur, m_geo_rect_ll, m_geo_rect_ur);
#endif
      if (!is_visible)
      {
         if ( m_dwBeginFlashTicks != 0 )
            draw_bounds( pMap );     // If flashing, draw anyway
         return SUCCESS;
      }

      DOUBLE dXs[ 4 ], dYs[ 4 ];
      for ( k = 0; k < 4; k++ )
         pMap->geo_to_surface( m_bnds[ k ].lat, m_bnds[ k ].lon, &dXs[ k ], &dYs[ k ] );

      INT iWorldPix;
      pMap->get_pixels_around_world( &iWorldPix );
      static const DOUBLE
         DEG2RAD = M_PI / 180.0,
         RAD2DEG = 180.0 / M_PI;
      DOUBLE
         dWorldXPix = iWorldPix * cos( DEG2RAD * rot ),
         dWorldYPix = iWorldPix * sin( DEG2RAD * rot );

      // Up to 3 tries to adjust improper corners
      for ( k = 1; k <= 3; k++ )
      {
         // Check by looking for a minus cross product at any vertex.
         // Q = (x(n-1)-x(n))*(y(n+1)-y(n)) - (y(n-1)-y(n))*(x(n+1)-x(n))
         INT i;
         for ( i = 0; i < 4; i++ )
         {
            INT ip1 = ( i + 1 ) % 4, im1 = ( i + 3 ) % 4;
            if (
                  ( ( dYs[ im1 ] - dYs[ i ] )
                     * ( dXs[ ip1 ] - dXs[ i ] ) )
                  - ( ( dXs[ im1 ] - dXs[ i ] )
                     * ( dYs[ ip1 ] - dYs[ i ] ) )
                  < 0.0 )
               goto FixIt;
         }
         break;      // All cross products are positive
      
         // Add the "pix around the world" to the most western vertex and try again
FixIt:
         DOUBLE dDotMin = +DBL_MAX;
         INT iMin = 0;
         for ( i = 0; i < 4; i++ )
         {
            // Dot product of around-the-world-to-the_east vector
            // with the vector to the current vertex
            DOUBLE dDot = ( dXs[ i ] * dWorldXPix ) + ( dYs[ i ] * dWorldYPix );
            if ( dDot < dDotMin )
            {
               dDotMin = dDot;
               iMin = i;
            }
         }
         dXs[ iMin ] += dWorldXPix;
         dYs[ iMin ] += dWorldYPix;
            
#if 0 && defined _DEBUG
         ATLTRACE( _T("Minus cross product, pass %d\n")
            _T("  UL = %.3f, %.3f\n")
            _T("  UR = %.3f, %.3f\n")
            _T("  LR = %.3f, %.3f\n")
            _T("  LL = %.3f, %.3f\n"),
            k,
            dXs[0], dYs[0],
            dXs[1], dYs[1],
            dXs[2], dYs[2],
            dXs[3], dYs[3] );
#endif
            
      } // Try to fudge up to three vertices

      // Make sure we have the right world wrap
      DOUBLE
         dXMin = dXs[ 0 ],
         dXMax = dXs[ 0 ],
         dYMin = dYs[ 0 ],
         dYMax = dYs[ 0 ];
      for ( k = 1; k < 4; k++ )
      {
         if ( dXs[ k ] > dXMax )
            dXMax = dXs[ k ];
         if ( dXs[ k ] < dXMin )
            dXMin = dXs[ k ];
         if ( dYs[ k ] > dYMax )
            dYMax = dYs[ k ];
         if ( dYs[ k ] < dYMin )
            dYMin = dYs[ k ];
      }
      for ( INT i = -1; i <= +1; i += 2 )
      {
         DOUBLE
            dXOffset = i * dWorldXPix,
            dYOffset = i * dWorldYPix;
         if ( dXMin + dXOffset <= pMap->vwidth()
            && dXMax + dXOffset >= 0.0
            && dYMin + dYOffset <= pMap->vheight()
            && dYMax + dYOffset >= 0.0 )
         {
            for ( k = 0; k < 4; k++ )
            {
               dXs[ k ] += dXOffset;
               dYs[ k ] += dYOffset;
            }
            break;
         }
      }

     for ( k = 0; k < 4; k++ )
     {
        m_pt[ k ].x = (LONG) ( dXs[ k ] + 0.5 );
        m_pt[ k ].y = (LONG) ( dYs[ k ] + 0.5 );
     }

     // set the rectangle
     minx = maxx = m_pt[0].x;
     miny = maxy = m_pt[0].y;
     for (k=1; k<4; k++)
     {
        if (minx > m_pt[k].x)
           minx = m_pt[k].x;
        if (maxx < m_pt[k].x)
           maxx = m_pt[k].x;
        if (miny > m_pt[k].y)
           miny = m_pt[k].y;
        if (maxy < m_pt[k].y)
           maxy = m_pt[k].y;
     }

     m_rect.SetRect(minx, miny, maxx, maxy);

     if ((m_rect.Width() < 20) && (m_rect.Height() < 20))
     {
        int tx, ty;
        tx = (minx + maxx) / 2;
        ty = (miny + maxy) / 2;
        m_rect.SetRect(tx-9, ty-9, tx+9, ty+9);
        draw_nitf_icon( pMap, pDC, tx, ty, m_bSelected );
     }
     else
     {
#if defined TIEPOINT_TEST || defined ALL_TESTS   // Drawing xy bounds
#if 0
         DOUBLE dLat1, dLon1, dLat2, dLon2;
         pMap->surface_to_geo( minx, miny, &dLat1, &dLon1 );
         pMap->surface_to_geo( maxx, miny, &dLat2, &dLon2 );
         futil->draw_geo_line( pMap, pDC,
            dLat1, dLon1, dLat2, dLon2, UTIL_COLOR_WHITE,   // white
            PS_SOLID, 1, UTIL_LINE_TYPE_RHUMB, TRUE, FALSE );
         pMap->surface_to_geo( maxx, maxy, &dLat1, &dLon1 );
         futil->draw_geo_line( pMap, pDC,
            dLat1, dLon1, dLat2, dLon2, UTIL_COLOR_WHITE,   // white
            PS_SOLID, 1, UTIL_LINE_TYPE_RHUMB, TRUE, FALSE );
         pMap->surface_to_geo( minx, maxy, &dLat2, &dLon2 );
         futil->draw_geo_line( pMap, pDC,
            dLat1, dLon1, dLat2, dLon2, UTIL_COLOR_WHITE,   // white
            PS_SOLID, 1, UTIL_LINE_TYPE_RHUMB, TRUE, FALSE );
         pMap->surface_to_geo( minx, miny, &dLat1, &dLon1 );
         futil->draw_geo_line( pMap, pDC,
            dLat1, dLon1, dLat2, dLon2, UTIL_COLOR_WHITE,   // white
            PS_SOLID, 1, UTIL_LINE_TYPE_RHUMB, TRUE, FALSE );
#else
         POINT ptsr[5] = {
            { m_rect.left, m_rect.top },
            { m_rect.right, m_rect.top },
            { m_rect.right, m_rect.bottom },
            { m_rect.left, m_rect.bottom},
            { m_rect.left, m_rect.top } };
            futil->polyline( pDC, ptsr, 5, UTIL_COLOR_DARK_YELLOW, 1 );
#endif
#endif

            // draw the rectangle
            do
            {
            if ( m_eFillControl != FILL_CONTROL_OUTLINE_FRAME
               && ( !MapView::m_scrolling || !pOvl->m_bHideDuringScroll ) )
            {
               rslt = draw_map_image( pMap );
               if ( m_dwBeginFlashTicks == 0    // Not flashing the frame
                  && !m_bIncompleteDraw         // Not incomplete rendering
                  && m_eFillControl != FILL_CONTROL_FILLED_WITH_FRAME   // Not explicit frame on
                  && ( m_eFillControl != FILL_CONTROL_FILLED_APP_DFLT_FRAME   // Not overlay default
                     || !pOvl->m_bShowBounds ) )   // and overlay frame on
                  break;      // No frame
            }
            draw_bounds( pMap );
         } while ( FALSE );
      }
   }

   return rslt;
}
// end of draw

// **************************************************************************
// **************************************************************************

int C_nitf_obj::draw_label(ActiveMap* map)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int tx, ty;
   POINT cpt[4];

   tx = m_rect.left;
   ty = m_rect.bottom + 3;

   CDC* dc = map->get_CDC();

   int font_size = 12;
   if (dc->IsPrinting())
   {
      double adjust_percentage = (double)PRM_get_registry_int("Printing", 
         "FontAdjustSizePercentage", 0);
      font_size = (int)(12.0 + 12.0*adjust_percentage/100.0);
   }

   futil->draw_text(dc, m_filename, tx, ty, 
      UTIL_ANCHOR_UPPER_LEFT, "Arial", font_size, 
      UTIL_FONT_BOLD, 
      UTIL_BG_RECT, UTIL_COLOR_BLACK,
      UTIL_COLOR_SKY_BLUE, 0, cpt);
   return SUCCESS;
}

// ********************************************************************
// ********************************************************************

void C_nitf_obj::invalidate(BOOL erase)
{
   CRect rc;

   rc = m_rect;
   rc.InflateRect(3, 3);
   OVL_get_overlay_manager()->invalidate_rect(rc, erase);

}



// ****************************************************************
// ****************************************************************
VOID C_nitf_obj::set_display_params()
{
   C_nitf_ovl::s_pNITFOvl->
      set_file_display_params( m_csFilespec, m_iImageInFile,
         m_eq_mode, m_trans_value, m_contr_minval, m_contr_ctrval, m_contr_maxval,
         m_pxdDisplayParams->xml );
}


// ****************************************************************
// ****************************************************************
void C_nitf_obj::set_display_mode()
{
   C_nitf_ovl::s_pNITFOvl->
      set_file_fill_control( m_csFilespec, m_iImageInFile, m_eFillControl );
}


// ****************************************************************
// ****************************************************************

int C_nitf_obj::get_histogram(CString & error_msg)  // get luminance histogram i.e. m_freq_lum
{
   CFid fid;
   int iRslt;
   CNitfUtil util;

   // Alternate fid file location
   CString csAltFolder = C_nitf_ovl::s_pNITFOvl->m_csFVWUserDataRootFolder
      + "\\GeoRect\\fid\\";

   string str( "" );
   NITFAppendDecoratedFileName( m_csFilespec, str );
   CString csAltFilespec = csAltFolder + str.c_str();

   // Try the fid file in the same directory as the image file
   iRslt = fid.open( m_csFilespec );
   if ( iRslt != SUCCESS )
   {
      // Try a local copy of the fid file
      iRslt = fid.open( csAltFilespec );
   }

   // Try to get the histogram from the .fid file
   do
   {
#if !defined NO_HISTOGRAM_CACHE || defined ALL_TESTS   // Disable to force new histogram calculation
      if ( iRslt == SUCCESS )
      {
         if ( m_freq_lum == NULL )
            m_freq_lum = new UINT[ 256 ];

         iRslt = fid.get_histogram( m_freq_lum );
         if ( iRslt == SUCCESS )
         {
            // Got the histogram.  See if there are scale and offset values
            CString csScale, csOffset;
            if ( SUCCESS == fid.get_key( FID_DATA_SCALE, csScale )
               && SUCCESS == fid.get_key( FID_DATA_OFFSET, csOffset ) )
            {
               // Make sure that there is a place to put the image scaling parameters
               XMLNodePtr pxnRoot = m_pxdDisplayParams->documentElement;
               if ( pxnRoot == NULL )
               {
                  m_pxdDisplayParams->loadXML(
                     L"<?xml version=\"1.0\"?>\n"
                     L"<Params/>\n" );
                  pxnRoot = m_pxdDisplayParams->documentElement;
               }
               XMLNodePtr pxnSO = pxnRoot->selectSingleNode( L"ScaleAndOffset" );
               if ( pxnSO == NULL )
                  pxnSO = pxnRoot->appendChild( XMLNodePtr( m_pxdDisplayParams->createElement( L"ScaleAndOffset" ) ) );

               XMLNodePtr pxn = pxnSO->selectSingleNode( L"Scale" );
               if ( pxn == NULL )
                  pxn = pxnSO->appendChild( XMLNodePtr( m_pxdDisplayParams->createElement( L"Scale" ) ) );
               pxn->text = _bstr_t( csScale );

               pxn = pxnSO->selectSingleNode( L"Offset" );
               if ( pxn == NULL )
                  pxn = pxnSO->appendChild( XMLNodePtr( m_pxdDisplayParams->createElement( L"Offset" ) ) );
               pxn->text = _bstr_t( csOffset );

               // Pass to ImageLib for image data scaling
               m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_LOAD_DISPLAY_PARAMS_XML,
                  m_pxdDisplayParams->xml, VARIANT_NULL, NULL, NULL, NULL, NULL );

            }
            break;
         }

         delete m_freq_lum;
         m_freq_lum = NULL;
      }
#endif

      // Get the histogram from the image file (a side effect is filling m_freq_lum)
      if ( SUCCESS != ( iRslt = get_imagelib_histogram( error_msg ) )
            || m_freq_lum == NULL )
            return FAILURE;

      fid.set_histogram( m_freq_lum );

      // Get the scale and offset (if any) from the histogram result
      _variant_t varXML;
      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_LOAD_DISPLAY_PARAMS_XML,
                  VARIANT_NULL, VARIANT_NULL, &varXML, NULL, NULL, NULL );
      do
      {
         if ( varXML.vt != VT_BSTR || varXML.bstrVal == NULL )
            break;

         _bstr_t bstr( varXML.bstrVal );
         if ( bstr.length() == 0 )
            break;

         m_pxdDisplayParams->loadXML( bstr );
         XMLNodePtr pxnRoot = m_pxdDisplayParams->documentElement;
         if ( pxnRoot == NULL )
            break;

         XMLNodePtr
            pxnS = pxnRoot->selectSingleNode( L"ScaleAndOffset/Scale" ),
            pxnO = pxnRoot->selectSingleNode( L"ScaleAndOffset/Offset" );
         if ( pxnS == NULL || pxnO == NULL )
            break;
                  
         fid.set_key( FID_DATA_SCALE, (LPCTSTR) _bstr_t( pxnS->text ) );
         fid.set_key( FID_DATA_OFFSET, (LPCTSTR) _bstr_t( pxnO->text ) );

      } while ( FALSE );

      // Try to save in the image file's folder
      iRslt = fid.save_as( m_csFilespec.Left( m_csFilespec.ReverseFind( '.' ) + 1 ) + "fid" );
      if ( iRslt != SUCCESS )
      {
         // Save in the system fid folder
         CreateAllDirectories( CT2W( csAltFolder ) );
         iRslt = csAltFilespec.Right( 5 ).Find( _T('.') );
         if ( iRslt < 0 )
            csAltFilespec += _T(".fid");
         else
            csAltFilespec = csAltFilespec.Left( csAltFilespec.ReverseFind( _T('.') ) + 1 ) + _T("fid");
         iRslt = fid.save_as( csAltFilespec );
      }
   } while ( FALSE );

   return iRslt;
}  // get_histogram()


// ****************************************************************
// ****************************************************************

// get the current image histogram from imagelib and compute the it's luminance histogram

int C_nitf_obj::get_imagelib_histogram(CString & error_msg)
{
   _variant_t var_hist, var_freq_red, var_freq_grn, var_freq_blu;
   PUINT freq_red, freq_grn, freq_blu;

   int err_code, k;
   CComBSTR berr_msg;

   try
   {
      InitImageLib();   // Make sure initialized
      m_pImageLib->get_histogram(&var_hist, &var_freq_red, &var_freq_grn, &var_freq_blu, &err_code, &berr_msg);
      if (err_code != 0)
      {
         // try reloading it
         load_image_file();
         m_pImageLib->get_histogram(&var_hist, &var_freq_red, &var_freq_grn, &var_freq_blu, &err_code, &berr_msg);
         if (err_code != 0)
         {
            AfxBSTR2CString(&error_msg, berr_msg);
            return FAILURE;
         }
      }
   }
   catch(COleException *e)
   {
      e->Delete();
      error_msg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   catch(CMemoryException *e)
   {
      e->Delete();
      error_msg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   catch(...)
   {
      error_msg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   freq_red = (PUINT) var_freq_red.parray->pvData;
   freq_grn = (PUINT) var_freq_grn.parray->pvData;
   freq_blu = (PUINT) var_freq_blu.parray->pvData;

   if (m_freq_lum == NULL)
      m_freq_lum = (unsigned int*) malloc(256 * sizeof(unsigned int));

   for (k=0; k<256; k++)
   {
      if ((freq_red[k] == freq_grn[k]) && (freq_blu[k] == freq_grn[k]))
         m_freq_lum[k] = freq_red[k];
      else
         m_freq_lum[k] = (int) (((double) freq_red[k] * 0.3) +
         ((double) freq_grn[k] * 0.59) +
         ((double) freq_blu[k] * 0.11));
   }
   return SUCCESS;
}


// ******************************************************************************
// ******************************************************************************

BOOL C_nitf_obj::get_nitf_info( CString& csInfo )
{
   CNitfUtil util;
   CString sdata;
   csInfo = m_info;     // Existing info or blank in case of error
   if ( m_info.GetLength() > 0 )
      return TRUE;

   CComBSTR ccbsErrorMsg;
   INT iErr;

   do
   {
      if ( m_eFileType != NITF_JPIP_FILE
         && m_eFileType != NITF_NITF_FILE )
      {

         // If file not loaded, special handling
         if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_FILE )
         {
            ImageLibProxyLoad( &iErr, &ccbsErrorMsg );
            if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_FILE )
            {
               // Only tile-cache source here
               if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_TILE_CACHE )
                  return FALSE;

               goto DBInfo;      // Get info from the imagery database

            }  // IMAGELIB_LOADED_FILE (retry)
         }  // IMAGELIB_LOADED_FILE

         CComBSTR ccbsInfo;
         ccbsErrorMsg.Empty();
         InitImageLib();
         m_pImageLib->get_info( &ccbsInfo, &iErr, &ccbsErrorMsg );
         if ( iErr != 0 )
            return FALSE;
   
         AfxBSTR2CString( &m_info, ccbsInfo );
         break;
      }

DBInfo:
      if ( !C_nitf_ovl::s_pNITFOvl->GetNITFDBInfoPage( this, m_info ) )
         return FALSE;

   } while ( FALSE );

   csInfo = m_info;

   return TRUE;
}

#ifndef ALWAYS_ROTATED

// ******************************************************************************
// ******************************************************************************

int C_nitf_obj::update_map_image_buffer(int width, int height, double ul_lat, double ul_lon, double lr_lat, double lr_lon, 
   INT& iErr, CString& csErrorMsg )
{
   //CMyDib dib;
   int pix_cnt;
   BOOL use_old = FALSE;

   if (!m_image_loaded)
      return FAILURE;

   pix_cnt = width * height;
   std::auto_ptr<BYTE> apbImg( new BYTE[ 3 * pix_cnt ] );
   PBYTE pbImg = apbImg.get();

   if ( m_apbOldImg.get() != NULL )
   {
      if ((m_old_width == width) && (m_old_height == height) && 
         (m_old_ul_lat == ul_lat) && (m_old_ul_lon == ul_lon) &&
         (m_old_lr_lat == lr_lat) && (m_old_lr_lon == lr_lon))
      {
         pbImg = m_apbOldImg.get();
         use_old = TRUE;
      }
   }

   if ( !use_old )
   {
      INT iResult = get_map_image( m_csFilespec, width, height, ul_lat, ul_lon, lr_lat, lr_lon, 
         pbImg, &iErr, csErrorMsg );
      if ( iResult != SUCCESS )
         return iResult;

      // Save the image
      m_apbOldImg = apbImg;
      m_old_ul_lat = ul_lat;
      m_old_ul_lon = ul_lon;
      m_old_lr_lat = lr_lat;
      m_old_lr_lon = lr_lon;
      m_old_width = width;
      m_old_height = height;
   }

   SetRGBBuffers( pbImg, pix_cnt );

   return SUCCESS;
}
// end of update_map_image_buffer

#endif // def ALWAYS_ROTATED

// ******************************************************************************
// ******************************************************************************

int C_nitf_obj::update_rotated_map_image_buffer(  int ulx, int uly, int urx, int ury, 
   int lrx, int lry, int llx, int lly, 
   double ul_lat, double ul_lon, double ur_lat, double ur_lon, 
   double lr_lat, double lr_lon, double ll_lat, double ll_lon, 
   INT cTiepoints, DOUBLE* pdTiepointsX, DOUBLE* pdTiepointsY,
                                      DOUBLE* pdTiepointsLat, DOUBLE* pdTiepointsLon,
                                      INT& iWidth, INT& iHeight, INT& iErr, CString& csErrorMsg )
{
   CMyDib dib;
   PBYTE pbImg = NULL;
   INT rslt;

   if (!m_image_loaded)
      return FAILURE;

   rslt = get_rotated_map_image( m_csFilespec, ulx, uly, urx, ury, lrx, lry, llx, lly,
      ul_lat, ul_lon, ur_lat, ur_lon, lr_lat, lr_lon, ll_lat, ll_lon,
                        cTiepoints, pdTiepointsX, pdTiepointsY, pdTiepointsLat, pdTiepointsLon,
                        iWidth, iHeight, pbImg, iErr, csErrorMsg );
   if ( rslt != SUCCESS )
   {
      free( pbImg );
      return rslt;
   }

   SetRGBBuffers( pbImg, iWidth * iHeight );
   free( pbImg );

   return SUCCESS;
}
// end of update_rotated_map_image_buffer

#if 0 // Now handled with auto_ptr
// ******************************************************************************
// ******************************************************************************

void C_nitf_obj::clear_image_buffer()
{
   if (m_red_buffer != NULL)
      free(m_red_buffer);
   if (m_grn_buffer != NULL)
      free(m_grn_buffer);
   if (m_blu_buffer != NULL)
      free(m_blu_buffer);

   m_red_buffer = m_grn_buffer = m_blu_buffer = NULL;
}
#endif

// ******************************************************************************
// ******************************************************************************

void C_nitf_obj::stretch_contrast()
{
   int k, thresh;
   CString error_txt;
   int curval, range;
   double tf;
   CFvwUtil *futil = CFvwUtil::get_instance();

   thresh = futil->round( m_cDisplayImagePixels / 10000.0);

   if (m_eq_mode == NITF_EQ_AUTO)
   {
      auto_stretch_contrast();
      return;
   }

   if (m_eq_mode == NITF_EQ_NONE)
      return;

   // check for "normal" values
   if ((m_contr_minval == 1) && (m_contr_ctrval == 128) && (m_contr_maxval == 255))
      return;

   // make the color map
   for (k=0; k<256; k++)
   {
      if (k <= m_contr_minval)
      {
         m_stretch_map[k] = 1;
         continue;
      }
      if (k >= m_contr_maxval)
      {
         m_stretch_map[k] = 255;
         continue;
      }
      curval = k;
      range = m_contr_maxval - m_contr_minval + 1;

      if (curval < m_contr_ctrval)
      {
         range = m_contr_ctrval - m_contr_minval;
         tf = (double) (curval - m_contr_minval) / (double) range;
         curval = (int) (tf * 128.0);
      }
      else
      {
         range = m_contr_maxval - m_contr_ctrval;
         tf = (double) (curval - m_contr_ctrval) / (double) range;
         curval = futil->round(tf * 128.0);
         curval += 128;
         if (curval > 255)
            curval = 255;
      }

      m_stretch_map[k] = curval;
   }

   for ( k=0; k < m_cDisplayImagePixels; k++ )
   {
      if ((m_red_buffer[k] > 0) || (m_grn_buffer[k] > 0) || (m_blu_buffer[k] > 0))
      {
         m_red_buffer[k] = m_stretch_map[m_red_buffer[k]];
         m_grn_buffer[k] = m_stretch_map[m_grn_buffer[k]];
         m_blu_buffer[k] = m_stretch_map[m_blu_buffer[k]];
      }
   }
}
// end of stretch_contrast

// ******************************************************************************
// ******************************************************************************

void C_nitf_obj::auto_stretch_contrast()
{
   int minval, maxval, k, val, range, thresh, total;
   int lum_val[256];
   BOOL notdone;
   double tf;
   CFvwUtil *futil = CFvwUtil::get_instance();

   for (k=0; k<256; k++)
      lum_val[k] = 0;

   // Generate histogram
   for ( k = 0; k < m_cDisplayImagePixels; k++ )
   {
      val = (int) (((double) m_red_buffer[k] * 0.3) + ((double) m_grn_buffer[k] * 0.59)
         + ((double) m_blu_buffer[k] * 0.11));
      if (val > 0)
         lum_val[val] += 1;
   }

   total = 0;
   for (k=0; k<256; k++)
      total += lum_val[k];

   thresh = futil->round((double) total / 1000.0);

   // find min value
   k = 0;
   notdone = TRUE;
   minval = 1;
   while (k<256 && notdone)
   {
      if (lum_val[k] > thresh)
      {
         minval = k;
         notdone = FALSE;
      }
      k++;
   }

   // find max value
   k = 255;
   notdone = TRUE;
   maxval = 255;
   while (k>=0 && notdone)
   {
      if (lum_val[k] > thresh)
      {
         maxval = k;
         notdone = FALSE;
      }
      k--;
   }

   range = maxval - minval + 1;

   // stretch the contrast
   for ( k = 0; k < m_cDisplayImagePixels; k++ )
   {
      if ((m_red_buffer[k] > 0) && (m_grn_buffer[k] > 0) && (m_blu_buffer[k] > 0))
      {
         val = m_red_buffer[k] - minval;
         if (val < 1)
            val = 1;
         tf = (double) (val) / (double) range;
         val = futil->round(tf * 256.0);
         if (val > 255)
            val = 255;
         m_red_buffer[k] = val;
         val = m_grn_buffer[k] - minval;
         if (val < 1)
            val = 1;
         tf = (double) (val) / (double) range;
         val = futil->round(tf * 256.0);
         if (val > 255)
            val = 255;
         m_grn_buffer[k] = val;
         val = m_blu_buffer[k] - minval;
         if (val < 1)
            val = 1;
         tf = (double) (val) / (double) range;
         val = futil->round(tf * 256.0);
         if (val > 255)
            val = 255;
         m_blu_buffer[k] = val;
      }
   }
}
// end of auto_stretch_contrast


// ******************************************************************************
// ******************************************************************************

int C_nitf_obj::draw_map_image( ActiveMap* map )
{
   CMyDib dib;
   int width, height;
   int rslt, scrn_width, scrn_height, k;
   CString error_msg;
   CRect rc;
   int minx, maxx, miny, maxy;

   if ( m_bShowSecondaryImages != C_nitf_ovl::s_pNITFOvl->m_bShowSecondaryImages )
   {
      Close();       // Get fresh copy of image with different overlay
      m_bShowSecondaryImages = C_nitf_ovl::s_pNITFOvl->m_bShowSecondaryImages;
   }

#ifndef ALWAYS_ROTATED
   double ul_lat, ul_lon, lr_lat, lr_lon;
   int nsavedc;
#endif
   CFvwUtil *futil = CFvwUtil::get_instance();
   CPoint tpt;
   INT iErr;

   if (!m_image_loaded)
      return FAILURE;

   CWaitCursor wait;
   CDC* dc = map->get_CDC();

   scrn_width = C_nitf_ovl::s_pNITFOvl->m_scrn_width;
   scrn_height = C_nitf_ovl::s_pNITFOvl->m_scrn_height;
   width = scrn_width;
   height = scrn_height;

   // check for off the screen
   DOUBLE      // Order is reversed because Y increases downward
      dVertices1[ 4 ][ 2 ] =
         {
            { 0.0, 0.0 },                 // Screen UL
            { 0.0, scrn_height },         // LL
            { scrn_width, scrn_height },  // LR
            { scrn_width, 0.0 }           // UR
         },
      dVertices2[ 4 ][ 2 ] =
         {
            { m_pt[ 0 ].x, m_pt[ 0 ].y }, // Image UL
            { m_pt[ 3 ].x, m_pt[ 3 ].y }, // LL
            { m_pt[ 2 ].x, m_pt[ 2 ].y }, // LR
            { m_pt[ 1 ].x, m_pt[ 1 ].y }  // UR
         };
   if ( !GeometryUtils::PolygonIntersection(
         4, dVertices1, 4, dVertices2 ) )
      return SUCCESS;

   minx = maxx = m_pt[0].x;
   miny = maxy = m_pt[0].y;

   for (k=1; k<4; k++)
   {
      if (minx > m_pt[k].x)
         minx = m_pt[k].x;
      if (miny > m_pt[k].y)
         miny = m_pt[k].y;
      if (maxx < m_pt[k].x)
         maxx = m_pt[k].x;
      if (maxy < m_pt[k].y)
         maxy = m_pt[k].y;
   }

   if (minx < 0)
      minx = 0;
   if (miny < 0)
      miny = 0;
   if (maxx > scrn_width)
      maxx = scrn_width;
   if (maxy > scrn_height)
      maxy = scrn_height;

   // sanity check
   if ((maxx < minx) || (maxy < miny))
      return FAILURE;

   InitImageLib();      // Make sure instance is loaded

#ifdef ALWAYS_ROTATED
   rslt = draw_map_image_rotated( map, iErr );
#else
   if ( m_map_is_rotated )
   {
      rslt = draw_map_image_rotated( map, iErr );
      goto ErrorCheck;
   }

   map->surface_to_geo(minx, miny, &ul_lat, &ul_lon);
   map->surface_to_geo(maxx, maxy, &lr_lat, &lr_lon);

   width = maxx - minx + 1;
   height = maxy - miny + 1;
   m_cDisplayImagePixels = width * height;

   rslt = update_map_image_buffer(width, height, ul_lat, ul_lon, lr_lat, lr_lon, iErr, error_msg);
   if (rslt == SUCCESS)
   {
      CPen pen;
      nsavedc = dc->SaveDC();

      pen.CreateStockObject(NULL_PEN);
      dc->SelectObject(&pen);
      dc->BeginPath();
      dc->Polygon(m_pt, 4);
      dc->EndPath();

      if ((m_red_buffer == NULL) || (m_grn_buffer == NULL) || (m_blu_buffer == NULL))
         return FAILURE;

      stretch_contrast();

      int bpp = GetDeviceCaps(dc->GetSafeHdc(), BITSPIXEL);
      if (bpp >= 24)
      {
         if (m_translucent && (m_trans_value < 255))
         {
            dib.initialize_alpha(width, height, 0, 0, 0);
            dib.set_rgb_image_alpha(m_red_buffer, m_grn_buffer, m_blu_buffer);
            dib.display_alpha(dc, minx, miny, m_trans_value);
         }
         else
         {
            dib.initialize(width, height, 0, 0, 0);
            dib.set_rgb_image(m_red_buffer, m_grn_buffer, m_blu_buffer);
            dib.display_transparent(dc, minx, miny, RGB(0,0,0));
         }
      }
      else
      {
         dib.initialize(width, height, 0, 0, 0);
         dib.set_rgb_image(m_red_buffer, m_grn_buffer, m_blu_buffer);
         dib.display_hashed_transparent(dc, minx, miny, m_trans_value);

      }

      if (nsavedc != 0)
         dc->RestoreDC(nsavedc);
   }

ErrorCheck:
#endif
   if ( rslt != SUCCESS )
   {
      int cx, cy;
      POINT cpt[4];
      cx = (minx + maxx) / 2;
      cy = (miny + maxy) / 2;
      LPCSTR pszText;
      if ( iErr == IMAGELIB_ERROR_SOURCE_BUSY )
         pszText = "Image Source Busy";   // JPIP server busy
      else if ( m_hBackgroundBuildThread != INVALID_HANDLE_VALUE )
         pszText = "Preprocessing";       // Overview build active
      else
         pszText = "No Image";            // General failure
      futil->draw_text( dc, pszText,
         cx, cy, UTIL_ANCHOR_CENTER_CENTER, "Arial", 20, 
         UTIL_FONT_BOLD, UTIL_BG_RECT, UTIL_COLOR_RED, UTIL_COLOR_BLACK,
         0.0, cpt);
   }
   return SUCCESS;

}
// end of draw_map_image


// ******************************************************************************
// ******************************************************************************

// find the intersection of the screen and the image polygon

int C_nitf_obj::find_intersection(  POINT *src_pt,  // IN
   int src_pt_cnt,  // IN
   POINT *clip_pt,  // IN
   int clip_pt_cnt )  // IN
{
   gpc_polygon srcpoly, clippoly, rsltpoly;

   int cnt, k;

   // create the source polygon
   srcpoly.num_contours = 1;
   srcpoly.hole = (int*) malloc(sizeof(int));
   srcpoly.hole[0] = FALSE;
   srcpoly.contour = (gpc_vertex_list*) malloc(sizeof(gpc_vertex_list));
   srcpoly.contour[0].num_vertices = src_pt_cnt;
   srcpoly.contour[0].vertex = (gpc_vertex*) malloc(src_pt_cnt * sizeof(gpc_vertex));
   for (k=0; k<src_pt_cnt; k++)
   {
      srcpoly.contour[0].vertex[k].x = (double) src_pt[k].x;
      srcpoly.contour[0].vertex[k].y = (double) src_pt[k].y;
   }

   // create the source polygon
   clippoly.num_contours = 1;
   clippoly.hole = (int*) malloc(sizeof(int));
   clippoly.hole[0] = FALSE;
   clippoly.contour = (gpc_vertex_list*) malloc(sizeof(gpc_vertex_list));
   clippoly.contour[0].num_vertices = clip_pt_cnt;
   clippoly.contour[0].vertex = (gpc_vertex*) malloc(clip_pt_cnt * sizeof(gpc_vertex));
   for (k=0; k<clip_pt_cnt; k++)
   {
      clippoly.contour[0].vertex[k].x = (double) clip_pt[k].x;
      clippoly.contour[0].vertex[k].y = (double) clip_pt[k].y;
   }

   gpc_polygon_clip(GPC_INT, &srcpoly, &clippoly, &rsltpoly);

   if (rsltpoly.contour == NULL)
   {
      gpc_free_polygon(&srcpoly);
      gpc_free_polygon(&clippoly);
      gpc_free_polygon(&rsltpoly);
      return FAILURE;
   }

   cnt = rsltpoly.contour[0].num_vertices;
   if (cnt < 2)
   {
      gpc_free_polygon(&srcpoly);
      gpc_free_polygon(&clippoly);
      gpc_free_polygon(&rsltpoly);
      return FAILURE;
   }

   m_cViewIntersectionVertices = cnt;
   m_apptViewIntersectionVertices.reset( new POINT[ cnt ] );
   for (k=0; k<cnt; k++)
   {
      m_apptViewIntersectionVertices.get()[k].x = (int) rsltpoly.contour[0].vertex[k].x;
      m_apptViewIntersectionVertices.get()[k].y = (int) rsltpoly.contour[0].vertex[k].y;
   }

   gpc_free_polygon(&srcpoly);
   gpc_free_polygon(&clippoly);
   gpc_free_polygon(&rsltpoly);

   return SUCCESS;
}
// end of find_intersection

// ******************************************************************************
// ******************************************************************************

// Find the geo and corresponding screen quadrangle that contains a possibly rotated image

int C_nitf_obj::find_bounding_quadrangle(
                        ActiveMap* map,                        // IN
                        INT cIntersectionVertices,             // IN
                        const POINT* pptIntersectionVertices,  // IN
                        POINT* imgpt,                          // OUT
                        double *ilat, double *ilon )           // OUT
{
   double minlat, maxlat, minlon, maxlon;
   int /*rslt,*/ k;

#if 1
   // Get the geolimits of the bounding polygon
   for ( k = 0; k < cIntersectionVertices; k++ )
   {
      DOUBLE dLat, dLon;
      if ( SUCCESS != map->surface_to_geo(
            pptIntersectionVertices[ k ].x, pptIntersectionVertices[ k ].y,
            &dLat, &dLon ) )
         return FAILURE;

      if ( k == 0 )
      {
         minlat = maxlat = dLat;
         minlon = maxlon = dLon;
      }
      else
      {
         if ( dLat > maxlat )
            maxlat = dLat;
         if ( dLat < minlat )
            minlat = dLat;
         if ( GEO_east_of_degrees( dLon, maxlon ) )
            maxlon = dLon;
         if ( GEO_east_of_degrees( minlon, dLon ) )
            minlon = dLon;
      }
   }

#else

   // get the geos of the bounding rectangle
   for (k=0; k<4; k++)
   {
      rslt = map->surface_to_geo(imgpt[k].x, imgpt[k].y, &(ilat[k]), &(ilon[k]));
      if (rslt != SUCCESS)
         return FAILURE;
   }

   // find the geo rectangle containing the image
   minlat = maxlat = ilat[0];
   minlon = maxlon = ilon[0];
   for (k=0; k<4; k++)
   {
      if (minlat > ilat[k])
         minlat = ilat[k];
      if (GEO_east_of_degrees(minlon, ilon[k]))
         minlon = ilon[k];
      if (maxlat < ilat[k])
         maxlat = ilat[k];
      if (GEO_east_of_degrees(ilon[k], maxlon))
         maxlon = ilon[k];
   }
#endif
   ilat[0] = maxlat;    // NW
   ilon[0] = minlon;
   ilat[1] = maxlat;    // NE
   ilon[1] = maxlon;
   ilat[2] = minlat;    // SE
   ilon[2] = maxlon;
   ilat[3] = minlat;    // SW
   ilon[3] = minlon;

   for (k=0; k<4; k++)
   {
      if ( SUCCESS != map->geo_to_surface(ilat[k],ilon[k], (int*)&(imgpt[k].x), (int*)&(imgpt[k].y) ) )
         return FAILURE;

      // Recalculate lat/lons for screen point roundoff
      if ( SUCCESS != map->surface_to_geo( imgpt[ k ].x, imgpt[ k ].y, &ilat[ k ], &ilon[ k ] ) )
         return FAILURE;
   }

   return SUCCESS;
}
// end of find_bounding_rectangle

// ******************************************************************************
// ******************************************************************************

int C_nitf_obj::draw_map_image_rotated( ActiveMap* map, INT& iErr )
{
   CMyDib dib;
   int width, height;
   int rslt, scrn_width, scrn_height, k;
   CString csErrorMsg;
   CRect rc;
   int minx, maxx, miny, maxy;
   CFvwUtil *futil = CFvwUtil::get_instance();
   CPoint tpt;
   POINT scrpt[4], imgpt[4];
   double ilat[4], ilon[4];

   if (!m_image_loaded)
      return FAILURE;

   CDC* dc = map->get_CDC();
   map->get_surface_size(&scrn_width, &scrn_height);
   width = scrn_width;
   height = scrn_height;

   // check for off the screen
   DOUBLE      // Order is reversed because Y increases downward
      dVertices1[ 4 ][ 2 ] =
         {
            { 0.0, 0.0 },                 // Screen UL
            { 0.0, scrn_height },         // LL
            { scrn_width, scrn_height },  // LR
            { scrn_width, 0.0 }           // UR
         },
      dVertices2[ 4 ][ 2 ] =
         {
            { m_pt[ 0 ].x, m_pt[ 0 ].y }, // Image UL
            { m_pt[ 3 ].x, m_pt[ 3 ].y }, // LL
            { m_pt[ 2 ].x, m_pt[ 2 ].y }, // LR
            { m_pt[ 1 ].x, m_pt[ 1 ].y }  // UR
         };
   if ( !GeometryUtils::PolygonIntersection(
         4, dVertices1, 4, dVertices2 ) )
      return SUCCESS;

   memcpy( imgpt, m_pt, sizeof(imgpt) );

   scrpt[0].x = 0;
   scrpt[0].y = 0;
   scrpt[1].x = scrn_width;
   scrpt[1].y = 0;
   scrpt[2].x = scrn_width;
   scrpt[2].y = scrn_height;
   scrpt[3].x = 0;
   scrpt[3].y = scrn_height;

   rslt = find_intersection( imgpt, 4, scrpt, 4 );
   if (rslt != SUCCESS)
      return FAILURE;

// debug only
//CPen yellowpen;
//CBrush nullbrush2;
//nullbrush2.CreateStockObject(NULL_BRUSH);
//yellowpen.CreatePen(PS_SOLID, 8, RGB(255,255,0));
//dc->SelectObject(&yellowpen);
//dc->Polygon(rsltpt, rsltcnt);

   // Find the rectangle containing the geopolygon
   minx = miny = INT_MAX;
   maxx = maxy = INT_MIN;

   // Make a copy of the bounding polygon
   INT rsltcnt = m_cViewIntersectionVertices;
   auto_ptr< POINT > apptResult( new POINT[ rsltcnt ] );
   POINT* rsltpt = apptResult.get();
   memcpy( rsltpt, m_apptViewIntersectionVertices.get(), rsltcnt * sizeof(POINT) );

   // Walk the edges to make sure that any projection stretching
   // is included in the min/max limits
   for ( k = 0; k < rsltcnt; k++ )
   {
      INT k2 = ( k + 1 ) % rsltcnt;

      DOUBLE dLat1, dLat2, dLon1, dLon2;
      map->surface_to_geo( rsltpt[ k ].x, rsltpt[ k ].y, &dLat1, &dLon1 );
      map->surface_to_geo( rsltpt[ k2 ].x, rsltpt[ k2 ].y, &dLat2, &dLon2 );

      // If crossed dateline east to west
      if ( dLon2 > dLon1 + 180.0 )
         dLon1 += 360.0;

      // If crossed dateline west to east
      else if ( dLon2 < dLon1 - 180.0 )
         dLon2 += 360.0;

      static const INT NUM_EDGE_STEPS = 10;
      for ( INT i = 0; i < NUM_EDGE_STEPS; i++ )
      {
         DOUBLE
            dLat = ( ( i * dLat2 ) + ( NUM_EDGE_STEPS - i ) * dLat1 ) / NUM_EDGE_STEPS,
            dLon = fmod( ( ( ( i * dLon2 ) + ( NUM_EDGE_STEPS - i ) * dLon1 ) / NUM_EDGE_STEPS )
               + 180.0, 360.0 ) - 180.0;

         INT x, y;
         map->geo_to_surface( dLat, dLon, &x, &y );

         if ( x <= minx )
            minx = x - 1;  // Pessimism
         if ( x >= maxx )
            maxx = x + 1;
         if ( y <= miny )
            miny = y - 1;
         if ( y >= maxy )
            maxy = y + 1;
      }
   }

#if 1 //&& defined USE_GETMAPPEDIMAGE       // 1 = new tiepoints calculation
   if ( rsltcnt < 3 )
      return SUCCESS;

#if defined TIEPOINT_TEST || defined ALL_TESTS    // Showing intersection vertices
   for ( k = 0; k < rsltcnt; k++ )
   {
      POINT pts[2];
      pts[ 0 ].x = rsltpt[ k ].x - 20; pts[ 1 ].x = rsltpt[ k ].x + 20;
      pts[ 0 ].y = rsltpt[ k ].y - 00; pts[ 1 ].y = rsltpt[ k ].y + 00;
      futil->polyline( dc, pts, 2, UTIL_COLOR_GREEN, 1 );
      pts[ 0 ].x = rsltpt[ k ].x - 00; pts[ 1 ].x = rsltpt[ k ].x + 00;
      pts[ 0 ].y = rsltpt[ k ].y - 20; pts[ 1 ].y = rsltpt[ k ].y + 20;
      futil->polyline( dc, pts, 2, UTIL_COLOR_GREEN, 1 );
      pts[ 0 ].x = rsltpt[ k ].x - 20; pts[ 1 ].x = rsltpt[ k ].x + 20;
      pts[ 0 ].y = rsltpt[ k ].y - 20; pts[ 1 ].y = rsltpt[ k ].y + 20;
      futil->polyline( dc, pts, 2, UTIL_COLOR_GREEN, 1 );
      pts[ 0 ].x = rsltpt[ k ].x + 20; pts[ 1 ].x = rsltpt[ k ].x - 20;
      pts[ 0 ].y = rsltpt[ k ].y - 20; pts[ 1 ].y = rsltpt[ k ].y + 20;
      futil->polyline( dc, pts, 2, UTIL_COLOR_GREEN, 1 );
   }
#endif
   // Get geo-bounding rectangle
   if ( SUCCESS != find_bounding_quadrangle( map,
               rsltcnt, rsltpt, imgpt, ilat, ilon ) )
      return FAILURE;

   // If more than 4 vertices, clip the ones with the smallest adjacent areas
   vector< POINT > ptExtras;
   while ( rsltcnt > 4 )
   {
      // Find the areas (x2 but who cares) of the triangles adjacent to each vertex
      DOUBLE dAreaMin = +DBL_MAX;
      INT kMin;
      for ( k = 0; k < rsltcnt; k++ )
      {
         INT k1 = ( k + 1 ) % rsltcnt;
         INT k2 = ( k + rsltcnt - 1 ) % rsltcnt;
         DOUBLE dArea =
            ( (DOUBLE) ( rsltpt[ k2 ].x - rsltpt[ k ].x )
               * (DOUBLE) ( rsltpt[ k1 ].y - rsltpt[ k ].y ) )
            - ( (DOUBLE) ( rsltpt[ k1 ].x - rsltpt[ k ].x )
               * (DOUBLE) ( rsltpt[ k2 ].y - rsltpt[ k ].y ) );

         if ( dArea < dAreaMin )
         {
            dAreaMin = dArea;
            kMin = k;
         }
      }

      // Remove the vertex with the smallest adjacent area
      ptExtras.push_back( rsltpt[ kMin ] );      // Save as an extra tiepoint
      memmove( rsltpt + kMin, rsltpt + kMin + 1, sizeof(POINT) * ( rsltcnt - 1 - kMin ) );
      rsltcnt--;
   }

   // If the intersection is a triangle, manufacture a 4th point
   if ( rsltcnt == 3 )
   {
      auto_ptr< POINT > apptTemp( new POINT[ 4 ] );
      POINT* pptTemp = apptTemp.get();

      // Find the longest side of the triangle
      DOUBLE dMax = -1.0;
      INT kMax;
      for ( k = 0; k < 3; k++ )
      {
         DOUBLE
            dX = rsltpt[ k ].x - rsltpt[ ( k + 1 ) % 3 ].x,
            dY = rsltpt[ k ].y - rsltpt[ ( k + 1 ) % 3 ].y,
            d = sqrt( ( dX * dX ) + ( dY * dY ) );
         if ( d > dMax )
         {
            dMax = d;
            kMax = k;
         }
      }

      // Reflect the third vertex across the longest side
      for ( k = 0; k < 3; k++ )
         pptTemp[ k ] = rsltpt[ ( kMax + 1 + k ) % 3 ];
      pptTemp[ 3 ].x = rsltpt[ kMax ].x + rsltpt[ ( kMax + 1 ) % 3 ].x - rsltpt[ ( kMax + 2 ) % 3 ].x;
      pptTemp[ 3 ].y = rsltpt[ kMax ].y + rsltpt[ ( kMax + 1 ) % 3 ].y - rsltpt[ ( kMax + 2 ) % 3 ].y;

      apptResult = apptTemp;     // Replace the original result
      rsltpt = apptResult.get();
      rsltcnt = 4;
   }

#ifdef USE_GETMAPPEDIMAGE
   static const INT
      NUM_TIEPOINTS_X_Y = 4,     // 4 = cubic fit
      NUM_TIEPOINTS = NUM_TIEPOINTS_X_Y * NUM_TIEPOINTS_X_Y;
   m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_SET_MAPPING_POLYNOMIAL_ORDERS,
      1, VARIANT_NULL, NULL, NULL, NULL, NULL );

   INT cTiepoints = NUM_TIEPOINTS + ptExtras.size();
   auto_ptr< DOUBLE >
      apdTiepointsX( new DOUBLE[ cTiepoints ] ),
      apdTiepointsY( new DOUBLE[ cTiepoints ] ),
      apdTiepointsLat( new DOUBLE[ cTiepoints ] ),
      apdTiepointsLon( new DOUBLE[ cTiepoints ] );
   DOUBLE
      *pdTiepointsX = &*apdTiepointsX,
      *pdTiepointsY = &*apdTiepointsY,
      *pdTiepointsLat = &*apdTiepointsLat,
      *pdTiepointsLon = &*apdTiepointsLon;
   for ( INT k1 = 0; k1 < NUM_TIEPOINTS_X_Y; k1++ )
   {
      DOUBLE
         dFrac12 = k1 / (DOUBLE) ( NUM_TIEPOINTS_X_Y - 1 ),
         dFrac11 = 1.0 - dFrac12,
         dX1 = ( dFrac11 * rsltpt[ 0 ].x ) + ( dFrac12 * rsltpt[ 1 ].x ),
         dX2 = ( dFrac11 * rsltpt[ 3 ].x ) + ( dFrac12 * rsltpt[ 2 ].x ),
         dY1 = ( dFrac11 * rsltpt[ 0 ].y ) + ( dFrac12 * rsltpt[ 1 ].y ),
         dY2 = ( dFrac11 * rsltpt[ 3 ].y ) + ( dFrac12 * rsltpt[ 2 ].y );
      for ( INT k2 = 0; k2 < NUM_TIEPOINTS_X_Y; k2++ )
      {
         DOUBLE
            dFrac22 = k2 / (DOUBLE) ( NUM_TIEPOINTS_X_Y - 1 ),
            dFrac21 = 1.0 - dFrac22;
         k = ( NUM_TIEPOINTS_X_Y * k1 ) + k2;
         pdTiepointsX[ k ] = ( dFrac22 * dX2 ) + ( dFrac21 * dX1 );
         pdTiepointsY[ k ] = ( dFrac22 * dY2 ) + ( dFrac21 * dY1 );
         map->surface_to_geo( pdTiepointsX[ k ], pdTiepointsY[ k ],
            &pdTiepointsLat[ k ], &pdTiepointsLon[ k ] );
      }
   }

   // Add any discarded boundary points to the tiepoints list
   k = NUM_TIEPOINTS;
   while ( !ptExtras.empty() )
   {
      pdTiepointsX[ k ] = ptExtras.back().x;
      pdTiepointsY[ k ] = ptExtras.back().y;

      map->surface_to_geo( ptExtras.back().x, ptExtras.back().y,
            &pdTiepointsLat[ k ], &pdTiepointsLon[ k ] );
      k++;
      ptExtras.pop_back();
   }

#else // !defined USE_GETMAPPEDIMAGE
   static const INT NUM_TIEPOINTS = 4;
   INT cTiepoints = NUM_TIEPOINTS;
   for ( k = 0; k < NUM_TIEPOINTS; k++ )
   {
      imgpt[ k ].x = rsltpt[ k ].x;
      imgpt[ k ].y = rsltpt[ k ].y;
      map->surface_to_geo( imgpt[ k ].x, imgpt[ k ].y,
         &ilat[ k ], &ilon[ k ] );
   }

#endif

#if defined TIEPOINT_TEST || defined ALL_TESTS   // Showing tiepoints
   for ( k = 0; k < cTiepoints; k++ )
   {
      INT
#if defined USE_GETMAPPEDIMAGE
         x = (INT) ( pdTiepointsX[ k ] + 0.5 ),
         y = (INT) ( pdTiepointsY[ k ] + 0.5 );
#else
         x = imgpt[ k ].x,
         y = imgpt[ k ].y; 
#endif
      POINT pts[2];
      pts[ 0 ].x = x - 15; pts[ 1 ].x = x + 15;
      pts[ 0 ].y = y - 00; pts[ 1 ].y = y + 00;
      futil->polyline( dc, pts, 2, UTIL_COLOR_MAGENTA, 1 );
      pts[ 0 ].x = x - 00; pts[ 1 ].x = x + 00;
      pts[ 0 ].y = y - 15; pts[ 1 ].y = y + 15;
      futil->polyline( dc, pts, 2, UTIL_COLOR_MAGENTA, 1 );
      pts[ 0 ].x = x - 15; pts[ 1 ].x = x + 15;
      pts[ 0 ].y = y - 15; pts[ 1 ].y = y + 15;
      futil->polyline( dc, pts, 2, UTIL_COLOR_MAGENTA, 1 );
      pts[ 0 ].x = x + 15; pts[ 1 ].x = x - 15;
      pts[ 0 ].y = y - 15; pts[ 1 ].y = y + 15;
      futil->polyline( dc, pts, 2, UTIL_COLOR_MAGENTA, 1 );
   }
#endif

#else // Old tiepoints calculation
free(rsltpt);
#endif

// sanity check
if ((maxx < minx) || (maxy < miny))
   return FAILURE;

width = maxx - minx + 1;
height = maxy - miny + 1;
m_cDisplayImagePixels = width * height;

   for ( k = 0; k < cTiepoints; k++ )
   {
#ifdef USE_GETMAPPEDIMAGE
      pdTiepointsX[ k ] -= minx;
      pdTiepointsY[ k ] -= miny;
#else
      imgpt[ k ].x -= minx;
      imgpt[ k ].y -= miny;
#endif
   }

   rslt = update_rotated_map_image_buffer(imgpt[0].x, imgpt[0].y, imgpt[1].x, imgpt[1].y, 
      imgpt[2].x, imgpt[2].y, imgpt[3].x, imgpt[3].y, 
      ilat[0], ilon[0], ilat[1], ilon[1],
      ilat[2], ilon[2], ilat[3], ilon[3],
#ifdef USE_GETMAPPEDIMAGE
      cTiepoints, pdTiepointsX, pdTiepointsY, pdTiepointsLat, pdTiepointsLon,
#else
      0, NULL, NULL, NULL, NULL,
#endif
      width, height, iErr, csErrorMsg );

   if (rslt != SUCCESS)
      return rslt;

   if ((m_red_buffer == NULL) || (m_grn_buffer == NULL) || (m_blu_buffer == NULL))
      return FAILURE;

   stretch_contrast();

   int bpp = GetDeviceCaps(dc->GetSafeHdc(), BITSPIXEL);

   if (bpp >= 24)
   {
      dib.initialize_alpha(width, height, 0, 0, 0);
      dib.set_rgb_image_alpha(m_red_buffer, m_grn_buffer, m_blu_buffer);
      dib.display_alpha(dc, minx, miny, m_trans_value);
   }
   else
   {
      dib.initialize(width, height, 0, 0, 0);
      dib.set_rgb_image(m_red_buffer, m_grn_buffer, m_blu_buffer);
      dib.display_hashed_transparent(dc, minx, miny, m_trans_value);
   }

   return SUCCESS;
}
// end of draw_map_image_rotated

#ifndef ALWAYS_ROTATED

// ******************************************************************************
// ******************************************************************************

INT C_nitf_obj::get_map_image( const CString& csFilename, INT iWidth, INT iHeight, 
   DOUBLE ul_lat, DOUBLE ul_lon, DOUBLE lr_lat, DOUBLE lr_lon, 
   PBYTE pbImgData, PINT piErr, CString& csErrorMsg )
{
   if ( !m_image_loaded )
      return FAILURE;

   _variant_t vImageData;
   INT iSize = iWidth * iHeight * 3;

   try
   {
      CComBSTR ccbsErrorMsg;

      // Load image into ImageLib instance
      if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_FILE )
      {
         ImageLibProxyLoad( piErr, &ccbsErrorMsg );
         if ( m_eImageLibLoadStatus == IMAGELIB_LOAD_ERROR )
            goto ErrorExit;
         if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_FILE )
         {
            if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_TILE_CACHE )
               return SUCCESS;   // TEMP

            DOUBLE dDegreesPerPixel =
               sqrt( ( ul_lat - lr_lat ) * ( lr_lon - ul_lon ) / ( iWidth * iHeight ) );
            TileCacheService::TileCacheStatusEnum eStatus;
#ifdef USE_GETRGBGROUPMAPIMAGE
            MSXML2::IXMLDOMDocumentPtr pxdTileData( __uuidof(DOMDocument60) );
#endif
            try
            {
               CComBSTR ccbsXMLTileData;
               HRESULT hr = m_tcsTileCacheService.RetrieveTiledImage(
                  m_bstrSubserviceUserName, m_bstrSubservicePassword,
                  m_bstrTileCacheSubserviceName,
                  m_bstrTileCacheImageIdentifier,
                  lr_lat, ul_lon,
                  ul_lat, lr_lon,
                  dDegreesPerPixel,
                  &eStatus,
                  &ccbsXMLTileData );
               if ( hr != S_OK )
                  return FAILURE;

               if ( eStatus != TileCacheService::TILE_CACHE_STATUS_OK )
               {
                  if ( eStatus == TileCacheService::TILE_CACHE_STATUS_PENDING )
                  {
                     if ( m_hBackgroundBuildThread == INVALID_HANDLE_VALUE )
                     {
                        // Save any retry hint
                        m_bstrBackgroundBuildParam = (BSTR) ccbsXMLTileData;
                        StartBackgroundBuildThread();      // Will retry after 10 seconds
                     }
                  }
                  return FAILURE;
               }
#ifndef USE_GETRGBGROUPMAPIMAGE
               CComBSTR ccbsErrMsg;
               INT iWidth2, iHeight2;
               hr = m_pImageLib->LoadImageXML( (BSTR) ccbsXMLTileData, &iWidth2,
                  &iHeight2, piErr, &ccbsErrMsg );
               if ( hr != S_OK )
                  return FAILURE;

               ccbsErrMsg.Empty();
#if 1
               goto GetMapImageTry;
#else
               hr = m_pImageLib->get_rgb_group_map_image( ul_lat, ul_lon, lr_lat, lr_lon,
                  iWidth, iHeight, pbImgData, piErr, &ccbsErrMsg );
               if ( hr != S_OK )
                  return FAILURE;
#endif
#else // def USE_GETRGBGROUPMAPIMAGE
               if ( !pxdTileData->loadXML( (BSTR) ccbsXMLTileData ) )
                  return FAILURE;   // Bad XML string

               // Make sure no other tile data is loaded
               m_pImageLib->close();

               // Load the tiles
               MSXML2::IXMLDOMNodeListPtr pxlTiles = pxdTileData->selectNodes( L"/TileList/Tile" );
               MSXML2::IXMLDOMNodePtr pxnTile;
               vector< IStreamPtr > apStreams;
               while ( NULL != ( pxnTile = pxlTiles->nextNode() ) )
               {
                  MSXML2::IXMLDOMElementPtr pxeTile( pxnTile );

                  static const struct
                  {
                     LPCWSTR  pwszCornerName;
                     INT      iXMul;
                     INT      iYMul;
                  } CornerInfo[4] =
                  {
                     { L"UpperLeft", 0, 0 },
                     { L"UpperRight", 1, 0 },
                     { L"LowerRight", 1, 1 },
                     { L"LowerLeft", 0, 1 }
                  };
                  
                  INT
                     iTileWidth = (LONG) pxeTile->getAttribute( L"TileWidth" ),
                     iTileHeight = (LONG) pxeTile->getAttribute( L"TileHeight" );

                  wostringstream wssGeoInfo;
                  for ( INT k = 0; k < 4; k++ )
                  {
#if 0
                     wssGeoInfo <<
                        L"XPos=" << ( CornerInfo[k].iXMul * iTileWidth ) << L","
                        L"YPos=" << ( CornerInfo[k].iYMul * iTileHeight ) << L",";
                     _bstr_t bstrName, bstrVal;
                     HRESULT hr;
                     _variant_t varVal;

                     bstrName = _bstr_t( CornerInfo[k].pwszCornerName ) + L"Lat";
                     hr = pxeTile->raw_getAttribute( bstrName, &varVal );
                     wssGeoInfo <<
                        L"Lat=" << (LPCWSTR) _bstr_t( varVal ) << L",";

                     bstrName = _bstr_t( CornerInfo[k].pwszCornerName ) + L"Lon";
                     hr = pxeTile->raw_getAttribute( bstrName, &varVal );
                     wssGeoInfo <<
                        L"Lon=" << (LPCWSTR) _bstr_t( varVal ) << L",";
#else
                     wssGeoInfo <<
                        L"XPos=" << ( CornerInfo[k].iXMul * iTileWidth ) << L","
                        L"YPos=" << ( CornerInfo[k].iYMul * iTileHeight ) << L","
                        L"Lat=" << (LPCWSTR) _bstr_t(
                           pxeTile->getAttribute( _bstr_t( CornerInfo[k].pwszCornerName ) + L"Lat" )
                           ) << L","
                        L"Lon=" << (LPCWSTR) _bstr_t(
                           pxeTile->getAttribute( _bstr_t( CornerInfo[k].pwszCornerName ) + L"Lon" )
                           ) << L",";
#endif
                  }
#if ( 0 || defined ALL_TESTS ) && defined _DEBUG
                  ATLTRACE( _T("Geo info = \"%s\"\n"), CW2T( wssGeoInfo.str().c_str() ) );
#endif

                  // Get the base64 string
                  _bstr_t bstrImageData = pxnTile->text;
                  INT cChars = bstrImageData.length();   // Input wide characters

                  // Worst case WC->MBC output
                  auto_ptr< CHAR > apszImageData( new CHAR[ 2 * cChars ] );
                  
                  // Make multibyte
                  cChars = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR) bstrImageData, cChars,
                     apszImageData.get(), 2 * cChars, NULL, NULL );
                  
                  // Decode the base-64
                  INT cBytes = Base64EncodeGetRequiredLength( cChars );
                  auto_ptr< BYTE > apbImageData( new BYTE[ cBytes ] );
                  if ( !Base64Decode( apszImageData.get(), cChars, apbImageData.get(), &cBytes ) )
                     return FAILURE;

                  // Create a standard stream in memory
                  IStreamPtr pStream;
                  if ( S_OK != CreateStreamOnHGlobal( 0, TRUE, (LPSTREAM*) &pStream ) )
                     return FAILURE;

                  ULONG ucBytes;
                  if ( S_OK != pStream->Write( apbImageData.get(), cBytes, &ucBytes ) )
                     return FAILURE;

                  ccbsErrorMsg.Empty();
                  hr = m_pImageLib->load_stream_group( pStream, _bstr_t( wssGeoInfo.str().c_str() ),
                     piErr, &ccbsErrorMsg );
                  ATLASSERT( hr == S_OK );

                  apStreams.push_back( pStream );     // Keep alive
               }  // Tile loop

#if ( 0 || defined ALL_TESTS ) && defined _DEBUG
               for ( UINT k = 0; k < apStreams.size(); k++ )
               {
                  apStreams[k]->AddRef();
                  INT n = apStreams[k]->Release();
                  ATLTRACE( _T("Before: stream #%d, ref count = %d\n"), k, n );
               }
#endif
               ccbsErrorMsg.Empty();
               hr = m_pImageLib->get_rgb_group_map_image( ul_lat, ul_lon, lr_lat, lr_lon,
                  iWidth, iHeight, pbImgData, piErr, &ccbsErrorMsg );
#if ( 0 || defined ALL_TESTS ) && defined _DEBUG
               for ( UINT k = 0; k < apStreams.size(); k++ )
               {
                  apStreams[k]->AddRef();
                  INT n = apStreams[k]->Release();
                  ATLTRACE( _T("After: stream #%d, ref count = %d\n"), k, n );
               }
#endif
#endif   // ndef USE_GETRGBGROUPMAPIMAGE
               return *piErr == 0 ? SUCCESS : FAILURE;

            }
            catch( ... )      // Anything
            {
               return FAILURE;
            }

         }  // IMAGELIB_LOADED_FILE after ImageLib::load()
      }  // IMAGELIB_LOADED_FILE

      // Do any overview building in a background thread unless it has failed
GetMapImageTry:
      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_OVERVIEW_BUILD,
         m_bBackgroundBuildThreadFailed ? VARIANT_TRUE : VARIANT_FALSE, VARIANT_NULL,
         NULL, NULL, NULL, NULL );
      
      ccbsErrorMsg.Empty();
#ifdef USE_GETMAPPEDIMAGE
      {
         static const INT NUM_TIEPOINTS = 4;
         DOUBLE
            dTiepointsX[ NUM_TIEPOINTS ] = { 0, iWidth - 1, 0, iWidth - 1},
            dTiepointsY[ NUM_TIEPOINTS ] = { 0, 0, iHeight - 1, iHeight - 1 },
            dTiepointsLat[ NUM_TIEPOINTS ] = { ul_lat, ul_lat, lr_lat, lr_lat },
            dTiepointsLon[ NUM_TIEPOINTS ] = { ul_lon, lr_lon, ul_lon, lr_lon };
         ULONG mOptsUsed;
         m_pImageLib->GetMappedImage(
            iWidth, iHeight,
            dTiepointsX, dTiepointsY,
            dTiepointsLat, dTiepointsLon,
            NUM_TIEPOINTS, INTERP_MODE_BICUBIC,
            MAPPING_OPTIONS, &mOptsUsed,
            &vImageData, piErr, &ccbsErrorMsg );
      }
#else
      m_pImageLib->get_map_image( iWidth, iHeight, ul_lat, ul_lon, lr_lat, lr_lon,
         &vImageData, piErr, &ccbsErrorMsg );
#endif //ndef USE_GETMAPPEDIMAGE

      // Reenable automatic overview file building
      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_OVERVIEW_BUILD,
         VARIANT_TRUE, VARIANT_NULL, NULL, NULL, NULL, NULL );

      if ( *piErr != 0 )
      {
         // Start an overview build if necessary
         if ( *piErr == IMAGELIB_ERROR_SUBFUNCTION_INHIBITED )
         {
            // Only if not already active
            if ( m_hBackgroundBuildThread == INVALID_HANDLE_VALUE )
            {
               // Abort any overview builds that aren't needed immediately
               POSITION pos = C_nitf_ovl::s_pNITFOvl->m_objlist.GetHeadPosition();
               while ( pos != NULL )
               {
                  NITFObjPtr pObj = C_nitf_ovl::s_pNITFOvl->m_objlist.GetNext( pos );
                  if ( pObj->m_priority == INT_MIN    // If unreferenced
                     || pObj->m_eFillControl == FILL_CONTROL_OUTLINE_FRAME )  // or not shown
                  {
                     // Abort any overview build even if underway
                     pObj->m_cbImageLibCallback2.m_eCallbackMode =
                        ImageLibCallbackInterface::CALLBACK_MODE_ABORTING;
                  }
               }  // Unnecessary overview build check loop

               if ( SUCCESS != StartBackgroundBuildThread() )
                  goto GetMapImageTry;    // Retry if background thread failed
            }
            return FAILURE;         // Draw later
         }

ErrorExit:
         AfxBSTR2CString( &csErrorMsg, ccbsErrorMsg );
         return FAILURE;
      }
   }
   catch( COleException *e )
   {
      e->Delete();
      csErrorMsg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   catch( CMemoryException *e )
   {
      e->Delete();
      csErrorMsg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   catch(...)
   {
      csErrorMsg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }

   ASSERT( iSize == vImageData.parray->rgsabound[0].cElements );
   memcpy( pbImgData, vImageData.parray->pvData, iSize );

   return SUCCESS;
}
// end of get_map_image

#endif   // def ALWAYS_ROTATED

// ******************************************************************************
// ******************************************************************************

INT C_nitf_obj::get_rotated_map_image( const CString& csFilename, 
   INT ulx, INT uly, INT urx, INT ury, 
   INT lrx, INT lry, INT llx, INT lly, 
   DOUBLE ul_lat, DOUBLE ul_lon, DOUBLE ur_lat, DOUBLE ur_lon, 
   DOUBLE lr_lat, DOUBLE lr_lon, DOUBLE ll_lat, DOUBLE ll_lon,
   INT cTiepoints, DOUBLE* pdTiepointsX, DOUBLE* pdTiepointsY,
   DOUBLE* pdTiepointsLat, DOUBLE* pdTiepointsLon,
   INT& iWidth, INT& iHeight, 
   PBYTE& pbImageData, INT& iErr, CString& csErrorMsg )
{
   if ( !m_image_loaded )
      return FAILURE;

   _variant_t varImageData;
   INT cPixels = iWidth * iHeight;
   CString ext;

   try
   {
      CComBSTR ccbsErrorMsg;

      if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_FILE )
      {
         ImageLibProxyLoad( &iErr, &ccbsErrorMsg );
         if ( m_eImageLibLoadStatus == IMAGELIB_LOAD_ERROR           // Load error
               || m_eImageLibLoadStatus == IMAGELIB_LOAD_DELAYED )   // JPIP server busy
            goto ErrorExit;

         // After reload attempt
         if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_FILE )
         {
            if ( m_eImageLibLoadStatus != IMAGELIB_LOADED_TILE_CACHE )
               return FAILURE;

#ifdef USE_GETRGBGROUPMAPIMAGE
            if ( m_map_is_rotated )
               return FAILURE;

            MSXML2::IXMLDOMDocumentPtr pxdTileData( __uuidof(DOMDocument60) );
#endif
            DOUBLE dDegreesPerPixel =
               sqrt( ( ul_lat - lr_lat ) * ( ( lr_lon > ul_lon ? lr_lon : lr_lon + 360.0 ) - ul_lon )
               / ( iWidth * iHeight ) );
            TileCacheService::TileCacheStatusEnum eStatus;
            try
            {
               CComBSTR ccbsXMLTileData;
               HRESULT hr;
               do
               {
                  hr = m_tcsTileCacheService.RetrieveTiledImage(
                     m_bstrSubserviceUserName, m_bstrSubservicePassword,
                     m_bstrTileCacheSubserviceName,
                     m_bstrTileCacheImageIdentifier,
                     lr_lat, ul_lon,
                     ul_lat, lr_lon,
                     dDegreesPerPixel,
                     &eStatus,
                     &ccbsXMLTileData );
                  if ( hr != E_FAIL || m_bBasicAuthorizationUsed
                        || ( m_tcsTileCacheService.GetStatusCode() != 401
                           && m_tcsTileCacheService.GetStatusCode() != 403 ) )
                     break;
                  
                  // If NTLM authorization failed, try basic
                  m_tcsTileCacheService.m_socket.RemoveAuthObject( _T("NTLM") );
                  m_tcsTileCacheService.m_socket.AddAuthObj( _T("Basic"),
                     &m_sbaoBasicAuthorization,  &m_sbaoBasicAuthorization );
                  m_bBasicAuthorizationUsed = TRUE;
               } while ( TRUE );
               if ( hr != S_OK )
                  return FAILURE;

               if ( eStatus != TileCacheService::TILE_CACHE_STATUS_OK )
               {
                  if ( eStatus == TileCacheService::TILE_CACHE_STATUS_PENDING )
                  {
                     if ( !IsBusy () )
                     {
                        // Save any retry hint
                        m_bstrBackgroundBuildParam = (BSTR) ccbsXMLTileData;
                        StartBackgroundBuildThread();      // Will retry after 10 seconds
                     }
                  }
                  return FAILURE;
               }

#ifdef USE_GETRGBGROUPMAPIMAGE
               if ( !pxdTileData->loadXML( (BSTR) ccbsXMLTileData ) )
                  return FAILURE;   // Bad XML string

               // Make sure no other tile data is loaded
               m_pImageLib->close();

               // Load the tiles
               MSXML2::IXMLDOMNodeListPtr pxlTiles = pxdTileData->selectNodes( L"/TileList/Tile" );
               MSXML2::IXMLDOMNodePtr pxnTile;
               vector< IStreamPtr > apStreams;
               while ( NULL != ( pxnTile = pxlTiles->nextNode() ) )
               {
                  MSXML2::IXMLDOMElementPtr pxeTile( pxnTile );

                  static const struct
                  {
                     LPCWSTR  pwszCornerName;
                     INT      iXMul;
                     INT      iYMul;
                  } CornerInfo[4] =
                  {
                     { L"UpperLeft", 0, 0 },
                     { L"UpperRight", 1, 0 },
                     { L"LowerRight", 1, 1 },
                     { L"LowerLeft", 0, 1 }
                  };
                  
                  INT
                     iTileWidth = (LONG) pxeTile->getAttribute( L"TileWidth" ),
                     iTileHeight = (LONG) pxeTile->getAttribute( L"TileHeight" );

                  wostringstream wssGeoInfo;
                  for ( INT k = 0; k < 4; k++ )
                  {
                     wssGeoInfo <<
                        L"XPos=" << ( CornerInfo[k].iXMul * iTileWidth ) << L","
                        L"YPos=" << ( CornerInfo[k].iYMul * iTileHeight ) << L","
                        L"Lat=" << (LPCWSTR) _bstr_t(
                           pxeTile->getAttribute( _bstr_t( CornerInfo[k].pwszCornerName ) + L"Lat" )
                           ) << L","
                        L"Lon=" << (LPCWSTR) _bstr_t(
                           pxeTile->getAttribute( _bstr_t( CornerInfo[k].pwszCornerName ) + L"Lon" )
                           ) << L",";
                  }
#if ( 0 || defined ALL_TESTS ) && defined _DEBUG
                  ATLTRACE( _T("Geo info = \"%s\"\n"), CW2T( wssGeoInfo.str().c_str() ) );
#endif

                  // Get the base64 string
                  _bstr_t bstrImageData = pxnTile->text;
                  INT cChars = bstrImageData.length();   // Input wide characters

                  // Worst case WC->MBC output
                  auto_ptr< CHAR > apszImageData( new CHAR[ 2 * cChars ] );
                  
                  // Make multibyte
                  cChars = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR) bstrImageData, cChars,
                     apszImageData.get(), 2 * cChars, NULL, NULL );
                  
                  // Decode the base-64
                  INT cBytes = Base64EncodeGetRequiredLength( cChars );
                  auto_ptr< BYTE > apbImageData( new BYTE[ cBytes ] );
                  if ( !Base64Decode( apszImageData.get(), cChars, apbImageData.get(), &cBytes ) )
                     return FAILURE;

                  // Create a standard stream in memory
                  IStreamPtr pStream;
                  if ( S_OK != CreateStreamOnHGlobal( 0, TRUE, (LPSTREAM*) &pStream ) )
                     return FAILURE;

                  ULONG ucBytes;
                  if ( S_OK != pStream->Write( apbImageData.get(), cBytes, &ucBytes ) )
                     return FAILURE;

                  ccbsErrorMsg.Empty();
                  hr = m_pImageLib->load_stream_group( pStream, _bstr_t( wssGeoInfo.str().c_str() ),
                     &iErr, &ccbsErrorMsg );
                  ATLASSERT( hr == S_OK );

                  apStreams.push_back( pStream );     // Keep alive
               }  // Tile loop

#if ( 0 || defined ALL_TESTS ) && defined _DEBUG
               for ( UINT k = 0; k < apStreams.size(); k++ )
               {
                  apStreams[k]->AddRef();
                  INT n = apStreams[k]->Release();
                  ATLTRACE( _T("Before: stream #%d, ref count = %d\n"), k, n );
               }
#endif
               pbImageData = (PBYTE) malloc( 3 * sizeof(BYTE) * cPixels );
               if ( pbImageData == NULL )
                  return FAILURE;

               ccbsErrorMsg.Empty();
               hr = m_pImageLib->get_rgb_group_map_image( ul_lat, ul_lon, lr_lat, lr_lon,
                  iWidth, iHeight, pbImageData, &iErr, &ccbsErrorMsg );
#if ( 0 || defined ALL_TESTS ) && defined _DEBUG
               for ( UINT k = 0; k < apStreams.size(); k++ )
               {
                  apStreams[k]->AddRef();
                  INT n = apStreams[k]->Release();
                  ATLTRACE( _T("After: stream #%d, ref count = %d\n"), k, n );
               }
#endif
               return iErr == 0 ? SUCCESS : FAILURE;

#else // ndef USE_GETRGBGROUPMAPIMAGE
               CComBSTR ccbsErrMsg;
               INT iWidth2, iHeight2;
               hr = m_pImageLib->LoadImageXML( (BSTR) ccbsXMLTileData, &iWidth2,
                  &iHeight2, &iErr, &ccbsErrMsg );
               if ( hr != S_OK )
                  return FAILURE;

               ccbsErrMsg.Empty();
#endif // ndef USE_GETRGBGROUPMAPIMAGE

            }  // try block for RetrieveTiledImage()
            catch( ... )      // Anything
            {
               return FAILURE;
            }

         }  // !IMAGELIB_LOADED_FILE, retry
      }  // !IMAGELIB_LOADED_FILE, initial check

      // Do any overview building in a background thread unless it has failed
GetMapImageTry:
      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_OVERVIEW_BUILD,
         m_bBackgroundBuildThreadFailed ? VARIANT_TRUE : VARIANT_FALSE,
         VARIANT_NULL, NULL, NULL, NULL, NULL );

      ccbsErrorMsg.Empty();

      ext = csFilename.Right(3);

      if (!ext.CompareNoCase("PDF"))
      {
         m_pImageLib->get_rotated_map_image( ulx, uly, urx, ury, lrx, lry, llx, lly, ul_lat, ul_lon,
            ur_lat, ur_lon, lr_lat, lr_lon, ll_lat, ll_lon, 
            &iWidth, &iHeight, &varImageData, &iErr, &ccbsErrorMsg );
      }
      else
      {

#ifdef USE_GETMAPPEDIMAGE
      {
         // If we are using manual win/lev, use the original min/max scale and offsets if possible
         static const VARIANT VAR_NULL = { VT_NULL };
         if ( m_eq_mode == NITF_EQ_MANUAL )
            m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_HISTOGRAM_SCALING,
               VARIANT_TRUE, VAR_NULL, NULL, NULL, NULL, NULL );
               
         ULONG mOptsUsed;
         m_pImageLib->GetMappedImage(
            iWidth, iHeight,
            pdTiepointsX, pdTiepointsY,
            pdTiepointsLat, pdTiepointsLon,
            cTiepoints, INTERP_MODE_BICUBIC,
            MAPPING_OPTIONS, &mOptsUsed,
            &varImageData, &iErr, &ccbsErrorMsg );

         if ( m_eq_mode == NITF_EQ_MANUAL )
            m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_HISTOGRAM_SCALING,
               VARIANT_FALSE, VAR_NULL, NULL, NULL, NULL, NULL );
               
      }
#else
         m_pImageLib->get_rotated_map_image( ulx, uly, urx, ury, lrx, lry, llx, lly, ul_lat, ul_lon,
            ur_lat, ur_lon, lr_lat, lr_lon, ll_lat, ll_lon, 
            &iWidth, &iHeight, &varImageData, &iErr, &ccbsErrorMsg );
#endif

      }
#if ( 0 && defined _DEBUG ) || defined ALL_TESTS
     ATLTRACE( _T("GetMappedImage(\"%s\") - time = %u, incomplete = %d, iErr = %d\n"),
        m_filename, GetTickCount(), m_bIncompleteDraw, iErr );
#endif

      m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_OVERVIEW_BUILD,
         VARIANT_TRUE, VARIANT_NULL, NULL, NULL, NULL, NULL );

      if ( iErr != 0 )
      {
         // Start an overview build if necessary
         if ( iErr == IMAGELIB_ERROR_SUBFUNCTION_INHIBITED )
         {
            if ( m_hBackgroundBuildThread == INVALID_HANDLE_VALUE )
            {
               // Abort any overview builds that aren't needed immediately
               POSITION pos = C_nitf_ovl::s_pNITFOvl->m_objlist.GetHeadPosition();
               while ( pos != NULL )
               {
                  NITFObjPtr pObj = C_nitf_ovl::s_pNITFOvl->m_objlist.GetNext( pos );
                  if ( pObj->m_priority == INT_MIN    // If unreferenced
                     || pObj->m_eFillControl == FILL_CONTROL_OUTLINE_FRAME )  // or not shown
                  {
                     // Abort any overview build even if underway
                     pObj->m_cbImageLibCallback2.m_eCallbackMode =
                        ImageLibCallbackInterface::CALLBACK_MODE_ABORTING;
                  }
               }  // Unnecessary overview build check loop

               if ( SUCCESS != StartBackgroundBuildThread() )
                  goto GetMapImageTry;    // Retry if background thread failed
            }
            return FAILURE;         // Draw later
         }

ErrorExit:
         AfxBSTR2CString( &csErrorMsg, ccbsErrorMsg );
         return FAILURE;
      }

   }
   catch( COleException *e )
   {
      e->Delete();
      csErrorMsg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   catch( CMemoryException *e )
   {
      e->Delete();
      csErrorMsg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }
   catch( ... )
   {
      csErrorMsg = _T("Unable to use ImageLib.dll");
      return FAILURE;
   }

   m_cDisplayImagePixels = iWidth * iHeight; // May have been modified
   INT iSize = m_cDisplayImagePixels * 3;
   ASSERT( iSize == varImageData.parray->rgsabound[0].cElements );

   pbImageData = (PBYTE) malloc( iSize );
   if ( pbImageData == NULL )
      return FAILURE;

   memcpy( pbImageData, varImageData.parray->pvData, iSize );

   return SUCCESS;
}
// end of get_rotated_map_image()


// ******************************************************************************
// ******************************************************************************
#if 0
int C_nitf_obj::extract_date(CString info, CString &date) 
{
   int pos, end;

   date = "";
   pos = info.Find("File Date: ", 0);
   if (pos < 0)
      return FAILURE;

   end = info.Find('\r', pos);
   date = info.Mid(pos+11, end - pos - 11);
   return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_obj::extract_color_info(CString info, CString &color_info) 
{
   int pos, end;

   color_info = "";
   pos = info.Find("Color Image: ", 0);
   if (pos < 0)
      return FAILURE;

   end = info.Find('\r', pos);
   color_info = info.Mid(pos+11, end - pos - 11);
   return SUCCESS;
}
#endif

// ******************************************************************************
// ******************************************************************************

int C_nitf_obj::convert_date(CString raw_date, CString &date) 
{
   CString year, mon, day, hour, min, sec, monstr;
   int imon, ti;

   date = raw_date;

   if (raw_date.GetLength() < 14)
      return FAILURE;

   year = raw_date.Left(4);
   ti = atoi(year);
   if ((ti < 1950) || (ti > 2050))
      return FAILURE;
   mon = raw_date.Mid(4, 2);
   ti = atoi(mon);
   if ((ti < 1) || (ti > 12))
      return FAILURE;
   day = raw_date.Mid(6,2);
   ti = atoi(day);
   if ((ti < 1) || (ti > 31))
      return FAILURE;
   hour = raw_date.Mid(8,2);
   ti = atoi(hour);
   if ((ti < 0) || (ti > 23))
      return FAILURE;
   min = raw_date.Mid(10,2);
   ti = atoi(min);
   if ((ti < 0) || (ti > 59))
      return FAILURE;
   sec = raw_date.Mid(12,2);
   ti = atoi(sec);
   if ((ti < 0) || (ti > 59))
      return FAILURE;

   imon = atoi(mon);
   switch(imon)
   {
   case 1: monstr = "January"; break;
   case 2: monstr = "February"; break;
   case 3: monstr = "March"; break;
   case 4: monstr = "April"; break;
   case 5: monstr = "May"; break;
   case 6: monstr = "June"; break;
   case 7: monstr = "July"; break;
   case 8: monstr = "August"; break;
   case 9: monstr = "September"; break;
   case 10: monstr = "October"; break;
   case 11: monstr = "November"; break;
   case 12: monstr = "December"; break;
   default:
      date = raw_date;
      return FAILURE;
      break;
   }

   date = day + " " + monstr + " " + year + "   " + hour + ":" + min + ":" + sec + "Z";

   return SUCCESS;
}
// end of convert_date

// ******************************************************************************
// ******************************************************************************

// m_pathname must be valid

int C_nitf_obj::load_image_file() 
{
   BOOL found = FALSE;
   double ul_lat, ul_lon, ur_lat, ur_lon, lr_lat, lr_lon, ll_lat, ll_lon;
   INT iErr;
   CComBSTR ccbsErrorMsg;
   int len, pos;

   ul_lat = ul_lon = ur_lat = ur_lon = lr_lat = lr_lon = ll_lat =  ll_lon = 0.0;
   // get the selected routes
   len = m_csFilespec.GetLength();
   pos = m_csFilespec.ReverseFind('\\');
   if (pos >= 0)
      m_filename = m_csFilespec.Right(len-pos-1);

   ImageLibProxyLoad( &iErr, &ccbsErrorMsg );
   if ( iErr == 0 )
   {
      m_pImageLib->set_callback( &m_cbImageLibCallback1 );
      m_image_loaded = TRUE;
      m_pImageLib->get_corner_coords(&ul_lat, &ul_lon, &ur_lat, &ur_lon, 
         &lr_lat, &lr_lon, &ll_lat, &ll_lon, &iErr);
      if ( iErr == 0 )
      {
         if ((ll_lat != 0.0) || (ll_lon != 0.0) || (ur_lat != 0.0) || (ur_lon != 0.0))
         {
            m_has_geo = TRUE;
            m_bnd_ul.lat = ul_lat;
            m_bnd_ul.lon = ul_lon;
            m_bnd_ur.lat = ur_lat;
            m_bnd_ur.lon = ur_lon;
            m_bnd_lr.lat = lr_lat;
            m_bnd_lr.lon = lr_lon;
            m_bnd_ll.lat = ll_lat;
            m_bnd_ll.lon = ll_lon;
            m_lat_per_pix = (ul_lat - ll_lat) / (double) m_height;
         }

         CComBSTR ccbsInfo;
         m_pImageLib->get_info( &ccbsInfo, &iErr, &ccbsErrorMsg );
         if ( iErr == 0 )
         {
            AfxBSTR2CString( &m_info, ccbsInfo );

            CNitfUtil util;
            util.extract_info_data( m_info, _T("File Date"), m_raw_date );
            convert_date( m_raw_date, m_img_date );

            CString csData;
            util.extract_info_data( m_info, "Multispectral", csData );
            m_is_multispectral = 0 == csData.CompareNoCase( _T("YES") );
            if ( !m_is_multispectral )
            {
               util.extract_info_data( m_info, "Category", csData );
               m_is_multispectral = 0 == csData.CompareNoCase( _T("Multispectral") );
            }

            util.extract_info_data( m_info, _T("Color Image"), csData );
            m_is_color = 0 == csData.CompareNoCase( _T("YES") );

         }
      }
   }

   return SUCCESS;
}
// load_image_file

// ******************************************************************************
// ******************************************************************************
#if 0
// Equal operator   
C_nitf_obj &C_nitf_obj::operator=(C_nitf_obj &obj) 
{
   int k;

   m_bnd_ll.lat = obj.m_bnd_ll.lat;
   m_bnd_ll.lon = obj.m_bnd_ll.lon;
   m_bnd_ur.lat = obj.m_bnd_ur.lat;
   m_bnd_ur.lon = obj.m_bnd_ur.lon;
   m_tip_info = obj.m_tip_info;
   m_help_text = obj.m_help_text;
   m_label = obj.m_label;
   m_eFileType = obj.m_eFileType;
   m_rect = obj.m_rect;

   m_selected = obj.m_selected;
   m_filename = obj.m_filename;
   m_csFilespec = obj.m_csFilespec;
   m_iImageInFile = obj.m_iImageInFile;

   m_width = obj.m_width;
   m_height = obj.m_height;
   m_ctr_x = obj.m_ctr_x;
   m_ctr_y = obj.m_ctr_y;
   m_lat_per_pix = obj.m_lat_per_pix;
   m_img_date = obj.m_img_date;
   m_has_geo = obj.m_has_geo;
   m_eFillControl = obj.m_eFillControl;
   m_raw_date = obj.m_raw_date;
   m_info = obj.m_info;

   for (k=0; k<4; k++)
   {
      m_pt[k].x = obj.m_pt[k].x;
      m_pt[k].y = obj.m_pt[k].y;
   }

   return *this;
}
// end of copy operator
#endif
// **************************************************************************
// **************************************************************************

void C_nitf_obj::make_translucent(int width, int height, BYTE *img)  // img is assumed to consist of RGB triples
{
   int x, y, k;

   k = 0;
   for (y=0; y<height; y++)
   {
      for (x=0; x<width; x++)
      {
         if ((y % 4) == 0)
         {
            if ((x % 4) == 0)
            {
               img[k+0] = 0;
               img[k+1] = 0;
               img[k+2] = 0;
            }
         }
         k += 3;
      }
   }
}

void C_nitf_obj::auto_contrast()
{
   int k, total, total2, curval;
   unsigned int thresh;

   if (m_freq_lum == NULL)
      return;

   // find the maximum value
   total = 0;
   for (k=0; k<256; k++)
   {
      curval = m_freq_lum[k];
      total += curval;
   }

   thresh = 100;

   // find the lowest value above threshold
   k = 1;
   while ((k<256) && (m_freq_lum[k] < thresh))
      k++;
   m_contr_minval = k;
   k = 255;
   while ((k>=0) && (m_freq_lum[k] < thresh))
      k--;
   m_contr_maxval = k;

   // apply contrast stretch as a test
   // find center value
   total2 = total / 2;
   total = 0;
   k = 0;
   while (total < total2)
   {
      curval = m_freq_lum[k];
      total += curval;
      k++;
   }

   m_contr_ctrval = k - 1;
}
// end of auto_contrast

//
// ImageLibProxyLoad() - Proxy implementation of ImageLib::Load()
//
HRESULT C_nitf_obj::ImageLibProxyLoad( PINT piErr, BSTR* pbsErrMsg )
{
   InitImageLib();   // We will always need a local copy

   HRESULT hr;
   CComBSTR ccbsErrMsg;
   m_eImageLibLoadStatus = IMAGELIB_LOAD_ERROR; // Assume failure

   _bstr_t bstr = m_pxdDisplayParams->xml;
   if ( bstr.length() > 0 )
   {
      hr = m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_LOAD_DISPLAY_PARAMS_XML,
         bstr, VARIANT_NULL, NULL, NULL, piErr, &ccbsErrMsg );
      if ( hr != S_OK || *piErr != SUCCESS )
         goto FailExit;
   }

   if ( m_eSourceLocationType == SOURCE_LOCATION_TYPE_TILE_CACHE )
   {
      m_eImageLibLoadStatus = IMAGELIB_LOADED_TILE_CACHE;
      *piErr = SUCCESS;
      return S_OK;
   }

   // No callback2 unless JPIP
   ccbsErrMsg.Empty();
   hr = m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_SET_CALLBACK2,
         VARIANT_NULL, VARIANT_NULL, NULL, NULL, piErr, &ccbsErrMsg );
   if ( FAILED( hr ) )
      goto FailExit;

   hr = m_pImageLib->load( _bstr_t( m_csFilespec ), &m_width, &m_height, piErr, pbsErrMsg );
   if ( hr != S_OK )
   {
      if ( *piErr == IMAGELIB_ERROR_SOURCE_BUSY )
      {
         C_nitf_ovl::s_pNITFOvl->m_bImageSourceBusy =
            C_nitf_ovl::s_pNITFOvl->m_bIncompleteDraw = TRUE;
         m_eImageLibLoadStatus = IMAGELIB_LOAD_DELAYED;     // Server is busy
      }
      goto Exit;
   }
      
   if ( *piErr != SUCCESS )
      goto Exit;

   ccbsErrMsg.Empty();
   if ( m_eSourceLocationType == SOURCE_LOCATION_TYPE_JPIP_SERVER )
   {
      m_pImageLib->set_callback( NULL );     // No compute progress for JPIP images
 
      hr = m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_SET_CALLBACK2,
         _variant_t( m_cbJPIPCallback.GetControllingUnknown() ), 2000,
         NULL, NULL, piErr, &ccbsErrMsg );
      if ( FAILED( hr ) )
         goto FailExit;

#if 1 || !defined _DEBUG || defined ALL_TESTS      // Normally allow partial input
      hr = m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_INCOMPLETE_OUTPUT,
         VARIANT_TRUE, VARIANT_NULL, NULL, NULL, piErr, &ccbsErrMsg );
      if ( FAILED( hr ) )
         goto FailExit;
#endif
   }
   else
   {
      _variant_t varEnableSubimages = C_nitf_ovl::s_pNITFOvl->m_bShowSecondaryImages
         ? VARIANT_TRUE : VARIANT_FALSE;
      hr = m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_SECONDARY_IMAGES,
         varEnableSubimages, VARIANT_NULL, NULL, NULL, piErr, &ccbsErrMsg );
      if ( FAILED( hr ) )
         goto FailExit;

      ccbsErrMsg.Empty();
      hr = m_pImageLib->SpecialFunction( IMAGELIB_SPEC_FUNC_ENABLE_SECONDARY_GRAPHICS,
         varEnableSubimages, VARIANT_NULL, NULL, NULL, piErr, &ccbsErrMsg );
   }
   if ( FAILED( hr ) )
   {
FailExit:
      *pbsErrMsg = ccbsErrMsg.Detach();
   }
   else
   {
      hr = S_OK;        // In case S_FALSE
      m_eImageLibLoadStatus = IMAGELIB_LOADED_FILE;
   }

Exit:
   return hr;

}  // End if ImageLibProxyLoad()


//
// Close()
//
VOID C_nitf_obj::Close()
{
   m_pImageLib = NULL;
   m_apbRGBBuffer.reset( NULL );
   m_apbOldImg.reset( NULL );
   m_eImageLibLoadStatus = IMAGELIB_NOT_LOADED;
}


//
// DrawLocaterArrow() - Draw arrow at center of view
//
VOID C_nitf_obj::DrawLocatorArrow( ActiveMap* pMap, INT cWidth, INT cHeight, DOUBLE dAngle, INT iColor )
{
   // Arrow length, 20% of max screen dimension
   DOUBLE
      dLen = ( 0.1 / 50.0 ) * ( ( cWidth > cHeight ) ? cWidth : cHeight ),
      dXCen = 0.5 * cWidth, dYCen = 0.5 * cHeight,
      dLenCos = dLen * cos( dAngle ), dLenSin = dLen * sin( dAngle );

   static const POINT ptArrowVertices[] = // +25, -75 units long
   {
      { 25,    0 },
      { 5,     FLASH_ARROW_ARROW_WIDTH / 2 },
      { 15,    FLASH_ARROW_ARROW_WIDTH / 8 },
      { -75,   FLASH_ARROW_ARROW_WIDTH / 8 },
      { -70,   0 },
      { -75,   -FLASH_ARROW_ARROW_WIDTH / 8 },
      { 15,    -FLASH_ARROW_ARROW_WIDTH / 8 },
      { 5,     -FLASH_ARROW_ARROW_WIDTH / 2 },
      { 25 ,   0 }
   };
   static const INT cArrowVertices = sizeof(ptArrowVertices) / sizeof(ptArrowVertices[0] );

   POINT pts[ cArrowVertices ];
   for ( INT k = 0; k < cArrowVertices; k++ )
   {
      pts[ k ].x = (LONG) ( dXCen
         + ( ptArrowVertices[ k ].x * dLenCos ) - ( ptArrowVertices[ k ].y * dLenSin ) );
      pts[ k ].y = (LONG) ( dYCen
         + ( ptArrowVertices[ k ].x * dLenSin ) + ( ptArrowVertices[ k ].y * dLenCos ) );
   }

   CFvwUtil::get_instance()->polyline( pMap->get_CDC(), pts, cArrowVertices, iColor, FLASH_ARROW_LINE_WIDTH );
}

// end of get_imagelib_histogram

