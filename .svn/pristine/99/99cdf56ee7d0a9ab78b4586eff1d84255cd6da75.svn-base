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

// MissionPackageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MissionPackageDlg.h"
#include "fvwutil.h"
#include "err.h"  // for ERR_report
#include "mainfrm.h"  // for CMainFrame
#include "InfoZip.h"
#include "param.h"
#include "OvlFctry.h"  // for C_overlay_factory
#include "FctryLst.h"
#include "wm_user.h"  // for declaration of WM_FILE_STATE_CHANGED

#include "map.h"  // for ViewMapProj

#include "favorite.h"  // for FavoriteList
#include "mem.h"  // for MEM_free
#include "MAPIWrapper.h"  // for messaging services
#include "file.h"
#include "getobjpr.h"


const int ICON_DIM_X = 16;
const int ICON_DIM_Y = 16;
const int ICON_LST_INITIAL_SIZE = 20;
const int ICON_LST_GROW_BY = 5;

#define OK_STR "Identical"
#define DISK_MORE_RECENT_STR "File on local disk more recent"
#define PACKAGE_MORE_RECENT_STR "File in package more recent"
#define NOT_FOUND_STR "File not found on local disk"

#define MAP_DISPLAY_SETTINGS_FOUND "Map display settings were found imbedded in the package"
#define MAP_DISPLAY_SETTINGS_NOT_FOUND "Map display settings were not found in the package"

/////////////////////////////////////////////////////////////////////////////
// CMissonPackageDlg dialog

UINT FileMonitorThread(void *param);
HWND mission_binder_dlg_hWnd = NULL;

static CString extract_path(CString& path)
{
   return path.Left(path.ReverseFind('\\'));
}

static CString extract_filename(CString& path)
{
   return path.Right(path.GetLength() - path.ReverseFind('\\') - 1);
}

static CString extract_extension(CString& path)
{
   return path.Mid(path.ReverseFind('.'));
}

static CString remove_extension(CString& path)
{
   int index = path.ReverseFind('.');

   if (index == -1)
      return path;

   return path.Left(index);
}

static CString get_directory_from_filename(CString& file_name)
{
   CString extension = extract_extension(file_name);

   // loop through the factory list.  If the extension matches one of the overlay's
   // in the factory list, we will use that overlay's folder

   OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension(extension);
   if (pOverlayTypeDesc != NULL)
      return pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;

   return PRM_get_registry_string("Main","USER_DATA","") + "\\MissionPackages\\MiscData";
}

// used to pass parameters to the file monitoring thread
typedef struct
{
   binder_file_t *binder_file;
} file_monitor_param_t;

// constructor
CMissionPackageDlg::CMissionPackageDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CMissionPackageDlg::IDD, pParent), m_zip_library_initialized(false), m_MAPI(NULL), m_package_service(NULL)
{
   //{{AFX_DATA_INIT(CMissionPackageDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

CMissionPackageDlg::~CMissionPackageDlg()
{
   if (m_MAPI) 
   {
      delete m_MAPI;
      m_MAPI = NULL;
   }

   if (m_package_service)
   {
      delete m_package_service;
      m_package_service = NULL;
   }
}


void CMissionPackageDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMissionPackageDlg)
   DDX_Control(pDX, IDC_OVERLAY_LST, m_overlay_lst_ctrl);
   DDX_Control(pDX, IDC_FILE_LST, m_file_lst_ctrl);
   DDX_Control(pDX, IDC_STORE_OPEN_OVERLAYS, m_store_open_overlays);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMissionPackageDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CMissionPackageDlg)
   ON_BN_CLICKED(IDC_ADD_OVERLAY, OnAddOverlay)
   ON_BN_CLICKED(IDC_OPEN_OVERLAY, OnOpenOverlay)
   ON_BN_CLICKED(IDC_ADD_ALL_OVERLAY, OnAddAllOverlays)
   ON_WM_CLOSE()
   ON_BN_CLICKED(IDC_ADD, OnAddFiles)
   ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
   ON_BN_CLICKED(IDC_REMOVE, OnRemove)
   ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
   ON_MESSAGE(WM_FILE_STATE_CHANGED, OnFileStateChanged)
   ON_BN_CLICKED(IDC_OPEN, OnOpen)
   ON_BN_CLICKED(IDC_OPEN_ALL, OnOpenAll)
   ON_BN_CLICKED(IDC_EXTRACT, OnExtract)
   ON_NOTIFY(NM_DBLCLK, IDC_FILE_LST, OnDblclkFileLst)
   ON_WM_DROPFILES()
   ON_NOTIFY(NM_DBLCLK, IDC_OVERLAY_LST, OnOverlayListDoubleClick)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_OVERLAY_LST, OnOverlayListItemchanged)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LST, OnFileListItemchanged)
   ON_BN_CLICKED(IDC_IMBED_FAVORITE, OnStoreFavorite)
   ON_BN_CLICKED(IDC_GOTO, OnGoto)
   ON_BN_CLICKED(IDC_MAIL, OnMail)
   ON_BN_CLICKED(IDC_HELP_MISSION_PACKAGE, OnHelp)
   ON_WM_MOUSEMOVE()
   ON_NOTIFY(LVN_KEYDOWN, IDC_FILE_LST, OnKeydownFileLst)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMissionPackageDlg message handlers

BOOL CMissionPackageDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // used in the file monitoring threads to post messages back
   // to this dialog
   mission_binder_dlg_hWnd = m_hWnd;

   // register the file list control for Ole drop targets - the whole
   // dialog will be a drop target
   DragAcceptFiles();

   // we want the list control to have full row select
   ListView_SetExtendedListViewStyle(m_file_lst_ctrl.GetSafeHwnd(), LVS_EX_FULLROWSELECT);

   // initialize file list control
   //

   LV_COLUMN lc;

   // initialize lc common entries
   memset(&lc, 0, sizeof(LV_COLUMN));
   lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
   lc.fmt = LVCFMT_LEFT;

   // initialize the File Name column
   lc.cx = 130;
   lc.pszText = "File Name";
   lc.iSubItem = 0;
   m_file_lst_ctrl.InsertColumn(1, &lc);

   // initialize the Status column
   lc.cx = 229;
   lc.pszText = "Comment";
   lc.iSubItem = 1;
   m_file_lst_ctrl.InsertColumn(2, &lc);

   initialize();

   m_store_open_overlays.SetCheck(1);

   // if messaging is not available then disable the button
   GetDlgItem(IDC_MAIL)->EnableWindow(m_MAPI->is_mail_available());

   m_overlay_lst_ctrl.m_parent = this;
   m_file_lst_ctrl.m_parent = this;

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CMissionPackageDlg::OnDropFiles(HDROP hDropInfo)
{
   UINT file_count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

   int count_sucessfully_added = 0;
   for(UINT i=0;i<file_count;i++)
   {
      char buf[MAX_PATH];
      DragQueryFile(hDropInfo, i, buf, MAX_PATH);
      CString full_path(buf);

      int image_pos = get_icon_from_file_hlpr(full_path);
      add_file_to_binder(full_path, image_pos, count_sucessfully_added);
   }

   DragFinish(hDropInfo);
}

void CMissionPackageDlg::InitPackageService()
{
   m_zip_library_initialized = false;

   CString package_ext = extract_extension(m_zip_name);

   if (m_package_service)
   {
      delete m_package_service;
      m_package_service = NULL;
   }

#ifdef GOV_RELEASE
   if (package_ext.CompareNoCase(CString(".XMP")) == 0)
      m_package_service = new XPlanPackageService();
   else
      m_package_service = new InfoZipPackageService();
#else
   m_package_service = new InfoZipPackageService();
#endif 

   if (m_package_service->Initialize() != SUCCESS)
   {
      ERR_report("Unable to load zip library");
      AfxMessageBox("Unable to load zip library.  Unable to continue...");

      delete m_package_service;
      m_package_service = NULL;
   }
   else
      m_zip_library_initialized = true;
}

