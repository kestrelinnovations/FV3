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

#if !defined(AFX_MISSIONBINDERDLG_H__FCE15CCC_0DFF_48CD_A781_27C799A6FBFB__INCLUDED_)
#define AFX_MISSIONBINDERDLG_H__FCE15CCC_0DFF_48CD_A781_27C799A6FBFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMissionPackageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMissonPackageDlg dialog

#include "InfoZip.h"
#include "SafeArray.h"

// forward declarations
class CMissionPackageDlg;
class MAPIWrapper;
class C_overlay;

enum file_state_t { FILE_STATE_UNKNOWN, FILE_OK, DISK_MORE_RECENT, PACKAGE_MORE_RECENT, FILE_NOT_FOUND };

// PackageService_Interface defines the interface that a package service
// needs to provide for the mission package dialog
class PackageService_Interface
{
public:
   virtual int Initialize() = 0;

   // update : true if updating zip file--overwrite only if newer
   // delete_files : true if deleting files from archive
   virtual int Zip(const char* archive_name, const char *file_name,
      int update, int delete_files) = 0;

   virtual int ExtractFile(const char* archive_name, const char* target_path,
      const char* file_name) = 0;
   virtual int ExtractToMemory(const char *archive_name, const char *file_name,
                              std::string& buffer) = 0;
   virtual ~PackageService_Interface() { }
};

class InfoZipPackageService : public PackageService_Interface
{
   // zip utilities
   CInfoZip *m_info_zip;

public:
   InfoZipPackageService() : m_info_zip(nullptr)
   {
   }
   virtual ~InfoZipPackageService();

   virtual int Initialize();
   virtual int Zip(const char* archive_name, const char *file_name,
      int update, int delete_files);
   virtual int ExtractFile(const char* archive_name, const char* target_path,
      const char* file_name);
   virtual int ExtractToMemory(const char *archive_name, const char *file_name,
                              std::string& buffer);
};

#ifdef GOV_RELEASE
class XPlanPackageService : public PackageService_Interface
{
   xplan_package_service::IPackageServicePtr m_package_service;

   // Used to store the results of ExtractToMemory
   ByteSafeArray m_extract_to_mem_array;

public:
   virtual int Initialize();
   virtual int Zip(const char* archive_name, const char *file_name,
      int update, int delete_files);
   virtual int ExtractFile(const char* archive_name, const char* target_path,
      const char* file_name);
   virtual int ExtractToMemory(const char *archive_name, const char *file_name,
                              std::string& buffer);
};
#endif // GOV_RELEASE

typedef struct
{
   CString filename;
   file_state_t file_state;
   FILETIME last_write_time;
   CString current_dir;
   bool tagged;
} binder_file_t;

// manages a mission binder file.  Provides operations to add, update, extract, and remove
class CMissionPackage
{
   // name of the zip file, e.g. aaa.xmp
   CString m_zip_name;

   // list of files in the binder
   CList<binder_file_t *, binder_file_t *> m_file_lst;

public:
   // destructor
   ~CMissionPackage();

   // read_TOC - read in the table of contents from the given filename and
   // populate the object's internal data structures
   int read_TOC(const char *TOC);

   int populate_list_ctrl(CMissionPackageDlg *dlg);
   int populate_extract_list_ctrl(CMissionPackageDlg *dlg, CListCtrl *file_lst);

   void destroy_threads();

   // destroy the file monitor associate with the given file
   void destroy_file_monitor(CString file_name);

   // writes out a table of contents file (TOC) to the disk and returns
   // the location of the file
   CString write_TOC();

   binder_file_t *add_file(CString& file_name);
   int remove_file(CString& file_name, BOOL destroy_thread = TRUE);
   binder_file_t *find_file(CString& file_name);

   void clear();

   // a file's state has changed, update the list control to reflect this
   void on_file_state_changed(CListCtrl *list_ctrl, CMissionPackageDlg *parent);

   // clear the tagged state for each of the packages
   void clear_tagged();

#ifdef _DEBUG
   // print a list of the file's in the binder
   void DUMP();
#endif
};

// file list tree control
class CPackageFileListCtrl : public CListCtrl
{
protected:
   afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);

   DECLARE_MESSAGE_MAP()

public:
   CMissionPackageDlg *m_parent;
};

