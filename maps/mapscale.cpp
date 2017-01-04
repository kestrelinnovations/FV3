// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// MapScale member functions

#include "stdafx.h"
#include "mapx.h"       // MAP_resolution_to_degrees
#include "mem.h"        // MEM_malloc, MEM_free
#include "Common\ComErrorObject.h"

/*------------------------------------------------------------------
-                          Definitions 
- -----------------------------------------------------------------*/

// CADRG 1:1 M (ONC) has this many pixels pole to pole
#define ONE_TO_1M_PIXELS_POLE_TO_POLE (2.0 * 66816.0)

// TIROS 1 km has this many pixels pole to pole
#define ONE_KM_RES_PIXELS_POLE_TO_POLE 20000.0

// The maximum value ever returned in size by the serialize member of MapScale.
// If you write the contents of buffer to the registry, you will need to pass
// a buffer of this size to the registry reading function, in order to read
// it back in.  The MAP_get_registry_scale function depends on this value.
#define MAP_SCALE_MAX_BUFFER_SIZE 12

///////////////////////////////////////////////////////////////////////////////
// Global Variables

// Predefined Scales
const MapScale ScaleDefines::m_world(WORLD_OVERVIEW);  // BLANK_MAP, VECTOR and TIROS
const MapScale ScaleDefines::m_80M(80000000);          // BLANK_MAP, VECTOR and TIROS 
const MapScale ScaleDefines::m_40M(40000000);          // BLANK_MAP, VECTOR and TIROS 
const MapScale ScaleDefines::m_20M(20000000);          // BLANK_MAP, VECTOR and TIROS 
const MapScale ScaleDefines::m_10M(10000000);          // BLANK_MAP and VECTOR 
const MapScale ScaleDefines::m_5M(5000000);            // CADRG, BLANK_MAP and VECTOR 
const MapScale ScaleDefines::m_4M(4000000);            // future sources 
const MapScale ScaleDefines::m_2M(2000000);            // CADRG, BLANK_MAP and VECTOR 
const MapScale ScaleDefines::m_1M(1000000);            // CADRG, BLANK_MAP and VECTOR             
const MapScale ScaleDefines::m_800K(800000);          // future sources 
//#define ONE_TO_501K ScaleDefines m_  // CADRG LFC
const MapScale ScaleDefines::m_500K(500000);           // CADRG and BLANK_MAP 
const MapScale ScaleDefines::m_dted(3.0, MapScale::ARC_SECONDS);   // DTED close to 1:500 K 
const MapScale ScaleDefines::m_400K(400000);           // future sources 
//#define ONE_TO_251K ScaleDefines m_  // CADRG TFC
const MapScale ScaleDefines::m_250K(250000);           // CADRG, BLANK_MAP and GEOTIFF
const MapScale ScaleDefines::m_200K(200000);           // future sources 
const MapScale ScaleDefines::m_100K(100000);           // BLANK_MAP, GEOTIFF 
const MapScale ScaleDefines::m_80K(80000);             // future sources 
const MapScale ScaleDefines::m_50K(50000);             // CADRG and BLANK_MAP 
const MapScale ScaleDefines::m_40K(40000);             // future sources 
const MapScale ScaleDefines::m_63360(63360);           // GEOTIFF
const MapScale ScaleDefines::m_30K(30000);             // GEOTIFF
const MapScale ScaleDefines::m_25K(25000);             // GEOTIFF
const MapScale ScaleDefines::m_24K(24000);             // GEOTIFF
const MapScale ScaleDefines::m_20K(20000);             // BLANK_MAP, GEOTIFF, and future sources 
const MapScale ScaleDefines::m_10K(10000);             // BLANK_MAP and future sources 
const MapScale ScaleDefines::m_8K(8000);               // future sources 
const MapScale ScaleDefines::m_5K(5000);               // BLANK_MAP and future sources 
const MapScale ScaleDefines::m_4K(4000);               // future sources 
const MapScale ScaleDefines::m_2K(2000);               // BLANK_MAP and future sources 
const MapScale ScaleDefines::m_1K(1000);               // BLANK_MAP and future sources 
const MapScale ScaleDefines::m_null;

const MapScale ScaleDefines::m_10_meter(10.0, MapScale::METERS);   // 10 meter 
const MapScale ScaleDefines::m_5_meter(5.0, MapScale::METERS);     // 5 meter 
const MapScale ScaleDefines::m_2_meter(2.0, MapScale::METERS);     // 2 meter
const MapScale ScaleDefines::m_1_meter(1.0, MapScale::METERS);     // 1 meter

