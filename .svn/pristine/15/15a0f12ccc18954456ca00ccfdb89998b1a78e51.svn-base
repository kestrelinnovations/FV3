// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// nitf_sql.cpp -- Nitf file overlay SQL query



#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "file.h"
#include "Param.h"
//#include "geo_tool.h"
#include "..\GetObjPr.h"
#include "..\MapViewTabsCtrl.h"
#include "map.h"
#define NITF_UTILITY_CLASSES
#include "nitf.h"
#include "nitf_util.h"
#include "wm_user.h"
using namespace NITFImagery;
#include "NitfOvlOptnPages.h"
#include "..\MapEngineCOM.h"
#include "NITFUtilities.h"


//#define IMAGE_DISPLAY_INFO_TABLE L"ImageDisplayInfo"
#define FLASH_DISPLAY_FIELD L"( COALESCE( di.TempDisplay, 0 ) & 16 )"  /* Assume 16 = 0x10 = NITF_TEMP_DISPLAY_FLASH_MASK */

static const BSTR
   GEO_QUERY_FIELD_NAMES =
      //L"Ranking,\n"         // Ranking in built into the ImageAndDisplay query as column 0
      //L"EffectivePriority"  // Ditto priority as column 1
      L"  fi.Filespec AS Filespec,\n"
      L"  COALESCE( fi.VolumeLabel, '' ) AS VolumeLabel,\n"
      L"  ImageType,\n"
      L"  LocationType,\n"
      L"  fi.ImageInFile AS ImageInFile,\n"
      L"  fi.FileTitle AS FileTitle,\n"
      L"  fi.ImageTitle AS ImageTitle,\n"
      L"  ULLat, ULLong,\n"
      L"  URLat, URLong,\n"
      L"  LRLat, LRLong,\n"
      L"  LLLat, LLLong,\n"
      L"  MinLat, MinLong, MaxLat, MaxLong,\n"
      L"  ImageWidth, ImageHeight,\n"
      L"  PixelWidthDeg, PixelHeightDeg,\n"
      L"  ImageUTCDateTime,\n"
      L"  MultibandCode,\n"
      L"  COALESCE( di.FillControl, 0 ) AS Fill,\n"               // Assume 0 == FILL_CONTROL_OUTLINE_FRAME
      L"  COALESCE( di.Opacity, 100 ) AS OpacityVal,\n"           // Opaque by default
      L"  COALESCE( di.EqualizationMode, +2 ) AS Equalization,\n" // Assume +2 = EQUALIZATION_MODE_AUTO_LINEAR
      L"  COALESCE( fi.PixelSizeMeters, 100.0 ) AS PixelSize,\n"  // Draw solid frame as default
      L"  COALESCE( di.MinDisplayLevel, 0 ) AS MinLevel,\n"
      L"  COALESCE( di.MidDisplayLevel, 128 ) AS MidLevel,\n"
      L"  COALESCE( di.MaxDisplayLevel, 255 ) AS MaxLevel,\n"
      L"  COALESCE( di.DisplayParamsXML, '' ) AS DisplayParamsXML,\n"
      L"  COALESCE( di.TempDisplay, 1 ) AS TempDisplay,\n"        // Default to no temp display
                                                                  // Assume 0x01 = NITF_TEMP_DISPLAY_NORMAL_MASK
      L"  " FLASH_DISPLAY_FIELD L" AS FlashDisplay,\n"
      L"  COALESCE( di.ID, 0 ) AS DisplayInfoID,\n",
   GEO_QUERY_FILENAME_FIELD_FOR_SQLSERVER =
      L"  CASE\n"
      L"    WHEN LocationType >= 100 THEN '(r)' + fi.FileName\n"
      L"    ELSE fi.FileName\n"
      L"  END AS FileName\n",
   GEO_QUERY_FILENAME_FIELD_FOR_POSTGRES =
      L"  CASE\n"
      L"    WHEN LocationType >= 100 THEN '(r)' || fi.FileName\n"
      L"    ELSE fi.FileName\n"
      L"  END AS FileName\n",

   OR_WHERE_NEW_FLASH_CLAUSE =
      L"  ( " FLASH_DISPLAY_FIELD L" ) != 0\n",

// Additional geo-query qualifiers and ordering
   GEO_QUERY_EXTRA_WHERE_AND_ORDER_BY_CLAUSE =
      //L"\tAND ImageType = 'NITF'\n"
      L")\n"            // Required when using OR WHERE query form
      L"ORDER BY\n"
      L"  FlashDisplay,\n"
      L"  TempDisplay,\n"
      L"  EffectivePriority,\n"
      L"  ImageUTCDateTime,\n"
      L"  fi.Filespec,\n"
      L"  Ranking\n",

   NEW_IMAGE_QUERY_FIELD_NAMES =
      //L"Ranking,\n"         // Ranking in built into the ImageAndDisplay query as column 0
      //L"EffectivePriority"  // Ditto priority as column 1
      L"  BoolGeoAvailable,\n"
      L"  ImageType,\n"
      L"  LocationType,\n"
      L"  fi.ImageInFile AS ImageInFile,\n"
      L"  ULLat, ULLong,\n"
      L"  LRLat, LRLong\n";

//-----------------------------------------------------------------------------
CString VtToString(const _variant_t& v)
{
   if (v.vt == VT_BSTR)
      return v.bstrVal;

   return "";
}

#import "NITFDBServer.tlb" no_namespace named_guids

INITFDBAgentPtr C_nitf_ovl::s_pNITFDBAgent = NULL;
enum ::EnumImageDBServerType C_nitf_ovl::s_eDBServerType = IMAGEDBSERVER_UNKNOWN_SERVER_TYPE;

//-----------------------------------------------------------------------------
int C_nitf_ovl::open_nitf_database()
{
   // don't load twice
   if ( s_pNITFDBAgent != NULL )
      return SUCCESS;

   HRESULT hr = E_FAIL;

   try
   {
      HRESULT hr = s_pNITFDBAgent.CreateInstance( CLSID_NITFDBAgent,
         NULL, CLSCTX_INPROC_SERVER ); 
      if ( SUCCEEDED( hr ) )
      {
         #ifdef SQL_DEBUG
            hr = s_pNITFDBAgent->raw_Open( 
               NULL, L"C:\\Temp\\FV_NITF_DBAgent.log", NULL, NULL );
         #else
            hr = s_pNITFDBAgent->raw_Open( 
               NULL, NULL, NULL, NULL );  // Default connect string
         #endif
         if ( SUCCEEDED( hr ) )
         {
            s_eDBServerType = static_cast< enum ::EnumImageDBServerType >
               ( (LONG) s_pNITFDBAgent->SpecialFunction( IMAGEDBSERVER_SPECFUNC_GET_DB_SERVER_TYPE, L"" ) );
            return SUCCESS;
         }

         close_nitf_database();
      }  
   }
   catch( _com_error &e)
   {
      CString sDescr = (char*)e.Description();
      CString sMsg = "INITFDBAgent - " + sDescr;
      // ERR_report( sMsg );
   }   

   return FAILURE;
}

//-----------------------------------------------------------------------------
void C_nitf_ovl::close_nitf_database()
{
   if ( s_pNITFDBAgent )
   {
      s_pNITFDBAgent->raw_Close();
      s_pNITFDBAgent = NULL;
   }
}


//-----------------------------------------------------------------------------
int C_nitf_ovl::make_topmost_or_bottommost( const C_nitf_obj* pObj, BOOL bTopmost )
{
   return make_topmost_or_bottommost( _bstr_t( pObj->m_csFilespec ), pObj->m_iImageInFile, bTopmost );
}

int C_nitf_ovl::make_topmost_or_bottommost( const _bstr_t& bstrFilespec, INT iImageInFile, BOOL bTopmost )
{
   s_pNITFDBAgent->SendImageToTopOrBottom(
         bstrFilespec, iImageInFile,
         L"Generic", L"Generic",
         (long) bTopmost );
 
   m_bReload = TRUE;
   OVL_get_overlay_manager()->invalidate_all( FALSE );
   return SUCCESS;
}


