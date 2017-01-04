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



//////////////////////////////////////////////////////////////////////////////
//                                                         //
//  FileDialogEx.cpp : implementation file                           //
//                                                         //
//                                                         //
//  CFileOverlayDialog implements a CFileDialog that uses the new Windows      //
//  2000 style open/save dialog. Use companion class CDocManagerEx in an   //
//  MFC framework app.                                          //
//                                                         //
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileDialogEx.h"
#include "..\MultiFileDialog.h"
#include "mapx.h"
#include "ovl_mgr.h"

using namespace std;

IMPLEMENT_DYNAMIC(CFileOverlayDialog, CFileDialog)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileOverlayDialog::CFileOverlayDialog(LPCTSTR lpszDefExt, LPCTSTR lpszDefDir, CWnd* pParentWnd) :
   CFileDialog(TRUE, lpszDefExt, NULL, 0, NULL, pParentWnd),
   m_strDefaultDir(lpszDefDir)
{
   CString strDefaultFilter = GetFilterFromFactoryList();
   
   InitializeFilter(strDefaultFilter);
   InitializeFilterIndex(strDefaultFilter, lpszDefExt);

   m_ofn.lStructSize = sizeof(OPENFILENAME);
   m_ofn.lpstrInitialDir = m_strDefaultDir;

   m_first_time_through = TRUE;

   // initialize m_current_filter_index...
   m_current_filter_index = 0;
}

// Gets the filter from the file overlay factories
CString CFileOverlayDialog::GetFilterFromFactoryList()
{
   CString strFilter = "All Files (*.*)|*.*|";

   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

      // if the current overlay type is a file overlay
      if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      {
         // get the default filter for the overlay type
         CString current_filter = pOverlayTypeDesc->fileTypeDescriptor.openFileDialogFilter;

         // Look for the double pipe "||" in the current filter string.
         // If it is present in the string, replace it with a single pipe "|"...
         while (current_filter.Find("||") != -1)
            current_filter.Replace("||", "|");

         // make sure the current filter has a trailing pipe character
         if (current_filter.GetLength() > 0 && 
            current_filter[current_filter.GetLength() - 1] != '|')
            current_filter += '|';

         // Append the current filter to the default filter
         strFilter += current_filter;   
      }
   }

   // Finally, add an extra pipe "|" to the end of the default_filter string.
   // This marks the end of the filter string...
   strFilter += '|';

   return strFilter;
}

// InitializeFilter initializes the OPENFILENAME's lpstrFilter member from the factory list.  We
// translate the given filter into commdlg format (lots of \0)
void CFileOverlayDialog::InitializeFilter(const CString& strFilter)
{
   m_strFilter = strFilter;

   // modify the buffer in place.  Note,  do not call ReleaseBuffer() since the string contains '\0' characters
   LPTSTR pch = m_strFilter.GetBuffer(0); 

   // MFC delimits with '|' not '\0'
   while ((pch = _tcschr(pch, '|')) != NULL)
      *pch++ = '\0';

   m_ofn.lpstrFilter = m_strFilter;
}

// figure out the initial filter index based on the default extension (strDefaultExt)
void CFileOverlayDialog::InitializeFilterIndex(const CString& strFilter, const CString& strDefaultExt)
{
   // Note, there are two '|'s for each filter before the filter index we are looking for.  
   // Note, the filter index is 1-based, rather than 0-based
   int extension_pos = strFilter.Find(strDefaultExt);
   if (extension_pos != -1)
   {
      int nPipeCharacterCount = 0;
      for(int i=0;i<extension_pos;++i)
      {
         if (strFilter[i] == '|')
            nPipeCharacterCount++;
      }
      m_ofn.nFilterIndex = nPipeCharacterCount / 2 + 1;
   }
   else
      m_ofn.nFilterIndex = 1;
}

CFileOverlayDialog::~CFileOverlayDialog()
{
   // TODO BILLY: SOMETHING HERE TO KEEP OTHER DIALOGS FROM BEING THE SAME SIZE AS THE OPEN FILE DIALOG...

   m_filenames_buffer.ReleaseBuffer();
}

void CFileOverlayDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFileOverlayDialog)
   DDX_Control(pDX, IDC_AVAILABLE_OVERLAYS_LIST, m_available_list);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileOverlayDialog, CFileDialog)
   //{{AFX_MSG_MAP(CFileOverlayDialog)
   ON_WM_SIZE()
   ON_NOTIFY(NM_DBLCLK, IDC_AVAILABLE_OVERLAYS_LIST, OnDblclkAvailableOverlayList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////
// DoModal override copied mostly from MFC, with modification to use
// m_ofnEx instead of m_ofn.
//
int CFileOverlayDialog::DoModal()
{
   ASSERT_VALID(this);
   ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
   ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

   // zero out the file buffer for consistent parsing later
   ASSERT(AfxIsValidAddress(m_ofn.lpstrFile, m_ofn.nMaxFile));
   DWORD nOffset = lstrlen(m_ofn.lpstrFile)+1;
   ASSERT(nOffset <= m_ofn.nMaxFile);
   memset(m_ofn.lpstrFile+nOffset, 0, (m_ofn.nMaxFile-nOffset)*sizeof(TCHAR));
   
   // WINBUG: This is a special case for the file open/save dialog,
   //  which sometimes pumps while it is coming up but before it has
   //  disabled the main window.
   HWND hWndFocus = ::GetFocus();
   BOOL bEnableParent = FALSE;
   m_ofn.hwndOwner = PreModal();
   AfxUnhookWindowCreate();
   if (m_ofn.hwndOwner != NULL && ::IsWindowEnabled(m_ofn.hwndOwner))
   {
      bEnableParent = TRUE;
      ::EnableWindow(m_ofn.hwndOwner, FALSE);
   }

   _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
   ASSERT(pThreadState->m_pAlternateWndInit == NULL);


   if (m_ofn.Flags & OFN_EXPLORER)
      pThreadState->m_pAlternateWndInit = this;
   else
      AfxHookWindowCreate(this);

   memset(&m_ofnEx, 0, sizeof(m_ofnEx));
   memcpy(&m_ofnEx, &m_ofn, sizeof(m_ofn));

   // set these flags again for the copy of the m_ofn struct (m_ofnEx)...
   m_ofnEx.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY
               | OFN_ENABLETEMPLATE | OFN_EXPLORER | OFN_ENABLEINCLUDENOTIFY | OFN_ENABLEHOOK 
               | OFN_ENABLESIZING; 
   m_ofnEx.hInstance = AfxGetInstanceHandle();
   m_ofnEx.lpTemplateName = MAKEINTRESOURCE(IDD_OVERLAY_LIST_BOX);

   WORD ext = m_ofnEx.nFileExtension;

   // Initialize buffer for file names.
   const DWORD number_of_fileNames = 100;
   const DWORD filename_max_length = MAX_PATH + 1;
   DWORD buffer_size = (number_of_fileNames * filename_max_length) + 1;

   m_ofnEx.lpstrFile = m_filenames_buffer.GetBuffer(buffer_size);;
   m_ofnEx.nMaxFile = buffer_size;


   int nResult;
   
   if (m_bOpenFileDialog)
      nResult = ::GetOpenFileName((OPENFILENAME*)&m_ofnEx);
   else
      nResult = ::GetSaveFileName((OPENFILENAME*)&m_ofnEx);

   memcpy(&m_ofn, &m_ofnEx, sizeof(m_ofn));
   m_ofn.lStructSize = sizeof(m_ofn);

   if (nResult)
      ASSERT(pThreadState->m_pAlternateWndInit == NULL);
   
   pThreadState->m_pAlternateWndInit = NULL;

   // WINBUG: Second part of special case for file open/save dialog.
   if (bEnableParent)
      ::EnableWindow(m_ofnEx.hwndOwner, TRUE);
   if (::IsWindow(hWndFocus))
      ::SetFocus(hWndFocus);

   PostModal();
   
   return nResult ? nResult : IDCANCEL;
}

/////////////////////////////////////////////////////////////////////////////
// When the open dialog sends a notification, copy m_ofnEx to m_ofn in
// case handler function is expecting updated information in the
// OPENFILENAME struct.
//
BOOL CFileOverlayDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
   memcpy(&m_ofn, &m_ofnEx, sizeof(m_ofn));
   m_ofn.lStructSize = sizeof(m_ofn);

   // Set the filter index in the m_ofnEx OPENFILENAME struct to the value of
   // m_current_filter_index + 1.  Then set this same value for the standard m_ofn struct...
   m_ofnEx.nFilterIndex = m_current_filter_index + 1;
   m_ofn.nFilterIndex = m_ofnEx.nFilterIndex;

   return CFileDialog::OnNotify( wParam, lParam, pResult);
}

