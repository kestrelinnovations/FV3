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



// lp_pp.cpp : implementation file
//

#include "stdafx.h"

#include <algorithm>

#include "include/points/GroupEnumerator.h"
#include "include/points/PointsDataView.h"
#include "common.h"
#include "..\resource.h"
#include "factory.h"
#include "lp_pp.h"
#include "localpnt.h"
#include "lpgrpnam.h"
#include "FalconView/include/param.h"
#include "err.h"
#include "..\multisel.h"
#include "..\FontDlg.h"

#include "FalconView/include/maps.h"     // for MAP_get_scale_from_string
#include "FalconView/include/wm_user.h"  // for ID_TREE_CHECKED
#include "FalconView/include/ovl_mgr.h"

#include "FalconView/localpnt/GroupNamesSearchField.h"

/////////////////////////////////////////////////////////////////////////////
// CLocalPointsPage property page

// Used for per-group / per-overlay font callback
C_localpnt_ovl *g_pOverlay = NULL;
CString g_strGroupName;

bool CLocalPointsPage::s_default_database_failed = false;

IMPLEMENT_DYNCREATE(CLocalPointsPage, CPropertyPage)

CLocalPointsPage::CLocalPointsPage() : CPropertyPage(CLocalPointsPage::IDD)
{
   //{{AFX_DATA_INIT(CLocalPointsPage)
   m_label_threshold = _T("");
   m_display_threshold = _T("");

   m_database_filename = _T("");
   m_id_prefix = _T("");
   m_default_group_name = _T("");
   //}}AFX_DATA_INIT

   m_tree_ctrl.m_parent = this;
}

CLocalPointsPage::~CLocalPointsPage()
{
}

void CLocalPointsPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CLocalPointsPage)
   DDX_Control(pDX, IDC_LABEL_THRESHOLD, m_display_threshold_control);
   DDX_Control(pDX, IDC_THRESHOLD, m_threshold_control);
   DDX_CBString(pDX, IDC_LABEL_THRESHOLD, m_label_threshold);
   DDX_CBString(pDX, IDC_THRESHOLD, m_display_threshold);

   DDX_Control(pDX, IDC_LOCAL_ID_SAMPLE, m_sample_id);
   DDX_Control(pDX, IDC_LOCAL_DEFAULT_GROUP_NAME, m_default_group_combo);
   DDX_Text(pDX, IDC_DATABASE_NAME, m_database_filename);
   DDX_Control(pDX, IDC_DATABASE_NAME, m_DB_filename_CB);
   DDX_Text(pDX, IDC_LOCAL_ID_PREFIX, m_id_prefix);
   DDV_MaxChars(pDX, m_id_prefix, 10);
   DDX_CBString(pDX, IDC_LOCAL_DEFAULT_GROUP_NAME, m_default_group_name);
   DDX_Control(pDX, IDC_TREE1, m_tree_ctrl);
   DDX_Text(pDX, IDC_STATIC_FONT, m_strFontDescription);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalPointsPage, CPropertyPage)
   //{{AFX_MSG_MAP(CLocalPointsPage)
   ON_CBN_SELCHANGE(IDC_LABEL_THRESHOLD, OnModified)
   ON_CBN_SELCHANGE(IDC_THRESHOLD, OnModified)

   ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
   ON_CBN_KILLFOCUS(IDC_DATABASE_NAME, OnKillfocusDatabaseName)
   ON_CBN_SELCHANGE(IDC_DATABASE_NAME, OnKillfocusDatabaseName)
   ON_EN_KILLFOCUS(IDC_LOCAL_ID_PREFIX, OnKillfocusLocalIdPrefix)
   ON_CBN_KILLFOCUS(IDC_LOCAL_DEFAULT_GROUP_NAME, OnKillfocusLocalDefaultGroupName)
   ON_NOTIFY(NM_CLICK, IDC_TREE1, OnClickTree)
   ON_MESSAGE(ID_TREE_CHECKED, OnTreeClicked)
   ON_NOTIFY(TVN_KEYDOWN, IDC_TREE1, OnKeydownTree)

   ON_BN_CLICKED(IDC_OPEN, OnOpen)
   ON_BN_CLICKED(IDC_CLOSE, OnClose)
   ON_BN_CLICKED(IDC_DEFAULT_FONT, OnDefaultFont)
   ON_BN_CLICKED(IDC_FONT, OnFont)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalPointsPage message handlers

//-----------------------------------------------------------------------------
static CString extract_filename(CString& path)
{
   return path.Right(path.GetLength() - path.ReverseFind('\\') - 1);
}