///////////////////////////////////////////////////////////////////////////////
// MapScale Class Implementation

// Constructor
MapScale::MapScale()
{
   m_scale = 0;            // NULL_SCALE
   m_resolution = 0.0;     // a chart scale
   m_resolution_unit = NM;
}

// Constructor for a chart scale, e.g., 1:5 M.  If you use this
// constructor, get_string() will return a scale-string, e.g.,
// "1:5 M".  1:scale_denominator is the chart scale.
MapScale::MapScale(int scale_denominator)
{
   // 0 is for NULL_SCALE.  WORLD_OVERVIEW is a special value outside of the
   // normal range.  The normal range is 1 to 110 million.
   if (scale_denominator < 0 || 
      (scale_denominator > 500000000 && scale_denominator != WORLD_OVERVIEW))
   {
      m_scale = 0;
      ERR_report("scale_denominator must be between 1 and 500 million, or "
         "WORLD_OVERVIEW.");
      ASSERT(0);
   }
   else
   {
      m_scale = scale_denominator;
   }
   m_resolution = 0.0;     // a chart scale
   m_resolution_unit = NM;
}

// Constructor for imagery, elevation posts, or other data types where the
// resolution is given rather than the scale.  The units of resolution are 
// specified by resolution_unit.  If you use this constructor, get_string()
// will return a resolution string based on the units specified.  For 
// example, MapScale(10.0, MapScale::METERS) will result in get_string()
// returning "10 meter".
MapScale::MapScale(double resolution, unit_t resolution_unit)
{
   ASSERT(resolution > 0.0);

   // in case of error, revert to the NULL_SCALE
   int scale = resolution_to_scale(resolution, resolution_unit);
   if (scale == 0)
   {
      ASSERT(0);
      m_scale = 0;            // NULL_SCALE
      m_resolution = 0.0;     // a chart scale
      m_resolution_unit = NM;
   }
   else
   {
      // assign a scale that is relative to CADRG
      m_scale = scale;
      m_resolution = resolution;
      m_resolution_unit = resolution_unit;
   }
}

MapScale::MapScale(double dScale, MapScaleUnitsEnum eScaleUnits)
{
   if (eScaleUnits == MAP_SCALE_DENOMINATOR)
   {
      // 0 is for NULL_SCALE.  WORLD_OVERVIEW is a special value outside of the
      // normal range.  The normal range is 1 to 110 million.
      if (dScale < 0 || dScale > 500000000)
      {
         m_scale = 0;
         ERR_report("scale_denominator must be between 1 and 500 million, or "
            "WORLD_OVERVIEW.");
         ASSERT(0);
      }
      else
      {
         m_scale = static_cast<int>(dScale);
      }
      m_resolution = 0.0;     // a chart scale
      m_resolution_unit = NM;
   }
   else if (eScaleUnits == MAP_SCALE_WORLD)
   {
      m_scale = WORLD_OVERVIEW;
      m_resolution = 0.0;
      m_resolution_unit = NM;
   }
   else
   {
      ASSERT(dScale > 0.0);

      MapScale::unit_t units;
      switch (eScaleUnits)
      {
      case MAP_SCALE_NM: units = MapScale::NM; break;
      case MAP_SCALE_MILE: units = MapScale::MILE; break;
      case MAP_SCALE_KILOMETER: units = MapScale::KILOMETER; break;
      case MAP_SCALE_METERS : units = MapScale::METERS; break;
      case MAP_SCALE_YARDS : units = MapScale::YARDS; break;
      case MAP_SCALE_FEET : units = MapScale::FEET; break;
      case MAP_SCALE_INCHES : units = MapScale::INCHES; break;
      case MAP_SCALE_ARC_DEGREES : units = MapScale::ARC_DEGREES; break;
      case MAP_SCALE_ARC_MINUTES : units = MapScale::ARC_MINUTES; break;
      case MAP_SCALE_ARC_SECONDS: units = MapScale::ARC_SECONDS; break;
      };
      
      // in case of error, revert to the NULL_SCALE
      int scale = resolution_to_scale(dScale, units);
      if (scale == 0)
      {
         ASSERT(0);
         m_scale = 0;            // NULL_SCALE
         m_resolution = 0.0;     // a chart scale
         m_resolution_unit = NM;
      }
      else
      {
         // assign a scale that is relative to CADRG
         m_scale = scale;
         m_resolution = dScale;
         m_resolution_unit = units;
      }
   }
}