void CMissionPackageDlg::initialize()
{
   // set the file name in the title
   CString title_bar_txt;
   title_bar_txt.Format("Mission Package - %s", extract_filename(m_zip_name));
   SetWindowText(title_bar_txt);

   if (!m_MAPI)
      m_MAPI = new MAPIWrapper;

   // initialize the image list used in both the overlay list
   // and the list of files
   m_images.DeleteImageList();
   m_images.Create(ICON_DIM_X, ICON_DIM_Y, ILC_COLORDDB | ILC_MASK, 
      ICON_LST_INITIAL_SIZE, ICON_LST_GROW_BY);
   m_images.SetBkColor(RGB(255,255,255));
   m_overlay_lst_ctrl.SetImageList(&m_images, LVSIL_SMALL);
   m_file_lst_ctrl.SetImageList(&m_images, LVSIL_SMALL);

   // populate the overlay list control with a list of currently
   // opened file overlays and allow registered 3rd party objects 
   // to add entries to the list
   update_overlay_list();

   // let the binder add items to the file list control
   m_file_lst_ctrl.DeleteAllItems();
   m_binder.populate_list_ctrl(this);

   if (!m_found_favorite)
   {
      GetDlgItem(IDC_FAV_TEXT)->SetWindowText(MAP_DISPLAY_SETTINGS_NOT_FOUND);
      GetDlgItem(IDC_GOTO)->EnableWindow(FALSE);
   }
   else
   {
      GetDlgItem(IDC_FAV_TEXT)->SetWindowText(MAP_DISPLAY_SETTINGS_FOUND);
      GetDlgItem(IDC_GOTO)->EnableWindow(TRUE);
   }

   OnFileListItemchanged(NULL, NULL);
}

// initialize the list control with currently file opened overlays.  Also,
// allow any registered 3rd party objects to add entries to this list
int CMissionPackageDlg::initialize_overlay_lst_ctrl()
{
   // obtain a list of overlays from the overlay manager
   CList <C_overlay *, C_overlay *> overlay_lst;
   OVL_get_overlay_manager()->get_overlay_list(overlay_lst);

   // loop through the list of overlays looking for file
   // overlays
   POSITION position = overlay_lst.GetHeadPosition();
   while (position)
   {
      C_overlay *overlay = overlay_lst.GetNext(position);

      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(overlay->get_m_overlayDescGuid());

      // if this overlay is a file overlay
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         // if the overlay has never been saved before, then don't add it to
         // the list
         long bHasBeenSaved = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);

         if (!bHasBeenSaved)
            continue;

         // get the overlay's icon and add it to the image list - currently
         // check for duplicate icons
         HICON hIcon = pOverlayTypeDesc->pIconImage->get_icon(16);
         int image_pos = m_images.Add(hIcon);

         // extract filename
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());

         CString file_spec = (char *)fileSpecification;
         CString file_name = extract_filename(file_spec);

         // add an item to the end of the overlay list
         m_overlay_lst_ctrl.InsertItem(m_overlay_lst_ctrl.GetItemCount(),
            file_name, image_pos);
      }
   }

   // initially, no items are selected so disable the "Add Overlay" button
   GetDlgItem(IDC_ADD_OVERLAY)->EnableWindow(FALSE);

   return SUCCESS;
}

// the overlay manager will call this whenver the overlay list needs to be
// updated
int CMissionPackageDlg::update_overlay_list()
{
   if (m_overlay_lst_ctrl.DeleteAllItems() == 0)
   {
      ERR_report("[CMissionPackageDlg::update_overlay_list] overlay "
         "list DeleteAllItems() failed");
      return FAILURE;
   }

   return initialize_overlay_lst_ctrl();
}

// message handler that gets called whenever an item is double-clicked on in
// the file overlay list.  Double-clicking an item will add the overlay to
// the mission package
void CMissionPackageDlg::OnOverlayListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
   if (m_overlay_lst_ctrl.GetSelectedCount() == 1)
      OnAddOverlay();

   *pResult = 0;
}

// message handler that gets called whenever the selection is changed in
// the file overlay list.  If no items are selected, we will disable the
// "Add Overlay" button.
void CMissionPackageDlg::OnOverlayListItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   const BOOL is_item_selected = m_overlay_lst_ctrl.GetSelectedCount() != 0;
   GetDlgItem(IDC_ADD_OVERLAY)->EnableWindow(is_item_selected);

   *pResult = 0;
}

// message handle that gets called whenever the selection is changed in
// the mission package file list.  Enabling of certain buttons depending on
// the state of the buttons selected
void CMissionPackageDlg::OnFileListItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   const BOOL is_item_selected = m_file_lst_ctrl.GetSelectedCount() != 0;

   int status_OK_count = 0;
   int status_NOT_FOUND_count = 0;
   int status_DISK_MORE_RECENT_count = 0;
   int status_PACKAGE_MORE_RECENT_count = 0;

   int item_index = -1;
   do
   {
      item_index = m_file_lst_ctrl.GetNextItem(item_index, LVNI_SELECTED);
      CString status = m_file_lst_ctrl.GetItemText(item_index, 1);

      if (item_index != -1)
      {
         // if the status is OK then open the file
         if (status == OK_STR)
            status_OK_count++;
         else if (status == NOT_FOUND_STR)
            status_NOT_FOUND_count++;
         else if (status == DISK_MORE_RECENT_STR)
            status_DISK_MORE_RECENT_count++;
         else if (status == PACKAGE_MORE_RECENT_STR)
            status_PACKAGE_MORE_RECENT_count++;
      }

   } while (item_index != -1);

   // the remove button is enabled if one or more items is selected
   GetDlgItem(IDC_REMOVE)->EnableWindow(is_item_selected);

   // the open button is enabled if any files are selected
   GetDlgItem(IDC_OPEN)->EnableWindow(is_item_selected);

   // the extract button is enabled if anything but OK is selected
   GetDlgItem(IDC_EXTRACT)->EnableWindow(is_item_selected);

   // the update button is enabled only if disk more recent is selected
   const BOOL only_more_recent_selected = (status_DISK_MORE_RECENT_count > 0) &&
      ((status_OK_count + status_NOT_FOUND_count + status_PACKAGE_MORE_RECENT_count) == 0);
   GetDlgItem(IDC_UPDATE)->EnableWindow(only_more_recent_selected);
}

void CMissionPackageDlg::OnAddOverlay() 
{
   // figure out which entry(s) are selected in the list control and
   // add them to the package
   int count_sucessfully_added = 0;
   int item_index = -1;
   do
   {
      item_index = m_overlay_lst_ctrl.GetNextItem(item_index,LVNI_SELECTED);

      // process selected item
      if (item_index != -1)
         add_overlay_hlpr(item_index, count_sucessfully_added);

   } while (item_index != -1);
}

// OnOpenOverlay will open an overlay from FalconView's File->Open handler and
// add this overlay to the package
void CMissionPackageDlg::OnOpenOverlay()
{
   C_overlay *overlay;
   if (OVL_get_overlay_manager()->open(overlay) != SUCCESS)
   {
      ERR_report("C_ovl_mgr::open() failed.");
      return;
   }

   // find the new overlay's index in the overlay list control and add
   // it to the package
   if (overlay != NULL)
   {
      int count_successfully_added = 0;

      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if ( pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         CString spec = (char *)fileSpecification;
         CString file_name = extract_filename(spec);
         for(int i=0;i<m_overlay_lst_ctrl.GetItemCount();i++)
         {
            if (m_overlay_lst_ctrl.GetItemText(i, 0).CompareNoCase(file_name) == 0)
            {
               add_overlay_hlpr(i, count_successfully_added);
               break;
            }
         }
      }
   }
}

// given a filename, this method will search through the list of opened
// overlays and return the full pathname
C_overlay *CMissionPackageDlg::get_overlay_full_pathname(CString filename)
{
   CList <C_overlay *, C_overlay *> overlay_lst;
   OVL_get_overlay_manager()->get_overlay_list(overlay_lst);
   POSITION position = overlay_lst.GetHeadPosition();

   CString full_pathname;
   while (position)
   {
      C_overlay *overlay = overlay_lst.GetNext(position);

      // if this overlay is a file overlay
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if ( pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         CString spec = (char *)fileSpecification;

         if (filename.CompareNoCase(extract_filename(spec)) == 0)
            return overlay;
      }
   }

   // should never get here
   ASSERT(0);
   return NULL;
}

