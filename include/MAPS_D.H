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

#ifndef MAPS_D_H
#define MAPS_D_H

/*------------------------------------------------------------------
-                            Includes
- -----------------------------------------------------------------*/

#include "geo_tool/geo_tool_d.h"  // for d_geo_t


/*------------------------------------------------------------------
-                          Definitions 
- -----------------------------------------------------------------*/

// World overview is a special case scale.  It does not have a fixed
// degrees per pixel value.  Instead the entire world is stretched or
// squashed into the surface area.
#define WORLD_OVERVIEW 2147483647

// forward declarations
enum MapScaleUnitsEnum;

/*------------------------------------------------------------------
-                       Forward Declarations
- -----------------------------------------------------------------*/
class MapCategory;
class ViewMapProj;
class MapProj;
class C_icon;
class HintText;
class CFVMenuNode;

// MapSource and MapCategory are identical in form and function, but in order
// make the compiler catch people mixing up a MapSource with a MapCategory,
// they need to be separate classes.  This base class provides all of the
// functionality.
class SerializableString
{
protected:
   CString m_string;

public:
   // Constructor
   // Use deserialize() or the assignment operator to set objects created with
   // the default contructor.
   SerializableString();

   // The given string is the user interface name for this object.  It will be
   // returned by get_string().
   SerializableString(const char *string);

   // Destructor
   ~SerializableString() {}

   // Get user interface string.
   CString get_string() const { return m_string; }

   // An buffer of the necessary size is dynamically created and filled in with
   // this object's data.  size is set to the number of bytes in buffer.  You
   // must call free_buffer() to release this memory.
   int serialize(BYTE **buffer, DWORD *size) const;

   // free a buffer created by serialize().
   int free_buffer(BYTE *buffer) const;

   // Unpack the given buffer and set this object's data from the contents.
   // This function will have no effect on this object if it returns FAILURE.
   int deserialize(BYTE *buffer, DWORD size);
}; // end SerializableString

// RE-ARCH
// Change CMapSource to MapSource change map_source_t to MapSource

// A set of MapTypes, which all use data in a common format will have the same
// MapSource.  Examples of MapSources are CADRG, NIMA DTED, CIB, and GEOTIFF.
class MapSource : public SerializableString
{
public:
   // Constructor
   // Use deserialize() or the assignment operator to set objects created with
   // the default contructor.
   MapSource();

   // The given string is the user interface name for this source.  It will be
   // returned by get_string().
   MapSource(const char *string);

   // Copy constructor
   MapSource(const MapSource &source);

   // Destructor
   ~MapSource() {}

   // Assignment
   const MapSource& operator =(const MapSource &source);

   // Comparison
   friend BOOL operator ==(const MapSource &s1, const MapSource &s2);
   friend BOOL operator !=(const MapSource &s1, const MapSource &s2);

   // For storing MapSource objects in a std::map we need the < operator
   bool operator <(const MapSource &s2) const { return get_string() < s2.get_string(); }

}; // end MapSource


// RE-ARCH
// Changed CMapScale to MapScale changed map_scale_t to MapScale

// The MapScale class provides a means by which MapType objects can be sorted.
// Digital charts have a scale assigned to them, e.g. CADRG 1:5 M (GNC) has a
// scale of 1:5 M.  Digital imagery has a resolution, e.g. CIB 10 meter has 
// pixels which cover a 10 meter square.  Elevation data has posts which are
// equally spaced in arc seconds, e.g., DTED 3 arc second has elevations 
// samples every 3" in both directions.  In order to be able to scale-in such
// that each progressive MapType that gets displayed shows you a smaller and
// smaller area with increasing detail, MapTypes must be sorted.
class MapScale
{
public:
   // For imagery, elevation posts, or other data types where the data has a
   // resolution rather than a scale, the MapScale will be set with a floating
   // point number in a specified unit.  The get_string member will return a
   // formated string showing the resolution in the specified unit.
   enum unit_t {NM, MILE, KILOMETER, METERS, YARDS, FEET, INCHES, 
      ARC_DEGREES, ARC_MINUTES, ARC_SECONDS};
   // NOTE: if additional unit_t values are added, they must be added to the
   // end of the list, or the serialize/deserialize functions will not work.

//private:
public:
   int m_scale;            // scale denominator 5,000,000 -> 1:5 M
   double m_resolution;     // non-chart MapTypes have a resolution
   unit_t m_resolution_unit;
   
public:
   // Constructor
   MapScale();