// Copy constructor
MapScale::MapScale(const MapScale &scale)
{
   m_scale = scale.m_scale;
   m_resolution = scale.m_resolution;
   m_resolution_unit = scale.m_resolution_unit;
}

// Assignment
const MapScale& MapScale::operator =(const MapScale &scale)
{
   m_scale = scale.m_scale;
   m_resolution = scale.m_resolution;
   m_resolution_unit = scale.m_resolution_unit;

   return *this;
}

MapScaleUnitsEnum MapScale::GetScaleUnits() const
{
   if (m_scale == WORLD_OVERVIEW)
      return MAP_SCALE_WORLD;
   
   if (m_resolution == 0.0)
      return MAP_SCALE_DENOMINATOR;
   
   switch(m_resolution_unit)
   {
   case MapScale::NM: return MAP_SCALE_NM;
   case MapScale::MILE: return MAP_SCALE_MILE;
   case MapScale::KILOMETER: return MAP_SCALE_KILOMETER;
   case MapScale::METERS: return MAP_SCALE_METERS;
   case MapScale::YARDS: return MAP_SCALE_YARDS;
   case MapScale::FEET: return MAP_SCALE_FEET;
   case MapScale::INCHES: return MAP_SCALE_INCHES;
   case MapScale::ARC_DEGREES: return MAP_SCALE_ARC_DEGREES;
   case MapScale::ARC_MINUTES: return MAP_SCALE_ARC_MINUTES;
   case MapScale::ARC_SECONDS: return MAP_SCALE_ARC_SECONDS;
   };
   
   return MAP_SCALE_NM;
}

// Comparison
BOOL operator ==(const MapScale &s1, const MapScale &s2)
{
   static const double EPS = 1e-6;

   // Although resolution is not considered for <= or >=, it is considered 
   // here, because you need to be certain that two MapScale objects are
   // exactly equivalent when == returns TRUE.
   return (s1.m_scale == s2.m_scale && fabs(s1.m_resolution - s2.m_resolution) < EPS &&
      s1.m_resolution_unit == s2.m_resolution_unit);
}

BOOL operator !=(const MapScale &s1, const MapScale &s2)
{
   static const double EPS = 1e-6;

   // Although resolution is not considered for <= or >=, it is considered 
   // here, because you need to be certain that two MapScale objects are
   // exactly equivalent when == returns TRUE.
   return (s1.m_scale != s2.m_scale || fabs(s1.m_resolution - s2.m_resolution) >= EPS ||
      s1.m_resolution_unit != s2.m_resolution_unit);
}

BOOL operator <(const MapScale &s1, const MapScale &s2)
{
   // since m_scale is the s2 denominator smaller m_scale implies larger
   // MapScale
   return (s1.m_scale > s2.m_scale);
}

BOOL operator >(const MapScale &s1, const MapScale &s2)
{
   // since m_scale is the s2 denominator smaller m_scale implies larger
   // MapScale
   return (s1.m_scale < s2.m_scale);
}

BOOL operator <=(const MapScale &s1, const MapScale &s2)
{
   // since m_scale is the s2 denominator smaller m_scale implies larger
   // MapScale
   return (s1.m_scale >= s2.m_scale);
}

BOOL operator >=(const MapScale &s1, const MapScale &s2)
{
   // since m_scale is the s2 denominator smaller m_scale implies larger
   // MapScale
   return (s1.m_scale <= s2.m_scale);
}

// Get user interface string.  The string will be of the form "1:5 M", 
// "1:500 K", or "1:500" for a scale set via the scale denominator.  The
// string will be of the form "10 meter", "1 foot", "3 arc second" for a
// scale set via a resolution.
CString MapScale::get_string() const
{
   CString str;

   try
   {
      IMapSeriesStringConverterPtr spConverter;
      CO_CREATE(spConverter, CLSID_MapSeriesStringConverter);

      _bstr_t ret = spConverter->ToString(_bstr_t(""), GetScale(), GetScaleUnits(), _bstr_t(""), VARIANT_FALSE, FORMAT_SCALE);
      str = (char *)ret;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("MapScale::get_string failed - %s", (char *)e.Description());
      ERR_report(msg);
      return "";
   }

   return str;
}

