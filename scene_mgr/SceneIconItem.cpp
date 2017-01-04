// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// SceneIconItem.cpp: implementation of the CSceneIconItem class.
//
//////////////////////////////////////////////////////////////////////

// stdafx first
#include "stdafx.h" 

// this file's header
#include "SceneIconItem.h"

// system includes

// third party files

// other FalconView headers
#include "geotrans.h"
#include "MAP.H"

// this project's headers
#include "scene_cov_ovl.h"
#include "SDSWrapper.h"


namespace scene_mgr
{

//////////////////////////////////////////////////////////////////////
// CGeoRect
//////////////////////////////////////////////////////////////////////

void CGeoRect::MapToDeviceWindow(MapProj* map, CRect* pRect)
{
   int ul_x, ul_y, lr_x, lr_y;
   map->geo_to_surface(m_ll_lat, m_ll_lon, &ul_x, &lr_y);
   map->geo_to_surface(m_ur_lat, m_ur_lon, &lr_x, &ul_y);

   // normalize the coordinates if the tile wrapped around the world
   if (ul_x > lr_x) 
   {
      int pixels_around_world;
      map->get_pixels_around_world(&pixels_around_world);

      if (lr_x < 0)
         lr_x += pixels_around_world;
      else
         ul_x -= pixels_around_world;
   }

   pRect->left   = ul_x;
   pRect->top    = ul_y;
   pRect->right  = lr_x;
   pRect->bottom = lr_y;
}

void CGeoRect::MapFromDeviceWindow(map_projection_utils::CMapProjWrapper* map, CRect* pRect)
{
   map->surface_to_geo(pRect->left, pRect->top, &m_ur_lat, &m_ll_lon);
   map->surface_to_geo(pRect->right, pRect->bottom, &m_ll_lat, &m_ur_lon);
}

void CGeoRect::Shrink(CRect* pRect, int weight, int width, int height)
{
   int shrink_factor;

   /* number of pixels the rectangle corners are moved in */
   shrink_factor = 1 + weight/2;

   /* move x coordinates closer together shrink_factor pixels each,
      if not possible then put pRect->left and pRect->right at ~midpoint */
   if (pRect->left < (pRect->right - 2 * shrink_factor))
   {
      pRect->left += shrink_factor;
      pRect->right -= shrink_factor;
   }
   else
   {
      pRect->left += (pRect->right - pRect->left)/2;
      pRect->right = pRect->left;
   }

   /* move y coordinates closer together shrink_factor pixels each,
      if not possible then put pRect->top and pRect->bottom at ~midpoint */
   if (pRect->top < (pRect->bottom - 2 * shrink_factor))
   {
      pRect->top += shrink_factor;
      pRect->bottom -= shrink_factor;
   }
   else
   {
      pRect->top += (pRect->bottom - pRect->top)/2;
      pRect->bottom = pRect->top;
   }

   /* clip shrunk rectangle against the screen boundaries such that sides
      of the shrunk rectangle that won't show will result in sides of the
      "clipped" rectangle that won't show. */
   if (pRect->left < -weight)
      pRect->left = -weight;

   if (pRect->top < -weight)
      pRect->top = -weight;

   if (pRect->right >= width+weight)
      pRect->right = width+weight;

   if (pRect->bottom >= height+weight)
      pRect->bottom = height+weight;
}

// Return this geo as a string of comma-separated numbers
CString CGeoRect::Format(int iCode/*=CGeoRect::LLUR*/)
{
   const int BUF_LEN = 255;
   char buffer[BUF_LEN];
   CString sResult = "Cannot convert";
   CString sTmp1, sTmp2;
   CGeoTrans geotrans;
   degrees_t kPrecision = 0.001;

   switch (iCode)
   {
      case CGeoRect::LLUR:
         if (geotrans.DLL_lat_lon_to_geo(m_ll_lat, m_ll_lon, kPrecision, buffer, BUF_LEN) == FAILURE)
            break;
         sTmp1 = buffer;
         if (geotrans.DLL_lat_lon_to_geo(m_ur_lat, m_ur_lon, kPrecision, buffer, BUF_LEN) == FAILURE)
            break;
         sTmp2 = buffer;
         sResult = sTmp1 + " --- " + sTmp2;
         break;
      case CGeoRect::LL:
         if (geotrans.DLL_lat_lon_to_geo(m_ll_lat, m_ll_lon, kPrecision, buffer, BUF_LEN) == SUCCESS)
            sResult = buffer;
         break;
      case CGeoRect::UR:
         if (geotrans.DLL_lat_lon_to_geo(m_ur_lat, m_ur_lon, kPrecision, buffer, BUF_LEN) == SUCCESS)
            sResult = buffer;
         break;
   }

   return sResult;
}

//////////////////////////////////////////////////////////////////////
// CSceneIconItem
//////////////////////////////////////////////////////////////////////

CSceneIconItem::CSceneIconItem(C_overlay *overlay) : C_icon(overlay)
{
   m_bbox.Zero();
   m_sourceID = -1;
   m_sceneTypeID = -1;
   m_flags = 0;
}

CSceneIconItem::~CSceneIconItem()
{
}

void CSceneIconItem::Draw(MapProj* map, CDC* pDC)
{
   COLORREF color;
   int style;
   int weight;
   CRect rect;
   int x_pixels_around_world;
   int width, height;

   // Determine color: should be green if on target device, bright if selected
   if (m_flags & SM_CATALOG)
      color = (m_flags & SM_SELECTED) ? sm_bright_yellow : sm_yellow;
   else if (m_flags & SM_TARGET)
      color = (m_flags & SM_SELECTED) ? sm_bright_green : sm_green;
   else
      color = (m_flags & SM_SELECTED) ? sm_bright_red : sm_red;

   // Determine style: back hatch if overlapped (even/odd flag)
   style = (m_flags & SM_CATALOG) ? HS_VERTICAL : ((m_flags & SM_ODD) ? HS_BDIAGONAL : HS_FDIAGONAL);

   // Determine line weight based on map view scale
   weight = (map->scale() >= ONE_TO_5M) ? 3 : 1;

   // Handle the case where m_bbox causes a degenerate rectangle.
   // This can happen on Tiros World.

   if (m_bbox.m_ll_lon == -180 && m_bbox.m_ur_lon == 180)
      m_bbox.m_ll_lon = -179;

   // Transform rectangle into screen space and shrink to accommodate line weight
   m_bbox.MapToDeviceWindow(map, &rect);

   // Get pixels around world to see if rectangles must be wrapped
   map->get_pixels_around_world(&x_pixels_around_world);
   map->get_surface_size(&width, &height);

   // Handle special cases for tiny rectangles
   if ( ((rect.right - rect.left + 1) < 4) &&
      ((rect.bottom - rect.top + 1) < 4) )
   {  // Draw solid rectangle if too small to look good
      CBrush brush(color);
      CRect fixedRect(rect.left, rect.top, rect.left+4, rect.top+4);
      pDC->FillRect(&fixedRect, &brush);
   }
   else     // draw rectangle using pattern brush
   {
      int drawBrushStyle = sm_getDrawBrushStyle();
      if (drawBrushStyle == SM_DRAWBRUSHSTYLE_HOLLOW)// draw using minimal tools
      {
         CBrush brush;
         brush.CreateStockObject(NULL_BRUSH);

         CPen pen(PS_SOLID, weight, color);

         CPen* oldpen = pDC->SelectObject(&pen);
         CBrush* oldbrush = pDC->SelectObject(&brush);
         int oldbkmode = pDC->SetBkMode(TRANSPARENT);

         if (rect.left < 0 && rect.right > width &&
               rect.top < 0  && rect.bottom > height )
         {
            // Draw simple screen sized rectangle if rectangle
            // fully encloses view window.
            pDC->Rectangle(-10, -10, width+20, height+20);  // add margins to hide edge
         }
         else if (rect.right + width < rect.left + x_pixels_around_world)  // See if need to wrap edges
         {
            // Draw normal rectangle
            m_bbox.Shrink(&rect, weight, width, height);
            pDC->Rectangle(rect.left, rect.top, rect.right+1, rect.bottom+1);
         }
         else
         {
            // Draw rectangle that wraps around the world
            // draws twice: left edge, right edge
            CRect rect1(rect);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);

            rect1 = rect;
            rect1.OffsetRect(x_pixels_around_world, 0);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
         }

         pDC->SetBkMode(oldbkmode);
         pDC->SelectObject(oldbrush);
         pDC->SelectObject(oldpen);
      }
      else if (drawBrushStyle == SM_DRAWBRUSHSTYLE_HATCH)  // draw using hatch brush tools (for printing)
      {
         CBrush brush(style, color);
         CPen pen(PS_SOLID, weight, color);

         CPen* oldpen = pDC->SelectObject(&pen);
         CBrush* oldbrush = pDC->SelectObject(&brush);
         int oldbkmode = pDC->SetBkMode(TRANSPARENT);

         if (rect.left < 0 && rect.right > width &&
             rect.top < 0  && rect.bottom > height )
         {
            // Draw simple screen sized rectangle if rectangle
            // fully encloses view window.
            pDC->Rectangle(-10, -10, width+20, height+20);  // add margins to hide edge
         }
         else if (rect.right + width < rect.left + x_pixels_around_world)  // See if need to wrap edges
         {
            // Draw normal rectangle
            m_bbox.Shrink(&rect, weight, width, height);
            pDC->Rectangle(rect.left, rect.top, rect.right+1, rect.bottom+1);
         }
         else
         {
            // Draw rectangle that wraps around the world
            // draws twice: left edge, right edge
            CRect rect1(rect);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);

            rect1 = rect;
            rect1.OffsetRect(x_pixels_around_world, 0);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
         }

         pDC->SetBkMode(oldbkmode);
         pDC->SelectObject(oldbrush);
         pDC->SelectObject(oldpen);
      }
      else if (drawBrushStyle == SM_DRAWBRUSHSTYLE_TRANSPARENT)  // draw using transparent tools
      {
         CBrush brush(color);
         CPen pen(PS_SOLID, weight, color);

         CPen* oldpen = pDC->SelectObject(&pen);
         CBrush* oldbrush = pDC->SelectObject(&brush);
         int oldbkmode = pDC->SetBkMode(OPAQUE);
         int oldrop2 = pDC->SetROP2(R2_MERGEPEN);

         if (rect.left < 0 && rect.right > width &&
             rect.top < 0  && rect.bottom > height )
         {
            // Draw simple screen sized rectangle if rectangle
            // fully encloses view window.
            pDC->Rectangle(-10, -10, width+20, height+20);  // add margins to hide edge
         }
         else if (rect.right + width < rect.left + x_pixels_around_world)  // See if need to wrap edges
         {
            // Draw normal rectangle
            m_bbox.Shrink(&rect, weight, width, height);
            pDC->Rectangle(rect.left, rect.top, rect.right+1, rect.bottom+1);
         }
         else
         {
            // Draw rectangle that wraps around the world
            // draws twice: left edge, right edge
            CRect rect1(rect);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);

            rect1 = rect;
            rect1.OffsetRect(x_pixels_around_world, 0);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
         }