   // Constructor for a chart scale, e.g., 1:5 M.  If you use this
   // constructor, get_string() will return a scale-string, e.g.,
   // "1:5 M".  1:scale_denominator is the chart scale.
   MapScale(int scale_denominator);

   // Constructor for imagery, elevation posts, or other data types where the
   // resolution is given rather than the scale.  The units of resolution are 
   // specified by resolution_unit.  If you use constructor, get_string() will
   // will return a resolution string based on the units specified.  For 
   // example, MapScale(10.0, MapScale::METERS) will result in get_string()
   // returning "10 meter".
   MapScale(double resolution, unit_t resolution_unit);

   // Constructor for scale / scale units format
   MapScale(double dScale, MapScaleUnitsEnum eScaleUnits);

   // Copy constructor
   MapScale(const MapScale &scale);

   // Destructor
   ~MapScale() {}

   // Assignment
   const MapScale& operator =(const MapScale &scale);

   // Comparison
   friend BOOL operator ==(const MapScale &s1, const MapScale &s2);
   friend BOOL operator !=(const MapScale &s1, const MapScale &s2);
   friend BOOL operator <(const MapScale &s1, const MapScale &s2);
   friend BOOL operator >(const MapScale &s1, const MapScale &s2);
   friend BOOL operator <=(const MapScale &s1, const MapScale &s2);
   friend BOOL operator >=(const MapScale &s1, const MapScale &s2);

   // Get the scale as an integer. For example, 250,000 for 1:250K
   int get_scale() const
   {
      return m_scale;
   }
   
   // Get the resolution as a float. For example, 1.0 for 1 meter per pixel
   double get_resolution() { return m_resolution; }
   
   // Get the resolution unit as a MapScale::unit_t. For example, METERS
   unit_t get_resolution_unit() { return m_resolution_unit; }

   double GetScale() const
   {
      if (m_scale == WORLD_OVERVIEW)
         return 1.0;
      if (m_resolution != 0.0)
         return m_resolution;

      return m_scale;
   }

   MapScaleUnitsEnum GetScaleUnits() const;
   
   // Get user interface string.  The string will be of the form "1:5 M", 
   // "1:500 K", or "1:500" for a scale set via the scale denominator.  The
   // string will be of the form "10 meter", "1 foot", "3 arc second" for a
   // scale set via a resolution.
   CString get_string() const;

   // Parses a valid user interface string and sets itself from the string.
   // If the string is invalid calling this function will set it equal to
   // NULL_SCALE.
   void MapScale::set_from_string(const char *string);

   // Returns a nominal degrees latitude per pixel value for displaying a maps
   // at this MapScale.  This value, or one close to it, should be used for
   // displaying all map data on a monitor.  If you want to know the relative
   // difference between two different MapScale objects you can use the ratio
   // of their degrees latitude per pixel values.
   degrees_t get_nominal_degrees_lat_per_pixel() const;

   // For chart scales, scales defined via the scale denomator, this function 
   // returns the scale denominator directly.  For scales set via resolution,
   // e.g., 10 meter or 3 arc second, a nominal value will be returned.
   int get_to_scale_denominator() const;

   // An buffer of the necessary size is dynamically created and filled in with
   // this object's data.  size is set to the number of bytes in buffer.  You
   // must call free_buffer() to release this memory.
   int serialize(BYTE **buffer, DWORD *size) const;

   // free a buffer created by serialize().
   int free_buffer(BYTE *buffer) const;

   // Unpack the given buffer and set this object's data from the contents.
   // This function will have no effect on this object if it returns FAILURE.
   int deserialize(BYTE *buffer, DWORD size);

private:
   // computes the nominal degrees per pixel value for a given resolution
   degrees_t resolution_to_degrees_latitude_per_pixel(double resolution, 
      unit_t unit) const;