// adds an overlay from the overlay list control with the given
// item index into the mission package
void CMissionPackageDlg::add_overlay_hlpr(int item_index, int &count_sucessfully_added)
{
   // retrieve filename from list control
   LVITEM item;
   memset(&item, 0, sizeof(LV_ITEM));
   item.iItem = item_index;
   item.mask = LVIF_IMAGE;

   CString filename = m_overlay_lst_ctrl.GetItemText(item_index, 0);
   m_overlay_lst_ctrl.GetItem(&item);

   // since we only placed the filename (and not complete path) in the list
   // control, we need to scan through the list of overlays to get the complete
   // path
   C_overlay *overlay = get_overlay_full_pathname(filename);
   if (overlay == NULL)
   {
      ERR_report("Unable to find overlay in list");
      return;
   }

   CString full_pathname;
   OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
   if ( pFvOverlayPersistence != NULL)
   {
      _bstr_t fileSpecification;
      pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
      full_pathname = (char *)fileSpecification;
   }

   // check to see if the overlay is dirty.  If so, then we will ask the user 
   // if they would like to save the overlay
   if (overlay->is_modified())
   {
      CString msg;
      msg.Format("%s has been modified.  Would you like to save the overlay before adding it to the package?", filename);
      int result = AfxMessageBox(msg, MB_YESNO);
      if (result == IDYES)
         OVL_get_overlay_manager()->save(overlay);
   }

   // unable to find overlay in overlay manager's list - may have been
   // closed at the same time that the overlay was added
   if (full_pathname == "")
      return;

   add_file_to_binder(full_pathname, item.iImage, count_sucessfully_added);

   // if we are dealing with a shape file, we should also add the additional
   // files associated with the shape file to the package
   CString extension = extract_extension(full_pathname);
   if (extension.CompareNoCase(".shp") == 0)
   {
      int nImageIndex;
      filename = remove_extension(full_pathname);

      if (FIL_access(filename + ".shx", FIL_EXISTS) == SUCCESS)
      {
         nImageIndex = get_icon_from_file(filename + ".shx");
         add_file_to_binder(filename + ".shx", nImageIndex, count_sucessfully_added);
      }
      if (FIL_access(filename + ".dbf", FIL_EXISTS) == SUCCESS)
      {
         nImageIndex = get_icon_from_file(filename + ".shx");
         add_file_to_binder(filename + ".dbf", nImageIndex, count_sucessfully_added);
      }
      if (FIL_access(filename + ".prf", FIL_EXISTS) == SUCCESS)
      {
         nImageIndex = get_icon_from_file(filename + ".prf");
         add_file_to_binder(filename + ".prf", nImageIndex, count_sucessfully_added);
      }
   }
}

void CMissionPackageDlg::add_file_to_binder(CString full_pathname, int image_index,
   int &count_sucessfully_added)
{
   // add file to binder data structure internally - will check for duplicates
   // and return NULL in this case
   binder_file_t *binder_file = m_binder.add_file(full_pathname);

   if (binder_file != NULL)
   {
      CWaitCursor wait_cursor;

      if (m_package_service->Zip(m_zip_name, full_pathname, TRUE, FALSE) != SUCCESS)
      {
         ERR_report("Zip operation, add, failed");

         CString msg;
         msg.Format("Unable to add file, %s, to zip.  %d files were "
            "sucessfully added.  Aborting add operation.", full_pathname, count_sucessfully_added);
         AfxMessageBox(msg);

         // remove the file that was added to the binder above.  No need to
         // destroy the thread because it has not yet been created [will be
         // created in call to add_to_file_lst below]
         const BOOL destroy_thread = FALSE;
         if (m_binder.remove_file(extract_filename(full_pathname), destroy_thread) != SUCCESS)
            ASSERT(0);

         return;
      }
      add_to_file_lst(image_index, binder_file);

      // we update binder after every file in case one of the zip operations fails.  This
      // will insure the TOC file stays consistent
      if (update_binder() != SUCCESS)
         AfxMessageBox("Warning : TOC not update sucessfully.  Mission package is inconsistent");

      count_sucessfully_added++;
   }
}

void CMissionPackageDlg::OnAddAllOverlays()
{
   int count_sucessfully_added = 0;
   for(int i=0;i<m_overlay_lst_ctrl.GetItemCount();i++)
      add_overlay_hlpr(i, count_sucessfully_added);
}

void CMissionPackageDlg::OnAddFiles() 
{
   // present the user with a File Open dialog and allow
   // them to choose any number of files to add to the binder
   static char szFilter[] = "All Files (*.*)|*.*||";
   CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | 
      OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, szFilter);
   dlg.m_ofn.lpstrTitle = "Add Files";

   if (dlg.DoModal() == IDOK)
   {
      POSITION position = dlg.GetStartPosition();
      int count_sucessfully_added = 0;
      while (position)
      {
         CString file_name = dlg.GetNextPathName(position);

         int image_pos = get_icon_from_file(file_name);
         if (image_pos == -1)
            image_pos = get_icon_from_file_hlpr(file_name);

         add_file_to_binder(file_name, image_pos, count_sucessfully_added);
      }
   }
}

// get the icon from the given file name and add it to the image list.  Return
// the index to the image list with the new icon or -1 if unsucessful
int CMissionPackageDlg::get_icon_from_file(CString& file_name)
{
   // loop through the factory list.  If the extension matches one of the overlay's
   // in the factory list, we will use that icon
   CString extension = extract_extension(file_name);

   OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension(extension);
   if (pOverlayTypeDesc != NULL)
      return m_images.Add(pOverlayTypeDesc->pIconImage->get_icon(16));

   // at this point, we couldn't find an overlay with the file's extension.  We will
   // use the Window's API to extract the icon from the filename
   //
   return get_icon_from_file_hlpr(extension);
}

int CMissionPackageDlg::get_icon_from_file_hlpr(const CString &extension)
{
   SHFILEINFO file_info;
   if (SHGetFileInfo(extension, FILE_ATTRIBUTE_NORMAL, &file_info, 
      sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES) == 0)
      return -1;

   ICONINFO info;
   int image_pos = -1;
   if (GetIconInfo(file_info.hIcon, &info) == 0)
      ERR_report("GetIconInfo failed");
   else
   {
      image_pos = m_images.Add(CBitmap::FromHandle(info.hbmColor), 
         CBitmap::FromHandle(info.hbmMask));
   }

   return image_pos;
}

// add a file to the binder and display it in the file list
void CMissionPackageDlg::add_to_file_lst(int image_index, binder_file_t *binder_file)
{
   if (!m_file_lst_ctrl.m_hWnd)
      return;

   LV_ITEM lv;

   CString file_name = binder_file->filename;

   // check for a duplicate file name - do not add it the file
   // name already exists in the binder
   for(int i=0;i<m_file_lst_ctrl.GetItemCount();i++)
   {
      if (m_file_lst_ctrl.GetItemText(i, 0).CompareNoCase(file_name) == 0)
         return;
   }

   // clear all items in the LV_ITEM struct
   memset(&lv, 0, sizeof(LV_ITEM));

   // the item index will be the current number of elements in the list
   // control since the items are zero-indexed
   int item_index = m_file_lst_ctrl.GetItemCount();

   // set the common fields
   lv.iItem = item_index;
   int ret = m_file_lst_ctrl.InsertItem(&lv);

   // a -1 for image_index will display no icon
   lv.mask = LVIF_IMAGE | LVIF_TEXT;
   lv.iSubItem = 0;
   lv.iImage = image_index;
   lv.pszText = file_name.GetBuffer(file_name.GetLength());
   ret = m_file_lst_ctrl.SetItem(&lv);

   // store a pointer to the binder associated with this item
   m_file_lst_ctrl.SetItemData(item_index, (DWORD)binder_file);

   // start a new thread to monitor the file's state (exists, missing, etc...)
   // the thread is responsible for freeing the memory allocated for
   // the parameters
   file_monitor_param_t *params = new file_monitor_param_t;
   params->binder_file = binder_file;

   // create an event that will be set when the thread should exit
   CString exit_thread_event_str;
   exit_thread_event_str.Format("Exit_Thread_%s", file_name);
   CreateEvent(NULL, TRUE, FALSE, exit_thread_event_str);

   AfxBeginThread(FileMonitorThread, (void *)params);
}

