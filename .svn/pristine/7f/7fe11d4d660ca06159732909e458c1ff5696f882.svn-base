// Copyright (c) 1994-2011,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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



#if !defined(AFX_GPSSTYLE_H__B4BC16A9_4C12_11D2_816E_00609733A64E__INCLUDED_)
#define AFX_GPSSTYLE_H__B4BC16A9_4C12_11D2_816E_00609733A64E__INCLUDED_

class CGpsStyle;
class C_gps_trail;

#include "..\resource.h"
#include "button.h"
#include "overlay_pp.h"
#include "ovlelem.h"
#include "..\FontDlg.h"    // for CFontDlg, IFontDialogCallback
#include "MovingMapPropertyPageGroup.h"


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// gpsstyle.h : header file
//

//
// CGPSProperties - contains the properties for a GPS trail
//
enum gps_icon_t { GPS_ICON_CIRCLE, GPS_ICON_DIAMOND, GPS_ICON_ARROW };

class CBullseyeProperties
{
protected:
   // properties for the bullseye
   boolean_t m_bullseye_on;
   double m_range_radius; // in meters
   units_t m_range_units;
   int m_num_range_rings;
   int m_num_radials;
   double m_angle_between_radials;
   boolean_t m_offset_enabled;
   double m_relative_azimuth;
   boolean_t m_clip_tamask;

public:
   BOOL operator !=( const CBullseyeProperties& bp )
   {
      return
         m_bullseye_on != bp.m_bullseye_on
         || fabs( m_range_radius - bp.m_range_radius ) > 1e-6
         || m_range_units != bp.m_range_units
         || m_num_range_rings != bp.m_num_range_rings
         || m_num_radials != bp.m_num_radials
         || fabs( m_angle_between_radials - bp.m_angle_between_radials ) > 1e-6
         || m_offset_enabled != bp.m_offset_enabled
         || fabs( m_relative_azimuth - bp.m_relative_azimuth ) > 1e-6
         || m_clip_tamask != bp.m_clip_tamask;
   }
   void set_bullseye_on(boolean on) { m_bullseye_on = on; }
   boolean_t get_bullseye_on() { return m_bullseye_on; }

   void set_range_radius(double radius) { m_range_radius = radius; }
   double get_range_radius() { return m_range_radius; }

   void set_range_units(units_t units) { m_range_units = units; }
   units_t get_range_units() { return m_range_units; }

   void set_num_range_rings(int n) { m_num_range_rings = n; }
   int get_num_range_rings() { return m_num_range_rings; }

   void set_num_radials(int r) { m_num_radials = r; }
   int get_num_radials() { return m_num_radials; }

   void set_angle_between_radials(int a) { m_angle_between_radials = a; }
   double get_angle_between_radials() { return m_angle_between_radials; }

   void set_offset_enabled(boolean_t enabled) { m_offset_enabled = enabled; }
   boolean_t get_offset_enabled() { return m_offset_enabled; }

   void set_relative_azimuth(double a) { m_relative_azimuth = a; }
   double get_relative_azimuth() 
   { 
      if (m_offset_enabled)
         return m_relative_azimuth; 

      return 0.0;
   }

   void set_clip_tamask(boolean_t clip) { m_clip_tamask = clip; }
   boolean_t get_clip_tamask() { return m_clip_tamask; }

public:
   CBullseyeProperties();

   // initialize the values of this properties object from the registry
   int initialize_from_registry(CString key);

   // store the values of this properties object to the registry
   int store_in_registry(CString key);

   /// <summary>
   /// Returns the size, in bytes, necessary to store the bullseye properties
   /// </summary>
   int get_block_size();

   /// <summary>
   /// Fills in an array of bytes used to store the bullseye properties
   /// </summary>
   void Serialize(BYTE *&block_ptr);

   /// <summary>
   /// Fills in bullseye properties with a given array of bytes
   /// </summary>
   void Deserialize(BYTE *block_ptr);
};

