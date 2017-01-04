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

// overlay_menu.cpp
//

#include "StdAfx.h"
#include "..\resource.h"
#include "overlay_menu.h"
#include "overlay.h"
#include "ovl_mgr.h"

OverlayMenu::OverlayMenu(CMFCRibbonButton* ribbon_button) :
   m_ribbon_button(ribbon_button)
{
   OVL_get_overlay_manager()->RegisterEvents(this);
   Initialize();
}

OverlayMenu::~OverlayMenu()
{
   OVL_get_overlay_manager()->UnregisterEvents(this);

   ClearMenuItems();
}

void OverlayMenu::ClearMenuItems()
{
   for (auto it = std::begin(m_menu_items); it != std::end(m_menu_items); ++it)
   {
      delete *it;
   }
   m_menu_items.clear();
   m_id_to_menu_item.clear();

   m_ribbon_button->RemoveAllSubItems();
}

void OverlayMenu::Initialize()
{
   OVL_get_type_descriptor_list()->ResetEnumerator();
	while ( OVL_get_type_descriptor_list()->MoveNext() )
	{
      OverlayTypeDescriptor* overlay_type = OVL_get_type_descriptor_list()->m_pCurrent;

      // Static overlays
      if (!overlay_type->fileTypeDescriptor.bIsFileOverlay)
      {
         // the factory will be omitted if the display name is empty
         if (overlay_type->displayName.IsEmpty())
            continue;

         OverlayMenuItem* menu_item = new OverlayMenuItem();

         menu_item->display_name = overlay_type->displayName;
         menu_item->overlay_type = overlay_type;
         menu_item->action = kToggleOverlay;
         
         // if the menu item has a parent display name, we will add it to the 
         // list of submenu items
         if (overlay_type->parentDisplayName.GetLength())
         {
            OverlayMenuItem* parent_menu_item;

            // check to see if there is an existing entry for the parent
            // display name
            auto it = std::find_if(std::begin(m_menu_items), 
               std::end(m_menu_items), [overlay_type](OverlayMenuItem* i)
               {
                  return i->display_name == std::string(overlay_type->parentDisplayName);
               });
            if (it != std::end(m_menu_items))
            {
               parent_menu_item = *it;
            }
            // if not, then add one
            else
            {
               parent_menu_item = new OverlayMenuItem();
               parent_menu_item->display_name = overlay_type->parentDisplayName;
               parent_menu_item->overlay_type = overlay_type;
               m_menu_items.push_back(parent_menu_item);
            }

            parent_menu_item->sub_menu_items.push_back(menu_item);
         }
         else
         {
            m_menu_items.push_back(menu_item);
         }
      }
      // File overlays
      else
      {
         OverlayMenuItem* menu_item = new OverlayMenuItem();

         menu_item->display_name = overlay_type->displayName;
         menu_item->overlay_type = overlay_type;
         m_menu_items.push_back(menu_item);

         // If the overlay type has an associated editor, then append a New 
         // menu item
         if (menu_item->overlay_type->pOverlayEditor != nullptr)
         {
            OverlayMenuItem* sub_menu_item = new OverlayMenuItem();
            sub_menu_item->display_name = "New...";
            sub_menu_item->overlay_type = overlay_type;
            sub_menu_item->action = kNewOverlay;
            menu_item->sub_menu_items.push_back(sub_menu_item);
         }

         // Add each opened, file overlay to the submenu
         C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();
         C_overlay* overlay = ovl_mgr->get_first_of_type(
            menu_item->overlay_type->overlayDescriptorGuid);
         while (overlay)
         {
            OverlayMenuItem* sub_menu_item = new OverlayMenuItem();
            sub_menu_item->display_name = ovl_mgr->GetOverlayDisplayName(
               overlay);
            sub_menu_item->action = kCloseFileOverlay;
            sub_menu_item->overlay = overlay;
            sub_menu_item->overlay_type = overlay_type;

            _bstr_t file_specification;
            dynamic_cast<OverlayPersistence_Interface *>(overlay)->
               get_m_fileSpecification(file_specification.GetAddress());

            if (ovl_mgr->test_file_menu((char *)file_specification))
               menu_item->pinned_file_overlays.push_back(sub_menu_item);
            else
               menu_item->opened_file_overlays.push_back(sub_menu_item);

            overlay = ovl_mgr->get_next_of_type(overlay,
               menu_item->overlay_type->overlayDescriptorGuid);
         }
      }
	}

   // Sort the list of top-level menu items by name
   std::sort(std::begin(m_menu_items), std::end(m_menu_items), 
      [] (OverlayMenuItem* menu_item_a, 
          OverlayMenuItem* menu_item_b)
      {
         return menu_item_a->display_name < menu_item_b->display_name;
      });

   // After the menu item entries have been sorted, add them to the menu
   UINT menu_id = IDRANGE_OVERLAY_MENU_ITEM_1;
   for (auto it = std::begin(m_menu_items); it != std::end(m_menu_items); ++it)
   {
      OverlayMenuItem* menu_item = *it;

      // if we are dealing with a submenu item, then create a new popup menu
      // and add the subitems to it.
      if (menu_item->sub_menu_items.size())
      {
         // append the submenu to the menu
         CMFCRibbonButton* ribbon_button = CreateRibbonButton(menu_id++,
            menu_item->display_name.c_str());
         m_ribbon_button->AddSubItem(ribbon_button);

         if (menu_item->overlay_type->fileTypeDescriptor.bIsFileOverlay)
         {
            // for each submenu item
            AddSubmenuItems(ribbon_button, menu_item->sub_menu_items, menu_id);
            menu_id += menu_item->sub_menu_items.size();

            if (menu_item->pinned_file_overlays.size())
            {
               ribbon_button->AddSubItem(new CMFCRibbonLabel("Pinned"));
               AddSubmenuItems(ribbon_button, menu_item->pinned_file_overlays, menu_id);
               menu_id += menu_item->pinned_file_overlays.size();
            }

            if (menu_item->opened_file_overlays.size())
            {
               ribbon_button->AddSubItem(new CMFCRibbonLabel("Opened"));
               AddSubmenuItems(ribbon_button, menu_item->opened_file_overlays, menu_id);
               menu_id += menu_item->opened_file_overlays.size();
            }
         }
         // Not a file overlay
         else
         {
            std::sort(std::begin(menu_item->sub_menu_items), std::end(menu_item->sub_menu_items), 
               [] (OverlayMenuItem* menu_item_a, 
               OverlayMenuItem* menu_item_b)
            {
               return menu_item_a->display_name < menu_item_b->display_name;
            });

            // for each submenu item
            AddSubmenuItems(ribbon_button, menu_item->sub_menu_items, menu_id);
            menu_id += menu_item->sub_menu_items.size();
         }
      }
      // o.w. a normal menu item without submenus
      else
      {
         menu_item->menu_id = menu_id;
         m_ribbon_button->AddSubItem(CreateRibbonButton(menu_id, 
               (*it)->display_name.c_str()));
         m_id_to_menu_item[menu_id] = menu_item;
         menu_id++;
      }
   }
}

