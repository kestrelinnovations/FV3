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



// lpdao.cpp : implementation of the CLocalPointSet class
//

#include "stdafx.h"
#include "localpnt.h"
#include "..\fvw.h"

#include "FalconView/localpnt/lpdao.h"
#include "FalconView/localpnt/lpicon.h"

CString CLocalPointSet::m_filename = "";

/////////////////////////////////////////////////////////////////////////////
// CLocalPointSet implementation

IMPLEMENT_DYNAMIC(CLocalPointSet, CDaoRecordset)

CLocalPointSet::CLocalPointSet(CDaoDatabase* pdb)
   : CDaoRecordset(pdb)
{
   //{{AFX_FIELD_INIT(CLocalPointSet)
   m_ID = _T("");
   m_Description = _T("");
   m_Latitude = 0.0;
   m_Longitude = 0.0;
   m_Elevation = 0;
   m_Area = _T("");
   m_Country_Code = _T("");
   m_Dtd_Id = _T("");
   m_Horz_Accuracy = 0.0f;
   m_Vert_Accuracy = 0.0f;
   m_Link_Name = _T("");
   m_Icon_Name = _T("");
   m_Comment = _T("");
   m_Pt_Quality = _T("");
   m_Elevation_Source = _T("");
   m_Elevation_Source_Map_Handler_Name = _T("");
   m_Elevation_Source_Map_Series_Name = _T("");
   m_Group_Name = _T("");


   // get the number of fields (columns) in the current file db's "Points" table...
   if (pdb != NULL)
   {
      CDaoTableDef tableDef(pdb);
      try
      {
         tableDef.Open("Points");
         m_nFields = (int)tableDef.GetFieldCount();
      }
      catch (CDaoException *e)
      {
         CString message = _T("Couldn't get field count -- Exception: ");
         if (e->m_pErrorInfo)                            // use exception data too
            message += e->m_pErrorInfo->m_strDescription;

         AfxMessageBox(message);                         // complain to the user
      }
   }
   else
      m_nFields = NUM_FIELDS_IN_POINTS_TABLE;  // default to the defined value


/*
   if (bIncludeAdditionalElevationFields)
      m_nFields = 18;  // add 2 to accommodate the two additional elevation fields
   else
      m_nFields = 16;
*/


   //}}AFX_FIELD_INIT
   m_nDefaultType = dbOpenDynaset;

}

// ********************************************************************


CString CLocalPointSet::GetDefaultDBName()
{
   return m_filename;
}

// ********************************************************************

void CLocalPointSet::set_filename(CString filename)
{
   m_filename = filename;
}

// ********************************************************************

CString CLocalPointSet::GetDefaultSQL()
{
   return _T("[Points]");
}

// ********************************************************************

void CLocalPointSet::DoFieldExchange(CDaoFieldExchange* pFX)
{
   //{{AFX_FIELD_MAP(CLocalPointSet)
   pFX->SetFieldType(CDaoFieldExchange::outputColumn);
   DFX_Text(pFX, _T("[ID]"), m_ID);
   DFX_Text(pFX, _T("[Description]"), m_Description);
   DFX_Double(pFX, _T("[Latitude]"), m_Latitude);
   DFX_Double(pFX, _T("[Longitude]"), m_Longitude);
   DFX_Short(pFX, _T("[Elevation]"), m_Elevation);
   DFX_Text(pFX, _T("[Elevation_Source]"), m_Elevation_Source);
   DFX_Text(pFX, _T("[Pt_Quality]"), m_Pt_Quality);
   DFX_Text(pFX, _T("[Area]"), m_Area);
   DFX_Text(pFX, _T("[Country_Code]"), m_Country_Code);
   DFX_Text(pFX, _T("[Dtd_Id]"), m_Dtd_Id);
   DFX_Single(pFX, _T("[Horz_Accuracy]"), m_Horz_Accuracy);
   DFX_Single(pFX, _T("[Vert_Accuracy]"), m_Vert_Accuracy);
   DFX_Text(pFX, _T("[Link_Name]"), m_Link_Name);
   DFX_Text(pFX, _T("[Icon_Name]"), m_Icon_Name);
   DFX_Text(pFX, _T("[Comment]"), m_Comment);
   DFX_Text(pFX, _T("[Group_Name]"), m_Group_Name);

   // if this is the current version of the Local Points db...
   if (m_nFields == NUM_FIELDS_IN_POINTS_TABLE)
   {
      DFX_Text(pFX, _T("[ElevationSourceMapHandlerName]"), m_Elevation_Source_Map_Handler_Name);
      DFX_Text(pFX, _T("[ElevationSourceMapSeriesName]"), m_Elevation_Source_Map_Series_Name);
   }
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
// CLocalPointSet diagnostics

#ifdef _DEBUG
void CLocalPointSet::AssertValid() const
{
   CDaoRecordset::AssertValid();
}

void CLocalPointSet::Dump(CDumpContext& dc) const
{
   CDaoRecordset::Dump(dc);
}
#endif //_DEBUG

// End CLocalPointSet diagnostics

// --------------------------------------------------------------------------

const CLocalPointSet& CLocalPointSet::operator=(C_localpnt_point &point)
{

   m_ID = point.m_id;
   m_Dtd_Id = point.m_dtd_id;
   m_Group_Name = point.m_group_name;
   m_Latitude = point.m_latitude;
   m_Longitude = point.m_longitude;
   m_Description = point.m_description;
   m_Icon_Name = point.GetIconName();
   m_Comment = point.m_comment;
   
   return *this;
}