         pDC->SetBkMode(oldbkmode);
         pDC->SelectObject(oldbrush);
         pDC->SelectObject(oldpen);
         pDC->SetROP2(oldrop2);
      }
      else if (drawBrushStyle == SM_DRAWBRUSHSTYLE_PATTERN)  // draw using pattern brush tools
      {
         int idMask;
         int idColor;

         int saveDC = pDC->SaveDC();

         if (m_flags & SM_ODD)
         {
            // Determine color: should be green if on target device, bright if selected
            if (m_flags & SM_CATALOG)
               idColor = (m_flags & SM_SELECTED) ? IDB_MDM_CATSEL1 : IDB_MDM_CAT1;
            else if (m_flags & SM_TARGET)
               idColor = (m_flags & SM_SELECTED) ? IDB_MDM_TGTSEL1 : IDB_MDM_TGT1;
            else
               idColor = (m_flags & SM_SELECTED) ? IDB_MDM_SRCSEL1 : IDB_MDM_SRC1;
            idMask  = IDB_MDM_MASK1;
         }
         else
         {
            // Determine color: should be green if on target device, bright if selected
            if (m_flags & SM_CATALOG)
               idColor = (m_flags & SM_SELECTED) ? IDB_MDM_CATSEL2 : IDB_MDM_CAT2;
            else if (m_flags & SM_TARGET)
               idColor = (m_flags & SM_SELECTED) ? IDB_MDM_TGTSEL2 : IDB_MDM_TGT2;
            else
               idColor = (m_flags & SM_SELECTED) ? IDB_MDM_SRCSEL2 : IDB_MDM_SRC2;
            idMask  = IDB_MDM_MASK2;
         }

         // Draws require first masking out old pixels; then merging in new ones.
         // Note that bitmaps used for masking and merging require the following:
         // - White pixels in mask bitmap indicate area that will be transparent.
         // - Black pixels in merge bitmap indicate area that will be transparent.
         CBitmap bmMask, bmColor;
         bmMask.LoadBitmap(idMask);
         bmColor.LoadBitmap(idColor);

         CBrush maskbrush(&bmMask);
         CBrush colorbrush(&bmColor);

         CPen pen(PS_SOLID, weight, color);
         pDC->SelectObject(&pen);

         if (rect.left < 0 && rect.right > width &&
             rect.top < 0  && rect.bottom > height )
         {
            // Draw simple screen sized rectangle if rectangle
            // fully encloses view window... make a bit larger

            CPen pen(PS_NULL, weight, color);
            pDC->SelectObject(&pen);

            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->Rectangle(-10, -10, width+20, height+20);  // add margins to hide edge
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->Rectangle(-10, -10, width+20, height+20);  // add margins to hide edge
         }
         else if (rect.right + width < rect.left + x_pixels_around_world)  // See if need to wrap edges
         {
            // Draw normal rectangle
            m_bbox.Shrink(&rect, weight, width, height);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->Rectangle(rect.left, rect.top, rect.right+1, rect.bottom+1);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->Rectangle(rect.left, rect.top, rect.right+1, rect.bottom+1);
         }
         else
         {
            // Draw rectangle that wraps around the world
            // draws twice: left edge, right edge
            CRect rect1(rect);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);

            rect1 = rect;
            rect1.OffsetRect(x_pixels_around_world, 0);
            m_bbox.Shrink(&rect1, weight, width, height);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
         }

         pDC->RestoreDC(saveDC);

         bmMask.DeleteObject();
         bmColor.DeleteObject();
      }
   }

   // also draw extra thick hollow rectangle if highlighting a catalog icon
   if ((m_flags & SM_CATALOG) && (m_flags & SM_HIGHLIGHTED))
   {
      weight = 4;

      CBrush brush;
      brush.CreateStockObject(NULL_BRUSH);

      COLORREF orange_color = RGB(255,127,0);
      CPen pen(PS_SOLID, weight, orange_color);

      CPen* oldpen = pDC->SelectObject(&pen);
      CBrush* oldbrush = pDC->SelectObject(&brush);
      int oldbkmode = pDC->SetBkMode(TRANSPARENT);

      if (rect.left < 0 && rect.right > width &&
          rect.top < 0  && rect.bottom > height )
      {
         // Draw simple screen sized rectangle if rectangle
         // fully encloses view window.
         pDC->Rectangle(-10, -10, width+20, height+20);  // add margins to hide edge
      }
      else if (rect.right + width < rect.left + x_pixels_around_world)  // See if need to wrap edges
      {
         // Draw normal rectangle (already shrunk no need to repeat)
         //m_bbox.Shrink(&rect, weight, width, height);
         pDC->Rectangle(rect.left, rect.top, rect.right+1, rect.bottom+1);
      }
      else
      {
         // Draw rectangle that wraps around the world (already shrunk no need to repeat)
         // draws twice: left edge, right edge
         CRect rect1(rect);
         //m_bbox.Shrink(&rect1, weight, width, height);
         pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);

         rect1 = rect;
         rect1.OffsetRect(x_pixels_around_world, 0);
         //m_bbox.Shrink(&rect1, weight, width, height);
         pDC->Rectangle(rect1.left, rect1.top, rect1.right+1, rect1.bottom+1);
      }

      pDC->SetBkMode(oldbkmode);
      pDC->SelectObject(oldbrush);
      pDC->SelectObject(oldpen);
   }

}

