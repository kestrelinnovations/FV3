// Copyright (c) 1994-2009 Georgia Tech Research Corporation, Atlanta, GA This
// file is part of FalconView(tm).

// FalconView(tm) is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.

// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(tm) is a trademark of Georgia Tech Research Corporation.

// viewtime.cpp : implementation file
//

#include "stdafx.h"
#include "viewtime.h"
#include "..\mainfrm.h"
#include "param.h"
#include "map.h"
#include "..\FontDlg.h"
#include "..\getobjpr.h"
#include "..\StatusBarInfoPane.h"
#include "err.h"
#include "..\CPlaybackDialogCallback.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"


/////////////////////////////////////////////////////////////////////////////
// CViewTime dialog

#define SECONDS_PER_DAY (24*60*60)
#define DAYS_PER_MILLISECOND (1.0 / (1000*SECONDS_PER_DAY))
#define HALF_SECOND (500) //milliseconds
#define GANT_ROW_HEIGHT 25
#define GANT_TEXT_OFFSET_X 6

#define MAX_PLAYBACK_RATE (99)
#define MIN_PLAYBACK_RATE (1) 
#define INVALID_PLAY_BACK_RATE -1
#define DEFAULT_PLAY_BACK_RATE 1
#define DEFAULT_ANCHOR_POS UTIL_ANCHOR_LOWER_LEFT

//static members
HICON CViewTime::iReset;
HICON CViewTime::iStop;
HICON CViewTime::iPlay;
HICON CViewTime::iEndset;
DATE CViewTime::m_signal_date;


void CViewTime::OnClose()
{
   ShowWindow(SW_HIDE);
	CMainFrame::SetPlaybackDialogActive(FALSE);
}

CViewTime::CViewTime(CWnd* pParent /*=NULL*/)
: CDialog(CViewTime::IDD, pParent),
m_gantt_wnd(nullptr)
{
	//{{AFX_DATA_INIT(CViewTime)
	m_rateS = _T("");
	//}}AFX_DATA_INIT

   m_state = 'STOP';
   m_elapsed = 0;
   
	// the playback rate is invalid until it is set from the registry
	m_rate = INVALID_PLAY_BACK_RATE;

	m_font_is_invalid = TRUE;

	// read the anchor position from the registry
	m_anchor_pos = UTIL_ANCHOR_INVALID;

   m_editLock = false;
   m_interval_initialized = FALSE;

	// sync to current time if no overlays are opened
	COleDateTime current_time;
	CTime time = CTime::GetCurrentTime();
	tm t1;
   time.GetGmtTm(&t1);
	current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
		t1.tm_min, t1.tm_sec);

	m_t0 = current_time;
	m_t1 = current_time;
	set_current_time(current_time);
	m_is_set = false;

   InitInterval();
   SetElapsedTime(m_duration);
   m_initialized = TRUE;
	m_draw_time_on_map = FALSE;

	m_controls_enabled = FALSE;
}


CViewTime::~CViewTime()
{
   // store the rate in the registry
	if (m_rate != INVALID_PLAY_BACK_RATE)
      PRM_set_registry_int("ViewTime","Rate", m_rate);

	// store the font properties in the registry
	if (!m_font_is_invalid)
	   m_font.save_in_registry("ViewTime\\Font");

	// store the anchor position to the registry
	if (m_anchor_pos != UTIL_ANCHOR_INVALID)
	   PRM_set_registry_int("ViewTime","AnchorPos", m_anchor_pos);

	// store the reverse flag
	if (m_reverse_playback.m_hWnd)
		PRM_set_registry_int("ViewTime","Reverse", m_reverse_playback.GetCheck());
		

	if (m_gantt_wnd != NULL)
		delete m_gantt_wnd;
}

int CViewTime::RegisterForNotifications(IDispatch *pPlaybackDialogCallback)
{
   m_listCallbacks.AddTail(pPlaybackDialogCallback);
   return SUCCESS;
}

int CViewTime::UnregisterForNotifications(IDispatch *pPlaybackDialogCallback)
{
   POSITION position = m_listCallbacks.Find(pPlaybackDialogCallback);
   if (position)
   {
      m_listCallbacks.RemoveAt(position);
      return SUCCESS;
   }

   return FAILURE;   // unable to find dispatch pointer in list
}
   

void CViewTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewTime)
	DDX_Control(pDX, IDC_VIEWTIME_RATE, m_rateC);
	DDX_Control(pDX, IDC_VIEWTIME_SLIDER, m_slider);
	DDX_CBString(pDX, IDC_VIEWTIME_RATE, m_rateS);
	DDV_MaxChars(pDX, m_rateS, 3);

   DDX_Control(pDX, IDC_VIEWTIME_PLAYSTOP, m_playstopB);
   DDX_Control(pDX, IDC_VIEWTIME_RESET, m_resetB);
	DDX_Control(pDX, IDC_VIEWTIME_ENDSET, m_endsetB);

	DDX_Control(pDX, IDC_DISPLAY, m_display_checkbox);
	DDX_Control(pDX, IDC_POSITION, m_position_combo);
	DDX_Control(pDX, IDC_REVERSE, m_reverse_playback);

	DDX_Control(pDX, IDC_DATETIME, m_date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewTime, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CViewTime)
	ON_BN_CLICKED(IDC_VIEWTIME_ENDSET, OnViewtimeEndset)
	ON_BN_CLICKED(IDC_VIEWTIME_RESET, OnViewtimeReset)
	ON_BN_CLICKED(IDC_VIEWTIME_MORE, OnViewtimeMore)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_VIEWTIME_Tn, OnKillfocusVIEWTIMETn)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_VIEWTIME_RATE, OnSelchangeViewtimeRate)
	ON_CBN_KILLFOCUS(IDC_VIEWTIME_RATE, OnKillfocusViewtimeRate)
	ON_CBN_DROPDOWN(IDC_VIEWTIME_RATE, OnDropdownViewtimeRate)
	ON_EN_SETFOCUS(IDC_VIEWTIME_Tn, OnSetfocusVIEWTIMETn)
	ON_BN_CLICKED(IDC_VIEWTIME_PLAYSTOP, OnToggleState)
	ON_EN_UPDATE(IDC_VIEWTIME_Tn, OnUpdateVIEWTIMETn)
	ON_EN_CHANGE(IDC_VIEWTIME_Tn, OnChangeVIEWTIMETn)
   ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DISPLAY, OnDisplay)
   ON_BN_CLICKED(IDC_DETAIL, OnDetails)
	ON_BN_CLICKED(IDC_VIEWTIME_FONT, OnFont)
	ON_CBN_SELCHANGE(IDC_POSITION, OnChangeAnchorPosition)
	ON_BN_CLICKED(IDC_SYNC, OnSyncToClockTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIME, OnDateChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTime message handlers

void CViewTime::Signal()
{
   signal(m_tn);
}

// tell the time sensitive overlays the current time
void CViewTime::signal(COleDateTime dt)
{
   // set the current view time of all overlays to the given date time
	static const UINT msg_viewtime_changed = RegisterWindowMessage("FVW_ViewTime_Changed");
	m_signal_date = dt.m_dt;

   // don't signal if the view hasn't yet been created
   if (UTL_get_active_non_printing_view() != NULL)
      OVL_get_overlay_manager()->set_current_view_time(m_signal_date);

   // Notify any clients using the IPlaybackDialog interface
   POSITION position = m_listCallbacks.GetHeadPosition();
   while (position)
   {
      IDispatch *pDispatch = m_listCallbacks.GetNext(position);

      try
      {
         fvw::IPlaybackDialogCallbackPtr smpCallback = pDispatch;
         if (smpCallback != NULL)
            smpCallback->OnPlaybackTimeChanged(dt);
         else
         {
                 CPlaybackDialogCallback callback(pDispatch);
                           callback.OnPlaybackTimeChanged(dt);
         }
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnPlaybackTimeChanged failed - %s", (char *)e.Description());
         ERR_report(msg);
      }
      catch(COleException *e)
      {
         ERR_report_ole_exception("OnPlaybackTimeChanged failed", *e);
         e->Delete();
      }
   }

   // invalidate the gant chart if it is opened
   refresh_gant_window();
}

// update the range of slider based on the current duration
void CViewTime::update_slider_range()
{
   if (m_slider.m_hWnd != NULL)
   {
      m_seconds = (m_duration*SECONDS_PER_DAY);
      m_slider.SetRange(0,SecondsToPos(m_seconds));
   }
}

void CViewTime::InitInterval()
{
   // get the earliest and latest time stamp from each of the time 
   // sensitive overlays
   if (OVL_get_overlay_manager()->get_time_span(m_t0, m_t1) == SUCCESS)
	{
      m_interval_initialized = TRUE;
		m_is_set = true;
	}
   else
      m_interval_initialized = FALSE;

	// remember if we were on the leading edge
	const int leading_edge = (m_elapsed == m_duration);

   // duration is the total span of time
   m_duration = m_t1 - m_t0;

	// if we were on the leading edge before, we should be on the leading edge now
	if (leading_edge)
	{
		m_elapsed = m_duration;

		if (m_interval_initialized)
		{
			update_slider_range();
			SetElapsedTime(m_duration);
		}
	}

   update_slider_range();
   
   // if we have zero duration, then grey out the playback buttons
   if (m_duration.GetTotalSeconds() == 0)
      enable_controls(FALSE);
   else 
      enable_controls(TRUE);
   
   update_start_end_times();
}

void CViewTime::InitInterval(COleDateTime t0, COleDateTime t1)
{
   COleDateTime start, stop;

   // if the interval has already been set expand the existing interval
   if (m_interval_initialized)
   {
      start = t0;
      stop = t1;

      if (m_t0 < start)
         start = m_t0;
      if (m_t1 > stop)
         stop = m_t1;
   }

   // try to get the time span for all open overlays
   else if (OVL_get_overlay_manager()->get_time_span(start, stop) == -1)
   {
      // if there are no time sensitive overlays open then
      // we use the given starting and ending times
      start = t0;
      stop = t1;
   }
   else
   {
      // get the earliest and latest time of all time-sensitive overlays,
      // including the span passed in
      if (t0 < start)
         start = t0;
      if (t1 > stop)
         stop = t1;
   }

   m_t0 = start;
   m_t1 = stop;

   // duration is the total span of time
   m_duration = m_t1 - m_t0;
   update_slider_range();

   // if we have zero duration, then grey out the playback buttons
   if (m_duration.GetTotalSeconds() == 0)
      enable_controls(FALSE);
   else 
      enable_controls(TRUE);

   update_start_end_times();

	m_interval_initialized = TRUE;
   m_is_set = true;
}

BOOL CViewTime::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // load the button icons
   iReset = AfxGetApp()->LoadIcon( IDI_PB_RESET );
   iStop  = AfxGetApp()->LoadIcon( IDI_PB_STOP  );
   iPlay  = AfxGetApp()->LoadIcon( IDI_PB_PLAY  );
   iEndset= AfxGetApp()->LoadIcon( IDI_PB_ENDSET);

   CButton *button = (CButton *)GetDlgItem(IDC_DETAIL);

   m_resetB.SetIcon(iReset);
   m_endsetB.SetIcon(iEndset);
   m_playstopB.SetIcon(iPlay);
   
   m_slider.SetPageSize(1);
	
   m_rateC.SetCurSel(0);
   m_rateS.Format("%d", get_playback_rate());
   m_rateC.SelectString(-1,m_rateS);

   SetState('STOP');

   // set the focus to the play/stop button
   CWnd *pWnd = GetDlgItem(IDC_VIEWTIME_PLAYSTOP);
   GotoDlgCtrl(pWnd);

   // find the start and stop times for each overlay
   InitInterval();

   // we only need to set the elapsed time to the entire duration once.  After,
   // that the elapsed time will be set to whatever it was last time the dialog
   // was opened
   if (!m_initialized)
   {
      SetElapsedTime(m_duration);
      m_initialized = TRUE;
   }
   else
      SetElapsedTime(m_elapsed);
   
   m_show_details = FALSE;
   m_delta_height = 0;

   // get the initial size of the window
   GetWindowRect(m_window_rect);

	// set the check box 'Display Time on Map'
	m_display_checkbox.SetCheck(m_draw_time_on_map);

	m_reverse_playback.SetCheck(PRM_get_registry_int("ViewTime", "Reverse", 0));

	// set the anchor position combo box
	switch(get_anchor_pos())
	{
	case UTIL_ANCHOR_LOWER_LEFT: m_position_combo.SetCurSel(0); break;
	case UTIL_ANCHOR_LOWER_CENTER: m_position_combo.SetCurSel(1); break;
	case UTIL_ANCHOR_LOWER_RIGHT: m_position_combo.SetCurSel(2); break;
	case UTIL_ANCHOR_UPPER_LEFT: m_position_combo.SetCurSel(3); break;
	case UTIL_ANCHOR_UPPER_CENTER: m_position_combo.SetCurSel(4); break;
	case UTIL_ANCHOR_UPPER_RIGHT: m_position_combo.SetCurSel(5); break;
	}

	m_date.SetFormat("MMM dd HH:mm:ss yyyy");

   return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewTime::set_draw_time_on_map(boolean_t d)
{
	m_draw_time_on_map = d;
	if (m_display_checkbox.m_hWnd)
		m_display_checkbox.SetCheck(m_draw_time_on_map);
}

void CViewTime::Travel( COleDateTimeSpan dts )
{
   m_tn += dts;
   SetElapsedTime( m_tn - m_t0 );
}

void CViewTime::Travel( COleDateTime dt )
{
   m_tn = dt;
   SetElapsedTime( m_tn - m_t0 );
}

// day of month
int CViewTime::m_dom[16] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

void CViewTime::UpdateButtonState()
{
   switch(GetState())
   {
   case 'PLAY':
      if (m_playstopB.m_hWnd != NULL)
         m_playstopB.SetIcon(iStop);  
      break;
   case 'STOP':
      if (m_playstopB.m_hWnd != NULL)
         m_playstopB.SetIcon(iPlay);
      break;
   };
}

void CViewTime::SetState( long state )
{
   CWnd *pMainWnd = AfxGetMainWnd();

   if (m_state != state)
   {
      m_state = state;
    
      if (state == 'PLAY')
      {
         OVL_get_overlay_manager()->OnPlaybackStarted();
   
         POSITION position = m_listCallbacks.GetHeadPosition();
         while (position)
         {
            IDispatch *pDispatch = m_listCallbacks.GetNext(position);

            try
            {
               fvw::IPlaybackDialogCallbackPtr smpCallback = pDispatch;
               if (smpCallback != NULL)
                  smpCallback->OnPlaybackStarted();
               else
               {
                  CPlaybackDialogCallback callback(pDispatch);
                  callback.OnPlaybackStarted();
               }
            }
            catch(_com_error &e)
            {
               CString msg;
               msg.Format("OnPlaybackStarted failed - %s", (char *)e.Description());
               ERR_report(msg);
            }
            catch(COleException *e)
            {
               ERR_report_ole_exception("OnPlaybackStarted failed", *e);
               e->Delete();
            }
         }
      }
      
      else if (state == 'STOP')
      {
         OVL_get_overlay_manager()->OnPlaybackStopped();
         
         POSITION position = m_listCallbacks.GetHeadPosition();
         while (position)
         {
            IDispatch *pDispatch = m_listCallbacks.GetNext(position);
            fvw::IPlaybackDialogCallbackPtr smpCallback = pDispatch;

            try
            {
               fvw::IPlaybackDialogCallbackPtr smpCallback = pDispatch;
               if (smpCallback != NULL)
                  smpCallback->OnPlaybackStopped();
               else
               {
                  CPlaybackDialogCallback callback(pDispatch);
                  callback.OnPlaybackStopped();
               }
            }
            catch(_com_error &e)
            {
               CString msg;
               msg.Format("OnPlaybackStopped failed - %s", (char *)e.Description());
               ERR_report(msg);
            }
            catch(COleException *e)
            {
               ERR_report_ole_exception("OnPlaybackStopped failed", *e);
               e->Delete();
            }
         }
      }
   }

   UpdateButtonState();
}

void CViewTime::Stop() 
{
   if (GetState()=='STOP')
   {
      SetState('STOP');
      return;
   }

   if (IsWindow(m_hWnd))
      KillTimer(m_timerHandle);

   SetState('STOP');
}

void CViewTime::Reset() 
{
	if (is_playback_reversed())
		Endset();
	else
	{
		Stop();
		OnHScroll(SB_THUMBPOSITION,SecondsToPos(0),NULL);
	}
}

void CViewTime::OnViewtimeReset() 
{
   Stop();
	OnHScroll(SB_THUMBPOSITION,SecondsToPos(0),NULL);
}

void CViewTime::Endset() 
{
   Stop();
   OnHScroll(SB_THUMBPOSITION,SecondsToPos(m_seconds),NULL);
}

void CViewTime::OnViewtimeEndset() 
{
   Endset();	
}

void CViewTime::OnTimer(UINT nIDEvent)
{
   MSG msg;
   BOOL bResult = PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_NOREMOVE);

   //CATCH-UP CODE
   //if there are other timer events in the que then we need to catch up
   if (bResult)
      return;

   // if we are not in play-back, there is nothing to do
   if (GetState() != 'PLAY')
   {
      KillTimer(nIDEvent);
      return;
   }

   /////////////////////////////////////////////////////////
   //this causes ship motion to be smooth
   //for all speeds and modes
   DWORD elapsedMilliseconds = HALF_SECOND;
   /////////////////////////////////////////////////////////

#define PLAYBACK_TIMESLICE ('REAL')  //('FIXD')
#if (PLAYBACK_TIMESLICE == 'REAL')
   /////////////////////////////////////////////////////////
   //this causes ship motion to be extremely jumpy
   //it looks bad.
   //how much real-time elapsed since the previous iteration
   DWORD _tickCount = m_tickCount;
   m_tickCount=GetTickCount();
   elapsedMilliseconds = m_tickCount - _tickCount;
   //real-time rate adjusted
   elapsedMilliseconds *= get_playback_rate();
   /////////////////////////////////////////////////////////
#endif

   //convert milliseconds to days
   COleDateTimeSpan days = ((double)elapsedMilliseconds * DAYS_PER_MILLISECOND);

   // update elapsed time.  If reversed flag is set, we will playback in
	// reverse
	DWORD start_tick = GetTickCount();
	{
		if (m_reverse_playback.GetCheck())
			SetElapsedTime( GetElapsedTime() - days );
		else
			SetElapsedTime( GetElapsedTime() + days );

		CView *view = UTL_get_active_non_printing_view();
		if (view)
		{
			view->UpdateWindow();
			CDC *pDC = view->GetDC();
			
			CRect clip_rect;
			int ret = pDC->GetClipBox(&clip_rect);
			view->ReleaseDC(pDC);

			if (ret != NULLREGION && CMainFrame::GetPlaybackDialog().in_playback())
			{
				LONG lIdle = 0;
				while (AfxGetApp()->OnIdle(lIdle++));
			}
		}
	}
	DWORD end_tick = GetTickCount();
	DWORD time_to_update = end_tick - start_tick;

	// if the time to signal and update the overlays is longer than the
	// 
	if (time_to_update > m_current_timeout)
	{
      UINT new_timeout = static_cast<int>(time_to_update * 1.1);

      if (m_current_timeout != new_timeout)
      {
		   KillTimer(nIDEvent);
		   SetTimer(nIDEvent, new_timeout, NULL); 

		   m_current_timeout = new_timeout;
         //TRACE("Adjusting timer (slower) to %d ms\n", m_current_timeout);
      }
	}
	else if (time_to_update < m_current_timeout)
	{
		const UINT target = HALF_SECOND / get_playback_rate();
      time_to_update = static_cast<UINT>(time_to_update * 1.1);
		UINT new_timeout = __max(target, time_to_update);

      if (new_timeout != m_current_timeout)
      {
		   KillTimer(nIDEvent);
		   SetTimer(nIDEvent, new_timeout, NULL);

		   m_current_timeout = new_timeout;
         //TRACE("Adjusting timer (faster) to %d ms\n", m_current_timeout);
      }
	}
}

