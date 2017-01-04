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



// DataObject.cpp
//

#include "stdafx.h"
#include "DataObject.h"
#include "err.h"
#include "newptdef.h"
#include "FalconView/include/fvwutil.h"
#include "FalconView/include/param.h"
#include "FalconView/include/utils.h"
#include "FalconView/localpnt/lpicon.h"
#include "factory.h"
#include "..\tabulareditor\Parser.h"
#include "..\tabulareditor\FavoriteImportDlg.h"
#include "..\tabulareditor\TargetGraphicsDlg.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/localpnt/lpeddlgc.h"

#define MIN_ELEVATION -9999
#define MAX_ELEVATION SHRT_MAX

const int MAX_DTD_ID_LENGTH = 5;

// constructor
LocalPointDataObject::LocalPointDataObject(C_overlay *overlay) : m_pt_list(500),
TabularEditorDataObject(overlay)
{
   m_column_info_list.Add(column_info_t("Icon", TRUE, TRUE, get_show_flag(0, TRUE)));
   m_column_info_list.Add(column_info_t("ID", FALSE, TRUE, get_show_flag(1, TRUE)));
   m_column_info_list.Add(column_info_t("Group", FALSE, TRUE, get_show_flag(2, TRUE)));
   m_column_info_list.Add(column_info_t("DTD ID", FALSE, TRUE, get_show_flag(3, TRUE)));
   m_column_info_list.Add(column_info_t("Elevation (in feet)", FALSE, TRUE, get_show_flag(4, TRUE)));
   m_column_info_list.Add(column_info_t("Elevation Source", FALSE, TRUE, get_show_flag(5, TRUE)));
   m_column_info_list.Add(column_info_t("Elevation Source Product", FALSE, TRUE, get_show_flag(6, TRUE)));
   m_column_info_list.Add(column_info_t("Elevation Source Map Scale", FALSE, TRUE, get_show_flag(7, TRUE)));
   m_column_info_list.Add(column_info_t("Description",FALSE, TRUE, get_show_flag(8, TRUE)));
   m_column_info_list.Add(column_info_t("Location", FALSE, TRUE, get_show_flag(9, TRUE)));
   m_column_info_list.Add(column_info_t("Altitude (in feet)", FALSE, TRUE, get_show_flag(10, TRUE)));
   m_column_info_list.Add(column_info_t("Horizontal Accuracy (in feet)", FALSE, TRUE, get_show_flag(11, FALSE)));
   m_column_info_list.Add(column_info_t("Vertical Accuracy (in feet)", FALSE, TRUE, get_show_flag(12, FALSE)));
   m_column_info_list.Add(column_info_t("Links", TRUE, TRUE, get_show_flag(13, TRUE)));
   m_column_info_list.Add(column_info_t("Comments", FALSE, TRUE, get_show_flag(14, TRUE)));
   m_column_info_list.Add(column_info_t("Country", FALSE, FALSE, get_show_flag(15, FALSE)));
   m_column_info_list.Add(column_info_t("ICAO Area", FALSE, FALSE, get_show_flag(16, FALSE)));
   m_column_info_list.Add(column_info_t("Point Quality", FALSE, FALSE, get_show_flag(17, FALSE)));


   update_translation_table();

   m_point_info = NULL;
   m_sort_last_row = FALSE;

   create_new_pre_point();
}

void LocalPointDataObject::GetColumnInfoList(CList<column_info_t, column_info_t> &column_info_list)
{
   const int size = m_column_info_list.GetSize();
   for(int i = 0;i<size;i++)
      column_info_list.AddTail(m_column_info_list[i]);
}

void LocalPointDataObject::create_new_pre_point()
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   m_pre_point = new C_localpnt_point(overlay);

   CNewLocalPointDefaults new_point_defaults;
   m_pre_point->m_group_name = new_point_defaults.GetGroupName();
   m_pre_point->m_id = new_point_defaults.get_and_increment_id();

   // set default icon for the new point
   m_pre_point->SetIconName("localpnt\\red_dot.ico");
   m_pre_point->m_icon_image = CIconImage::load_images(
      m_pre_point->GetIconName());
}

void LocalPointDataObject::update_translation_table()
{
   m_translation_table.RemoveAll();
   const int size = m_column_info_list.GetSize();
   for(int i = 0;i<size;i++)
   {
      if (m_column_info_list[i].is_shown)
         m_translation_table.Add(i);
   }
}

void LocalPointDataObject::translate(int &col)
{
   if (m_translation_on && col >= 0 && col < GetNumCols())
      col = m_translation_table[col];
}

BOOL LocalPointDataObject::get_show_flag(int col, BOOL default_value)
{
   CString section = "TabularEditor\\LocalPoint";
   CString key;
   key.Format("%d", col);

   return (BOOL)PRM_get_registry_int(section, key, default_value);
}

void LocalPointDataObject::set_show_flag(int col, BOOL is_shown)
{
   CString section = "TabularEditor\\LocalPoint";
   CString key;
   key.Format("%d", col);

   PRM_set_registry_int(section, key, is_shown);
   m_column_info_list[col].is_shown = is_shown;
}

LocalPointDataObject::~LocalPointDataObject()
{
   delete m_point_info;
   delete m_pre_point;
}

void LocalPointDataObject::on_modified()
{
   delete m_point_info;
   m_point_info = NULL;
}

int LocalPointDataObject::GetNumCols()
{
   if (m_translation_on)
      return m_translation_table.GetSize();

   return m_column_info_list.GetSize();
}

BOOL LocalPointDataObject::GetIsShown(int col)
{
   return m_column_info_list[col].is_shown;
}

void LocalPointDataObject::SetIsShown(CList<BOOL, BOOL> &show_flags_list)
{
   POSITION position = show_flags_list.GetHeadPosition();
   int col_index = 0;

   while(position)
      set_show_flag(col_index++, show_flags_list.GetNext(position));

   update_translation_table();
}

int LocalPointDataObject::GetColName(int col, CString *string)
{
   translate(col);
   *string = m_column_info_list[col].column_name;

   return UG_SUCCESS;
}

long LocalPointDataObject::GetNumRows()
{
   // remember to return an additional row to allow the user to enter data

   const int current_tab = m_ctrl->GetCurrentTab();
   CString current_group = m_ctrl->m_CUGTab->GetTabLabel(current_tab);

   if (current_group == "All Points")
      return m_pt_list.GetCount() + 1;

   POSITION position = m_pt_list.GetHeadPosition();
   long count = 0;
   while (position)
   {
      if (m_pt_list.GetNext(position)->m_group_name == current_group)
         count++;
   }

   return count + 1;
}

