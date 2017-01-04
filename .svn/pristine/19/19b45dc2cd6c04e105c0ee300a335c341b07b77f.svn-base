#pragma once
#include "afxcmn.h"

#define TREE_VIEW_CHECK_STATE_CHANGE (WM_USER + 100) 

/**
 * @class   CustomTreeControl
 *
 * @brief   A simple tree control extension for checkable tree views. This
 *          control will automatically check or uncheck all child tree items
 *          when the parent item's check box is toggled. It also encapsulates a
 *          common work-around to the lack of this feature, and posts a custom
 *          message to the parent only when an actual box has been checked or
 *          unchecked.
 *          
 *          quick start:
 *          
 *          0) If you only want the auto checking and don't need a listener,
 *          skip to step 3
 *          
 *          1) Add this to your message map:
 *          ON_MESSAGE(TREE_VIEW_CHECK_STATE_CHANGE, OnTreeViewCheckStateChange)
 *          
 *          2) Implement the handler method (if desired):  
 *          LRESULT YourClass::OnTreeViewCheckStateChange( WPARAM wParam, LPARAM
 *          lParam)
 *          {
 *             HTREEITEM   item = (HTREEITEM)lParam;
 *             BOOL newState = m_layerTreeControl.GetCheck(item);
 *             return 0L;
 *          }
 *          
 *          3) Everything else is exactly like CTreeCtrl except you declare a
 *          CustomTreeControl instead, of course...
 *          
 *          See MDMInstallWebMapType.cpp in the catalog project for an example
 *          of use.
 *          
 *          Design notes: Modify this to improve existing features.
 *                        Extend this, in a new class, to add features.
 *                        TODO:This needs to be moved to a more generalized
 *                              location like FvCoreMFC...
 *
 * @author  MattWeber
 * @date 12/13/2013
 */

class CustomTreeControl :
   public CTreeCtrl
{
public:
   CustomTreeControl(void);
   ~CustomTreeControl(void);

   bool ItemHasSelectedChildren(HTREEITEM hItem);

   afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
   afx_msg LRESULT OnTreeViewCheckStateChange(WPARAM wParam, LPARAM lParam);
   DECLARE_MESSAGE_MAP()

protected:
   void SetCheckForNodeAndAllChildren(HTREEITEM theItem, BOOL theState);
};

