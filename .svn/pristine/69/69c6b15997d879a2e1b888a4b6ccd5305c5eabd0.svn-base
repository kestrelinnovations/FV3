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

// MapTpSel.h

#ifndef MAP_TYPE_SELCTION_H
#define MAP_TYPE_SELCTION_H

#include "maps_d.h"  // MapCategory, MapSource, MapScale

class MapTypeSelection
{
private:
   CComboBox &m_category_ctrl;
   CComboBox &m_scale_ctrl;
   std::vector<MapType *> m_vecMapTypes;

public:
   MapTypeSelection(CComboBox &category_ctrl, CComboBox &scale_ctrl) :
      m_category_ctrl(category_ctrl), m_scale_ctrl(scale_ctrl)
   {
   }

   ~MapTypeSelection();

   // Setup the category control with a list of map categories.
   // Call this function inside of OnInitDialog.
   bool set_category_list(const MapCategory &initialCategory);

   // Setup the scale control with a list of map scale descriptions.
   // Call this function inside of OnInitDialog, and any time the category
   // changes.  Use current_source and current_scale to select a specific
   // item.  The defaults will select the first item in the list.  Use smallest
   // and largest to limit the list of map types.  The defaults will give you
   // all possible values within the given category.

   // Setup the scale control with a list of map type descriptions.
   // Call this function inside of OnInitDialog, and any time the category
   // changes.  Use source, scale, and series to select a specific item.
   // Use smallest and largest to limit the list of map types.  Note the
   // smallest scale is WORLD, and the largest scale is ONE_TO_1K.
   bool set_scale_list(const MapCategory &category, 
      const MapSource &source, const MapScale &scale, const MapSeries &series,
      const MapScale &smallest, const MapScale &largest, BOOL show_closest = TRUE);

   // This version of the function shows all map types in the given category.
   bool set_scale_list(const MapCategory &category, 
      const MapSource &source, const MapScale &scale, const MapSeries &series, 
      BOOL show_closest = TRUE);
   bool set_scale_list(const MapCategory &category, const MapType& map_type,
      BOOL show_closest = TRUE);

   // This version of the function shows all map types in the given category,
   // and the first item in the list is selected.
   bool set_scale_list(const MapCategory &category, BOOL show_closest = TRUE);

   // get the category currently selected
   bool get_selected_category(MapCategory &category);

   // get the map currently selected by returning it's category and
   // the index in that category's map list
   bool get_selected_map(MapCategory &category, int &map_index, 
      MapSource& mapSource, MapScale& mapScale, MapSeries& mapSeries);

protected:
   static bool MapTypeSort(MapType *a, MapType *b);
   void ResetScaleControl();
};

// This class is used to help discriminate between between
// map types with the same string representation 
class MapGroup
{
public:
   MapGroup() {}

   MapGroup(const MapCategory& category, const MapSource &source, const MapScale &scale, 
      const MapSeries &series) : m_map_category(category), m_source(source), m_scale(scale), m_series(series)
                             
   {
   }

   // Destructor
   virtual ~MapGroup() {}

   // Get Category
   MapCategory get_category() { return m_map_category; }
 
   // Get Source 
   const MapSource &get_source() const { return m_source; }
   
   // Get Scale
   const MapScale &get_scale() const { return m_scale; }

   // Get Series
   const MapSeries &get_series() const { return m_series; }

private:
   MapCategory m_map_category;
   MapSource m_source;
   MapScale m_scale;
   MapSeries m_series;
};

// Your tree control should have the "Has buttons", "Has Lines", and "Lines at Root"
// styles selected in the resource editor
class MapTypeSelectionTree
{
protected:
   CTreeCtrl &m_tree_ctrl;
   std::vector<MapGroup *> m_vecMapGroups;

public:

   // constructor
   MapTypeSelectionTree(CTreeCtrl &tree_ctrl) : m_tree_ctrl(tree_ctrl) { }

   // destructor
   ~MapTypeSelectionTree();

   // call this from your dialog's OnInitDialog
   void on_initialize(BOOL bSelectAll = FALSE,
      BOOL show_checkboxes_for_groups = FALSE);
   void on_initialize(CList<MapGroup, MapGroup> &map_list, BOOL bSelectAll = FALSE,
      BOOL show_checkboxes_for_groups = FALSE);
   
   void get_selected_list(CList<MapType*, MapType*> &map_list);
   void get_selected_list(CList<MapGroup*, MapGroup*> &map_list);

   // return TRUE if any items are selected in the tree
   BOOL is_item_selected();

private:
   BOOL is_in_list(MapGroup *map_group, CList<MapGroup, MapGroup> &map_list);
};

#endif
