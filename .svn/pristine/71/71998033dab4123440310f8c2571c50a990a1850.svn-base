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



// MouseClickResource.cpp: implementation of the CMouseClickResource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MouseClickResource.h"
#include "..\..\Common\map_server_client.h"   // contains message types for the WM_COPY message
#include "mem.h"           // MEM_malloc, MEM_free
#include "ovlelem.h"
#include "mapx.h"
#include "CCallback.h"
#include "err.h"
#include "SnapTo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMouseClickResource::CMouseClickResource()
{
   m_pCallback = NULL;
   m_next_click_id = 0;
   m_snap_to = FALSE;

   m_geoRect_dragger = new GeoBoundsDragger;
   m_geoCircle_dragger = new GeoCircleDragger;
}

CMouseClickResource::~CMouseClickResource()
{
	delete m_geoRect_dragger;
	delete m_geoCircle_dragger;
}

void CMouseClickResource::setup_geobounds_dragger(MapProj *view, d_geo_t anchor)
{
   // make sure the east bounds does not initially equal the west bounds.  This is because
   // in GeoBounds::prepare_for_redraw, we don't want to be in the condition in which
   // the geo width is less than or equal to zero since this will cause 360 to be added
   // to the geo width which causes the dragger to wrap around the world
   d_geo_t anchor2 = anchor;
   anchor2.lon -= 1.0e-07;

   m_geoRect_dragger->set_bounds_no_error_check(anchor2, anchor);
   m_geoRect_dragger->set_modified_edge(UTL_IDC_SIZE_SE);
   m_geoRect_dragger->prepare_for_redraw(view);
   m_geoRect_dragger->set_previous(anchor);
}

void CMouseClickResource::setup_geocircle_dragger(MapProj *view, d_geo_t anchor)
{
   m_geoCircle_dragger->define(anchor.lat, anchor.lon, 1.0e-07, 1.0e-07, 0);
   m_geoCircle_dragger->prepare_for_redraw(view);
   m_geoCircle_dragger->set_first_time(TRUE);
}

// return TRUE if we handle the left mouse button down, otherwise return FALSE.  drag
// will be set to TRUE if we are going to do a geo-rect or geo-circle drag
boolean_t CMouseClickResource::on_left_button_down(ViewMapProj *view, CPoint point, boolean_t &drag)
{
   if (m_pCallback == NULL)
      return FALSE;

   //
   // convert mouse point to geographic location
   //
   d_geo_t geo;
   view->surface_to_geo(point.x, point.y, &geo.lat, &geo.lon);
   
   //
   // Note: it is possible due to race conditions that after the 
   // click someone else requested the mouse click, and will be getting 
   // this click that was really not intended for them because it occurred 
   // before they asked for it
   //
   if (m_type == SINGLE_CLICK)
   {
      // if this is a snap-to operation
      if (get_snap_to())
      {
			boolean_t ret = TRUE;
         SnapToInfo snap_to;
         
         // if we can snap to the point
         if (OVL_get_overlay_manager()->test_snap_to(view, point))
            ret = OVL_get_overlay_manager()->do_snap_to(view, point, &snap_to);
         
         // otherwise, if no snap-to information is available, we will send the 
         // client just the latitude,longitude
         else
         {
            snap_to.m_lat = geo.lat;
            snap_to.m_lon = geo.lon;
         }
         if (TRUE == ret)
				SendSnapToInfo(&snap_to);
			else
				SendSnapToCancel();
      }
      // otherwise, this is not a snap-to operation so just send the 
      // location of the click
      else
         SendClick(geo);
   }
   // Geo-rect type drag
   else if (m_type == DRAG_GEORECT)
   {
      drag = TRUE;
      setup_geobounds_dragger(view, geo);
   }
   // Geo-circle type drag
   else if (m_type == DRAG_GEOCIRCLE)
   {
      drag = TRUE;
      setup_geocircle_dragger(view, geo);
   }
   else
      return FALSE;
   
   return TRUE;
}