CMFCRibbonButton* OverlayMenu::CreateRibbonButton(UINT nID, const char* label)
{
   CMFCRibbonButton* b = new CMFCRibbonButton(nID, label);
   b->SetDefaultMenuLook();
   static_cast<CMFCRibbonBaseElement *>(b)->SetParentRibbonBar(m_ribbon_button->GetParentRibbonBar());
   return b;
}

void OverlayMenu::AddSubmenuItems(CMFCRibbonButton* ribbon_button,
   const std::vector<OverlayMenuItem*>& menu_items, int menu_id)
{
   for (auto it = std::begin(menu_items); 
      it != std::end(menu_items); ++it)
   {
      ribbon_button->AddSubItem(CreateRibbonButton(menu_id, 
         (*it)->display_name.c_str()));

      (*it)->menu_id = menu_id;
      m_id_to_menu_item[menu_id] = *it;
      menu_id++;
   }
}

void OverlayMenu::MenuItemSelected(UINT nID)
{
   auto it = m_id_to_menu_item.find(nID);
   if (it != m_id_to_menu_item.end())
   {
      C_ovl_mgr* ovl_mgr = OVL_get_overlay_manager();
      GUID overlay_type_guid = it->second->overlay_type->overlayDescriptorGuid;

      if (it->second->action == kToggleOverlay)
      {
         ovl_mgr->toggle_static_overlay(overlay_type_guid);
      }
      else if (it->second->action == kNewOverlay)
      {
         C_overlay* overlay;
         ovl_mgr->create(overlay_type_guid, &overlay);
         ovl_mgr->SwitchToEditor(overlay);
      }
      else if (it->second->action == kCloseFileOverlay)
      {
         ovl_mgr->close(it->second->overlay);
      }
   }
}

int OverlayMenu::IsMenuItemChecked(UINT nID)
{
   auto it = m_id_to_menu_item.find(nID);
   if (it != m_id_to_menu_item.end())
   {
      if (it->second->overlay_type && it->second->action != kNewOverlay)
         return OVL_get_overlay_manager()->get_first_of_type(it->second->overlay_type->overlayDescriptorGuid) != nullptr;
   }

   return 0;
}

// OverlayStackChangedObserver_Interface implementation
//
void OverlayMenu::OverlayAdded(C_overlay* overlay)
{
   ClearMenuItems();
   Initialize();
}

void OverlayMenu::OverlayRemoved(C_overlay* overlay)
{
   ClearMenuItems();
   Initialize();
}
