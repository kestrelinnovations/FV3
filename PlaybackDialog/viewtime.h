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

#if !defined(AFX_VIEWTIME_H__4AC15A37_48DF_11D3_AF54_00105A9B4825__INCLUDED_)
#define AFX_VIEWTIME_H__4AC15A37_48DF_11D3_AF54_00105A9B4825__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// viewtime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewTime dialog

#include "FalconView/include/ovlelem.h"
#include "..\resource.h"
#include <afxdtctl.h>

// foward declarations
class ActiveMap;
class GanttWnd;

class CViewTime : public CDialog
{
// Construction
public:
   CViewTime(CWnd* pParent = NULL);   // standard constructor
   ~CViewTime();

private:

   // the font used to draw the current time
   OvlFont m_font;
   boolean_t m_redraw;
   boolean_t m_controls_enabled;
   
   // this flag tells us if the font has been read from
   // the registry yet or not (initially set to TRUE)
   boolean_t m_font_is_invalid;

   CList<IDispatch *, IDispatch *> m_listCallbacks;

// Dialog Data
   //{{AFX_DATA(CViewTime)
   enum { IDD = IDD_VIEWTIME };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Playback_Control.htm";}

   CBitmapButton m_playstopB;
   CComboBox m_rateC;
   CSliderCtrl m_slider;
   CBitmapButton m_stopB;
   CBitmapButton m_resetB;
   CBitmapButton m_playB;
   CBitmapButton m_endsetB;
   CString m_rateS;
   CButton m_display_checkbox;
   CComboBox m_position_combo;
   CButton m_reverse_playback;
   CDateTimeCtrl m_date;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CViewTime)
   public:
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CViewTime)
   afx_msg void OnViewtimeEndset();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnViewtimeReset();
   afx_msg void OnViewtimeMore();
   afx_msg void OnPaint();
   afx_msg void OnKillfocusVIEWTIMETn();
   virtual BOOL OnInitDialog();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnClose();
   afx_msg void OnSelchangeViewtimeRate();
   afx_msg void OnKillfocusViewtimeRate();
   afx_msg void OnDropdownViewtimeRate();
   afx_msg void OnSetfocusVIEWTIMETn();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnToggleState();
   afx_msg void OnUpdateVIEWTIMETn();
   afx_msg void OnChangeVIEWTIMETn();
   afx_msg void OnDetails();
   afx_msg void OnDisplay();
   afx_msg void OnFont();
   afx_msg void OnChangeAnchorPosition();
   afx_msg void OnSyncToClockTime();
   afx_msg void OnDateChange(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   bool m_editLock;
   static HICON iReset;
   static HICON iStop;
   static HICON iPlay;
   static HICON iEndset;

   boolean_t m_initialized;
   boolean_t m_interval_initialized;

   // are details for the dialog being shown (gant chart, etc...)
   boolean_t m_show_details;

   GanttWnd *m_gantt_wnd;

   // are we going to draw the current time on the map
   boolean_t m_draw_time_on_map;

   // the current time rectangle
   CRect m_rect;

   // anchor position of the current time text
   int m_anchor_pos;

   // the height that the window was changed to accomodate the gant chart
   int m_delta_height;
   CRect m_window_rect;

   //storage
   static int m_dom[16];
   COleDateTime m_t0;
   COleDateTime m_t1;
   COleDateTime m_tn; // current time
   double m_seconds; //duration
   bool m_is_set;

   //playback storage
   long m_state;
   UINT m_timerHandle;
   UINT m_rate;
   COleDateTimeSpan m_elapsed;
   COleDateTimeSpan m_duration;
   DWORD m_tickCount;
   UINT m_current_timeout;

   //playback functions
   void SetState( long state );
   long GetState() {return m_state;};
   void UpdateButtonState();
   void UpdateWindowText();
   UINT GetPlaybackRate() {return m_rate;};
   short SecondsToPos( double seconds );
   double PosToSeconds( short pos );

   COleDateTimeSpan GetElapsedTime() { return m_elapsed; }
   void SetElapsedTime(COleDateTimeSpan dts);
   void Play();
   void Stop();

   //functions
   void Travel(COleDateTimeSpan dts);
   void Travel(COleDateTime dt);
   void XForm(int direction);

public:
   int RegisterForNotifications(IDispatch *pPlaybackDialogCallback);
   int UnregisterForNotifications(IDispatch *pPlaybackDialogCallback);

   void Signal();

   void set_draw_time_on_map(boolean_t d);
   boolean_t get_draw_time_on_map() { return m_draw_time_on_map; }

private:

   void signal(COleDateTime dt);
   void SetAbsoluteTime(COleDateTime dt);
   void update_slider_range();
   void set_start_time( COleDateTime dt );
   void set_stop_time( COleDateTime dt );
   void UpdateAbsoluteTimeFrame(COleDateTime dt);
   void InitInterval();
   void InitInterval(COleDateTime start, COleDateTime end);

   // enable/disable the playback controls
   void enable_controls(boolean_t enable);

   // redraw the gant window 
   void refresh_gant_window();

   // update the starting and ending edit boxes
   void update_start_end_times();

public:

   COleDateTime get_starting_time() { return m_t0; }
   COleDateTime get_ending_time() { return m_t1; }

   void SetPlaybackRate( UINT rate );

   int get_playback_rate();
   int get_anchor_pos();

   void Reset();
   void Endset();

   void set_absolute_time(COleDateTime dt);
   void on_new_absolute_time(COleDateTime dt);
   void set_current_time(COleDateTime dt);
   COleDateTime get_current_time() { return m_tn; }
   
   // this is called in OnInitDialog() and should be called in the open/pre-close
   // functions of all time-sensitive overlays
   void update_interval() { update_gant_chart(); SetElapsedTime(m_duration); }
   void update_interval(COleDateTime start, COleDateTime end) 
   { 
      InitInterval(start,end);
      SetElapsedTime(m_duration);
   }

   // return whether or not we are in playback mode
   boolean_t in_playback() { return m_state == 'PLAY'; }

   bool is_playback_reversed() { return m_reverse_playback.m_hWnd && m_reverse_playback.GetCheck(); }

   boolean_t get_controls_enabled() { return m_controls_enabled; }

   // called when an overlay is closed, opened or has changed its color
   void update_gant_chart();

   // draw the time on the screen
   void draw(ActiveMap *map);

   // callback function for CFontDlg's Apply button
   static void on_apply_font(CFontDlg *dlg);

   // toggle the state of the playback (play/stop)
   void toggle_state() { OnToggleState(); }

   // returns TRUE if the time cursor is attached to the future edge
   boolean_t is_future_edge();

   static DATE m_signal_date;

   // return the font object associated with the playback dialog
   OvlFont &get_font();
}; 

class GanttWnd : public CWnd
{
public:
   GanttWnd(CDialog *parent, int num_tsovl);
   ~GanttWnd();

   void refresh(COleDateTime current_time);

   void set_num_tsovl(int num_tsovl) { m_num_tsovl = num_tsovl; }

private:
   int m_scroll_position;
   CDialog *m_parent;
   COleDateTime m_current_time;
   int m_num_tsovl;
protected:
   //{{AFX_MSG(GanttWnd)
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, 
      CScrollBar* pScrollBar);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWTIME_H__4AC15A37_48DF_11D3_AF54_00105A9B4825__INCLUDED_)