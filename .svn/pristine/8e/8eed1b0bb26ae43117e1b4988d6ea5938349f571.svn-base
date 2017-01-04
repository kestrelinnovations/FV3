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
// TargetGraphics2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "TargetGraphics2Dlg.h"
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

#include "ovl_mgr.h"
#include "..\getobjpr.h"
#include "..\MouseClickResource.h"


#import "GeodataOverlayServer.tlb" named_guids

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphics2Dlg dialog

CTargetGraphics2Dlg::CTargetGraphics2Dlg(CWnd* pParent /*=NULL*/)
   : CTargetGraphicsDlgBase(pParent)
{

   // create MapTypeSelection object for category and scale controls
   m_selected_list = NULL;
}

CTargetGraphics2Dlg::~CTargetGraphics2Dlg()
{
}

void CTargetGraphics2Dlg::initialize_geobounds_controls()
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
         feature_t target = m_selected_list->GetHead();
         IFeaturePtr feature = target.feature;
         IPointPtr point = feature->Geometry;
         m_geoctrl1.SetGeocx(point->y, point->x);
         m_geoctrl2.SetGeocx(point->y, point->x);
      }
   }
}

int CTargetGraphics2Dlg::selected_list_count()
{
   return m_selected_list->GetCount();
}

void CTargetGraphics2Dlg::get_points(std::vector<d_geo_t>& points)
{
   POSITION position = m_selected_list->GetHeadPosition();
   while (position)
   {
      feature_t item = m_selected_list->GetNext(position);

      d_geo_t point;
      point.lat = get_lat(item); 
      point.lon = get_lon(item);

      points.push_back(point);
   }
}

void CTargetGraphics2Dlg::get_items(std::vector<ITEM>& items)
{
   POSITION position = m_selected_list->GetHeadPosition();
   while (position)
   {
      feature_t item_feature = m_selected_list->GetNext(position);
      IFeaturePtr feature = item_feature.feature;
      IFvDataSourcePtr dataSource = item_feature.dataSource;

      IFvDataSourceProviderPtr pProvider = NULL;
      C_overlay *pOverlay = OVL_get_overlay_manager()->get_first();
      while (pOverlay)
      {
         COverlayCOM* pOverlayCom = dynamic_cast<COverlayCOM*>(pOverlay);
         if (pOverlayCom != NULL)
         {
            pProvider = pOverlayCom->GetFvOverlay();
            if (pProvider != NULL)
            {
               if (pProvider->GetDataSource()->connectString == dataSource->connectString)
                  break;
            }
            pProvider = NULL;
         }

         pOverlay = OVL_get_overlay_manager()->get_next(pOverlay);
      }

      GeodataOverlayServer::IFvTargetGraphicsOverlayPtr tgOverlay = 
         static_cast<GeodataOverlayServer::IFvTargetGraphicsOverlayPtr>(pProvider);
      tgOverlay->SetTargetGraphicPrinting(VARIANT_TRUE);
      tgOverlay->SetTargetIcon("\\icons\\Localpnt\\target.ico");
      tgOverlay->SetTargetGraphicFeature(feature);

      IPointPtr point = feature->Geometry;

      ITEM item;
      item.lat = point->y;
      item.lon = point->x;
      item.id = get_id(item_feature);
      
      _bstr_t bLinkNames("LinkNames");
      _variant_t linkNames = feature->GetFieldByName(bLinkNames);
      //CString itemLinks(linkNames);

      item.links = "";

      item.description = get_description(item_feature);
      item.comment = get_comment(item_feature);

      items.push_back(item);
   }
}

void CTargetGraphics2Dlg::get_point_info(UINT current_page_number, CString& point_name, CString& point_description, 
                    CString& coord_string1, CString& coord_string2,
                    CString& coord_string3, CString& coord_string4,
                    CString& comment)
{
   POSITION pos = m_selected_list->FindIndex((current_page_number - 1) % m_selected_list->GetCount());
   feature_t target = m_selected_list->GetAt(pos);

   point_name = get_id(target);
   point_description = get_description(target);
      
   get_center_coordinate_strings(get_lat(target), get_lon(target), 
      coord_string1, coord_string2, coord_string3, coord_string4);

   comment = get_comment(target) + "\n";
}

class CGeoRectBoundsSelection : public IMouseClickResourceCallback
{
   CList<feature_t, feature_t> m_selectedItems;
   format_t m_initialFormat;

public:
   void Initialize(CList<feature_t, feature_t>* pSelectedItems, format_t initialFormat)
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

      CTargetGraphics2Dlg dlg;

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

void CTargetGraphics2Dlg::enable_clipboard_format()
{
   if (m_selected_list->GetCount() != 1)
      m_format_clipboard.EnableWindow(FALSE);
}

void CTargetGraphics2Dlg::handle_get_position_from_map()
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

CString CTargetGraphics2Dlg::get_id(feature_t f)
{
   IFeaturePtr feature = f.feature;
   _variant_t var = feature->GetFieldByName("ID");
   CString id(var);

   return id;
}

CString CTargetGraphics2Dlg::get_comment(feature_t f)
{
   IFeaturePtr feature = f.feature;
   _variant_t var = feature->GetFieldByName("Comment");
   CString id(var);

   return id;
}

CString CTargetGraphics2Dlg::get_description(feature_t f)
{
   IFeaturePtr feature = f.feature;
   _variant_t var = feature->GetFieldByName("Description");
   CString id(var);

   return id;
}

double CTargetGraphics2Dlg::get_lat(feature_t f)
{
   IFeaturePtr feature = f.feature;
   IPointPtr point = feature->Geometry;
   return point->y;
}

double CTargetGraphics2Dlg::get_lon(feature_t f)
{
   IFeaturePtr feature = f.feature;
   IPointPtr point = feature->Geometry;
   return point->x;
}

