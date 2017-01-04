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

// MapTpSel.cpp

#include <stdafx.h>

#include "MapTpSel.h"
#include "maps.h"    // CMapType, MAP_get_map_list, etc.
#include "err.h"
#include "Common\ComErrorObject.h"

#define NUM_CATEGORIES 5
#define CLOSEST_SCALE_INDEX -1000

// foward declarations
CString MapTypeToString(const CString& category_str, MapType *pMapType);

// MapTypeSelection
//

MapTypeSelection::~MapTypeSelection()
{
   ResetScaleControl();
}

void MapTypeSelection::ResetScaleControl()
{
   for (size_t i=0; i<m_vecMapTypes.size(); i++)
      delete m_vecMapTypes[i];

   m_vecMapTypes.erase(m_vecMapTypes.begin(), m_vecMapTypes.end());

   if (m_scale_ctrl.m_hWnd)
      m_scale_ctrl.ResetContent();
}

// Setup the category control with a list of map categories.
// Call this function inside of OnInitDialog.
bool MapTypeSelection::set_category_list(const MapCategory & initialCategory)
{
   m_category_ctrl.ResetContent();

   // populate the category combo box with a list of available map groups
   try
   {
      IMapGroupNamesPtr spMapGroupNames;
      IMapGroupsPtr spMapGroups;

      CO_CREATE(spMapGroupNames, CLSID_MapGroupNames);
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      HRESULT hr = spMapGroupNames->SelectAll();
      while (hr == S_OK)
      {
         // only add a category to the combo-box if it has one or more available map types
         if (spMapGroupNames->m_Disabled == FALSE &&
            spMapGroups->SelectWithData(spMapGroupNames->m_Identity) == S_OK &&
             spMapGroups->m_DoesDataExist)
         {
            const int nIndex = m_category_ctrl.AddString(spMapGroupNames->m_GroupName); 
            m_category_ctrl.SetItemData(nIndex, spMapGroupNames->m_Identity);
         }

         hr = spMapGroupNames->MoveNext();
      }
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("Error populating list of available map categories: Reason: %s", (char *)e.Description());
      ERR_report(msg);
      return false;
   }

   m_category_ctrl.SelectString(-1, initialCategory.get_string());
   
   return true;
}

// static
bool MapTypeSelection::MapTypeSort(MapType *a, MapType *b)
{
   return a->get_scale() < b->get_scale() ? true : false;
}

// Setup the scale control with a list of map type descriptions.
// Call this function inside of OnInitDialog, and any time the category
// changes.  Use source, scale, and series to select a specific item.
// Use smallest and largest to limit the list of map types.
bool MapTypeSelection::set_scale_list(const MapCategory &category, 
   const MapSource &source, const MapScale &scale, const MapSeries &series,
   const MapScale &smallest, const MapScale &largest, BOOL show_closest /*=TRUE*/)
{
   try
   {
      IMapGroupNamesPtr spMapGroupNames;
      IMapGroupsPtr spMapGroups;

      CO_CREATE(spMapGroupNames, CLSID_MapGroupNames);
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      if (spMapGroupNames->SelectByName(_bstr_t(category.get_string())) != S_OK)
      {
         ERR_report("MapTypeSelection::set_scale_list - invalid category name");
         return false;
      }

      // clear old contents from the scale combo box
      ResetScaleControl();

      // When all scales are being used there will be an entry for going to the
      // scale closest to the current scale, except when the category is DTED.
      if (show_closest && smallest == WORLD && largest == ONE_TO_1K && category != DTED_CATEGORY)
      {
         m_scale_ctrl.AddString("Closest");
         m_scale_ctrl.SetItemDataPtr(0, NULL);
      }

      // used to build the display string for each map type
      CString category_str = category.get_string();
      int index;

      // index of selected map type
      int current_scale_index = 0;

      // load scale combo box with this category's scale options
      HRESULT hr = spMapGroups->SelectWithData(spMapGroupNames->m_Identity);
      while (hr == S_OK && spMapGroups->m_DoesDataExist)
      {
         MapSource currentMapSource(spMapGroups->m_ProductName);
         MapScale currentMapScale(spMapGroups->m_Scale, spMapGroups->m_ScaleUnits);
         MapSeries currentMapSeries(spMapGroups->m_SeriesName);

         // if this map type's scale is within the range and data is available
         // for this map type, it will be added
         if (smallest <= currentMapScale && currentMapScale <= largest)
         {
            MapType* pMapType = new MapType(currentMapSource, currentMapScale, currentMapSeries);

            CString scale_str = MapTypeToString(category_str, pMapType);

            // set the string and the data item
            m_scale_ctrl.AddString(scale_str);
            // store the index in the map type list with the list box entry
            index = m_scale_ctrl.GetCount() - 1;
            if (index >= 0)
            {
               m_scale_ctrl.SetItemDataPtr(index, reinterpret_cast<void *>(pMapType));
               m_vecMapTypes.push_back(pMapType);
            }
            else
            {
               delete pMapType;
               ERR_report("GetCount() failed.");
               index = 0;
            }

            // if map_type matches the current map spec, it will be selected
            if (source == currentMapSource && scale == currentMapScale && series == currentMapSeries)
               current_scale_index = index;
         }

         hr = spMapGroups->MoveNext();
      }

      // When all scales are being used there will be an entry for going to the
      // scale closest to the current scale - select this as the default
      if (smallest == WORLD && largest == ONE_TO_1K && category != DTED_CATEGORY && show_closest)
         m_scale_ctrl.SetCurSel(0);
      else
      {
         m_scale_ctrl.SetCurSel(current_scale_index);
      }

      return true;
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("MapTypeSelection::set_scale_list failed - %s", (char *)e.Description());
      ERR_report(msg);
      return false;
   }
}

