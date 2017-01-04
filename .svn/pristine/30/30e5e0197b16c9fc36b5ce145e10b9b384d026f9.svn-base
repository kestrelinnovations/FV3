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



#include "stdafx.h"
#include "favorite.h"
#include "map.h"
#include "mem.h"
#include "param.h"
#include "gps.h"
#include "services.h"
#include "getobjpr.h"  // for fvw_get_frame()
#include "MAPIWrapper.h"
#include "proj.h"
#include "MapViewTabsCtrl.h"
#include "wm_user.h"
#include "..\Common\ComErrorObject.h"
#include "..\FvCore\Include\GuidStringConverter.h"
#include "maps.h"
#include "MapEngineCOM.h"

#include "FalconView/GeospatialViewController.h"

// For FVWID_Overlay_ guids used to convert between legacy class name and overlay descriptor guid
//
#include "factory.h"
#include "ar_edit\factory.h"
#include "PrintToolOverlay\factory.h"
#include "catalog\factory.h"
#include "SkyViewOverlay\factory.h"
#include "pointex\factory.h"
#include "scalebar\factory.h"
#include "FctryLst.h"
#include "grid_map\factory.h"
#include "contour\factory.h"
#include "TAMask\factory.h"
#include "nitf\factory.h"
#include "shp\factory.h"
#include "MovingMapOverlay\factory.h"
#include "TacticalModel\factory.h"
#include "MapHandlerUtils.h"
#include "getobjpr.h"


using namespace std;

FavoriteList *FavoriteList::m_favorite_lst = NULL;

const char* FAVORITE_EXTENSION = ".fvx";

UINT FavoritesFileMonitor(void *p);

/////////////////////////////////////////////////////////////////////////////
// FavoriteList

class EntrySort
{
public:
   static short m_sort_type;

   favorite_entry_t entry;
   bool operator<(const EntrySort &e) const 
   { 
      if (m_sort_type == 0)
         return entry.index < e.entry.index; 
      else if (m_sort_type == 1)
         return entry.name.CompareNoCase(e.entry.name) < 0;
      //else if (m_sort_type == 2)
         return e.entry.name.CompareNoCase(entry.name) < 0;
   }
};

short EntrySort::m_sort_type = 0;

// constructor
FavoriteList::FavoriteList()
{
   // start a change notification thread for the favorites directory
   //
   /*
   m_hEvents[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
   m_hEvents[1] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
   ::ResetEvent(m_hEvents[0]);
   ::ResetEvent(m_hEvents[1]);

   AfxBeginThread(FavoritesFileMonitor, reinterpret_cast<void *>(m_hEvents));
   */

   // FavoritesFileMonitor is disabled until certain operations, e.g. moving a large collection of Favorites, can
   // temporarily ignore notifications from the monitor
}

CString FavoriteList::GetFavoritesPath()
{
   CString strFavoritePath = PRM_get_registry_string("Main", 
         "ReadWriteUserData");
   strFavoritePath += "\\Favorites";

   strFavoritePath.Replace("\\\\", "\\");

   return strFavoritePath;
}

UINT FavoritesFileMonitor(void *p)
{
   HANDLE hExitThread = reinterpret_cast<HANDLE *>(p)[0];
   HANDLE hThreadComplete = reinterpret_cast<HANDLE *>(p)[1];

   CString strFavoritePath = FavoriteList::GetFavoritesPath();

   HANDLE h = ::FindFirstChangeNotification(strFavoritePath, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
   if (h != INVALID_HANDLE_VALUE)
   {
      HANDLE handles[2];
      handles[0] = h;
      handles[1] = hExitThread;

      for(;;)
      {
         DWORD dwStatus = ::WaitForMultipleObjects(2, handles, FALSE, INFINITE);
         if (dwStatus == WAIT_OBJECT_0)
         {
            CMainFrame *pFrame = fvw_get_frame();
            if (pFrame != NULL)
               pFrame->PostMessage(WM_FAVORITES_DIRECTORY_CHANGED, 0, 0);

            if (!FindNextChangeNotification(handles[0]))
               break;
         }
         else
            break;
      }

      FindCloseChangeNotification(h);
   }
   ::SetEvent(hThreadComplete);
   return 0;
}

// destructor
FavoriteList::~FavoriteList()
{
   // FavoritesFileMonitor is disabled until certain operations, e.g. moving a large collection of Favorites, can
   // temporarily ignore notifications from the monitor
   /*
   SetEvent(m_hEvents[0]);
   WaitForSingleObject(m_hEvents[1], INFINITE);
   */

   delete_list(&m_fav_list);

   /*
   CloseHandle(m_hEvents[0]);
   CloseHandle(m_hEvents[1]);
   */
}

void FavoriteList::OnFavoritesChanged()
{
   initialize_from_registry("Favorites");
}

void FavoriteList::delete_list(CList<favorite_entry_t, favorite_entry_t &> *list)
{
   POSITION position = list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t entry = m_fav_list.GetNext(position);
      if (entry.sub_list != NULL)
      {
         delete_list(entry.sub_list);
         delete entry.sub_list;
         entry.sub_list = NULL;
      }
   }
   list->RemoveAll();
}

// read in the favorites from the registry.  Returns SUCCESS/FAILURE.
int FavoriteList::initialize_from_registry(CString key)
{
   // clear out the existing list
   delete_list(&m_fav_list);

   // instead of initializing from the registry we will now 
   // read all of the Favorites files from the user's Favorite 
   // directory and populate m_fav_list
   return read_in_favorites(&m_fav_list, GetFavoritesPath());
}

int FavoriteList::read_in_favorites(CList<favorite_entry_t, favorite_entry_t&> *fav_list,
                                    CString favorite_path)
{
   // get the first filename match
   WIN32_FIND_DATA findFileData;
   HANDLE h_dirSearch = FindFirstFile(favorite_path + "\\*.fvx", &findFileData);

   vector<EntrySort> entry_lst;
   vector<EntrySort> entry_no_index_lst;

   int ordering_index = 0;
   if (h_dirSearch != INVALID_HANDLE_VALUE)
   do
   {
      favorite_entry_t entry;

      // get the filename of the favorite to load
      CString file_name = favorite_path + "\\" + findFileData.cFileName;

      // remove extension and store the favorite name
      CString path(findFileData.cFileName);
      entry.name = path.Left(path.ReverseFind('.'));

      entry.path = favorite_path;

      LoadData(file_name, entry.favorite);

      EntrySort e;
      e.entry = entry;
      e.entry.index = ordering_index++;

      entry_lst.push_back(e);

   } while (FindNextFile(h_dirSearch, &findFileData));

   FindClose(h_dirSearch);

   // we've finished reading in the files, now sort the sortable 
   // entries
   const int entry_size = entry_lst.size();
   if (entry_size> 0)
   {
      sort(entry_lst.begin(), entry_lst.end());
      for(int i=0;i<entry_size;++i)
         fav_list->AddTail(entry_lst[i].entry);
   }

   int last_index = 1;
   if (entry_size > 0)
      last_index = fav_list->GetTail().index;

   const int entry_no_index_size = entry_no_index_lst.size();
   for(int i=0;i<entry_no_index_size;i++)
   {
      favorite_entry_t entry = entry_no_index_lst[i].entry;
      entry.index = ++last_index;
      fav_list->AddTail(entry);
   }

   // in addition, recursively add any subdirs to the list
   h_dirSearch = FindFirstFile(favorite_path + "\\*.*", &findFileData);

   // if the list is empty, there is nothing to do
   if (h_dirSearch == INVALID_HANDLE_VALUE)
      return SUCCESS;

   do
   {
      if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
         == FILE_ATTRIBUTE_DIRECTORY)
      {
         CString name(findFileData.cFileName);
         if (name != "." && name != "..")
         {
            favorite_entry_t entry;
            entry.name = CString(findFileData.cFileName);
            entry.sub_list = new CList<favorite_entry_t, favorite_entry_t &>;
            entry.path = favorite_path + "\\" + entry.name;
            
            fav_list->AddHead(entry);

            int ret = read_in_favorites(entry.sub_list, favorite_path + "\\" + entry.name);
            if (ret != SUCCESS)
            {
               CString msg;
               msg.Format("Error reading favorites from directory: %s", entry.name);
               ERR_report(msg);
            }
         }
      }

   } while (FindNextFile(h_dirSearch, &findFileData));

   FindClose(h_dirSearch);

   return SUCCESS;
}

// write the favorite entry with the given index in the list to disk.
// Returns SUCCESS/FAILURE.
int FavoriteList::write_entry_to_disk(favorite_list_t *fav_list, int index,
                                      int ordering_index /*=-1*/)
{
   POSITION position = fav_list->FindIndex(index);
   if (position)
   {
      favorite_entry_t entry = fav_list->GetAt(position);

      // make sure the favorite path exists before trying to write
      // a file to it
      CreateAllDirectories(_bstr_t(entry.path + "\\"));

      CString file_name = entry.path + "\\" + entry.name;
      file_name += FAVORITE_EXTENSION;

      // Write Favorite to file using the IFvFavorite interface
      WriteData(file_name, entry.favorite);
   }
   else
      return FAILURE;

   return SUCCESS;
}

// add an entry to the favorites list at the given index with the given
// values.  If index is -1, then add the entry to the end of the list.  Returns
// SUCCESS/FAILURE
int FavoriteList::Add(int index, CString name, CString folder_name,
   FavoriteData& favorite, boolean_t store_open_overlays,
   favorite_list_t *&added_to_list)
{
   if (store_open_overlays)
      AddOpenedOverlaysToFavorite(favorite);

   if (index == -1)
   {
      favorite_entry_t entry;

      // set the entry's values
      entry.name = name;
      entry.path = folder_name;
      entry.favorite = favorite;

      if (m_fav_list.GetSize() == 0)
         entry.index = 1;
      else
         entry.index = m_fav_list.GetSize() + 1;

      if (Add_hlpr(&m_fav_list, entry, added_to_list) == FALSE)
      {
         m_fav_list.AddTail(entry);
         added_to_list = &m_fav_list;
      }
   }
   else
   {
      // clear out existing entry if necessary
      favorite_entry_t existingEntry;
      int existingCount = 0;
      get_entry_at_index(&m_fav_list, index, existingCount, existingEntry);

      favorite_entry_t entry;
      entry.name = name;
      entry.path = folder_name;
      entry.favorite = favorite;
      
      int count = 0;
      BOOL done = FALSE;
      set_entry_at_index(&m_fav_list, index, count, entry, done, added_to_list);
   }

   UpdateFavoritesMenu();

   return SUCCESS;
}

