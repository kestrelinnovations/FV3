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

// PointsCommandMessageHandler.cpp

#include "stdafx.h"
#include "MovingMapCommandMessageHandler.h"

#include "FalconView/getobjpr.h"
#include "FalconView/include/gps.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/include/param.h"
#include "FalconView/include/TabularEditorDlg.h"
#include "FalconView/mapview.h"
#include "FalconView/MovingMapOverlay/cdi.h"
#include "FalconView/MovingMapOverlay/factory.h"
#include "FalconView/MovingMapOverlay/LoadMovingMapSettingsDialog.h"
#include "FalconView/MovingMapOverlay/MovingMapFeed.h"
#include "FalconView/PlaybackDialog/viewtime.h"
#include "FalconView/resource.h"
#include "FalconView/TAMask/factory.h"

BEGIN_MESSAGE_MAP(MovingMapCommandMessageHandler, CCmdTarget)
   ON_COMMAND(ID_GPS_TABULAR_EDITOR, OnGPSTabularEditor)
   ON_UPDATE_COMMAND_UI(ID_GPS_TABULAR_EDITOR, OnUpdateGPSTabularEditor)
   ON_COMMAND(ID_GPS_CONNECT, OnGPSConnect)
   ON_UPDATE_COMMAND_UI(ID_GPS_CONNECT, OnUpdateGPSConnect)
   ON_COMMAND(ID_GPS_SMOOTH_SCROLLING, OnGPSSmoothScrolling)
   ON_UPDATE_COMMAND_UI(ID_GPS_SMOOTH_SCROLLING, OnUpdateGPSSmoothScrolling)
   ON_COMMAND(ID_CYCLE_CENTER, OnGPSCycleCenter)
   ON_UPDATE_COMMAND_UI(ID_CYCLE_CENTER, OnUpdateGPSCycleCenter)
   ON_COMMAND(ID_GPS_AUTOCENTER, OnGPSAutoCenter)
   ON_UPDATE_COMMAND_UI(ID_GPS_AUTOCENTER, OnUpdateGPSAutoCenter)
   ON_COMMAND(ID_GPS_AUTOROTATE, OnGPSAutoRotate)
   ON_UPDATE_COMMAND_UI(ID_GPS_AUTOROTATE, OnUpdateGPSAutoRotate)
   ON_COMMAND(ID_GPS_TRAILPOINTS, OnGPSTrailPoints)
   ON_UPDATE_COMMAND_UI(ID_GPS_TRAILPOINTS, OnUpdateGPSTrailPoints)
   ON_COMMAND(ID_GPS_TOGGLE_PREDICTIVE_PATH, OnTogglePredictivePath)
   ON_UPDATE_COMMAND_UI(ID_GPS_TOGGLE_PREDICTIVE_PATH, OnUpdateTogglePredictivePath)
   ON_COMMAND(ID_GPS_TOGGLE_TAMASK, OnToggleTerrainMask)
   ON_UPDATE_COMMAND_UI(ID_GPS_TOGGLE_TAMASK, OnUpdateToggleTerrainMask)
   ON_COMMAND(ID_GPS_CDI, OnGPSCDI)
   ON_UPDATE_COMMAND_UI(ID_GPS_CDI, OnUpdateGPSCDI)
   ON_COMMAND(ID_GPS_PLAYBACK, OnGPSPlayback)
   ON_UPDATE_COMMAND_UI(ID_GPS_PLAYBACK, OnUpdateGPSPlayback)
   ON_COMMAND(ID_GPS_STYLES, OnGPSStyles)
   ON_UPDATE_COMMAND_UI(ID_GPS_STYLES, OnUpdateGPSStyles)
   ON_COMMAND(ID_GPS_OPTIONS, OnGPSOptions)
   ON_UPDATE_COMMAND_UI(ID_GPS_OPTIONS, OnUpdateGPSOptions)
   ON_COMMAND(ID_MOVING_MAP_SAVE_SETTINGS, OnMovingMapSaveSettings)
   ON_UPDATE_COMMAND_UI(ID_MOVING_MAP_SAVE_SETTINGS, OnUpdateMovingMapSaveSettings)
   ON_COMMAND(ID_MOVING_MAP_LOAD_SETTINGS, OnMovingMapLoadSettings)
   ON_UPDATE_COMMAND_UI(ID_MOVING_MAP_LOAD_SETTINGS, OnUpdateMovingMapLoadSettings)
   ON_COMMAND(ID_GPS_COAST_TRACK, OnGPSCoastTrack)
   ON_UPDATE_COMMAND_UI(ID_GPS_COAST_TRACK, OnUpdateGPSCoastTrack)
   ON_COMMAND(ID_GPS_RANGE_BEARING, OnRangeBearing)
   ON_UPDATE_COMMAND_UI(ID_GPS_RANGE_BEARING, OnUpdateRangeBearing)
   ON_COMMAND(ID_CDI_CENTER_COURSE, OnCdiCenterCourse)
   ON_UPDATE_COMMAND_UI(ID_CDI_CENTER_COURSE, OnUpdateCdiCenterCourse)
   ON_COMMAND(ID_CDI_CENTER_SHIP, OnCdiCenterShip)
   ON_UPDATE_COMMAND_UI(ID_CDI_CENTER_SHIP, OnUpdateCdiCenterShip)
   ON_COMMAND(ID_CDI_UNITS_FEET, OnCdiUnitsFeet)
   ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_FEET, OnUpdateCdiUnitsFeet)
   ON_COMMAND(ID_CDI_UNITS_KILOMETERS, OnCdiUnitsKilometers)
   ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_KILOMETERS, OnUpdateCdiUnitsKilometers)
   ON_COMMAND(ID_CDI_UNITS_METERS, OnCdiUnitsMeters)
   ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_METERS, OnUpdateCdiUnitsMeters)
   ON_COMMAND(ID_CDI_UNITS_NAUTICAL_MILES, OnCdiUnitsNauticalMiles)
   ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_NAUTICAL_MILES, OnUpdateCdiUnitsNauticalMiles)
   ON_COMMAND(ID_CDI_UNITS_STATUTE_MILES, OnCdiUnitsStatuteMiles)
   ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_STATUTE_MILES, OnUpdateCdiUnitsStatuteMiles)
   ON_COMMAND(ID_CDI_UNITS_YARDS, OnCdiUnitsYards)
   ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_YARDS, OnUpdateCdiUnitsYards)
   ON_COMMAND(ID_GPS_CLOCK, OnGPSClock)
   ON_UPDATE_COMMAND_UI(ID_GPS_CLOCK, OnUpdateGPSClock)
