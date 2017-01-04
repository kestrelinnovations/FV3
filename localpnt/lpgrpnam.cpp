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



// lpgrpnam.cpp : implementation file
//

#include "stdafx.h"
#include "lpgrpnam.h"
#include "..\fvw.h"

CString CLocalPointGroupNameSet::m_filename = "";

/////////////////////////////////////////////////////////////////////////////
// CLocalPointGroupNameSet

IMPLEMENT_DYNAMIC(CLocalPointGroupNameSet, CDaoRecordset)

CLocalPointGroupNameSet::CLocalPointGroupNameSet(CDaoDatabase* pdb)
   : CDaoRecordset(pdb)
{
   //{{AFX_FIELD_INIT(CLocalPointGroupNameSet)
   m_Group_Name = _T("");
   m_Search = FALSE;
   m_nFields = 2;
   //}}AFX_FIELD_INIT
   m_nDefaultType = dbOpenDynaset;
}

void CLocalPointGroupNameSet::set_filename(CString filename)
{
   m_filename = filename;
}

CString CLocalPointGroupNameSet::GetDefaultDBName()
{
   return m_filename;
}

CString CLocalPointGroupNameSet::GetDefaultSQL()
{
   return _T("[Group_Names]");
}

void CLocalPointGroupNameSet::DoFieldExchange(CDaoFieldExchange* pFX)
{
   //{{AFX_FIELD_MAP(CLocalPointGroupNameSet)
   pFX->SetFieldType(CDaoFieldExchange::outputColumn);
   DFX_Text(pFX, _T("[Group_Name]"), m_Group_Name);
   DFX_Bool(pFX, _T("[Search]"), m_Search);
   //}}AFX_FIELD_MAP

   // This code was added to modify the DAO binding structure for each field that
   // is a text field so that the new callback function is called instead of the 
   // MFC function.  This fix was required to correct a character buffer overwrite 
   // error introduced by the upgrade to DAO 3.6
   if (pFX->m_nOperation == CDaoFieldExchange::BindField)
   {
      for (int i = 0; i < this->m_nFields; i++)
      {
         LPDAOCOLUMNBINDING pcb = &m_prgDaoColBindInfo[i];
         if (pcb->dwDataType == DAO_CHAR)
         {
            pcb->cbDataOffset = (DWORD)MyDaoStringAllocCallback;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CLocalPointGroupNameSet diagnostics

#ifdef _DEBUG
void CLocalPointGroupNameSet::AssertValid() const
{
   CDaoRecordset::AssertValid();
}

void CLocalPointGroupNameSet::Dump(CDumpContext& dc) const
{
   CDaoRecordset::Dump(dc);
}
#endif //_DEBUG
