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




#if !defined(AFX_TARGETGRAPHICS_H__9B5601A3_4289_4444_9D2B_3D6CA3837717__INCLUDED_)
#define AFX_TARGETGRAPHICS_H__9B5601A3_4289_4444_9D2B_3D6CA3837717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TargetGraphicsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetGraphicsDlg dialog

#include "TargetGraphicsDlgBase.h"

#include "..\ITargetGraphics.h"

class CTargetGraphicsDlg : public CTargetGraphicsDlgBase
{
   // Construction
public:
   CTargetGraphicsDlg(CWnd* pParent = NULL);   // standard constructor
   ~CTargetGraphicsDlg();

   void set_selected_list(CList<target_t, target_t> *list) { m_selected_list = list; }
  
   // Implementation
protected:
   virtual void initialize_geobounds_controls();
   virtual void enable_clipboard_format();

   virtual void get_point_info(UINT current_page_number, CString& point_name, CString& point_description, 
                    CString& coord_string1, CString& coord_string2,
                    CString& coord_string3, CString& coord_string4,
                    CString& comment);
  
   virtual void get_points(std::vector<d_geo_t>& points);

   virtual void get_items(std::vector<ITEM>& items);

   virtual void handle_get_position_from_map();

   virtual int selected_list_count();
   CList<target_t, target_t> *m_selected_list;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGETGRAPHICS_H__9B5601A3_4289_4444_9D2B_3D6CA3837717__INCLUDED_)