   // computes a scale value from the resolution
   int resolution_to_scale(double resolution, unit_t unit) const;

   // convert a unit_t to an output string
   const char *unit_to_string(unit_t unit) const;

   // convert an output string to a unit - returns TRUE for success
   BOOL string_to_unit(const char *string, unit_t &unit);

}; // end MapScale

// In the event that a MapSource and MapScale do not uniquely identify a 
// MapType the MapSeries must be used to make the MapType unique.  In addition,
// the MapType can be used to enhance the user interface, even if it is not
// required for uniqueness.  When a series with a non-empty string is available
// for a map type, then it will be shown when scale is shown, e.g., 1:5 M (GNC)
class MapSeries
{
private:
   CString m_string;

public:
   // Constructor
   // Use deserialize() or the assignment operator to set objects created with
   // the default contructor.
   MapSeries();

   // The given string is the user interface name for this series.  It will be
   // returned by get_string().
   MapSeries(const char *string);

   // Copy constructor
   MapSeries(const MapSeries &series);

   // Destructor
   ~MapSeries() {}

   // Assignment
   const MapSeries& operator =(const MapSeries &series);

   // Comparison
   friend BOOL operator ==(const MapSeries &s1, const MapSeries &s2);
   friend BOOL operator !=(const MapSeries &s1, const MapSeries &s2);
   friend BOOL operator ==(const MapSeries &series, const char *series_string);
   friend BOOL operator !=(const MapSeries &series, const char *series_string);

   // Get user interface string.
   CString get_string() const { return m_string; }
}; // end MapSeries

// RE-ARCH 
// Changed CMapType to MapType

// A MapType represents a set of map data from a uniquely identified source, at
// the specified scale, and with the specified series (optional).  In the event
// that MapSource and MapScale do not uniquely identify a MapType, then that 
// MapType must have a non-null MapSeries.  A null MapSeries is one created
// with the default contructor or one with an empty string.
class MapType
{
protected:
   MapSource m_source;
   MapScale m_scale;
   MapSeries m_series;

   CString m_strHandlerName;
   long m_lIdentity;

public:

   void SetHandlerName(CString strHandler) { m_strHandlerName = strHandler; }
   CString GetHandlerName() { return m_strHandlerName; }

   void SetIdentity(long lId) { m_lIdentity = lId; }
   long GetIdentity() { return m_lIdentity; }

   MapType()
   {
   }

   // Constructor - if two or more map types from a given MapSource can have
   // the same MapScale, then you must use this constructor with a valid 
   // MapSeries.
   MapType(const MapSource &source, const MapScale &scale, 
      const MapSeries &series);

   // Constructor for MapTypes that do not need a MapSeries
   MapType(const MapSource &source, const MapScale &scale);

   // Destructor
   virtual ~MapType() {}

   virtual bool is_kind_of(const CString& type)
   {
      return (type == "MapType");
   }

   // Get Source   
   const MapSource &get_source() const { return m_source; }
   
   // Get Scale
   const MapScale &get_scale() const { return m_scale; }

   // Get Series
   const MapSeries &get_series() const { return m_series; }

   // Priority is used for sorting when two map types have the same exact
   // scale.  By default all MapTypes will return 0.  To move a map type
   // up in the list return a value greater than 0.  To move it down, return
   // a value less than 0.
   virtual int get_priority() const { return 0; }

   // Gets the degrees per pixel values for display of this map type on a
   // monitor.  That is, the dpp values that will be used to define a North Up
   // Equal Arc WGS-84 map centered along the given latitude.  This function 
   // uses the values returned by get_degrees_per_pixel_data, if they are
   // within an acceptable tolerance.  Otherwise acceptable defaults are 
   // provided.
   int get_degrees_per_pixel_monitor(degrees_t center_lat,
      degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix);

   // Gets the degrees per pixel values for display of this map type on a
   // printer.  That is, the dpp values that will be used to define a North Up
   // Equal Arc WGS-84 map centered along the given latitude.  This function 
   // uses the values returned by get_degrees_per_pixel_data, if they are
   // within an acceptable tolerance.  Otherwise acceptable defaults are 
   // provided.
   int get_degrees_per_pixel_printer(degrees_t center_lat,
      degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix);