//-----------------------------------------------------------------------------
BOOL C_nitf_ovl::GetDefaultableDisplayAdjustments()
{
   // Get the current user name by looking at the %USERPROFILE% environment variable
   static const INT PROFILE_BUF_LEN = _MAX_PATH + 100;
   WCHAR wchrProfile[ PROFILE_BUF_LEN ];

   // Now get the actual string
   INT n = ExpandEnvironmentStringsW( L"%USERPROFILE%", wchrProfile, PROFILE_BUF_LEN );
   ATLASSERT( n <= PROFILE_BUF_LEN );

   LPWCH pwchUserName = wcsrchr( wchrProfile, L'\\' );   // Look for last backslash
   if ( pwchUserName == NULL )
   {
      ATLASSERT( FALSE && "Missing currrent user name" );
      return FALSE;
   }

   wostringstream wssSQLCmd;

   // If not SQL Server, make a temp table rather than a filterable recordset
   if ( s_eDBServerType != IMAGEDBSERVER_SQLSERVER_SERVER_TYPE )
   {
      static const BSTR bsCreateTempTable =
         L"CREATE TEMP TABLE DefaultableDisplayAdjustments\n"
         L"(\n"
         L"  Filespec varchar(400),\n"
         L"  ImageInFile smallint,\n"
         L"  Fill smallint\n"
         L")\n";
      s_pNITFDBAgent->ExecuteSQL(  bsCreateTempTable );

      wssSQLCmd <<
         L"INSERT INTO DefaultableDisplayAdjustments\n"
         L"(\n"
         L"  Filespec,\n"
         L"  ImageInFile\n,"
         L"  Fill\n"
         L")\n";
   }

   wssSQLCmd <<
      L"SELECT\n"
      L"  Filespec,\n"
      L"  ImageInFile,\n"
      L"  COALESCE( FillControl, 0 ) AS Fill\n"
      L"FROM " <<
         ( s_eDBServerType == IMAGEDBSERVER_POSTGRES_SERVER_TYPE ? NITF_POSTGRES_TBL_PREFIX : L"" ) <<
         NITF_IMAGE_DISPLAY_INFO_TABLE L"\n"
      L"WHERE UserName = '" << pwchUserName + 1 << L"'\n"
      L"  AND (\n"
      L"      COALESCE( Priority, 0 ) != 0\n"
      L"    OR COALESCE( FillControl, 0 ) != 0\n"
      L"    OR COALESCE( Opacity, 100 ) != 100\n"
      L"    OR COALESCE( EqualizationMode, +2 ) != +2\n"
      L"    OR COALESCE( MinDisplayLevel, 0 ) != 0\n"
      L"    OR COALESCE( MidDisplayLevel, 128 ) != 128\n"
      L"    OR COALESCE( MaxDisplayLevel, 255 ) != 255\n"
      L"  )\n" << ends;

   if ( s_eDBServerType != IMAGEDBSERVER_SQLSERVER_SERVER_TYPE )
   {
      m_sprsDfltableDsplyAdjs = NULL;
      return SUCCEEDED( ExecuteSQLStream( wssSQLCmd ) );
   }

   if ( SUCCEEDED( QuerySQLStream( wssSQLCmd, (_Recordset*&) m_sprsDfltableDsplyAdjs ) ) )
      return TRUE;

   m_sprsDfltableDsplyAdjs = NULL;
   return FALSE;
}


//-----------------------------------------------------------------------------
BOOL C_nitf_ovl::CanDisplayAdjustmentsBeDefaulted( const C_nitf_obj* const pObj )
{
   wstring wstrFilespec = CT2W( pObj->m_csFilespec );
   QuoteDBString( wstrFilespec );

   wostringstream wssSQL;
   wssSQL <<
      L"ImageInFile = " << pObj->m_iImageInFile << L"\n"
      L"AND Filespec =\n" <<
      L"  " << WLCString( wstrFilespec ) << L"\n" << ends;

   FilterDfltableDsplyAdjs( wssSQL.str().c_str() );

   return !m_sprsDfltableDsplyAdjs->adoEOF;
}


//-----------------------------------------------------------------------------
VOID C_nitf_ovl::FilterDfltableDsplyAdjs( LPCWSTR pwszFilterString )
{
   if ( s_eDBServerType == IMAGEDBSERVER_SQLSERVER_SERVER_TYPE )
      m_sprsDfltableDsplyAdjs->Filter =
         pwszFilterString == NULL ? L"" : pwszFilterString;

   else
   {
      wostringstream wssSQL;
      wssSQL <<
         L"SELECT * From DefaultableDisplayAdjustments\n";
      
      if ( pwszFilterString != NULL )
         wssSQL <<
            L"WHERE " << pwszFilterString;
      
      wssSQL <<
         L"\n" << ends;

      THROW_IF_NOT_OK(
         QuerySQLStream( wssSQL, (_Recordset*&) m_sprsDfltableDsplyAdjs ) );
   }
}


//-----------------------------------------------------------------------------
VOID C_nitf_ovl::ResetAllImageDisplayAdjustments()
{
   if ( s_pNITFOvl != NULL )
   {
      s_pNITFOvl->m_eImagesOnlyNoEditor = IMAGES_ONLY_NO_EDITOR_UNKNOWN;
      s_pNITFOvl->m_eQueryFilterDisplay = QUERY_FILTER_DISPLAY_UNSPECIFIED;
   }
   PRM_delete_registry_section( "NitfFile" );      // Legacy parameters

   ResetImageDisplayAdjustments( NULL, NULL );
}  // End of C_nitf_ovl::ResetAllImageDisplayAdjustments()


//-----------------------------------------------------------------------------
VOID C_nitf_ovl::ResetImageDisplayAdjustments( ViewMapProj* pMap, C_icon* pIcon )
{
   C_nitf_obj* pObj = static_cast< C_nitf_obj* >( pIcon );

   _bstr_t bstrFilespec( L"" );
   INT iImageInFile = 0;
   if ( pObj != NULL )
   {
      bstrFilespec = pObj->m_csFilespec;
      iImageInFile = pObj->m_iImageInFile;
   }

   _bstr_t bstrNullParams =
      L"Priority = 0,\n"
      L"Opacity = NULL,\n"
      L"EqualizationMode = NULL,\n"
      L"MinDisplayLevel = NULL,\n"
      L"MidDisplayLevel = NULL,\n"
      L"MaxDisplayLevel = NULL,\n"
      L"DisplayParamsXML = NULL,\n"
      L"FillControl = NULL";

   if ( s_bEditOn )
      bstrNullParams += L"\n";
   else
      bstrNullParams += L",\n"
         L"TempDisplay = NULL\n";

   if ( SUCCESS == open_nitf_database() )
   {
      s_pNITFDBAgent->UpdateImageDisplayInfoRecord(
         bstrFilespec, iImageInFile,
         L"Generic", L"Generic",
         bstrNullParams,
         (long) TRUE );    // [in] long bNoCreateRecord,

      static const BSTR bsClearEmptyDisplay =
         L"DELETE FROM " NITF_DB_TABLE_PREFIX_PLACEHOLDER NITF_IMAGE_DISPLAY_INFO_TABLE L"\n"
         L"WHERE\n"
         L"    Priority = 0\n"
         L"  AND Opacity IS NULL\n"
         L"  AND EqualizationMode IS NULL\n"
         L"  AND MinDisplayLevel IS NULL\n"
         L"  AND MidDisplayLevel IS NULL\n"
         L"  AND MaxDisplayLevel IS NULL\n"
         L"  AND DisplayParamsXML IS NULL\n"
         L"  AND FillControl IS NULL\n"
         L"  AND ( COALESCE( TempDisplay, 0 ) & -2 ) = 0\n";   // -2 = 0xFFFFFFFE
      s_pNITFDBAgent->ExecuteSQL( bsClearEmptyDisplay );
   }

   if ( s_pNITFOvl != NULL )
   {
      s_pNITFOvl->m_bReload = TRUE;
      OVL_get_overlay_manager()->invalidate_all( FALSE );
   }

}  // End of C_nitf_ovl::ResetImageDisplayAdjustments()


//-----------------------------------------------------------------------------
int C_nitf_ovl::set_file_fill_control( const CString& filespec, INT iImageInFile,
                                          EnumFillControl eFillControl )
{
   wostringstream wss;
   wss << L"FillControl = " << (LONG) eFillControl << ends;

   s_pNITFDBAgent->UpdateImageDisplayInfoRecord(
                     _bstr_t( filespec ), iImageInFile,
                     L"Generic", L"Generic",
                     _bstr_t( wss.str().c_str() ),
                     (long) FALSE );   // [in] long bNoCreateRecord,

   return SUCCESS;
}



