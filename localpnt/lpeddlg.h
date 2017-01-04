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



// LPtEdDlg.h : header file
//

#ifndef LPTEDDLG_H
#define LPTEDDLG_H

//#include "localpnt.h"

#include "..\resource.h"
#include "FalconView/include/geocx1.h"
#include "..\..\..\fvw_core\common\SnapWindow.h"

//forward
class C_localpnt_point;
class C_localpnt_ovl;
class CGeocx1;
class CFontDlg;



/////////////////////////////////////////////////////////////////////////////
// CLocalPtEditDlg dialog

//{{AFX_INCLUDES()
#include "FalconView/include/geocx1.h"
//}}AFX_INCLUDES
class CLocalPtEditDlg : public CDialog
{
// Construction
public:
   CLocalPtEditDlg(CWnd* pParent = NULL);

   ~CLocalPtEditDlg();


// Dialog Data
   //{{AFX_DATA(CLocalPtEditDlg)
   enum { IDD = IDD_LOCAL_POINT_DLG };
   CButton m_follow_link;
   CComboBox m_group_name_control;
   CGeocx1 m_geocx;
   CString m_comment;
   CString m_dtd_id;
   CString m_description;
   CString m_id;
   CString m_group_name;
   double    m_latitude;
   double    m_longitude;
   double m_altitude_meters;
   CString m_elevation_source;
   CString m_elevation_source_map_handler_name;
   short m_elevation_source_map_series_id;
   int m_elevation;
   float m_vert_accuracy;
   float m_horz_accuracy;
   //}}AFX_DATA

// C_overlay *m_parent_overlay;
   C_localpnt_ovl *m_parent_overlay;

   CBitmap bitmap;

   //TO DO: kevin: comment
   //CString m_elevation_source;

   //TO DO: kevin: comment
   COleVariant m_bookmark;

   //TO DO: kevin: comment
   //initial values (before any modification)
   CString m_initial_id;
   CString m_initial_group_name;
   CString m_initial_description;
   CString m_initial_dtd_id;
   int m_initial_elevation;
   CString m_initial_elevation_source;
   CString m_initial_elevation_source_map_handler_name;
   short m_initial_elevation_source_map_series_id;
   double m_initial_latitude;
   double m_initial_longitude;
   double m_initial_altitude_meters;
   float  m_initial_horz_accuracy;
   float m_initial_vert_accuracy;
   CString m_initial_link_name;
   CString m_initial_comment;
   CString m_initial_icon;

   //TO DO: kevin: comment
   int m_last_elevation;

   //TO DO: kevin: comment
   CString m_overlay_filespec;   //local_point_overlay_filename

   //TO DO: kevin: comment
   HICON m_hIcon;
   CString m_icon;

   //is the mouse over the frame for the icon?
   BOOL m_mouse_in_frame;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CLocalPtEditDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
protected:
      CSnapWindow m_snapHandler; // helper class to support snaping to the desktop
   LRESULT OnEnterSizeMove(WPARAM, LPARAM);
   LRESULT OnMoving(WPARAM, LPARAM);


   // Generated message map functions
   //{{AFX_MSG(CLocalPtEditDlg)
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   afx_msg void OnOcxKillFocus();
   afx_msg void OnApply();
   afx_msg void OnLocalNext();
   afx_msg void OnLocalPrev();
   afx_msg void OnKillfocusLocptElev();
   afx_msg void OnSetfocusLocptElev();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnKillfocusLocptVertHorzAccu();
   afx_msg void OnKillfocusAltitudeMeters();
   afx_msg void OnClose();
   afx_msg void OnDestroy();
   afx_msg void OnEditLinks();
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnFont();
   DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:

   int set_edit_focus(CString id, CString group_name, CString filespec,
                      BOOL apply_changes = TRUE, BOOL redraw = TRUE);

   int apply_changes(BOOL redraw);

private:
   //Enable/Disable the dialog controls
   int EnableControlWindows(BOOL bEnable = TRUE);

   //TO DO: kevin: make these more efficient (e.g., let get_local_point take an overlay pointer if known)
   //get pointers to the local point and overlay we are
   C_localpnt_ovl* get_local_point_overlay() const;
   C_localpnt_point* get_local_point(const char* id, const char* group_name, const char* filespec) const;

   BOOL is_current_point_modified() const;
   BOOL has_primary_key_value_changed() const;

   //fill (or refill) group name combo box from database
   int fill_group_name_combo_box();

   //save/load dialog screen position in registry
   void save_dialog_position();
   void load_dialog_position(int& x, int& y);

   //for cycling
   POSITION get_local_point_position(const char* id, const char* group_name, const char* filespec) const;
   void cycle(int direction);

   //DDX functions
   void WINAPI DDX_float_blank_iff_less_than_zero(CDataExchange* pDX, int nIDC, float &f);
   void AFXAPI DDX_Elevation(CDataExchange* pDX, int nIDC,
      int& elevation, int initial_elevation, CString& elevation_source,
      const CString& initial_elevation_source);

   static void on_overlay_font(CFontDlg *dlg);

   CString FormatElevationSource();
};

#endif  // ifndef LPTEDDLG_H
