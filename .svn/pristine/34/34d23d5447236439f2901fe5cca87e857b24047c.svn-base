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
// CIconLoader
//

#include "stdafx.h"


// NOTE:  The following were brought in from CHUM.h.   Need to sort out which ones are really necessary.
#include "common.h"
#include "overlay.h"
#include "chstruct.h"
#include "polyline.h"
#include "IndexFil.h"
#include "maps_d.h"
#include "param.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "ogrsf_frmts\ogrsf_frmts.h"
#import "fvw.tlb" named_guids
#import "SampledMotionPlayback.tlb" no_namespace, named_guids, exclude("IMotionPlayback")
#include "..\..\..\fvw_core\FvBaseDataSet\FvBaseDataSet.h"


#include "..\getobjpr.h"
#include "err.h"
#include "file.h"
#include "graphics.h"
#include "map.h"
#include "ovl_mgr.h"

CMapStringToOb CIconImage::m_loaded_images;  // list of already-loaded images
CList<CIconAlias*, CIconAlias*>CIconImage::m_aliases;  // list of defined aliases
CImageList* CIconImage::m_drag_image = NULL;  // drag list to be used for drag operations
boolean_t CIconImage::m_cursor_showing;  // indicates whether NO DROP cursor showing
boolean_t CIconImage::m_fDragging = FALSE;  // indicates whether in DRAG MODE
HCURSOR CIconImage::m_cursor_no_access;  // cursor at start of drag operation
LPARAM CIconImage::m_lParam_holder;  // LPARAM carried through the drag operation
boolean_t CIconImage::m_check_subdir = FALSE;

//
//-----------------------------------------------------------------------------
//


