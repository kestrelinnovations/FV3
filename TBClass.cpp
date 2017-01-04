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



//
// CTBClass.cpp : implementation file
//

#include "stdafx.h"
#include "TBClass.h"

#include "err.h"
#include "resource.h"
#include "OvlFctry.h"
#include "dib.h"
#include "FvwGraphicDrawingSize.h"

 // these constants represent the dimensions and number of buttons in
 // the default MFC-generated toolbar. Added to support 24 bit toolbars
static const int  kImageWidth (16);
static const int  kImageHeight (15);
static const int  kNumImages (8);

static const UINT kToolBarBitDepth (ILC_COLOR24);

 // this color will be treated as transparent in the loaded bitmaps --
 // in other words, any pixel of this color will be set at runtime to
 // the user's button color. The Visual Studio toolbar editor defaults
 // to 192, 192, 192 (light gray) =(0x0C0,0xC0,0xC0).
static const RGBTRIPLE  kBackgroundColor = {192, 192, 192};


/////////////////////////////////////////////////////////////////////////////
// CTBClass

int nResetCount = 0;

IMPLEMENT_DYNCREATE(CTBClass, CToolBar)

BEGIN_MESSAGE_MAP(CTBClass, CToolBar)
   //{{AFX_MSG_MAP(CTBClass)
   //}}AFX_MSG_MAP
   
   ON_NOTIFY_REFLECT(TBN_QUERYDELETE, OnQueryDelete)
   ON_NOTIFY_REFLECT(TBN_QUERYINSERT, OnQueryInsert)
   ON_NOTIFY_REFLECT(TBN_TOOLBARCHANGE, OnToolbarChange)
   ON_NOTIFY_REFLECT(TBN_BEGINDRAG, OnBeginDrag)
   ON_NOTIFY_REFLECT(TBN_ENDDRAG, OnEndDrag)
   ON_NOTIFY_REFLECT(TBN_BEGINADJUST, OnBeginAdjust)
   ON_NOTIFY_REFLECT(TBN_ENDADJUST, OnEndAdjust)
   ON_NOTIFY_REFLECT(TBN_GETBUTTONINFO, OnGetButtonInfo)
   ON_NOTIFY_REFLECT(TBN_CUSTHELP, OnCustomHelp)
   ON_NOTIFY_REFLECT(TBN_RESET, OnReset)
   ON_WM_CONTEXTMENU()
   ON_COMMAND(ID_CUSTOMIZE_TOOLBAR, OnCustomizeToolbar)
END_MESSAGE_MAP()

CTBClass::CTBClass()
{
   int i;

   m_custom = FALSE;
   m_num_buttons = 0;

   for (i=0; i<MAX_BUTTONS; i++)
      m_tb_notify[i].pszText=NULL;

   m_toolbar_bitmap = NULL;
   m_bEnableVerticalToolbarColumnWrap = FALSE;
   m_sizeDefaultButton = m_sizeButton;  // Remember default size
}

CTBClass::~CTBClass()
{
   int i;
   for(i=0; i<m_num_buttons; i++)
   {
      if ((HBITMAP)m_button_bitmap_list[i])
         m_button_bitmap_list[i].DeleteObject();
   }

   for(size_t i=0; i<m_vecButtonBitmaps.size(); ++i)
      delete m_vecButtonBitmaps[i];

   for (i=0; i<m_num_buttons; i++)
      delete[] m_tb_notify[i].pszText;

   if (m_toolbar_bitmap != NULL)
      delete m_toolbar_bitmap;
}
//
//===========================================================================================
//

int CTBClass::setup(CWnd* parent, CString bitmap_filename, 
                    CString title, int num_buttons, long *separator_lst /*=NULL*/,
                    int num_separators /*=0*/)
{
   if (!CreateEx(parent, TBSTYLE_FLAT | TBSTYLE_WRAPABLE, WS_CHILD | WS_VISIBLE ))
   {
      ERR_report("[CTBClass::setup] Failed to create " + title + " editor toolbar.");
      return FAILURE;
   }
   
   if (m_toolbar_bitmap == NULL)
      m_toolbar_bitmap = new CDib;
   
   if (!m_toolbar_bitmap->LoadMappedBitmap(bitmap_filename))
   {
      ERR_report("[CTBClass::setup] Unable to load " + bitmap_filename);
      return FAILURE;
   }
   
   if (!SetBitmap((HBITMAP)*m_toolbar_bitmap))
   {
      ASSERT(0);
      return FAILURE;
   }   
   
   UINT lpArray[] = { ID_CLIENT_TB1, ID_CLIENT_TB2, ID_CLIENT_TB3,
      ID_CLIENT_TB4, ID_CLIENT_TB5, ID_CLIENT_TB6, ID_CLIENT_TB7,
      ID_CLIENT_TB8, ID_CLIENT_TB9, ID_CLIENT_TB10, ID_CLIENT_TB11,
      ID_CLIENT_TB12, ID_CLIENT_TB13, ID_CLIENT_TB14, ID_CLIENT_TB15,
      ID_CLIENT_TB16, ID_CLIENT_TB17, ID_CLIENT_TB18, ID_CLIENT_TB19,
      ID_CLIENT_TB20};

   // if the separator_lst is valid, fill in the array with the corresponding
   // positions
   if (separator_lst)
   {
      int separator_indx = 0;
      int id_indx = 0;
      for(int i=0;i<20;i++)
      {
         if (separator_indx < num_separators && separator_lst[separator_indx] == i)
         {
            lpArray[i] = ID_SEPARATOR;
            separator_indx++;

         }
         else
         {
            lpArray[i] = ID_CLIENT_TB1 + id_indx;
            id_indx++;
         }
      }
   }

   if (!SetButtons(lpArray, num_buttons))
   {
      ASSERT(0);
      return FAILURE;
   }
   
   return setup_hlpr(title, FALSE, FALSE);
}
//
//===========================================================================================
//

