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



// lpeddlgc.cpp : implementation file
//

#include "stdafx.h"
#include "lpeddlg.h"
#include "localpnt.h"
#include "FalconView/localpnt/lpeddlgc.h"
#include "FalconView/localpnt/lpicon.h"

//-----------------------------------------------------------------------------

CLocalpntEditDlgControl::CLocalpntEditDlgControl()
{
   m_dialog=NULL;
}

//-----------------------------------------------------------------------------

CLocalpntEditDlgControl::~CLocalpntEditDlgControl()
{
   ASSERT(!m_dialog);  //m_dialog should have been deleted 
                       //(by calling DestroyWindow) earlier

   //delete dialog if it has not been deleted
   if (m_dialog)
      delete m_dialog;
}

//-----------------------------------------------------------------------------

int CLocalpntEditDlgControl::Create()
{
   int result = SUCCESS;
   if (!m_dialog)
   {
      m_dialog = new CLocalPtEditDlg;
      if (!m_dialog->Create(CLocalPtEditDlg::IDD))
         result = FAILURE;
   }

   return result;
}

//-----------------------------------------------------------------------------

int CLocalpntEditDlgControl::SetFocus(const char* id, 
                                      const char* group_name, 
                                      const char* filespec, 
                                      BOOL apply_changes /*=TRUE*/, 
                                      BOOL redraw /* =TRUE */)
{
   if (m_dialog)
      m_dialog->set_edit_focus(id, group_name, filespec, apply_changes, redraw);

   return SUCCESS; //TO DO: Kevin: meaningful return code or none
}

//-----------------------------------------------------------------------------

int CLocalpntEditDlgControl::SetFocus(C_localpnt_point *point, 
                                      BOOL apply_changes /*=TRUE*/, 
                                      BOOL redraw /* =TRUE */ )
{
   if (m_dialog)
   {
      if (point)
      {
         ASSERT(point->m_overlay);
         if (point->m_overlay)  
         {
            CString currentFileSpec;
            OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(point->m_overlay);
            if (pFvOverlayPersistence != NULL)
            {
               _bstr_t fileSpecification;
               pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
               currentFileSpec = (char *)fileSpecification;
            }

            m_dialog->set_edit_focus(point->m_id, point->m_group_name, currentFileSpec, apply_changes, redraw);
         }
      }
      else
         m_dialog->set_edit_focus("", "", "", apply_changes, redraw);
   }

   return SUCCESS; //TO DO: kevin: meaningful return code or none
}

//-----------------------------------------------------------------------------

BOOL CLocalpntEditDlgControl::DialogExists()
{
   return (m_dialog!=NULL);
}

//-----------------------------------------------------------------------------

BOOL CLocalpntEditDlgControl::ShowDialog()
{
   ASSERT(m_dialog);
    
   if (m_dialog)
   {
      return m_dialog->ShowWindow(SW_SHOW);
   }
   else
      return FALSE;
}

//-----------------------------------------------------------------------------

int CLocalpntEditDlgControl::DestroyDialog()
{
   //TO DO: kevin: make sure this asks to save changes

   ASSERT(m_dialog);

   int result = SUCCESS;
   if (m_dialog)
   {
      if (!m_dialog->DestroyWindow())
         result = FAILURE;
   }
   
   return result;
}

//-----------------------------------------------------------------------------

int CLocalpntEditDlgControl::ApplyChanges(BOOL redraw)
{
   ASSERT(m_dialog);

   int result;

   if (m_dialog)
      result = m_dialog->apply_changes(redraw);
   else
      result = SUCCESS;
   
   return result;
   
}