int CLocalPointsPage::fill_group_name_list_box()
{
   // we are also going to fill in the currently active
   // combo box here
   m_DB_filename_CB.ResetContent();

   // fill the tree with a list of opened Points overlays along
   // with the Active Local Point Database and their groups as
   // checkable sub-items
   m_tree_ctrl.DeleteAllItems();

   // ** there is a bug in the microsoft common controls tree control class that
   // forces you to turn off the style and then back on to be able to set
   // the state image index to zero
   m_tree_ctrl.ModifyStyle(TVS_CHECKBOXES, 0);
   m_tree_ctrl.ModifyStyle(0, TVS_CHECKBOXES);

   // obtain a list of overlays from the overlay manager
   C_overlay *active = NULL;
   C_overlay *pCurrentOverlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Points);
   while (pCurrentOverlay)
   {
      CString currentFileSpec;
      OverlayPersistence_Interface *pFvOverlayPersistence =
         dynamic_cast<OverlayPersistence_Interface *>(pCurrentOverlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         currentFileSpec = (char *)fileSpecification;
      }

      if ( currentFileSpec.CompareNoCase(m_database_filename) == 0)
         active = pCurrentOverlay;

      pCurrentOverlay = OVL_get_overlay_manager()->get_next_of_type(pCurrentOverlay, FVWID_Overlay_Points);
   }

   m_default_group_combo.ResetContent();  //clear defalut group name combo box

   std::vector<ParentNodeList> m_node_lst;

   if (!s_default_database_failed)
   {
      ParentNodeList node;
      node.m_DB_filename = m_database_filename;
      node.m_overlay = active;
      m_node_lst.push_back(node);
   }
   m_current_list_box_database_filename = m_database_filename;
   m_initial_database_filename = m_database_filename;

   // loop through the list of overlays looking for points overlays
   pCurrentOverlay = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_Points);
   while (pCurrentOverlay)
   {
      CString DB_pathname;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pCurrentOverlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         DB_pathname = (char *)fileSpecification;
      }

      // we already took care of the active local points database above
      if (DB_pathname.CompareNoCase(m_database_filename) != 0)
      {
         ParentNodeList node;
         node.m_DB_filename = DB_pathname;
         node.m_overlay = pCurrentOverlay;
         m_node_lst.push_back(node);
      }

      pCurrentOverlay = OVL_get_overlay_manager()->get_next_of_type(pCurrentOverlay, FVWID_Overlay_Points);
   }
   std::sort(m_node_lst.begin(), m_node_lst.end());

   for(size_t i=0;i<m_node_lst.size();i++)
   {
      CString DB_pathname = m_node_lst[i].m_DB_filename;
      C_overlay *overlay = m_node_lst[i].m_overlay;
      C_localpnt_ovl* points_overlay = static_cast<C_localpnt_ovl *>(overlay);

      // skip unsaved overlays
      OverlayPersistence_Interface* pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      bool unnamed = false;
      if (pFvOverlayPersistence != NULL)
      {
         long bHasBeenSaved = 0;
         pFvOverlayPersistence->get_m_bHasBeenSaved(&bHasBeenSaved);
         unnamed = bHasBeenSaved == 0;
      }

      // add entry to the tree
      HTREEITEM parent_tree_item = m_tree_ctrl.InsertItem(DB_pathname);

      // remove the checkbox if this is the active local points database and it
      // is not opened (makes no sense to toggle it on/off)
      if (overlay == NULL)
         m_tree_ctrl.SetCheck(parent_tree_item, 0);
      else
      {
         m_tree_ctrl.SetCheck(parent_tree_item,
            points_overlay->get_show_overlay());
      }

      // store a pointer to the items overlay
      m_tree_ctrl.SetItemData(parent_tree_item,
         reinterpret_cast<DWORD_PTR>(points_overlay));

      // make the active local points database highlighted
      CString currentFilespec;
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         currentFilespec = (char *)fileSpecification;
      }

      if (overlay == NULL || currentFilespec.CompareNoCase(m_database_filename) == 0)
         m_tree_ctrl.SetItemState(parent_tree_item, TVIS_BOLD, TVIS_BOLD);

      // add entry to list DB filename combobox if it does not already exist
      // in the list
      if (m_DB_filename_CB.FindStringExact(0, DB_pathname) == CB_ERR && !unnamed)
         m_DB_filename_CB.AddString(DB_pathname);

      // add groups
      if (overlay != NULL)
         fill_tree_with_group_names_unnamed(overlay, parent_tree_item);
      else
         fill_tree_with_group_names(DB_pathname, parent_tree_item);
   }

   if (m_node_lst.size())
   {
      m_tree_ctrl.SelectItem(m_tree_ctrl.GetRootItem());
      UpdateFontDescription();
   }

   m_DB_filename_CB.SelectString(0, m_database_filename);

   return SUCCESS;
}

int CLocalPointsPage::fill_tree_with_group_names_unnamed(C_overlay *overlay, HTREEITEM parent)
{
   C_localpnt_ovl *local = (C_localpnt_ovl *)overlay;

   POSITION position = local->m_group_lst.GetHeadPosition();
   while (position)
   {
      POINT_GROUP *group = local->m_group_lst.GetNext(position);

      HTREEITEM child_item = m_tree_ctrl.InsertItem(group->name, parent);
      m_tree_ctrl.SetCheck(child_item, group->search);
      m_tree_ctrl.SetItemData(child_item, group->search);

      // add name to default group name combo box if it is not
      // already in the list
      if (m_default_group_combo.FindStringExact(0, group->name) == CB_ERR)
         m_default_group_combo.AddString(group->name);
   }

   return SUCCESS;
}