// required C_icon derived class member function to return a help text string
CString CSceneIconItem::get_help_text()
{
   CString sInfo;

   if (m_flags & SM_CATALOG)
   {
      sInfo.Format("%s %s.", GetPath(), GetFilename());
   }
   else
   {
      CString path = GetPath();

      sInfo.Format("%s %s%s for %s.", IsSelected() ? "Unselect" : "Select",
         path.Right(1) == "\\" ? path : path + "\\",
         GetFilename(), IsTarget() ? "deletion" : "copying");
   }

   return sInfo;
}

// required C_icon derived class member function to return a tool tip string
CString CSceneIconItem::get_tool_tip()
{
   CString sInfo;

   if (m_flags & SM_CATALOG)
   {
      sInfo.Format("%s %s", GetPath(), GetFilename());
   }
   else
   {
      sInfo.Format("%s %s File",
         IsSelected() ? "Selected" : "Unselected",
         IsTarget() ? "Target" : "Source");
   }

   return sInfo;
}

// Determine if the point is over the file icon.
boolean_t CSceneIconItem::hit_test(CPoint point)
{
   return TRUE;
}

// ----------------------------------------------------------------------------

CRegionItem::CRegionItem()
{
   m_bbox.Zero();
   m_mapType = -1;
   m_sourceID = -1;
   m_ID = -1;
   m_type = 0L;

   m_pRgn = new CRgn;
   VERIFY(m_pRgn->CreateRectRgn(0,0,0,0));     // arbitrary initialization
}

