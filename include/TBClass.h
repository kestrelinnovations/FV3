// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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



//
// TBClass.h
//

#ifndef TBCLASS_H
#define TBCLASS_H 1

#include "common.h"
#include <set>

// foward declarations
class CDib;


/////////////////////////////////////////////////////////////////////////////
// TBClass window
struct ButtonInfo
{
   int idCommand;
   byte fsStyle;
   CString Text;
   BOOL custom; // can button be moved/added/deleted by the user
};

class CTBClass : public CToolBar
{
protected:
   DECLARE_DYNCREATE(CTBClass)

// Construction
public:
   CTBClass();

// Attributes
public:
   enum {MAX_BUTTONS = 40};

// Operations
public:
   // Set up toolbar based on resource file
   // for each button in the toolbar.  It defines the default order.
   // return SUCCESS or FAILURE
   virtual int setup(CWnd* parent, UINT id_res, CString title, BOOL customizable = FALSE, BOOL bUseImageList = FALSE);
   virtual int CTBClass::setup_24_bit(CWnd* parent, UINT id_res_normal,UINT id_res_disabled,UINT id_res_hot, CString title, BOOL customizable= FALSE);

   // Set up the toolbar based on a given bitmap
   virtual int setup(CWnd *,CString,CString,int,int *,int) { return -1; }
   virtual int setup(CWnd* parent, CString bitmap_filename, CString title, 
                  int num_buttons, long *separator_lst = NULL, int num_separators = 0);

   void set_does_reset_preserve_separators(boolean_t drps) { m_does_reset_preserve_separators = drps; }

   // common to both variations of setup
   int setup_hlpr(CString title, BOOL customizable, BOOL bUseImageList);

   // delete all existing buttons
   int clear_all_buttons();
   int hide_all_buttons();
   int hide_button(int CommandID,int hide=TRUE);

   virtual BOOL GetMessageString(UINT /*nID*/, CString& /*rMessage*/) const { return FALSE; }
   
   // add a new button to the toolbar 
   // CmdID is returned when the button is pressed
   // Text is the message text,\ntooltip text
   // Style determines the button type:TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_SEP,TBSTYLE_CHECKGROUP, or TBSTYLE_GROUP
   // Bitmap is a pointer to a CBitmap (you may leave this null and use set_button_bitmap() if desired
   // return 0 on success;
   // overlayDescGuid can be specified to associate an overlay type with a toolbar button.  This is used, for example,
   // for the editor toggle toolbar
   int add_button_from_icon(int CmdID,CString Text,BYTE fsStyle, HICON hIcon, BOOL customizable, GUID overlayDescGuid /*= GUID_NULL*/);
   int add_button_from_bitmap(int CmdID,CString Text,BYTE fsStyle, const CString& strBitmapFilename, BOOL customizable, GUID overlayDescGuid /*= GUID_NULL*/);
   int add_button(int CmdID,CString Text,BYTE fsStyle=TBSTYLE_BUTTON, int iBitmap = 0, BOOL customizable=TRUE, GUID overlayDescGuid = GUID_NULL);

   // remove a button from the toolbar with the given command ID
   int remove_button(int CmdID);

   // save the toolbar state in the registry
   // if RegValue = "" then use the toolbar title as the RegValue
   virtual int SaveState(CString RegValue="", CString regSubKey = "Software\\XPlan\\FalconView\\Toolbars50", HKEY regKey=HKEY_CURRENT_USER);

   // restore the toolbar state from the registry
   // if RegValue = "" then use the toolbar title as the RegValue
   int RestoreState(CString RegValue="", CString regSubKey = "Software\\XPlan\\FalconView\\Toolbars50", HKEY regKey=HKEY_CURRENT_USER);

   // if enable is true (initial default state) the button can be added or deleted from
   // the toolbar;
   // return SUCCESS or FAILURE
   int set_button_configurable(int CommandID,BOOL enable);

   // changes the button name and the tooltip to text;
   // return SUCCESS or FAILURE
   int set_button_text(int CommandID, const char* psztext);

   // change the toolbar bitmap for the CommandID 
   // the BitMap pointer must point to a Bitmap that will not be deleted until
   // after the toolbar is destroyed.
   // return SUCCESS or FAILURE
   int set_button_bitmap(int CommandID, CBitmap* pBitmap, CBitmap* pMask);

   // set the button ICON from the external file whos full path is in FileName.
   int set_button_bitmap(int CommandID, const char *FileName);

   // override defaqult window function to change the tooltip 
   // to the text passed in by set_button_text().
   virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;   // ClassWizard generated virtual function overrides