//-----------------------------------------------------------------------------
int C_nitf_ovl::set_file_display_params( const CString& filespec, INT iImageInFile,
                                          int eq_mode, int opacity,
                                          int min_level, int mid_level, int max_level,
                                          const _bstr_t& bstrDisplayParamsXML )
{
   // eq_mode        - EqualizationMode
   // opacity        - Opacity
   // min_level      - MinDisplayLevel
   // mid_level      - MidDisplayLevel
   // max_level      - MaxDisplayLevel

   wostringstream wssSetList;
   BOOL bOnce = FALSE;

   if ( eq_mode >= 0 )
   {
      wssSetList << L"EqualizationMode = " << eq_mode;
      bOnce = TRUE;
   }

   if ( opacity >= 0 && opacity <= 255 )
   {
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wssSetList << L"Opacity = " << (INT) ( ( opacity / 2.55 ) + 0.5 );
   }

   if ( min_level >= 0 )
   {     
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wssSetList << L"MinDisplayLevel = " << min_level;
   }

   if ( mid_level >= 0 )
   {
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wssSetList << L"MidDisplayLevel = " << mid_level;
   }

   if ( max_level >= 0 )
   {
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wssSetList << L"MaxDisplayLevel = " << max_level;
   }

   if ( bstrDisplayParamsXML.length() > 0 )
   {
      if ( bOnce )
         wssSetList << L",\n";
      else
         bOnce = TRUE;
      wstring wstr = bstrDisplayParamsXML;
      QuoteDBString( wstr );           // Enclose in single quotes and double embedded ones
      wssSetList << L"DisplayParamsXML = " << wstr;
   }

   try
   {
      if ( bOnce )
      {
         wssSetList << L"\n";
         s_pNITFDBAgent->UpdateImageDisplayInfoRecord(
                  _bstr_t( filespec ), iImageInFile,
                  L"Generic", L"Generic",
                  wssSetList.str().c_str(),
                  (long) FALSE );   // [in] long bNoCreateRecord
      }
      return SUCCESS;
   
   } catch (...) {}

   return FAILURE;
}


