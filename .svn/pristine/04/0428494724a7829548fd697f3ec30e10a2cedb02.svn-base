// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// TstOvlPP.cpp : implementation file
//

#include "stdafx.h"
#include "TstOvlPP.h"
#include "testglob.h"
#include "OvlFctry.h"

/////////////////////////////////////////////////////////////////////////////
// CTestOvlkitPP property page

IMPLEMENT_DYNCREATE(CTestOvlkitPP, CPropertyPage)

CTestOvlkitPP::CTestOvlkitPP() : CPropertyPage(CTestOvlkitPP::IDD)
{
   //{{AFX_DATA_INIT(CTestOvlkitPP)
   m_testOvlToggle = FALSE;
   m_testOpenCloseOvlFile = FALSE;
   //}}AFX_DATA_INIT
}

CTestOvlkitPP::~CTestOvlkitPP()
{
}

void CTestOvlkitPP::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTestOvlkitPP)
   DDX_Control(pDX, IDC_STATIC_OVL_LIST, m_ctrlStaticOvlListBox);
   DDX_Check(pDX, IDC_TEST_OVL_TOGGLE, m_testOvlToggle);
   DDX_Check(pDX, IDC_TEST_OPEN_AND_CLOSE_OVL_FILE, m_testOpenCloseOvlFile);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestOvlkitPP, CPropertyPage)
   //{{AFX_MSG_MAP(CTestOvlkitPP)
   ON_BN_CLICKED(IDC_TEST_OVL_TOGGLE, OnClickTestStaticOvlToggles)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestOvlkitPP message handlers

void CTestOvlkitPP::OnClickTestStaticOvlToggles() 
{
   // enable or disable the static overlay list depending on its parent (IDC_TEST_OVL_TOGGLE)
   UpdateData(TRUE);
   if (m_testOvlToggle)
   {
      GetDlgItem(IDC_STATIC_OVL_LIST)->EnableWindow(TRUE);
      GetDlgItem(IDC_STATIC_OVL_LIST_TITLE)->EnableWindow(TRUE);
   }
   else
   {
      GetDlgItem(IDC_STATIC_OVL_LIST)->EnableWindow(FALSE);
      GetDlgItem(IDC_STATIC_OVL_LIST_TITLE)->EnableWindow(FALSE);
   }
}

BOOL CTestOvlkitPP::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

   // reset the contents of the static overlay list box
   m_ctrlStaticOvlListBox.ResetContent();


   // populate the static overlay list box with the currently available static overlay types...
   OVL_get_type_descriptor_list()->ResetEnumerator();
   while ( OVL_get_type_descriptor_list()->MoveNext() )
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

      // if the factory is a static overlay factory
      if (!pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         // skip any static overlays that have editors (e.g., Map Data Manager)
         if (pOverlayTypeDesc->pOverlayEditor != NULL)
            continue;

         // the factory will be omitted if the request string is empty
         if (!pOverlayTypeDesc->displayName.IsEmpty())
         {
            int nIndex = m_ctrlStaticOvlListBox.AddString(pOverlayTypeDesc->displayName);
            m_ctrlStaticOvlListBox.SetItemDataPtr(nIndex, reinterpret_cast<void *>(pOverlayTypeDesc));
         }
      }
   }

   // by default, select the first static overlay in the list box...
   m_ctrlStaticOvlListBox.SetSel(0);

   // Disable the static overlay list box and title until we click their parent (IDC_TEST_OVL_TOGGLE)
   GetDlgItem(IDC_STATIC_OVL_LIST)->EnableWindow(FALSE);
   GetDlgItem(IDC_STATIC_OVL_LIST_TITLE)->EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTestOvlkitPP::OnApply() 
{
   // if the static overlay toggles option was selected on the Test Settings property sheet, get the
   // list of static overlays selected for toggling from the CTestOvlkitPP property page list box...
   if (m_testOvlToggle)
   {
      CString selected_display_title, selected_class_name;
      CArray<int, int> m_index_list;  // to store indexes of static overlays selected from the list box

      int num_selected_static_overlays = m_ctrlStaticOvlListBox.GetSelCount();

      // set the size then populate the list of indexes for the current set of selected static overlays...
      m_index_list.SetSize(num_selected_static_overlays);
      m_ctrlStaticOvlListBox.GetSelItems(num_selected_static_overlays, m_index_list.GetData());

      // set the size for the list of currently-selected static overlay class names...
      gSelectedStaticOvlList.SetSize(num_selected_static_overlays);

      // set the size for the array of toggle counters for the currently-selected static overlays...
      gSelectedStaticOvlCounters.SetSize(num_selected_static_overlays);

      for (int i=0; i<num_selected_static_overlays; i++)
      {
         OverlayTypeDescriptor *pOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_ctrlStaticOvlListBox.GetItemDataPtr(m_index_list.GetAt(i)));

         // add this static overlay factory to the list of selected static overlays
         // that will be used in the toggle test code in tstovtog.cpp...
         gSelectedStaticOvlList.SetAt(i, pOverlayTypeDesc);

         // initialize the toggle counter for this static overlay...
         gSelectedStaticOvlCounters.SetAt(i, 0);
      }
   }
   
   return CPropertyPage::OnApply();
}