CRegionItem::CRegionItem(long ID, long mapType, long sourceID, long lRegionType)
{
   ASSERT(sourceID == 0);  // the sourceID parameter will be removed

   m_bbox.Zero();
   m_ID = ID;
   m_mapType = mapType;
   m_sourceID  = sourceID;
   m_type = lRegionType;

   m_pRgn = new CRgn;
   VERIFY(m_pRgn->CreateRectRgn(0,0,0,0));     // arbitrary initialization
}

CRegionItem::~CRegionItem()
{
   if (m_pRgn && m_pRgn->GetSafeHandle() != NULL)
      if (m_pRgn->DeleteObject())
      {
         delete m_pRgn;    // only call delete if DeleteObject is successful
         m_pRgn = NULL;
      }
}

int CRegionItem::ConvertRegionToBlob(CByteArray& regionBlob)
{
   // Get the size of the region data
   int nBytesNeeded = m_pRgn->GetRegionData(NULL, 0);

   // Put the region data into the CByteArray
   regionBlob.SetSize(nBytesNeeded);
   if (m_pRgn->GetRegionData((RGNDATA*)regionBlob.GetData(), regionBlob.GetSize()) == ERROR)
   {
      ERR_report("ConvertRegionToBlob() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

int CRegionItem::ConvertBlobToRegion(CByteArray& regionBlob, XFORM* pXform /*=NULL*/)
{
   CRgn tmpRgn;

   // CreateFromData using passed (identity by default) matrix
   if (!tmpRgn.CreateFromData(pXform, regionBlob.GetSize(), (RGNDATA*)regionBlob.GetData()))
   {
      ERR_report("ConvertBlobToRegion() failed.");
      return FAILURE;
   }

   m_pRgn->CopyRgn(&tmpRgn);  // copy it into the CRegionItem

   return SUCCESS;
}

int CRegionItem::Draw(MapProj* map, CDC* pDC)
{
   COLORREF color;
   int style;
   int weight;
   int weightframe;
   int bkmode;
   int rop2;
   int idMask;
   int idColor;
   CRect rRgnbounds;

   try
   {
      // Choose color and style based on type
      switch (m_type)
      {
         case SM_UNSELECTED_SOURCE:
            color = sm_red;
            style = HS_FDIAGONAL;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_SRC1;
            break;
         case SM_SELECTED_SOURCE:
            color = sm_bright_red;
            style = HS_FDIAGONAL;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_SRCSEL1;
            break;
         case SM_UNSELECTED_TARGET:
            color = sm_green;
            style = HS_FDIAGONAL;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_TGT1;
            break;
         case SM_SELECTED_TARGET:
            color = sm_bright_green;
            style = HS_FDIAGONAL;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_TGTSEL1;
            break;
         case SM_UNSELECTED_CATALOG:
            color = sm_yellow;
            style = HS_VERTICAL;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_CAT1;
            break;
         case SM_SELECTED_CATALOG:
            color = sm_bright_yellow;
            style = HS_VERTICAL;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_CATSEL1;
            break;
         case SM_HIGHLIGHTED_CATALOG:
         case SM_HIGHLIGHTED_SELECTED_CATALOG:
            color = sm_bright_orange;
            style = HS_VERTICAL;
            weight = 3;
            weightframe = 5;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_CATSEL1;
            break;
         default: // unknown... should never happen
            ASSERT(0);
            color = RGB(80,80,0);
            style = HS_CROSS;
            weight = 1;
            weightframe = 3;
            bkmode = TRANSPARENT;
            rop2 = R2_COPYPEN;
            idMask = IDB_MDM_MASK1;
            idColor = IDB_MDM_SRC2;
      }

      int drawBrushStyle = sm_getDrawBrushStyle();

      m_pRgn->GetRgnBox(&rRgnbounds);  // get the bounds of the region

      if (drawBrushStyle == SM_DRAWBRUSHSTYLE_HOLLOW)// draw using minimal tools
      {
         CBrush brush;
         brush.CreateStockObject(NULL_BRUSH);

         CPen pen(PS_SOLID, weight, color);

         CPen* oldpen = pDC->SelectObject(&pen);
         CBrush* oldbrush = pDC->SelectObject(&brush);
         int oldbkmode = pDC->SetBkMode(TRANSPARENT);

         CBrush solidbrush(color);

         // Get pixels around world to wrap region
         int x_pixels_around_world;
         map->get_pixels_around_world(&x_pixels_around_world);

         if (rRgnbounds.left > 0 && rRgnbounds.right > 0)
         {
            // if bounds towards right side
            // draw left side, right side
            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);

            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);
         }
         else
         {
            // if bounds towards left side
            // draw right side, left side
            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);

            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);
         }

         pDC->SetBkMode(oldbkmode);
         pDC->SelectObject(oldbrush);
         pDC->SelectObject(oldpen);
      }
      else if (drawBrushStyle == SM_DRAWBRUSHSTYLE_HATCH)  // draw using hatch brush tools (for printing)
      {
         CBrush brush(style, color);
         CPen pen(PS_SOLID, weight, color);

         CPen* oldpen = pDC->SelectObject(&pen);
         CBrush* oldbrush = pDC->SelectObject(&brush);
         int oldbkmode = pDC->SetBkMode(TRANSPARENT);

         CBrush solidbrush(color);

         // Get pixels around world to wrap region
         int x_pixels_around_world;
         map->get_pixels_around_world(&x_pixels_around_world);

         if (rRgnbounds.left > 0 && rRgnbounds.right > 0)
         {
            // if bounds towards right side
            // draw left side, right side
            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);

            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);
         }
         else
         {
            // if bounds towards left side
            // draw right side, left side
            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);

            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);
         }

         pDC->SetBkMode(oldbkmode);
         pDC->SelectObject(oldbrush);
         pDC->SelectObject(oldpen);
      }
      else if (drawBrushStyle == SM_DRAWBRUSHSTYLE_TRANSPARENT)  // draw using transparent tools
      {
         int saveDC = pDC->SaveDC();

         bkmode = OPAQUE;
         rop2 = R2_MERGEPEN;

         CBrush brush(color);
         CPen pen(PS_SOLID, weight, color);

         pDC->SetBkMode(bkmode);
         pDC->SetROP2(rop2);

         pDC->SelectObject(&brush);
         pDC->SelectObject(&pen);

         // Get pixels around world to wrap region
         int x_pixels_around_world;
         map->get_pixels_around_world(&x_pixels_around_world);

         if (rRgnbounds.left > 0 && rRgnbounds.right > 0)
         {
            // if bounds towards right side
            // draw left side, right side
            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);

            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);
         }
         else
         {
            // if bounds towards left side
            // draw right side, left side
            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);

            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->PaintRgn(m_pRgn);
         }

         pDC->RestoreDC(saveDC);
      }
      else if (drawBrushStyle == SM_DRAWBRUSHSTYLE_PATTERN)  // draw using pattern brush tools
      {
         int saveDC = pDC->SaveDC();

         // Draws require first masking out old pixels; then merging in new ones.
         // Note that bitmaps used for masking and merging require the following:
         // - White pixels in mask bitmap indicate area that will be transparent.
         // - Black pixels in merge bitmap indicate area that will be transparent.
         CBitmap bmMask, bmColor;
         bmMask.LoadBitmap(idMask);
         bmColor.LoadBitmap(idColor);

         CBrush maskbrush(&bmMask);
         CBrush colorbrush(&bmColor);

         CPen pen(PS_SOLID, weight, color);
         pDC->SelectObject(&pen);

         CBrush solidbrush(color);

         // Get pixels around world to wrap region
         int x_pixels_around_world;
         map->get_pixels_around_world(&x_pixels_around_world);

         if (rRgnbounds.left > 0 && rRgnbounds.right > 0)
         {
            // if bounds towards right side
            // draw left side, right side
            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SetROP2(R2_COPYPEN);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);

            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SetROP2(R2_COPYPEN);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);
         }
         else
         {
            // if bounds towards left side
            // draw right side, left side
            m_pRgn->OffsetRgn(x_pixels_around_world, 0);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SetROP2(R2_COPYPEN);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);

            m_pRgn->OffsetRgn(-x_pixels_around_world, 0);
            pDC->SelectObject(&maskbrush);
            pDC->SetROP2(R2_MASKPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SelectObject(&colorbrush);
            pDC->SetROP2(R2_MERGEPEN);
            pDC->PaintRgn(m_pRgn);
            pDC->SetROP2(R2_COPYPEN);
            pDC->FrameRgn(m_pRgn, &solidbrush, weightframe, weightframe);
         }

         pDC->RestoreDC(saveDC);
      }
   }
   catch (CResourceException* e)
   {
      ERR_report("CResourceException drawing region");
      e->Delete();
      return FAILURE;
   }
   catch (CMemoryException* e)
   {
      ERR_report("CMemoryException drawing region");
      e->Delete();
      return FAILURE;
   }
   return SUCCESS;
}