void FavoriteList::AddOpenedOverlaysToFavorite(FavoriteData& favorite)
{
   favorite.overlay_types.clear();
   favorite.file_specifications.clear();

   CList<C_overlay *, C_overlay *> overlay_lst;
   OVL_get_overlay_manager()->get_overlay_list(overlay_lst);

   POSITION position = overlay_lst.GetHeadPosition();
   while (position)
   {
      C_overlay *overlay = overlay_lst.GetNext(position);
      CString file_spec;

      CString overlayDescGuidStr = OverlayDescGuidToString(
         overlay->get_m_overlayDescGuid());
      if (overlayDescGuidStr.IsEmpty())
         continue;

      // don't add a file overlay if it has not already been saved
      OverlayPersistence_Interface* pFvOverlayPersistence =
         dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         long bHasBeenSaved = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
         if (!bHasBeenSaved)
            continue;

         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         file_spec = (char *)fileSpecification;

         // don't modify file specs that aren't files (URLs, connect strings...)
         if (DoesFileExist(file_spec))
         {
            // replace any occurrences of the user data and the hd data paths
            // with the strings USER_DATA and HD_DATA.  This way, if favorites
            // are exported and shared across machines in which the location
            // of these directories differ, e.g. C:\pfps\data\... and 
            // d:\pfps\data\..., then the favorites will not break
            CString user_data = PRM_get_registry_string("Main", "USER_DATA");
            CString hd_data = PRM_get_registry_string("Main","HD_DATA");

            file_spec.MakeLower();
            user_data.MakeLower();
            hd_data.MakeLower();
            if (file_spec.Find(user_data) != -1)
            {
               file_spec = 
                  file_spec.Right(file_spec.GetLength() - user_data.GetLength());
               file_spec = "USER_DATA" + file_spec;
            }
            if (file_spec.Find(hd_data) != -1)
            {
               file_spec =
                  file_spec.Right(file_spec.GetLength() - hd_data.GetLength());
               file_spec = "HD_DATA" + file_spec;
            }
         }
      }

      favorite.overlay_types.push_back(overlay->get_m_overlayDescGuid());
      favorite.file_specifications.push_back(std::string(file_spec));
   }
}

BOOL FavoriteList::Add_hlpr(favorite_list_t *fav_list, 
                            favorite_entry_t new_entry,
                            favorite_list_t *&added_to_list)
{
   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t entry = fav_list->GetNext(position);

      if (entry.sub_list != NULL && entry.path.CompareNoCase(new_entry.path) == 0)
      {
         entry.sub_list->AddTail(new_entry);
         added_to_list = entry.sub_list;
         return TRUE;
      }
      else if (entry.sub_list != NULL)
      {
         if (Add_hlpr(entry.sub_list, new_entry, added_to_list) == TRUE)
            return TRUE;
      }
   }

   return FALSE;
}

// return an index to the entry in the favorites list with the given name.  Returns
// -1 if the name is not in the list
int FavoriteList::Find(const CString &name, CString path /* = "" */)
{
   if (path == "")
      path = GetFavoritesPath();

   // loop through the list looking for an entry with the given name.  If one is
   // found, then return the index
   int index = 0;
   if (Find_hlpr(&m_fav_list, name, path, index) == TRUE)
      return index;

   return -1;
}

BOOL FavoriteList::Find_hlpr(CList<favorite_entry_t, favorite_entry_t&> *fav_list, 
                        const CString &name, const CString &path, int &index)
{
   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t &entry = fav_list->GetNext(position);
      BOOL is_dir = (entry.sub_list != NULL);

      if (entry.sub_list != NULL)
      {
         if (Find_hlpr(entry.sub_list, name, path, index) == TRUE)
            return TRUE;
      }
      else if (entry.name.CompareNoCase(name) == 0 && entry.path.CompareNoCase(path) == 0)
         return TRUE;

      if (!is_dir)
         index++;
   }

   return FALSE;
}

int FavoriteList::MoveEntry(favorite_list_t *fav_list, int selected_index, BOOL move_down,
                            int &new_index)
{
   new_index = selected_index;
   POSITION position = fav_list->FindIndex(selected_index);
   if (position == NULL)
   {
      ERR_report("Invalid position");
      return FAILURE;
   }

   favorite_entry_t move_from_entry = fav_list->GetAt(position);
   POSITION from_pos = position;
   int index_to = selected_index;
   move_down ? fav_list->GetNext(position) : fav_list->GetPrev(position);
   while (position)
   {
      favorite_entry_t move_to_entry = fav_list->GetAt(position);
      index_to = index_to + ((move_down) ? 1 : -1);
      if (move_to_entry.sub_list != NULL)
      {
         move_down ? fav_list->GetNext(position) : fav_list->GetPrev(position);
         continue;
      }

      // swap indices
      int tmp = move_to_entry.index;
      move_to_entry.index = move_from_entry.index;
      move_from_entry.index = tmp;

      // swap entries in the list
      fav_list->SetAt(position, move_from_entry);
      fav_list->SetAt(from_pos, move_to_entry);

      // write the files to disk with the new ordering indices
      write_entry_to_disk(fav_list, index_to, move_from_entry.index);
      write_entry_to_disk(fav_list, selected_index, move_to_entry.index);

      new_index = index_to;

      break;
   }

   return SUCCESS;
}

int FavoriteList::MoveEntryToTop(favorite_list_t *fav_list, int selected_index, int &new_index)
{
   new_index = selected_index;
   POSITION position = fav_list->FindIndex(selected_index);
   if (position == NULL)
   {
      ERR_report("Invalid position");
      return FAILURE;
   }

   favorite_entry_t move_from_entry = fav_list->GetAt(position);
   POSITION from_pos = position;
   position = fav_list->GetHeadPosition();
   int index_to = 0;
   while(position && index_to < selected_index)
   {
      favorite_entry_t move_to_entry = fav_list->GetAt(position);
      if (move_to_entry.sub_list != NULL)
      {
         fav_list->GetNext(position);
         index_to++;
         continue;
      }

      // swap indices
      int tmp = move_to_entry.index;
      move_to_entry.index = move_from_entry.index;
      move_from_entry.index = tmp;

      // swap entries in the list
      fav_list->SetAt(position, move_from_entry);
      fav_list->SetAt(from_pos, move_to_entry);

      // write the files to disk with the new ordering indices
      write_entry_to_disk(fav_list, index_to, move_from_entry.index);
      write_entry_to_disk(fav_list, selected_index, move_to_entry.index);

      new_index = index_to;

      break;
   }

   return SUCCESS;
}

int FavoriteList::MoveEntryToBottom(favorite_list_t *fav_list, int selected_index, int &new_index)
{
   new_index = selected_index;
   POSITION position = fav_list->FindIndex(selected_index);
   if (position == NULL)
   {
      ERR_report("Invalid position");
      return FAILURE;
   }

   favorite_entry_t move_from_entry = fav_list->GetAt(position);
   POSITION from_pos = position;
   position = fav_list->GetTailPosition();
   int index_to = fav_list->GetCount() - 1;
   while(position && index_to > selected_index)
   {
      favorite_entry_t move_to_entry = fav_list->GetAt(position);
      if (move_to_entry.sub_list != NULL)
      {
         fav_list->GetPrev(position);
         index_to--;
         continue;
      }

      // swap indices
      int tmp = move_to_entry.index;
      move_to_entry.index = move_from_entry.index;
      move_from_entry.index = tmp;

      // swap entries in the list
      fav_list->SetAt(position, move_from_entry);
      fav_list->SetAt(from_pos, move_to_entry);

      // write the files to disk with the new ordering indices
      write_entry_to_disk(fav_list, index_to, move_from_entry.index);
      write_entry_to_disk(fav_list, selected_index, move_to_entry.index);

      new_index = index_to;

      break;
   }

   return SUCCESS;
}

int FavoriteList::Sort(favorite_list_t *fav_list, BOOL ascending)
{
   vector<EntrySort> entry_lst;

   EntrySort::m_sort_type = (ascending) ? 1 : 2;

   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t entry = fav_list->GetNext(position);
      if (entry.sub_list != NULL)
         continue;

      EntrySort e;
      e.entry = entry;
      entry_lst.push_back(e);
   }

   sort(entry_lst.begin(), entry_lst.end());
   position = fav_list->GetHeadPosition();
   int ordering_index = 1;
   int index = -1;
   while (position)
   {
      index++;
      favorite_entry_t entry = fav_list->GetAt(position);
      if (entry.sub_list != NULL)
      {
         fav_list->GetNext(position);
         continue;
      }

      entry.index = ordering_index;
      fav_list->SetAt(position, entry_lst[ordering_index-1].entry);
      write_entry_to_disk(fav_list, index, ordering_index);
      
      ordering_index++;
      fav_list->GetNext(position);
   }

   EntrySort::m_sort_type = 0;

   return SUCCESS;
}

int FavoriteList::rename_favorite(favorite_list_t *fav_list, int selected_index, 
                                  CString new_name)
{
   POSITION position = fav_list->FindIndex(selected_index);
   if (position == NULL)
   {
      ERR_report("Invalid position");
      return FAILURE;
   }

   favorite_entry_t &entry = fav_list->GetAt(position);

   // delete old favorite file
   if (entry.sub_list == NULL)
   {
      if (!MoveFile(entry.path + "\\" + entry.name + ".fvx", entry.path + "\\" + new_name + ".fvx"))
         return FAILURE;

      entry.name = new_name;
   }
   // rename directory
   else
   {
      CString new_path = entry.path;
      new_path =  new_path.Left(new_path.ReverseFind('\\'));
      new_path += "\\" + new_name;
      if (!MoveFile(entry.path, new_path))
      {
         // MoveFile may have failed because the folder, entry.path, does not yet exist on disk.  In this
         // case, we just need to create the new directory
         CreateAllDirectories(_bstr_t(new_path + "\\"));
      }

      entry.name = new_name;
      entry.path = new_path;
   }

   UpdateFavoritesMenu();

   return SUCCESS;
}

int FavoriteList::move_to_folder(favorite_list_t *fav_list, std::vector<int>& vecSelected, CString path_name)
{
   std::vector<POSITION> vecPositions;

   // get a position for each of the items
   size_t i;
   for (i=0;i<vecSelected.size();i++)
      vecPositions.push_back(fav_list->FindIndex(vecSelected[i]));

   // now remove each of the elements in the list
   for(i=0;i<vecPositions.size();i++)
      move_to_folder(fav_list, vecPositions[i], path_name);

   return SUCCESS;
}

int FavoriteList::move_to_folder(favorite_list_t *fav_list, POSITION position,
                                 CString path_name)
{
   if (position == NULL)
      return FAILURE;

   favorite_entry_t entry = fav_list->GetAt(position);

   // delete old favorite file
   if (entry.sub_list == NULL)
   {
      if (!MoveFile(entry.path + "\\" + entry.name + ".fvx", path_name + "\\" + entry.name + ".fvx"))
      {
         DWORD error = GetLastError();
         ERR_report("Moving favorite failed");
         return FAILURE;
      }
   }
   else
   {
      if (!MoveFile(entry.path + "\\", path_name + "\\" + entry.name))
      {
         DWORD error_code = GetLastError();
         ERR_report("Unable to move directory");
         return FAILURE;
      }
   }

   fav_list->RemoveAt(position);
   entry.path = path_name;

   if (entry.sub_list != NULL)
      change_path_name(entry.sub_list, path_name);

   CString folder_name = path_name.Right(path_name.GetLength() - path_name.ReverseFind('\\') - 1);
   position = fav_list->GetHeadPosition();

   favorite_list_t *move_to_list = get_list_from_path_name(path_name);
   if (move_to_list != NULL)
      move_to_list->AddTail(entry);
   else
   {
      ERR_report("Could not find favorite list from path");
      return FAILURE;
   }

   return SUCCESS;
}

void FavoriteList::change_path_name(favorite_list_t *fav_list, CString path_name)
{
   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t &entry = fav_list->GetNext(position);
      if (entry.sub_list != NULL)
         change_path_name(entry.sub_list, path_name + "\\" + entry.name);
      else
         entry.path = path_name;
   }
}