int CLocalPointsPage::fill_tree_with_group_names(CString filename, HTREEITEM parent_tree_item)
{
   // TODO Determine why m_database_filename is used when passed a filename
   if (m_database_filename.IsEmpty())
      return SUCCESS;

   // fill active local point database tree with group names
   int result = FAILURE;
   LPSTR extension = PathFindExtension(filename);
   if (CString(".LPT").CompareNoCase(extension) == 0)
   {
      try
      {
         CDaoDatabase db;
         db.Open(filename, FALSE, TRUE);

         CLocalPointGroupNameSet set(&db);

         CLocalPointGroupNameSet::set_filename(filename);
         set.Open(AFX_DAO_USE_DEFAULT_TYPE, NULL, dbForwardOnly | dbReadOnly);

         while (!set.IsEOF() && !set.IsBOF())
         {
            HTREEITEM child_item = m_tree_ctrl.InsertItem(set.m_Group_Name, parent_tree_item);
            m_tree_ctrl.SetCheck(child_item, set.m_Search);
            m_tree_ctrl.SetItemData(child_item, set.m_Search);

            // add name to default group name combo box if it is not
            // already in the list
            if (m_default_group_combo.FindStringExact(0, set.m_Group_Name) == CB_ERR)
               m_default_group_combo.AddString(set.m_Group_Name);

            set.MoveNext();  //next record
         }
         result = SUCCESS;
      }
      catch (CDaoException* e)
      {
         //AfxMessageBox("CDaoException filling groupname list box");
         ERR_report("CDaoException filling groupname list box");
         e->Delete();
         m_tree_ctrl.DeleteAllItems();
         m_default_group_combo.ResetContent();  //clear combo box
      }
      catch (CMemoryException* e)
      {
         ERR_report("CMemoryException filling groupname list box");
         e->Delete();
         m_tree_ctrl.DeleteAllItems();
         m_default_group_combo.ResetContent();  //clear combo box
      }
   }
   else if (CString(".LPS").CompareNoCase(extension) == 0)
   {
      std::string filename_stdstr(filename);
      points::PointsDataView data_view(filename_stdstr);
      points::GroupEnumerator* groups_enumerator = data_view.SelectAllGroups();
      for (bool success = groups_enumerator->MoveFirst();
         success == true; success = groups_enumerator->MoveNext())
      {
         std::string group_name = groups_enumerator->GetGroupName();
         HTREEITEM child_item = m_tree_ctrl.InsertItem(
            group_name.c_str(), parent_tree_item);
         bool search = groups_enumerator->GetSearch();
         m_tree_ctrl.SetCheck(child_item, search);
         m_tree_ctrl.SetItemData(child_item, search);
         // add name to default group name combo box if it is not
         // already in the list
         if (m_default_group_combo.FindStringExact(0, group_name.c_str()) ==
            CB_ERR)
            m_default_group_combo.AddString(group_name.c_str());
      }
   }

   return result;
}

//-----------------------------------------------------------------------------

int CLocalPointsPage::save_changes_to_group_name_list_box()
{
   ASSERT(!m_current_list_box_database_filename.IsEmpty());

   // loop through each of the points databases and save the changes
   // made to the groups state
   HTREEITEM current_parent = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );

   while (current_parent != NULL)
   {
      // if the item doesn't have any children then there is nothing
      // to do
      if (!m_tree_ctrl.ItemHasChildren(current_parent))
      {
         current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
         continue;
      }

      C_localpnt_ovl *overlay = reinterpret_cast<C_localpnt_ovl *>(
         m_tree_ctrl.GetItemData(current_parent));

      CString currentFileSpec;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(overlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         currentFileSpec = (char *)fileSpecification;
      }

      // if the overlay is NULL then this is the active database
      CString DB_name;
      if (overlay == NULL || currentFileSpec.CompareNoCase(m_database_filename) == 0)
      {
         DB_name = m_database_filename;
      }
      else
      {
         DB_name = currentFileSpec;
      }

      if (overlay != NULL)
         save_changes_hlpr_unnamed(overlay, current_parent);
      else
         save_changes_hlpr_named(DB_name, current_parent);

      // Try to get the next parent item
      current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
   }

   return SUCCESS;
}