//-----------------------------------------------------------------------------
int C_nitf_ovl::select_nitf_records( ActiveMap& active_map )
{
   POSITION pos = m_objlist.GetHeadPosition();
   while ( pos != NULL )
      m_objlist.GetNext( pos )->m_priority = INT_MIN;      // Mark tentatively unreferenced
   
   d_geo_t  gptLL1, gptUR1, gptLL2 = { 0.0, 0.0 }, gptUR2 = { 0.0, 0.0 };
   double   degPerPixLat, degPerPixLon;
   
   active_map.get_vmap_bounds( &gptLL1, &gptUR1 );
   active_map.get_vmap_degrees_per_pixel( &degPerPixLat, &degPerPixLon ); 
   
   try
   {
      BOOL bOnScreenBkgdThreadActive = FALSE;  // Don't need any aborts yet

      // If crosses dateline, make two regions
      if ( gptLL1.lon > gptUR1.lon )
      {
         gptLL2.lat = gptLL1.lat;
         gptUR2.lat = gptUR1.lat;
         gptLL2.lon = -180.0;
         gptUR2.lon = gptUR1.lon;
         gptUR1.lon = +180.0;
      }

      if ( m_eImagesOnlyNoEditor == IMAGES_ONLY_NO_EDITOR_UNKNOWN )
      {
         m_eImagesOnlyNoEditor = IMAGES_ONLY_NO_EDITOR_FALSE;  // Assume not enabled 
         CString cs = PRM_get_registry_string( "NitfFile", "ImagesOnlyNoEditor", "N" );
         if ( !cs.CompareNoCase( "Y" ) )
            m_eImagesOnlyNoEditor = IMAGES_ONLY_NO_EDITOR_PRETRUE;
      }

      wostringstream wss;
      UINT mDisplayMask = m_mTempDisplayMask;   // Assume no mods
      do
      {
         // If query tool has not been run at least once
         if ( m_mTempDisplayMask == NITF_TEMP_DISPLAY_NORMAL_MASK )
         {
            if ( m_eImagesOnlyNoEditor != IMAGES_ONLY_NO_EDITOR_TRUE )
               break;
         }
         else  // Editor run once
         {
            if ( m_eImagesOnlyNoEditor == IMAGES_ONLY_NO_EDITOR_PRETRUE )
               m_eImagesOnlyNoEditor = IMAGES_ONLY_NO_EDITOR_TRUE;

            if ( m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_UNSPECIFIED )
            {
               m_eQueryFilterDisplay = QUERY_FILTER_DISPLAY_FALSE;  // Assume not enabled 
               CString cs = PRM_get_registry_string( "NitfFile", "OnlyFilteredObjects", "N" );
               if ( !cs.CompareNoCase( "Y" ) )
                  m_eQueryFilterDisplay = QUERY_FILTER_DISPLAY_TRUE;
            }

            wss <<
               L"\tAND ( COALESCE( di.TempDisplay, 1 ) & " <<
                  ( ( m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_TRUE )
                  ? m_mTempDisplayMask       // Temp display mask if filter is on
                  : ( mDisplayMask = s_bEditOn
                     ? m_mTempDisplayMask | NITF_TEMP_DISPLAY_NORMAL_MASK  // Normal + flags if editor on
                     : ( NITF_TEMP_DISPLAY_NORMAL_MASK
                        | NITF_TEMP_DISPLAY_KEEP_TEMP_MASK
                        | NITF_TEMP_DISPLAY_TEMP_MASK ) ) ) <<
                  L" ) != 0\n";

            if ( m_eImagesOnlyNoEditor != IMAGES_ONLY_NO_EDITOR_TRUE || s_bEditOn
               || m_eQueryFilterDisplay == QUERY_FILTER_DISPLAY_TRUE )
               break;
         }
         wss <<
            L"\tAND COALESCE( di.FillControl, " << FILL_CONTROL_OUTLINE_FRAME <<
               L" ) != " << FILL_CONTROL_OUTLINE_FRAME << L"\n";
      } while ( FALSE );

      _bstr_t bstrExtraWhereAndOrderByClause = wss.str().c_str();
      bstrExtraWhereAndOrderByClause += GEO_QUERY_EXTRA_WHERE_AND_ORDER_BY_CLAUSE;

      _RecordsetPtr pRecordset;
      ATLASSERT( NITF_TEMP_DISPLAY_FLASH_MASK == 0x0010 );  // See GEO_QUERY_FIELD_NAMES
      ATLASSERT( NITF_TEMP_DISPLAY_NORMAL_MASK == 0x0001 );

      #if defined TIMING_TEST_1 && defined _DEBUG
         DWORD dwTicks = GetTickCount();
      #endif

      HRESULT hr = s_pNITFDBAgent->raw_QueryGeoLimitedImageAndDisplayInfoEx(
         gptLL1.lat, gptLL1.lon, gptUR1.lat, gptUR1.lon,
         gptLL2.lat, gptLL2.lon, gptUR2.lat, gptUR2.lon,    // May be null
         _bstr_t( GEO_QUERY_FIELD_NAMES )       // Fields to fetch
            + ( ( s_eDBServerType == IMAGEDBSERVER_SQLSERVER_SERVER_TYPE )
            ? GEO_QUERY_FILENAME_FIELD_FOR_SQLSERVER
            : GEO_QUERY_FILENAME_FIELD_FOR_POSTGRES ),
         TRUE,                                  // Ignore other users
         L"Generic",                            // ViewDisplayName  MATT_TODO --> what is default view name ?
         bstrExtraWhereAndOrderByClause,        // Extra filter and/or ORDER BY
         OR_WHERE_NEW_FLASH_CLAUSE,             // OR any image with flash enabled
         &pRecordset );

      #if defined TIMING_TEST_1 && defined _DEBUG
         ATLTRACE( _T("Query time = %d ms\n"), (INT) ( GetTickCount() - dwTicks ) );
      #endif

      if ( SUCCEEDED( hr ) )
      {
         #ifdef SQL_DEBUG
         long limit = pRecordset->GetFields()->Count;
         ATLTRACE( _T("Total number of records = %d, total number of fields = %d\n"),
            pRecordset->RecordCount, limit );
         for ( long i = 0; i < limit; i++)
            ATLTRACE(_T("%d: %s\n"), i+1, 
            (LPTSTR) pRecordset->GetFields()->Item[i]->Name );
         #endif
         
         vector< NITFObjPtr > anopSelectedObjs;
         while ( !pRecordset->adoEOF )
         {
            try
            {
               NITFObjPtr pObj;

               CString csFilespec =
                  VtToString( pRecordset->adoFields->GetItem( L"Filespec" )->Value );
               INT iImageInFile =
                  (long) pRecordset->adoFields->GetItem( L"ImageInFile" )->Value;
               CString csVolumeLabel =
                  VtToString( pRecordset->adoFields->GetItem( L"VolumeLabel" )->Value );

               // See if we already have this file
               POSITION pos = m_objlist.GetHeadPosition();
               while ( pos != NULL )
               { 
                  POSITION pos1 = pos;
                  pObj = m_objlist.GetNext( pos );
                  if ( csFilespec.CompareNoCase( pObj->m_csFilespec ) == 0
                     && iImageInFile == pObj->m_iImageInFile
                     && csVolumeLabel.CompareNoCase( pObj->m_csVolumeLabel ) == 0 )
                  {
                     m_objlist.RemoveAt( pos1 );
                     if ( pObj->IsBusy() )
                        bOnScreenBkgdThreadActive = TRUE;  // Need to insure access to builder
                     goto GotObj;
                  }
               }
               pObj = new C_nitf_obj( this );
               pObj->m_csFilespec = csFilespec;
               pObj->m_iImageInFile = iImageInFile;
               pObj->m_csVolumeLabel = csVolumeLabel;
GotObj:

               pObj->m_filename =
                  VtToString( pRecordset->adoFields->GetItem( L"FileName" )->Value );
               pObj->m_file_title =
                  VtToString( pRecordset->adoFields->GetItem( L"FileTitle" )->Value );
               pObj->m_img_title =
                  VtToString( pRecordset->adoFields->GetItem( L"ImageTitle" )->Value );
               pObj->m_eSourceLocationType = (EnumSourceLocationType) (SHORT)
                  pRecordset->adoFields->GetItem( L"LocationType" )->Value;

               do // Once
               {
                  if ( pObj->m_eSourceLocationType != SOURCE_LOCATION_TYPE_TILE_CACHE )
                  {
                     if ( pObj->m_eFileAccessStatus == C_nitf_obj::FILE_ACCESS_NOT_CHECKED )
                     {
                        pObj->m_eFileAccessStatus =
                           ( 
                              // Ok if the file can be accessed
                              FIL_access( pObj->m_csFilespec, FIL_EXISTS ) == SUCCESS

                              // or it's a multispectral file base name
                              || pObj->m_filename.Find( _T("_xxx_") ) != -1

                              // or it's on an internet server
                              || pObj->m_eSourceLocationType == SOURCE_LOCATION_TYPE_JPIP_SERVER
                              || pObj->m_eSourceLocationType == SOURCE_LOCATION_TYPE_NETWORK_SERVER
                           )
                           ? C_nitf_obj::FILE_ACCESS_OK
                           : C_nitf_obj::FILE_ACCESS_NOT_ACCESSIBLE;
                     }
                     if ( pObj->m_eFileAccessStatus != C_nitf_obj::FILE_ACCESS_OK )
                        break;         // File not accessible
                  }
                  else     // Tile cache
                  {
                     // Image is from a TileCacheService, parse filespec into retrieval components
                     // Isolate the tile-cache server canonical name
                     INT iPos1 = pObj->m_csFilespec.Find( _T('|'), 0 );
                     ASSERT( iPos1 != -1 );
                     
                     // Set up the TileCacheService object
                     if ( !pObj->m_bTileCacheServiceInitialized )
                     {
                        if ( S_OK != pObj->m_tcsTileCacheService.SetUrl(
                                       CString( _T("http://") )
                                          + pObj->m_csFilespec.Left( iPos1 )
                                          + _T(":8080/soap/servlet/rpcrouter" ) ) )
                           break;      // Bad URL

                        _bstr_t bstrServerCanonicalName = pObj->m_csFilespec.Left( iPos1 );

                        // Assume can use Windows credentials (for now)
                        pObj->m_snaoNTLMAuthorization.Init( s_QueryDlg.m_pxdQueryToolConfiguration,
                           bstrServerCanonicalName );
                        if ( !pObj->m_tcsTileCacheService.m_socket.AddAuthObj(
                                 _T("NTLM"), &pObj->m_snaoNTLMAuthorization,
                                 pObj->m_snaoNTLMAuthorization.GetUserName().length() > 0
                                    ? &pObj->m_snaoNTLMAuthorization : NULL ) )
                           break;

                        // TCS basic authorization
                        pObj->m_bBasicAuthorizationUsed = FALSE;  // Haven't tried to use yet
                        pObj->m_sbaoBasicAuthorization.Init( s_QueryDlg.m_pxdQueryToolConfiguration,
                           bstrServerCanonicalName );

                        pObj->m_tcsTileCacheService.SetTimeout( 60 * 1000 );
                        
                        // Extract tile-cache subservice name
                        INT iPos2 = pObj->m_csFilespec.Find( _T('|'), iPos1 + 1 );
                        ASSERT( iPos2 != -1 );
                        pObj->m_bstrTileCacheSubserviceName =
                           pObj->m_csFilespec.Mid( iPos1 + 1, iPos2 - iPos1 - 1 );

                        // Skip the identifier type (for now )
                        iPos1 = pObj->m_csFilespec.Find( _T('|'), iPos2 + 1 );
                        ASSERT( iPos1 != -1 );

                        // Extract the image identifer
                        pObj->m_bstrTileCacheImageIdentifier = pObj->m_csFilespec.Mid( iPos1 + 1 );

                        // Find the username and password for this subservice
                        NITFCredentialsFromXML( s_QueryDlg.m_pxdQueryToolConfiguration,
                           bstrServerCanonicalName,
                           BSTR( L"TileCacheService" ),
                           pObj->m_bstrTileCacheSubserviceName,
                           &pObj->m_bstrSubserviceUserName, &pObj->m_bstrSubservicePassword );

                        pObj->m_bTileCacheServiceInitialized = TRUE;
                     }  // TileCacheService object not set up
                  }  // Tile cache

                  pObj->m_image_loaded = TRUE;   // Not tested here if tile-cache
               } while ( FALSE );

               CString csType = VtToString( pRecordset->adoFields->GetItem( L"ImageType" )->Value );
               csType.TrimRight();
               FileTypeLookupIter itftl = m_mpFileTypeLookup.find( csType );
               if ( itftl == m_mpFileTypeLookup.end() )
               {
                  ATLASSERT( FALSE && "Unrecognized image file type" );
                  pObj->m_eFileType = NITF_UNDEFINED_FILE;
               }
               else
                  pObj->m_eFileType = itftl->second;

               pObj->m_contr_minval = (long)pRecordset->adoFields->GetItem( L"MinLevel" )->Value;
               pObj->m_contr_ctrval = (long)pRecordset->adoFields->GetItem( L"MidLevel" )->Value;
               pObj->m_contr_maxval = (long)pRecordset->adoFields->GetItem( L"MaxLevel" )->Value;

               pObj->m_priority = (long)pRecordset->adoFields->GetItem( L"EffectivePriority" )->Value;

               // Check whether there is a display override
               pObj->m_mTempDisplayMap = mDisplayMask
                  & (LONG) pRecordset->adoFields->GetItem( L"TempDisplay" )->Value;
               if ( 0 !=
                  ( pObj->m_mTempDisplayMap & ~NITF_TEMP_DISPLAY_NORMAL_MASK ) )
               {
                  // Override fill control
                  pObj->m_eFillControl = (EnumFillControl)
                     (LONG) pRecordset->adoFields->GetItem( L"Fill" )->Value;

                  // If selected or flashing, make sure frame is visible
                  if ( 0 != ( pObj->m_mTempDisplayMap &
                     ( NITF_TEMP_DISPLAY_SELECTED_MASK | NITF_TEMP_DISPLAY_FLASH_MASK ) )
                     && pObj->m_eFillControl != FILL_CONTROL_OUTLINE_FRAME )
                     pObj->m_eFillControl = FILL_CONTROL_FILLED_WITH_FRAME;

                  // If flashing, mark the start time and reset the flash flag
                  if ( 0 != ( pObj->m_mTempDisplayMap & NITF_TEMP_DISPLAY_FLASH_MASK ) )
                  {
                     pObj->m_mTempDisplayMap &= ~NITF_TEMP_DISPLAY_FLASH_MASK;
                     pObj->m_dwBeginFlashTicks = ULONG_MAX; // Announce flash initiation
                     wostringstream wssSQLCmd;
                     switch ( s_eDBServerType )
                     {
                        case IMAGEDBSERVER_SQLSERVER_SERVER_TYPE:
                           wssSQLCmd <<
                              L"UPDATE " NITF_IMAGE_DISPLAY_INFO_TABLE L" SET\n"
                              L"  TempDisplay = TempDisplay & " << ~NITF_TEMP_DISPLAY_FLASH_MASK << L"\n"
                              L"WHERE [ID] = " << (LONG) pRecordset->adoFields->GetItem( L"DisplayInfoID" )->Value << L"\n";
                           break;

                        case IMAGEDBSERVER_POSTGRES_SERVER_TYPE:
                           wssSQLCmd <<
                              L"UPDATE " NITF_POSTGRES_TBL_PREFIX NITF_IMAGE_DISPLAY_INFO_TABLE L" SET\n"
                              L"  TempDisplay = TempDisplay & " << ~NITF_TEMP_DISPLAY_FLASH_MASK << L"\n"
                              L"WHERE id = " << (LONG) pRecordset->adoFields->GetItem( L"DisplayInfoID" )->Value << L"\n";
                           break;

                        default:
                           ATLASSERT( FALSE && "Unknown database server type" );
                     }  // switch( s_eDBServerType )
                     s_pNITFDBAgent->ExecuteSQL( wssSQLCmd.str().c_str() );
                  }
               }  // Display override
               else
               {
                  // Default fill control
                  pObj->m_eFillControl = m_eImagesOnlyNoEditor == IMAGES_ONLY_NO_EDITOR_TRUE
                     ? FILL_CONTROL_FILLED_WITHOUT_FRAME
                     : (EnumFillControl) (LONG) pRecordset->adoFields->GetItem( L"Fill" )->Value;
               }
            
               // May be able to release ImageLib instance and buffers
               if ( pObj->m_eFillControl == FILL_CONTROL_OUTLINE_FRAME )
                  pObj->Close();

               // Opacity internally is 0-255
               pObj->m_trans_value =
                  static_cast<int>( 2.551 * (long) pRecordset->adoFields->GetItem( L"OpacityVal" )->Value );
               
               pObj->m_eq_mode = (LONG) pRecordset->adoFields->GetItem( L"Equalization" )->Value;

               _bstr_t bstrXML = pRecordset->adoFields->GetItem( L"DisplayParamsXML" )->Value;
#if 0 // Load even if null
               if ( bstrXML.length() > 0 )
#endif
                  pObj->m_pxdDisplayParams->loadXML( bstrXML );
               
               pObj->m_has_geo = TRUE;
               #define OBJOFF( item ) ( (PBYTE) &( (C_nitf_obj*) 0 )->item - (PBYTE) 0 )
               static const struct ObjItems
               {
                  INT      iByteOffset;
                  LPCWSTR  pwszFieldName;
               } dItems[] =
               {
                  { OBJOFF( m_bnd_ll.lat ),           L"LLLat" },
                  { OBJOFF( m_bnd_ll.lon ),           L"LLLong" },
                  { OBJOFF( m_bnd_lr.lat ),           L"LRLat" },
                  { OBJOFF( m_bnd_lr.lon ),           L"LRLong" },
                  { OBJOFF( m_bnd_ul.lat ),           L"ULLat" },
                  { OBJOFF( m_bnd_ul.lon ),           L"ULLong" },
                  { OBJOFF( m_bnd_ur.lat ),           L"URLat" },
                  { OBJOFF( m_bnd_ur.lon ),           L"URLong" },
                  { OBJOFF( m_geo_rect_ll.lat ),      L"MinLat" },
                  { OBJOFF( m_geo_rect_ll.lon ),      L"MinLong" },
                  { OBJOFF( m_geo_rect_ur.lat ),      L"MaxLat" },
                  { OBJOFF( m_geo_rect_ur.lon ),      L"MaxLong" },
                  { OBJOFF( m_pixel_size_in_meters ), L"PixelSize" },
                  { 0 }
               };
               const ObjItems* pdItem = dItems;
               do
               {
                  * (DOUBLE*) ( pdItem->iByteOffset + (PBYTE) &*pObj ) =
                     (DOUBLE) pRecordset->adoFields->
                        GetItem( _bstr_t( pdItem->pwszFieldName ) )->Value;
               } while ( (++pdItem)->iByteOffset != 0 );

               INT iMultibandCode = (INT) pRecordset->adoFields->GetItem( L"MultibandCode" )->Value;
               pObj->m_is_color = iMultibandCode >= 1;
               pObj->m_is_multispectral = iMultibandCode >= 2;  // Multi or single file multispectral

               if ( pObj->m_bSelected )   // If currently selected, save for the end
                  anopSelectedObjs.push_back( pObj );
               else
                  m_objlist.AddTail( pObj );
            }
            catch( _com_error e )
            {
               TRACE( _T("Com error\n") );
            }
            catch(...)
            {
               ASSERT(FALSE);
               throw;
            }
               
            pRecordset->MoveNext();
         }  // while ( !EOF )

         // Add the selected objects to the end
         for ( vector< NITFObjPtr >::iterator it = anopSelectedObjs.begin();
            it != anopSelectedObjs.end(); it++ )
            m_objlist.AddTail( *it );
         
      }  // Query succeeded

      // Remove any unused and idle objects
      pos = m_objlist.GetHeadPosition();
      while ( pos != NULL )
      {
         POSITION pos1 = pos;
         NITFObjPtr pObj = m_objlist.GetNext( pos );
         if ( pObj->m_priority == INT_MIN )     // If unreferenced
         {
            if ( pObj->IsBusy() )
            {
               // Abort the overview build if it hasn't yet started or onscreen needed
               if ( pObj->m_cbImageLibCallback2.m_dProgressPercent <= 0.0
                  || bOnScreenBkgdThreadActive )
                  pObj->m_cbImageLibCallback2.m_eCallbackMode =
                     ImageLibCallbackInterface::CALLBACK_MODE_ABORTING;
            }
            else if ( pObj->m_dwBeginFlashTicks == 0 )
            {
               m_objlist.RemoveAt( pos1 );
               pos = m_objlist.GetHeadPosition();  // Recheck
            }  // !IsBusy and not flashing
         }  // Unreferenced
      }  // Object loop
   
   }
   catch( _com_error &e)
   {
      CString sDescr = (char*)e.Description();
      CString sMsg = "Database Error: " + sDescr;
      // AfxMessageBox (sMsg);
      return FAILURE;
   }   

   return SUCCESS;
}