// Create a region from blob data, scaling and clipping as needed
// Returns a visible rectangular region if too small to see
// Returns SUCCESS if a visible region remains, otherwise FAILURE
int CRegionItem::GetClippedRegion(MapProj* map, CRgn* pRgn, CByteArray& regionBlob, BOOL bLeftEdge/*=FALSE*/)
{
   //const int tinysize = 2;
   CByteArray combinedBlob;
   CRgn rgnInput, rgnOutput, rgnCombined, rgnView;
   CRect rInput, rOutput;
   d_geo_t map_ur, map_ll;
   int x_pixels_around_world;
   int width, height;
   int result;
   int status = SUCCESS;
   int nBytesNeeded;

   CGeoRect geo;
   geo.m_ll_lon =  0.0;
   geo.m_ll_lat =  0.0;
   geo.m_ur_lon =  0.0;
   geo.m_ur_lat =  0.0;

   CRect offsetRect;
   geo.MapToDeviceWindow(map, &offsetRect);

   // This only works on the current view
   degrees_t degLat, degLon;
   map->get_vmap_degrees_per_pixel(&degLat, &degLon);

   // Scale down the results (see header regarding RGN_PRECISION)
   XFORM transform_matrix;
   transform_matrix.eM11 = (float)(1.0 / RGN_PRECISION / degLon);
   transform_matrix.eM12 = 0.0;
   transform_matrix.eM21 = 0.0;
   transform_matrix.eM22 = (float)(1.0 / RGN_PRECISION / degLat);
   transform_matrix.eDx  = (float) offsetRect.left;
   transform_matrix.eDy  = (float) offsetRect.top;

   //-------------------------------------------------------------------------
   // 1. CreateFromData using identity matrix, return SUCCESS if empty blob
   if (!rgnInput.CreateFromData(NULL, regionBlob.GetSize(), (RGNDATA*)regionBlob.GetData()))
   {
      ERR_report("GetClippedRegion:CreateFromData() failed.");
      status = FAILURE;
      goto lbl_exit;
   }

   // Check to see if region is empty and return if so
   rgnInput.GetRgnBox(&rInput);  // get the bounds of the region

   if (rInput.IsRectNull())
   {
      //status = FAILURE;      // empty region passed in
      goto lbl_exit;
   }

   //-------------------------------------------------------------------------
   // 2. Inflate to ensure that smallest of components can be seen in world view

   /* disabled for now... CIB 1 creates regions that are only 1 on a side or less!
   InflateRgn(&rgnInput, &transform_matrix);
   */

   //-------------------------------------------------------------------------
   // 3. Clip input Rgn against a view window region
   if (map->get_vmap_bounds(&map_ll, &map_ur) != SUCCESS)
   {
      ERR_report("GetClippedRegion:get_vmap_bounds() failed.");
      status = FAILURE;
      goto lbl_exit;
   }

   ASSERT(-180.0 <= map_ll.lon && map_ll.lon <= 180.0);
   ASSERT(-180.0 <= map_ur.lon && map_ur.lon <= 180.0);

   // Get pixels around world to see if region can be inflated
   map->get_pixels_around_world(&x_pixels_around_world);
   map->get_vsurface_size(&width, &height);

   rgnCombined.CreateRectRgn(0,0,0,0);  // create empty region (cannot copyrgn into null rgn)

   // Don't need to clip if in world view
   if (x_pixels_around_world == width)
   {
      rgnCombined.CopyRgn(&rgnInput);   // in world view do not clip
   }
   else
   {
      // If viewable area wraps around world a second clipped region
      // needs to be created on the 'other' side of the world.
      // This routine gets called twice once with the bLeftEdge flag TRUE
      // and again as FALSE for the right side.  FALSE is default when
      // view area does not cross edge of world
      if (map_ll.lon > map_ur.lon)
      {
         if (bLeftEdge)
            map_ll.lon = map_ur.lon - width * degLon;
         else // right edge (default)
            map_ur.lon = map_ll.lon + width * degLon;
      }

      // inflate map bounds to 1 degree larger taking care not to wrap around
      // this is needed to avoid roundoff problems at very close up scales
      if (map_ll.lon > -179.0)
         map_ll.lon -= 1.0;
      if (map_ll.lat > -89.0)
         map_ll.lat -= 1.0;
      if (map_ur.lon < 179.0)
         map_ur.lon += 1.0;
      if (map_ur.lat < 89.0)
         map_ur.lat += 1.0;

      if (!rgnView.CreateRectRgn(
         (long)( map_ll.lon * RGN_PRECISION ),    // preserve precision before casting
         (long)(-map_ll.lat * RGN_PRECISION ),
         (long)( map_ur.lon * RGN_PRECISION ),
         (long)(-map_ur.lat * RGN_PRECISION )
         ))
      {
         ERR_report("GetClippedRegion:CreateRectRgn() failed.");
         status = FAILURE;
         goto lbl_exit;
      }

      // Use CombineRgn using RGN_AND to clip and trivially reject
      result = rgnCombined.CombineRgn(&rgnInput, &rgnView, RGN_AND);

      if (result == ERROR)
      {
         ERR_report("GetClippedRegion:CombineRgn() failed.");
         status = FAILURE;
         goto lbl_exit;
      }
      else if (result == NULLREGION)
      {
         // do what?
      }
   }

   //-------------------------------------------------------------------------
   // 4. Transform combined Rgn into output Rgn
   // Get the size of the region data
   nBytesNeeded = rgnCombined.GetRegionData(NULL, 0);

   // Put the combined region data into a new CByteArray so we can transform it
   combinedBlob.SetSize(nBytesNeeded);
   if (rgnCombined.GetRegionData((RGNDATA*)combinedBlob.GetData(), combinedBlob.GetSize()) == ERROR)
   {
      ERR_report("GetClippedRegion:GetRegionData() failed.");
      status = FAILURE;
      goto lbl_exit;
   }

   // CreateFromData from new blob using transform matrix (This can reduce to a null region)
   if (!rgnOutput.CreateFromData(&transform_matrix, combinedBlob.GetSize(), (RGNDATA*)combinedBlob.GetData()))
   {
      ERR_report("GetClippedRegion:CreateFromData() failed.");
      status = FAILURE;
      goto lbl_exit;
   }

   //-------------------------------------------------------------------------
   // 5. Check to see if output region is too small to view and create rect if needed

   if (rgnOutput.GetRgnBox(&rOutput) == SIMPLEREGION)
   {
      if (rOutput.Width() == 0 && rOutput.Height() == 0)
      {
            rgnCombined.GetRgnBox(&rOutput);

            // Manually transform the rectangle. TODO: this won't work for rotation
            rOutput.left = (int)((double)rOutput.left * (double)transform_matrix.eM11 + (double)transform_matrix.eDx);
            rOutput.right = rOutput.left + 1;
            rOutput.top = (int)((double)rOutput.top * (double)transform_matrix.eM22 + (double)transform_matrix.eDy);
            rOutput.bottom = rOutput.top + 1;
      }

      while (rOutput.Width() < 5) rOutput.InflateRect(1, 0); // Inefficient algorithm prevents rounding trouble
      while (rOutput.Height() < 5) rOutput.InflateRect(0, 1);
      rgnOutput.SetRectRgn(&rOutput);
   }

   /*if (rOutput.Width() < tinysize || rOutput.Height() < tinysize)
   {
      if (0)   // disabled for now... CIB 1 creates regions that are only 1 on a side or less!
      {
         // region is super narrow or super short, handle by drawing two rectangles, one at each corner
         /*
          * TODO: this code is 'better than nothing' but still suffers from portions of the
          * the region becoming invisible when scaled too small.  A means to 'inflate' all independent
          * portions of the region is needed to fix the problem.  (non-trivial)
          *
          * A better solution is to move the transform process into the region draw code
          * and pass around blobs instead of CRgn's inside the CRegionItem class.  That way full
          * resolution data will be available at draw time instead of being lost.  (Major work, later)
          * /
         CRect rect;
         rgnCombined.GetRgnBox(&rect);  // get the bounds of the untransformed region

         // Manually transform the rectangle. TODO: this won't work for rotation
         int x0 = (int)((double)rect.left   * (double)transform_matrix.eM11 + (double)transform_matrix.eDx);
         int y0 = (int)((double)rect.top    * (double)transform_matrix.eM22 + (double)transform_matrix.eDy);
         int x1 = (int)((double)rect.right  * (double)transform_matrix.eM11 + (double)transform_matrix.eDx);
         int y1 = (int)((double)rect.bottom * (double)transform_matrix.eM22 + (double)transform_matrix.eDy);

         // Make 'two' new rectangular regions (one for each corner) combined
         CRgn rgnTmp0, rgnTmp1;

         rgnTmp0.CreateRectRgn(x0 - tinysize/2, y0 - tinysize/2, x0 + tinysize/2, y0 + tinysize/2);
         rgnTmp1.CreateRectRgn(x1 - tinysize/2, y1 - tinysize/2, x1 + tinysize/2, y1 + tinysize/2);

         // Replace current region with combination of two corner rectangles
         if (rgnOutput.CombineRgn(&rgnTmp0, &rgnTmp1, RGN_OR) == ERROR)
         {
            ERR_report("GetClippedRegion:CombineRgn() failed.");
            return FAILURE;
         }

         rgnOutput.GetRgnBox(&rOutput);  // get the bounds of the untransformed region
      }
   }*/

   //-------------------------------------------------------------------------
   // 6. Copy clipped and optionally inflated output region for return
   pRgn->CopyRgn(&rgnOutput);
   status = SUCCESS;

lbl_exit:
   return status;
}

