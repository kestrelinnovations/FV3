// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// localpnt.cpp : implementation file
//

#include "stdafx.h"
#include "FalconView/include/wm_user.h"

#include "err.h"
#include "FalconView/include/showrmk.h"
#include "FalconView/include/icondlg.h"
#include "FalconView/include/LinksDlg.h"
#include "FalconView/include/param.h"
#include "FalconView/localpnt/lpdao.h"
#include "FalconView/localpnt/lpicon.h"
#include "lpgrpnam.h"
#include "minlpset.h"
#include "lplookup.h"
#include "newptdef.h"
#include "FalconView/include/file.h"
#include "CommaDelimitedString.h"
#include "FalconView/include/map.h"
#include "..\mapview.h"
#include "FalconView/include/OvlFctry.h"
#include "..\ExifImage.h"

#include "LocalPointFilter.h"
#include "ElevationDlg.h"

//#ifdef GOV_RELEASE
//#include "records.h"
//#endif

#include "DataObject.h"
#include "factory.h"
#include "..\tabulareditor\TargetGraphicsDlg.h"
#include "CoT\LPointsMessage.h"  // codec for CoT messages
#include "PointOverlayFonts.h"
#include "Common/ComErrorObject.h"
#include "Common/GenericWorkItem.h" 
#include "FalconView/include/drag_utils.h"
#include "FalconView/include/ovl_mgr.h"
#include "FalconView/PlaybackDialog/viewtime.h"
#include "FalconView/mainfrm.h"
#include "FalconView/StatusBarManager.h"
#include "FalconView/include/SnapTo.h"
#include "FalconView/localpnt/lpeddlgc.h"



// static variable declarations
boolean_t C_localpnt_ovl::m_bEdit = FALSE;
boolean_t C_localpnt_ovl::m_above_threshold = FALSE;

static boolean_t g_drag_lock_state;

const long POINT_EXPORT_SRC_LOCAL = 5;

//TO DO: add drag offset (offset from cursor to center of icon)

C_localpnt_ovl::EditorMode C_localpnt_ovl::m_editor_mode=SELECT;
CLocalpntEditDlgControl C_localpnt_ovl::m_EditDialogControl;  //controls for modeless dialog chum editor

// ----------------------------------------------------------------------------
////////////////////////////////////////////////
// C_localpnt_ovl functions
////////////////////////////////////////////////
// ----------------------------------------------------------------------------

// constructor
C_localpnt_ovl::C_localpnt_ovl(CString name) :
CFvOverlayPersistenceImpl(name), m_bDragging(FALSE),
m_work_items_stream(nullptr)
{
   m_in_drag = FALSE;
   m_exceeded_drag_threshold=FALSE;
   m_image_in_drag=NULL;
   m_ID_in_drag.Empty();
   m_group_name_in_drag.Empty();
   m_back_image=NULL;
   m_current_hilight = NULL;

   // DAO stuff
   m_database=NULL;

   set_modified(FALSE);

   m_LinkEditDialog = NULL;

   m_reload_on_next_draw = TRUE;

   // set display threshold from value stored in the registry
   CString threshold = PRM_get_registry_string("Local Points", 
      "Display Threshold", "1:2 M");
   m_display_threshold = MAP_get_scale_from_string(threshold);

   // set the label threshold from registry
   threshold = PRM_get_registry_string("Local Points", 
      "Label Threshold", "1:250 K");
   m_label_threshold = MAP_get_scale_from_string(threshold);

   g_drag_lock_state = (boolean_t) PRM_get_registry_int("C_localpnt_ovl", "drag lock state", TRUE);

   m_show_overlay = TRUE;

   m_data_object = new LocalPointDataObject(this);
   m_target_graphic_printing_on = FALSE;

   // add default group
   {
      CNewLocalPointDefaults new_point_defaults;

      POINT_GROUP* pGroup = new POINT_GROUP;
      pGroup->name = new_point_defaults.GetGroupName();
      pGroup->search = TRUE;

      m_group_lst.AddTail(pGroup);
   }

   m_pPointOverlayFonts = new CPointOverlayFonts();

   // Generate a GUID for this overlay
   //
   GUID guid;
   ::CoCreateGuid(&guid);

   wchar_t buf[45];
   StringFromGUID2(guid, buf, 45);

   m_strOverlayGuid = (char *)_bstr_t(buf);
   m_posEnumerator = NULL;

   m_isImporting = false;
}

HRESULT C_localpnt_ovl::Initialize(GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;
   return S_OK;
}

// ----------------------------------------------------------------------------

C_localpnt_ovl::~C_localpnt_ovl()
{
   clear_ovl();

   while (!m_exportedPoints.IsEmpty())
      delete m_exportedPoints.RemoveHead();

   if (CIconDragDlg::get_dlg())
      close_icon_dlg();

   //close and delete the editor dialog
   if (m_EditDialogControl.DialogExists()) 
      m_EditDialogControl.DestroyDialog(); 

   close_and_delete_dao_objects();

   delete m_LinkEditDialog;

   ClearGroups();
   while (!m_links_lst.IsEmpty())
      delete m_links_lst.RemoveTail();

   delete m_data_object;
   delete m_pPointOverlayFonts;
}

void C_localpnt_ovl::set_modified(boolean_t mod_flag)
{
   C_overlay::set_modified(mod_flag);

   if (mod_flag)
      m_data_object->on_modified();

   if (mod_flag && g_tabular_editor_dlg)
      g_tabular_editor_dlg->UpdateData(FALSE);
}

CString C_localpnt_ovl::id_to_series_name(CString handler_name, long series_id)
{
   if ((handler_name == "UNK") || (handler_name == "USER") || (handler_name == ""))
   {
      return "";
   }

   // to go from handler name / identity to user interface string
   IMapSeriesRowsetPtr spMapSeriesRowset;
   CO_CREATE(spMapSeriesRowset, CLSID_MapSeriesRowset);

   spMapSeriesRowset->Initialize(_bstr_t(handler_name));
   spMapSeriesRowset->SelectByIdentity(series_id);

   CString product_name = ((LPCWSTR)spMapSeriesRowset->m_ProductName);
   double scale = spMapSeriesRowset->m_Scale;
   MapScaleUnitsEnum units = spMapSeriesRowset->m_ScaleUnits;
   CString series_name = ((LPCWSTR)spMapSeriesRowset->m_SeriesName);

   //Now get the friendly name
   IMapSeriesStringConverterPtr m_smpMapSeriesStringConverter;
   CO_CREATE(m_smpMapSeriesStringConverter, CLSID_MapSeriesStringConverter);

   CString user_friendly_name = (char *)m_smpMapSeriesStringConverter->ToString(
      _bstr_t(product_name), scale, units, _bstr_t(series_name),
      spMapSeriesRowset->m_IsSoftScale ? VARIANT_TRUE : VARIANT_FALSE,
      FORMAT_PRODUCT_NAME_SCALE_SERIES);

   return user_friendly_name;
}

void C_localpnt_ovl::SetOverlayFont(OvlFont& font, bool bClearGroupAndPointFonts /*= false*/)
{
   if (m_pPointOverlayFonts != NULL)
      m_pPointOverlayFonts->SetOverlayFont(font, bClearGroupAndPointFonts);
}

void C_localpnt_ovl::SetGroupFont(OvlFont& font, std::string strGroupName, bool bClearPointFonts /*= false*/)
{
   if (m_pPointOverlayFonts != NULL)
   {
      m_pPointOverlayFonts->SetGroupFont(font, strGroupName);

      // clear point fonts for any points in the group
      if (bClearPointFonts && m_data_object != NULL)
      {
         POSITION position = m_data_object->GetHeadPosition();
         while (position)
         {
            C_localpnt_point *pPoint = m_data_object->GetNext(position);
            if (pPoint->m_group_name == strGroupName.c_str())
            {
               FVPOINT p(std::string(pPoint->m_id), strGroupName);
               m_pPointOverlayFonts->RemovePointFont(p);
            }
         }
      }
   }
}

OvlFont& C_localpnt_ovl::GetFont(std::string strPointId, std::string strGroupName)
{
   ASSERT(m_pPointOverlayFonts != NULL);
   return m_pPointOverlayFonts->GetFont(strPointId, strGroupName);
}

void C_localpnt_ovl::SetPointFont(OvlFont& font, FVPOINT p)
{
   if (m_pPointOverlayFonts != NULL)
      m_pPointOverlayFonts->SetPointFont(font, p);
}

// ----------------------------------------------------------------------------

//removes all items from the overlay list, and set the current selection
// and hilight to NULL
int C_localpnt_ovl::clear_ovl()
{
   unselect_current_selections();
   m_current_hilight=NULL;

   m_data_object->clear();

   return SUCCESS;
}

// ----------------------------------------------------------------------------

//delete the point specified and remove it from the list

//NOTE: this function does not redraw the screen
//WARNING: need to clear highlight and select (if point is higlighted or selected)
int C_localpnt_ovl::delete_point_from_overlay_and_database(C_localpnt_point* point, bool bNotifyClients /* = true */)
{
   ASSERT(point);

   if (bNotifyClients)
   {
      const bool bDeleteElement = true;
      OnElementModified(point, bDeleteElement);
   }

   delete_links_for_point(point->m_id, point->m_group_name);

   m_data_object->remove(point);

   set_modified(TRUE);

   return SUCCESS;
}

// ----------------------------------------------------------------------------

//does the following:
//0) prompts the user whether or not to override drag lock
//1) if hilighed, unhilight
//2) if selected, unselect, and cycle to next point
//3) delete point from database and memory 
//4) invalidate point
int C_localpnt_ovl::delete_local_point(MapProj* map, C_localpnt_point* point, bool bNotifyClients /* = true */)
{
   ASSERT(point);

   int result = FAILURE;

   // If the current Local Point overlay is read-only, display a message box
   // and return SUCCESS to cancel the point delete...
   if (m_bIsReadOnly)
   {
      // Get the current Local Point overlay...
      C_overlay* m_current_overlay = OVL_get_overlay_manager()->get_current_overlay();

      // Get the current file name...
      CString fileName = OVL_get_overlay_manager()->GetOverlayDisplayName(m_current_overlay);

      AfxMessageBox("The Local Points overlay file " + fileName + " cannot be edited because it is Read-Only.  If you wish to edit this file, first save it under a different name.");
      return SUCCESS;
   }

   if (point == NULL)
      return FAILURE;

   CString strOldId = point->m_id;
   CString strOldGroupName = point->m_group_name;

   if (!get_drag_lock_state() || override_drag_lock_delete()) //if drag lock is not on or user chooses to ignore it...)
   {
      // the point might have been wiped out if drag-lock was on.  This is because the dialog
      // pumps messages and the local point draw always creates all points from scratch on
      // every draw.
      point = find_local_point_in_list(strOldId, strOldGroupName);
      if (point == NULL)
         return FAILURE;

      //remove hilight from point if it is hilighted  (1)
      if (point == m_current_hilight)
      {
         if (CRemarkDisplay::test_active())
            CRemarkDisplay::close_dlg();
         unhilight_current_hilight();
      }

      if (m_selected_points.GetCount() == 1 && m_selected_points.Find(point))
      {
         //get position of current selection in list so we can cycle to the next point at the
         //end of this function
         POSITION pos = NULL;
         pos = m_data_object->Find(point);

         cycle(map, pos);

         //we just cycled to the next point on the screen.  If we cycled back to the
         //point we are deleting, we need to set the current_selection to NULL, and 
         // set the selection in the edit dialog box to "" (blank).

         if (m_selected_points.Find(point))
         {
            /*
            // if the edit dialog is up and contains this point, set editor dialog focus to
            // "" (blank) before deleting the point in focus, do NOT update point before deleting
            */

            if (m_EditDialogControl.DialogExists())
               m_EditDialogControl.SetFocus("","","", FALSE);

            set_link_edit_dlg_focus(NULL);

            unselect_current_selections();                        
         }

         //since we haven't delete the point yet, we may have 
      }
      else if (m_selected_points.GetCount() > 1 && m_selected_points.Find(point))
      {
         point->invalidate();
         POSITION pos = m_selected_points.Find(point);
         m_selected_points.RemoveAt(pos);
      }
      else
         //tO DO: invalidate label also
         point->invalidate();        //invalidate the point we're deleting

      result = delete_point_from_overlay_and_database(point, bNotifyClients);  // (3)
   }

   return result;
}

// ----------------------------------------------------------------------------
BOOL C_localpnt_ovl::we_are_below_the_label_threshold(const MapProj* map)
{
   BOOL result;

   result = (m_label_threshold <= map->scale())  &&  
      (m_label_threshold != NULL_SCALE);

   return result;
}
// ----------------------------------------------------------------------------

BOOL C_localpnt_ovl::we_are_below_the_display_threshold(const MapProj* map)
{
   BOOL result;

   result = (m_display_threshold <= map->scale())  &&  
      (m_display_threshold != NULL_SCALE);

   return result;
}
// ----------------------------------------------------------------------------


int C_localpnt_ovl::draw(ActiveMap* map)
{
   if (m_reload_on_next_draw || !get_valid())
   {
      fill_local_point_list_from_database(map);

      if (m_reload_on_next_draw && g_tabular_editor_dlg)
         g_tabular_editor_dlg->UpdateData(FALSE);

      m_reload_on_next_draw = FALSE;
   }

   // While dragging, it is possible to receive a call to draw.  Since
   // the points will be reloaded, the pointer stored in the CIconImage
   // class will be invalid.  If we were dragging and a current selection
   // exists, then we will need to reset the value in CIconImage
   if (m_in_drag && m_selected_points.GetCount() > 0)
      CIconImage::m_lParam_holder = (LPARAM)get_current_selection();

   //CFvwUtil util;
   //CSize textsize;
   C_localpnt_point* point;
   //int x, y;
   POSITION pos;

   // variables used when we leave our display threshold for restoring our
   // edit state when we can dispaly again
   static EditorMode current_tool = SELECT;
   static boolean_t dialog_was_up = FALSE;

   // the following code was basically copied from Chum -- we probably need
   // something a little bit more generic in a function some time
   if (we_are_below_the_display_threshold(map) == FALSE)
   {
      // if we are already above the display threshold, then just return
      // we don't have anything else that needs to be done because it
      // was done on the first "draw" after the threshold was passed.
      if (!m_above_threshold  &&  m_bEdit  &&  get_current())  // only currently edited overlay
      {
         // remember the current tool setting for restoring later
         current_tool = m_editor_mode;
         m_above_threshold = TRUE;

         // we need to return without doing any drawing... also want
         // to turn off the current tool
         set_editor_mode(SELECT);

         // we need to take the edit dialog down if it is current up
         if (m_EditDialogControl.DialogExists())  //if is the editor is up...
         {
            //close the edit dialog
            //TO DO: Kevin: prompt user whether or not to save changes
            m_EditDialogControl.SetFocus(NULL);  //save changes
            m_EditDialogControl.DestroyDialog();
            dialog_was_up = TRUE;
         }
         else
            dialog_was_up = FALSE;
      }

      // if we are in edit mode, don't set this until we get to the top most
      // overlay (the current one)
      if (!m_bEdit)
         m_above_threshold = TRUE;
      return SUCCESS;
   }
   else
   {
      // last time we drew, we were above threshold, we need to restore
      // our editor tool now
      if (m_above_threshold  &&  m_bEdit  && get_current())
      {
         set_editor_mode(current_tool);
         m_above_threshold = FALSE;

         // turn the edit dialog box back on if we need to
         if (dialog_was_up)
         {
            // turn the editor dialog "on" and set the edit focus to the current
            // selection the topmost overlay (topmost overlay was found above)
            m_EditDialogControl.Create();

            //Note: it's OK if current_selection is NULL
            if (m_selected_points.GetCount() != 1)
               m_EditDialogControl.SetFocus(NULL);
            else
               m_EditDialogControl.SetFocus(m_selected_points.GetHead());
            m_EditDialogControl.ShowDialog();

            // we want the main falconview window to be active, not the edit dialog
            AfxGetMainWnd()->SetActiveWindow();
         }
      }

      // if we are in edit mode, don't set this until we get to the top most
      // overlay (the current one)
      if (!m_bEdit)
         m_above_threshold = FALSE;
   }

   const bool below_threshold = (we_are_below_the_display_threshold(map) && !m_data_object->IsEmpty() &&
      m_show_overlay == TRUE);


   //if this overlay is set to draw at this scale, and the list is not empty
   if (m_target_graphic_printing_on || below_threshold)
   {
      // set flag to draw/not-draw labels based up current map scale
      BOOL draw_labels = we_are_below_the_label_threshold(map);

      CDC* dc = map->get_CDC();

      pos = m_data_object->GetTailPosition();
      while (pos)
      {
         point = m_data_object->GetPrev(pos);

         ASSERT(point);
         if (point)
         {
            BOOL draw = true;

            // if the overlay is unnamed then the points have not been
            // filtered on the group name - we will do that here
            POSITION position = m_group_lst.GetHeadPosition();
            while (position)
            {
               POINT_GROUP *group = m_group_lst.GetNext(position);
               if (group->name == point->m_group_name)
               {
                  draw = group->search;
                  break;
               }
            }

            const bool is_target_graphic = (m_target_graphic_printing_on && 
               point->m_id == m_target_point_id && point->m_group_name == m_target_point_group);

            // draw target.ico
            if (is_target_graphic)
            {
               CIconImage *image = CIconImage::load_images("\\localpnt\\target.ico");
               image->draw_icon(dc, point->m_x, point->m_y, 32);
            }
            else if (draw && below_threshold)
            {
               const boolean_t selected =
                  (m_selected_points.Find(point) && m_bEdit && get_current());

               const boolean_t hilighted = point == m_current_hilight;

               // we will draw halftone icons when in edit mode to show
               // non current points overlays
               const boolean_t halftone = m_bEdit && !get_current();

               point->draw(map, dc, selected, hilighted, draw_labels, m_pPointOverlayFonts, halftone);
            }
         }
      }
   }

   // TO DO: use this value in draw?
   // from overlay.h:
   // When m_valid == TRUE do a full window redraw, otherwise draw
   // from scratch. draw() should set m_valid to TRUE when it is done.
   set_valid(TRUE);

   return SUCCESS;
}
// end of draw

// ----------------------------------------------------------------------------

// determine if the point hits the object's selection region(s)

C_icon *C_localpnt_ovl::icon_hit_test(const MapProj* map, CPoint point)
{
   POSITION          next;
   C_localpnt_point* pt;

   // if we are above our display threshold, then we do nothing here
   if (m_above_threshold)
      return NULL;

   // search the overlay list first
   if (we_are_below_the_display_threshold(map) && !m_data_object->IsEmpty())
   {
      next = m_data_object->GetHeadPosition();
      do
      {
         // get this local point (and move position of the next one)
         pt = m_data_object->GetNext(next);

         ASSERT(pt->m_icon_image);

         BOOL visible = true;

         // if the overlay is unnamed then the points have not been
         // filtered on the group name - we will do that here
         POSITION position = m_group_lst.GetHeadPosition();
         while (position)
         {
            POINT_GROUP *group = m_group_lst.GetNext(position);
            if (group->name == pt->m_group_name)
            {
               visible = group->search;
               break;
            }
         }

         if (!is_point_in_drag(pt) && pt->m_rect.PtInRect(point) && visible)
            return (C_icon *)pt;

      } while (next != NULL);
   }

   return NULL;
}
// end of icon_hit_test

// ----------------------------------------------------------------------------

C_icon *C_localpnt_ovl::hit_test(MapProj* map, CPoint point)
{
   return icon_hit_test(map, point);
}
// end of hit_test


// ----------------------------------------------------------------------------
//1) if query_name is not empty, execute a stored query
//   if stored query does not exist, store it
//2) if query name is null, execute SQL and do not store query
int C_localpnt_ovl::execute_query(CString &SQL, CString query_name/*=""*/, 
                                  COleVariant* param0 /* = NULL */, COleVariant* param1 /* = NULL */)
{
   CDBOpenerCloser dbOpenerCloser(m_database, get_specification()); //OK for m_database to be null
   CDaoDatabase* database= dbOpenerCloser.GetDatabase();

   CDaoQueryDef qd(database);

   int result = FAILURE;
   try
   {
      if (!query_name.IsEmpty())
      {
         //try to open stored query, if it doesn't exist, create it
         try
         {
            qd.Open(query_name);
         }
         catch (CDaoException* e)
         {
            // If we can't find the stored query, create it
            if (e->m_scode == E_DAO_ObjectNotFound)
            {
               e->Delete();

               //stored query doesn't exist, so we need to create one
               qd.Create(query_name, SQL);
               qd.Append();
            }
            else
            {
               CString msg;
               msg = "Failed to open LocalPoint query " + query_name + ":" + e->m_pErrorInfo->m_strDescription;
               ERR_report(msg);
               e->Delete();
            }
         }
      }
      else
      {
         ASSERT(query_name.IsEmpty());
         qd.Create(NULL, SQL);  
      }

      CDaoParameterInfo info;
      COleVariant *value;
      CString name;
      //set paramters 
      //NOTE: the maximum number of parameters this execute_query() currently supports is 2
      for (int i=0; i <= qd.GetParameterCount()-1 && i<=2-1 /* max parameters */; i++)
      {
         qd.GetParameterInfo(i, info);

         switch(i)
         {
         case 0: value = param0;
            break;
         case 1: value = param1;
            break;
         }

         ASSERT(value);  

         //TO DO: verify we always need to add []'s
         //name = "["+ info.m_strName + "]";  
         //qd.SetParamValue(name, *value);
         qd.SetParamValue(i, *value);
      }

      qd.Execute();

      qd.Close();

      result = SUCCESS;
   }
   catch (CDaoException* e)
   {
      CString msg;
      msg = "Execute LocalPoint query " + query_name + ":" + e->m_pErrorInfo->m_strDescription;
      ERR_report(msg);
      e->Delete();
   }
   catch (CMemoryException* e)
   {
      ERR_report("CMemoryException C_localpnt_ovl::execute_query");
      e->Delete();
   }

   return result;
}
// end of execute_query

int C_localpnt_ovl::file_new()
{
   static int next_file_number = 1;

   // Create a new name for the file.
   CString defaultDir = OVL_get_type_descriptor_list()->
      GetOverlayTypeDescriptor(FVWID_Overlay_Points)->
         fileTypeDescriptor.defaultDirectory;

   m_fileSpecification.Format("%s\\points%d.lps", defaultDir,
      next_file_number++);

   return SUCCESS;
}