// **************************************************************************
// **************************************************************************

#ifndef NO_SQL_SERVER_DB      // If using SQL Server database

HRESULT C_nitf_ovl::QuerySQLStream( const wostringstream& wssSQLCmd, _Recordset*& pRecordset )
{
   return s_pNITFDBAgent->raw_QuerySQL( _bstr_t( wssSQLCmd.str().c_str() ), NULL, &pRecordset );
}


HRESULT C_nitf_ovl::ExecuteSQLStream( const wostringstream& wssSQLCmd, PLONG plRecordsAffected )
{
   return s_pNITFDBAgent->raw_ExecuteSQL( _bstr_t( wssSQLCmd.str().c_str() ), plRecordsAffected );
}


// **************************************************************************
// **************************************************************************

BOOL C_nitf_ovl::AddExplorerFile( LPCWSTR pwszFilespec, BOOL& bNeedNewTempDB )
{
   do
   {
      if ( s_pNITFDBAgent == NULL )        // Only if overlay is open
         break;

      wstring wstrVolumeLabel( L"" );   // Assume not volume based
      BOOL bRemovableMedia = FALSE;

      // Network source URLs need special handling
      LPCWSTR pwsz = wcsstr( pwszFilespec, L"://" );
      if ( pwsz < pwszFilespec + 4
            || pwsz > pwszFilespec + 5 )
         goto NotURL;         // Expect 4 or 5 character protocol

      while ( --pwsz >= pwszFilespec )
         if ( !isalpha( *pwsz ) )
            goto NotURL;      // Should be alphabetic protocol string

      // Make sure that any JPIP URL has properly escaped resource characters
      size_t i = 6 + wcsspn( pwszFilespec + 6, L"/\\" );         // Start of source
      if ( pwszFilespec[ i ] != L'\0' )
      {
         // End of source
         i += wcscspn( pwszFilespec + i, L"/\\" );
         if ( pwszFilespec[ i ] != L'\0' )
         {
            // Beginning of resource
            i += wcsspn( pwszFilespec + i, L"/\\" );
            auto_ptr< WCHAR > apwchTemp( new WCHAR[ ( 3 * wcslen( pwszFilespec ) ) + 1 ] );
            LPWSTR pwszTemp = apwchTemp.get();
            memcpy( pwszTemp, pwszFilespec, i * sizeof(WCHAR) );

            // Escape certain characters in the resource.  Assume that any "%" is already
            // encoded
            LPCWCH pwchIn = pwszFilespec + i;
            LPWCH pwchOut = pwszTemp + i;
            do
            {
               if ( iswalnum( (INT) *pwchIn )
                  || NULL != wcschr( L"$-_.+!*'(),&/:;=?@%", (INT) *pwchIn ) )
                  *pwchOut++ = *pwchIn;
               else
               {
#if _MSC_VER >= 1400
                  swprintf_s( pwchOut, 4, L"%%%02X", (INT) *pwchIn );
#else
                  swprintf( pwchOut, L"%%%02X", (INT) *pwchIn );
#endif
                  pwchOut += 3;
               }
            } while ( *(++pwchIn) != L'\0' );
            *pwchOut = L'\0';

            EnterCriticalSection( &m_csUtilityProcAccess );

            // Make sure that a worker thread is active
            do
            {
               // Check whether the worker thread is active
               if ( m_pUtilityThread != NULL )
               {
                  // Check for the thread still running
                  if ( WAIT_OBJECT_0 != WaitForSingleObject( m_pUtilityThread->m_hThread, 0 ) )
                  {
                     if ( m_bUtilityThreadActive )
                        break;      // Still in business

                     // Wait for it to finish exiting
                     WaitForSingleObject( m_pUtilityThread->m_hThread, INFINITE );
                  }
                  delete m_pUtilityThread;
               }

               // Start a new utility thread
               if ( FAILED( CoMarshalInterThreadInterfaceInStream(
                  __uuidof( INITFDBAgent ), s_pNITFDBAgent, &m_pNITFDBAgentIStream ) ) )
               {
                  ATLASSERT( FALSE && "Marshal NITFDBAgent pointer to worker thread failed" );
                  break;
               }
               m_pUtilityThread = AfxBeginThread( UtilityProc, (LPVOID) this,
                  THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED );
               if ( m_pUtilityThread == NULL )
               {
                  ATLASSERT( FALSE && "Utility process thread failed to start" );
                  break;
               }
               m_pUtilityThread->m_bAutoDelete = FALSE;
               m_bUtilityThreadActive = TRUE;
               m_pUtilityThread->ResumeThread();

            } while ( FALSE );

            if ( m_bUtilityThreadActive )
            {
               XML2DocumentPtr pxd(  __uuidof( MSXML2::DOMDocument60 ) );
               pxd->loadXML(
                  L"<?xml version=\"1.0\"?>\n"
                  L"<UtilityControl/>\n" );
               XMLNodePtr pxnRoot = pxd->documentElement;
               XMLNodePtr pxn = pxnRoot->appendChild( pxd->createElement( L"CatalogFile" ) );
               pxn->text = pwszTemp;

               m_wtdUtilityControl.push_back( pxd->xml );
            }

            LeaveCriticalSection( &m_csUtilityProcAccess );
            return TRUE;

         }
      }
      return FALSE;  // Unrecognized URL structure
      
      // Not a URL
NotURL:
      if ( bNeedNewTempDB )
      {
         if ( SUCCESS != open_nitf_database() )    // Harmless if already opened
            break;                                 // Quit

         // First drop any existing temporary db
         s_pNITFDBAgent->raw_SpecialFunction(
            IMAGEDBSERVER_SPECFUNC_ENABLE_TEMP_DB_TABLE, L"C=0", NULL );

         bNeedNewTempDB = FALSE;


         if ( S_OK != s_pNITFDBAgent->raw_SpecialFunction(
               IMAGEDBSERVER_SPECFUNC_ENABLE_TEMP_DB_TABLE, L"C=1", NULL ) )
            break;

      }  // Temp DB opening

      // Find the volume name if CDROM source
      DWORD dwSerialNumber;
      if ( NITFGetVolumeInformation(
         pwszFilespec,
         wstrVolumeLabel,
         dwSerialNumber,
         bRemovableMedia ) )
      {
         if ( !bRemovableMedia )    // Only for CDROMs
            wstrVolumeLabel.erase();
      }

      // Add the file to the database
      EnumImageLibStatus eFileStatus = IMAGEXXX_SUPPRESS_ERROR_REPORT;
      LONG lNongeorefImages;
      LONG lGeorefImages;

      try
      {
         s_pNITFDBAgent->AddImageFileEx(
            pwszFilespec,
            wstrVolumeLabel.c_str(),
            L"" ,                // No search root path
            &eFileStatus, NULL,
            &lNongeorefImages, &lGeorefImages );
      }
      catch ( ... )
      {
         break;
      }

      // Display the new file or maybe pass it on (to GeoRect probably)
      return PostAddExplorerFile( pwszFilespec, bRemovableMedia, wstrVolumeLabel );

   } while ( FALSE);
   return FALSE;

}  // AddSingleFile()