int CTBClass::setup(CWnd* parent, UINT id_res, CString title, BOOL customizable /*= FALSE */, BOOL bUseImageList /*= FALSE*/)
{
   // Tool Bar
   if ( !this->CreateEx( parent, TBSTYLE_FLAT | TBSTYLE_WRAPABLE,
      WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), id_res ) )
   {
      ERR_report("Failed to create "+title+" editor toolbar.");
      return FAILURE;
   }

   // Load the toolbar icon bar
   if (!this->LoadToolBar(id_res))
   {
      ERR_report("Failed to load "+title+" editor toolbar.");
      return FAILURE;
   }

   return setup_hlpr(title, customizable, bUseImageList);
}



int CTBClass::setup_24_bit(CWnd* parent, UINT id_res_normal,UINT id_res_disabled,UINT id_res_hot, CString title, BOOL customizable)
{

   // this code is not done...
   ASSERT(0); // I have not tested this code yet.  I beleive it will work for 24 bit color but I don't know about 16bit or 8 bit
   // Tool Bar
   if ( !this->CreateEx( parent, TBSTYLE_FLAT | TBSTYLE_WRAPABLE,
      WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), id_res_normal ) )
   {
      ERR_report("Failed to create "+title+" editor toolbar.");
      return FAILURE;
   }

   // Load the toolbar icon bar
   if (!this->LoadToolBar(id_res_normal))
   {
      ERR_report("Failed to load "+title+" editor toolbar.");
      return FAILURE;
   }


    // attach the hicolor bitmaps to the toolbar
   AttachToolbarImages (id_res_normal,
                   id_res_disabled,
                   id_res_hot);

   return setup_hlpr(title, customizable, FALSE);
}




 // find every pixel of the default background color in the specified
 // bitmap and set each one to the user's button color.
static void ReplaceBackgroundColor (CBitmap& ioBM)
{
    // figure out how many pixels there are in the bitmap
   BITMAP      bmInfo;

   VERIFY (ioBM.GetBitmap (&bmInfo));
   
   // we dont support 8 bit color for standard toolbars here
   if(bmInfo.bmBitsPixel != 24)
   {
      INFO_report("Toolbar loaded with 8 bit color (background may mot show transparent correctly");
      return;
   }
   ASSERT (bmInfo.bmWidthBytes == (bmInfo.bmWidth * (3)));

   const UINT     numPixels (bmInfo.bmHeight * bmInfo.bmWidth);

    // get a pointer to the pixels
    DIBSECTION  ds;

    VERIFY (ioBM.GetObject (sizeof (DIBSECTION), &ds) == sizeof (DIBSECTION));

   VERIFY (ds.dsBm.bmBitsPixel == 24);
   RGBTRIPLE*     pixels = reinterpret_cast<RGBTRIPLE*>(ds.dsBm.bmBits);
   VERIFY (pixels != NULL);

    // get the user's preferred button color from the system
   const COLORREF    buttonColor (::GetSysColor (COLOR_BTNFACE));
   const RGBTRIPLE      userBackgroundColor = {
      GetBValue (buttonColor), GetGValue (buttonColor), GetRValue (buttonColor)};

    // search through the pixels, substituting the user's button
    // color for any pixel that has the magic background color
   for (UINT i = 0; i < numPixels; ++i)
   {
      if (pixels [i].rgbtBlue == kBackgroundColor.rgbtBlue &&
         pixels [i].rgbtGreen == kBackgroundColor.rgbtGreen &&
         pixels [i].rgbtRed == kBackgroundColor.rgbtRed)
      {
         pixels [i] = userBackgroundColor;
      }
   }
}


 // create an image list for the specified BMP resource
