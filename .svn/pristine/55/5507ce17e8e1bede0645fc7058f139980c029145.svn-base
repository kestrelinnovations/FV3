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



// IconItem.h: interface for the CSceneIconItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEICONITEM_H__389A4763_6849_11D2_B082_00104B8C938E__INCLUDED_)
#define AFX_SCENEICONITEM_H__389A4763_6849_11D2_B082_00104B8C938E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ovl_d.h"
#include "overlay.h"
#include "Common\SafeArray.h"
#include "FvMappingCore\Include\MapProjWrapper.h"

// Flag bits for database; sorts descending by this field for display
#define SM_VALID       0x01     // if this bit is not set the item is unknown or null
#define SM_SELECTED    0x02
#define SM_ODD         0x04
#define SM_TARGET      0x10
#define SM_CATALOG     0x20
#define SM_HIGHLIGHTED 0x40

// Type values for regions
#define SM_UNSELECTED_SOURCE             (SM_VALID)
#define SM_UNSELECTED_TARGET             (SM_VALID|SM_TARGET)
#define SM_SELECTED_SOURCE               (SM_VALID|SM_SELECTED)
#define SM_SELECTED_TARGET               (SM_VALID|SM_SELECTED|SM_TARGET)
#define SM_UNSELECTED_CATALOG            (SM_VALID|SM_CATALOG)
#define SM_SELECTED_CATALOG              (SM_VALID|SM_SELECTED|SM_CATALOG)
#define SM_HIGHLIGHTED_CATALOG           (SM_VALID|SM_CATALOG|SM_HIGHLIGHTED)
#define SM_HIGHLIGHTED_SELECTED_CATALOG  (SM_VALID|SM_CATALOG|SM_HIGHLIGHTED|SM_SELECTED)


// Forward declarations
class MapProj;

namespace scene_mgr
{

// Color constants (original)
const COLORREF sm_red           = RGB(170, 0, 0);
const COLORREF sm_green         = RGB(0, 170, 0);
const COLORREF sm_bright_red    = RGB(255, 85, 85);
const COLORREF sm_bright_green  = RGB(85, 255, 85);
const COLORREF sm_yellow        = RGB(170, 170, 0);
const COLORREF sm_bright_yellow = RGB(255, 255, 85);
const COLORREF sm_bright_orange = RGB(255, 128, 0);

class CGeoRect
{
public:
   degrees_t m_ll_lon;
   degrees_t m_ll_lat;
   degrees_t m_ur_lon;
   degrees_t m_ur_lat;

   enum { LL, UR, LLUR };

public:
   CGeoRect() {};
   CGeoRect(degrees_t ll_lon, degrees_t ll_lat, degrees_t ur_lon, degrees_t ur_lat) 
      { m_ll_lon = ll_lon; m_ll_lat = ll_lat; m_ur_lon = ur_lon; m_ur_lat = ur_lat; };
   virtual ~CGeoRect() {};

   CGeoRect& operator=(const CGeoRect& r)
   { 
      m_ll_lon = r.m_ll_lon; 
      m_ll_lat = r.m_ll_lat; 
      m_ur_lon = r.m_ur_lon; 
      m_ur_lat = r.m_ur_lat;
      return *this;
   }

   void Zero(void) { m_ll_lat = m_ll_lon = m_ur_lat = m_ur_lon = 0.0; };
   void MapToDeviceWindow(MapProj* map, CRect* pRect);
   void MapFromDeviceWindow(map_projection_utils::CMapProjWrapper* map, CRect* pRect);
   void Shrink(CRect* pRect, int weight, int width, int height);
   void Normalize(void) 
   { 
      if (m_ll_lon > 180) m_ll_lon -= 180.0;
      if (m_ll_lon < -180) m_ll_lon += 180.0;
      if (m_ur_lon > 180) m_ur_lon -= 180.0;
      if (m_ur_lon < -180) m_ur_lon += 180.0;

      if (m_ll_lon > m_ur_lon) m_ur_lon += 360.0; 
   };
   void SetToWorld(void)
   {
      m_ll_lat = -90.0; 
      m_ll_lon = -180.0; 
      m_ur_lat = 90.0; 
      m_ur_lon = 180.0; 
   }