   // get the total number of buttons in the toolbar...
   int get_current_total_num_buttons();

   // return the overlay descriptor guid associated with the given button
   GUID GetOverlayDescGuid(int nButtonNum);
   UINT GetCommandId(int nButtonNum);
   void SetButtonPushed(const GUID& overlayDescGuid, BOOL bPushed);

   // debug function prints trace messages about the Toolbar
#ifdef _DEBUG
   void DUMP_INFO();
#endif

// Overrides
   // This overide allows us to send WM_COMMAND messages to this class first before passing
   // the message to the parent window which is the default behavior.
   virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult );

   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TBClass)
   //}}AFX_VIRTUAL

// Implementation
public:
   virtual ~CTBClass();


   // Generated message map functions
protected:
   //{{AFX_MSG(TBClass)
   //}}AFX_MSG
   afx_msg void OnQueryDelete(NMHDR *hdr, LRESULT *result);
   afx_msg void OnQueryInsert(NMHDR *hdr, LRESULT *result);
   afx_msg void OnToolbarChange(NMHDR *notify, LRESULT *result);
   afx_msg void OnBeginDrag(NMHDR *hdr, LRESULT *result);
   afx_msg void OnEndDrag(NMHDR *hdr, LRESULT *result);
   afx_msg void OnBeginAdjust(NMHDR *notify, LRESULT *result);
   afx_msg void OnEndAdjust(NMHDR *notify, LRESULT *result);
   afx_msg void OnGetButtonInfo(NMHDR *hdr, LRESULT *result);
   afx_msg void OnCustomHelp(NMHDR *notify, LRESULT *result);
   afx_msg void OnReset(NMHDR *notify, LRESULT *result);
   afx_msg void OnContextMenu(CWnd *wnd, CPoint point);
   afx_msg void OnCustomizeToolbar();
   DECLARE_MESSAGE_MAP()
   virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);

protected:
   BOOL m_custom;    // can this toolbar be changed?
   int m_num_buttons;   // number of buttons on this toolbar read from the resource (does not change)
   
   CDib *m_toolbar_bitmap;

   // used to add toolbar buttons as icons or bitmaps
   CImageList m_imageList;

   // used for 24 bit toolbars only
   CImageList  m_ToolbarImages;
   CImageList  m_ToolbarImagesDisabled;
   CImageList  m_ToolbarImagesHot;

   // load up 3 24 bit toolbars to normal, hot (for mouse roll over or pressed) and disabled (grey)
   // any thing colored (196,196,196) grey is made transparent
   void CTBClass::AttachToolbarImages (UINT inNormalImageID, UINT inDisabledImageID,UINT inHotImageID);

   // if the button image is extracted from an executable, the bitmaps must stay
   // alive until the toolbar is destroyed.  This array stores those bitmaps
   CBitmap m_button_bitmap_list[MAX_BUTTONS];

   enum {CUSTOM = 1};            // currently only 1 used flag bit
   TBNOTIFY m_tb_notify[MAX_BUTTONS];        // list of TBNOTIFY structure for each button in the toolbar
   DWORD m_tb_flags[MAX_BUTTONS];            // per button customizable
   std::vector<GUID> m_vecOverlayDescGuids;  // each button can have an associated overlay type guid
   std::vector<UINT> m_vecCmdIds;
   std::vector<CBitmap *> m_vecButtonBitmaps;

   CList<TBBUTTON, TBBUTTON> m_button_lst;

   // flag that determines if resetting the toolbar will preserve separators or not
   boolean_t m_does_reset_preserve_separators;

   void SetVerticalColumnWrap();

private:
   boolean_t m_bEnableVerticalToolbarColumnWrap;
   CSize m_sizeDefaultButton;   // Default size
};

class COverlayEditorToolbar : public CTBClass
{
public:
   COverlayEditorToolbar();
   ~COverlayEditorToolbar();
   int setup(CWnd* parent, UINT id_res, CString title, GUID overlayDescGuid);

protected:
   DECLARE_DYNCREATE(COverlayEditorToolbar)

   // there is no (easy) way to get the WM_SHOWWINDOW message piped to this class since it is handled by
   // the CMiniDockFrameWnd.  However, DelayShow is always called when the toolbar is being hidden.  This
   // override is used to toggle the current editor state if the toolbar is closed by the user
   virtual void DelayShow(BOOL bShow);

   GUID m_overlayDescGuid;
   static std::set< COverlayEditorToolbar* > s_stOverlayEditorToolbars;
};

#endif 
/////////////////////////////////////////////////////////////////////////////