int C_localpnt_ovl::open(const CString& filespec)
{
   // initialize the saved flag to be TRUE (this means that the file 
   // has been saved with this name before)
   m_fileSpecification = filespec;

   if (m_fileSpecification.Find(".lps") == m_fileSpecification.GetLength() - 4)
   {
      int ret = open_lps(filespec);
      if (ret == SUCCESS)
      {
#ifdef GOV_RELEASE
         SaveToFeaturesOptionsEditor(filespec);
#endif
      }
      return ret;
   }

   if (filespec.IsEmpty() || _access(filespec, 0) !=0)
   { 
      // If the user tries to open the local point database and it doesn't exist,
      // then warn the user and allow them to choose another one from the overlay 
      // options dialog
      if (filespec.CompareNoCase(C_localpnt_ovl::get_local_point_database_filespec()) == 0)
      {
         CString msg;
         msg.Format("The XPlan Local Point Database, %s,\n"
            "does not exist.  Please choose a different database from the Overlay\n"
            "Options dialog or create a new Points file from the File | New menu.", filespec);
         AfxMessageBox(msg, MB_ICONEXCLAMATION);

         OVL_get_overlay_manager()->overlay_options_dialog(get_m_overlayDescGuid());
      }
      else
         ERR_report("File does not exist");

      return FAILURE;
   }

   // create and open dao database if not already done
   int result = SUCCESS;
   if (!m_database)
   {
      create_and_open_member_database();
   }
   else
   {
      //make sure database was not closed by some ill-behaved code 
      //elsewhere in FalconView that may have rudely closed the local point database
      //and reopen if necessary
      check_that_database_is_open_and_reopen_as_needed();
   }

   // BUG: this code assumes that if m_database != NULL that the modified and
   // deleted tables exist.  This assumption is NOT TRUE;

   // If we have not succeeded in the following, DO NOT allow user to edit database:
   //    1) create dao objects (database)
   //    2) open dao objects
   //    3) create modified and deleted tables and links backup table

   if (!(SUCCESS == result && m_database && m_database->IsOpen()))
   {
      AfxMessageBox("Error: Point editor cannot be started");
      return FAILURE;
   }

   // convert database (if necessary) to support multiple links per item
   convert_localpnt_database();

   // read in points from file
   if (ReadPointsFromFile(filespec) != SUCCESS)
   {
      ERR_report("Error reading groups from file");
      return FAILURE;
   }

   // read in groups from file
   if (ReadGroupsFromFile(filespec) != SUCCESS)
   {
      ERR_report("Error reading groups from file");
      return FAILURE;
   }

   if (ReadLinksFromFile(filespec) != SUCCESS)
   {
      ERR_report("Error reading links from file");
      return FAILURE;
   }

   ReadFontsFromFile(filespec);

   // finished reading from file - close file
   close_and_delete_dao_objects();

#ifdef GOV_RELEASE
   SaveToFeaturesOptionsEditor(filespec);
#endif

   return SUCCESS;
}

int C_localpnt_ovl::ReadPointsFromFile(const CString& strFilespec)
{
   short numFields;

   try
   {
      // get the number of fields (columns) in the current file db's "Points" table...
      CDaoTableDef tableDef(m_database);
      try
      {
         tableDef.Open("Points");
         numFields = tableDef.GetFieldCount();
      }
      catch (CDaoException *e)
      {
         CString message = _T("Couldn't get field count -- Exception: ");
         if (e->m_pErrorInfo)                            // use exception data too
            message += e->m_pErrorInfo->m_strDescription;

         AfxMessageBox(message);                         // complain to the user
      }


      CLocalPointSet set(m_database);
      CLocalPointSet::set_filename(strFilespec);

      if (m_bIsReadOnly)
         set.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbForwardOnly | dbReadOnly);
      else
         set.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbForwardOnly);


      while (!set.IsEOF() && !set.IsBOF())
      {
         C_localpnt_point* pNewPoint = new C_localpnt_point(this);

         pNewPoint->m_id = set.m_ID;
         pNewPoint->m_description = set.m_Description;
         pNewPoint->m_latitude = static_cast<float>(set.m_Latitude);
         pNewPoint->m_longitude = static_cast<float>(set.m_Longitude);
         pNewPoint->m_elv = set.m_Elevation;
         pNewPoint->m_area_code = ' ';
         if (set.m_Area.GetLength() > 0)
            pNewPoint->m_area_code = set.m_Area[0];

         strcpy(pNewPoint->m_country_code, "");
         if (set.m_Country_Code.GetLength())
         {
            strncpy(pNewPoint->m_country_code, set.m_Country_Code, 2);
            pNewPoint->m_country_code[set.m_Country_Code.GetLength()] = '\0';
         }

         strcpy(pNewPoint->m_dtd_id, "");
         if (set.m_Dtd_Id.GetLength())
         {
            strncpy(pNewPoint->m_dtd_id, set.m_Dtd_Id, 8);
            pNewPoint->m_dtd_id[set.m_Dtd_Id.GetLength()] = '\0';
         }

         pNewPoint->m_horz_accuracy = set.m_Horz_Accuracy;
         pNewPoint->m_vert_accuracy = set.m_Vert_Accuracy;
         std::string icon_name =  set.m_Icon_Name;
         if (icon_name.find('\\') == std::string::npos)
         {
            icon_name = "localpnt\\" + icon_name;
         }
         if (icon_name.find('.') == std::string::npos)
         {
            icon_name += ".ico";
         }

         pNewPoint->SetIconName(icon_name.c_str());
         pNewPoint->m_comment = set.m_Comment;
         pNewPoint->m_qlt = ' ';
         if (set.m_Pt_Quality.GetLength())
            pNewPoint->m_qlt = set.m_Pt_Quality[0];

         pNewPoint->m_elv_src = set.m_Elevation_Source;

         if (numFields == NUM_FIELDS_IN_POINTS_TABLE) // this file is the current version, so include code
         { // for the additional "Points" table fields.
            pNewPoint->m_elev_src_map_handler_name = set.m_Elevation_Source_Map_Handler_Name;
            pNewPoint->m_elev_src_map_series_id = atoi(set.m_Elevation_Source_Map_Series_Name);
         }
         else  // this file is an older version that lacks the additional fields, so set values to "UNK"
         {
            pNewPoint->m_elev_src_map_handler_name = "UNK";
            pNewPoint->m_elev_src_map_series_id = 0;
         }

         pNewPoint->m_group_name = set.m_Group_Name;
         pNewPoint->m_icon_image =
            CIconImage::load_images(pNewPoint->GetIconName());

         m_data_object->AddTail(pNewPoint);

         set.MoveNext();
      }
   }
   catch (CDaoException *e)
   {
      e->Delete();
      ERR_report("CDaoException reading points");
      return FAILURE;
   }

   return SUCCESS;
}

void C_localpnt_ovl::ClearGroups()
{
   while (!m_group_lst.IsEmpty())
   {
      POINT_GROUP* group = m_group_lst.RemoveTail();
      delete group;
   }
}

int C_localpnt_ovl::ReadGroupsFromFile(const CString& strFilespec)
{
   try
   {
      CLocalPointGroupNameSet set(m_database);
      CLocalPointGroupNameSet::set_filename(strFilespec);

      set.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbForwardOnly);

      ClearGroups();

      while (!set.IsEOF() && !set.IsBOF())
      {
         POINT_GROUP* pGroup = new POINT_GROUP;
         pGroup->name = set.m_Group_Name;
         pGroup->search = set.m_Search;

         m_group_lst.AddTail(pGroup);

         set.MoveNext();
      }
   }
   catch (CDaoException *e)
   {
      e->Delete();
      ERR_report("CDaoException reading groups");
      return FAILURE;
   }

   return SUCCESS;
}

int C_localpnt_ovl::ReadLinksFromFile(const CString& strFilespec)
{
   try
   {
      CDaoRecordset set(m_database);
      set.Open(dbOpenTable, "Links", dbForwardOnly);

      while (!set.IsEOF() && !set.IsBOF())
      {
         POINT_LINK* pLink = new POINT_LINK;

         COleVariant var;
         CString str;

         set.GetFieldValue("Link_Name", var);
         str.Format("%s", var.lVal);
         pLink->link_name = str;

         set.GetFieldValue("ID", var);
         str.Format("%s", var.lVal);
         pLink->id = str;

         set.GetFieldValue("Group_Name", var);
         str.Format("%s", var.lVal);
         pLink->group_name = str;

         m_links_lst.AddTail(pLink);

         set.MoveNext();
      }
   }
   catch (CDaoException *e)
   {
      e->Delete();
      ERR_report("CDaoException reading links");
      return FAILURE;
   }

   return SUCCESS;
}

int C_localpnt_ovl::ReadFontsFromFile(const CString& strFilespec)
{
   ASSERT(m_pPointOverlayFonts != NULL);

   try
   {
      CFontDataSet fontData(m_database);
      fontData.set_filename(strFilespec);
      fontData.Open(dbOpenTable, "FontData", dbForwardOnly);

      CFontLinksSet fontLinks(m_database);
      fontLinks.set_filename(strFilespec);
      fontLinks.Open(dbOpenTable, "FontLinks", dbForwardOnly);

      m_pPointOverlayFonts->ReadFontsFromFile(&fontData, &fontLinks);
   }
   catch (CDaoException *e)
   {
      // font table is not required
      e->Delete();
   }

   return SUCCESS;
}

int C_localpnt_ovl::SavePointsToFile(CString& strFilespec)
{
   short numFields;

   try
   {
      CString strSql("DELETE FROM Points");
      execute_query(strSql);


      // get the number of fields (columns) in the current file db's "Points" table...
      CDaoTableDef tableDef(m_database);
      try
      {
         tableDef.Open("Points");
         numFields = tableDef.GetFieldCount();
      }
      catch (CDaoException *e)
      {
         CString message =                               // compose a message
            _T("Couldn't get field count -- Exception: ");
         if (e->m_pErrorInfo)                            // use exception data too
            message += e->m_pErrorInfo->m_strDescription;

         AfxMessageBox(message);                         // complain to the user
      }


      CLocalPointSet set(m_database);
      CLocalPointSet::set_filename(strFilespec);

      set.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbAppendOnly);

      POSITION position = m_data_object->GetHeadPosition();
      while (position)
      {
         C_localpnt_point* pPoint = m_data_object->GetNext(position);

         set.AddNew();
         set.m_ID = pPoint->m_id;
         set.m_Description = pPoint->m_description;
         set.m_Latitude = pPoint->m_latitude;
         set.m_Longitude = pPoint->m_longitude;
         set.m_Elevation = pPoint->m_elv;
         set.m_Area = pPoint->m_area_code;
         set.m_Country_Code = pPoint->m_country_code;
         set.m_Dtd_Id = pPoint->m_dtd_id;
         set.m_Horz_Accuracy = pPoint->m_horz_accuracy;
         set.m_Vert_Accuracy = pPoint->m_vert_accuracy;

         // we have to strip everything but the file name (including the
         // extension) when saving to lpt for backwards compatibility
         std::string icon_name = pPoint->GetIconName();
         // strip leading directory structure
         auto last_slash_position = icon_name.find_last_of('\\');
         if (last_slash_position != std::string::npos)
            icon_name = icon_name.substr(last_slash_position + 1);
         // string trailing extension
         icon_name.erase(icon_name.find_last_of('.'));
         set.m_Icon_Name = icon_name.c_str();

         set.m_Comment = pPoint->m_comment;
         set.m_Pt_Quality = pPoint->m_qlt;
         set.m_Elevation_Source = pPoint->m_elv_src;

         if (numFields == NUM_FIELDS_IN_POINTS_TABLE) // this file is the current version, so include code
         { // for the additional "Points" table fields.
            set.m_Elevation_Source_Map_Handler_Name = pPoint->m_elev_src_map_handler_name;

            char pszNum [32] = {0};
            CString series_name;
            series_name.Format("%d", pPoint->m_elev_src_map_series_id);
            set.m_Elevation_Source_Map_Series_Name = series_name;
         }

         set.m_Group_Name = pPoint->m_group_name;
         set.Update();
      }

      if (numFields != NUM_FIELDS_IN_POINTS_TABLE) // this file is NOT the current version, so warn the user that
      { // by saving the file under it's current name (a simple Save),
         // the newer "Points" table field information will be lost.
         int count = strFilespec.ReverseFind('\\');
         count = strFilespec.GetLength() - count - 1;
         CString fileName = strFilespec.Right(count);
         CString msg;
         msg.Format( _T("The Local Points file \"%s\" is FalconView version 4.1.0 or older, "), fileName);
         msg += _T("and as such the Elevation Source Product and Map Scale information ");
         msg += _T("will not be saved and will be lost once the file is closed.\n\n");
         msg += _T("Saving the file under a new name will make it's version current and will ");
         msg += _T("preserve the Elevation Source Product and Map Scale information.\n\n");
         AfxMessageBox(msg);
      }
   }
   catch (CDaoException *e)
   {
      e->Delete();
      ERR_report("CDaoException saving points");
      return FAILURE;
   }

   return SUCCESS;
}

int C_localpnt_ovl::SaveGroupsToFile(CString& strFilespec)
{
   try
   {
      CString strSql("DELETE FROM Group_Names");
      execute_query(strSql);

      //      CLocalPointGroupNameSet set;
      CLocalPointGroupNameSet set(m_database);

      CLocalPointGroupNameSet::set_filename(strFilespec);

      set.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbAppendOnly);

      POSITION position = m_group_lst.GetHeadPosition();
      while (position)
      {
         POINT_GROUP *pGroup = m_group_lst.GetNext(position);

         set.AddNew();
         set.m_Group_Name = pGroup->name;
         set.m_Search = pGroup->search;
         set.Update();
      }
   }
   catch(CDaoException *e)
   {
      e->Delete();
      ERR_report("CDaoException saving groups");
      return FAILURE;
   }

   return SUCCESS;
}

int C_localpnt_ovl::SaveLinksToFile(CString& strFilespec)
{
   try
   {
      CString strSql("DELETE FROM Links");
      execute_query(strSql);

      CDaoRecordset set(m_database);
      set.Open(dbOpenTable, "Links", dbAppendOnly);

      POSITION position = m_links_lst.GetHeadPosition();
      while (position)
      {
         POINT_LINK* pLink = m_links_lst.GetNext(position);

         set.AddNew();
         set.SetFieldValue("Link_Name", (LPCSTR)pLink->link_name);
         set.SetFieldValue("ID", (LPCSTR)pLink->id);
         set.SetFieldValue("Group_Name", (LPCSTR)pLink->group_name);
         set.Update();
      }
   }
   catch(CDaoException *e)
   {
      e->Delete();
      ERR_report("CDaoException saving links");
      return FAILURE;
   }

   return SUCCESS;
}

int C_localpnt_ovl::SaveFontsToFile(CString& strFilespec)
{
   ASSERT(m_pPointOverlayFonts != NULL);

   try
   {
      CString strSqlFontData("DELETE FROM FontData");
      CString strSqlFontLinks("DELETE FROM FontLinks");

      execute_query(strSqlFontData);
      execute_query(strSqlFontLinks);

      CFontDataSet fontDataSet(m_database);
      CFontLinksSet fontLinksSet(m_database);

      fontDataSet.set_filename(strFilespec);
      fontLinksSet.set_filename(strFilespec);

      fontDataSet.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbAppendOnly);
      fontLinksSet.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbAppendOnly);

      m_pPointOverlayFonts->SaveFontsToFile(&fontDataSet, &fontLinksSet);
   }
   catch(CDaoException *e)
   {
      // the font tables are not required
      e->Delete();
   }

   return SUCCESS;
}

int C_localpnt_ovl::load_list_from_points_table(CDaoDatabase* database, MapProj* map)
{
   C_localpnt_point *point;
   CMinLocalPointSet set;
   int result = FAILURE;

   //load points whose group_name "search" is TRUE
   try
   {
      CMinLocalPointSet set(database);

      set.Open(dbOpenSnapshot, "SELECT * FROM [Points]", dbForwardOnly | dbReadOnly);

      while (!set.IsEOF() && !set.IsBOF())
      {
         point = new C_localpnt_point(this);  //create new point

         point->set_point_data(set, map);

         // trim id and description for display purposes
         point->m_id.TrimRight();
         point->m_description.TrimRight();

         m_data_object->AddTail(point);
         set.MoveNext();
      }

      result=SUCCESS;
   }
   catch (CDaoException* e)
   {
      //tO DO: close querydef
      //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
      ERR_report(e->m_pErrorInfo->m_strDescription);
      e->Delete();
      if (set.IsOpen())
         set.Close();
   }
   catch (CMemoryException* e)
   {
      //tO DO: close querydef
      AfxMessageBox("memory exception");
      e->Delete();
      if (set.IsOpen())
         set.Close();
   }

   return result;
}
// end of load_list_from_points_table


// ----------------------------------------------------------------------------

//NOTE: uses get_specification() for the database filename
//NOTE: set_modified = TRUE; huh?
//NOTE: does not do any invalidation

// Purpose: loads all of the points from the datbase, taking into account the MODIFIED and DELETED tables
// so that the points loaded do not depend upon whether or not the database is "dirty"

int C_localpnt_ovl::fill_local_point_list_from_database(MapProj* map)
{
   // refresh the screen coordinates of each of the points in the list
   POSITION position = m_data_object->GetHeadPosition();
   while (position)
      m_data_object->GetNext(position)->refresh_screen_coordinates(map);

   return SUCCESS;
}

//TO DO (78): move this member function to a more appropriate class
//TO DO (79): test
bool C_localpnt_ovl::crosses_date_line(const d_geo_t in_ll, const d_geo_t in_ur) const
{

   // longitude diagram: (180) ---- (90) ----- 0 ---- 90 ---- 180 

   //if right lon < left lon we must have crossed the date line (or have a rectagle which 
   //is greater than 180 degrees of longitude - which should not be allowed)
   return (in_ur.lon < in_ll.lon);  
}
// end of crosses_date_line


// ----------------------------------------------------------------------------


// given a rectangle that crosses the international date line, two rectangles that cover the
// same area, but do not cross the international date line
int C_localpnt_ovl::split_across_date_line(const d_geo_t in_ll, const d_geo_t in_ur,
                                           d_geo_t& piece1_ll, d_geo_t& piece1_ur,
                                           d_geo_t& piece2_ll, d_geo_t& piece2_ur) const
{

   if (!crosses_date_line(in_ll, in_ur))
      return FAILURE;

   //calculate piece 1
   piece1_ll.lat = in_ll.lat;    //lattitudes stay the same
   piece1_ur.lat = in_ur.lat;    //lattitudes stay the same
   piece1_ll.lon = in_ll.lon;    //keep left longitude
   piece1_ur.lon = 180;          //date line

   //calculate piece 2
   piece2_ll.lat = in_ll.lat;    //lattitudes stay the same
   piece2_ur.lat = in_ur.lat;    //lattitudes stay the same
   piece2_ll.lon = -180;         //date line
   piece2_ur.lon = in_ur.lon;    //keep right longitude

   return SUCCESS;
}
// end of split_across_date_line

// ----------------------------------------------------------------------------

int C_localpnt_ovl::load_points_in_rectangle(MapProj *map, const d_geo_t ll, const d_geo_t ur, 
                                             CList<C_icon *, C_icon *> *list, CPointOverlayFilter *pFilter) 
{
   int result = FAILURE;

   CDaoDatabase *database=NULL;


   // change cursor to hour glass while opening file as long as we are not
   // scrolling the map
   std::auto_ptr<CWaitCursor> wait_cursor;
   if (!MapView::m_scrolling && !CMainFrame::GetPlaybackDialog().in_playback())
   {
      //
      // allocate the wait cursor
      //
      CWaitCursor* tmp_wait = new CWaitCursor;
      if (tmp_wait)
      {
         std::auto_ptr<CWaitCursor> tmp_auto_ptr(tmp_wait);
         wait_cursor = tmp_auto_ptr;
      }
   }

   // Create an object to manage opening and closing of databases
   // (see dbopncls.h).  If dbOpenerCloser opens the database,
   // it is closed when it goes out of scope

   try
   {
      CDBOpenerCloser dbOpenerCloser(m_database, get_specification()); //OK for m_database to be null
      database= dbOpenerCloser.GetDatabase();

      if (!crosses_date_line(ll, ur))
      {
         result = load_points_in_non_dateline_intersecting_rectangle(map, database, ll, ur, list);
      }
      else
      {
         d_geo_t piece1_ll;
         d_geo_t piece1_ur;
         d_geo_t piece2_ll;
         d_geo_t piece2_ur;
         if (split_across_date_line(ll, ur, piece1_ll, piece1_ur, piece2_ll, piece2_ur) == SUCCESS)
         {
            int result1 = load_points_in_non_dateline_intersecting_rectangle(map, database, piece1_ll, piece1_ur, list);
            int result2 = load_points_in_non_dateline_intersecting_rectangle(map, database, piece2_ll, piece2_ur, list);
            result = (result1 == SUCCESS && result2 == SUCCESS) ? SUCCESS : FAILURE;
         }
         else
         {  
            ERR_report("split_across_date_line failed");
         }
      }

   }
   catch (CDaoException* e)
   {
      //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
      ERR_report(e->m_pErrorInfo->m_strDescription);
      e->Delete();
   }
   catch (CMemoryException* e)
   {
      ERR_report("memory exception");
      e->Delete();
   }

   return result;
}
// end of load_points_in_rectangle


