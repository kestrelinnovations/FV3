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

// elevcalc.h

#pragma once

#ifndef ELEVCALC_H
#define ELEVCALC_H 1

#include "nma_dted.h"
#include <afxtempl.h>  // for CList

#import "VvodAnalysisServer.tlb" no_namespace, named_guids

class CElevCalc;

typedef void (*percent_callback_t)(double);

class CElevSeg
{
public:

   double m_lat;
   double m_lon;
   double m_left_width;  // in meters
   double m_right_width;  // in meters
   int m_max_elev;  // calced - in feet (including ends)
   int m_max_types_used;  // calced - packed indicator or dted types used for elevation info, 1 = dted1, 2 = dted2, 4 = dted3
   double m_max_elev_lat;  // calced (including ends)
   double m_max_elev_lon;  // calced (including ends)
   BOOL m_valid_data;  // calced - is data complete and valid for seg

   CElevSeg();
   ~CElevSeg();
};

// *************************************************************
// *************************************************************


class CElevLeg
{
public:

   double m_lat1;  // starting latitute of leg
   double m_lon1;  // starting longitute of leg
   double m_lat2;  // ending latitute of leg
   double m_lon2;  // ending longitute of leg
   double m_left_width;  // in meters
   double m_right_width;  // in meters
   int m_num_shape_points;  // number of lat/long shape points (used only if total points is unspecified
   double *m_shape_lat;  // array of shape lats
   double *m_shape_lon;  // array of shape lons
   double *m_shape_angle;  // array of shape angles
   double m_length;  // calced - in meters
   double m_bearing;  // calced - decimal degrees (0 is north)
   int m_max_elev;  // calced - in feet (including ends)
   int m_max_types_used;  // calced - packed indicator or dted types used for elevation info, 1 = dted1, 2 = dted2, 4 = dted3
   double m_max_elev_lat;  // calced (including ends)
   double m_max_elev_lon;  // calced (including ends)
   double m_start_dist;  // calced - distance along seg of first point
   double m_end_dist;  // calced - distance along seg of last point
   double m_inc_dist;  // calced - distance between points
   int m_num_points;  // calced - number of elev points in array
   int *m_elev_array; // malloced and calced - elev points
   int *m_elev_no_vvod_array;  // malloced and calced - elev points
   BOOL m_valid_data;  // calced - is data complete and valid for seg

   CElevLeg();
   ~CElevLeg();
};

// *************************************************************
// *************************************************************

class CElevCalc
{
public:

   CElevCalc();  
   ~CElevCalc();

   double m_percent;
   int m_max_dted_level;

   percent_callback_t m_callback;


   int get_max_elev_of_leg(double lat1, double lon1,
                     double lat2, double lon2,
                     double left_width, double right_width, // in meters
                     combo_dted_hit_and_miss_cache *dted_cache1,
                     combo_dted_hit_and_miss_cache *dted_cache2,
                     int *max_elev, double *max_elev_lat, double *max_elev_lon, // OUTPUT in feet
                     int *types_used);

   // return the max elev of only the route ends
   int get_max_elev_of_leg_ends(double lat1, double lon1, double lat2, double lon2,
                        double left_width, double right_width, // in meters
                        combo_dted_hit_and_miss_cache *dted_cache1,
                        combo_dted_hit_and_miss_cache *dted_cache2,
                        IVvodAnalysisPtr &smpVvodAnalysis,
                        int *seg_max_elev, double *seg_max_elev_lat, double *seg_max_elev_lon, // OUTPUT in feet
                        int *types_used);

   // same as above except the angle of each point is specified
   int get_max_elev_of_leg2(double lat1, double lon1,
                           double lat2, double lon2,
                           double angle1, double angle2,
                           double left_width, double right_width, // in meters
                           combo_dted_hit_and_miss_cache *dted_cache1,
                           combo_dted_hit_and_miss_cache *dted_cache2,
                           IVvodAnalysisPtr &smpVvodAnalysis,
                           int *max_elev, int *max_elev_no_vvod, 
                           double *max_elev_lat, double *max_elev_lon, // OUTPUT in feet
                           int *types_used);

   void GetVVODMaxElevInPoly(IVvodAnalysisPtr &smpVvodAnalysis, int nNumVertices, double *pLat, double *pLon, 
                                     int *pMaxElev, double *pMaxElevLat, double *pMaxElevLon);

   // this function allows caller to set a callback function that can be used to 
   // provide user feedback on the progress of the elevation array process
   void set_percent_callback(percent_callback_t callback) {m_callback = callback;}

   // this function sets the maximum dted level used in the search
   void set_max_dted_level(int max_dted_level);

   // this function returns max elevation within the semi-circular ends of a route leg
   int get_max_elev_of_leg_ends(double lat1, double lon1, double lat2, double lon2,
                        double left_width, double right_width, // in meters
                        IVvodAnalysisPtr &smpVvodAnalysis,
                        int *seg_max_elev, double *seg_max_elev_lat, double *seg_max_elev_lon, // OUTPUT in feet
                        int *types_used);

   // this function returns the maximum elevation within a rectangle route leg
   int get_max_elev_of_leg(double lat1, double lon1,
                     double lat2, double lon2,
                     double left_width, double right_width, // in meters
                     int *max_elev, double *max_elev_lat, double *max_elev_lon, // OUTPUT in feet
                     int *types_used);

   // this function gets an elevation array for a list of legs.  
   // num_points is the total number of points in the array.  
   // Each leg will have a number of points proportional to it's length.
   // If num_points is 0, then the number of points in each leg is taken from "leg->m_num_shape_points"
   // In all cases the elevation array will be returned as an array in each leg.  
   // In the case of num_points==0 and num_shape_points==0 the array will contain only the max elev point.
   // The max elev point will include the semi-circular area around both ends of each leg
   int get_elevation_array(CList<CElevLeg*, CElevLeg*> &leg_list, int num_points, int *types_used);


   // this function calculates the 2D (top down) distance along a path and the 3D (walking) distance 
   // accounting for changes in elevation
   int get_path_distance(double *lat, double *lon, int num_points, // array of input points
                    double *dist_2d,  // total flat surface distance in path
                    double *dist_3d,   // total distance including elevation changes
                    int *types_used);

   int get_elev_in_feet(double lat, double lon, 
                  combo_dted_hit_and_miss_cache &dted_cache1,
                  combo_dted_hit_and_miss_cache &dted_cache2,
                  combo_dted_hit_and_miss_cache &dted_cache3,
                  BOOL *dted2, BOOL *dted3, int *elev );

   int get_path_distance(double lat1, double lon1, double lat2, double lon2, 
                  combo_dted_hit_and_miss_cache &dted_cache1,
                  combo_dted_hit_and_miss_cache &dted_cache2,
                  combo_dted_hit_and_miss_cache &dted_cache3,
                  double *dist_2d,  // total flat surface distance
                  double *dist_3d,   // total distance including elevation changes
                  int *types_used);


};

#endif  // #ifndef ELEVCALC_H
