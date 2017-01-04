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

#ifndef FAVORITE_FAVORITE_H
#define FAVORITE_FAVORITE_H

#include "resource.h"
#include "geo_tool_d.h"
#include "overlay.h"
#include "maps_d.h"
#include "FalconView/Viewpoint3d.h"

#include "browseforfolder.h"

// foward declarations
class MapSpec;
class MapProj;
class MapView;
class MAPIWrapper;

/////////////////////////////////////////////////////////////////////////////
// FavoriteList

struct FavoriteData
{
   long map_group_identity;
   MapType map_type;
   MapProjectionParams map_proj_params;

   std::vector<GUID> overlay_types;
   std::vector<std::string> file_specifications;
};

typedef struct _favorite_entry_t_
{
   _favorite_entry_t_()
   { 
      sub_list = nullptr;
   }

   CString name;
   CString path;
   int index;
   FavoriteData favorite;

   CList <_favorite_entry_t_, _favorite_entry_t_ &> *sub_list;
} favorite_entry_t;

typedef CList <favorite_entry_t, favorite_entry_t &> favorite_list_t;

class FavoriteList
{
private:
   FavoriteList();
   ~FavoriteList();
   static FavoriteList *m_favorite_lst;
   HANDLE m_hEvents[2]; // 0 - exit thread, 1 - thread complete

public:
   friend class OrgFavoriteDlg;

   static FavoriteList *get_instance()
   {
      if (NULL == m_favorite_lst)
      {
         m_favorite_lst = new FavoriteList();
         m_favorite_lst->initialize_from_registry("Favorites");
      }

      return m_favorite_lst;
   }

   static void destroy_instance()
   {
      if (m_favorite_lst != NULL)
         delete m_favorite_lst;
   }

   static CString GetFavoritesPath();

private:
   CList <favorite_entry_t, favorite_entry_t &> m_fav_list;
   VOID UpdateFavoritesMenu();

public:
   // called whenever the directory or files in the favorites directory are modified
   void OnFavoritesChanged();

   // read in the favorites from the registry.  Returns SUCCESS/FAILURE.
   int initialize_from_registry(CString key);

   
   // write the favorite entry with the given index in the list to disk.
   // Returns SUCCESS/FAILURE.
   int write_entry_to_disk(favorite_list_t *fav_list, int index, int ordering_index = -1);

   // add an entry to the favorites list at the given index with the given
   // values.  If index is -1, then add the entry to the end of the list.  Returns
   // SUCCESS/FAILURE
   int Add(int index, CString name, CString folder_name,
      FavoriteData& favorite, boolean_t store_open_overlays,
      favorite_list_t *&added_to_list);
   BOOL Add_hlpr(CList<favorite_entry_t, favorite_entry_t &> *fav_list, 
      favorite_entry_t entry, favorite_list_t *&added_to_list);
   void AddOpenedOverlaysToFavorite(FavoriteData& favorite);

   // return an index to the entry in the favorites list with the given name.  Returns
   // -1 if the name is not in the list
   int Find(const CString &name, CString path = "");
   BOOL Find_hlpr(CList<favorite_entry_t, favorite_entry_t&> *fav_list, 
      const CString &name, const CString &path, int &index);

   int MoveEntry(favorite_list_t *fav_list, int selected_index, BOOL move_down, int &new_index);
   int MoveEntryToTop(favorite_list_t *fav_list, int selected_index, int &new_index);
   int MoveEntryToBottom(favorite_list_t *fav_list, int selected_index, int &new_index);

   int Sort(favorite_list_t *fav_list, BOOL ascending);

   // build the favorites menu from the list
   void init_menu(CMenu *pMenu);
   void init_menu_hlpr(CMenu *pMenu, CList<favorite_entry_t, favorite_entry_t&> *fav_list,
      UINT &menu_id, int &popup_index);

   // Get the full specification of the favorite at the given index
   CString GetFavoriteFilename(int index);

   int get_data_by_name(favorite_list_t *fav_list, const CString &name,
      FavoriteData& favorite);

   // load the favorite data from the given file
   int LoadData(const CString& file_name, FavoriteData& favorite);