// ----------------------------------------------------------------------------
int C_localpnt_ovl::load_points_in_non_dateline_intersecting_rectangle(MapProj *map, CDaoDatabase* database, 
                                                                       const d_geo_t ll, const d_geo_t ur, CList<C_icon *, C_icon *> *list) const
{
   int result = FAILURE;

   try
   {
      //      CDBOpenerCloser dbOpenerCloser(m_database, get_specification()); //OK for m_database to be null
      //    database= dbOpenerCloser.GetDatabase();
      CMinLocalPointSet set(database);
      if (!database)
         return FAILURE;

      C_localpnt_point *point=NULL;

      CString ll_lat, ll_lon, ur_lat, ur_lon;
      ll_lat.Format("%f",ll.lat);
      ll_lon.Format("%f",ll.lon);
      ur_lat.Format("%f",ur.lat);
      ur_lon.Format("%f",ur.lon);

      //TO DO: kevin
      //TO DO: kevin: use a paramaterized stored query
      //CString SQL =  "SELECT * FROM [Points];"

      CString SQL = "SELECT Points.ID, Points.Group_Name, Points.Latitude, Points.Longitude, Points.Icon_Name, Points.Description " 
         "FROM [Group_Names] INNER JOIN [Points] ON [Group_Names].[Group_Name] = [Points].[Group_Name] " 
         "WHERE ((([Group_Names].[Search])=True) "
         "AND (Points.Latitude <= " + ur_lat + ") "
         "AND (Points.Latitude >= " + ll_lat + ") "
         "AND (Points.Longitude <= " + ur_lon + ") "
         "AND (Points.Longitude >= " + ll_lon + "));";

      set.Open(dbOpenSnapshot, SQL, dbForwardOnly | dbReadOnly);

      int count = 0;
      while (!set.IsEOF() && !set.IsBOF())
      {
         point = new C_localpnt_point(const_cast<C_localpnt_ovl *>(this));  //create new point

         //
         // For FalconView 3.2, we're going to need to get a MapProj*
         // somehow, probably from the PointExport overlay that indirectly called
         // this function.

         point->set_point_data(set, map);

         //trim id and description for display purposes
         point->m_id.TrimRight();
         point->m_description.TrimRight();

         list->AddTail(static_cast<C_icon*>(point)); //add point to list
         set.MoveNext();  //next record...

         count++;
      }

      set.Close();

      result=SUCCESS;
   }
   catch (CDaoException* e)
   {
      //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
      ERR_report(e->m_pErrorInfo->m_strDescription);
      e->Delete();
   }
   catch (CMemoryException* e)
   {
      AfxMessageBox("memory exception");
      e->Delete();
   }

   return result;
}
// end of load_points_in_non_dateline_intersecting_rectangle


// ----------------------------------------------------------------------------

HCURSOR C_localpnt_ovl::get_default_cursor()
{
   if (get_editor_mode() == ADD)
      return AfxGetApp()->LoadCursor(IDC_LOCAL_POINT_ADD_CURSOR);
   else
      return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}
// end of get_default_cursor


// ----------------------------------------------------------------------------

//static
int C_localpnt_ovl:: set_local_point_database_filespec(CString &text)
{
   return PRM_write_registry(HKEY_LOCAL_MACHINE,"Software\\XPlan\\CONFIGURATION",
      "Default Points Database", REG_SZ, (const unsigned char*) (const char *) text, text.GetLength()+1);
}
// end of set_local_point_database_filespec


// ----------------------------------------------------------------------------

// NOTE: this should only be called once from the overlay at startup. 
// After that, the overlay should use its member variable because
// the database name in the registry may have changed
//
// TO DO: rename 
CString C_localpnt_ovl::get_local_point_database_filespec()
{
   CString result;

   DWORD type;
   unsigned char buffer[MAX_PATH];
   DWORD buffer_size = MAX_PATH;

   if (PRM_read_registry(HKEY_LOCAL_MACHINE,"Software\\XPlan\\CONFIGURATION",
      "Default Points Database", &type, (unsigned char*) &buffer, &buffer_size) == SUCCESS)
   {
      result = buffer;

      //check type to see that it was a string
      if (type != REG_SZ)
         ERR_report("Local Point Database registry error.");
   }
   else
      result = "";

   return result;
}
// end of get_local_point_database_filespec


// ----------------------------------------------------------------------------

//TO DO: document this
void C_localpnt_ovl::close_and_delete_dao_objects()
{
   //close database 
   try
   {
      if (m_database)
      {  
         ASSERT_VALID(m_database);

         if (m_database->IsOpen())
            m_database->Close();
      }
   }
   catch (CDaoException* e)
   {
      //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
      ERR_report(e->m_pErrorInfo->m_strDescription);
      e->Delete();
   }
   catch (CMemoryException* e)
   {
      AfxMessageBox("CMemoryException");
      ERR_report("CMemoryException");
      e->Delete();
   }

   //delete database
   if (m_database)
   {  
      delete m_database;
      m_database=NULL;
   }
}
// end of close_and_delete_dao_objects

//NOTE: database must already be open
//create a table with the structure as the points table in the ICD
//except the indices and primary key
//TO DO: rename "create modified table"
int C_localpnt_ovl::create_table_with_points_structure_no_indices_etc(
   CDaoDatabase* database, const char* table_name)
{
   ASSERT(database);
   ASSERT_VALID(database);
   ASSERT(database->IsOpen());  //database must already be open

   int result=FAILURE;
   try
   {

      CDaoFieldInfo fi;

      // -------- Points table -----------
      CDaoTableDef td(database);
      td.Create(table_name);

      //ID
      clear_field_info(fi);
      fi.m_strName="ID";
      fi.m_nType=dbText;
      fi.m_lSize=12;
      fi.m_bRequired = TRUE;
      td.CreateField(fi);

      //Description
      td.CreateField("Description", dbText, 40); 

      //Latitude
      clear_field_info(fi);
      fi.m_strName="Latitude";
      fi.m_nType=dbDouble;
      fi.m_bRequired = TRUE;
      td.CreateField(fi);

      //Longitutde
      clear_field_info(fi);
      fi.m_strName="Longitude";
      fi.m_nType=dbDouble;
      fi.m_bRequired = TRUE;
      td.CreateField(fi);

      //Elevation
      td.CreateField("Elevation", dbInteger, 2); 

      //Elevation Source   
      clear_field_info(fi);
      fi.m_strName="Elevation_Source";
      fi.m_nType=dbText;
      fi.m_lSize=5;
      //fi.m_strValidationRule="\"DAFIF\" Or \"DTED\" Or \"UNK\" Or \"USER\""; 
      //fi.m_strValidationText="Valid values={DAFIF, DTED, UNK, USER}"; 
      fi.m_strDefaultValue="UNK";   
      td.CreateField(fi);

      // Elevation Source Product Name
      clear_field_info(fi);
      fi.m_strName="ElevationSourceMapHandlerName";
      fi.m_nType=dbText;
      fi.m_lSize=20;
      fi.m_strDefaultValue="UNK";
      td.CreateField(fi);

      // Elevation Source Map Series Name
      clear_field_info(fi);
      fi.m_strName="ElevationSourceMapSeriesName";
      fi.m_nType=dbText;
      fi.m_lSize=10;
      fi.m_strDefaultValue="UNK";
      td.CreateField(fi);

      //Pt_Quality
      clear_field_info(fi);
      fi.m_strName="Pt_Quality";
      fi.m_nType=dbText;
      fi.m_lSize=1;
      fi.m_strDefaultValue="0";
      td.CreateField(fi);

      //Area
      td.CreateField("Area", dbText, 1); 

      //Country Code
      td.CreateField("Country_Code", dbText, 2); 

      //Dtd_Id
      td.CreateField("Dtd_ID", dbText, 5); 

      //Horz_Accuracy
      clear_field_info(fi);
      fi.m_strName="Horz_Accuracy";
      fi.m_nType=dbSingle;
      //fi.m_strDefaultValue="-1";  //CREATE() FAILES IN THE RELEASE BUILD IF THIS LINE IS HERE. 
      td.CreateField(fi);

      //Vert Accuracy
      clear_field_info(fi);
      fi.m_strName="Vert_Accuracy";
      fi.m_nType=dbSingle;
      //fi.m_strDefaultValue="-1"; //CREATE() FAILES IN THE RELEASE BUILD IF THIS LINE IS HERE. 
      td.CreateField(fi);

      //Link Name
      td.CreateField("Link_Name", dbText, 255); 

      //Icon Name
      td.CreateField("Icon_Name", dbText, 255); 

      //Comment Name
      td.CreateField("Comment", dbText, 255); 

      //Group Name
      clear_field_info(fi);
      fi.m_strName="Group_Name";
      fi.m_nType=dbText;
      fi.m_lSize=64;
      fi.m_bRequired = TRUE;  //note this field is required in this table, but not below
      td.CreateField(fi);

      td.Append();

      result=SUCCESS;
   }
   catch(CDaoException* e) 
   {
      //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
      ERR_report(e->m_pErrorInfo->m_strDescription);
      e->Delete();
   }
   catch(CMemoryException* e) 
   {
      //AfxMessageBox("Memory Exception");
      ERR_report("Memory Exception");
      e->Delete();
   }

   return result;
}
// end of create_table_with_points_structure_no_indices_etc

void C_localpnt_ovl::check_that_database_is_open_and_reopen_as_needed()
{

   //ASSERT if database is not open they should be since they are not NULL
   ASSERT(m_database && m_database->IsOpen());

   //open database if it was mysteriously closed
   if (m_database && !m_database->IsOpen())  //if database is not open
   {
      try
      {
         //open database
         CString filespec = get_specification();
         //m_database->Open(filespec, TRUE /* exclusive */);
         m_database->Open(filespec);
      }
      catch (CDaoException* e)
      {
         //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
         ERR_report(e->m_pErrorInfo->m_strDescription);
         e->Delete();
      }
      catch (CMemoryException* e)
      {
         //AfxMessageBox("CMemoryException");
         ERR_report("CMemoryException");
         e->Delete();
      }
   }
}
// end of check_that_database_is_open_and_reopen_as_needed


// ----------------------------------------------------------------------------
//create and open database if needed (i.e., m_database != NULL)
int C_localpnt_ovl::create_and_open_member_database()
{
   int result = FAILURE;
   if (!m_database)
   {
      try
      {
         //create and open a database 
         m_database = new CDaoDatabase();  
         CString filespec = get_specification();
         //m_database->Open(filespec);

         // Set the exclusive flag in the read-only open to true because of:
         // PRB: DAO MDB on Read-Only Media Must Be Opened Exclusively
         // http://support.microsoft.com/?scid=kb;en-us;191737
         if (m_bIsReadOnly)
            m_database->Open(filespec, TRUE, TRUE);
         else
            m_database->Open(filespec, FALSE, FALSE);

         result = SUCCESS;
      }
      catch (CDaoException* e)
      {
         CString msg="The database could not be opened for the following reason:  " +
            e->m_pErrorInfo->m_strDescription;
         AfxMessageBox(msg);
         ERR_report(msg);
         e->Delete();
         close_and_delete_dao_objects();
      }
      catch (CMemoryException* e)
      {
         //AfxMessageBox("CMemoryException");
         ERR_report("CMemoryException");
         e->Delete();
         close_and_delete_dao_objects();
      }
   }
   else
   {
      ASSERT(m_database->IsOpen());
      result = SUCCESS;
   }

   return result;
}
// end of create_and_open_member_database


// ----------------------------------------------------------------------------


void C_localpnt_ovl::enter_edit_focus()
{
   //draw current selections
   POSITION pos = m_selected_points.GetHeadPosition();
   while (pos)
   {
      C_localpnt_point *point = m_selected_points.GetNext(pos);
      ASSERT(point);
      point->draw_selected(UTL_get_active_non_printing_view(), FALSE);
   }
}
// end of enter_edit_focus


// ----------------------------------------------------------------------------

void C_localpnt_ovl::release_edit_focus()
{
   //AfxMessageBox("release edit focus");

   //if (m_selected_points.GetCount() > 0)
   // ;

   //tO DO: ask whether or not to apply changes?
   if (m_EditDialogControl.DialogExists())
   {
      C_localpnt_ovl::m_EditDialogControl.ApplyChanges(TRUE);
      C_localpnt_ovl::m_EditDialogControl.DestroyDialog(); 
   }
}
// end of release_edit_focus


// ----------------------------------------------------------------------------

int C_localpnt_ovl::selected_union_test_selected(MapProj* map,  
                                                 CPoint point, UINT flags, boolean_t *drag, HCURSOR *cursor, 
                                                 HintText **hint, BOOL test)
{
   ASSERT(drag);
   ASSERT(cursor);
   ASSERT(hint);

   C_localpnt_point* pt;
   //CClientDC dc(view);

   *drag = FALSE;
   *cursor = NULL;
   *hint = NULL;

   // if we are above our display threshold, then we do nothing here
   if (m_above_threshold)
      return FAILURE;

   // see if the cursor is over an object
   pt = (C_localpnt_point*)icon_hit_test(map, point);

   // if we are not over a point, then return without handling the select
   if (!pt)
      return FAILURE;

   // if the local point editor is not up...
   if (!m_bEdit)
   { 
      //case (1):
      if (test)
      {
         m_hint.set_tool_tip(pt->get_tool_tip());
         m_hint.set_help_text(pt->get_help_text());
         *hint = &m_hint;

         //TO DO: need this?
         *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      }
      else
      {
         show_info(pt);    //show point information in remark dialog
      }
   }
   //else if we are in the editor, then begin drag according to current point type...
   else  
   {

      // if this is the current (topmost) overlay...
      if (get_current())
      {
         //case (2): Editor is up, this is the current overlay
         if (test)
         {
            // TO DO: implement copy feature in if block
            // TO DO: is this correct?
            if (flags == MK_CONTROL)
            {
               *cursor = AfxGetApp()->LoadCursor(IDC_ADDPOINT_CURSOR);
               m_hint.set_tool_tip("Copy point");
            }  
            else
               m_hint.set_tool_tip("Select point");
         }
         else  //select (not test)
         {
            // if the remark dialog is up AND we have the focus, then go ahead with
            // point info for this hit point
            if (CRemarkDisplay::test_active()  &&  CRemarkDisplay::get_focus_overlay() == this)
            {
               show_info(pt);
            }

            set_current_selection(pt);

            // store the drag status both in the overlay manager
            // and in the overlay
            *drag = TRUE;  //store the drag status in the overlay manager

            *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);

            //set the drag location to the mouse point (not the center of the icon)
            m_drag_start_loc = point;

            store_drag_info(pt);

            // drag operation does not actually start here: if user does 
            // move mouse before mouse-up, then we never go into move mode, 
            // hence m_in_drag is not set here... 
         }

      }
      // non-current local point overlay was selected
      else
      {
         //case (3): editor is up, non-current local point overlay was selected 
         if (test)
         {
            // TO DO: what does this mean? or do?
            // if the user point icon that was hit is the top most icon at the 
            // given point
            {
               CString s;

               s.Format("Select %s", OVL_get_overlay_manager()->GetOverlayDisplayName(this));
               m_hint.set_tool_tip(s);
               *hint = &m_hint;
            }
         }
         else // selected (not test)
         {
            OVL_get_overlay_manager()->make_current(this);
            OVL_get_overlay_manager()->invalidate_all();
            GUID currentEditorGuid = OVL_get_overlay_manager()->GetCurrentEditor();

            // must be in NO_EDIT or USER_POINT mode to select the overlay
            if (currentEditorGuid == GUID_NULL || currentEditorGuid == FVWID_Overlay_Points)
            {            
               // this chum overlay becomes the current overlay in FalconView
               OVL_get_overlay_manager()->make_current(this, TRUE);

               *drag = FALSE;
               *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

               if (!m_selected_points.Find(pt))
               {
                  unselect_current_selections();
                  // select the hit member, but go ahead and draw immediately rather than posting invalidation
                  set_current_selection(pt);
                  pt->draw_selected(UTL_get_active_non_printing_view(), TRUE); //immediate
               } 
            }

            if (m_selected_points.GetCount() == 1)
            {
               C_localpnt_point *current_selection = m_selected_points.GetHead();
               m_EditDialogControl.SetFocus(current_selection->m_id, 
                  current_selection->m_group_name, get_specification());
               set_link_edit_dlg_focus(current_selection);
            }
         }
      }
   }
   *hint = &m_hint;
   return SUCCESS;
}
// end of selected_union_test_selected


// ----------------------------------------------------------------------------

int C_localpnt_ovl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   if (m_bDragging)
   {
      HCURSOR cursor = NULL;
      HintText *hintText = NULL;

      drag(OVL_get_overlay_manager()->get_view_map(), point, flags, &cursor, &hintText);

      if (cursor != NULL)
         pMapView->SetCursor(cursor);
      if (hintText != NULL)
      {
         pMapView->SetTooltipText(_bstr_t(hintText->get_tool_tip()));
         pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hintText->get_help_text()));
      }

      return SUCCESS;
   }

   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   BOOL dummy;
   HCURSOR hCursor = NULL;
   HintText *hint = NULL;
   int ret = selected_union_test_selected(&mapProjWrapper, point, flags, &dummy, &hCursor, 
      &hint, TRUE /* test*/);
   if (hCursor!= NULL)
      pMapView->SetCursor(hCursor);
   if (ret == SUCCESS && hint != NULL)
   {
      pMapView->SetTooltipText(_bstr_t(hint->get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hint->get_help_text()));
   }

   return ret;
}
// end of test_selected

int C_localpnt_ovl::on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags)
{
   if (m_bDragging)
   {
      drop(OVL_get_overlay_manager()->get_view_map(), point, nFlags);
      return SUCCESS;
   }

   return FAILURE;
}

// ----------------------------------------------------------------------------

int C_localpnt_ovl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   HCURSOR cursor;
   HintText *hint = NULL;
   return selected_union_test_selected(&mapProjWrapper, point, flags, &m_bDragging, &cursor, 
      &hint, FALSE /* select (not test)*/);
}
// end of selected_union_test_selected


// ----------------------------------------------------------------------------

void C_localpnt_ovl::drag(ViewMapProj* map, CPoint point, UINT nFlags, 
                          HCURSOR *cursor, HintText **hint)
{
   ASSERT(cursor);
   ASSERT(hint);

   *hint = NULL;
   *cursor = NULL;

   static_cast<MapView *>(map->get_CView())->ScrollMapIfPointNearEdge(point.x,
      point.y);

   // don't do anything at all if we haven't moved and we're not yet 
   // in drag mode
   if (m_drag_start_loc == point  &&  !m_in_drag)
      return;

   //see if we have exceeded the drag threshold, if so note that
   if (!m_exceeded_drag_threshold)

      if (!drag_utils::in_drag_threshold(m_drag_start_loc, point))
         m_exceeded_drag_threshold=TRUE;

   // set the cursor to the move cursor if we are within the threshold region
   // and have not yet exceeded the drag threshold
   if (!m_exceeded_drag_threshold && drag_utils::in_drag_threshold(m_drag_start_loc, point))
      *cursor = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
   else
      *cursor = NULL;

   // if we haven't started the drag yet, do so now
   if (!m_in_drag)
   {
      ASSERT(m_selected_points.GetCount() > 0);  //something must be selected

      // initiate the icon drag process
      m_image_in_drag->begin_drag(map, m_drag_start_loc, (LPARAM)get_current_selection());

      m_in_drag = TRUE;
   }

   //we are dragging an icon image...
   m_image_in_drag->move_drag(map, point);

   // Check to see if the cursor is over an icon that can be snapped to with
   // the current context.  test_snap_to() returns TRUE in this case.
   if (OVL_get_overlay_manager()->test_snap_to(map, point))
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      m_hint.set_help_text("Snap to point");
      m_hint.set_tool_tip("Snap To");
      *hint = &m_hint;
   }
}
// end of drag


// ----------------------------------------------------------------------------

void C_localpnt_ovl::cancel_drag(ViewMapProj* map)
{
   m_bDragging = FALSE;

   if (m_in_drag)
   {
      ASSERT(m_image_in_drag);
      if (m_image_in_drag)
         m_image_in_drag->cancel_drag(map);
      m_in_drag=FALSE;
   }
}
// end of cancel_drag


// ----------------------------------------------------------------------------
BOOL C_localpnt_ovl::override_drag_lock_move()
{
   //display "override question and return TRUE" if the user chooses to override the drag lock
   return (AfxMessageBox(IDS_DRAG_LOCK_OVERRIDE_PROMPT, MB_YESNO) == IDYES);
}
// end of override_drag_lock_move


// ----------------------------------------------------------------------------

BOOL C_localpnt_ovl::override_drag_lock_delete()
{
   //display "override question and return TRUE" if the user chooses to override the drag lock
   return (AfxMessageBox(IDS_DRAG_LOCK_DELETE_OVERRIDE_PROMPT, MB_YESNO) == IDYES);
}
// end of override_drag_lock_delete


// ----------------------------------------------------------------------------

// if we don't need to remember the drag lock state from the last 
// session then remove this registry and set a default value instead
boolean_t C_localpnt_ovl::get_drag_lock_state()
{
   return g_drag_lock_state;
}
// end of get_drag_lock_state


// ----------------------------------------------------------------------------

void C_localpnt_ovl::set_drag_lock_state(boolean_t drag_state)
{
   g_drag_lock_state = drag_state;
   PRM_set_registry_int("C_localpnt_ovl", "drag lock state", drag_state);
}
// end of set_drag_lock_state


// ----------------------------------------------------------------------------

