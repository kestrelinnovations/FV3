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



// viewinv.h

#ifndef MAP_VIEW_INVALIDATE_H
#define MAP_VIEW_INVALIDATE_H 1

#include "common.h"  // boolean_t

// Base class for map view invalidation objects.
class MapViewInvalidation
{
protected:
   boolean_t m_erase_background;

public:
   MapViewInvalidation(boolean_t erase_background);

   virtual void invalidate(CWnd *wnd) = 0;
};

// A map view invalidation object for invalidating the entire window.
class MapViewInvalidateAll : public MapViewInvalidation
{
public:
   MapViewInvalidateAll(boolean_t erase_background);

   virtual void invalidate(CWnd *wnd);
};

// A map view invalidation object for invalidating a rectangle in the window.
class MapViewInvalidateRect : public MapViewInvalidation
{
private:
   CRect m_rect;

public:
   MapViewInvalidateRect(LPCRECT rect, boolean_t erase_background);

   virtual void invalidate(CWnd *wnd);
};

// A map view invalidation object for invalidating a CRgn in the window.
class MapViewInvalidateRgn : public MapViewInvalidation
{
private:
   CRgn m_rgn;

public:
   MapViewInvalidateRgn(CRgn *rgn, boolean_t erase_background);

   virtual void invalidate(CWnd *wnd);
};

// A map view invalidation object for invalidating the map itself
class MapViewInvalidateMap : public MapViewInvalidation
{
public:
   MapViewInvalidateMap(boolean_t erase_background);

   virtual void invalidate(CWnd *wnd);
};

#endif   // end MAP_VIEW_INVALIDATE_H