class CPredictivePathProperties
{
   boolean_t m_predictive_path_on;
   int m_predictive_path_icon_color;
   boolean_t m_predictive_path_center_line_on;
   int m_num_trend_dots;
   int m_trend_dot_time[4];
   _bstr_t m_bstrNormalPath3DIconFile;
   _bstr_t m_bstrWarningPath3DIconFile;
   _bstr_t m_bstrCriticalPath3DIconFile;

public:
   void set_predictive_path_on(boolean_t on) { m_predictive_path_on = on; }
   boolean_t get_predictive_path_on() { return m_predictive_path_on; }

   void set_predictive_path_center_line_on(boolean_t on) { m_predictive_path_center_line_on = on; }
   boolean_t get_predictive_path_center_line_on() { return m_predictive_path_center_line_on; }

   void set_predictive_path_icon_color(int color) { m_predictive_path_icon_color = color; }
   int get_predictive_path_icon_color() { return m_predictive_path_icon_color; }

   void set_num_trend_dots(int num) { ASSERT(num >= 1 && num <= 4); m_num_trend_dots = num; }
   int get_num_trend_dots() { return m_num_trend_dots; }

   void set_trend_dot_time(int num, int time) { ASSERT(num >= 0 && num < 4); m_trend_dot_time[num] = time; }
   int get_trend_dot_time(int num) { ASSERT(num >= 0 && num < 4); return m_trend_dot_time[num]; }

   void set_normal_path_3D_icon_file( LPCSTR pszIconFile ){ m_bstrNormalPath3DIconFile = pszIconFile; }
   _bstr_t& get_normal_path_3D_icon_file(){ return m_bstrNormalPath3DIconFile; }

   void set_warning_path_3D_icon_file( LPCSTR pszIconFile ){ m_bstrWarningPath3DIconFile = pszIconFile; }
   _bstr_t& get_warning_path_3D_icon_file(){ return m_bstrWarningPath3DIconFile; }

   void set_critical_path_3D_icon_file( LPCSTR pszIconFile ){ m_bstrCriticalPath3DIconFile = pszIconFile; }
   _bstr_t& get_critical_path_3D_icon_file(){ return m_bstrCriticalPath3DIconFile; }

public:
   CPredictivePathProperties();

   // initialize the values of this properties object from the registry
   int initialize_from_registry(CString key);

   // store the values of this properties object to the registry
   int store_in_registry(CString key);

   /// <summary>
   /// Returns the size, in bytes, necessary to store the predictive path properties
   /// </summary>
   int get_block_size();

   /// <summary>
   /// Fills in an array of bytes used to store the predictive path properties
   /// </summary>
   void Serialize(BYTE *&block_ptr);

   /// <summary>
   /// Fills in predictive path properties with a given array of bytes
   /// </summary>
   void Deserialize(BYTE *block_ptr);
};


class CAuxDataProperties
{
   std::vector<boolean_t> m_values;

public:

   void set_num_values(int i);

   void set_value(int i, boolean_t val) { if(i < (int)m_values.size()) m_values[i] = val; }
   boolean_t get_value(int i) { if(i < (int)m_values.size()) return m_values[i]; }

public:
   CAuxDataProperties();
   ~CAuxDataProperties();

   // initialize the values of this properties object from the registry
   int initialize_from_registry(CString key);

   // store the values of this properties object to the registry
   int store_in_registry(CString key);

   /// <summary>
   /// Returns the size, in bytes, necessary to store the predictive path properties
   /// </summary>
   int get_block_size();

   /// <summary>
   /// Fills in an array of bytes used to store the predictive path properties
   /// </summary>
   void Serialize(BYTE *&block_ptr);

   /// <summary>
   /// Fills in predictive path properties with a given array of bytes
   /// </summary>
   void Deserialize(BYTE *block_ptr);
};

class CGPSProperties
{
public:
   CGPSProperties();