END_MESSAGE_MAP()

void MovingMapCommandMessageHandler::OnGPSTabularEditor()
{
   C_gps_trail *trail;

   //Get the topmost drawing overlay
   trail = static_cast<C_gps_trail*>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   ASSERT(trail);

   trail->OnTabularEditor();
}

void MovingMapCommandMessageHandler::OnUpdateGPSTabularEditor(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(g_tabular_editor_dlg != NULL);
}

void MovingMapCommandMessageHandler::OnGPSConnect()
{
   C_gps_trail::set_toggle('CONN');
}

void MovingMapCommandMessageHandler::OnUpdateGPSConnect(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('CONN') );
}

void MovingMapCommandMessageHandler::OnGPSSmoothScrolling()
{
   C_gps_trail::set_toggle('CCEN');
}

void MovingMapCommandMessageHandler::OnUpdateGPSSmoothScrolling(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('CCEN') );
}

void MovingMapCommandMessageHandler::OnGPSCycleCenter()
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (trail)
      trail->cycle_tracking_center_num();
}

void MovingMapCommandMessageHandler::OnUpdateGPSCycleCenter(CCmdUI* pCmdUI)
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (trail)
   {
      GPSPointIcon icon;
      trail->get_current_point(&icon);
      const BOOL enable_button = icon.has_multiple_centers() && C_gps_trail::get_toggle('ACEN');
      pCmdUI->Enable(enable_button);
   }
}

void MovingMapCommandMessageHandler::OnGPSAutoCenter()
{
   C_gps_trail::set_toggle('ACEN');
}

void MovingMapCommandMessageHandler::OnUpdateGPSAutoCenter(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('ACEN') );
}

void MovingMapCommandMessageHandler::OnGPSAutoRotate()
{
   C_gps_trail::set_toggle('AROT');
}

void MovingMapCommandMessageHandler::OnUpdateGPSAutoRotate(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('AROT') );
}

void MovingMapCommandMessageHandler::OnGPSTrailPoints()
{
   C_gps_trail::set_toggle('TRAI');
}

void MovingMapCommandMessageHandler::OnUpdateGPSTrailPoints(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('TRAI') );
}

void MovingMapCommandMessageHandler::OnTogglePredictivePath()
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (trail)
   {
      int ppath_on = trail->get_properties()->GetPredictivePathProperties()->get_predictive_path_on();
      trail->get_properties()->GetPredictivePathProperties()->set_predictive_path_on(!ppath_on);
      
      // recompute the predictive path when it is toggled on
      if (trail->get_properties()->GetPredictivePathProperties()->get_predictive_path_on())
         trail->ComputePredictivePath();
      else
         trail->get_predictive_points_3D().clear();  // 

      trail->set_modified(TRUE);
      OVL_get_overlay_manager()->invalidate_all();
   }
}