int CMissionPackageDlg::update_binder()
{
   CString toc_location = m_binder.write_TOC();

   // update the TOC file in the zip file
   int ret = m_package_service->Zip(m_zip_name, toc_location, FALSE, FALSE);

   remove(toc_location);

   return ret;
}

// based on the given compare flag (-1, 0, or 1), set the status text of
// the item with the given index in the list control
void UpdateListControl(CListCtrl *list_ctrl, int item_index, int cmp_flag)
{
   if (cmp_flag == -1)
      list_ctrl->SetItemText(item_index, 1, DISK_MORE_RECENT_STR);
   else if (cmp_flag == 0)
      list_ctrl->SetItemText(item_index, 1, OK_STR);
   else if (cmp_flag == 1)
      list_ctrl->SetItemText(item_index, 1, PACKAGE_MORE_RECENT_STR);
}

// Given two FILETIME structures, return -1 if a<b, 0 if a==b, and 1 if a>b
file_state_t CompareDate(FILETIME a, FILETIME b)
{
   CTime A(a);
   CTime B(b);

   if (A < B)
      return DISK_MORE_RECENT;

   if (A == B)
      return FILE_OK;

   // A > B
   return PACKAGE_MORE_RECENT;
}

// this procedure is called for each file added to the binder.  It keeps
// tabs on a particular file and updates the file list's status (now called comment) field
UINT FileMonitorThread(void *p)
{
   file_monitor_param_t *cast_p = (file_monitor_param_t *)p;
   file_monitor_param_t param = *cast_p;

   // the thread is responsible for freeing the memory allocated 
   // to the parameters object
   delete cast_p;

   // retrieve the name of the file that we will be monitoring
   CString file_name = param.binder_file->filename;

   // retrieve the directory from the filename (FalconView overlays will
   // be placed in the appropriate folder, all other files will reside 
   // in the MiscData folder
   CString directory_name = param.binder_file->current_dir;

   // construct the full path name of the file we are monitoring
   CString full_path = directory_name + "\\" + file_name;

   // check to see if the file exists and if so gets its
   // current filetime info
   WIN32_FIND_DATA find_data;
   HANDLE handle = FindFirstFile(full_path, &find_data);

   FILETIME last_write_time = find_data.ftLastWriteTime;

   // if it does exist, compare against binder date/time
   if (handle != INVALID_HANDLE_VALUE)
   {
      param.binder_file->file_state = CompareDate(param.binder_file->last_write_time, last_write_time);
      PostMessage(mission_binder_dlg_hWnd, WM_FILE_STATE_CHANGED, 0, 0);
      FindClose(handle);
   }
   // otherwise, the file doesn't exist.  so update the field in the 
   // list box accordingly
   else
   {
      param.binder_file->file_state = FILE_NOT_FOUND;
      PostMessage(mission_binder_dlg_hWnd, WM_FILE_STATE_CHANGED, 0, 0);
   }

   CString exit_thread_event_str;
   exit_thread_event_str.Format("Exit_Thread_%s", extract_filename(file_name));
   HANDLE exit_thread_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, exit_thread_event_str);
   ResetEvent(exit_thread_event);

   // we will use MsgWaitForMultipleObjects to detect file condition
   while (1)
   {
      CString dir = param.binder_file->current_dir;
      dir += "\\";

      CString full_path = dir + file_name;

      HANDLE change_handle = FindFirstChangeNotification(dir,
         FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);

      // wait for a change to the directory containing the file or for the
      // exit thread event
      //
      DWORD result;

      if (change_handle == INVALID_HANDLE_VALUE)   // directory doesn't exist
         result = WaitForSingleObject(exit_thread_event, 500);
      else
      {
         HANDLE handles[2];
         handles[0] = exit_thread_event;
         handles[1] = change_handle;

         result = WaitForMultipleObjects(2, handles, FALSE, 500);
      }

      // exit thread event was signalled
      if (result == WAIT_OBJECT_0)
      {
         CString finish_thread_event_str;
         finish_thread_event_str.Format("Finish_Thread_%s", extract_filename(file_name));
         HANDLE finish_thread_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, finish_thread_event_str);
         SetEvent(finish_thread_event);

         FindCloseChangeNotification(change_handle);

         return 0;
      }

      // some file in the directory we monitored was changed.  Let's 
      // check to see if it was the file we were watching
      else if (result == WAIT_OBJECT_0 + 1)
      {
         HANDLE handle = FindFirstFile(full_path, &find_data);

         if (handle != INVALID_HANDLE_VALUE)
         {
            param.binder_file->file_state = CompareDate(param.binder_file->last_write_time, find_data.ftLastWriteTime);
            PostMessage(mission_binder_dlg_hWnd, WM_FILE_STATE_CHANGED, 0, 0);
         }
         else
         {
            param.binder_file->file_state = FILE_NOT_FOUND;
            PostMessage(mission_binder_dlg_hWnd, WM_FILE_STATE_CHANGED, 0, 0);
         }
      }

      FindCloseChangeNotification(change_handle);
   }

   return 0;
}

void CMissionPackageDlg::destroy_threads()
{
   m_binder.destroy_threads();
}

void CMissionPackageDlg::OnOK() 
{
   CDialog::OnOK();
   DestroyWindow();
}

void CMissionPackageDlg::OnCancel()
{
   CDialog::OnCancel();
   DestroyWindow();
}

void CMissionPackageDlg::PostNcDestroy() 
{
   // destroy all threads created
   destroy_threads();

   CDialog::PostNcDestroy();
   delete this;
   CMainFrame::m_mission_binder_dlg = NULL;
   mission_binder_dlg_hWnd = NULL;
}

void CMissionPackageDlg::OnClose() 
{
   CDialog::OnClose();
   DestroyWindow();
}

void CMissionPackageDlg::OnUpdate() 
{
   const int num_selected = m_file_lst_ctrl.GetSelectedCount();

   // loop through all the selected items in the file list
   int item_index = -1;
   do
   {
      item_index = m_file_lst_ctrl.GetNextItem(item_index, LVNI_SELECTED);

      if (item_index == -1)
         break;

      CString status = m_file_lst_ctrl.GetItemText(item_index, 1);

      binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(item_index);

      CString file_name = binder->filename;

      if (item_index != -1)
      {
         if (status == OK_STR)
         {
            CString msg;
            msg.Format("No need to update %s", file_name);
            AfxMessageBox(msg);
         }
         // the file on the disk is more recent, update the zip file accordingly
         else if (status == DISK_MORE_RECENT_STR)
         {
            binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(item_index);

            CString directory = binder->current_dir;
            CString full_path = directory + "\\" + file_name;
            CWaitCursor wait_cursor;
            if (m_package_service->Zip(m_zip_name, full_path, FALSE, FALSE) != SUCCESS)
            {
               CString msg;
               msg.Format("Zip operation, UPDATE, failed for %s", file_name);
               AfxMessageBox(msg);
            }
            else
            {
               // find the file in the binder
               binder_file_t *binder_file = m_binder.find_file(file_name);

               if (binder_file != NULL)
               {
                  // get the new date/time of the file in the binder
                  WIN32_FIND_DATA find_data;
                  HANDLE handle = FindFirstFile(full_path, &find_data);
                  binder_file->last_write_time = find_data.ftLastWriteTime;
                  FindClose(handle);

                  // update the TOC since the date/time of the file has changed
                  if (update_binder() != SUCCESS)
                     AfxMessageBox("Warning : TOC not updated sucessfully.  "
                     "Mission package is inconsistent");
                  else
                  {
                     binder_file->file_state = FILE_OK;
                     ::PostMessage(mission_binder_dlg_hWnd, WM_FILE_STATE_CHANGED, 0, 0);
                  }
               }
               else
                  ASSERT(0);
            }
         }
         else
         {
            CString msg;
            msg.Format("Unable to update %s from disk [status not valid]", file_name);
            AfxMessageBox(msg);
         }
      }

   } while (item_index != -1);
}

