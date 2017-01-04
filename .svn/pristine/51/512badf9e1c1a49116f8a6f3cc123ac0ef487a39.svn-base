// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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

#include "stdafx.h"

#include "BoundariesOverlay.h"

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
#include "StringUtils.h"

using namespace std;

// Constructor
BoundariesOverlay::BoundariesOverlay()
{
   m_infoDialogBaseTitle = "Boundaries Overlay Info --- ";

   m_large_file = FALSE;
}

// Destructor
BoundariesOverlay::~BoundariesOverlay()
{
#ifdef TIMING_TEST
   DWORD dw1 = GetTickCount();
#endif

   clear_ovl();

   m_tiers.clear();

   if (this == CRemarkDisplay::get_focus_overlay())
      CRemarkDisplay::close_dlg();

#ifdef TIMING_TEST
   ATLTRACE(  _T("~Boundaries(), t=%d\n"), (INT) ( GetTickCount() - dw1 ) );
#endif

}  // ~BoundariesOverlay()


void BoundariesOverlay::clear_ovl()
{
#ifndef FAST_SHAPES_CLEAR
   HEAP_REPORT( _T("Before boundaries::clear( tiers )") )
#endif

      // First detach the shape object cache
   for_each( m_tiers.begin(),
      m_tiers.end(), [&]( BoundariesTier& tier )
   {
      for ( BoundariesTierComponentMapIter it1 = tier.m_mpTierComponents.begin();
         it1 != tier.m_mpTierComponents.end(); it1++ )
      {
         for ( SHPFileLoDMapIter it2 = it1->second.m_mpLoDs.begin();
            it2 != it1->second.m_mpLoDs.end(); it2++ )
         {
#ifdef FAST_SHAPES_CLEAR
            for ( SHPObjectMapIter it3 = it2->second.m_mpCachedShapes.begin();
                  it3 != it2->second.m_mpCachedShapes.end(); it3++ )
               it3->second.detach();
#endif
            it2->second.m_mpCachedShapes.clear();
         }
         it1->second.m_mpLoDs.clear();
      }
   });

   // Now clear the active shapes and heap
   CBaseShapeFileOverlay::clear_ovl();

   m_data_loaded.clear();
}  // clear_ovl()


/*static*/
void BoundariesOverlay::ReadInOpenState( vector<BoundariesTier>& tiers )
{
   // Build up a string to represent the current open state.
   string tiers_open = "";
   for_each(tiers.begin(),
      tiers.end(), [&](BoundariesTier& tier)
   {
      tiers_open += tier.m_on ? "Y" : "N";
   });

   // Read the open state from the registry.  Use the current state as default.
   string sdata = PRM_get_registry_string("Boundaries Overlay",
      "Boundaries Overlay Options",
      tiers_open.c_str());

   size_t tier_index = 0;
   for_each(tiers.begin(), tiers.end(),
      [&](BoundariesTier& tier)
   {
      if ( tier_index >= sdata.length() )
         tier.m_on = true;
      else
         tier.m_on = ( sdata[tier_index] != 'Y' ? false : true );
      ++tier_index;
   });
}

