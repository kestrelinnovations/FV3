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

// PrntIcon.h

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_PRNTICON_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_PRNTICON_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/common.h"
#include "FalconView/include/ovl_d.h"   // C_icon
#include "FalconView/include/Maps.h"    // for page_map_list_t
// this project's headers
// forward definitions

// ERROR CODES
#define PAST_90N -1000  // only used for area charts
#define PAST_80N -1001  // only used for area charts
#define PAST_80S -1002  // only used for area charts
#define PAST_90S -1003  // only used for area charts
#define SURFACE_TOO_LARGE -1004  // used by Calc() functions

class CPrintToolOverlay;
class SettableMapProj;
class MapProj;
class PageLayoutFileVersion;
interface IFvMapView;

class PrintIcon : public C_icon
{
public:
   // Construction/Destruction
   explicit PrintIcon(CPrintToolOverlay *parent);
   virtual ~PrintIcon();

   CPrintToolOverlay *get_parent() const;

   // Returns TRUE if this icon is a instance of the given class.
   virtual boolean_t is_kind_of(const char *class_name);

   // Draw the object.
   virtual void Draw(MapProj *map, CDC* pDC) = 0;

   // Erase the object.
   virtual void Invalidate() = 0;

   // Returns true if the object is selected.
   virtual bool IsSelected() const = 0;

   // If the object is selected it will be unselected and the selection handles
   // will be removed.  Otherwise this function does nothing.
   virtual bool UnSelect() = 0;

   // Compute printer page size dependent properties.
   // Returns SUCCESS, FAILURE, or SURFACE_TOO_LARGE.  SURFACE_TOO_LARGE can
   // only be returned when surface_check is true.  surface_check of true
   // should test the surface size and return SURFACE_TOO_LARGE if there is
   // a problem.
   virtual int Calc(bool data_check = true, bool surface_check = false) = 0;

   // If the point hits an object the pointer to that object is returned.
   // For simple objects the function will return the "this" pointer or NULL.
   // For a compound object the function may return the "this" pointer, NULL,
   // or a pointer to another PrintIcon which represents the component of
   // compound object hit by pt.
   virtual PrintIcon *GetAtViewCoords(const CPoint& pt) = 0;

   // This function handles the selected operation when an object is selected,
   // in the current overlay and the Page Layout Tool is active.  It assumes it
   // is called under the right circumstances.
   // This function returns TRUE if some action is taken, FALSE otherwise.  If
   // TRUE, drag and cursor will be set accordingly.  When *drag is set to TRUE
   // one or more of on_drag, on_drop, or cancel_drag members will get called,
   // before the drag-operation is completed.  A drag operation can be a resize,
   // move, rotate, etc.
   virtual boolean_t on_selected(ViewMapProj *view, CPoint point, UINT flags,
      boolean_t *drag, HCURSOR *cursor) = 0;

   // This function handles the drag operation.  If a PrintIcon returns drag
   // equal to TRUE, then on_drag will be called each time the object moves.
   // The display should be updated to show the drag operation.
   virtual void on_drag(IFvMapView *pMapView, CPoint point, UINT flags,
      HCURSOR* pCursor, HintText &hint) = 0;

   // Called when a drag operation is completed.  The change should be applied
   // and the display should be updated.
   virtual void on_drop(IFvMapView *pMapView, CPoint point, UINT flags) = 0;

   // Called when a drag operation is aborted.  There state of the PrintIcon
   // must return to what it was before the drag operation began.  The object
   // is no longer being dragged.  Any drag lines on the display need to be
   // removed.
   virtual void on_cancel_drag(IFvMapView *pMapView) = 0;

   // This function handles the test_selected operation for the object the
   // point is above, if it is in the current overlay and the Page Layout Tool
   // is active.  It assumes it is called under the right circumstances.
   // This function returns TRUE when the cursor and hint are set.
   virtual boolean_t on_test_selected(MapProj *view, CPoint point, UINT flags,
      HCURSOR *cursor, HintText &hint) = 0;

   // If the object is selected, it must invalidate itself and return TRUE.
   // If one or more components of the object are selected, then the selected
   // component(s) should be invalidated and destroyed, and the function should
   // return FALSE.
   virtual boolean_t on_delete(ViewMapProj *view) = 0;

   // This print icon will add menu items that pertain to it, if any.
   virtual void on_menu(MapProj *map, CPoint point,
      CList<CFVMenuNode*, CFVMenuNode*> &list) = 0;

   // Serialization
   virtual void Serialize(CArchive *ar, PageLayoutFileVersion *version) = 0;

   // Returns TRUE if the object in a valid state after a Serialize-Load, FALSE
   // otherwise.
   virtual boolean_t WasLoadOK() = 0;

   // Add your map projection(s) to the given list.
   virtual int add_map_projections_to_list(page_map_list_t *list,
         boolean_t start_new_page = TRUE,
      double offset_from_left = -1,
      double offset_from_top = -1,
      page_orientation_t orientation = PAGE_ORIENTATION_DEFAULT) = 0;


   // returns TRUE if a page is selected.  Increments page_index.
   virtual boolean_t get_current_page(int *page_index) = 0;

   // trys to select a page in this object
   virtual int select_page(int page_number, int *page_index) = 0;

private:
   boolean_t m_no_map_data;
   boolean_t m_show_page_center_symbol;

public:
   boolean_t get_no_map_data(void) const
   {
      return m_no_map_data;
   }
   void set_no_map_data(boolean_t no_map_data)
   {
      m_no_map_data = no_map_data;
   }

   boolean_t get_show_page_center_symbol(void) const
   {
      return m_show_page_center_symbol;
   }
   void set_show_page_center_symbol(boolean_t on)
   {
      m_show_page_center_symbol = on;
   }
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_PRNTICON_H_