static void MakeToolbarImageList (UINT       inBitmapID,
                          CImageList&  outImageList)
{
   CBitmap     bm;

    // if we use CBitmap::LoadBitmap() to load the bitmap, the colors
    // will be reduced to the bit depth of the main screen and we won't
    // be able to access the pixels directly. To avoid those problems,
    // we'll load the bitmap as a DIBSection instead and attach the
    // DIBSection to the CBitmap.
   VERIFY (bm.Attach (::LoadImage (::AfxFindResourceHandle(
      MAKEINTRESOURCE (inBitmapID), RT_BITMAP),
      MAKEINTRESOURCE (inBitmapID), IMAGE_BITMAP, 0, 0,
      (LR_DEFAULTSIZE | LR_CREATEDIBSECTION))));

    // replace the specified color in the bitmap with the user's
    // button color
   ::ReplaceBackgroundColor (bm);

    // create a 24 bit image list with the same dimensions and number
    // of buttons as the toolbar
   VERIFY (outImageList.Create (
      kImageWidth, kImageHeight, kToolBarBitDepth, kNumImages, 0));

    // attach the bitmap to the image list
   VERIFY (outImageList.Add (&bm, RGB (0, 0, 0)) != -1);
}

 // load the high color toolbar images and attach them to m_wndToolBar
void CTBClass::AttachToolbarImages (UINT inNormalImageID,
                             UINT inDisabledImageID,
                             UINT inHotImageID)
{
    // make high-color image lists for each of the bitmaps
   MakeToolbarImageList (inNormalImageID, m_ToolbarImages);
   MakeToolbarImageList (inDisabledImageID, m_ToolbarImagesDisabled);
   MakeToolbarImageList (inHotImageID,    m_ToolbarImagesHot);

    // get the toolbar control associated with the CToolbar object
   CToolBarCtrl&  barCtrl = GetToolBarCtrl();

    // attach the image lists to the toolbar control
   barCtrl.SetImageList (&m_ToolbarImages);
   barCtrl.SetDisabledImageList (&m_ToolbarImagesDisabled);
   barCtrl.SetHotImageList (&m_ToolbarImagesHot);
}

//
//===========================================================================================
//