void C_localpnt_ovl::drop(ViewMapProj* map, CPoint point, UINT nFlags)
{
   CWindowDC            dc(map->get_CView());
   CWnd *view = dc.GetWindow();

   m_bDragging = FALSE;

   if (m_in_drag)
   {
      //CFvwUtil fvwutl;

      // If we haven't moved out of threshold rect, then cancel the drag/drop operation
      if (!m_exceeded_drag_threshold && drag_utils::in_drag_threshold(m_drag_start_loc, point))
         m_image_in_drag->cancel_drag(map);

      // Else If this is a read-only Local Points overlay, display a message box
      // and then cancel the drag/drop operation...
      else if (m_bIsReadOnly)
      {
         // Get the current Local Point overlay...
         C_overlay* m_current_overlay = OVL_get_overlay_manager()->get_current_overlay();

         // Get the current file name...
         CString fileName = OVL_get_overlay_manager()->GetOverlayDisplayName(m_current_overlay);

         AfxMessageBox("The Local Points overlay file " + fileName + " cannot be edited because it is Read-Only.  If you wish to edit this file, first save it under a different name.");
         // return SUCCESS;
         // return FAILURE;
         m_image_in_drag->cancel_drag(map);
      }
      else
      { 
         C_localpnt_point*    pt = NULL;

         // end the drag and get the point information LPARAM from begin drag call
         m_image_in_drag->end_drag(map, point, (LPARAM*)&pt);
         ASSERT(pt);  

         // get the absolute screen coordinates
         CPoint spt(point);
         view->ClientToScreen(&spt);

         CWnd* pDropWnd = view->WindowFromPoint(spt);
         ASSERT(pDropWnd);

         //TO DO: is this comment correct?
         // if this is the list control window or the mapview window
         // then drag our droppable image
         if (pDropWnd->m_hWnd == view->m_hWnd)
         {
            //TO DO: allow for the functionality in the if block
            // dragging while holding down the CONTROL key will duplicate an existing
            // icon entry
            if (nFlags & MK_CONTROL)
            {
               //TO DO: copy other information instead of just icon name?
               //(e.g., group name, etc?)
               //copy point instead of moving it
               C_drag_item di;
               di.m_title = pt->GetIconName();
               //di.m_item_icon = pt->m_displayIcon;
               di.m_item_icon = pt->m_icon_image;
               receive_new_point(map, point, nFlags, &di);
               set_modified(TRUE);  //TO DO: need this here or does receive new point do this?
            }
            else
            {
               if ((!get_drag_lock_state() || override_drag_lock_move()) && pt) //if drag lock is not on or user chooses to ignore it...
               {
                  // After potentially putting up the message box, the variable can be destroyed.

                  //if we are moving the current selection and the edit dialog is up...
                  if (m_EditDialogControl.DialogExists())
                  {
                     //apply changes in edit dialog 
                     //if (m_EditDialogControl.IsDialogModified())
                     m_EditDialogControl.ApplyChanges(FALSE);
                  }

                  // These are to temporarily store the new lat/lon values we
                  // get when we actually drop the point, whether it's from a
                  // snap-to-info struct or just from the map itself. They are
                  // initialized to the current values so we can tell if the
                  // user cancels out of a multi-choice snap-to dialog.
                  float new_latitude  = m_selected_points.GetHead()->m_latitude;
                  float new_longitude = m_selected_points.GetHead()->m_longitude;

                  // These are to store the new screen coordinate values.
                  int   new_x         = 0;
                  int   new_y         = 0;

                  // Check to see if the cursor is over an icon that can be snapped to with
                  // the current context.  test_snap_to() returns TRUE in this case.
                  SnapToInfo* snap_to_info = NULL;
                  if (OVL_get_overlay_manager()->test_snap_to(map, point))
                  {
                     snap_to_info = new SnapToInfo();
                     memset(snap_to_info, 0, sizeof(snap_to_info));

                     if (OVL_get_overlay_manager()->do_snap_to(map, point, snap_to_info))
                     {
                        new_latitude  = (float)snap_to_info->m_lat;
                        new_longitude = (float)snap_to_info->m_lon;
                        map->geo_to_surface(new_latitude, new_longitude, &new_x, &new_y);
                     }
                  }
                  else
                  {
                     // compute the new location (lat/lon) based upon the CPoint
                     degrees_t      lat;
                     degrees_t      lon;
                     map->surface_to_geo(point.x, point.y, &lat, &lon);
                     new_latitude  = (float) lat;
                     new_longitude = (float) lon;
                     new_x = point.x;
                     new_y = point.y;
                  }

                  // if we're not at the exact same point as we were before...
                  if (!(m_selected_points.GetHead()->m_latitude == new_latitude && m_selected_points.GetHead()->m_longitude == new_longitude))
                  {
                     //
                     // BUG FIX NOTE:
                     // Originally, this part of the function just called pt->unselect()
                     // and view->InvalidateRect(pt->m_text_rect, FALSE) on the original
                     // screen location for this point, and then, just before the call to
                     // update_point_in_database_because_point_moved(), it invalidated the
                     // new screen location for this point.  This caused a bug when the
                     // function update_point_in_database_because_point_moved() put up an
                     // AfxMessageBox(), causing the draw() call to the overlay to complete,
                     // and the invalidations to not work properly.  Now, we save off copies
                     // of the point objects, and then we invalidate them AFTER the place
                     // where the AfxMessageBox() happens.
                     //

                     // Save off the point so we can invalidate it later.
                     C_localpnt_point old_pt = *(m_selected_points.GetHead());

                     // set the point to its new values
                     m_selected_points.GetHead()->m_latitude  = new_latitude;
                     m_selected_points.GetHead()->m_longitude = new_longitude;
                     m_selected_points.GetHead()->m_x         = new_x;
                     m_selected_points.GetHead()->m_y         = new_y;

                     // mark the new location for drawing selected
                     m_selected_points.GetHead()->draw_selected(map->get_CView(), TRUE);

                     //TO DO: put this code in one place (it's here and in receive new point
                     // invalide the rectangle in the new location
                     m_selected_points.GetHead()->m_text_rect = CRect(point.x + 16 /*pt->m_rect_size_x*/ + 2, 
                        point.y + 16 /*pt->m_rect_size_y*/ - m_selected_points.GetHead()->m_text_rect.Height(),
                        point.x + 16 /*pt->m_rect_size_x*/ + 2 + m_selected_points.GetHead()->m_text_rect.Width(), 
                        point.y + 16 /*pt->m_rect_size_y*/ + 1);


                     // Save off the point so we can invalidate it later.
                     C_localpnt_point new_pt = *(m_selected_points.GetHead());

                     // Update the points in the points table
                     // NOTE: this function may end up displaying an AfxMessageBox().
                     // See the bug fix description just above for further details.
                     update_point_in_database_because_point_moved(m_selected_points.GetHead(), snap_to_info);
                     Update3DPoint(m_selected_points.GetHead());

                     // invalidate the old and new screen coordinates
                     old_pt.invalidate();
                     new_pt.invalidate();

                     //update info box if we're updating the point in it
                     if (get_current_hilight() == m_selected_points.GetHead())
                        show_info(m_selected_points.GetHead());

                     //
                     // When we move a point, we want to pop up the local point
                     // editor dialog if it's not already open, and set the focus
                     // to the moved point in the editor dialog
                     //
                     if (!m_EditDialogControl.DialogExists())
                        m_EditDialogControl.Create();

                     m_EditDialogControl.SetFocus(m_selected_points.GetHead()->m_id, m_selected_points.GetHead()->m_group_name, get_specification(), FALSE, FALSE);
                     m_EditDialogControl.ShowDialog();

                     set_link_edit_dlg_focus(m_selected_points.GetHead());

                     OnElementModified(m_selected_points.GetHead(), false);

                     set_modified(TRUE);
                  }
               }
            }
         }
      }

      // turn off drag mode
      m_in_drag = FALSE;
      m_exceeded_drag_threshold=FALSE;

   }  
} // end of drop

//TO DO: Kevin: make const
//returns TRUE if the key exists 
boolean_t C_localpnt_ovl::does_key_exist(CString id, CString group_name)
{
   POSITION position = m_data_object->GetHeadPosition();
   while (position)
   {
      C_localpnt_point *point = m_data_object->GetNext(position);

      if (point->m_id == id && point->m_group_name == group_name)
         return TRUE;
   }

   return FALSE;
}

// ----------------------------------------------------------------------------

POSITION C_localpnt_ovl::get_local_point_position_in_list(
   const char* id, const char* group_name) const
{
   C_localpnt_point* point;

   //find point that matches ID & group name in local point overlay
   POSITION at;
   POSITION next = m_data_object->GetHeadPosition();
   BOOL found=FALSE;
   while (next)
   {
      at=next;
      point = m_data_object->GetNext(next);
      if (point->m_id.Compare(id)==0 && point->m_group_name.Compare(group_name)==0)
      {
         found=TRUE;
         ASSERT(m_data_object->GetAt(at)==point);
         break;
      }
   }

   if (!found)
      at=NULL;

   return at;
}
// end of get_local_point_position_in_list


// ----------------------------------------------------------------------------

//returns a pointer to the point with the specified id and groupname
C_localpnt_point* C_localpnt_ovl::find_local_point_in_list(
   const char* id, const char* group_name) const
{
   C_localpnt_point* point=NULL;
   POSITION pos = get_local_point_position_in_list(id, group_name);
   if (pos)
      point = m_data_object->GetAt(pos);

   return point;
}
// end of find_local_point_in_list

// ----------------------------------------------------------------------------

//NOTE: only shows info for local points currently loaded
int C_localpnt_ovl::show_info(const char* id, const char* group_name)
{
   ASSERT(0);  //this function has never been tested
   C_localpnt_point* point= find_local_point_in_list(id, group_name);
   return show_info(point); 
}
// end of show_info


// ----------------------------------------------------------------------------

// Show the local point associated with the screen x,y 
// does the following:
// 1) unhilights current hilight and sets hilight to new point
// 2) puts up the Local Point Information dialog
//THROWS CMemoryException
int C_localpnt_ovl::show_info(C_localpnt_point* point)
{
   CFvwUtil *util = CFvwUtil::get_instance();

   CString text("");
   CString temp("");

   text = "ID: " + point->m_id + "\r\n";
   text += "Group Name: " + point->m_group_name + "\r\n";
   text += "Description: " + point->m_description + "\r\n";
   //TO DO change CLocalPoint::m_Dtd_ID to m_DTD_Id for consitency w/ CLocalPointRecordSet?

   text += "DTD ID: " + CString(point->m_dtd_id) + "\r\n";


   //elevation
   text += "Elevation: ";
   if (point->m_elv_src == "UNK")
      text += "Unknown\r\n";
   else
   {
      temp.Format("%d", point->m_elv);  
      text += temp + " feet\r\n";
   }
   text += "Elevation Source: " + point->m_elv_src + "\r\n";
   text += "Elevation Source Product: " + point->m_elev_src_map_handler_name + "\r\n";

   temp.Format("%d", point->m_elev_src_map_series_id);  
   text += "Elevation Source Map Scale: " + temp + "\r\n";

   temp.Format("Altitude: %0.f feet\r\n",
      METERS_TO_FEET(point->m_altitude_meters));
   text += temp;

   //TO DO: make sure this is the proper behavior...
   //country
   char tstr[121];
   text += "Country: ";
   if (util->find_country(point->m_country_code, tstr))
      text += tstr;
   text += "\r\n";

   //location

   text += "Location: " + GEO_get_formatted_location_string(point->m_latitude, 
      point->m_longitude) + "\r\n";

   text += "ICAO Area: " + CString(point->m_area_code) + "\r\n";

   text += "Point Quality: "+ CString(point->m_qlt) + "\r\n";

   text += "Horz. Accuracy: "; 
   if (point->m_horz_accuracy !=-1)
   {
      temp.Format("+/- %.1f feet\r\n", point->m_horz_accuracy);
      text += temp;
   }
   else
      text += "Unknown\r\n";

   text += "Vert. Accuracy: "; 
   if (point->m_vert_accuracy !=-1)
   {
      temp.Format("+/- %.1f feet\r\n", point->m_vert_accuracy);
      text += temp;
   }
   else
      text += "Unknown\r\n";

   CStringArray links;
   get_links_for_point(point->m_id, point->m_group_name, links);
   text+= "Links:";
   text+="\r\n";
   for (int i = 0; i < links.GetSize(); i++)
   {
      text += "     ";
      text += links[i];
      text += "\r\n";
   }

   text += "Comment: \r\n" + point->m_comment + "\r\n";

   CString title;
   title.Format("Point Information (%s)", OVL_get_overlay_manager()->GetOverlayDisplayName(this));

   // display dialog box showing data about the local point info
   CRemarkDisplay::display_dlg(AfxGetApp()->m_pMainWnd, text, title, this);

   set_current_hilight(point);  //highlight point

   return SUCCESS;
}

// ----------------------------------------------------------------------------
void C_localpnt_ovl::view_link_callback(ViewMapProj *map, C_icon* point, LPARAM index)
{
   // This callback function must take a ViewMapProj*, but it doesn't need
   // one, so all it does is call the version of this function without it.
   view_link((C_localpnt_point*) point, index);
}
// end of view_link_callback


// ----------------------------------------------------------------------------
void C_localpnt_ovl::view_link(C_localpnt_point* point, LPARAM index)
{
   ASSERT(point);
   if (!point)
      return;

   C_localpnt_ovl* ovl = (C_localpnt_ovl*) point->m_overlay;

   ASSERT(ovl);
   if (ovl)
   {
      if (index >= 0 && index < ovl->m_links.GetSize())
         CLinksEditDlg::view_link((LPSTR)(LPCSTR)(ovl->m_links[index]));
   }
}
// end of view_link


// ----------------------------------------------------------------------------
// add local point commands to menu if hit
//static
void C_localpnt_ovl::menu(ViewMapProj* map, CPoint point, 
                          CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   CFvwUtil *pFvwUtil = CFvwUtil::get_instance();
   m_listObjectMenuItems.RemoveAll();

   C_localpnt_point * pt = (C_localpnt_point *) icon_hit_test(map, point);
   if (!pt)
      return;

   // Set the current point as selected.  This is for when
   // a user has one point selected, then right-clicks another
   // point.  The new point needs to be marked as selected.
   if (get_current() && m_bEdit)
   {
      set_current_selection(pt);
      pt->invalidate();

      // since we are doing invalidations in above, we need to make sure
      // that the view is updated before proceeding.  Otherwise, a draw could
      // occur anytime after we have stored the icons.  This will be a problem
      // because we will be drawing the overlay from scratch and draw will destroy
      // all the icons in the icon list.
      CView *view = UTL_get_active_non_printing_view();
      if (view)
      {
         view->UpdateWindow();
      }
   }

   // count the number of items hit.  If multiple points are hit 
   // the menu structure is different
   m_links.RemoveAll();
   CList <C_localpnt_point *, C_localpnt_point *> pt_lst;
   POSITION position = m_data_object->GetHeadPosition();
   while (position)
   {
      C_localpnt_point *pt = m_data_object->GetNext(position);

      BOOL visible = true;

      // if the overlay is unnamed then the points have not been
      // filtered on the group name - we will do that here
      POSITION position = m_group_lst.GetHeadPosition();
      while (position)
      {
         POINT_GROUP *group = m_group_lst.GetNext(position);
         if (group->name == pt->m_group_name)
         {
            visible = group->search;
            break;
         } 
      }

      if (!is_point_in_drag(pt) && pt->m_rect.PtInRect(point) && visible)
         pt_lst.AddTail(pt);
   }

   // for each point hit (rather than just the topmost point)
   position = pt_lst.GetHeadPosition();
   int link_index = 0;
   while (position)
   {
      C_localpnt_point *pt = pt_lst.GetNext(position);

      // This item gets added no matter what
      if (pt_lst.GetCount() == 1)
      {
         CString txt;
         txt.Format("Get Info on %s...", pt->m_id);
         list.AddTail(new CCIconMenuItem(txt, (C_icon*) pt, &show_info_callback));
      }
      else
      {
         CString txt;
         txt.Format("Get Info on %s", pt->m_id);
         list.AddTail(new CCIconMenuItem(txt, (C_icon *)pt, &show_info_callback));
      }

      CString sub;
      if (pt_lst.GetCount() > 1)
         sub.Format("%s\\", pt->m_id);

      // These items get added only if the editor is turned on
      if (get_current() && m_bEdit)
      {
         list.AddTail(new CCIconMenuItem(sub + "Edit Point...", 
            (C_icon*) pt, &edit_obj_properties_callback));

         list.AddTail(new CCIconMenuItem(sub + "Edit Point Links...",
            (C_icon*) pt, &edit_local_point_links_callback));

         list.AddTail(new CCIconMenuItem(sub + "Delete Point...", 
            (C_icon*) pt, &delete_local_point_callback));

         // give overlay element callback a chance to add menu items for the first item
         //
         if (pt == pt_lst.GetHead())
         {
            const int nOverlayHandle = OVL_get_overlay_manager()->get_overlay_handle(this);
            const UINT size = m_vecOverlayElementCallbacks.size();
            for(UINT i=0;i<size;++i)
            {
               try
               {
                  CString strMenuItems = (char *)m_vecOverlayElementCallbacks[i]->GetMenuItems(nOverlayHandle, 
                     _bstr_t(pt->GetObjectGuid().c_str()));

                  CList<CString*, CString*> listTextItems;
                  pFvwUtil->parse_the_text(strMenuItems, listTextItems);

                  POSITION position = listTextItems.GetHeadPosition();
                  int nIndex = 0;
                  while (position)
                  {
                     CString* pText = listTextItems.GetNext(position);
                     list.AddTail(new CCIconLparamMenuItem(pText->GetBuffer(), pt, overlay_element_callback, nIndex));
                     m_listObjectMenuItems.AddTail(*pText);
                     delete pText;
                     nIndex++;
                  }
               }
               catch(_com_error &e)
               {
                  CString msg;
                  msg.Format("OnSelected callback failed: %s", (char *)e.Description());
                  ERR_report(msg);
               }
            }
         }
      }

      list.AddTail(new CCIconMenuItem(sub + "Target Graphic Printing...",
         (C_icon*) pt, &target_graphic_printing_callback));

      // add the links to the menu
      CStringArray links;
      get_links_for_point(pt, links);

      for (int i = 0; i < links.GetSize(); i++)
      {
         CString text;

         text = links[i];
         text = "Links\\" + text;
         list.AddTail(new CCIconLparamMenuItem((LPCSTR)text,(C_icon*) pt, &view_link_callback, link_index++));
      }

      m_links.Append(links);
   }
}

boolean_t C_localpnt_ovl::paste_OLE_data_object( ViewMapProj* map, CPoint* pPt, COleDataObject* data_object)
{
   int nPhotosAdded = 0;

   // if an image file with EXIF data was dropped on this overlay, then add it as a new point
   if ( data_object->IsDataAvailable(CF_HDROP) )
   {
      STGMEDIUM stg_medium;
      HGLOBAL hglobal = NULL;

      // get the file name and extension from the COLEDataObject
      data_object->GetData((CLIPFORMAT) CF_HDROP, &stg_medium);
      if (stg_medium.tymed == TYMED_HGLOBAL)
         hglobal = stg_medium.hGlobal;

      // how many files are dropped onto your Form
      // (see DragQueryFile in win32.hlp for details)
      UINT nFileCount = DragQueryFile( (HDROP)hglobal,0xffffffff,0,0);

      // get the name of these files and see if you can use them
      for(UINT i = 0; i < nFileCount; ++i)
      {
         char buffer[MAX_PATH];
         DragQueryFile ((HDROP)hglobal, i, buffer, MAX_PATH);

         ExifImage image((wchar_t *)_bstr_t(buffer));

         // if the file has geo-tagging information 
         double dLat, dLon;
         if (image.GetGpsLocation(dLat, dLon))
         {
            CString strImageDescription;
            image.GetImageDescription(strImageDescription);

            C_localpnt_point* pPoint = new C_localpnt_point(this);

            CNewLocalPointDefaults newLocalPointDefaults;

            pPoint->m_id = newLocalPointDefaults.get_and_increment_id();
            pPoint->m_group_name = newLocalPointDefaults.GetGroupName();
            pPoint->m_latitude = static_cast<float>(dLat);
            pPoint->m_longitude = static_cast<float>(dLon);
            pPoint->m_vert_accuracy = -1;
            pPoint->m_horz_accuracy = -1;
            pPoint->m_elv = -1;
            pPoint->m_elv_src = "UNK";
            pPoint->m_elev_src_map_handler_name = "UNK";
            pPoint->m_elev_src_map_series_id = 0;
            std::string icon_name = "localpnt\\Photo.ico";
            pPoint->m_icon_image = CIconImage::load_images(icon_name.c_str());
            pPoint->SetIconName(icon_name.c_str());
            pPoint->m_comment = strImageDescription;

            insert_point(NULL, pPoint, NULL);
            AddLink(pPoint->m_id, pPoint->m_group_name, buffer);
            nPhotosAdded++;
         }
      }
   }

   if (nPhotosAdded > 0)
   {
      OVL_get_overlay_manager()->InvalidateOverlay(this);
      return TRUE;
   }

   return FALSE;
}



// ----------------------------------------------------------------------------
//static
void C_localpnt_ovl::delete_local_point_callback(ViewMapProj *map, C_icon* point)
{
   // This callback function must take a ViewMapProj*, but it doesn't need
   // one, so all it does is call the version of this function without it.
   ASSERT(point);
   if (!point)
      return;

   C_localpnt_ovl *thisovl = (C_localpnt_ovl*) point->m_overlay;

   ASSERT(thisovl);
   if (thisovl)
      thisovl->delete_local_point(map, (C_localpnt_point*) point);
}
// end of delete_local_point_callback

// static
void C_localpnt_ovl::overlay_element_callback(ViewMapProj* pMap, C_icon* pIcon, LPARAM lpData)
{
   if (pIcon == NULL)
      return;

   C_localpnt_point *pPoint = static_cast<C_localpnt_point *>(pIcon);
   ((C_localpnt_ovl *)pIcon->m_overlay)->overlay_element_callback_hlpr(pPoint, static_cast<int>(lpData));
}

void C_localpnt_ovl::overlay_element_callback_hlpr(C_localpnt_point* pPoint, int nMenuItemIndex)
{
   const int nOverlayHandle = OVL_get_overlay_manager()->get_overlay_handle(this);
   std::string strObjectGuid = pPoint->GetObjectGuid();

   POSITION pos = m_listObjectMenuItems.FindIndex(nMenuItemIndex);
   CString strMenuItem;
   if (pos)
      strMenuItem = m_listObjectMenuItems.GetAt(pos);

   const UINT size = m_vecOverlayElementCallbacks.size();
   for(UINT i=0;i<size;++i)
   {
      try
      {
         m_vecOverlayElementCallbacks[i]->OnMenuItem(nOverlayHandle, _bstr_t(strObjectGuid.c_str()),
            _bstr_t(strMenuItem));
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnSelected callback failed: %s", (char *)e.Description());
         ERR_report(msg);
      }
   }
}

// static
void C_localpnt_ovl::target_graphic_printing_callback(ViewMapProj *map, C_icon* point)
{
   C_localpnt_ovl *thisovl = (C_localpnt_ovl*) point->m_overlay;

   thisovl->target_graphic_printing_callback_hlpr(point);
}

void C_localpnt_ovl::target_graphic_printing_callback_hlpr(C_icon *icon)
{
   C_localpnt_point* point = static_cast<C_localpnt_point*>(icon);
   if (point == nullptr)
      return;

   LocalPointDataObject* pDataObject = static_cast<LocalPointDataObject *>(GetTabularEditorDataObject());

   CTargetGraphicsDlg target_dlg;
   CList<target_t, target_t> selected_list;

   target_t sel_item;
   sel_item.comment = point->m_comment;
   sel_item.description = point->m_description;
   sel_item.id = point->m_id;

   degrees_t lat;
   degrees_t lon;
   point->get_lat(lat);
   point->get_lon(lon);
   sel_item.lat = lat;
   sel_item.lon = lon;

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
   sel_item.links = links;

   selected_list.AddTail(sel_item);
   target_dlg.set_selected_list(&selected_list);
   target_dlg.DoModal();
}


