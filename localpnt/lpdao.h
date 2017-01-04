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



// lpdao.h : interface of the CLocalPointSet class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef LPDAO_H
#define LPDAO_H 1

#define NUM_FIELDS_IN_POINTS_TABLE 18  // This is the number of fields in the current version of
                                          // the Local Points db's "Points" table.


//forward
class C_localpnt_point;


/////////////////////////////////////////////////////////////////////////////

class CLocalPointSet : public CDaoRecordset
{
public:
   CLocalPointSet(CDaoDatabase* pDatabase = NULL);
   DECLARE_DYNAMIC(CLocalPointSet)

// Field/Param Data
   //{{AFX_FIELD(CLocalPointSet, CDaoRecordset)
   CString m_ID;
   CString m_Description;
   double m_Latitude;
   double m_Longitude;
   short m_Elevation;
   CString m_Area;
   CString m_Country_Code;
   CString m_Dtd_Id;
   float m_Horz_Accuracy;
   float m_Vert_Accuracy;
   CString m_Link_Name;
   CString m_Icon_Name;
   CString m_Comment;
   CString m_Pt_Quality;
   CString m_Elevation_Source;
   CString m_Elevation_Source_Map_Handler_Name;
   CString m_Elevation_Source_Map_Series_Name;
   CString m_Group_Name;
   //}}AFX_FIELD

   static CString m_filename;
   static void set_filename(CString filename);
   const CLocalPointSet& operator=(C_localpnt_point &point);


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CLocalPointSet)
   public:
   virtual CString GetDefaultDBName();  // REVIEW:  Get a comment here
   virtual CString GetDefaultSQL();   // default SQL for Recordset
   virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
   //}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

};

#endif  // ifndef LPDAO_H