int CLocalPointsPage::save_changes_hlpr_unnamed(C_overlay *overlay,
   HTREEITEM parent)
{
   C_localpnt_ovl *local = static_cast<C_localpnt_ovl *>(overlay);

   BOOL checked = m_tree_ctrl.GetCheck(parent);
   local->set_show_overlay(checked);

   // for each of the children
   HTREEITEM child = m_tree_ctrl.GetChildItem(parent);
   while (child != NULL)
   {
      BOOL checked = m_tree_ctrl.GetCheck(child);

      // the group check state was modified
      if (checked != (BOOL)m_tree_ctrl.GetItemData(child))
      {
         //set "initial value" to what we are saving in the database
         m_tree_ctrl.SetItemData(child, checked);

         // get group name
         CString group_name = m_tree_ctrl.GetItemText(child);

         // update the checked state in the overlay's group list
         POSITION position = local->m_group_lst.GetHeadPosition();
         while (position)
         {
            POINT_GROUP *group = local->m_group_lst.GetNext(position);
            if (group->name == group_name)
            {
               group->search = checked;
               break;
            }
         }
      }

      child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
   }

   local->set_modified(TRUE);
   local->set_reload_on_next_draw();

   return SUCCESS;
}

int CLocalPointsPage::save_changes_hlpr_named(CString DB_name, HTREEITEM parent)
{
   CLocalPointGroupNameSet set;
   CLocalPointGroupNameSet::set_filename(DB_name);

   C_localpnt_ovl *overlay = (C_localpnt_ovl *)OVL_get_overlay_manager()->is_file_open(DB_name, FVWID_Overlay_Points);
   if (overlay != NULL)
      overlay->set_reload_on_next_draw();

   LPSTR extension = PathFindExtension(DB_name);
   if (CString(".LPT").CompareNoCase(extension) == 0)
   {
      try
      {
         set.Open();

         // for each of the children
         HTREEITEM child = m_tree_ctrl.GetChildItem(parent);
         while (child != NULL)
         {
            BOOL checked = m_tree_ctrl.GetCheck(child);

            // the group check state was modified
            if (checked != (BOOL)m_tree_ctrl.GetItemData(child))
            {
               //set "initial value" to what we are saving in the database
               m_tree_ctrl.SetItemData(child, checked);

               // get group name
               CString group_name = m_tree_ctrl.GetItemText(child);

               if (set.FindFirst("Group_Name = \"" + group_name + "\""))
               {
                  set.Edit();
                  set.m_Search = checked;
                  set.Update();
               }
            }

            child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
         }

         set.Close();
      }
      catch (CDaoException* e)
      {
         ERR_report(e->m_pErrorInfo->m_strDescription);
         e->Delete();
         if (set.IsOpen())
            set.Close();

      }
      catch (CMemoryException* e)
      {
         ERR_report("CMemoryException updating Group_Names table");
         e->Delete();
         if (set.IsOpen())
            set.Close();
      }
   }
   else if (CString(".LPS").CompareNoCase(extension) == 0)
   {
      GroupNamesSearchField group_names_search_field;

      if (group_names_search_field.Initialize(std::string(DB_name)) == SUCCESS)
      {
         // for each of the children
         HTREEITEM child = m_tree_ctrl.GetChildItem(parent);
         while (child != NULL)
         {
            BOOL checked = m_tree_ctrl.GetCheck(child);

            // the group check state was modified
            if (checked != (BOOL)m_tree_ctrl.GetItemData(child))
            {
               //set "initial value" to what we are saving in the database
               m_tree_ctrl.SetItemData(child, checked);

               // get group name
               CString group_name = m_tree_ctrl.GetItemText(child);

               group_names_search_field.UpdateSearch(std::string(group_name),
                  checked == TRUE);
            }

            child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
         }
      }
   }

   return SUCCESS;
}

//-----------------------------------------------------------------------------

BOOL CLocalPointsPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   //set database filename
   m_database_filename = C_localpnt_ovl::get_local_point_database_filespec();

   // store a copy of the intial databaes filename
   m_initial_database_filename = m_database_filename;

   fill_group_name_list_box();

   //TO DO: make a function to get and set these in the overlay class
   //or make a local point util object
   //TO DO: fix defaults
   // get the existing values for the thresholds
   m_display_threshold= PRM_get_registry_string("Local Points",
      "Display Threshold", "1:2 M");
   m_label_threshold = PRM_get_registry_string("Local Points",
      "Label Threshold", "1:250 K");

   //get default group name and ID prefix from registry
   /*
   CNewLocalPointDefaults defaults;
   m_id_prefix = defaults.GetIDPrefix();
   m_default_group_name = defaults.GetGroupName();
   */

   // get defaults for ...

   CString test = m_new_point_defaults.GetGroupName();
   m_default_group_name = test;
   m_id_prefix = m_new_point_defaults.GetIDPrefix();

   //m_id_sample=m_new_point_defaults.GetID();
   CString sample = m_new_point_defaults.GetID();
   m_sample_id.SetWindowText(sample);

   UpdateData(FALSE);

   //setup next id correclty
   OnKillfocusLocalIdPrefix();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