int CRegionItem::InflateRgn(CRgn* pRgn, XFORM* pXform)
{
   CRgn origRgn;
   origRgn.CreateRectRgn(0,0,0,0);
   origRgn.CopyRgn(pRgn);

   XFORM x = *pXform;

   //TRACE("<<<---------- Inflate... ------------\n");
   for(int loop = 0; loop < 10; ++loop)
   {
      TRACE("==================================\n");
      TRACE("times\t%14.4f,%14.4f\n", (double)x.eM11, (double)x.eM22);
      TRACE(" plus\t%14.4f,%14.4f\n", (double)x.eDx,  (double)x.eDy);
      TRACE("==================================\n");

      CRect rOrig;
      origRgn.GetRgnBox(&rOrig);
      TRACE("%02d)\trOrig:%d,%d,delta:%d,%d", loop, rOrig.left, rOrig.top, rOrig.Width(), rOrig.Height());

      // 4. GetRegionData into new blob
      // Get the size of the region data
      int nBytesNeeded = origRgn.GetRegionData(NULL, 0);

      // Put the combined region data into a new CByteArray so we can transform it
      CByteArray tmpBlob;
      tmpBlob.SetSize(nBytesNeeded);
      if (origRgn.GetRegionData((RGNDATA*)tmpBlob.GetData(), tmpBlob.GetSize()) == ERROR)
      {
         ERR_report("GetClippedRegion:GetRegionData() failed.");
         return FAILURE;
      }

      // 5. CreateFromData from new blob using transform matrix (This can reduce to a null region)
      CRgn rgnXformed;
      if (!rgnXformed.CreateFromData(&x, tmpBlob.GetSize(), (RGNDATA*)tmpBlob.GetData()))
      {
         ERR_report("GetClippedRegion:CreateFromData() failed.");
         return FAILURE;
      }

      // 6. Check to see if new region is too small to view and create rect if needed
      CRect rXformed;
      rgnXformed.GetRgnBox(&rXformed);  // get the bounds of the xformed region
      TRACE("\trXformed:%d,%d,%d,%d", rXformed.left, rXformed.top, rXformed.right, rXformed.bottom);

      // If region is very small then inflate it
      const int tinysize = 2;
      if (rXformed.Width() > tinysize && rXformed.Height() > tinysize)
         break;

      x.eM11 *= 2.0;
      x.eM12  = 0.0;
      x.eM21  = 0.0;
      x.eM22 *= 2.0;
      x.eDx  += 0.0;
      x.eDy  += 0.0;

      /*
      CRgn rgnInflated, rgn1;

      // must create a region before using CopyRgn
      rgnInflated.CreateRectRgn(0,0,0,0);
      rgn1.CreateRectRgn(0,0,0,0);

      rgnInflated.CopyRgn(&origRgn);
      rgn1.CopyRgn(&origRgn);

      int k = (int)(RGN_PRECISION * loop);

      rgn1.OffsetRgn(0, -k);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(k, 0);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(0, k);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(0, k);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(-k, 0);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(-k, 0);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(0, k);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);

      rgn1.OffsetRgn(0, k);
      rgnInflated.CombineRgn(&rgnInflated, &rgn1, RGN_OR);


      CRect r1;
      rgnInflated.GetRgnBox(&r1);  // get the bounds of the region
      TRACE("\tr1:%d,%d,%d,%d", r1.left, r1.top, r1.right, r1.bottom);

      origRgn.CopyRgn(&rgnInflated);
      */

      TRACE("\n");
   }

   pRgn->CopyRgn(&origRgn);
   //TRACE("\n----------------------------->>>\n");

   return SUCCESS;
}