/* static */
void BoundariesOverlay::add_tier_file_names(
   __in const string& data_path,
   __in const string& tier_path,
   __inout BoundariesTierComponentMap& components )
{
   using namespace std;

#ifdef TIMING_TEST_2
   DWORD dw1 = GetTickCount();
#endif

   CShapeRead shp( nullptr );
   SHPHandle hSHP;
 
   // find all the files with a .shp extension
   string full_path = data_path + tier_path + "*.shp";
   
   WIN32_FIND_DATAA find_file_data;
   
   HANDLE handle = FindFirstFileA( full_path.c_str(), &find_file_data );
   if ( handle != INVALID_HANDLE_VALUE )  // If any .shp file(s)
   {
      do
      {
         string file_name = find_file_data.cFileName;
         // strip off the .shp extension
         file_name.erase( file_name.size() - 4 );

         // Look for a level-of-detail tag
         int iLoD;
         string::size_type pos;
         char ch;
         string strLoDTag;
         if ( ( pos = file_name.find( "_LoD" ) ) != string::npos
               && 1 == sscanf_s( file_name.substr( pos + 4 ).c_str(), "%d%c", &iLoD, &ch, 1 ) )
         {
            strLoDTag = file_name.substr( pos );
            file_name.erase( pos );    // Remove the LoD tag
         }
         else
            iLoD = 1000;  // No valid LoD tag, use full resolution

         // Check for extracted localized file(s)
         HANDLE hFindLocalizedFile = INVALID_HANDLE_VALUE;
         if ( string( find_file_data.cFileName ).find( "_local_" ) == string::npos
               && INVALID_HANDLE_VALUE != ( hFindLocalizedFile =
                  FindFirstFileA(
                     ( data_path + tier_path + file_name + "_local_*" + strLoDTag + ".shp" ).c_str(),
                        &find_file_data ) ) )
            FindClose( hFindLocalizedFile );

         // Use the master if there aren't any localized versions
         if ( hFindLocalizedFile == INVALID_HANDLE_VALUE )
         {
            pair< BoundariesTierComponentMapIter, bool > p;
            BoundariesTierComponentMapIter& it = p.first;

            // Make sure that there is a LoD map entry for this base file name
            if ( ( it = components.find( file_name ) ) == components.end() )
               p = components.insert( BoundariesTierComponentMap::value_type( file_name, BoundariesTierComponent() ) );

            ATLASSERT( it->second.m_mpLoDs.find( iLoD ) == it->second.m_mpLoDs.end()
               && "Unexpected duplicate entry in the LoD map" );
            it->second.m_mpLoDs.insert( SHPFileLoDMap::value_type( iLoD, SHPFileLoD() ) );

            // Open the.shp file
            string strFilespec = data_path + tier_path + file_name + strLoDTag + ".shp";
            hSHP = shp.SHPOpen( strFilespec.c_str(), "rb" );
            if ( hSHP != NULL )
            {
               // Update the boundaries of this component
               int nShapeType, cEntities;
               double adfMinBound[ 4 ], adfMaxBound[ 4 ];
               shp.SHPGetInfo( hSHP, &cEntities, &nShapeType, adfMinBound, adfMaxBound );

               NEW_MIN( it->second.m_dMinLat, adfMinBound[ 1 ] );
               NEW_MIN( it->second.m_dMinLon, adfMinBound[ 0 ] );
               NEW_MAX( it->second.m_dMaxLat, adfMaxBound[ 1 ] );
               NEW_MAX( it->second.m_dMaxLon, adfMaxBound[ 0 ] );
               shp.SHPClose( hSHP );
            }
         }

      } while ( FindNextFileA(handle, &find_file_data) );

     FindClose(handle);
   }  // Any .shp files
#ifdef TIMING_TEST_2
   DWORD dw2 = GetTickCount();
   ATLTRACE( _T("AddTierFiles( %s ) time = %d\n"), tier_path.c_str(), dw2 - dw1 );
#endif


}  // add_tier_file_names()

