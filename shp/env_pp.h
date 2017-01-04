// Copyright (c) 1994-2011,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// env_pp.h

#ifndef ENV_PP_H
#define ENV_PP_H 1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// env_pp.h : header file
//

#include "overlay_pp.h"
#include "env.h"
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CEnvOvlOptionsPage dialog

class CEnvOvlOptionsPage : public COverlayPropertyPage
{
   DECLARE_DYNCREATE(CEnvOvlOptionsPage)

// Construction
public:
   CEnvOvlOptionsPage();
   ~CEnvOvlOptionsPage();

// Dialog Data
   //{{AFX_DATA(CEnvOvlOptionsPage)
   enum { IDD = IDD_OVL_ENV_DLG };
   BOOL  m_other_areas;
   BOOL  m_other_state_and_fed;
   BOOL  m_potent_sensitive;
   BOOL  m_state_local;
   BOOL  m_2000_agl;
   BOOL  m_indian_reservation;
   BOOL  m_background;
   int      m_poly_edge;
   BOOL  m_near_line;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CEnvOvlOptionsPage)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CEnvOvlOptionsPage)
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnApply();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void On2000Color();
   afx_msg void OnOtherAreaColor();
   afx_msg void OnOtherStateColor();
   afx_msg void OnIndianColor();
   afx_msg void OnSensColor();
   afx_msg void OnStateColor();
   afx_msg void OnFill();
   afx_msg void OnNearLine();
   afx_msg void OnBDefaultColors();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   C_env_ovl *m_env_ovl;
   char m_data[10];
   int m_fill_style;
   COLORREF m_2000_color;
   COLORREF m_sens_color;
   COLORREF m_indian_color;
   COLORREF m_state_color;
   COLORREF m_other_state_color;
   COLORREF m_other_area_color;
   static COLORREF m_CustClr[16]; // array of custom colors 

   void select_color(int ctrl, COLORREF & rgb_color); 
   void save_custom_colors();
   void load_custom_colors();
   void DrawFillButton();

private:
   BOOL SaveProperties() const;
   BOOL UpdateRegistry( const CString& csNewValue, LPCSTR pszValueName ) const;
   BOOL UpdateRegistry( LPCSTR pszNewValue, LPCSTR pszValueName ) const;
};

class CEnvironmentalPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CEnvOvlOptionsPage(); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ifndef ENV_PP_H
