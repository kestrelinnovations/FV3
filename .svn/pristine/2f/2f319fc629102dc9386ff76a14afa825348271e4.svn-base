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

// TAMaskStatus.h: interface for the CTAMaskStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAMASKSTATUS_H__45405F45_084C_4EAA_9939_DC094A0C8A9C__INCLUDED_)
#define AFX_TAMASKSTATUS_H__45405F45_084C_4EAA_9939_DC094A0C8A9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTAMaskStatus : public CDialog  
{

public:
   CTAMaskStatus();
	CTAMaskStatus(CWnd* pParent /*=NULL*/);
	virtual ~CTAMaskStatus();

// Dialog Data
	//{{AFX_DATA(TAMaskStatus)
	enum { IDD = IDD_TAMASK_STATUS_DISPLAY };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Terrain_Avoidance_with_Moving_Map.htm";}

	int m_Altitude;
	CString	m_CautionText;
	CString	m_OKText;
	CString	m_WarnText;
   BOOL     m_ShowClearances;    // Show the clearance values as margins not altitudes
	//}}AFX_DATA

   // Pass in the TAMask XML preference string or subset
   bool set_preferences(LPCTSTR preference_string);

   // Altitude is in feet, Use -99999 to flag a lost feed on the altitude
   void set_altitude(int Altitude);       // Source is a manual entry
   void set_gps_altitude(int Altitude, C_overlay *pOverlay);   // Source is a measured / GPS or Route source

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTAMaskStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnOK();
   virtual void OnRadioClicked();
	afx_msg void OnClose();
	//}}AFX_MSG

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TAMaskStatus)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   void draw_color_button_rgb(CWnd *cwnd, int button_id, COLORREF color);
   void draw_basic_button(CDC * dc, RECT rc);

   COLORREF m_WarnColor;
	COLORREF m_CautionColor;
	COLORREF m_OKColor;

	COLORREF m_StatusColor; // Used to draw the status color frame, Caution color for manual altitude feed
                           // OK color for valid Altitude feed, Warning for lost feed

   bool m_ShowWarn;        // Show clearance level for "warning"
   bool m_ShowCaution;
   bool m_ShowOK;

   int   m_WarnClearance;     // Margins of clearance
   int   m_CautionClearance;
   int   m_OKClearance;

   int   m_TestAlt;           // Active altitude
   bool  m_UsingGPSAlt;       // In GPS active map mode
   bool  m_LastKnown;         // GPS lost, using last known altitude
};

#endif // !defined(AFX_TAMASKSTATUS_H__45405F45_084C_4EAA_9939_DC094A0C8A9C__INCLUDED_)