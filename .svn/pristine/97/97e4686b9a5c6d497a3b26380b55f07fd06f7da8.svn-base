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



// minlpset.cpp : implementation file
//

#include "stdafx.h"
#include "minlpset.h"
#include "..\fvw.h"

/////////////////////////////////////////////////////////////////////////////
// CMinLocalPointSet

IMPLEMENT_DYNAMIC(CMinLocalPointSet, CDaoRecordset)

CMinLocalPointSet::CMinLocalPointSet(CDaoDatabase* pdb)
   : CDaoRecordset(pdb)
{
   //{{AFX_FIELD_INIT(CMinLocalPointSet)
   m_ID = _T("");
   m_Description = _T("");
   m_Latitude = 0.0;
   m_Longitude = 0.0;
   m_Icon_Name = _T("");
   m_Group_Name = _T("");
   m_nFields = 6;
   //}}AFX_FIELD_INIT
   m_nDefaultType = dbOpenSnapshot;
   m_bCheckCacheForDirtyFields = FALSE;
}


CString CMinLocalPointSet::GetDefaultDBName()
{
   return _T("");
}

CString CMinLocalPointSet::GetDefaultSQL()
{
   return _T("[FV_Load]");
}

void CMinLocalPointSet::DoFieldExchange(CDaoFieldExchange* pFX)
{
   //{{AFX_FIELD_MAP(CMinLocalPointSet)
   pFX->SetFieldType(CDaoFieldExchange::outputColumn);
   DFX_Text(pFX, _T("[ID]"), m_ID);
   DFX_Text(pFX, _T("[Description]"), m_Description);
   DFX_Double(pFX, _T("[Latitude]"), m_Latitude);
   DFX_Double(pFX, _T("[Longitude]"), m_Longitude);
   DFX_Text(pFX, _T("[Icon_Name]"), m_Icon_Name);
   DFX_Text(pFX, _T("[Group_Name]"), m_Group_Name);
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
// CMinLocalPointSet diagnostics

#ifdef _DEBUG
void CMinLocalPointSet::AssertValid() const
{
   CDaoRecordset::AssertValid();
}

void CMinLocalPointSet::Dump(CDumpContext& dc) const
{
   CDaoRecordset::Dump(dc);
}
#endif //_DEBUG