void CMouseClickResource::on_drag(ViewMapProj *view, CPoint point, UINT flags, 
      HCURSOR* pCursor, HintText **hint)
{
   // if the cursor is outside of the view, then use the no drop cursor
   CRect client(0, 0, view->get_surface_width(), view->get_surface_height());
   if (!client.PtInRect(point))
   {
      *pCursor = AfxGetApp()->LoadCursor(IDC_NODROP);
      return;
   }

   // let the appropriate dragger do its thing
   if (m_type == DRAG_GEORECT)
   {
      CClientDC dc(view->get_CView());
      m_geoRect_dragger->on_drag(view, &dc, point, flags, pCursor, m_HintText);
      *pCursor = AfxGetApp()->LoadCursor(IDC_RECTANGLE_CURSOR);
   }
   else if (m_type == DRAG_GEOCIRCLE)
   {
      m_geoCircle_dragger->on_drag(view, point, flags, pCursor, m_HintText);
      *pCursor = AfxGetApp()->LoadCursor(IDC_OVAL_CURSOR);
   }

   *hint = &m_HintText;
}

// cancels a click request programmatically
int CMouseClickResource::cancel_click_request(int click_id)
{
	// if click ids don't match, then another click request was received
	// after
	if (click_id != m_next_click_id)
		return FAILURE;

	return on_cancel() ? SUCCESS : FAILURE;
}

// called when ESC key is pressed.  Should return TRUE if a single click
// operation was canceled, FALSE otherwise
boolean_t CMouseClickResource::on_cancel()
{
	if (!is_active())
      return FALSE;

	if (m_type == SINGLE_CLICK)
	{
		if (get_snap_to())
			SendSnapToCancel();
		else
			SendClickCancel();
	}
	else if (m_type == DRAG_GEORECT)
		SendGeoRectCancel();
	else if (m_type == DRAG_GEOCIRCLE)
		SendGeoCircleCancel();
	else 
		return FALSE;

	return TRUE;
}

void CMouseClickResource::on_cancel_drag(ViewMapProj *view)
{
   if (is_active() && (m_type == DRAG_GEORECT || m_type == DRAG_GEOCIRCLE))
   {
      if (m_type == DRAG_GEORECT)
		{
         CClientDC dc(view->get_CView());
         m_geoRect_dragger->on_cancel_drag(view, &dc);
			SendGeoRectCancel();
		}
      else if (m_type == DRAG_GEOCIRCLE)
		{
         m_geoCircle_dragger->on_cancel_drag(view);
			SendGeoCircleCancel();
		}
   }
}

int CMouseClickResource::SendGeoRectCancel()
{
   if (m_pCallback != NULL)
   {
      m_pCallback->OnGeoRectBoundsCanceled(m_next_click_id);
      delete m_pCallback;
      m_pCallback = NULL;
      return SUCCESS;
   }

	return FAILURE;
}

int CMouseClickResource::SendGeoCircleCancel()
{
   if (m_pCallback != NULL)
   {
      m_pCallback->OnGeoCircleBoundsCanceled(m_next_click_id);
      delete m_pCallback;
      m_pCallback = NULL;
      return SUCCESS;
   }

	return FAILURE;
}

// return TRUE if we performed a drop operation, FALSE otherwise
boolean_t CMouseClickResource::on_drop(ViewMapProj *view, CPoint point, UINT flags)
{
   CWaitCursor wait;
   
   if (is_active() && (m_type == DRAG_GEORECT || m_type == DRAG_GEOCIRCLE))
   {
      CRect client(0, 0, view->get_surface_width(), view->get_surface_height());
      if (!client.PtInRect(point))
      {
         on_cancel_drag(view);
         return TRUE;
      }
      
      if (m_type == DRAG_GEORECT)
      {
         // finish the dropping of the geo bounds
         CClientDC dc(view->get_CView());
         m_geoRect_dragger->on_drop(view, &dc, point, flags);

			SendGeoRect(m_geoRect_dragger->get_nw().lat,
				m_geoRect_dragger->get_nw().lon,
				m_geoRect_dragger->get_se().lat,
				m_geoRect_dragger->get_se().lon);
      }
      else if (m_type == DRAG_GEOCIRCLE)
      {
         // finish the dropping of the geo circle
         m_geoCircle_dragger->on_drop(view, point, flags);

			SendGeoCircle(m_geoCircle_dragger->get_lat(), m_geoCircle_dragger->get_lon(),
				m_geoCircle_dragger->get_vert());
      }
      
      return TRUE;
   }

   return FALSE;
}