//
// AddExplorerFileEnd()
//
VOID C_nitf_ovl::AddExplorerFileEnd( BOOL bCheckThread )
{
   do
   {
      if ( bCheckThread && CheckUtilityThreadAlive() )
         break;   // Still in business.  Let the worker thread do the wakeup

      CFrameWndEx* pFrame = UTL_get_frame();
      if ( pFrame )
         pFrame->PostMessage( WM_NITF_UTILITY_THREAD_EVENT, 101, 0 );

   } while ( FALSE );
}


//
// CheckUtilityThreadAlive()
//
BOOL C_nitf_ovl::CheckUtilityThreadAlive()
{
   if ( m_pUtilityThread != NULL )
   {
      // Check for the thread still running
      if ( WAIT_OBJECT_0 != WaitForSingleObject( m_pUtilityThread->m_hThread, 0 ) )
      {
         if ( m_bUtilityThreadActive )
            return TRUE;
      }
   }
   return FALSE;
}


//
// OnNITFUtilityThreadEvent()
//
LRESULT C_nitf_ovl::OnNITFUtilityThreadEvent( WPARAM wParam, LPARAM lParam )
{
   wstring wstrEmpty( L"" );              // Can be modified

   if ( wParam != 101 )
      ATLASSERT( FALSE && "C_nitf_ovl::OnNITFUtilityThreadEvent() - wParam expected to be 101" );

   // If this is the end of the new imagery
   else if ( lParam == 0 )
   {
      m_bMultipleExplorerFiles = FALSE;      // No more yes/no to all
      do
      {
         if ( m_bstrLastExplorerFilespec.length() == 0 ) // If there is no new image to center on
            break;

         if ( m_eNewExplorerCurrentImageDisplay == NEW_EXPLORER_IMAGE_CENTER_MAP_ASK )
         {
            int rslt = AfxMessageBox( "Do you want to center the map on the new image?", MB_YESNO );
            if ( rslt != IDYES )
               break;
                
            m_eNewExplorerCurrentImageDisplay = NEW_EXPLORER_IMAGE_CENTER_MAP_ALWAYS;
         }
         if ( m_eNewExplorerCurrentImageDisplay == NEW_EXPLORER_IMAGE_CENTER_MAP_ALWAYS )
         {
            m_eNewExplorerCurrentImageDisplay = NEW_EXPLORER_IMAGE_CENTER_MAP_LAST;
            PostAddExplorerFile( m_bstrLastExplorerFilespec, FALSE, wstrEmpty );
         }

      } while ( FALSE );
      m_eNewExplorerCurrentImageDisplay = NEW_EXPLORER_IMAGE_IDLE;
   }
   else 
   {
      _bstr_t bstrFilespec( (BSTR) lParam, FALSE );
      PostAddExplorerFile( (LPCWSTR) bstrFilespec, FALSE, wstrEmpty );
   }

   return 0;
}