void CMissionPackageDlg::OnOpen() 
{
   const int num_selected = m_file_lst_ctrl.GetSelectedCount();

   // loop through all the selected items in the file list
   int item_index = -1;
   do
   {
      item_index = m_file_lst_ctrl.GetNextItem(item_index, LVNI_SELECTED);
      if (item_index != -1)
         open_hlpr(item_index);
   } while (item_index != -1);
}

void CMissionPackageDlg::open_hlpr(int item_index)
{
   CString status = m_file_lst_ctrl.GetItemText(item_index, 1);

   binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(item_index);

   CString file_name = binder->filename;

   // construct the full path from the filename
   CString full_path = binder->current_dir + "\\" + file_name;

   if (item_index != -1)
   {
      bool bExtractFileFromPackage = false;

      // if the status is OK then open the file
      if (status == OK_STR)
         bExtractFileFromPackage = false;
      else if (status == NOT_FOUND_STR)
         bExtractFileFromPackage = true;
      else if (status == DISK_MORE_RECENT_STR)
      {
         CString msg;
         msg.Format("Existing file on disk, %s, is newer than file in package.\n"
            "Would you like to overwrite the existing file?", file_name);
         int nRet = AfxMessageBox(msg, MB_YESNOCANCEL | MB_DEFBUTTON2);
         if (nRet == IDCANCEL)
            return;
         bExtractFileFromPackage = (nRet == IDYES);
      }
      else if (status == PACKAGE_MORE_RECENT_STR)
      {
         CString msg;
         msg.Format("File in package, %s, is newer than existing file on disk.\n"
            "Would you like to overwrite the existing file?", file_name);
         int nRet = AfxMessageBox(msg, MB_YESNOCANCEL);
         if (nRet == IDCANCEL)
            return;
         bExtractFileFromPackage = (nRet == IDYES);
      }
      // the status is not valid -- must be FILE_OK
      else
      {
         CString msg;
         msg.Format("Status not valid for %s", file_name);
         AfxMessageBox(msg);
      }

      // if the file should be extracted from the package
      if (bExtractFileFromPackage)
      {
         CString path = binder->current_dir;

         CWaitCursor wait_cursor;
         if (m_package_service->ExtractFile(m_zip_name, path, file_name) != SUCCESS)
         {
            CString msg;
            msg.Format("Zip operation, EXTRACT, failed for file %s", full_path);
            AfxMessageBox(msg);
         }

         // if an icon wasn't assigned previously, because the file didn't exist
         // on the system, assign an icon now
         LVITEM lv_item;
         memset(&lv_item, 0, sizeof(LV_ITEM));
         lv_item.iItem = item_index;
         lv_item.mask = LVIF_IMAGE;
         m_file_lst_ctrl.GetItem(&lv_item);
         if (lv_item.iImage == -1)
         {
            lv_item.iImage = get_icon_from_file(file_name);
            m_file_lst_ctrl.SetItem(&lv_item);
         }
      }

      CString extension = extract_extension(file_name);

      OverlayTypeDescriptor *pOverlayTypeDesc = overlay_type_util::GetOverlayTypeFromFileExtension(extension);

      // if this file is one of the FalconView registered types then launch it using FalconView 
      // note, we'll let CFPS open routes via ShellExecute
      HINSTANCE h;
      if (pOverlayTypeDesc != NULL && pOverlayTypeDesc->overlayDescriptorGuid != FVWID_Overlay_Route)
      {
         C_overlay *ret_overlay;
         OVL_get_overlay_manager()->OpenFileOverlay(pOverlayTypeDesc->overlayDescriptorGuid, full_path, ret_overlay);
      }
      else
         h = ShellExecute(NULL, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
   }
}

void CMissionPackageDlg::OnOpenAll()
{
   for (int index=0;index<m_file_lst_ctrl.GetItemCount(); index++)
      open_hlpr(index);
}

void CMissionPackageDlg::OnExtract() 
{
   CMissionPackageExtractDlg dlg;

   dlg.set_parent_dlg(this);
   dlg.set_binder(&m_binder);

   m_binder.clear_tagged();

   // mark items for extraction
   int item_index = -1;
   do
   {
      item_index = m_file_lst_ctrl.GetNextItem(item_index, LVNI_SELECTED);

      if (item_index == -1)
         break;

      binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(item_index);
      binder->tagged = true;
   } while(1);

   // store state of dirs in case the user cancels after changing
   // something in the extract dialog
   CList <CString, CString> old_dir_list;
   for(int i=0;i<m_file_lst_ctrl.GetItemCount();i++)
   {
      binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(i);
      old_dir_list.AddTail(binder->current_dir);
   }


   // open the extraction dialog to allow users to choose which paths
   // the files are extracted to
   if (dlg.DoModal() != IDOK)
   {
      // restore state of dirs in case they have been changed
      POSITION pos = old_dir_list.GetHeadPosition();
      int i = 0;
      while (pos)
      {
         binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(i++);
         binder->current_dir = old_dir_list.GetNext(pos);
      }

      return;
   }

   // re-write the TOC in case any directories have changed
   update_binder();

   const int num_selected = m_file_lst_ctrl.GetSelectedCount();

   // loop through all the selected items in the file list
   item_index = -1;
   do
   {
      item_index = m_file_lst_ctrl.GetNextItem(item_index, LVNI_SELECTED);

      if (item_index == -1)
         break;

      binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(item_index);

      CString status = m_file_lst_ctrl.GetItemText(item_index, 1);

      CString file_name = binder->filename;

      // construct full path name from file name
      CString full_path = binder->current_dir + "\\" + file_name;

      if (item_index != -1)
      {
         bool do_extract = true;

         if (do_extract)
         {
            CString path = binder->current_dir;

            CWaitCursor wait_cursor;
            if (m_package_service->ExtractFile(m_zip_name, path, file_name) != SUCCESS)
            {
               CString msg;
               msg.Format("Zip operation, EXTRACT, failed for file %s", full_path);
               AfxMessageBox(msg);
            }

            // if an icon wasn't assigned previously, because the file didn't exist
            // on the system, assign an icon now
            LVITEM lv_item;
            memset(&lv_item, 0, sizeof(LV_ITEM));
            lv_item.iItem = item_index;
            lv_item.mask = LVIF_IMAGE;
            m_file_lst_ctrl.GetItem(&lv_item);
            if (lv_item.iImage == -1)
            {
               lv_item.iImage = get_icon_from_file(file_name);
               m_file_lst_ctrl.SetItem(&lv_item);
            }
         }
      }

   } while (item_index != -1);
}

void CMissionPackageDlg::OnRemove() 
{
   const int num_selected = m_file_lst_ctrl.GetSelectedCount();

   int *item = NULL;

   // sanity check before malloc
   if (num_selected > 0)
      item = new int[num_selected];
   else
      return;

   // store the list of items that were selected so we can 
   // delete them in reverse order
   int i = 0;
   POSITION position = m_file_lst_ctrl.GetFirstSelectedItemPosition();
   while (position)
   {
      item[i++] = m_file_lst_ctrl.GetNextSelectedItem(position);
   }

   int num_sucessfully_removed = 0;
   do
   {
      on_remove_hlpr(item[--i], num_sucessfully_removed);
   } while (i);

   delete [] item;
}

void CMissionPackageDlg::on_remove_hlpr(int index, int &num_sucessfully_removed)
{
   CString file_name = m_file_lst_ctrl.GetItemText(index, 0);

   m_binder.remove_file(file_name);

   // remove item from the list control
   m_file_lst_ctrl.DeleteItem(index);

   // delete file from archive
   CWaitCursor wait_cursor;
   if (m_package_service->Zip(m_zip_name, file_name, FALSE, TRUE) != SUCCESS)
   {
      ERR_report("CMissionPackage::OnRemove] zip operation, delete, failed");

      CString msg;
      msg.Format("Unable to remove file from mission package, %s.  "
         "%d files were removed sucessfully", file_name, num_sucessfully_removed);
      AfxMessageBox(msg);

      return;
   }

   if (update_binder() != SUCCESS)
      AfxMessageBox("Warning : TOC not updated sucessfully.  Mission package is inconsistent");

   num_sucessfully_removed++;
}

void CMissionPackageDlg::OnRemoveAll()
{
   int num_sucessfully_removed = 0;
   for(int i=m_file_lst_ctrl.GetItemCount() - 1;i >= 0; i--)
      on_remove_hlpr(i, num_sucessfully_removed);
}

// CreateNew - create a new mission binder with the given filename
int CMissionPackageDlg::CreateNew(CString& filename)
{
   m_zip_name = filename;

   InitPackageService();

   m_found_favorite = FALSE;

   // if the dialog is already opened then we need to clear out data structures
   // and kill threads
   if (mission_binder_dlg_hWnd != NULL)
   {
      destroy_threads();
      m_binder.clear();
      initialize();
   }

   if (!m_zip_library_initialized)
   {
      ERR_report("[CMissionPackageDlg::CreateNew] zip library not initialized");
      return FAILURE;
   }

   return update_binder();
}

// Load - reads the given mission binder's TOC file and adds the entries
// to the file list
int CMissionPackageDlg::Load(CString& F)
{
   CString filename(F);

   // store zip name for use in title bar, etc...
   m_zip_name = filename;
   InitPackageService();

   if (!m_zip_library_initialized)
   {
      ERR_report("[CMissionPackageDlg::Load] zip library not initialized");
      return FAILURE;
   }

   // attempt to extract the table of contents into memory
   std::string buffer;
   if (m_package_service->ExtractToMemory(filename, "TOC", buffer) != SUCCESS)
   {
      CString msg;
      msg.Format("Unsuccessfuly extracted TOC from %s", filename);
      ERR_report(msg);
      return FAILURE;
   }

   // determine if a favorite is stored in the mission package and
   // set the controls accordingly
   // NOTE:  We only support FVX now, due to changes in the FV favorite format.
   std::string buffer2;
   if (m_package_service->ExtractToMemory(F, "_FVX", buffer2) != SUCCESS)
      m_found_favorite = FALSE;
   else
      m_found_favorite = TRUE;

   // if the dialog is already opened then we need to clear out data structures
   // and kill threads
   if (mission_binder_dlg_hWnd != NULL)
   {
      destroy_threads();
      m_binder.clear();
   }

   // read in the file names stored in the binder from the table
   // of contents and populate the file list control
   m_binder.read_TOC(const_cast<char *>(buffer.c_str()));

   // if the dialog is already opened, then we need to re-initialize it
   if (mission_binder_dlg_hWnd != NULL)
      initialize();

   return SUCCESS;
}

LONG CMissionPackageDlg::OnFileStateChanged(UINT, LONG)
{
   m_binder.on_file_state_changed(&m_file_lst_ctrl, this);

   return 0;
}

void CMissionPackageDlg::OnDblclkFileLst(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_file_lst_ctrl.GetSelectedCount() == 1)
      OnOpen();

   *pResult = 0;
}