favorite_list_t *FavoriteList::get_list_from_path_name(const CString &path)
{
   CString root_path = GetFavoritesPath() + "\\";
   CString path_name = path + "\\";

   root_path.MakeUpper();
   path_name.MakeUpper();
   path_name.Replace((LPCTSTR)root_path, "");

   favorite_list_t *fav_list = &m_fav_list;
   while (path_name.GetLength())
   {
      CString name = path_name;
      int index = path_name.Find("\\");
      if (index != -1)
          name = path_name.Left(index);

      POSITION position = fav_list->GetHeadPosition();
      while(position)
      {
         favorite_entry_t entry = fav_list->GetNext(position);
         if (entry.sub_list != NULL && entry.name.CompareNoCase(name) == 0)
         {
            fav_list = entry.sub_list;
            break;
         }
      }

      path_name.Replace(name + "\\", "");
   }

   return fav_list;
}

// build the favorites menu from the list
void FavoriteList::init_menu(CMenu *pMenu)
{
   pMenu->AppendMenu(MF_STRING, ID_FAVORITES_ORG, "Manage Favorites...");
   pMenu->AppendMenu(MF_SEPARATOR);

   UINT menu_id = IDRANGE_FAVORITE_MENU_ITEM_1;
   int popup_index = 0;
   init_menu_hlpr(pMenu, &m_fav_list, menu_id, popup_index);
}

void FavoriteList::init_menu_hlpr(CMenu *pMenu, CList<favorite_entry_t, favorite_entry_t&> *fav_list,
                                  UINT &menu_id, int &popup_index)
{
   UINT flags = MF_STRING | MF_BYCOMMAND;

   static CMenu popup[256];
   
   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t entry = fav_list->GetNext(position);

      if (entry.sub_list != NULL)
      {
         const int current_popup_index = popup_index;
         popup_index++;

         // if necessary, destroy the menu attached to a CMenu 
         // object and free any memory that the menu occupied
         if (popup[current_popup_index].m_hMenu)
            popup[current_popup_index].DestroyMenu();
         
         // create an empty menu and attach it to a CMenu object
         popup[current_popup_index].CreateMenu();

         init_menu_hlpr(&popup[current_popup_index], entry.sub_list, menu_id, popup_index);
         
         // append the submenu as a popup with the source name
         pMenu->AppendMenu(MF_POPUP, (UINT)popup[current_popup_index].m_hMenu, 
            entry.name);
      }
      else
      {
         pMenu->AppendMenu(flags, menu_id, entry.name);
         menu_id++;
      }
   }
   
   // if no items are in the Favorites list, then append a disabled "(Empty)" menu item
   if (fav_list->GetCount() == 0)
   {
      UINT flags = MF_STRING | MF_BYCOMMAND | MF_GRAYED | MF_DISABLED;
      pMenu->AppendMenu(flags, IDRANGE_FAVORITE_MENU_ITEM_EMPTY, "(Empty)");
   }
}

int FavoriteList::LoadData(const CString& file_name, FavoriteData& favorite)
{
   try
   {
      IFvFavoritePtr favorite_ptr;
      CO_CREATE(favorite_ptr, CLSID_FvFavorite);

      long group_id;
      _bstr_t source, series;
      double scale;
      MapScaleUnitsEnum scale_units;
      MapProjectionParams proj_params;
      double zoom_percent;
      SAFEARRAY* overlay_types_sa = nullptr;
      SAFEARRAY* file_specs_sa = nullptr;

      favorite_ptr->Read(_bstr_t(file_name), &group_id,
         source.GetAddress(), &scale, &scale_units, series.GetAddress(),
         &proj_params.center.lat, &proj_params.center.lon,
         &proj_params.rotation, &zoom_percent, &proj_params.type,
         &proj_params.tilt, &overlay_types_sa, &file_specs_sa);

      favorite.map_group_identity = group_id;
      favorite.map_type = MapType(MapSource((char *)source),
         MapScale(scale, scale_units), MapSeries((char *)series));
      favorite.map_proj_params = proj_params;
      favorite.map_proj_params.zoom_percent = zoom_percent;

      BstrSafeArray overlay_types, file_specs;
      overlay_types.Attach(overlay_types_sa);
      file_specs.Attach(file_specs_sa);

      int count = overlay_types.GetNumElements();
      for (int i=0; i<count; ++i)
      {
         string_utils::CGuidStringConverter guid(overlay_types[i]);
         favorite.overlay_types.push_back(guid.GetGuid());

         favorite.file_specifications.push_back((char *)_bstr_t(
            file_specs[i]));
      }
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   return SUCCESS;
}

// Write the given favorite to disk
int FavoriteList::WriteData(const CString& file_name, FavoriteData& favorite)
{
   try
   {
      IFvFavoritePtr favorite_ptr;
      CO_CREATE(favorite_ptr, CLSID_FvFavorite);

      BstrSafeArray overlay_types_sa, filespecs_sa;
      const size_t count = favorite.overlay_types.size();
      for (size_t i = 0; i < count; ++i)
      {
         string_utils::CGuidStringConverter guid(favorite.overlay_types[i]);
         overlay_types_sa.Append(
            _bstr_t(guid.GetGuidString().c_str()).Detach());
         filespecs_sa.Append(
            _bstr_t(favorite.file_specifications[i].c_str()).Detach());
      }

      if (favorite.map_type.get_source() == "")
      {
         favorite_ptr->WriteActiveOverlaysOnly(_bstr_t(file_name),
            &overlay_types_sa, &filespecs_sa);
      }
      else
      {
         favorite_ptr->Write(_bstr_t(file_name), favorite.map_group_identity,
            _bstr_t(favorite.map_type.get_source_string()),
            favorite.map_type.get_scale().GetScale(),
            favorite.map_type.get_scale().GetScaleUnits(),
            _bstr_t(favorite.map_type.get_series_string()),
            favorite.map_proj_params.center.lat,
            favorite.map_proj_params.center.lon,
            favorite.map_proj_params.rotation,
            favorite.map_proj_params.zoom_percent,
            favorite.map_proj_params.type, favorite.map_proj_params.tilt,
            &overlay_types_sa, &filespecs_sa);
      }
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
      return FAILURE;
   }

   return SUCCESS;
}

// get the data stored in the favorite data block
CString FavoriteList::GetFavoriteFilename(int index)
{
   favorite_entry_t entry;
   
   int count = 0;
   get_entry_at_index(&m_fav_list, index, count, entry);

   CString file_name = entry.path + "\\" + entry.name;
   file_name += FAVORITE_EXTENSION;
   return file_name;
}

int FavoriteList::get_data_by_name(favorite_list_t *fav_list,
   const CString &name, FavoriteData& favorite)
{
   // root
   if (fav_list == NULL)
      fav_list = &m_fav_list;

   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t entry = fav_list->GetNext(position);
      if (entry.name.CompareNoCase(name) == 0 && entry.sub_list == NULL)
      {
         favorite = entry.favorite;
         return SUCCESS;
      }
   }

   return FAILURE;
}

BOOL FavoriteList::get_entry_at_index(CList<favorite_entry_t, favorite_entry_t &> *list,
                                                  int index, int &count,
                                                  favorite_entry_t &entry)
{
   POSITION pos = list->GetHeadPosition();
   while(pos)
   {
      favorite_entry_t lcl_entry = list->GetNext(pos);
      const bool is_dir = lcl_entry.sub_list != NULL;

      if (index == count && !is_dir)
      {
         entry = lcl_entry;
         return TRUE;
      }

      if (is_dir)
      {
         if (get_entry_at_index(lcl_entry.sub_list, index, count, entry))
            return TRUE;
      }

      if (!is_dir)
         count++;
   }

   return FALSE;
}

void FavoriteList::set_entry_at_index(favorite_list_t *list,
                                                  int index, int &count,
                                                  favorite_entry_t &set_entry,
                                                  BOOL &done,
                                                  favorite_list_t *&added_to_list)
{
   POSITION pos = list->GetHeadPosition();
   while(pos)
   {
      favorite_entry_t &entry = list->GetNext(pos);
      const bool is_dir = entry.sub_list != NULL;

      if (is_dir)
         set_entry_at_index(entry.sub_list, index, count, set_entry, done, added_to_list);

      if (index == count && !done)
      {
         entry = set_entry;
         added_to_list = list;
         done = TRUE;
         return;
      }
      else if (index == count)
         return;

      if (!is_dir)
         count++;
   }
}

int FavoriteList::find_index(CList<favorite_entry_t, favorite_entry_t &> *fav_list, int i)
{
   int count = 0;
   return find_index_hlpr(&m_fav_list, fav_list, i, count);
}

int FavoriteList::find_index_hlpr(CList<favorite_entry_t, favorite_entry_t &> *current_fav_list, 
                                  CList<favorite_entry_t, favorite_entry_t &> *target_fav_list,
                                  int i, int &count)
{
   POSITION position = current_fav_list->GetHeadPosition();
   int lcl_count = 0;
   while (position)
   {
      favorite_entry_t entry = current_fav_list->GetNext(position);
      if (entry.sub_list != NULL)
      {
         int ret = find_index_hlpr(entry.sub_list, target_fav_list, i, count);
         if (ret != -1)
            return ret;
      }
      else
      {
         if (target_fav_list == current_fav_list && lcl_count == i)
            return count;
         count++;
      }

      lcl_count++;
   }

   return -1;
}

favorite_list_t *FavoriteList::find_sub_list(const CString &path_name)
{
   if (GetFavoritesPath().CompareNoCase(path_name) == 0)
      return &m_fav_list;

   return find_sub_list_hlpr(&m_fav_list, path_name);
}

favorite_list_t *FavoriteList::find_sub_list_hlpr(favorite_list_t *fav_list, const CString &path_name)
{
   POSITION position = fav_list->GetHeadPosition();
   while (position)
   {
      favorite_entry_t entry = fav_list->GetNext(position);
      if (entry.sub_list == NULL)
         continue;

      if (entry.path == path_name)
         return entry.sub_list;

      favorite_list_t *list = find_sub_list_hlpr(entry.sub_list, path_name);
      if (list != NULL)
         return list;
   }

   return NULL;
}

// return the name of the favorite at the given index
CString FavoriteList::get_name(int index)
{
   POSITION position = m_fav_list.FindIndex(index);
   
   if (position == NULL)
      return "ERROR - please report";
   
   return m_fav_list.GetAt(position).name;
}

CString FavoriteList::get_path_name(int index)
{
   POSITION position = m_fav_list.FindIndex(index);

   if (position == NULL)
      return "";

   CString path = GetFavoritesPath() + "\\" + m_fav_list.GetAt(position).name;
   return path + ".fvx";
}

// remove a single element from the list
int FavoriteList::delete_item(CList<favorite_entry_t, favorite_entry_t &> *fav_list,
                              POSITION position)
{
   if (position == NULL)
      return FAILURE;

   favorite_entry_t entry = fav_list->GetAt(position);

   const int is_dir = entry.sub_list != NULL;

   // delete the file or directory from the disk
   //

   if (is_dir)
   {
      if (!RemoveDirectory(entry.path))
      {
         LPVOID lpMsgBuf;
         DWORD rc = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

         CString msg;
         if (rc)
         {
            msg.Format("Failed deleting directory %s - %s", entry.path, (LPCTSTR)lpMsgBuf);
            LocalFree(lpMsgBuf);
         }
         else
            msg.Format("Failed deleting directory %s", entry.path);

         AfxMessageBox(msg);
         return FAILURE;
      }
   }
   else
   {
      if (!DeleteFile(entry.path + "\\" + entry.name + ".fvx"))
      {
         ERR_report("Failure deleting file");
         return FAILURE;
      }
   }

   fav_list->RemoveAt(position);

   return SUCCESS;
}