//
// PostAddExplorerFile()
//
  
BOOL C_nitf_ovl::PostAddExplorerFile(
      LPCWSTR     pwszFilespec,
      BOOL        bRemovableMedia,
      wstring&    wstrVolumeLabel )
{
   // Retrieve the (possibly new) info from the database
   wstring wstrFilespec = pwszFilespec;
   QuoteDBString( wstrFilespec );     // Surround filespec with single quotes

   wostringstream wossWhereAndOrderByClause;
   wossWhereAndOrderByClause <<
      L" AND fi.Filespec = " << wstrFilespec << L"\n";

   if ( bRemovableMedia )
   {
      QuoteDBString( wstrVolumeLabel );
      wossWhereAndOrderByClause << L"  AND fi.VolumeLabel = " << wstrVolumeLabel << L"\n";
   }
   wossWhereAndOrderByClause << ends;

   _RecordsetPtr spRecordset;
   HRESULT hr = s_pNITFDBAgent->raw_QueryImageAndDisplayInfoEx(
                  NEW_IMAGE_QUERY_FIELD_NAMES, FALSE, L"generic",\
                  _bstr_t( ( wossWhereAndOrderByClause.str() +
                  L") ORDER BY ImageInFile\n" ).c_str() ),
                  NULL,
                  &spRecordset );
   if ( FAILED( hr ) )
      ATLASSERT( FALSE && "Test for existing command-line NITF file failed" );
   else
   {
      // Only if the image was decoded into the database
      CComBSTR ccbsErrorTitle, ccbsErrorFmt;
      const int ERRORMSG_LEN = 501;
      WCHAR wchErrorMsg[ ERRORMSG_LEN ];
      ccbsErrorTitle.LoadString( IDS_NITF_CMD_IMAGE_ERROR_TITLE );

      if ( spRecordset->adoEOF )
      {
         ccbsErrorFmt.LoadString( IDS_NITF_CMD_IMAGE_ADD_ERROR );
         _snwprintf_s( wchErrorMsg, ERRORMSG_LEN, 500, (LPCWSTR) ccbsErrorFmt, pwszFilespec );
         MessageBoxW( NULL, wchErrorMsg, (LPCWSTR) ccbsErrorTitle, MB_OK | MB_ICONERROR );
      }

      // Handle ungeoreferenced image file
      else if ( 0 == (LONG) spRecordset->adoFields->GetItem( L"BoolGeoAvailable" )->Value )
      {
         // Find the GeoRect app
         CHAR chOpenCmd[ ( 2 * MAX_PATH ) + 1 ];
         do
         {
            DWORD dwType = REG_SZ;
            DWORD dwSize = 2 * MAX_PATH;
            if ( SUCCESS == PRM_read_registry( HKEY_CLASSES_ROOT, "Applications\\GeoRect.exe\\Shell\\Open\\Command",
               "", &dwType, (PUCHAR) chOpenCmd, &dwSize ) )
            {
               LPCH pch = strstr( chOpenCmd, "\"%1\"" );
               if ( pch != NULL
                  && pch - chOpenCmd + wcslen( pwszFilespec ) < ( 2 * MAX_PATH ) - 4 )
               {
                  TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
                  INT pch_len = ( 2 * MAX_PATH ) + 1 - (pch - chOpenCmd);
                  strcpy_s( pch + 1, pch_len - 1, CW2A( pwszFilespec ) );
                  strcat_s( pch, pch_len, "\"" );

                  ccbsErrorFmt.LoadString( IDS_NITF_CMD_IMAGE_NOT_GEO_ERROR );
                  _snwprintf_s( wchErrorMsg, ERRORMSG_LEN, 500, (LPCWSTR) ccbsErrorFmt, pwszFilespec );
                  if ( IDYES == MessageBoxW( NULL, wchErrorMsg, (LPCWSTR) ccbsErrorTitle, MB_YESNO | MB_ICONQUESTION ) )
                     WinExec( chOpenCmd, SW_SHOW );

                  break;
               }
            }
            ccbsErrorFmt.LoadString( IDS_NITF_CMD_IMAGE_NOT_GEO_NO_GEORECT_ERROR );
            _snwprintf_s( wchErrorMsg, ERRORMSG_LEN, 500, (LPCWSTR) ccbsErrorFmt, pwszFilespec );
            MessageBoxW( NULL, wchErrorMsg, (LPCWSTR) ccbsErrorTitle, MB_OK | MB_ICONERROR );
         } while ( FALSE );
      }

      // Fully georeferenced
      else
      {
         MapView* pMapView = fvw_get_view();
         if ( !pMapView->IsKindOf( RUNTIME_CLASS( MapView ) ) )
            pMapView = NULL;

         if ( m_eNewExplorerInitialImageDisplay != NEW_EXPLORER_IMAGE_DISPLAY_ONLY )
         {
            static const DOUBLE
               DEG2RAD = M_PI / 180.0,
               RAD2DEG = 180.0 / M_PI;
            DOUBLE
               dLatUL = DEG2RAD * (DOUBLE) spRecordset->adoFields->GetItem( L"ULLat" )->Value,
               dLatLR = DEG2RAD * (DOUBLE) spRecordset->adoFields->GetItem( L"LRLat" )->Value,
               dLonUL = DEG2RAD * (DOUBLE) spRecordset->adoFields->GetItem( L"ULLong" )->Value,
               dLonLR = DEG2RAD * (DOUBLE) spRecordset->adoFields->GetItem( L"LRLong" )->Value,
               dXMean2 = ( cos( dLatUL ) * cos( dLonUL ) ) + ( cos( dLatLR ) * cos( dLonLR ) ),
               dYMean2 = ( cos( dLatUL ) * sin( dLonUL ) ) + ( cos( dLatLR ) * sin( dLonLR ) ),
               dZMean2 = sin( dLatUL ) + sin( dLatLR ),
               dRMean2 = sqrt( ( dXMean2 * dXMean2 ) + ( dYMean2 * dYMean2 ) ),
               dLat = RAD2DEG * atan2( dZMean2, dRMean2 ),
               dLon = RAD2DEG * atan2( dYMean2, dXMean2 );

            if ( m_eNewExplorerCurrentImageDisplay == NEW_EXPLORER_IMAGE_IDLE )
               m_eNewExplorerCurrentImageDisplay = m_eNewExplorerInitialImageDisplay;

            switch ( m_eNewExplorerCurrentImageDisplay )
            {
               case NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ASK:
               {
                  CYesNoAllDlg dlg( m_bMultipleExplorerFiles );
                  switch( dlg.DoModal() )
                  {
                     case IDC_OVL_NITF_YES_TO_ALL:
                        m_eNewExplorerCurrentImageDisplay = NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ALWAYS;

                     case IDC_OVL_NITF_YES:
                        goto CenterNewTab;

                     case IDC_OVL_NITF_NO_TO_ALL:
                        m_eNewExplorerCurrentImageDisplay = NEW_EXPLORER_IMAGE_DISPLAY_ONLY;
                  }
                  pMapView = NULL;
                  break;      // No centering here
               }

               case  NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ALWAYS:
               {
CenterNewTab:     CMapViewTabsBar* pMapViewTabsBar = fvw_get_frame()->GetMapViewTabsBar();
                  if ( pMapViewTabsBar != NULL )
                     pMapViewTabsBar->AddTab();

                  goto CenterMap;
               }

               case NEW_EXPLORER_IMAGE_CENTER_MAP_ASK:
               case NEW_EXPLORER_IMAGE_CENTER_MAP_ALWAYS:
                  m_bstrLastExplorerFilespec = pwszFilespec;   // Remember for later
                  pMapView = NULL;     // No updates yet
                  break;

               case NEW_EXPLORER_IMAGE_CENTER_MAP_LAST:
               {
                  // Center the map on the new image if possible
CenterMap:        if ( pMapView != NULL )
                     pMapView->get_map_engine()->change_center( dLat, dLon );
               }

            } // switch( m_eNewExplorerCurrentImageDisplay )
         }  // Not display-only

         // Force a redraw even if the map doesn't move
         m_bReload= TRUE;
         OVL_get_overlay_manager()->invalidate_all( TRUE );

         // Force window update before handling any more images
         if ( pMapView != NULL )
            pMapView->UpdateWindow();

      }  // Georeferenced
   }  // DB query succeeded
   return TRUE;
}
 