int CMouseClickResource::SendClick(d_geo_t geo)
{
   if (m_pCallback != NULL)
   {
      // we want the m_pCallback to persist if OnMouseClick sets it
      // so we will NULL m_pCallback before OnMouseClick
      IMouseClickResourceCallback* pCallback = m_pCallback;
      m_pCallback = NULL;
      pCallback->OnMouseClick(m_next_click_id, geo.lat, geo.lon);
      delete pCallback;
      return SUCCESS;
   }

	return FAILURE;
}

int CMouseClickResource::SendClickCancel()
{
   if (m_pCallback != NULL)
   {
      m_pCallback->OnMouseClickCanceled(m_next_click_id);
      delete m_pCallback;
      m_pCallback = NULL;
      return SUCCESS;
   }

	return FAILURE;
}

int CMouseClickResource::SendSnapToInfo(SnapToInfo *snap_to)
{
   int point_type;
   CString key_text("");

   if (snap_to == NULL)
      return FAILURE;

   // get the point type and key text for the point that was snapped to
   // if possible
   if (is_filtered(snap_to->m_pnt_src_rs))
      point_type = -1;
   else
   {
      point_type = snap_to->m_pnt_src_rs;
      key_text = snap_to->m_strKey;
   }

	if (m_pCallback != NULL)
   {
      // we want the m_pCallback to persist if OnMouseClick sets it
      // so we will NULL m_pCallback before OnMouseClick
      IMouseClickResourceCallback* pCallback = m_pCallback;
      m_pCallback = NULL;
      pCallback->OnSnapToInfo(m_next_click_id, snap_to, point_type, key_text);
      delete pCallback;
      m_snap_to = FALSE;            // reset snap to flag

      return SUCCESS;
   }

	return FAILURE;
}

int CMouseClickResource::SendSnapToCancel()
{
   if (m_pCallback != NULL)
   {
      m_pCallback->OnSnapToInfoCanceled(m_next_click_id);
      delete m_pCallback;
      m_pCallback = NULL;
      m_snap_to = FALSE;            // reset snap to flag

      return SUCCESS;
   }

	return FAILURE;
}

int CMouseClickResource::ReceiveMouseClickEvents(
		IMouseClickResourceCallback* pCallback, mouse_click_t type)
{
   if (pCallback != NULL)
   {
      on_cancel();
      m_next_click_id++;
   }

   m_pCallback = pCallback;
	m_type = type;
   return m_next_click_id;
}

bool CMouseClickResource::is_filtered(int point_src)
{
   int type;

   switch (point_src) 
   {
   case SnapToInfo::USER_SRC: type = 0x1; break;
   case SnapToInfo::WAYPOINT_SRC: type = 0x2; break;
   case SnapToInfo::LOCAL_SRC: type = 0x4; break;
   case SnapToInfo::GPS_SRC: type = 0x8; break;          
   case SnapToInfo::AIR_REFUEL: type = 0x10; break;
   case SnapToInfo::TRAIN_ROUTE: type = 0x20; break;
   case SnapToInfo::AIRPORT: type = 0x40; break;
   case SnapToInfo::RUNWAY: type = 0x80; break;
   case SnapToInfo::HELIPORT: type = 0x100; break;
   case SnapToInfo::HELIPAD: type = 0x200; break;
   case SnapToInfo::NAVAID: type = 0x400; break;
   case SnapToInfo::AIRSPACE_BOUNDARY: type = 0x800; break;
   case SnapToInfo::SUAS: type = 0x1000; break;
   case SnapToInfo::AIMPOINT: type = 0x2000; break;
   case SnapToInfo::DROPZONE: type = 0x4000; break;
   case SnapToInfo::ATS_ROUTE: type = 0x8000; break;
   }

   return !(type & m_snap_to_filter);
}