   BOOL IsZero(void)
   {
      return (m_ll_lat == 0.0 && m_ll_lon == 0.0 &&
              m_ur_lat == 0.0 && m_ur_lon == 0.0 );
   }

   BOOL IsCloseEnough(double d1, double d2)
   {
      // 1000th of a degree is close enough.
      // Any higher can result in failure due to roundoff when copying files.
      const double fArbitraryPrecision = 0.001;

      // Compute absolute value of difference
      // Returns true if very small
      double fDiff = d1 - d2;
      if (fDiff < 0)
         return (-fDiff < fArbitraryPrecision);
      else
         return (fDiff < fArbitraryPrecision);
   }

   BOOL RectEqual(CGeoRect* p)
   {
      /* We want the following but precision roundoff
       * makes these fail sometimes.  Use method following
      return (
         p->m_ll_lon == m_ll_lon  &&
         p->m_ll_lat == m_ll_lat  &&
         p->m_ur_lon == m_ur_lon  &&
         p->m_ur_lat == m_ur_lat
         );
       */

      BOOL b1 = IsCloseEnough(p->m_ll_lon, m_ll_lon);
      BOOL b2 = IsCloseEnough(p->m_ll_lat, m_ll_lat);
      BOOL b3 = IsCloseEnough(p->m_ur_lon, m_ur_lon);
      BOOL b4 = IsCloseEnough(p->m_ur_lat, m_ur_lat);
      if (b1 && b2 && b3 && b4)
         return TRUE;
      else
         return FALSE;
   }

   // Returns TRUE if p lies 'inside' this rectangle
   BOOL RectInside(CGeoRect* p)
   {
      return (
         p->m_ll_lon > m_ll_lon  &&
         p->m_ll_lat > m_ll_lat  &&
         p->m_ur_lon < m_ur_lon  &&
         p->m_ur_lat < m_ur_lat
         );
   }

   CString Format(int iCode=(LL|UR));

   // pR1 and pR2 must be normalized for this to function properly.
   void UnionGeoRect(CGeoRect* pR1, CGeoRect* pR2)
   {
      degrees_t ll_lon = (pR1->m_ll_lon < pR2->m_ll_lon) ? pR1->m_ll_lon : pR2->m_ll_lon;
      degrees_t ll_lat = (pR1->m_ll_lat < pR2->m_ll_lat) ? pR1->m_ll_lat : pR2->m_ll_lat;
      degrees_t ur_lon = (pR1->m_ur_lon > pR2->m_ur_lon) ? pR1->m_ur_lon : pR2->m_ur_lon;
      degrees_t ur_lat = (pR1->m_ur_lat > pR2->m_ur_lat) ? pR1->m_ur_lat : pR2->m_ur_lat;

      m_ll_lon = ll_lon;
      m_ll_lat = ll_lat;
      m_ur_lon = ur_lon;
      m_ur_lat = ur_lat;
   }

   degrees_t GetCenterLat()
   {
      return (m_ll_lat + m_ur_lat) / 2.0;
   }

   degrees_t GetCenterLon()
   {
      return (m_ll_lon + m_ur_lon) / 2.0;
   }
};

class CSceneIconItem : public C_icon
{
public:
   CGeoRect m_bbox;
   long     m_sourceID;
   long     m_sceneTypeID;
   long     m_lFilesize;
   DWORD    m_flags;
   CString  m_sPath;
   CString  m_sFilename;
public:
   CSceneIconItem(C_overlay *overlay);
   virtual ~CSceneIconItem();
   
   void Draw(MapProj* map, CDC* pDC);
   
   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "CSceneIconItem"; }
   
   // required C_icon derived class member function to return a help text string
   CString get_help_text();
   
   // required C_icon derived class member function to return a tool tip string
   CString get_tool_tip();
   
   // Determine if the point is over the file icon.
   boolean_t hit_test(CPoint point);
   
   BOOL IsSelected(void)   { return m_flags & SM_SELECTED; };
   void SetSelected(BOOL b)
   {
      if (b)
         m_flags |= SM_SELECTED;
      else
         m_flags &= ~SM_SELECTED;
   }
   