int CTBClass::setup_hlpr(CString title, BOOL customizable, BOOL bUseImageList)
{
   EnableDocking(CBRS_ALIGN_ANY);
   
   if (!ModifyStyle(0, CCS_ADJUSTABLE))
   {
      ERR_report("ModifyStyle() failed.");
      return FAILURE;
   }
   
// if (customizable)
// {
   CToolBarCtrl &tbc = GetToolBarCtrl();

   if (bUseImageList)
   {
      m_imageList.Create(16, 15, ILC_COLORDDB, 20, 5);
      m_imageList.SetBkColor( ::GetSysColor(COLOR_BTNFACE) );
      tbc.SetImageList(&m_imageList);
   }
   
   // modify the style to include adjustable
   
   // get the number of buttons
   m_num_buttons = tbc.GetButtonCount();
   ASSERT(m_num_buttons);
   
   // generate a list of TBNOTIFY structures for each button in the toolbar
   // this list is later used to fill in the customize dialog
   if (m_num_buttons)
   {
      int i;
      CString text,tip;
      
      for (i = 0; i < m_num_buttons; i++)
      {
         if (!tbc.GetButton(i, &m_tb_notify[i].tbButton))
         {
            ERR_report("GetButton() failed.");
            return FAILURE;
         }
         
         text.LoadString(m_tb_notify[i].tbButton.idCommand);
         tip =text.Mid(text.ReverseFind('\n')+1);
         m_tb_notify[i].cchText = tip.GetLength();

         m_tb_notify[i].pszText = new char[m_tb_notify[i].cchText+1];
         strcpy_s(m_tb_notify[i].pszText,m_tb_notify[i].cchText+1,(LPCTSTR)tip);
         
         m_tb_flags[i] = CUSTOM;
      }
   }
// }
// else
// {
//    // not a customizable toolbar
//    m_num_buttons = 0;
//    m_custom = FALSE;
// }
   
   m_custom = customizable;

   // Set the title and finish setting up.
   SetWindowText(title);
   
   SetBarStyle(GetBarStyle()
      | CBRS_GRIPPER | CBRS_TOP | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

   // store the full set of buttons so that they can be restored when
   // the user click 'Reset' in the customize toolbar dialog
   for(int i=0;i<m_num_buttons;i++)
   {
      TBBUTTON button;
      tbc.GetButton(i, &button);
      m_button_lst.AddTail(button);
   }
   
   if (m_custom)
   { 
      // this adds or removes tools based on the last time it was saved.
      // if this is not user customizable, we don't want to restore a 
      // possibly corrupt registry entry
      RestoreState();
   }
   
   return SUCCESS;
}
//
//===========================================================================================
//
 
int CTBClass::RestoreState(CString RegValue, CString regSubKey, HKEY regKey)
{
   if (RegValue=="")
      GetWindowText(RegValue);

   // if there is an associated registry subkey
   if (regSubKey.GetLength())
   {
      // restore the toolbar state from the registry
      GetToolBarCtrl().RestoreState(regKey, regSubKey, RegValue);
   }
   // if this is null the Restore state will write to the root key directly so fail instead
   else
      return FAILURE;

   return SUCCESS;
}
//
//===========================================================================================
//

int CTBClass::SaveState(CString RegValue, CString regSubKey, HKEY regKey)
{
   if (RegValue=="")
      this->GetWindowText(RegValue);

   // if there is an associated registry subkey
   if (regSubKey.GetLength())
   {
      // restore the toolbar state from the registry
      GetToolBarCtrl().SaveState(regKey, regSubKey, RegValue);
   }
   // if this is null the Restore state will write to the rook key directly so fail instead
   else
      return FAILURE;

   return SUCCESS;
}
//
//===========================================================================================
//


#ifdef _DEBUG
void CTBClass::DUMP_INFO()
{
   TBBUTTON btn;
   int nb,i;
   CString text;
   CToolBarCtrl &tbc = GetToolBarCtrl();

   nb = tbc.GetButtonCount();

   for (i = 0; i < nb; i++)
   {
      if (!tbc.GetButton(i, &btn))
      {
         //ERR_report("GetButton() failed.");
         TRACE("ERROR GetButtonFailed \n");
         return;
      }

      TRACE("i=%d, bmindx %d CMD 0x%x stringid=%d\n",i,btn.iBitmap,btn.idCommand,btn.iString);
   }
}
#endif



/////////////////////////////////////////////////////////////////////////////
// CTBClass message handlers

void CTBClass::OnQueryDelete(NMHDR *hdr, LRESULT *result)
{
// TRACE("OnQueryDelete()\n");

   // special case for the zoom control

   TBNOTIFY *notify = reinterpret_cast<TBNOTIFY *>(hdr);

   int i;
   int CmdID = notify->tbButton.idCommand;

   for (i=0; i<m_num_buttons; i++)
   {
      if (m_tb_notify[i].tbButton.idCommand == CmdID)
         break;
   }
   
   if (i == m_num_buttons)
   {
      *result = TRUE;
      return;
   }

   // can the toolbar be customised and the button be customised
   *result = m_custom && (m_tb_flags[i]&CUSTOM);
}
//
//===========================================================================================
//

void CTBClass::OnQueryInsert(NMHDR *hdr, LRESULT *result)
{
// TRACE("OnQueryInsert()\n");

   TBNOTIFY *notify = reinterpret_cast<TBNOTIFY *>(hdr);

   int i;
   int CmdID = notify->tbButton.idCommand;

   for (i=0;i<m_num_buttons;i++)
   {
      if (m_tb_notify[i].tbButton.idCommand == CmdID)
         break;
   }
   
   if (i == m_num_buttons)
   { // this happens on seperators
      *result = TRUE;
      return;
   }

   *result = m_custom && (m_tb_flags[i]&CUSTOM);
}
//
//===========================================================================================
//

void CTBClass::OnToolbarChange(NMHDR *notify, LRESULT *result)
{
// TRACE("OnToolbarChange()\n");
   // force the frame window to recalculate the size
   GetParentFrame()->RecalcLayout();
   Invalidate();
   SaveState();
   *result = TRUE;
}
//
//===========================================================================================
//

void CTBClass::OnBeginDrag(NMHDR *hdr, LRESULT *result)
{
// TRACE("OnBeginDrag()\n");
}
//
//===========================================================================================
//

void CTBClass::OnEndDrag(NMHDR *hdr, LRESULT *result)
{
// TRACE("OnEndDrag()\n");
}
//
//===========================================================================================
//

void CTBClass::OnBeginAdjust(NMHDR *notify, LRESULT *result)
{
   // allocate memory to store the button information.
   //nResetCount = ::SendMessage(notify->hwndFrom, TB_BUTTONCOUNT, 0, 0);
   //lpSaveButtons = (LPTBBUTTON)GlobalAlloc(GPTR, sizeof(TBBUTTON) * nResetCount);
   
   // Save the current configuration so if the user presses
   // reset, the original toolbar can be restored.
   //for(int i = 0; i < nResetCount; i++)
   //{
      //::SendMessage(notify->hwndFrom, TB_GETBUTTON, i,
         //(LPARAM)(lpSaveButtons + i));
   //}
}
//
//===========================================================================================
//

void CTBClass::OnEndAdjust(NMHDR *notify, LRESULT *result)
{
   GetParentFrame()->RecalcLayout();
   Invalidate();

   GetToolBarCtrl().AutoSize();

   SaveState();

   // free the memory allocated during TBN_BEGINADJUST
// GlobalFree((HGLOBAL)lpSaveButtons);
}
//
//===========================================================================================
//

void CTBClass::OnGetButtonInfo(NMHDR *hdr, LRESULT *result)
{
//   TRACE("OnGetButtonInfo()\n");

   TBNOTIFY *tb_notify = reinterpret_cast<TBNOTIFY *>(hdr);

   // if the index is valid
   if (0 <= tb_notify->iItem && tb_notify->iItem < m_num_buttons)
   {
      // copy the stored button structure
      tb_notify->tbButton = m_tb_notify[tb_notify->iItem].tbButton;
         
      // copy the text for the button label in the dialog
      if (m_tb_notify[tb_notify->iItem].cchText > 0)
      {
         strcpy_s(tb_notify->pszText,
            strlen(m_tb_notify[tb_notify->iItem].pszText) + 1, /* todo: should use actual buffer length */
            m_tb_notify[tb_notify->iItem].pszText);
         tb_notify->cchText = m_tb_notify[tb_notify->iItem].cchText;
      }
         
      // indicate valid data was sent
      *result = TRUE;
   }
   // else there is no button for this index
   else
   {
      *result = FALSE;
   }
}
//
//===========================================================================================
//

void CTBClass::OnCustomHelp(NMHDR *notify, LRESULT *result)
{
   AfxGetApp()->WinHelp(HID_BASE_RESOURCE + IDD_CUSTOMIZE_TOOLBAR);
}
//
//===========================================================================================
//

void CTBClass::OnReset(NMHDR *notify, LRESULT *result)
{
   int nCount, i;
   
   // Remove all of the existing buttons starting with the
   // last and working down.
   nCount = ::SendMessage(notify->hwndFrom, TB_BUTTONCOUNT, 0, 0);
   for(i = nCount - 1; i >= 0; i--)
      ::SendMessage(notify->hwndFrom, TB_DELETEBUTTON, i, 0);
   
   // Restore the buttons that were saved.
   POSITION position = m_button_lst.GetHeadPosition();
   while(position)
   {
      TBBUTTON button = m_button_lst.GetNext(position);
      if (m_does_reset_preserve_separators == TRUE || 
         (button.fsStyle & TBSTYLE_SEP) == 0)
      {
         ::SendMessage(notify->hwndFrom, TB_ADDBUTTONS,
            (WPARAM)1, (LPARAM)&button);
      }
   }

   GetParentFrame()->RecalcLayout();
   Invalidate();
   GetToolBarCtrl().AutoSize();
}
//
//===========================================================================================
//

// This function initializes and tracks the toolbar pop-up menu
void CTBClass::OnContextMenu(CWnd *wnd, CPoint point)
{
   if (!m_custom)
      return;

   CMenu menu;

   // load the menu from resources
   VERIFY(menu.LoadMenu(IDR_TOOLBAR_MENU));

   /// track the menu as a pop-up
   CMenu* pPopup = menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);

   // Enable/Disable the Customize menu item
   if (m_custom)
      pPopup->EnableMenuItem(ID_CUSTOMIZE_TOOLBAR, MF_ENABLED);
   else
      pPopup->EnableMenuItem(ID_CUSTOMIZE_TOOLBAR, MF_GRAYED);

   // force all message in this menu to be sent here
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}
//
//===========================================================================================
//