// Parses a valid user interface string and sets itself from the string.
// If the string is invalid calling this function will set it equal to
// NULL_SCALE.
void MapScale::set_from_string(const char *string)
{
   // set yourself to NULL_SCALE in case of failure
   m_scale = 0;
   m_resolution = 0.0;
   m_resolution_unit = NM;

   CString str(string);

   // need to get rid of any commas in the string (e.g "1:17,000" -> "1:17000")
   str.Remove(',');

   // WORLD_OVERVIEW is a special case
   if (str == "World")
   {
      m_scale = WORLD_OVERVIEW;
   }
   else
   {
      int count;
      int one;
      int denominator;
      char M_or_K;

      // see if this is a chart scale, e.g., 1:10 M, 1:10 K, or 1:10
      count = sscanf_s(str, "%d:%d %c", &one, &denominator, &M_or_K, 1);
      if (count >= 2) 
      {
         // always 1:denominator, where denominator is between 1 and 999
         if (one == 1 && 0 < denominator)
         {
            if (count == 2)
               m_scale = denominator;
            else if (M_or_K == 'M')
               m_scale = denominator * 1000000;
            else if (M_or_K == 'K')
               m_scale = denominator * 1000;
         }
      }
      // see if this is a resolution string
      else if (str.GetLength() < 80)
      {
         double resolution;
         const int UNIT_STRING_LEN = 80;
         char unit_string[UNIT_STRING_LEN];

         if (sscanf_s(str, "%lf %s", &resolution, unit_string, UNIT_STRING_LEN) == 2)
         {
            unit_t unit;

            if (resolution > 0.0 && string_to_unit(unit_string, unit))
            {
               // returns 0 for failure
               m_scale = resolution_to_scale(resolution, unit);
               if (m_scale)
               {
                  m_resolution = resolution;
                  m_resolution_unit = unit;
               }
            }
         }
      }
   }
}

// Returns a nominal degrees latitude per pixel value for displaying a maps
// at this MapScale.  This value, or one close to it, should be used for
// displaying all map data on a monitor.  If you want to know the relative
// difference between two different MapScale objects you can use the ratio
// of their degrees latitude per pixel values.
degrees_t MapScale::get_nominal_degrees_lat_per_pixel() const
{
   ASSERT(m_scale > 0);

   // base map scales of 1:5 M or larger off of 1:1 M CADRG
   if (m_scale <= 5000000)
   {
      return (((double)m_scale / 1000000.0) * 
         (180.0 / ONE_TO_1M_PIXELS_POLE_TO_POLE));
   }
   // base small scale maps off of 1 km TIROS
   else
   {
      int nOneKilometerScale = MapScale(1.0, MapScale::KILOMETER).m_scale;
      return (((double)m_scale / nOneKilometerScale) *
         (180.0 / ONE_KM_RES_PIXELS_POLE_TO_POLE));
   }
}

// For chart scales, scales defined via the scale denomator, this function 
// returns the scale denominator directly.  For scales set via resolution,
// e.g., 10 meter or 3 arc second, a nominal value will be returned.
int MapScale::get_to_scale_denominator() const
{
   if (m_resolution > 0.0 && m_scale > 0)
   {
      // TO DO: power = 10 ^ floor(log(m_scale))
      // set power to the greatest power of 10 less or equal to m_scale
      int power = 10000000; // 10M
      while (power > m_scale) power = power / 10;

      // if m_scale happens to already be a nice round number
      if (m_scale % power == 0)
         return m_scale;

      // avoid rounding more than 10%, e.g., 1:1.5 K, and 1:1.5 M
      if (m_scale < 2 * power)
         power = power / 10;

      return (int)((double)m_scale / (double)power + 0.5) * power;
   }
   else
      return m_scale;
}

// An buffer of the necessary size is dynamically created and filled in with
// this object's data.  size is set to the number of bytes in buffer.  You
// must call free_buffer() to release this memory.
int MapScale::serialize(BYTE **buffer, DWORD *size) const
{
   float resolution = static_cast<float>(m_resolution);

   *size = sizeof(m_scale) + sizeof(resolution) + sizeof(m_resolution_unit);

   *buffer = (BYTE *)MEM_malloc(*size);
   if (*buffer == NULL)
   {
      *size = 0;
      ERR_report("MEM_malloc() failed.");
      return FAILURE;
   }

   memcpy(*buffer, &m_scale, sizeof(m_scale));
   int offset = sizeof(m_scale);
   memcpy(*buffer + offset, &resolution, sizeof(resolution));
   offset += sizeof(resolution);
   memcpy(*buffer + offset, &m_resolution_unit, sizeof(m_resolution_unit));

   return SUCCESS;
}

