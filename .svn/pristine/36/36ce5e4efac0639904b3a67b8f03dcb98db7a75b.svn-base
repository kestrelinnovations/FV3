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

// FavoriteImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\fvw.h"
#include "FavoriteImportDlg.h"
#include "MapTpSel.h"   // MapTypeSelection
#include "..\getobjpr.h"
#include "mapx.h"
#include "..\favorite.h"
#include "..\Common\ComErrorObject.h"
#include "..\getobjpr.h"
#include "fvwutil.h"
#include "maps.h"


/////////////////////////////////////////////////////////////////////////////
// CFavoriteImportDlg dialog


CFavoriteImportDlg::CFavoriteImportDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CFavoriteImportDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFavoriteImportDlg)
   m_favorite_name = _T("");
   //}}AFX_DATA_INIT

   // create MapTypeSelection object for category and scale controls
   m_map_type = new MapTypeSelection(m_category, m_scale);
}

CFavoriteImportDlg::~CFavoriteImportDlg()
{
   delete m_map_type;
}


void CFavoriteImportDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFavoriteImportDlg)
   DDX_Control(pDX, IDC_SCALE_LIST, m_scale);
   DDX_Control(pDX, IDC_CATEGORY, m_category);
   DDX_Control(pDX, IDC_FOLDER_VIEW_TREE, m_treeBrowseCtrl);
   DDX_Text(pDX, IDC_FAVORITE_NAME, m_favorite_name);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFavoriteImportDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(CFavoriteImportDlg)
   ON_CBN_SELCHANGE(IDC_SCALE_LIST, OnSelchangeScaleList)
   ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategory)
   ON_BN_CLICKED(IDC_NEW_FOLDER, OnNewFolder)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CFavoriteImportDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CFavoriteImportDlg)
   ON_EVENT(CFavoriteImportDlg, IDC_FOLDER_VIEW_TREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavoriteImportDlg message handlers

BOOL CFavoriteImportDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   //
   // initialize the spec with the current view map
   //
   CView* view = UTL_get_active_non_printing_view();
   const MapProj* curr = UTL_get_current_view_map(view);
   m_map_spec.center.lat = curr->actual_center_lat();
   m_map_spec.center.lon = curr->actual_center_lon();
   m_map_spec.source = curr->source();
   m_map_spec.scale = curr->scale();
   m_map_spec.series = curr->series();
   m_map_spec.rotation = curr->actual_rotation();
   m_map_spec.zoom_percent = curr->actual_zoom_percent();

   // if the requested zoom percent was Native, then make sure we stay
   // at the Native zoom percent
   if (curr->requested_zoom_percent() == NATIVE_ZOOM_PERCENT)
      m_map_spec.zoom_percent = NATIVE_ZOOM_PERCENT;
   else if (curr->requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
      m_map_spec.zoom_percent = TO_SCALE_ZOOM_PERCENT;

   m_map_spec.projection_type = curr->projection_type();

   MapCategory category = MAP_get_category(m_map_spec.source);

   // initialize category list box
   m_map_type->set_category_list(category);

   // initialize scale list box
   m_map_type->set_scale_list(category, m_map_spec.source, m_map_spec.scale,
      m_map_spec.series, FALSE);

   m_folder_name = FavoriteList::GetFavoritesPath();

   CreateAllDirectories(_bstr_t(m_folder_name + "\\"));

   m_treeBrowseCtrl.SetRootDir(m_folder_name);
   m_treeBrowseCtrl.SetRootDisplayName("Favorites");
   m_treeBrowseCtrl.SetTargetDir(m_folder_name);
   m_treeBrowseCtrl.FillTree();

   // if there is one or more blank name entries, then we will enable the 
   // favorite "name (prefix):" edit box
   POSITION position = m_name_list->GetHeadPosition();
   GetDlgItem(IDC_FAVORITE_NAME)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_NAME_PREFIX)->ShowWindow(SW_HIDE);
   while (position)
   {
      if (!m_name_list->GetNext(position).GetLength())
      {
         GetDlgItem(IDC_NAME_PREFIX)->ShowWindow(SW_SHOW);
         GetDlgItem(IDC_FAVORITE_NAME)->ShowWindow(SW_SHOW);
         GetDlgItem(IDC_NAME_AUTO_GENERATED)->ShowWindow(SW_HIDE);
         break;
      }
   }

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CFavoriteImportDlg::OnSelchangeScaleList() 
{
}

void CFavoriteImportDlg::OnSelchangeCategory()
{
   MapCategory category;

   m_map_type->get_selected_category(category);
   const BOOL show_closest = FALSE;
   m_map_type->set_scale_list(category, show_closest);
}

void CFavoriteImportDlg::OnSelectedFolderViewTree(LPCTSTR strFolderName)
{
   m_folder_name = strFolderName;
}

void CFavoriteImportDlg::OnNewFolder() 
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   CCreateNewFolder dlg(this);
   if (dlg.DoModal() == IDOK)
   {
      CString new_path(m_folder_name + "\\" + dlg.get_folder_name());
      futil->ensure_directory(new_path);
      m_treeBrowseCtrl.FillTree();

      // add this new directory to our structure
      favorite_list_t *fav_list = FavoriteList::get_instance()->find_sub_list(m_folder_name);
      if (fav_list != NULL)
      {
         favorite_entry_t entry;

         entry.name = CString(dlg.get_folder_name());
         entry.path = new_path;
         entry.sub_list = new favorite_list_t;

         fav_list->AddTail(entry);
      }
   }
}