void CTBClass::OnCustomizeToolbar()
{
   CFrameWnd *pfw;
   DWORD style;
   BOOL could_break = FALSE;

   // let user play with customization dialog but, since the toolbar appearance gets currupted
   // during the process, hide it first (reshow it later)...
   pfw = GetParentFrame();

   style = GetBarStyle();

   if ((style & (CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT)) || IsFloating())
   {  // the display somtimes messes up when the toolbar is docked vertically or 
      // when it is floating
      could_break = TRUE; 
   }
   
   if (could_break)  // hide it so you dont see the nasty display
      pfw->ShowControlBar(this, FALSE, FALSE);

   GetToolBarCtrl().Customize();
   
   if (could_break) 
      GetParentFrame()->ShowControlBar(this, TRUE, FALSE);

// DUMP_INFO();
}
//
//===========================================================================================
//

int CTBClass::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
   int i, rc;
   
   rc = CToolBar::OnToolHitTest(point, pTI );

   if (rc != -1 && pTI)
   {
      if (((int)pTI->lpszText) == -1)
      {
         for (i = 0; i <m_num_buttons;i++)
         {
            if (m_tb_notify[i].tbButton.idCommand == (int)pTI->uId)
               break;
         }

         if (i != m_num_buttons)
         {
            pTI->lpszText = (char *)malloc(m_tb_notify[i].cchText+1);
            strcpy_s(pTI->lpszText,
               strlen(m_tb_notify[i].pszText) + 1, /* todo: should use actual buffer length */
               m_tb_notify[i].pszText);
         }
      }
   }

   return rc;
}
//
//===========================================================================================
//


////////////////////////////////////////////////////////////
//
// Setup/configuration
//
///////////////////////////////////////////////////////////

int CTBClass::set_button_configurable(int CommandID,BOOL enable)
{
   int i;

   for (i = 0; i <m_num_buttons;i++)
   {
      if (m_tb_notify[i].tbButton.idCommand == CommandID)
         break;
   }
   
   if (i == m_num_buttons)
      return FAILURE;
   else
   {
      if (enable)
      {
         m_tb_flags[i] |= CUSTOM;
      }
      else
      {
         m_tb_flags[i] &= ~CUSTOM;
      }
   }
   return SUCCESS;
}
//
//===========================================================================================
//

   // changes the name 
