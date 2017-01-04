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

// menu.cpp - contains UtilMenu implementation
//


#include "stdafx.h"
#include "utils.h"

UtilMenu::UtilMenu(CMenu *menu) : m_menu(menu)
{

}

// Returns the index of the first item in the menu with the given name.  Returns
// -1 if the menu entry is not found
int UtilMenu::get_menu_pos(const char *name)
{
   if (m_menu)
   {
		CString string;
		
		// test each menu item in the 
		const int item_count = m_menu->GetMenuItemCount();
		for (int i = 0; i < item_count; i++)
		{
			m_menu->GetMenuString(i, string, MF_BYPOSITION);
			if (string == name)
				return i;
		}
   }

   return -1;
}

// Returns the submenu with the given name.  NULL if none is found.
CMenu *UtilMenu::get_submenu_by_name(const char *name)
{
	// test each item in the menu for name, until a match is found or all items
   // are tested
   if (m_menu)
   {
      int i;
      int item_count;
		CString string;
		
		// test each menu item in the 
		item_count = m_menu->GetMenuItemCount();
		for (i = 0; i < item_count; i++)
		{
			m_menu->GetMenuString(i, string, MF_BYPOSITION);
			if (string == name)
				return m_menu->GetSubMenu(i);
		}
   }
	
   return NULL;
}

// Removes all menu items after the one with the given menu string, if a 
// menu item with that string is found, and returns TRUE.  Otherwise FALSE 
// is returned.
boolean_t UtilMenu::remove_menu_items_after(CString menu_string)
{
	CString i_menu_string;
	
	if (m_menu)
   {
      int i = 0;
      int index = -1;
      int count = m_menu->GetMenuItemCount();
		
      // search menu items in order until a match is found
      while (i < count)
      {
         // if an item with the given id is found, remove all items after it
			m_menu->GetMenuString(i, i_menu_string, MF_BYPOSITION);
         if (i_menu_string == menu_string)
         {
            // the index of the first item after the match
            index = i + 1;
				
            // there are count - index items that must be removed
            for (i=0; i < (count - index); i++)
               m_menu->RemoveMenu(index, MF_BYPOSITION);
				
            return TRUE;   // the menu item with the given ID was found
         }
			
         i++;
      }
   }
	
   return FALSE;
}

// Removes all menu items after the one with the given ID, if a menu item
// with that ID is found, and returns TRUE.  Otherwise FALSE is returned.
boolean_t UtilMenu::remove_menu_items_after(UINT id)
{
	if (m_menu)
   {
      int i = 0;
      int index = -1;
      int count = m_menu->GetMenuItemCount();
		
      // search menu items in order until a match is found
      while (i < count)
      {
         // if an item with the given id is found, remove all items after it
         if (m_menu->GetMenuItemID(i) == id)
         {
            // the index of the first item after the match
            index = i + 1;
				
            // there are count - index items that must be removed
            for (i=0; i < (count - index); i++)
               m_menu->RemoveMenu(index, MF_BYPOSITION);
				
            return TRUE;   // the menu item with the given ID was found
         }
			
         i++;
      }
   }
	
   return FALSE;
}

// appends a submenu with the given name
CMenu* UtilMenu::append_submenu(const char* pSubmenuName)
{
   CMenu* pSubMenu = new CMenu();
   pSubMenu->CreateMenu();
   m_menu->AppendMenu(MF_POPUP, (UINT)pSubMenu->m_hMenu, pSubmenuName);
   return pSubMenu;
}

