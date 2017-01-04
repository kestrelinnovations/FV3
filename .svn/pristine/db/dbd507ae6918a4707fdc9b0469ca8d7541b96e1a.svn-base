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

#if !defined(AFX_ILAYER_H__1E4408DE_7CFF_11D3_80A9_00C04F60B086__INCLUDED_)
#define AFX_ILAYER_H__1E4408DE_7CFF_11D3_80A9_00C04F60B086__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ILayer.h : header file
//

#include "LayerOvl.h"      // class CLayerOvl


/////////////////////////////////////////////////////////////////////////////
// Layer command target

class Layer : public CCmdTarget
{
   DECLARE_DYNCREATE(Layer)
   DECLARE_OLECREATE(Layer)

   Layer();           // protected constructor used by dynamic creation

// Member variables
private:
   // client's window handle
   HWND m_client_window_handle;

   // name of client
   CString m_client_name;

   // has the client registered with the server yet
   boolean_t m_client_registered_with_server;

   // number of clients connected to the map server 
   static int m_num_clients;                
   static void increment_num_clients(void) { m_num_clients++; }                            
   static void decrement_num_clients(void) { m_num_clients--; } 

   // map of icon handles to icon filenames
   CMap <int, int, CString, CString> m_icon_map;

   // map of symbol handles to OvlSymbols
   CMap <int, int, OvlSymbol *, OvlSymbol *> m_symbol_map;

   // the handle to the next icon
   int m_next_icon_handle;

   // the handle to the next symbol
   int m_next_symbol_handle;

   // dispatch pointer to the client's callback object
   IDispatch FAR* m_pdisp;

   // due to the stateful nature of overlay creation, we need to keep the
   // STA COM object's owning thread.
   DWORD m_owning_thread_id;
private:

   // see if a symbol with the given name exists in the symbol map.  If so,
   // return the symbol's handle
   int lookup_symbol_name(CString name);

   // lookup the symbol with the given handle.  Return a pointer to the symbol
   // if found.  Otherwise return NULL.
   OvlSymbol *lookup_symbol(int symbol_handle);

   int CloseStaticOverlay(GUID overlayDescGuid);
   int OpenStaticOverlay(GUID overlayDescGuid);
   int CloseFileOverlay(GUID overlayDescGuid, const char* filespec);
   int OpenFileOverlay(GUID overlayDescGuid, const char* filespec);

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(Layer)
   public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL

   static int get_num_clients(void) { return m_num_clients; }    

// Implementation
protected:
   virtual ~Layer();
   void remove_dispatch_ptr(IDispatch* pdisp);