int LocalPointDataObject::GetCell(int col, long row, CUGCell *cell)
{
   int ret = SUCCESS;

   const long num_rows = m_ctrl->GetNumberRows();
   translate_row(row);

   // draw arrow in left-most column to denote the selected icon
   if ( row >= 0 && col == -1 )
   {
      C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
      C_localpnt_point *currently_selected_point = overlay->get_current_selection();

      cell->SetText("");

      if (currently_selected_point == get_at_index(row))
      {
         cell->SetCellType(UGCT_ARROW);
         cell->SetCellTypeEx(UGCT_ARROWRIGHT);
      }
      else if (row == (num_rows - 1))
         cell->SetText("*");
      else
         cell->SetCellType(UGCT_NORMAL);
   }

   // get column headers
   if ( col >= 0 && row == -1 )
   {
      CString column_name;
      GetColName(col, &column_name);
      cell->SetText(column_name);
   }

   // get data cells
   else if (col >= 0 && row >= 0)
   {
      translate(col);
      switch(col)
      {
      case 0:
         ret = GetCell_Icon(row, cell);
         break;

      case 1:
         ret = GetCell_ID(row, cell);
         break;

      case 2:
         ret = GetCell_Group(row, cell);
         break;

      case 3:
         ret = GetCell_DTD_ID(row, cell);
         break;

      case 4:
         ret = GetCell_Elevation(row, cell);
         break;

      case 5:
         ret = GetCell_ElevationSource(row, cell);
         break;

      case 6:
         ret = GetCell_ElevationSourceMapHandlerName(row, cell);
         break;

      case 7:
         ret = GetCell_ElevationSourceMapSeriesName(row, cell);
         break;

      case 8:
         ret = GetCell_Description(row, cell);
         break;

      case 9:
         ret = GetCell_Location(row, cell);
         break;

      case 10:
         ret = GetCell_Altitude(row, cell);
         break;

      case 11:
         ret = GetCell_HorzAcc(row, cell);
         break;

      case 12:
         ret = GetCell_VertAcc(row, cell);
         break;

      case 13:
         ret = GetCell_Links(row, cell);
         break;

      case 14:
         ret = GetCell_Comments(row, cell);
         break;

      case 15:
         ret = GetCell_Country(row, cell);
         break;

      case 16:
         ret = GetCell_ICAO_Area(row, cell);
         break;

      case 17:
         ret = GetCell_PointQuality(row, cell);
         break;
      }
   }

   return ret;
}

int LocalPointDataObject::OnEditStart(int col, long row,CWnd **edit)
{
   translate(col);
   translate_row(row);

   if (m_column_info_list[col].hard_edit && !m_hard_edit)
      return FALSE;
   if (!m_column_info_list[col].allow_edit)
      return FALSE;

   if (row < 0 || row >= GetNumRows())
      return FALSE;

   // custom edit types
   if (m_column_info_list[col].column_name == "Icon")
   {
      OnEditIcon(row, *edit);
      return FALSE;
   }
   if (m_column_info_list[col].column_name == "Links")
   {
      OnEditLinks(row);
      return FALSE;
   }

   return TRUE;
}

int LocalPointDataObject::OnEditVerify(int col,long row,CWnd *edit,UINT *vcKey)
{
   CString column_name;
   GetColName(col, &column_name);

   if (column_name == "DTD ID")
   {
      if ( edit->GetWindowTextLength() >= MAX_DTD_ID_LENGTH && (*vcKey != VK_BACK && *vcKey != VK_DELETE))
         return FALSE;

      return TRUE;
   }

   if (column_name == "Elevation (in feet)")
   {
      const BOOL bAllowMetersSymbol = TRUE;
      return LimitNum(reinterpret_cast<long>(edit->GetSafeHwnd()), (char)*vcKey, MIN_ELEVATION, MAX_ELEVATION, 0, bAllowMetersSymbol);
   }

   if (column_name == "Vertical Accuracy (in feet)" ||
       column_name == "Horizontal Accuracy (in feet)")
       return LimitNum(reinterpret_cast<long>(edit->GetSafeHwnd()), (char)*vcKey, 0, SHRT_MAX, 20);

   return TRUE;
}

int LocalPointDataObject::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
   if (cancelFlag)
      return TRUE;

   CString column_name;
   GetColName(col, &column_name);

   translate_row(row);

   if (column_name == "ID")
      return OnEditID(row, string);

   if (column_name == "Group")
      return OnEditGroup(row, string);

   if (column_name == "DTD ID")
      return OnEditDTD_ID(row, string);

   if (column_name == "Elevation (in feet)")
      return OnEditElevation(row, string);

   if (column_name == "Description")
      return OnEditDescription(row, string);

   if (column_name == "Location")
      return OnEditLocation(row, string);

   if (column_name == "Altitude (in feet)")
      return OnEditAltitude(row, string);

   if (column_name == "Vertical Accuracy (in feet)")
      return OnEditVertAcc(row, string);

   if (column_name == "Horizontal Accuracy (in feet)")
      return OnEditHorzAcc(row, string);

   if (column_name == "Comments")
      return OnEditComments(row, string);

   return TRUE;
}

int LocalPointDataObject::OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow)
{
   if (*newrow < 0 || *newrow >= GetNumRows())
      return FALSE;

   return TRUE;
}

void LocalPointDataObject::OnSelChanged(int col, long row, CString text)
{
   CString column_name;
   GetColName(col, &column_name);

   translate_row(row);

   if (column_name == "Group")
      OnEditGroup(row, text);
}

void LocalPointDataObject::OnMenuStart(int col, int row, int base_ID)
{
   if (row == (m_ctrl->GetNumberRows() - 1))
      return;

   m_ctrl->AddMenuItem(base_ID, "Center Map");
   m_ctrl->AddMenuItem(base_ID + 1, "Delete");
   m_ctrl->AddMenuItem(base_ID + 2, "Add to Favorites...");
   m_ctrl->AddMenuItem(base_ID + 3, "Target Graphic Printing...");
   m_ctrl->AddMenuItem(-1, "");  // separator

   m_ctrl->AddMenuItem(base_ID + 4, "Update Elevation");
   m_ctrl->AddMenuItem(base_ID + 5, "Update All Elevations");
   m_ctrl->AddMenuItem(base_ID + 6, "Edit Point...");
}

