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



// minlpset.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMinLocalPointSet DAO recordset

class CMinLocalPointSet : public CDaoRecordset
{
public:
   CMinLocalPointSet(CDaoDatabase* pDatabase = NULL);
   DECLARE_DYNAMIC(CMinLocalPointSet)

// Field/Param Data
   //{{AFX_FIELD(CMinLocalPointSet, CDaoRecordset)
   CString m_ID;
   CString m_Description;
   double m_Latitude;
   double m_Longitude;
   CString m_Icon_Name;
   CString m_Group_Name;
   //}}AFX_FIELD

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMinLocalPointSet)
   public:
   virtual CString GetDefaultDBName();  // Default database name
   virtual CString GetDefaultSQL();  // Default SQL for Recordset
   virtual void DoFieldExchange(CDaoFieldExchange* pFX);  // RFX support
   //}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif
};