// ----------------------------------------------------------------------------
//static
void C_localpnt_ovl::edit_local_point_links_callback(ViewMapProj *map, C_icon* point)
{
   // This callback function must take a ViewMapProj*, but it doesn't need
   // one, so all it does is call the version of this function without it.
   edit_local_point_links((C_localpnt_point*) point);
}
// end of edit_local_point_links_callback


// ----------------------------------------------------------------------------

//static
void C_localpnt_ovl::edit_local_point_links(C_localpnt_point* point)
{
   ASSERT(point);
   if (!point)
      return;

   C_localpnt_ovl* ovl = (C_localpnt_ovl*) point->m_overlay;

   ASSERT(ovl);
   if (ovl)
   {
      // if the object doesn't exist yet, create it
      if (NULL == ovl->m_LinkEditDialog)
         ovl->m_LinkEditDialog = new CLinksEditDlg(&static_save_links);

      // if the window isn't created, create it
      if (ovl->m_LinkEditDialog->GetHwnd() == NULL)
         ovl->m_LinkEditDialog->Create();

      ovl->set_link_edit_dlg_focus(point);
      ovl->set_current_selection(point);
   }
}
// end of edit_local_point_links


// ----------------------------------------------------------------------------
//static
void C_localpnt_ovl::edit_obj_properties_callback(ViewMapProj *map, C_icon* point)
{
   // This callback function must take a ViewMapProj*, but it doesn't need
   // one, so all it does is call the version of this function without it.
   edit_obj_properties((C_localpnt_point*) point);
}
// end of edit_obj_properties_callback


// ----------------------------------------------------------------------------

//static
void C_localpnt_ovl::show_info_callback(ViewMapProj *map, C_icon* point)
{
   // This callback function must take a ViewMapProj*, but it doesn't need
   // one, so all it does is call the version of this function without it.
   ASSERT(point);
   if (!point)
      return;

   C_localpnt_ovl *thisovl = (C_localpnt_ovl*) point->m_overlay;

   ASSERT(thisovl);
   if (thisovl)
      thisovl->show_info((C_localpnt_point*) point);
}
// end of show_info_callback


// ----------------------------------------------------------------------------

void C_localpnt_ovl::edit_obj_properties(C_localpnt_point* point)
{
   ASSERT(point);
   if (!point)
      return;

   C_localpnt_ovl* ovl = (C_localpnt_ovl*) point->m_overlay;

   ASSERT(ovl);
   if (ovl)
   {
      ovl->m_EditDialogControl.Create();
      ovl->m_EditDialogControl.SetFocus(point->m_id, point->m_group_name, ovl->get_specification());
      ovl->set_current_selection(point);
      ovl->m_EditDialogControl.ShowDialog();
   }
}
// end of edit_obj_properties


// ----------------------------------------------------------------------------

boolean_t C_localpnt_ovl::test_snap_to(ViewMapProj* map, CPoint point)
{
   C_localpnt_point *pt;
   pt = (C_localpnt_point*) hit_test(map, point);

   return (pt !=NULL); //return true if point !=NULL
}
// end of test_snap_to


// ----------------------------------------------------------------------------

boolean_t C_localpnt_ovl::do_snap_to(ViewMapProj* map, CPoint point, 
                                     CList<SnapToInfo *, SnapToInfo *> &snap_to_list)
{
   ASSERT(map);

   C_localpnt_point  *local_point;
   SnapToInfo        *info;
   POSITION          pos;
   boolean_t         hit            = FALSE;

   // find any local points that "point" hits and add to snap to list
   pos = m_data_object->GetHeadPosition();
   while (pos)
   {
      local_point = m_data_object->GetNext(pos);

      ASSERT(local_point);
      ASSERT(local_point->m_icon_image);

      // Only if we can satisfy the following three conditions
      // will we add the current point to the snap-to list
      // 1. there is a non-NULL local point
      // 2. the local point being dragged (if there is one)
      //    is not the local point we're currently examining
      // 3. the current local point is hit by the point being dropped
      if (local_point && !is_point_in_drag(local_point) && local_point->hit_test(point))
      {
         //fill in info structure from rec
         info = new SnapToInfo();

         info->m_lat = local_point->m_latitude;
         info->m_lon = local_point->m_longitude;
         info->m_elv = local_point->m_elv;
         if (local_point->m_elv_src == "DAFIF")
            info->m_elv_src = SnapToInfo::DAFIF;
         else if (local_point->m_elv_src == "UNK")
            info->m_elv_src = SnapToInfo::ELV_UNKNOWN;
         else if (local_point->m_elv_src == "USER")
            info->m_elv_src = SnapToInfo::USER_DEFINED_ELV;
         else if (local_point->m_elv_src == "DTED")
            info->m_elv_src = SnapToInfo::DTED;

         info->m_strKey.Format("%s,%s", local_point->m_id, local_point->m_group_name);

         info->m_horz_accuracy = local_point->m_horz_accuracy;
         info->m_vert_accuracy = local_point->m_vert_accuracy;
         info->m_area_code = local_point->m_area_code;
         strcpy(info->m_dtd_id, local_point->m_dtd_id);
         strcpy(info->m_country_code, local_point->m_country_code);

         info->m_qlt = local_point->m_qlt;

         CString id = local_point->m_id;
         id.Remove('\0');
         id.MakeUpper();
         if (id.GetLength() > 12)
         {
            id.Truncate(12);
         }
         if (m_bHasBeenSaved)
         {
            CString fix = id + "/L\0";
            strncpy_s(info->m_fix_rs, 15, fix, 15);
            info->m_pnt_src_rs = SNAPTO_STRUCT::LOCAL_SRC;
         }
         else
         {
            CString fix = "." + id + '\0';
            strncpy_s(info->m_fix_rs, 15, fix, 15);
            info->m_pnt_src_rs = SNAPTO_STRUCT::USER_SRC;
         }

         // write the database name for route snap-to
         strncpy(info->m_db_lookup_rs, info->m_strKey, sizeof(info->m_db_lookup_rs));
         strncpy(info->m_db_lookup_filename_rs, get_specification(), sizeof(info->m_db_lookup_filename_rs));

         strncpy(info->m_description_rs, local_point->m_description, sizeof(info->m_description_rs));

         strcpy(info->m_icon_type, "LocalPoint");

         if (strcmp(info->m_fix_rs, info->m_description_rs) == 0)
            strcpy(info->m_icon_description, info->m_description_rs);
         else
         {
            if (strlen(info->m_fix_rs) + strlen(info->m_description_rs) < 48)
            {
               sprintf(info->m_icon_description, "%s,  %s",
                  info->m_fix_rs, info->m_description_rs);
            }
            else
               strcpy(info->m_icon_description, info->m_description_rs);
         }

         // add the snap to object to the list
         snap_to_list.AddTail(info);

         // at least one hit
         hit = TRUE;
      }
   }  // while

   return hit;
}
// end of do_snap_to


// ----------------------------------------------------------------------------

// Called when the editor is toggled
//
// Note: this may be a good candidate for making a default implementation
//       as the code is exactly the same in Chum and similar to others that
//       could first call the base class and then do their own stuff.
//
//int C_localpnt_ovl::set_edit_on(const MapProj* map, boolean_t edit)
int C_localpnt_ovl::set_edit_on(boolean_t edit)
{
   // Get the current Local Point overlay...
   C_overlay* m_current_overlay = OVL_get_overlay_manager()->get_current_overlay();

   // If there is an overlay currently open, first determine if it is a Local Points overlay.
   // If it is, get the filename then check to see if the file is read-only.
   // If the current Local Points overlay file is read-only, display a message box informing the user that the
   // current overlay is read-only and as such it cannot be edited directly.
   if (m_current_overlay)
   {
      // Get the class name of the current overlay...
      GUID overlayDescGuid = m_current_overlay->get_m_overlayDescGuid();

      if (overlayDescGuid == FVWID_Overlay_Points) // IF the current overlay is a Local Points overlay...
      {
         // Get the current file specification...
         CString fileName = OVL_get_overlay_manager()->GetOverlayDisplayName(m_current_overlay);

         long lReadOnly = 0;
         OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(m_current_overlay);
         if (pFvOverlayPersistence != NULL)
            pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

         // IF the current overlay file is read_only
         // AND the edit flag = 1 (i.e. we are entering the Local Points editor),
         // THEN display a message box and get out without opening the Local Points editor
         if (lReadOnly && edit)
         {
            AfxMessageBox("The Local Points overlay file " + fileName + " cannot be edited because it is Read-Only.  If you wish to edit this file, first save it under a different name.");
            return SUCCESS;
         }
      }
   }

   //if already in the specified edit state, do nothing
   if (m_bEdit == edit)  
      return SUCCESS;

   if (edit)
      set_editor_mode(SELECT);
   else
      close_icon_dlg(); // turn off icon dialog if on (i.e., from ADD mode)

   m_bEdit = edit;  //set edit state

   return SUCCESS;
}
// end of set_edit_on

#ifdef GOV_RELEASE
void C_localpnt_ovl::SaveToFeaturesOptionsEditor(const CString& pathname)
{
   try
   {
      using namespace xplan_package_service;
      IFeatureOptionsEditorPtr editor;
            ILocalPointGroupListPtr groupList;

      CO_CREATE(editor, CLSID_FeatureOptionsEditor);
      CO_CREATE(groupList, CLSID_LocalPointGroupList);

      POSITION pos = m_group_lst.GetHeadPosition();
      while (pos)
      {
         POINT_GROUP *pGroup = m_group_lst.GetNext(pos);
         groupList->AddGroup(_bstr_t(pGroup->name),
            pGroup->search == TRUE ? VARIANT_TRUE : VARIANT_FALSE);
      }
      VARIANT_BOOL show = (get_show_overlay() == TRUE)
         ? VARIANT_TRUE : VARIANT_FALSE;

      editor->Load();
      editor->AddFile(_bstr_t(pathname), show, groupList);
      editor->Save();
   }
   catch (_com_error& e)
   {
      CString msg;
      msg.Format("SaveToFeaturesOptionsEditor failed: %s", e.Description());
      ERR_report(msg);
   }
}
#endif

// saves the points overlay into an access database with the given name
int C_localpnt_ovl::save_as(const CString & pathname, long nSaveFormat)
{
#ifdef GOV_RELEASE
   SaveToFeaturesOptionsEditor(pathname);
#endif

   enum { 
      kSaveFormatUnspecified = 0, 
      kSaveFormatLpsFiles = 1,
      kSaveFormatPointFiles = 2,
      kSaveFormatPointFiles_3_3_1 = 3
   };

   if (nSaveFormat == kSaveFormatUnspecified &&
      (pathname.Find(".lpt") != pathname.GetLength() - 4))
   {
      nSaveFormat = kSaveFormatLpsFiles;
   }
   else if (nSaveFormat == kSaveFormatUnspecified)
   {
      nSaveFormat = kSaveFormatPointFiles;
   }

   // if the "3.3.1 file" type was selected, use the regular Access97 template file...
   CString template_path = PRM_get_registry_string("Main", "HD_DATA");
   switch (nSaveFormat)
   {
   case kSaveFormatLpsFiles:
      template_path += "\\points\\template.lps";
      break;

      // otherwise, use the AccessXP template file... 
   case kSaveFormatPointFiles:
      template_path += "\\points\\templateXP.lpt";
      break;

   case kSaveFormatPointFiles_3_3_1:
      template_path += "\\points\\template.lpt";
      break;
   }

   // check to see if there is already a file opened with the given name.
   C_overlay *overlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Points);
   while (overlay)
   {
      CString currentFileSpec;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         currentFileSpec = (char *)fileSpecification;
      }

      if (overlay != this && currentFileSpec.CompareNoCase(pathname) == 0)
      {
         CString msg;
         msg.Format("There is already a points overlay opened named %s.  "
            "Please choose another name.", currentFileSpec);
         AfxMessageBox(msg);
         return FAILURE;
      }

      overlay = OVL_get_overlay_manager()->get_next_of_type(overlay, FVWID_Overlay_Points);
   }


   CFvwUtil *util = CFvwUtil::get_instance();

   // create database by copying the blank local points database to the given pathname.
   {
      // delete the file to ensure that it does not exist...
      ::DeleteFile(pathname);

      // copy the template file to the filename...
      int result = ::CopyFile(template_path, pathname, FALSE);
      if (!result)
      {
         CString msg;
         msg.Format("Error copying template file: %s to points directory",
            template_path);
         AfxMessageBox(msg);
         result = FAILURE;
      }
   }

   // get the attributes of the new local points file
   DWORD attributes = GetFileAttributes(pathname);
   if (attributes == -1)
   {
      char message[255];

      // get WIN32 error code generated by GetFileAttributes()
      long int error = GetLastError();

      // log file name and error code
      sprintf(message, "Can't get attributes for %s\nGetLastError() == %lx", pathname, error);
      ERR_report(message);
      return FAILURE;
   }

   // check the attributes to see if the file is read-only.
   // if the file is read-only, set it to have write access.
   if (attributes & FILE_ATTRIBUTE_READONLY)
   {
      // Set it's attributes exactly the same, except XOR away the read-only attribute
      if (SetFileAttributes(pathname, attributes ^ FILE_ATTRIBUTE_READONLY) == FALSE)
      {
         char message[255];

         // get WIN32 error code generated by GetFileAttributes()
         long int error = GetLastError();

         // log file name and error code
         sprintf(message, "Can't set attributes for %s\nGetLastError() == %lx", pathname, error);
         ERR_report(message);
         return FAILURE; 
      }

      // reset the read-only flag...
      m_bIsReadOnly = 0;
   }

   CString old_name = OVL_get_overlay_manager()->GetOverlayDisplayName(this);
   m_fileSpecification = pathname;

   // reset the data object so the new name is reflected in the tabular editor's title bar
   if (g_tabular_editor_dlg != NULL)
   {
      g_tabular_editor_dlg->change_tab_name(this, OVL_get_overlay_manager()->GetOverlayDisplayName(this));
      g_tabular_editor_dlg->set_data_object(m_data_object);
   }

   int result = SUCCESS;
   switch (nSaveFormat)
   {
   case kSaveFormatLpsFiles:
      result = save_as_lps(pathname);
      break;

   case kSaveFormatPointFiles:
   case kSaveFormatPointFiles_3_3_1:

      // close the open database
      close_and_delete_dao_objects();

      if (create_and_open_member_database() != SUCCESS)
      {
         ERR_report("Unable to open points database");
         return FAILURE;
      }

      SaveGroupsToFile(CString(pathname));
      SavePointsToFile(CString(pathname));
      SaveLinksToFile(CString(pathname));

      // if we are saving the file in 3.3.1 (Access97) format, do not call SaveFontsToFile() because
      // the Access97 points template file does not contain the FontData and FontLinks tables...
      if (nSaveFormat == kSaveFormatPointFiles)
         SaveFontsToFile(CString(pathname));

      // close the open database
      close_and_delete_dao_objects();
      break;
   }

   if (result == SUCCESS)
      set_modified(FALSE);

   return result;
}

// ----------------------------------------------------------------------------

// NOTE: does not check to see if overlay is modified
int C_localpnt_ovl::save_or_abandon_changes(BOOL bSave)
{
   if (bSave)
   {
      save();
      set_modified(FALSE);
   }

   return SUCCESS;
}

int C_localpnt_ovl::prompt_to_save_or_abandon_changes_then_peform_request(BOOL* cancel /*=NULL*/)
{
   ASSERT(is_modified());  // should not be calling this with unmodified overlay

   UINT message_box_style = cancel ? MB_YESNOCANCEL : MB_YESNO;
   int choice = AfxMessageBox(IDS_LOCAL_POINT_SAVE_CHANGES_PROMPT, message_box_style);

   if (IDYES == choice || IDNO == choice)
   {   
      // save changes if choice = YES, otherwise abandon changes
      save_or_abandon_changes(IDYES == choice);
   }
   else
   {
      ASSERT(cancel);  //should not be able to push the cancel button and get here otherwise

      if (cancel)
         *cancel = TRUE;
   }

   return SUCCESS; //TO DO: return a meaningful result here
}
// end of prompt_to_save_or_abandon_changes_then_peform_request


// ----------------------------------------------------------------------------

//static 
void C_localpnt_ovl::clear_field_info( CDaoFieldInfo &fi)
{
   //TO DO use memset 0 instead for efficiency

   fi.m_strName="";           // Primary
   fi.m_nType=0;              // Primary
   fi.m_lSize=0;              // Primary
   fi.m_lAttributes=0;        // Primary
   fi.m_nOrdinalPosition=0;   // Secondary
   fi.m_bRequired=0;          // Secondary
   fi.m_bAllowZeroLength=0;   // Secondary
   fi.m_lCollatingOrder=0;    // Secondary
   fi.m_strForeignName="";    // Secondary
   fi.m_strSourceField="";    // Secondary
   fi.m_strSourceTable="";    // Secondary
   fi.m_strValidationRule=""; // All
   fi.m_strValidationText=""; // All
   fi.m_strDefaultValue="";   // All
}
// end of clear_field_info


// ----------------------------------------------------------------------------

BOOL C_localpnt_ovl::create_db(CString filespec)
{
   /*
   struct CDaoFieldInfo
   {      
   CString m_strName;           // Primary
   short m_nType;               // Primary
   long m_lSize;                // Primary
   long m_lAttributes;          // Primary
   short m_nOrdinalPosition;    // Secondary
   BOOL m_bRequired;            // Secondary
   BOOL m_bAllowZeroLength;     // Secondary
   long m_lCollatingOrder;      // Secondary
   CString m_strForeignName;    // Secondary
   CString m_strSourceField;    // Secondary
   CString m_strSourceTable;    // Secondary
   CString m_strValidationRule; // All
   CString m_strValidationText; // All
   CString m_strDefaultValue;   // All
   };
   */
   int result = FAILURE;

   // delete the file
   DeleteFile(filespec);  //TO DO: check for error

   CDaoDatabase database;

   try
   {
      database.Create(filespec);

      CDaoFieldInfo fi;

      // -------- Points table -----------
      CDaoTableDef td(&database);
      td.Create("Points");

      //ID
      clear_field_info(fi);
      fi.m_strName="ID";
      fi.m_nType=dbText;
      fi.m_lSize=12;
      fi.m_bRequired = TRUE;
      td.CreateField(fi);

      //Description
      td.CreateField("Description", dbText, 40); 

      //Latitude
      clear_field_info(fi);
      fi.m_strName="Latitude";
      fi.m_nType=dbDouble;
      fi.m_bRequired = TRUE;
      td.CreateField(fi);

      //Longitutde
      clear_field_info(fi);
      fi.m_strName="Longitude";
      fi.m_nType=dbDouble;
      fi.m_bRequired = TRUE;
      td.CreateField(fi);

      //Elevation
      td.CreateField("Elevation", dbInteger, 2); 

      //Elevation Source   
      clear_field_info(fi);
      fi.m_strName="Elevation_Source";
      fi.m_nType=dbText;
      fi.m_lSize=5;
      //fi.m_strValidationRule="\"DAFIF\" Or \"DTED\" Or \"UNK\" Or \"USER\""; 
      //fi.m_strValidationText="Valid values={DAFIF, DTED, UNK, USER}"; 
      fi.m_strDefaultValue="UNK";   
      td.CreateField(fi);

      // Elevation Source Product Name
      clear_field_info(fi);
      fi.m_strName = "ElevationSourceMapHandlerName";
      fi.m_nType = dbText;
      fi.m_lSize = 20;
      //fi.m_strValidationRule = "\"DTED\" Or \"GeoTIFFDEM\" Or \"UNK\""; 
      //fi.m_strValidationText = "Valid values={DTED, GeoTIFFDEM, UNK}"; 
      fi.m_strDefaultValue = "UNK";   
      td.CreateField(fi);

      // Elevation Source Map Series Name
      clear_field_info(fi);
      fi.m_strName = "ElevationSourceMapSeriesName";
      fi.m_nType = dbText;
      fi.m_lSize = 10;
      //fi.m_strValidationRule = "\"1\" Or \"2\" Or \"3\" Or \"4\" Or \"5\" Or \"UNK\""; 
      //fi.m_strValidationText = "Valid values={1, 2, 3, 4, 5, UNK}"; 
      fi.m_strDefaultValue = "UNK";   
      td.CreateField(fi);

      //Pt_Quality
      clear_field_info(fi);
      fi.m_strName="Pt_Quality";
      fi.m_nType=dbText;
      fi.m_lSize=1;
      fi.m_strDefaultValue="0";
      td.CreateField(fi);

      //Area
      td.CreateField("Area", dbText, 1); 

      //Country Code
      td.CreateField("Country_Code", dbText, 2); 

      //Dtd_Id
      td.CreateField("Dtd_ID", dbText, 5); 

      //Horz_Accuracy
      clear_field_info(fi);
      fi.m_strName="Horz_Accuracy";
      fi.m_nType=dbSingle;
      //fi.m_strDefaultValue="-1"; //CREATE() FAILES IN THE RELEASE BUILD IF THIS LINE IS HERE.  
      td.CreateField(fi);

      //Vert Accuracy
      clear_field_info(fi);
      fi.m_strName="Vert_Accuracy";
      fi.m_nType=dbSingle;
      //fi.m_strDefaultValue="-1"; //CREATE() FAILES IN THE RELEASE BUILD IF THIS LINE IS HERE.
      td.CreateField(fi);

      //Link Name
      td.CreateField("Link_Name", dbText, 255); 

      //Icon Name
      td.CreateField("Icon_Name", dbText, 64); 

      //Comment Name
      td.CreateField("Comment", dbText, 80); 

      //Group Name
      clear_field_info(fi);
      fi.m_strName="Group_Name";
      fi.m_nType=dbText;
      fi.m_lSize=64;
      fi.m_bRequired = TRUE;  //note this field is required in this table, but not below
      td.CreateField(fi);

      td.Append();
      // -------- End Points table -----------
      // -------- Group Names Table -----------
      CDaoTableDef td2(&database);
      td2.Create("Group_Names");

      //Group Name
      clear_field_info(fi);
      fi.m_strName="Group_Name";
      fi.m_nType=dbText;
      fi.m_lSize=64;
      td2.CreateField(fi);

      //Select
      td2.CreateField("Search", dbBoolean, 1); 
      td2.Append();
      // -------- End Group Names Table -----------

      //add primary key to Points Table   
      database.Execute("ALTER TABLE Points ADD constraint keyname PRIMARY KEY(ID, Group_Name)");

      //add indexes to Points table
      database.Execute("CREATE INDEX lat_lon_index ON POINTS (LATITUDE, LONGITUDE)");
      database.Execute("CREATE INDEX ID_index ON POINTS (Group_Name)");

      //add primary key to Group_Names Table   
      database.Execute("ALTER TABLE Group_Names ADD constraint keyname PRIMARY KEY(Group_Name)");

      result = SUCCESS;
   }
   catch(CDaoException* e) 
   {
      //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
      CString msg;
      msg = "Exception in create_db: " + e->m_pErrorInfo->m_strDescription;
      ERR_report(msg);
      e->Delete();
   }

   return result;
}
// end of create_db