   // Get user interface strings.
   virtual CString get_source_string() const { return m_source.get_string(); }
   virtual CString get_scale_string() const { return m_scale.get_string(); }
   virtual CString get_series_string() const { return m_series.get_string(); }

   // Get the degrees per pixel values of the data.  That is, the dpp values
   // that most closely match your data if it is displayed in a North Up Equal 
   // Arc WGS-84 map centered along the given latitude.
   virtual int get_degrees_per_pixel_data(degrees_t center_lat,
      degrees_t *deg_lat_per_pix, degrees_t *deg_lon_per_pix, BOOL *bLockDegreesPerPixel);

   // Returns true if there is any map data, anywhere in the world, for this
   // map type available on the system, false otherwise.
   virtual bool is_data_available();

   // override this function and return 'true' if the map type implements
   // a dialog box used in Options->Map or Map->Options
   virtual bool implements_properties() { return false; }

   // call the map type's properties dialog - the HWND of the parent
   // window is passed in so the dialog box can be a child of the FalconView
   // window.  This function should return SUCCESS (0) or FAILURE (-1)
   virtual int display_properties(HWND /*parent_hWnd*/) { return SUCCESS; }


   // Performs an object specific function when the object is selected.  Based
   // on context, i.e., the state or mode of the application as well as this
   // overlay, the overlay will determine what action, if any, will take place.
   // This function will return SUCCESS to indicate that this overlay processed 
   // the selection, so no other overlay will process this selection. 
   //
   // The cursor parameter is used to overide the default cursor. Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint. Setting it to NULL indicates you
   // do not want to overide the default.
   virtual int selected(ViewMapProj* /*map*/, CPoint /*point*/, UINT /*flags*/, boolean_t* /*drag*/,
      HCURSOR *cursor, HintText **hint)
   {
      *cursor = NULL;
      *hint = NULL;

      return FAILURE;
   }

   // Sets the cursor and icon (help text and cursor) to indicate what will
   // happen if the selected member is called with the same parameters.  
   // Returns SUCCESS whenever the selected member would have returned SUCCESS,
   // FAILURE otherwise.  Use default implementation to do nothing.
   //
   // The cursor parameter is used to overide the default cursor. Setting it to NULL indicates
   // you do not want to overide the default.
   //
   // The hint parameter is used to overide the default hint. Setting it to NULL indicates you
   // do not want to overide the default.
   virtual int test_selected(ViewMapProj* /*map*/, CPoint /*point*/, UINT /*flags*/, 
      HCURSOR *cursor, HintText **hint)
   {
      *cursor = NULL;
      *hint = NULL;

      return FAILURE;
   }

   // determine if the point hits the object's selection region(s)
   virtual C_icon *hit_test(MapProj* /*map*/, CPoint /*point*/)
   {
      return NULL;
   }     

   // add menu items to the popup menu based on context
   virtual void menu(MapProj* /*map*/, CPoint /*point*/, 
      CList<CFVMenuNode *, CFVMenuNode *> & /*list*/) 
   {
   }

   virtual HintText *get_hint()
   {
      return NULL;
   }

   // Gives the opportunity to the map type to clean up on opened resources.
   // when changing to another map type
   virtual int close(void)
   {
      return SUCCESS;
   }


   // Map type comparison for equality
   friend bool operator ==(const MapType &m1, const MapType &m2);
   friend bool operator !=(const MapType &m1, const MapType &m2);

}; // end MapType

// Ordered lists of MapTypes will be grouped into map categories.  These list
// determines how scale-in and scale-out work.  The MapCategory class provides
// a unique identifier for a map category and it's name.
class MapCategory : public SerializableString
{
public:
   // Constructor
   // Use deserialize() or the assignment operator to set objects created with
   // the default contructor.
   MapCategory();

   // The given string is the user interface name for this source.  It will be
   // returned by get_string().
   MapCategory(const char *string);

   // Copy constructor
   MapCategory(const MapCategory &category);

   // Destructor
   ~MapCategory() {}