// This version of the function shows all map types in the given category.
bool MapTypeSelection::set_scale_list(const MapCategory &category, 
   const MapSource &source, const MapScale &scale, const MapSeries &series,
   BOOL show_closest /*= TRUE*/)
{
   MapScale smallest = WORLD;
   MapScale largest = ONE_TO_1K;
   
   return set_scale_list(category, source, scale, series, smallest, largest, show_closest);
}

bool MapTypeSelection::set_scale_list(const MapCategory &category,
   const MapType& map_type, BOOL show_closest /*= TRUE*/)
{
   return set_scale_list(category, map_type.get_source(), map_type.get_scale(),
      map_type.get_series(), show_closest);
}

// This version of the function shows all map types in the given category,
// and the first item in the list is selected.
bool MapTypeSelection::set_scale_list(const MapCategory &category, BOOL show_closest /*=TRUE*/)
{
   MapSource source = NULL_SOURCE;
   MapScale scale = NULL_SCALE;
   MapSeries series = NULL_SERIES;
   MapScale smallest = WORLD;
   MapScale largest = ONE_TO_1K;
   
   return set_scale_list(category, source, scale, series, smallest, largest, show_closest);
}

// get the category currently selected
bool MapTypeSelection::get_selected_category(MapCategory &category)
{
   int current_selection = m_category_ctrl.GetCurSel();
   if (current_selection == CB_ERR)
      return false;
   
   CString category_str;
   m_category_ctrl.GetLBText(current_selection, category_str);
   category = MapCategory(category_str);
   
   return true;
}

// get the map currently selected by returning it's category and
// the index into that category's map list
bool MapTypeSelection::get_selected_map(MapCategory &category, 
                                        int &map_index, 
                                        MapSource& mapSource, MapScale& mapScale, MapSeries& mapSeries)
{
   // get index into map list for current selection
   map_index = m_scale_ctrl.GetCurSel();
   if (CB_ERR == map_index)
      return false;

   // get the map_index
   MapType* map_type = reinterpret_cast<MapType *>(m_scale_ctrl.GetItemDataPtr(map_index));

   get_selected_category(category);

   // if "Closest" is selected return an index of -1
   if (map_type == NULL)
   {
      // "Closest" is not valid for DTED
      if (category == DTED_CATEGORY)
        return false;

      // Note the CB_ERR is -1, so CLOSEST_SCALE_INDEX can not be -1.
      map_index = -1;
   }
   else
   {
      mapSource = map_type->get_source();
      mapScale = map_type->get_scale();
      mapSeries = map_type->get_series();
   }

   return true;
}


//
// class MapTypeSelectionTree implementation
//

MapTypeSelectionTree::~MapTypeSelectionTree()
{
   for (size_t i=0; i<m_vecMapGroups.size(); i++)
      delete m_vecMapGroups[i];
}

BOOL MapTypeSelectionTree::is_in_list(MapGroup *map_group, 
                                      CList<MapGroup, MapGroup> &map_list)
{
   POSITION position = map_list.GetHeadPosition();
   while (position)
   {
      MapGroup m = map_list.GetNext(position);
      if (map_group->get_category() == m.get_category() &&
         map_group->get_source() == m.get_source() &&
         map_group->get_scale() == m.get_scale() &&
         map_group->get_series() == m.get_series())
         return TRUE;
   }

   return FALSE;
}

// call this from your dialog's OnInitDialog
void MapTypeSelectionTree::on_initialize(BOOL bSelectAll /*= FALSE*/,
   BOOL show_checkboxes_for_groups /*= FALSE*/)
{
   CList<MapGroup, MapGroup> map_list;
   on_initialize(map_list, bSelectAll, show_checkboxes_for_groups);
}