void CMissionPackageDlg::OnStoreFavorite()
{
   // get current map
   CView *view = UTL_get_active_non_printing_view();
   ViewMapProj *map = NULL;
   if (view)
      map = UTL_get_current_view_map(view);
   if (map == NULL)
   {
      ERR_report("Unable to obtain current map");
      return;
   }

   FavoriteData favorite;
   const MapSpec* spec = map->spec();
   favorite.map_group_identity = 0;
   favorite.map_type = MapType(spec->source, spec->scale, spec->series);
   favorite.map_proj_params.type = spec->projection_type;
   favorite.map_proj_params.center = spec->center;
   favorite.map_proj_params.rotation = spec->rotation;
   favorite.map_proj_params.zoom_percent = spec->zoom_percent;

   if (m_store_open_overlays.GetCheck())
      FavoriteList::get_instance()->AddOpenedOverlaysToFavorite(favorite);

   // write to temp disk file as _FVX
   char tempFilename[MAX_PATH];
   ::GetTempPath(MAX_PATH, tempFilename);
   ::PathAppend(tempFilename, "_FVX");

   FavoriteList::get_instance()->WriteData(tempFilename, favorite);

   // store in zip file
   int ret = m_package_service->Zip(m_zip_name, tempFilename, FALSE, FALSE);

   // remove the temporary file
   remove(tempFilename);

   // let user know that the map settings have been stored successfully
   AfxMessageBox("Map settings have been successfully stored in the package"); 

   // update text
   GetDlgItem(IDC_FAV_TEXT)->SetWindowText(MAP_DISPLAY_SETTINGS_FOUND);

   // enable Goto button
   GetDlgItem(IDC_GOTO)->EnableWindow(TRUE);
}

void CMissionPackageDlg::OnGoto()
{
   // unzip the favorite data into the temp directory
   char temp_dir[MAX_PATH + 1];
   GetTempPath(MAX_PATH, temp_dir);
   if (m_package_service->ExtractFile(m_zip_name, temp_dir, "_FVX") != SUCCESS)
   {
      CString msg;
      msg.Format("Unsuccessfuly extracted _FVX from %s", m_zip_name);
      ERR_report(msg);
      return;
   }

   // get data from favorite
   CString favorite_filespec = temp_dir;
   favorite_filespec += "\\";
   favorite_filespec += "_FVX";
   FavoriteData favorite;
   FavoriteList::get_instance()->LoadData(favorite_filespec, favorite);

   // goto the favorite
   CMainFrame::GotoFavorite(&favorite);
}

void CMissionPackageDlg::OnMail()
{
   m_MAPI->send_mail(m_zip_name, extract_filename(m_zip_name));
}

void CMissionPackageDlg::OnHelp()
{
   // just translate the message into the AFX standard help command.
   // this is equivalent to hitting F1 with this dialog box in focus
   SendMessage(WM_COMMAND, ID_HELP, 0);
}

//-----------------------------------------------------------------------------
// CMissionPackage implementation
//

// read_TOC - read in the table of contents from the given filename and
// populate the object's internal data structures
int CMissionPackage::read_TOC(const char *TOC)
{
   char *token; 

   // the file is ascii.  The first line is the number of entries
   // in the toc file
   int num_entries;
   char* context;
   token = strtok_s(const_cast<char *>(TOC), "\r\n", &context);
   if (sscanf_s(token, "%d", &num_entries) != 1)
   {
      ERR_report("Unable to read number of entries in TOC file");
      return FAILURE;
   }

   // for each of the entries, read data and populate internal list
   for (int i=0;i<num_entries;i++)
   {
      // read filename
      token = strtok_s(NULL, "\r\n", &context);

      if (token == NULL)
         return FAILURE;

      // construct a new binder file struct
      binder_file_t *binder_file = new binder_file_t;

      // if successfull, populate binder file with information
      // obtained from the TOC and add the entry to the list
      if (binder_file != NULL)
      {
         CString file_name(token);

         binder_file->filename = file_name;
         binder_file->file_state = FILE_STATE_UNKNOWN;

         // read FILETIME
         DWORD low_date, high_date;
         token = strtok_s(NULL, " \r\n", &context);
         sscanf_s(token, "%X", &low_date);

         token = strtok_s(NULL, "\r\n", &context);
         sscanf_s(token, "%X", &high_date);

         token = strtok_s(NULL, "\r\n", &context);
         binder_file->current_dir = CString(token);

         binder_file->last_write_time.dwLowDateTime = low_date;
         binder_file->last_write_time.dwHighDateTime = high_date;

         m_file_lst.AddTail(binder_file);
      }
   }

   return SUCCESS;
}

// destructor
CMissionPackage::~CMissionPackage()
{
   clear();
}

void CMissionPackage::clear()
{
   while (!m_file_lst.IsEmpty())
      delete m_file_lst.RemoveTail();
}

binder_file_t *CMissionPackage::add_file(CString& file_name)
{
   // check for duplicates
   POSITION position = m_file_lst.GetHeadPosition();
   CString cmp_file = extract_filename(file_name);
   while (position)
   {
      if (m_file_lst.GetNext(position)->filename.CompareNoCase(cmp_file) == 0)
      {
         CString msg;
         msg.Format("%s already exists in the package", cmp_file);
         AfxMessageBox(msg);
         return NULL;
      }
   }

   binder_file_t *binder_file = new binder_file_t;

   if (binder_file != NULL)
   {
      binder_file->filename = extract_filename(file_name);
      binder_file->current_dir = extract_path(file_name);
      binder_file->file_state = FILE_STATE_UNKNOWN;

      // obtain the filetime
      WIN32_FIND_DATA find_data;
      HANDLE handle = FindFirstFile(file_name, &find_data);
      binder_file->last_write_time = find_data.ftLastWriteTime;
      FindClose(handle);

      m_file_lst.AddTail(binder_file);

      return binder_file;
   }

   return NULL;
}