void CViewTime::Play()
{
   if (GetState()=='PLAY')
   {
      SetState('PLAY');
      return;
   }

	// if the playback is reversed then we will reset playback if at the start
	if (m_reverse_playback.GetCheck())
	{
		if (m_elapsed <= COleDateTimeSpan(0,0,0,0))
			Reset();
	}
	// otherwise, reset playback when at the end of the time span
	else
	{
		if (m_elapsed >= m_duration)
			Reset();
	}

   

   UINT tick = HALF_SECOND;
   UINT timeout = tick / get_playback_rate();

   //get the number of milliseconds since windows was started
   //this value rolls over every 49.7 days
   m_tickCount = GetTickCount();

   m_timerHandle = SetTimer((UINT)this, timeout, NULL);
   //TRACE("Setting timer to %d ms\n", timeout);

	m_current_timeout = timeout;

   SetState('PLAY');
}

void CViewTime::OnToggleState() 
{
	// if we have zero duration, then it doesn't make sense to toggle
	// the state since the controls will be disabled (remember, we can
	// get here from toggle_state())
	if (m_duration.GetTotalSeconds() == 0)
		return;

   switch (GetState())
   {
   default:
   case 'PLAY':
      Stop();
      break;
   case 'STOP':
      Play();
      break;
   }
}