// ----------------------------------------------------------------------------

//TO DO: Note: this may be a good candidate for a default funtion
//the implementation is the same as chum
void C_localpnt_ovl::display_icon_dlg()
{
   if (CIconDragDlg::get_dlg() == NULL)
   {
      CIconDragDlg* icon_dlg = new CIconDragDlg("Point Icons");
      icon_dlg->set_dlg(icon_dlg);
      icon_dlg->load_dir("localpnt");
      icon_dlg->sort_icons();
   }
   else if (!CIconDragDlg::get_dlg()->IsWindowVisible())
      CIconDragDlg::get_dlg()->ShowWindow(SW_SHOW);
}
// end of display_icon_dlg

// ----------------------------------------------------------------------------

//TO DO: Note: this may be a good candidate for a default funtion
//the implementation is the same as chum

void C_localpnt_ovl::close_icon_dlg()
{
   if (CIconDragDlg::get_dlg())
      CIconDragDlg::get_dlg()->close_dlg();
}
// end of close_icon_dlg

// ----------------------------------------------------------------------------

void C_localpnt_ovl::set_editor_mode(EditorMode mode)
{
   // set flag to draw/not-draw labels based up current map scale

   m_editor_mode = mode;  //store mode 

   //switch to new mode
   switch(mode)
   {
   case SELECT:
      close_icon_dlg();  //close icon dialog (OK to call when already up)
      break;

   case ADD:
      // Send a message to the frame to put up the icon dialog window
      SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, ID_DISPLAY_LOCALPNT_ICON_DLG, 0);
      break;
   }
}
// end of set_editor_mode


// ----------------------------------------------------------------------------

int C_localpnt_ovl::pre_close(BOOL* cancel)
{
   if (cancel)
      *cancel = FALSE;

   //if we are not canceling the close, close and delete dao objects
   if (cancel && !*cancel)
      close_and_delete_dao_objects();

   if (g_tabular_editor_dlg != NULL && g_tabular_editor_dlg->get_data_object() == m_data_object)
      CTabularEditorDlg::destroy_dialog();

   return SUCCESS;
}
// end of pre_close

// ----------------------------------------------------------------------------

void C_localpnt_ovl::unselect_current_selections()
{
   POSITION pos = m_selected_points.GetHeadPosition();
   while (pos)
   {
      C_localpnt_point *point = m_selected_points.GetNext(pos);
      point->invalidate();
   }

   m_selected_points.RemoveAll();
}
// end of unselect_current_selection

void C_localpnt_ovl::remove_point_from_selections(C_localpnt_point *pPoint)
{
   POSITION pos = m_selected_points.Find(pPoint);
   if (pos != NULL)
   {
      pPoint->invalidate();
      m_selected_points.RemoveAt(pos);
   }
}


// ----------------------------------------------------------------------------

void C_localpnt_ovl::unhilight_current_hilight()
{
   if (m_current_hilight)
   {
      m_current_hilight->invalidate();
      m_current_hilight=NULL;
   }
}
// end of unhilight_current_hilight


// ----------------------------------------------------------------------------
//sets the current hilight to point, invalidating the screen as necessary
void C_localpnt_ovl::set_current_hilight(C_localpnt_point* point)
{
   ASSERT(point);

   if (m_current_hilight != point)  //if the point is not already hilighted...
   {
      unhilight_current_hilight();
      m_current_hilight=point;
      if (m_current_hilight)
         m_current_hilight->invalidate();
   }
}
// end of set_current_hilight


// ----------------------------------------------------------------------------
// Adds the current hilight to point, invalidating the screen as necessary
// Note: draws by invalidation (not immediate)
void C_localpnt_ovl::add_current_selection(C_localpnt_point* point)
{
   if (point != NULL && !m_selected_points.Find(point))  //if the point is not already in focus
   {
      m_selected_points.AddTail(point);
      point->invalidate();
   }

   // Note: the following line is local point specific
   if (m_selected_points.GetCount() == 1)
   {
      C_localpnt_point* pnt = m_selected_points.GetHead();
      m_EditDialogControl.SetFocus(pnt->m_id, pnt->m_group_name, get_specification());
   }
   else
      m_EditDialogControl.SetFocus(NULL);

   if (NULL != m_LinkEditDialog)
   {
      if (m_selected_points.GetCount() == 1)
      {
         set_link_edit_dlg_focus(m_selected_points.GetHead());
      }
      else
      {
         set_link_edit_dlg_focus(NULL);
      }
   }

   if (g_tabular_editor_dlg)
      g_tabular_editor_dlg->on_selection_changed();

   // notify overlay element clients that a selection has been made
   //
   if (point != NULL)
   {
      const int nOverlayHandle = OVL_get_overlay_manager()->get_overlay_handle(this);
      std::string strObjectGuid = point->GetObjectGuid();
      const UINT size = m_vecOverlayElementCallbacks.size();
      for(UINT i=0;i<size;++i)
      {
         try
         {
            m_vecOverlayElementCallbacks[i]->OnSelected(nOverlayHandle, _bstr_t(strObjectGuid.c_str()));
         }
         catch(_com_error &e)
         {
            CString msg;
            msg.Format("OnSelected callback failed: %s", (char *)e.Description());
            ERR_report(msg);
         }
      }
   }
}
//end add_current_selection

// ----------------------------------------------------------------------------
//sets the current hilight to point, invalidating the screen as necessary
//Note: draws by invalidation (not immediate)
void C_localpnt_ovl::set_current_selection(C_localpnt_point* point)
{
   unselect_current_selections();
   add_current_selection(point);
}
// end of set_current_selection

// Use GetBestPointElevation() to get the elevation along with the product name and
// map series name of the elevation source used.
void C_localpnt_ovl::get_best_point_elevation(double lat, double lon, int& elevation,
                                              CString& elevation_source,
                                              CString& new_elev_src_map_handler_name,
                                              short& new_elev_src_map_series_id)
{
   IElevationDataPtr pElevationData;
   CO_CREATE(pElevationData, __uuidof(ElevationData));

   _bstr_t bstrProductUsed;
   _bstr_t bstrSeriesUsed;
   MapScaleUnitsEnum eScaleUnitsUsed;
   double dElevation, dScaleUsed;

   dElevation = pElevationData->GetBestPointElevation(lat, lon, ELEV_UNITS_FEET, 
      bstrProductUsed.GetAddress(),
      &dScaleUsed,
      &eScaleUnitsUsed,
      bstrSeriesUsed.GetAddress());

   if (elevation == MISSING_ELEVATION)
   {
      elevation = 0; //set elevation to zero in case some people misinterpret "UNK"
      elevation_source="UNK";
      new_elev_src_map_handler_name = "UNK";
      return;
   }


   // round double elevation to the nearest int...
   elevation = (int)(dElevation + 0.5);

   // set the map handler name string...
   elevation_source = (char*)bstrProductUsed;

   // replace empty string with "UNK"...
   if (elevation_source == "")
      elevation_source = "UNK";

   // make map handler name string all upper case...
   elevation_source.MakeUpper();

   new_elev_src_map_handler_name = elevation_source;

   if (elevation_source == "UNK")
   {
      new_elev_src_map_handler_name = "";
      new_elev_src_map_series_id = 0;
      elevation = 0;
   }

   if (elevation_source == "USER")
   {
      new_elev_src_map_handler_name = "";
      new_elev_src_map_series_id = 0;
   }

   if ((elevation_source == "") || (elevation_source == "USER") || (elevation_source == "UNK"))
      return;

   // Now get the Series ID
   IMapGroupsPtr spMapGroups;
   CO_CREATE(spMapGroups, CLSID_MapGroups);

   if (spMapGroups->SelectPositionOnProductScale(0, bstrProductUsed, dScaleUsed, 
      eScaleUnitsUsed, bstrSeriesUsed) == S_OK)
   {
      new_elev_src_map_handler_name = (char *)spMapGroups->m_MapHandlerName;
      new_elev_src_map_series_id = static_cast<short>(spMapGroups->m_MapSeriesIdentity);

      if (new_elev_src_map_handler_name == "Lidar")
      {
         elevation_source = "DTED";
      }
   }

   if (new_elev_src_map_handler_name == "Dted")
      new_elev_src_map_handler_name = "DTED";
}
// end of get_best_point_elevation()



// ----------------------------------------------------------------------------
// set the "search" flag for a specified group name
int C_localpnt_ovl::set_group_name_search(const CString& group_name, BOOL search)
{
   POSITION position = m_group_lst.GetHeadPosition();
   while (position)
   {
      POINT_GROUP* pGroup = m_group_lst.GetNext(position);
      if (pGroup && pGroup->name.CompareNoCase(group_name) == 0)
      {
         pGroup->search = search;
         return SUCCESS;
      }
   }

   ASSERT(0);  // group name not found
   return SUCCESS;
}



// ----------------------------------------------------------------------------

int C_localpnt_ovl::get_group_name_info(const CString& group_name, BOOL& exists, BOOL& search) const
{
   ASSERT(!group_name.IsEmpty());

   POSITION position = m_group_lst.GetHeadPosition();
   while (position)
   {
      POINT_GROUP *pGroup = m_group_lst.GetNext(position);
      if (pGroup->name.CompareNoCase(group_name) == 0)
      {
         exists = TRUE;
         search = pGroup->search;

         return SUCCESS;
      }
   }

   // if the group name has not been found
   exists = FALSE;
   search = FALSE;

   return SUCCESS;
}


// ----------------------------------------------------------------------------
//does the following:
//1) add new record to database setting required fields from input paramaters
//2) copy record from database (getting defaults) to a new local point
//3) add local point to list in memory
int C_localpnt_ovl::receive_new_point(ViewMapProj* map, CPoint pt, UINT flags, 
                                      C_drag_item* drag_item)
{
   ASSERT(drag_item);

   // check to see if the current overlay is Read-Only.  If it is, display a message box and
   // let's get the hell out of Dodge...
   // Get the current Local Point overlay...
   C_overlay* m_current_overlay = OVL_get_overlay_manager()->get_current_overlay();

   // Get the current file specification...
   CString fileName = OVL_get_overlay_manager()->GetOverlayDisplayName(m_current_overlay);

   // IF the current overlay file is read-only, display a message box and
   // return SUCCESS to cancel addition of the new point...
   if (m_bIsReadOnly)
   {
      AfxMessageBox("The Local Points overlay file " + fileName + " cannot be edited because it is Read-Only.  If you wish to edit this file, first save it under a different name.");
      return SUCCESS;
   }

   ASSERT(drag_item->m_title.GetLength() < 255);
   ASSERT(drag_item->m_title.GetLength() > 0);

   // We need to do the snap-to stuff first, because
   // if the user cancels from a multi-choice snap-to,
   // we would like to exit without having to do lots
   // of cleanup in the database and the overlay
   degrees_t new_latitude, new_longitude;
   int       new_x, new_y;
   SnapToInfo* snap_to_info = NULL;

   // get lat/lon and screen coords
   if (OVL_get_overlay_manager()->test_snap_to(map, pt))
   {
      snap_to_info = new SnapToInfo();
      memset(snap_to_info, 0, sizeof(snap_to_info));

      if (OVL_get_overlay_manager()->do_snap_to(map, pt, snap_to_info))
      {
         new_latitude  = snap_to_info->m_lat;
         new_longitude = snap_to_info->m_lon;
         map->geo_to_surface(new_latitude, new_longitude, &new_x, &new_y);
      }
      else
      {
         if (snap_to_info)
            delete snap_to_info;

         // if we get here, then the user cancelled
         // a multi-choice snap-to dialog, so exit
         return SUCCESS;
      }
   }
   else
   {
      map->surface_to_geo(pt.x, pt.y, &new_latitude, &new_longitude);
      new_x = pt.x;
      new_y = pt.y;
   }

   C_localpnt_point* point = new C_localpnt_point(this);
   ASSERT(point !=NULL);

   if (point == NULL)
   {
      if (snap_to_info)
         delete snap_to_info;

      return FAILURE;
   }


   // Set icon_image from drag item
   // TO DO: do we just want a function to load this from the icon name?
   point->m_icon_image = drag_item->m_item_icon;  //TO DO: rename to something better

   // set lat/lon and screen coords
   point->m_latitude  = (float)new_latitude;
   point->m_longitude = (float)new_longitude;
   point->m_x         = new_x;
   point->m_y         = new_y;

   //set icon name from drag item
   point->SetIconName(drag_item->m_item_icon->get_item_filename());

   CNewLocalPointDefaults new_point_defaults;  //create an object to manage new local point id's/groupnames

   point->m_group_name = new_point_defaults.GetGroupName();

   do
   {
      point->m_id=new_point_defaults.get_and_increment_id();
   }
   while (does_key_exist(point->m_id, point->m_group_name) == TRUE);

   if (insert_point(map, point, snap_to_info) == FAILURE)
   {
      ERR_report("Unable to insert new point");
      return FAILURE;
   }

   // Set icon's bounding box
   // TO DO: kevin: don't hard code size of icon
   point->m_rect.SetRect(pt.x - 16, pt.y - 16, pt.x + 16, pt.y +16);

   //draw the point immediately
   CClientDC dc(map->get_CView());

   //if this overlay is set to draw at this scale, and the list is not empty
   if (we_are_below_the_display_threshold(map)) //TO DO: warning message?
   {
      // set flag to draw/not-draw labels based up current map scale
      BOOL draw_labels = we_are_below_the_label_threshold(map);

      point->draw(map, &dc, 
         m_selected_points.Find(point) && m_bEdit  &&  get_current(),
         point == m_current_hilight, draw_labels, m_pPointOverlayFonts, FALSE);
   }

   //
   // When we drop a new point, we want to pop up the local point
   // editor dialog if it's not already open, and set the focus
   // to the new point in the editor dialog
   //
   if (!m_EditDialogControl.DialogExists())
      m_EditDialogControl.Create();

   m_EditDialogControl.SetFocus(point->m_id, point->m_group_name, get_specification());
   m_EditDialogControl.ShowDialog();

   // also set the focus in the Link Edit dialog
   set_link_edit_dlg_focus(point);

   return SUCCESS;
}
// end of receive_new_point


// ----------------------------------------------------------------------------
void C_localpnt_ovl::set_importing(bool isImporting)
{
   m_isImporting = isImporting;
   if (m_isImporting)
   {
      m_allSelected = false;
   }
}


int C_localpnt_ovl::insert_point(ViewMapProj* map, C_localpnt_point *point, SnapToInfo* snap_to_info,
                                 BOOL bSetModified /*= TRUE*/, BOOL bMapElevationLookup /*= TRUE */,
                                 bool bNotifyClients /* = true */, bool bCallUpdateData /* ignored */)
{
   // it's actually a set of records, but since it always represents a SINGLE
   // local point, it is always of size one, so we can treat it as a record
   CLocalPointSet point_record(m_database);

   //if the group name does not exist, add it to the table
   //if the group name, is not displayed, prompt the user whether or not to display it
   BOOL exists;
   BOOL search;
   if (get_group_name_info(point->m_group_name, exists, search) == SUCCESS)
   {
      //TO DO: check return value
      if (!exists)
         add_to_group_name_table(point->m_group_name);  //add to table, search = TRUE by default
      else
         if (!search)
         {

            CString msg;
            AfxFormatString1 (msg, IDS_GROUP_NAME_OFF_PROMPT_ON, point->m_group_name);
            if (AfxMessageBox(msg, MB_YESNO) == IDYES)
            {
               if (set_group_name_search(point->m_group_name, TRUE) != SUCCESS) //turn group name sarch on
                  ERR_report("set_group_name_search() for " + point->m_group_name + " failed.");

               if (map)
                  fill_local_point_list_from_database(map);
               OVL_get_overlay_manager()->InvalidateOverlay(this);
            }   
         }
   }
   else
      ERR_report("get_group_name_info() for " + point->m_group_name + " failed");


   //check that ID is not too long
   //TO DO: Kevin: put in string table
   if (point->m_id.GetLength() > 12)
   {
      AfxMessageBox("The point ID cannot be greater than 12 characters.  You must change the default ID in the Point options.");

      if (snap_to_info)
         delete snap_to_info;

      return FAILURE;
   }

   int     new_elevation;
   CString new_elevation_src;
   CString new_elev_src_map_handler_name;
   short new_elev_src_map_series_id;
   bool    use_new_elev_data = TRUE;

   if (bMapElevationLookup)
   {
      get_best_point_elevation(point->m_latitude, point->m_longitude,
         new_elevation, new_elevation_src,
         new_elev_src_map_handler_name, new_elev_src_map_series_id);
   }
   else
   {
      new_elevation = point->m_elv;
      new_elevation_src = point->m_elv_src;
      new_elev_src_map_handler_name = point->m_elev_src_map_handler_name;
      new_elev_src_map_series_id = point->m_elev_src_map_series_id;
   }

   // if DTED is not available and the user manually entered an
   // elevation for this point already, ask them if they want to
   // keep the elevation or not
   if ("UNK"  == new_elevation_src && "USER" == point->m_elv_src && snap_to_info == NULL)
   {
      if (m_isImporting)
      {
         if (!m_allSelected)
         {
            CElevationDlg dlg;
            dlg.DoModal();
            m_elevationResult = dlg.m_result;
            if ((m_elevationResult == YES) || (m_elevationResult == YES_ALL))
               use_new_elev_data = FALSE;
            if ((m_elevationResult == YES_ALL) || (m_elevationResult == NO_ALL))
               m_allSelected = true;
         }
         else
         {
            if ((m_elevationResult == YES) || (m_elevationResult == YES_ALL))
               use_new_elev_data = FALSE;
         }
      }
      else
      {
      CString msg;
      msg  = "An elevation was manually entered for this Point.\n\n";
      msg += "Do you wish to keep the elevation that was entered manually?";

      if (IDYES == AfxMessageBox(msg, MB_YESNO))
         use_new_elev_data = FALSE;
   }
   }

   // if DTED is available for the point, or if the user selected
   // NO in the message box, fill in the elevation
   if (new_elevation_src != "UNK" && use_new_elev_data)
   {
      point->m_elv = new_elevation;
      point->m_elv_src = new_elevation_src;
      point->m_elev_src_map_handler_name = new_elev_src_map_handler_name;
      point->m_elev_src_map_series_id = new_elev_src_map_series_id;
   }

   if (snap_to_info != NULL)
   {
      set_snap_to_info(point, snap_to_info);
   }

   // add this to our chain of CIcon's
   m_data_object->AddTail(point);

   if (bNotifyClients)
   {
      const bool bDeleteElement = false;
      OnElementModified(point, bDeleteElement);
   }

   // unselect the current selection and set focus to this one
   unselect_current_selections();

   set_current_selection(point);

   // we modified the overlay
   if (bSetModified)
      set_modified(TRUE);  

   if (snap_to_info)
      delete snap_to_info;

   return SUCCESS; 
}
// end of insert_point


// ----------------------------------------------------------------------------
// called when the info box focus has changed
void C_localpnt_ovl::release_focus()
{
   unhilight_current_hilight();
}
// end of release_focus


// ----------------------------------------------------------------------------

int C_localpnt_ovl::close_edit_dialog_but_ask_to_save_changes()
{
   ASSERT(0);  //not used, get rid of this function

   // TO DO: implement save changes part

   return m_EditDialogControl.DestroyDialog();   
}
// end of close_edit_dialog_but_ask_to_save_changes


// ----------------------------------------------------------------------------

// *****************************************************************************
// Receives all keydown messeges from the Overlay Manager and filters them for
// this overlay... a return of false will allow the keystroke to be 
// processed normally in MapView. This function will get called only if 
// the File Overlay is current and open in its editor.

boolean_t C_localpnt_ovl::offer_keydown(IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
                                        UINT nFlags, CPoint mouse_pt)
{
   if (!m_bEdit)
      return FALSE;

   int processed = FALSE;  //we have not processed (and eaten) the key

   switch (nChar)
   {
   case VK_DELETE:
      // if this is a repeated key messege, then skip ths notice
      // cause we don't want delete repeats to occur
      if ((nFlags & (1 << 14))  !=  0)
         //repeated key
         processed = TRUE;    
      else
         //first time key
         if (m_selected_points.GetCount() > 0)
         {
            CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

            processed = delete_local_point(&mapProjWrapper, m_selected_points.GetHead()) == SUCCESS;
         }

         break;

   case VK_ESCAPE:
      if (m_in_drag)
      {
         cancel_drag(OVL_get_overlay_manager()->get_view_map());
         return TRUE;
      }
      break;
   }

   return processed;
}
// end of offer_keydown