// Returns true if geoBounds rectangle intersects the region contained in regionBlob.
// Also return true if no region exists in regionBlob.
bool CRegionItem::DoesRectangleIntersectRegion(CGeoRect geoBounds, CByteArray& regionBlob)
{
   CRgn rgn, rgnRectLeft, rgnRectRight, rgnLRCombined, rgnCombined;
   bool bLeftSideUsed = false;
   int  result;                  // used to hold result codes returned from Rgn functions
   bool bResult = true;

   // CreateFromData using identity matrix, return SUCCESS if empty blob
   if (!rgn.CreateFromData(NULL, regionBlob.GetSize(), (RGNDATA*)regionBlob.GetData()))
   {
      //ERR_report("DoesRectangleIntersectRegion:CreateFromData() failed.");
      return true;   // this is considered a match!
   }

   // Test to see if bounds are wrapped around world
   // Create a left side rectangle if so
   if (geoBounds.m_ll_lon > geoBounds.m_ur_lon)
   {
      CGeoRect tmpBounds = geoBounds;

      tmpBounds.m_ur_lon = 180;
      geoBounds.m_ll_lon = -180;

      if (!rgnRectLeft.CreateRectRgn(
         (long)( tmpBounds.m_ll_lon * RGN_PRECISION ),    // preserve precision before casting
         (long)(-tmpBounds.m_ll_lat * RGN_PRECISION ),
         (long)( tmpBounds.m_ur_lon * RGN_PRECISION ),
         (long)(-tmpBounds.m_ur_lat * RGN_PRECISION )
         ))
      {
         ERR_report("DoesRectangleIntersectRegion:CreateRectRgn() failed.");
         bResult = false;
      }

      bLeftSideUsed = true;
   }

   // Always do this one (right side) unless error
   if (bResult)
   {
      if (!rgnRectRight.CreateRectRgn(
         (long)( geoBounds.m_ll_lon * RGN_PRECISION ),    // preserve precision before casting
         (long)(-geoBounds.m_ll_lat * RGN_PRECISION ),
         (long)( geoBounds.m_ur_lon * RGN_PRECISION ),
         (long)(-geoBounds.m_ur_lat * RGN_PRECISION )
         ))
      {
         ERR_report("DoesRectangleIntersectRegion:CreateRectRgn() failed.");
         bResult = false;
      }
   }

   // Now merge left and right regions (if needed) and AND to see if intersection
   if (bResult)
   {
      // Combine the left and right regions as needed
      rgnLRCombined.CreateRectRgn(0,0,0,0);  // create empty region (cannot use a null rgn)
      if (bLeftSideUsed)
         result = rgnLRCombined.CombineRgn(&rgnRectRight, &rgnRectLeft,  RGN_OR);
      else
         result = rgnLRCombined.CopyRgn(&rgnRectRight);

      // Use CombineRgn using RGN_AND to clip and trivially reject
      rgnCombined.CreateRectRgn(0,0,0,0);    // create empty region (cannot use a null rgn)
      if (result != ERROR)
         result = rgnCombined.CombineRgn(&rgn, &rgnLRCombined, RGN_AND);

      if (result == ERROR)
      {
         ERR_report("DoesRectangleIntersectRegion:CombineRgn() failed.");
         bResult = false;
      }
      else if (result == NULLREGION)
      {
         bResult = false;
      }
      else
         bResult = true;
   }

   return bResult;
}