void CViewTime::OnViewtimeMore() 
{
	AfxMessageBox("Future site of additional playback controls");
}

void CViewTime::XForm(int direction)
{
   switch (direction)
   {
   case 'WRIT': //to the dialog
   case 'TO':
		m_date.SetTime(m_tn);
		SetWindowText(m_tn.Format("%b %d %H:%M:%S %Y"));
      break;
   }
}

void CViewTime::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
   XForm('TO');

   refresh_gant_window();

	// Do not call CDialog::OnPaint() for painting messages
}

/*
   Flame ON !!!
   MFC is clipping the UINT nPos paramter in ::OnHScroll to 16bits
   The result is a signed short.
   So the maximum valid slider position is 0x7fff (32767)
   Code excerpt from mfc/src/wincore.cpp line 1881:
	case AfxSig_vwwx:
      ...
      int nPos = (short)HIWORD(wParam);
   This is not documented in the CSliderCtrl::SetRange function.
   The parameters to SetRange should be shorts and not ints
   !!! Flame OFF
*/
#define MAXSLIDER (0x7fff) //careful! see note above

double CViewTime::PosToSeconds( short pos )
{
   double seconds = pos;
   seconds /= MAXSLIDER;
   seconds *= m_seconds;
   return seconds;
}

short CViewTime::SecondsToPos( double seconds )
{
	// watch out for divide by zero
	if (m_seconds == 0)
		return 0;

   double pos = seconds;
   pos /= m_seconds;
   pos *= MAXSLIDER;
   return (short)pos;
}