class CPackageOverlayListCtrl : public CListCtrl
{
   protected:
   afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);

   DECLARE_MESSAGE_MAP()

public:
   CMissionPackageDlg *m_parent;
};

class CMissionPackageDlg : public CDialog
{
public:
   // standard constructor
   CMissionPackageDlg(CWnd* pParent = NULL);   

   // destructor
   virtual ~CMissionPackageDlg();

   int CreateNew(CString& filename);
   int Load(CString& filename);

   // add a file to the binder and display it in the file list
   void add_to_file_lst(int image_index, binder_file_t *binder_file);

   // get the icon from the given file name and add it to the image list.  Return
   // the index to the image list with the new icon or -1 if unsucessful
   int get_icon_from_file(CString& file_name);
   int get_icon_from_file_hlpr(const CString &full_path);

   // the overlay manager will call this whenever the overlay list needs to be
   // updated
   int update_overlay_list();

   // given a filename, this method will search through the list of opened
   // overlays and return the full pathname
   C_overlay *get_overlay_full_pathname(CString filename);

   void add_file_to_binder(CString full_pathname, int image_index, 
      int &count_sucessfully_added);

   int update_binder();


// Dialog Data
   //{{AFX_DATA(CMissionPackageDlg)
   enum { IDD = IDD_MISSION_PACKAGE };
        CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Mission_Package_Composer.htm";}

   CPackageOverlayListCtrl m_overlay_lst_ctrl;
   CPackageFileListCtrl m_file_lst_ctrl;
   CButton m_store_open_overlays;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMissionPackageDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CMissionPackageDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnAddOverlay();
        afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnOpenOverlay();
   afx_msg void OnAddAllOverlays();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnClose();
   afx_msg void OnAddFiles();
   afx_msg void OnUpdate();
   afx_msg void OnRemove();
   afx_msg void OnRemoveAll();
   afx_msg LONG OnFileStateChanged(UINT, LONG);
   afx_msg void OnOpen();
   afx_msg void OnOpenAll();
   afx_msg void OnExtract();
   afx_msg void OnDblclkFileLst(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDropFiles(HDROP hDropInfo);
   afx_msg void OnOverlayListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnOverlayListItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnFileListItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnStoreFavorite();
   afx_msg void OnGoto();
   afx_msg void OnMail();
   afx_msg void OnHelp();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnKeydownFileLst(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void initialize();
   void InitPackageService();

   // initialize the list control with currently opened overlays.  Also,
   // allow any registered 3rd party objects to add entries to this list
   int initialize_overlay_lst_ctrl();

   void destroy_threads();

   // adds an overlay from the overlay list control with the given
   // item index into the mission package
   void add_overlay_hlpr(int item_index, int &count_sucessfully_added);

   // called from both the Open handler and the Open All handler
   void open_hlpr(int item_index);

   void on_remove_hlpr(int index, int &num_sucessfully_removed);

private:

   CString m_zip_name;

   // Load detects whether or not a favorite is stored in the package
   BOOL m_found_favorite;

   CMissionPackage m_binder;

   PackageService_Interface* m_package_service;

   // messaging utilities
   MAPIWrapper *m_MAPI;

   bool m_zip_library_initialized;

public:

   // image list for overlay list as well as the file list
   CImageList m_images;
};

/////////////////////////////////////////////////////////////////////////////
// CMissionPackageExtractDlg dialog

class CMissionPackageExtractDlg : public CDialog
{
// Construction
public:
   CMissionPackageExtractDlg(CWnd* pParent = NULL);   // standard constructor

   void set_parent_dlg(CMissionPackageDlg *dlg) { m_dlg = dlg; }
   void set_binder(CMissionPackage *binder) { m_binder = binder; }

// Dialog Data
   //{{AFX_DATA(CMissionPackageExtractDlg)
   enum { IDD = IDD_MISSION_PACKAGE_EXTRACT };

   CListCtrl m_file_lst_ctrl;
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMissionPackageExtractDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CMissionPackageExtractDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnChange();
   afx_msg void OnFileListItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:

   // image list for overlay list as well as the file list
   CImageList m_images;
   CMissionPackageDlg *m_dlg;
   CMissionPackage *m_binder;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISSIONBINDERDLG_H__FCE15CCC_0DFF_48CD_A781_27C799A6FBFB__INCLUDED_)