   // Assignment
   const MapCategory& operator =(const MapCategory &category);

   long GetGroupId() const;

   // Comparison
   friend BOOL operator ==(const MapCategory &c1, const MapCategory &c2);
   friend BOOL operator !=(const MapCategory &c1, const MapCategory &c2);
}; // end MapCategory

MapCategory GetCategoryFromGroupId(long group_identity);

/*------------------------------------------------------------------
-                            Typedefs 
- -----------------------------------------------------------------*/

//
// map categories. A category is a set of MapTypes
//

class CategoryDefines
{
public:
   static const MapCategory m_blank;
   static const MapCategory m_vector;
   static const MapCategory m_vmap;
   static const MapCategory m_dnc;
   static const MapCategory m_wvs;
   static const MapCategory m_raster;
   static const MapCategory m_dted;
   static const MapCategory m_null;

   CategoryDefines() {}
};

// Be sure to add this to the goto dialog in MapTpSel.cpp
//       MapTypeSelection::set_category_list( ... ) 
// and   MapTypeSelection::get_selected_category( ... )
#define BLANK_CATEGORY CategoryDefines::m_blank
#define VECTOR_CATEGORY CategoryDefines::m_vector
#define VMAP_CATEGORY CategoryDefines::m_vmap
#define DNC_CATEGORY CategoryDefines::m_dnc
#define WVS_CATEGORY CategoryDefines::m_wvs
#define RASTER_CATEGORY CategoryDefines::m_raster
#define DTED_CATEGORY CategoryDefines::m_dted
#define NULL_CATEGORY CategoryDefines::m_null

//
// map sources
//
// Note that when adding new sources, the values for existing sources
// (except for DTED_NIMA and DTED_CMS) need to be preserved because these
// values are saved when FalconView is closed and are then used to initialize
// the FalconView map when it is started again.  That is, the values must be
// backwards-compatible with previous versions of FalconView.
//
typedef MapSource map_source_t;

class SourceDefines
{
public:
   static const MapSource m_blank;
   static const MapSource m_vector;
   static const MapSource m_vmap;
   static const MapSource m_dnc;
   static const MapSource m_tiros;
   static const MapSource m_cadrg;
   static const MapSource m_ecrg;
   static const MapSource m_cib;
   static const MapSource m_dted_cms;
   static const MapSource m_dted_nima;
   static const MapSource m_geotiff;
//   static const MapSource m_mrsid;
   static const MapSource m_null;

   SourceDefines() {}
};

#define BLANK_MAP SourceDefines::m_blank
#define VECTOR SourceDefines::m_vector
#define VMAP SourceDefines::m_vmap
#define DNC SourceDefines::m_dnc
#define TIROS SourceDefines::m_tiros
#define CADRG SourceDefines::m_cadrg
#define ECRG SourceDefines::m_ecrg
#define ADRG CADRG   // TO DO: go back and replace all ADRG with CADRG
#define CIB SourceDefines::m_cib
#define DTED_CMS SourceDefines::m_dted_cms
#define DTED_NIMA SourceDefines::m_dted_nima
#define GEOTIFF SourceDefines::m_geotiff
//#define MRSID SourceDefines::m_mrsid
#define NULL_SOURCE SourceDefines::m_null

//
// map scales
//
typedef MapScale map_scale_t;

class ScaleDefines
{
public:
   static const MapScale m_world;
   static const MapScale m_80M;
   static const MapScale m_40M;
   static const MapScale m_20M;
   static const MapScale m_10M;
   static const MapScale m_5M;
   static const MapScale m_4M;
   static const MapScale m_2M;
   static const MapScale m_1M;
   static const MapScale m_800K;
   static const MapScale m_500K;
   static const MapScale m_dted;
   static const MapScale m_400K;
   static const MapScale m_250K;
   static const MapScale m_200K;
   static const MapScale m_100K;
   static const MapScale m_80K;
   static const MapScale m_50K;
   static const MapScale m_40K;
   static const MapScale m_63360;
   static const MapScale m_30K;
   static const MapScale m_25K;
   static const MapScale m_24K;
   static const MapScale m_20K;
   static const MapScale m_10K;
   static const MapScale m_8K;
   static const MapScale m_5K;
   static const MapScale m_4K;
   static const MapScale m_2K;
   static const MapScale m_1K;
   static const MapScale m_null;

