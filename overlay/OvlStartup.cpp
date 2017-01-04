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



// OvlStartup.cpp : implementation file
//

#include "stdafx.h"
#include "..\resource.h"
#include "OvlStartup.h"
#include "overlay.h"
#include "param.h"
#include "OvlFctry.h"
#include "FvCore\Include\GuidStringConverter.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"


/////////////////////////////////////////////////////////////////////////////
// COvlStartup dialog


COvlStartup::COvlStartup(CWnd* pParent /*=NULL*/)
	: CDialog(COvlStartup::IDD, pParent)
{
	//{{AFX_DATA_INIT(COvlStartup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COvlStartup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COvlStartup)
	DDX_Control(pDX, IDC_STARTUP_LIST, m_ovl_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COvlStartup, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(COvlStartup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COvlStartup message handlers

BOOL COvlStartup::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// fill it with all of the non-service overlay types
	int			index;

   m_ovl_list.ResetContent();  //clear list box (for filter)

	OVL_get_type_descriptor_list()->ResetEnumerator();
   while ( OVL_get_type_descriptor_list()->MoveNext() )
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

      // we skip static overlays with editors by convention
      if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay && pOverlayTypeDesc->pOverlayEditor != NULL)
         continue;

      // the factory will be omitted if the request string is empty
      if (pOverlayTypeDesc->displayName.IsEmpty())
         continue;

      // add the string
      index = m_ovl_list.AddString(pOverlayTypeDesc->displayName);

      // check to see if this overlay type should be restored at startup
      long defaultRestoreAtStartup = pOverlayTypeDesc->bDefaultRestoreAtStartup;

      string_utils::CGuidStringConverter guidConv(pOverlayTypeDesc->overlayDescriptorGuid);

      const int nRestoreAtStartup = PRM_get_registry_int("Overlay Manager\\Restore2", guidConv.GetGuidString().c_str(), defaultRestoreAtStartup);

      m_ovl_list.SetCheck(index, !nRestoreAtStartup);
      m_ovl_list.SetItemDataPtr(index, reinterpret_cast<void *>(pOverlayTypeDesc));
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COvlStartup::OnOK() 
{
   // loop through list box saving the NEVER_RESTORE flag for each checked
	// and turning it off for each NOT checked overlay type.
	int			index = 0;
	int			count = m_ovl_list.GetCount();

	while (index < count)
	{
		// get the data from the list
		const int nRestoreAtStartup = !m_ovl_list.GetCheck(index);
		OverlayTypeDescriptor* pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_ovl_list.GetItemDataPtr(index));

      string_utils::CGuidStringConverter guidConv(pOverlayTypeDesc->overlayDescriptorGuid);

      PRM_set_registry_int("Overlay Manager\\Restore2", guidConv.GetGuidString().c_str(), nRestoreAtStartup);

		// go on to the next item
		index++;
	}
	
	CDialog::OnOK();
}
LRESULT COvlStartup::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

