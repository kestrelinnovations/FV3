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

// IPlaybackDialog.cpp
//

#include "StdAfx.h"
#include "IPlaybackDialog.h"
#include "err.h"        // for ERR_report
#include "mainfrm.h"
#include "PlaybackDialog\viewtime.h"

IMPLEMENT_DYNCREATE(PlaybackDialog, CCmdTarget)

IMPLEMENT_OLECREATE(PlaybackDialog, "FalconView.PlaybackDialog", 0x417B8832, 0x934F, 0x4101, 
                   0xB9, 0x6E, 0x40, 0xDE, 0x43, 0x56, 0x3E, 0x12)

PlaybackDialog::PlaybackDialog() 
   : m_pPlaybackDialogCallback(NULL)
{
	EnableAutomation();
}

PlaybackDialog::~PlaybackDialog()
{
   UnregisterForNotifications();
}

void PlaybackDialog::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(PlaybackDialog, CCmdTarget)
	//{{AFX_MSG_MAP(PlaybackDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(PlaybackDialog, CCmdTarget)
	//{{AFX_DISPATCH_MAP(PlaybackDialog)
   DISP_FUNCTION(PlaybackDialog, "StartPlayback", StartPlayback, VT_I4, VTS_NONE)
   DISP_FUNCTION(PlaybackDialog, "StopPlayback", StopPlayback, VT_I4, VTS_NONE)
   DISP_FUNCTION(PlaybackDialog, "SetCurrentDateTime", SetCurrentDateTime, VT_I4, VTS_DATE)
   DISP_FUNCTION(PlaybackDialog, "GetCurrentDateTime", GetCurrentDateTime, VT_I4, VTS_PDATE)
   DISP_FUNCTION(PlaybackDialog, "SetCurrentTimeToBeginning", SetCurrentTimeToBeginning, VT_I4, VTS_NONE)
   DISP_FUNCTION(PlaybackDialog, "SetCurrentTimeToEnd", SetCurrentTimeToEnd, VT_I4, VTS_NONE)
   DISP_FUNCTION(PlaybackDialog, "SetTimeSpan", SetTimeSpan, VT_I4, VTS_DATE VTS_DATE)
   DISP_FUNCTION(PlaybackDialog, "SetPlaybackRate", SetPlaybackRate, VT_I4, VTS_I4)
   DISP_FUNCTION(PlaybackDialog, "RegisterForNotifications", RegisterForNotifications, VT_I4, VTS_DISPATCH)
   DISP_FUNCTION(PlaybackDialog, "UnregisterForNotifications", UnregisterForNotifications, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IPlaybackDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IPlaybackDialog =
{ 0xC7C24556, 0x772C, 0x42a5, { 0xBA, 0xCD, 0x76, 0x89, 0xCA, 0xFB, 0xB1, 0x8C } };

BEGIN_INTERFACE_MAP(PlaybackDialog, CCmdTarget)
	INTERFACE_PART(PlaybackDialog, IID_IPlaybackDialog, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlaybackDialog message handlers

long PlaybackDialog::StartPlayback()
{
   if (!CMainFrame::GetPlaybackDialog().in_playback())
		CMainFrame::GetPlaybackDialog().toggle_state();

	return SUCCESS;
}

long PlaybackDialog::StopPlayback()
{
   if (CMainFrame::GetPlaybackDialog().in_playback())
		CMainFrame::GetPlaybackDialog().toggle_state();

	return SUCCESS;
}

long PlaybackDialog::SetCurrentDateTime(DATE dateCurrent)
{
   CMainFrame::GetPlaybackDialog().set_current_time(dateCurrent);

	return SUCCESS;
}

long PlaybackDialog::GetCurrentDateTime(DATE *pCurrent)
{
   *pCurrent = (DATE)CMainFrame::GetPlaybackDialog().get_current_time();
   return SUCCESS;
}

long PlaybackDialog::SetCurrentTimeToBeginning()
{
   CMainFrame::GetPlaybackDialog().Reset();

	return SUCCESS;
}

long PlaybackDialog::SetCurrentTimeToEnd()
{
   CMainFrame::GetPlaybackDialog().Endset();

	return SUCCESS;
}

long PlaybackDialog::SetTimeSpan(DATE dateStart, DATE dateEnd)
{
   CMainFrame::GetPlaybackDialog().update_interval(dateStart, dateEnd);

   return SUCCESS;
}

long PlaybackDialog::SetPlaybackRate(long lPlaybackRate)
{
   if (lPlaybackRate < 1 || lPlaybackRate > 99)
		return FAILURE;

	CMainFrame::GetPlaybackDialog().SetPlaybackRate(lPlaybackRate);

	return SUCCESS;
}

long PlaybackDialog::RegisterForNotifications(IDispatch *pPlaybackDialogCallback)
{
   int status = SUCCESS;
   try
   {
      m_pPlaybackDialogCallback = pPlaybackDialogCallback;
      m_pPlaybackDialogCallback->AddRef();
      
      status = CMainFrame::GetPlaybackDialog().RegisterForNotifications(m_pPlaybackDialogCallback);
   }
   catch(_com_error &e)
   {
      status = FAILURE;
      CString msg;
      msg.Format("IPlaybackDialog::RegisterForNotifications failed: %s", (char *)e.Description());
      ERR_report(msg);
   }

   return status;
}

long PlaybackDialog::UnregisterForNotifications()
{
   int status = SUCCESS;
   if (m_pPlaybackDialogCallback != NULL)
   {
      try
      {
         status = CMainFrame::GetPlaybackDialog().UnregisterForNotifications(m_pPlaybackDialogCallback);
         if (status == FAILURE)
            ERR_report("Unable to unregister playback callback");
         
         m_pPlaybackDialogCallback->Release();
         m_pPlaybackDialogCallback = NULL;
      }
      catch(_com_error &e)
      {
         status = FAILURE;
         CString msg;
         msg.Format("IPlaybackDialog::UnregisterForNotifications failed: %s", (char *)e.Description());
         ERR_report(msg);
      }
   }

   return status;
}