// free a buffer created by serialize().
int MapScale::free_buffer(BYTE *buffer) const
{
   if (buffer == NULL)
   {
      ERR_report("NULL pointer.");
      return FAILURE;
   }

   MEM_free(buffer);

   return SUCCESS;
}

// Unpack the given buffer and set this object's data from the contents.
// This function will have no effect on this object if it returns FAILURE.
int MapScale::deserialize(BYTE *buffer, DWORD size)
{
   int scale;
   unit_t unit;
   float resolution;

   // the size of the deserialize buffer must match the serialize buffer 
   // exactly
   if (size != sizeof(m_scale) + sizeof(resolution) + 
      sizeof(m_resolution_unit))
   {
      ERR_report("buffer size is not valid.");
      return FAILURE;
   }

   // 0 is for NULL_SCALE.  WORLD_OVERVIEW is a special value outside of the
   // normal range.  The normal range is 1 to 110 million.
   memcpy(&scale, buffer, sizeof(m_scale));
   if (scale < 0 || (scale > 240000000 && scale != WORLD_OVERVIEW))
   {
      ERR_report("Invalid scale.");
      return FAILURE;
   }

   // The resoution and unit is always saved, even though it is not used for
   // chart scales.
   int offset = sizeof(m_scale);
   memcpy(&resolution, buffer + offset, sizeof(resolution));
   offset += sizeof(resolution);
   memcpy(&unit, buffer + offset, sizeof(m_resolution_unit));

   // round to 3 decimal places
   double dbl_resolution = static_cast<int>(1000 * (resolution + 0.0005)) / 1000.0; // get rid of float -> double rounding error

   // Chart scales always have resolution of zero and unit of NM.
   if (dbl_resolution == 0.0)
   {
      if (unit != NM)
      {
         ERR_report("Zero is not a valid resolution.");
         return FAILURE;
      }
   }
   else
   {
      // Compute the scale implied by the resolution.
      // The function returns 0 in case of failure.
      int computed_scale = resolution_to_scale(dbl_resolution, unit);
      if (computed_scale == 0)
      {
         ERR_report("resolution_to_scale() failed.");
         return FAILURE;
      }
   }

   m_scale = scale;
   m_resolution = dbl_resolution;
   // round to two decimal places
   m_resolution = static_cast<double>(static_cast<int>((m_resolution * 100.0) + 0.5)) / 100.0;
   m_resolution_unit = unit;
   
   return SUCCESS;
}