// item_filename = a path fragment + filename that does not begin with a slash
// nor have an extentsion.  This path fragment is appended to HD_DATA
// e.g., HD_DATA\\<item_filename>.ico
CIconImage* CIconImage::load_images(CString item_filename, CString item_title /*=""*/,
                                    LPARAM lParam /* = 0 */,
                                    boolean_t always_add /*=FALSE*/)
{
   int i;
   CString t;
   HICON tIcon;
   ICONINFO iconInfo;
   BITMAP iconBitmap;
   CIconImage* already_loaded;
   CIconImage* new_image;
   CString icon_dir;

   // start by checking for this file in the already-loaded list
   if (!always_add && m_loaded_images.Lookup(item_filename, (CObject*&)already_loaded))
      return already_loaded;
      
   // create a new entry
   new_image = new CIconImage;
   
   if (!new_image)
      return NULL;
   
   // Either set the title (if specified) or derive the title
   // from the filename.
   if (item_title == "")
   {
      int title_start = item_filename.ReverseFind('\\') + 1;
      int title_end   = item_filename.ReverseFind('.');
      
      if (title_start < 0  ||  title_start > item_filename.GetLength())
         title_start = 0;
      
      ASSERT(title_start >= 0); 
      ASSERT(title_end > 0);  
      ASSERT(title_end > title_start);  //at least one character
      ASSERT(title_end < 255 && title_start < 255);  //at least one character
      
      // extract the actual title portion
      new_image->m_title = item_filename.Mid(title_start, title_end - title_start);
   }
   else
      new_image->m_title = item_title;
   
   // set the LPARAM value and filename
   new_image->m_lParam = lParam;
   new_image->m_filename = item_filename;

   if (::GetFileAttributes(item_filename) != INVALID_FILE_ATTRIBUTES)
      new_image->m_full_pathname = item_filename;
   else
      new_image->m_full_pathname = PRM_get_registry_string("Main", "HD_DATA") + "\\icons\\" + item_filename;
   
   // now that we've derived the entire filename, grab the portion that
   // points at the icon directory -- sans icon filename
   icon_dir = new_image->m_full_pathname.Left(new_image->m_full_pathname.ReverseFind('\\'));

   // store the exists flag of the last checked directory, so that we do not
   // have to check this for each icon in the directory!
   static CString g_last_icon_dir;
   static int g_last_icon_dir_status;

   if (g_last_icon_dir != icon_dir)
   {
      g_last_icon_dir = icon_dir;
      g_last_icon_dir_status = FIL_access(icon_dir, FIL_EXISTS);
   }

   if (g_last_icon_dir_status != SUCCESS)
   {
      // if the directory does not exist, then we must look  for an alias
      // for this directory... if THAT does not exist, then we present
      // an error log entry and return IDI_TI_UNKNOWN icons
      //
      // NOTE:-----------------------------------------------------------
      // If we got this far, this means that there does not already
      // exist a loaded iconimage corresponding to this potential
      // alias. If there is, it will return "naturally" from the Lookup
      // done earlier in this function. THEREFORE, we will not scan
      // the list of CIconImages associated with the alias directory --
      // should one be found. We will create one from the return
      // of loading the "original" CIconImage.
      POSITION pos_alias = m_aliases.GetHeadPosition();
      CIconAlias* this_alias;
      CIconImage* temp_alias_cicon;
      CString orig_name;
      CString filename;
      CString old_alias_filename;
      int alias_len;
      CString dir_part;

      dir_part = icon_dir.Right(icon_dir.GetLength()-icon_dir.ReverseFind('\\')-1);
      while (pos_alias)
      {
         this_alias = (CIconAlias*)m_aliases.GetNext(pos_alias);
         ASSERT(this_alias);

         if (this_alias->m_alias.CompareNoCase(dir_part) == 0)
         {
            alias_len = dir_part.GetLength();

            // find the corresponding entry with the new original 
            filename = item_filename.Right(item_filename.GetLength() - item_filename.ReverseFind('\\'));
            orig_name = this_alias->m_orig + filename;

            // find the original CIconImage (or load it if necessary)
            // temp_alias_cicon = new CIconImage;
            temp_alias_cicon = load_images(orig_name, item_title, NULL);

            // copy the HICONs from the alias parent
            new_image->m_mega_icon = temp_alias_cicon->m_mega_icon;
            new_image->m_large_icon = temp_alias_cicon->m_large_icon;
            new_image->m_small_icon = temp_alias_cicon->m_small_icon;

            // add this to the alias list
            this_alias->m_alias_list.AddTail(new_image);
            
            // put the new icon in the mapped list
            m_loaded_images.SetAt(item_filename, (CObject*)new_image);
            
            return new_image;
         }
      }

      // If we get this far, then that means that there was a failure in loading
      // even an aliased directory. Create a new CIconImage and load it down 
      // with IDI_TI_UNKNOWN icons.
      new_image->m_title = item_title;
      new_image->m_filename = item_filename;
      new_image->m_mega_icon = AfxGetApp()->LoadIcon(IDI_TI_UNKNOWN);
      new_image->m_large_icon = new_image->m_mega_icon;
      new_image->m_small_icon = new_image->m_mega_icon;

      ERR_report("Unable to load requested icon: "+item_filename);
      
      // put the new icon in the mapped list
      m_loaded_images.SetAt(item_filename, (CObject*)new_image);
      
      return new_image;
   }
   
   HICON      icons[3] = {NULL, NULL, NULL};
   new_image->m_large_icon = new_image->m_small_icon = new_image->m_mega_icon = NULL;
   
   // load all three supported icon sizes - if they fail loading, we will 
   // end up loading the unknown icon below
   icons[0] = (HICON)LoadImage(NULL, new_image->m_full_pathname, IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
   icons[1] = (HICON)LoadImage(NULL, new_image->m_full_pathname, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
   icons[2] = (HICON)LoadImage(NULL, new_image->m_full_pathname, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
   
   // step through each icon and filter it
   for (i = 0; i < 3; i++)
   {
      tIcon = icons[i];
      
      if (!tIcon)
         continue;
      
      // continue you can't get the icon info, then don't support this HICON
      if (GetIconInfo(tIcon, &iconInfo))
      {
         // if you can't get the object info for this BITMAP, then don't support it
         if (GetObject((HGDIOBJ)iconInfo.hbmColor, sizeof(BITMAP), (void*)&iconBitmap))
         {
            int size = iconBitmap.bmWidth;
            
            // disperse this icon image
            switch (size)
            {
            case 16:
               new_image->m_small_icon = tIcon;
               break;
               
            case 32:
               new_image->m_large_icon = tIcon;
               break;
               
            case 48:
               new_image->m_mega_icon = tIcon;
               break;
            }
         }
         
         // delete the HBITMAP resources that GetIconInfo creates
         DeleteObject((HGDIOBJ)iconInfo.hbmColor);
         DeleteObject((HGDIOBJ)iconInfo.hbmMask);
      }
   }
   
   // if we didn't get a valid large icon, then load the unknown icon
   if (!new_image->m_large_icon)
   {
      new_image->m_large_icon = AfxGetApp()->LoadIcon(IDI_TI_UNKNOWN);
   }
   
   // if we don't have a small, then assign the large
   if (!new_image->m_small_icon)
      new_image->m_small_icon = new_image->m_large_icon;
   
   // if we don't have a mega, then assign the large
   if (!new_image->m_mega_icon)
      new_image->m_mega_icon = new_image->m_large_icon;
   
   
   // put the new icon in the mapped list - make sure the key is unique.
   // If the key already exists
   if (m_loaded_images.Lookup(item_filename, (CObject*&)already_loaded))
   {
      // force a unique key, otherwise we will get memory leaks when
      // cleanup is called
      static int increment_key = 0;
      CString key;
      key.Format("%s%d.ico", 
         item_filename.Left(item_filename.GetLength() - 4), increment_key++);

      m_loaded_images.SetAt(key, (CObject*)new_image);
   }
   else
      m_loaded_images.SetAt(item_filename, (CObject*)new_image);
   
   return new_image;
}


//
//-----------------------------------------------------------------------------
//

HICON CIconImage::get_icon(int icon_size)
{
   switch (icon_size)
   {
   case 16:
      return m_small_icon;
      
   case 32:
      return m_large_icon;
      
   case 48:
      return m_mega_icon;
   }
   
   return NULL;
}


//
//-----------------------------------------------------------------------------
//

HICON CIconImage::set_icon(int icon_size, HICON hicon )
{
   switch (icon_size)
   {
   case 16:
      m_small_icon = hicon;
      return m_small_icon;
      
   case 32:
      m_large_icon = hicon;
      return m_large_icon;
      
   case 48:
      m_mega_icon = hicon;
      return m_mega_icon;
   }
   
   return NULL;
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::draw_selected(CDC *dc, int x, int y, int icon_size)
{
   // draw the border
   display_bounds_handles(dc, x, y, icon_size, FALSE);
   
   // draw the icon image itself
   this->draw_icon(dc, x, y, icon_size);
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::draw_hilighted(CDC *dc, int x, int y, int icon_size)
{
   // draw the image background
   GRA_draw_hilight_rect(dc, x, y, icon_size);
   
   // draw the icon image itself
   draw_icon(dc, x, y, icon_size);
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::invalidate(int x, int y, int icon_size)
{
   CRect   inval_rect(get_invalidate_rect(x, y, icon_size));
   OVL_get_overlay_manager()->invalidate_rect(inval_rect);
}


//
//-----------------------------------------------------------------------------
//

CRect CIconImage::get_invalidate_rect(int x, int y, int icon_size)
{
   // we must invalidate the whole icon + half of any present handle + 1
   int   rect_size = (icon_size / 2) + (GRA_handle_size() / 2) + 1;
   CRect inval_rect(x - rect_size, y - rect_size, x + rect_size, y + rect_size);
   
   return inval_rect;
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::draw_dev_indep_icon(CDC *dc, int x, int y, int width,
                                    int height, HICON icon, int new_size /*-1*/)
{
   // if the icon size changed, we need to rebuild the icons
   if (m_di_icon.m_and_mask.get_width() != width || 
       m_di_icon.m_and_mask.get_height() != height)
   {
      m_di_icon.m_and_mask.clear();
      m_di_icon.m_xor_mask.clear();

      m_di_icon_created = FALSE;
   }

   //
   // if the icon substitute has not been created, try to create it.
   //
   if (!m_di_icon_created)
   {
      //
      // if the create for this icon has previously failed, return
      //
      if (m_di_icon_create_failed)
         return FAILURE;
      
      //
      // otherwise try to create the icon substitute
      //
      if (m_di_icon.create(icon) != SUCCESS)
      {
         ERR_report("create failed");
         m_di_icon_create_failed = TRUE;
         return FAILURE;
      }
      
      //
      // make sure to set the loaded flag
      //
      m_di_icon_created = TRUE;
   }

   //
   //  draw the icon substitute
   //
   if (m_di_icon.draw(dc, x, y, width, height, m_drawing_mode == ICON_HALFTONE, new_size, new_size) != SUCCESS)
   {
      return FAILURE;
   }
   
   return SUCCESS;
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::draw_icon(CDC *dc, int x, int y, int icon_size, int new_size /*-1*/)
{
   HICON      drawIcon;
   
   switch (icon_size)
   {
   case 16:
      drawIcon = m_small_icon;
      break;
      
   case 32:
      drawIcon = m_large_icon;
      break;
      
   case 48:
      drawIcon = m_mega_icon;
      break;
      
   default:
      drawIcon = m_large_icon;
      break;
   }
   
   //
   // If the icon is not being printed to the screen, use a device-independent
   // icon substitute.
   //
   // Also, if the icon is being drawn to the screen, but you are in animation
   // mode, then use a device-independent icon substitute.  This is done because
   // on Windows 95 in non-256 color mode (e.g. 16 bit or true color) in
   // animation mode, transparency is hosed.  That is, doing transparency in
   // a dib section in a memory dc doesn't work.  It also doesn't work in 256
   // color mode without making all of the non-system-color palette entries 
   // unique from the system color entries.  The Microsoft tech support guy
   // claims he got it to screw up on NT 4.0 also, although I have never seen 
   // this personally.
   //
   // Use a device-independent icon when drawing to a memory bitmap also because
   // on at least 95 and 98 in 16-bit and  true-color mode, the transparency seems 
   // to sometimes not work correctly when doing Edit Copy Map.
   //
   // In other words, the icon needs to be drawn as device-independent when:
   // 1) printing
   // 2) in animation mode
   // 3) edit copy map
   // 4) map server CreateMap call
   //
   if (dc->IsPrinting() || dc->GetWindow() == NULL)
   {
      draw_dev_indep_icon(dc, x - (icon_size / 2), y - (icon_size / 2),
         icon_size, icon_size, drawIcon, new_size);
      return;
   }
   
   DrawIconEx(dc->m_hDC, x - (icon_size / 2), y - (icon_size / 2), drawIcon,
      icon_size, icon_size, 0, 0, DI_NORMAL);
}


// Get a rect of the active area of the D.I. icon substitute
const CRect* CIconImage::get_di_icon_active_rect()
{
   if ( m_di_icon_created )
   {
      if ( m_di_icon_active_rect.bottom < 0 )
      {
         const BITMAPINFOHEADER&  bmih = m_di_icon.m_and_mask.get_bitmap_info().bmiHeader;
         m_di_icon_active_rect.SetRect(
            bmih.biWidth, bmih.biHeight, 0, 0 );

         int idx = 0;
         PBYTE pbMask = m_di_icon.m_and_mask.get_mask();
         for ( int iY = 0; iY < bmih.biHeight; iY++ )
         {
            for ( int iX = 0; iX < bmih.biWidth; iX++, idx += bmih.biBitCount )
            {
               static const BYTE bits[] =
                  { 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7 };
               if ( 0 == ( pbMask[ idx / 8 ] & bits[ idx & 0x7 ] ) )
               {
                  if ( iX < m_di_icon_active_rect.left )
                     m_di_icon_active_rect.left = iX;
                  if ( iX > m_di_icon_active_rect.right )
                     m_di_icon_active_rect.right = iX;
                  if ( iY < m_di_icon_active_rect.top )
                     m_di_icon_active_rect.top = iY;
                  if ( iY > m_di_icon_active_rect.bottom )
                     m_di_icon_active_rect.bottom = iY;
               }
            }
         }
      }

      // Only return rectangle if at least one non-transparent pixel
      if ( m_di_icon_active_rect.bottom >= m_di_icon_active_rect.top )
         return &m_di_icon_active_rect;
   }
   return NULL;
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::clean_up()
{
   CIconImage*      temp_image;
   POSITION         pos;
   CString         key;
   
   // Iterate through the entire map deleting the object
   for( pos = m_loaded_images.GetStartPosition(); pos != NULL; )
   {
      m_loaded_images.GetNextAssoc(pos, key, (CObject*&)temp_image);
      if (temp_image)
      {
         DestroyIcon(temp_image->m_mega_icon);
         DestroyIcon(temp_image->m_large_icon);
         DestroyIcon(temp_image->m_small_icon);

         delete temp_image;
      }
   }
   m_loaded_images.RemoveAll();

   // loop through all aliases and remove their objects
   // current alias_dir
   POSITION pos_alias = m_aliases.GetHeadPosition();
   CIconAlias* this_alias;

   while (pos_alias)
   {
      this_alias = (CIconAlias*)m_aliases.GetNext(pos_alias);
      ASSERT(this_alias);
      if (this_alias)
         delete this_alias;
   }
   m_aliases.RemoveAll();
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::begin_drag(ViewMapProj* map, CPoint point, LPARAM lParam)
{
   CWindowDC dc(map->get_CView());
   CWnd* view = dc.GetWindow();

   // empty any existing drag image list
   if (m_drag_image  &&  m_drag_image->GetSafeHandle())
      m_drag_image->DeleteImageList();

   // create the drag image list
   m_drag_image = new CImageList;
   if (!m_drag_image->Create(32, 32, TRUE, 1, 0))
      return FAILURE;

   // add the icon for the current CIconImage (large icon)
   if (m_drag_image->Add(m_large_icon) == -1)
      return FAILURE;

   // grab the current cursor and no drop cursor
   m_cursor_no_access = ::LoadCursor(NULL, IDC_NO);

   // get the absolute screen coordinates
   CPoint pt(point);
   view->ClientToScreen(&pt);

   // begin the drag, offset from bottom left to center
   VERIFY(m_drag_image->BeginDrag(0, CPoint(16, 16)));
   VERIFY(m_drag_image->DragEnter(CWnd::GetDesktopWindow(), pt));

   // capture the mouse for this application
   view->SetCapture();

   // set our flag for the mouse move functionality
   m_fDragging = TRUE;

   // indicate that the cursor is not currently shown
   m_cursor_showing = FALSE;

   // keep the caller's LPARAM value
   m_lParam_holder = lParam;

   return SUCCESS;
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::move_drag(ViewMapProj* map, CPoint point)
{
   CWindowDC dc(map->get_CView());
   CWnd* view = dc.GetWindow();

   if (m_fDragging)
   {
      // get the absolute screen coordinates
      CPoint pt(point);
      view->ClientToScreen(&pt);
      
      // copy to the member we'll use for mouse move
      CPoint move_pt(pt);
      
      CWnd* pDropWnd = view->WindowFromPoint(pt);
      ASSERT(pDropWnd);
      
      // if this is the list control window or the mapview window
      // then drag our droppable image
      
      if (pDropWnd->m_hWnd == UTL_get_active_non_printing_view()->m_hWnd)
      {
         // if the cursor is showing, then disable it
         if (m_cursor_showing)
         {
            // show the MapView's current cursor now
            SetCursor(fvw_get_view()->get_cursor());
            m_cursor_showing = FALSE;
         }
         
      }
      // otherwise drag a "no drop" cursor around
      //
      else
      {
         if (!m_cursor_showing)
         {
            SetCursor(m_cursor_no_access);
            m_cursor_showing = TRUE;
         }
      }
      
      // move the drag image
      VERIFY(m_drag_image->DragMove(move_pt));
   }
   return SUCCESS;
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::end_drag(ViewMapProj* map, CPoint point, LPARAM* lParam)
{
   CWindowDC dc(map->get_CView());
   CWnd* view = dc.GetWindow();

   if (m_fDragging)
   {
      // release mouse capture
      VERIFY(::ReleaseCapture());
      
      // end dragging
      VERIFY(m_drag_image->DragLeave(view->GetDesktopWindow()));
      m_drag_image->EndDrag();
      
      // reset the cursor to the MapView's cursor
      SetCursor(fvw_get_view()->get_cursor());
      ShowCursor(TRUE);
      
      // get the absolute screen coordinates
      CPoint pt(point);
      view->ClientToScreen(&pt);
      
      // get the CWnd pointer of the window that is under the mouse cursor
      CWnd* pDropWnd = view->WindowFromPoint(pt);
      ASSERT(pDropWnd);
      
      // convert from screen coordinates to drop target client coordinates
      pDropWnd->ScreenToClient(&pt);
      
      // reset our flag
      m_fDragging = FALSE;
      
      // remove our imagelist
      delete m_drag_image;
      m_drag_image = NULL;
      
      // if we are over the main view window, return LPARAM and success
      if (pDropWnd->m_hWnd == fvw_get_view()->m_hWnd())
      {
         if (lParam)
            *lParam = m_lParam_holder;
         return SUCCESS;
      }
      else
         return FAILURE;
   }
   return
      FAILURE;
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::cancel_drag(ViewMapProj* map)
{
   CWindowDC dc(map->get_CView());
   CWnd* view = dc.GetWindow();

   if (m_fDragging)
   {
      // release mouse capture
      VERIFY(::ReleaseCapture());
      
      // end dragging
      VERIFY(m_drag_image->DragLeave(view->GetDesktopWindow()));
      m_drag_image->EndDrag();
      
      // reset the cursor
      SetCursor(fvw_get_view()->get_cursor());
      ShowCursor(TRUE);
      
      // reset our flag
      m_fDragging = FALSE;
      
      // remove our imagelist
      delete m_drag_image;
      m_drag_image = NULL;
      
      return SUCCESS;
   }
   return FAILURE;
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::hide_drag()
{
   if (m_fDragging)
   {
      // hide dragging
      VERIFY(m_drag_image->DragShowNolock(FALSE));
      return SUCCESS;
   }
   return FAILURE;
}


//
//-----------------------------------------------------------------------------
//

int CIconImage::show_drag()
{
   if (m_fDragging)
   {
      // show dragging
      VERIFY(m_drag_image->DragShowNolock(TRUE));
      return SUCCESS;
   }
   return FAILURE;
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::display_bounds_handles(CDC* dc, int x, int y, 
                                        int icon_size, BOOL filled)
{
   CRect      m_bounds(x - (icon_size / 2),     y - (icon_size / 2),
      x + (icon_size / 2) - 1, y + (icon_size / 2) - 1);
   
   GRA_draw_select_box(dc, m_bounds, TRUE);
}



//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//

UINT CIconImage::preload_icons(LPVOID lParam)
{
   load_dir_icons("ovlmgr");
   load_dir_icons("system");
   load_dir_icons("user");

   return 0;
}


//
//-----------------------------------------------------------------------------
//

void CIconImage::load_dir_icons(CString item_directory)
{
   CString temp_fn;
   WIN32_FIND_DATA findFileData;
   CString new_title;
   CString icon_root = PRM_get_registry_string("Main", "HD_DATA")+
      "\\icons\\";
   
   // get the first filename match
   HANDLE h_dirSearch = 
      FindFirstFile(icon_root+item_directory + "\\*.ico", &findFileData);
   
   if (h_dirSearch == INVALID_HANDLE_VALUE)
   {
      LPVOID lpMsgBuf;
      
      // output the error return message and return FAILURE
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
         (LPTSTR) &lpMsgBuf, 0, NULL);
      ERR_report((char*)lpMsgBuf);
      ERR_report(item_directory);
      return;
   }
   
   do
   {
      // get the filename used for the icon load
      temp_fn = item_directory+"\\"+findFileData.cFileName;
      
      // create a new object for this icon load
      CIconImage::load_images(temp_fn);
      
   } while (FindNextFile(h_dirSearch, &findFileData));
   
   FindClose(h_dirSearch);
   return;
}


//
//-----------------------------------------------------------------------------
//

// used to establish aliases so that we can refer, for example, to "chum\*.ico"
// when the actual files are "chumred\*.ico". This allows us to maintain
// consistant filenames (ie., "chum") regardless of some user presentation
// switch (ie., "red" or "blue" based). 
//
// NOTE: When this function is called it loops through all icons looking for 
// m_alias_dir = alias_dir, and replaces the HICON's with IDI_TI_UNKNOWN. Then 
// it scans for all EXISTING icons whose name is orig_dir\*.ico
// 
int CIconImage::set_alias(CString alias_dir, CString orig_dir)
{
   // loop through all current definitions to see if any exist for the
   // current alias_dir
   POSITION pos_alias = m_aliases.GetHeadPosition();
   POSITION pos_cicons;
   CIconAlias* this_alias;
   CIconImage* temp_alias_cicon;
   CIconImage* temp_orig_cicon;
   CString orig_name;
   CString filename;
   CString old_alias_filename;
   int alias_len = alias_dir.GetLength();


   while (pos_alias)
   {
      this_alias = (CIconAlias*)m_aliases.GetNext(pos_alias);
      ASSERT(this_alias);

      if (this_alias->m_alias.CompareNoCase(alias_dir) == 0)
      {
         // change the association value so that this alias now points at the
         // new orig_dir
         this_alias->m_orig = orig_dir;

         // now loop through the alias list doing lookups on each
         // of it's members
         pos_cicons = this_alias->m_alias_list.GetHeadPosition();

         while (pos_cicons)
         {
            temp_alias_cicon = (CIconImage*)this_alias->m_alias_list.GetNext(pos_cicons);
            ASSERT(temp_alias_cicon);

            // find the corresponding entry with the new original 
            old_alias_filename = temp_alias_cicon->m_filename;
            filename = old_alias_filename.Right(old_alias_filename.GetLength() - alias_len);
            orig_name = orig_dir + filename;

            // find the original CIconImage (or load it if necessary)
            temp_orig_cicon = load_images(orig_name, temp_alias_cicon->m_title, NULL);
                                    
            // copy the HICONS
            temp_alias_cicon->m_mega_icon = temp_orig_cicon->m_mega_icon;
            temp_alias_cicon->m_large_icon = temp_orig_cicon->m_large_icon;
            temp_alias_cicon->m_small_icon = temp_orig_cicon->m_small_icon;
         }

         // we don't need to do anything more since this was already here
         return SUCCESS;
      }
   }

   // if we got this far, then we don't have an alias entry yet, so we'll
   // have to create a map image for it.
   CIconAlias* new_alias = new CIconAlias(orig_dir, alias_dir);
   m_aliases.AddTail(new_alias);

   return SUCCESS;
}


//
//-----------------------------------------------------------------------------
//

// used to establish aliases so that we can refer, for example, to "chum\*.ico"
// when the actual files are "chumred\*.ico". This allows us to maintain
// consistant filenames (ie., "chum") regardless of some user presentation
// switch (ie., "red" or "blue" based). 
//
// NOTE: When this function is called it loops through all icons looking for 
// m_alias_dir = alias_dir, and replaces the HICON's with IDI_TI_UNKNOWN. Then 
// it scans for all EXISTING icons whose name is orig_dir\*.ico
// 
CString CIconImage::get_original_directory(CString alias_dir)
{
   // loop through all current definitions to see if any exist for the
   // current alias_dir
   POSITION pos_alias = m_aliases.GetHeadPosition();
   CIconAlias* this_alias;
   CString orig_name;
   CString filename;
   CString old_alias_filename;
   int alias_len = alias_dir.GetLength();


   while (pos_alias)
   {
      this_alias = (CIconAlias*)m_aliases.GetNext(pos_alias);
      ASSERT(this_alias);

      if (this_alias->m_alias.CompareNoCase(alias_dir) == 0)
         return this_alias->m_orig;
   }

   return "";
}


//
//-----------------------------------------------------------------------------
//

#ifdef _DEBUG

// routine that will loop through all icons that are found in the current
// map table and dump them to the debug window
void CIconImage::dump_list()
{
   POSITION pos = m_loaded_images.GetStartPosition();
   CIconImage* current_icon;
   CString current_name;
   CString dump_string;

   OutputDebugString("-------------------------------\n");
   OutputDebugString("\n");
   OutputDebugString("Icon mappings:\n\n");

   while (pos)
   {
      // grab the current entry
      m_loaded_images.GetNextAssoc(pos, current_name, (CObject*&)current_icon);

      dump_string.Format("%s\t%s\n", current_name, current_icon->m_filename);
      OutputDebugString(dump_string);
   }

   OutputDebugString("\n");
   OutputDebugString("-------------------------------\n");
   OutputDebugString("\n");
}

#endif