void LocalPointDataObject::OnMenuCommand(int col, long row, int item)
{
   translate_row(row);

   C_localpnt_point *point = get_at_index(row);
   if (point == NULL)
      return;

   CFavoriteImportDlg dlg;
   CList<d_geo_t, d_geo_t> location_list;
   d_geo_t location;
   CString string;
   CList<CString, CString> name_list;

   // Target Graphics variables and data
   degrees_t lat;
   degrees_t lon;
   point->get_lat(lat);
   point->get_lon(lon);

   CString links = "";
   CStringArray links_array;
   C_localpnt_ovl *ovl = static_cast<C_localpnt_ovl*>(point->m_overlay);
   if (ovl != nullptr)
   {
      ovl->get_links_for_point(point, links_array);
      for (int i = 0; i < links_array.GetCount(); i++)
      {
         CString link = links_array.GetAt(i);
         links = links + link;
         if (i != links_array.GetCount() - 1)
            links = links + ",";
      }
   }

   CTargetGraphicsDlg target_dlg(m_ctrl->GetParent());
   CList<target_t, target_t> selected_list;
   target_t sel_item;

   switch(item)
   {
      // Center Map
   case 0:
      center_map(point);
      break;

      // Delete
   case 1:
      delete_row(row);
      break;

      // Add to Favorites...
   case 2:
      if (get_location_at(row, m_ctrl->GetCurrentTab(), location, string))
      {
         location_list.AddTail(location);
         dlg.set_location_list(&location_list);

         name_list.AddTail(string);
         dlg.set_name_list(&name_list);
         dlg.DoModal();
      }
      break;

      // Target Graphic Printing...
   case 3:
      sel_item.comment = point->m_comment;
      sel_item.description = point->m_description;
      sel_item.id = point->m_id;
      sel_item.lat = lat;
      sel_item.lon = lon;
      sel_item.links = links;
      selected_list.AddTail(sel_item);

      //target_dlg.set_data_object(this);
      target_dlg.set_selected_list(&selected_list);
      target_dlg.DoModal();

      break;

      // Update Elevation
   case 4:

      {
         int nResult = IDYES;
         if (point->m_elv_src == "USER" || point->m_elv_src == "DAFIF")
         {
            CString msg;
            msg.Format("The elevation for this point was %s.  Are you sure you want to overwrite the elevation for this point?",
               point->m_elv_src == "USER" ? "entered manually" : "obtained from DAFIF");

            nResult = ::MessageBox(m_ctrl->m_hWnd, msg, "FalconView", MB_YESNO | MB_ICONQUESTION);
         }

         if (nResult == IDYES)
         {
            const BOOL bDTEDAvailable = update_elevation(point);
            if (!bDTEDAvailable)
               ::MessageBox(m_ctrl->m_hWnd, "No DTED is available for the selected point.", MB_OK, MB_ICONEXCLAMATION);
         }
      }
      break;

      // Update All Elevations
   case 5:
      {
         CWaitCursor wait_cursor;
         BOOL bSomeMapElevationDataMissing = FALSE;
         BOOL bMapElevationDataAvailable = TRUE;
         CString pointIDs;
         int numPointsMissingMapElevationData = 0;

         C_localpnt_ovl* overlay = static_cast<C_localpnt_ovl*>(m_parent_overlay);
         overlay->set_importing(true);

         POSITION position = m_pt_list.GetHeadPosition();
         while (position)
         {
            C_localpnt_point *point = m_pt_list.GetNext(position);
            if (point->m_elv_src != "USER" && point->m_elv_src != "DAFIF") // do not wipe out "USER" or "DAFIF"
            {
               bMapElevationDataAvailable = update_elevation(point);

               if (!bMapElevationDataAvailable)
               {
                  bSomeMapElevationDataMissing = TRUE;
                  numPointsMissingMapElevationData++;

                  if (numPointsMissingMapElevationData == 1)
                  {
                     CString newPointID;
                     newPointID.Format("%s", point->m_id);
                     pointIDs += newPointID;
                  }
                  else
                  {
                     if (numPointsMissingMapElevationData <= 20)
                     {
                        CString newPointID;

                        if (numPointsMissingMapElevationData == 8 || numPointsMissingMapElevationData == 16)
                           pointIDs += ",\n";
                        else
                           pointIDs += ", ";


                        if (numPointsMissingMapElevationData == 20)
                           newPointID.Format("%s...", point->m_id);
                        else
                           newPointID.Format("%s", point->m_id);

                        pointIDs += newPointID;
                     }
                  }
               }
            }
         }

         overlay->set_importing(false);

         if (bSomeMapElevationDataMissing)
         {
            CString msg;
            if (numPointsMissingMapElevationData == 1)
               msg.Format("Map elevation data are not available for the following Local Point: %s", pointIDs);
            else if (numPointsMissingMapElevationData > 5)
               msg.Format("Map elevation data are not available for the following Local Points:\n%s", pointIDs);
            else
               msg.Format("Map elevation data are not available for the following Local Points: %s", pointIDs);

            ::MessageBox(m_ctrl->m_hWnd, msg, MB_OK, MB_ICONEXCLAMATION);
         }
      }
      break;

      // Edit Point...
   case 6:
      C_localpnt_ovl::edit_obj_properties(point);
      break;
   }
}

void LocalPointDataObject::on_selection_changed()
{
   // the selection was changed, insure that the selected row is visible
   //

   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   long row = get_index(overlay->get_current_selection());
   inverse_translate_row(row);
   m_ctrl->GotoCell(0, row);
}

int LocalPointDataObject::SortBy(int *cols, int num, int flags)
{
   if (num < 0)
      return UG_ERROR;

   const int sort_col = cols[0];

   CString column_name;
   GetColName(sort_col, &column_name);

   BOOL is_string = TRUE;
   if (column_name == "Elevation (in feet)" ||
       column_name == "Horizontal Accuracy (in feet)" ||
       column_name == "Vertical Accuracy (in feet)")
       is_string = FALSE;

   if (flags == UG_SORT_ASCENDING)
      CellStringEntry::sort_type = is_string ?
TABULAR_ED_SORT_TYPE_STRING_ASCENDING : TABULAR_ED_SORT_TYPE_NUMBER_ASCENDING;

   else if (flags == UG_SORT_DESCENDING)
      CellStringEntry::sort_type = is_string ?
TABULAR_ED_SORT_TYPE_STRING_DECENDING : TABULAR_ED_SORT_TYPE_NUMBER_DECENDING;

   TabularEditorDataObject::SortBy(sort_col);

   return UG_SUCCESS;
}

void LocalPointDataObject::OnDragDrop(long from_row, int to_tab)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(from_row);

   if (point == NULL || point == m_pre_point)
      return;

   // get the name of the tab that the point was dropped to
   CString new_group_name = m_ctrl->m_CUGTab->GetTabLabel(to_tab);

   if (new_group_name == "All Points")
      return;

   point_info_t point_info;
   overlay->lookup_info(point, &point_info);
   point_info.group_name = new_group_name;
   int ret = overlay->apply_changes(point, point_info, FALSE);
   update_edit_dialog(point);
   ReSort();
}

void LocalPointDataObject::MoveToOverlay(long nFromRow, CString strToOverlay)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(nFromRow);

   if (point == NULL || point == m_pre_point)
      return;

   C_overlay *pOverlay = OVL_get_overlay_manager()->FindOverlayByDisplayName(strToOverlay);
   if (pOverlay->get_m_overlayDescGuid() != FVWID_Overlay_Points || overlay == pOverlay)
      return;

   C_localpnt_ovl *pOverlayTo = static_cast<C_localpnt_ovl *>(pOverlay);

   POSITION pos = m_pt_list.Find(point);
   if (pos != NULL)
   {
      m_pt_list.RemoveAt(pos);
      point->SetParentOverlay(pOverlayTo);
      static_cast<LocalPointDataObject *>(pOverlayTo->GetTabularEditorDataObject())->AddTail(point);

      overlay->remove_point_from_selections(point);

      overlay->set_modified(TRUE);
      pOverlayTo->set_modified(TRUE);

      if (C_localpnt_ovl::m_EditDialogControl.DialogExists())
         C_localpnt_ovl::m_EditDialogControl.SetFocus(point);

      OVL_get_overlay_manager()->InvalidateOverlay(overlay);
      OVL_get_overlay_manager()->InvalidateOverlay(pOverlayTo);
   }
}

void LocalPointDataObject::MoveGroupToOverlay(long from_group, CString strToOverlay)
{
   // Get overlays that we're copying from and to
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_overlay *pOverlay = OVL_get_overlay_manager()->FindOverlayByDisplayName(strToOverlay);
   if (pOverlay->get_m_overlayDescGuid() != FVWID_Overlay_Points || overlay == pOverlay)
      return;

   C_localpnt_ovl *pOverlayTo = static_cast<C_localpnt_ovl *>(pOverlay);

   // Get the group name
   if (from_group == -1)
      from_group = m_ctrl->GetCurrentTab();
   CString current_group = "All Points";
   if (from_group != 0)
      current_group = m_ctrl->m_CUGTab->GetTabLabel(from_group);

   // Go through the list, and move points
   C_localpnt_point *final_point = NULL;
   BOOL modified = FALSE;
   int counter = 0;
   POSITION position = m_pt_list.FindIndex(counter);
   while (position)
   {
      C_localpnt_point *point = m_pt_list.GetAt(position);
      if (point->m_group_name == current_group)
      {
         m_pt_list.RemoveAt(position);
         point->SetParentOverlay(pOverlayTo);
         static_cast<LocalPointDataObject *>(pOverlayTo->GetTabularEditorDataObject())->AddTail(point);

         overlay->remove_point_from_selections(point);

         final_point = point;
         modified = TRUE;

         counter--;
      }

      counter++;
      position = m_pt_list.FindIndex(counter);
   }

   if (modified == TRUE)
   {
      overlay->set_modified(TRUE);
      pOverlayTo->set_modified(TRUE);

      if (C_localpnt_ovl::m_EditDialogControl.DialogExists())
         C_localpnt_ovl::m_EditDialogControl.SetFocus(final_point);

      // Remove group from old overlay
      position = overlay->m_group_lst.GetHeadPosition();
      while (position)
      {
         POSITION old_pos = position;
         POINT_GROUP *group = overlay->m_group_lst.GetNext(position);
         if (group->name == current_group)
         {
            overlay->m_group_lst.RemoveAt(old_pos);
            break;
         }
      }

      // Add group to new overlay
      POINT_GROUP* pGroup = new POINT_GROUP;
      pGroup->name = current_group;
      pGroup->search = TRUE;
      pOverlayTo->m_group_lst.AddTail(pGroup);

      OVL_get_overlay_manager()->InvalidateOverlay(overlay);
      OVL_get_overlay_manager()->InvalidateOverlay(pOverlayTo);
   }
}