void CViewTime::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   switch (nSBCode)
   {
   case   SB_LEFT:      //   Scroll to far left.
   case   SB_LINELEFT:  //   Scroll left.
   case   SB_PAGELEFT:  //   Scroll one page left.
      SetElapsedTime( GetElapsedTime()-COleDateTimeSpan(0,0,1,0) );
      break;
      
   case   SB_RIGHT:     //   Scroll to far right.
   case   SB_LINERIGHT: //   Scroll right.
   case   SB_PAGERIGHT: //   Scroll one page right.
      SetElapsedTime( GetElapsedTime()+COleDateTimeSpan(0,0,1,0) );
      break;
      
   case   SB_THUMBPOSITION://   Scroll to absolute position. The current position is specified by the nPos parameter.
   case   SB_THUMBTRACK:   //   Drag sc
      COleDateTimeSpan dts = ((double)PosToSeconds(nPos)/SECONDS_PER_DAY);
      SetElapsedTime(dts);
      break;
   }
}

void CViewTime::SetElapsedTime(COleDateTimeSpan elapsed)
{
   // make sure the interval is properly initialized
   if (!m_interval_initialized)
      InitInterval();

   // if the given time span is less than zero, then set the elapsed 
   // time to zero
   if (elapsed < COleDateTimeSpan(0,0,0,0))
      elapsed = 0;
   // otherwise, if the given time span is greater than the duration, then 
   // set the elapsed time to the duration
   else if (elapsed > m_duration)
      elapsed = m_duration;

   m_elapsed = elapsed;

   // set the slider position to the elapsed time
   double seconds = (m_elapsed * SECONDS_PER_DAY);

   if (m_slider.m_hWnd != NULL)
      m_slider.SetPos(SecondsToPos(seconds));

   // set the current time
   m_tn = m_t0 + m_elapsed;

   if (!m_editLock)
   {
      if (m_slider.m_hWnd != NULL)
         XForm('TO');
   }

	// invalidate the text rect on the screen if necessary since the
	// time has changed
	if (m_draw_time_on_map)
		OVL_get_overlay_manager()->invalidate_rect(m_rect);

   // update status bar pane
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
   {
      CTextInfoPane *pUTCInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_FALCONVIEW_TIME_UTC));
      if (pUTCInfoPane)
      {
         // DDHHMMSSZ MON YY
         CString strCurrentTime = m_tn.Format("%d%H%M%SZ %b ");
         CString strYear; // there is a VS 2005 bug that breaks dates before 1900, so we have to do the year separately
         strYear.Format("%04d", m_tn.GetYear());
         strCurrentTime.Append(strYear);
         pUTCInfoPane->SetText(strCurrentTime);
      }

      CTextInfoPane *pLocalInfoPane = static_cast<CTextInfoPane *>(pFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_FALCONVIEW_TIME_LOCAL));
      if (pLocalInfoPane)
      {
         long lTimeZoneSeconds, lDaylightSavingsTimeBias;
         _get_timezone(&lTimeZoneSeconds);
         _get_dstbias(&lDaylightSavingsTimeBias);

         lTimeZoneSeconds += lDaylightSavingsTimeBias;

         const int nHours =  lTimeZoneSeconds / (60 * 60);

         COleDateTime dt = m_tn - COleDateTimeSpan(0, nHours, 0, 0);
         CString strLocalTime;
         strLocalTime.Format("%s (UTC %c %d:00)", dt.Format("%b %d %H:%M:%S %Y"), (nHours > 0) ? '-' : '+', nHours);

         pLocalInfoPane->SetText(strLocalTime);
      }
   }

   Signal();

	int reverse = m_reverse_playback.m_hWnd && m_reverse_playback.GetCheck();

	if (reverse && m_elapsed == COleDateTimeSpan(0,0,0,0))
		Stop();
	else if (!reverse && m_elapsed >= m_duration)
      Stop();
}