int CMouseClickResource::SendGeoRect(double nw_lat, double nw_lon,
												 double se_lat, double se_lon)
{
   if (m_pCallback != NULL)
   {
      // we want the m_pCallback to persist if OnGeoRectBounds sets it
      // so we will NULL m_pCallback before OnGeoRectBounds
      IMouseClickResourceCallback* pCallback = m_pCallback;
      m_pCallback = NULL;
      pCallback->OnGeoRectBounds(m_next_click_id, nw_lat,nw_lon,se_lat,se_lon);
      delete pCallback;
      return SUCCESS;
   }

	return FAILURE;
}

int CMouseClickResource::SendGeoCircle(double lat, double lon, double radius)
{
   if (m_pCallback != NULL)
   {
      // we want the m_pCallback to persist if OnGeoCircle sets it
      // so we will NULL m_pCallback before OnGeoCircle
      IMouseClickResourceCallback* pCallback = m_pCallback;
      m_pCallback = NULL;
      pCallback->OnGeoCircleBounds(m_next_click_id, lat, lon, radius);
      delete pCallback;
      return SUCCESS;
   }

	return FAILURE;
}

// CMouseClickWindowsMessageCallback
//

CMouseClickWindowsMessageCallback::CMouseClickWindowsMessageCallback(HWND hWnd) :
m_owner_hWnd(hWnd)
{
}

void CMouseClickWindowsMessageCallback::OnMouseClick(int click_id, double lat, double lon)
{
   const int DATA_LEN = 11+2+4+8+8;
   unsigned char data[DATA_LEN];
   
   //create data block
   strncpy_s((char*) &data[0], DATA_LEN, "FALCONVIEW", 11);  
   ((short&) data[11]) = FV_GET_MOUSECLICK_MSG;
   ((long&) data[11+2]) = click_id;
   ((double&) data[11+2+4]) = lat;
   ((double&) data[11+2+4+8]) = lon;

   COPYDATASTRUCT cds;
   cds.dwData = 0;
   cds.cbData = sizeof(data);
   cds.lpData = (void*) &data;

   //Note: sending of the message and setting the owner to NULL
   // should probably be atomic

   CWaitCursor wait;

   //send message
   DWORD result;
   BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
      (LPARAM)&cds, SMTO_BLOCK, 5000, &result);  //5 seconds
}

void CMouseClickWindowsMessageCallback::OnMouseClickCanceled(int click_id)
{
   const int DATA_LEN = 17;
   unsigned char data[DATA_LEN];
   
   //create data block
   strncpy_s((char*) &data[0], DATA_LEN, "FALCONVIEW", 11);  
   ((short&) data[11]) = FV_GET_MOUSECLICK_CANCELED;
   ((long&) data[11+2]) = click_id;

   COPYDATASTRUCT cds;
   cds.dwData = 0;
   cds.cbData = 17;
   cds.lpData = (void*) &data;

   //Note: sending of the message and setting the owner to NULL
   // should probably be atomic

   CWaitCursor wait;

   //send message
   DWORD result;
   BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
      (LPARAM)&cds, SMTO_BLOCK, 5000, &result);  //5 seconds
}

void CMouseClickWindowsMessageCallback::OnSnapToInfo(int click_id, SnapToInfo *snap_to, int point_type, CString key_text)
{
   int data_size = 11               // header
                      + sizeof(short)  // message type
                      + sizeof(long)   // click id
                      + sizeof(double) // latitude
                      + sizeof(double) // longitude
                      + sizeof(int)    // point type
                      + sizeof(int)    // length of key text
                      + key_text.GetLength();

   unsigned char *data = (unsigned char *)MEM_malloc(data_size);

   // create data block
   strncpy_s((char *) data, data_size, "FALCONVIEW", 11);  
   ((short&) data[11]) = FV_SNAP_TO_INFO_MSG;
   ((long&) data[11+2]) = click_id;
   ((double&) data[11+2+4]) = snap_to->m_lat;
   ((double&) data[11+2+4+8]) = snap_to->m_lon;
   ((int&) data[11+2+4+8+8]) = point_type;
   ((int&) data[11+2+4+8+8+4]) = key_text.GetLength();
   strncpy_s((char*) &data[11+2+4+8+8+4+4], data_size - (11+2+4+8+8+4+4), key_text, key_text.GetLength());

   COPYDATASTRUCT cds;
   cds.dwData = 0;
   cds.cbData = data_size;
   cds.lpData = (void*) data;

   CWaitCursor wait;

   // send message
   DWORD result;
   BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
      (LPARAM)&cds, SMTO_BLOCK, 5000, &result);  // 5 seconds

   MEM_free(data);
}

