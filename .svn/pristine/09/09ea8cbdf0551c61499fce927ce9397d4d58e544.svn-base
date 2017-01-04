// Copyright (c) 1994-2011,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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

// env.cpp -- Environmental file overlay

#include "stdafx.h"

#include "env.h"

#include "param.h"
#include "showrmk.h"
#include "file.h"
#include "shpread.h"
#include "refresh.h"
#include "err.h"
#include "ovlutil.h"
#include "..\mapview.h"
#include "factory.h"
#include "..\StatusBarManager.h"
#include "..\mainfrm.h"
#include "maps.h"
#include "ovl_mgr.h"
#include <math.h>


// The following two items must match
static LPCSTR const pszEnvironmentalTypeNames[] =
{ "mad2000", "madsens", "madindn", "madstat", "madostat", "madother" };

enum { MAD2000AGL = 0, MADSENS = 1, MADINDRES = 2,
   MADSTAT = 3, MADOSTAT = 4, MADOTHER = 5, MADLAST = MADOTHER }; 


// Constructor
C_env_ovl::C_env_ovl()

{
   for (int i=0; i<NUM_ENVIRONMENTAL_TYPES; i++)
      m_pDisplayProperties[i] = new SHPDisplayProperties;
  
   OnDisplayPropertiesChanged();
   m_infoDialogBaseTitle = "Environmental Info --- ";

   m_large_file = FALSE;
}

void C_env_ovl::check_for_large_files()
{
   CString sdata, pathname;

   m_large_file = FALSE;

   if (m_data_path.GetLength() < 10)
      return;

   int size_threshold = PRM_get_registry_int("Environmental", "EnvSizeThreshold", 30000000);

   // load shape objects from various shape files
   int total_size = 0;
   int file_size;
   sdata = PRM_get_registry_string("Environmental", "EnvOptions", "YYNNNN");
   for (int i=0; i<NUM_ENVIRONMENTAL_TYPES; i++)
   {
      if (sdata[i] == 'N')
         continue;

      CString filename = m_data_path + CA2T( pszEnvironmentalTypeNames[i] );

      // Check the total file size
      CString shpname = filename + ".shp";
      HANDLE hFile = CreateFile( shpname, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
      if ( hFile != INVALID_HANDLE_VALUE )
      {
         file_size = GetFileSize( hFile, NULL );
         total_size += file_size;
         CloseHandle( hFile );
      }
   }

   if (total_size > size_threshold)
      m_large_file = TRUE;
}


HRESULT C_env_ovl::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   m_data_path = PRM_get_registry_string("Main", "ReadWriteUserData") +
      "\\Shape\\Environmental\\";

   if ( SUCCESS != check_group_files() )
   {
#ifdef MAKE_DERIVED_SHAPEFILES
      load_default_groups();
      CString error_txt;
      do
      {
         if ( SUCCESS == make_group_shape_files( error_txt ) )
         {
            if ( SUCCESS == check_group_files() )  // Recheck
               break;

            error_txt = _T("check_group_files() recheck failed");
         }
         AfxMessageBox(error_txt);
         return E_FAIL;

      } while ( FALSE );
#else
      AfxMessageBox( _T("check_group_files() failed, missing environmental group files") );
      return E_FAIL;
#endif
   }

   check_for_large_files();

   return S_OK;
}

// Destructor
C_env_ovl::~C_env_ovl()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.

   // Clean up any string lists
#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES
   m_mpGroupEnvTypes.clear();
#endif
}

void C_env_ovl::Finalize()
{
   clear_ovl();

   for ( int k = 0; k < NUM_ENVIRONMENTAL_TYPES; k++ )
      m_ampCachedShapesInfo[ k ].clear();

   HEAP_REPORT( _T("After m_ampCachedShapesInfo.clear()") )

   if (this == CRemarkDisplay::get_focus_overlay())
      CRemarkDisplay::close_dlg();

   for (int i=0; i<NUM_ENVIRONMENTAL_TYPES; i++)
   {
      delete m_pDisplayProperties[i];
      m_pDisplayProperties[i] = NULL;
   }
}