// **************************************************************************
// **************************************************************************
VOID C_nitf_ovl::DropTempDB()
{
   s_pNITFDBAgent->raw_SpecialFunction(
      IMAGEDBSERVER_SPECFUNC_ENABLE_TEMP_DB_TABLE, L"C=0", NULL );
}

#endif


//
// GetNITFDBInfo()
//
BOOL C_nitf_ovl::GetNITFDBInfoPage( const C_nitf_obj* const pObj, CString& csInfo )
{
   CComBSTR ccbsErrorMsg;
   CComBSTR ccbsInfoPage;

   _bstr_t bstrOptions =
      L"<?xml version=\"1.0\"?>\n"
      L"<!--GetImageInfoPage() options-->\n"
      L"<GetImageInfoPageOptions>\n"
      L"  <VolumeLabel>\n"
      L"    ";
   bstrOptions += (LPCWSTR) CT2OLE( pObj->m_csVolumeLabel );
   bstrOptions +=
      L"\n"
      L"  </VolumeLabel>\n"
      L"  <RichText>1</RichText>\n"
      L"</GetImageInfoPageOptions>\n";
   _variant_t varOptions = bstrOptions;

   HRESULT hr = s_pNITFDBAgent->raw_GetImageInfoPage(
      _bstr_t( pObj->m_csFilespec ), pObj->m_iImageInFile,
      varOptions, &ccbsErrorMsg, &ccbsInfoPage );
   if ( hr != S_OK )
      return FALSE;

   csInfo = (LPCWSTR) ccbsInfoPage;
   return TRUE;

}  // End of GetNITFDBInfo()


//
// QuoteDBString();
//
wstring& C_nitf_ovl::QuoteDBString( wstring& wstrInOut )
{
   return NITFQuoteString( wstrInOut, s_eDBServerType );
}


//
// UtilityProc() - Background database utility process
//
UINT C_nitf_ovl::UtilityProc( LPVOID pvThis )
{
   return reinterpret_cast< C_nitf_ovl* >( pvThis )->UtilityProc();
}

UINT C_nitf_ovl::UtilityProc()
{
   CoInitialize( NULL );

   XML2DocumentPtr pxdControl( __uuidof( MSXML2::DOMDocument60 ) );

   typedef set< wstring, WStringILess > FilespecSet;
   typedef FilespecSet::iterator FilespecSetIter;
   FilespecSet wstrPreviousFilespecs;  // To trap multiple references and recursion

   BOOL bInCriticalSection = FALSE;
   try
   {
      if ( S_OK != CoGetInterfaceAndReleaseStream(
            m_pNITFDBAgentIStream, __uuidof( INITFDBAgent ), (LPVOID*) &m_fphJPIPProbeHost.m_spNITFDBAgent ) )
      {
         ATLASSERT( FALSE && "Failed to unmarshall utility INITFDBAgent in C_nitf_ovl::UtilityProc()" );
         goto Exit;
      }
      do
      {
         EnterCriticalSection( &m_csUtilityProcAccess );
         bInCriticalSection = TRUE;

         if ( m_wtdUtilityControl.empty() )
            break;
 
         pxdControl->loadXML( m_wtdUtilityControl.front() );
         m_wtdUtilityControl.pop_front();

         if ( !m_wtdUtilityControl.empty() )
            m_bMultipleExplorerFiles = TRUE;     // More to follow

         LeaveCriticalSection( &m_csUtilityProcAccess );
         bInCriticalSection = FALSE;


         using namespace NITFImagery;
         FilespecList wstrFilespecList;

         // Collect the list of filespecs
         XMLNodeListPtr pxl = pxdControl->selectNodes( L"UtilityControl/CatalogFile" );
         XMLNodePtr pxn;
         while ( NULL != ( pxn = pxl->nextNode() ) )
            wstrFilespecList.push_back( (LPCWSTR) pxn->text );

         // Process the entire list with possible expansions
         for ( size_t idx = 0; idx < wstrFilespecList.size(); idx++ )
         {
            // Check for duplicate URLs
            pair< FilespecSetIter, BOOL > p
               = wstrPreviousFilespecs.insert( wstrFilespecList[ idx ] );
            if ( !p.second )
               continue;         // We already handled this one

            if ( idx + 1 < wstrFilespecList.size() )
               m_bMultipleExplorerFiles = TRUE;     // Assume multiple files involved

            wstring& wstrFilespec = wstrFilespecList[ idx ];
            LPCWSTR pwszFilespec = wstrFilespec.c_str();

            // Check for a list of files hidden in an IP XML document, probably a RSS feed
            if ( 0 == _wcsnicmp( pwszFilespec, L"http://", 7 )
               || 0 == _wcsnicmp( pwszFilespec, L"https://", 8 ) )
            {
               LPCWSTR pwsz;
               if ( NULL != ( pwsz = wcsrchr( pwszFilespec, L'.' ) ) )
               {
                  if ( 0 == _wcsicmp( pwsz + 1, L"xml" ) )
                  {
                     // Will expand into a list of filespecs if something we recognize
                     ExpandIPXML2URL( wstrFilespecList, pwszFilespec );
                     continue;      // The list may have expanded
                  }
               }
            }

            // We're only handling JPIP:// URLs for now
            if ( _wcsnicmp( pwszFilespec, L"jpip://", 7 ) != 0 )
               continue;

            CRemoteServerFileProbe probe;
            probe.m_eRemoteFileProbeStatus = probe.ProbeFile( pwszFilespec );

            if ( probe.m_eRemoteFileProbeStatus != CRemoteServerFileProbe::REMOTE_STATUS_PROBE_DONE )
            {
               MessageBeep( MB_ICONERROR );
               continue;
            }

            // Make sure the JPIP metadata decoder info is available
            m_fphJPIPProbeHost.InitializeMetadataDecoders();

            // Turn the JPIP metadata into SQL commands
            m_fphJPIPProbeHost.AnalyzeMetadata( &probe );

            // Put into the imagery database
            vector< _bstr_t >::iterator iter;
            for ( iter = probe.m_abstrSQLCmds.begin(); iter != probe.m_abstrSQLCmds.end(); iter++ )
            {
               LONG lAffectedRecords;
               HRESULT hr = m_fphJPIPProbeHost.m_spNITFDBAgent->raw_ExecuteSQL( *iter, &lAffectedRecords );
               ATLASSERT( SUCCEEDED( hr ) );
            }

            // Wake up the main thread
            CFrameWndEx* pFrame = UTL_get_frame();
            if ( pFrame )
               pFrame->PostMessage( WM_NITF_UTILITY_THREAD_EVENT, 101,
                  (LPARAM) _bstr_t( pwszFilespec ).Detach() );    // Pass the new filespec to the main thread

         }  // URL loop
      } while ( TRUE );
   }
   catch ( ... )
   {
      ATLASSERT( FALSE && "Exception in C_nitf_ovl::UtilityProc()" );
   }
Exit:
   m_bUtilityThreadActive = FALSE;

   // Release the decoders document and the database interface
   m_fphJPIPProbeHost.ThreadClear();

   if ( bInCriticalSection )
      LeaveCriticalSection( &m_csUtilityProcAccess );

   // Wake up the main thread to reset yes/no/all messages
   AddExplorerFileEnd( FALSE );

   CoUninitialize();

   return 0;
}

////////////////////////////////////////////////////////////////////////
//
// CNITFJPIPProbeHost
//
////////////////////////////////////////////////////////////////////////

CNITFJPIPProbeHost::CNITFJPIPProbeHost( C_nitf_ovl *pNITFOvl ) :
   m_pNITFOvl( pNITFOvl )
{
}

VOID CNITFJPIPProbeHost::ThreadClear()
{
   m_pxnMetadataDecodersRoot = NULL;
   m_pxdMetadataDecoders = NULL;
   m_spNITFDBAgent = NULL;
}


//
// QuerySQL() - query NITF database from the utility thread
//
HRESULT CNITFJPIPProbeHost::QuerySQL( const wostringstream& woss, _Recordset** pprs )
{
   return m_spNITFDBAgent->raw_QuerySQL( _bstr_t( woss.str().c_str() ), L"", pprs );
}

// End of nitf_sql.cpp