/////////////////////////////////////////////////////////////////////////////////
// File dialog initialization
/////////////////////////////////////////////////////////////////////////////////
BOOL CFileOverlayDialog::OnInitDialog() 
{
   CRect      rect;
   CRect      dlg_rect;
   CRect      dlg_new_pos;
   CRect      button_rect;
   CPoint      pos_tl;


   // call the base class implementation
   CDialog::OnInitDialog();

   HWND hWnd = ::GetParent(m_hWnd);

   EnumChildWindows(hWnd, (WNDENUMPROC)EnumChildProc, (LPARAM)this);

   // Set the intial values for the height and width of the File Open dialog...
   m_old_height = 365;
   m_old_width = 749;

   // set flag for this instance of OvlMgrAvailList to indicate that this is the Open File dialog...
   m_available_list.m_this_is_the_open_file_dialog = TRUE;

   // initialize the available overlays list
   m_available_list.initialize();
   
   // set the selected entry for the available list control
   m_available_list.set_selected_entry(0);

   // update the data collection for the dialog box
   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

//////////////////////////////////////////////////////////////////////////////
// This is a callback used to search for the window handle. This is used as
// the worker by EnumChildWindows().
//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CFileOverlayDialog::EnumChildProc( HWND hwnd,        // handle to window 
                                 LPARAM lParam )   // our CHILD_PROCESS_PARAMETERS block
{
   char classname[256];
   char window_title[256];
   int id = 0;

   ::GetWindowText(hwnd, window_title, 256);

   CFileOverlayDialog *pDlg = (CFileOverlayDialog *)lParam;

   id = ::GetDlgCtrlID(hwnd);

   pDlg->m_hWndMyID = hwnd;

   int numchars = GetClassName(hwnd, (LPSTR)&classname, 256);

   return TRUE; // to continue enumeration
}

void CFileOverlayDialog::OnSize(UINT nType, int cx, int cy)
{
   CRect ovl_list;
   float width_change = 0;
   float height_change = 0;
   float old_height_ovllistbox = 0;
   float new_height_ovllistbox = 0;
   int list_width = 0;
   int list_height = 0;
   
   CWnd * ovl_list2 = GetDlgItem(IDC_AVAILABLE_OVERLAYS_LIST);

   if (ovl_list2)
   {
      if (m_first_time_through)  // if this is the first time through this function for this
      {                     // instance of the file open dialog...
         cx = 749;
         cy = 365;
      }
      
      // calculate how much the file dialog has grown or shrunk...
      width_change = (float)cx - m_old_width;
      height_change = (float)cy - m_old_height;

      // use height_change to set the new height of the overlay list control so that
      // it grows/shrinks with the file dialog...
      ovl_list2->GetWindowRect(&ovl_list);
      ScreenToClient(&ovl_list);
      old_height_ovllistbox = static_cast<float>(ovl_list.Height());
      new_height_ovllistbox = old_height_ovllistbox + height_change;

      if (m_first_time_through)  // if this is the first time through this function for this
      {                     // instance of the file open dialog...
         list_height = 274;      // this is the height of the overlay list control
         m_first_time_through = FALSE;
      }
      else
         list_height = static_cast<int>(new_height_ovllistbox);   // this is the height of the overlay list control
      
      list_width = 175;                        // this is the width of the overlay list control

      ovl_list2->SetWindowPos(NULL, 0, 0, // no move
                        list_width, list_height,
                        SWP_NOZORDER | SWP_NOMOVE |SWP_SHOWWINDOW | SWP_FRAMECHANGED);
      
      m_old_width = static_cast<float>(cx);
      m_old_height = static_cast<float>(cy);
   }

   Invalidate();
   
   CDialog::OnSize(nType, cx, cy);   
}