//==========================================================================

CRegionList::CRegionList()
{
   m_nCurrent = 0;
   RemoveAll();
}


CRegionList::~CRegionList()
{
   m_nCurrent = 0;
   RemoveAll();
}


void CRegionList::RemoveAll(void)
{
   for (int i = 0; i < GetSize(); ++i)
   {
      CRegionItem *item = GetAt(i);
      ASSERT(item);
      if (item) delete item;
   }

   CArray<CRegionItem*,CRegionItem*>::RemoveAll();
}


CRegionItem* CRegionList::GetFirst(void)
{
   m_nCurrent = 0;
   if (m_nCurrent < GetSize())
      return GetAt(m_nCurrent++);
   else
      return NULL;
}

CRegionItem* CRegionList::GetNext(void)
{
   if (m_nCurrent < GetSize())
      return GetAt(m_nCurrent++);
   else
      return NULL;
}

int CRegionList::DrawRegions(MapProj* map, CDC* pDC)
{
   int result = FAILURE;

   try
   {
      for (int i = 0; i < GetSize(); ++i)
         result = GetAt(i)->Draw(map, pDC);
   }
   catch (CMemoryException* e)
   {
      e->Delete();
      result = FAILURE;
   }
   return result;
}


// Locate the first region that matches the arguments specified
// Returns NULL if none found
CRegionItem* CRegionList::FindFirstRegionItem(long mapType, long sourceID, long type)
{
   for (int i = 0; i < GetSize(); ++i)
      if (GetAt(i)->m_mapType == mapType && GetAt(i)->m_sourceID == sourceID && GetAt(i)->m_type == type)
      {
         m_lFindMapType = mapType;
         m_lFindSourceID = sourceID;
         m_lFindRgnType = type;
         m_nFindPosition = i + 1;
         return (GetAt(i));
      }

   m_nFindPosition = 0;
   return NULL;
}

CRegionItem* CRegionList::FindNextRegionItem(void)
{
   for (int i = m_nFindPosition; i < GetSize(); ++i)
      if (GetAt(i)->m_mapType == m_lFindMapType && GetAt(i)->m_sourceID == m_lFindSourceID && GetAt(i)->m_type == m_lFindRgnType)
      {
         m_nFindPosition = i + 1;
         return (GetAt(i));
      }

   m_nFindPosition = 0;
   return NULL;
}

// Locate specified type of region in list... add new one if not found
// Then combine rectangle into the region
int CRegionList::CombineAddRegionRect(CGeoRect& gtmp, long sourceID, long mapTypeID, long type)
{
   // Make into a rectangular region for combining (see header regarding RGN_PRECISION)
   CRgn rectRgn;
   rectRgn.CreateRectRgn(
      (long)( gtmp.m_ll_lon * RGN_PRECISION ),    // preserve precision before casting
      (long)(-gtmp.m_ll_lat * RGN_PRECISION ),
      (long)( gtmp.m_ur_lon * RGN_PRECISION ),
      (long)(-gtmp.m_ur_lat * RGN_PRECISION )
      );

   // Find region or create if it does not exist
   // Should only be one of each type in this list
   CRegionItem *pRgnItem = FindFirstRegionItem(mapTypeID, sourceID, type);
   if (pRgnItem == NULL)
   {
      pRgnItem = new CRegionItem(0, mapTypeID, sourceID, type);
      Add(pRgnItem);
   }

   // Combine rgn with existing rgn
   pRgnItem->m_pRgn->CombineRgn(pRgnItem->m_pRgn, &rectRgn, RGN_OR);

   return SUCCESS;
}

// Returns the rectangle that totally encloses all the regions in region list
// Union new rectangles with one passed in
void CRegionList::GetBoundingRgnBox(CRect* pRect)
{
   CRect rect;

   rect.SetRectEmpty();
   for (int i = 0; i < GetSize(); ++i)
   {
      GetAt(i)->m_pRgn->GetRgnBox(&rect);
      pRect->UnionRect(pRect, &rect);
   }
}


};  // namespace scene_mgr