void CMouseClickWindowsMessageCallback::OnSnapToInfoCanceled(int click_id)
{
   const int DATA_LEN = 17;
   unsigned char data[DATA_LEN];

   // create data block
   strncpy_s((char *) data, DATA_LEN, "FALCONVIEW", 11);  
   ((short&) data[11]) = FV_SNAP_TO_INFO_CANCELED;
   ((long&) data[11+2]) = click_id;

   COPYDATASTRUCT cds;
   cds.dwData = 0;
   cds.cbData = 17;
   cds.lpData = (void*) data;

   CWaitCursor wait;

   // send message
   DWORD result;
   BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, (WPARAM)(HWND)AfxGetMainWnd()->m_hWnd,
      (LPARAM)&cds, SMTO_BLOCK, 5000, &result);  // 5 seconds
}

void CMouseClickWindowsMessageCallback::OnGeoRectBounds(int click_id, double nw_lat, double nw_lon,
      double se_lat, double se_lon)
{
   COPYDATASTRUCT cds;
   const int DATA_LEN = 11+2+4+8+8+8+8;
	unsigned char data[DATA_LEN];
	
	// create data block with the bounds of the georect dragger
	strncpy_s((char*) &data[0], DATA_LEN, "FALCONVIEW", 11);
	((short&) data[11]) = FV_RUBBERBAND_GEORECT_MSG;
	((long&) data[11+2]) = click_id;
	((double&) data[11+2+4]) = nw_lat;
	((double&) data[11+2+4+8]) = nw_lon;
	((double&) data[11+2+4+8+8]) = se_lat;
	((double&) data[11+2+4+8+8+8]) = se_lon;
	
	cds.dwData = 0;
	cds.cbData = sizeof(data);
	cds.lpData = (void *) &data;
	
	// send the message to the client application's window
	DWORD result;
	BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, 
		(WPARAM)(HWND)AfxGetMainWnd()->m_hWnd, (LPARAM)&cds, SMTO_BLOCK, 
		5000, &result);
}

void CMouseClickWindowsMessageCallback::OnGeoRectBoundsCanceled(int click_id)
{
   COPYDATASTRUCT cds;
   const int DATA_LEN = 17;
	unsigned char data[DATA_LEN];
	
	// create data block with the bounds of the georect dragger
	strncpy_s((char*) &data[0], DATA_LEN, "FALCONVIEW", 11);
	((short&) data[11]) = FV_RUBBERBAND_GEORECT_CANCELED;
	((long&) data[11+2]) = click_id;
	
	cds.dwData = 0;
	cds.cbData = sizeof(data);
	cds.lpData = (void *) &data;
	
	// send the message to the client application's window
	DWORD result;
	BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, 
		(WPARAM)(HWND)AfxGetMainWnd()->m_hWnd, (LPARAM)&cds, SMTO_BLOCK, 
		5000, &result);
}

void CMouseClickWindowsMessageCallback::OnGeoCircleBounds(int click_id, double lat, double lon, double radius)
{
   COPYDATASTRUCT cds;
   const int DATA_LEN = 11+2+4+8+8+8;
	unsigned char data[DATA_LEN];
	
	// create data block with the bounds of the geocircle dragger
	strncpy_s((char*) &data[0], DATA_LEN, "FALCONVIEW", 11);
	((short&) data[11]) = FV_RUBBERBAND_GEOCIRCLE_MSG;
	((long&) data[11+2]) = click_id;
	((double&) data[11+2+4]) = lat;
	((double&) data[11+2+4+8]) = lon;
	((double&) data[11+2+4+8+8]) = radius;
	
	cds.dwData = 0;
	cds.cbData = sizeof(data);
	cds.lpData = (void *) &data;
	
	// send the message to the client application's window
	DWORD result;
	BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, 
		(WPARAM)(HWND)AfxGetMainWnd()->m_hWnd, (LPARAM)&cds, SMTO_BLOCK, 
		5000, &result);
}