int FavoriteList::delete_item_multi(CList<favorite_entry_t, favorite_entry_t &> *fav_list,
                                    std::vector<int>& vecSelected)
{
   std::vector<POSITION> vecPositions;

   // get a position for each of the items
   size_t i;
   for (i=0;i<vecSelected.size();i++)
      vecPositions.push_back(fav_list->FindIndex(vecSelected[i]));

   // now remove each of the elements in the list
   for(i=0;i<vecPositions.size();i++)
      delete_item(fav_list, vecPositions[i]);

   UpdateFavoritesMenu();

   return SUCCESS;
}

CString FavoriteList::OverlayDescGuidToString(GUID overlayDescGuid)
{
   // certain overlay types will be excluded from being stored in a favorite
   if (overlayDescGuid == FVWID_Overlay_MapDataManager || 
       overlayDescGuid == FVWID_Overlay_MapServer || 
       overlayDescGuid == FVWID_Overlay_NitfFiles)
   {
      return "";
   }

   string_utils::CGuidStringConverter guidConv(overlayDescGuid);
   return guidConv.GetGuidString().c_str();
}

GUID FavoriteList::StringToOverlayDescGuid(const CString& overlayDescGuidStr)
{
   std::string s = std::string(overlayDescGuidStr);
   string_utils::CGuidStringConverter guidConv(s);
   GUID overlayDescGuid = guidConv.GetGuid();
   if (overlayDescGuid != GUID_NULL)
      return overlayDescGuid;

   // if the overlay descriptor is NULL then try to treat the string as an old class name (for backwards
   // compatibility with 4.1 favorites)
   //
   CString strClassName = overlayDescGuidStr;

   if (strClassName == "CRoute")
      return FVWID_Overlay_Route;
   if (strClassName == "C_gps_trail")
      return FVWID_Overlay_MovingMapTrail;
   if (strClassName == "C_ovlkit_ovl")
      return FVWID_Overlay_Drawing;
   if (strClassName == "C_ar_edit")
      return FVWID_Overlay_TrackOrbit;
   if (strClassName == "C_localpnt_ovl")
      return FVWID_Overlay_Points;
   if (strClassName == "C_chum_ovl")
      return FVWID_Overlay_ManualChum;
   if (strClassName == "C_threat_ovl")
      return FVWID_Overlay_Threat;
   if (strClassName == "TacticalGraphicsOverlay")
      return FVWID_Overlay_TacticalGraphics;
   if (strClassName == "CPrintToolOverlay")
      return FVWID_Overlay_PageLayout;
   if (strClassName == "Cmov_sym_overlay")
      return FVWID_Overlay_SkyView;
   if (strClassName == "CPointExportOverlay")
      return FVWID_Overlay_PointExport;
   if (strClassName == "CScaleBarOverlay")
      return FVWID_Overlay_ScaleBar;
   if (strClassName == "C_env_ovl")
      return FVWID_Overlay_Environmental;
   if (strClassName == "C_echum_ovl")
      return FVWID_Overlay_ElectronicChum;
   if (strClassName == "C_dropzone_ovl")
      return FVWID_Overlay_Dropzones;
   if (strClassName == "C_dafnav_ovl")
      return FVWID_Overlay_Navaids;
   if (strClassName == "C_dafheli_ovl")
      return FVWID_Overlay_Heliports;
   if (strClassName == "C_dafport_ovl")
      return FVWID_Overlay_Airports;
   if (strClassName == "C_dafats_ovl")
      return FVWID_Overlay_Airways;
   if (strClassName == "C_daffuel_ovl")
      return FVWID_Overlay_RefuelingRoutes;
   if (strClassName == "C_dafmtr_ovl")
      return FVWID_Overlay_MilitaryTrainingRoutes;
   if (strClassName == "C_dafway_ovl")
      return FVWID_Overlay_Waypoints;
   if (strClassName == "C_dafpara_ovl")
      return FVWID_Overlay_ParachuteJump;
   if (strClassName == "C_aimpnts_ovl")
      return FVWID_Overlay_Aimpoints;
   if (strClassName == "C_dafsuas_ovl")
      return FVWID_Overlay_SuasBoundaries;
   if (strClassName == "C_dafbnd_ovl")
      return FVWID_Overlay_AirspaceBoundaries;
   if (strClassName == "GridOverlay")
      return FVWID_Overlay_CoordinateGrid;
   if (strClassName == "C_vmap_ovl")
      return FVWID_Overlay_VectorSmartMap;
   if (strClassName == "C_dnc_ovl")
      return FVWID_Overlay_DigitalNauticalChart;
   if (strClassName == "C_wvs_ovl")
      return FVWID_Overlay_WorldVectorShoreline;
   if (strClassName == "C_shp_ovl")
      return FVWID_Overlay_ShapeFile;
   if (strClassName == "CShadowOvl")
      return FVWID_Overlay_Shadow;
   if (strClassName == "C_contour_ovl")
      return FVWID_Overlay_ContourLines;
   if (strClassName == "C_TAMask_ovl")
      return FVWID_Overlay_TerrainAvoidanceMask;
   if (strClassName == "C_model_ovl")
      return FVWID_Overlay_TacticalModel;

   return GUID_NULL;
}


VOID FavoriteList::UpdateFavoritesMenu()
{
   CMainFrame* pFrame = fvw_get_frame();
   if ( pFrame )
      pFrame->UpdateFavoritesMenu();
}


/////////////////////////////////////////////////////////////////////////////
// AddFavoriteDlg dialog

HWND AddFavoriteDlg::ms_hWnd = NULL;

AddFavoriteDlg::AddFavoriteDlg(CWnd* pParent /*=NULL*/)
   : CDialog(AddFavoriteDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(AddFavoriteDlg)
   m_center_str = _T("");
   m_name = _T("");
   m_maptype_str = _T("");
   m_projection_str = _T("");
   m_tilt_str = _T("");
   m_rotation_str = _T("");
   m_zoom_percent_str = _T("");
   //}}AFX_DATA_INIT

   m_show_create_in_folder = -1;
}

AddFavoriteDlg::~AddFavoriteDlg()
{
   PRM_set_registry_int("Favorites","ShowCreateInFolder", m_show_create_in_folder);
   ms_hWnd = NULL;
}

void AFXAPI DDV_ValidFilename(CDataExchange* pDX, CString filename)
{
   if (pDX->m_bSaveAndValidate)
   {
      if (filename.GetLength() > 100)
      {
         AfxMessageBox("The favorite name cannot exceed 100 characters");
         pDX->Fail();
      }

      CString invalid_filename_chars("/\\:*?\"<>|");
      for (int i=0;i<invalid_filename_chars.GetLength();i++)
      {
         const int index = filename.Find(invalid_filename_chars[i]);
         if (index != -1)
         {
            CString msg;
            msg.Format("A favorite name or directory cannot contain any of the following characters:\n\t\t%s", 
               invalid_filename_chars);
            AfxMessageBox(msg);
            pDX->Fail();
         }
      }
   }
}

void AddFavoriteDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AddFavoriteDlg)
   DDX_Text(pDX, IDC_CENTER, m_center_str);
   DDX_Text(pDX, IDC_MAPTYPE, m_maptype_str);
   DDX_Text(pDX, IDC_PROJECTION, m_projection_str);
   DDX_Text(pDX, IDC_TILT, m_tilt_str);
   DDX_Text(pDX, IDC_ROTATION, m_rotation_str);
   DDX_Text(pDX, IDC_ZOOM_PERCENT, m_zoom_percent_str);
   DDX_Control(pDX, IDC_STORE_MAP_DISPLAY_SETTINGS, m_store_map_display_settings);
   DDX_Control(pDX, IDC_STORE_OVERLAYS, m_store_open_overlays);
   DDX_Control(pDX, IDC_FOLDER_VIEW_TREE, m_treeBrowseCtrl);
   DDX_Control(pDX, IDOK, m_ok_button);
   //}}AFX_DATA_MAP
   DDX_Text(pDX, IDC_EDIT, m_name);
   DDV_ValidFilename(pDX, m_name);
}

void AddFavoriteDlg::SetMapParams(long map_group_identity,
   MapType map_type, const MapProjectionParams& map_proj_params)
{
   const int STR_LEN = 70;
   char center_s[STR_LEN];

   m_favorite.map_group_identity = map_group_identity;
   m_favorite.map_type = map_type;
   m_favorite.map_proj_params = map_proj_params;

   // map type
   {
      CString category_str, scale_str, source_str;

      // get the category string
      category_str = MAP_get_category_string(MAP_get_category(
         map_type.get_source()));

      // if the series is not NULL it will be part of the string
      if (map_type.get_series_string().IsEmpty())
         scale_str.Format("%s", map_type.get_scale_string());
      else
         scale_str.Format("%s (%s)", map_type.get_scale_string(),
              map_type.get_series_string());
      
      // if the source string is not the same as the category string,
      // it will be part of the string
      source_str = map_type.get_source_string();
      if (category_str != source_str)
      {
         CString string;
         
         string.Format("%s %s", source_str, scale_str);
         scale_str = string;
      }

      if (category_str == "Blank")
         m_maptype_str = category_str + " " + scale_str;
      else
         m_maptype_str = scale_str;
   }

   // center
   {
      GEO_lat_lon_to_string(map_proj_params.center.lat,
         map_proj_params.center.lon, center_s, STR_LEN);

      m_center_str = "Center: " + CString(center_s);
   }

   // rotation
   {
      CString rotation_str;
      double rotation = 360.0 - map_proj_params.rotation;
      if (rotation <= 360.0)
      {
         rotation_str.Format("%05.1lf\260", rotation);
      }
      else
         rotation_str.Format("000.0\260");
      
      m_rotation_str = "Rotation: " + rotation_str;
   }

   // zoom percent
   {
      CString zoom_str;
      
      if (map_proj_params.zoom_percent == NATIVE_ZOOM_PERCENT)
         zoom_str = "Zoom percent: Native";
      else if (map_proj_params.zoom_percent == TO_SCALE_ZOOM_PERCENT)
         zoom_str = "Zoom percent: To Scale";
      else
      {
         zoom_str.Format("Zoom percent: %d%\x025",
            map_proj_params.zoom_percent);
      }

      m_zoom_percent_str = zoom_str;
   }

   // projection
   {
      m_projection_str = "Projection: " +
         ProjectionID(map_proj_params.type).get_string();
   }

   if (map_proj_params.type == GENERAL_PERSPECTIVE_PROJECTION)
      m_tilt_str.Format("Tilt: %0.f", map_proj_params.tilt);
   else
      m_tilt_str = "";

   // setup a default name using the map type and center
   m_name = m_maptype_str;
   m_name += " ";
   m_name += center_s;

   // file names cannot have ':' or '"' in them, replace all occurences with a space
   m_name.Replace(':', ' ');
   m_name.Replace('\"', ' ');
}