void CViewTime::SetPlaybackRate( UINT rate )
{
   if (rate > MAX_PLAYBACK_RATE)
      rate = MAX_PLAYBACK_RATE;
   else if (rate < MIN_PLAYBACK_RATE)
		rate = MIN_PLAYBACK_RATE;

   if (m_rate != rate)
   {
      // set the new rate before notifying observers
      m_rate = rate;

      OVL_get_overlay_manager()->OnPlaybackRateChanged();
      
      POSITION position = m_listCallbacks.GetHeadPosition();
      while (position)
      {
         IDispatch *pDispatch = m_listCallbacks.GetNext(position);

         try
         {
            fvw::IPlaybackDialogCallbackPtr smpCallback = pDispatch;
            if (smpCallback != NULL)
               smpCallback->OnPlaybackRateChanged(rate);
            else
            {
               CPlaybackDialogCallback callback(pDispatch);
               callback.OnPlaybackRateChanged(rate);
            }
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("OnPlaybackRateChanged failed - %s", (char *)e.Description());
            ERR_report(msg);
         }
         catch(COleException *e)
         {
            ERR_report_ole_exception("OnPlaybackRateChanged failed", *e);
            e->Delete();
         }
      }
   }
	
   m_rateS.Format("%d",rate);
   UpdateData(FALSE);
	
	if (GetState() == 'PLAY')
	{
		Stop();
		Play();
	}
}

int CViewTime::get_playback_rate()
{
	// if we haven't read the rate from the registry yet do so now
   if (m_rate == INVALID_PLAY_BACK_RATE)
		m_rate = PRM_get_registry_int("ViewTime","Rate", DEFAULT_PLAY_BACK_RATE);

	return m_rate;
}

int CViewTime::get_anchor_pos()
{
	if (m_anchor_pos == UTIL_ANCHOR_INVALID)
		m_anchor_pos = PRM_get_registry_int("ViewTime", "AnchorPos", DEFAULT_ANCHOR_POS);

	return m_anchor_pos;
}

void CViewTime::OnSelchangeViewtimeRate() 
{
   int rate;
   int sel = m_rateC.GetCurSel();
   switch(sel)
   {
   default:
   case 0: rate=1; break;
   case 1: rate=2; break;
   case 2: rate=5; break;
   case 3: rate=10; break;
   case 4: rate=20; break;
   case 5: rate=50; break;
	case 6: rate=99; break;
   }
   m_rateS.Format("%d",rate);
	SetPlaybackRate(rate);

   m_editLock=false;
}

void CViewTime::OnSetfocusVIEWTIMETn() 
{
   m_editLock=true;	
}

void CViewTime::OnKillfocusVIEWTIMETn() 
{
   XForm('FROM');
   m_editLock=false;
   Travel(m_tn);
}

void CViewTime::OnUpdateVIEWTIMETn() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
}

void CViewTime::OnChangeVIEWTIMETn() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	
}

void CViewTime::OnDropdownViewtimeRate() 
{
   m_editLock=true;
}

void CViewTime::OnKillfocusViewtimeRate() 
{
   UpdateData(TRUE);
   int rate = atoi(m_rateS);
   SetPlaybackRate(rate);
   rate = GetPlaybackRate();
   m_rateS.Format("%d",rate);

   m_editLock = false;	
}

BOOL CViewTime::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CViewTime::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CDialog::OnCommand(wParam, lParam);
}

void CViewTime::OnOK() 
{
   XForm('FROM');

   m_editLock=false;
   Travel(m_tn);

   CWnd *pWnd = GetDlgItem(IDC_VIEWTIME_PLAYSTOP);
   GotoDlgCtrl(pWnd);
}

void CViewTime::OnCancel() 
{
	Invalidate(false);
}

void CViewTime::UpdateAbsoluteTimeFrame(COleDateTime dt)
{
   if (dt < m_t0)
      set_start_time(dt);
   else if (dt > m_t1)
      set_stop_time(dt);

   update_start_end_times();
}

void CViewTime::SetAbsoluteTime(COleDateTime dt)
{
   // let the bounds know
   UpdateAbsoluteTimeFrame(dt);
   
   // calculate the elapsed time
   COleDateTimeSpan elapsed = dt - m_t0;
   
   // make it happen
   SetElapsedTime(elapsed);
}

void CViewTime::set_absolute_time(COleDateTime dt)
{
   // signal the dialog
   SetAbsoluteTime(dt);

   // tell the world
   signal(dt);
}

void CViewTime::set_start_time(COleDateTime dt)
{
   m_t0 = dt;
   m_duration = m_t1 - m_t0;
   update_slider_range();
}

void CViewTime::set_stop_time(COleDateTime dt)
{
   m_t1 = dt;
   m_duration = m_t1 - m_t0;
   update_slider_range();
}

//static entry point
void CViewTime::on_new_absolute_time(COleDateTime dt)
{
	if (!m_is_set)
	{
		m_t0 = dt;
		m_t1 = dt;
		m_is_set = true;
	}

   // make sure the controls are enabled 
   enable_controls(TRUE);
   
   if (GetState() == 'PLAY')
   {
      UpdateAbsoluteTimeFrame(dt);
      return;
   }
   
   if (GetState() == 'STOP')
   {
      // are we sliding somewhere in the middle
      if (m_elapsed > COleDateTimeSpan(0,0,0,0) && m_elapsed < m_duration)
      {
         UpdateAbsoluteTimeFrame(dt);
         return;
      }
      
      // time cursor attached to future edge
      if (m_elapsed == m_duration && dt > m_t1 ||
         m_elapsed == COleDateTimeSpan(0,0,0,0) && dt < m_t0)
      {
         SetAbsoluteTime(dt);
         return;
      }

		Signal();
   }
}