void CMissionPackage::destroy_file_monitor(CString file_name)
{
   // make sure that we don't have a full path name but rather just the
   // filename
   ASSERT(extract_filename(file_name) == file_name);

   CString finish_thread_event_str;
   finish_thread_event_str.Format("Finish_Thread_%s", file_name);
   HANDLE finish_handle = CreateEvent(NULL, TRUE, FALSE, finish_thread_event_str);

   CString exit_thread_event_str;
   exit_thread_event_str.Format("Exit_Thread_%s", file_name);
   HANDLE exit_thread_event = OpenEvent(EVENT_ALL_ACCESS, FALSE, exit_thread_event_str);
   SetEvent(exit_thread_event);

   DWORD result = WaitForSingleObject(finish_handle, 1000);
   if (result == WAIT_TIMEOUT)
      ERR_report("Waiting for thread to exit timed out");

   CloseHandle(finish_handle);
   CloseHandle(exit_thread_event);
}

int CMissionPackage::remove_file(CString& file_name, BOOL destroy_thread /* = TRUE */)
{
   ASSERT(extract_filename(file_name) == file_name);

   // search for the file in the binder
   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder_file = m_file_lst.GetAt(position);

      if (binder_file->filename.CompareNoCase(file_name) == 0)
      {
         m_file_lst.RemoveAt(position);

         if (destroy_thread)
            destroy_file_monitor(binder_file->filename);

         delete binder_file;

         return SUCCESS;
      }

      m_file_lst.GetNext(position);
   }

   return FAILURE;
}

binder_file_t *CMissionPackage::find_file(CString& file_name)
{
   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder_file = m_file_lst.GetNext(position);

      if (binder_file->filename.CompareNoCase(file_name) == 0)
         return binder_file;
   }

   return NULL;
}

#ifdef _DEBUG

void CMissionPackage::DUMP()
{
   TRACE("TOC\n");
   TRACE("--------------------\n");

   if (m_file_lst.GetCount() == 0)
      TRACE("Empty\n");

   POSITION position = m_file_lst.GetHeadPosition();
   int i = 1;
   while (position)
   {
      binder_file_t *binder_file = m_file_lst.GetNext(position);
      TRACE("%d: %s\n", i++, binder_file->filename);
   }
}

#endif

// writes out a table of contents file (TOC) to the disk and returns
// the location of the file
CString CMissionPackage::write_TOC()
{
   char tempFilename[MAX_PATH];
   ::GetTempPath(MAX_PATH, tempFilename);
   ::PathAppend(tempFilename, "TOC");

   FILE *file = NULL;
   fopen_s(&file, tempFilename, "w");
   if (file == NULL)
   {
      ERR_report("Could not open TOC file for writing");
      return "";
   }

   // write the number of files in the binder
   fprintf(file, "%d\n", m_file_lst.GetCount());

   // write out each of the file names to the TOC and the file time.  Also,
   // write the directory that the file came from.  This directory is used
   // until the file is no longer found at that location.  If it is not found,
   // then the default directory for the file type is used instead.
   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder = m_file_lst.GetNext(position);

      CString file_name = binder->filename;

      fprintf(file, "%s\n", file_name);

      fprintf(file, "%X %X\n", binder->last_write_time.dwLowDateTime,
         binder->last_write_time.dwHighDateTime);

      fprintf(file, "%s\n", binder->current_dir);
   }

   // close the file
   fclose(file);

   return tempFilename;
}

int CMissionPackage::populate_list_ctrl(CMissionPackageDlg *dlg)
{
   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder = m_file_lst.GetNext(position);

      int image_pos = dlg->get_icon_from_file(binder->filename);
      dlg->add_to_file_lst(image_pos, binder);
   }

   return SUCCESS;
}

int CMissionPackage::populate_extract_list_ctrl(CMissionPackageDlg *dlg, CListCtrl *list_ctrl)
{

   if (!list_ctrl->m_hWnd)
      return SUCCESS;

   LV_ITEM lv;

   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder_file = m_file_lst.GetNext(position);

      CString file_name = binder_file->filename;

      if (binder_file->tagged)
      {

         // clear all items in the LV_ITEM struct
         memset(&lv, 0, sizeof(LV_ITEM));

         // the item index will be the current number of elements in the list
         // control since the items are zero-indexed
         int item_index = list_ctrl->GetItemCount();

         // set the common fields
         lv.iItem = item_index;
         int ret = list_ctrl->InsertItem(&lv);

         // a -1 for image_index will display no icon
         lv.mask = LVIF_IMAGE | LVIF_TEXT;
         lv.iSubItem = 0;
         lv.iImage = dlg->get_icon_from_file(binder_file->filename);
         lv.pszText = file_name.GetBuffer(file_name.GetLength());
         ret = list_ctrl->SetItem(&lv);

         CString dir = binder_file->current_dir;
         dir += "\\";

         lv.mask = LVIF_TEXT;
         lv.iSubItem = 1;
         lv.pszText = dir.GetBuffer(dir.GetLength());
         ret = list_ctrl->SetItem(&lv);

         list_ctrl->SetItemData(item_index, (DWORD)binder_file);
      }
   }

   return SUCCESS;
}

void CMissionPackage::destroy_threads()
{
   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder = m_file_lst.GetNext(position);
      destroy_file_monitor(binder->filename);
   }
}

// a file's state has changed, update the list control to reflect this
void CMissionPackage::on_file_state_changed(CListCtrl *list_ctrl, CMissionPackageDlg *parent)
{
   CString current_status;

   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
   {
      binder_file_t *binder = m_file_lst.GetNext(position);

      for (int i=0;i<list_ctrl->GetItemCount();i++)
      {
         if (list_ctrl->GetItemText(i, 0).CompareNoCase(binder->filename) == 0)
         {
            switch(binder->file_state)
            {
            case FILE_OK: 
               list_ctrl->SetItemText(i, 1, OK_STR); 
               break;
            case DISK_MORE_RECENT:
               list_ctrl->SetItemText(i, 1, DISK_MORE_RECENT_STR); 
               break;
            case PACKAGE_MORE_RECENT: 
               list_ctrl->SetItemText(i, 1, PACKAGE_MORE_RECENT_STR); 
               break;
            case FILE_NOT_FOUND: 

               // if the binder's state has change to not found, then
               // we will update the directory that that file should
               // be pointing to which will be the default directory for
               // the given file type
               current_status = list_ctrl->GetItemText(i, 1);
               if (current_status != CString(NOT_FOUND_STR))
               {
                  binder->current_dir = get_directory_from_filename(binder->filename);

                  if (parent->update_binder() != SUCCESS)
                     AfxMessageBox("Warning : TOC not updated sucessfully.  Mission package is inconsistent");
               }

               list_ctrl->SetItemText(i, 1, NOT_FOUND_STR);
               break;
            case FILE_STATE_UNKNOWN:
               list_ctrl->SetItemText(i, 1, "** ERROR **");
               break;
            }

            break;
         }
      }
   }
}

void CMissionPackageDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
   // need to figure out a way to receive mouse events when over 
   // enabled buttons.  I derived a class from CButton and allowed it
   // to set the help text, but it doesn't receive messages when disabled
   // and doesn't get a message for when the mouse leaves the button
   /*
   for(int i=0;i<NUM_BUTTONS;i++)
   {
   if (m_button_info[i].rect.PtInRect(point))
   {
   GetDlgItem(IDC_STATUS)->SetWindowText(m_button_info[i].text);
   return;
   }
   }


   GetDlgItem(IDC_STATUS)->SetWindowText("");
   */

   CDialog::OnMouseMove(nFlags, point);
}

void CMissionPackageDlg::OnKeydownFileLst(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

   if (pLVKeyDow->wVKey == VK_DELETE)
      OnRemove();

   *pResult = 0;
}

// clear the tagged state for each of the packages
void CMissionPackage::clear_tagged()
{
   POSITION position = m_file_lst.GetHeadPosition();
   while (position)
      m_file_lst.GetNext(position)->tagged = false;
}