void C_localpnt_ovl::set_snap_to_info(C_localpnt_point *point, SnapToInfo *snap_to_info)
{
   if (snap_to_info)
   {
      //set horizontal and vertical accuracy
      point->m_horz_accuracy = snap_to_info->m_horz_accuracy;
      point->m_vert_accuracy = snap_to_info->m_vert_accuracy;

      strcpy(point->m_country_code, snap_to_info->m_country_code);
      point->m_area_code = snap_to_info->m_area_code;
      point->m_elv = snap_to_info->m_elv;
      strcpy(point->m_dtd_id, snap_to_info->m_dtd_id);
      point->m_description = CString(snap_to_info->m_description_rs);

      switch (snap_to_info->m_elv_src)
      {
      case SnapToInfo::DAFIF:
         point->m_elv_src = "DAFIF";
         break;
      case SnapToInfo::ELV_UNKNOWN:
         point->m_elv_src = "UNK";
         break;
      case SnapToInfo::USER_DEFINED_ELV:
         point->m_elv_src = "USER";
         break;
      case SnapToInfo::DTED:
         point->m_elv_src = "DTED";
         break;
      default:
         point->m_elv_src = "";
      }

      // Clear the Map Series Product and Map Handler Name
      point->m_elev_src_map_handler_name = "";
      point->m_elev_src_map_series_id = 0;
   }
}

// ----------------------------------------------------------------------------
//WARNING: this can only be called if the id and group_name have not changed
int C_localpnt_ovl::update_point_in_database_because_point_moved(C_localpnt_point *point, SnapToInfo *snap_to_info) 
{
   // modify the entry in the internal data structure if the database
   // does not yet exist
   if (snap_to_info)
   {
      set_snap_to_info(point, snap_to_info);
   }
   else
   {
      int     new_elevation;
      CString new_elevation_src;
      CString new_elev_src_map_handler_name;
      short new_elev_src_map_series_id;
      bool    use_new_elev_data = TRUE;

      get_best_point_elevation(point->m_latitude, point->m_longitude,
         new_elevation, new_elevation_src,
         new_elev_src_map_handler_name, new_elev_src_map_series_id);

      // if DTED is not available and the user manually entered an
      // elevation for this point already, ask them if they want to
      // keep the elevation or not
      if ("UNK"  == new_elevation_src && "USER" == point->m_elv_src)
      {
         CString msg;
         msg  = "An elevation was manually entered for this Point, and\n";
         msg += "the area where it now resides has no available elevation data.\n";
         msg += "\n";
         msg += "Do you wish to keep the elevation that was entered manually?";

         if (IDYES == AfxMessageBox(msg, MB_YESNO))
            use_new_elev_data = FALSE;
      }

      // if DTED is available for the point, or if the user selected
      // NO in the message box, fill in the elevation
      if (use_new_elev_data)
      {
         point->m_elv = new_elevation;
         point->m_elv_src = new_elevation_src;
         point->m_elev_src_map_handler_name = new_elev_src_map_handler_name;
         point->m_elev_src_map_series_id = new_elev_src_map_series_id;
      }

      // set horizontal and vertical accuracy to unknown
      point->m_horz_accuracy = -1;
      point->m_vert_accuracy = -1;

      // set country, point quality, and m_area to their defaults (blank
      strcpy(point->m_country_code, " ");
      point->m_qlt = '0';
      point->m_area_code = ' ';
   }

   return SUCCESS;
}

// ----------------------------------------------------------------------------

void C_localpnt_ovl::set_point_properties_in_db(C_localpnt_point *point, SnapToInfo *snap_to_info,
                                                CLocalPointSet *point_record, BOOL bMapElevationLookup /* = TRUE */)
{
   ASSERT(point);
   ASSERT(point_record);
   ASSERT(point_record->IsOpen());

   // sets ID, group, lat, lon, description, and icon name
   *point_record = *point;

   if (snap_to_info)
   {
      //set horizontal and vertical accuracy
      point_record->m_Horz_Accuracy = snap_to_info->m_horz_accuracy;
      point_record->m_Vert_Accuracy = snap_to_info->m_vert_accuracy;

      point_record->m_Country_Code  = snap_to_info->m_country_code;
      point_record->m_Area          = snap_to_info->m_area_code;
      point_record->m_Elevation     = snap_to_info->m_elv;
      point_record->m_Dtd_Id        = snap_to_info->m_dtd_id;
      point_record->m_Description   = snap_to_info->m_description_rs;

      switch (snap_to_info->m_elv_src)
      {
      case SnapToInfo::DAFIF:
         point_record->m_Elevation_Source = "DAFIF";
         break;
      case SnapToInfo::ELV_UNKNOWN:
         point_record->m_Elevation_Source = "UNK";
         break;
      case SnapToInfo::USER_DEFINED_ELV:
         point_record->m_Elevation_Source = "USER";
         break;
      case SnapToInfo::DTED:
         point_record->m_Elevation_Source = "DTED";
         break;
      default:
         point_record->m_Elevation_Source = "";
      }
   }
   else
   {
      int     new_elevation;
      CString new_elevation_src;
      CString new_elev_src_map_handler_name;
      short new_elev_src_map_series_id;
      bool    use_new_elev_data = TRUE;

      if (bMapElevationLookup)
      {
         get_best_point_elevation(point->m_latitude, point->m_longitude,
            new_elevation, new_elevation_src,
            new_elev_src_map_handler_name, new_elev_src_map_series_id);
      }
      else
      {
         new_elevation = point->m_elv;
         new_elevation_src = point->m_elv_src;
         new_elev_src_map_handler_name = point->m_elev_src_map_handler_name;
         new_elev_src_map_series_id = point->m_elev_src_map_series_id;
      }

      // if DTED is not available and the user manually entered an
      // elevation for this point already, ask them if they want to
      // keep the elevation or not
      if ("UNK"  == new_elevation_src && "USER" == point_record->m_Elevation_Source)
      {
         CString msg;
         msg  = "An elevation was manually entered for this Point, and\n";
         msg += "the area where it now resides has no available elevation data.\n";
         msg += "\n";
         msg += "Do you wish to keep the elevation that was entered manually?";

         if (IDYES == AfxMessageBox(msg, MB_YESNO))
            use_new_elev_data = FALSE;
      }

      // if DTED is available for the point, or if the user selected
      // NO in the message box, fill in the elevation
      if (use_new_elev_data)
      {
         point_record->m_Elevation        = new_elevation;
         point_record->m_Elevation_Source = new_elevation_src;
         point_record->m_Elevation_Source_Map_Handler_Name = new_elev_src_map_handler_name;

         CString series_name;
         series_name.Format("%d", new_elev_src_map_series_id);  
         point_record->m_Elevation_Source_Map_Series_Name = series_name;
      }

      //set horizontal and vertical accuracy to unknown
      point_record->m_Horz_Accuracy=-1;
      point_record->m_Vert_Accuracy=-1;

      //set country, point quality, and m_area to there defaults (blank
      point_record->m_Country_Code=" ";
      point_record->m_Pt_Quality="0";
      point_record->m_Area=" ";
   }

   // Make sure all data are valid
   if (point_record->m_Country_Code.IsEmpty())
      point_record->m_Country_Code = " ";
   if (point_record->m_Pt_Quality.IsEmpty())
      point_record->m_Pt_Quality = " ";
   if (point_record->m_Area.IsEmpty())
      point_record->m_Area = " ";
   if (point_record->m_Dtd_Id.IsEmpty())
      point_record->m_Dtd_Id = " ";
   if (point_record->m_Description.IsEmpty())
      point_record->m_Description = " ";
   if (point_record->m_Comment.IsEmpty())
      point_record->m_Comment = " ";
}
// end of set_point_properties_in_db


// ----------------------------------------------------------------------------

int C_localpnt_ovl::add_to_group_name_table(CString group_name, BOOL search /*= TRUE */,
                                            BOOL update_tabular_editor /*= TRUE*/)
{
   ASSERT(group_name.GetLength() > 0);

   // check for existing
   bool found = false;
   POSITION position = m_group_lst.GetHeadPosition();
   while (position)
   {
      if (m_group_lst.GetNext(position)->name.CompareNoCase(group_name) == 0)
         found = true;
   }

   // add a new entry to the group map if it doesn't already exist
   if (!found)
   {
      POINT_GROUP *pGroup = new POINT_GROUP;
      pGroup->name = group_name;
      pGroup->search = search;
      m_group_lst.AddTail(pGroup);

      // reset the data object since a new group has been added - a 
      // new tab will be added to reflect this
      if (g_tabular_editor_dlg != NULL && update_tabular_editor)
         g_tabular_editor_dlg->set_data_object(m_data_object, TRUE);
   }

   return SUCCESS;
}

// ----------------------------------------------------------------------------
int C_localpnt_ovl::open_edit_dialog_and_set_focus_to_current_selection_if_any()
{
   m_EditDialogControl.Create();

   if (m_selected_points.GetCount() == 1)
   {
      C_localpnt_point *current_selection = m_selected_points.GetHead();
      ASSERT(!current_selection->m_id.IsEmpty());
      ASSERT(!current_selection->m_group_name.IsEmpty());

      m_EditDialogControl.SetFocus(current_selection->m_id, 
         current_selection->m_group_name, 
         get_specification());

      set_link_edit_dlg_focus(current_selection);
   }
   else
   {
      m_EditDialogControl.SetFocus("", "", "");
      set_link_edit_dlg_focus(NULL);
   }

   m_EditDialogControl.ShowDialog();

   // TO DO: meaningful result
   return SUCCESS;
}
// end of open_edit_dialog_and_set_focus_to_current_selection_if_any


// ----------------------------------------------------------------------------

// NOTE: This is very similar to chum. We could send in a pointer to the list
// and make a generic function.
// Called by: C_localpnt_ovl::cycle
C_localpnt_point* C_localpnt_ovl::get_next_in_point_list_cycle(POSITION &pos, int direction)
{
   ASSERT(pos);

   C_localpnt_point *point = NULL;

   // pos is not NULL
   if (direction >0) 
   {
      //forward loop
      if (pos)
         point =  m_data_object->GetNext(pos);
      if (pos == NULL)
         pos = m_data_object->GetHeadPosition();
   }
   else  
   {  
      //backwards loop
      point = m_data_object->GetPrev(pos); 
      if (pos == NULL)
         pos = m_data_object->GetTailPosition();
   }

   return point;
}
// end of get_next_in_point_list_cycle


// ----------------------------------------------------------------------------

//TO DO: comment
void C_localpnt_ovl::cycle(MapProj* map, POSITION pos, int direction /*= 1 */) 
{
   C_localpnt_point *point;
   POSITION initial_pos;

   ASSERT(pos !=NULL);

   initial_pos = pos;
   BOOL found = FALSE;

   if (pos!=NULL)
   {
      //get the point at position "pos", and advance pos to the next item in the list
      point = get_next_in_point_list_cycle(pos, direction);

      BOOL done=FALSE;
      while (!done) //while we haven't gone in a complete circle  
      {
         if (pos==initial_pos) //if we've gotten to the position we started at
            done=TRUE;

         point = get_next_in_point_list_cycle(pos, direction);

         // if this point is in view, we're done
         if (point->in_view(map))
         {
            found = TRUE;
            break;
         }
      }
   }

   if (!found)
      point=NULL;

   //set current selection to point OK for point to be NULL
   set_current_selection(point);
}
// end of cycle


// ----------------------------------------------------------------------------

int C_localpnt_ovl::Close(BOOL &cancel)
{
   ASSERT(this);

   // create the close list
   CList<C_overlay *, C_overlay *> close_list;
   close_list.AddHead(this);

   // request that the overlay manager close this overlay
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager(); 
   ASSERT(ovl_mgr);                                

   return ovl_mgr->close_overlays(close_list, &cancel);
}
// end of Close


// ----------------------------------------------------------------------------

int C_localpnt_ovl::save()
{
   if (create_and_open_member_database() != SUCCESS)
   {
      ERR_report("Unable to open points database");
      return FAILURE;
   }

   SaveGroupsToFile(CString(get_specification()));
   SavePointsToFile(CString(get_specification()));
   SaveLinksToFile(CString(get_specification()));
   SaveFontsToFile(CString(get_specification()));

   // close the open database
   close_and_delete_dao_objects();

   set_modified(FALSE);

   return SUCCESS;
}


// ----------------------------------------------------------------------------
void C_localpnt_ovl::set_link_edit_dlg_focus(C_localpnt_point* point)
{
   if (NULL != m_LinkEditDialog && m_LinkEditDialog->GetHwnd() != NULL)
   {
      if (NULL != point)
      {
         CCommaDelimitedString handle;

         m_links.RemoveAll();
         get_links_for_point(point, m_links);

         handle.add(point->m_id);
         handle.add(point->m_group_name);

         m_LinkEditDialog->set_focus(handle, &m_links);
      }
      else
      {
         CString empty;
         empty.Empty();

         m_LinkEditDialog->set_focus(empty, NULL);
      }
   }
}
// end of set_link_edit_dlg_focus


// ----------------------------------------------------------------------------
void C_localpnt_ovl::static_save_links(CString handle, CStringArray &links)
{
   //get the overlay pointer from the overlay manager and
   //call the non-static variety of this function
   C_localpnt_ovl *overlay = NULL;

   overlay = (C_localpnt_ovl*) OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Points);

   if (overlay)
      overlay->save_links(handle, links);
}
// end of static_save_links


// ----------------------------------------------------------------------------
//TO DO: Edward/Kevin: If this (or any) function is going to throw any exceptions, 
//this needs to be documented up here!
void C_localpnt_ovl::save_links(CString handle, CStringArray &links)
{
   CString id, group;
   CCommaDelimitedString str(handle);

   str.get_first(id);
   str.get_next(group);

   // clear any links in the link list with the given id and group
   POSITION position = m_links_lst.GetTailPosition();
   while (position)
   {
      POSITION currentPos = position;
      POINT_LINK* pLink = m_links_lst.GetPrev(position);

      if (pLink && pLink->id == id && pLink->group_name == group)
      {
         delete pLink;
         m_links_lst.RemoveAt(currentPos);
      }
   }

   // add each of the given links to the link list
   const int nSize = links.GetSize();
   for(int i=0;i<nSize;++i)
   {
      POINT_LINK* pLink = new POINT_LINK;
      pLink->id = id;
      pLink->group_name = group;
      pLink->link_name = links[i];

      m_links_lst.AddTail(pLink);
   }

   set_modified(TRUE);

   /*
   CDaoRecordset         new_links_set(m_database);
   new_links_set.Open(dbOpenTable, "Links", dbAppendOnly);
   new_links_set.AddNew();
   new_links_set.SetFieldValue("Link_Name", (LPCSTR)links[i]);
   new_links_set.SetFieldValue("ID", (LPCSTR)id);
   new_links_set.SetFieldValue("Group_Name", (LPCSTR)group);
   new_links_set.Update();
   */
}
// end of save_links


// ----------------------------------------------------------------------------
void C_localpnt_ovl::get_links_for_point(C_localpnt_point* point, CStringArray &links)
{
   ASSERT(point);
   if (!point)
      return;

   get_links_for_point(point->m_id, point->m_group_name, links);
}
// end of get_links_for_point


// ----------------------------------------------------------------------------

void C_localpnt_ovl::get_links_for_point_unnamed(CString id, CString group_name, CStringArray &links)
{
   POSITION position = m_links_lst.GetHeadPosition();

   while (position)
   {
      POINT_LINK *pLink = m_links_lst.GetNext(position);
      if (pLink->id == id && pLink->group_name == group_name)
         links.Add(pLink->link_name);
   }
}
// end of get_links_for_point_unnamed

void C_localpnt_ovl::AddLink(CString id, CString group_name, CString strLink)
{
   POINT_LINK *pPointLink = new POINT_LINK;
   pPointLink->id = id;
   pPointLink->group_name = group_name;
   pPointLink->link_name = strLink;

   m_links_lst.AddTail(pPointLink);
}

// ----------------------------------------------------------------------------

void C_localpnt_ovl::get_links_for_point(CString id, CString group_name, CStringArray &links)
{
   get_links_for_point_unnamed(id, group_name, links);
   return;
}

void C_localpnt_ovl::convert_localpnt_database()
{
   try
   {
      CDBOpenerCloser dbOpenerCloser(m_database, get_specification());
      CDaoDatabase* database = dbOpenerCloser.GetDatabase();

      // STEP 1: Add the "Links" table to the database
      BOOL need_to_convert_database = FALSE;

      // Test for the existence of the Links table
      CDaoRecordset test_set(database);

      try
      {
         test_set.Open(dbOpenTable, "Links", dbAppendOnly);
      }
      catch (CDaoException* e)
      {
         // the "Links" table was not in the database, so we need to create it
         if (e->m_scode == E_DAO_VtoNameNotFound)
         {
            need_to_convert_database = TRUE;
            e->Delete();
         }
         // rethrow
         else
         {
            CString msg="DAO exception in C_localpnt_ovl::convert_localpnt_database(): ";
            msg += e->m_pErrorInfo->m_strDescription;
            ERR_report(msg);
            e->Delete();
            return;
         }
      }

      if (need_to_convert_database)
      {
         create_links_table("Links", database);

         // STEP 2: Scan the main table for links and add them to the "Links" table
         CDaoRecordset old_links_set(database);
         CDaoRecordset links_set(database);
         CDaoQueryDef  old_links_query(database);
         CString       SQL;

         SQL = "SELECT ALL ID, Group_Name, Link_Name FROM Points WHERE NOT Link_Name = \"\"";
         try
         {
            old_links_query.Create(NULL, SQL);
            old_links_set.Open(&old_links_query);
            links_set.Open(dbOpenTable, "Links", dbAppendOnly);
         }
         catch (CDaoException* e)
         {
            CString msg="DAO exception in C_localpnt_ovl::convert_localpnt_database(): ";
            msg += e->m_pErrorInfo->m_strDescription;
            ERR_report(msg);
            e->Delete();
            return;
         }

         while(!links_set.IsEOF())
         {
            try
            {
               links_set.AddNew();
               links_set.SetFieldValue("ID", old_links_set.GetFieldValue("ID"));
               links_set.SetFieldValue("Group_Name", old_links_set.GetFieldValue("Group_Name"));
               links_set.SetFieldValue("Link_Name", old_links_set.GetFieldValue("Link_Name"));
               links_set.Update();
               old_links_set.MoveNext();
            }
            catch (CDaoException* e)
            {
               CString msg="DAO exception in C_localpnt_ovl::convert_localpnt_database(): ";
               msg += e->m_pErrorInfo->m_strDescription;
               ERR_report(msg);
               e->Delete();
               return;
            }
         }
      }
   }
   catch (CDaoException* e)
   {
      //TO DO: kevin/Edward: 
      // 1) sometimes e->m_pErrorInfo is NULL (for whatever bizarre reason).
      //  Deal with this properly
      // 2) Search for this everywhere else in FV and fix.

      CString msg="DAO exception in C_localpnt_ovl::convert_localpnt_database(): ";
      msg += e->m_pErrorInfo->m_strDescription;
      ERR_report(msg);

      //DO NOT THROW unless going to set up catches for this soewhere in the stack chain of
      //every thing this calls
   }
}
// end of convert_localpnt_database


// ----------------------------------------------------------------------------

int C_localpnt_ovl::create_links_table(CString table_name, CDaoDatabase* database)
{
   CDBOpenerCloser dbOpenerCloser(m_database, get_specification());

   // if no database was passed in, get the one from dbOpenerCloser
   if (!database)
      database = dbOpenerCloser.GetDatabase();

   CDaoFieldInfo field_info;
   CDaoTableDef  table_def(database);
   CString       sql;
   int           result = SUCCESS;

   table_def.Create(table_name);

   //Link_Name
   clear_field_info(field_info);
   field_info.m_strName   = "Link_Name";
   field_info.m_nType     = dbText;
   field_info.m_lSize     = 255;//the value of _MAX_PATH
   field_info.m_bRequired = TRUE;
   table_def.CreateField(field_info);

   //ID
   clear_field_info(field_info);
   field_info.m_strName   = "ID";
   field_info.m_nType     = dbText;
   field_info.m_lSize     = 12;
   field_info.m_bRequired = TRUE;
   table_def.CreateField(field_info);

   //Group Name
   clear_field_info(field_info);
   field_info.m_strName   = "Group_Name";
   field_info.m_nType     = dbText;
   field_info.m_lSize     = 64;
   field_info.m_bRequired = TRUE;
   table_def.CreateField(field_info);

   try
   {
      // save the table in the database
      table_def.Append();

      // create primary key from all three fields
      sql  = "ALTER TABLE ";
      sql += table_name;
      sql += " ADD constraint keyname PRIMARY KEY(Link_Name, ID, Group_Name)";
      database->Execute(sql);
   }
   catch (CDaoException* e)
   {
      CString msg="DAO exception in CDBOpenerCloser: ";
      msg += e->m_pErrorInfo->m_strDescription;
      ERR_report(msg);
      result = FAILURE;
   }

   return result;
}
// end of create_links_table


// ----------------------------------------------------------------------------

void C_localpnt_ovl::delete_links_for_point(CString id, CString group_name)
{
   POSITION position = m_links_lst.GetTailPosition();
   while (position)
   {
      POSITION currentPos = position;
      POINT_LINK* pLink = m_links_lst.GetPrev(position);

      if (pLink && pLink->id == id && pLink->group_name == group_name)
      {
         delete pLink;
         m_links_lst.RemoveAt(currentPos);
      }
   }
}
// end of delete_links_for_point


// ----------------------------------------------------------------------------

// Gets the links associated with the item that's been double-clicked,
// and activates all of them
int C_localpnt_ovl::on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point)
{
   ASSERT(pMapView);
   if (pMapView == NULL) 
      return FAILURE;

   // if the left buttion is the only thing pressed
   if (flags == MK_LBUTTON)
   {
      CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

      C_icon* icon_pnt = icon_hit_test(&mapProjWrapper, point);

      if (icon_pnt && icon_pnt->is_kind_of("C_localpnt_point"))
      {
         CStringArray links;

         get_links_for_point((C_localpnt_point*)icon_pnt, links);

         for (int i = 0; i < links.GetSize(); i++)
            CLinksEditDlg::view_link(links[i]);

         return SUCCESS;
      }
   }

   return FAILURE;
}
// end of on_dbl_click


// ----------------------------------------------------------------------------

void C_localpnt_ovl::set_reload_on_next_draw()
{
   m_reload_on_next_draw = TRUE;
}
// end of set_reload_on_next_draw


// ----------------------------------------------------------------------------

bool C_localpnt_ovl::is_point_in_drag(C_localpnt_point* local_point)
{
   ASSERT(local_point);

   if (NULL == local_point || !m_in_drag)
      return FALSE;

   return (m_ID_in_drag         == local_point->m_id &&
      m_group_name_in_drag == local_point->m_group_name);
}
// end of is_point_in_drag


// ----------------------------------------------------------------------------