void LocalPointDataObject::OnTabSelected(int nId)
{
   delete m_point_info;
   m_point_info = NULL;
}

int LocalPointDataObject::GetCell_Icon(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   cell->SetCellType(FVCT_ICON);

   std::string icon_txt;
   if (point == m_pre_point)
   {
      icon_txt = m_pre_point->GetIconName();
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      icon_txt = m_point_info->icon;
   }

   std::string format("%s\\icons\\");
   format += icon_txt;

   CString icon_name;
   icon_name.Format(format.c_str(), PRM_get_registry_string("Main", "HD_DATA"));

   cell->SetText(icon_name);

   return UG_SUCCESS;
}

void LocalPointDataObject::OnEditIcon(int row, CWnd *parent_edit)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   MapProj *map = (MapProj *)UTL_get_current_view_map();

   CString initial_dir = PRM_get_registry_string("Main", "HD_DATA", "") + "\\icons\\localpnt\\*.ico";

   static char szFilter[] = "Icon Files (*.ico)|*.ico||";
   CFileDialog dlg(TRUE, NULL, initial_dir, OFN_FILEMUSTEXIST, szFilter, parent_edit);
   dlg.m_ofn.lpstrTitle = "Select New Icon";

   if (dlg.DoModal() == IDOK)
   {
      C_localpnt_point *point = get_at_index(row);

      if (point == m_pre_point)
      {
         m_pre_point->m_icon_image = CIconImage::load_images(dlg.GetPathName(), "", 0);
         m_pre_point->SetIconName(m_pre_point->m_icon_image->get_item_title());
      }
      else
      {
         point_info_t point_info;
         overlay->lookup_info(point, &point_info);

         point->m_icon_image = CIconImage::load_images(dlg.GetPathName(), "", 0);
         point_info.icon = point->m_icon_image->get_item_title();

         overlay->apply_changes(point, point_info, TRUE);
         update_edit_dialog(point);
      }
   }
}

