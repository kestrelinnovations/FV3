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



#pragma once

class ActiveMap;
class CPredictivePath;
class CPredictivePathProperties;
class MapProj;

/// <summary>
/// This class is used to render a predictive path on the screen.  All methods are static because
/// this object has no state.
/// </summary>
class CPredictivePathRenderer
{
public:
   /// <summary>
   /// Renders the predictive path trend dots on the screen.  The relative time and number of trend dots is
   /// given by the properties object.  The geographic location of the trend dots is given by the predictive
   /// path object itself.
   /// </summary>
   /// <param name="pMap">Map used to compute geographic to projected coordinate system transforms</param>
   /// <param name="pDC">Graphics context used to darw the predictive path trend dots</param>
   /// <param name="pPredictivePath">The predictive path object that represents a list of predictive path points</param>
   /// <param name="pProp">The properties objects</param>
   static void RenderTrendDots(MapProj* pMap, CDC* pDC, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp);

   /// <summary>
   /// Renders the predictive path center line on the screen.  The geographic location of the trend dots is
   /// given by the predictive path object itself.
   /// </summary>
   /// <param name="pMap">Map used to compute geographic to projected coordinate system transforms</param>
   /// <param name="pDC">Graphics context used to darw the predictive path center line</param>
   /// <param name="pPredictivePath">The predictive path object that represents a list of predictive path points</param>
   /// <param name="pProp">The properties objects</param>
   static void RenderCenterLine(MapProj* pMap, CDC* pDC, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp);
   
   /// <summary>
   /// Invalidates the location of the trend dots on the screen
   /// </summary>
   /// <param name="pMap">Map used to compute geographic to projected coordinate system transforms</param>
   /// <param name="pPredictivePath">The predictive path object that represents a list of predictive path points</param>
   /// <param name="pProp">The properties objects</param>
   static void InvalidateTrendDots(MapProj* pMap, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp);

   /// <summary>
   /// Invalidates the location of the center line on the screen
   /// </summary>
   /// <param name="pMap">Map used to compute geographic to projected coordinate system transforms and for the 
   /// graphics context</param>
   /// <param name="pPredictivePath">The predictive path object that represents a list of predictive path points</param>
   /// <param name="pProp">The properties objects</param>
   static void InvalidateCenterLine(MapProj* pMap, CPredictivePath *pPredictivePath, CPredictivePathProperties *pProp);
};