BEGIN_MESSAGE_MAP(AddFavoriteDlg, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   //{{AFX_MSG_MAP(AddFavoriteDlg)
   ON_BN_CLICKED(IDC_CREATE_IN, OnCreateIn)
   ON_BN_CLICKED(IDC_NEW_FOLDER, OnNewFolder)
   ON_BN_CLICKED(IDC_STORE_MAP_DISPLAY_SETTINGS, OnCheckboxChanged)
   ON_BN_CLICKED(IDC_STORE_OVERLAYS, OnCheckboxChanged)
   ON_MESSAGE(WM_FAVORITES_DIRECTORY_CHANGED, OnFavoritesDirectoryChanged)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(AddFavoriteDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(AddFavoriteDlg)
   ON_EVENT(AddFavoriteDlg, IDC_FOLDER_VIEW_TREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddFavoriteDlg message handlers

BOOL AddFavoriteDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   ms_hWnd = m_hWnd;

   // initialize the checkboxes
   m_store_map_display_settings.SetCheck(1);
   m_store_open_overlays.SetCheck(
      PRM_get_registry_int("Favorites", "StoreOpenOverlays", 1));

   // get a pointer to the edit control
   CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT);

   // set the focus to the edit control
   GotoDlgCtrl(edit);

   // select all the characters in the edit control
   edit->SetSel(0,-1);

   show_create_in_folder(FALSE);
   show_create_in_folder(PRM_get_registry_int("Favorites","ShowCreateInFolder", FALSE));

   m_root_folder_name = m_folder_name = FavoriteList::GetFavoritesPath();
   CreateAllDirectories(_bstr_t(m_folder_name + "\\"));

   m_treeBrowseCtrl.SetRootDir(m_folder_name);
   m_treeBrowseCtrl.SetRootDisplayName("Favorites");
   m_treeBrowseCtrl.SetTargetDir(m_folder_name);
   m_treeBrowseCtrl.FillTree();

   return FALSE;  // return TRUE unless you set the focus to a control
}

void AddFavoriteDlg::OnCheckboxChanged()
{
   // disable the OK button if neither checkbox is selected
   m_ok_button.EnableWindow(
      m_store_map_display_settings.GetCheck() ||
      m_store_open_overlays.GetCheck() );
}

void AddFavoriteDlg::OnNewFolder() 
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   CCreateNewFolder dlg(this);
   if (dlg.DoModal() == IDOK)
   {
      CString new_path(m_folder_name + "\\" + dlg.get_folder_name());
      CreateAllDirectories(_bstr_t(new_path + "\\"));

      // add this new directory to our structure
      favorite_list_t *fav_list = FavoriteList::get_instance()->find_sub_list(m_folder_name);
      if (fav_list != NULL)
      {
         favorite_entry_t entry;

         entry.name = CString(dlg.get_folder_name());
         entry.path = new_path;
         entry.sub_list = new favorite_list_t;

         fav_list->AddTail(entry);

         // strip off the root directory to get a path relative to the root
         CString relativePath = new_path.Right(new_path.GetLength() - m_root_folder_name.GetLength() - 1);

         // select the newly created directory in the tree
         m_treeBrowseCtrl.SetTargetDir(relativePath);
         m_treeBrowseCtrl.FillTree();
      }
      else
      {
         // couldn't find the sub-list - internal data structure is out of sync
         ASSERT(0);
      }
   }
}

LPARAM AddFavoriteDlg::OnFavoritesDirectoryChanged(WPARAM, LPARAM)
{
   // traverse up a directory until we find a directory that exists
   if (FIL_access(m_folder_name, FIL_EXISTS) != SUCCESS)
   {
      do
      {
         m_folder_name = m_folder_name.Left(m_folder_name.ReverseFind('\\'));

      } while (m_folder_name.GetLength() != 0 && FIL_access(m_folder_name, FIL_EXISTS) != SUCCESS);
   }

   if (m_folder_name.CompareNoCase(m_root_folder_name) == 0 || m_folder_name.GetLength() == 0)
   {
      m_folder_name = m_root_folder_name;
      m_treeBrowseCtrl.SetTargetDir(m_root_folder_name);
   }
   // set target relative to root
   else
      m_treeBrowseCtrl.SetTargetDir(m_folder_name.Mid(m_root_folder_name.GetLength() + 1));

   m_treeBrowseCtrl.FillTree();
   return 0;
}

void AddFavoriteDlg::OnSelectedFolderViewTree(LPCTSTR strFolderName)
{
   m_folder_name = strFolderName;
}

void AddFavoriteDlg::OnCreateIn() 
{
   show_create_in_folder(!m_show_create_in_folder);
}

void AddFavoriteDlg::show_create_in_folder(BOOL show)
{
   GetDlgItem(IDC_CREATE_IN)->SetWindowText(show ? "Create in <<" : "Create in >>");

   if (m_show_create_in_folder != show)
   {
      UINT control_ids[] = { IDC_SEPARATOR,  IDC_MAPTYPE, IDC_CENTER, IDC_ROTATION, 
         IDC_ZOOM_PERCENT, IDC_PROJECTION, IDC_STORE_MAP_DISPLAY_SETTINGS, 
         IDC_STORE_OVERLAYS, IDOK, IDCANCEL, NULL };
      
      m_show_create_in_folder = show;

      CRect client_rect;
      GetDlgItem(IDC_FOLDER_VIEW_TREE)->GetClientRect(client_rect);
      int delta_height = client_rect.Height() + 10;

      GetDlgItem(IDC_CREATE_IN_TEXT)->ShowWindow(show ? SW_SHOW : SW_HIDE);
      GetDlgItem(IDC_NEW_FOLDER)->ShowWindow(show ? SW_SHOW : SW_HIDE);
      GetDlgItem(IDC_FOLDER_VIEW_TREE)->ShowWindow(show ? SW_SHOW : SW_HIDE);

      if (!show)
         delta_height = -delta_height;
      
      // move controls
      int index = 0;
      while(control_ids[index] != NULL)
      {
         CWnd *wnd = GetDlgItem(control_ids[index]);
         CRect rect;
         
         wnd->GetWindowRect(rect);
         ScreenToClient(rect);
         rect += CPoint(0, delta_height);
         wnd->MoveWindow(rect);
         
         index++;
      }
      
      // resize the dialog
      CRect dlg_screen_rect;
      GetWindowRect(dlg_screen_rect);
      MoveWindow(dlg_screen_rect.TopLeft().x, dlg_screen_rect.TopLeft().y,
         dlg_screen_rect.Width(), dlg_screen_rect.Height() + delta_height);
   }
}

void AddFavoriteDlg::OnOK() 
{
   int result = IDYES;

   // get the name entered from the edit box - if it is invalid then
   // get out of here, the focus will automatically be set to the
   // control with the invalid data
   if (UpdateData() == FALSE)
      return;

   // get whether or not we should save the map settings
   FavoriteData favorite_to_store = m_favorite;
   if (!m_store_map_display_settings.GetCheck())
   {
      // to indicate that the map settings should not be restored by
      // this favorite, we will use an empty source name
      favorite_to_store.map_type = MapType(MapSource(), MapScale());
   }

   // get whether or not we should store the currently opened overlays
   boolean_t store_open_overlays = m_store_open_overlays.GetCheck();

   // get the index into the favorite name with the same name.  Index will
   // be -1 if the given name does not exist in the list
   int index = FavoriteList::get_instance()->Find(m_name, m_folder_name);

   // if there is already a favorite with the same name, then ask the user
   // if they want to overwrite the existing favorite
   if (index != -1)
   {
      result = AfxMessageBox("The name you have entered already exists on your Favorites menu.  Would you like to overwrite it?",
         MB_YESNO);
   }

   // add a new item to the favorites list.  
   if (result == IDYES)
   {
      // The index is used to determine where the item will be added 
      // to the list.  If index = -1, the item is added to the end of the list
      favorite_list_t *added_to_list;
      FavoriteList::get_instance()->Add(index, m_name, m_folder_name,
         favorite_to_store, store_open_overlays, added_to_list);

      // save the favorites list to disk
      FavoriteList::get_instance()->write_entry_to_disk(added_to_list, added_to_list->GetCount() - 1);
   }

   // store the 'store open overlays' checkbox in the registry
   PRM_set_registry_int("Favorites", "StoreOpenOverlays", store_open_overlays);
   
   CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// OrgFavoriteDlg dialog

HWND OrgFavoriteDlg::ms_hWnd = NULL;

OrgFavoriteDlg::OrgFavoriteDlg(CWnd* pParent /*=NULL*/)
   : CDialog(OrgFavoriteDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(OrgFavoriteDlg)
   m_center_str = _T("");
   m_projection_str = _T("");
   m_tilt_str = _T("");
   m_rotation_str = _T("");
   m_zoom_percent_str = _T("");
   m_maptype_str = _T("");
   m_fav_name_str = _T("");
   //}}AFX_DATA_INIT

   m_drag_image = NULL;
   m_pMapi = new MAPIWrapper;
}

OrgFavoriteDlg::~OrgFavoriteDlg()
{
   if (m_drag_image)
   {
      m_drag_image->EndDrag();

      delete m_drag_image;
      m_drag_image = NULL;
   }
   delete m_pMapi;
   ms_hWnd = NULL;
}


void OrgFavoriteDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OrgFavoriteDlg)
   DDX_Control(pDX, IDC_LISTCTRL, m_listctrl);
   DDX_Text(pDX, IDC_CENTER, m_center_str);
   DDX_Text(pDX, IDC_PROJECTION, m_projection_str);
   DDX_Text(pDX, IDC_TILT, m_tilt_str);
   DDX_Text(pDX, IDC_ROTATION, m_rotation_str);
   DDX_Text(pDX, IDC_ZOOM_PERCENT, m_zoom_percent_str);
   DDX_Text(pDX, IDC_MAPTYPE, m_maptype_str);
   DDX_Text(pDX, IDC_FAV_NAME, m_fav_name_str);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OrgFavoriteDlg, CDialog)
   //{{AFX_MSG_MAP(OrgFavoriteDlg)
   ON_WM_CREATE()
   ON_WM_PAINT()

   ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL, OnSelchangeList)
   ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnDblclkList)
   ON_NOTIFY(LVN_KEYDOWN, IDC_LISTCTRL, OnKeydownListctrl)
   ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LISTCTRL, OnEndRename)
   ON_NOTIFY(LVN_BEGINDRAG, IDC_LISTCTRL, OnBegindragListctrl)

   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_RENAME, OnRename)
   ON_BN_CLICKED(IDC_GOTO, OnGoto)
   ON_BN_CLICKED(IDC_GOTO_NEW_TAB, OnGotoNewTab)
   ON_COMMAND(ID_BACK, OnBack)
   ON_BN_CLICKED(IDC_CREATE_FOLDER, OnCreateFolder)
   ON_BN_CLICKED(IDC_MOVE_TO, OnMoveTo)
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONUP()
   ON_COMMAND(ID_MOVE_DOWN, OnMoveDown)
   ON_COMMAND(ID_MOVE_UP, OnMoveUp)
   ON_COMMAND(ID_SORT_ASCENDING, OnSortAscending)
   ON_COMMAND(ID_SORT_DESCENDING, OnSortDescending)
   ON_COMMAND(ID_MOVE_BOTTOM, OnMoveBottom)
   ON_COMMAND(ID_MOVE_TOP, OnMoveTop)
   ON_COMMAND(ID_MAIL_FAVORITE, OnMailFavorite)
   ON_MESSAGE(WM_FAVORITES_DIRECTORY_CHANGED, OnFavoritesDirectoryChanged)
   ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OrgFavoriteDlg message handlers