void C_env_ovl::clear_ovl()
{
   HEAP_REPORT( _T("Before env::clear( types )") )

   for ( int k = 0; k < NUM_ENVIRONMENTAL_TYPES; k++ )
   {
      SHPFileLoDMap& mpl = m_ampCachedShapesInfo[ k ];
      for ( SHPFileLoDMapIter it = mpl.begin(); it != mpl.end(); it++ )
         it->second.clear();
   }  // Types

   // Now clear the active shapes and heap
   CBaseShapeFileOverlay::clear_ovl();

   m_data_loaded.clear();
}  // clear_ovl()


int C_env_ovl::draw(ActiveMap* map)
{
   // Desired level-of-detail
   degrees_t dDLat, dDLon;
   map->get_vmap_degrees_per_pixel( &dDLat, &dDLon );
   int iLoD = (int) ( 0.99 + ( log( 180.0 / ( 512.0 * __max( dDLat, dDLon ) ) ) / log( 2.0 ) ) );

   if (!m_large_file)
   {
      m_scrn_ll.lat = -9999;
      if ( m_data_loaded.find( iLoD ) == m_data_loaded.end() )
#ifdef TIMING_TEST
      {
         DWORD dw1 = GetTickCount();
#endif
         m_scrn_ll.lat = -90.0; m_scrn_ll.lon = -180.0;
         m_scrn_ur.lat = +90.0; m_scrn_ur.lon = +180.0;
         open( iLoD );
#ifdef TIMING_TEST
         ATLTRACE( _T("Env::Open(full LoD %d) - %d ms\n"), iLoD, (INT) ( GetTickCount() - dw1 ) );
      }
#endif
   }
   else
   {
      if (!get_valid())
      {
#ifdef TIMING_TEST
         DWORD dw1 = GetTickCount();
#endif
         map->get_vmap_bounds(&m_scrn_ll, &m_scrn_ur);
         open( iLoD );
#ifdef TIMING_TEST
         ATLTRACE( _T("Env::Open(limited LoD %d) - %d ms\n"), iLoD, (INT) ( GetTickCount() - dw1 ) );
#endif
      }
   }

   int ret = DrawShapeObjects(map);
   set_valid(TRUE);
   return ret;
}

void C_env_ovl::release_focus()
{
   if (m_selected_obj != NULL)
   {
      m_selected_obj = NULL;

      // since the selected object changed we need to invalidate the overlay
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }
}

int C_env_ovl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   HCURSOR hCursor = NULL;
   HintText *hint = NULL;
   int ret = OnTestSelected(&mapProjWrapper, point, &hCursor, &hint);
   if (hCursor != NULL)
      pMapView->SetCursor(hCursor);
   if (ret == SUCCESS && hint != NULL)
   {
      pMapView->SetTooltipText(_bstr_t(hint->get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hint->get_help_text()));
   }

   return ret;
}

int C_env_ovl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   boolean_t drag;
   HCURSOR cursor;
   HintText *hint = NULL;
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   if (OnSelected(this, &mapProjWrapper, point, &drag, &cursor, &hint) == FAILURE)
   {
      release_focus();
      return FAILURE;
   }

   return SUCCESS;
}

// return the filename associated with the given shape file
std::string C_env_ovl::GetFilenameFromShape(const SHPObject* pShapeObject) const
{
   std::string pathname = PRM_get_registry_string("Main", "ReadWriteUserData") +
      "\\Shape\\Environmental\\";

   return pathname + pszEnvironmentalTypeNames[ pShapeObject->displayOrder ];
}

// add environmental overlay menu items
void C_env_ovl::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   OnMenu(this, map, point, list);
}

