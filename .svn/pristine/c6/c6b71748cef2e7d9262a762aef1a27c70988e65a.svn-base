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
#include "MaskClipRgn.h"
#include "map.h"
#include "geo_tool.h"
#include "err.h"
#include "fvwutil.h"
#include "ovlelem.h"
#include <gdiplus.h>
#include "utils.h"

/// <summary>
/// Since clipping is disabled, we simply return the bounding rect of the given map
/// <summary>
/// <returns>Returns SUCCESS if successful, FAILURE otherwise.  The arguments passed in are filled with
/// the computed bounds.<returns>
int CMaskNullClipRgn::ComputeClippedGeoBounds(MapProj *pMap, double& ll_lat, double& ll_lon, double& ur_lat, double& ur_lon)
{
   if (pMap == NULL)
   {
      ERR_report("The parameter pMap can not be NULL");
      return FAILURE;
   }

   d_geo_t ll, ur;
   if (pMap->get_vmap_bounds(&ll, &ur) != SUCCESS)
   {
      ERR_report("Failed getting vmap bounds");
      return FAILURE;
   }

   ll_lat = ll.lat;
   ll_lon = ll.lon;
   ur_lat = ur.lat;
   ur_lon = ur.lon;

   return SUCCESS;
}

int CMaskNullClipRgn::ClipElevationData(short *pElevationData, 
   short* pClippedData, int nWidth, int nHeight, double llLat, double llLon, 
   double dppLat, double dppLon)
{
   // nothing to do
   return SUCCESS;
}

/// <summary>
/// Helper function to determine the closest angle, within the given range, to a target angle
/// </summary>
double CMaskCircularClipRgn::calc_closest_angle(double start, double end, double target)
{
   CFvwUtil *pUtil = CFvwUtil::get_instance();

   // normalize the angles so that start is at 0.0
   double norm_end = pUtil->normalize_angle(end - start);
   double norm_target = pUtil->normalize_angle(target - start);

   // if the target angle falls within the given bounds we return that (note, if start == end then we
   // are dealing with the entire circle.  In this case, return the target)
   if (norm_target <= norm_end || fabs(start - end) < 1e-6)
      return target;

   // compute the shortest arc from start to target.  Note: here we are dealing in the translated
   // angles so start = 0.0
   double min_start_to_target = min(norm_target, 360.0 - norm_target);

   // note: since target is not within [start,end] it must be greater than end
   double min_end_to_target = min(norm_target - norm_end, pUtil->normalize_angle(norm_end - norm_target));

   // the given starting angle is closer to the target, return it
   if (min_start_to_target < min_end_to_target)
      return start;

   // otherwise, the ending angle is closer to the target
   return end;
}

/// <summary>
/// Based on the given map's geo-bounds and the clipping region computes a clipped geo-bounds
/// <summary>
/// <returns>Returns SUCCESS if successful, FAILURE otherwise.  The arguments passed in are filled with
/// the computed bounds.<returns>
int CMaskCircularClipRgn::ComputeClippedGeoBounds(MapProj *pMap, 
                                           double& ll_lat, double& ll_lon, double& ur_lat, double& ur_lon)
{
   if (pMap == NULL)
   {
      ERR_report("The parameter pMap can not be NULL");
      return FAILURE;
   }

   pMap->geo_to_surface(m_dCenterLat, m_dCenterLon, &m_nCenterSurfaceX, &m_nCenterSurfaceY);
   
   // compute the lower-left and upper-right bounding geo-coordinates of the circular region
   //
   // for now, just return the bounding geo-coordinates of the circular region taking into account
   // the starting and ending arc angles.  Later, we will clip this rect against the given map's bounds
   double unused, angle;
   double dWest, dEast, dNorth, dSouth;

   // determine the heading within [starting, ending]-clockwise that is closest to 270.0.  This will determine
   // angle used to compute the western bounds.  If the closest angle is between [0, 180], then we will
   // use the center longitude as the western bounds
   angle = calc_closest_angle(m_dStartHeading, m_dEndHeading, 270.0);
   if (angle <= 180)
      dWest = m_dCenterLon;
   else
   {
      GEO_distance_to_geo(m_dCenterLat, m_dCenterLon, m_dRadiusMeters, angle, &unused, &dWest);
   }

   // compute the eastern bounds in the same manner
   angle = calc_closest_angle(m_dStartHeading, m_dEndHeading, 90.0);
   if (angle > 180.0)
      dEast = m_dCenterLon;
   else
   {
      GEO_distance_to_geo(m_dCenterLat, m_dCenterLon, m_dRadiusMeters, angle, &unused, &dEast);
   }

   // the northern bounds will be computed using the angle that is closest to 0.0
   angle = calc_closest_angle(m_dStartHeading, m_dEndHeading, 0.0);
   if (angle > 90 && angle < 270)
      dNorth = m_dCenterLat;
   else
   {
      GEO_distance_to_geo(m_dCenterLat, m_dCenterLon, m_dRadiusMeters, angle, &dNorth, &unused);  
   }

   // the southern bounds will be computed using the angle that is closest to 180.0
   angle = calc_closest_angle(m_dStartHeading, m_dEndHeading, 180.0);
   if (angle < 90 || angle > 270)
      dSouth = m_dCenterLat;
   else
   {
      GEO_distance_to_geo(m_dCenterLat, m_dCenterLon, m_dRadiusMeters, angle, &dSouth, &unused);  
   }

   ll_lat = dSouth;
   ll_lon = dWest;
   ur_lat = dNorth;
   ur_lon = dEast;

   d_geo_t ll, ur;
   if (pMap->get_vmap_bounds(&ll, &ur) != SUCCESS)
   {
      ERR_report("Failed getting vmap bounds");
      return FAILURE;
   }

   if (ll.lat > ll_lat)
      ll_lat = ll.lat;
   if (ur.lat < ur_lat)
      ur_lat = ur.lat;

   if (ll.lon > ll_lon)
      ll_lon = ll.lon;
   if (ur.lon < ur_lon)
      ur_lon = ur.lon;

   if (ll_lat >= ur_lat || ll_lon >= ur_lon)
      return S_FALSE;

   return SUCCESS;
}