void BoundariesOverlay::check_for_large_files()
{
   m_large_file = FALSE;

   if (m_data_path.length() < 10)
      return;

   int size_threshold = PRM_get_registry_int(
      "Boundaries Overlay", "BoundariesOverlaySizeThreshold", 30000000);

   // load shape objects from various shape files
   int total_size = 0;
   ReadInOpenState(m_tiers);

   for_each(m_tiers.begin(), m_tiers.end(), [&](BoundariesTier& tier)
   {
      if (!tier.m_on)
         return;

      for ( BoundariesTierComponentMapIter it1 = tier.m_mpTierComponents.begin();
         it1 != tier.m_mpTierComponents.end(); it1++ )
      {
         for ( SHPFileLoDMapIter it2 = it1->second.m_mpLoDs.begin();
            it2 != it1->second.m_mpLoDs.end(); it2++ )
         {
            if ( it2->first != 1000 )
               continue;         // Check only the full resolution files

            // Check the file size
            string shpname = m_data_path + tier.m_strTierFolder + it1->first + ".shp";
            HANDLE hFile = CreateFileA( shpname.c_str(), GENERIC_READ,
               FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
            if ( hFile != INVALID_HANDLE_VALUE )
            {
               int file_size = GetFileSize( hFile, NULL );
               total_size += file_size;
               CloseHandle( hFile );
            }
         }  // Various LoDs
      }  // Tier components
   });   // Tiers

   if (total_size > size_threshold)
      m_large_file = TRUE;
}  // check_for_large_files()


/*static*/
string BoundariesOverlay::GetDataPath()
{
   string result = PRM_get_registry_string("Main", "ReadWriteUserData") +
      "\\Shape\\Boundaries Overlay\\";
   return result;
}

HRESULT BoundariesOverlay::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   m_data_path = GetDataPath();

#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES
   static BOOL bOnce = FALSE;
   if ( !bOnce )
   {
      bOnce = TRUE;
#ifdef TIMING_TEST_2
      DWORD dw0 = GetTickCount(), dw1, dw2, dw3, dw4, dw5;
#endif
      if ( SUCCESS != check_localized_shapefiles() )
      {
#ifdef TIMING_TEST_2
         dw1 = GetTickCount();
#endif
         CString error_txt;
         do
         {
#ifdef TIMING_TEST_2
            dw2 = GetTickCount();
#endif
            if ( SUCCESS == make_localized_shapefiles( error_txt ) )
            {
#ifdef TIMING_TEST_2
               dw3 = GetTickCount();
#endif
               if ( SUCCESS == check_localized_shapefiles() )  // Recheck
                  break;

               error_txt = _T("check_localized_shapefiles() recheck failed");
            }
            AfxMessageBox(error_txt);
            return E_FAIL;

         } while ( FALSE );
#ifdef TIMING_TEST_2
         dw4 = GetTickCount();
         ATLTRACE( _T("Localized files make time = %d, recheck time = %d\n"), dw3 - dw2, dw4 - dw3 );
#endif
      }
#ifdef TIMING_TEST_2
      dw5 = GetTickCount();
      ATLTRACE( _T("Localized files check/make time = %d\n"), dw5 - dw0 );
#endif
   }
#endif

   // initialize from registry
   OnDisplayPropertiesChanged();

   check_for_large_files();

   return S_OK;
}

int BoundariesOverlay::draw(ActiveMap* map)
{
   // Find the level-of-detail number
   int iLoD;
#if 0 // Extra points mean don't need to use max LoD data
   ProjectionEnum eProj = map->requested_projection();
   if ( eProj != EQUALARC_PROJECTION
         && eProj != MERCATOR_PROJECTION )
      iLoD = 99;  // If curved latitudes, don't use depopulated data
   else
#endif
   {
      degrees_t dDLat, dDLon;
      map->get_vmap_degrees_per_pixel( &dDLat, &dDLon );
      iLoD = (INT) ( log( 180.0 / ( 512.0 * __max( dDLat, dDLon ) ) ) / log( 2.0 ) );
   }

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
         open( iLoD );  // Read everything
#ifdef TIMING_TEST
         ATLTRACE( _T("Boundaries::Open(full) - %d ms\n"), (INT) ( GetTickCount() - dw1 ) );
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
         map->get_vmap_bounds( &m_scrn_ll, &m_scrn_ur );
         open( iLoD );  // Read selected area
#ifdef TIMING_TEST
         ATLTRACE( _T("Boundaries::Open(limited %d) - %d ms\n"), iLoD, (INT) ( GetTickCount() - dw1 ) );
#endif
      }
   }

   int ret = DrawShapeObjects(map);
   set_valid(TRUE);

   return ret;
}

void BoundariesOverlay::release_focus()
{
   if (m_selected_obj != NULL)
   {
      m_selected_obj = NULL;

      // since the selected object changed we need to invalidate the overlay
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }
}