void CFavoriteImportDlg::OnOK() 
{
   UpdateData();

   int map_index;
   MapCategory category;
   MapSource mapSource;
   MapScale mapScale;
   MapSeries mapSeries;

   if (m_map_type->get_selected_map(category, map_index, mapSource, mapScale, mapSeries) == false)
      return;

   if (map_index >= 0)
   {
      m_map_spec.source = mapSource;
      m_map_spec.scale = mapScale;
      m_map_spec.series = mapSeries;
   }

   // construct map type string used in image name
   CString scale_str, source_str, category_str;
   {
      MapCategory category = MAP_get_category(m_map_spec.source);
      category_str = category.get_string();

      // if the series is not NULL it will be part of the string
      if (m_map_spec.series.get_string().IsEmpty())
         scale_str.Format("%s", m_map_spec.scale.get_string());
      else
         scale_str.Format("(%s) - %s", m_map_spec.series.get_string(), m_map_spec.scale.get_string());

      // remove colons since they cannot be part of a filename
      scale_str.Replace(':', ' ');
   }

   // add each of the locations in the list with the specified folder name
   // and favorite name to the favorites list
   //

   int result = IDYES;
   CString name;

   POSITION position = m_location_list->GetHeadPosition();
   POSITION pos2 = m_name_list->GetHeadPosition();
   int name_index = 1;
   while (position && pos2)
   {
      m_map_spec.center = m_location_list->GetNext(position);
      CString base_name = m_name_list->GetNext(pos2);
      int entry_index;

      // construct a unique name from the given base name if the name 
      // is not specified
      if (base_name.GetLength() == 0)
      {
         do 
         {
            name.Format("%s%d %s", m_favorite_name, name_index++, scale_str);

            // get the index into the favorite name with the same name.  Index will
            // be -1 if the given name does not exist in the list
            entry_index = FavoriteList::get_instance()->Find(name, m_folder_name);

         } while (entry_index != -1);
      }
      else
         // append the map scale to the name
         name = base_name + " " + scale_str;

      // make sure the name is still unique
      do
      {
         entry_index = FavoriteList::get_instance()->Find(name, m_folder_name);
         int count = 1;
         if (entry_index != -1)
         {
            CString str;
            str.Format("%s(%d)_%s", base_name, count++, scale_str);
            name = str;
         }
      } while (entry_index != -1);

      // replace invalid filename characters
      CString invalid_filename_chars("/\\:*?\"<>|");
      const int count = invalid_filename_chars.GetLength();
      for(int i=0;i<count;i++)
         name.Replace(invalid_filename_chars[i], ' ');

      // The index is used to determine where the item will be added 
      // to the list.  If index = -1, the item is added to the end of the list
      favorite_list_t *added_to_list;

      long map_group_identity = 0;
      MapView* map_view = fvw_get_view();
      if (map_view)
         map_group_identity = map_view->GetMapGroupIdentity();

      FavoriteData favorite;
      favorite.map_group_identity = map_group_identity;
      favorite.map_proj_params.type = m_map_spec.projection_type;
      favorite.map_type = MapType(m_map_spec.source, m_map_spec.scale,
         m_map_spec.series);
      favorite.map_proj_params.center = m_map_spec.center;
      favorite.map_proj_params.rotation = m_map_spec.rotation;
      favorite.map_proj_params.zoom_percent = m_map_spec.zoom_percent;

      FavoriteList::get_instance()->Add(-1, name, m_folder_name, favorite,
         FALSE, added_to_list);

      // save the favorites list to the registry
      FavoriteList::get_instance()->write_entry_to_disk(added_to_list, added_to_list->GetCount() - 1);
   }

   CDialog::OnOK();
}
LRESULT CFavoriteImportDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