// returns TRUE if the time cursor is attached to the future edge
boolean_t CViewTime::is_future_edge()
{
	return (m_elapsed == m_duration || m_elapsed == COleDateTimeSpan(0,0,0,0));
}

void CViewTime::set_current_time(COleDateTime dt) 
{ 
	if (dt.GetStatus() == COleDateTime::invalid)
		return;

	if (!m_is_set)
	{
		m_t0 = dt;
		m_tn = dt;
		m_t1 = dt;
	}
	else
	{
		if (dt <= m_t0)
			m_tn = m_t0;
		else if (dt >= m_t1)
			m_tn = m_t1;
		else
			m_tn = dt; 
		
		if (!m_interval_initialized)
			InitInterval();
	}
   
   SetElapsedTime(m_tn - m_t0);
}

// enable/disable the playback controls
void CViewTime::enable_controls(boolean_t enable)
{
	m_controls_enabled = enable;

   if (IsWindow(m_resetB.m_hWnd))
   {
      m_resetB.EnableWindow(enable);
      m_endsetB.EnableWindow(enable);
      m_playstopB.EnableWindow(enable);
      GetDlgItem(IDC_DETAIL)->EnableWindow(enable);
   }
}

// update the starting and ending edit boxes
void CViewTime::update_start_end_times()
{
   if (IsWindow(m_resetB.m_hWnd))
   {
      CString start = "Start : " + m_t0.Format("%b %d %H:%M:%S %Y");
      CString end = "End  : " + m_t1.Format("%b %d %H:%M:%S %Y");
      
      GetDlgItem(IDC_START)->SetWindowText(start);
      GetDlgItem(IDC_END)->SetWindowText(end);
   }

	// if the current time is out of range then gray the button - sync 
	// to clock would only snap to the ends in this case
	if (IsWindow(m_hWnd) && IsWindow(GetDlgItem(IDC_SYNC)->m_hWnd))
	{
		COleDateTime current_time;
		CTime time = CTime::GetCurrentTime();
		tm t1;
      time.GetGmtTm(&t1);
		current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
			t1.tm_min, t1.tm_sec);

		// the new size of the view time dialog depends on the
		// number of time sensitive overlays
		int num_tsovl = OVL_get_overlay_manager()->
			get_num_time_sensitive_overlays();
		
		const int disable = (current_time < m_t0 || m_t1 < current_time);
		GetDlgItem(IDC_SYNC)->EnableWindow(!disable || num_tsovl == 0 || !m_interval_initialized);
	}
}

void CViewTime::OnDetails()
{
   CRect window_rect;
   GetWindowRect(window_rect);
   
   // the new size of the view time dialog depends on the
   // number of time sensitive overlays
   int num_tsovl = OVL_get_overlay_manager()->
      get_num_time_sensitive_overlays();
   
   // if we are going from no details to details (show details)
   if (!m_show_details)
   {
      // change the bitmap from down arrow to up arrow
      CButton *button = (CButton *)GetDlgItem(IDC_DETAIL);
      button->SetWindowText("<< Details");
      
      // Expand the window to accomodate the gant chart.  Only
		// expand enough to accomodate four time sensitive overlays.
		// After that, we will add the scroll bar
      m_delta_height = (__min(num_tsovl,4)+2)*GANT_ROW_HEIGHT+GANT_ROW_HEIGHT/2;
      MoveWindow(window_rect.TopLeft().x, window_rect.TopLeft().y,
         window_rect.Width(), window_rect.Height() + m_delta_height);

		m_gantt_wnd = new GanttWnd(this, num_tsovl);
      
      Invalidate();
      
      m_show_details = TRUE;
   }
   // otherwise we are going from details to no details (hide details)
   else
   {
      // change the bitmap from up arrow to down arrow
      CButton *button = (CButton *)GetDlgItem(IDC_DETAIL);
      button->SetWindowText("Details >>");
      
      // shrink the window
      MoveWindow(window_rect.TopLeft().x, window_rect.TopLeft().y,
         window_rect.Width(), window_rect.Height() - m_delta_height);
      
      delete m_gantt_wnd;
      m_gantt_wnd = NULL;
      m_show_details = FALSE;
   }
}

void CViewTime::refresh_gant_window()
{
   // make sure the gant chart window actually exists
   if (m_gantt_wnd == NULL || !m_gantt_wnd->m_hWnd)
      return;

   m_gantt_wnd->refresh(m_tn);
}

// called when an overlay is closed, opened or has changed its color
void CViewTime::update_gant_chart()
{
	// reinitialize the view time interval
   InitInterval();
	
	// get the total number of time sensitive overlays
	
   int num_tsovl = OVL_get_overlay_manager()->
      get_num_time_sensitive_overlays();
	
	// if there are no longer any time sensitive overlays then effectively
   // hide the details...
   if (num_tsovl == 0)
	{
		// sync to current time if no overlays are opened
		COleDateTime current_time;
		CTime time = CTime::GetCurrentTime();
		tm t1;
      time.GetGmtTm(&t1);
		current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
			t1.tm_min, t1.tm_sec);
		
		m_t0 = current_time;
		m_t1 = current_time;
		set_current_time(current_time);
		m_is_set = false;
		
		update_start_end_times();
		
		if (m_show_details)
			OnDetails();
		
		if (IsWindow(m_hWnd))
		{
			CWnd *wnd = GetDlgItem(IDC_DETAIL);
			if (wnd)
				wnd->EnableWindow(FALSE);
		}
		
      return;
	}
   
   // make sure the gant chart window actually exists
   if (m_gantt_wnd == NULL || !m_gantt_wnd->m_hWnd)
      return;
   
   int old_delta_height = m_delta_height;
   m_delta_height = (__min(num_tsovl,4)+2)*GANT_ROW_HEIGHT+GANT_ROW_HEIGHT/2;
   
   if (old_delta_height != m_delta_height)
   {
      CRect window_rect;
      GetWindowRect(window_rect);
      
      // resize the window to take into account the removal/addition of any time
      // sensitive overlays
      MoveWindow(window_rect.TopLeft().x, window_rect.TopLeft().y,
         window_rect.Width(), m_window_rect.Height() + m_delta_height);
   }

	// resize the gant chart window
	delete m_gantt_wnd;
	m_gantt_wnd = new GanttWnd(this, num_tsovl);
   
   Invalidate();
}