BOOL CFileOverlayDialog::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
   int selected;
   CString   display_title;

   TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
   UINT nID = pNMHDR->idFrom;
   
   if (pTTT->uFlags & TTF_IDISHWND)
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
      switch (nID)
      {
         case IDC_AVAILABLE_OVERLAYS_LIST_ADD:
         {
            // ADD uses the available list, so grab that data.
            selected = m_available_list.get_selected_entry();
            if (selected >= 0)
            {
               display_title = m_available_list.get_item_label(selected);

               strcpy_s(pTTT->szText, 80, "Open ");
               strcat_s(pTTT->szText, 80, display_title);
            }
            break;
         }
            
         case IDC_AVAILABLE_OVERLAYS_LIST_ADD_NEW:
         {
            // ADD uses the available list, so grab that data.
            selected = m_available_list.get_selected_entry();
            if (selected >= 0)
            {
               display_title = m_available_list.get_item_label(selected);
               GUID overlayDescGuid = *reinterpret_cast<GUID *>(m_available_list.get_item_data(selected));
            
               if (OVL_get_type_descriptor_list()->IsStaticOverlay(overlayDescGuid))
               {
                  strcpy_s(pTTT->szText, 80, "Open ");
                  strcat_s(pTTT->szText, 80, display_title);
               }
               else
               {
                  strcpy_s(pTTT->szText, 80, "New ");
                  strcat_s(pTTT->szText, 80, display_title);
               }
            }
            break;
         }
      }
      
      return(TRUE);
   }
   
   return(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////
// This function sets the file dialog directory and filter to match the overlay that
// was double-clicked in the Available Overlays listbox control...
////////////////////////////////////////////////////////////////////////////////////////
void CFileOverlayDialog::OnDblclkAvailableOverlayList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   C_ovl_mgr*   ovl_mgr = OVL_get_overlay_manager();
   CPoint point;
   int j;

   // get the current cursor location in screen coordinates
   GetCursorPos(&point);

   // convert the cursor location to the coordinates of the list control
   m_available_list.ScreenToClient(&point);

   // get the index of the item that was double clicked, if any
   int index = m_available_list.hit_test(point);
   if (index == -1)
   {
      *pResult = SUCCESS;
      return;
   }

   // set the selected entry and the focus on the current list control
   SetFocus();
   m_available_list.set_selected_entry(index);

   // get the current selection from the available list...
   int   avail_selection = m_available_list.get_selected_entry();

   // get the class name for the selected (double-clicked) overlay...
   GUID overlayDescGuid = *reinterpret_cast<GUID *>(m_available_list.get_item_data(avail_selection));

   // get the factory associated with the selected class name...
   OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlayDescGuid);
   if (pOverlayTypeDesc == NULL)
   {
      ERR_report("Unable to get overlay type");
      *pResult = SUCCESS;
      return;
   }

   // get the filter string associated with the current factory...
   CString current_filter;
   if (pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay)
      current_filter = pOverlayTypeDesc->fileTypeDescriptor.openFileDialogFilter;

   j = current_filter.Find("|");
   CString default_filter_string = current_filter.Left(j);

   // since we are updating the OPENFILENAME struct, the input filter string must be delimited
   // with the NULL character ('\0') instead of the pipe.  So look for the pipe in the current
   // filter string and replace it with '\0'...
   // First make sure there are no double pipes already in the input filter string
   current_filter.Replace("||", "|");

   while (current_filter.Find("|") != -1)
      current_filter.Replace("|", "\\0");

   // add the additional null character to the end of the current filter string...
   int num_chars = current_filter.GetLength();
   current_filter.Insert(num_chars, "\\0");

   // update the OPENFILENAME struct with the newly-selected file-type filter...
   LPCTSTR new_filter = current_filter;
   m_ofnEx.lpstrFilter = new_filter;
   m_ofn.lpstrFilter = new_filter;

   // find the extension associated with this default filter...
   j = default_filter_string.Find("*.");
   CString current_extension = default_filter_string.Mid(j);
   current_extension.Remove(')');

   // if there are multiple extensions in the default filter (such as for the Moving Map overlay), they
   // will be seperated by commas.  To make current_extension work correctly as a filter for all the
   // extensions, replace the commas with semicolons...
   current_extension.Replace(",", ";");

   // Get the most recent directory associated with the current overlay (the directory in
   // which the last file open was done for this overlay type) and use that as the initial
   // directory for the File Open dialog.  If there is no valid recent directory for the
   // current overlay, the get_most_recent_directory() function returns the overlay's default 
   // directory.
   CString initialDirectory = overlay_type_util::get_most_recent_directory(pOverlayTypeDesc->overlayDescriptorGuid);

   CFileDialog::OnTypeChange();

   // find the zero-based index of this filter string in the default filter drop down list
   m_current_filter_index = GetParent()->SendDlgItemMessage(1136, CB_FINDSTRINGEXACT, 0, (LPARAM) (LPCTSTR)default_filter_string);

   if (m_current_filter_index == CB_ERR)
      m_current_filter_index = 0;

   // use this index to set the current selection in the default filter drop down list
   // to correspond to the currently-selected overlay type...
   GetParent()->SendDlgItemMessage(1136, CB_SETCURSEL, m_current_filter_index, 0);

   // append the current file extension to the initial directory path so that the newly
   // displayed files are filtered properly.
   // add a backslash to the end of the string if there is not already one there...
   if (strcmp(initialDirectory.Right(1), "\\") != 0)
      initialDirectory += "\\";
   initialDirectory += current_extension;


