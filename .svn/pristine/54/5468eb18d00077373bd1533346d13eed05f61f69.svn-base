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



// MouseClickResource.h: interface for the CMouseClickResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSECLICKRESOURCE_H__736765A4_3C5F_11D2_8F30_00104B242B5F__INCLUDED_)
#define AFX_MOUSECLICKRESOURCE_H__736765A4_3C5F_11D2_8F30_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "geo_tool_d.h"
#include "ovl_d.h"

// foward declarations
class SnapToInfo;
class GeoBoundsDragger;
class GeoCircleDragger;
class MapProj;
class ViewMapProj;

interface IMouseClickResourceCallback
{
   virtual void OnMouseClick(int click_id, double lat, double lon) = 0;
   virtual void OnMouseClickCanceled(int click_id) = 0;

   virtual void OnSnapToInfo(int click_id, SnapToInfo *snap_to, int point_type, CString key_text) = 0;
   virtual void OnSnapToInfoCanceled(int click_id) = 0;

   virtual void OnGeoRectBounds(int click_id, double nw_lat, double nw_lon,
      double se_lat, double se_lon) = 0;
   virtual void OnGeoRectBoundsCanceled(int click_id) = 0;

   virtual void OnGeoCircleBounds(int click_id, double lat, double lon, double radius) = 0;
   virtual void OnGeoCircleBoundsCanceled(int click_id) = 0;

   virtual ~IMouseClickResourceCallback() { }
};

// implementation of the IMouseClickResourceCallback interface that
// uses the WM_COPY windows message to pass back information to a 
// client
class CMouseClickWindowsMessageCallback : public IMouseClickResourceCallback
{
   HWND m_owner_hWnd;

public:
   CMouseClickWindowsMessageCallback(HWND hWnd);

   void OnMouseClick(int click_id, double lat, double lon);
   void OnMouseClickCanceled(int click_id);

   void OnSnapToInfo(int click_id, SnapToInfo *snap_to, int point_type, CString key_text);
   void OnSnapToInfoCanceled(int click_id);

   void OnGeoRectBounds(int click_id, double nw_lat, double nw_lon,
      double se_lat, double se_lon);
   void OnGeoRectBoundsCanceled(int click_id);

   void OnGeoCircleBounds(int click_id, double lat, double lon, double radius);
   void OnGeoCircleBoundsCanceled(int click_id);
};

// implementation of the IMouseClickResourceCallback interface that
// uses a dispatch interface to pass back information to a client
class CMouseClickDispatchCallback : public IMouseClickResourceCallback
{
   IDispatchPtr m_pdisp;

public:
   CMouseClickDispatchCallback(IDispatch *disp);

   void OnMouseClick(int click_id, double lat, double lon);
   void OnMouseClickCanceled(int click_id);

   void OnSnapToInfo(int click_id, SnapToInfo *snap_to, int point_type, CString key_text);
   void OnSnapToInfoCanceled(int click_id);

   void OnGeoRectBounds(int click_id, double nw_lat, double nw_lon,
      double se_lat, double se_lon);
   void OnGeoRectBoundsCanceled(int click_id);

   void OnGeoCircleBounds(int click_id, double lat, double lon, double radius);
   void OnGeoCircleBoundsCanceled(int click_id);
};

enum mouse_click_t { SINGLE_CLICK, DRAG_GEORECT, DRAG_GEOCIRCLE };

// This class provides a way to receive various mouse click events
// outside of any overlay.  For example, the IMap automation interface
// uses this class to implement its GetMouseClick method.  This class,
// while active, will consume mouse click events.  In other words, no
// overlay will receive these events.
//
// To initiate, call ReceiveMouseClickEvents with a class implementing
// the IMouseClickResourceCallback interface.  The callback class will
// receive either the click event callback or the corresponding cancel event.
//
class CMouseClickResource  
{
   IMouseClickResourceCallback* m_pCallback;

   int m_next_click_id;
   bool m_snap_to;
   int m_snap_to_filter;

   mouse_click_t m_type;

   GeoBoundsDragger *m_geoRect_dragger;
   GeoCircleDragger *m_geoCircle_dragger;

   HintText m_HintText;
   
public:
	CMouseClickResource();
	virtual ~CMouseClickResource();

   // note that this class will own the object passed in.  Do not delete 
	// it yourself 
   int ReceiveMouseClickEvents(IMouseClickResourceCallback* pCallback,
			mouse_click_t type);

	// returns TRUE if either the owner window or the dispatch interface is set
	int is_active() { return m_pCallback != NULL; }

   int SendClick(d_geo_t geo);
	int SendClickCancel();
   int SendSnapToInfo(SnapToInfo* snap_to);
	int SendSnapToCancel();
	int SendGeoRect(double nw_lat, double nw_lon, double se_lat, double se_lon);
   int SendGeoRectCancel();
	int SendGeoCircle(double lat, double lon, double radius);
	int SendGeoCircleCancel();

   void set_snap_to(int filter) { m_snap_to = TRUE; m_snap_to_filter = filter; }
   bool get_snap_to() { return m_snap_to; }

   bool is_filtered(int point_src);

   mouse_click_t get_click_type() { return m_type; }

   void setup_geobounds_dragger(MapProj *view, d_geo_t anchor);
   void setup_geocircle_dragger(MapProj *view, d_geo_t anchor);

   // return TRUE if we handle the left mouse button down, otherwise return FALSE.  drag
   // will be set to TRUE if we are going to do a geo-rect or geo-circle drag
   boolean_t on_left_button_down(ViewMapProj *view, CPoint point, boolean_t &drag);

   void on_drag(ViewMapProj *view, CPoint point, UINT flags, 
      HCURSOR* pCursor, HintText **hint);

	// called when ESC key is pressed.  Should return TRUE if a single click
	// operation was canceled, FALSE otherwise
	boolean_t on_cancel();

	// cancels a click request programmatically
	int cancel_click_request(int click_id);

   void on_cancel_drag(ViewMapProj *view);
	
   // return TRUE if we performed a drop operation, FALSE otherwise
   boolean_t on_drop(ViewMapProj *view, CPoint point, UINT flags);
};

#endif // !defined(AFX_MOUSECLICKRESOURCE_H__736765A4_3C5F_11D2_8F30_00104B242B5F__INCLUDED_)