//-----------------------------------------------------------------------------
//write threshold to registry and invalidate overlay
void CLocalPointsPage::write_thresholds_to_registry()
{


   BOOL dirty = FALSE;

   //TO DO: fix defaults
   //display threshold
   CString initial_display_threshold = PRM_get_registry_string("Local Points",
      "Display Threshold", "1:2 M");

   if (m_display_threshold.Compare(initial_display_threshold) != 0)
   {
      PRM_set_registry_string("Local Points", "Display Threshold",
         m_display_threshold);
      dirty = TRUE;
   }

   //label threshold
   CString initial_label_threshold = PRM_get_registry_string("Local Points",
      "Label Threshold", "1:250 K");

   if (m_label_threshold.Compare(initial_label_threshold) != 0)
   {
      PRM_set_registry_string("Local Points", "Label Threshold",
         m_label_threshold);
      dirty = TRUE;
   }

   //invalidate overlay if dirty
   if (dirty)
   {
      // obtain a list of overlays from the overlay manager - we need
      // to set the threshold for all opened local points overlays
      CList <C_overlay *, C_overlay *> overlay_lst;
      OVL_get_overlay_manager()->get_overlay_list(overlay_lst);

      POSITION position = overlay_lst.GetHeadPosition();
      while (position)
      {
         C_overlay *overlay = overlay_lst.GetNext(position);
         if (overlay->get_m_overlayDescGuid() == FVWID_Overlay_Points)
         {
            C_localpnt_ovl *local = (C_localpnt_ovl *)overlay;
            local->set_display_threshold(MAP_get_scale_from_string(m_display_threshold));
            local->set_label_threshold(MAP_get_scale_from_string(m_label_threshold));
            local->invalidate();
         }
      }
   }
}
//-----------------------------------------------------------------------------

void CLocalPointsPage::OnOK()
{
   // TODO: Add your specialized code here and/or call the base class
   UpdateData(TRUE);

   //TO DO: this kindof sucks because this is called by OnApply()
   CWnd *OK = GetDlgItem(IDOK);
   if (OK && OK != GetFocus())
      OK->SetFocus();

   BOOL refresh_overlay = FALSE;
   BOOL reopen_overlay = FALSE;

   //if the group name filter was modified, save changes, and indicate that we need
   //to refresh the overlay
   if (group_name_list_box_is_modified())
   {
      if (save_changes_to_group_name_list_box() != SUCCESS)
         ERR_report("save_changes_to_group_name_list_box failed");  //TO DO: messagebox?
   }

   //if the database filename was modified save changes, and indicate that we need
   //to close and reopen the overlay
   if (database_filename_is_modified())
   {
      if (C_localpnt_ovl::set_local_point_database_filespec(m_database_filename) != SUCCESS)   //write to registry
          ERR_report("set_local_point_database_name() failed.");

      m_initial_database_filename = m_database_filename;

      reopen_overlay = TRUE; //we need to close and reopen the overlay
   }

   if (global_show_is_modified())
      save_global_show();

   OVL_get_overlay_manager()->invalidate_all(); //TO DO: is this the correct invalidation function?

   //write thresholds to registry and invalidate overlay if they changed
   write_thresholds_to_registry();

   //Note: changes to "New Local Point Defaults" are written to the registry
   //when m_new_point_defaults is destroyed;
   m_new_point_defaults.SetGroupName(m_default_group_name);
   ASSERT(m_id_prefix == m_new_point_defaults.GetIDPrefix()); //this should already be set in on ID prefix changed

   CPropertyPage::OnOK();
}

//-----------------------------------------------------------------------------
BOOL CLocalPointsPage::database_filename_is_modified()
{
   return (m_database_filename != m_initial_database_filename);
}

//-----------------------------------------------------------------------------
BOOL CLocalPointsPage::group_name_list_box_is_modified()
{
   // loop through the child entries of the tree control and look
   // for any modified items (the original item state was stored in the
   // the item data)

   HTREEITEM current_parent = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );

   while (current_parent != NULL)
   {
      C_localpnt_ovl* overlay = reinterpret_cast<C_localpnt_ovl* >(
         m_tree_ctrl.GetItemData(current_parent));

      // If the overlay is currently null but the check is currently true
      // OR the overlay's show state does not match the check state
      BOOL parent_check_state = m_tree_ctrl.GetCheck(current_parent);
      if ((overlay == nullptr && parent_check_state == TRUE) ||
         (overlay != nullptr &&
            overlay->get_show_overlay() != parent_check_state))
      {
         return TRUE;
      }

      if (m_tree_ctrl.ItemHasChildren(current_parent))
      {
         HTREEITEM child = m_tree_ctrl.GetChildItem(current_parent);
         while (child != NULL)
         {
            if (UserChangedCheck(m_tree_ctrl, child))
               return TRUE;

            child = m_tree_ctrl.GetNextItem(child, TVGN_NEXT);
         }
      }

      // Try to get the next item
      current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
   }

   return FALSE;
}

