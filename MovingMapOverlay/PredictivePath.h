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



#pragma once

#include "geo_tool\geo_tool_d.h"
#include "gps.h"

class CPredictivePathProperties;

/// <summary>
/// This structure holds the necessary information to compute predictive path points used 
/// in the ComputePredictivePath method.
/// </summary>
struct PredictivePathSamplePoint
{
   /// <summary>
   /// Default constructor for the PredictivePathSamplePoint structure
   /// </summary>
   PredictivePathSamplePoint()
   {
      ZeroMemory( this,  sizeof(*this) );
   }

   /// <summary>
   /// Constructor for the PredictivePathSamplePoint structure
   /// </summary>
   PredictivePathSamplePoint(COleDateTime _dtDateTime, double _dSpeedFtPerSec, double _dHeading,
      double _dLat, double _dLon, double _dAltMeters )
   {
      dtDateTime = _dtDateTime;
      dSpeedFtPerSec = _dSpeedFtPerSec;
      dHeadingDeg = _dHeading;
      dLat = _dLat;
      dLon = _dLon;
      dAltMeters = _dAltMeters;
   }

   COleDateTime dtDateTime;
   double dSpeedFtPerSec;
   double dHeadingDeg;
   double dLat;
   double dLon;
   double dAltMeters;
};


struct PredictivePoint
{
   d_geo_t  m_gptLatLon;
   double   m_dAltMeters;
   double   m_dHeadingDeg;
   double   m_dSpeedFtPerSec;
}; // struct PredictivePoint

typedef std::vector< PredictivePoint > PredictivePoints;
typedef PredictivePoints::iterator PredictivePointsIter;



/// <summary>
/// This class is responsible for encapsulating and calculating the predictive path points
/// </summary>
class CPredictivePath
{
   PredictivePoints     m_vecPredictivePoints;
   PredictivePointsIter m_it;

public:

   /// <summary>
   /// This method computes a list of predictive path points starting at dStartTime and ending
   /// at dStartTime plus the last trend dot time.  The two given sample points are used to 
   /// extrapolate a set of locations using a second order dead reckoning algorithm.
   /// </summary>
   /// <param name="samplePoint1">First sample point used to compute predictive points.</param>
   /// <param name="samplePoint2">Second sample point used to compute predictive points.</param>
   /// <param name="dStartTimeSecs">The starting time for the first predictive point.</param>
   /// <param name="pPredictivePathProp">The properties used to control the predictive path, 
   /// e.g., number of trend dots.</param>
   /// <remarks>This method assumes that the time of samplePoint1 is less than the time of samplePoint2.</remarks>
   void ComputePredictivePath(const PredictivePathSamplePoint& samplePoint1, const PredictivePathSamplePoint& samplePoint2,
      COleDateTime dtStartTime, CPredictivePathProperties* pPredictivePathProp);

   /// <summary>
   /// Moves to the first predictive point in the list.
   /// <summary>
   /// <returns>Returns true if there are any points in the list, false otherwise</returns>
   bool MoveFirst()
   {
      m_it = m_vecPredictivePoints.begin();
      return m_it != m_vecPredictivePoints.end();
   }

   /// <summary>
   /// Moves to the next predictive point in the list
   /// </summary>
   /// <returns>Returns true if there is a next valid point in the list, false otherwise</returns>
   bool MoveNext()
   {
      m_it++;
      return m_it != m_vecPredictivePoints.end();
   }

   /// <summary>
   /// Moves to the given predictive point in the list
   /// </summary>
   /// <param name="n">The nth predictive path point</param>
   /// <returns>Returns true if there is a valid point in the list, false otherwise</returns>
   bool MoveTo(int n)
   {
      if (n < 0 || (size_t)n >= m_vecPredictivePoints.size())
         return false;

      m_it = m_vecPredictivePoints.begin() + n;
      return true;
   }

   /// <value>The latitude of the current predictive path point</value>
   double get_m_Latitude() const
   {
      return m_it->m_gptLatLon.lat;
   }

   /// <value>The longitude of the current predictive path point</value>
   double get_m_Longitude() const
   {
      return m_it->m_gptLatLon.lon;
   }

   /// <value>The altitude of the current predictive path point</value>
   double get_AltMeters() const
   {
      return m_it->m_dAltMeters;
   }

   /// <value>The heading at the current predictive path point</value>
   double get_HeadingDeg() const
   {
      return m_it->m_dHeadingDeg;
   }

   /// <value>The speed at the current predictive path point</value>
   double get_SpeedFtPerSec() const
   {
      return m_it->m_dSpeedFtPerSec;
   }
}; // class CPredictivePath

// End of PredictivePath.h
