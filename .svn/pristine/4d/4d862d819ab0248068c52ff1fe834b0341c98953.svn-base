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

#if !defined(AFX_IPTEXPRT_H__4F622773_9736_4A71_9A50_6BC9DA416D1A__INCLUDED_)
#define AFX_IPTEXPRT_H__4F622773_9736_4A71_9A50_6BC9DA416D1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IPtExprt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PointExport command target

// connection points interface
// {181FD91D-034E-4c90-B71E-13FB09234BD8}
#import "fvw.tlb" named_guids

// PointExportDispatchContainer - used to hold dispatch pointers
// from multiple clients.  Can be used to retrieve a list of dispatch
// pointers
class PointExportDispatchContainer
{
   CList <IDispatch FAR*, IDispatch FAR*> m_dispatch_lst;

public:
   POSITION get_head() { return m_dispatch_lst.GetHeadPosition(); }
   IDispatch FAR *get_next(POSITION &position) { return m_dispatch_lst.GetNext(position); }
   void add(IDispatch FAR* disp) { m_dispatch_lst.AddTail(disp); }
   void remove(IDispatch FAR* disp)
   {
      POSITION position = m_dispatch_lst.Find(disp);
      if (position)
         m_dispatch_lst.RemoveAt(position);
   }
};

// declared in IPtExprt.cpp
extern PointExportDispatchContainer g_point_export_dispatch_lst;

class PointExport : public CCmdTarget
{
   DECLARE_DYNCREATE(PointExport)
   DECLARE_OLECREATE(PointExport)

   PointExport();           // protected constructor used by dynamic creation

// member variables
private:
   // dispatch pointer to the client's callback object
   IDispatch FAR* m_pdisp;
   VARIANT m_pdisp_var;

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(PointExport)
   public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL

// Implementation
protected:
   virtual ~PointExport();

   // Generated message map functions
   //{{AFX_MSG(PointExport)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
   // Generated OLE dispatch map functions
   //{{AFX_DISPATCH(PointExport)
   afx_msg long Register(const VARIANT FAR& dispatch_pointer);
   afx_msg long AddRectangleRegion(long layer_handle, double nw_lat, double nw_lon, double se_lat, double se_lon);
   afx_msg long AddCircleRegion(long layer_handle, double center_lat, double center_lon, double radius);
   afx_msg long MoveRegion(long layer_handle, long object_handle, double offset_lat, double offset_lon);
   afx_msg long DeleteRegion(long layer_handle, long object_handle);
   afx_msg long ChangeRegionColor(long layer_handle, long object_handle, long color);
   afx_msg long ChangeRegionLineWidth(long layer_handle, long object_handle, long width);
   afx_msg long DoExport(long layer_handle, LPCSTR file_spec);
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()

   BEGIN_CONNECTION_PART(PointExport, ExportRegionChangeConnPt)
   CONNECTION_IID(fvw::IID_IExportRegionChangeNotify)
      virtual int GetMaxConnections();
   END_CONNECTION_PART(ExportRegionChangeConnPt)
   
   DECLARE_CONNECTION_MAP()

   // Iterate through the connection points' list and invoke the notify
   void FireExportRegionChange(long object_handle);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPTEXPRT_H__4F622773_9736_4A71_9A50_6BC9DA416D1A__INCLUDED_)