int CTBClass::set_button_text(int CommandID, const char* psztext)
{
   int i,textlen;

   for (i = 0; i <m_num_buttons;i++)
   {
      if (m_tb_notify[i].tbButton.idCommand == CommandID)
         break;
   }
   
   if (i == m_num_buttons)
      return FAILURE;
   else
   {
       textlen = strlen(psztext);
       
       ASSERT(textlen < 255);
       if (textlen >  255) 
          return FAILURE;
       
       m_tb_notify[i].cchText = textlen;
       delete[] m_tb_notify[i].pszText;
       m_tb_notify[i].pszText = new char[textlen+1];
       strcpy_s(m_tb_notify[i].pszText,textlen+1,psztext);
   }

   return SUCCESS;
}
//
//===========================================================================================
//

int CTBClass::set_button_bitmap(int CommandID, CBitmap* pBitmap,CBitmap* pMask)
{
   int i;//,index;
   int bmindex;

   CToolBarCtrl &tbc = GetToolBarCtrl();
   

   for (i = 0; i <m_num_buttons;i++)
   {
      if (m_tb_notify[i].tbButton.idCommand == CommandID)
         break;
   }
   
   if (i == m_num_buttons)
      return FAILURE;
   else
   {

      TBBUTTON bi;
   if(!tbc.GetButton(i,&bi))
   {
      ERR_report("Failed to get button info.");
      return FAILURE;
   }
   CImageList* ilist = tbc.GetImageList();
   if (ilist != NULL)
   {
      bmindex = ilist->Replace(bi.iBitmap,pBitmap,pMask);
     tbc.SetImageList(ilist);
   }
   // CAB the following code would screw up the layout (I beleive the problem is a windows bug
   //     because it only happend if the control is docked on the side
   //   m_tb_notify[i].tbButton.iBitmap = bmindex;

   //   index = CommandToIndex(CommandID);
   //   if (index >= 0)
   //   { // this is currently a displayed icon on the toolbar
   //      tbc.DeleteButton(index);
   //      tbc.InsertButton(index,&m_tb_notify[i].tbButton);
   //   }

     Invalidate();
      return SUCCESS;
   }

}
//
//===========================================================================================
//





int CTBClass::set_button_bitmap(int CommandID, const char *FileName)
{
   int rc = FAILURE;
   
   HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), FileName, IMAGE_ICON, 16, 16, LR_LOADFROMFILE|LR_LOADMAP3DCOLORS );
   
   // try to get extract the icon from the executable
   if (hIcon == NULL)
   {
      SHFILEINFO file_info;
      if (SHGetFileInfo(FileName, FILE_ATTRIBUTE_NORMAL, &file_info, 
         sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON) == 0)
         return FAILURE;
      
      hIcon = file_info.hIcon;
   }
   
   // draw the icon to the bitmap
   //
   CDC *pDC = GetDC();
   if (pDC != NULL)
   {
      CDC memory_dc, mask_dc;
      memory_dc.CreateCompatibleDC(pDC);
      mask_dc.CreateCompatibleDC(pDC);
      memory_dc.SetMapMode(MM_TEXT);
      mask_dc.SetMapMode(MM_TEXT);
      
      const int button_index = CommandID - ID_CUSTOM_TB1;
      if (button_index < 0 || button_index >= MAX_BUTTONS)
      {
         DestroyIcon(hIcon);
         return FAILURE;
      }
      
      CBitmap *bitmap = &m_button_bitmap_list[button_index];
      
      // delete previous bitmap if necessary
      if ((HBITMAP)*bitmap)
         bitmap->DeleteObject();
      
      const int x_size = GetSystemMetrics(SM_CXSMICON);
      const int y_size = GetSystemMetrics(SM_CYSMICON);
      
      bitmap->CreateCompatibleBitmap(pDC, x_size, y_size);
      CBitmap mask_bmp;
      mask_bmp.CreateCompatibleBitmap(pDC, x_size, y_size);
      
      CBitmap *old_bitmap = memory_dc.SelectObject(bitmap);
      CBitmap *old_mask_bitmap = mask_dc.SelectObject(&mask_bmp);

      DrawIconEx(mask_dc.m_hDC, 0, 0, hIcon, 16, 16, 0, 0, DI_MASK);
      DrawIconEx(memory_dc.m_hDC, 0, 0, hIcon, 16, 16, 0, 0, DI_IMAGE);
      
      const COLORREF background_color = GetSysColor(COLOR_BTNFACE);
      const COLORREF top_left = memory_dc.GetPixel(0,0);
      for(int y=0;y<y_size;y++)
      {
         for(int x=0;x<x_size;x++)
         {
            if (memory_dc.GetPixel(x, y) == top_left)
               memory_dc.SetPixel(x, y, background_color);
         }
      }
      
      memory_dc.SelectObject(old_bitmap);
      mask_dc.SelectObject(old_mask_bitmap);
      ReleaseDC(pDC);
      
      rc = set_button_bitmap(CommandID, bitmap,&mask_bmp);
   }

   DestroyIcon(hIcon);
   
   return rc;
}

//
//===========================================================================================
//