BOOL OrgFavoriteDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   FavoriteList *fav_list = FavoriteList::get_instance();

   // create the memory dc used for the preview window
   //
   CRect preview_rect;
   CWnd *wnd = CWnd::FromHandle(GetDlgItem(IDC_PREVIEW)->m_hWnd);
   CDC *pDC = wnd->GetDC();
   wnd->GetClientRect(&preview_rect);
   
   m_memory_dc.CreateCompatibleDC(pDC);
   m_memory_bmp.CreateCompatibleBitmap(pDC, preview_rect.Width(), preview_rect.Height());
   m_memory_dc.SelectObject(&m_memory_bmp);
   
   // fill up the list control
   int num_favorites = fav_list->get_count();

   // initialize the image list used in both the overlay list
   // and the list of files
   m_images.DeleteImageList();

   const int icon_dim_X = 16;
   const int icon_dim_Y = 16;
   const int initial_size = 2;
   const int grow_by = 1;
   m_images.Create(icon_dim_X, icon_dim_Y, ILC_COLORDDB | ILC_MASK, 
      initial_size, grow_by);
   m_images.SetBkColor(RGB(255,255,255));
   m_listctrl.SetImageList(&m_images, LVSIL_SMALL);

   // load the favorite / folder icon into the image list
   HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_FAV));
   m_images.Add(hIcon);

   hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_FOLDER));
   m_images.Add(hIcon);

   m_current_list = &fav_list->m_fav_list;

   m_current_path = FavoriteList::GetFavoritesPath();

   initialize_list();
   select_first_item();
   
   m_listctrl.SetFocus();

   LOGFONT log_font;
   CFont *font = GetDlgItem(IDC_MAPTYPE)->GetFont();
   font->GetLogFont(&log_font);
   log_font.lfWeight = FW_BOLD;
   m_font.CreateFontIndirect(&log_font);
   GetDlgItem(IDC_FAV_NAME)->SetFont(&m_font);

   // add the tool bar to the dialog
   m_toolbar.Create(this);
   m_toolbar.LoadToolBar(IDR_MANAGE_FAV);
   m_toolbar.ShowWindow(SW_SHOW);
   m_toolbar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY);
   RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

   m_toolbar.GetToolBarCtrl().EnableButton(ID_BACK, FALSE);
   m_toolbar.GetToolBarCtrl().EnableButton(ID_MAIL_FAVORITE, m_pMapi->is_mail_available());

   m_show = -1;
   ms_hWnd = m_hWnd;

   return FALSE;  // return TRUE unless you set the focus to a control
}

void OrgFavoriteDlg::initialize_list()
{
   m_listctrl.SetRedraw(FALSE);

   m_listctrl.DeleteAllItems();
   
   POSITION position = m_current_list->GetHeadPosition();
   while(position)
   {
      favorite_entry_t entry = m_current_list->GetNext(position);

      const int icon_index = entry.sub_list != NULL ? 1 : 0;
      
      m_listctrl.InsertItem(m_listctrl.GetItemCount(), entry.name, icon_index);
      m_listctrl.SetItemData(m_listctrl.GetItemCount() - 1, (DWORD)entry.sub_list);
   }

   m_listctrl.SetRedraw(TRUE);
   m_listctrl.Invalidate();
}

void OrgFavoriteDlg::select_item(int index)
{
   if (index < 0 || index >= m_listctrl.GetItemCount())
   {
      select_first_item();
      return;
   }

   m_listctrl.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
   
   CString item_text = m_listctrl.GetItemText(index, 0);
         
   FavoriteData favorite;
   if (FavoriteList::get_instance()->get_data_by_name(m_current_list, item_text,
      favorite) == SUCCESS)
   {
      m_fav_name_str = item_text;
      set_edit_controls(&favorite);
      draw_preview_map(&favorite);
   }

   m_listctrl.EnsureVisible(index, FALSE);
}

void OrgFavoriteDlg::select_first_item()
{
   const int count = m_listctrl.GetItemCount();
   if (count == 0)
   {
      draw_preview_map(NULL);
      return;
   }

   BOOL selected_item = FALSE;
   for(int i=0;i<count;i++)
   {
      // if it is not a directory, then select it
      if (m_listctrl.GetItemData(i) == NULL)
      {
         select_item(i);
         selected_item = TRUE;

         break;
      }
   }

   // if no items were selected just select the first directory
   if (!selected_item)
      select_item(0);
}

// message handler that gets called whenever the selection is changed in
// the favorite list.  If exactly one item is selected, we will show
// the favorite in the preview window
void OrgFavoriteDlg::OnSelchangeList(NMHDR* pNMHDR, LRESULT* pResult)
{
   NMLISTVIEW *nmlistview = (NMLISTVIEW *)pNMHDR;

   if (!(nmlistview->uChanged & LVIF_STATE && nmlistview->uNewState & LVIS_SELECTED))
      return;

   // if there is only one selected item, then fill in the edit controls
   // and the preview window with the map spec and associated preview
   if (m_listctrl.GetSelectedCount() == 1)
   {
      FavoriteList *fav_list = FavoriteList::get_instance();
      int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
      
      const BOOL is_dir = m_listctrl.GetItemData(selected_index) != NULL;

      if (is_dir)
      {
         set_edit_controls(NULL);
         draw_preview_map(NULL);
      }
      else
      {
         CString item_text = m_listctrl.GetItemText(selected_index, 0);

         FavoriteData favorite;
         fav_list->get_data_by_name(m_current_list, item_text, favorite);
         
         m_fav_name_str = item_text;

         if (favorite.map_type.get_source_string().GetLength() > 0)
         {
            set_edit_controls(&favorite);
            draw_preview_map(&favorite);
         }
         // the favorite is only storing active overlays, there is no 
         // preview to be drawn
         else
         {
            set_edit_controls(NULL);
            draw_preview_map(NULL);

            m_maptype_str = "Only active overlays stored";
            UpdateData(FALSE);
         }
      }
      
      GetDlgItem(IDC_GOTO)->EnableWindow(!is_dir);
      GetDlgItem(IDC_GOTO_NEW_TAB)->EnableWindow(!is_dir);

      if (m_toolbar.m_hWnd)
      {
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_UP, !is_dir);
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_DOWN, !is_dir);
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_TOP, !is_dir);
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_BOTTOM, !is_dir);
      }
      GetDlgItem(IDC_RENAME)->EnableWindow(TRUE);
   }
   // otherwise, set the edit controls to say that multiple items are selected,
   // and draw 'NO PREVIEW' in the preview window
   else
   {
      set_edit_controls(NULL);
      draw_preview_map(NULL);

      GetDlgItem(IDC_GOTO)->EnableWindow(FALSE);
      GetDlgItem(IDC_GOTO_NEW_TAB)->EnableWindow(FALSE);
      GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);
      if (m_toolbar.m_hWnd)
      {
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_UP, FALSE);
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_DOWN, FALSE);
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_TOP, FALSE);
         m_toolbar.GetToolBarCtrl().EnableButton(ID_MOVE_BOTTOM, FALSE);
      }
   }
   
   CRect rect;
   GetDlgItem(IDC_PREVIEW)->GetWindowRect(&rect);
   ScreenToClient(rect);
   InvalidateRect(rect, FALSE);

   *pResult = 0;
}

int OrgFavoriteDlg::get_favorite_index(int selected_index)
{
   FavoriteList *fav_list = FavoriteList::get_instance();

   return fav_list->find_index(m_current_list, selected_index);
}

// message handler that gets called whenever an item is double-clicked on in
// the favorite list.  Double-clicking a folder will open that folder.  
// Double-clicking a favorite will goto that favorite
void OrgFavoriteDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
   if (m_listctrl.GetSelectedCount() == 1)
   {
      int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);

      // if this is not a directory
      if (m_listctrl.GetItemData(selected_index) == NULL)
         fvw_get_frame()->goto_favorite(get_favorite_index(selected_index));
      else
      {
         m_current_path += "\\" + m_listctrl.GetItemText(selected_index, 0);

         m_current_list = reinterpret_cast<CList<favorite_entry_t, favorite_entry_t&> *> (m_listctrl.GetItemData(selected_index));
         m_toolbar.GetToolBarCtrl().EnableButton(ID_BACK, TRUE);
         initialize_list();
         select_first_item();
      }
   }

   *pResult = 0;
}

// using the map spec's data, fill up the edit controls appropriately
void OrgFavoriteDlg::set_edit_controls(FavoriteData* favorite,
                                       boolean_t hide_maptype_str /*=FALSE*/)
{
   if (favorite == NULL)
   {
      m_fav_name_str = "No preview available";
      if (hide_maptype_str)
         m_maptype_str = "";

      m_maptype_str = "";
      m_center_str = "";
      m_projection_str = "";
      m_tilt_str = "";
      m_rotation_str = "";
      m_zoom_percent_str = "";
      
      UpdateData(FALSE);
      return;
   }

   // map type
   {
      CString category_str, scale_str, source_str;
      
      // get the category string
      category_str = MAP_get_category_string(MAP_get_category(
         favorite->map_type.get_source()));
      
      // if the series is not NULL it will be part of the string
      if (favorite->map_type.get_series_string().IsEmpty())
         scale_str.Format("%s", favorite->map_type.get_scale_string());
      else
         scale_str.Format("%s (%s)", favorite->map_type.get_scale_string(),
            favorite->map_type.get_series_string());
      
      // if the source string is not the same as the category string,
      // it will be part of the string
      source_str = favorite->map_type.get_source_string();
      if (category_str != source_str)
      {
         CString string;
         
         string.Format("%s %s", source_str, scale_str);
         scale_str = string;
      }
      
      if (category_str == "Vector" || category_str == "Blank")
         m_maptype_str = category_str + " " + scale_str;
      else
         m_maptype_str = scale_str;
   }
   
   // center
   {
      const int STR_LEN = 70;
      char center_s[STR_LEN];
      
      GEO_lat_lon_to_string(favorite->map_proj_params.center.lat,
         favorite->map_proj_params.center.lon, center_s, STR_LEN);
      
      m_center_str = "Center: " + CString(center_s);
   }
   
   // rotation
   {
      CString rotation_str;
      double rotation = 360.0 - favorite->map_proj_params.rotation;
      if (rotation <= 360.0)
      {
         rotation_str.Format("%05.1lf\260", rotation);
      }
      else
         rotation_str.Format("000.0\260");
      
      m_rotation_str = "Rotation: " + rotation_str;
   }
   
   // zoom percent
   {
      CString zoom_str;

      if (favorite->map_proj_params.zoom_percent == NATIVE_ZOOM_PERCENT)
         zoom_str = "Zoom percent: Native";
      else if (favorite->map_proj_params.zoom_percent == TO_SCALE_ZOOM_PERCENT)
         zoom_str = "Zoom percent: To Scale";
      else
         zoom_str.Format("Zoom percent: %d%\x025",
         favorite->map_proj_params.zoom_percent);

      m_zoom_percent_str = zoom_str;
   }
   
   // projection
   {
      m_projection_str = "Projection: " +
         ProjectionID(favorite->map_proj_params.type).get_string();
   }

   if (favorite->map_proj_params.type == GENERAL_PERSPECTIVE_PROJECTION)
      m_tilt_str.Format("Tilt: %0.f", favorite->map_proj_params.tilt);
   else
      m_tilt_str = "";

   UpdateData(FALSE);
}

void OrgFavoriteDlg::OnKeydownListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
   
   if (pLVKeyDow->wVKey == VK_DELETE)
      OnDelete();
   
   *pResult = 0;
}

