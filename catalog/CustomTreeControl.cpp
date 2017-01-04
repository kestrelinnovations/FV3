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

#include "stdafx.h"
#include "CustomTreeControl.h"

//---------------------------------------------------------------------------
// Please see comments in CustomTreeControl.h for intended use of this class.
//---------------------------------------------------------------------------
CustomTreeControl::CustomTreeControl(void)
{
}

CustomTreeControl::~CustomTreeControl(void)
{
}

// REFACTOR:
// From the documentation, it seems like TVN_ITEMCHANGED is the correct
// message to listen for checkbox changes, however it doesn't get fired...
// This is where George and I left off:
// From George{
//    http://msdn.microsoft.com/en-us/library/windows/desktop/bb773175(v=vs.85).aspx
//
//    (Hypothesis) If FalconView is updated to use the latest common controls,
//    the event for tree item changed may get triggered. If this is true, 
//    then the existing code that is implemented using the click and key 
//    press handlers can be reduced.
// }
BEGIN_MESSAGE_MAP(CustomTreeControl, CTreeCtrl)
   ON_MESSAGE(TREE_VIEW_CHECK_STATE_CHANGE, OnTreeViewCheckStateChange)
   ON_NOTIFY_REFLECT(NM_CLICK, &CustomTreeControl::OnNMClick)
   ON_NOTIFY_REFLECT(TVN_KEYDOWN, &CustomTreeControl::OnTvnKeydown)
END_MESSAGE_MAP()

/**
 * @fn   void CustomTreeControl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
 *
 * @brief   Captures a click event and checks whether an item's check box was
 *          toggled. If so, sends the TREE_VIEW_CHECK_STATE_CHANGE message
 *          locally, with the toggled item.
 *
 * @author  MattWeber
 * @date 12/13/2013
 *
 * @param [in,out]   pNMHDR   If non-null, the nmhdr.
 * @param [out]   pResult     If non-null, the result.
 */

void CustomTreeControl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{

      HTREEITEM item;
      UINT flags;

      // verify that we have a mouse click in the check box area
      CPoint p;
      GetCursorPos(&p);

      ScreenToClient(&p);
      item = HitTest(p, &flags);

      // set selection to clicked item
      SelectItem(item);

      if(item && (flags & TVHT_ONITEMSTATEICON))
      {
            // Post message state has changed…
            PostMessage(TREE_VIEW_CHECK_STATE_CHANGE,0,(LPARAM)item);
      }

      *pResult = 0; 
}

/**
 * @fn   void CustomTreeControl::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
 *
 * @brief   If space key pressed sends the TREE_VIEW_CHECK_STATE_CHANGE message
 *          locally, with the toggled item.
 *
 * @author  MattWeber
 * @date 12/13/2013
 *
 * @param [in,out]   pNMHDR   If non-null, the nmhdr.
 * @param [out]   pResult     If non-null, the result.
 */

void CustomTreeControl::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
   if(pTVKeyDown->wVKey==VK_SPACE)
      {
            // Determine the selected tree item.
            HTREEITEM item = GetSelectedItem();

            if(item!=NULL)
            {
                  // handle state change here or post message to another handler
                  // Post message state has changed…
                  PostMessage(TREE_VIEW_CHECK_STATE_CHANGE,0,(LPARAM)item);
            }
      } 
   *pResult = 0;
}

/**
 * @fn   LRESULT CustomTreeControl::OnTreeViewCheckStateChange(WPARAM wParam,
 *       LPARAM lParam)
 *
 * @brief   Called when an item's checkbox is toggled. Currently, sets the 
 *          checkbox state for all children of the toggled node. The message
 *          is then passed to the parent object.
 *
 * @author  MattWeber
 * @date 12/13/2013
 *
 * @param   wParam   The wParam field of the message.
 * @param   lParam   The HTREEITEM selected.
 *
 * @return  GetParent()->PostMessage(TREE_VIEW_CHECK_STATE_CHANGE, 0, (LPARAM)item);
 */

LRESULT CustomTreeControl::OnTreeViewCheckStateChange(WPARAM wParam, LPARAM lParam)
{

      // Handle message here…
      HTREEITEM   item = (HTREEITEM)lParam;

      SetCheckForNodeAndAllChildren(item, GetCheck(item));

      // checkState holds the check box state AFTER it has been changed. So
      // basically the new state...
      //BOOL checkState = GetCheck(item);
      return GetParent()->PostMessage(TREE_VIEW_CHECK_STATE_CHANGE, 0, (LPARAM)item);
}

/**
 * @fn   void CustomTreeControl::SetCheckForNodeAndAllChildren(HTREEITEM theItem,
 *       BOOL theState)
 *
 * @brief   First sets the parent checkbox state (theItem) to theState. Then does
 *          so for all children recursively.
 *
 * @author  MattWeber
 * @date 12/13/2013
 *
 * @param   theItem  The root HTREEITEM to be toggled
 * @param   theState true to show the check, false to remove the check
 */

void CustomTreeControl::SetCheckForNodeAndAllChildren(HTREEITEM theItem, BOOL theState)
{
   SetCheck(theItem,theState);
   HTREEITEM item = GetChildItem(theItem);

   while(item != nullptr)
   {
      SetCheckForNodeAndAllChildren(item,theState);
      item = GetNextSiblingItem(item);
   }
}

/**
 * @fn   bool CustomTreeControl::ItemHasSelectedChildren(HTREEITEM hItem)
 *
 * @brief   Determines whether the HTREEITEM has children that are selected.
 *          Does so recursively until the first selected item is found. Note:
 *          this method only checks children. If a selected item that has no
 *          children is passed in as hItem, on the intial call, this method will
 *          return false.
 *
 * @author  MattWeber
 * @date 12/13/2013
 *
 * @param   hItem Handle of the item.
 *
 * @return  true if any child items are checked, false otherwise.
 */

bool CustomTreeControl::ItemHasSelectedChildren(HTREEITEM hItem)
{

   if(!ItemHasChildren(hItem))
   {
      return false;
   }

   HTREEITEM item = GetChildItem(hItem);

   while(item != nullptr)
   {  // notice we're calling GetCheck(item) first, for a reason
      if(GetCheck(item) || ItemHasSelectedChildren(item))
      {
         return true;
      }
      item = GetNextSiblingItem(item);
   }
   return false;
}