// remove a button from the toolbar with the given command ID
int CTBClass::remove_button(int CmdID)
{
   CToolBarCtrl &tbc = GetToolBarCtrl();

   const int button_index = tbc.CommandToIndex(CmdID);
   tbc.DeleteButton(button_index);

   POSITION position = m_button_lst.FindIndex(button_index);
   if (position)
      m_button_lst.RemoveAt(position);

   m_num_buttons--;

   return SUCCESS;
}
//
//===========================================================================================
//

int CTBClass::add_button_from_icon(int CmdID,CString Text,BYTE fsStyle,
                         HICON hIcon, BOOL customizable, GUID overlayDescGuid /*= GUID_NULL*/)
{
   ASSERT(m_imageList.m_hImageList != NULL); // make sure you set bUseImageList in setup()
   const int iBitmap = m_imageList.Add(hIcon);
   return add_button(CmdID, Text, fsStyle, iBitmap, customizable, overlayDescGuid);
}

int CTBClass::add_button_from_bitmap(int CmdID,CString Text,BYTE fsStyle,
                         const CString& strBitmapFilename, BOOL customizable, GUID overlayDescGuid /*= GUID_NULL*/)
{
   ASSERT(m_imageList.m_hImageList != NULL); // make sure you set bUseImageList in setup()

   HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, strBitmapFilename, IMAGE_BITMAP, 0, 0, 
      LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
   CBitmap *pBitmap = new CBitmap();
   pBitmap->Attach(hBitmap);
   m_vecButtonBitmaps.push_back(pBitmap);

   const int iBitmap = m_imageList.Add(pBitmap, ::GetSysColor(COLOR_WINDOW) );
   return add_button(CmdID, Text, fsStyle, iBitmap, customizable, overlayDescGuid);
}

int CTBClass::add_button(int CmdID,CString Text,BYTE fsStyle, int iBitmap, BOOL customizable, GUID overlayDescGuid /*= GUID_NULL*/)
{
   TBBUTTON button;
   CString tip;
   CToolBarCtrl &tbc = GetToolBarCtrl();
   
   if (m_num_buttons >= MAX_BUTTONS)
   {
      ERR_report("too many buttons added to toolbar.");
      return -2;
   }
   
   button.idCommand = CmdID;
   button.fsStyle = fsStyle;
   button.fsState = TBSTATE_ENABLED;
   button.dwData = NULL;
   button.iBitmap = iBitmap;

   button.iString = NULL;
   tbc.AddButtons(1,&button);
   m_button_lst.AddTail(button);

   if (!tbc.GetButton(m_num_buttons, &m_tb_notify[m_num_buttons].tbButton))
   {
      ERR_report("GetButton() failed.");
      return -1;
   }

   tip =Text.Mid(Text.ReverseFind('\n')+1);
   m_tb_notify[m_num_buttons].cchText = tip.GetLength();
   m_tb_notify[m_num_buttons].pszText = new char[m_tb_notify[m_num_buttons].cchText+1];

   if (!m_tb_notify[m_num_buttons].pszText)
      return -1;

   strcpy_s(m_tb_notify[m_num_buttons].pszText,m_tb_notify[m_num_buttons].cchText+1,(LPCTSTR)tip);
   m_tb_flags[m_num_buttons] = customizable ? CUSTOM:0;

   m_num_buttons++;

   m_vecOverlayDescGuids.push_back(overlayDescGuid);
   m_vecCmdIds.push_back(CmdID);

   return 0;
}
//
//===========================================================================================
//

int CTBClass::clear_all_buttons()
{
   int i;
   CToolBarCtrl &tbc = GetToolBarCtrl();

   for (i=0;i<m_num_buttons;i++)
   {
      delete [] m_tb_notify[i].pszText;
   }

   m_num_buttons = 0;
   m_button_lst.RemoveAll();

   // delete all the buttons from the toolbar itself
   while (tbc.DeleteButton(0) == TRUE)
   {
   }

   return 0;   
}
//
//===========================================================================================
//

int CTBClass::hide_all_buttons()
{
   int i;
   CToolBarCtrl &tbc = GetToolBarCtrl();
   
   for (i=0;i<m_num_buttons;i++)
   {
      if(m_tb_notify[i].tbButton.fsStyle != TBSTYLE_SEP) // dont hide seperators
         tbc.HideButton(m_tb_notify[i].tbButton.idCommand,TRUE);
   }

   return 0;
}
//
//===========================================================================================
//

int CTBClass::hide_button(int CommandID,int hide)
{
   int rc;
   CToolBarCtrl &tbc = GetToolBarCtrl();
   rc =  tbc.HideButton(CommandID,hide);
   tbc.AutoSize();
   return rc;
}
//
//===========================================================================================
//

BOOL CTBClass::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult )
{
   if (message==WM_COMMAND)
   { 
      // give this class the first crack at the WM_COMMAND messages generated by clicking a toolbar button
      LRESULT lres;
      lres = OnCommand(wParam,NULL);
      return lres ? lres :CToolBar::OnChildNotify( message, wParam,  lParam,  pLResult );
   }
   else
      return CToolBar::OnChildNotify( message, wParam,  lParam,  pLResult );
}
//
//===========================================================================================
//