void MovingMapCommandMessageHandler::OnUpdateTogglePredictivePath(CCmdUI* pCmdUI)
{
   C_gps_trail *trail = dynamic_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (trail)
      pCmdUI->SetCheck(trail->get_properties()->GetPredictivePathProperties()->get_predictive_path_on());
}

void MovingMapCommandMessageHandler::OnToggleTerrainMask()
{
   OVL_get_overlay_manager()->toggle_static_overlay(FVWID_Overlay_TerrainAvoidanceMask);
}

void MovingMapCommandMessageHandler::OnUpdateToggleTerrainMask(CCmdUI* pCmdUI)
{
   C_overlay *overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask);
   pCmdUI->SetCheck(overlay != NULL);
}

void MovingMapCommandMessageHandler::OnGPSCDI()
{
   C_gps_trail::set_toggle('CDI');
}

void MovingMapCommandMessageHandler::OnUpdateGPSCDI(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('CDI') );
}

void MovingMapCommandMessageHandler::OnGPSPlayback()
{
   C_gps_trail::set_toggle('PLAY');
}

void MovingMapCommandMessageHandler::OnUpdateGPSPlayback(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::get_toggle('PLAY') );
}

void MovingMapCommandMessageHandler::OnGPSStyles()
{
   C_gps_trail::set_toggle('STYL');

   //OVL_get_overlay_manager()->overlay_options_dialog("C_gps_trail");
   C_gps_trail::get_topmost_trail()->on_gps_options();
}

void MovingMapCommandMessageHandler::OnUpdateGPSStyles(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(C_gps_trail::get_toggle('STYL'));
}

void MovingMapCommandMessageHandler::OnGPSOptions()
{
   C_gps_trail::set_toggle('OPTI');
}

void MovingMapCommandMessageHandler::OnUpdateGPSOptions(CCmdUI* pCmdUI)
{
   // Disable the options button if we are already connected
   C_gps_trail *pOverlay = static_cast<C_gps_trail *>(OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));
   if (pOverlay)
      pCmdUI->Enable(!pOverlay->is_connected());

   pCmdUI->SetCheck( C_gps_trail::get_toggle('OPTI') );
}

void MovingMapCommandMessageHandler::OnMovingMapSaveSettings()
{
   CString strInitialDir = PRM_get_registry_string("Main", "USER_DATA", "") + "\\MovingMapSettings\\";

   CFvwUtil *futil = CFvwUtil::get_instance();
   futil->ensure_directory(strInitialDir);

   static char filter[] = "Moving Map Settings File (*.mms)|*.mms||";
   CFileDialog dlg(FALSE, "mms", strInitialDir + "*.mms", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter,
      fvw_get_frame());
   dlg.m_ofn.lpstrTitle = "Save Moving Map Settings";
   
   // get a filename from the user and save the file
   if (dlg.DoModal() == IDOK)
   {  
      C_gps_trail *pTrail = static_cast<C_gps_trail*>
         (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));

      if (pTrail)
      {
         MovingMapFeed *pMovingMapFeed = pTrail->GetMovingMapFeed();
         if (pMovingMapFeed->SaveProperties(dlg.GetPathName()) != SUCCESS)
         {
            AfxMessageBox("Unable to save moving map settings.  Check error log for details");
         }
      }
   }
}

void MovingMapCommandMessageHandler::OnUpdateMovingMapSaveSettings(CCmdUI* pCmdUI)
{
   C_gps_trail *pTrail = static_cast<C_gps_trail*>
      (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));

   if (pTrail)
   {
      MovingMapFeed *pMovingMapFeed = pTrail->GetMovingMapFeed();
      pCmdUI->Enable(pMovingMapFeed->SupportsProperties());
   }
}