void CViewTime::OnDisplay()
{
	m_draw_time_on_map = m_display_checkbox.GetCheck();

	// invalidate the map so that the current time display gets erased or drawn
	OVL_get_overlay_manager()->invalidate_all();
}

OvlFont &CViewTime::get_font()
{
	// read the font properties from the registry if necessary
	if (m_font_is_invalid)
	{
	   m_font.initialize_from_registry("ViewTime\\Font",
         "Arial",30,0,UTIL_COLOR_BLACK, UTIL_BG_RECT, UTIL_COLOR_SKY_BLUE);
		m_font_is_invalid = FALSE;
	}

	return m_font;
}

// draw the time on the screen
void CViewTime::draw(ActiveMap *map)
{
	if (m_draw_time_on_map)
	{
		CDC *dc = map->get_CDC();
		int surface_width, surface_height;
		int x, y;
		CString fontname;
		int size, attributes, fg_color, type, bg_color;
		CFvwUtil *futil = CFvwUtil::get_instance();
		POINT pt[4];
		CString current_time = m_tn.Format("%b %d %H:%M:%S %Y");

		const int anchor_pos = get_anchor_pos();

		OvlFont &font = get_font();
		
		// get the font information from the font object
		font.get_font(fontname, size, attributes);
      font.get_foreground(fg_color);
      font.get_background(type, bg_color);

		// get the position to draw the text on the map
		map->get_surface_size(&surface_width, &surface_height);
		switch (anchor_pos)
		{
		case UTIL_ANCHOR_LOWER_LEFT:
			x = 0;
			y = surface_height;
			break;
		case UTIL_ANCHOR_LOWER_CENTER:
			x = surface_width / 2;
			y = surface_height;
			break;
		case UTIL_ANCHOR_LOWER_RIGHT:
			x = surface_width;
			y = surface_height;
			break;
		case UTIL_ANCHOR_UPPER_LEFT:
			x = 0;
			y = 0;
			break;
		case UTIL_ANCHOR_UPPER_CENTER:
			x = surface_width / 2;
			y = 0;
			break;
		case UTIL_ANCHOR_UPPER_RIGHT:
			x = surface_width;
			y = 0;
			break;
		}
		
		// draw the text to the screen
		futil->draw_text(dc, current_time, x, y, anchor_pos, fontname, size, 
			attributes, type, fg_color, bg_color, 0, pt);

		m_rect = CRect(pt[0].x, pt[1].y, pt[2].x, pt[3].y);
	}
}

void CViewTime::OnFont()
{
	CFontDlg dlg;
   dlg.set_apply_callback(&on_apply_font);
   dlg.set_font(get_font());
	 
	// turn off playmode mode when changing the font.  This is because
	// the view time dialog hogs the user-interface time and could
	// potentially lock up FalconView when the font dialog is opened
	boolean_t old_state = m_state;
	Stop();

	if (dlg.DoModal() == IDOK) 
	{
		m_font = dlg;

		// redraw overlay
		OVL_get_overlay_manager()->invalidate_all();
	}

	// restore the old playback mode if necessary
	if (old_state == 'PLAY')
		Play();
}

// callback function for CFontDlg's Apply button
void CViewTime::on_apply_font(CFontDlg *dlg)
{
	CMainFrame::GetPlaybackDialog().m_font = *dlg;
	
	// redraw
	OVL_get_overlay_manager()->invalidate_all();
}

// handle the combo-box IDC_POSITION selection change
void CViewTime::OnChangeAnchorPosition()
{
	switch (m_position_combo.GetCurSel())
	{
	case 0: m_anchor_pos = UTIL_ANCHOR_LOWER_LEFT; break;
	case 1: m_anchor_pos = UTIL_ANCHOR_LOWER_CENTER; break;
	case 2: m_anchor_pos = UTIL_ANCHOR_LOWER_RIGHT; break;
	case 3: m_anchor_pos = UTIL_ANCHOR_UPPER_LEFT; break;
	case 4: m_anchor_pos = UTIL_ANCHOR_UPPER_CENTER; break;
	case 5: m_anchor_pos = UTIL_ANCHOR_UPPER_RIGHT; break;
	}

	// redraw
	OVL_get_overlay_manager()->invalidate_all();
}

void CViewTime::OnSyncToClockTime()
{
   COleDateTime current_time;
	CTime time = CTime::GetCurrentTime();
	tm t1;
   time.GetGmtTm(&t1);
	current_time.SetDateTime(1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday, t1.tm_hour, 
		t1.tm_min, t1.tm_sec);

	set_current_time(current_time);
}

void CViewTime::OnDateChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	COleDateTime date_time;
	m_date.GetTime(date_time);
	
	set_current_time(date_time);
	
	*pResult = 0;
}
LRESULT CViewTime::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