// THIS SETS THE OBJECT EDIT BOX STRING AND THEN SENDS AN OK_BUTTON "CLICKED" MESSAGE TO CHANGE THE
// DISPLAYED DIRECTORY... IS THERE A BETTER WAY TO CHANGE THE DIRECTORY???
   CWnd* pCtrl = GetParent()->GetDlgItem(edt1);
   if (pCtrl == NULL)
      pCtrl = GetParent()->GetDlgItem(cmb13);
   if (pCtrl != NULL)
   {
      pCtrl->SetWindowText(initialDirectory);
      GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (long)GetDlgItem(IDOK)->GetSafeHwnd());   
      pCtrl->SetWindowText("");
   }


   GetParent()->Invalidate(TRUE);

   *pResult = SUCCESS;
}
// END OF OnDblclkAvailableOverlayList

void CFileOverlayDialog::OnFileNameChange()
{
   TCHAR temp_buffer;
    
   // Get the required size for the 'files' buffer
   UINT size_of_files = CommDlg_OpenSave_GetSpec(GetParent()->m_hWnd, &temp_buffer, 1);

   // Get the required size for the 'folder' buffer
   UINT size_of_folder = CommDlg_OpenSave_GetFolderPath(GetParent()->m_hWnd, &temp_buffer, 1);

   // Check if lpstrFile and nMaxFile are large enough
   if (size_of_files + size_of_folder > m_ofn.nMaxFile)
   {
      // Create buffer for file names.
      m_filenames_buffer.ReleaseBuffer();
      
      DWORD buffer_size = size_of_files + size_of_folder + 1;

      // Attach buffer to OPENFILENAME member.
      m_ofnEx.lpstrFile = m_filenames_buffer.GetBuffer(buffer_size);
      m_ofnEx.nMaxFile = buffer_size;
   }

   CFileDialog::OnFileNameChange();
}