BOOL CLocalPointsPage::save_global_show()
{
   // loop through the child entries of the tree control and look
   // for any modified items

   HTREEITEM current_parent = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );

   while (current_parent != NULL)
   {
      C_localpnt_ovl *overlay = reinterpret_cast<C_localpnt_ovl *>(
         m_tree_ctrl.GetItemData(current_parent));

      // if overlay is NULL, then the item is the active local point database but
      // the file is closed
      if (overlay == NULL)
      {
         C_overlay *overlay;
         if (OVL_get_overlay_manager()->OpenFileOverlay(FVWID_Overlay_Points,
            m_database_filename, overlay) == SUCCESS)
         {
            C_localpnt_ovl* points_overlay = static_cast<C_localpnt_ovl *>(
               overlay);
            m_tree_ctrl.SetItemData(current_parent,
               reinterpret_cast<DWORD_PTR>(points_overlay));
         }
      }

      if (overlay != NULL)
         overlay->set_show_overlay(m_tree_ctrl.GetCheck(current_parent));

      // Try to get the next item
      current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
   }

   return TRUE;
}

BOOL CLocalPointsPage::global_show_is_modified()
{
   // loop through the child entries of the tree control and look
   // for any modified items

   HTREEITEM current_parent = m_tree_ctrl.GetNextItem(NULL, TVGN_CHILD );

   while (current_parent != NULL)
   {
      C_localpnt_ovl *overlay = reinterpret_cast<C_localpnt_ovl *>(
         m_tree_ctrl.GetItemData(current_parent));

      if (overlay == NULL && m_tree_ctrl.GetCheck(current_parent))
         return TRUE;

      if (overlay != NULL && overlay->get_show_overlay() != m_tree_ctrl.GetCheck(current_parent))
         return TRUE;

      // Try to get the next item
      current_parent = m_tree_ctrl.GetNextItem(current_parent, TVGN_NEXT);
   }

   return FALSE;
}

//-----------------------------------------------------------------------------
//NOTE: a very similar if not identical function exists in NewDatabaseDlg
void CLocalPointsPage::OnBrowse()
{
   // set default extension and filter
   static char filter[] =
      "Point Files (*.lps)|*.lps|"
      "Point 4.2.1 Files (*.lpt)|*.lpt|"
      "Point 3.3.1 Files (*.lpt)|*.lpt|"
      "All Files (*.*)|*.*||";

   CString initial_dir =
      PRM_get_registry_string("Main", "ReadWriteUserData", "");
   initial_dir += "\\Points\\*.lps";

   // set up file dialog box so the user can select the name/file...
   CFileDialog dlg(TRUE, NULL, initial_dir, OFN_HIDEREADONLY, filter, NULL);

   // allow the user to select a file...
   if (dlg.DoModal() == IDOK)
   {
      m_database_filename = dlg.GetPathName();
      SetModified(TRUE);
      UpdateData(FALSE);

      if (group_name_list_box_is_modified())
      {
         if (AfxMessageBox(IDS_LOCAL_POINT_GROUP_NAME_FILTER_SAVE, MB_YESNO) == IDYES)
            save_changes_to_group_name_list_box();
      }
      fill_group_name_list_box();
   }
}

//-----------------------------------------------------------------------------

void CLocalPointsPage::OnModified()
{
   SetModified(TRUE);
}


//-----------------------------------------------------------------------------

void CLocalPointsPage::OnKillfocusDatabaseName()
{
   // This method could be reentrant due to the potential AfxMessageBox
   static bool in_kill_focus_database_name = false;
   if (in_kill_focus_database_name)
      return;

   in_kill_focus_database_name = true;

   UpdateData();

   int sel = m_DB_filename_CB.GetCurSel();
   if (sel != CB_ERR)
      m_DB_filename_CB.GetLBText(sel, m_database_filename);

   //set modified true if database filename is modified
   if (database_filename_is_modified())
   {
      SetModified(TRUE);

      //TO DO: make this a function that is called here and in browse button code
      if (group_name_list_box_is_modified())
      {
         if (AfxMessageBox(IDS_LOCAL_POINT_GROUP_NAME_FILTER_SAVE, MB_YESNO) == IDYES)
            save_changes_to_group_name_list_box();
      }

      if (database_filespec_is_valid())
         fill_group_name_list_box();
      else
         OnBrowse();
   }

   in_kill_focus_database_name = false;
}

//-----------------------------------------------------------------------------

void CLocalPointsPage::OnKillfocusLocalIdPrefix()
{
   UpdateData();

   //set new id in New Point Defaults
   m_new_point_defaults.SetIDPrefix(m_id_prefix);
   //change sample id text
   CString sample = m_new_point_defaults.GetID();
   m_sample_id.SetWindowText(sample);
}

//-----------------------------------------------------------------------------

void CLocalPointsPage::OnKillfocusLocalDefaultGroupName()
{
   UpdateData(TRUE);

   //set new group_name in New Point Defaults
   m_new_point_defaults.SetGroupName(m_default_group_name);
   //change sample id text
   CString sample = m_new_point_defaults.GetID();
   m_sample_id.SetWindowText(sample);

}

//-----------------------------------------------------------------------------
//returns TRUE if a local point overlay is open
BOOL CLocalPointsPage::local_point_overlay_is_open()
{
   return OVL_get_overlay_manager()->is_file_open(m_database_filename, FVWID_Overlay_Points) != NULL;
}