void C_env_ovl::OnDisplayPropertiesChanged()
{
   // Detect near line
   CString sdata = PRM_get_registry_string("Environmental", "DetectNearLine", "Y");
   m_near_line_mode = sdata[0] != 'N';

   // Polygon fill
   sdata = PRM_get_registry_string("Environmental", "EnvFill", "0");
   int fill_style = atoi(sdata);

   // Background
   sdata = PRM_get_registry_string("Environmental", "EnvBackground", "Y");
   int background = sdata[0] != 'N';

   static const CString
      colorKeys[ NUM_ENVIRONMENTAL_TYPES ] =
         { "2000AglColor", "SensColor", "IndianColor", "StateColor", "OtherStateColor", "OtherAreaColor" },
      defaultColors[ NUM_ENVIRONMENTAL_TYPES ] =
         { "255072072", "236118000", "128000000", "000213000", "000128000", "128128000" };

   CFvwUtil* futil = CFvwUtil::get_instance();
   for ( int i = 0; i < NUM_ENVIRONMENTAL_TYPES; i++ )
   {
      m_pDisplayProperties[i]->background = background;
      m_pDisplayProperties[i]->color_rgb = futil->string_to_color( PRM_get_registry_string( "Environmental", colorKeys[i], defaultColors[i] ) );
      m_pDisplayProperties[i]->width = 2;
      m_pDisplayProperties[i]->fill_style = fill_style;
      m_pDisplayProperties[i]->line_style = UTIL_LINE_SOLID;
      m_pDisplayProperties[i]->diameter = 8;
      m_pDisplayProperties[i]->use_icon = 0;
      m_pDisplayProperties[i]->pIconImage = NULL;
   }

   check_for_large_files();
}