void MovingMapCommandMessageHandler::OnMovingMapLoadSettings()
{
   CString strInitialDir = PRM_get_registry_string("Main", "USER_DATA", "") + "\\MovingMapSettings\\";

   CFvwUtil *futil = CFvwUtil::get_instance();
   futil->ensure_directory(strInitialDir);

   static char szFilter[] = "Moving Map Settings File (*.mms)|*.mms||";
   CLoadMovingMapSettingsDialog dlg(TRUE, "mms", strInitialDir + "*.mms", OFN_HIDEREADONLY | 
      OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, szFilter);
   dlg.m_ofn.lpstrTitle = "Load Moving Map Settings";
   
   if (dlg.DoModal() == IDOK)
   {
      const BOOL bAutoConnect = dlg.GetAutoConnect();

      C_gps_trail *pTrail = static_cast<C_gps_trail*>
         (OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_MovingMapTrail));

      long bHasBeenSaved = 0;
      pTrail->get_m_bHasBeenSaved(&bHasBeenSaved);

      // If the trail has been modified or has already been saved, then we do
      // not want to overwrite the current moving map settings.  In this case,
      // we want to create a new overlay
      if (pTrail && (bHasBeenSaved || pTrail->is_modified()) )
         pTrail = NULL;

      POSITION position = dlg.GetStartPosition();
      while (position)
      {
         // Create a new overlay for each additional moving map settings file loaded
         if (pTrail == NULL)
         {
            C_overlay *pOverlay;
            OVL_get_overlay_manager()->create(FVWID_Overlay_MovingMapTrail, &pOverlay);
            pTrail = static_cast<C_gps_trail*>(pOverlay);
         }

         CString strPathName = dlg.GetNextPathName(position);
         if (pTrail != NULL)
         {
            if (pTrail->GetMovingMapFeed()->LoadProperties(strPathName, bAutoConnect) != SUCCESS)
            {
               CString msg;
               msg.Format("Failed loading moving map properties [%s].  See error log for details",
                  strPathName);
               AfxMessageBox(msg);
            }

            // extract filename without the extension
            int pos1 = strPathName.ReverseFind('\\');
            int pos2 = strPathName.Find('.', pos1);
            CString strFilename = strPathName.Mid(pos1 + 1, pos2 - (pos1 + 1));

            pTrail->SetAutoSaveName(strFilename);
         }

         pTrail = NULL;
      }
   }
}

void MovingMapCommandMessageHandler::OnUpdateMovingMapLoadSettings(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}


void MovingMapCommandMessageHandler::OnGPSCoastTrack()
{
   C_gps_trail::open_gps_tools(1);
}

void MovingMapCommandMessageHandler::OnUpdateGPSCoastTrack(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck( C_gps_trail::is_gps_tools_opened(1) );
}

void MovingMapCommandMessageHandler::OnRangeBearing()
{
   C_gps_trail::open_gps_tools(0);
}

void MovingMapCommandMessageHandler::OnUpdateRangeBearing(CCmdUI *pCMdUI)
{
   pCMdUI->SetCheck( C_gps_trail::is_gps_tools_opened(0) );
}

void MovingMapCommandMessageHandler::OnCdiCenterCourse()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiCenterCourse();
}

void MovingMapCommandMessageHandler::OnUpdateCdiCenterCourse(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiCenterCourse( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiCenterShip()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiCenterShip();
}

void MovingMapCommandMessageHandler::OnUpdateCdiCenterShip(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiCenterShip( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiUnitsFeet()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiUnitsFeet();
}

void MovingMapCommandMessageHandler::OnUpdateCdiUnitsFeet(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiUnitsFeet( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiUnitsKilometers()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiUnitsKilometers();
}

void MovingMapCommandMessageHandler::OnUpdateCdiUnitsKilometers(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiUnitsKilometers( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiUnitsMeters()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiUnitsMeters();
}

void MovingMapCommandMessageHandler::OnUpdateCdiUnitsMeters(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiUnitsMeters( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiUnitsNauticalMiles()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiUnitsNauticalMiles();
}

void MovingMapCommandMessageHandler::OnUpdateCdiUnitsNauticalMiles(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiUnitsNauticalMiles( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiUnitsStatuteMiles()
{
   // TODO: Add your command handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiUnitsStatuteMiles();
}

void MovingMapCommandMessageHandler::OnUpdateCdiUnitsStatuteMiles(CCmdUI* pCmdUI)
{
   // TODO: Add your command update UI handler code here
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiUnitsStatuteMiles( pCmdUI );
}

void MovingMapCommandMessageHandler::OnCdiUnitsYards()
{
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnCdiUnitsYards();
}

void MovingMapCommandMessageHandler::OnUpdateCdiUnitsYards(CCmdUI* pCmdUI)
{
   if (C_gps_trail::m_cdi)
      C_gps_trail::m_cdi->OnUpdateCdiUnitsYards( pCmdUI );
}

void MovingMapCommandMessageHandler::OnGPSClock()
{
   CMainFrame::GetPlaybackDialog().set_draw_time_on_map(
      !CMainFrame::GetPlaybackDialog().get_draw_time_on_map());
   OVL_get_overlay_manager()->invalidate_all();
}

void MovingMapCommandMessageHandler::OnUpdateGPSClock(CCmdUI *pCmdUI)
{
   pCmdUI->SetCheck(CMainFrame::GetPlaybackDialog().get_draw_time_on_map());
}