void CMouseClickWindowsMessageCallback::OnGeoCircleBoundsCanceled(int click_id)
{
   COPYDATASTRUCT cds;
   const int DATA_LEN = 17;
	unsigned char data[DATA_LEN];
	
	// create data block with the bounds of the geocircle dragger
	strncpy_s((char*) &data[0], DATA_LEN, "FALCONVIEW", 11);
	((short&) data[11]) = FV_RUBBERBAND_GEOCIRCLE_CANCELED;
	((long&) data[11+2]) = click_id;
	
	cds.dwData = 0;
	cds.cbData = sizeof(data);
	cds.lpData = (void *) &data;
	
	// send the message to the client application's window
	DWORD result;
	BOOL sms = SendMessageTimeout(m_owner_hWnd, WM_COPYDATA, 
		(WPARAM)(HWND)AfxGetMainWnd()->m_hWnd, (LPARAM)&cds, SMTO_BLOCK, 
		5000, &result);
}

// CMouseClickDispatchCallback
//

CMouseClickDispatchCallback::CMouseClickDispatchCallback(IDispatch *disp)
{
   m_pdisp = disp;
}

void CMouseClickDispatchCallback::OnMouseClick(int click_id, double lat, double lon)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnMouseClick( click_id, lat, lon);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnMouseClick(click_id, lat, lon);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnMouseClick failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnMouseClick failed", *e);
      e->Delete();
   }
}
 
void CMouseClickDispatchCallback::OnMouseClickCanceled(int click_id)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnMouseClickCanceled(click_id);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnMouseClickCanceled(click_id);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnMouseClickCanceled failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnMouseClickCanceled failed", *e);
      e->Delete();
   }
}

void CMouseClickDispatchCallback::OnSnapToInfo(int click_id, SnapToInfo *snap_to, int point_type, CString key_text)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnSnapToInfo( click_id,snap_to->m_lat,snap_to->m_lon,point_type, _bstr_t(key_text));
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnSnapToInfo(click_id, snap_to->m_lat, snap_to->m_lon, point_type, key_text);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnSnapToInfo failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnSnapToInfo failed", *e);
      e->Delete();
   }
}

void CMouseClickDispatchCallback::OnSnapToInfoCanceled(int click_id)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnSnapToInfoCanceled( click_id);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnSnapToInfoCanceled(click_id);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnSnapToInfoCanceled failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnSnapToInfoCanceled failed", *e);
      e->Delete();
   }
}

void CMouseClickDispatchCallback::OnGeoRectBounds(int click_id, double nw_lat, double nw_lon,
      double se_lat, double se_lon)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnGeoRectBounds( click_id,nw_lat,nw_lon,se_lat,se_lon);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnGeoRectBounds(click_id,nw_lat,nw_lon,se_lat,se_lon);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnGeoRectBounds failed - %s", (char *)e.Description());
      ERR_report(msg);
      return;
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnGeoRectBounds failed", *e);
      e->Delete();
   }
}

void CMouseClickDispatchCallback::OnGeoRectBoundsCanceled(int click_id)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnGeoRectBoundsCanceled(click_id);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnGeoRectBoundsCanceled(click_id);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnGeoRectBoundsCanceled failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnGeoRectBoundsCanceled failed", *e);
      e->Delete();
   }
}

void CMouseClickDispatchCallback::OnGeoCircleBounds(int click_id, double lat, double lon, double radius)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnGeoCircleBounds( click_id,lat,lon,radius);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnGeoCircleBounds(click_id,lat,lon,radius);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnGeoCircleBounds failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnGeoCircleBounds failed", *e);
      e->Delete();
   }
}

void CMouseClickDispatchCallback::OnGeoCircleBoundsCanceled(int click_id)
{
   try
   {
      fvw::ICallbackPtr callback = m_pdisp;
      if (callback != NULL)
      {
         callback->OnGeoCircleBoundsCanceled( click_id);
         return;
      }

      CCallback callbackImpl(m_pdisp);
      callbackImpl.OnGeoCircleBoundsCanceled(click_id);
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("OnGeoCircleBoundsCanceled failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
   catch(COleException *e)
   {
      ERR_report_exception("OnGeoCircleBounds failed", *e);
      e->Delete();
   }
}