int LocalPointDataObject::GetCell_ID(int row, CUGCell *cell)
{
   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   cell->SetText(point->m_id);

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditID(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   if (point == m_pre_point)
   {
      m_pre_point->m_id = string;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.id = string;
      ret = overlay->apply_changes(point, point_info, FALSE);
      update_edit_dialog(point);
   }

   return (ret == SUCCESS);
}

int LocalPointDataObject::GetCell_Group(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   cell->SetCellType(UGCT_DROPLIST);
   cell->SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
   cell->SetLabelText(overlay->get_group_list());

   cell->SetText(point->m_group_name);

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditGroup(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   if (point == m_pre_point)
   {
      m_pre_point->m_group_name = string;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.group_name = string;
      ret = overlay->apply_changes(point, point_info, FALSE);
      update_edit_dialog(point);
      ReSort();
   }

   return (ret == SUCCESS);
}

int LocalPointDataObject::GetCell_DTD_ID(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString dtd_txt;
   if (point == m_pre_point)
      dtd_txt = m_pre_point->m_dtd_id;
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      dtd_txt = m_point_info->dtd_id;
   }

   cell->SetText(dtd_txt);

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditDTD_ID(int row, CString string)
{
   // limit the length of the string
   string = string.Left(MAX_DTD_ID_LENGTH);

   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   if (point == m_pre_point)
   {
      strcpy(m_pre_point->m_dtd_id, string);
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.dtd_id = string;
      ret = overlay->apply_changes(point, point_info, FALSE);
      update_edit_dialog(point);
   }

   return (ret == SUCCESS);
}

int LocalPointDataObject::GetCell_Elevation(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString elevation_src;
   int elev;

   if (point == m_pre_point)
   {
      elevation_src = m_pre_point->m_elv_src;
      elev = m_pre_point->m_elv;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      elevation_src = m_point_info->elevation_source;
      elev = m_point_info->elevation;
   }

   if (elevation_src == "UNK")
   {
      cell->SetDataType(UGCELLDATA_STRING);
      cell->SetText("");
   }
   else
   {
      cell->SetDataType(UGCELLDATA_NUMBER);
      cell->SetNumberDecimals(0);
      cell->SetNumber(elev);
   }

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditElevation(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   int elevation;

   // if the elevation string is empty, set elevation to NULL. otherwise, convert string value to an int...
   if (string == "")
      elevation = NULL;
   else
      elevation = atoi((LPCTSTR)string);

   const int nLength = string.GetLength();
   if (nLength && (string[nLength - 1] == 'M' || string[nLength - 1] == 'm'))
   {
      elevation = atoi((LPCTSTR)string.Left(nLength - 1));
      elevation = static_cast<int>(METERS_TO_FEET(elevation));
   }

   // The elevation should be limited to a short
   elevation = __max(__min(MAX_ELEVATION, elevation), MIN_ELEVATION);

   if (point == m_pre_point)
   {
      if (m_pre_point->m_elv != elevation)
      {
         // if user input an empty string, set the elevation source to "UNK". Otherwise, the elevation is
         // still a user-defined value, so the elevation source should be "USER".
         if (string == "")
            m_pre_point->m_elv_src = "UNK";
         else
            m_pre_point->m_elv_src = "USER";
      }

      m_pre_point->m_elv = elevation;
      m_pre_point->m_elev_src_map_handler_name = "UNK";
      m_pre_point->m_elev_src_map_series_id = 0;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);

      if (point_info.elevation != elevation)
      {
         // if user input an empty string, set the elevation source to "UNK". Otherwise, the elevation is
         // still a user-defined value, so the elevation source should be "USER".
         if (string == "")
            point_info.elevation_source = "UNK";
         else
            point_info.elevation_source = "USER";
      }

      point_info.elevation = elevation;
      point_info.elevation_source_map_handler_name = "UNK";
      point_info.elevation_source_map_series_id = 0;
      ret = overlay->apply_changes(point, point_info, FALSE);
   }

   update_edit_dialog(point);

   return (ret == SUCCESS);
}


int LocalPointDataObject::GetCell_ElevationSourceMapHandlerName(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString src;

   if (point == m_pre_point)
   {
      src = m_pre_point->m_elev_src_map_handler_name;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      src = m_point_info->elevation_source_map_handler_name;
   }

   cell->SetText(src);
   cell->SetTextColor(RGB(132,130,132));
   cell->SetBackColor(RGB(214,211,206));
   cell->SetReadOnly(TRUE);

   return UG_SUCCESS;
}

int LocalPointDataObject::GetCell_ElevationSourceMapSeriesName(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString src;

   if (point == m_pre_point)
   {
      short id = point->m_elev_src_map_series_id;
      src = C_localpnt_ovl::id_to_series_name(point->m_elev_src_map_handler_name, id);
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      short id = point->m_elev_src_map_series_id;
      src = C_localpnt_ovl::id_to_series_name(point->m_elev_src_map_handler_name, id);
   }

   cell->SetText(src);
   cell->SetTextColor(RGB(132,130,132));
   cell->SetBackColor(RGB(214,211,206));
   cell->SetReadOnly(TRUE);

   return UG_SUCCESS;
}
////////////////////////////////




int LocalPointDataObject::GetCell_ElevationSource(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString src;

   if (point == m_pre_point)
   {
      src = m_pre_point->m_elv_src;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      src = m_point_info->elevation_source;
   }

   cell->SetText(src);
   cell->SetTextColor(RGB(132,130,132));
   cell->SetBackColor(RGB(214,211,206));
   cell->SetReadOnly(TRUE);

   return UG_SUCCESS;
}

int LocalPointDataObject::GetCell_Description(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString descr;
   if (point == m_pre_point)
   {
      descr = m_pre_point->m_description;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      descr = m_point_info->description;
   }

   cell->SetText(descr);

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditDescription(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   if (point == m_pre_point)
   {
      m_pre_point->m_description = string;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.description = string;
      ret = overlay->apply_changes(point, point_info, FALSE);
      update_edit_dialog(point);
   }

   return (ret == SUCCESS);
}

int LocalPointDataObject::GetCell_Location(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   char geo_string[GEO_MAX_LAT_LON_STRING+1];

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   d_geo_t location;

   if (point == m_pre_point)
   {
      cell->SetText("");
      return UG_SUCCESS;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      location.lat = m_point_info->latitude;
      location.lon = m_point_info->longitude;
   }

   int result = m_geotrans.DLL_lat_lon_to_geo(location.lat, location.lon,
      geo_string, GEO_MAX_LAT_LON_STRING+1);
   if (result != SUCCESS)
   {
      ERR_report("Unable to convert lat, lon to a string");
      return UG_ERROR;
   }

   cell->SetText(CString(geo_string));

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditLocation(int row, CString string)
{
   char geo_string[GEO_MAX_LAT_LON_STRING+1];

   // get default datum
   char datum[6], display_type[21];
   GEO_get_default_display(display_type, 21);
   if (strcmp(display_type, "SECONDARY") == 0)
      GEO_get_secondary_datum(datum, 6);
   else
      GEO_get_primary_datum(datum, 6);

   // verify location
   double lat = 0.0, lon = 0.0;
   int status = m_geotrans.DLL_location_to_geo(string, datum, lat, lon, geo_string);
   int ret;

   if (status != SUCCESS)
   {
      m_geotrans.DLL_report_string_to_degrees_error(status);
      return TRUE;
   }

   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);

   if (point == m_pre_point)
   {
      m_pre_point->m_latitude = static_cast<float>(lat);
      m_pre_point->m_longitude = static_cast<float>(lon);
      ret = SUCCESS;

      // if we are adding a point in another group tab, automatically set
      // the group of the point so it shows up in that tab
      const int current_tab = m_ctrl->GetCurrentTab();
      CString current_group = m_ctrl->m_CUGTab->GetTabLabel(current_tab);
      if (current_group != "All Points")
         m_pre_point->m_group_name = current_group;

      // now that a valid location has been input for the pre-point, we will
      // promote it and add it to the list
      if (overlay->insert_point(NULL, m_pre_point, NULL) != SUCCESS)
      {
         ERR_report("Unable to add new point to the Points overlay");
         return FALSE;
      }

      create_new_pre_point();

      m_ctrl->Invalidate();
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.latitude = lat;
      point_info.longitude = lon;
      ret = overlay->apply_changes(point, point_info, TRUE);
      update_edit_dialog(point);
   }

   return TRUE;
}

int LocalPointDataObject::GetCell_Altitude(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   double altitude_meters;
   if (point == m_pre_point)
   {
      altitude_meters = m_pre_point->m_altitude_meters;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      altitude_meters = m_point_info->altitude_meters;
   }

   cell->SetDataType(UGCELLDATA_NUMBER);
   cell->SetNumberDecimals(0);
   cell->SetNumber(METERS_TO_FEET(altitude_meters));

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditAltitude(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   float f;
   if (sscanf((LPCTSTR)string, "%f", &f) == 0)
      return FALSE;

   double altitude_meters = FEET_TO_METERS(f);

   if (point == m_pre_point)
   {
      point->m_altitude_meters = altitude_meters;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.altitude_meters = altitude_meters;
      ret = overlay->apply_changes(point, point_info, TRUE);
      update_edit_dialog(point);
   }

   return TRUE;
}

int LocalPointDataObject::GetCell_VertAcc(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   float vert_acc;
   if (point == m_pre_point)
   {
      vert_acc = m_pre_point->m_vert_accuracy;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      vert_acc = m_point_info->vert_acc;
   }

   if (vert_acc < 0)
   {
      cell->SetDataType(UGCELLDATA_STRING);
      cell->SetText("");
   }
   else
   {
      cell->SetDataType(UGCELLDATA_NUMBER);
      cell->SetNumberDecimals(2);
      cell->SetNumber(vert_acc);
   }

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditVertAcc(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   float f;
   if (sscanf((LPCTSTR)string, "%f", &f) == 0)
      return FALSE;

   if (point == m_pre_point)
   {
      point->m_vert_accuracy = f;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.vert_acc = f;
      ret = overlay->apply_changes(point, point_info, TRUE);
      update_edit_dialog(point);
   }

   return TRUE;
}

int LocalPointDataObject::GetCell_HorzAcc(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   float horz_acc;
   if (point == m_pre_point)
   {
      horz_acc = m_pre_point->m_horz_accuracy;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      horz_acc = m_point_info->horz_acc;
   }

   if (horz_acc < 0)
   {
      cell->SetDataType(UGCELLDATA_STRING);
      cell->SetText("");
   }
   else
   {
      cell->SetDataType(UGCELLDATA_NUMBER);
      cell->SetNumberDecimals(2);
      cell->SetNumber(horz_acc);
   }

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditHorzAcc(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   float f;
   if (sscanf((LPCTSTR)string, "%f", &f) == 0)
      return FALSE;

   if (point == m_pre_point)
   {
      m_pre_point->m_horz_accuracy = f;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.horz_acc = f;
      ret = overlay->apply_changes(point, point_info, TRUE);
      update_edit_dialog(point);
   }

   return TRUE;
}

int LocalPointDataObject::GetCell_Links(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CStringArray links;
   if (point != m_pre_point)
      overlay->get_links_for_point(point, links);

   if (links.GetSize())
   {
      cell->SetCellType(FVCT_NOTE);
      cell->SetText(links[0]);

      CString note_text;
      const int size = links.GetSize();
      for(int i=0;i<size;i++)
      {
         note_text += links[i];
         if (i != (size - 1))
            note_text += "\n";
      }
      cell->SetLabelText(note_text);
   }
   else
      cell->SetCellType(UGCT_NORMAL);

   return UG_SUCCESS;
}

void LocalPointDataObject::OnEditLinks(int row)
{
   // open the links dialog for the particular row
   C_localpnt_ovl::edit_local_point_links(get_at_index(row));
}

int LocalPointDataObject::GetCell_Comments(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString comment;
   if (point == m_pre_point)
   {
      comment = m_pre_point->m_comment;
   }
   else
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      comment = m_point_info->comment;
   }

   cell->SetText(comment);

   return UG_SUCCESS;
}

int LocalPointDataObject::GetCell_Country(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   cell->SetTextColor(RGB(132,130,132));
   cell->SetBackColor(RGB(214,211,206));

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString country;
   if (point != m_pre_point)
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      CFvwUtil *futil = CFvwUtil::get_instance();
      char tstr[41];
      if (futil->find_country((char *)(LPCTSTR)m_point_info->country_code, tstr))
         country = CString(tstr);
   }

   cell->SetText(country);

   return UG_SUCCESS;
}

int LocalPointDataObject::GetCell_ICAO_Area(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   cell->SetTextColor(RGB(132,130,132));
   cell->SetBackColor(RGB(214,211,206));

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString area_str;
   if (point != m_pre_point)
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      area_str = CString(m_point_info->area_code);
   }

   cell->SetText(area_str);

   return UG_SUCCESS;
}

int LocalPointDataObject::GetCell_PointQuality(int row, CUGCell *cell)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   cell->SetTextColor(RGB(132,130,132));
   cell->SetBackColor(RGB(214,211,206));

   if (row < 0 || row >= GetNumRows())
      return UG_ERROR;

   C_localpnt_point *point = get_at_index(row);

   CString quality;
   if (point != m_pre_point)
   {
      if (m_point_info == NULL || m_point_info->index != row)
      {
         if (m_point_info == NULL)
            m_point_info = new point_info_t;
         overlay->lookup_info(point, m_point_info);
         m_point_info->index = row;
      }

      quality = CString(m_point_info->qlt);
   }

   cell->SetText(quality);

   return UG_SUCCESS;
}

int LocalPointDataObject::OnEditComments(int row, CString string)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = get_at_index(row);
   int ret;

   if (point == m_pre_point)
   {
      m_pre_point->m_comment = string;
      ret = SUCCESS;
   }
   else
   {
      point_info_t point_info;
      overlay->lookup_info(point, &point_info);
      point_info.comment = string;
      ret = overlay->apply_changes(point, point_info, TRUE);
      update_edit_dialog(point);
   }

   return TRUE;
}


void LocalPointDataObject::clear()
{
   // remove (and delete) all points from icon list
   while (!m_pt_list.IsEmpty())
   {
      delete m_pt_list.RemoveHead();
   }
}

void LocalPointDataObject::remove(C_localpnt_point *point)
{
   POSITION pos = m_pt_list.Find(point);

   ASSERT(pos != NULL);  // should always find point

   if (pos != NULL)
   {
      m_pt_list.RemoveAt(pos);

      if (m_parent_overlay != nullptr)
      {
         C_localpnt_ovl *overlay =
            static_cast<C_localpnt_ovl *>(m_parent_overlay);
         overlay->Remove3DPoint(point);
      }

      delete point;
      point = NULL;

      ReSort();
   }
}

// return the position of a point in the list
POSITION LocalPointDataObject::Find(C_localpnt_point *point)
{
   return m_pt_list.Find(point);
}

C_localpnt_point *LocalPointDataObject::FindById(CString id)
{
   // local point IDs are only 12 character max
   id = id.Left(12);

   POSITION position = m_pt_list.GetHeadPosition();
   while (position)
   {
      C_localpnt_point *pPoint = m_pt_list.GetNext(position);
      if (pPoint->m_id == id)
         return pPoint;
   }

   return NULL;
}

C_localpnt_point *LocalPointDataObject::FindByGuid(CString strGuid)
{
   POSITION position = m_pt_list.GetHeadPosition();
   while (position)
   {
      C_localpnt_point *pPoint = m_pt_list.GetNext(position);
      if (strGuid == pPoint->GetObjectGuid().c_str())
         return pPoint;
   }

   return NULL;
}

int LocalPointDataObject::get_index(C_localpnt_point *point, CString group /*= ""*/)
{
   CString current_group;
   if (group == "")
   {
      const int current_tab = m_ctrl->GetCurrentTab();
      current_group = m_ctrl->m_CUGTab->GetTabLabel(current_tab);
   }
   else
      current_group = group;

   POSITION position = m_pt_list.GetHeadPosition();
   int index = 0;
   while (position)
   {
      C_localpnt_point *P = m_pt_list.GetNext(position);

      if (current_group != "All Points" && P->m_group_name != current_group)
         continue;

      if (P == point)
         return index;
      index++;
   }

   return -1;
}

// Nonzero if the list in empty, 0 otherwise
BOOL LocalPointDataObject::IsEmpty() const
{
   return m_pt_list.IsEmpty();
}

// Gets the position of the tail element of this list; NULL if the list is empty.
POSITION LocalPointDataObject::GetTailPosition()
{
   return m_pt_list.GetTailPosition();
}

POSITION LocalPointDataObject::GetHeadPosition()
{
   return m_pt_list.GetHeadPosition();
}

C_localpnt_point *LocalPointDataObject::GetNext(POSITION &pos)
{
   return m_pt_list.GetNext(pos);
}

C_localpnt_point *LocalPointDataObject::GetPrev(POSITION &pos)
{
   return m_pt_list.GetPrev(pos);
}

C_localpnt_point *LocalPointDataObject::GetAt(POSITION pos)
{
   return m_pt_list.GetAt(pos);
}

C_localpnt_point *LocalPointDataObject::get_at_index(int index, int current_tab /*=-1*/)
{
   POSITION position;
   if (current_tab == -1)
      current_tab = m_ctrl->GetCurrentTab();
   CString current_group = "All Points";
   if (current_tab != 0)
      current_group = m_ctrl->m_CUGTab->GetTabLabel(current_tab);

   if (current_group == "All Points")
   {
      if (index == m_pt_list.GetCount())
         return m_pre_point;

      position = m_pt_list.FindIndex(index);
      if (position)
         return m_pt_list.GetAt(position);
   }

   int count = 0;
   position = m_pt_list.GetHeadPosition();
   while (position)
   {
      C_localpnt_point *point = m_pt_list.GetNext(position);
      if (point->m_group_name == current_group)
      {
         if (index == count)
            return point;

         count++;
      }
   }
   if (index == count)
      return m_pre_point;

   ASSERT(0);
   return NULL;
}

void LocalPointDataObject::AddTail(C_localpnt_point *point)
{
   m_pt_list.AddTail(point);

   if (m_parent_overlay != nullptr)
   {
      C_localpnt_ovl *overlay =
         static_cast<C_localpnt_ovl *>(m_parent_overlay);
      overlay->Add3DPoint(point);
   }
}

// a row was selected in the tabular editor - update
// highlight / selected state
void LocalPointDataObject::on_selected(int irow)
{
   long row = irow;
   translate_row(row);

   C_localpnt_point *point = get_at_index(row);
   if (point)
   {
      C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
      overlay->set_current_selection(point);
   }
}

// delete a particular row

void LocalPointDataObject::delete_selected_row()
{
   // Because delete_local_point can put up a dialog box asking if the user
   // wants to delete the point when drag-lock is enabled, this method can get
   // called again in that time.  If this same point is deleted twice, then FV
   // will crash
   static bool bDeleteInProgress = false;

   if (bDeleteInProgress)
      return;

   TRACE("Deleting selected row\n");

   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = overlay->get_current_selection();
   if (point)
   {
      bDeleteInProgress = true;
      overlay->delete_local_point((MapProj *)UTL_get_current_view_map(), point);
      bDeleteInProgress = false;
   }
}

void LocalPointDataObject::delete_row(int row)
{
   CList<int, int> row_list;
   row_list.AddTail(row);
   delete_row(row_list);
}

void LocalPointDataObject::delete_row(CList<int, int> &row_list)
{
   CList<C_localpnt_point *, C_localpnt_point *> point_list;

   POSITION position = row_list.GetHeadPosition();
   while (position)
      point_list.AddTail(get_at_index(row_list.GetNext(position)));

   position = point_list.GetHeadPosition();
   while (position)
   {
      C_localpnt_point *point = point_list.GetNext(position);
      if (point && (point != m_pre_point))
      {
         C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
         ViewMapProj *map = NULL;
         CView *view = NULL;
         view = UTL_get_active_non_printing_view();
         if (view)
         {
            map = UTL_get_current_view_map(view);
            if (map)
               overlay->delete_local_point((MapProj *)map, point);
            else
               ERR_report("Could not get map");
         }
         else
            ERR_report("Could not get view");
      }
   }
}

// center the map at the currently selected point
void LocalPointDataObject::center_map()
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   C_localpnt_point *point = overlay->get_current_selection();

   if (point != m_pre_point)
      center_map(point);
}