int CTBClass::get_current_total_num_buttons()
{
   CToolBarCtrl &tbc = GetToolBarCtrl();
   ASSERT(tbc);
   return tbc.GetButtonCount();
}

GUID CTBClass::GetOverlayDescGuid(int nButtonNum)
{
   if (nButtonNum < 0 || nButtonNum >= static_cast<int>(m_vecOverlayDescGuids.size()) )
   {
      ASSERT(0);  // invalid button number
      return GUID_NULL;
   }

   return m_vecOverlayDescGuids[nButtonNum];
}

UINT CTBClass::GetCommandId(int nButtonNum)
{
   if (nButtonNum < 0 || nButtonNum >= static_cast<int>(m_vecCmdIds.size()) )
   {
      ASSERT(0);  // invalid button number
      return 0;
   }

   return m_vecCmdIds[nButtonNum];
}

void CTBClass::SetButtonPushed(const GUID& overlayDescGuid, BOOL bPushed)
{
   std::vector<GUID>::const_iterator it = std::find(
      m_vecOverlayDescGuids.begin(), 
      m_vecOverlayDescGuids.end(), 
      overlayDescGuid);

   if (it != m_vecOverlayDescGuids.end())
   {
      int nIndex = it - m_vecOverlayDescGuids.begin();

      UINT nNewStyle = GetButtonStyle(nIndex) & ~(TBBS_CHECKED | TBBS_INDETERMINATE);
      if (bPushed == 1)
         nNewStyle |= TBBS_CHECKED;
      SetButtonStyle(nIndex, nNewStyle | TBBS_CHECKBOX);
   }
}



void CTBClass::SetVerticalColumnWrap()
{
   m_bEnableVerticalToolbarColumnWrap =
      0 != reg::get_registry_int( "Screen Drawing",
               "EditorToolbarsColumnWrap", 1 );
}


CSize CTBClass::CalcDynamicLayout(int nLength, DWORD nMode)
{
   int cx;
   if ( IsKindOf( RUNTIME_CLASS( COverlayEditorToolbar ) ) )
   {
      cx = EDITOR_TOOLBAR_BUTTON_EXPANSION;
      if ( m_sizeDefaultButton.cx + cx != m_sizeButton.cx )
         SetSizes( m_sizeDefaultButton + CSize( cx, cx ),
            m_sizeImage );
   }

   if ( 0 == ( nMode & LM_VERTDOCK )
         || !m_bEnableVerticalToolbarColumnWrap )
      return CToolBar::CalcDynamicLayout( nLength, nMode );

   CRect rc1, rc2;

   rc1.SetRectEmpty();
   CalcInsideRect( rc1, FALSE );
   cx = m_sizeButton.cx - rc1.Width();
   
   GetParentFrame()->GetWindowRect( &rc1 );
   GetWindowRect( &rc2 );
   do
   {
      CSize sz = CToolBar::CalcDynamicLayout( cx, nMode & ~( LM_COMMIT | LM_VERTDOCK ) );
      if ( rc2.top + sz.cy <= rc1.bottom
            || cx > 5 * m_sizeButton.cx )
         break;

      cx += m_sizeButton.cx;
   } while ( TRUE );
         
   return CToolBar::CalcDynamicLayout( cx, nMode & ~LM_VERTDOCK );
}

//
//===========================================================================================
//


// COverlayEditorToolbar
//
std::set< COverlayEditorToolbar* > COverlayEditorToolbar::s_stOverlayEditorToolbars;

IMPLEMENT_DYNCREATE(COverlayEditorToolbar, CTBClass)

COverlayEditorToolbar::COverlayEditorToolbar()
{
   s_stOverlayEditorToolbars.insert( this );
}

COverlayEditorToolbar::~COverlayEditorToolbar()
{
   s_stOverlayEditorToolbars.erase( this );
}

int COverlayEditorToolbar::setup(CWnd* parent, UINT id_res, CString title, GUID overlayDescGuid)
{
   m_overlayDescGuid = overlayDescGuid;

   SetVerticalColumnWrap();   // Whether to allow column wrap
   
   return __super::setup(parent, id_res, title);
}

// there is no (easy) way to get the WM_SHOWWINDOW message piped to this class since it is handled by
// the CMiniDockFrameWnd.  However, DelayShow is always called when the toolbar is being hidden.  This
// override is used to toggle the current editor state if the toolbar is closed by the user
void COverlayEditorToolbar::DelayShow(BOOL bShow)
{
   // if the toolbar is being hidden
   if (bShow == FALSE)
   {
      // if the edit mode hasn't been changed then the editor mode should be toggled off
      if (m_overlayDescGuid == OVL_get_overlay_manager()->GetCurrentEditor())
         OVL_get_overlay_manager()->set_mode(GUID_NULL);
   }

   __super::DelayShow(bShow);
}