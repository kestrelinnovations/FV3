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

// overlay_menu.h
//


#include "OverlayStackChangedObserver_Interface.h"

struct OverlayTypeDescriptor;

enum OverlayMenuItemAction
{
   kToggleOverlay,
   kNewOverlay,
   kCloseFileOverlay
};

// OverlayMenuItem contains information about a single menu item in the overlay
// menu
struct OverlayMenuItem
{
   UINT menu_id;
   OverlayTypeDescriptor* overlay_type;
   OverlayMenuItemAction action;

   // overlay is set only if the action is kCloseFileOverlay
   C_overlay* overlay;

   std::string display_name;

   std::vector<OverlayMenuItem*> sub_menu_items;
   std::vector<OverlayMenuItem*> pinned_file_overlays;
   std::vector<OverlayMenuItem*> opened_file_overlays;
};


// OverlayMenu encapsulates the behavior of the Overlay menu. It handles
// the construction of the menu, pinning/unpinning overlays, and keeping
// up with changes made to the overlay stack.
//
class OverlayMenu : public OverlayStackChangedObserver_Interface
{
public:
   explicit OverlayMenu(CMFCRibbonButton* ribbon_button);
   ~OverlayMenu();

   void Initialize();
   void MenuItemSelected(UINT nID);
   int IsMenuItemChecked(UINT nID);

   // OverlayStackChangedObserver_Interface
   virtual void OverlayAdded(C_overlay* overlay) override;
   virtual void OverlayRemoved(C_overlay* overlay) override;
   virtual void OverlayOrderChanged() override
   {
   }
   virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override
   {
   }
   virtual void OverlayDirtyChanged(C_overlay* overlay) override
   {
   }

protected:
   void ClearMenuItems();
   CMFCRibbonButton* CreateRibbonButton(UINT nID, const char* label);
   void AddSubmenuItems(CMFCRibbonButton* ribbon_button, 
      const std::vector<OverlayMenuItem*>& menu_items, int menu_id);

private:
   CMFCRibbonButton* m_ribbon_button;
   std::vector<OverlayMenuItem*> m_menu_items;
   std::map<UINT, OverlayMenuItem*> m_id_to_menu_item;
};