   static int GetNextAutoColor();

private:
   // ship properties      
   CString m_symbol_type;
   int m_ship_fg_color;
   int m_ship_bg_color;
   int m_ship_size;
   int m_ship_line_size;

   CBullseyeProperties m_bullseyeProp;
   CPredictivePathProperties m_predictivePathProp;
   CAuxDataProperties m_auxDataProp;

   // trail icon properties
   gps_icon_t m_trail_icon_type;
   int m_trail_icon_fg_color;
   _bstr_t m_bstrTrail3DIconFile;
   _bstr_t m_bstr3DIconFilePath;

   // trail timeout properties
   boolean_t m_timeout_on;
   int m_timeout_seconds;

   // coast track update rate (second)
   int m_ctrack_update_rate;

   // moving map feed type (NMEA, Predator, etc...)
   int m_feed_type;

   // frozen feed timeout interval (-1 indicates never)
   int m_frozen_feed_timeout;

   boolean_t m_auto_save_trail;
   
   boolean_t m_trail_rewind_enabled;   // Debugging tool, set in registry manually

   boolean_t m_display_altitude_label;
   CString m_nickname_label;
   OvlFont m_nickname_font;

   BOOL m_bAutoColorIncrement;
   static int m_nAutoColor;

   BOOL m_bDisplayNickname;
   BOOL m_bOriginAtNose;
   
public:

   // get/set functions
   void set_symbol_type(CString type) { m_symbol_type = type; }
   CString get_symbol_type() { return m_symbol_type; }
   
   void set_ship_bg_color(int color) { m_ship_bg_color = color; }
   int get_ship_bg_color() { return m_ship_bg_color; }
   
   void set_ship_fg_color(int color) { m_ship_fg_color = color; }
   int get_ship_fg_color() { return m_ship_fg_color; }
   
   void set_ship_size(int size) { m_ship_size = size; }
   int get_ship_size() { return m_ship_size; }
   
   void set_ship_line_size(int size) { m_ship_line_size = size; }
   int get_ship_line_size() { return m_ship_line_size; }

   CBullseyeProperties* GetBullseyeProperties() { return &m_bullseyeProp; }
   CPredictivePathProperties* GetPredictivePathProperties() { return &m_predictivePathProp; }
   CAuxDataProperties* GetAuxDataProperties() { return &m_auxDataProp; }
   void set_trail_icon_type(gps_icon_t type) { m_trail_icon_type = type; }
   gps_icon_t get_trail_icon_type() { return m_trail_icon_type; }
   
   void set_trail_icon_fg_color(int color) { m_trail_icon_fg_color = color; }
   int get_trail_icon_fg_color() { return m_trail_icon_fg_color; }

   void set_timeout_on(boolean_t on) { m_timeout_on = on; }
   boolean_t get_timeout_on() { return m_timeout_on; }

   void set_timeout_seconds(int seconds) { m_timeout_seconds = seconds; }
   int get_timeout_seconds() { return m_timeout_seconds; }

   void set_ctrack_update_rate(int rate) { m_ctrack_update_rate = rate; }
   int get_ctrack_update_rate() { return m_ctrack_update_rate; }

   void set_feed_type(int f) { m_feed_type = f; }
   int get_feed_type() { return m_feed_type; }

   void set_frozen_feed_timeout(int t) { m_frozen_feed_timeout = t; }
   int get_frozen_feed_timeout() { return m_frozen_feed_timeout; }

   void set_auto_save_trail(boolean_t a) { m_auto_save_trail = a; }
   boolean_t get_auto_save_trail() { return m_auto_save_trail; }

   void set_display_altitude_label(boolean_t d) { m_display_altitude_label = d; }
   boolean_t get_display_altitude_label() { return m_display_altitude_label; }

   void set_nickname_label(CString s) { m_nickname_label = s; }
   CString get_nickname_label() { return m_nickname_label; }

   OvlFont &get_nickname_font() { return m_nickname_font; }
   void set_nickname_font(OvlFont font) { m_nickname_font = font; }