//-----------------------------------------------------------------------------
BOOL CLocalPointsPage::database_filespec_is_valid(void)
{
   UpdateData(TRUE);
   int sel = m_DB_filename_CB.GetCurSel();
   if (sel != CB_ERR)
      m_DB_filename_CB.GetLBText(sel, m_database_filename);

   // check for valid database name, ask to create a new one if it doesn't
   // exist, etc
   if (::GetFileAttributes(m_database_filename) == INVALID_FILE_ATTRIBUTES)
   {
      CString msg;
      msg.Format("The specified database file, %s, does not exist or cannot be opened.", m_database_filename);
      AfxMessageBox(msg);
      CWnd *window = GetDlgItem(IDC_DATABASE_NAME);
      if (window)
         window->SetWindowText(m_initial_database_filename);

      return FALSE;
   }

   return TRUE;
}

//-----------------------------------------------------------------------------

BOOL CLocalPointsPage::OnApply()
{
   if (database_filespec_is_valid())
      return CPropertyPage::OnApply();
   else
      return FALSE;
}

void CLocalPointsPage::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
   // this is per Q261289 - the checked state is set after this message
   // so we cannot successfully determine if a group has been changed
   ::PostMessage(m_hWnd, ID_TREE_CHECKED, 0, 0);

   *pResult = 0;
}

void CLocalPointsPage::OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult)
{
   TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

   // this is per Q261289 - the checked state is set after this message
   // so we cannot successfully determine if a group has been changed
   ::PostMessage(m_hWnd, ID_TREE_CHECKED, 0, 0);

   *pResult = 0;
}

LONG CLocalPointsPage::OnTreeClicked(UINT, LONG)
{
   if (group_name_list_box_is_modified() || global_show_is_modified())
      SetModified(TRUE);

   UpdateFontDescription();

   return 0;
}

void CLocalPointsPage::UpdateFontDescription()
{
   m_strFontDescription = "";

   HTREEITEM hItem = m_tree_ctrl.GetSelectedItem();

   if ((hItem != NULL) && m_tree_ctrl.ItemHasChildren(hItem))
   {
      C_localpnt_ovl *pOverlay = reinterpret_cast<C_localpnt_ovl *>(
         m_tree_ctrl.GetItemData(hItem));

      CString currentFileSpec;
      OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);
      if (pFvOverlayPersistence != NULL)
      {
         _bstr_t fileSpecification;
         pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
         currentFileSpec = (char *)fileSpecification;
      }

      CString strDbName = (pOverlay == NULL) ? m_database_filename : currentFileSpec;
      strDbName = strDbName.Right(strDbName.GetLength() - strDbName.ReverseFind('\\') - 1);

      m_strFontDescription.Format("Font for %s:", strDbName);
   }
   else if (hItem != NULL)
   {
      HTREEITEM hParentItem = m_tree_ctrl.GetParentItem(hItem);
      if (hParentItem != NULL)
      {
        C_localpnt_ovl *pOverlay = reinterpret_cast<C_localpnt_ovl *>(
           m_tree_ctrl.GetItemData(hParentItem));

        CString currentFileSpec;
        OverlayPersistence_Interface *pFvOverlayPersistence = dynamic_cast<OverlayPersistence_Interface *>(pOverlay);
        if (pFvOverlayPersistence != NULL)
        {
           _bstr_t fileSpecification;
           pFvOverlayPersistence->get_m_fileSpecification(fileSpecification.GetAddress());
           currentFileSpec = (char *)fileSpecification;
        }

        CString strDbName = (pOverlay == NULL) ? m_database_filename : currentFileSpec;
        strDbName = strDbName.Right(strDbName.GetLength() - strDbName.ReverseFind('\\') - 1);

        m_strFontDescription.Format("Font for group '%s' in %s:", m_tree_ctrl.GetItemText(hItem), strDbName);
      }
   }

   UpdateData(FALSE);
}

void CLocalPointsPage::OnFont()
{
   HTREEITEM hItem = m_tree_ctrl.GetSelectedItem();
   if (hItem == NULL)
      return;

   C_localpnt_ovl* points_overlay = NULL;
   CString strGroupName;
   if (m_tree_ctrl.ItemHasChildren(hItem))
   {
      points_overlay = reinterpret_cast<C_localpnt_ovl *>(
         m_tree_ctrl.GetItemData(hItem));
   }
   else
   {
      HTREEITEM hParentItem = m_tree_ctrl.GetParentItem(hItem);
      if (hParentItem != NULL)
      {
         points_overlay = reinterpret_cast<C_localpnt_ovl *>(
            m_tree_ctrl.GetItemData(hParentItem));
         strGroupName = m_tree_ctrl.GetItemText(hItem);
      }
   }

   // load temporary overlay
   bool bNeedToDeleteOverlay = false;
   if (points_overlay == NULL)
   {
      C_overlay* ret;
      if (OVL_get_overlay_manager()->OpenFileOverlay(
         FVWID_Overlay_Points, m_database_filename, ret) == SUCCESS)
      {
         points_overlay = static_cast<C_localpnt_ovl *>(ret);
      }

      bNeedToDeleteOverlay = true;
   }

   // check read-only
   OverlayPersistence_Interface *pFvOverlayPersistence =
      dynamic_cast<OverlayPersistence_Interface *>(points_overlay);
   if (pFvOverlayPersistence != NULL)
   {
      long lReadOnly = 0;
      pFvOverlayPersistence->get_m_bIsReadOnly(&lReadOnly);

      if (lReadOnly)
      {
         AfxMessageBox("Unable to change the font.  The overlay is read-only");
         return;
      }
   }

   OvlFont& font = points_overlay->GetFont("", std::string(strGroupName));

   CFontDlg dlg;
   dlg.set_apply_callback(&on_overlay_font);
   dlg.set_font(font);
   g_pOverlay = points_overlay;
   g_strGroupName = strGroupName;
   dlg.DoModal();

   if (bNeedToDeleteOverlay)
   {
      points_overlay->save();
      OVL_get_overlay_manager()->delete_overlay(points_overlay);
      points_overlay->Release();
   }
}