   // Write the given favorite to disk
   int WriteData(const CString& file_name, FavoriteData& favorite);

   // return the name of the favorite at the given index
   CString get_name(int index);

   // returns the full path name of a given favorite
   CString get_path_name(int index);

   // get the number of favorites that are stored
   int get_count() { return m_fav_list.GetCount(); }

   // remove an element from the list and update the registry
   int delete_item(CList<favorite_entry_t, favorite_entry_t &> *fav_list, POSITION position);

   // remove a set of elements from the list and update the registry
   int delete_item_multi(CList<favorite_entry_t, favorite_entry_t &> *fav_list, std::vector<int>& vecSelected);

   int find_index(favorite_list_t *fav_list, int i);
   int find_index_hlpr(favorite_list_t *current_fav_list, 
      favorite_list_t *target_fav_list, int i, int &count);

   favorite_list_t *find_sub_list(const CString &path_name);
   favorite_list_t *find_sub_list_hlpr(favorite_list_t *fav_list, const CString &path_name);

   int rename_favorite(favorite_list_t *fav_list, int selected_index, CString new_name);

   int move_to_folder(favorite_list_t *fav_list, POSITION position, CString path_name);
   favorite_list_t *get_list_from_path_name(const CString &path_name);

   int move_to_folder(favorite_list_t *fav_list, std::vector<int>& vecSelected, CString path_name);

   void change_path_name(favorite_list_t *fav_list, CString path_name);

private:

   int read_in_favorites(CList<favorite_entry_t, favorite_entry_t&> *fav_list,
      CString favorite_path);

   BOOL get_entry_at_index(CList<favorite_entry_t, favorite_entry_t &> *list,
      int index, int &count,
      favorite_entry_t &entry);

   void set_entry_at_index(CList<favorite_entry_t, favorite_entry_t &> *list,
      int index, int &count,
      favorite_entry_t &set_entry,
      BOOL &done, favorite_list_t *&added_to_list);

   void delete_list(CList<favorite_entry_t, favorite_entry_t &> *list);

   int get_favorite_index(int selected_index);

   static CString OverlayDescGuidToString(GUID overlayDescGuid);
   static GUID StringToOverlayDescGuid(const CString& strClassName);
};

/////////////////////////////////////////////////////////////////////////////
// AddFavoriteDlg dialog



class AddFavoriteDlg : public CDialog
{
// Construction
public:
   AddFavoriteDlg(CWnd* pParent = NULL);   // standard constructor
   ~AddFavoriteDlg();

   static HWND ms_hWnd;

// Dialog Data
//{{AFX_DATA(AddFavoriteDlg)
   enum { IDD = IDD_FAVORITE_ADD };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Add_to_Favorites.htm";}

   CString m_center_str;
   CString m_name;
   CString m_maptype_str;
   CString m_projection_str;
   CString m_tilt_str;
   CString m_rotation_str;
   CString m_zoom_percent_str;
   CButton m_store_map_display_settings;
   CButton m_store_open_overlays;
   CButton m_ok_button;
   CBrowseForFolder m_treeBrowseCtrl;
//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddFavoriteDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(AddFavoriteDlg)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnCreateIn();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   afx_msg void OnNewFolder();
   afx_msg LPARAM OnFavoritesDirectoryChanged(WPARAM, LPARAM);
   afx_msg void OnCheckboxChanged();
   DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

protected:
   FavoriteData m_favorite;

   // flag stating whether the create in folder is being displayed or not
   BOOL m_show_create_in_folder;

   void show_create_in_folder(BOOL show);
   CString m_folder_name;
   CString m_root_folder_name;

public:
   void SetMapParams(long map_group_identity, MapType map_type,
      const MapProjectionParams& map_proj_params);
};

/////////////////////////////////////////////////////////////////////////////
// CFavoriteListCtrl window

class CFavoriteListCtrl : public CListCtrl
{
// Construction
public:
   CFavoriteListCtrl();

// Attributes
public:

// Operations
public:

   CImageList *CreateDragImageEx(LPPOINT lpPoint);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFavoriteListCtrl)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CFavoriteListCtrl();

   // Generated message map functions