void LocalPointDataObject::center_map(C_localpnt_point *point)
{
   if (point && point != m_pre_point)
   {
      CView* view = UTL_get_active_non_printing_view();
      if (UTL_change_view_map_to_best(view, point->m_latitude, point->m_longitude) == FAILURE)
      {
         ERR_report("Failed to set current center");
      }
      OVL_get_overlay_manager()->invalidate_all();
   }
}

// return the geo location associated with the given row
BOOL LocalPointDataObject::get_location_at(long row, int tab, d_geo_t &location, CString &string)
{
   C_localpnt_point *point = get_at_index(row, tab);
   if (point == NULL)
      return FALSE;

   location.lat = point->m_latitude;
   location.lon = point->m_longitude;

   string = point->m_id;

   return TRUE;
}

BOOL LocalPointDataObject::update_elevation(C_localpnt_point *point)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   CString id, group_name, description, dtd_id;
   float horz_accuracy, vert_accuracy;
   int elevation;
   CString elevation_source;
   CString elevation_source_map_handler_name;
   short elevation_source_map_series_id;
   double lat, lon, alt;
   CString comment, icon;
   CString country_code;
   char area_code, qlt;

   overlay->lookup_info(point, id, group_name, description, dtd_id,
                        horz_accuracy, vert_accuracy, elevation, elevation_source,
                        elevation_source_map_handler_name, elevation_source_map_series_id,
                        lat, lon, alt, comment, icon, country_code, area_code, qlt);


   int     new_elevation;
   CString new_elevation_src;
   CString new_elev_src_map_handler_name;
   short new_elev_src_map_series_id;

   overlay->get_best_point_elevation(point->m_latitude, point->m_longitude,
                                     new_elevation, new_elevation_src,
                                     new_elev_src_map_handler_name, new_elev_src_map_series_id);


   BOOL bMapElevationDataAvailable = new_elevation_src == "UNK" ? FALSE : TRUE;

   overlay->apply_changes(point, point->GetIconName(), comment, dtd_id,
      description, id, group_name, lat, lon, alt, new_elevation_src,
      new_elev_src_map_handler_name, new_elev_src_map_series_id, new_elevation,
      vert_accuracy, horz_accuracy, TRUE);

   return bMapElevationDataAvailable;
}

