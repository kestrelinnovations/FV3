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



// clip.cpp

#include "stdafx.h"

#include "utils.h"
#include "err.h"

// Constructor
// Creates an empty exclude clip region for the given DC.  The DC must
// be valid.  The state of the DC is saved by the constructor and restored
// by the destructor.
ExcludeClipRegion::ExcludeClipRegion(CDC *dc, bool bSaveDC /*=true*/) : m_dc(dc)
{
   ASSERT(dc);

   if (dc && bSaveDC)
   {
      m_saved_DC = dc->SaveDC();
      ASSERT(m_saved_DC);
   }
   else
      m_saved_DC = 0;
}

// Destructor
// Restores the DC passed to the constructor.
ExcludeClipRegion::~ExcludeClipRegion()
{
   if (m_saved_DC)
      m_dc->RestoreDC(m_saved_DC);
   else
      m_dc->SelectClipRgn(NULL);
}

// Adds a rectangle to the no-draw region.
int ExcludeClipRegion::add_rect(CRect &rect)
{
   if (m_dc == NULL)
   {
      ERR_report("NULL DC");
      return FAILURE;
   }
   
   if (!rect.IsRectEmpty())
   {
      if (m_dc->ExcludeClipRect(&rect) == ERROR)
      {
         ERR_report("ExcludeClipRect() failed.");
         return FAILURE;
      }
   }

   return SUCCESS;
}

// add a region to the no-draw region
int ExcludeClipRegion::add_region(CRgn &rgn)
{
   if (m_dc == NULL)
   {
      ERR_report("NULL DC");
      return FAILURE;
   }

   // dwCount is the size, in bytes, of the region data
   DWORD dwCount = rgn.GetRegionData(NULL,0);
   
   BYTE	*pData = new BYTE[dwCount];
   
   memset(pData, 0, dwCount);		
   LPRGNDATA lpRgnData = reinterpret_cast<LPRGNDATA>(pData);
   lpRgnData->rdh.dwSize = sizeof(lpRgnData->rdh);
   DWORD	dwResult = rgn.GetRegionData(lpRgnData, dwCount);

   // loop through the rectangles making up the region
   char *buffer_ptr = lpRgnData->Buffer;
   for(int rindex = 0; rindex < (int)lpRgnData->rdh.nCount; rindex++) 
   {
      // get the next CRect in the buffer and add it to the clip region
      if (m_dc->ExcludeClipRect((CRect *)buffer_ptr) == ERROR)
      {
         ERR_report("ExcludeClipRect() failed.");
         free(lpRgnData);
         return FAILURE;
      }
         
      // increment the pointer to the list of CRects
      buffer_ptr += sizeof(CRect);
   }

   // free the memory used by the region data 
   delete [] pData;

   return SUCCESS;
}

// Adds a circle to the no-draw region.
int ExcludeClipRegion::add_circle(int x, int y, int radius)
{
   CRect rect;

   if (m_dc == NULL)
   {
      ERR_report("NULL DC");
      return FAILURE;
   }

   if (radius <= 0)
   {
      ERR_report("Invalid radius.");
      return FAILURE;
   }

   // get half the width of the square with corner points on the circle
   int d = (int)(0.707106781 * (double)radius + 0.5);

   // exclude the area inside the square, as it is bound by the circle
   rect.SetRect(x - d, y - d, x + d + 1, y + d + 1);
   if (m_dc->ExcludeClipRect(&rect) == ERROR)
   {
      ERR_report("ExcludeClipRect() failed.");
      return FAILURE;
   }
    
   // If r < 8 then the difference between the rectangle bounding the
   // circle and the one bound by the circle is 2 pixels or less.  In
   // that case there is no need to worry about excluding the part of
   // the circle outside the inner square.
   if (radius < 8)
      return SUCCESS;

   // effective diameter of the circle
   int diameter = 2 * radius + 1;

   // add rectangles to cover the left and right arcs
   while (rect.Width() < diameter)
   {
      if (rect.Height() < 9 || rect.Width() > (diameter - 8))
      {
         rect.left = x - radius;
         rect.right = x + radius + 1;
         rect.top = y + (radius/8);
         rect.bottom = y - (radius/8) - 1;
      }
      else
         rect.InflateRect(4, -4);

      if (m_dc->ExcludeClipRect(&rect) == ERROR)
      {
         ERR_report("ExcludeClipRect() failed.");
         return FAILURE;
      }
   }

   // reset rect and add rectangles to cover the top and bottom arcs
   rect.SetRect(x - d, y - d, x + d + 1, y + d + 1);
   while (rect.Height() < diameter)
   {
      if (rect.Width() < 9 || rect.Height() > (diameter - 8))
      {
         rect.left = x - (radius/8);
         rect.right = x + (radius/8) + 1;
         rect.top = y - radius;
         rect.bottom = y + radius + 1;
      }
      else
         rect.InflateRect(-4, 4);

      if (m_dc->ExcludeClipRect(&rect) == ERROR)
      {
         ERR_report("ExcludeClipRect() failed.");
         return FAILURE;
      }
   }

   return SUCCESS;
}

// Adds a polygon to the clip region.  The function closes the polygon 
// automatically, if necessary, by assuming a line from the last vertex 
// to the first.  See CreatePolygonRgn on-line help for more information.
// The parameters to add_polygon are analogous to those in the 
// CreatePolygonRgn(LPPOINT lpPoints, int nCount, and int nMode) function.
int ExcludeClipRegion::add_polygon(LPPOINT points, int num_points, 
                                   int mode)
{
   CRgn region;

   // this function creates a bunch of CRects that cover the polygon
   if (region.CreatePolygonRgn(points, num_points, mode) == FALSE)
   {
      ERR_report("CreatePolygonRgn() failed.");
      return FAILURE;
   }

   int status = add_region(region);
   region.DeleteObject();

   if (status != SUCCESS)
      ERR_report("add_region() failed.");

   return status;
}