int CMaskCircularClipRgn::ClipElevationData(short *pElevationData, short* pClippedData, int nWidth, int nHeight,
         double llLat, double llLon, double dppLat, double dppLon)
{
   // re-fetch the data from source for masking
   memcpy(pClippedData, pElevationData, sizeof(short)* nWidth * nHeight);



   // Prepare the mask for the data
   Gdiplus::Bitmap clipMask(nWidth, nHeight);

   // clear out clipMask
   Gdiplus::Rect rect(0, 0, nWidth - 1, nHeight - 1);
   Gdiplus::BitmapData bitmapData;
   clipMask.LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat1bppIndexed, &bitmapData);
   memset(bitmapData.Scan0, 0, bitmapData.Stride * bitmapData.Height);
   clipMask.UnlockBits(&bitmapData);

   // draw the clip region onto the clip mask - bits that are set will be preserved in the elevation data array.  All other
   // values will be set to ELEVATION_DATA_CLIPPED
   Gdiplus::Graphics *pGraphics = Gdiplus::Graphics::FromImage(&clipMask);

   // compute the center relative to the elevation data
   int centerX = static_cast<int>((m_dCenterLon - llLon ) / dppLon);
   int centerY = static_cast<int>((m_dCenterLat - llLat) / dppLat);

   // compute the radius of the ellipse in pixels
   double dEast, dNorth, unused;
   GEO_distance_to_geo(m_dCenterLat, m_dCenterLon, m_dRadiusMeters, 90.0, &unused, &dEast);
   GEO_distance_to_geo(m_dCenterLat, m_dCenterLon, m_dRadiusMeters, 0.0, &dNorth, &unused);
   int radiusPixelsX = static_cast<int>((dEast - llLon ) / dppLon) - centerX;
   int radiusPixelsY = static_cast<int>((dNorth - llLat) / dppLat) - centerY;

   // compute corner of ellipse
   int ulx_ellipse = centerX - radiusPixelsX;
   int uly_ellipse = centerY - radiusPixelsY;

   Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255));
   if (fabs(m_dSweepAngle - 360) < 1e-6)
   {
      pGraphics->FillPie(&brush, ulx_ellipse, uly_ellipse, radiusPixelsX * 2, radiusPixelsY * 2, 0, 360);
   }
   else
   {
      // compute the geographic location of the arc's endpoints so that we can compute the angle
      GeoArc arc;
      d_geo_t geoCenter = { m_dCenterLat, m_dCenterLon };
      arc.define(geoCenter, m_dRadiusMeters, m_dEndHeading, min(m_dSweepAngle, 360 - 1e-6), FALSE);
      d_geo_t arcStart = arc.get_start();
      d_geo_t arcEnd = arc.get_end();

      int startX = static_cast<int>((arcStart.lon - llLon ) / dppLon);
      int startY = static_cast<int>((arcStart.lat - llLat) / dppLat);
      int endX = static_cast<int>((arcEnd.lon - llLon ) / dppLon);
      int endY = static_cast<int>((arcEnd.lat - llLat) / dppLat);

      float startAngle = (float)(CFvwUtil::get_instance()->line_angle(centerX, centerY, startX, startY));
      float endAngle = (float)(CFvwUtil::get_instance()->line_angle(centerX, centerY, endX, endY));
      float sweepAngle = (float)CFvwUtil::get_instance()->normalize_angle(endAngle - startAngle);

      pGraphics->FillPie(&brush, ulx_ellipse, uly_ellipse, radiusPixelsX * 2, radiusPixelsY * 2, startAngle, sweepAngle);
   }

   clipMask.LockBits(&rect, Gdiplus::ImageLockModeRead,  PixelFormat32bppARGB, &bitmapData);

   BYTE *pData = (BYTE *)bitmapData.Scan0;
   for(int j = 0; j<nHeight; j++)
   {
      for(int i=0; i<nWidth; i++)
      {
         if ( *(pData + j * bitmapData.Stride + 4 * i) != 255)
         {
            *(pClippedData + nWidth * j + i) = ELEVATION_DATA_CLIPPED;
         }
      }
   }

   clipMask.UnlockBits(&bitmapData);

   delete pGraphics;

   return SUCCESS;
}

BOOL CMaskCircularClipRgn::DidScreenPositionChange(double dLat, double dLon)
{
   if (m_nCenterSurfaceX == 0 && m_nCenterSurfaceY == 0)
      return FALSE;

   const int THRESHOLD_IN_PIXELS = 1;

   ViewMapProj *pMapProj = UTL_get_current_view_map();
   if (pMapProj != NULL)
   {
      int x, y;
      pMapProj->geo_to_surface(dLat, dLon, &x, &y);
      return abs(x - m_nCenterSurfaceX) >= THRESHOLD_IN_PIXELS || abs(y - m_nCenterSurfaceY) >= THRESHOLD_IN_PIXELS;
   }

   return FALSE;
}

BOOL CMaskCircularClipRgn::DidStartHeadingChange(double dStartHeading)
{
   if ( fabs(360.0 - m_dSweepAngle) < 1e-6)
      return FALSE;

   return fabs( dStartHeading - m_dStartHeading ) >= 1e-6;
}