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

// GPSAuxData.cpp : implementation file
//

#include "stdafx.h"
#include "gps.h"
#include "gpsstyle.h"
#include "optndlg.h"
#include "AuxDataOptionsPage.h"
#include "MovingMapFeed.h"
#include "ovl_mgr.h"

// CAuxDataOptionsPage dialog

IMPLEMENT_DYNAMIC(CAuxDataOptionsPage, CPropertyPage)

CAuxDataOptionsPage::CAuxDataOptionsPage(CAuxDataProperties* pProperties /*= NULL*/, C_overlay* pOverlay /*= NULL*/)
	: CPropertyPage(CAuxDataOptionsPage::IDD), m_prop(pProperties),m_pOverlay(pOverlay),m_bApplyImmediately(false)
{
	m_modified = FALSE;
	m_NumAuxTypes = 0;	
	m_buttons = NULL;
}

CAuxDataOptionsPage::~CAuxDataOptionsPage()
{
	delete [] m_buttons;
}

void CAuxDataOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	for(int i = 0; i < m_NumAuxTypes; i++)
	{		
		DDX_Control(pDX, 0x100+i, m_buttons[i]);
	}
}


BEGIN_MESSAGE_MAP(CAuxDataOptionsPage, CPropertyPage)	
	ON_BN_CLICKED(0x100, OnModified)
	ON_BN_CLICKED(0x100+1, OnModified)
	ON_BN_CLICKED(0x100+2, OnModified)
	ON_BN_CLICKED(0x100+3, OnModified)
	ON_BN_CLICKED(0x100+4, OnModified)
	ON_BN_CLICKED(0x100+5, OnModified)
	ON_BN_CLICKED(0x100+6, OnModified)
	ON_BN_CLICKED(0x100+7, OnModified)
	ON_BN_CLICKED(0x100+8, OnModified)
END_MESSAGE_MAP()


// CAuxDataOptionsPage message handlers

BOOL CAuxDataOptionsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitializeControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAuxDataOptionsPage::InitializeControls()
{   
	// if the property page applies to a specific overlay
   if (m_pOverlay != NULL)
   {
	   C_gps_trail* trail = dynamic_cast<C_gps_trail*>(m_pOverlay);
	   if(trail != NULL)
	   {		   
			fvw::IMovingMapFeedAuxiliaryDataPtr auxData;
			if(trail->GetMovingMapFeed()->m_pMovingMapFeed && trail->GetMovingMapFeed()->m_pMovingMapFeed->QueryInterface(__uuidof(fvw::IMovingMapFeedAuxiliaryData), (void**)&auxData) == S_OK)
			{
				CFont* font = new CFont();
				font->CreateFont(14,0,0,0,FW_NORMAL,(BYTE)0,0,0,
							ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY,DEFAULT_PITCH | FF_DONTCARE,"Arial");
				CWnd::SetFont(font);
				long val = 0;
				auxData->GetNumAuxTypes(&val);
				m_NumAuxTypes = (int)val;
				m_buttons = new CButton[m_NumAuxTypes];
				m_prop->set_num_values(m_NumAuxTypes);
				for(int i = 0; i < m_NumAuxTypes; i++)
				{
					BSTR displayName;
					long enabled;
					auxData->GetAuxDataType(i, &displayName, &enabled);    

					//m_buttons[i] = new CButton();
					m_buttons[i].Create(CString(displayName), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(10, 0 +20*i, 270, 20 +20*i), this, 0x100+i);
					//m_buttons[i].ShowWindow(SW_SHOW);
					m_buttons[i].SetCheck((int)enabled);					
					m_buttons[i].SetFont(font);
					m_buttons[i].SetParent(this);
				}
			}			
	   }
   }

   UpdateData(FALSE);
   SetModified(TRUE);
}

BOOL CAuxDataOptionsPage::OnApply() 
{
   if (!m_modified)
      return TRUE;

   if (store_values() == FALSE)
      return FALSE;

	// if the property page applies to a specific overlay
   if (m_pOverlay != NULL)
   {
	   C_gps_trail* trail = dynamic_cast<C_gps_trail*>(m_pOverlay);
	   if(trail != NULL)
	   {		   
			fvw::IMovingMapFeedAuxiliaryDataPtr auxData;
			if(trail->GetMovingMapFeed()->m_pMovingMapFeed && trail->GetMovingMapFeed()->m_pMovingMapFeed->QueryInterface(__uuidof(fvw::IMovingMapFeedAuxiliaryData), (void**)&auxData) == S_OK)
			{
				for(int i = 0; i < m_NumAuxTypes; i++)
				{
					auxData->EnableAuxDataType(i, m_buttons[i].GetCheck());
				}
		   }			
	   }
      m_pOverlay->set_modified(TRUE);
      OVL_get_overlay_manager()->InvalidateOverlay(m_pOverlay);
   }

   m_modified = FALSE;

	return TRUE;
}

void CAuxDataOptionsPage::OnModified() 
{
	
   m_modified = TRUE;
   SetModified(TRUE);

   //if (m_bApplyImmediately)
      //OnApply();
}

BOOL CAuxDataOptionsPage::store_values()
{
   if (m_prop != NULL)
   {
      // make a copy of valid properties
      CAuxDataProperties oldProperties = *m_prop;
      if (UpdateData() == FALSE)
      {
         *m_prop = oldProperties;
         InitializeControls();
         return FALSE;
      }
	  for(int i = 0; i < m_NumAuxTypes; i++)
	  {
		 m_prop->set_value(i, m_buttons[i].GetCheck());
	  }
   }

   return TRUE;
}

BOOL CAuxDataOptionsPage::PreTranslateMessage(MSG* msg)
{		
	if(msg->message == WM_COMMAND)
	{		
		int x = 7;
	}

	return FALSE;
}