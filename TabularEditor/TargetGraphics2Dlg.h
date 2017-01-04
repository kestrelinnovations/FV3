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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TargetGraphics2Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphics2Dlg dialog

#include "TargetGraphicsDlgBase.h"

#include "..\ITargetGraphics2.h"


class CTargetGraphics2Dlg : public CTargetGraphicsDlgBase
{
   // Construction
public:
   CTargetGraphics2Dlg(CWnd* pParent = NULL);   // standard constructor
   ~CTargetGraphics2Dlg();

   void set_selected_list(CList<feature_t, feature_t> *list) { m_selected_list = list; }


   // Implementation
protected:

   virtual void handle_get_position_from_map();
   virtual int selected_list_count();

   virtual void initialize_geobounds_controls();
   virtual void enable_clipboard_format();

   virtual void get_point_info(UINT current_page_number, CString& point_name, CString& point_description, 
                    CString& coord_string1, CString& coord_string2,
                    CString& coord_string3, CString& coord_string4,
                    CString& comment);

   virtual void get_points(std::vector<d_geo_t>& points);

   virtual void get_items(std::vector<ITEM>& items);

   CList<feature_t, feature_t> *m_selected_list;

private:
   CString get_id(feature_t f);
   CString get_comment(feature_t f);
   CString get_description(feature_t f);
   double get_lat(feature_t f);
   double get_lon(feature_t f);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.