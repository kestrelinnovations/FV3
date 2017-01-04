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



// gps.cpp

#include "stdafx.h"
#include "DataObject.h"
#include "param.h"
#include "file.h"
#include "err.h"
#include "nmea.h"
#include "PredictivePath.h"
#include "PredictivePathRenderer.h"
#include "PredictivePathOptionsPage.h"
#include "BullseyeOptionsPage.h"
#include "AuxDataOptionsPage.h"
#include "map.h"
#include "..\getobjpr.h"
#include "mov_sym.h"    // 3d viewer overlay
#include "..\SkyViewOverlay\skyview.h"
#include "..\SkyViewOverlay\factory.h"
#include "cdi.h"
#include "mem.h"
#include "gpstools.h"
#include "csttrack.h"
#include "rb.h"
#include "GPSTrailOverlayUtils.h"
#include "refresh.h"    // for FVW_refresh_rotation_state
#include "Utility\AlignWnd.h"   // for CAlignWnd
#include "MovingMapFeed.h"
#include "wm_user.h"
#include "LinksDlg.h"
#include "showrmk.h"
#include "factory.h"
#include "ovlElementContainer.h"
#include "FctryLst.h"
#include "..\overlay\OverlayCOM.h"
#include "..\StatusBarManager.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"
#include "MovingMapCameraEvents.h"
#include "FvCore/StdVectorIStream.h"


/////////////////////////////////////////////////////////////////////////////
// Definitions
//
#define GPS_NULL_TYPE -1
#define GPS_RMC_TYPE 1
#define GPS_GGA_TYPE 2
#define GPS_GLL_TYPE 3
#define GPS_VTG_TYPE 4

// file version defines
#define MAJOR_VERSION 3
#define MINOR_VERSION 20
#define GPS_REVISION 5

const UINT wm_Connect = RegisterWindowMessage("FVW_GPS_CONNECT");
const UINT wm_Disconnect = RegisterWindowMessage("FVW_GPS_DISCONNECT");

/////////////////////////////////////////////////////////////////////////////
// STATIC STORAGE
/////////////////////////////////////////////////////////////////////////////
boolean_t      C_gps_trail::m_edit_on = FALSE;
int            C_gps_trail::m_toolbar_id = IDR_GPS_TB;
CCourseIndicator *C_gps_trail::m_cdi=NULL;

int            C_gps_trail__registry_options_state=0;
GPS_options_t  C_gps_trail::m_gps_options;
boolean_t      C_gps_trail::m_need_to_catch_up=FALSE;

int            C_gps_trail::m_radius=6;                     // trail icon radius
int            C_gps_trail::m_playback_rate=1;
GPSToolsPropertySheet C_gps_trail::m_gpstools_dialog;

long C_gps_trail::s_skyview_object_handle = -1;
ISampledMotionPlaybackPtr C_gps_trail::s_sampled_motion_playback;
bool C_gps_trail::s_sv_playback_time_advancement_enabled = false;
double C_gps_trail::s_sv_playback_time_advancement_multiplier = 0.0;
CLinksEditDlg* C_gps_trail::m_pLinksEditDlg = NULL;
GPSPointIcon*  C_gps_trail::m_pLinksEditPoint = NULL;

CLSID          C_gps_trail::s_clsidPngEncoder = CLSID_NULL;
C_gps_trail::InterlockData C_gps_trail::s_idThreadInterlock;

///////////////

//toolbar button toggles
boolean_t      C_gps_trail::m_TautoCenter=FALSE;
boolean_t      C_gps_trail::m_TautoRotate=FALSE;
boolean_t      C_gps_trail::m_TtrailPoints=FALSE;
boolean_t      C_gps_trail::m_Tcdi=FALSE;
boolean_t      C_gps_trail::m_Tplayback=FALSE;
boolean_t      C_gps_trail::m_TcontinuousCentering=FALSE;

double C_gps_trail::m_rotation_frac_pos_x = 0.0;
double C_gps_trail::m_rotation_frac_pos_y = 0.33333333;

CList<HWND, HWND&> C_gps_trail::m_hwnd_connect_notify_lst;
int C_gps_trail::m_connected_handle = -1;

static const IID IID_IGPSFeed =
{ 0x72305D81, 0x56CA, 0x4796, { 0x8F, 0x12, 0xA0, 0x8B, 0x1D, 0x8F, 0x58, 0xE4 } };

/////////////////////////////////////////////////////////////////////////////
// STATIC STORAGE END
/////////////////////////////////////////////////////////////////////////////

SymbolContainer *SymbolContainer::s_pSymbolContainer = NULL;

// constructor
SymbolContainer::SymbolContainer()
{
   // read in the symbols found in the symbol directory
   // and store them in a list
   CString symbol_path = PRM_get_registry_string("Main","HD_DATA");
   symbol_path += "\\symbols\\";
   WIN32_FIND_DATA find_file_data;
   HANDLE handle = FindFirstFile(symbol_path + "*.sym", &find_file_data);

   if (handle != INVALID_HANDLE_VALUE)
   {
      do 
      {
         OvlSymbol *new_symbol = new OvlSymbol();
         new_symbol->read_from_file(symbol_path + find_file_data.cFileName);
         m_symbol_lst.AddTail(new_symbol);
      } while(FindNextFile(handle, &find_file_data));

      FindClose(handle);
   }
   else
      AfxMessageBox("No symbols were found in the symbols directory.  Please re-install FalconView!");
}

OvlSymbol *SymbolContainer::get_symbol(int i)
{
   POSITION position = m_symbol_lst.FindIndex(i);
   if (position)
      return m_symbol_lst.GetAt(position);
   return NULL;
}

int SymbolContainer::define_symbol(OvlSymbol *sym_to_set, CString sym_name)
{
   POSITION position = m_symbol_lst.GetHeadPosition();
   boolean_t found = FALSE;
   while (position)
   {
      OvlSymbol *symbol = m_symbol_lst.GetNext(position);
      if (symbol->get_name() == sym_name)
      {
         sym_to_set->make_copy_from(symbol);
         return SUCCESS;
      }
   }

   return FAILURE;
}

// destructor
SymbolContainer::~SymbolContainer()
{
   while (!m_symbol_lst.IsEmpty())
      delete m_symbol_lst.RemoveHead();
}


////////////////////////////////////////////////////////////////////////////////
//
// C_gps_trail
//
////////////////////////////////////////////////////////////////////////////////

// static
boolean_t C_gps_trail::get_toggle( long key )
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (trail == NULL)
      return FALSE;

   switch(key)
   {
   case 'CCEN': return m_TcontinuousCentering; break;
   case 'ANIM': return TRUE; break;
   case 'CONN': return trail->is_connected();
   case 'ACEN': return m_TautoCenter; break;
   case 'AROT': return m_TautoRotate; break;
   case 'TRAI': return m_TtrailPoints;break;
   case 'CDI' : return m_Tcdi;        break;
   case 'STYL': return FALSE; break;
   case 'OPTI': return FALSE; break;
   default    : return FALSE; break;
   };
}

// static
boolean_t C_gps_trail::set_toggle( long key )
{
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = NULL;
   
   switch(key)
   {
   case 'CCEN':
      m_TcontinuousCentering = !m_TcontinuousCentering;
      
      if (m_TcontinuousCentering)
      {
         if (!get_toggle('ACEN'))//is off
            set_toggle('ACEN');  //turn on
         if (!get_toggle('ANIM'))//is off
            set_toggle('ANIM');  //turn on
      }
      else
      {
         if (get_toggle('ANIM')) //is on
            set_toggle('ANIM');  //turn off
      }
      
      break;
      
   case 'ANIM':
      
      break;
      
   case 'CONN':
      {
         C_overlay *trail = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail);

         // if there is no gps trail overlay created, create it now
         if (trail == NULL)
         {
            if (OVL_get_overlay_manager()->create(FVWID_Overlay_MovingMapTrail, &trail) != SUCCESS)
            {
               ERR_report("Error creating new moving map overlay");
               return FALSE;
            }
            const boolean_t invalidate = FALSE;
            OVL_get_overlay_manager()->make_current(trail, invalidate);
         }

         ASSERT(trail->get_m_overlayDescGuid() == FVWID_Overlay_MovingMapTrail);
         (dynamic_cast<C_gps_trail *>(trail))->toggle_connection();

         break;
      }
      
   case 'ACEN':
      m_TautoCenter = !m_TautoCenter;
      if (!m_TautoCenter)
      {
         if (get_toggle('AROT')) //is on
            set_toggle('AROT');  //turn off
         if (get_toggle('CCEN')) //is on
            set_toggle('CCEN');  //turn off
      }
      
      if (m_TautoCenter)
         m_gps_options |= GPS_AUTO_CENTER;
      else
         m_gps_options &= ~GPS_AUTO_CENTER;
      
      // GEORGE
      
      //need access to the view map
      if (view)
      {
         map = UTL_get_current_view_map(view);
         get_topmost_trail()->set_valid(FALSE);
         reset_auto_center_and_rotate(map, m_TautoCenter);
      }
      
      break;
      
   case 'AROT':
      m_TautoRotate = !m_TautoRotate;
      if (m_TautoRotate)
      {
         if (!get_toggle('ACEN')) //is off
            set_toggle('ACEN');   //turn on
      }
      if (m_TautoRotate)
         m_gps_options |= GPS_AUTO_ROTATE;
      else
         m_gps_options &= ~GPS_AUTO_ROTATE;
      
      // GEORGE
      //need access to the view map
      if (view)
      {
         map = UTL_get_current_view_map(view);
         get_topmost_trail()->set_valid(FALSE);
         reset_auto_center_and_rotate(map, m_TautoRotate);
      }
      
      break;
      
   case 'TRAI':
      m_TtrailPoints= !m_TtrailPoints;
      
      if (C_gps_trail::m_TtrailPoints)
         m_gps_options |= GPS_LEAVE_TRAIL;
      else
         m_gps_options &= ~GPS_LEAVE_TRAIL;
      
      invalidate_trails();
      break;
      
   case 'CDI' :
      m_Tcdi        = !m_Tcdi;
      if (m_cdi)
         if (m_Tcdi)
            m_cdi->ShowWindow(SW_SHOW);
         else
            m_cdi->ShowWindow(SW_HIDE);
         break;
   case 'STYL':
      //      m_styleDlg.DoModal();
      break;
      
   case 'OPTI':
      open_options_dialog();
      break;
      
   default    :
      return FALSE;
      break;
   };
   return TRUE;
}

// toggle the gps connection (connect/disconnect)
int C_gps_trail::toggle_connection()
{
   ASSERT(m_moving_map_feed);

   // insure that we are in GPS mode when connecting (it is possible to 
   // use the accelerator Ctrl+Q when outside GPS mode)

   OVL_get_overlay_manager()->set_mode(FVWID_Overlay_MovingMapTrail);

   m_moving_map_feed->ToggleConnection();

   if (m_moving_map_feed->is_connected() == FALSE)
   {
      m_pPredictivePathTimer->stop();

      m_pFrozenTrailTimer->stop();
      stop_frozen_trail_warning();
   }
   else
   {
      m_pFrozenTrailTimer->restart();
   }

   return SUCCESS;
}

BOOL C_gps_trail::is_connected()
{
   ASSERT(m_moving_map_feed);

   return m_moving_map_feed->is_connected();
}


void C_gps_trail::power_broadcast( UINT nPowerEvent, UINT nEventData )
{
   switch ( nPowerEvent )  // Type of notification
   {
      case PBT_APMSUSPEND: // About to suspend
         if ( !m_bSuspendedConnection && is_connected() )
         {
            toggle_connection();   // Disconnect before suspend
            m_bSuspendedConnection = TRUE;
         }
         break;

      case PBT_APMRESUMEAUTOMATIC:
      case PBT_APMRESUMESUSPEND:
         if ( m_bSuspendedConnection )  // Resuming after suspended connection
         {
            //Sleep(2000);
            toggle_connection(); // Reconnect
            m_bSuspendedConnection = FALSE;
         }
   };
}


// static
void C_gps_trail::open_options_dialog()
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (trail == NULL)
      return;

   trail->open_options_dialog_hlpr();
}

void C_gps_trail::open_options_dialog_hlpr()
{
   ASSERT(m_moving_map_feed);

   if (m_moving_map_feed->Options() != SUCCESS)
      ERR_report("Error displaying moving map feed's option dialog");
}

void C_gps_trail::reset_auto_center_and_rotate(ViewMapProj *map, boolean_t flagOn)
{
   if (flagOn)
   {
      C_gps_trail *pTrail = get_topmost_trail();
      
      if (pTrail)
         pTrail->map_update(map, TRUE);
   }
   else
   {
      if (map->actual_rotation() != 0.0)
      {
         //
         // set the rotation to 0
         //
         CView *view = UTL_get_active_non_printing_view();
         
         if (UTL_change_view_map(view, map->source(), map->scale(), map->series(),
            map->actual_center_lat(), map->actual_center_lon(),
            0.0, map->actual_zoom_percent(), map->projection_type()) == SUCCESS)
            
         {
            OVL_get_overlay_manager()->invalidate_all(TRUE);
         }
      }
   }

   FVW_refresh_rotation_state();
}

// constructor
C_gps_trail::C_gps_trail() : 
   CFvOverlayPersistenceImpl(""),
   m_rect(0,0,0,0),
   m_invalidate_rect(0,0,0,0),
   m_camera_events(nullptr),
   m_ptShip3D( this ),
   m_dwLastShip3DUpdateTicks( 0 )
{
}

HRESULT C_gps_trail::InternalInitialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   // One-time initialization
   if ( s_clsidPngEncoder == CLSID_NULL
         && 0 > GetPngEncoderClsid() )
      return E_FAIL;

   // initialize the default propertes object from the registry
   m_properties.initialize_from_registry("GPS Options");

   if (m_properties.GetAutoColorIncrement())
      m_properties.set_ship_fg_color(CGPSProperties::GetNextAutoColor());

   m_pAutoSaveTimer = new CMovingMapAutoSaveTimer(this, AUTO_SAVE_TIMEOUT);
   m_pPredictivePathTimer = new CPredictivePathTimer(this, PREDICTIVE_PATH_TIMEOUT);
   m_pFrozenTrailTimer = new CFrozenTrailTimer(this, m_properties.get_frozen_feed_timeout() * 1000);
   m_p3DCatchUpTimer = new C3DCatchUpTimer( this, THREE_D_RETRY_TIMEOUT );

   // start auto-save timer if necessary
   if (m_properties.get_auto_save_trail())
      UTL_get_timer_list()->add(m_pAutoSaveTimer);

   UTL_get_timer_list()->add(m_pPredictivePathTimer);
   UTL_get_timer_list()->add(m_pFrozenTrailTimer);
   UTL_get_timer_list()->add( m_p3DCatchUpTimer );

   // create the ship symbol
   m_shipSymbol = new OvlSymbol;
   // create a ship symbol duplicate for use in the Vertical Display...
   m_shipSymbol_2 = new OvlSymbol;
   m_ptShip3D.m_ePointType = GPS3DPoint::GPSPT_SHIP;

   m_last_hit = NULL;
   m_selected_point = NULL;
   m_north_up_angle = (float)0.0;
   set_modified(FALSE);
   m_next_point_type = GPS_NULL_TYPE;
   
   m_current_point = m_next_point;
   m_current_point.m_overlay = this;   // point for current position icon
   m_next_point.m_overlay = this;      // point for the next point added to the
   // list
   // used to enable and disable show_communication output
   m_bad_sentence_count = 50;
   
   // used to indicate that show_communication has the remark dialog
   m_show_communications = FALSE;
   
   m_moving_map_echo_file = PRM_get_registry_string("Main", "USER_DATA");
   m_moving_map_echo_file += "\\gps\\moving map.txt";
   
   m_all_input_echo_file = PRM_get_registry_string("Main", "USER_DATA");
   m_all_input_echo_file += "\\gps\\gps echo.txt";
   
   if (C_gps_trail__registry_options_state==0)
      load_registry_settings();

   // default to dont try to communicate with skyview (mov_sym_overlay)
   m_update_3d_camera = FALSE;
   
   m_playback_head = NULL;
   m_playback_tail = NULL;
   
   if (!m_cdi)
   {
      //crank up a cdi
      m_cdi = new CCourseIndicator;
   }
   
   m_rollover = COleDateTimeSpan(0);
   m_best_date.SetTime(0,0,0);
   m_need_to_catch_up = FALSE;
   
   //GPS III
   m_gll_sentence_count = 0;
   
   //clear the display list
   display_list_reset();
   m_display_list_valid = false;
   
   //scroll track
   set_scrolling_track( 0.5, 0.5, 0 ); //center, track-up
   
   m_used_external_client_feed = FALSE;
   
   // create the coast track and range bearing managers
   m_coast_track_mgr = new CoastTrackManager;
   m_rb_mgr = new RangeBearingManager;
   m_dragger = new RangeBearingDragger;
   
   // tell the coast track manager and the range and bearing manager
   // who the parent overlay is
   m_coast_track_mgr->set_parent_overlay(this);
   m_rb_mgr->set_parent_overlay(this);
   
   m_is_closing = FALSE;
   m_playback_slide = FALSE;
   
   m_comment_dlg_up = FALSE;
   m_update_coast_tracks = FALSE;

   m_moving_map_feed = new MovingMapFeed(this);

   fvw::IMovingMapFeedAuxiliaryDataPtr auxData;
   if(GetMovingMapFeed()->m_pMovingMapFeed && GetMovingMapFeed()->m_pMovingMapFeed->QueryInterface(__uuidof(fvw::IMovingMapFeedAuxiliaryData), (void**)&auxData) == S_OK)
   {
      CAuxDataProperties* auxProp = m_properties.GetAuxDataProperties();

      long val = 0;
      auxData->GetNumAuxTypes(&val);
      auxProp->set_num_values((int)val);
      for(int i = 0; i < val; i++)
      {
         _bstr_t displayName;
         long enabled;
         auxData->GetAuxDataType(i, displayName.GetAddress(), &enabled);
         auxProp->set_value(i,enabled);
      }
   }

   DefineShipSymbols();

   m_deg_per_pixel.lat = 0;
   m_deg_per_pixel.lon = 0;

   m_data_object = new GPSDataObject(this);

   m_show_symbol = TRUE;

   m_warning_displayed = FALSE;
   m_halt_warning_thread = TRUE;
   m_halt_warning_thread_event = NULL;
   m_warning_thread_started = FALSE;

   m_tracking_center_num = 0;

   m_bSuspendedConnection = FALSE;      // Not in power suspension
   m_pPredictivePath = new CPredictivePath();

   CComObject<MovingMapCameraEvents>::CreateInstance(&m_camera_events);
   m_camera_events->AddRef();
   m_camera_events->Initialize(this);

   return S_OK;
}

C_gps_trail::~C_gps_trail()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object
   // may not be  released until the next garbage collection.
   m_camera_events->Release();
}

void C_gps_trail::Finalize()
{
   //shut the communication window if necessary
   CloseCommDlg();
   
   //was this the only lonesome trail
   
   if (get_trail_count() == 0)
   {
      if (m_cdi)
      {
         delete m_cdi;
         m_cdi = NULL;
      }
   }
   
   delete m_rb_mgr;
   m_rb_mgr = NULL;
   delete m_coast_track_mgr;
   m_coast_track_mgr = NULL;
   delete m_dragger;
   m_dragger = NULL;
   delete m_shipSymbol;
   m_shipSymbol = NULL;
   delete m_shipSymbol_2;
   m_shipSymbol_2 = NULL;

   POSITION pos = UTL_get_timer_list()->Find(m_pAutoSaveTimer);
   if ( pos != NULL )
      UTL_get_timer_list()->remove_at( pos );
   delete m_pAutoSaveTimer;
   m_pAutoSaveTimer = NULL;

   pos = UTL_get_timer_list()->Find(m_pPredictivePathTimer);
   if ( pos != NULL )
      UTL_get_timer_list()->remove_at( pos );
   delete m_pPredictivePathTimer;
   m_pPredictivePathTimer = NULL;

   pos = UTL_get_timer_list()->Find(m_pFrozenTrailTimer);
   if ( pos != NULL )
      UTL_get_timer_list()->remove_at( pos );
   delete m_pFrozenTrailTimer;
   m_pFrozenTrailTimer = NULL;
   
   save_registry_settings();

   delete m_moving_map_feed;
   m_moving_map_feed = NULL;
   delete m_data_object;
   m_data_object = NULL;

   m_icon_list.Clear();

   m_aPredictivePoints3D.clear();   // Remove predictive points in 3D
   delete m_pPredictivePath;
   m_pPredictivePath = NULL;

   m_ptShip3D.RemoveFrom3D();

   pos = UTL_get_timer_list()->Find( m_p3DCatchUpTimer );
   if ( pos != NULL)
      UTL_get_timer_list()->remove_at( pos );
   delete m_p3DCatchUpTimer;
   m_p3DCatchUpTimer = NULL;
   
}

void C_gps_trail::set_modified(boolean_t mod_flag)
{
   C_overlay::set_modified(mod_flag);

   if (mod_flag && g_tabular_editor_dlg)
      g_tabular_editor_dlg->UpdateData(FALSE);
}