int C_env_ovl::open( int iLoD )
{
   CWaitCursor waitCursor;

   CShapeRead shp( static_cast< CBaseShapeFileOverlay* >( this ) );
   std::string error_txt, sdata;
   CString tooltipStr, helptextStr, labelStr;   // Preallocate to avoid unneccesary heap operations
   int rslt;
   CDbase dbf;
   BOOL bDBFOpen = FALSE;

   // Remove all the current drawable objects
   m_selected_obj = NULL;
   m_vecShpObjects.clear();

   // load shape objects from various shape files
   sdata = PRM_get_registry_string("Environmental", "EnvOptions", "YYNNNN");
      
   for ( int iType = 0; iType < NUM_ENVIRONMENTAL_TYPES; iType++ )
   {
      if (sdata[ iType ] == 'N')
         continue;

      // Find the best LoD file available
      SHPFileLoDMap& mpl = m_ampCachedShapesInfo[ iType ];
      SHPFileLoDMapIter it1 = mpl.lower_bound( iLoD );
      if ( it1 == mpl.end() )
      {
         ATLASSERT( FALSE && "No full resolution file entry in LoD map" );
         it1--; // Use the best we have
      }

      // Get a set of shape objects covering (with extra margin) the target area
      CStringA
         base_filename = m_data_path + pszEnvironmentalTypeNames[ iType ],
         full_filename = base_filename;   // Assume not LoD tag
      if ( it1->first != 1000 )
      {
         CStringA cs;
         cs.Format( "_LoD%d", it1->first );
         full_filename += cs;
      }

      SHPObjectMap& mpShpObjects = it1->second.m_mpCachedShapes;

      // Get a set of shape objects covering (with extra margin) the target area
      rslt = shp.open( NULL, full_filename, error_txt,  mpShpObjects,
         m_scrn_ll, m_scrn_ur, it1->second.m_gptCacheLL, it1->second.m_gptCacheUR,
         true );

      m_data_loaded.insert( it1->first );    // In case small file loading

      // Ignore the shape objects outside of the target area
      bDBFOpen = FALSE;      // No DB file open yet

      for ( SHPObjectMapIter it2 = mpShpObjects.begin(); it2 != mpShpObjects.end(); it2++ )
      {
         SHPObject& shape = *(it2->second);
         if ( GEO_intersect( m_scrn_ll.lat, m_scrn_ll.lon,
                              m_scrn_ur.lat, m_scrn_ur.lon,
                              shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                              shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
            m_vecShpObjects.push_back( it2->second );

         // Fill in the DB info if not yet done
         if ( shape.displayOrder < 0 )
         {
            // Open dbf file to retrieve tooltip / helptext information
            if ( !bDBFOpen )
            {
               int ret = dbf.OpenDbf( full_filename + ".dbf", OF_READ);
               if ( ret != DB_NO_ERROR )
               {  
                  // unable to open DBF file
                  CString msg;
                  msg.Format( _T("Unable to open DBF file: %s.dbf\n: error = %d"), full_filename, ret );
                  ERR_report( msg );
                  return FAILURE;
               }
               bDBFOpen = TRUE;
            }

            // Populate each shape object with the appropriate display
            // order, properties, tooltip, and helptext
            shape.displayOrder = iType;
            shape.pDisplayProperties = m_pDisplayProperties[ iType ];

            dbf.GotoRec( shape.nShapeId + 1 );
            tooltipStr = GetField( &dbf, "AREANAME" );
            tooltipStr.TrimRight();

            helptextStr = GetField( &dbf, "DESIGNATE" );
            helptextStr.TrimRight();

            // Set the label for the shape file based on the AREANAME and
            // DESIGNATE fields
            labelStr = tooltipStr;

            // Don't combine AREANAME and DESIGNATE if DESIGNATE is embedded in AREANAME
            if ( tooltipStr.Find( helptextStr ) == -1 )
            {
               labelStr += _T(", ");
               labelStr += helptextStr;
            }

            shape.ClearStrings();

            size_t c = shape.m_wHelpTextOffset = tooltipStr.GetLength() + 1;
            size_t cBufferLen = c + helptextStr.GetLength() + 1 + labelStr.GetLength() + 1;
            shape.m_pchShapeStrings = reinterpret_cast< LPCH >(
               m_clsHeapAllocator.allocate( cBufferLen * sizeof(CHAR) ) );

            strcpy_s( shape.m_pchShapeStrings, cBufferLen, tooltipStr );
            cBufferLen -= c;

            strcpy_s( shape.m_pchShapeStrings + c, cBufferLen, helptextStr );
            c = helptextStr.GetLength() + 1;
            cBufferLen -= c;

            shape.m_wLabelOffset = shape.m_wHelpTextOffset + c;
            strcpy_s( shape.m_pchShapeStrings + shape.m_wLabelOffset, cBufferLen, labelStr );

         }  // Shape metadata needed

      }  // Cached shape objects loop

      if ( bDBFOpen )
      {
         dbf.CloseDbf();
         bDBFOpen = FALSE;
      }

      // check for empty list;
      if (m_vecShpObjects.empty())
         continue;

      // set the overlay's type based on the first object (all objects will have the same type)
      m_type = m_vecShpObjects[0]->nSHPType;

   }  // Environmental type loop

   std::sort(m_vecShpObjects.begin(), m_vecShpObjects.end(), CBaseShapeFileOverlay::SortShapesByDisplayOrder);

   if (rslt != SUCCESS)
   {
      return FAILURE;
   }

   set_valid(TRUE);

   return SUCCESS;
}  // open()

MapScale C_env_ovl::get_smallest_scale()
{ 
   CString display_above = PRM_get_registry_string("Environmental", 
      "EnvHideAbove", "1:5 M");
   set_smallest_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_scale; 
}

void C_env_ovl::set_smallest_scale(MapScale scale) 
{ 
   m_smallest_scale = scale; 
}

MapScale C_env_ovl::get_smallest_labels_scale()
{ 
   CString display_above = PRM_get_registry_string("Environmental", 
      "LabelsHideAbove", "1:250 K");

   set_smallest_labels_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_labels_scale; 
}

void C_env_ovl::set_smallest_labels_scale(MapScale scale) 
{ 
   m_smallest_labels_scale = scale; 
}

const CString & C_env_ovl::get_name() const
{ 
   static const CString name("Environmental");

   return name;
}


// *****************************************************************
// *****************************************************************

int C_env_ovl::check_group_files()
{
   WIN32_FIND_DATAA find_file_data;
   int iResult = SUCCESS;  // Assume all base files will be found
   for ( int iType = 0; iType < NUM_ENVIRONMENTAL_TYPES; iType++ )
   {
      SHPFileLoDMap& mpl = m_ampCachedShapesInfo[ iType ];

      // Find all group files with a .shp extension
      HANDLE handle = FindFirstFileA( m_data_path + pszEnvironmentalTypeNames[ iType ] + "*.shp", &find_file_data );
      if ( handle != INVALID_HANDLE_VALUE )  // If any group .shp file(s)
      {
         do
         {
            std::string file_name = find_file_data.cFileName;
            // strip off the .shp extension
            file_name.erase( file_name.size() - 4 );

            // Look for a level-of-detail tag
            int iLoD;
            std::string::size_type pos;
            char ch;
            if ( ( pos = file_name.find( "_LoD" ) ) == std::string::npos
                  || 1 != sscanf_s( file_name.substr( pos + 4 ).c_str(), "%d%c", &iLoD, &ch, 1 ) )
               iLoD = 1000;  // No valid LoD tag, use full resolution

            ATLASSERT( mpl.find( iLoD ) == mpl.end()
               && "Unexpected duplicate entry in the LoD map" );
            mpl.insert( SHPFileLoDMap::value_type( iLoD, SHPFileLoD() ) );

         } while ( FindNextFileA( handle, &find_file_data ) );

         FindClose( handle );
      
      }  // Any .shp files

      if ( mpl.find( 1000 ) == mpl.end() )
         iResult = FAILURE;    // Didn't find max resolution .shp file
      
   }  // Types loop

   return iResult;
}
// end of check_group_files


#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES

// *****************************************************************
// **************************************************************************

void C_env_ovl::load_default_groups() 
{
   // Clear any existing group map
   m_mpGroupEnvTypes.clear();
   ATLASSERT( MADLAST + 1 == NUM_ENVIRONMENTAL_TYPES );

   LPCSTR const * ppsz;

   // 2000AGL
   static LPCTSTR const psz2000AGLGroups[] =
   {
      "National Historic Park",
      "National Lakeshore",
      "National Monument",
      "National Park",
      "National Recreation Area",
      "National Historic Park",
      "National River",
      "National Scenic River",
      "National Seashore",
      "National Wildlife Refuge",
      "Primitive Area",
      "Recommended Wilderness",
      "Wilderness (Forest Service)",
      "Wilderness (FWS)",
      "Wilderness (NPS)",
      "Wilderness (USFS)",
      "Wilderness Area",
      "Wilderness Unit",
      "Wilderness (Forest Service)",
      "World Heritage Site (designates Mammoth Cave National Park)",
      NULL
   };
   for ( ppsz = psz2000AGLGroups; *ppsz != NULL; ppsz++ )
      m_mpGroupEnvTypes.insert( EnvGroupMapValue( *ppsz, MAD2000AGL ) );


   // Sensitive
   static LPCTSTR const pszSensitiveGroups[] =
   {
      "Biosphere Reserve",
      "Conservation Area",
      "International Peace Garden",
      "National Battlefield",
      "National Battlefield Park",
      "National Conservation Area",
      "National Estuarine Research Reserve",
      "National Historic Site",
      "National Memorial",
      "National Military Park",
      "National Natural Landmark",
      "National Preserve",
      "National Reserve and Recreation Area",
      "National Scientific Reserve",
      "Parkway",
      "Waterfowl Management Area",
      "Waterfowl Reserve",
      "Wild and Scenic River",
      "Wilderness (BLM)",
      "Wilderness (State)",
      "Wilderness Study Area",
      "Wildlife Management Area",
      NULL
   };
   for ( ppsz = pszSensitiveGroups; *ppsz != NULL; ppsz++ )
      m_mpGroupEnvTypes.insert( EnvGroupMapValue( *ppsz, MADSENS ) );


   // Indian
   static LPCTSTR const pszIndianReservationGroups[] =
   {
      "Indian Reservation",
      NULL
   };
   for ( ppsz = pszIndianReservationGroups; *ppsz != NULL; ppsz++ )
      m_mpGroupEnvTypes.insert( EnvGroupMapValue( *ppsz, MADINDRES ) );


   // StateLocal
   static LPCTSTR const pszStateLocalGroups[] =
   {
      "Bird Sanctuary",
      "County Park",
      "County Regional Park",
      "Forest Park",
      "Forest Preserve",
      "Park",
      "Recreation Area",
      "Recreation Lands",
      "Regional Par",
      "State Beach",
      "State Bird Refuge",
      "State Historic Park",
      "State Historical Monument",
      "State Lake",
      "State Memorial Park",
      "State Natural Area",
      "State Park",
      "State Park and Forest",
      "State Recreation Area",
      "State Reserve",
      "State Vehicular Recreation Area",
      "State Wildlife Recreation Area",
      NULL
   };
   for ( ppsz = pszStateLocalGroups; *ppsz != NULL; ppsz++ )
      m_mpGroupEnvTypes.insert( EnvGroupMapValue( *ppsz, MADSTAT ) );


   // OtherStateFed
   static LPCTSTR const pszOtherStateFedGroups[] =
   {
      "Government Reservation",
      "Military Reservation",
      "National Forest",
      "National Grasslands",
      "State Forest",
      NULL
   };
   for ( ppsz = pszOtherStateFedGroups; *ppsz != NULL; ppsz++ )
      m_mpGroupEnvTypes.insert( EnvGroupMapValue( *ppsz, MADOSTAT ) );


   // OtherAreas
   static LPCTSTR const pszOtherAreasGroups[] =
   {
      "Future Planning Area",
      "Management Area",
      "Natural Area",
      "Other Area",
      "Outstanding Natural Area",
      "Private Reserve",
      "Rancheria",
      "Refuge",
      "State Seabird Refuge",
      "Waterfowl Production Area",
      "Wayside",
      "Wildlife Research Area",
      NULL
   };
   for ( ppsz = pszOtherAreasGroups; *ppsz != NULL; ppsz++ )
      m_mpGroupEnvTypes.insert( EnvGroupMapValue( *ppsz, MADOTHER ) );

}
// end of load_default_groups


// *****************************************************************
// *****************************************************************

int C_env_ovl::make_group_shape_files( CString& error_txt )
{
   CWaitCursor waitCursor;

   // Hook shape reader to private heap
   CShapeRead shp( static_cast< CBaseShapeFileOverlay* >( this ) );

   SHPHandle hSHP_input;
   SHPHandle hSHP_output[ NUM_ENVIRONMENTAL_TYPES ];

   CDbase dbf_input;
   CDbase dbf_output[ NUM_ENVIRONMENTAL_TYPES ];

   int iRslt = SUCCESS; // Assumed

   WIN32_FIND_DATAA find_file_data;    // For LoD files

   // Loop for full resolution env file then all level-of-detail files
   HANDLE hFindFile = INVALID_HANDLE_VALUE;  // Full resolution pass first
   do
   {
      CStringA
         csBaseInputFilespec,
         csBaseOutputFilespec,
         csFilespec = m_data_path
            + ( ( hFindFile == INVALID_HANDLE_VALUE ) ? "mad.shp" : find_file_data.cFileName );

      //////////////////////////////////////////////////////////
      ///////////  Open the files  ///////////////////////
      //////////////////////////////////////////////////////////

      // Open the input .shp file
      hSHP_input = shp.SHPOpen( csFilespec, "rb" );
      if ( hSHP_input == NULL )
      {
         error_txt.Format( _T("Unable to open: %s for input"), CA2T( csFilespec ) );
         iRslt = FAILURE;
         goto Done;
      }

      // Remove the extension
      csBaseInputFilespec = csFilespec.Left( csFilespec.GetLength() - 4 );

      // Open the input .dbf file
      csFilespec = csBaseInputFilespec + ".dbf";
      if ( DB_NO_ERROR != dbf_input.OpenDbf( csFilespec, OF_READ ) )
      {
         error_txt.Format( _T("Unable to open %s for input"), CA2T( csFilespec ) );
         iRslt = FAILURE;
         goto Done;
      }
   
      // Create the field structure
      DbfField_t* fld = (DbfField_t*) malloc( dbf_input.m_DbInfo.NumFields * sizeof(DbfField_t) );
      for ( int k=0; k < dbf_input.m_DbInfo.NumFields; k++ )
      {
         strncpy_s( fld[k].Name, 11, dbf_input.m_DbHead->Field[k].Name, 11 );
         fld[k].Tipe = dbf_input.m_DbHead->Field[k].Tipe;
         fld[k].Length = dbf_input.m_DbHead->Field[k].Length;
         fld[k].Decimal = dbf_input.m_DbHead->Field[k].Decimal;
      }
      
      // Open the output files
      ZeroMemory( hSHP_output, sizeof(hSHP_output) );

      for ( int iType = 0; iType < NUM_ENVIRONMENTAL_TYPES; iType++ )
      {
         if ( hFindFile != INVALID_HANDLE_VALUE )   // If LoD pass
         {
            int iPos = csBaseInputFilespec.Find( "_LoD" );
            if ( iPos <= 0 )
               ATLASSERT( iPos > 0 && "Missing _LoD in filename" );
            else
               csBaseOutputFilespec = m_data_path
                  + pszEnvironmentalTypeNames[ iType ]
                  + csBaseInputFilespec.Right( csBaseInputFilespec.GetLength() - iPos );
         }
         else
            csBaseOutputFilespec = m_data_path
               + pszEnvironmentalTypeNames[ iType ];  // Assume not LoD pass

         csFilespec = csBaseOutputFilespec + ".shp";
         if ( NULL == ( hSHP_output[ iType ] = shp.SHPCreate( csFilespec, SHPT_POLYGON ) ) )
            goto OpenFail;

         csFilespec = csBaseOutputFilespec + ".dbf";
         if ( DB_NO_ERROR != dbf_output[ iType ].CreateDbf( csFilespec, fld, dbf_input.m_DbInfo.NumFields ) )
         {
OpenFail:   error_txt.Format( "Unable to open %s for output", CA2T( csFilespec ) );
            iRslt = FAILURE;
            goto Done;
         }
      }  // Open ouput types loop

      free( fld );

      // Find number of shapes in the input
      int nShapeType, nEntities;
      double adfMinBound[ 4 ], adfMaxBound[ 4 ];
      shp.SHPGetInfo( hSHP_input, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

      // Pass the input records to the designated output files
      for ( int iEntity = 0; iEntity < nEntities; iEntity++ )
      {
         CString sdata;
         char data[256];

         // Read the next shape from the input .shp file
         SHPObjectPtr spShape( new
            ( m_clsHeapAllocator.allocate( sizeof(SHPObject) ) ) // Allocate from private heap
            SHPObject( &m_clsHeapAllocator ) );
         SHPObject& shape = *spShape;

         shp.SHPReadObject( hSHP_input, iEntity, shape );

         int rslt = dbf_input.GotoRec( iEntity + 1 );
         rslt = dbf_input.GetFieldData( "DESIGNATE", data );
         sdata = data;
         sdata.TrimRight();
         sdata.TrimLeft();

         // put the record in the right output file
         EnvGroupMapIter it = m_mpGroupEnvTypes.find( sdata );
         if ( it == m_mpGroupEnvTypes.end() )
         {
            ATLTRACE(
               _T("Unrecognized/unexpected \"DESIGNATE\" field value = \"%s\" in mad.dbf\n"),
               CA2T( sdata ) );
            continue;
         }

         // Write the input record to the appropriate output files
         int& iGroup = it->second;

         field_data_t recdata[ 32 ];
         dbf_input.GetRecordData( recdata );
         dbf_output[ iGroup ].AppendRecord( recdata );
         shp.SHPWriteObject( hSHP_output[ iGroup ], -1, &shape );

#ifdef FAST_SHAPES_CLEAR
         spShape.detach(); // Abandon the shape data
         if ( ( iEntity % 100 ) == 99 )   // Occasionally purge the private heap
            CBaseShapeFileOverlay::clear_ovl();
#endif
      }  // Entity loop
         
Done: shp.SHPClose( hSHP_input );
      dbf_input.CloseDbf();

      for ( int i = 0; i < NUM_ENVIRONMENTAL_TYPES; i++ )
      {
         shp.SHPClose( hSHP_output[ i ] );
         if ( dbf_output[ i ].m_DbOpened )
         {
            dbf_output[ i ].EndAppend();
            dbf_output[ i ].CloseDbf();
         }
      }

      // Look for more level-of-detail (mad_LoDn.shp) files
      if ( hFindFile == INVALID_HANDLE_VALUE )
      {
         if ( INVALID_HANDLE_VALUE ==
               ( hFindFile = FindFirstFileA( m_data_path + "mad_LoD*.shp", &find_file_data ) ) )
            break;   // No LoD files
      }
      else if ( !FindNextFileA( hFindFile, &find_file_data ) ) // If no more LoDs
         break;
   } while ( TRUE ); // Level-of-detail loop

   // Clear the group type map
   m_mpGroupEnvTypes.clear();
   clear_ovl();   // Decoded shapes

   return iRslt;
}
// end of make_group_shape_files

#endif
// ***************************************************************

// End of env.cpp
