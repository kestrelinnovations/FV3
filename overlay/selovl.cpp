// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// selovl.cpp : implementation file
//

#include "stdafx.h"
#include "selovl.h"
#include "ovl_mgr.h"
#include "OvlFctry.h"
#include "err.h"
#include "..\getobjpr.h"


using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// SelectOverlayDlg dialog

SelectOverlayDlg::SelectOverlayDlg(selection_t selection_mode, 
      boolean_t file_overlays_only /* = FALSE */, 
      boolean_t dirty_overlays_only /* = FALSE */,
      long bRequireFileExists /* = 0 */,
      boolean_t allow_multi_select /* = FALSE */,
      boolean_t saveable_overlays_only /* = FALSE */)
      : CDialog(SelectOverlayDlg::IDD, NULL),
     m_pSelectedOverlayTypeDesc(NULL)
{
   //{{AFX_DATA_INIT(SelectOverlayDlg)
   //}}AFX_DATA_INIT

   m_multi_select = allow_multi_select;

   // initialize selection mode
   m_mode = selection_mode;

   m_bFileOverlaysOnly = file_overlays_only;
   m_bRequireFileExists = bRequireFileExists;
   m_bRequireDirty = dirty_overlays_only;
   m_bSaveableOverlaysOnly = saveable_overlays_only;

   // initialize labels to defaults
   strcpy_s(m_title, 41, "Select Overlay");
   strcpy_s(m_label, 43, "Select Overlay");
   strcpy_s(m_OK_label, 16, "OK");
}


void SelectOverlayDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectOverlayDlg)
   DDX_Control(pDX, IDOK, m_OK);
   DDX_Control(pDX, IDC_LIST_TITLE, m_list_title);
   DDX_Control(pDX, IDC_OVERLAY_LIST, m_list);
   DDX_Control(pDX, IDC_OVERLAY_LIST_MULTI, m_list_multi);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectOverlayDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(SelectOverlayDlg)
   ON_LBN_DBLCLK(IDC_OVERLAY_LIST, OnDblclkOvlList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SelectOverlayDlg message handlers

class SelectOverlayEntry
{
public:
   SelectOverlayEntry(CString name, OverlayTypeDescriptor *pOverlayTypeDesc) 
   { 
      m_name = name; 
      m_pOverlayTypeDesc = pOverlayTypeDesc;
   }

   CString m_name;
   OverlayTypeDescriptor* m_pOverlayTypeDesc;

   bool operator<(const SelectOverlayEntry &p) const
   {
      // have Route be the top-most entry in the list
      if (p.m_name == "Route")
         return false;

      return m_name < p.m_name;
   }
};

BOOL SelectOverlayDlg::OnInitDialog() 
{
   C_ovl_mgr* ovl_mgr;
   C_overlay* overlay;

   CDialog::OnInitDialog();

   // hide or show the appropriate list box (single selection or 
   // multi-selection)
   if (m_multi_select)
      GetDlgItem(IDC_OVERLAY_LIST)->ShowWindow(SW_HIDE);
   else
      GetDlgItem(IDC_OVERLAY_LIST_MULTI)->ShowWindow(SW_HIDE);

   // get the overlay manager
   ovl_mgr = OVL_get_overlay_manager();
   if (ovl_mgr == NULL)
   {
      ERR_report("OVL_get_overlay_manager() failed.");
      return FALSE;
   }

   // list for File New Overlay Types and OverlayEditors
   if (m_mode == SELECT_OVERLAY_TYPE) 
   {
      vector<SelectOverlayEntry> overlay_lst;

      // the file overlays
      OVL_get_type_descriptor_list()->ResetEnumerator();
      while ( OVL_get_type_descriptor_list()->MoveNext() )
      {
         OverlayTypeDescriptor *pOverlayTypeDesc = 
            OVL_get_type_descriptor_list()->m_pCurrent;

         // display all file overlays that have an associated editor
         if ( pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay && 
              pOverlayTypeDesc->pOverlayEditor != NULL)
         {
            // skip insertion if is_user_controllable(false)
            if (!pOverlayTypeDesc->is_user_controllable)
            {
               continue;
            }
            overlay_lst.push_back(SelectOverlayEntry(pOverlayTypeDesc->displayName, pOverlayTypeDesc));
         }
      }

      // include mission package in the list
      overlay_lst.push_back(SelectOverlayEntry("Mission Package", NULL));

      sort(overlay_lst.begin(), overlay_lst.end());

      if (m_multi_select)
      {
         for(size_t i=0;i<overlay_lst.size();++i)
            m_list_multi.AddString(overlay_lst[i].m_name);
      }
      else
      {
         // add each overlay's title to the single-selection list control.  In addition, we
         // will store the associated overlay factory as item data
         for(size_t i=0;i<overlay_lst.size();++i)
         {
            int nIndex = m_list.AddString(overlay_lst[i].m_name);
            m_list.SetItemDataPtr(nIndex, reinterpret_cast<void *>(overlay_lst[i].m_pOverlayTypeDesc));
         }
      }

      // select the first item in the list by default
      if (m_multi_select)
         m_list_multi.SetSel(0);
      else
         m_list.SetCurSel(0);

      // select the type based on the current mode
      if (ovl_mgr->GetCurrentEditor() == GUID_NULL)
         select_class_title_from_current_overlay(ovl_mgr->get_current_overlay());
      else
         select_class_title_from_current_mode(ovl_mgr->GetCurrentEditor());

      // set labels for overlay type selection
      SetWindowText(m_title);
      m_list_title.SetWindowText(m_label);
      m_OK.SetWindowText(m_OK_label);
   }
   else if (m_mode == SELECT_OVERLAY)
   {
      boolean_t skip;
      CString text;
      CSize extent;
      int max_extent = 0; 

      // get the DC so you can call GetTextExtent
      CDC *dc;
      if (m_multi_select)
         dc = m_list_multi.GetDC();
      else
         dc = m_list.GetDC();

      overlay = ovl_mgr->get_first();
      while (overlay)
      {
         skip = FALSE;
         OverlayPersistence_Interface* pFvOverlayPersistence = 
            dynamic_cast<OverlayPersistence_Interface *>(overlay);

         // Skip the overlay if we are filtering on file overlays and this
         // overlay is it is not a file overlay OR if its overlay type
         // descriptor has an empty save-as filter
         if (m_bFileOverlaysOnly && pFvOverlayPersistence == NULL)
            skip = TRUE;

         if (m_bSaveableOverlaysOnly)
         {
            OverlayTypeDescriptor* desc =
               OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(
               overlay->get_m_overlayDescGuid());
            if (!desc->fileTypeDescriptor.bIsFileOverlay ||
               desc->fileTypeDescriptor.saveFileDialogFilter.IsEmpty())
            {
               skip = TRUE;
            }
         }

         if (m_bRequireFileExists)
         {
            // if it is required that the file exists on the disk, then check it
            if (pFvOverlayPersistence == NULL)
               skip = TRUE;
            else
            {
               long bHasBeenSaved = 0;
               pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
               if (!bHasBeenSaved)
                  skip = TRUE;
            }
         }

         if (m_bRequireDirty)
         {
            // if it is required that the overlay is dirty then check it
            if (pFvOverlayPersistence == NULL)
               skip = TRUE;
            else
            {
               long bDirty = 0;
               pFvOverlayPersistence->get_m_bIsDirty(&bDirty);
               if (!bDirty)
                  skip = TRUE;
            }
         }

         if (!skip)
         {
            // skip insertion if is_user_controllable(false)
            // or FvOverlayTypeOverrides UserControllable(False) 
            if (overlay != nullptr)
            {
               long userControllable = TRUE;
               // check the interface overrides implemented at run time.
               overlay->get_m_UserControllable(&userControllable); 
               if (userControllable == 0)  
               {
                  overlay = ovl_mgr->get_next(overlay);
                  continue;
               }
               OverlayTypeDescriptor* pOverlayTypeDesc = nullptr;
               pOverlayTypeDesc = OVL_get_type_descriptor_list()->
                  GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());
               if (pOverlayTypeDesc != nullptr)
               {
                  if (!pOverlayTypeDesc->is_user_controllable)
                  {
                     overlay = ovl_mgr->get_next(overlay);
                     continue;
                  }
               }
            }

            if (overlay->is_modified())
               text = OVL_get_overlay_manager()->GetOverlayDisplayName(overlay) + " *";
            else
               text = OVL_get_overlay_manager()->GetOverlayDisplayName(overlay);

            if (m_multi_select)
               m_list_multi.AddString(text);
            else
               m_list.AddString(text);

            // if the extent of this entry is the largest so far, use it for
            // the maximum
            if (dc)
            {
               extent = dc->GetTextExtent(text);
               if (extent.cx > max_extent)
                  max_extent = extent.cx;
            }
         }

         overlay = ovl_mgr->get_next(overlay);
      }

      // using max_extent directly makes the scroll range too large
      max_extent = 80 * max_extent / 100;

      if (m_multi_select)
         m_list_multi.SetHorizontalExtent(max_extent);
      else
         m_list.SetHorizontalExtent(max_extent);

      // select the first item in the list (the current most overlay)
      if (m_multi_select)
         m_list_multi.SetSel(0);
      else
         m_list.SetCurSel(0);

      // set labels for overlay selection
      SetWindowText(m_title);
      m_list_title.SetWindowText(m_label);
      m_OK.SetWindowText(m_OK_label);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void SelectOverlayDlg::OnDblclkOvlList() 
{
   OnOK();
}

void SelectOverlayDlg::OnOK() 
{
   int index;
   CString selection;

   int num_selected = 1;
   CArray<int,int> selected_lst;
   if (m_multi_select)
   {
      num_selected = m_list_multi.GetSelCount();

      selected_lst.SetSize(num_selected);
      m_list_multi.GetSelItems(num_selected, selected_lst.GetData()); 
   }
   else
   {
      selected_lst.SetSize(1);
      selected_lst[0] = m_list.GetCurSel();
   }

   for(int i=0;i<num_selected;i++)
   {
      index = selected_lst[i];
      if (index != LB_ERR)
      {
         if (m_multi_select)
            m_list_multi.GetText(index, selection);
         else
            m_list.GetText(index, selection);

         if (m_mode == SELECT_OVERLAY_TYPE)
         {
            m_pSelectedOverlayTypeDesc = reinterpret_cast<OverlayTypeDescriptor *>(m_list.GetItemDataPtr(index));
         }
         else if (m_mode == SELECT_OVERLAY)
         {
            int end;

            // strip off " *"
            end = selection.Find(" *");
            if (end != -1)
               m_selection.AddTail(selection.Left(end));
            else
               m_selection.AddTail(selection);
         }
         else
            ERR_report("Invalid mode.");
      }
      else
         ERR_report("GetCurSel() failed.");
   }

   CDialog::OnOK();
}


// Use the class title associated with the current overlay, provided its
// title is in the list.
void SelectOverlayDlg::select_class_title_from_current_overlay(
   C_overlay *current_overlay)
{
   const int TITLE_LEN = 81;
   char title[TITLE_LEN];

   if (current_overlay == NULL)
   {
      if (m_multi_select)
         m_list_multi.SelectString(-1, "Route");
      else
         m_list.SelectString(-1, "Route");

      return;
   }

   // get the title associated with the given overlay
   strcpy_s(title, TITLE_LEN, OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(current_overlay->get_m_overlayDescGuid())->displayName);

   if (m_multi_select)
      m_list_multi.SelectString(-1, title);
   else
      m_list.SelectString(-1, title);
}

// Use the class title associated with the current mode, provided its title
// is in the list.
void SelectOverlayDlg::select_class_title_from_current_mode(GUID overlayDescGuid)
{
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc != NULL)
   {
      CString strTitle = pOverlayTypeDesc->displayName;
      if (strTitle.GetLength() > 0)
      {
         if (m_multi_select)
            m_list_multi.SelectString(-1, strTitle);
         else
            m_list.SelectString(-1, strTitle);
      }
   }
}


// Sets the title of the dialog box.
int SelectOverlayDlg::set_title(const char *title)
{
   int length;

   length = strlen(title);
   if (length < 1 || length > 40)
   {
      ERR_report("Invalid title length.");
      return FAILURE;
   }

   strcpy_s(m_title, 41, title);

   if (GetSafeHwnd())
      SetWindowText(title);

   return SUCCESS;
}

// Sets the label of the list control.
int SelectOverlayDlg::set_label(const char *label)
{
   int length;
   const int BUFFER_LEN = 43;
   char buffer[BUFFER_LEN];

   length = strlen(label);
   if (length < 1 || length > 40)
   {
      ERR_report("Invalid label length.");
      return FAILURE;
   }

   // add a short cut and : to the label
   sprintf_s(buffer, BUFFER_LEN, "&%s:", label);
   strcpy_s(m_label, 43, buffer);

   if (m_list_title.GetSafeHwnd())
      m_list_title.SetWindowText(m_label);

   return SUCCESS;
}

// Sets the label on the OK button.
int SelectOverlayDlg::set_OK_label(const char *label)
{
   int length;

   length = strlen(label);
   if (length < 1 || length > 15)
   {
      ERR_report("Invalid label length.");
      return FAILURE;
   }

   // add a short cut and : to the label
   strcpy_s(m_OK_label, 16, label);

   if (m_OK.GetSafeHwnd())
      m_OK.SetWindowText(m_OK_label);

   return SUCCESS;
}

const char *SelectOverlayDlg::get_selection(int index)
{
   POSITION position = m_selection.FindIndex(index);

   if (position)
      return m_selection.GetAt(position);

   return NULL;
}

LRESULT SelectOverlayDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