void C_gps_trail::set_properties(CGPSProperties *properties)
{
   CMainFrame *frame = fvw_get_frame();
   const int overlay_handle = OVL_get_overlay_manager()->get_overlay_handle(this);

   const int oldFeedType = m_properties.get_feed_type(); 
   m_properties = *properties; 

   if (m_moving_map_feed->is_connected())
      m_properties.set_feed_type(oldFeedType);

   // add or remove the auto save timer from the timer list if necessary
   POSITION autoSaveTimerPos = UTL_get_timer_list()->Find(m_pAutoSaveTimer);
   if (m_properties.get_auto_save_trail() == FALSE && autoSaveTimerPos != NULL)
   {
      UTL_get_timer_list()->remove_at(autoSaveTimerPos);
   }
   else if (m_properties.get_auto_save_trail() && autoSaveTimerPos == NULL)
   {
      m_pAutoSaveTimer->reset();
      UTL_get_timer_list()->add(m_pAutoSaveTimer);
   }

   m_moving_map_feed->on_feed_changed();

   // the timer may have changed, reset it here if we are currently connected
   if (m_moving_map_feed->is_connected())
   {
      m_pFrozenTrailTimer->stop();
      int timeout = m_properties.get_frozen_feed_timeout();
      if (timeout != -1)
      {
         m_pFrozenTrailTimer->set_period(timeout * 1000);
         m_pFrozenTrailTimer->start();
      }
   }

   DefineShipSymbols();
   set_valid(FALSE);
}

TabularEditorDataObject* C_gps_trail::GetTabularEditorDataObject()
{
   return m_data_object;
}

// toggle the tabular editor for this overlay
void C_gps_trail::OnTabularEditor()
{
   if (g_tabular_editor_dlg != NULL)
      CTabularEditorDlg::destroy_dialog();

   else
      CTabularEditorDlg::create_dialog(m_data_object);
}

GPSPointIcon *C_gps_trail::get_at_index(long i)
{
   POSITION position = get_icon_list()->find_index(i);
   if (position)
      return get_icon_list()->get_next(position);

   return NULL;
}

int C_gps_trail::get_index(GPSPointIcon *point)
{
   GPSPointIcon *current = get_icon_list()->get_first();
   int index = 0;
   while(current)
   {
      if (current == point)
         return index;

      current = get_icon_list()->get_next();
      index++;
   }

   return -1;
}


// returns TRUE if the info box is up and it was put there
// by this overlay
boolean_t C_gps_trail::get_info_box_up()
{
   // If the info box is up and it was put up by this gps trail, then return
   // TRUE.  Otherwise return FALSE.  This is used to determine if the
   // Route Point Information box is up and needs updating.
   return (this == CRemarkDisplay::get_focus_overlay());
}

int C_gps_trail::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   C_icon *hit_point;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   
   // display information for the selected point
   hit_point = hit_test( &mapProjWrapper, point);
   
   // if no point was hit, then this function will do nothing
   if (hit_point == NULL)
   {
      // if a point was selected then it needs to be unselected
      if (m_selected_point != NULL)
      {
         // invalidate the old selected point
         invalidate_object(m_selected_point, TRUE);
         
         // set the selection status of the old selected point to FALSE
         m_selected_point->set_selected(FALSE);
         m_shipSymbol->set_selected(FALSE);
         
         // there is no longer a selected point
         m_selected_point = NULL;
         
         // redraw the current overlay
         set_valid(FALSE);
         
         // ??
         if (!get_toggle('CCEN'))
            OVL_get_overlay_manager()->invalidate_all();
      }
      
      return FAILURE;
   }
   
   // process left-clicking a coast track
   if (hit_point->is_kind_of("CoastTrack"))
   {
      // toggle the selection status of this coast track
      m_coast_track_mgr->toggle_selection((CoastTrack *)hit_point);
      
      return SUCCESS;
   }
   
   // process left-clicking a range/bearing object
   else if (hit_point->is_kind_of("RangeBearing"))
   {
      // allow the dragging of range/bearing objects
      //*drag = TRUE;
      
      //m_rb_object_index = m_rb_mgr->get_rb_index((RangeBearing *)hit_point);
      //RangeBearingData data = ((RangeBearing *)hit_point)->get_data();
      //m_old_rb_location.lat = data.m_lat;
      //m_old_rb_location.lon = data.m_lon;

      //d_geo_t anchor;
      //map->surface_to_geo(point.x, point.y, &anchor.lat, &anchor.lon);
      
      //m_dragger->set_anchor(anchor, RANGE_BEARING_RADIUS);
      //m_dragger->draw(map, point);
      
      return SUCCESS;
   }
   
   // process left click of GPS Point icon
   else if (hit_point->is_kind_of("GPSPointIcon"))
   {
      // if not current, make it current (but only if we are in the GPS editor)
      if (!get_current() && m_edit_on)
      {
         if (OVL_get_overlay_manager()->make_current(this) != SUCCESS)
         {
            ERR_report("make_current() failed.");
            return FAILURE;
         }
      }
      
      // invalidate the previous selected point (if one)
      if (m_selected_point != NULL)
      {
         invalidate_object(m_selected_point, TRUE);
         m_selected_point->set_selected(FALSE);
      }
      
      // set the newly selected point
      m_selected_point = (GPSPointIcon *)hit_point;
      m_selected_point->set_selected(TRUE);
      m_shipSymbol->set_selected(TRUE);
      
      // invalidate the location of the newly selected point
      invalidate_object(m_selected_point, TRUE);

      set_link_edit_dlg_focus(m_selected_point);
      
      // popup the info box for the selected point - the info dialog box should
      // only be displayed if we are not in edit mode
      if (!m_edit_on)
         get_point_info(OVL_get_overlay_manager()->get_view_map(), hit_point);
      
      // redraw the current overlay
      set_valid(FALSE);
      
      if (!get_toggle('CCEN'))
         OVL_get_overlay_manager()->invalidate_all();
      
      return SUCCESS;
   }
   
   return FAILURE;
}

// This function sets the cursor and hint to give the user feedback 
// indicating what will happen if the selected() member is called with the
// same view, point, flags.
int C_gps_trail::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   IFvStatusBarManager *pStatusBarMgr = pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager();

   C_icon *icon = hit_test(&mapProjWrapper, point);
   
   if (icon == NULL)
   {
      int point_list_length;

      // no points, no help text
      get_point_list_length(&point_list_length);
      if (point_list_length >= 1)
      {
         // a default hint has no tool tip, just help text
         pStatusBarMgr->SetStatusBarHelpText(_bstr_t(m_current_point.get_help_text()));
      }

      return FAILURE;
   }
   
   if (icon->is_kind_of("CoastTrack"))
   {
      // set cursor to select cursor
      pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      pMapView->SetTooltipText(_bstr_t(icon->get_tool_tip()));
      pStatusBarMgr->SetStatusBarHelpText(_bstr_t(icon->get_help_text()));
   }  
   else if (icon->is_kind_of("RangeBearing"))
   {
      // set cursor to select cursor
      pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
      pMapView->SetTooltipText(_bstr_t(icon->get_tool_tip()));
      pStatusBarMgr->SetStatusBarHelpText(_bstr_t(icon->get_help_text()));
   }
   else if (icon->is_kind_of("GPSPointIcon"))
   {
      GPSPointIcon *gps_icon = (GPSPointIcon *)icon;

      // set cursor to select cursor
      pMapView->SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    
      // set the tool tip of the gps point to the point's time and date.
      // if there is no date, then only time will appear
      if (gps_icon == &m_current_point)
         pMapView->SetTooltipText(_bstr_t(m_properties.get_nickname_label()));
      else
         pMapView->SetTooltipText(_bstr_t(gps_icon->get_tool_tip()));

      pStatusBarMgr->SetStatusBarHelpText(_bstr_t(icon->get_help_text()));
   }

   // tell the view that the cursor is over a moving object so that
   // the tool-tip will not constantly be erased and redrawn
   MapView *pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView != NULL)
      pView->set_tooltip_over_moving_object();
   
   return SUCCESS;
}

HCURSOR C_gps_trail::get_default_cursor()
{
   return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

// Allows the overlay manager to pass double-click notices to the gps overlay,
// If the point hits a coast track, then the coast track data dialog will
// be displayed
int C_gps_trail::on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   C_icon *icon = hit_test(&mapProjWrapper, point);
   
   // if the user double clicked a coast track then open up the data dialog
   if (icon != NULL && icon->is_kind_of("CoastTrack"))
   {
      CoastTrack *ctrack = (CoastTrack *)icon;
      int index = m_coast_track_mgr->get_ctrack_index(ctrack);
      
      if (index != -1)
      {
         m_coast_track_mgr->modify_coast_track(index);
         return SUCCESS;
      }
   }

   return FAILURE;
}

/////////////////////////////////////////////////////////////////////////////
// add menu items to the menu 
//
void C_gps_trail::menu(ViewMapProj* vmapproj, CPoint point,
                       CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   map_projection_utils::CMapProjWrapper map(vmapproj->GetSettableMapProj());

   C_icon *c_icon = hit_test(&map, point);
   
   if (c_icon == NULL)
      return;
   
   if (c_icon->is_kind_of("GPSPointIcon"))
   {
      GPSPointIcon *icon = (GPSPointIcon *)c_icon;
      
      list.AddTail(new CCIconMenuItem("Get Moving Map Point Info...",
         icon, &get_point_info));
      
      // add the GPS Comment menu items if the last_hit GPS point is not
      // the current ship
      if (icon != &m_current_point && m_edit_on)
      {
         if (!icon->has_comment())
            list.AddTail(new CCIconMenuItem("Add Comment...",
            icon, &add_comment));
         else
         {
            list.AddTail(new CCIconMenuItem("Edit Comment...",
               icon, &edit_comment));
            
            list.AddTail(new CCIconMenuItem("Delete Comment",
               icon, &delete_comment));
         }

         list.AddTail(new CCIconMenuItem("Edit Point Links...", icon, &edit_gps_point_links_callback));
      }

      m_links.RemoveAll();

      // add the links to the menu
      CStringArray links;
      icon->GetLinks(links);

      const int nSize = links.GetSize();
      for (int i = 0; i < nSize; i++)
      {
         CString text;

         text = "Links\\" + links[i];
         list.AddTail(new CCIconLparamMenuItem((LPCSTR)text, c_icon, &view_link_callback, i));
      }

      m_links.Append(links);
      
      if (is_3D_camera_available()) 
      {
         // append the name of the route to the menu items so they are distinguishable
         CString trail_name;
         trail_name.Format(" [%s]", OVL_get_overlay_manager()->GetOverlayDisplayName(this));

         if (m_update_3d_camera)
         {
            list.AddTail(new CLparamMenuItem("Detach Camera" + trail_name,
               (LPARAM)FALSE, &enable_3d_camera_update));
         }
         else
         {
            list.AddTail(new CLparamMenuItem("Attach Camera" + trail_name,
               (LPARAM)TRUE, &enable_3d_camera_update));
         }
      }
   }
   else if (c_icon->is_kind_of("CoastTrack"))
   {
      int handle = OVL_get_overlay_manager()->get_overlay_handle(this); 
      
      list.AddTail(new CCIconLparamMenuItem("Stop", c_icon, &stop_coast_track, (LPARAM)handle));
      list.AddTail(new CCIconLparamMenuItem("Edit", c_icon, &edit_coast_track, (LPARAM)handle));
      list.AddTail(new CCIconLparamMenuItem("Delete", c_icon, &delete_coast_track, (LPARAM)handle));
   }
}

bool C_gps_trail::is_3D_camera_available()
{
   if (skyview_overlay::IsOverlayTypeEnabled())
   {
      // is there an active moving symbol overlay
      return Cmov_sym_overlay::GetStateIndicators()->m_symbol != NULL;
   }
   else
   {
      MapView* map_view = fvw_get_view();
      if (map_view)
         return map_view->IsProjectionSupported(GENERAL_PERSPECTIVE_PROJECTION);
   }

   return false;
}

int C_gps_trail::process_sentence(const char *line)
{
   // Increment the bad sentence count each time a sentence is received.  If
   // this sentence contains a valid fix m_bad_sentence_count will be set to
   // 0 by the function that processes it.  If this line contains a valid
   // VTG sentence m_bad_sentence_count will be decremented by the function
   // that processes it.
   m_bad_sentence_count++;
   
   // echo's all GPS input to "gps echo.txt" if this is the moving map display
   // trail and echo is on
   all_input_echo(line);
   
   // test for National Marine Electronics Association sentence
   if (NMEA_sentence::NMEA_test(line) == FALSE)
   {
      show_communication("Non-NMEA compliant sentence received.");
      return FAILURE;
   }
   
   //are we behind?
   C_gps_trail::m_need_to_catch_up = FALSE;
   
   // test for RMC Lat-Lon sentence
   if (strncmp(line, "$GPRMC", 6) == 0)
      return process_RMC_sentence(line);
   
   // test for GGA Lat-Lon sentence
   if (strncmp(line, "$GPGGA", 6) == 0)
      return process_GGA_sentence(line);
   
   // test for GLL Lat-Lon sentence
   if (strncmp(line, "$GPGLL", 6) == 0)
      return process_GLL_sentence(line);
   
   // test for VTG sentence
   if (strncmp(line, "$GPVTG", 6) == 0)
      return process_VTG_sentence(line);
   
   show_communication("Unrecognized NMEA compliant sentence received.");
   
   return SUCCESS;
}


int C_gps_trail::get_point_list_length(int *length)
{
   *length = get_icon_list()->get_count();
   return SUCCESS;
}


int C_gps_trail::clear_trail(void)
{
   // remove the info box if this overlay has its focus
   if (m_selected_point)
   {
      CloseCommDlg();
      
      m_selected_point = NULL;
   }
   
   // remove all points from trail
   get_icon_list()->delete_all();
   get_icon_list()->set_trail_end(NULL);
   
   // reset the current position point to its unknown state
   m_current_point.reset_point();
   //m_current_point = NULL;
   
   // reset other member to initial values
   m_last_hit = NULL;
   m_north_up_angle = (float)0.0;
   set_modified(FALSE);
   m_next_point_type = GPS_NULL_TYPE;
   
   // remove trail and icon from screen
   if (!m_rect.IsRectEmpty())
   {
      set_valid(FALSE); // redraw from scratch
      OVL_get_overlay_manager()->invalidate_rect(m_rect, FALSE);
   }
   
   return SUCCESS;
}

int C_gps_trail::save_as(const CString &filespec, long nSaveFormat)
{
   // if we opened this file in the old format then save it in the old format
   CString ext = filespec.Right(4);
   if (ext.CompareNoCase(".gpx") == 0)
   {
      try
      {
         int ret = save_as_gpx(filespec);
         if (ret == SUCCESS)
         {
            m_fileSpecification = filespec;

            // set modified flag
            set_modified(FALSE);
         }
         
         return ret;
      }
      catch (_com_error& e)
      {
         REPORT_COM_ERROR(e);
         return FAILURE;
      }
   }
   else if (ext.CompareNoCase(".gps") == 0)
   {
      int result = IDNO;

      // if there are any range/bearing objects, coast tracks, or comments
      // then warn the user that these can't be saved in a .gps file
      const bool range_bearing_objects_exist = (m_rb_mgr->get_count() > 0);
      const bool coast_track_objects_exist = (m_coast_track_mgr->get_count() > 0);
      const bool comments_exist = (get_number_comments() > 0);
      bool links_exist = false;

      GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
      while (point != NULL)
      {
         if (point->GetLinksCount() > 0)
         {
            links_exist = true;
            break;
         }
         point = (GPSPointIcon *)get_icon_list()->get_next();
      }

      CString message("There are ");

      CStringArray arrElements;
      arrElements.Add("Properties");
      if (comments_exist)
         arrElements.Add("Moving Map comments");
      if (range_bearing_objects_exist)
         arrElements.Add("Range/Bearing objects");
      if (coast_track_objects_exist)
         arrElements.Add("Coast Track objects");
      if (links_exist)
         arrElements.Add("Links");

      const int nCount = arrElements.GetSize();
      if (nCount == 1)
         message += arrElements[0];
      else if (nCount == 2)
         message += arrElements[0] + " and " + arrElements[1];
      else if (nCount > 2)
      {
         for(int i=0;i<nCount-1;++i)
            message += arrElements[i] + ", ";
         message += "and " + arrElements[nCount-1];
      }

      message += " that cannot be saved to a ASCII .gps file.  Would you like to save to a binary .gpb file instead?";

      result = AfxMessageBox(message, MB_YESNO);

      if (result == IDNO)
         return old_save_as(filespec);

      return OVL_get_overlay_manager()->save_as(this);
   }

   CFile file;
   CFileException ex;

   // get the number of bytes required to store the gps data to disk
   int block_size = get_block_size();

   // allocate block
   BYTE *block = new BYTE[block_size];
   BYTE *block_ptr = block;

   // get the gps block data
   if (Serialize(block_ptr) != SUCCESS)
   {
      ERR_report("Unable to serialize gps data.");

      delete [] block;
      return FAILURE;
   }

   // try to open a gps file for writing
   if (!file.Open(filespec, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &ex))
   {
      ERR_report_exception("Couldn't open source file", ex);
      delete [] block;
      return FAILURE;
   }

   file.Write(block, block_size);
   file.Close();

   delete [] block;

   m_fileSpecification = filespec;

   // reset the data object so the new name is reflected in the tabular
   // editor's title bar
   if (g_tabular_editor_dlg != NULL)
      g_tabular_editor_dlg->set_data_object(m_data_object);

   // set modified flag
   set_modified(FALSE);

   return SUCCESS;
}

// get the block of bytes needed to save this gps overlay to disk
int C_gps_trail::Serialize(BYTE *&block_ptr)
{
   const int SIGNATURE_LEN = 5;
   char signature[SIGNATURE_LEN];
   const int HEADER_LEN = 7;
   char header[HEADER_LEN];
   
   // write the file's header
   strcpy_s(header, HEADER_LEN, "FVWGPS");
   memcpy(block_ptr, header, 6);
   block_ptr += 6;
   
   // write the version info
   BYTE major = ((int)MAJOR_VERSION/10 << 4) | (MAJOR_VERSION-(10*((int)MAJOR_VERSION/10)));
   BYTE minor = ((int)MINOR_VERSION/10 << 4) | (MINOR_VERSION-(10*((int)MINOR_VERSION/10)));
   memcpy(block_ptr, &major, 1);
   block_ptr++;
   memcpy(block_ptr, &minor, 1);
   block_ptr++;
   short revision = GPS_REVISION;
   memcpy(block_ptr, &revision, sizeof(short));
   block_ptr += sizeof(short);
   
   // trail data
   {
      strcpy_s(signature, SIGNATURE_LEN, "TRL0");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;
      
      int block_size = get_trail_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);
      
      serialize_trail(block_ptr);
   }
   
   // coast track
   {
      strcpy_s(signature, SIGNATURE_LEN, "CTRK");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;
      
      int block_size = m_coast_track_mgr->get_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);
      
      m_coast_track_mgr->Serialize(block_ptr);
   }
   
   // options
   {
      strcpy_s(signature, SIGNATURE_LEN, "OPT5");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;
      
      int block_size = m_properties.get_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);
      
      m_properties.Serialize(block_ptr);
   }

   // bullseye properties
   {
      strcpy_s(signature, SIGNATURE_LEN, "BEP3");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;

      int block_size = m_properties.GetBullseyeProperties()->get_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);

      m_properties.GetBullseyeProperties()->Serialize(block_ptr);
   }

   // range and bearing
   {
      strcpy_s(signature, SIGNATURE_LEN, "RNGB");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;

      int block_size = m_rb_mgr->get_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);

      m_rb_mgr->Serialize(block_ptr);
   }

   // meta-data for the trail points
   {
      strcpy_s(signature, SIGNATURE_LEN, "META");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;

      int block_size = get_meta_data_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);

      serialize_meta_data(block_ptr);
   }

   // links for the trail points
   {
      strcpy_s(signature, SIGNATURE_LEN, "LNKS");
      memcpy(block_ptr, signature, 4);
      block_ptr += 4;
      
      int block_size = get_links_block_size();
      memcpy(block_ptr, &block_size, sizeof(int));
      block_ptr += sizeof(int);

      serialize_link_data(block_ptr);
   }
   
   return SUCCESS;
}

int C_gps_trail::serialize_link_data(BYTE *&block_ptr)
{
   int num_points = get_icon_list()->get_count();
   memcpy(block_ptr, &num_points, sizeof(int));
   block_ptr += sizeof(int);

   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   while (point != NULL)
   {
      CStringArray links;
      point->GetLinks(links);

      int numLinks = links.GetSize();
      memcpy(block_ptr, &numLinks, sizeof(int));
      block_ptr += sizeof(int);

      for(int i=0;i<numLinks;++i)
      {  
         CString str = links[i];

         int length = str.GetLength();
         memcpy(block_ptr, &length, sizeof(int));
         block_ptr += sizeof(int);
         if (length > 0)
         {
            memcpy(block_ptr, str, length);
            block_ptr += length;
         }
      }

      point = (GPSPointIcon *)get_icon_list()->get_next();
   }

   return SUCCESS;
}

int C_gps_trail::serialize_meta_data(BYTE *&block_ptr)
{
   int num_points = get_icon_list()->get_count();
   memcpy(block_ptr, &num_points, sizeof(int));
   block_ptr += sizeof(int);

   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   while (point != NULL)
   {
      int size  = point->m_meta_data.GetLength();
      memcpy(block_ptr, &size, sizeof(int));
      block_ptr += sizeof(int);
      if (size > 0)
      {
         memcpy(block_ptr, point->m_meta_data, size);
         block_ptr += size;
      }

      point = (GPSPointIcon *)get_icon_list()->get_next();
   }

   return SUCCESS;
}