   BOOL GetAutoColorIncrement() { return m_bAutoColorIncrement; }
   void SetAutoColorIncrement(BOOL bAutoColor) { m_bAutoColorIncrement = bAutoColor; }

   BOOL GetDisplayNickname() { return m_bDisplayNickname; }
   void SetDisplayNickname(BOOL bDisplayNickname) { m_bDisplayNickname = bDisplayNickname; }

   BOOL GetOriginAtNose() { return m_bOriginAtNose; }
   void SetOriginAtNose(BOOL bOriginAtNose) { m_bOriginAtNose = bOriginAtNose; }

   boolean_t get_trail_rewind_enabled() const { return  m_trail_rewind_enabled; }

   void set_trail_3D_icon_file( LPCSTR pszIconFile ){ m_bstrTrail3DIconFile = pszIconFile; }
   const _bstr_t& get_trail_3D_icon_file() const { return m_bstrTrail3DIconFile; }
   
   const _bstr_t& get_3D_icon_file_path() const { return m_bstr3DIconFilePath; }


public:
   // initialize the values of this properties object from the registry
   int initialize_from_registry(CString key);

   // store the values of this properties object to the registry
   int store_in_registry(CString key);

   // return the number of bytes required to store the properties
   int get_block_size();

   int Serialize(BYTE *&block);
   int deserialize(BYTE *block);
};

/////////////////////////////////////////////////////////////////////////////
// CGpsStyle dialog

void GetColor( COLORREF &color, CString title);

class CGpsStyle : 
   public COverlayPropertyPage, 
   public IFontDialogCallback
{
   DECLARE_DYNCREATE(CGpsStyle)

// Construction
public:
   // standard constructor
   CGpsStyle(CGPSProperties* pProperties = NULL, C_gps_trail *trail = NULL);      
   ~CGpsStyle();

   void store_values();
   
   COLORREF m_background;
   COLORREF m_foreground;
   COLORREF m_icon_fg;

// Dialog Data
   //{{AFX_DATA(CGpsStyle)
   enum { IDD = IDD_GPS_STYLE };
   CButton m_auto_save;
   CComboBox   m_frozen_feed;
   CButton m_altitude_on;
   CButton  m_timeout_on_ctrl;
   CEdit m_timeout_edit_ctrl;
   CSliderCtrl m_sliderShipC;
   CSliderCtrl m_sliderLineC;
   CFVWColorButton   m_backgroundC;
   CFVWColorButton   m_foregroundC;
   CFVWColorButton   m_icon_fgC;
   CComboBox   m_shipTypeC;
   UINT  m_lineSize;
   UINT  m_shipSize;
   CComboBox m_gpsfeed;
   CComboBox m_icon_type;
   int      m_timeout_seconds;
   BOOL  m_timeout_on;
   CString m_nickname;
   CButton m_ckAutoIncrement;
   CButton m_ckDisplayNick;
   CButton m_ckOriginAtNose;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGpsStyle)
   public:
   virtual BOOL OnApply();
   virtual void OnOK();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
public:
   // Generated message map functions
   //{{AFX_MSG(CGpsStyle)
   afx_msg void OnGpsStyleColorBackground();
   afx_msg void OnGpsStyleColorForeground();
   afx_msg void OnIconFgColor();
   virtual BOOL OnInitDialog();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnSelchangeGPSFeed();
   afx_msg void OnModified();
   afx_msg void OnUnitsModified();
   afx_msg void OnFont();
   afx_msg void OnApplyClicked();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

protected:
   CGPSProperties *m_prop;
   C_gps_trail *m_p_trail;
   boolean_t m_modified;

   // IFontDialogCallback
public:
   void OnApplyFont(CFontDlg* pFontDialog);
   void OnFontModified();
};

class CMovingMapGeneralPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() 
   { 
      return new CGpsStyle(CMovingMapRootPropertyPage::GetProperties()); 
   }
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPSSTYLE_H__B4BC16A9_4C12_11D2_816E_00609733A64E__INCLUDED_)