void OrgFavoriteDlg::OnDelete()
{
   const int num_selected = m_listctrl.GetSelectedCount();

   std::vector<int> vecSelected;
   
   // store the list of items that were selected so we can 
   // delete them in reverse order
   int nNumFavoritesSelected = 0;
   int nNumDirectoriesSelected = 0;
   int i = 0;
   POSITION position = m_listctrl.GetFirstSelectedItemPosition();
   while (position)
   {
      const int nSelectedItem = m_listctrl.GetNextSelectedItem(position);
      POSITION pos = m_current_list->FindIndex(nSelectedItem);
      if (pos != NULL)
      {
         if (m_current_list->GetAt(pos).sub_list == NULL)
            nNumFavoritesSelected++;
         else
            nNumDirectoriesSelected++;
      }
      vecSelected.push_back(nSelectedItem);
   }

   if (vecSelected.size() == 0)
      return;

   CString str;
   if (nNumDirectoriesSelected == 0)
      str.Format("Are you sure you want to delete the selected favorite%s?", nNumFavoritesSelected > 1 ? "s" : "");
   else if (nNumFavoritesSelected == 0)
      str.Format("Are you sure you want to delete the selected director%s?", nNumDirectoriesSelected > 1 ? "ies" : "y");
   else
      str.Format("Are you sure you want to delete the selected items?");

   if (AfxMessageBox(str, MB_YESNO) == IDNO)
      return;

   // delete the items from the favorites list
   FavoriteList::get_instance()->delete_item_multi(m_current_list, vecSelected);

   // reload the list control's strings
   initialize_list();
   select_item(vecSelected[0]);

   Invalidate();
}

// returns -1 if no favorite is selected or if a directory is selected
int OrgFavoriteDlg::GetSelectedFavoriteIndex()
{
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index != -1)
   {
      if (m_listctrl.GetItemData(selected_index) == NULL)
         selected_index = get_favorite_index(selected_index);
   }

   return selected_index;
}

void OrgFavoriteDlg::OnGoto() 
{
   int favoriteIndex = GetSelectedFavoriteIndex();
   if (favoriteIndex != -1)
      fvw_get_frame()->goto_favorite(favoriteIndex);
}

// returns TRUE if map data is available at given favorite
BOOL OrgFavoriteDlg::IsMapDataAvailable(int favoriteIndex)
{
   CString favorite_filename =
      FavoriteList::get_instance()->GetFavoriteFilename(favoriteIndex);

   FavoriteData favorite;
   if (FavoriteList::get_instance()->LoadData(favorite_filename, favorite) !=
      SUCCESS)
   {
      return FALSE;
   }

   // initialize a local map engine
   MapEngineCOM map_engine;
   map_engine.init(FALSE, FALSE);

   ViewMapProj* mapProj = UTL_get_current_view_map();
   if (mapProj == NULL)
      return FALSE;
   map_engine.set_view_dimensions(mapProj->get_surface_width(),
      mapProj->get_surface_height());

   int status = map_engine.change_map_type(favorite.map_type.get_source(),
      favorite.map_type.get_scale(), favorite.map_type.get_series(),
      favorite.map_proj_params.center.lat, favorite.map_proj_params.center.lon, 
      favorite.map_proj_params.rotation, favorite.map_proj_params.zoom_percent,
      favorite.map_proj_params.type);

   if (status == SUCCESS)
      return TRUE;

   fvw_get_view()->OnMapError(status);

   return FALSE;
}

void OrgFavoriteDlg::OnGotoNewTab()
{
   int favoriteIndex = GetSelectedFavoriteIndex();
   if (favoriteIndex == -1)
      return;

   // before creating a new tab determine if map data is available at the favorite's location
   //
   if (IsMapDataAvailable(favoriteIndex))
   {
      fvw_get_frame()->GetMapViewTabsBar()->AddTab();
      fvw_get_frame()->goto_favorite(favoriteIndex);
   }
}

void OrgFavoriteDlg::OnRename()
{
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index == -1)
      return;

   m_listctrl.SetFocus();

   m_listctrl.EditLabel(selected_index);
}

void OrgFavoriteDlg::OnEndRename(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index == -1)
      return;

   CString strText;
   if (pDispInfo->item.pszText == NULL)
   {
      POSITION pos = m_current_list->FindIndex(selected_index);
      if (pos == NULL)
         return;
      strText = m_current_list->GetAt(pos).name;
   }
   else
      strText = CString(pDispInfo->item.pszText);

   if (FavoriteList::get_instance()->rename_favorite(m_current_list, selected_index, strText) == SUCCESS)
      m_listctrl.SetItemText(selected_index, 0, strText);

   if (m_listctrl.GetItemData(selected_index) == NULL)
   {
      m_fav_name_str = pDispInfo->item.pszText;
      UpdateData(FALSE);
   }

   *pResult = 0;
}

void OrgFavoriteDlg::draw_preview_map(FavoriteData* favorite)
{
   if (favorite != NULL &&
       favorite->map_proj_params.type != GENERAL_PERSPECTIVE_PROJECTION)
   {
      // create an instance of a map engine
      MapEngineCOM map_engine;

      map_engine.init();

      // get the preview rect in which the map will be drawn
      CWnd *wnd = CWnd::FromHandle(GetDlgItem(IDC_PREVIEW)->m_hWnd);
      CRect preview_rect;

      wnd->GetClientRect(&preview_rect);

      map_engine.set_view_dimensions(preview_rect.Width(),
         preview_rect.Height());

      int ret = map_engine.change_map_type(favorite->map_type.get_source(),
         favorite->map_type.get_scale(), favorite->map_type.get_series(), 
         favorite->map_proj_params.center.lat,
         favorite->map_proj_params.center.lon,
         favorite->map_proj_params.rotation,
         favorite->map_proj_params.zoom_percent, 
         favorite->map_proj_params.type);
      
      const boolean_t display_wait_cursor = TRUE;
      boolean_t map_drawn_from_scratch;
      if (ret == SUCCESS)
      {
         map_engine.draw_map(&m_memory_dc, display_wait_cursor, &map_drawn_from_scratch);
         
         // draw the resized pixmap onto the view
         show_preview_window(TRUE);

         // after drawing overlays to the preview window they need to be invalidated
         OVL_get_overlay_manager()->invalidate_all();
      }
      else
         show_preview_window(FALSE);

      map_engine.uninit();
   }
   else
      show_preview_window(FALSE);
}

void OrgFavoriteDlg::show_preview_window(int show)
{
   if (m_show != show)
   {
      GetDlgItem(IDC_PREVIEW)->ShowWindow(show);
      m_show = show;
   }
}

void OrgFavoriteDlg::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   CWnd *wnd = CWnd::FromHandle(GetDlgItem(IDC_PREVIEW)->m_hWnd);
   CDC *pDC = wnd->GetDC();
   CRect preview_rect;

   wnd->GetClientRect(&preview_rect);
   
   // copy contents of memory dc into the preview window's dc
   pDC->BitBlt(0, 0, preview_rect.Width(), 
      preview_rect.Height(), &m_memory_dc, preview_rect.TopLeft().x, preview_rect.TopLeft().y, SRCCOPY);
}

void OrgFavoriteDlg::OnBack() 
{
   m_current_path = m_current_path.Left(m_current_path.ReverseFind('\\'));

   FavoriteList *pFavoriteList = FavoriteList::get_instance();
   m_current_list = pFavoriteList->get_list_from_path_name(m_current_path);

   if (m_current_list == &pFavoriteList->m_fav_list)
      m_toolbar.GetToolBarCtrl().EnableButton(ID_BACK, FALSE);

   initialize_list();
   select_first_item();
}

void OrgFavoriteDlg::OnCreateFolder() 
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   // create a new unique default folder name "New Folder (x)"
   BOOL found = TRUE;
   int count = 1;
   CString folder_name;
   while (found)
   {
      if (count == 1)
         folder_name = "New Folder";
      else
         folder_name.Format("New Folder (%d)", count);

      // check the list for an existing directory with the same
      // name
      found = FALSE;
      POSITION pos = m_current_list->GetHeadPosition();
      while (pos)
      {
         favorite_entry_t &entry = m_current_list->GetNext(pos);
         if (entry.sub_list != NULL && entry.name == folder_name)
         {
            found = TRUE;
            break;
         }
      }

      count++;
   }

   favorite_entry_t entry;
   entry.name = CString(folder_name);
   entry.path = m_current_path + "\\" + folder_name;
   entry.sub_list = new favorite_list_t;

   m_current_list->AddTail(entry);

   initialize_list();
   
   // select the new entry for editing
   m_listctrl.SetFocus();
   m_listctrl.EditLabel(m_listctrl.GetItemCount() - 1);
}

void OrgFavoriteDlg::OnMoveTo() 
{
   const int num_selected = m_listctrl.GetSelectedCount();
   if (num_selected == 0)
      return;

   CFavoriteMoveTo dlg;
   if (dlg.DoModal() == IDOK)
      OnMoveTo(dlg.get_path_name());
}

void OrgFavoriteDlg::OnMoveTo(CString path_name)
{
   const int num_selected = m_listctrl.GetSelectedCount();
   if (num_selected == 0)
      return;

   std::vector<int> vecSelected;
   
   // store the list of items that were selected so we can 
   // move them in reverse order
   int i = 0;
   POSITION position = m_listctrl.GetFirstSelectedItemPosition();
   while (position)
      vecSelected.push_back(m_listctrl.GetNextSelectedItem(position));

   FavoriteList::get_instance()->move_to_folder(m_current_list, vecSelected, path_name);
   initialize_list();
   select_first_item();
}


VOID OrgFavoriteDlg::OnDestroy()
{
   UpdateFavoritesMenu();
}


VOID OrgFavoriteDlg::UpdateFavoritesMenu()
{
   CMainFrame* pFrame = fvw_get_frame();
   if ( pFrame )
      pFrame->UpdateFavoritesMenu();
}


/////////////////////////////////////////////////////////////////////////////
// CCreateNewFolder dialog


CCreateNewFolder::CCreateNewFolder(CWnd* pParent /*=NULL*/)
   : CDialog(CCreateNewFolder::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCreateNewFolder)
   m_edit_str = _T("");
   //}}AFX_DATA_INIT
}


void CCreateNewFolder::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCreateNewFolder)
   DDX_Text(pDX, IDC_EDIT, m_edit_str);
   DDV_ValidFilename(pDX, m_edit_str);
   //}}AFX_DATA_MAP
}

BOOL CCreateNewFolder::OnInitDialog() 
{
   CDialog::OnInitDialog();

   GotoDlgCtrl(GetDlgItem(IDC_EDIT));

   return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CCreateNewFolder, CDialog)
   //{{AFX_MSG_MAP(CCreateNewFolder)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateNewFolder message handlers
/////////////////////////////////////////////////////////////////////////////
// CFavoriteMoveTo dialog


CFavoriteMoveTo::CFavoriteMoveTo(CWnd* pParent /*=NULL*/)
   : CDialog(CFavoriteMoveTo::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFavoriteMoveTo)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CFavoriteMoveTo::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFavoriteMoveTo)
   DDX_Control(pDX, IDC_FOLDER_VIEW_TREE, m_treeBrowseCtrl);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFavoriteMoveTo, CDialog)
   //{{AFX_MSG_MAP(CFavoriteMoveTo)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CFavoriteMoveTo, CDialog)
    //{{AFX_EVENTSINK_MAP(CFavoriteMoveTo)
   ON_EVENT(AddFavoriteDlg, IDC_FOLDER_VIEW_TREE, 1 /* FolderSelected */, OnSelectedFolderViewTree, VTS_BSTR)
   //}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavoriteMoveTo message handlers

