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




#if !defined(AFX_TARGETGRAPHICSBASE_H__9B5601A3_4289_4444_9D2B_3D6CA3837717__INCLUDED_)
#define AFX_TARGETGRAPHICSBASE_H__9B5601A3_4289_4444_9D2B_3D6CA3837717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TargetGraphicsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphicsDlg dialog

#include "FalconView/include/Maps.h"          // for page_map_list_t
#include "FVGridCtrl.h"  // for selected_item_t
#include "FalconView/include/maps_d.h"   // for MapSource, MapScale, MapSeries
#include "..\ZoomPercentComboBox.h" // for ZoomComboBox
#include "..\resource.h"
#include "..\MovingMapOverlay\ctrakdlg.h" // for CMappedBitmapButton
#include "FalconView/include/geocx1.h"

// forward declarations
class MapTypeSelection;
class MapTypeSelectionTree;
class MapType;
class MapGroup;
class MapEngineCOM;

enum format_t { 
   FORMAT_HTML, 
   FORMAT_JPG, 
   FORMAT_GEOTIFF, 
   FORMAT_PNG, 
   FORMAT_PRINTER, 
   FORMAT_CLIPBOARD 
};

enum output_size_type_t {
   OUTPUT_SIZE_PIXELS,
   OUTPUT_SIZE_GEOBOUNDS
};

typedef struct
{
   double lat;
   double lon;
   CString id;
   CString links;
   CString description;
   CString comment;
} ITEM;


CString get_map_type_string(MapType *map_type);
void MakeValidFilename(CString &strFilename);

class CTargetGraphicsDlgBase : public CDialog
{
   // Construction
public:
   CTargetGraphicsDlgBase(CWnd* pParent = NULL);   // standard constructor
   ~CTargetGraphicsDlgBase();

   void set_initial_format(format_t format) { m_initialFormat = format; }
   void set_initial_output_size_type(output_size_type_t output_size_type) { m_initialOutputSizeType = output_size_type; }
   void set_initial_geo_bounds(double nw_lat, double nw_lon, double se_lat, double se_lon)
   {
      m_nw_lat = nw_lat;
      m_nw_lon = nw_lon;
      m_se_lat = se_lat;
      m_se_lon = se_lon;
      m_geo_bounds_valid = TRUE;
   }

   int m_scale_percent;

   static int add_map_projections_to_list(page_map_list_t *list);
   int add_map_projections_to_list_hlpr(page_map_list_t *list);
   int add_map_projections_to_list_hlpr(page_map_list_t *page_list, MapGroup *map_group);
   static void draw_page_text(CDC *dc, UINT current_page_number);
   void draw_page_text_hlpr(CDC *dc, UINT current_page_number);
   void output_graphics(MapGroup *map_group, CString &info_text,
      COleDateTime &current_time, CString& package_name);

   static void on_end_printing();
   static BOOL is_active() { return s_dlg_ptr != NULL; }

   static CTargetGraphicsDlgBase *s_dlg_ptr;

   CList<MapEngineCOM*, MapEngineCOM*> m_delete_list;

   // Dialog Data
   //{{AFX_DATA(CTargetGraphicsDlg)
   enum { IDD = IDD_TAB_TARGET_GRAPHICS };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Target_Graphics_Printing_01.htm";}

   CButton m_publish;
   CButton m_embedImages;
   CTreeCtrl m_tree_ctrl;
   CProgressCtrl m_progress_ctrl;
   CComboBox m_jpeg_quality_combo;
   ZoomComboBox m_ctrlZoom;
   CString m_effective_scale;
   CString m_output_path;
   int  m_image_width;
   int  m_image_height;
   CButton m_format_html;
   CButton m_format_jpg;
   CButton m_format_geotiff;
   CButton m_format_png;
   CButton m_format_printer;
   CButton m_format_clipboard;
   CMappedBitmapButton m_bitmap_button;
   CButton m_output_size_pixels;
   CButton m_output_size_geobounds;
   CGeocx1 m_geoctrl1;
   CGeocx1 m_geoctrl2;
   //}}AFX_DATA


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CTargetGraphicsDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

   // Implementation
protected:
   virtual void initialize_geobounds_controls() = 0;
   virtual void enable_clipboard_format() = 0;
   virtual void handle_get_position_from_map() = 0;

   // Generated message map functions
   //{{AFX_MSG(CTargetGraphicsDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnZoomModified();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnModified();
   afx_msg void OnBrowsePath();
   afx_msg void OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg LONG OnTreeClicked(UINT, LONG);
   afx_msg void OnPrintPreview();
   afx_msg void OnUnselectAll();
   afx_msg void OnOutputSizeChanged();
   afx_msg void OnGetPositionFromMap();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
   
   MapTypeSelectionTree *m_map_type_tree;
   int m_jpeg_quality;

   format_t m_format, m_initialFormat;
   output_size_type_t m_initialOutputSizeType;
   BOOL m_geo_bounds_valid;
   double m_nw_lat, m_nw_lon, m_se_lat, m_se_lon;

   void update_effective_scale();
   void retrieve_scale_percent();
   void get_rxpf_info(MapType *map_type, double lat, double lon);
   CString get_rxpf_info_text(BOOL use_HTML);
   CString get_center_coordinates_table(double lat, double lon);
   void get_center_coordinate_strings(double lat, double lon, CString &str1, CString &str2,
      CString &str3, CString &str4);

   CString get_overlay_list(const CString& package_name);
   void create_HTML_index();

   virtual void get_point_info(UINT current_page_number, CString& point_name, 
      CString& point_description,  CString& coord_string1, CString& coord_string2, 
      CString& coord_string3, CString& coord_string4, CString& comment) = 0;

   virtual void get_points(std::vector<d_geo_t>& points) = 0;
   virtual void get_items(std::vector<ITEM>& items) = 0;

   virtual int selected_list_count() = 0;
     
   
   CMap<MapGroup *, MapGroup *, int, int> m_zoom_percent_map;
   MapGroup *m_current_map_group;

   void SaveDialogStateInRegistry();
};

class HTMLEntry
{
public:
   CString m_name;
   CString m_output_path;

   bool operator<(const HTMLEntry &p) const;

   CString extract_point_name() const;
   CString extract_scale() const;
   CString extract_description();
   MapScale get_map_scale() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGETGRAPHICSBASE_H__9B5601A3_4289_4444_9D2B_3D6CA3837717__INCLUDED_)