   static const MapScale m_10_meter;
   static const MapScale m_5_meter;
   static const MapScale m_2_meter;
   static const MapScale m_1_meter;

   ScaleDefines() {}
};

#define WORLD ScaleDefines::m_world    // BLANK_MAP, VECTOR and TIROS 
#define ONE_TO_80M ScaleDefines::m_80M   // BLANK_MAP, VECTOR and TIROS 
#define ONE_TO_40M ScaleDefines::m_40M   // BLANK_MAP, VECTOR and TIROS 
#define ONE_TO_20M ScaleDefines::m_20M   // BLANK_MAP, VECTOR and TIROS 
#define ONE_TO_10M ScaleDefines::m_10M   // BLANK_MAP and VECTOR 
#define ONE_TO_5M ScaleDefines::m_5M    // CADRG, BLANK_MAP and VECTOR 
#define ONE_TO_4M ScaleDefines::m_4M    // future sources 
#define ONE_TO_2M ScaleDefines::m_2M    // CADRG, BLANK_MAP and VECTOR 
#define ONE_TO_1M ScaleDefines::m_1M    // CADRG, BLANK_MAP and VECTOR             
#define ONE_TO_800K ScaleDefines::m_800K  // future sources 
//#define ONE_TO_501K ScaleDefines m_  // CADRG LFC
#define ONE_TO_500K ScaleDefines::m_500K  // CADRG and BLANK_MAP 
#define DTED_SCALE ScaleDefines::m_dted   // DTED close to 1:500 K 
#define ONE_TO_400K ScaleDefines::m_400K  // future sources 
//#define ONE_TO_251K ScaleDefines m_  // CADRG TFC
#define ONE_TO_250K ScaleDefines::m_250K  // CADRG, BLANK_MAP, and GEOTIFF
#define ONE_TO_200K ScaleDefines::m_200K  // future sources 
#define ONE_TO_100K ScaleDefines::m_100K  // BLANK_MAP and GEOTIFF 
#define ONE_TO_80K ScaleDefines::m_80K   // future sources 
#define ONE_TO_50K ScaleDefines::m_50K   // CADRG and BLANK_MAP 
#define ONE_TO_40K ScaleDefines::m_40K   // future sources 
#define ONE_TO_63360 ScaleDefines::m_63360   // GEOTIFF
#define ONE_TO_30K ScaleDefines::m_30K       // GEOTIFF
#define ONE_TO_25K ScaleDefines::m_25K       // GEOTIFF
#define ONE_TO_24K ScaleDefines::m_24K       // GEOTIFF
#define ONE_TO_20K ScaleDefines::m_20K   // BLANK_MAP and future sources 
#define ONE_TO_10K ScaleDefines::m_10K   // BLANK_MAP and future sources 
#define ONE_TO_8K ScaleDefines::m_8K    // future sources 
#define ONE_TO_5K ScaleDefines::m_5K    // BLANK_MAP and future sources 
#define ONE_TO_4K ScaleDefines::m_4K    // future sources 
#define ONE_TO_2K ScaleDefines::m_2K    // future sources 
#define ONE_TO_1K ScaleDefines::m_1K    // future sources 
#define NULL_SCALE ScaleDefines::m_null

#define ADRI_SCALE ScaleDefines::m_10_meter             // CIB 10 meter 
#define ADRI_5M_SCALE ScaleDefines::m_5_meter           // CIB 5 meter 
#define ADRI_1M_SCALE ScaleDefines::m_1_meter           // CIB 1 meter 
#define TEN_METER ScaleDefines::m_10_meter              // GEOTIFF
#define FIVE_METER ScaleDefines::m_5_meter              // GEOTIFF
#define TWO_METER ScaleDefines::m_2_meter               // GEOTIFF
#define ONE_METER ScaleDefines::m_1_meter               // GEOTIFF