// import the data from the given parser object
int LocalPointDataObject::import_data(CTabularEditorDlg *tabular_editor, Parser *parser)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;
   overlay->set_importing(true);

   CString current_line;
   const int MAX_DESCRIPTION_LENGTH = 40;  // CAB I don't know if there is a way to read this from the DB instead of hard coding it
   const int MAX_ID_LENGTH = 12;
   const int MAX_GROUP_NAME_LENGTH = 64;
   const int MAX_COMMENT_LENGTH = 255;
   const int MAX_ICON_NAME_LENGTH = 255;
   const int MAX_LINK_NAME_LENGTH = 255;


   int imported_point_count = 0;
   int total_point_count = 0;
   int lat_lon_status;

   parser->open_file(TRUE);

   IDtedPtr smpDted;
   smpDted.CreateInstance(__uuidof(Dted));

   int lat_lon_parts;
   CString complete_lat_lon_str;

   while (1)  // break when token count is zero
   {
      C_localpnt_point* point = new C_localpnt_point(overlay);
      lat_lon_parts = 0;
      complete_lat_lon_str = "";

      BOOL add_link = FALSE;
      CString link_str;

      CList<CString, CString> token_list;
      parser->get_tokens(token_list);
      if (token_list.GetCount() == 0)
      {
         delete point;
         break;
      }

      CList<CString, CString>& column_header_list = parser->get_properties()->get_column_header_list();

      // fill in defaults
      CNewLocalPointDefaults new_point_defaults;
      CString group_name = new_point_defaults.GetGroupName();

      CString id = new_point_defaults.get_and_increment_id();
      while (overlay->does_key_exist(id, group_name) == TRUE)
      {
         id = new_point_defaults.get_and_increment_id();
      }

      point->m_id = id;
      point->m_group_name = group_name;
      point->m_latitude = 0;
      point->m_longitude = 0;
      point->m_vert_accuracy = -1;
      point->m_horz_accuracy = -1;
      point->m_elv = -1;
      point->m_elv_src = "UNK";

      point->SetIconName("localpnt\\red_dot.ico");
      point->m_icon_image = CIconImage::load_images(point->GetIconName());

      // fill in values from import
      POSITION token_position = token_list.GetHeadPosition();
      POSITION header_position = column_header_list.GetHeadPosition();
      while(token_position && header_position)
      {
         CString token = token_list.GetNext(token_position);
         token.TrimLeft().TrimRight();
         char *header_both = _strdup(column_header_list.GetNext(header_position));

         CString header(strtok((char *)(LPCTSTR)header_both, "[]"));
         header.TrimRight();

         CString header_ext(strtok(NULL, "[]"));
         header_ext.TrimRight();
         header_ext.TrimLeft();

         if (header == "Do not import")
         {
            free(header_both);
            continue;
         }

         if (header == "ID")
            point->m_id = token.Left(MAX_ID_LENGTH);
         else if (header == "Group")
            point->m_group_name = token.Left(MAX_GROUP_NAME_LENGTH);
         else if (header == "DTD ID")
            strcpy(point->m_dtd_id, token.Left(MAX_DTD_ID_LENGTH));
         else if (header == "Elevation (in feet)")
            point->m_elv = atoi(token);
         else if (header == "Elevation Source")
         {
            if(token == "UNK" || token == "DTED" || token == "USER" || token == "DAFIF")
               point->m_elv_src = token;
         }
         else if (header == "Elevation Source Map Handler Name")
         {
            if(token == "UNK" || token == "DTED" || token == "GeoTIFFDEM")
               point->m_elev_src_map_handler_name = token;
         }
         else if (header == "Elevation Source Map Series ID")
            point->m_elev_src_map_series_id = atoi(token);
         else if (header == "Description")
            point->m_description = token.Left(MAX_DESCRIPTION_LENGTH);  // description is limited to 40 Char
         else if (header == "Location")
         {
            if (header_ext == "Latitude and Longitude" || header_ext == "MGRS")
            {
               lat_lon_parts = 2;
               complete_lat_lon_str = token;
            }
            else if (header_ext == "Latitude Only")
            {
               lat_lon_parts++;
               complete_lat_lon_str = token + " " + complete_lat_lon_str;
            }
            else if (header_ext == "Longitude Only")
            {
               lat_lon_parts++;
               complete_lat_lon_str = complete_lat_lon_str + " " + token;
            }

            if (lat_lon_parts == 2)
            {
               CGeoTrans m_geotrans;
               char geo_string[GEO_MAX_LAT_LON_STRING+1];

               // replace O's in the string with zeros
               complete_lat_lon_str.Replace('o', '0');
               complete_lat_lon_str.Replace('O', '0');

               // this can crash if passed in a weird string
               double lat, lon;
               try
               {
                  lat_lon_status = m_geotrans.DLL_location_to_geo(complete_lat_lon_str, "WGS84",
                     lat, lon, geo_string);

                  point->m_latitude = static_cast<float>(lat);
                  point->m_longitude = static_cast<float>(lon);
               }
               catch(...)
               {
                  ERR_report("Bad lat/lon passed to DLL_location_to_geo");
               };
            }
         }
         else if (header == "Vertical Accuracy (in feet)")
            point->m_vert_accuracy = static_cast<float>(atof(token));
         else if (header == "Horizontal Accuracy (in feet)")
            point->m_horz_accuracy = static_cast<float>(atof(token));
         else if (header == "Comments")
         {
            if(point->m_comment != "") // CAB seperate multiple comments by commas
            {
               point->m_comment += ", ";
            }
            point->m_comment += token;
            point->m_comment = point->m_comment.Left(MAX_COMMENT_LENGTH);
         }

         else if (header == "Icon")
         {
            point->m_icon_image = CIconImage::load_images(token,"",0);

            CString icon_name = token.Mid(token.ReverseFind('\\') + 1);
            point->SetIconName(icon_name.Left(icon_name.GetLength() - 4));
         }

         else if (header == "Links")
         {
            add_link = TRUE;
            link_str = CString(token);
         }
         else if (header == "Country")
         {
            CFvwUtil *futil = CFvwUtil::get_instance();
            char name[40], code[3];
            futil->get_first_country(name, 40, code, 3);
            if (CString(name).CompareNoCase(token) == 0)
               strcpy(point->m_country_code, code);
            else
            {
               while(futil->get_next_country(name, 40, code, 3))
               {
                  if (CString(name).CompareNoCase(token) == 0)
                     strcpy(point->m_country_code, code);
               }
            }
            code[2] = '\0';
         }
         else if (header == "ICAO Area")
         {
            if (strlen(token) > 0)
               point->m_area_code = token[0];
         }
         else if (header == "Point Quality")
         {
            if (strlen(token) > 0)
               point->m_qlt = token[0];
         }

         free(header_both);
      }

      // apply filters - loop backwards so that the first filter will overwrite
      // any prior filters
      ParserProperties *prop = parser->get_properties();
      const int filter_count = prop->get_num_filters();
      for(int i=filter_count-1;i>=0 && filter_count;--i)
      {
         filter_t filter = prop->get_filter(i);

         POSITION token_position = token_list.GetHeadPosition();
         POSITION header_position = column_header_list.GetHeadPosition();
         while (header_position && token_position)
         {
            CString token = token_list.GetNext(token_position);
            token.MakeLower();  // CAB do a case insensitive filter.
            CString header = column_header_list.GetNext(header_position);
            bool matched = false;
            if (filter.column_header == header)
            {
               char *fstr = _strdup(filter.filter_string);
               char *f = strtok(fstr, ",");
               while(f)
               {
                  CString match_against(f);
                  match_against.TrimLeft();
                  match_against.TrimRight();
                  match_against.MakeLower();  // CAB do a case insensitive filter.
                  if (match_against.GetLength())
                  {
                     if (match_against == "*")
                        matched = true;
                     else if (match_against[0] == '~')
                     {
                        CString m = match_against.Right(match_against.GetLength() - 1);
                        if (token.Left(m.GetLength()) == m)
                           matched = true;
                     }
                     else if (match_against[match_against.GetLength()-1] == '~')
                     {
                        CString m = match_against.Left(match_against.GetLength() - 1);
                        if (token.Right(m.GetLength()) == m)
                           matched = true;
                     }
                     else if (token.Find(match_against, 0) != -1)
                        matched = true;
                  }

                  f = strtok(NULL, ",");
               }

               free(fstr);

               if (matched)
               {
                  point->m_icon_image = CIconImage::load_images(filter.icon_assigned, "",0);
                  point->SetIconName(point->m_icon_image->
                     get_item_title().Left(MAX_ICON_NAME_LENGTH));

                  if (filter.group_name != "Do not overwrite")
                     point->m_group_name = filter.group_name;
               }
            }
         }
      }

      // if the lat,lon couldn't be parsed, then don't add the point
      if (lat_lon_status == SUCCESS)
      {
         if (smpDted != NULL && point->m_elv_src == "UNK")
         {
            try
            {
               short DTEDLevelUsed = 0;
               long lElevation = smpDted->GetElevation(point->m_latitude, point->m_longitude, 0,
                  DTED_ELEVATION_FEET, &DTEDLevelUsed);
               if (DTEDLevelUsed != 0 && lElevation != PARTIAL_DTED_ELEVATION)
               {
                  point->m_elv = static_cast<short>(lElevation);
                  point->m_elv_src ="DTED";
               }
            }
            catch(_com_error &e)
            {
               CString msg;
               msg.Format("Unable to get DTED elevation: %s", (char *)e.Description());
               ERR_report(msg);
            }
         }

         // do not set the overlay to modified until after all the points have
         // been added
         const BOOL bSetModified = FALSE;
         const BOOL bMapElevationLookup = FALSE;
         if (overlay->insert_point(NULL, point, NULL, bSetModified, bMapElevationLookup, true, false) == SUCCESS)
         {
            imported_point_count++;
            if (add_link)
            {
               CString linkName = link_str.Left(MAX_LINK_NAME_LENGTH);
               if (linkName.GetLength() > 0)
               {
                  CStringArray links;
                  overlay->get_links_for_point(point->m_id, point->m_group_name, links);

                  // multiple links are '|' separated
                  linkName += "|";  // eliminates special case in following loop
                  int current_pos = 0;
                  int next_pos;
                  while ( (next_pos = linkName.Find('|', current_pos)) != -1)
                  {
                     links.Add(linkName.Mid(current_pos, next_pos - current_pos));
                     current_pos = next_pos + 1;
                  }

                  CString handle;
                  handle.Format("%s,%s", point->m_id, point->m_group_name);
                  overlay->save_links(handle, links);
               }
            }
         }
      }
      else
         delete point;

      total_point_count++;
   }

   overlay->set_importing(false);


   if (smpDted != NULL)
      smpDted->Terminate();

   tabular_editor->UpdateData(FALSE);

   CString msg;
   if (total_point_count == 0)
      msg.Format("No points imported");
   else
   {
      msg.Format("Successfully imported %d / %d points\n\nReview your output file for accuracy!!!",
      imported_point_count, total_point_count);

      overlay->set_modified(TRUE);
   }

   AfxMessageBox(msg);

   return SUCCESS;
}

// get a list of tabs for this data object
void LocalPointDataObject::get_tab_list(CList<CString, CString> &list)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   list.AddTail("All Points");

   // add a tab for each of the groups
   CString group_lst = overlay->get_group_list();
   char *token = strtok((char *)(LPCTSTR)group_lst, "\n");
   while (token)
   {
      list.AddTail(CString(token));
      token = strtok(NULL, "\n");
   }
}

void LocalPointDataObject::update_edit_dialog(C_localpnt_point *point)
{
   C_localpnt_ovl *overlay = (C_localpnt_ovl *)m_parent_overlay;

   if (overlay->get_current_selection() == point)
   {
      if (!C_localpnt_ovl::m_EditDialogControl.DialogExists())
         return;

      CString strFileSpecification;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         strFileSpecification = (char *)fileSpecification;
      }

      C_localpnt_ovl::m_EditDialogControl.SetFocus(point->m_id,
         point->m_group_name, strFileSpecification, FALSE, FALSE);
      C_localpnt_ovl::m_EditDialogControl.ShowDialog();
   }
}
