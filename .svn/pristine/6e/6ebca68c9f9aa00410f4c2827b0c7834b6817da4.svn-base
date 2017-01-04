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



// lpgrpnam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalPointGroupNameSet DAO recordset

class CLocalPointGroupNameSet : public CDaoRecordset
{
public:
   CLocalPointGroupNameSet(CDaoDatabase* pDatabase = NULL);
   static void set_filename(CString filename);
   static CString m_filename;

   DECLARE_DYNAMIC(CLocalPointGroupNameSet)
// Field/Param Data
   //{{AFX_FIELD(CLocalPointGroupNameSet, CDaoRecordset)
   CString m_Group_Name;
   BOOL m_Search;
   //}}AFX_FIELD

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CLocalPointGroupNameSet)
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
private:
};