   BOOL IsHighlighted(void)   { return m_flags & SM_HIGHLIGHTED; };
   void SetHighlighted(BOOL b)
   {
      if (b)
         m_flags |= SM_HIGHLIGHTED;
      else
         m_flags &= ~SM_HIGHLIGHTED;
   }
   
   BOOL IsTarget(void)     { return m_flags & SM_TARGET; };
   void GetBbox(CGeoRect& bbox) { bbox = m_bbox; };
   long GetSourceID(void)  { return m_sourceID; };
   long GetSceneTypeID(void) { return m_sceneTypeID; };
   long GetFilesize(void)  { return m_lFilesize; };
   CString GetPath(void)   { return m_sPath; };
   CString GetFilename(void) { return m_sFilename; };
   
   // order: source-nonselected, target-nonselected, source-selected, then target-selected
   static int GetSortValue(DWORD bIsTarget, DWORD bIsSelected)
   {
      if (!bIsTarget && !bIsSelected)
         return 0;
      if (bIsTarget && !bIsSelected)
         return 1;
      if (!bIsTarget && bIsSelected)
         return 2;
      
      //if (bIsTarget && bIsSelected)
         return 3;
   }
   
   static bool Sort(CSceneIconItem *a, CSceneIconItem *b)
   {
      return GetSortValue(a->m_flags & SM_TARGET, a->m_flags & SM_SELECTED) < 
         GetSortValue(b->m_flags & SM_TARGET, b->m_flags & SM_SELECTED);
   }
};

#define maxX   360
#define maxY   180

class CRegionItem
{
public:
   CRgn*    m_pRgn;
   CGeoRect m_bbox;
   long     m_mapType;
   long     m_sourceID;
   long     m_ID;
   long     m_type;
public:
   CRegionItem();
   CRegionItem(long ID, long mapType, long sourceID, long lRegionType);
   virtual ~CRegionItem();

   int ConvertRegionToBlob(CByteArray& regionBlob);
   int ConvertBlobToRegion(CByteArray& regionBlob, XFORM* pXform=NULL);
   int Draw(MapProj* map, CDC* pDC);
   int GetClippedRegion(MapProj* map, CRgn* pRgn, CByteArray& regionBlob, BOOL bLeftEdge=FALSE);
   int InflateRgn(CRgn* pRgn, XFORM* pXform);
   bool DoesRectangleIntersectRegion(CGeoRect geoBounds, CByteArray& regionBlob);

   long GetID(void)     { return m_ID; };
   long GetType(void)   { return m_type; };

   BOOL IsCatalog(void)   { return m_type & SM_CATALOG; };
   void SetCatalog(BOOL b)
   {
      if (b)
         m_type |= SM_CATALOG;
      else
         m_type &= ~SM_CATALOG;
   };

   BOOL IsSelected(void)   { return m_type & SM_SELECTED; };
   void SetSelected(BOOL b)
   {
      if (b)
         m_type |= SM_SELECTED;
      else
         m_type &= ~SM_SELECTED;
   };

   BOOL IsHighlighted(void)   { return m_type & SM_HIGHLIGHTED; };
   void SetHighlighted(BOOL b)
   {
      if (b)
         m_type |= SM_HIGHLIGHTED;
      else
         m_type &= ~SM_HIGHLIGHTED;
   };
};

class CRegionList : public CArray<CRegionItem*,CRegionItem*>
{
   int  m_nCurrent;
   int  m_nFindPosition;
   long m_lFindMapType;
   long m_lFindSourceID;
   long m_lFindRgnType;

public:
   CRegionList();
   virtual ~CRegionList();
   void RemoveAll(void);

   CRegionItem* GetFirst(void);
   CRegionItem* GetNext(void);
   int  DrawRegions(MapProj* map, CDC* pDC);
   CRegionItem* FindFirstRegionItem(long mapType, long sourceID, long type);
   CRegionItem* FindNextRegionItem(void);
   int CombineAddRegionRect(CGeoRect& gtmp, long sourceID, long mapTypeID, long type);
   void GetBoundingRgnBox(CRect* pRect);
};

};  // namespace scene_mgr

#endif // !defined(AFX_SCENEICONITEM_H__389A4763_6849_11D2_B082_00104B8C938E__INCLUDED_)






