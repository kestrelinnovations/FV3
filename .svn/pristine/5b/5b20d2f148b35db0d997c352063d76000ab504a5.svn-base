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



#if !defined(AFX_CSTTRACK_H__FCA2557D_8FE5_11D3_80AA_00C04F60B086__INCLUDED_)
#define AFX_CSTTRACK_H__FCA2557D_8FE5_11D3_80AA_00C04F60B086__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ctrakdlg.h : header file for the coast track dialog
//

#include "..\resource.h"
#include "common.h"
#include "..\..\fvw_core\geo_tool\common.h"  // for units_t
#include "FalconView/include/geocx1.h"

class CMappedBitmapButton : public CBitmapButton
{
public:
   BOOL LoadMapped();
};

// foward declarations
class CoastTrackManager;
class C_gps_trail;

/////////////////////////////////////////////////////////////////////////////
// CoastTrackPage dialog

class CoastTrackData;

class CoastTrackPage : public CPropertyPage
{
   DECLARE_DYNCREATE(CoastTrackPage)

private:
   boolean_t m_is_initialized;

// Construction
public:
   CoastTrackPage();
   CoastTrackPage(CoastTrackManager *ctrack_mgr);
   ~CoastTrackPage();

// Dialog Data
   //{{AFX_DATA(CoastTrackPage)
   enum { IDD = IDD_COAST_TRACK };
   CListCtrl m_ctrack_list_ctrl;
   CBitmapButton m_add_button;
   int m_update_rate;
   //}}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CoastTrackPage)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CoastTrackPage)
   virtual BOOL OnInitDialog();
   afx_msg void OnStop();
   afx_msg void OnCenter();
   afx_msg void OnDelete();
   afx_msg void OnDoubleClickList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnAddCoastTrack();
   afx_msg void OnEdit();
   afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnChangeUpdateRate();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
   // add_coast_track - adds a coast track to the list control with
   // the given coast track data.  Returns the index into the list
   // control where the coast track was added
   int add_coast_track(CoastTrackData *ctrack);

   // update_coast_track - updates a coast track with the given index
   // into the list control with the given coast track data
   void update_coast_track(int index, CoastTrackData *data);

   // delete a coast track from the list control with the given index
   void delete_coast_track(int index);

   // set_selected - will set a given element of the list control's
   // selection status to the given value
   void set_selected(int index, boolean_t is_selected);

   // hide the selected coast track from the dialog (but do not remove it)
   void hide_coast_track(int index);

   void set_update_rate(int update_rate) { m_update_rate = update_rate; }

private:
   // pointer to the coast track manager
   CoastTrackManager *m_coast_track_mgr;

public:
   void on_size(UINT nType, int cx, int cy);
   
   // return the minimum rectangle that this property page can be
   void GetMinimumRect(CRect &minrect);
};

/////////////////////////////////////////////////////////////////////////////
// CoastTrackDataDialog dialog

class CoastTrackDataDialog : public CDialog
{
// Construction
public:
   CoastTrackDataDialog(CWnd* pParent = NULL);   // standard constructor
   ~CoastTrackDataDialog();                      // destructor

   // returns a CoastTrackData object with the dialog data
   CoastTrackData &GetData();

   // set the dialog with the given CoastTrackData object
   void SetData(CoastTrackData &data);

   // does the coast track need to update the position
   boolean_t update_report() { return m_modified_report; }
   void set_modified_report(boolean_t mod) { m_modified_report = mod; }

// Dialog Data
   //{{AFX_DATA(CoastTrackDataDialog)
   enum { IDD = IDD_COAST_TRACK_DATA };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/mvmp/Coast_Track_Data_Editor.htm";}

   CButton m_vector_on_check;
   CButton m_initial_traj_on_check;
   CComboBox m_disttime_units;
   CEdit m_label_editctrl;
   CComboBox m_velocity_units;
   CEdit m_velocity_editctrl;
   CEdit m_heading_editctrl;
   CEdit m_disttime_editctrl;
   CGeocx1 m_geoctrl;
   BOOL m_vector_on;
   BOOL m_initial_traj_on;
   CMappedBitmapButton m_bitmap_button;
   CButton m_rhumbline_radio;
   CButton m_greatcircle_radio;
   CButton m_distance_radio;
   CButton m_time_radio;
   double m_disttime;
   int m_heading;
   double m_velocity;
   //}}AFX_DATA

private:
   CoastTrackData *m_ctrack_data;

   // updates the course vector group based on the current coast track 
   // data in m_ctrack_data
   void update_course_vector();

   units_t comboSel_to_enum(int index);
   int enum_to_comboSel(units_t units);

   // update the velocity group based on the current coast track
   // data in m_ctrack_data 
   void update_velocity();

   // used to handle the Apply
   boolean_t m_is_modified_not_add;
   CoastTrackManager *m_coast_track_mgr;
   int m_index;
   boolean_t m_already_added;
   boolean_t m_is_modified;

   boolean_t m_report_added;

   double m_geo_lat, m_geo_lon;

public:
   void set_report_added(boolean_t added) { m_report_added = added; }
   boolean_t get_report_added() { return m_report_added; }

   void set_already_added(boolean_t added) { m_already_added = added; }
   boolean_t get_already_added() { return m_already_added; }
   
   void set_coast_track_mgr(CoastTrackManager *mgr) { m_coast_track_mgr = mgr; }
   
   void set_index(int index) { m_index = index; }
   int get_index() { return m_index; }

   void set_is_modified(boolean_t mod) { m_is_modified = mod; }

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CoastTrackDataDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CoastTrackDataDialog)
   virtual BOOL OnInitDialog();
   afx_msg void OnGetPositionFromMap();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnRhumbline();
   afx_msg void OnGreatCircle();
   afx_msg void OnChangeLabel();
   afx_msg void OnVectorOn();
   afx_msg void OnInitialTraj();
   afx_msg void OnDistance();
   afx_msg void OnTime();
   afx_msg void OnKillfocusDisttimeEdit();
   afx_msg void OnSelchangeDisttimeUnits();
   afx_msg void OnKillfocusHeading();
   afx_msg void OnSelchangeVelocityUnits();
   afx_msg void OnKillfocusVelocity();
   afx_msg void OnApply();
   afx_msg void OnGeocxKillFocus();
   virtual void OnOK();
   //}}AFX_MSG
   DECLARE_EVENTSINK_MAP()
   DECLARE_MESSAGE_MAP()

private:
   boolean_t m_modified_report;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSTTRACK_H__FCA2557D_8FE5_11D3_80AA_00C04F60B086__INCLUDED_)