void C_localpnt_ovl::store_drag_info(C_localpnt_point* local_point)
{
   ASSERT(local_point);

   if (NULL == local_point)
   {
      m_image_in_drag = NULL;
      m_ID_in_drag.Empty();
      m_group_name_in_drag.Empty();
   }
   else
   {
      ASSERT(local_point->m_icon_image);
      m_image_in_drag = local_point->m_icon_image;
      m_ID_in_drag = local_point->m_id;
      m_group_name_in_drag = local_point->m_group_name;
   }
}
// end of store_drag_info


// ----------------------------------------------------------------------------

// create a new instance of this type of point filter. 
CPointOverlayFilter *C_localpnt_ovl::create_point_overlay_filter(const CString& registry_key)
{
   return new CLocalPointFilter("Local Points", registry_key);
}
// end of create_point_overlay_filter


// ----------------------------------------------------------------------------

// get the current filter, as a string, for this overlay
// for local points, this is a list of groups that are currently on
CString C_localpnt_ovl::get_filter_str()
{
   CString result("\"");

   POSITION position = m_group_lst.GetHeadPosition();
   while (position)
   {
      POINT_GROUP* pGroup = m_group_lst.GetNext(position);
      if (pGroup->search)
      {
         if (result != "\"")
            result += CString(",");

         result += pGroup->name;
      }

   }
   result += CString("\"");

   return result;
}

// ----------------------------------------------------------------------------

// returns true if setting the current properties from the given filter will
// change something
bool C_localpnt_ovl::properties_differ(CString filter)
{
   return get_filter_str() != filter;
}
// end of properties_differ


// ----------------------------------------------------------------------------

// set the current properties of this overlay from the filter
void C_localpnt_ovl::set_properties_from_filter(CString filter)
{
   POSITION position = m_group_lst.GetHeadPosition(); 
   while (position)
   {
      POINT_GROUP* pGroup = m_group_lst.GetNext(position);

      if ( (pGroup->search && filter.Find(pGroup->name) == -1) ||
         (!pGroup->search && filter.Find(pGroup->name) != -1) )
      {
         pGroup->search = !pGroup->search;
      }
   }
}
// end of set_properties_from_filter


// ----------------------------------------------------------------------------

TabularEditorDataObject* C_localpnt_ovl::GetTabularEditorDataObject()
{
   return m_data_object;
}

// toggle the tabular editor for this overlay
void C_localpnt_ovl::OnTabularEditor()
{
   if (g_tabular_editor_dlg != NULL)
      CTabularEditorDlg::destroy_dialog();

   else
      CTabularEditorDlg::create_dialog(m_data_object);
}
// end of OnTabularEditor


// ----------------------------------------------------------------------------

int C_localpnt_ovl::apply_changes(C_localpnt_point* point, point_info_t point_info, BOOL redraw)
{
   return apply_changes(point, point_info.icon, point_info.comment, point_info.dtd_id,
      point_info.description, point_info.id, point_info.group_name, point_info.latitude,
      point_info.longitude, point_info.altitude_meters, point_info.elevation_source,
      point_info.elevation_source_map_handler_name, point_info.elevation_source_map_series_id,
      point_info.elevation, point_info.vert_acc, point_info.horz_acc, redraw);
}

int C_localpnt_ovl::apply_changes(C_localpnt_point* point, CString icon,
                                  CString comment, CString dtd_id, CString description, CString id, 
                                  CString group_name, double latitude, double longitude,
                                  double altitude_meters, CString elevation_source,
                                  CString elevation_source_map_handler_name, short elevation_source_map_series_id,
                                  int elevation, float vert_accuracy, float horz_accuracy, BOOL redraw, bool bNotifyClients /* = true */)
{
   int result = SUCCESS;

   //////// BEGIN APPLICATION OF CHANGES TO OVERLAY IN MEMORY /////////////////////////
   //make changes to overlay in memory

   // if the results of the database operation are successful OR of the db is read-only, then 
   if (point)
   {
      //if point move and redraw requested, invalidate old position
      if (redraw && (point->m_id != id  ||  m_latitude != point->m_latitude  ||  
         longitude != point->m_longitude))
         point->invalidate(FALSE); // never recalc label before changeing label below in copy_changes_back_to...()

      // update point in memory and database
      {
         BOOL exists;
         BOOL search;
         if (get_group_name_info(group_name, exists, search) != SUCCESS)
         {
            return FAILURE;
         }

         //TO DO: check return value
         if (!exists)
            add_to_group_name_table(group_name);  //add to table, search = TRUE by default

         // [Bug 4648] Preserve the font of a point even if the id or group name changes
         if (m_pPointOverlayFonts != NULL && m_pPointOverlayFonts->DoesPointFontExist(std::string(point->m_id), std::string(point->m_group_name)))
         {
            OvlFont& font = GetFont(std::string(point->m_id), std::string(point->m_group_name));
            m_pPointOverlayFonts->SetPointFont(font, FVPOINT(std::string(id), std::string(group_name)));
         }

         // copy our copy back into the original
         point->SetIconName(icon);
         point->m_comment = comment;
         strncpy(point->m_dtd_id, dtd_id, 9);
         point->m_description = description;
         point->m_id = id;
         point->m_group_name = group_name;
         point->m_latitude = latitude;
         point->m_longitude = longitude;
         point->m_altitude_meters = altitude_meters;
         point->m_elv = elevation;
         point->m_elv_src = elevation_source;
         point->m_elev_src_map_handler_name = elevation_source_map_handler_name;
         point->m_elev_src_map_series_id = elevation_source_map_series_id;
         point->m_vert_accuracy = vert_accuracy;
         point->m_horz_accuracy = horz_accuracy;

         CView* active_non_printing_view = UTL_get_active_non_printing_view();
         MapProj* map = UTL_get_current_view_map(active_non_printing_view);
         map->geo_to_surface(latitude, longitude, &(point->m_x), &(point->m_y));

         if (bNotifyClients)
         {
            const bool bDeleteElement = false;
            OnElementModified(point, bDeleteElement);
         }
      }

      //update info box if we're updating the point in it
      if (get_current_hilight() == point)
         show_info(point);

      //if point move and redraw requested, invalidate new position
      if (redraw && (point->m_id != m_id || m_latitude != point->m_latitude  
         || m_longitude != point->m_longitude))
         point->invalidate(point->m_id != m_id); // invalidate with text box recalc if necessary

      Update3DPoint(point);
   }

   //set modified flag to true
   set_modified(TRUE);

   //////// END APPLICATION OF CHANGES TO OVERLAY IN MEMORY ///////////////////////////

   return result;
}
// end of apply_changes


// ----------------------------------------------------------------------------

int C_localpnt_ovl::update_point_record(C_localpnt_point* point, CString icon, CString comment, CString dtd_id, 
                                        CString description, CString id, CString group_name, double latitude, double longitude,
                                        CString elevation_source,
                                        CString elevation_source_map_handler_name, short elevation_source_map_series_id,
                                        int elevation, float vert_accuracy, float horz_accuracy)
{
   // do not allow blank ids or group names
   if (id.IsEmpty() || group_name.IsEmpty())
   {
      AfxMessageBox(IDS_USER_SPECIFIED_LOCAL_POINT_WITH_BLANK_ID_OR_GROUPNAME);
      return FAILURE;
   }

   BOOL modified = (point->m_id.CompareNoCase(id) || point->m_group_name.CompareNoCase(group_name));

   int result = FAILURE;

   if (modified)
   {
      //if the primary key has changed, and the new primary key (id & group name) already exists,
      //then fail, and put up a mesage box
      if(does_key_exist(id, group_name))
      {
         AfxMessageBox(IDS_USER_SPECIFIED_LOCAL_POINT_WITH_KEY_VIOLATION);
         return FAILURE;
      }
   }

   point->SetIconName(icon);
   point->m_comment = comment; 

   strcpy(point->m_dtd_id, dtd_id);

   point->m_horz_accuracy = horz_accuracy;
   point->m_vert_accuracy = vert_accuracy;

   // set.m_Link_Name = m_link_name;

   point->m_id = id;
   point->m_group_name = group_name;
   point->m_description = description;

   point->m_elv = m_elevation;
   point->m_elv_src = m_elevation_source;

   point->m_elev_src_map_handler_name = elevation_source_map_handler_name;
   point->m_elev_src_map_series_id = elevation_source_map_series_id;

   point->m_latitude = (float)latitude;
   point->m_longitude = (float)longitude;

   return SUCCESS;
}

// ----------------------------------------------------------------------------

int C_localpnt_ovl::lookup_info(C_localpnt_point *point, point_info_t *point_info)
{
   return lookup_info(point, point_info->id, point_info->group_name, point_info->description,
      point_info->dtd_id, point_info->horz_acc, point_info->vert_acc,
      point_info->elevation, point_info->elevation_source,
      point_info->elevation_source_map_handler_name, point_info->elevation_source_map_series_id,
      point_info->latitude, point_info->longitude, point_info->altitude_meters, point_info->comment, point_info->icon,
      point_info->country_code, point_info->area_code, point_info->qlt);
}

int C_localpnt_ovl::lookup_info(C_localpnt_point *pt, CString& id, 
                                CString& group_name, CString &description, CString &dtd_id, 
                                float &horz_accuracy, float &vert_accuracy, int &elevation, CString &elevation_source,
                                CString &elevation_source_map_handler_name, short &elevation_source_map_series_id,
                                double &latitude, double &longitude, double& altitude_meters, CString &comment, CString &icon,
                                CString &country_code, char &area_code, char &qlt)
{
   id = pt->m_id;
   group_name = pt->m_group_name;

   description = pt->m_description;

   dtd_id = pt->m_dtd_id;
   horz_accuracy = pt->m_horz_accuracy;
   vert_accuracy = pt->m_vert_accuracy;

   elevation = pt->m_elv;
   elevation_source = pt->m_elv_src;

   elevation_source_map_handler_name = pt->m_elev_src_map_handler_name;
   elevation_source_map_series_id = pt->m_elev_src_map_series_id;

   latitude = pt->m_latitude;
   longitude = pt->m_longitude;
   altitude_meters = pt->m_altitude_meters;

   // m_link_name = set.m_Link_Name;

   comment = pt->m_comment;
   icon = pt->GetIconName();

   country_code = CString(pt->m_country_code);
   area_code = pt->m_area_code;
   qlt = pt->m_qlt;

   id.TrimRight();
   group_name.TrimRight();
   description.TrimRight();
   dtd_id.TrimRight();
   comment.TrimRight();

   icon.TrimRight();  //otherwise initial_icon might be " " and m_icon gets changed to "" in DDX 

   return SUCCESS;
}
// end of lookup_info


// ----------------------------------------------------------------------------

// obtain the list of group names for the overlay
CString C_localpnt_ovl::get_group_list()
{
   CString strGroupList;

   POSITION position = m_group_lst.GetHeadPosition();
   while (position)
   {
      POINT_GROUP* pGroup = m_group_lst.GetNext(position);
      strGroupList += pGroup->name + "\n";
   }

   return strGroupList;
}

// Overlay element overrides
//

HRESULT C_localpnt_ovl::RegisterForCallbacks(VARIANT varDispatch)
{
   // add the dispatch pointer to our list
   IDispatchPtr spDispatch(varDispatch.pdispVal);
   spDispatch.AddRef();
   m_vecOverlayElementCallbacks.push_back(spDispatch);

   return S_OK;
}

// On receiving a new CoT message
HRESULT C_localpnt_ovl::UpdateElement(BSTR strElementXml)
{
   // first, determine if this message adds, modifies, or deletes a point
   //
   CLocalPointsMessage localPointsMsg;

   _bstr_t bstrElementXml(strElementXml);
   std::string elementXml( (char *)bstrElementXml );
   localPointsMsg.CreateFromXML(elementXml);

   // ignore messages from ourselve
   if (localPointsMsg.m_bstrRemarksSource == _bstr_t(m_strOverlayGuid.c_str()))
      return S_OK;

   C_localpnt_point *pPoint = NULL;
   if (m_data_object != NULL)
      pPoint = m_data_object->FindByGuid((char *)localPointsMsg.m_bstrEventUID);

   ViewMapProj *pMap = UTL_get_current_view_map();

   // If the point couldn't be found in the list, then add it
   if (pPoint == NULL)
   {
      if ( localPointsMsg.m_dtEventStale == 0 )
         return SUCCESS;

      C_localpnt_point* pNewPoint = new C_localpnt_point(this);
      pNewPoint->InitFromMessage(&localPointsMsg);

      if (pNewPoint->m_group_name == "")
         pNewPoint->m_group_name = "Default";

      pNewPoint->m_icon_image =
         CIconImage::load_images(pNewPoint->GetIconName());

      return insert_point(pMap, pNewPoint, NULL, TRUE, TRUE, false) == SUCCESS ? S_OK : E_FAIL;
   }

   // If the point exists and if the message is to delete it
   if (localPointsMsg.m_dtEventStale == 0)
   {
      return delete_local_point(pMap, pPoint, false) == SUCCESS ? S_OK : E_FAIL;
   }

   // Otherwise, the point exists and needs to be modified with updated information
   //
   C_localpnt_point tmpPoint;
   tmpPoint.InitFromMessage(&localPointsMsg);

   // preserve group name if it doesn't exist in the message
   CString strGroupName = tmpPoint.m_group_name;
   if (strGroupName == "")
      strGroupName = pPoint->m_group_name;

   if (m_EditDialogControl.DialogExists())
      m_EditDialogControl.SetFocus(pPoint);

   return apply_changes(pPoint, tmpPoint.GetIconName(), tmpPoint.m_comment,
      tmpPoint.m_dtd_id, tmpPoint.m_description, tmpPoint.m_id, strGroupName,
      tmpPoint.m_latitude, tmpPoint.m_longitude, 0.0, tmpPoint.m_elv_src,
      tmpPoint.m_elev_src_map_handler_name, tmpPoint.m_elev_src_map_series_id,
      tmpPoint.m_elv, tmpPoint.m_vert_accuracy,
      tmpPoint.m_horz_accuracy, TRUE, false) == SUCCESS ? S_OK : E_FAIL;

}

// Let clients know that an overlay element has changed
void C_localpnt_ovl::OnElementModified(C_localpnt_point *pPoint, bool bDeleteElement)
{
   if (pPoint == NULL)
      return;

   const int nOverlayHandle = OVL_get_overlay_manager()->get_overlay_handle(this);
   const UINT size = m_vecOverlayElementCallbacks.size();

   _bstr_t bstrXml = pPoint->ToXml(bDeleteElement);

   for(UINT i=0;i<size;++i)
   {
      try
      {
         m_vecOverlayElementCallbacks[i]->OnElementModified(nOverlayHandle, bstrXml);
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("OnElementModified callback failed: %s", (char *)e.Description());
         ERR_report(msg);
      }
   }
}

HRESULT C_localpnt_ovl::Reset()
{
   if (m_data_object == NULL)
   {
      ERR_report("Points data object is NULL");
      return E_FAIL;
   }

   m_posEnumerator = m_data_object->GetHeadPosition();
   return S_OK;
}

HRESULT C_localpnt_ovl::MoveNext(long *pbRet)
{
   *pbRet = TRUE;
   if (m_data_object == NULL)
   {
      ERR_report("Points data object is NULL");
      return E_FAIL;
   }

   if (m_posEnumerator == NULL)
      *pbRet = FALSE;
   else
   {
      C_localpnt_point *pPoint = m_data_object->GetNext(m_posEnumerator);
      if (pPoint == NULL)
         *pbRet = FALSE;
      else
         m_strCurrentElementXml = (char *)pPoint->ToXml(false);
   }

   return S_OK;
}

BSTR C_localpnt_ovl::bstrCurrentXml()
{
   return _bstr_t(m_strCurrentElementXml).Detach();
}

STDMETHODIMP C_localpnt_ovl::get_PointType(long* pPointType)
{
   *pPointType = POINT_EXPORT_SRC_LOCAL; 
   return S_OK;
}

// Return a filter string representing the current overlay's properties
STDMETHODIMP C_localpnt_ovl::raw_GetFilterFromProperties(BSTR* pbstrFilter)
{
   *pbstrFilter = _bstr_t(get_filter_str()).Detach();
   return S_OK;
}

// Set an overlay's properties based on the given filter string
STDMETHODIMP C_localpnt_ovl::raw_SetPropertiesFromFilter(BSTR bstrFilter)
{
   CString filter = (char *)_bstr_t(bstrFilter);
   set_properties_from_filter(filter);
   return S_OK;
}

// enumeration of exportable points
//
STDMETHODIMP C_localpnt_ovl::raw_SelectPointsInRectangle(double dLowerLeftLat, double dLowerLeftLon, 
                                                         double dUpperRightLat, double dUpperRightLon, long *pbResult)
{
   while (!m_exportedPoints.IsEmpty())
      delete m_exportedPoints.RemoveHead();

   d_geo_t ll = { dLowerLeftLat, dLowerLeftLon };
   d_geo_t ur = { dUpperRightLat, dUpperRightLon };
   if (load_points_in_rectangle(NULL, ll, ur, &m_exportedPoints, NULL) == SUCCESS)
   {
      m_crntExportedPointsPos = m_exportedPoints.GetHeadPosition();
      *pbResult = m_crntExportedPointsPos != NULL ? TRUE : FALSE;
   }
   else
      *pbResult = FALSE;

   return S_OK;
}

STDMETHODIMP C_localpnt_ovl::raw_MoveNext(long *pbResult)
{
   m_exportedPoints.GetNext(m_crntExportedPointsPos);
   *pbResult = m_crntExportedPointsPos != NULL ? TRUE : FALSE;
   return S_OK;
}

STDMETHODIMP C_localpnt_ovl::get_Latitude(double* pdLatitude)
{
   if (m_crntExportedPointsPos != NULL)
      m_exportedPoints.GetAt(m_crntExportedPointsPos)->get_lat(*pdLatitude);
   else
      *pdLatitude = 0.0;

   return S_OK;
}

STDMETHODIMP C_localpnt_ovl::get_Longitude(double* pdLongitude)
{
   if (m_crntExportedPointsPos != NULL)
      m_exportedPoints.GetAt(m_crntExportedPointsPos)->get_lon(*pdLongitude);
   else
      *pdLongitude = 0.0;

   return S_OK;
}

STDMETHODIMP C_localpnt_ovl::get_PointKey(BSTR* pbstrPointKey)
{
   if (m_crntExportedPointsPos != NULL)
   {
      *pbstrPointKey = _bstr_t(m_exportedPoints.GetAt(m_crntExportedPointsPos)->get_key()).Detach();
   }
   else
      *pbstrPointKey = _bstr_t(L"").Detach();

   return S_OK;
}

FalconViewOverlayLib::IWorkItemsPtr C_localpnt_ovl::GetMarshaledWorkItems()
{
   if (m_work_items_stream != nullptr)
   {
      // Marshal work items stream to this thread
      THROW_IF_NOT_OK(::CoGetInterfaceAndReleaseStream(
         m_work_items_stream, FalconViewOverlayLib::IID_IWorkItems,
         reinterpret_cast<void**>(&m_marshaled_work_items)));
      m_work_items_stream = nullptr;
   }

   return m_marshaled_work_items;
}

void C_localpnt_ovl::Add3DPoint(C_localpnt_point* point)
{
   auto work_items = GetMarshaledWorkItems();
   if (work_items == nullptr)
      return;

   // Queue up a work item to update the point in a background thread
   CComObject<GenericWorkItem>* work_item = GenericWorkItem::CreateWorkItem(
      [point, this](
         FalconViewOverlayLib::IDisplayElements* display_elements)
   {
      points_overlay_utils::Add3DPoint(display_elements, point,
         GetFont(std::string(point->m_id),
         std::string(point->m_group_name)));
   });
   work_items->QueueWorkItem(work_item);
   work_item->Release();
}

void C_localpnt_ovl::Update3DPoint(C_localpnt_point* point)
{
   auto work_items = GetMarshaledWorkItems();
   if (work_items == nullptr)
      return;

   // Queue up a work item to update the point in a background thread
   CComObject<GenericWorkItem>* work_item = GenericWorkItem::CreateWorkItem(
      [point, this](FalconViewOverlayLib::IDisplayElements* display_elements)
   {
      points_overlay_utils::Update3DPoint(display_elements, point,
         GetFont((LPCTSTR)point->m_id, (LPCTSTR)point->m_group_name));
   });
   work_items->QueueWorkItem(work_item);
   work_item->Release();
}

void C_localpnt_ovl::Remove3DPoint(C_localpnt_point* point)
{
   auto work_items = GetMarshaledWorkItems();
   if (work_items == nullptr)
      return;

   // Queue up a work item to update the point in a background thread
   long handle = point->m_handle;
   CComObject<GenericWorkItem>* work_item = GenericWorkItem::CreateWorkItem(
      [handle](FalconViewOverlayLib::IDisplayElements* display_elements)
   {
      display_elements->DeleteObject(handle);
   });
   work_items->QueueWorkItem(work_item);
   work_item->Release();
}

void points_overlay_utils::Add3DPoint(
   FalconViewOverlayLib::IDisplayElements* display_elements,
   C_localpnt_point* point, OvlFont& point_font)
{
   _bstr_t icon_name = PRM_get_registry_string("Main", "ReadOnlyAppData", "");
   icon_name += L"\\icons\\";
   icon_name += _bstr_t(point->GetIconName());

   CString name;
   int size, attributes;
   COLORREF fg_color, bg_color;
   int bg_type;
   point_font.get_font(name, size, attributes);
   point_font.get_foreground_RGB(fg_color);
   point_font.get_background_RGB(bg_type, bg_color);

   // m_map_display_elements->SetFont(name, size, attributes, fg_color,
   //    bg_color, bg_type);

   long image_handle = display_elements->CreateImageFromFilename(
      _bstr_t(icon_name));

   point->m_handle = display_elements->AddImage(image_handle,
      point->m_latitude, point->m_longitude, point->m_altitude_meters,
      1.0, 0.0, _bstr_t(point->m_id));
}

void points_overlay_utils::Update3DPoint(
   FalconViewOverlayLib::IDisplayElements* display_elements,
   C_localpnt_point* point, OvlFont& point_font)
{
   display_elements->ModifyPosition(point->m_handle,
      point->m_latitude, point->m_longitude, point->m_altitude_meters);

   // TODO: update font
}