void MapTypeSelectionTree::on_initialize(CList<MapGroup, MapGroup> &map_list,
   BOOL bSelectAll /*= FALSE*/, BOOL show_checkboxes_for_groups /*= FALSE*/)
{
   try
   {
      IMapGroupNamesPtr spMapGroupNames;
      IMapGroupsPtr spMapGroups;

      CO_CREATE(spMapGroupNames, CLSID_MapGroupNames);
      CO_CREATE(spMapGroups, CLSID_MapGroups);

      m_tree_ctrl.DeleteAllItems();
      m_tree_ctrl.ModifyStyle(TVS_CHECKBOXES, 0);
      m_tree_ctrl.ModifyStyle(0, TVS_CHECKBOXES);

      HRESULT hrGroupNames = spMapGroupNames->SelectAll();

      while (hrGroupNames == S_OK)
      {
         if (spMapGroupNames->m_Disabled == TRUE)
         {
            hrGroupNames = spMapGroupNames->MoveNext();
            continue;
         }

         HRESULT hrGroups = spMapGroups->SelectWithData(spMapGroupNames->m_Identity);

         MapCategory category(spMapGroupNames->m_GroupName);

         // add category entry to the tree if there is any data available for the current category and hide the check box
         HTREEITEM parent_tree_item = NULL;
         if (hrGroups == S_OK && spMapGroups->m_DoesDataExist)
         {
            parent_tree_item = m_tree_ctrl.InsertItem(category.get_string());

            // Hide group checkbox if required
            if(! show_checkboxes_for_groups)
            {
               m_tree_ctrl.SetItemState(parent_tree_item, 0, TVIS_STATEIMAGEMASK);
            }

            // If all map types are to be checked, group node should be checked too
            if(bSelectAll)
            {
               m_tree_ctrl.SetCheck(parent_tree_item);
            }
         }

         // get list of maps in the given category
         while (hrGroups == S_OK && spMapGroups->m_DoesDataExist)
         {
            MapSource currentMapSource(spMapGroups->m_ProductName);
            MapScale currentMapScale(spMapGroups->m_Scale, spMapGroups->m_ScaleUnits);
            MapSeries currentMapSeries(spMapGroups->m_SeriesName);

            MapGroup* pMapGroup = new MapGroup(category, currentMapSource, currentMapScale, currentMapSeries);
            MapType map_type(currentMapSource, currentMapScale, currentMapSeries);

            HTREEITEM child_item = m_tree_ctrl.InsertItem(
               MapTypeToString(category.get_string(), &map_type), parent_tree_item);

            m_tree_ctrl.SetItemData(child_item, reinterpret_cast<DWORD_PTR>(pMapGroup));
            
            m_vecMapGroups.push_back(pMapGroup);

            if (bSelectAll || is_in_list(pMapGroup, map_list))
               m_tree_ctrl.SetCheck(child_item);

            hrGroups = spMapGroups->MoveNext();
         }

         hrGroupNames = spMapGroupNames->MoveNext();
      }
   }
   catch(_com_error& e)
   {
      CString msg;
      msg.Format("MapTypeSelectionTree::on_initialize failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

void MapTypeSelectionTree::get_selected_list(CList<MapType *, MapType *> &map_list)
{
   CList<MapGroup*, MapGroup*> selectedMaps;

   get_selected_list(selectedMaps);

   POSITION pos = selectedMaps.GetHeadPosition();
   while (pos)
   {
      MapGroup* map_group = selectedMaps.GetNext(pos);

      MapType* map_type = new MapType(map_group->get_source(), map_group->get_scale(),
         map_group->get_series());

      map_list.AddTail(map_type);
   }
}

void MapTypeSelectionTree::get_selected_list(CList<MapGroup *, MapGroup *> &map_list)
{
   HTREEITEM current_parent = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );

   while (current_parent != NULL)
   {
      HTREEITEM child = m_tree_ctrl.GetChildItem(current_parent);
      while (child != NULL)
      {
         BOOL checked = m_tree_ctrl.GetCheck(child);

         if (!checked)
         {
            child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
            continue;
         }
         
         // the group check state was modified 
         map_list.AddTail(reinterpret_cast<MapGroup *>(m_tree_ctrl.GetItemData(child)));
         
         child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
      }
      
      // Try to get the next item
      current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
   }
}

// return TRUE if any items are selected in the tree
BOOL MapTypeSelectionTree::is_item_selected()
{
   HTREEITEM current_parent = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );
   while (current_parent != NULL)
   {
      HTREEITEM child = m_tree_ctrl.GetChildItem(current_parent);
      while (child != NULL)
      {
         if (m_tree_ctrl.GetCheck(child))
            return TRUE;
         
         child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
      }
      current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
   }

   return FALSE;
}

// Helper functions
//

CString MapTypeToString(const CString& category_str, MapType *pMapType)
{
   CString scale_str, source_str;

   // if the series is not NULL it will be part of the string
   if (pMapType->get_series_string().IsEmpty())
      scale_str.Format("%s", pMapType->get_scale_string());
   else
      scale_str.Format("%s (%s)", pMapType->get_scale_string(),
      pMapType->get_series_string());

   // if the source string is not the same as the category string,
   // it will be part of the string
   source_str = pMapType->get_source_string();
   if (category_str != source_str)
   {
      CString string;

      string.Format("%s %s", source_str, scale_str);
      scale_str = string;
   }

   return scale_str;
}