#define GNC_SERIES "GNC"
#define JNC_SERIES "JNC"
#define ONC_SERIES "ONC"
#define TPC_SERIES "TPC"
#define JOG_SERIES "JOG"
#define TLM_SERIES "TLM"
#define LFC_SERIES "LFC Day"
#define TFC_SERIES "TFC"
#define BLACK_AND_WHITE_SERIES "B&W"
#define COLOR_SERIES "Color"
#define COLOR_INFRARED_SERIES "CIR"
#define NULL_SERIES ""

class ProjectionID : public SerializableString 
{
public:
   // Constructor
   // Use deserialize() or the assignment operator to set objects created with
   // the default contructor.
   ProjectionID();

   // The given string is the user interface name for this source.  It will be
   // returned by get_string().
   ProjectionID(const char *string);

   // Copy constructor
   ProjectionID(const ProjectionID &source);

   ProjectionID(ProjectionEnum eProjectionType);
   ProjectionEnum GetProjectionType();

   // Destructor
   ~ProjectionID() {}

   // Assignment
   const ProjectionID& operator =(const ProjectionID &source);

   // Comparison
   friend BOOL operator ==(const ProjectionID &s1, const ProjectionID &s2);
   friend BOOL operator !=(const ProjectionID &s1, const ProjectionID &s2);

}; // end ProjectionID

struct MapProjectionParams
{
   MapProjectionParams() :
      rotation(0.0),
      zoom_percent(100),
      type(EQUALARC_PROJECTION),
      tilt(-90.0)
   {
      center.lat = center.lon = 0.0;
   }

   d_geo_t center;
   degrees_t rotation;
   int zoom_percent;
   ProjectionEnum type;

   // Upward angle of tilt. This parameter is only applicable in the General
   // Perspective projection
   degrees_t tilt;
};

class MapSpec
{

public:

   d_geo_t center;
   MapSource source;
   MapScale scale;
   MapSeries series;
   double rotation;
   int zoom_percent;
   ProjectionEnum projection_type;

   double m_dBrightness;
   double m_dContrast;
   int m_nContrastMidval;

   // these parameters are used to construct an area chart and 
   // are used in the sub-pages bindings and projection parameters
   bool m_use_mosaic_map_parameters;
   degrees_t m_mosaic_std_parallel_1, m_mosaic_std_parallel_2;
   double m_mosaic_offset_x;
   double m_mosaic_offset_y;
   double m_mosaic_meters_per_pixel_lat;
   double m_mosaic_meters_per_pixel_lon;
   double m_mosaic_center_lat;
   double m_mosaic_center_lon;
   int m_mosaic_surface_width;
   int m_mosaic_surface_height;
   int m_row;
   int m_col;
   int m_mosaic_num_row;
   int m_mosaic_num_col;

   MapSpec();

   degrees_t center_lat() const { return center.lat; }
   degrees_t center_lon() const { return center.lon; }
};

//
// define map_spec_t
//
typedef MapSpec map_spec_t;

// Comparison - MapSource
BOOL operator ==(const MapSource &s1, const MapSource &s2);
BOOL operator !=(const MapSource &s1, const MapSource &s2);

// Comparison - MapScale
BOOL operator ==(const MapScale &s1, const MapScale &s2);
BOOL operator !=(const MapScale &s1, const MapScale &s2);
BOOL operator <(const MapScale &s1, const MapScale &s2);
BOOL operator >(const MapScale &s1, const MapScale &s2);
BOOL operator <=(const MapScale &s1, const MapScale &s2);
BOOL operator >=(const MapScale &s1, const MapScale &s2);

// Comparison - MapSeries
BOOL operator ==(const MapSeries &s1, const MapSeries &s2);
BOOL operator !=(const MapSeries &s1, const MapSeries &s2);
BOOL operator ==(const MapSeries &series, const char *series_string);
BOOL operator !=(const MapSeries &series, const char *series_string);

// Comparison - MapCategory
BOOL operator ==(const MapCategory &c1, const MapCategory &c2);
BOOL operator !=(const MapSeries &s1, const MapSeries &s2);

// Comparison - MapType
bool operator ==(const MapType &m1, const MapType &m2);
bool operator !=(const MapType &m1, const MapType &m2);

#endif