// computes the nominal degrees per pixel value for a given resolution
degrees_t MapScale::resolution_to_degrees_latitude_per_pixel(double resolution, 
   unit_t unit) const
{
   if (resolution <= 0.0)
   {
      ERR_report("Invalid resolution.");
      return 0.0;
   }

   // the number of degrees of latitude covered by 1 pixel centered on the
   // equator will determine m_scale by comparing the dpp value of this map
   // scale to the nominal dpp values of a 1:1 M chart
   degrees_t degrees_lat_per_pixel = 0.0;
   degrees_t degrees_lon_per_pixel;
   switch (unit)
   {
      case NM:
         MAP_resolution_to_degrees(0.0, NM_TO_METERS(resolution), 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case MILE:
         MAP_resolution_to_degrees(0.0, FEET_TO_METERS(resolution * 5280.0), 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case KILOMETER:
         MAP_resolution_to_degrees(0.0, resolution * 1000.0, 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case METERS:
         MAP_resolution_to_degrees(0.0, resolution, 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case YARDS:
         MAP_resolution_to_degrees(0.0, FEET_TO_METERS(resolution * 3.0), 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case FEET:
         MAP_resolution_to_degrees(0.0, FEET_TO_METERS(resolution), 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case INCHES:
         MAP_resolution_to_degrees(0.0, FEET_TO_METERS(resolution * 8.33333333333333333333333333333e-2), 
            degrees_lat_per_pixel, degrees_lon_per_pixel);
         break;
      case ARC_DEGREES:
         degrees_lat_per_pixel = resolution;
         break;
      case ARC_MINUTES:
         degrees_lat_per_pixel = MIN_TO_DEG(resolution);
         break;
      case ARC_SECONDS:
         degrees_lat_per_pixel = SEC_TO_DEG(resolution);
         break;
      default:
         ASSERT(0);
         ERR_report("Invalid unit.");
   }

   return degrees_lat_per_pixel;
}

// computes a scale value from the resolution
int MapScale::resolution_to_scale(double resolution, unit_t unit) const
{
   degrees_t dpp = resolution_to_degrees_latitude_per_pixel(resolution, unit);
   if (dpp == 0.0)
   {
      ERR_report("resolution_to_degrees_latitude_per_pixel() failed.");
      return 0;
   }

   // if you were to display a world overview equal arc projection display
   // with the resolution at the equator being the input resolution, you
   // would need this many pixels from pole to pole
   double pixels_pole_to_pole = 180.0 / dpp;

   // assign a scale that is relative to CADRG
   return (int)(1000000.0 * ONE_TO_1M_PIXELS_POLE_TO_POLE / pixels_pole_to_pole);
}

// convert a unit_t to an output string
const char *MapScale::unit_to_string(unit_t unit) const
{
   switch (unit)
   {
      case NM:
         return "NM";
      case MILE:
         return "mile";
      case KILOMETER:
         return "km";
      case METERS:
         return "meter";
      case YARDS:
         return "yard";
      case FEET:
         return "foot";
      case INCHES:
         return "inch";
      case ARC_DEGREES:
         return "arc deg";
      case ARC_MINUTES:
         return "arc min";
      case ARC_SECONDS:
         return "arc sec";
      default:
         ASSERT(0);
         return "unit";
   }
}

// convert an output string to a unit - returns TRUE for success
BOOL MapScale::string_to_unit(const char *string, unit_t &unit)
{
   if (strcmp(string, "NM") == 0)
   {
      unit = NM;
      return TRUE;
   }

   if (strcmp(string, "mile") == 0)
   {
      unit = MILE;
      return TRUE;
   }

   if (strcmp(string, "km") == 0)
   {
      unit = KILOMETER;
      return TRUE;
   }

   if (strcmp(string, "meter") == 0)
   {
      unit = METERS;
      return TRUE;
   }

   if (strcmp(string, "yard") == 0)
   {
      unit = YARDS;
      return TRUE;
   }

   if (strcmp(string, "foot") == 0)
   {
      unit = FEET;
      return TRUE;
   }

   if (strcmp(string, "inch") == 0)
   {
      unit = INCHES;
      return TRUE;
   }

   if (strcmp(string, "arc degrees") == 0)
   {
      unit = ARC_DEGREES;
      return TRUE;
   }

   if (strcmp(string, "arc minutes") == 0)
   {
      unit = ARC_MINUTES;
      return TRUE;
   }

   if (strcmp(string, "arc second") == 0)
   {
      unit = ARC_SECONDS;
      return TRUE;
   }

   return FALSE;
}

// End MapScale Class Implemenation
///////////////////////////////////////////////////////////////////////////////

// Given the center latitude and the size of a square pixel in meters, this
// function returns the degrees per pixel values for data with the given 
// resolution in an equal arc projection.
int MAP_resolution_to_degrees(degrees_t center_lat, double resolution_meters, 
   degrees_t &degrees_lat_per_pixel, degrees_t &degrees_lon_per_pixel)
{
   d_geo_t p1, p2;
   double d_lat;
   double d_lon;
   double bearing;

   // 0.01 degrees of latitude is equal to 0.6 minutes of latitude, which is
   // roughly equivalent to 0.6 NM which equals 1111.2 meters.  Similarly 0.01
   // degrees of longitude at the equator is roughtly eqivalent to 1111.2 
   // meters.  0.01 degrees of latitude will cover 1111.2 meters +/- 0.1% any
   // where.  As you move away from the equator 0.01 degrees of longitude
   // covers significantly fewer meters.

   // To avoid doing great circle calculations with points that are extremely
   // close together a geo-box of fixed size was choosen rather than trying to
   // use a resolution_meters square.

   // calculate the distance in meters of 0.01 degree of latitude at lat
   if (center_lat < 89.0)
   {
      p1.lat = center_lat + 0.005;
      p2.lat = center_lat - 0.005;
   }
   else
   {
      p1.lat = center_lat - 0.005;
      p2.lat = center_lat - 2 * 0.005;
   }

   p1.lon = p2.lon = 0.0;
   if (GEO_calc_range_and_bearing(p1, p2, d_lat, bearing, TRUE) != SUCCESS)
      return FAILURE;

   // calculate the distance in meters of 0.01 degree of longitude at lat
   p1.lon = 0.005;
   p2.lon = -0.005;
   p1.lat = p2.lat = center_lat;
   if (GEO_calc_range_and_bearing(p1, p2, d_lon, bearing, TRUE) != SUCCESS)
      return FAILURE;

   // degrees / meters * meters per pixel = degrees per pixel
   degrees_lat_per_pixel = 0.01 / d_lat * resolution_meters;
   degrees_lon_per_pixel = 0.01 / d_lon * resolution_meters;

   return SUCCESS;
}

int MAP_get_degrees_per_pixel_world(int surface_width, int surface_height,
   degrees_t &degrees_lat_per_pixel, degrees_t &degrees_lon_per_pixel)
{
   if (surface_width <= 0 || surface_height <= 0)
   {
      ERR_report("Invalid surface dimensions.");
      return FAILURE;
   }

   /* measure from center of pixels */
   degrees_lat_per_pixel = HALF_WORLD_DEG/(double)(surface_height);
   degrees_lon_per_pixel = WORLD_DEG/(double)(surface_width);

   return SUCCESS;
}

int MAP_get_degrees_per_pixel_monitor(degrees_t center_lat, const MapScale &scale,
   degrees_t &degrees_lat_per_pixel, degrees_t &degrees_lon_per_pixel)
{
   // the map scale has a nominal degrees per pixel value for every scale
   degrees_lat_per_pixel = scale.get_nominal_degrees_lat_per_pixel();

   // For scales of 1:10 M and larger you want to maintain the correct ratio
   // between the degrees latitude and longitude per pixel along the center 
   // latitude.  That is, you want a small circle to be a circle, when it is
   // centered at center_lat.
   if (scale >= ONE_TO_10M)
   {
      degrees_t d_lat, d_lon;

      if (MAP_resolution_to_degrees(center_lat, 1.0, d_lat, d_lon) != SUCCESS)
      {
         ERR_report("MAP_resolution_to_degrees() failed.");
         return FAILURE;
      }

      degrees_lon_per_pixel = degrees_lat_per_pixel * d_lon / d_lat;
   }
   else if (scale > WORLD)
   {
      //TRACE("%s: %lf x %lf\n", scale.get_string(), 360.0 / degrees_lat_per_pixel, 180.0 / degrees_lat_per_pixel);
      degrees_lon_per_pixel = degrees_lat_per_pixel;
   }
   else
   {
      ERR_report("Call MAP_get_degrees_per_pixel_world_overview().");
      return FAILURE;
   }
      
   return SUCCESS;   
}

// Read a MapScale from the registry.
MapScale MAP_get_registry_scale(const char* section, const char* value_name, 
   const MapScale &default_scale)
{
   DWORD size = MAP_SCALE_MAX_BUFFER_SIZE;
   BYTE *buffer;

   buffer = (BYTE *)MEM_malloc(size);
   if (buffer == NULL)
   {
      ERR_report("MEM_malloc() failed.");
      return default_scale;
   }

   // if we can read a binary buffer from the specified location
   if (PRM_get_registry_binary(section, value_name, buffer, &size) == SUCCESS)
   {
      MapScale scale;

      // and we can deserialize that buffer into a MapScale
      if (scale.deserialize(buffer, size) == SUCCESS)
      {
         MEM_free(buffer);
         return scale;                 // return the regitry value
      }
   }

   MEM_free(buffer);

   return default_scale;               // return the default value
}

// Write a MapScale to the registry.
int MAP_set_registry_scale(const char* section, const char* value_name, 
   const MapScale &scale)
{
   // serialize the scale
   BYTE *buffer;
   DWORD size;
   if (scale.serialize(&buffer, &size) != SUCCESS)
   {
      ERR_report("serialize() failed.");
      return FAILURE;
   }

   int status;

   // avoid writing what can not be read - MAP_get_registry_scale
   if (size > MAP_SCALE_MAX_BUFFER_SIZE)
   {
      ERR_report("Size limit exceeded.");
      status = FAILURE;
   }
   else
   {
      // write the buffer to the registry
      status = PRM_set_registry_binary(section, value_name, buffer, size);
   }

   // free the memory allocated by serialize
   scale.free_buffer(buffer);

   return status;
}
