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




// GoToPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "GoToPropSheet.h"
#include "MapGoToPropPage.h"
#include "ShapeGoToPropPage.h"
#include "ShapeGoToPropPageDisabled.h"
#include "DrawingGoToPropPage.h"
#include "shp\factory.h"
#include "overlay.h"
#include "param.h"


/////////////////////////////////////////////////////////////////////////////
// CGoToPropSheet

IMPLEMENT_DYNAMIC(CGoToPropSheet, CPropertySheet)

CGoToPropSheet::CGoToPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
   : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init();
}

CGoToPropSheet::CGoToPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
   : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init();
}

void CGoToPropSheet::Init()
{
   // obtain a list of overlays from the overlay manager
   CList <C_overlay *, C_overlay *> overlay_lst;
   OVL_get_overlay_manager()->get_overlay_list(overlay_lst);

   // determine if any Shape files are currently open
   BOOL shape_file_currently_open = FALSE;
   POSITION position = overlay_lst.GetHeadPosition();
   while (position)
   {
      C_overlay *overlay = overlay_lst.GetNext(position);
      if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_ShapeFile)
      {
         shape_file_currently_open = TRUE;
         break;
      }
   }

   // determine if any Drawing files are currently open
   BOOL drawing_file_currently_open = FALSE;
   position = overlay_lst.GetHeadPosition();
   while (position)
   {
      C_overlay *overlay = overlay_lst.GetNext(position);
      if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_Drawing)
      {
         drawing_file_currently_open = TRUE;
         break;
      }
   }

   m_mapGoToPP = new CMapGoToPropPage();
   AddPage(m_mapGoToPP);

   if (shape_file_currently_open)
   {
      m_shapeGoToPP = new CShapeGoToPropPage;
      m_shapeGoToPP_disabled = NULL;
      AddPage(m_shapeGoToPP);
   }
   else
   {
      m_shapeGoToPP = NULL;
      m_shapeGoToPP_disabled = new CShapeGoToPropPageDisabled;
      AddPage(m_shapeGoToPP_disabled);
   }

   // only create the GoTo "Drawing Files" Tab property sheet if Drawing Editor
   // exists and is enabled in the overlay configuration file
   if (OVL_get_type_descriptor_list()->IsOverlayEnabled(FVWID_Overlay_Drawing))
   {
      if (drawing_file_currently_open)
      {
         m_drawingGoToPP = new CDrawingGoToPropPage;
         m_drawingGoToPP_disabled = NULL;
         AddPage(m_drawingGoToPP);
      }
      else
      {
         m_drawingGoToPP = NULL;
         m_drawingGoToPP_disabled = new CDrawingGoToPropPageDisabled;
         AddPage(m_drawingGoToPP_disabled);
      }
   }
   else
   {
      m_drawingGoToPP_disabled = NULL;
      m_drawingGoToPP = NULL;
   }

}

CGoToPropSheet::~CGoToPropSheet()
{
   delete m_mapGoToPP;
   delete m_shapeGoToPP;
   delete m_shapeGoToPP_disabled;
   delete m_drawingGoToPP;
   delete m_drawingGoToPP_disabled;
}


BEGIN_MESSAGE_MAP(CGoToPropSheet, CPropertySheet)
   //{{AFX_MSG_MAP(CGoToPropSheet)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CGoToPropSheet message handlers

BOOL CGoToPropSheet::OnInitDialog() 
{
   return CPropertySheet::OnInitDialog();
}

void CGoToPropSheet::OnClose() 
{
   CPropertySheet::OnClose();
}