// static
void CLocalPointsPage::on_overlay_font(CFontDlg *dlg)
{
   OvlFont font;
   font = *dlg;

   const bool bClearGroupAndPointFonts = true;

   if (g_strGroupName.GetLength() == 0)
      g_pOverlay->SetOverlayFont(font, bClearGroupAndPointFonts);
   else
      g_pOverlay->SetGroupFont(font, std::string(g_strGroupName), bClearGroupAndPointFonts);

   g_pOverlay->set_modified(TRUE);
   OVL_get_overlay_manager()->InvalidateOverlay(g_pOverlay);
}

void CLocalPointsPage::OnOpen()
{
   C_overlay *overlay;
   if (OVL_get_overlay_manager()->OpenFileOverlays(FVWID_Overlay_Points, &overlay) == SUCCESS)
      fill_group_name_list_box();
}

void CLocalPointsPage::OnClose()
{
   HTREEITEM hItem = m_tree_ctrl.GetSelectedItem();

   // close the selected item if it is a root item in the tree control
   if ((hItem != NULL) && m_tree_ctrl.GetParentItem(hItem) == NULL)
   {
      C_localpnt_ovl *overlay = reinterpret_cast<C_localpnt_ovl *>(
         m_tree_ctrl.GetItemData(hItem));
      int ret = FAILURE;
      if (overlay != NULL)
         ret = OVL_get_overlay_manager()->close(overlay);

      if (ret == SUCCESS)
         fill_group_name_list_box();

   }
}

void CLocalPointsPage::OnDefaultFont()
{
   OvlFont defaultFont;
   defaultFont.ReadFromRegistry("Database Settings\\Local Point Font", UTIL_FONT_NAME_ARIAL,
      12, "00000000", "0000FFFF", 1, false, false, true, false);

   CFontDlg dlg;
   dlg.set_apply_callback(&on_apply_font);
   dlg.set_font(defaultFont);

   dlg.DoModal();
}

BOOL CLocalPointsPage::UserChangedCheck(
   const CMyTreeCtrl& tree_control, const HTREEITEM& item) const
{
   BOOL checked = tree_control.GetCheck(item);
   BOOL was_checked = (BOOL)tree_control.GetItemData(item);
   return (checked != was_checked);
}

// static
void CLocalPointsPage::on_apply_font(CFontDlg *dlg)
{
   OvlFont font;
   font = *dlg;
   font.SaveToRegistry("Database Settings\\Local Point Font");
}

BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
   //{{AFX_MSG_MAP(CMyTreeCtrl)
   ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
   ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_CLOSE_TREE_ITEM, OnCloseTreeItem)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMyTreeCtrl::OnCloseTreeItem()
{
   C_localpnt_ovl *overlay = reinterpret_cast<C_localpnt_ovl *>(
      GetItemData(m_hActiveItem));
   if (overlay == NULL)
      return;

   if (OVL_get_overlay_manager()->close(overlay) == SUCCESS)
      m_parent->fill_group_name_list_box();
}

void CMyTreeCtrl::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
   // send WM_CONTEXTMENU to self
   SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());

   // mark message as handled and suppress default handling
   *pResult = 1;
}

void CMyTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint ptMousePos)
{
   // if Shift-F10
   if (ptMousePos.x == -1 && ptMousePos.y == -1)
      ptMousePos = (CPoint) GetMessagePos();

   ScreenToClient(&ptMousePos);

   UINT uFlags;
   HTREEITEM htItem;

   htItem = HitTest( ptMousePos, &uFlags );

   if( htItem == NULL || !ItemHasChildren(htItem))
      return;

   m_hActiveItem = htItem;

   CMenu menu;
   CMenu* pPopup;

   // the font popup is stored in a resource
   menu.LoadMenu(IDR_TREEITEM_CONTEXTMENU);
   pPopup = menu.GetSubMenu(0);
   ClientToScreen(&ptMousePos);
   pPopup->TrackPopupMenu( TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this );
}
