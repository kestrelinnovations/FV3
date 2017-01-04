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



/////////////////////////////////////////////////////////////////////////////
// TargetGraphicsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TargetGraphicsDlg.h"
#include "geotrans.h"

#include "MapTpSel.h"   // MapTypeSelection
#include "..\services.h" // for map_server
#include "..\favorite.h"
#include "..\PrintToolOverlay\PrntPage.h" // for parse_map_type_string
#include "TGSummary.h"
#include "mem.h"
#include "..\overlay\OverlayCOM.h"
#include "..\MissionPackageDlg.h"
#include "wm_user.h"    // for ID_TREE_CHECKED
#include "..\MapEngineCOM.h"
#include "..\ExifImage.h"
#include "..\getobjpr.h" // for fvw_get_frame()
#include "..\grid_map\factory.h"
#include "..\scalebar\factory.h"
#include "..\getobjpr.h"
#include "..\MouseClickResource.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphicsDlg dialog

CTargetGraphicsDlg::CTargetGraphicsDlg(CWnd* pParent /*=NULL*/)
   : CTargetGraphicsDlgBase(pParent)
{
   // create MapTypeSelection object for category and scale controls
   m_selected_list = NULL;
}

CTargetGraphicsDlg::~CTargetGraphicsDlg()
{ 
}

void CTargetGraphicsDlg::initialize_geobounds_controls()
{
   // gray out geobounds controls if there is more than one point being processed
   if (m_selected_list->GetCount() > 1)
      m_output_size_geobounds.EnableWindow(FALSE);
   else
   {
      if (m_geo_bounds_valid)
      {
         m_geoctrl1.SetGeocx(m_nw_lat, m_nw_lon);
         m_geoctrl2.SetGeocx(m_se_lat, m_se_lon);
      }
      else
      {
         target_t target = m_selected_list->GetHead();
         m_geoctrl1.SetGeocx(target.lat, target.lon);
         m_geoctrl2.SetGeocx(target.lat, target.lon);
      }
   }
}

int CTargetGraphicsDlg::selected_list_count()
{
   return m_selected_list->GetCount();
}

void CTargetGraphicsDlg::enable_clipboard_format()
{
   if (m_selected_list->GetCount() != 1)
      m_format_clipboard.EnableWindow(FALSE);
}

void CTargetGraphicsDlg::get_points(std::vector<d_geo_t>& points)
{
   POSITION position = m_selected_list->GetHeadPosition();
   while (position)
   {
      target_t item = m_selected_list->GetNext(position);

      d_geo_t point;
      point.lat = item.lat; 
      point.lon = item.lon;

      points.push_back(point);
   }
}

void CTargetGraphicsDlg::get_items(std::vector<ITEM>& items)
{
   POSITION position = m_selected_list->GetHeadPosition();
   while (position)
   {
      target_t target_item = m_selected_list->GetNext(position);

      ITEM item;
      item.lat = target_item.lat;
      item.lon = target_item.lon;
      item.id = target_item.id;
      item.links = target_item.links;
      item.description = target_item.description;
      item.comment = target_item.comment;

      items.push_back(item);
   }
}

void CTargetGraphicsDlg::get_point_info(UINT current_page_number, CString& point_name, CString& point_description, 
                    CString& coord_string1, CString& coord_string2,
                    CString& coord_string3, CString& coord_string4,
                    CString& comment)
{
   POSITION pos = m_selected_list->FindIndex((current_page_number - 1) % m_selected_list->GetCount());
   target_t target = m_selected_list->GetAt(pos);

   point_name = target.id;
   point_description = target.description;

   get_center_coordinate_strings(target.lat, target.lon, coord_string1, coord_string2,
                    coord_string3, coord_string4);

   comment = target.comment + "\n";
}

class CGeoRectBoundsSelection : public IMouseClickResourceCallback
{
   CList<target_t, target_t> m_selectedItems;
   format_t m_initialFormat;

public:
   void Initialize(CList<target_t, target_t>* pSelectedItems, format_t initialFormat)
   {
      m_selectedItems.AddTail(pSelectedItems);
      m_initialFormat = initialFormat;
   }

   void OnMouseClick(int click_id, double lat, double lon) { }
   void OnMouseClickCanceled(int click_id) { }

   void OnSnapToInfo(int click_id, SnapToInfo *snap_to, int point_type, CString key_text) { }
   void OnSnapToInfoCanceled(int click_id) { }

   void OnGeoRectBounds(int click_id, double nw_lat, double nw_lon,
      double se_lat, double se_lon)
   {
      // restart the target graphics dialog
      //

      CTargetGraphicsDlg dlg;

      // make a local copy of the selected list
      dlg.set_selected_list(&m_selectedItems);
      dlg.set_initial_format(m_initialFormat);
      dlg.set_initial_output_size_type(OUTPUT_SIZE_GEOBOUNDS);
      dlg.set_initial_geo_bounds(nw_lat, nw_lon, se_lat, se_lon);

      dlg.DoModal();
   }

   void OnGeoRectBoundsCanceled(int click_id) { }
   void OnGeoCircleBounds(int click_id, double lat, double lon, double radius) { }
   void OnGeoCircleBoundsCanceled(int click_id) { }
};

void CTargetGraphicsDlg::handle_get_position_from_map()
{
   MapView* pView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   if (pView != NULL)
   {
      CGeoRectBoundsSelection* pGeoRectBoundsSelection = new CGeoRectBoundsSelection();
      pGeoRectBoundsSelection->Initialize(m_selected_list, m_format);

      // the mouse click resource class will own pGeoRectBoundsSelection and delete
      // it when it is through with it
      pView->m_mouse_click_resource->ReceiveMouseClickEvents(pGeoRectBoundsSelection, DRAG_GEORECT);
   }
}
