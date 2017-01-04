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

// IPlaybackDialog.h
//


class PlaybackDialog: public CCmdTarget
{
	DECLARE_DYNCREATE(PlaybackDialog)
   DECLARE_OLECREATE(PlaybackDialog)

	PlaybackDialog();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlaybackDialog)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~PlaybackDialog();

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(PlaybackDialog)
   afx_msg long StartPlayback();
   afx_msg long StopPlayback();
   afx_msg long SetCurrentDateTime(DATE dateCurrent);
   afx_msg long GetCurrentDateTime(DATE *pCurrent);
   afx_msg long SetCurrentTimeToBeginning();
   afx_msg long SetCurrentTimeToEnd();
   afx_msg long SetTimeSpan(DATE dateStart, DATE dateEnd);
   afx_msg long SetPlaybackRate(long lPlaybackRate);
   afx_msg long RegisterForNotifications(IDispatch *pPlaybackDialogCallback);   
   afx_msg long UnregisterForNotifications();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

   IDispatch *m_pPlaybackDialogCallback;
};