int BoundariesOverlay::on_mouse_moved(IFvMapView *pMapView, CPoint point,
   UINT flags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(
      pMapView->m_pCrntMapProj());

   HCURSOR hCursor = NULL;
   HintText *hint = NULL;
   int ret = OnTestSelected(&mapProjWrapper, point, &hCursor, &hint);
   if (hCursor != NULL)
      pMapView->SetCursor(hCursor);
   if (ret == SUCCESS && hint != NULL)
   {
      pMapView->SetTooltipText(_bstr_t(hint->get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->
         SetStatusBarHelpText(_bstr_t(hint->get_help_text()));
   }

   return ret;
}

int BoundariesOverlay::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   boolean_t drag;
   HCURSOR cursor;
   HintText *hint = NULL;
   map_projection_utils::CMapProjWrapper mapProjWrapper(
      pMapView->m_pCrntMapProj());
   if (OnSelected(this, &mapProjWrapper, point, &drag, &cursor, &hint) ==
      FAILURE)
   {
      release_focus();
      return FAILURE;
   }

   return SUCCESS;
}

// add overlay menu items
void BoundariesOverlay::menu(ViewMapProj* map, CPoint point,
   CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   OnMenu(this, map, point, list);
}

/*static*/
string BoundariesOverlay::ComputeColorString(int i)
{
   // red percieves well on black or white background, so we anchor the colors
   // with that.  We will only add colors (ie get closer to white) from here.
   int r = 255;

   // possible values are 0, 128, 256
   int g = ((i % 3 ) * 128);
   g = min(255, g);

   // possible values are 0, 64, 128, 192, 256
   int b = ((i % 5 ) *  64);
   b = min(255, b);

   //this combination of colors repeats every 3 * 5 = 15 iterations

   stringstream ss;
   ss << r;

   if ( g < 100 )
      ss << "0";
   if ( g < 10 )
      ss << "0";
   ss << g;

   if ( b < 100 )
      ss << "0";
   if ( b < 10 )
      ss << "0";
   ss << b;
   return ss.str();
}

void BoundariesOverlay::FillTiers(const string& data_path,
   BoundariesTierVec& tiers)
{
   // fill
   string sdata = PRM_get_registry_string("Boundaries Overlay", "Fill", "0");
   int fill_style = atoi(sdata.c_str());

   // background
   sdata = PRM_get_registry_string("Boundaries Overlay", "Background", "Y");
   BOOL background = sdata[0] != 'N';

   // for each folder under m_data_path that begins with "tier", search
   // for .shp files and add them
   string search_path = data_path;
   search_path += "tier*";

   vector<string> tier_folders;

   WIN32_FIND_DATAA find_file_data;
   HANDLE handle = FindFirstFileA(search_path.c_str(), &find_file_data);
   if (handle != INVALID_HANDLE_VALUE)
   {
      do
      {
         if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
         {
            string folder_name = find_file_data.cFileName;
            folder_name += "\\";
            tier_folders.push_back(folder_name);
         }
      } while( FindNextFileA( handle, &find_file_data ) );

      FindClose(handle);
   }

   // FindFirstFile does not guarentee a sort order.
   sort(tier_folders.begin(), tier_folders.end());

   int i = 0;
   CFvwUtil* futil = CFvwUtil::get_instance();
   for_each(tier_folders.begin(), tier_folders.end(),
      [&]( const string& folder_name )
   {
      // Add tier to vector before filling it in
      tiers.push_back( BoundariesTier() );
      BoundariesTier& tier = tiers.back();

      tier.m_on = true;
      tier.m_strTierFolder = folder_name;
      tier.m_color_key = folder_name.substr(0,folder_name.length() - 1);
      tier.m_color_key += "-Color";
      tier.m_default_color = ComputeColorString(i);

      SHPDisplayProperties* properties = &tier.m_display_properties;
      properties->background = background;
      properties->color_rgb = futil->string_to_color(
         PRM_get_registry_string( "Boundaries Overlay",
         tier.m_color_key.c_str(), tier.m_default_color.c_str()));
      properties->width = 2;
      properties->fill_style = fill_style;
      properties->line_style = UTIL_LINE_SOLID;
      properties->diameter = 8;
      properties->use_icon = 0;
      properties->pIconImage = NULL;

      add_tier_file_names( data_path, folder_name, tier.m_mpTierComponents );
      ++i;
   });
}

void BoundariesOverlay::OnDisplayPropertiesChanged()
{
   // detect near line
   CString sdata = PRM_get_registry_string(
      "Boundaries Overlay", "DetectNearLine", "Y");
   m_near_line_mode = sdata[0] != 'N';

   sdata = PRM_get_registry_string(
      "Boundaries Overlay", "WaterAreaLabels", "N");
   m_bWaterAreaLabels = sdata[0] != 'N';

   sdata = PRM_get_registry_string(
      "Boundaries Overlay", "KeepAreaLabelsInView", "Y");
   m_bKeepAreaLabelsInView = sdata[0] != 'N';

   clear_ovl();
   m_tiers.clear();
   FillTiers(m_data_path, m_tiers);

}

int BoundariesOverlay::open( INT iLoD )
{
   CWaitCursor waitCursor;

   CShapeRead shp( static_cast< CBaseShapeFileOverlay* >( this ) );
   string error_txt;
   int rslt;
   BOOL bDBFOpen = FALSE;
   CFvwUtil *futil = CFvwUtil::get_instance();
   CDbase dbf;

   // Remove all the current objects
   m_selected_obj = NULL;
   m_vecShpObjects.clear();

   ReadInOpenState(m_tiers);

   size_t tier_index = 0;
   for_each(m_tiers.begin(), m_tiers.end(),
      [&](BoundariesTier& tier)
   {
      if (!tier.m_on)
         return;

      // Avoid reallocating string storage unneccesarily
      string
         base_filename, full_filename,
         strToolTip, strHelpText, strLabel,
         tooltip_field_name_1, tooltip_field_name_2,
         tooltip_field_value_1, tooltip_field_value_2,
         statbar_field_name_1, statbar_field_name_2,
         statbar_field_value_1, statbar_field_value_2;
      CString csNameFieldValue, csCountryFieldValue;

      for ( BoundariesTierComponentMapIter it1 = tier.m_mpTierComponents.begin();
         it1 != tier.m_mpTierComponents.end(); it1++ )
      {
         BoundariesTierComponent& component = it1->second;

         // Ignore if component doesn't intersect the view
         if ( !GEO_intersect( m_scrn_ll.lat, m_scrn_ll.lon,
                              m_scrn_ur.lat, m_scrn_ur.lon,
                              component.m_dMinLat, component.m_dMinLon,
                              component.m_dMaxLat, component.m_dMaxLon ) )
            continue;

         // Find the best LoD file available
         SHPFileLoDMapIter it2 = component.m_mpLoDs.lower_bound( iLoD );
         if ( it2 == component.m_mpLoDs.end() )
         {
            ATLASSERT( FALSE && "No full resolution file entry in LoD map" );
            it2--; // Use the best we have
         }

         SHPObjectMap& mpShpObjects = it2->second.m_mpCachedShapes;

         // Get a set of shape objects covering (with extra margin) the target area
         base_filename = m_data_path + tier.m_strTierFolder + it1->first,
         full_filename = base_filename;   // Assume not LoD tag
         if ( it2->first != 1000 )
         {
            CStringA cs;
            cs.Format( "_LoD%d", it2->first );
            full_filename += cs;
         }

         rslt = shp.open( NULL, full_filename.c_str(), error_txt,  mpShpObjects,
            m_scrn_ll, m_scrn_ur, it2->second.m_gptCacheLL, it2->second.m_gptCacheUR, true );

         m_data_loaded.insert( it2->first );

         // Ignore the shape objects outside of the target area
         bDBFOpen = FALSE;      // No DB file open yet

         for ( SHPObjectMapIter it = mpShpObjects.begin(); it != mpShpObjects.end(); it++ )
         {
            SHPObject& shape = *(it->second);

            // Fill in the DB info if not yet done
            if ( shape.displayOrder < 0 )
            {
               if ( !bDBFOpen )
               {
                  const string strShortFilespec = base_filename.substr( 0, base_filename.find( "_local_" ) );
                  C_shp_ovl::LoadPreferences( strShortFilespec, nullptr,
                     nullptr, nullptr, nullptr,
                     &tooltip_field_name_1, &tooltip_field_name_2,
                     &statbar_field_name_1, &statbar_field_name_2,
                     nullptr, nullptr,
                     nullptr, nullptr,
                     nullptr, nullptr,
                     nullptr, nullptr, nullptr, nullptr, &error_txt );

                  // Open dbf file to retrive tooltip / helptext information
                  string dbfFilename = base_filename;
                  dbfFilename += ".dbf";
                  int ret = dbf.OpenDbf( dbfFilename.c_str(), OF_READ );
                  if ( ret != DB_NO_ERROR )
                  {
                     // unable to open DBF file
                     string msg( "Unable to open DBF file: " );
                     msg += dbfFilename;
                     msg += "\n: error = ";
                     msg += to_string( static_cast<long double>(ret) );
                     ERR_report( msg.c_str() );
                     rslt = FAILURE;
                  }
                  else
                     bDBFOpen = TRUE;
               }

               shape.displayOrder = 100 - tier_index;   // Draw tier 1 after tier 2 to keep shorelines consistent
               shape.pDisplayProperties = &tier.m_display_properties;

               if ( bDBFOpen )
               {
                  dbf.GotoRec( shape.nShapeId + 1 );

                  csCountryFieldValue = GetField( &dbf, "COUNTRY" );
                  csNameFieldValue = GetField( &dbf, "NAME" );
                  csCountryFieldValue.TrimRight();
                  csNameFieldValue.TrimRight();
                  if ( csCountryFieldValue == "FN"  // Actually a shoreline or limit line
                        || csNameFieldValue == "water/agua/d'eau" )  // Shoreline boundary
                     shape.displayOrder = SHAPE_NOT_DISPLAYED;   // Don't display;
                  else
                  {
                     // Concatenate the two tooltip value fields into the tooltip (no dupes)
                     tooltip_field_value_1.clear();
                     if ( !tooltip_field_name_1.empty() )
                     {
                        tooltip_field_value_1 = GetField( &dbf,
                           tooltip_field_name_1.c_str() );
                        tooltip_field_value_1 = rtrim( &tooltip_field_value_1 );
                     }
                     strToolTip = tooltip_field_value_1;

                     tooltip_field_value_2.clear();
                     if ( !tooltip_field_name_2.empty() )
                     {
                        tooltip_field_value_2 = GetField( &dbf,
                           tooltip_field_name_2.c_str() );
                        tooltip_field_value_2 = rtrim( &tooltip_field_value_2 );
                     }

                     if ( !tooltip_field_value_2.empty() )
                     {
                        do
                        {
                           if ( !strToolTip.empty() )
                           {
                              if ( 0 == _stricmp( strToolTip.c_str(), tooltip_field_value_2.c_str() ) )
                                 break;
                              strToolTip += ", ";
                           }
                           strToolTip += tooltip_field_value_2.c_str();
                        } while ( FALSE );
                     }

                     // Concatenate the two status bar value fields into the help text (no dupes)
                     statbar_field_value_1.clear();
                     if ( !statbar_field_name_1.empty() )
                     {
                        statbar_field_value_1 = GetField( &dbf,
                           statbar_field_name_1.c_str() );
                        statbar_field_value_1 = rtrim( &statbar_field_value_1 );
                     }
                     strHelpText = statbar_field_value_1;

                     statbar_field_value_2.clear();
                     if ( !statbar_field_name_2.empty() )
                     {
                        statbar_field_value_2 = GetField( &dbf,
                           statbar_field_name_2.c_str() );
                        statbar_field_value_2 = rtrim( &statbar_field_value_2 );
                     }

                     if ( !statbar_field_value_2.empty() )
                     {
                        do
                        {
                           if ( !strHelpText.empty() )
                           {
                              if ( 0 == _stricmp( statbar_field_value_1.c_str(), statbar_field_value_2.c_str() ) )
                                 break;

                              strHelpText += ", ";
                           }
                           strHelpText += statbar_field_value_2.c_str();
                        } while ( FALSE );
                     }

                     // Concatenate all four tooltip and help text fields into the label (no dupes)
                     strLabel = strToolTip;     // First two fields already dupe purged

                     if ( !statbar_field_value_1.empty()
                        && 0 != statbar_field_value_1.compare( tooltip_field_value_1 )
                        && 0 != statbar_field_value_1.compare( tooltip_field_value_2 ) )
                     {
                        if ( !strLabel.empty() )
                           strLabel += ", ";
                        strLabel += statbar_field_value_1;
                     }

                     if ( !statbar_field_value_1.empty()
                        && 0 != statbar_field_value_2.compare( tooltip_field_value_1 )
                        && 0 != statbar_field_value_2.compare( tooltip_field_value_2 )
                        && 0 != statbar_field_value_2.compare( statbar_field_value_1 ) )
                     {
                        if ( !strLabel.empty() )
                           strLabel += ", ";
                        strLabel += statbar_field_value_2.c_str();;
                     }

                     // Make a single heap allocation for the 3 text items
                     ATLASSERT( shape.m_pchShapeStrings == nullptr
                        && "Shape string block shouldn't be allocated yet" );
                     shape.ClearStrings();

                     size_t c = shape.m_wHelpTextOffset = strToolTip.size() + 1;
                     size_t cBufferLen = c + strHelpText.size() + 1 + strLabel.size() + 1;
                     shape.m_pchShapeStrings = reinterpret_cast< LPCH > (
                        m_clsHeapAllocator.allocate( cBufferLen * sizeof(char) ) );

                     strcpy_s( shape.m_pchShapeStrings, cBufferLen, strToolTip.c_str() );
                     cBufferLen -= c;

                     strcpy_s( shape.m_pchShapeStrings + c, cBufferLen, strHelpText.c_str() );
                     c = strHelpText.size() + 1;
                     cBufferLen -= c;

                     shape.m_wLabelOffset = shape.m_wHelpTextOffset + c;
                     strcpy_s( shape.m_pchShapeStrings + shape.m_wLabelOffset, cBufferLen, strLabel.c_str() );

                  }  // Displayable
               }  // .dbf open
            }  // .dbf metadata needed

            if ( shape.displayOrder != SHAPE_NOT_DISPLAYED
                  && GEO_intersect( m_scrn_ll.lat, m_scrn_ll.lon,
                              m_scrn_ur.lat, m_scrn_ur.lon,
                              shape.m_grcBounds.ll.lat, shape.m_grcBounds.ll.lon,
                              shape.m_grcBounds.ur.lat, shape.m_grcBounds.ur.lon ) )
               m_vecShpObjects.push_back( it->second );

         }  // Cached shape objects loop
     
         if ( bDBFOpen )
         {
            dbf.CloseDbf();
            bDBFOpen = FALSE;
         }
      }  // for_each component/filename
      tier_index++;
   });  // for_each tier

   // Check for empty list;
   if ( m_vecShpObjects.empty() )
      m_type = -1;
   else
   {
      m_type = m_vecShpObjects[0]->nSHPType;
      sort(m_vecShpObjects.begin(), m_vecShpObjects.end(),
         CBaseShapeFileOverlay::SortShapesByDisplayOrder);
   }

   if (rslt != SUCCESS)
   {
      return FAILURE;
   }

   set_valid(TRUE);

   return SUCCESS;
}

MapScale BoundariesOverlay::get_smallest_scale()
{
   CString cs = PRM_get_registry_string("Boundaries Overlay",
      "BoundariesHideAbove", "NEVER");
   set_smallest_scale(MAP_get_scale_from_string( cs ) );
   return m_smallest_scale;
}

void BoundariesOverlay::set_smallest_scale(MapScale scale)
{
   m_smallest_scale = scale;
}

MapScale BoundariesOverlay::get_smallest_labels_scale()
{
   CString cs = PRM_get_registry_string("Boundaries Overlay",
      "LabelsHideAbove", "NEVER");

   set_smallest_labels_scale( MAP_get_scale_from_string( cs ) );
   return m_smallest_labels_scale;
}

void BoundariesOverlay::set_smallest_labels_scale(MapScale scale)
{
   m_smallest_labels_scale = scale;
}

MapScale BoundariesOverlay::get_largest_area_labels_scale()
{
   CString cs = PRM_get_registry_string("Boundaries Overlay",
      "AreaLabelsHideBelow", "NEVER");

   if ( cs == "NEVER" )
      cs = "ALWAYS";        // Reverse top and bottom sense
   else if ( cs == "ALWAYS" )
      cs = "NEVER";

   set_largest_area_labels_scale( MAP_get_scale_from_string( cs ) );
   return m_largest_area_labels_scale;
}

void BoundariesOverlay::set_largest_area_labels_scale( MapScale scale )
{
   m_largest_area_labels_scale = scale;
}

const string & BoundariesOverlay::get_name() const
{
   static const string name("Boundaries Overlay");
   return name;
}

BOOL BoundariesOverlay::IsAreaLabelEnabled( const SHPObject& shape ) const
{
   // Ignore "water/agua/d'eau" labels
   return !shape.IsLabelEmpty()
      && ( m_bWaterAreaLabels
         || 0 != _strnicmp( shape.GetLabel(), "water", 5 ) );
}


#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES

// *****************************************************************
// *****************************************************************

int BoundariesOverlay::check_localized_shapefiles()
{
   WIN32_FIND_DATAA find_file_data;

   // Look through all "tier" subfolders of the boundaries data
   HANDLE hTierFolder = FindFirstFileA( ( m_data_path + "tier*" ).c_str(), &find_file_data );
   if ( hTierFolder != INVALID_HANDLE_VALUE )
   {
      do
      {
         if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
         {
            // Look at all non-localized (no "_local_" in the name) shapefiles
            string strTierFolder = m_data_path + find_file_data.cFileName + "\\";
            HANDLE hShapefileFolder = FindFirstFileA( ( strTierFolder + "*.shp" ).c_str(), &find_file_data );
            if ( hShapefileFolder != INVALID_HANDLE_VALUE )
            {
               do
               {

                  if ( ( find_file_data.dwFileAttributes & ( FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY ) ) == 0
                     && NULL == strstr( find_file_data.cFileName, "_local_" ) )
                  {
                     // Remove the extension
                     find_file_data.cFileName[ strlen( find_file_data.cFileName ) - 4 ] = '\0';

                     // Extract any "_LoDn" tag
                     LPCH psz;
                     string strLoDTag;
                     if ( NULL != ( psz = strstr( find_file_data.cFileName, "_LoD" ) ) )
                     {
                        strLoDTag = psz;
                        *psz = '\0';
                     }

                     // Need to find at least one localized derived file
                     HANDLE hLocalizedShapefile = FindFirstFileA(
                        (strTierFolder + find_file_data.cFileName + "_local_*" + strLoDTag + ".shp" ).c_str(),
                        & find_file_data );
                     if ( hLocalizedShapefile == INVALID_HANDLE_VALUE )
                     {
#if 0 && defined _DEBUG
                        ATLTRACE( _T("Can't find localized file for %s%s\n"),
                           find_file_data.cFileName, strLoDTag.c_str() );
#endif
                        FindClose( hShapefileFolder );
                        FindClose( hTierFolder );
                        return FAILURE;
                     }
                     FindClose( hLocalizedShapefile );
                  }
               } while ( FindNextFileA( hShapefileFolder, &find_file_data ) );
               FindClose( hShapefileFolder );
            }
         }
      } while ( FindNextFileA( hTierFolder, &find_file_data ) );

      FindClose( hTierFolder );
   }

   return SUCCESS;
}  // check_localized_shapefiles()


// *****************************************************************
// *****************************************************************

int BoundariesOverlay::make_localized_shapefiles( __inout CString& error_txt )
{
   CWaitCursor waitCursor;
   WIN32_FIND_DATAA find_file_data;

   // Build descriptor for excluding shapefile elements in boundaries
   FileFieldExclusions mpFieldExclusions;

   FieldExclusionValues stExclusionValues;
   stExclusionValues.insert( "FN" );
   mpFieldExclusions[ "COUNTRY" ] = stExclusionValues;

   stExclusionValues.clear();
   stExclusionValues.insert( "water/agua/d'eau" );
   mpFieldExclusions[ "NAME" ] = stExclusionValues;

   // Look through all "tier" subfolders of the boundaries data
   HANDLE hTierFolder = FindFirstFileA( ( m_data_path + "tier*" ).c_str(), &find_file_data );
   if ( hTierFolder != INVALID_HANDLE_VALUE )
   {
      do
      {
         if ( ( find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
         {
            // Look at all non-localized (no "_local_" in the name) and non-LoD (no "_LoD") shapefiles
            string strTierFolder = m_data_path + find_file_data.cFileName + "\\";
            HANDLE hShapefileFolder = FindFirstFileA( ( strTierFolder + "*.shp" ).c_str(), &find_file_data );
            if ( hShapefileFolder != INVALID_HANDLE_VALUE )
            {
               do
               {
                  if ( ( find_file_data.dwFileAttributes & ( FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY ) ) == 0
                     && NULL == strstr( find_file_data.cFileName, "_local_" )
                     && NULL == strstr( find_file_data.cFileName, "_LoD" ) )
                  {
                     // Remove the extension
                     find_file_data.cFileName[ strlen( find_file_data.cFileName ) - 4 ] = '\0';

                     // Build all localized files for this component file
                     int i = extract_local_shape_files( strTierFolder.c_str(),
#if 0 && defined _DEBUG
                        "f:\\temp\\shp\\", find_file_data.cFileName, mpFieldExclusions, error_txt );
#else
                        strTierFolder.c_str(), find_file_data.cFileName, mpFieldExclusions, error_txt );
#endif

                  }
               } while ( FindNextFileA( hShapefileFolder, &find_file_data ) );
               FindClose( hShapefileFolder );
            }
         }
      } while ( FindNextFileA( hTierFolder, &find_file_data ) );

      FindClose( hTierFolder );
   }

   return SUCCESS;
}
// end of make_localized_shapefiles()

#endif
// ***************************************************************
// End of BoundariesOverlay.cpp