BOOL CFavoriteMoveTo::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_folder_name = FavoriteList::GetFavoritesPath();
   
   m_treeBrowseCtrl.SetRootDir(m_folder_name);
   m_treeBrowseCtrl.SetRootDisplayName("Favorites");
   m_treeBrowseCtrl.SetTargetDir(m_folder_name);
   m_treeBrowseCtrl.FillTree();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CFavoriteMoveTo::OnSelectedFolderViewTree(LPCTSTR strFolderName)
{
   m_folder_name = strFolderName;
}

int OrgFavoriteDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   m_drop_target.Register(&m_listctrl);
   
   return 0;
}

void OrgFavoriteDlg::OnMoveDown() 
{
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index == -1)
      return;

   BOOL move_down = TRUE;
   int new_index;
   FavoriteList::get_instance()->MoveEntry(m_current_list, selected_index, move_down, new_index);
   initialize_list();
   select_item(new_index);
}

void OrgFavoriteDlg::OnMoveUp() 
{
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index == -1)
      return;

   BOOL move_down = FALSE;
   int new_index;
   FavoriteList::get_instance()->MoveEntry(m_current_list, selected_index, move_down, new_index);
   initialize_list();
   select_item(new_index);
}

void OrgFavoriteDlg::OnMoveBottom() 
{
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index == -1)
      return;
   int new_index;
   FavoriteList::get_instance()->MoveEntryToBottom(m_current_list, selected_index, new_index);
   initialize_list();
   select_item(new_index);
}

void OrgFavoriteDlg::OnMoveTop() 
{
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index == -1)
      return;
   int new_index;
   FavoriteList::get_instance()->MoveEntryToTop(m_current_list, selected_index, new_index);
   initialize_list();
   select_item(new_index);
}


void OrgFavoriteDlg::OnSortAscending() 
{
   const BOOL ascending = TRUE;
   FavoriteList::get_instance()->Sort(m_current_list, ascending);
   initialize_list();
   select_first_item();
}

void OrgFavoriteDlg::OnSortDescending() 
{
   const BOOL ascending = FALSE;
   FavoriteList::get_instance()->Sort(m_current_list, ascending);
   initialize_list();
   select_first_item();
}

void OrgFavoriteDlg::OnMailFavorite() 
{
   CList<CString, CString> pathname_list;
   CList<CString, CString> filename_list;

   int selected_index = -1;
   do
   {
      selected_index = m_listctrl.GetNextItem(selected_index, LVNI_SELECTED);
      if (selected_index != -1)
      {
         if (m_listctrl.GetItemData(selected_index) != NULL)
            continue;

         POSITION pos = m_current_list->FindIndex(selected_index);
         if (pos == NULL)
            continue;

         favorite_entry_t entry = m_current_list->GetAt(pos);
         pathname_list.AddTail(entry.path + "\\" + entry.name + ".fvx");
         filename_list.AddTail(entry.name + ".fvx");

      }
   } while(selected_index != -1);

   if (pathname_list.GetCount())
      m_pMapi->send_mail(pathname_list, filename_list);
}

void OrgFavoriteDlg::OnBegindragListctrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

   CPoint pt;
   CRect item_rect, bounding_box;
   CPoint offset = CPoint(24, 16);

   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);

   m_drag_image = m_listctrl.CreateDragImageEx(&pt);

   ASSERT(m_drag_image);


   // pos of top/left of selected image
   VERIFY(m_listctrl.GetItemRect(selected_index, &item_rect, LVIR_ICON));
   VERIFY(m_listctrl.GetItemRect(selected_index, &bounding_box, LVIR_BOUNDS));

   CPoint drag_hot_spot;
   drag_hot_spot.x = (item_rect.left - bounding_box.left) + offset.x;
   drag_hot_spot.y = (item_rect.top - bounding_box.top) + offset.y;

   // begin the drag
   VERIFY(m_drag_image->BeginDrag(0, drag_hot_spot));
   VERIFY(m_drag_image->DragEnter(&m_listctrl, CPoint(0,0)));

   // capture the mouse for this application
   SetCapture();
   
   *pResult = 0;
}

void OrgFavoriteDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (m_drag_image)
   {
      m_drag_image->DragMove(point);

      UINT uFlags;
      ClientToScreen(&point);
      m_listctrl.ScreenToClient(&point);
      int item = m_listctrl.HitTest(point, &uFlags);

      BOOL need_update = FALSE;
      
      const int count = m_listctrl.GetItemCount();
      for(int i=0;i<count;i++)
      {
         if (i != item && m_listctrl.GetItemData(i) != NULL)
         {
            UINT old_state = m_listctrl.GetItemState(i, LVIF_STATE);
            m_listctrl.SetItemState(i, old_state & ~LVIS_DROPHILITED, LVIF_STATE);
            UINT new_state = m_listctrl.GetItemState(i, LVIF_STATE);

            if (old_state != new_state)
            {
               m_listctrl.RedrawItems(i, i);
               need_update = TRUE;
            }
         }
      }
      
      // hilight a folder if the mouse is over it
      if (0 <= item && item < m_listctrl.GetItemCount() &&
         m_listctrl.GetItemData(item) != NULL)
      {
         UINT old_state = m_listctrl.GetItemState(item, LVIF_STATE);
         m_listctrl.SetItemState(item, LVIS_DROPHILITED, LVIF_STATE);
         UINT new_state = m_listctrl.GetItemState(item, LVIF_STATE);

         if (old_state != new_state)
         {
            m_listctrl.RedrawItems(item, item);
            need_update = TRUE;
         }
      }

      if (need_update)
      {
         ImageList_DragShowNolock(FALSE);
         m_listctrl.UpdateWindow();
         ImageList_DragShowNolock(TRUE);
      }
   }
   
   CDialog::OnMouseMove(nFlags, point);
}

void OrgFavoriteDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
   if (m_drag_image)
   {
      ReleaseCapture();

      // end dragging
      VERIFY(m_drag_image->DragLeave(&m_listctrl));
      m_drag_image->EndDrag();

      delete m_drag_image;
      m_drag_image = NULL;

      // figure out the path name that the mouse cursor is over
      UINT uFlags;
      ClientToScreen(&point);
      m_listctrl.ScreenToClient(&point);
      int item = m_listctrl.HitTest(point, &uFlags);
      if (0 <= item && item < m_listctrl.GetItemCount() &&
           m_listctrl.GetItemData(item) != NULL)
      {
         // move the selected 
         OnMoveTo(m_current_path + "\\" + m_listctrl.GetItemText(item, 0));
      }
   }
   
   CDialog::OnLButtonUp(nFlags, point);
}

LPARAM OrgFavoriteDlg::OnFavoritesDirectoryChanged(WPARAM, LPARAM)
{
   FavoriteList *pFavoriteList = FavoriteList::get_instance();

   m_current_list = pFavoriteList->get_list_from_path_name(m_current_path);

   CString strSelectedName;
   int selected_index = m_listctrl.GetNextItem(-1, LVNI_SELECTED);
   if (selected_index != NULL)
   {
      POSITION pos = m_current_list->FindIndex(selected_index);
      if (pos != NULL)
         strSelectedName = m_current_list->GetAt(pos).name;
   }

   // if a directory was deleted outside of FalconView then we will need to refresh the dialog
   if (m_current_list == NULL)
   {
      m_current_list = &pFavoriteList->m_fav_list;
      initialize_list();
      select_first_item();
   }
   else
   {
      initialize_list();

      // reselect the previous name if possible
      int index = 0;
      POSITION pos = m_current_list->GetHeadPosition();
      while (pos)
      {
         if (m_current_list->GetNext(pos).name == strSelectedName)
            break;
         index++;
      }

      select_item(index);
   }


   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFavoriteListCtrl

CFavoriteListCtrl::CFavoriteListCtrl()
{
}

CFavoriteListCtrl::~CFavoriteListCtrl()
{
}


BEGIN_MESSAGE_MAP(CFavoriteListCtrl, CListCtrl)
   //{{AFX_MSG_MAP(CFavoriteListCtrl)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavoriteListCtrl message handlers

CImageList *CFavoriteListCtrl::CreateDragImageEx( LPPOINT lpPoint )
{
   CRect cSingleRect;
   CRect cCompleteRect( 0,0,0,0 );
   int nIdx;
   BOOL bFirst = TRUE;
   //
   // Determine the size of the drag image 
   //
   POSITION pos = GetFirstSelectedItemPosition();
   while (pos)
   {
      nIdx = GetNextSelectedItem( pos );
      GetItemRect( nIdx, cSingleRect, LVIR_BOUNDS );
      if (bFirst)
      {
         // Initialize the CompleteRect
         GetItemRect( nIdx, cCompleteRect, LVIR_BOUNDS );
         bFirst = FALSE;
      }
      cCompleteRect.UnionRect( cCompleteRect, cSingleRect );
   }

   //
   // Create bitmap in memory DC
   //
   CClientDC cDc(this);
   CDC cMemDC;
   CBitmap   cBitmap;

   if(!cMemDC.CreateCompatibleDC(&cDc))
      return NULL;

   if(!cBitmap.CreateCompatibleBitmap(&cDc, cCompleteRect.Width(), cCompleteRect.Height()))
      return NULL;
   
   CBitmap* pOldMemDCBitmap = cMemDC.SelectObject( &cBitmap );
   // Here green is used as mask color
   cMemDC.FillSolidRect(0,0,cCompleteRect.Width(), cCompleteRect.Height(), RGB(0, 255, 0)); 

   //
   // Paint each DragImage in the DC
   //
   CImageList *pSingleImageList;
   CPoint cPt;

   pos = GetFirstSelectedItemPosition();
   while (pos)
   {
      nIdx = GetNextSelectedItem( pos );
      GetItemRect( nIdx, cSingleRect, LVIR_BOUNDS );

      pSingleImageList = CreateDragImage( nIdx, &cPt);
      if (pSingleImageList)
      {
         pSingleImageList->DrawIndirect( &cMemDC, 
                     0, 
                     CPoint( cSingleRect.left-cCompleteRect.left, 
                     cSingleRect.top-cCompleteRect.top ),
                     cSingleRect.Size(), 
                     CPoint(0,0));
         delete pSingleImageList;
      }
   }

   cMemDC.SelectObject( pOldMemDCBitmap );
   //
   // Create the imagelist with the merged drag images
   //
   CImageList* pCompleteImageList = new CImageList;
   
   pCompleteImageList->Create(cCompleteRect.Width(), 
               cCompleteRect.Height(), 
               ILC_COLOR | ILC_MASK, 0, 1);
   // Here green is used as mask color
   pCompleteImageList->Add(&cBitmap, RGB(0, 255, 0)); 

   cBitmap.DeleteObject();
   //
   // as an optional service:
   // Find the offset of the current mouse cursor to the imagelist
   // this we can use in BeginDrag()
   //
   if ( lpPoint )
   {
      CPoint cCursorPos;
      GetCursorPos( &cCursorPos );
      ScreenToClient( &cCursorPos );
      lpPoint->x = cCursorPos.x - cCompleteRect.left;
      lpPoint->y = cCursorPos.y - cCompleteRect.top;
   }

   return( pCompleteImageList );
}
LRESULT AddFavoriteDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