   // Generated message map functions
   //{{AFX_MSG(Layer)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
   // Generated OLE dispatch map functions
   //{{AFX_DISPATCH(Layer)
   afx_msg long SetFont(long layer_handle, LPCTSTR font_name, long size, long attributes, long fg_color, long bg_color, long bg_type);
   afx_msg long SetLineType(long layer_handle, long line_type);
   afx_msg long SetFillType(long layer_handle, long fill_type);
   afx_msg long DeleteLayer(long layer_handle);
   afx_msg long AddEllipse(long layer_handle, double lat, double lon, double vertical, double horizontal, double rotation);
   afx_msg long AddPolyline(long layer_handle, const VARIANT FAR& lat_lon_array, long num_points);
   afx_msg long AddPolygon(long layer_handle, const VARIANT FAR& lat_lon_array, long num_points);
   afx_msg long DeleteObject(long layer_handle, long object_handle);
   afx_msg long Refresh(long layer_handle);
   afx_msg long SetAnchorType(long layer_handle, long anchor_type);
   afx_msg long AddText(long layer_handle, double lat, double lon, LPCTSTR text, long x_offset, long y_offset);
   afx_msg long AddBitmap(long layer_handle, LPCTSTR filename, double lat, double lon);
   afx_msg long AddIcon(long layer_handle, LPCTSTR filename, double lat, double lon, LPCTSTR icon_text);
   afx_msg long AddIconMulti(long layer_handle, LPCTSTR filename, const VARIANT FAR& lat_lon_array, const VARIANT FAR& icon_text_array, long num_points);
   afx_msg long AddObjectToGroup(long layer_handle, long object_handle, long group_id);
   afx_msg long MoveObjectToTop(long layer_handle, long object_handle);
   afx_msg long MoveObjectToBottom(long layer_handle, long object_handle);
   afx_msg long MoveLayerToBottom(long layer_handle);
   afx_msg long MoveLayerToTop(long layer_handle);
   afx_msg long MoveBitmap(long layer_handle, long object_handle, double lat, double lon);
   afx_msg long GetProducer(BSTR FAR* producer_string);
   afx_msg long GetVersion(short FAR* major, short FAR* minor, short FAR* revision);
   afx_msg long AddLine(long layer_handle, double lat1, double lon1, double lat2, double lon2, long x_offset, long y_offset);
   afx_msg long SetLayerThreshold(long layer_handle, long scale_denominator);
   afx_msg long SetGroupThreshold(long layer_handle, long group_handle, long scale_denominator);
   afx_msg long SetObjectThreshold(long layer_handle, long object_handle, long scale_denominator);
   afx_msg long DeleteGroup(long layer_handle, long group_handle);
   afx_msg long OpenOverlay(long type, LPCTSTR filespec);
   afx_msg long CloseOverlay(long type, LPCTSTR filespec);
   afx_msg long MoveLayerAbove(long layer_handle, long overlay_handle);
   afx_msg long MoveLayerBelow(long layer_handle, long overlay_handle);
   afx_msg long GetOverlayInfo(long overlay_handle, long FAR* type, BSTR FAR* filespec);
   afx_msg long GetOverlayList(VARIANT FAR* handle_lst, long FAR* overlay_count);
   afx_msg long GetActiveOverlay();
   afx_msg long AddDotToSymbol(long symbol_handle, long x, long y);
   afx_msg long AddLineToSymbol(long symbol_handle, long x1, long y1, long x2, long y2);
   afx_msg long AddCircleToSymbol(long symbol_handle, long center_x, long center_y, long radius);
   afx_msg long AddSymbol(long layer_handle, long symbol_handle, double latitude, double longitude, double scale_factor, double rotation);
   afx_msg long MoveSymbol(long layer_handle, long object_handle, double latitude, double longitude, double rotation);
   afx_msg long CreateSymbol(LPCTSTR symbol_name, short FAR* symbol_exists);
   afx_msg long MoveIcon(long layer_handle, long object_handle, double lat, double lon, long rotation);
   afx_msg long AddIconToSet(long layer_handle, long icon_set_handle, long rotation, LPCTSTR filename);
   afx_msg long AddIconEx(long layer_handle, long icon_set_handle, double lat, double lon, long rotation, LPCTSTR icon_text);
   afx_msg long CreateIconSet(long layer_handle);
   afx_msg long ShowMainToolbar(BOOL show);
   afx_msg long ShowEditorToolbar(BOOL show);
   afx_msg long ShowStatusBar(BOOL show);
   afx_msg long SetupToolbarButton(long toolbar_handle, long button_number, LPCTSTR tooltip_text, LPCTSTR status_bar_text);
   afx_msg long RemoveToolbar(long toolbar_handle);
   afx_msg long SetIsTimeSensitive(long layer_handle, BOOL enabled);
   afx_msg long ModifyObject(long layer_handle, long object_handle);
   afx_msg long DeleteAllObjects(long layer_handle);
   afx_msg long ShowLayer(long layer_handle, BOOL show_layer);
   afx_msg long SetPen(long layer_handle, short fg_red, short fg_green, short fg_blue, short bg_red, short bg_green, short bg_blue, BOOL turn_off_background, long line_width, long line_style);
   afx_msg long AddGeoRect(long layer_handle, double nw_lat, double nw_lon, double se_lat, double se_lon);
   afx_msg long AddArc(long layer_handle, double lat, double lon, double radius, double heading_to_start, double turn_arc, BOOL clockwise);
   afx_msg long RegisterWithMapServer(LPCTSTR client_name, long window_handle, const VARIANT FAR& dispatch_pointer);
   afx_msg long CreateLayer(LPCTSTR layer_name);
   afx_msg long CreateLayerEx(LPCTSTR layer_name, LPCTSTR icon_name);
   afx_msg long SetAbsoluteTime(long layer_handle, DATE date_time);
   afx_msg long CreateOverlay(long type);
   afx_msg long CloseOverlayEx(long overlay_handle);
   afx_msg long SetToolbarButtonState(long toolbar_handle, long button_number, short button_down);
   afx_msg long SetObjectData(long layer_handle, long object_handle, long object_data);
   afx_msg long GetObjectData(long layer_handle, long object_handle, long FAR* object_data);
   afx_msg long SetPlaybackRate(long playback_rate);
   afx_msg long StartPlayback();
   afx_msg long StopPlayback();
   afx_msg long PlaybackBegin();
   afx_msg long PlaybackEnd();
   afx_msg long AddEditorButton(long layer_handle, LPCTSTR button_text, LPCTSTR bitmap_filename, LPCTSTR class_ID_string);
   afx_msg long SetModified(long layer_handle, short modified);
   afx_msg long SetDispatch(long layer_handle, const VARIANT FAR& dispatch_pointer);
   afx_msg long AddToolbar(LPCTSTR filename, long num_buttons, LPCTSTR toolbar_name, short initial_position, const VARIANT FAR& separator_lst, long num_separators);
   afx_msg long RemoveEditorButton(long layer_handle);
   afx_msg long AddLineWithEmbeddedText(long layer_handle, double lat1, double lon1, double lat2, double lon2, long x_offset, long y_offset, LPCTSTR embedded_text);
   afx_msg long AddMilStd2525Symbol(long layer_handle, double lat, double lon, LPCTSTR symbol_id);
   afx_msg long SetTextRotation(long layer_handle, double rotation_degrees);
   afx_msg long GetModified(long layer_handle);
   afx_msg long SaveOverlay(long layer_handle);
   afx_msg long AddIconOffset(long layer_handle, LPCTSTR filename, double lat, double lon, LPCTSTR icon_text, long x_offset, long y_offset);
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILAYER_H__1E4408DE_7CFF_11D3_80A9_00C04F60B086__INCLUDED_)
