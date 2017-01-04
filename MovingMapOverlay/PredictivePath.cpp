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
#include "PredictivePath.h"
#include "gpsstyle.h"

#include "fvwutil.h"

void CPredictivePath::ComputePredictivePath(const PredictivePathSamplePoint& samplePoint1, 
                                            const PredictivePathSamplePoint& samplePoint2,
                                            COleDateTime dtStartTime, 
                                            CPredictivePathProperties* pPredictivePathProp)
{
   m_vecPredictivePoints.clear();

   if ( samplePoint1.dLat < -90.0 || samplePoint2.dLat < -90.0 )
      return;

   int cPoints = pPredictivePathProp->get_num_trend_dots();
   ASSERT( samplePoint1.dtDateTime <= samplePoint2.dtDateTime );

   // A time line showing where stuff falls ( increasing time --> )
   //
   // samplePoint1------samplePoint2----------dStartTimeSecs---------------(dtStartTime + nLastTrendDotTime)
   //                         ^------ span ------^
   //                         ^----------------------- total time ---------------------^

   const int nLastTrendDotTime = pPredictivePathProp->get_trend_dot_time( cPoints - 1 );

   COleDateTimeSpan span = dtStartTime - samplePoint2.dtDateTime;
   int nTotalTime = nLastTrendDotTime + static_cast<int>( span.GetTotalSeconds() );

   // if the last known point was more than 120 seconds away, then no predictions are made
   if (nTotalTime > (nLastTrendDotTime + 120))
      return;

   // Compute the acceleration between the two sample points.  We check to make sure we 
   // don't divide by zero.
   double
      dAccelerationFtSecSec = 0.0,
      dRateOfClimbMetersSec = 0.0,
      dRateOfTurnDegSec = 0.0,
      dDeltaTime = COleDateTimeSpan( samplePoint2.dtDateTime - samplePoint1.dtDateTime ).GetTotalSeconds();
   
   //  We check to make sure we don't divide by zero.
   if ( dDeltaTime > 1e-6 )
   {
      // Compute the acceleration between the two sample points
      dAccelerationFtSecSec = ( samplePoint2.dSpeedFtPerSec - samplePoint1.dSpeedFtPerSec ) / dDeltaTime;
      dRateOfTurnDegSec = ( fmod( samplePoint2.dHeadingDeg - samplePoint1.dHeadingDeg + 540.0, 360.0 ) - 180.0 )
         / dDeltaTime,
      dRateOfClimbMetersSec = ( samplePoint2.dAltMeters - samplePoint1.dAltMeters ) / dDeltaTime;
   }
   
   // The delta distance uses the formula: d = x0 + v0*t + 1/2*a*t^2.  x0 is the initial distance
   // and our case this is zero.  Also, the total time is computed for one second.  So, the formula
   // reduces to d = 0 + v0*1.0 + 1/2*a*(1.0)^2 = v0 + 1/2*a.

   CFvwUtil *pFvwUtil = CFvwUtil::get_instance();

   // Note that we compute a predictive point for each point past samplePoint2 since dCurrentLat and
   // dCurrentLon are cummulative.  However, only points that start at the given 'dtStartTime'
   // parameter are actually added to the list of predictive path points.
   //
   double
      dCurrentLat = samplePoint2.dLat,
      dCurrentLon = samplePoint2.dLon;
   for ( int i = 0; i < nTotalTime; i++ )
   {
      if ( i > 0  && GEO_calc_end_point( dCurrentLat, dCurrentLon,
            FEET_TO_METERS( 1.0 * ( samplePoint2.dSpeedFtPerSec + ( dAccelerationFtSecSec * ( i - 0.5 ) ) ) ),
            pFvwUtil->normalize_angle( samplePoint2.dHeadingDeg + ( dRateOfTurnDegSec * ( i - 0.5 ) ) ), 
            &dCurrentLat, &dCurrentLon, TRUE ) != SUCCESS )
         return;  // Endpoint calc failed, no more predictive points

      if ( i >= span.GetTotalSeconds() )  // Ready to start saving
      {
         // Save the predictive point
         PredictivePoint ppt;
         ppt.m_gptLatLon.lat = dCurrentLat;
         ppt.m_gptLatLon.lon = dCurrentLon;
         ppt.m_dAltMeters = samplePoint2.dAltMeters + ( dRateOfClimbMetersSec * i );
         ppt.m_dHeadingDeg = pFvwUtil->normalize_angle( samplePoint2.dHeadingDeg + ( dRateOfTurnDegSec * i ) );
         ppt.m_dSpeedFtPerSec = samplePoint2.dSpeedFtPerSec + ( dAccelerationFtSecSec * i );
         m_vecPredictivePoints.push_back( ppt );
      }
   }  // Integration loop

}  // ComputePredictivePath()


// End of PredictivePath.cpp
