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



class ExcludeClipRegion;
class MapProj;

#define ELEVATION_DATA_CLIPPED -32766

/// <summary>
/// Base class used to encapsulate mask clipping logic
/// </summary>
class CMaskClipRgn
{
public:
   /// <summary>
   /// Based on the given map's geo-bounds and the clipping region computes a clipped geo-bounds
   /// <summary>
   /// <returns>Returns SUCCESS if successful, FAILURE otherwise<returns>
   virtual int ComputeClippedGeoBounds(MapProj *pMap, double& ll_lat, double& ll_lon, double& ur_lat, double& ur_lon) = 0;

   virtual int ClipElevationData(short *pElevationData, short* pClippedData, int nWidth, int nHeight,
         double llLat, double llLon, double dppLat, double dppLon) = 0;
};

/// <summary>
/// Represents a null clipping region.  In other words, clipping is disabled
/// </summary>
class CMaskNullClipRgn : public CMaskClipRgn
{
public:
   /// <summary>
   /// Since clipping is disabled, we simply return the bounding rect of the given map
   /// <summary>
   /// <returns>Returns SUCCESS if successful, FAILURE otherwise.  The arguments passed in are filled with
   /// the computed bounds.<returns>
   virtual int ComputeClippedGeoBounds(MapProj *pMap, double& ll_lat, double& ll_lon, double& ur_lat, double& ur_lon);

   virtual int ClipElevationData(short *pElevationData, short* pClippedData, int nWidth, int nHeight,
         double llLat, double llLon, double dppLat, double dppLon);
};

class CMaskCircularClipRgn : public CMaskClipRgn
{
   // parameters for a circular (or arc) clipping region
   double m_dCenterLat;
   double m_dCenterLon;
   double m_dRadiusMeters;
   double m_dStartHeading;
   double m_dEndHeading;
   double m_dSweepAngle;

   // used to determine if the clipping region has moved on the screen
   int m_nCenterSurfaceX;
   int m_nCenterSurfaceY;

public:

   /// <summary>
   /// Constructor for a circular clipping region
   /// <summary>
   CMaskCircularClipRgn(double dCenterLat, double dCenterLon, double dRadiusMeters,
      double dStartHeading, double dEndHeading, double dSweepAngle)
   {
      Reinitialize(dCenterLat, dCenterLon, dRadiusMeters, dStartHeading, dEndHeading, dSweepAngle);

      m_nCenterSurfaceX = 0;
      m_nCenterSurfaceY = 0;
   }

   void Reinitialize(double dCenterLat, double dCenterLon, double dRadiusMeters,
      double dStartHeading, double dEndHeading, double dSweepAngle)
   {
      m_dCenterLat = dCenterLat;
      m_dCenterLon = dCenterLon;
      m_dRadiusMeters = dRadiusMeters;
      m_dStartHeading = dStartHeading;
      m_dEndHeading = dEndHeading;
      m_dSweepAngle = dSweepAngle;
   }

   /// <summary>
   /// Based on the given map's geo-bounds computes a clipped geo-bounds based this circular clip region
   /// <summary>
   /// <returns>Returns SUCCESS if successful, FAILURE otherwise<returns>
   virtual int ComputeClippedGeoBounds(MapProj *pMap, double& ll_lat, double& ll_lon, double& ur_lat, double& ur_lon);

   virtual int ClipElevationData(short *pElevationData, short* pClippedData, int nWidth, int nHeight,
         double llLat, double llLon, double dppLat, double dppLon);

   double GetRadiusMeters() { return m_dRadiusMeters; }
   double GetSweepAngle() { return m_dSweepAngle; }

   BOOL DidScreenPositionChange(double dLat, double dLon);
   BOOL DidStartHeadingChange(double dStartHeading);

protected:
   /// <summary>
   /// Helper function to determine the closest angle, within the given range, to a target angle
   /// </summary>
   double calc_closest_angle(double start, double end, double target);
};