protected:
   //{{AFX_MSG(CFavoriteListCtrl)
   // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// OrgFavoriteDlg dialog

class OrgFavoriteDlg : public CDialog
{
// Construction
public:
   OrgFavoriteDlg(CWnd* pParent = NULL);  // standard constructor
   ~OrgFavoriteDlg();  // destructor

   static HWND ms_hWnd;

// Dialog Data
   //{{AFX_DATA(OrgFavoriteDlg)
   enum { IDD = IDD_FAVORITE_ORG };
   CFavoriteListCtrl m_listctrl;
   CString m_center_str;
   CString m_projection_str;
   CString m_tilt_str;
   CString m_rotation_str;
   CString m_zoom_percent_str;
   CString m_maptype_str;
   CString m_fav_name_str;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(OrgFavoriteDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(OrgFavoriteDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangeList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnDelete();
   afx_msg void OnRename();
   afx_msg void OnPaint();
   afx_msg void OnGoto();
   afx_msg void OnGotoNewTab();
   afx_msg void OnBack();
   afx_msg void OnEndRename(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnCreateFolder();
   afx_msg void OnKeydownListctrl(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnMoveTo();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnBegindragListctrl(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMoveDown();
   afx_msg void OnMoveUp();
   afx_msg void OnSortAscending();
   afx_msg void OnSortDescending();
   afx_msg void OnMoveBottom();
   afx_msg void OnMoveTop();
   afx_msg void OnMailFavorite();
   afx_msg LPARAM OnFavoritesDirectoryChanged(WPARAM, LPARAM);
   afx_msg void OnDestroy();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   MapView *m_view;

   CDC m_memory_dc;
   CBitmap m_memory_bmp;

   // image list for favorite list (to show folders)
   CImageList m_images;

   CFont m_font;
   int m_show;

   favorite_list_t *m_current_list;
   CList<favorite_list_t *, favorite_list_t *> m_parent_list;
   CString m_current_path;

   CToolBar m_toolbar;

   // OLE Drag and Drop Support
   COleDropTarget m_drop_target;

   CImageList *m_drag_image;

   MAPIWrapper *m_pMapi;

private:
   void set_edit_controls(FavoriteData* favorite,
      boolean_t hide_maptype_str = FALSE);
   void draw_preview_map(FavoriteData* favorite);
   void initialize_list();
   void show_preview_window(int show);
   int get_favorite_index(int selected_index);
   void OnMoveTo(CString path_name);
   void select_first_item();
   void select_item(int index);
   VOID UpdateFavoritesMenu();

   // returns -1 if no favorite is selected or if a directory is selected
   int GetSelectedFavoriteIndex();

   // returns TRUE if map data is available at given favorite
   BOOL IsMapDataAvailable(int favoriteIndex);
};

/////////////////////////////////////////////////////////////////////////////
// CCreateNewFolder dialog

class CCreateNewFolder : public CDialog
{
// Construction
public:
   CCreateNewFolder(CWnd* pParent = NULL);   // standard constructor
   CString get_folder_name() { return m_edit_str; }

   // Dialog Data
   //{{AFX_DATA(CCreateNewFolder)
   enum { IDD = IDD_FAVORITE_NEW_FOLDER };
   CString m_edit_str;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CCreateNewFolder)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CCreateNewFolder)
   // NOTE: the ClassWizard will add member functions here
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFavoriteMoveTo dialog

class CFavoriteMoveTo : public CDialog
{
// Construction
public:
   CFavoriteMoveTo(CWnd* pParent = NULL);   // standard constructor

   CString get_path_name() { return m_folder_name; }

   // Dialog Data
   //{{AFX_DATA(CFavoriteMoveTo)
   enum { IDD = IDD_FAVORITE_MOVE_TO };
   CBrowseForFolder m_treeBrowseCtrl;
   // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CFavoriteMoveTo)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CFavoriteMoveTo)
   virtual BOOL OnInitDialog();
   afx_msg void OnSelectedFolderViewTree(LPCTSTR strFolderName);
   DECLARE_EVENTSINK_MAP()
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   CString m_folder_name;
};

#endif  // FAVORITE_FAVORITE_H