BEGIN_MESSAGE_MAP(CPackageFileListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(CPackageFileListCtrl)
   ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
   ON_WM_CONTEXTMENU()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPackageFileListCtrl::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // send WM_CONTEXTMENU to self
   SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());

   // mark message as handled and suppress default handling
   *pResult = 1;
}

void CPackageFileListCtrl::OnContextMenu(CWnd* pWnd, CPoint ptMousePos) 
{
   // if Shift-F10
   if (ptMousePos.x == -1 && ptMousePos.y == -1)
      ptMousePos = (CPoint) GetMessagePos();

   ScreenToClient(&ptMousePos);

   if (GetSelectedCount() < 1)
      return;

   CMenu menu;
   menu.CreatePopupMenu();

   if (m_parent->GetDlgItem(IDC_UPDATE)->IsWindowEnabled())
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_UPDATE, "Update");
   if (m_parent->GetDlgItem(IDC_EXTRACT)->IsWindowEnabled())
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_EXTRACT, "Extract");
   if (m_parent->GetDlgItem(IDC_OPEN)->IsWindowEnabled())
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_OPEN, "Open");
   if (m_parent->GetDlgItem(IDC_REMOVE)->IsWindowEnabled())
      menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_REMOVE, "Remove");

   ClientToScreen(&ptMousePos);
   menu.TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, m_parent );
}

BEGIN_MESSAGE_MAP(CPackageOverlayListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(CPackageOverlayListCtrl)
   ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
   ON_WM_CONTEXTMENU()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPackageOverlayListCtrl::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   // send WM_CONTEXTMENU to self
   SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());

   // mark message as handled and suppress default handling
   *pResult = 1;
}

void CPackageOverlayListCtrl::OnContextMenu(CWnd* pWnd, CPoint ptMousePos) 
{
   // if Shift-F10
   if (ptMousePos.x == -1 && ptMousePos.y == -1)
      ptMousePos = (CPoint) GetMessagePos();

   ScreenToClient(&ptMousePos);

   if (GetSelectedCount() < 1)
      return;

   CMenu menu;
   menu.CreatePopupMenu();
   menu.AppendMenu(MF_STRING | MF_BYCOMMAND, IDC_ADD_OVERLAY, "Add Overlay");

   ClientToScreen(&ptMousePos);
   menu.TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, m_parent );
}
/////////////////////////////////////////////////////////////////////////////
// CMissionPackageExtractDlg dialog


CMissionPackageExtractDlg::CMissionPackageExtractDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CMissionPackageExtractDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CMissionPackageExtractDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CMissionPackageExtractDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMissionPackageExtractDlg)
   DDX_Control(pDX, IDC_FILE_LST, m_file_lst_ctrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMissionPackageExtractDlg, CDialog)
   //{{AFX_MSG_MAP(CMissionPackageExtractDlg)
   ON_BN_CLICKED(IDC_CHANGE_PATH, OnChange)
   ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LST, OnFileListItemchanged)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMissionPackageExtractDlg message handlers

void CMissionPackageExtractDlg::OnFileListItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
   const BOOL is_item_selected = m_file_lst_ctrl.GetSelectedCount() != 0;

   GetDlgItem(IDC_CHANGE_PATH)->EnableWindow(is_item_selected);
}

BOOL CMissionPackageExtractDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   // we want the list control to have full row select
   ListView_SetExtendedListViewStyle(m_file_lst_ctrl.GetSafeHwnd(), LVS_EX_FULLROWSELECT);

   // initialize file list control
   //

   LV_COLUMN lc;

   // initialize lc common entries
   memset(&lc, 0, sizeof(LV_COLUMN));
   lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
   lc.fmt = LVCFMT_LEFT;

   // initialize the File Name column
   lc.cx = 130;
   lc.pszText = "File Name";
   lc.iSubItem = 0;
   m_file_lst_ctrl.InsertColumn(1, &lc);

   // initialize the Status column
   lc.cx = 418;
   lc.pszText = "Extract To";
   lc.iSubItem = 1;
   m_file_lst_ctrl.InsertColumn(2, &lc);

   // initialize the image list used in both the overlay list
   // and the list of files
   m_file_lst_ctrl.SetImageList(&m_dlg->m_images, LVSIL_SMALL);

   // let the binder add items to the file list control
   m_file_lst_ctrl.DeleteAllItems();
   m_binder->populate_extract_list_ctrl(m_dlg, &m_file_lst_ctrl);

   GetDlgItem(IDC_CHANGE_PATH)->EnableWindow(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CMissionPackageExtractDlg::OnChange()
{
   // open a directory selection dialog box
   CFvwUtil *futil = CFvwUtil::get_instance();
   CString path = futil->get_path_from_user("Select path for extraction");

   // use the selected path in each of the selected files (change
   // the binder's current_dir)
   if (path != "")
   {
      int item_index = -1;
      do
      {
         item_index = m_file_lst_ctrl.GetNextItem(item_index, LVNI_SELECTED);

         if (item_index == -1)
            break;

         binder_file_t *binder = (binder_file_t *)m_file_lst_ctrl.GetItemData(item_index);

         binder->current_dir = path;

         // clear all items in the LV_ITEM struct
         LV_ITEM lv;
         memset(&lv, 0, sizeof(LV_ITEM));

         // set the common fields
         lv.iItem = item_index;
         lv.mask = LVIF_TEXT;
         lv.iSubItem = 1;
         lv.pszText = path.GetBuffer(path.GetLength());
         m_file_lst_ctrl.SetItem(&lv);

      } while(1);
   }
}

// InfoZipPackageService implementation
//

int InfoZipPackageService::Initialize()
{
   m_info_zip = new CInfoZip;

   return m_info_zip->Initialize();
}

int InfoZipPackageService::Zip(const char* archive_name, const char *file_name,
   int update, int delete_files)
{
   return m_info_zip->Zip(archive_name, file_name, update, delete_files);
}

int InfoZipPackageService::ExtractFile(const char* archive_name, const char* target_path,
   const char* file_name)
{
   return m_info_zip->ExtractFile(archive_name, target_path, file_name);
}

int InfoZipPackageService::ExtractToMemory(const char *archive_name,
   const char *file_name, std::string& buffer)
{
   unzip_buffer_t zip_buffer;
   zip_buffer.strlength = 0;
   zip_buffer.strptr = NULL;
   int ret = m_info_zip->ExtractToMemory(archive_name, file_name, &zip_buffer);
   
   if (ret == SUCCESS)
      buffer = (char *)zip_buffer.strptr;

   return ret;
}

InfoZipPackageService::~InfoZipPackageService()
{
   delete m_info_zip;
}

#if GOV_RELEASE
// XPlanPackageService implementation
//

int XPlanPackageService::Initialize()
{
   HRESULT hr = m_package_service.CreateInstance(
      xplan_package_service::CLSID_PackageService);
   return hr == S_OK ? SUCCESS: FAILURE;
}

int XPlanPackageService::Zip(const char* archive_name, const char *file_name,
   int update, int delete_files)
{
   try
   {
      m_package_service->Zip(_bstr_t(archive_name), _bstr_t(file_name), 
         update, delete_files);
   }
   catch (_com_error &e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   return SUCCESS;
}

int XPlanPackageService::ExtractFile(const char* archive_name,
   const char* target_path, const char* file_name)
{
   try
   {
      m_package_service->ExtractFile(_bstr_t(archive_name), 
         _bstr_t(target_path), bstr_t(file_name));
   }
   catch (_com_error &e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   return SUCCESS;
}

int XPlanPackageService::ExtractToMemory(const char *archive_name, 
   const char *file_name, std::string& buffer)
{
   try
   {
      SAFEARRAY *sa = m_package_service->ExtractToMemory
         (_bstr_t(archive_name), _bstr_t(file_name));

      if (!sa)
         return FAILURE;

      m_extract_to_mem_array.Attach(sa);
      buffer = (char *)m_extract_to_mem_array.AccessData();
      m_extract_to_mem_array.UnAccessData();
   }
   catch (_com_error &e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   return SUCCESS;
}

#endif // GOV_RELEASE
LRESULT CMissionPackageDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