// get the block of bytes needed to save a gps trail to disk
int C_gps_trail::serialize_trail(BYTE *&block_ptr)
{
   char sentence[MAX_NMEA_SENTENCE_LENGTH+1];
   NMEA_sentence nmea;
   int zero = 0;
   
   // write out the number of GPS points
   int num_points = get_icon_list()->get_count();
   memcpy(block_ptr, &num_points, sizeof(int));
   block_ptr += sizeof(int);
   
   // save all points
   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   
   while (point != NULL)
   {
      // write the size of the comment and the comment string (zero for no comment)
      if (point->has_comment())
      {
         int comment_size = point->get_comment_txt().GetLength();
         memcpy(block_ptr, &comment_size, sizeof(int));
         block_ptr += sizeof(int);
         
         if (comment_size > 0)
         {
            memcpy(block_ptr, point->get_comment_txt(), comment_size);
            block_ptr += comment_size;
         }     
      }
      else
      {  
         int zero = 0;
         memcpy(block_ptr, &zero, sizeof(int));
         block_ptr += sizeof(int);
      }     
      
      // copy the gps point data into the NMEA_sentence object
      nmea = *point;
      
      // make GGA sentence
      if (nmea.build_GGA(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
      {
         int length = strlen(sentence);
         memcpy(block_ptr, &length, sizeof(int));
         block_ptr += sizeof(int);
         
         memcpy(block_ptr, sentence, length);
         block_ptr += length;
      }
      else
      {
         memcpy(block_ptr, &zero, sizeof(int));
         block_ptr += sizeof(int);
      }
      
      // make RMC sentence
      if (nmea.build_RMC(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
      {
         int length = strlen(sentence);
         memcpy(block_ptr, &length, sizeof(int));
         block_ptr += sizeof(int);
         
         memcpy(block_ptr, sentence, length);
         block_ptr += length;
      }
      else
      {
         memcpy(block_ptr, &zero, sizeof(int));
         block_ptr += sizeof(int);
      }
      
      // make VTG sentence
      if (nmea.build_VTG(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
      {
         int length = strlen(sentence);
         memcpy(block_ptr, &length, sizeof(int));
         block_ptr += sizeof(int);
         
         memcpy(block_ptr, sentence, length);
         block_ptr += length;
      }
      else
      {
         memcpy(block_ptr, &zero, sizeof(int));
         block_ptr += sizeof(int);
      }
      
      point = (GPSPointIcon *)get_icon_list()->get_next();
   }
   
   return SUCCESS;
}

// returns the number of bytes necessary to write out the gps
// overlay to disk
int C_gps_trail::get_block_size()
{
   int size = 0;
   
   // size of file header and version info
   size += 6 + 2 + sizeof(short);
   
   // header + block size for trail block
   size += 4 + sizeof(int);
   
   // size of trail block
   size += get_trail_block_size();
   
   // header + block size for coast track block
   size += 4 + sizeof(int);
   
   // size of coast track block
   size += m_coast_track_mgr->get_block_size();
   
   // header + block size for options
   size += 4 + sizeof(int);
   
   // size of properties block
   size += m_properties.get_block_size();

   // header + size of bullseye properties block + properties block
   size += 4 + sizeof(int) + m_properties.GetBullseyeProperties()->get_block_size();

   // header + block size for range and bearing objects
   size += 4 + sizeof(int);

   // size of range and bearing objects
   size += m_rb_mgr->get_block_size();

   // header + block size for meta data + meta data size
   size += 4 + sizeof(int) + get_meta_data_block_size();

   // header + block size for links + links size
   size += 4 + sizeof(int) + get_links_block_size();

   return size;
}

int C_gps_trail::get_meta_data_block_size()
{
   int size = 0;

   // an int needed for the number of points
   size += sizeof(int);

   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();

   while (point != NULL)
   {
      size += sizeof(int);  // for size of string
      size += point->m_meta_data.GetLength();  // string

      point = (GPSPointIcon *)get_icon_list()->get_next();
   }

   return size;
}

int C_gps_trail::get_links_block_size()
{
   int size = 0;

   size += sizeof(int);    // number of points

   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   while (point != NULL)
   {
      size += sizeof(int);    // number of links for this point

      CStringArray links;
      point->GetLinks(links);
      const int nNumLinks = links.GetSize();
      for(int i=0;i<nNumLinks;++i)
      {
         size += sizeof(int);    // size of string
         size += links[i].GetLength();
      }

      point = (GPSPointIcon *)get_icon_list()->get_next();
   }

   return size;
}

// get the number of bytes needed to write out the current gps trail
int C_gps_trail::get_trail_block_size()
{
   char sentence[MAX_NMEA_SENTENCE_LENGTH+1];
   NMEA_sentence nmea;
   int size = 0;
   
   // an int needed for the number of points
   size += sizeof(int);
   
   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   
   while (point != NULL)
   {
      // does this point have a comment?  If so, we need to save it's size
      // otherwise, save the size as zero
      size += sizeof(int);
      if (point->has_comment())
      {
         int comment_size = point->get_comment_txt().GetLength();
         if (comment_size > 0)
            size += comment_size;
      }
      
      // copy the gps point data into the NMEA_sentence object
      nmea = *point;
      
      // make GGA sentence
      if (nmea.build_GGA(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
      {
         // size of string
         size += sizeof(int);
         
         // number of characters in string
         size += strlen(sentence);
      }
      else
      {
         size += sizeof(int);
      }
      
      // make RMC sentence
      if (nmea.build_RMC(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
      {
         size += sizeof(int);
         size += strlen(sentence);
      }
      else
      {
         size += sizeof(int);
      }
      
      // make VTG sentence
      if (nmea.build_VTG(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
      {
         size += sizeof(int);
         size += strlen(sentence);
      }
      else
      {  
         size += sizeof(int);
      }
      
      point = (GPSPointIcon *)get_icon_list()->get_next();
   }
   
   return size;
}


// save the gps overlay in the old ascii file format
int C_gps_trail::old_save_as(const CString &filespec)
{  
   char sentence[MAX_NMEA_SENTENCE_LENGTH+1];
   int index = 0;
   GPSPointIcon *point;
   NMEA_sentence nmea;
   FILE *out = NULL;
   
   // don't save empty trails.
   if (get_icon_list()->is_empty())
   {
      ERR_report("No points in the Moving Map trail.");
      return FAILURE;
   }
   
   // open output file
   fopen_s(&out, filespec, "w");
   if (out == NULL)
   {
      ERR_report("Can't open Moving Map file for writting.");
      return FAILURE;
   }
   
   // save all points
   point = (GPSPointIcon *)get_icon_list()->get_first();
   
   while (point != NULL)
   {
      // copy the gps point data into the NMEA_sentence object
      nmea = *point;
      
      // make GGA sentence
      if (nmea.build_GGA(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
         fputs(sentence, out);      // write out GGA sentence
      
      // make RMC sentence
      if (nmea.build_RMC(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
         fputs(sentence, out);      // write out RMC sentence
      
      // make VTG sentence
      if (nmea.build_VTG(sentence, MAX_NMEA_SENTENCE_LENGTH+1))
         fputs(sentence, out);      // write out VTG sentence
      
      point = (GPSPointIcon *)get_icon_list()->get_next();
   }
   
   fclose(out);
   
   m_fileSpecification = filespec;
   
   // set modified flag
   set_modified(FALSE);
   
   return SUCCESS;
}

int C_gps_trail::file_new()
{
   // assure unique filenames
   static int id = 1;

   // make a temp name
   CString name;
   m_fileSpecification.Format("Trail%d.gpb", id++);

   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// Loads a GPS trail from an NMEA compliant gps file. It also sets
// the map center to the last point in the trail.
//
int C_gps_trail::open(const CString &filespec)
{
   GPS_options_t options;              // save m_gps_options temporarily
   CFVWaitHelp wait("Loading Moving Map Trail...");
   
   // check to make sure we don't have an empty filespec
   if (filespec.IsEmpty())
   {
      ERR_report("Passed an empty filespec.");
      return FAILURE;
   }
   
   // save current options and set m_gps_options to all off
   options = m_gps_options;
   m_gps_options = 0;
   
   // free point memory
   clear_trail();
   
   // try opening the GPS file in the new file format
   CString ext = filespec.Right(4);
   if (ext.CompareNoCase(".gps") == 0)
   {
      if (open_old(filespec) != SUCCESS)
      {
         CString msg;
         
         msg.Format("%s is not a valid ASCII GPS file.", filespec);
         ERR_report(msg);
         
         return FAILURE;
      }
   }
   else
   {
      if (ext.CompareNoCase(".gpx") == 0)
      {
         try
         {
            if (open_gpx(filespec) != SUCCESS)
            {
               CString msg;

               msg.Format("%s is not a valid GPX file.", filespec);
               ERR_report(msg);

               return FAILURE;
            }
         }
         catch (_com_error& e)
         {
            REPORT_COM_ERROR(e);
            return FAILURE;
         }
      }
      else
      {
         if (open_new(filespec) != SUCCESS)
         {
            CString msg;
            msg.Format("%s is not a valid binary GPB file.", filespec);
            ERR_report(msg);

            return FAILURE;
         }
      }
   }
   // restore gps options
   m_gps_options = options;
   
   // update specification
   m_fileSpecification = filespec;
   
   // set modified flag to FALSE since the file was just opened
   set_modified(FALSE);
   
   // set the current view time
   {
      COleDateTime t0, t1;
      
      // get the interval for the trail that was just read in
      if (get_view_time_span(t0, t1) == TRUE)
      {
         // update the interval in the view time dialog and set this overlay's 
         // current time
         CMainFrame::GetPlaybackDialog().update_interval(t0, t1);
      }

      // need access to the view map for the call to set_current_view_time()
      CView *view = UTL_get_active_non_printing_view();
      MapProj *map = NULL;
      if (view)
         map = UTL_get_current_view_map(view);

      // set the current view time of this overlay and force the coast tracks
      // to get updated
      m_update_coast_tracks = TRUE;
      set_current_view_time(map, CMainFrame::GetPlaybackDialog().get_current_time());
   }
   
   return SUCCESS;
}

// open a gps file using the new binary gps format
int C_gps_trail::open_new(CString filespec)
{
   CFile file;
   CFileException ex;
   int byte_count;
   char signature[4];
   int block_size;
   BYTE *block;
   
   // try to open a gps file for reading
   if (!file.Open(filespec, CFile::modeRead | CFile::shareExclusive, &ex))
   {
      ERR_report_exception("Couldn't open source file", ex);
      return FAILURE;
   }
   
   // read and validate the header
   char header[6];
   file.Read(header, 6);
   if (strncmp(header, "FVWGPS", 6) != 0)
      return FAILURE;
   
   // read and validate the file version number
   BYTE major, minor;
   short revision;
   file.Read(&major, 1);
   file.Read(&minor, 1);
   file.Read(&revision, sizeof(short));
   
   if ((major >> 4)*10 + (major & 0x0F) != MAJOR_VERSION || 
      (minor >> 4)*10 + (minor & 0x0F) != MINOR_VERSION || 
      revision != GPS_REVISION)
      return FAILURE;
   
   do
   {
      // get block signature.  If the number of bytes returned
      // from the Read() function is < 0 then we are probably at
      // the end of the file.  In this case, get out of the loop
      byte_count = file.Read(signature, 4);
      
      if (byte_count == 4)
      {
         // get number of bytes in the current block
         file.Read(&block_size, sizeof(int));
         
         if (block_size > 0)
         {
            // get the block of data 
            block = new BYTE[block_size];
            file.Read(block, block_size);
            
            // perform the appropriate function based on the block's 4 byte signature
            
            // GPS trail data
            if (strncmp(signature,"TRL0",4) == 0)
            {
               deserialize_trail(block);
            }  
            // Coast track dataw
            else if (strncmp(signature,"CTRK",4) == 0)
            {
               m_coast_track_mgr->deserialize(block, this);
            }
            // GPS style options
            else if (strncmp(signature,"OPT5",4) == 0)
            {
               m_properties.deserialize(block);
               DefineShipSymbols();
            }
            else if (strncmp(signature,"BEP3",4) == 0)
            {
               m_properties.GetBullseyeProperties()->Deserialize(block);
            }
            else if (strncmp(signature,"RNGB",4) == 0)
            {
               m_rb_mgr->deserialize(block, this);
            }
            else if (strncmp(signature,"META",4) == 0)
            {
               deserialize_meta_data(block);
            }
            else if (strncmp(signature,"LNKS",4) == 0)
            {
               deserialize_link_data(block);
            }
            
            delete [] block;
         }
      }
      
   } while (byte_count > 0);
   
   return SUCCESS;
}

int C_gps_trail::deserialize_link_data(BYTE *block)
{
   BYTE *block_ptr = block;

   // get the number of points
   int num_points;
   memcpy(&num_points, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   while (point != NULL && num_points)
   {
      int numLinks;
      memcpy(&numLinks, block_ptr, sizeof(int));
      block_ptr += sizeof(int);

      CStringArray links;
      for(int i=0;i<numLinks;++i)
      {
         int size;
         memcpy(&size, block_ptr, sizeof(int));
         block_ptr += sizeof(int);
         if (size > 0)
         {
            char *tmp_buffer = new char[size + 1];
            memcpy(tmp_buffer, block_ptr, size);
            tmp_buffer[size] = '\0';
            links.Add(CString(tmp_buffer));
            block_ptr += size;

            delete [] tmp_buffer;
         }
      }
      point->SetLinks(links);

      --num_points;
      point = (GPSPointIcon *)get_icon_list()->get_next();
   }

   return SUCCESS;
}

int C_gps_trail::deserialize_meta_data(BYTE *block)
{
   BYTE *block_ptr = block;

   // get the number of points
   int num_points;
   memcpy(&num_points, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   while (point != NULL && num_points)
   {
      int size;
      memcpy(&size, block_ptr, sizeof(int));
      block_ptr += sizeof(int);
      if (size > 0)
      {
         char *tmp_buffer = new char[size + 1];
         if (tmp_buffer != NULL)
         {
            memcpy(tmp_buffer, block_ptr, size);
            tmp_buffer[size] = '\0';
            point->m_meta_data = CString(tmp_buffer);
         }
         block_ptr += size;

         delete [] tmp_buffer;
      }

      --num_points;
      point = (GPSPointIcon *)get_icon_list()->get_next();
   }

   return SUCCESS;
}

// deserialize the given block of data
int C_gps_trail::deserialize_trail(BYTE *block)
{
   BYTE *block_ptr = block;
   int string_size;
   char sentence[MAX_NMEA_SENTENCE_LENGTH+2];
   int num_points;
   CString comment_txt;
   int size = 0;
   
   // get the number of gps points
   memcpy(&num_points, block_ptr, sizeof(int));
   block_ptr += sizeof(int);
   
   // used to store the comments into the appropriate gps points
   CList <CString *, CString *> comment_lst;
   
   // loop through each point (for each point we should of writen out a 
   // GGA, RMC, and VTG sentence)
   for (int i=0; i<num_points; i++)
   { 
      // read the size of the comment text
      memcpy(&size, block_ptr, sizeof(int));
      block_ptr += sizeof(int);
      
      // read the comment text if any
      if (size != 0)
      {
         char* tmp_buffer = new char[size + 1];
         memcpy(tmp_buffer, block_ptr, size);
         tmp_buffer[size] = '\0';
         block_ptr += size;
         comment_lst.AddTail(new CString(tmp_buffer));
         delete [] tmp_buffer;
      }
      else
         comment_lst.AddTail(new CString(""));
      
      for(int j=0;j<3;j++)
      {
         // get the size of the next string
         memcpy(&string_size, block_ptr, sizeof(int));
         block_ptr += sizeof(int);

         // get the next string
         if (string_size > 0)
         {
            memcpy(sentence, block_ptr, string_size);
            block_ptr += string_size;
            sentence[string_size] = '\0';

            // process the sentence read in
            if (strlen(sentence) <= MAX_NMEA_SENTENCE_LENGTH)
               process_sentence(sentence);
         }
      }
   }
   
   // include the last point in the file is it contains a fix
   if (m_next_point.is_valid())
   {
      add_point();
      
      set_current_point( &m_next_point );
   }
   
   if (get_icon_list()->is_empty())
   {
      // the gps file is allowed to be empty, so don't write
      // out a ERR_ message

      return FAILURE;
   }
   
   // store the comments with the appropriate points
   POSITION str_pos = comment_lst.GetHeadPosition();
   GPSPointIcon *point = (GPSPointIcon *)get_icon_list()->get_first();
   while (point && str_pos)
   {
      CString str = *comment_lst.GetNext(str_pos);
      
      if (str == "")
      {
         point->set_has_comment(FALSE);
      }
      else
      {
         point->set_has_comment(TRUE);
         point->set_comment_txt(str);
      }
      
      point = (GPSPointIcon *)get_icon_list()->get_next();
   }
   
   // clean up
   while (!comment_lst.IsEmpty())
      delete comment_lst.RemoveTail();
   
   
   return SUCCESS;
}

// open a gps file using the previous ascii format
int C_gps_trail::open_old(CString filespec)
{
   // open output file
   FILE *in = NULL;
   fopen_s(&in, filespec, "r");
   if (in == NULL)
   {
      ERR_report("Can't open GPS file for reading.");
      return FAILURE;
   }
   
   char sentence[MAX_NMEA_SENTENCE_LENGTH+2];
   
   // while points in read from the file
   while (fgets(sentence, (MAX_NMEA_SENTENCE_LENGTH+1), in) != NULL)
   {
      // process the sentence read in
      if (strlen(sentence) <= MAX_NMEA_SENTENCE_LENGTH)
         process_sentence(sentence);
   }
   
   fclose(in);
   
   // include the last point in the file is it contains a fix
   if (m_next_point.is_valid())
   {
      add_point();
      
      set_current_point( &m_next_point );
   }
   
   if (get_icon_list()->is_empty())
   {
      ERR_report("Not a valid GPS file.");
      
      return FAILURE;
   }
   
   return SUCCESS;
}

boolean_t C_gps_trail::center_on(ViewMapProj* map)
{
   GPSPointIcon *point;
   d_geo_t map_ur, map_ll;
   
   if (get_icon_list()->is_empty())
      return FAILURE;
   
   // change map center to first point in the list
   point = (GPSPointIcon *)get_icon_list()->get_last();
   
   map->get_vmap_bounds(&map_ll, &map_ur);
   if (!GEO_in_bounds(map_ll.lat, map_ll.lon, map_ur.lat, map_ur.lon,
      point->get_latitude(), point->get_longitude()))
   {
      CView *pView = UTL_get_active_non_printing_view();
      if (UTL_change_view_map_to_best(pView, point->get_latitude(), 
         point->get_longitude()) == SUCCESS)
      {
         return TRUE;
      }
   }
   
   return FALSE;
}

int C_gps_trail::process_RMC_sentence(const char *sentence) 
{
   NMEA_sentence rmc;
   boolean_t time_out = FALSE;
   
   // parse the sentence into its peices
   if (!rmc.process_RMC(sentence))
   {
      m_bad_sentence_count += 4;
      show_communication("Received RMC sentence without a "
         "valid fix.");
      return FAILURE;
   }
   
   //GPS III
   m_gll_sentence_count = 0;
   
   // If the time in m_next_point is different from the time in this sentence,
   // then add the m_next_point and start a new point.  This is to sychronize
   // with the GPS input when it contains more than one type of sentence with
   // time in it: RMC, GGA, GLL.
   if 
      (
      m_next_point.valid_time() && 
      rmc.valid_time() &&
      m_next_point.get_time() != rmc.get_time()
      )
   {
      //NEW POINT
      int next_time = (int)m_next_point.get_time();
      int this_time = (int)rmc.get_time();
      
      // if time difference is 1 second or more, then time-out
      if (next_time != this_time)
      {
         time_out = TRUE;
         //ROLLOVER
         if (next_time > this_time) // implies midnight crossing
            if (!rmc.valid_date())       // the new date is unknown
               m_best_date += COleDateTimeSpan(1);
      }
   }
   
   if (time_out)
   {
      m_next_point_type = GPS_NULL_TYPE;
      //RMC
      if (add_point() == FAILURE)
         return FAILURE;
   }
   
   // add the last point to the list and start a new one,
   // if it started in this function
   else
      if (m_next_point_type == GPS_RMC_TYPE)
      {
         m_next_point_type = GPS_NULL_TYPE;
         //RMC
         if (add_point() == FAILURE)
            return FAILURE;
      }
      
      // create a new point - mark its origin
      if (m_next_point_type == GPS_NULL_TYPE)
      {
         m_next_point.reset_point();
         m_next_point_type = GPS_RMC_TYPE;
      }
      
      // get the latitude and longitude
      m_next_point.set_latitude(rmc.get_latitude());
      m_next_point.set_longitude(rmc.get_longitude());
      
      // only set the time if it hasn't already been set
      if (m_next_point.null_time())
         m_next_point.set_time(rmc.get_time());
      
      // only set the speed if it isn't already set
      if (m_next_point.m_speed_knots == -1.0f)
         m_next_point.m_speed_knots = rmc.get_speed_in_knots();
      
      // only set the true heading if it isn't already set
      if (m_next_point.m_true_heading == -1.0f)
         m_next_point.m_true_heading = rmc.get_true_heading();
      
      // only set the magnetic heading if it isn't arleady set
      if (m_next_point.m_magnetic_heading == -1.0f)
         m_next_point.m_magnetic_heading = rmc.get_mag_heading();
      
      // set date - only in rmc sentences
      if (rmc.valid_date())
         m_next_point.set_date( rmc.get_year(), rmc.get_month(), rmc.get_day() );
      
      // echo this RMC sentence to the moving map echo file
      moving_map_echo(sentence);
      
      // clear bad sentence count to disable show_communication
      m_bad_sentence_count = 0;
      
      return SUCCESS;
}


int C_gps_trail::process_GGA_sentence(const char *sentence) 
{
   NMEA_sentence gga;
   boolean_t time_out = FALSE;
   
   // parse the sentence into its peices
   if (!gga.process_GGA(sentence))
   {
      m_bad_sentence_count += 4;
      show_communication("Received GGA sentence without a "
         "valid fix.");
      return FAILURE;
   }
   
   //GPS III
   m_gll_sentence_count = 0;
   
   // If the time in m_next_point is different from the time in this sentence,
   // then add the m_next_point and start a new point.  This is to sychronize
   // with the GPS input when it contains more than one type of sentence with
   // time in it: GGA, GGA, GLL.
   if 
      (
      m_next_point.valid_time() && 
      gga.valid_time() &&
      m_next_point.get_time() != gga.get_time()
      )
   {
      //NEW POINT
      int next_time = (int)m_next_point.get_time();
      int this_time = (int)gga.get_time();
      
      // if time difference is 1 second or more, then time-out
      if (next_time != this_time)
      {
         time_out = TRUE;
         //ROLLOVER
         if (next_time > this_time) // implies midnight crossing
            if (!gga.valid_date())       // the new date is unknown
               m_best_date += COleDateTimeSpan(1);
      }
   }
   
   if (time_out)
   {
      m_next_point_type = GPS_NULL_TYPE;
      //GGA
      if (add_point() == FAILURE)
         return FAILURE;
   }
   
   // add the last point to the list and start a new one,
   // if it started in this function
   else
      if (m_next_point_type == GPS_GGA_TYPE)
      {
         m_next_point_type = GPS_NULL_TYPE;
         //GGA
         if (add_point() == FAILURE)
            return FAILURE;
      }
      
      // create a new point - mark its origin
      if (m_next_point_type == GPS_NULL_TYPE)
      {
         m_next_point.reset_point();
         m_next_point_type = GPS_GGA_TYPE;
      }
      
      //FORCING this value to 3 hinders our ability to determine if the GPS fix is lost
      // assume valid lat-lon data means 3 satellites
      if (gga.get_num_satellites() == 0)
         m_satellites = 3;
      else
         m_satellites = gga.get_num_satellites();
      
      // get the latitude and longitude
      m_next_point.set_latitude(gga.get_latitude());
      m_next_point.set_longitude(gga.get_longitude());
      
      // only set the time if it hasn't already been set
      if (m_next_point.null_time())
         m_next_point.set_time(gga.get_time());
      
      // get the altitude, GPS_UNKNOWN_ALTITUDE for unknown
      m_next_point.m_msl = gga.get_altitude();
      
      // echo this GGA sentence to the moving map echo file
      moving_map_echo(sentence);
      
      // clear bad sentence count to disable show_communication
      m_bad_sentence_count = 0;
      
      return SUCCESS;
}


int C_gps_trail::process_GLL_sentence(const char *sentence) 
{
   NMEA_sentence gll;
   boolean_t time_out = FALSE;
   
   // parse the sentence into its peices
   if (!gll.process_GLL(sentence))
   {
      m_bad_sentence_count += 4;
      show_communication("Received GLL sentence without a "
         "valid fix.");
      return FAILURE;
   }
   
   //GPS III
   m_gll_sentence_count++;
   if (m_gll_sentence_count < 2)
      return FAILURE;
   
   // If the time in m_next_point is different from the time in this sentence,
   // then add the m_next_point and start a new point.  This is to sychronize
   // with the GPS input when it contains more than one type of sentence with
   // time in it: RMC, GGA, GLL.
   if
      (
      m_next_point.valid_time() &&
      gll.valid_time() &&
      m_next_point.get_time() != gll.get_time()
      )
   {
      //NEW POINT
      int next_time = (int)m_next_point.get_time();
      int this_time = (int)gll.get_time();
      
      // if time difference is 1 second or more, then time-out
      if (next_time != this_time)
      {
         time_out = TRUE;
         //ROLLOVER
         if (next_time > this_time) // implies midnight crossing
            if (!gll.valid_date())       // the new date is unknown
               m_best_date += COleDateTimeSpan(1);
      }
   }
   
   if (time_out)
   {
      m_next_point_type = GPS_NULL_TYPE;
      //GLL
      if (add_point() == FAILURE)
         return FAILURE;
   }
   
   // add the last point to the list and start a new one,
   // if it started in this function
   else
      if (m_next_point_type == GPS_GLL_TYPE)
      {
         m_next_point_type = GPS_NULL_TYPE;
         //GLL
         if (add_point() == FAILURE)
            return FAILURE;
      }
      
      // create a new point - mark its origin
      if (m_next_point_type == GPS_NULL_TYPE)
      {
         m_next_point.reset_point();
         m_next_point_type = GPS_GLL_TYPE;
      }
      
      // get the latitude and longitude
      m_next_point.set_latitude(gll.get_latitude());
      m_next_point.set_longitude(gll.get_longitude());
      
      // only set the time if it hasn't already been set
      if (m_next_point.null_time())
         m_next_point.set_time(gll.get_time());
      
      // echo this GLL sentence to the moving map echo file
      moving_map_echo(sentence);
      
      // clear bad sentence count to disable show_communication
      m_bad_sentence_count = 0;
      
      return SUCCESS;
}

int C_gps_trail::process_VTG_sentence(const char *sentence) 
{  
   NMEA_sentence vtg;
   
   // parse the sentence into its peices
   if (!vtg.process_VTG(sentence))
   {
      show_communication("Received VTG sentence with no speed "
         "or heading data.");
      return FAILURE;
   }
   
   // only set the speed in knots if it is valid
   if (vtg.get_speed_in_knots() != -1.0f)
      m_next_point.m_speed_knots = vtg.get_speed_in_knots();
   
   // only set the speed in kilometers/hour if it is valid
   if (vtg.get_speed_in_km_hr() != -1.0f)
      m_next_point.m_speed_km_hr = vtg.get_speed_in_km_hr();
   
   // only set the true heading if it is valid
   if (vtg.get_true_heading() != -1.0f)
      m_next_point.m_true_heading = vtg.get_true_heading();
   
   // only set the magnetic heading if it is valid
   if (vtg.get_mag_heading() != -1.0f)
      m_next_point.m_magnetic_heading = vtg.get_mag_heading();
   
   // echo this VTG sentence to the moving map echo file
   moving_map_echo(sentence);
   
   // decrement bad sentence count as this sentence was recognized
   m_bad_sentence_count--;
   
   return SUCCESS;
}

void C_gps_trail::enable_3d_camera_update(CString strMenuLabel, LPARAM lparam)
{
   int start_indx = strMenuLabel.Find('[');
   int end_indx = strMenuLabel.Find(']');

   CString overlay_name = strMenuLabel.Mid(start_indx+1, end_indx-start_indx-1);

   C_overlay *overlay = OVL_get_overlay_manager()->FindOverlayByDisplayName(overlay_name);
   if (overlay && overlay->get_m_overlayDescGuid() == FVWID_Overlay_MovingMapTrail)
   {
      C_gps_trail *trail = static_cast<C_gps_trail *>(overlay);
      trail->set_update_3d_camera_enabled(lparam);
   }
}

STDMETHODIMP C_gps_trail::raw_GetDataSource(IFvDataSource **ppDataSource)
{

   CComObject<CFvGPSDataSource> *pDS;
   CComObject<CFvGPSDataSource>::CreateInstance(&pDS);
   this->AddRef();   // increment ref count so the trail object will not go out of scope if we close the object
                     // Release is called in the DS destructor
   pDS->Initialize(this);
   pDS->AddRef();

   *ppDataSource = pDS;
   return S_OK;
}

STDMETHODIMP C_gps_trail::raw_OnSkyViewConnectionInitialized()
{
   return S_OK;
}

STDMETHODIMP C_gps_trail::raw_OnSkyViewConnectionTerminating()
{
   set_update_3d_camera_enabled(FALSE);
   return S_OK;
}

STDMETHODIMP C_gps_trail::raw_OnAttachedCameraChanging()
{
   set_update_3d_camera_enabled(FALSE);
   return S_OK;
}

void C_gps_trail::set_update_3d_camera_enabled(boolean_t enable,
   boolean_t update_mapview /*=TRUE*/)
{
   // If SkyView is installed, then use the SkyView specific method
   if (skyview_overlay::IsOverlayTypeEnabled())
   {
      set_update_skyview_enabled(enable);
   }
   else  // otherwise, use the internal 3D viewer
   {
      MapView* map_view = fvw_get_view();
      if (map_view && update_mapview)
      {
         if (enable)
         {
            // Attach camera to this overlay
            map_view->AttachToCamera(m_camera_events);

            // Change to 3D projection
            map_view->ChangeProjectionType(GENERAL_PERSPECTIVE_PROJECTION);
         }
         else
         {
            map_view->AttachToCamera(nullptr);
         }
      }
      
      m_update_3d_camera = enable;
   }
}

void C_gps_trail::set_update_skyview_enabled(boolean_t enable) 
{
   if (enable)
   {
      if (Cmov_sym_overlay::GetStateIndicators()->m_symbol != NULL)
      {
         m_update_3d_camera = TRUE;

         if (Cmov_sym_overlay::m_skyview_interface != NULL && 
            Cmov_sym_overlay::m_skyview_interface->GetDispatch() != NULL)
         {
            // Create a "platform" for the camera.
            s_skyview_object_handle = Cmov_sym_overlay::m_skyview_interface->Add3DShape(0);

            // Make the camera platform too small to be seen.
            Cmov_sym_overlay::m_skyview_interface->Scale3DObject(s_skyview_object_handle, 0.01f);

            // Attach the camera to the platform.
            ISkyViewConnectionCameraEventsPtr spCameraEvents(this);
            Cmov_sym_overlay::m_skyview_interface->attach_camera_to_object(s_skyview_object_handle, spCameraEvents);

            // Create a motion playback for the platform.
            IMotionPlaybackPtr motion_playback;
            Cmov_sym_overlay::m_skyview_interface->CreateAndAssociateCOMMotionPlayback(
               "SampledMotionPlayback", s_skyview_object_handle, motion_playback);

            s_sampled_motion_playback = motion_playback;

            // Set the samples that the motion playback will use to control the
            // position and orientation of the platform.
            set_motion_playback_samples();

            // Update the status of SkyView's playback mechanism.
            update_skyview();
      
            // Enable SkyView's playback mechanism.
            Cmov_sym_overlay::m_skyview_interface->EnablePlayback(true);
         }

         // disable move/rotate for the SkyView symbol
         Cmov_sym_overlay::m_skyview_interface->set_can_rotate_symbol(FALSE);
         Cmov_sym_overlay::m_skyview_interface->set_can_move_symbol(FALSE);
      }
   }
   else
   {
      m_update_3d_camera = FALSE;

      if (Cmov_sym_overlay::m_skyview_interface != NULL && 
         Cmov_sym_overlay::m_skyview_interface->GetDispatch() != NULL &&
         s_skyview_object_handle != -1)
      {
         // Disable SkyView's playback mechanism.
         Cmov_sym_overlay::m_skyview_interface->EnablePlayback(false);

         if (s_sampled_motion_playback != NULL)
         {
            // Release the motion playback.
            Cmov_sym_overlay::m_skyview_interface->UnassociateAndReleaseCOMMotionPlayback(s_skyview_object_handle);
            s_sampled_motion_playback = NULL;
         }

         // Delete the camera platform.  This will return the camera to free operation.
         Cmov_sym_overlay::m_skyview_interface->detach_camera_from_object(s_skyview_object_handle);
      }

      s_skyview_object_handle = -1;

      // reenable move/rotate for the SkyView symbol
      Cmov_sym_overlay::m_skyview_interface->set_can_rotate_symbol(TRUE);
      Cmov_sym_overlay::m_skyview_interface->set_can_move_symbol(TRUE);
   }
}

void C_gps_trail::set_motion_playback_samples()
{
   // A sample will include time, lat, lon, alt, and heading.
   static const int DOUBLES_PER_SAMPLE = 5;

   int sample_count = m_icon_list.get_count();
   if (sample_count < 2)
   {
      // To do: support this case.
      return;
   }

   SAFEARRAY *sample_sa = ::SafeArrayCreateVector(VT_R8, 0, sample_count * DOUBLES_PER_SAMPLE);
   if (!sample_sa)
   {
      ERR_report("SafeArrayCreateVector() failed");
      return;
   }

   double *sample_data;
   if (FAILED(::SafeArrayAccessData(sample_sa, reinterpret_cast<void **>(&sample_data))))
   {
      ERR_report("SafeArrayAccessData() failed");
   }
   else
   {
      int point_index = 0;
      GPSPointIcon *point = get_icon_list()->get_first();
      while (point)
      {
         double *sample = sample_data + (point_index * DOUBLES_PER_SAMPLE);

         sample[0] = point->m_dateTime;
         sample[1] = point->get_latitude();
         sample[2] = point->get_longitude();
         sample[3] = point->m_msl;
         sample[4] = point->m_true_heading;

         point = get_icon_list()->get_next();
         ++point_index;
      }

      if (FAILED(::SafeArrayUnaccessData(sample_sa)))
      {
         ERR_report("SafeArrayUnaccessData() failed");
      }
      else
      {
         try
         {
            s_sampled_motion_playback->SetSamples(
               &sample_sa,
               VARIANT_FALSE, // alt is not constant
               VARIANT_FALSE, // heading is not constant
               VARIANT_TRUE,  // pitch is constant
               VARIANT_TRUE,  // bank is constant
               0.0,  // constant alt: ignored
               0.0,  // constant heading: ignored
               0.0,  // constant pitch
               0.0); // constant bank
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("SampledMotionPlayback::SetSamples failed: (%s)", (char *)e.Description());
            ERR_report(msg);
         }
      }
   }
   
   if (FAILED(::SafeArrayDestroy(sample_sa)))
   {
      ERR_report("SafeArrayDestroy() failed");
   }
}

void C_gps_trail::get_point_info(ViewMapProj *view, C_icon *icon)
{
   GPSPointIcon *point = (GPSPointIcon *)icon;
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(icon->m_overlay);
   
   // popup the info box for this point
   show_point_info(point);
   
   // set the info box focus
   trail->set_info_box_focus(point);
}


void C_gps_trail::set_info_box_focus(GPSPointIcon *point)
{  
   // if the info box focus is being removed from this overlay, or it
   // is being placed on a point in the trail, the info box focus is
   // not on the show_communication dialog
   if (m_show_communications)
      m_show_communications = FALSE;
   
   if (m_selected_point != point)
   {
      /////////////////////////////////////////////
      //the invalid rect
      CRect irect;
      
      //the currently selected object is invalid
      irect = invalidate_object(m_selected_point);
      
      //select the new point
      if (m_selected_point)
         m_selected_point->set_selected(FALSE);
      m_selected_point = point;
      point->set_selected(TRUE);
      m_shipSymbol->set_selected(TRUE);

      //the new point is invalid
      irect = invalidate_object(m_selected_point);
   }
}

void C_gps_trail::show_point_info(GPSPointIcon *point)
{
   //TRIVIAL REJECTION
   if (!point)
      return;
   
   CString edit;
   CString line;
   int hour, minute;
   float second;
   const int STR_LEN = 41;
   char lat_lon_str[STR_LEN];
   const int DATUM_LEN = 6;
   char datum[DATUM_LEN];
   
   // convert lat-lon to formated string in default format and datum
   GEO_lat_lon_to_string(point->get_latitude(), point->get_longitude(), 
      lat_lon_str, STR_LEN);
   
   // get the default datum, so it can be included with the location string
   GEO_get_default_datum(datum, DATUM_LEN);
   
   // put LOCATION string in edit string
   edit.Format("Location: (%s)  %s\r\n", datum, lat_lon_str);
   
   // put ELEVATION in edit string if available
   if (point->m_msl != GPS_UNKNOWN_ALTITUDE)
   {
      line.Format("Antenna Altitude: %0.0f ft (MSL)\r\n", 
         METERS_TO_FEET(point->m_msl));
      edit += line;
   }
   
   // add SPEED and HEADING lines to edit string
   if (point->m_speed_km_hr != -1.0 || point->m_speed_knots != -1.0)
   {
      float km_hr = point->m_speed_km_hr;
      float knots = point->m_speed_knots;

      if (km_hr == -1.0)
         km_hr = (float)KNOTS_TO_KM_PER_H(knots);
      if (knots == -1.0)
         knots = km_hr * 0.53995333005f;

      if (point->m_speed_knots > 0.0)
      {
         // add SPEED line
         line.Format("Ground Speed: %03.0f Kts, %03.0f km/h, %03.0f mph\r\n",
            knots, km_hr, km_hr * 0.621371192L);
         edit += line;
          
         // add HEADING line
         line.Format("Course: %03.0f\260 Magnetic, %03.0f\260 True\r\n",
            point->m_magnetic_heading, point->m_true_heading);
         edit += line;
      }
      else 
         edit += "Ground Speed: 0\r\nCourse: Unknown\r\n";
   }
   else
      edit += "Ground Speed: Unknown\r\nCourse: Unknown\r\n";
   
   // add DATE and TIME to edit string
   if (point->valid_time())
   {
      hour   = point->get_hour();
      minute = point->get_minute();
      second = point->get_second();
      
      // if the date is valid, include the day and date in the output
      if (point->valid_date())
      {
         int year  = point->get_year();
         int month = point->get_month();
         int day   = point->get_day();
         
         //we might have a 2 digit year so massage it if necessary
         year = GPS_get_y2k_compliant_year(year);
         
         // construct a CTime to take advantage of the Format member, i.e.,
         // get the day and month as strings
         COleDateTime t(year, month, day, hour, minute, (int)second);
         edit += t.Format("Date: %A, %d %B, %Y\r\n");
      }
      
      line.Format("Time: %02d:%02d:%02.0f Zulu\r\n", hour, minute, second);
      edit += line;
   }
   
   // add meta data to edit string
   int new_pos;
   CString meta_info = extract_element(point->m_meta_data, "MetaInfo", 0, new_pos);
   if (meta_info != "")
   {
      CString tag = extract_element(meta_info, "tag", 0, new_pos);
      while (tag != "")
      {
         CString data = extract_element(meta_info, "data", new_pos, new_pos);

         if (data != "")
         {
            line.Format("%s: %s\r\n", tag, data);
            edit += line;
         }

         tag = extract_element(meta_info, "tag", new_pos, new_pos);
      }
   }

   // add the GPS comment if one exists for this point
   if (point->has_comment())
   {
      edit += "\r\n\r\n" + point->get_comment_txt();
   }

   CStringArray links;
   point->GetLinks(links);
   const int nSize = links.GetSize();
   if (nSize > 0)
   {
      CString text;
      text += "\r\nLinks:";
      text += "\r\n";
      for (int i = 0; i < nSize; i++)
      {
         text += "     ";
         text += links[i];
         text += "\r\n";
      }
      edit += text;
   }

   CString title;
   title.Format("Moving Map Point Information - %s", OVL_get_overlay_manager()->GetOverlayDisplayName(point->get_overlay()) );

   // display the GPS point info in the info dialog box   
   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, edit, title, point->m_overlay);
}

void C_gps_trail::release_edit_focus() 
{
   // this overlay must be the current overlay when this function is called
   ASSERT(get_current());
   
   // must be in route edit mode when this function gets called
   //   ASSERT(get_edit_on());
   
   // need to invalidate the entire route so that it gets drawn in the right color
   //   invalidate(FALSE);
}

// called by CRemarkDisplay when this overlay looses the info box focus
void C_gps_trail::release_focus()
{
   if (m_selected_point)
   {
      invalidate_object(m_selected_point);
      m_selected_point->set_selected(FALSE);
      m_shipSymbol->set_selected(FALSE);
      m_selected_point = NULL;
   }
   else
   {
      // Note is_dlg_trail(this) is only TRUE during a moving map display.
      // When the moving map display is started, the info box is used for
      // feedback until the GPS receiver starts outputting valid fixes.  At
      // that point the receiver info box will be used for GPS information,
      // if update info is turned on, or it will be removed if update info
      // is turned off.  If the user closes the info dialog while it is being
      // used to show communications information, update information must be
      // turned off in order to avoid popping it up again when the next
      // sentence is received.
      /*
      if (m_gpsdlg.is_dlg_trail(this))
      {
      PRM_set_registry_string("GPS Options", "UPDATE_INFO", "NO");
      m_gps_options &= (~GPS_UPDATE_INFO);
      }
      */
   }
}

int C_gps_trail::show_last_point_info(ViewMapProj *map)
{
   // if there are no gps points then return failure
   if (!m_current_point.is_valid())
      return FAILURE;
   // update info box for the current point
   get_point_info(map, &m_current_point);
   
   return SUCCESS;
}

// build the default name for next new gps trail overlay
void C_gps_trail::get_next_new_default_name(CString &default_name)
{
   static int id = 1;
   
   // build the instance filename from the template
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_MovingMapTrail);
   default_name.Format("%s\\Trail%d.gpb", pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory, id++);
}

const char *C_gps_trail::get_default_extension()
{
   const CString spec = get_specification();

   CString ext = spec.Right(4);
   if (ext.CompareNoCase(".gps") == 0)
      return "gps";

   return "gpb";
}

// Inform the gps trail that it is about to be closed, if the trail is 
// dirty the user will be asked if they want to save the changes. If the
// user cancels the close this fucntion will return cancel == TRUE.  Note: 
// cancel = NULL can be input to this function, in which case the user is
// not given the option to cancel.
int C_gps_trail::pre_close(boolean_t *cancel)
{
   m_is_closing = TRUE;
   
   // close the GPS tools dialog if it is opened
   if (C_gps_trail::is_gps_tools_opened())
      C_gps_trail::m_gpstools_dialog.DestroyWindow();
   
   // stop the coast track / range bearing timer
   KillTimer(NULL, 0);
   
   if (cancel)
      *cancel = FALSE;
   
   // since this overlay is a time-sensitive overlay we need to update
   // the view time dialog if it is opened
   CMainFrame::GetPlaybackDialog().update_interval();

   if (g_tabular_editor_dlg != NULL && g_tabular_editor_dlg->get_data_object() == m_data_object)
      CTabularEditorDlg::destroy_dialog();

   // close the Link Editor, if open
   if (m_pLinksEditDlg)
   {
      delete m_pLinksEditDlg;
      m_pLinksEditDlg = NULL;
   }

   // if this is the last GPS overlay being closed and we are attached to
   // a 3D viewer, then unattach
   if (m_update_3d_camera)
      set_update_3d_camera_enabled(FALSE);
   
   return SUCCESS;
}

int C_gps_trail::auto_save_gps_trail()
{
   // if the overlay hasn't changed or we are not connected, then don't autosave
   if (!is_modified() || !is_connected())
      return SUCCESS;

   // if the trail has never been assigned a name we need to
   // do so here
   long bHasBeenSaved = 0;
   get_m_bHasBeenSaved(&bHasBeenSaved);

   const long eSaveFormatUnspecified = 0;

   if (!bHasBeenSaved)
   {
      put_m_bHasBeenSaved(TRUE);

      CTime time = CTime::GetCurrentTime();

      const CString documentsPath = PRM_get_registry_string("Main", 
         "ReadWriteUserData") + "\\Gps";

      CString strAutoSaveName;

      // if the auto save name has not been set, then we will use the feed name
      if (m_strAutoSaveName.GetLength() == 0)
         strAutoSaveName = m_moving_map_feed->get_feed_name();
      else
         strAutoSaveName = m_strAutoSaveName;

      CString invalid_filename_chars("/\\:*?\"<>|");
      for(int i=0;i<invalid_filename_chars.GetLength();++i)
         strAutoSaveName.Replace(invalid_filename_chars[i], ' ');

      CString filespec;
      filespec.Format("%s\\%s %s.gpb", documentsPath, strAutoSaveName, time.FormatGmt("%Y-%m-%d %H%MZ"));

      // make sure the auto-save name isn't already used (this can happen, for instance, if two
      // separate moving map overlays are saved at the same time)
      POSITION pos;
      int fileNum = 2;
      do 
      {
         OVL_get_overlay_manager()->find_by_specification(filespec, FVWID_Overlay_MovingMapTrail, pos);
         if (pos != NULL)
         {
            filespec.Format("%s\\%s %s (%d).gpb", documentsPath, strAutoSaveName, time.FormatGmt("%Y-%m-%d %H%MZ"), fileNum++);
         }
      } while (pos != NULL);

      save_as(filespec, eSaveFormatUnspecified);
   }
   else
   {
      const long eSaveFormatUnspecified = 0;
      save_as(get_specification(), eSaveFormatUnspecified);
   }

   return SUCCESS;
}

void C_gps_trail::moving_map_echo(const char *line)
{
   FILE *out = NULL;
   const int BUFFER_LEN = 82;
   char buffer[BUFFER_LEN];
   
   // don't echo processed NMEA sentences when echo is disabled 
   if ((m_gps_options & GPS_MOVING_MAP_ECHO_ON) == 0)
      return;
   
   fopen_s(&out, m_moving_map_echo_file, "a");
   if (out == NULL)
   {
      ERR_report("Can't open file.");
      return;
   }
   
   if (fseek(out, 0, SEEK_END))
   {
      ERR_report("fseek() failed.");
      fclose(out);
      return;
   }
   
   // append EOL to the line
   strncpy_s(buffer, BUFFER_LEN, line, MAX_NMEA_SENTENCE_LENGTH);
   strcat_s(buffer, BUFFER_LEN, "\n");
   
   // add it to the file
   fputs(buffer, out);
   
   fclose(out);
}

// set the name of the moving map echo file
void C_gps_trail::set_moving_map_echo_file(const char *file)
{
   m_moving_map_echo_file = file;
}

void C_gps_trail::all_input_echo(const char *line)
{
   FILE *out = NULL;
   const int BUFFER_LEN = 82;
   char buffer[BUFFER_LEN];
   
   // don't echo if echo is not on
   if ((m_gps_options & GPS_ALL_INPUT_ECHO_ON) == 0)
      return;
   
   fopen_s(&out, m_all_input_echo_file, "a");
   if (out == NULL)
   {
      ERR_report("Can't open file.");
      return;
   }
   
   if (fseek(out, 0, SEEK_END))
   {
      ERR_report("fseek() failed.");
      fclose(out);
      return;
   }
   
   // append EOL to the line
   strncpy_s(buffer, BUFFER_LEN, line, 80);
   strcat_s(buffer, BUFFER_LEN, "\n");
   
   // add it to the file
   fputs(buffer, out);
   
   fclose(out);
}

// set the name of the all input echo file
void C_gps_trail::set_all_input_echo_file(const char *file)
{
   m_all_input_echo_file = file;
}

int C_gps_trail::show_communication(const char *line, boolean_t new_buffer)
{
   static std::string buffer_com;

   if (m_bad_sentence_count < 50 && new_buffer == FALSE)
      return SUCCESS;
   
   if (new_buffer)
   {
      buffer_com = "";
      m_bad_sentence_count = 50;
   }
   
   // append the new line to the end of the buffer
   buffer_com += line;
   buffer_com += "\r\n";

   // if new_buffer is FALSE and the info box is not focused on this overlay,
   // then only show the communications information if update info is turned on
   if (new_buffer == FALSE && CRemarkDisplay::get_focus_overlay() != this &&
      (m_gps_options & GPS_UPDATE_INFO) == 0)
      return SUCCESS;

   CString title;
   title.Format("Moving Map Communication - %s", OVL_get_overlay_manager()->GetOverlayDisplayName(this));

   // display the GPS Communication dialog box
   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, buffer_com.c_str(), title, this);
   
   // set flag indicating this overlay is using the CRemarkDisplay
   // dialog to show communications
   m_show_communications = TRUE;
   
   return SUCCESS;
}

// returns TRUE if any opened gps trails are connected
// static
boolean_t C_gps_trail::connected()
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   while (trail)
   {
      if (trail->is_connected())
         return TRUE;

      trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_next_of_type(trail, FVWID_Overlay_MovingMapTrail));
   }

   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// GPS Add Point function for use with the map server

int C_gps_trail::add_point(GPSPointIcon &point)

{
   if (!point.is_valid())
   {
      ERR_report("point is invalid.");
      return FAILURE;
   }

   m_next_point = point;
   
   // passing in TRUE tells add_point that it will be adding a point from
   // the map server and not the serial feed
   return add_point();
}

int C_gps_trail::add_point(GPS_QUEUED_POINT &qpoint)
{
   // guarantee that the point is added via the main thread
   ATL::CCritSecLock lock( s_idThreadInterlock );
   {
      m_listAddPointQueue.AddTail(qpoint);

      int nOverlayHandle = OVL_get_overlay_manager()->get_overlay_handle(this);

      CMainFrame *pFrame = fvw_get_frame();
      if (pFrame)
         pFrame->PostMessage(WM_MOVING_MAP_POINT_ADDED, (WPARAM)nOverlayHandle, 0);
   }

   return SUCCESS;
}

int C_gps_trail::add_point_from_queue()
{
   if (m_listAddPointQueue.IsEmpty())
   {
      ERR_report("Moving map point queue is empty");
      return FAILURE;
   }

   ATL::CCritSecLock lock( s_idThreadInterlock );
   GPS_QUEUED_POINT qpoint = m_listAddPointQueue.RemoveHead();
   lock.Unlock();

   // fill in the GPSPointIcon structure
   GPSPointIcon point;
   point.set_latitude(qpoint.latitude);
   point.set_longitude(qpoint.longitude);
   point.m_msl = qpoint.msl; // meters
   point.m_true_heading = qpoint.true_heading;
   point.m_magnetic_heading = qpoint.magnetic_heading;
   point.m_speed_knots = qpoint.speed_knots;
   point.m_speed_km_hr = qpoint.speed_km_hr;
   point.m_dateTime = COleDateTime(qpoint.dateTime);
   point.m_meta_data = qpoint.meta_data;

   if (!point.is_valid())
   {
      ERR_report("point is invalid.");
      return FAILURE;
   }

   m_next_point = point;
   
   return SUCCESS;
}

// Turn GPS mode on or off.
int C_gps_trail::set_edit_on(boolean_t edit_on)
{
   //is this redundant
   if (m_edit_on == edit_on)
      return SUCCESS;
   
   // If there routes are disabled, then it is not necessary for the course
   // deviation indicator button to be shown
   
   //remember grasshopper
   m_edit_on = edit_on;
   
   //we just transited modes
   int showCmd;
   if (m_edit_on)
      showCmd=SW_SHOW;
   else
      showCmd=SW_HIDE;
   
   //hide all the gps bells and whistles

   // close the GPS tools dialog if it is opened
   if (C_gps_trail::is_gps_tools_opened())
      C_gps_trail::m_gpstools_dialog.DestroyWindow();

   // close the Link Editor, if open
   if (!m_edit_on && m_pLinksEditDlg != NULL)
   {
      delete m_pLinksEditDlg;
      m_pLinksEditDlg = NULL;
   }

   // GEORGE
   //need access to the view map
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = NULL;
   if (view)
      map = UTL_get_current_view_map(view);

   reset_auto_center_and_rotate(map, m_edit_on);

   //all is well
   return SUCCESS;
}

void C_gps_trail::CloseCommDlg()
{
   // remove the info box if this overlay has its focus
   if (get_info_box_up())
      CRemarkDisplay::close_dlg();
}


int C_gps_trail::GetPngEncoderClsid()
{
   if ( s_clsidPngEncoder != CLSID_NULL )
      return 0;

   const static int failure = -1;
   UINT cEncoders, cEncoderBytes;

   int iResult = failure;
   Gdiplus::GetImageEncodersSize( &cEncoders, &cEncoderBytes );
   if ( cEncoderBytes > 0 )
   {
      Gdiplus::ImageCodecInfo* image_codec_info =
         static_cast< Gdiplus::ImageCodecInfo* >(
            malloc( cEncoderBytes ) );
      if ( image_codec_info != NULL )
      {
         Gdiplus::GetImageEncoders( cEncoders,
            cEncoderBytes, image_codec_info );

      for ( UINT j = 0; j < cEncoders; ++j )
      {
         if ( wcscmp( image_codec_info[ j ].MimeType, L"image/png" ) == 0 )
         {
            s_clsidPngEncoder = image_codec_info[ j ].Clsid;
            iResult = j;  // Success
            break;
         }
      }

      free( image_codec_info );
      }
   }
   return iResult;
}


void C_gps_trail::DefineShipSymbols()
{
   DefineSymbol(m_shipSymbol);
   DefineSymbol(m_shipSymbol_2);

   // Build a simple ship image
   OvlSymbol osShip;
   double dRefY = DefineSymbol( &osShip );   // Non-0 if nose shifted

   DOUBLE dShipSize = osShip.get_scale();
   dRefY *= dShipSize;

   // Find min bounding rectangle
   CRect rcShip( 0, 0, 0, 0 );
   POSITION pos = osShip.get_head_pos();
   while ( pos != NULL )
   {
      OvlSymbolPrimative* pPrimitive = osShip.get_next( pos );

      // Convert the element centered on the origin
      pPrimitive->convert( 0.0, 0.0, 0.0, dShipSize );

      // Get the element's bounding rectangle
      CRect rc = pPrimitive->get_rect( osShip.get_pen() );

      // If it is not an empty rectangle, add it to the total rectangle
      if ( !rc.IsRectEmpty() )
         rcShip |= rc;
   }

   if ( !rcShip.IsRectEmpty() )
   {
      double
         dBmpWidth = rcShip.right - rcShip.left,
         dBmpCenterX = 0.5 + ( 0.5 * dBmpWidth ), 
         dBmpCenterY = 0.5 + __max( rcShip.bottom, -rcShip.top + ( 2.0 * dRefY ) ),
         dBmpHeight = 2.0 * dBmpCenterY;

      // Translate the reference location (nose or original zero) to the center of the new bitmap
      pos = osShip.get_head_pos();
      while ( pos != NULL )
         osShip.get_next( pos )->convert( dBmpCenterX, dBmpCenterY, 0.0, dShipSize );

      Gdiplus::Bitmap bitmap( 1 + (int) dBmpWidth, 1 + (int) dBmpHeight );
      Gdiplus::Graphics gdip_graphics( &bitmap );
      Gdiplus::Color color( 0, 0, 0, 0 );
      gdip_graphics.Clear( color );
      CDC dc;
      dc.Attach( gdip_graphics.GetHDC() );
   
      osShip.view_draw( static_cast< CGenericProjector* >( nullptr ), &dc );

      gdip_graphics.ReleaseHDC(dc.Detach());

      StdVectorIStream stream;
      bitmap.Save( &stream, &s_clsidPngEncoder );
      m_aShipSymbolRawBytes = *stream.GetBytes();

   }  // Non-empty ship rectangle
}  // Define ship symbols


double C_gps_trail::DefineSymbol(OvlSymbol *shipSymbol)
{
   // remove all primatives, so symbol can be redefined.
   shipSymbol->remove_all_primatives();

   // define the symbol based on the type; fallback to a bomber
   // symbol if neccessary
   if (SymbolContainer::GetInstance()->define_symbol(shipSymbol, m_properties.get_symbol_type()) 
      != SUCCESS)
   {
      shipSymbol->set_unit(24.0f);
      shipSymbol->add_line(0,-24,0,24);
      shipSymbol->add_line(0,-4,-24,8);
      shipSymbol->add_line(0,-4,24,8);
      shipSymbol->add_line(0,21,-9,24);
      shipSymbol->add_line(0,21,9,24);
   }

   // set the foreground and background pen of the symbol
   OvlPen &pen = shipSymbol->get_pen();
   pen.set_foreground_pen(m_properties.get_ship_fg_color(),
      UTIL_LINE_SOLID, m_properties.get_ship_line_size());
   pen.set_background_pen(m_properties.get_ship_bg_color());

   // set the ship's size
   shipSymbol->set_scale(m_properties.get_ship_size());

   // translate symbol
   double dMinY = m_properties.GetOriginAtNose() ?
      shipSymbol->translate_symbol_by_min_y() : 0.0;

   // invalidate the symbol
   shipSymbol->invalidate();
   OVL_get_overlay_manager()->InvalidateOverlay(this);
   return dMinY;
}

#if 0 // Not used
void C_gps_trail::define_symbol_airlift()
{
   // get the user's data path and append the symbol's file name
   CString file_spec = PRM_get_registry_string("Main", "HD_DATA");
   file_spec += "\\symbols\\airlift.sym";

   m_shipSymbol->read_from_file(file_spec);
   m_shipSymbol_2->read_from_file(file_spec);
}

void C_gps_trail::define_symbol_bomber()
{
   // get the user's data path and append the symbol's file name
   CString file_spec = PRM_get_registry_string("Main", "HD_DATA");
   file_spec += "\\symbols\\bomber.sym";

   m_shipSymbol->read_from_file(file_spec);
   m_shipSymbol_2->read_from_file(file_spec);
}

void C_gps_trail::define_symbol_fighter()
{
   // get the user's data path and append the symbol's file name
   CString file_spec = PRM_get_registry_string("Main", "HD_DATA");
   file_spec += "\\symbols\\fighter.sym";

   m_shipSymbol->read_from_file(file_spec);
   m_shipSymbol_2->read_from_file(file_spec);
}

void C_gps_trail::define_symbol_helicopter()
{
   // get the user's data path and append the symbol's file name
   CString file_spec = PRM_get_registry_string("Main", "HD_DATA");
   file_spec += "\\symbols\\helicopter.sym";

   m_shipSymbol->read_from_file(file_spec);
   m_shipSymbol_2->read_from_file(file_spec);
}

#endif

/*
This routine will invalidate a rectangle large enough
for a correct redraw of the area under the object point
*/
CRect C_gps_trail::invalidate_object(GPSPointIcon *point, boolean_t bInvalidate )
{
   //this is what we will invalidate
   //initially empty
   CRect irect(0,0,0,0);
   
   //no can do if 
   if (point == NULL)
      //return empty rect
      return irect;
   
   //is this the own ship
   if (point == &m_current_point)
   {
      if ( m_shipSymbol != nullptr )
      {
         irect = m_shipSymbol->get_rect_w_selection();

         // if the range ring is on, invalidate a rect big enough
         // to take care of it
         if (m_properties.GetBullseyeProperties()->get_bullseye_on())
            irect |= m_bullseye.get_rect();

         if (m_properties.get_display_altitude_label())
            irect |= m_altitude_label_bounds;

         if (m_properties.get_nickname_label().GetLength() && m_properties.GetDisplayNickname())
            irect |= m_nickname_label_bounds;
      }
   }
   else
   {
      if (display_trail_points())
      {
         irect = point->m_rect;
         
         //center of trail point
         //irect.OffsetRect(viewPoint);
         //grow to trail point size
         irect.InflateRect(m_radius,m_radius);
         //are we selected
         if (point->get_selected())
            //grow to selected trail point size
            irect.InflateRect(POINT_SELECTED_BORDER+1,POINT_SELECTED_BORDER+1);
      }
      else
         irect.SetRectEmpty();
   }
   
   
   // do we have something to invalidate and are we supposed to invalidate
   if (!irect.IsRectEmpty() && bInvalidate)
   {
      //cause a redraw
      invalidate_rect(irect);
      invalidate_rect(m_shipSymbol->get_rect_w_selection());
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }
   
   //tell the caller about what we just invalidated
   return irect;
}

void C_gps_trail::load_registry_settings()
{
   // get gps display options from registry
   m_gps_options = GPS_SPEED_KNOTS;
   if (PRM_get_registry_string("GPS Options", "UPDATE_INFO", "YES") == "YES")
      m_gps_options |= GPS_UPDATE_INFO;
   if (PRM_get_registry_string("GPS Options", "TRAIL_ON", "YES") == "YES")
      m_gps_options |= GPS_LEAVE_TRAIL;
   if (PRM_get_registry_string("GPS Options", "AUTO_CENTER", "YES") == "YES")
      m_gps_options |= GPS_AUTO_CENTER;
   if (PRM_get_registry_string("GPS Options", "AUTO_ROTATE", "NO") == "YES")
      m_gps_options |= GPS_AUTO_ROTATE;
   if (PRM_get_registry_string("GPS Options", "ECHO_ALL_INPUT", "NO") == "YES")
      m_gps_options |= GPS_ALL_INPUT_ECHO_ON;
   if (PRM_get_registry_string("GPS Options", "ECHO_VALID_INPUT", "NO") == "YES")
      m_gps_options |= GPS_MOVING_MAP_ECHO_ON;
   
   m_playback_rate = PRM_get_registry_int("GPS Options", "PLAYBACK_RATE", 1);
   
   m_TautoCenter  = (m_gps_options & GPS_AUTO_CENTER)!=0;
   m_TautoRotate  = (m_gps_options & GPS_AUTO_ROTATE)!=0;
   m_TtrailPoints = (m_gps_options & GPS_LEAVE_TRAIL)!=0;
   
   //don't need to load again
   C_gps_trail__registry_options_state++;
}

void C_gps_trail::save_registry_settings()
{
   // save display options
   if (m_gps_options & GPS_UPDATE_INFO)
      PRM_set_registry_string("GPS Options", "UPDATE_INFO", "YES");
   else
      PRM_set_registry_string("GPS Options", "UPDATE_INFO", "NO");
   
   if (m_gps_options & GPS_LEAVE_TRAIL)
      PRM_set_registry_string("GPS Options", "TRAIL_ON", "YES");
   else
      PRM_set_registry_string("GPS Options", "TRAIL_ON", "NO");
   
   if (m_gps_options & GPS_AUTO_CENTER)
      PRM_set_registry_string("GPS Options", "AUTO_CENTER", "YES");
   else
      PRM_set_registry_string("GPS Options", "AUTO_CENTER", "NO");
   
   if (m_gps_options & GPS_AUTO_ROTATE)
      PRM_set_registry_string("GPS Options", "AUTO_ROTATE", "YES");
   else
      PRM_set_registry_string("GPS Options", "AUTO_ROTATE", "NO");
   
   if (m_gps_options & GPS_ALL_INPUT_ECHO_ON)
      PRM_set_registry_string("GPS Options", "ECHO_ALL_INPUT", "YES");
   else
      PRM_set_registry_string("GPS Options", "ECHO_ALL_INPUT", "NO");
   
   if (m_gps_options & GPS_MOVING_MAP_ECHO_ON)
      PRM_set_registry_string("GPS Options", "ECHO_VALID_INPUT", "YES");
   else
      PRM_set_registry_string("GPS Options", "ECHO_VALID_INPUT", "NO");
   
   PRM_set_registry_int("GPS Options","PLAYBACK_RATE", m_playback_rate );
}

void C_gps_trail::invalidate()
{
   //redraw our trail
   invalidate_rect(m_rect);
   C_overlay::invalidate();
   GetOvlElementContainer()->invalidate();
}

void C_gps_trail::invalidate_trails()
{
   //get a pointer to the overlay manager
   C_ovl_mgr *om = OVL_get_overlay_manager();
   
   //need a trail pointer
   C_gps_trail *pTrail=NULL;
   
   //get the first trail
   pTrail = dynamic_cast<C_gps_trail *>(om->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   while (pTrail)
   {
      //make it invalid
      pTrail->set_valid(FALSE);
      //get the next trail
      pTrail = dynamic_cast<C_gps_trail *>(om->get_next_of_type(pTrail, FVWID_Overlay_MovingMapTrail));
   }
   
   //redraw everything
   om->InvalidateOverlaysOfType(FVWID_Overlay_MovingMapTrail);
}

/*
Y2K COMPLIANCE
*/
int GPS_get_y2k_compliant_year( int year )
{
/*
Y2K
The GPS Atomic Clock Base Date Time is:
1980-JAN-05 / 1980-JAN-06 00:00:00.00
Midnight
So it's safe to assume there are no GPS files before 1980.

  However we will add a 10 year safety buffer for good measure
  We will treat the 2 digit years 71-99 as being 1971-1999
  We will treat the 2 digit years 00-70 as being 2000-2070
   */
#define GPS_Y2K_1900_OFFSET (70)
   
   //do we have a 2 digit year?
   if (year < 100)
   {
      //let's play guess the millenium
      if (year > GPS_Y2K_1900_OFFSET)
         //20th century
         year += 1900;
      else
         //21st century
         year += 2000;
   }
   
   //compliance
   return year;
}

boolean_t C_gps_trail::visual_occlusion( CPoint p1, CPoint p2 )
{
   //get the vector between the two points
   CPoint dp = (p2-p1);
   
   //don't get closer than this
#define MINPROXIMITY (m_radius*3)
   //this calculation avoids the sqrt function
#define MINPROXIMITY2 (MINPROXIMITY * MINPROXIMITY)
   
   //what is the proximity
   int dist2 = (int)( dp.x * dp.x + dp.y * dp.y );
   
   //are we too close?
   boolean_t bResult = (dist2 < MINPROXIMITY2);
   
   //return the answer
   return bResult;
}

void C_gps_trail::handle_mapscale_changes( ActiveMap* map)
{
   //get the current mapping controls
   d_geo_t deg_per_pixel;
   map->get_vmap_degrees_per_pixel(&deg_per_pixel.lat, &deg_per_pixel.lon);

   //no need to do anything if it's unchanged
   if (deg_per_pixel.lat == m_deg_per_pixel.lat &&
      deg_per_pixel.lon == m_deg_per_pixel.lon)
      return;
   
   //the map scale has changed
   m_deg_per_pixel = deg_per_pixel;

   //determine which points will be shown for this mapscale
   set_viewable_points(map);
   
   m_display_list_valid = false;

}

void C_gps_trail::handle_mapproj_changes(void)
{
   m_display_list_valid = false;
}

//transform a view scalar into a geo scalar
double C_gps_trail::view_distance_to_geo_distance( ActiveMap* map, double viewDistance)
{
   //get the client rectangle
   CRect crect;
   MapView* map_view = fvw_get_view();
   if (map_view)
      map_view->GetClientRect(&crect);
   
   //get the center point of the view
   CPoint vpoint = crect.CenterPoint();
   
   //where is the center point in the world
   d_geo_t a;
   map->surface_to_geo(vpoint.x, vpoint.y, &a.lat, &a.lon);
   
   //move min distance away from center of view
   vpoint.x += (long)viewDistance;
   
   //where is the min distance point in the world
   d_geo_t b;
   map->surface_to_geo(vpoint.x, vpoint.y, &b.lat, &b.lon);
   
   //how far apart are these two points geo wise
   double range,angle;
   int status = GEO_distance
      (
      a.lat, a.lon,
      b.lat, b.lon,
      &range,
      &angle
      );
   
   //pass back the distance
   return range;
}

void C_gps_trail::set_viewable_points( ActiveMap* map)
{
   //what is the minimum proximity geo units
   double minGeoDistance = view_distance_to_geo_distance(map, MINPROXIMITY);
   
   //get the first point
   POSITION p;
   GPSPointIcon *prev_viewable_point = get_icon_list()->get_first(p);
   
   //gotta have at least one point
   if (!prev_viewable_point)
      return;
   
   //the first trail point is always viewable
   prev_viewable_point->m_viewable=true;
   
   //test every point in the list
   while(p)
   {
      //get the next point in the list
      GPSPointIcon *next_point = get_icon_list()->get_next(p);

      // comments and selected points are always viewable
      if (next_point->has_comment() || next_point->get_selected() || next_point->GetLinksCount() > 0)
      {
         next_point->m_viewable = true;
         prev_viewable_point = next_point;
         continue;
      }
      
      //how far apart are these two points geo wise
      double range,angle;
      int status = GEO_distance
         (
         prev_viewable_point->get_latitude(),
         prev_viewable_point->get_longitude(),
         next_point->get_latitude(),
         next_point->get_longitude(),
         &range,
         &angle
         );
      
      //are they too close
      if (range < minGeoDistance)
         //this point will never be shown in the view at this mapscale
         next_point->m_viewable = false;
      else
      {
         //this point will be shown when in view at this mapscale
         next_point->m_viewable = true;
         prev_viewable_point = next_point;
      }
   }
}

////////////////////////
//TRAIL PLAYBACK STUFF//
////////////////////////

/*
For purposes of trail playback,
We need to know whether a point is in the past present or future
*/
boolean_t C_gps_trail::is_in_the_future( GPSPointIcon *point )
{
   COleDateTime there = point->get_date_time();
   if (there.GetStatus() == COleDateTime::null)
      return TRUE;
   
   return (there > m_current_view_time);
}


boolean_t C_gps_trail::in_playback_mode()
{
   return (CMainFrame::IsPlaybackDialogActive());
}

POSITION C_gps_trail::get_last_position()
{
   return get_icon_list()->get_last_position();
}

COleDateTimeSpan C_gps_trail::get_trail_duration()
{
   POSITION pos;
   //get the first point in the list
   GPSPointIcon *first = get_icon_list()->get_first(pos);
   
   //get the last point in the list
   GPSPointIcon *last = get_icon_list()->get_last(pos);
   
   //get the time stamps
   COleDateTime dt0 = first->get_date_time();
   COleDateTime dt1 = last->get_date_time();
   
   //calc the difference
   COleDateTimeSpan dts = (dt1-dt0);
   
   //duration result
   return dts;
}

void C_gps_trail::invalidate_trail_time_slice(COleDateTime dt0, COleDateTime dt1,
                                              boolean_t do_invalidate)
{
   POSITION position;
   
   boolean_t new_show_status;
   
   // swap the times, if necessary, so that dt0 is always less than dt1.  If
   // this is the case then then the current position has moved backwards
   // in time.  So, the points in the given time interval will no longer be
   // seen
   if (dt1 < dt0)
   {
      COleDateTime tmp = dt0;
      dt0 = dt1;
      dt1 = tmp;
      
      new_show_status = FALSE;
   }
   // Otherwise, we are moving foward in time.  So the points in the time
   // interval will now be seen
   else
   {
      new_show_status = TRUE;
   }
   
   GPSPointIcon *point = get_icon_list()->get_first(position);
   
   // pass over all points in the list until we get to the
   // first point that is past the start time, dt0
   while(position != NULL && point != NULL && point->get_date_time() <= dt0)
      point = get_icon_list()->get_next(position);
   
   // invalidate points starting from the point found above up to
   // any points that are before the end time, dt1
   while(point != NULL && point->get_date_time() <= dt1)
   {
      if (point->m_viewable)
      {
         point->m_show_point = new_show_status;
         
         if (do_invalidate)
            invalidate_object(point, TRUE);
      }
      
      if (position)
         point = get_icon_list()->get_next(position);
      else
         point = NULL;
   }
}

// set_current_view_time_trail - sets the position of the ship based
// on the current view time.  Returns TRUE if the ships position changed,
// FALSE otherwise
boolean_t C_gps_trail::set_current_view_time_trail(MapProj *map)
{
   boolean_t do_invalidate = (!m_TcontinuousCentering || !m_edit_on);
   
   //trivial rejection
   if (!m_current_point.is_valid())
      return FALSE;
   
   //get the current point time
   COleDateTime dt0 = m_current_point.get_date_time();
   
   // invalidate the old current position if it has changed
   m_current_point.m_rect = invalidate_object(&m_current_point, FALSE);
   CRect old_rect = m_current_point.m_rect;

   POSITION position = m_current_point.m_display_list_top.GetHeadPosition();
   while (position)
      m_current_point.m_display_list_top.GetNext(position)->invalidate();
   position = m_current_point.m_display_list_bottom.GetHeadPosition();
   while (position)
      m_current_point.m_display_list_bottom.GetNext(position)->invalidate();

   // goto new position
   m_current_view_time = CMainFrame::GetPlaybackDialog().get_current_time();
   SetPlaybackPosition(m_current_view_time);

   // update the predictive path points if necessary
   //
   update_predictive_path(map, m_current_view_time);

   MapView* map_view = fvw_get_view();
   if (map_view &&
      map_view->GetMapProjParams().type == GENERAL_PERSPECTIVE_PROJECTION)
   {
      update_predictive_points();
      update_3d_camera();
      update_TAMask();
      return FALSE;
   }
   
   m_p3DCatchUpTimer->stop();   // Don't need in 2D mode

   // get the new current point time
   COleDateTime dt1 = m_current_point.get_date_time();

   // if the position hasn't changed
   if (dt0 == dt1)
   {
      // make sure we always set the current point's rect, even if the position
      // hasn't changed.  Not sure why, but somehow the shipsymbol's rect is 
      // getting reset to (0,0,0,0).  Need to find out why and remove this block
      {
         d_geo_t anchor = {m_current_point.get_latitude(), m_current_point.get_longitude() };
         m_shipSymbol->set_anchor(anchor);
         m_shipSymbol_2->set_anchor(anchor);
         m_shipSymbol->set_rotation(m_north_up_angle);
         m_shipSymbol_2->set_rotation(m_north_up_angle < 180.0 ? 90.0 : 270.0);
         m_shipSymbol->prepare_for_redraw(map);
         m_shipSymbol_2->prepare_for_redraw(map);

         m_current_point.m_rect = m_shipSymbol->get_rect_w_selection();
      }

      return FALSE;
   }

   // set the current point's angle
   m_north_up_angle = get_current_heading(0);

   // pretend to draw the ship so we will have the new rect
   d_geo_t anchor = { m_current_point.get_latitude(), m_current_point.get_longitude() };
   m_shipSymbol->set_anchor(anchor);
   m_shipSymbol_2->set_anchor(anchor);
   m_shipSymbol->set_rotation(m_north_up_angle);
   m_shipSymbol_2->set_rotation(m_north_up_angle < 180.0 ? 90.0 : 270.0);
   m_shipSymbol->prepare_for_redraw(map);
   m_shipSymbol_2->prepare_for_redraw(map);

   CFvwUtil *futil = CFvwUtil::get_instance();
   if (m_properties.GetBullseyeProperties()->get_bullseye_on())
   {
      m_bullseye.SetCenterLocation(m_current_point.get_latitude(), m_current_point.get_longitude());
      m_bullseye.SetNumRangeRings(m_properties.GetBullseyeProperties()->get_num_range_rings());
      m_bullseye.SetDistBetweenRings(m_properties.GetBullseyeProperties()->get_range_radius());
      m_bullseye.SetHeading(m_current_point.m_true_heading);
      m_bullseye.SetRelativeAzimuth(m_properties.GetBullseyeProperties()->get_relative_azimuth());
      m_bullseye.SetNumRadials(m_properties.GetBullseyeProperties()->get_num_radials());
      m_bullseye.SetAngleBetweenRadials(m_properties.GetBullseyeProperties()->get_angle_between_radials());

      m_bullseye.prepare_for_redraw(map);
      m_current_point.m_rect = m_bullseye.get_rect();
      m_current_point.m_rect |= m_shipSymbol->get_rect_w_selection();
   }
   else
      m_current_point.m_rect = m_shipSymbol->get_rect_w_selection();

   if (m_properties.get_display_altitude_label() && m_current_point.m_msl != GPS_UNKNOWN_ALTITUDE)
   {
      POINT cpt[4];
      const int ship_size = m_properties.get_ship_size();

      double alt_ft = METERS_TO_FEET(m_current_point.m_msl) / 100.0;
      CString alt_str;
      alt_str.Format("%d", (int)(alt_ft + 0.5));
      while (alt_str.GetLength() < 3)
         alt_str = "0" + alt_str;

      CString font_name;
      int size, attributes;
      int fg_color;
      int bg_type, back_color;
      OvlFont &font = m_properties.get_nickname_font();
      font.get_font(font_name, size, attributes);
      font.get_foreground(fg_color);
      font.get_background(bg_type, back_color);

      int width = 0, height = 0;
      CWnd *wnd = CWnd::GetDesktopWindow();
      if (wnd)
      {
         CDC *dc = wnd->GetDC();
         if (dc)
         {
            futil->get_text_size(dc, alt_str, font_name, 
               size, attributes, &width, &height);
            wnd->ReleaseDC(dc);
         }
      }

      int x_offset = -(int)((ship_size + width / 2) * sin(DEG_TO_RAD(90 - m_north_up_angle - map->actual_rotation())) + 0.5);
      int y_offset = -(int)((ship_size + width / 2) * cos(DEG_TO_RAD(90 - m_north_up_angle - map->actual_rotation())) + 0.5);

      int x, y;
      map->geo_to_surface(m_current_point.get_latitude(), m_current_point.get_longitude(),
         &x, &y);
      futil->compute_text_poly(x + x_offset, y + y_offset,UTIL_ANCHOR_CENTER_CENTER,
         width, height, 0.0, cpt);

      CRect altitude_label_bounds = CRect(cpt[0].x, cpt[0].y, cpt[2].x, cpt[2].y);

      m_current_point.m_rect |= altitude_label_bounds;
   }

   if (m_properties.get_nickname_label().GetLength() && m_properties.GetDisplayNickname())
   {
      POINT cpt[4];
      const int ship_size = m_properties.get_ship_size();

      CString font_name;
      int size, attributes;
      int fg_color;
      int bg_type, back_color;
      OvlFont &font = m_properties.get_nickname_font();
      font.get_font(font_name, size, attributes);
      font.get_foreground(fg_color);
      font.get_background(bg_type, back_color);

      int width = 0, height = 0;
      CWnd *wnd = CWnd::GetDesktopWindow();
      if (wnd)
      {
         CDC *dc = wnd->GetDC();
         if (dc)
         {
            futil->get_text_size(dc, m_properties.get_nickname_label(), font_name, 
               size, attributes, &width, &height);
            wnd->ReleaseDC(dc);
         }
      }

      int x_offset = -(int)((ship_size + width / 2) * sin(DEG_TO_RAD(270 - m_north_up_angle - map->actual_rotation())) + 0.5);
      int y_offset = -(int)((ship_size + width / 2) * cos(DEG_TO_RAD(270 - m_north_up_angle - map->actual_rotation())) + 0.5);

      int x, y;
      map->geo_to_surface(m_current_point.get_latitude(), m_current_point.get_longitude(),
         &x, &y);
      futil->compute_text_poly(x + x_offset, y + y_offset,UTIL_ANCHOR_CENTER_CENTER,
         width, height, 0.0, cpt);

      CRect nickname_label_bounds = CRect(cpt[0].x, cpt[0].y, cpt[2].x, cpt[2].y);

      m_current_point.m_rect |= nickname_label_bounds;
   }

   // invalidate the location of the old current position
   invalidate_rect(old_rect);

   // invalidate the new current position
   if (do_invalidate)
   {
      invalidate_rect(m_current_point.m_rect);

      POSITION position = m_current_point.m_display_list_top.GetHeadPosition();
      while (position)
      {
         OvlElement *element = m_current_point.m_display_list_top.GetNext(position);
         element->prepare_for_redraw(map);
         element->invalidate();
      }
      position = m_current_point.m_display_list_bottom.GetHeadPosition();
      while (position)
      {
         OvlElement *element = m_current_point.m_display_list_bottom.GetNext(position);
         element->prepare_for_redraw(map);
         element->invalidate();
      }

      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }

   // notify any range/bearing objects that reference this gps ship
   // of the new location
   position = rb_ref_lst.GetHeadPosition();
   while (position)
   {
      rb_ref_t ref = rb_ref_lst.GetNext(position);
      d_geo_t new_location = { m_current_point.get_latitude(), 
         m_current_point.get_longitude() };
      ref.rb_obj->update(map, new_location, ref.from_not_to);
   }

   // erase/redraw the trail segment that changed and set the show_point
   // flag of the points in the given time interval
   if (display_trail_points())
      invalidate_trail_time_slice(dt0,dt1, do_invalidate);

   // if there is a trail timeout set then we need to invalidate
   // the trail segment that has changed
   if (m_properties.get_timeout_on() && m_properties.get_timeout_seconds() > 0)
   {
      COleDateTimeSpan trail_timeout(0,0,0,m_properties.get_timeout_seconds());

      // we want to make sure the date-times are passed in ordered.  Otherwise, if
      // the first time is after the second, invalidate_trail_time_slice 
      // hide the trail dots which we don't want to do in this case
      if (dt0 - trail_timeout < dt1 - trail_timeout)
         invalidate_trail_time_slice((dt0 - trail_timeout), 
         (dt1 - trail_timeout), do_invalidate);
      else
         invalidate_trail_time_slice((dt1 - trail_timeout),
         (dt0 - trail_timeout), do_invalidate);
   }
   
   //make sure the trail has some of a rect
   if (!display_trail_points())
      m_rect = m_current_point.m_rect;
   
   // update the gps info box if it is opened and is focused on the ship
   if (get_info_box_up() && m_selected_point == &m_current_point)
   {
      // update the info box with the latest point's info
      show_point_info(&m_current_point);
   }

   on_position_changed(map);

   return TRUE;
}

void C_gps_trail::on_position_changed(MapProj *map)
{
   // update course deviation indicator
   cdi_update();
   
   // update the 3d viewpoint position 
   update_3d_camera();

   // TAMask update invalidate area
   update_TAMask();
}

HRESULT C_gps_trail::OnPlaybackStarted()
{
   update_3d_camera();
   return S_OK;
}

HRESULT C_gps_trail::OnPlaybackStopped()
{
   update_3d_camera();
   return S_OK;
}

HRESULT C_gps_trail::OnPlaybackRateChanged(long lNewPlaybackRate)
{
   return S_OK;
}

// Returns the color that is used by the playback dialog's gannt chart for this overlay
HRESULT C_gps_trail::GetGanntChartColor(COLORREF *pColor)
{
   *pColor = CFvwUtil::get_instance()->code2color(m_properties.get_trail_icon_fg_color());
   return S_OK;
}

void C_gps_trail::timer_update_predictive_path()
{
   m_predictive_path_timer_delta++;

   CView *view = UTL_get_active_non_printing_view();
   if (view)
   {
      MapProj *pMap = UTL_get_current_view_map(view);

      if (pMap)
      {
         update_predictive_path(pMap, m_current_view_time + 
            COleDateTimeSpan(0,0,0,m_predictive_path_timer_delta));
   
         MapView* map_view = fvw_get_view();
         if ( map_view &&
               map_view->GetMapProjParams().type == GENERAL_PERSPECTIVE_PROJECTION)
            update_predictive_points();
      }
   }
}

void C_gps_trail::update_predictive_path(MapProj *map, COleDateTime time)
{
   CPredictivePathProperties *pPredictivePathProp = m_properties.GetPredictivePathProperties();

   if (!pPredictivePathProp->get_predictive_path_on())
      return;

   // Invalidate where the predictive path was
   CPredictivePathRenderer::InvalidateTrendDots(map, m_pPredictivePath, pPredictivePathProp);
   CPredictivePathRenderer::InvalidateCenterLine(map, m_pPredictivePath, pPredictivePathProp);

   ComputePredictivePath(time);

   // Invalidate where the predictive path is now
   CPredictivePathRenderer::InvalidateTrendDots(map, m_pPredictivePath, pPredictivePathProp);
   CPredictivePathRenderer::InvalidateCenterLine(map, m_pPredictivePath, pPredictivePathProp);

   OVL_get_overlay_manager()->InvalidateOverlay(this);
}

void C_gps_trail::ComputePredictivePath()
{
   if (!m_properties.GetPredictivePathProperties()->get_predictive_path_on())
      return;

   ComputePredictivePath(m_current_view_time);
}

void C_gps_trail::ComputePredictivePath(COleDateTime time)
{
   PredictivePathSamplePoint samplePoint1;
   PredictivePathSamplePoint samplePoint2(m_current_point.m_dateTime, KNOTS_TO_FEET_PER_S(m_current_point.m_speed_knots),
      m_current_point.m_true_heading, m_current_point.get_latitude(), m_current_point.get_longitude(),
      m_current_point.m_msl );

   // Note the playback tail is actually one point after m_current_point.  To get the point before m_current_point,
   // we will get the point at m_playback_tail's index - 2.
   int index = get_index(m_playback_tail);
   if (index > 1)
   {
      GPSPointIcon *icon = get_at_index(index - 2);
      samplePoint1.dtDateTime = icon->m_dateTime;
      samplePoint1.dSpeedFtPerSec = KNOTS_TO_FEET_PER_S(icon->m_speed_knots);
      samplePoint1.dHeadingDeg = icon->m_true_heading;
      samplePoint1.dLat = icon->get_latitude();
      samplePoint1.dLon = icon->get_longitude();
      samplePoint1.dAltMeters = icon->m_msl;
   }
   else
      samplePoint1 = samplePoint2;

   m_pPredictivePath->ComputePredictivePath(samplePoint1, samplePoint2, time, m_properties.GetPredictivePathProperties());
}

boolean_t C_gps_trail::need_to_recenter_map(ActiveMap* map)
{
   // if the view time dialog is not playing or we are not receiving
   // points from a connection, then there is no need to recenter the
   // map 
   if (!CMainFrame::GetPlaybackDialog().in_playback() && !m_moving_map_feed->is_connected())
      return FALSE;
   
   //SYSTEM AND USER CHECK
   if (!is_autocentering())
      return FALSE;
   
   //CONTINUOUS CENTERING CHECK
   if (get_toggle('CCEN'))
      return TRUE;
   
   // verify that the current point's lat/lon is valid.  If not,
   // there is no need to recenter the map
   if (!GEO_valid_degrees(m_current_point.get_latitude(),
      m_current_point.get_longitude()))
      return FALSE;
   
   // VIEW CHECK
   if (!map->geo_in_surface(m_current_point.get_latitude(),
      m_current_point.get_longitude()))
      return TRUE;
   
   //APRON CHECK
   int x, y;
   map->geo_to_surface
      (
      m_current_point.get_latitude(),
      m_current_point.get_longitude(),
      &x, &y
      );
   boolean_t inApron = m_auto_center_rect.PtInRect(CPoint(x,y));
   if (!inApron)
      return TRUE;
   
   // no need to recenter the map
   return FALSE;
}

void C_gps_trail::SetPlaybackPosition( COleDateTime dt )
{
   //get first node
   POSITION p = get_icon_list()->get_first_position();
   POSITION _p = p;
   
   if (!_p)
      return;
   
   //get the first point
   GPSPointIcon *point = get_icon_list()->get_next(_p);
   
   //test every point in the list
   while(_p)
   {
      POSITION __p = _p;
      //get the next point
      GPSPointIcon *_point = get_icon_list()->get_next(__p);
      
      //gotta have a point
      ASSERT(_point);
      
      //stop when we get to a point in the future
      if (_point->get_date_time() > dt)
         break;
      
      point = _point;
      p = _p;
      
      _p=__p;
   }
   
   //do we have a point?
   if (point)
   {
      //set the current point
      m_playback_tail = point;
      m_playback_position_tail = p;
      m_current_point = *m_playback_tail;
      
      get_icon_list()->get_next(p);
      if (p)
      {
         m_playback_position_next = p;
         m_playback_position_tail = p;
         m_playback_tail = get_icon_list()->get_next( m_playback_position_next );
      }
   }
}

//count the number of open GPS trails
int C_gps_trail::get_trail_count()
{
   //get a point to the overlay manager
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   
   //better be valid
   if (!ovl_mgr)
      return 0;
   
   //the number of open trails
   int cnt;
   
   //get a pointer to the first trail
   C_gps_trail *pTrail = dynamic_cast<C_gps_trail *>(ovl_mgr->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   for (cnt=0; pTrail; cnt++)
      //get the next one
      pTrail = dynamic_cast<C_gps_trail *>(ovl_mgr->get_next_of_type(pTrail, FVWID_Overlay_MovingMapTrail));
   
   //how many were there?
   return cnt;
}

boolean_t C_gps_trail::display_trail_points()
{
   return m_TtrailPoints;
}

C_gps_trail *C_gps_trail::get_topmost_trail()
{
   //get a point to the overlay manager
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   
   //better be valid
   if (!ovl_mgr)
      return NULL;
   
   //get a point to the first trail
   C_gps_trail *pTrail = dynamic_cast<C_gps_trail *>(ovl_mgr->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   
   //pass back the trail pointer
   return pTrail;
}

boolean_t C_gps_trail::is_topmost_trail()
{
   //get the first trail
   C_gps_trail *pTrail = get_topmost_trail();
   
   //are we the topmost trail?
   boolean_t bResult = (pTrail == this);
   
   //return the answer
   return bResult;
}

void C_gps_trail::DoDateTimeFirstPassAnalysis
(
 GPSPointIcon *&firstValidPoint,
 GPSPointIcon *&lastValidPoint,
 GPSPointIcon *&firstInvalidPoint,
 GPSPointIcon *&lastInvalidPoint,
 int& validCount,
 int& invalidCount,
 int& validBadSequenceCount,
 int& invalidBadSequenceCount,
 int& firstValidPointInvalidBadSequenceCount
 )
{
   firstValidPoint = NULL;
   lastValidPoint = NULL;
   firstInvalidPoint = NULL;
   lastInvalidPoint = NULL;
   validCount=0;
   invalidCount=0;
   validBadSequenceCount=0;
   invalidBadSequenceCount=0;
   firstValidPointInvalidBadSequenceCount=0;
   
   //get first node
   POSITION p = get_icon_list()->get_first_position();
   
   //search every point in the list
   while(p)
   {
      GPSPointIcon *point = get_icon_list()->get_next(p);
      
      //what kind of date do we have valid or invalid
      if (point->valid_date())
      {
         //inc the count
         validCount++;
         
         // check the time sequence
         if (lastValidPoint)
            if (point->m_dateTime < lastValidPoint->m_dateTime)
            {
               CString msg;
               msg.Format("The time for the point at time %s is before the last valid point which was at time %s",
                  COleDateTime(point->m_dateTime).Format("%b %d %H:%M:%S %Y"),
                  COleDateTime(lastValidPoint->m_dateTime).Format("%b %d %H:%M:%S %Y"));

               ERR_report(msg);
               validBadSequenceCount++;
            }
            
            //remember the last point with a good date
            lastValidPoint = point;
            
            //remember the first point with a good date
            if (!firstValidPoint)
            {
               firstValidPoint = point;
               firstValidPointInvalidBadSequenceCount = invalidBadSequenceCount;
            }
      }
      else
      {
         //inc the count
         invalidCount++;
         
         //check the time sequence
         if (lastInvalidPoint)
            if (point->m_dateTime < lastInvalidPoint->m_dateTime)
               invalidBadSequenceCount++;
            
            //remember the last point with a missing date
            lastInvalidPoint = point;
            
            //remember the first point with a missing date
            if (!firstInvalidPoint)
               firstInvalidPoint = point;
      }
   }
}

boolean_t C_gps_trail::NormalizeTrailPointDates()
{
   GPSPointIcon *firstValidPoint;
   GPSPointIcon *lastValidPoint;
   GPSPointIcon *firstInvalidPoint;
   GPSPointIcon *lastInvalidPoint;
   int validCount;
   int invalidCount;
   int validBadSequenceCount;
   int invalidBadSequenceCount;
   int firstValidPointInvalidBadSequenceCount;
   
   //Take a look around
   DoDateTimeFirstPassAnalysis(firstValidPoint, lastValidPoint,firstInvalidPoint,
      lastInvalidPoint, validCount, invalidCount, validBadSequenceCount,
      invalidBadSequenceCount, firstValidPointInvalidBadSequenceCount);
   
   //KNOWN DATES ARE CORRECTLY ORDERED
   
   //If there are no unknown dates then we're all done
   if (invalidCount==0)
      return TRUE;
   
   //THERE ARE SOME UNKNOWN DATES
   
   //Are there any known dates?
   if (validCount==0)
   {
      //EVERY DATE IS UNKNOWN
      
      //If they're all in order then we're done      
      if (invalidBadSequenceCount==0)
         return TRUE;
      
      //Get 'em all in line!!!
      DoForcedDateSequencing();
      return TRUE;
   }
   
   //THERE ARE SOME KNOWN DATES
   
   //get the first point in the list
   GPSPointIcon *first = get_icon_list()->get_first();
   ASSERT(firstValidPoint);
   
   //If the first point is good then just sequence
   if (firstValidPoint == first)
   {
      //Get 'em all in line!!!
      DoForcedDateSequencing();
      return TRUE;
   }
   
   //THERE ARE UNKNOWNS BEFORE THE FIRST GOOD DATE
   
   //are all rollovers (if any) after the first good date?
   if (invalidBadSequenceCount==0 || firstValidPointInvalidBadSequenceCount==0)
   {
      //set the first date
      first->set_date( firstValidPoint->get_year(), firstValidPoint->get_month(), firstValidPoint->get_day() );
      //Get 'em all in line!!!
      DoForcedDateSequencing();
      return TRUE;
   }
   
   //THERE ARE ROLLOVERS IN THE UNKNOWN DATES
   //THERE ARE UNKNOWN ROLLOVERS BEFORE THE FIRST GOOD DATE
   
   //get the first good date
   COleDateTime date0 = firstValidPoint->m_dateTime;
   
   //subtract the number rollover days that occurred before the first good date
   date0 -= COleDateTimeSpan( firstValidPointInvalidBadSequenceCount );
   
   //set the first date
   first->set_date( date0.GetYear(), date0.GetMonth(), date0.GetDay() );
   
   //Get 'em all in line!!!
   DoForcedDateSequencing();
   return TRUE;
}

void C_gps_trail::DoForcedDateSequencing()
{
   POSITION p;
   GPSPointIcon *prev;
   GPSPointIcon *next;
   
   next = get_icon_list()->get_first(p);
   
   while(p)
   {
      prev = next;
      next = get_icon_list()->get_next(p);
      midnight_rollover_check( *prev, *next );
   }
}

boolean_t C_gps_trail::is_autocentering()
{
   //   if (get_toggle('ACEN'))
   if (auto_center_on())
      if (is_topmost_trail())
         return TRUE;
      
      return FALSE;
}

boolean_t C_gps_trail::is_autorotating()
{
   //   if (get_toggle('AROT'))
   if (auto_rotate_on())
      if (is_topmost_trail())
         //   if (is_autocentering())
         return TRUE;
      
      return FALSE;
}

boolean_t C_gps_trail::auto_center_on()
{
   if (!get_edit_on())
      return FALSE;
   
   return get_toggle('ACEN');
}

boolean_t C_gps_trail::auto_rotate_on()
{
   if (!get_edit_on())
      return FALSE;
   
   return get_toggle('AROT');
}

////////////////////////////
//TIME VIEW PROCESSING STUFF
////////////////////////////

// Returns the beginning and ending times for the overlay's time segment.  If the time segment is valid 
// set pTimeSegmentValue to a non-zero integer, otherwise set it to zero.  An example where the time segment
// would be invalid is an uncalculated route.  In this case, the overlay is not accounted for in the overall
// clock time and does not show up in the playback dialog's gannt chart
HRESULT C_gps_trail::GetTimeSegment( DATE *pBegin, DATE *pEnd, long *pTimeSegmentValid)
{
   COleDateTime begin, end;
   *pTimeSegmentValid = get_view_time_span(begin, end);
   *pBegin = begin;
   *pEnd = end;

   return S_OK;
}

// gets the beginning and ending times for this GPS overlay.  The begin and
// end times take into account the gps trail as well as any coast tracks.
// returns TRUE if the beging and end time are valid.
boolean_t C_gps_trail::get_view_time_span(COleDateTime& begin, COleDateTime& end)
{
   COleDateTime trail_begin, trail_end;
   COleDateTime coast_track_begin, coast_track_end;
   COleDateTime rb_begin, rb_end;
   
   if (m_is_closing)
      return FALSE;
   
   // try to get a beginning and ending time for the GPS trail
   boolean_t trail_status = get_view_time_span_trail(trail_begin, trail_end);
   
   // try to get a beginning and ending time for any coast tracks
   boolean_t ctrack_status = m_coast_track_mgr->get_view_time_span(
      coast_track_begin, coast_track_end);

   boolean_t rb_status = m_rb_mgr->get_view_time_span(rb_begin, rb_end);

   // if we couldn't get times for the GPS track or the coast tracks then there
   // is nothing we can do
   if (!trail_status && !ctrack_status)
   {
      if (rb_status)
      {
         begin = rb_begin;
         end = rb_end;
         return TRUE;
      }
      // the begin and end date times are invalid
      else
         return FALSE;
   }
   // otherwise, if the trail status is invalid (but the coast track status
   // is valid), then return the values obtained from the coast track manager
   else if (!trail_status)
   {
      // if the range and bearing status is valid also, then intersect
      // the coast track times with the range and bearing times
      if (rb_status)
      {
         if (coast_track_begin < rb_begin &&
            coast_track_begin < rb_end)
            begin = coast_track_begin;
         else
            begin = rb_begin;

         if (coast_track_end > rb_begin &&
            coast_track_end > rb_end)
            end = coast_track_end;
         else
            end = rb_end;
      }
      else
      {
         begin = coast_track_begin;
         end = coast_track_end;
      }
      return TRUE;
   }
   // otherwise, if the coast status is invalid (but the track status 
   // is valid), then return the values obtained from get_view_time_span_trail())
   else if (!ctrack_status)
   {
      // if the range and bearing status is valid also, then intersect
      // the trail times with the range and bearing times
      if (rb_status)
      {
         if (rb_begin < trail_begin &&
            rb_begin < trail_end)
            begin = rb_begin;
         else
            begin = trail_begin;

         if (rb_end > trail_begin &&
            rb_end > trail_end)
            end = rb_end;
         else
            end = trail_end;
      }
      else
      {
         begin = trail_begin;
         end = trail_end;
      }
      return TRUE;
   }
   // otherwise, both the trail and coast track status are valid so return
   // the intersection of these times
   else
   {
      if (coast_track_begin < trail_begin &&
         coast_track_begin < trail_end)
         begin = coast_track_begin;
      else
         begin = trail_begin;
      
      if (coast_track_end > trail_begin &&
         coast_track_end > trail_end)
         end = coast_track_end;
      else
         end = trail_end;

      // if the range and bearing status is also valid, then intersect
      // the intersection with the range and bearing times
      if (rb_status)
      {
         if (rb_begin < begin &&
            rb_begin < end)
            begin = rb_begin;
         else
            begin = begin;

         if (rb_end > begin &&
            rb_end > end)
            end = rb_end;
         else
            end = end;
      }

      return TRUE;
   }
}

// get the time span for the current GPS trail.  Returns TRUE if the start
// and end time are valid  
boolean_t C_gps_trail::get_view_time_span_trail(COleDateTime &start, 
                                                COleDateTime &end)  
{
   boolean_t bResult = NormalizeTrailPointDates();
   if (!bResult)
      return FALSE;
   
   // list position
   POSITION pos;
   
   // get the first point in the list
   GPSPointIcon *first = get_icon_list()->get_first(pos);
   
   // get the last point in the list
   GPSPointIcon *last = get_icon_list()->get_last(pos);
   
   // do we have a begin and an end?  If not then return FALSE
   if (first ==NULL || last == NULL)
   {
      return FALSE;
   }
   
   // get the beginning and ending time stamps for the GPS trail
   start = first->get_date_time();
   end = last->get_date_time();

   // The following is required to work-around a bug in _tcsftime introduced in Visual Studio 2005.  The
   // bug has been fixed in VS2005 SP1.  Once FalconView moves to SP1 runtimes the following can be removed.
   //
   if (start.GetYear() < 1900 || end.GetYear() < 1900)
   {
      const int nYearShift = max(1900 - start.GetYear(), 1900 - end.GetYear());

      GPSPointIcon *pCurrent = get_icon_list()->get_first();
      while (pCurrent)
      {
         COleDateTime dt = pCurrent->get_date_time();
         pCurrent->set_date(dt.GetYear() + nYearShift, dt.GetMonth(), dt.GetDay());
         pCurrent = get_icon_list()->get_next();
      }

      start = first->get_date_time();
      end = last->get_date_time();
   }
   
   // the begin and end date times are valid
   return TRUE;
}

HRESULT C_gps_trail::OnPlaybackTimeChanged(DATE dateCurrentTime)
{
   set_current_view_time(UTL_get_current_view_map(), dateCurrentTime);
   return S_OK;
}

void C_gps_trail::set_current_view_time(MapProj *map, COleDateTime time )
{
   m_current_view_time = time;

   if (map == NULL) 
      return;

   // update the coast tracks if the appropriate flag has been set (the
   // flag gets set in the timer_proc).  This flag is necessary, so the 
   // coast tracks don't get updated everytime a new GPS point is added
   // and the update rate is, say, every 15 seconds (this function gets
   // called each time a new GPS point is added)
   if (map->is_projection_set() && m_coast_track_mgr->get_count() > 0 &&
      (m_update_coast_tracks || !CMainFrame::GetPlaybackDialog().is_future_edge() ||
      !CMainFrame::GetPlaybackDialog().in_playback()))
   {
      m_coast_track_mgr->set_current_view_time(map, time);
      m_update_coast_tracks = FALSE;
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }
   
   // tell the range bearing manager the current view time and
   // the current own-ship position
   d_geo_t current_loc;
   current_loc.lat = m_current_point.get_latitude();
   current_loc.lon = m_current_point.get_longitude();
   m_rb_mgr->set_current_view_time(time, current_loc);
   
   const boolean_t position_changed = set_current_view_time_trail(map);
   
   // update the map if necessary
   if (position_changed)
      map_update(map);
}

C_gps_trail *C_gps_trail::get_active_trail()
{
   return dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
}

boolean_t C_gps_trail::is_active_trail()
{
   return this == get_active_trail();
}

// open the coast track dialog if it is not already opened.  Will
// be initialized with the current coast track info from the coast
// track manager
void C_gps_trail::open_gps_tools(int page_number)
{
   // get a pointer to the current gps overlay
   C_gps_trail *gps_trail = get_topmost_trail();
   
   // insure that we have a valid pointer
   if (gps_trail == NULL)
      return;
   
   // construct the modeless property sheet
   m_gpstools_dialog.set_focus(gps_trail, page_number);
}

// returns TRUE if the coast track dialog is opened
boolean_t C_gps_trail::is_gps_tools_opened(int page_number /*=-1*/)
{
   if (!m_gpstools_dialog.GetSafeHwnd())
      return FALSE;
   
   return page_number == -1 ||
      page_number == m_gpstools_dialog.GetActiveIndex();
}

// callback to handle the coast track timing events (coast tracks
// will get updated automatically every so often)
void CALLBACK EXPORT C_gps_trail::coast_track_timer_proc(HWND hWnd,
                                                         UINT nMsg, UINT nIDEvent, 
                                                         DWORD dwTime)
{
   C_gps_trail *gps_trail = get_topmost_trail();
   
   // Make sure we have a valid gps trail pointer.
   // Also, we don't need to update the coast tracks if the view
   // time dialog is currently in playback mode
   if (gps_trail != NULL && !CMainFrame::GetPlaybackDialog().in_playback())
   {
      if (gps_trail->get_coast_track_mgr()->is_timer_on())
      {
         // get the current system time
         COleDateTime current_time;
         CTime time = CTime::GetCurrentTime();
         tm t1;
         time.GetGmtTm(&t1);
         current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
            t1.tm_min, t1.tm_sec);

         // determine whether or not the coast track(s) have
         // timed out
         const boolean_t ctracks_timed_out = 
            gps_trail->get_coast_track_mgr()->timed_out(current_time);

         // if the coast track timer has timed out, then perform
         // the necessary updates
         if (ctracks_timed_out)
         {
            // update the view time dialog
            gps_trail->set_update_coast_tracks(TRUE);
            CMainFrame::GetPlaybackDialog().on_new_absolute_time(current_time);
         }

         // if the view time dialog isn't opened then we need to 
         // invalidate and update the coast tracks and range/bearing objects
         if (!CMainFrame::IsPlaybackDialogActive() && ctracks_timed_out)
         {
            gps_trail->get_coast_track_mgr()->invalidate_coast_tracks();

            //need access to the view map
            CView *view = UTL_get_active_non_printing_view();
            if (view)
            {
               MapProj *map = UTL_get_current_view_map(view);
               gps_trail->get_coast_track_mgr()->update_coast_tracks(map, 
                  current_time);
            }
         }
      }
   }
}

// open the GPS Options property sheet for this trail
void C_gps_trail::on_gps_options()
{
   CString title;
   title.Format("Moving Map Options - %s", OVL_get_overlay_manager()->GetOverlayDisplayName(this));

   CPropertySheet propsheet(title);

   CGpsStyle gps_style_page(get_properties(), this);
   CMovingMapPredictivePathOptionsPage predictive_path_page(get_properties()->GetPredictivePathProperties(), this);
   CBullseyeOptionsPage bullseyeOptionsPage(get_properties()->GetBullseyeProperties(), this);   
   CAuxDataOptionsPage auxDataOptionsPage(get_properties()->GetAuxDataProperties(), this);

   propsheet.AddPage(&gps_style_page);
   propsheet.AddPage(&predictive_path_page);
   propsheet.AddPage(&bullseyeOptionsPage);

   fvw::IMovingMapFeedAuxiliaryDataPtr auxData;
   if(GetMovingMapFeed()->m_pMovingMapFeed && GetMovingMapFeed()->m_pMovingMapFeed->QueryInterface(__uuidof(fvw::IMovingMapFeedAuxiliaryData), (void**)&auxData) == S_OK)
   {
      propsheet.AddPage(&auxDataOptionsPage);
   }
   
   propsheet.DoModal();
}

// add a comment to the given GPSPointIcon
void C_gps_trail::add_comment(ViewMapProj *map, C_icon *icon)
{
   // the icon needs to be a GPSPointIcon
   if (!icon->is_kind_of("GPSPointIcon"))
      return;
   
   GPSPointIcon *gps_icon = (GPSPointIcon *)icon;
   if (gps_icon->has_comment())
      return;
   
   C_gps_trail *trail = get_topmost_trail();
   if (trail == NULL)
      return;
   
   trail->add_comment_hlpr(gps_icon);
}

void C_gps_trail::add_comment_hlpr(GPSPointIcon *gps_icon)
{
   // close the gps point info dialog if it is opened
   CRemarkDisplay::close_dlg();   
   
   // show the selection box around the given gps icon
   if (m_selected_point != gps_icon)
   {
      /////////////////////////////////////////////
      //the invalid rect
      CRect irect;
      
      //the currently selected object is invalid
      irect = invalidate_object(m_selected_point);
      
      //select the new point
      if (m_selected_point)
         m_selected_point->set_selected(FALSE);
      m_selected_point = gps_icon;
      gps_icon->set_selected(TRUE);
      m_shipSymbol->set_selected(TRUE);

      //the new point is invalid
      irect = invalidate_object(m_selected_point);
      /////////////////////////////////////////////
   }
   
   CGPSCommentTextEdit dlg;
   
   m_comment_dlg_up = TRUE;
   if (dlg.DoModal() == IDOK)
   {
      gps_icon->set_has_comment(TRUE);
      gps_icon->set_comment_txt(dlg.get_text());

      set_modified(TRUE);
   } 
   m_comment_dlg_up = FALSE;
   
   invalidate_object(gps_icon);
}

// edit an existing GPS comment
void C_gps_trail::edit_comment(ViewMapProj *map, C_icon *icon)
{
   // the icon needs to be a GPSPointIcon
   if (!icon->is_kind_of("GPSPointIcon"))
      return;
   
   GPSPointIcon *gps_icon = (GPSPointIcon *)icon;
   if (!gps_icon->has_comment())
      return;
   
   C_gps_trail *trail = get_topmost_trail();
   if (trail == NULL)
      return;
   
   trail->edit_comment_hlpr(gps_icon);
}

// edit the comment of the given icon
void C_gps_trail::edit_comment_hlpr(GPSPointIcon *gps_icon)
{
   // close the gps point info dialog if it is opened
   CRemarkDisplay::close_dlg();
   
   // show the selection box around the given gps icon
   if (m_selected_point != gps_icon)
   {
      /////////////////////////////////////////////
      //the invalid rect
      CRect irect;
      
      //the currently selected object is invalid
      irect = invalidate_object(m_selected_point);
      
      //select the new point
      if (m_selected_point)
         m_selected_point->set_selected(FALSE);
      m_selected_point = gps_icon;
      gps_icon->set_selected(TRUE);
      m_shipSymbol->set_selected(TRUE);
      
      //the new point is invalid
      irect = invalidate_object(m_selected_point);
      /////////////////////////////////////////////
   }
   
   CGPSCommentTextEdit dlg;
   
   // initialize the dialog with the current text
   dlg.set_text(gps_icon->get_comment_txt());
   
   m_comment_dlg_up = TRUE;
   if (dlg.DoModal() == IDOK)
   {
      gps_icon->set_comment_txt(dlg.get_text());
      set_modified(TRUE);
   }
   m_comment_dlg_up = FALSE;
   
   invalidate_object(gps_icon);
}

// delete an existing GPS comment
void C_gps_trail::delete_comment(ViewMapProj *map, C_icon *icon)
{
   // the icon needs to be a GPSPointIcon
   if (!icon->is_kind_of("GPSPointIcon"))
      return;
   
   GPSPointIcon *gps_icon = (GPSPointIcon *)icon;
   if (!gps_icon->has_comment())
      return;
   
   gps_icon->set_comment_txt("");
   gps_icon->set_has_comment(FALSE);
   get_topmost_trail()->set_modified(TRUE);
   
   // redraw the current overlay
   get_topmost_trail()->invalidate_object((GPSPointIcon *)icon, TRUE);
   get_topmost_trail()->set_valid(FALSE);
}

// static
void C_gps_trail::edit_gps_point_links_callback(ViewMapProj *map, C_icon* pPoint)
{
   if (pPoint == NULL)
      return;

   C_gps_trail* pOverlay = static_cast<C_gps_trail*>(pPoint->m_overlay);
   if (pOverlay != NULL)
   {
      // if the object doesn't exist yet, create it
      if (C_gps_trail::m_pLinksEditDlg == NULL)
         C_gps_trail::m_pLinksEditDlg = new CLinksEditDlg(&static_save_links);

      // if the window isn't created, create it
      if (C_gps_trail::m_pLinksEditDlg->GetHwnd() == NULL)
         C_gps_trail::m_pLinksEditDlg->Create();

      // invalidate the previous selected point (if one)
      if (pOverlay->get_selected_point() != NULL)
      {
         pOverlay->invalidate_object(pOverlay->get_selected_point(), TRUE);
         pOverlay->get_selected_point()->set_selected(FALSE);
      }
      
      // set the newly selected point
      pOverlay->set_selected_point(static_cast<GPSPointIcon *>(pPoint));
      pOverlay->get_selected_point()->set_selected(TRUE);
      
      // invalidate the location of the newly selected point
      pOverlay->invalidate_object(pOverlay->get_selected_point(), TRUE);
      pOverlay->set_link_edit_dlg_focus(static_cast<GPSPointIcon *>(pPoint));
   }
}

void C_gps_trail::set_link_edit_dlg_focus(GPSPointIcon *pPoint)
{
   if (m_pLinksEditDlg != NULL && m_pLinksEditDlg->GetHwnd() != NULL)
   {
      if (pPoint != NULL)
      {
         m_links.RemoveAll();
         pPoint->GetLinks(m_links);

         m_pLinksEditDlg->set_focus("MovingMapPoint", &m_links);

         m_pLinksEditPoint = pPoint;
      }
      else
      {
         CString empty;
         empty.Empty();

         m_pLinksEditDlg->set_focus(empty, NULL);
      }
   }
}

// static
void C_gps_trail::static_save_links(CString handle, CStringArray &links)
{
   if (m_pLinksEditPoint != NULL)
   {
      m_pLinksEditPoint->get_overlay()->set_modified(TRUE);
      m_pLinksEditPoint->SetLinks(links);

      OVL_get_overlay_manager()->InvalidateOverlay(m_pLinksEditPoint->get_overlay());
   }
}

void C_gps_trail::view_link_callback(ViewMapProj *map, C_icon* point, LPARAM index)
{
   GPSPointIcon *pGpsPoint = static_cast<GPSPointIcon *>(point);
   if (pGpsPoint != NULL)
   {
      CStringArray links;
      pGpsPoint->GetLinks(links);

      const int nSize = links.GetSize();
      if (index >= 0 && index < nSize)
         CLinksEditDlg::view_link(links[index]);
   }
}

void C_gps_trail::stop_coast_track(ViewMapProj *map, C_icon *icon,
                                   LPARAM lparam)
{
   // the icon needs to be a CoastTRack
   if (!icon->is_kind_of("CoastTrack"))
      return;
   
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay((int)lparam);
   if (overlay == NULL)
      return;
   
   // get the coast tracks index in the list
   int index = (dynamic_cast<C_gps_trail *>(overlay))->get_coast_track_mgr()->get_ctrack_index((CoastTrack *)icon);
   
   (dynamic_cast<C_gps_trail *>(overlay))->get_coast_track_mgr()->stop_coast_track(index);
}

void C_gps_trail::edit_coast_track(ViewMapProj *map, C_icon *icon,
                                   LPARAM lparam)
{
   // the icon needs to be a CoastTRack
   if (!icon->is_kind_of("CoastTrack"))
      return;
   
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay((int)lparam);
   if (overlay == NULL)
      return;
   
   // get the coast tracks index in the list
   int index = (dynamic_cast<C_gps_trail *>(overlay))->get_coast_track_mgr()->get_ctrack_index((CoastTrack *)icon);
   
   (dynamic_cast<C_gps_trail *>(overlay))->get_coast_track_mgr()->modify_coast_track(index);
}

void C_gps_trail::delete_coast_track(ViewMapProj *map, C_icon *icon,
                                     LPARAM lparam)
{
   // the icon needs to be a CoastTRack
   if (!icon->is_kind_of("CoastTrack"))
      return;
   
   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay((int)lparam);
   if (overlay == NULL)
      return;
   
   // get the coast tracks index in the list
   int index = (dynamic_cast<C_gps_trail *>(overlay))->get_coast_track_mgr()->get_ctrack_index((CoastTrack *)icon);
   
   (dynamic_cast<C_gps_trail *>(overlay))->get_coast_track_mgr()->delete_coast_track(index);
}

// add an HWND to the connect/disconnect notification list
void C_gps_trail::add_HWND_to_connect_notify_lst(HWND hWnd)
{
   m_hwnd_connect_notify_lst.AddTail(hWnd);
}

// remove an HWND from the connect/disconnect notification list
void C_gps_trail::remove_HWND_from_connect_notify_lst(HWND hWnd)
{
   POSITION position = m_hwnd_connect_notify_lst.Find(hWnd);
   
   if (position)
      m_hwnd_connect_notify_lst.RemoveAt(position);
}

// add an HWND to the current position notification list
void C_gps_trail::add_HWND_to_current_pos_notify_lst(HWND hWnd)
{
   m_hwnd_current_pos_notify_lst.AddTail(hWnd);
}

// remove an HWND from the current position notification list
void C_gps_trail::remove_HWND_from_current_pos_notify_lst(HWND hWnd)
{
   POSITION position = m_hwnd_current_pos_notify_lst.Find(hWnd);
   
   if (position)
      m_hwnd_current_pos_notify_lst.RemoveAt(position);
}

// test to see if this route can do a single point snap to at this point
boolean_t C_gps_trail::test_snap_to(ViewMapProj *view, CPoint point)
{
   // we want to be able to snap to coast tracks
   return m_coast_track_mgr->hit_test(point);
}

// get the snap to objects for this route at this point
boolean_t C_gps_trail::do_snap_to(ViewMapProj *vmp, CPoint point,
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
{
   map_projection_utils::CMapProjWrapper view(vmp->GetSettableMapProj());

   C_icon *icon = hit_test(&view, point);

   // make sure the icon is a coast track
   if (icon && icon->is_kind_of("CoastTrack"))
   {

      // cast the C_icon to a CoastTrack
      CoastTrack *ctrack_icon = (CoastTrack *)icon;

      // fill in the snap-to info structure and add it to the list
      SnapToInfo *snap_to = new SnapToInfo;
      snap_to->m_strKey = icon->get_key();
      snap_to->m_strTooltip = icon->get_tool_tip();
      snap_to->m_overlayDescGuid = icon->m_overlay->get_m_overlayDescGuid();
      snap_to->m_strOverlayName = OVL_get_overlay_manager()->GetOverlayDisplayName(icon->m_overlay);

      CString icon_type(icon->get_tool_tip());
      if (icon_type.GetLength() > 10)
      {
         ERR_report("Tool tip too long.");
         icon_type = icon_type.Left(10);
      }
      strcpy_s(snap_to->m_icon_type, 11, icon_type);

      // set the icon-description string (max 50 characters)
      CString icon_description(icon->get_help_text());
      if (icon_description.GetLength() < 51)
      {
         if (icon_description.GetLength() + OVL_get_overlay_manager()->GetOverlayDisplayName(this).GetLength() 
            < 48)
         {
            icon_description += ", ";
            icon_description += OVL_get_overlay_manager()->GetOverlayDisplayName(this);
         }
      }
      else
      {
         ERR_report("Help text too long.");
         icon_description = icon_description.Left(50);
      }
      strcpy_s(snap_to->m_icon_description, 51, icon_description);

      CoastTrackData &ctrack_data = ctrack_icon->get_coast_track_data();
      snap_to->m_lat = ctrack_data.get_current_position().lat;
      snap_to->m_lon = ctrack_data.get_current_position().lon;
      snap_to->m_pnt_src_rs = SnapToInfo::OTHER;
      strcpy_s(snap_to->m_src_datum, 6, "WGS84");

      snap_to_list.AddTail(snap_to);

      return TRUE;
   }

   return FALSE;
}

void C_gps_trail::add_range_bearing_reference(RangeBearing *rb_obj, 
   boolean_t from_not_to)
{
   rb_ref_lst.AddTail(rb_ref_t(rb_obj, from_not_to));
   d_geo_t current_location = { m_current_point.get_latitude(), 
      m_current_point.get_longitude() };
   rb_obj->update(NULL, current_location, from_not_to);
}

void C_gps_trail::remove_range_bearing_reference(RangeBearing *rb_obj)
{
   POSITION position = rb_ref_lst.GetHeadPosition();
   while (position)
   {
      rb_ref_t ref = rb_ref_lst.GetAt(position);
      if (ref.rb_obj == rb_obj)
      {
         rb_ref_lst.RemoveAt(position);
         break;
      }
      rb_ref_lst.GetNext(position);
   }
}

// returns the number of comments associated with this trail
int C_gps_trail::get_number_comments()
{
   int count = 0;

   GPSPointIcon *icon = get_icon_list()->get_first();
   while (icon != NULL)
   {
      if (icon->has_comment())
         count++;

      icon = get_icon_list()->get_next();
   }

   return count;
}

UINT WarningThread(void *p)
{
   C_gps_trail *trail = static_cast<C_gps_trail *>(p);

   // number of times the ship should blink
   int count = 16;

   while (count-- && !trail->get_halt_warning_thread())
   {
      trail->set_show_symbol(!trail->get_show_symbol());
      trail->invalidate_object(&trail->get_current_position(), TRUE);

      Sleep(300);
   }

   trail->set_show_symbol(TRUE);
   trail->invalidate_object(&trail->get_current_position(), TRUE);
   trail->set_warning_displayed(TRUE);

   HANDLE h = trail->get_halt_warning_thread_event();
   if (h != NULL)
      SetEvent(h);

   trail->set_warning_thread_started(FALSE);

   return 0;
}

void C_gps_trail::start_frozen_trail_warning()
{
   if (!m_warning_displayed)
   {
      m_halt_warning_thread = FALSE;
      AfxBeginThread(WarningThread, this);
      m_warning_thread_started = TRUE;
   }
}

void C_gps_trail::stop_frozen_trail_warning(BOOL invalidate /*=TRUE*/)
{
   if (m_warning_thread_started)
   {
      m_halt_warning_thread_event = CreateEvent(NULL, TRUE, FALSE, NULL);
      ResetEvent(m_halt_warning_thread_event);

      m_halt_warning_thread = TRUE;

      DWORD result = WaitForSingleObject(m_halt_warning_thread_event, 2000);
      if (result == WAIT_TIMEOUT)
         ERR_report("Error waiting for frozen trail warning");

      CloseHandle(m_halt_warning_thread_event);
      m_halt_warning_thread_event = NULL;
   }
   else
      m_halt_warning_thread = TRUE;

   m_warning_displayed = FALSE;

   if (invalidate)
      invalidate_object(&get_current_position(), TRUE);
}

BOOL C_gps_trail::IsBetween(GPSPointIcon *pLeft, COleDateTime &dt, GPSPointIcon *pRight)
{
   if (pLeft == NULL && pRight == NULL)
      return FALSE;

   if (pLeft == NULL)
      return dt <= pRight->get_date_time();

   if (pRight == NULL)
      return pLeft->get_date_time() < dt;

   return pLeft->get_date_time() < dt && dt <= pRight->get_date_time();
}

GPSPointIcon *C_gps_trail::ClosestPoint(GPSPointIcon *pLeft, COleDateTime& dt, GPSPointIcon *pRight)
{
   if (pLeft == NULL && pRight == NULL)
      return NULL;

   if (pLeft == NULL)
      return pRight;

   if (pRight == NULL)
      return pLeft;

   COleDateTimeSpan dtSpanLeft = dt - pLeft->get_date_time();
   COleDateTimeSpan dtSpanRight = pRight->get_date_time() - dt;

   return dtSpanLeft < dtSpanRight ? pLeft : pRight;
}

GPSPointIcon *C_gps_trail::FindClosestPointByTime(DATE d)
{
   GPSPointIcon *pLeftBound = get_icon_list()->get_first();
   GPSPointIcon *pRightBound = get_icon_list()->get_next();

   COleDateTime dt(d);

   while(pLeftBound != NULL && pRightBound != NULL)
   {
      if (IsBetween(pLeftBound, dt, pRightBound))
         return ClosestPoint(pLeftBound, dt, pRightBound);

      pLeftBound = pRightBound;
      pRightBound = get_icon_list()->get_next();
   }

   return NULL;
}


void C_gps_trail::Add3DPoint( GPSPointIcon* pPoint )
{
   if ( (BOOL ) m_work_items )
   {
      // Queue up a work item to add the point in a background thread
      //
      CComObject< AddGPSTrailWorkItem >* pIWorkItem;
      HRESULT hr = CComObject< AddGPSTrailWorkItem >::CreateInstance( &pIWorkItem );
      ATLASSERT( SUCCEEDED( hr ) );
      pIWorkItem->Initialize( pPoint->m_sppThis );

      m_work_items->QueueWorkItem( pIWorkItem );
   }
}


void C_gps_trail::Update3DPoint( GPS3DPoint* pPoint )
{
   if ( (BOOL ) m_work_items )
   {
      // Queue up a work item to add or update the point in a background thread
      //
      CComObject< UpdateGPS3DPointWorkItem >* pIWorkItem;
      HRESULT hr = CComObject< UpdateGPS3DPointWorkItem >::CreateInstance( &pIWorkItem );
      ATLASSERT( SUCCEEDED( hr ) );
      pIWorkItem->Initialize( pPoint->m_sppThis, ++(pPoint->m_iUpdateSequence) );

      m_work_items->QueueWorkItem( pIWorkItem );
   }
}


void C_gps_trail::Remove3DPoint( long iPointHandle )
{
   ATL::CCritSecLock lock( s_idThreadInterlock );
   if ( iPointHandle > 0 )
   {
      if ( (BOOL) m_work_items )
      {
         // Queue up a work item to remove the point in a background thread
         CComObject< RemoveGPS3DPointWorkItem >* pIWorkItem;
         HRESULT hr = CComObject< RemoveGPS3DPointWorkItem>::CreateInstance( &pIWorkItem );
         ATLASSERT( SUCCEEDED( hr ) );
         pIWorkItem->Initialize( iPointHandle, this );

         m_work_items->QueueWorkItem( pIWorkItem );
      }
   }
}  // Remove3DPoint()


void C_gps_trail::Remove3DPoint( GPSPointIcon* pPoint )
{
   Remove3DPoint( pPoint->m_3DHandle );
   pPoint->m_3DHandle = 0;
}  // Remove3DPoint( GPSPointIcon* )


void C_gps_trail::Remove3DPoint( GPS3DPoint* pPoint )
{
   Remove3DPoint( pPoint->m_3DHandle );
   pPoint->m_3DHandle = 0;
}  // Remove3DPoint( GPS3DPoint* )


////////////////////////////////////////////////////////////////////////////////
//
// CGPSComment
//
////////////////////////////////////////////////////////////////////////////////

/*-==-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
BEGIN_MESSAGE_MAP(CGPSCommentTextEdit, CDialog)
//{{AFX_MSG_MAP(CGPSCommentTextEdit)
ON_EN_CHANGE(IDC_EDIT, OnEditChange)
ON_BN_CLICKED(IDC_APPLY, OnApply)
ON_WM_SIZE()
ON_WM_GETMINMAXINFO()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGPSCommentTextEdit::CGPSCommentTextEdit(CWnd* pParent /*=NULL*/)
: CDialog(CGPSCommentTextEdit::IDD, pParent)
{
   //{{AFX_DATA_INIT(CGPSCommentTextEdit)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_initialized = FALSE;
}

CGPSCommentTextEdit::~CGPSCommentTextEdit(void)
{
   
}

BOOL CGPSCommentTextEdit::OnInitDialog()
{
   CDialog::OnInitDialog();
   CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT);
   
   // set the text in the edit control
   edit->SetWindowText(m_text);
   
   // set the focus to the edit control
   GotoDlgCtrl(edit);
   
   // select all the characters in the edit control
   edit->SetSel(0,-1);
   
   // let's change the window title to suit the comment dialog
   SetWindowText("Edit Trail Point Comment");

   m_initialized = TRUE;
   
   return FALSE;  // return TRUE unless you set the focus to a control
}

void CGPSCommentTextEdit::OnSize(UINT nType, int cx, int cy)
{
   if (m_initialized)
   {
      CRect rect;
      CRect edit_rect;
      int margin = 8;

      GetWindowRect(&rect);
      rect.DeflateRect(9, 25);
      
      CAlignWnd buttonset;
      buttonset.Add(GetDlgItem(IDOK)->m_hWnd);
      buttonset.Add(GetDlgItem(IDCANCEL)->m_hWnd);
      buttonset.Add(GetDlgItem(IDC_APPLY)->m_hWnd);
      
      buttonset.Align(m_hWnd, rect, ALIGN_RIGHT|ALIGN_TOP);
      
      GetWindowRect(&edit_rect);
      edit_rect.DeflateRect(50, 18);
      edit_rect.OffsetRect(-40, 7);
      ScreenToClient(&edit_rect);
      GetDlgItem(IDC_EDIT)->MoveWindow(&edit_rect, FALSE);

      Invalidate();
   }
   
   CDialog::OnSize(nType, cx, cy);
}

void CGPSCommentTextEdit::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
   lpMMI->ptMinTrackSize = CPoint(162,114);
   lpMMI->ptMaxTrackSize = CPoint(470, 262);

   CDialog::OnGetMinMaxInfo(lpMMI);
}

void CGPSCommentTextEdit::OnOK() 
{  
   CDialog::OnOK();
}

void CGPSCommentTextEdit::OnEditChange()
{
   CEdit *edit_ctrl = (CEdit *)GetDlgItem(IDC_EDIT);
   edit_ctrl->GetWindowText(m_text);
}

void CGPSCommentTextEdit::OnApply()
{   
   
}
/*-==-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

CString extract_element(const CString &str, const CString element, int start_from, int& new_pos)
{
   int i = str.Find("<" + element + ">", start_from);
   int j = str.Find("</" + element + ">", start_from);

   if (i != -1 && j != -1)
   {
      CString M = str.Mid(i + element.GetLength() + 2, j - i - element.GetLength() - 2);
      new_pos = j + element.GetLength() + 3;

      return M;
   }

   return "";
}


////////////////////////////////////////////////////////////////////////////////
//
// GPS3DPoint
//
////////////////////////////////////////////////////////////////////////////////

GPS3DPoint::~GPS3DPoint()
{
   RemoveFrom3D();
}

void GPS3DPoint::RemoveFrom3D()
{
   // Make sure that any prior 3D work items have been processed
   DWORD dwT0 = GetTickCount();
   while ( m_sppThis.use_count() > 1
         && (INT) ( GetTickCount() - dwT0 ) < 10 * 1000 )   // Timeout check
      Sleep( 50 );      // 3D work still queued

   m_pGPSTrail->Remove3DPoint( this );   // May not exist
}

// End of gps.cpp
