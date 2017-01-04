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



/* category.cpp */

#include "stdafx.h"
#include <map>

#include "maps.h"
#include "param.h"   // PRM_get_registry_binary and PRM_set_registry_binary
#include "mem.h"  // MEM_malloc, MEM_free
#include "err.h"

#include "Common/ComErrorObject.h"

// The maximum value ever returned in size by the serialize member of MapCategory.
// If you write the contents of buffer to the registry, you will need to pass
// a buffer of this size to the registry reading function, in order to read
// it back in.  The MAP_get_registry_category function depends on this value.
#define MAP_CATEGORY_MAX_BUFFER_SIZE 512

///////////////////////////////////////////////////////////////////////////////
// Static Functions

// Copy the elements of the src list to the dst list.  dst list must be empty.
// The actual pointers in src are copied directly into dst.  No new elements
// are created, so when dst is destroyed it should simply be emptied, but it's
// elements must not be deleted.
static int copy_map_type_list(CList<MapType *, MapType *> &dst, 
                              CList<MapType *, MapType *> &src);

// Copy the elements of the src list with the given MapSource to the dst list.
// dst list must be empty.  The actual pointers in src are copied directly into
// dst.  No new elements are created, so when dst is destroyed it should simply
// be emptied, but it's elements must not be deleted.
static int copy_map_type_list(CList<MapType *, MapType *> &dst, 
   CList<MapType *, MapType *> &src, const MapSource &source);

// Copy the elements of the src list with the given MapSource and MapSeries to 
// the dst list. dst list must be empty.  The actual pointers in src are copied
// directly into dst.  No new elements are created, so when dst is destroyed it 
// should simply be emptied, but it's elements must not be deleted.
static int copy_map_type_list(CList<MapType *, MapType *> &dst, 
   CList<MapType *, MapType *> &src, const MapSource &source, 
   const MapSeries &series);

// Sets up the MapSource to MapCategory mappings for each unique MapSource
// in the given list of MapType.
static void add_source_to_category_mappings(const MapCategory &category, 
                                            CList<MapType *, MapType *> &list);

// Sorts a list of MapTypes.
static int sort_map_types(CList<MapType *, MapType *> &list);

// Used in sorting MapTypes
static int __cdecl MapType_compare(const void *map1, const void *map2);

///////////////////////////////////////////////////////////////////////////////
// Global Variables

// Predefined Map Categories
const MapCategory CategoryDefines::m_blank("Blank");
const MapCategory CategoryDefines::m_vector("Vector");
const MapCategory CategoryDefines::m_vmap("Vector Smart Map");
const MapCategory CategoryDefines::m_dnc("Digital Nautical Chart");
const MapCategory CategoryDefines::m_wvs("World Vector Shoreline");
const MapCategory CategoryDefines::m_raster("Raster");
const MapCategory CategoryDefines::m_dted("Elevation");
const MapCategory CategoryDefines::m_null;

///////////////////////////////////////////////////////////////////////////////
// Static Variables

// List of MapTypes for the four map categories
static CList<MapType *, MapType *> blank_maps;
static CList<MapType *, MapType *> vector_maps;
static CList<MapType *, MapType *> vmap_maps;
static CList<MapType *, MapType *> dnc_maps;
static CList<MapType *, MapType *> wvs_maps;
static CList<MapType *, MapType *> raster_maps;
static CList<MapType *, MapType *> dted_maps;

static CList<MapType *, MapType *> g_all_maps;
static CList<MapCategory, MapCategory> g_category_list;

// A mapping from each MapSource to a MapCategory - all MapTypes with the same
// MapSource must be in the same MapCategory.
static CArray<MapSource,MapSource> lcl_sources;
static CArray<MapCategory,MapCategory> lcl_source_to_category;

static std::map<MapSource, MapCategory> g_map_source_to_category_map;

// forward declarations
static BOOL s_bCategoriesInitialized = FALSE;
int map_create_categories(void);


///////////////////////////////////////////////////////////////////////////////
// MapCategory Class Implementation

// Constructor
// Use deserialize() or the assignment operator to set objects created with
// the default contructor.
MapCategory::MapCategory() : SerializableString()
{
}

// The given string is the user interface name for this source.  It will be
// returned by get_string().
MapCategory::MapCategory(const char *string) :
   SerializableString(string)
{
}

long MapCategory::GetGroupId() const
{
   try
   {
      IMapGroupNamesPtr groups;
      CO_CREATE(groups, CLSID_MapGroupNames);
      groups->SelectByName(_bstr_t(m_string));
      return groups->m_Identity;
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
   return FAILURE;
}

MapCategory GetCategoryFromGroupId(long group_identity)
{
   try
   {
      IMapGroupNamesPtr groups;
      CO_CREATE(groups, CLSID_MapGroupNames);
      groups->SelectByIdentity(group_identity);
      return (char *)groups->m_GroupName;
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
   return "";
}

// Copy constructor.
MapCategory::MapCategory(const MapCategory &category) : SerializableString()
{
   m_string = category.m_string;
}

// Assignment
const MapCategory& MapCategory::operator =(const MapCategory &category)
{
   m_string = category.m_string;

   return *this;
}

// Comparison
BOOL operator ==(const MapCategory &c1, const MapCategory &c2)
{
   return c1.m_string.CompareNoCase(c2.m_string) == 0;
}

BOOL operator !=(const MapCategory &c1, const MapCategory &c2)
{
   return c1.m_string.CompareNoCase(c2.m_string) != 0;
}

// End MapCategory Class Implementation
///////////////////////////////////////////////////////////////////////////////

// Read a MapCategory from the registry.
MapCategory MAP_get_registry_category(const char* section, const char* value_name, 
   const MapCategory &default_category)
{
   DWORD size = MAP_CATEGORY_MAX_BUFFER_SIZE;
   BYTE *buffer;

   buffer = (BYTE *)MEM_malloc(size);
   if (buffer == NULL)
   {
      ERR_report("MEM_malloc() failed.");
      return default_category;
   }

   // if we can read a binary buffer from the specified location
   if (PRM_get_registry_binary(section, value_name, buffer, &size) == SUCCESS)
   {
      MapCategory category;

      // and we can deserialize that buffer into a MapCategory
      if (category.deserialize(buffer, size) == SUCCESS)
      {
         MEM_free(buffer);
         return category;                // return the regitry value
      }
   }

   MEM_free(buffer);

   return default_category;              // return the default value
}

// Write a MapCategory to the registry.
int MAP_set_registry_category(const char* section, const char* value_name, 
   const MapCategory &category)
{
   // serialize the category
   BYTE *buffer;
   DWORD size;
   if (category.serialize(&buffer, &size) != SUCCESS)
   {
      ERR_report("serialize() failed.");
      return FAILURE;
   }

   int status;

   // avoid writing what can not be read - MAP_get_registry_category
   if (size > MAP_CATEGORY_MAX_BUFFER_SIZE)
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
   category.free_buffer(buffer);

   return status;
}

/* get map category */
MapCategory MAP_get_category(MapSource source)
{
   if (!s_bCategoriesInitialized)
      map_create_categories();

   std::map<MapSource, MapCategory>::iterator it = g_map_source_to_category_map.begin();
   while (it != g_map_source_to_category_map.end())
   {
      if (it->first == source)
         return it->second;
      
      it++;
   }
   
   return NULL_CATEGORY;
}

// Copies all of the MapType objects in the input category into map_list.
// map_list must be empty when it is passed into this function.  The MapType
// pointers placed in map_list are the actual MapType objects used inside of
// the maps library.  No new MapTypes are created by this function.  When
// map_list is destroyed, it should simply be emptied.  It's elements MUST
// NOT be deleted.  Note the default behavior for ~CList is to delete the list
// itself but not delete any of the data stored in it's elements, which is the
// behavior expected by this function.
int MAP_get_map_list(MapCategory category, CList<MapType *, MapType *> &map_list,
                     BOOL available_maps_only /*=FALSE*/)
{
   if (!s_bCategoriesInitialized)
      map_create_categories();

   POSITION position = g_all_maps.GetHeadPosition();
   while (position)
   {
      MapType *type = g_all_maps.GetNext(position);
      MapCategory cat = MAP_get_category(type->get_source());
      if (cat == category)
      {
         if (available_maps_only && !type->is_data_available())
            continue;

         map_list.AddTail(type);
      }
   }
   
   return SUCCESS;
}

// Copies all of the MapType objects with the given source into map_list.
// map_list must be empty when it is passed into this function.  The MapType
// pointers placed in map_list are the actual MapType objects used inside of
// the maps library.  No new MapTypes are created by this function.  When
// map_list is destroyed, it should simply be emptied.  It's elements MUST
// NOT be deleted.  Note the default behavior for ~CList is to delete the list
// itself but not delete any of the data stored in it's elements, which is the
// behavior expected by this function.
int MAP_get_map_list(const MapSource &source, 
   CList<MapType *, MapType *> &map_list)
{
   MapCategory category = MAP_get_category(source);

   return MAP_get_map_list(category, map_list);
}

// Copies all of the MapType objects with the given source and series into map_list.
// map_list must be empty when it is passed into this function.  The MapType
// pointers placed in map_list are the actual MapType objects used inside of
// the maps library.  No new MapTypes are created by this function.  When
// map_list is destroyed, it should simply be emptied.  It's elements MUST
// NOT be deleted.  Note the default behavior for ~CList is to delete the list
// itself but not delete any of the data stored in it's elements, which is the
// behavior expected by this function.
int MAP_get_map_list(const MapSource &source, const MapSeries &series,
   CList<MapType *, MapType *> &map_list)
{
   MapCategory category = MAP_get_category(source);

   if (category == BLANK_CATEGORY)
   {
      return copy_map_type_list(map_list, blank_maps, source, series);
   }

   if (category == VECTOR_CATEGORY)
   {
      return copy_map_type_list(map_list, vector_maps, source, series);
   }

   if (category == VMAP_CATEGORY)
   {
      return copy_map_type_list(map_list, vmap_maps, source, series);
   }

   if (category == DNC_CATEGORY)
   {
      return copy_map_type_list(map_list, dnc_maps, source, series);
   }

   if (category == WVS_CATEGORY)
   {
      return copy_map_type_list(map_list, wvs_maps, source, series);
   }
   
   if (category == RASTER_CATEGORY)
   {
      return copy_map_type_list(map_list, raster_maps, source, series);
   }

   if (category == DTED_CATEGORY)
   {
      return copy_map_type_list(map_list, dted_maps, source, series);
   }

   ERR_report("Invalid source.");
   return FAILURE;
}

// Copyies all of the MapType objects available into the map_list from
// every category
int MAP_get_map_list(CList<MapType *, MapType *> &map_list)
{
   if (!s_bCategoriesInitialized)
      map_create_categories();

   map_list.AddTail(&g_all_maps);
   return SUCCESS;
}

// Returns the MapType object that has the given source, scale, and series.
// Returns NULL for failure.
MapType *MAP_get_map_type(const MapSource &source, const MapScale &scale, 
   const MapSeries &series)
{
   if (!s_bCategoriesInitialized)
      map_create_categories();

   POSITION position = g_all_maps.GetHeadPosition();
   while (position)
   {
      MapType *map_type = g_all_maps.GetNext(position);
      if (map_type->get_source() == source &&
          map_type->get_scale() == scale &&
          map_type->get_series() == series)
      {
         return map_type;
      }
   }

   return NULL;
}

// Blank map type is a "vector" based map and can be displayed at any map
// scale.  However, we would like to present FalconView users with a list of
// possible scales.  This method defines that list
void AddBlankMapTypes()
{
   int blank_map_scales[] = { 100000000, 50000000, 20000000,
      10000000, 5000000, 2000000, 1000000, 500000, 250000,
      100000, 50000, 20000, 10000, 5000, 2000, 1000 };

   const int nNumScales = sizeof(blank_map_scales) / sizeof(int);

   for(int i=0;i<nNumScales;++i)
   {
      MapScale scale(blank_map_scales[i]);
      
      MapType *map_type = new MapType("Blank", scale, "");
      map_type->SetHandlerName("Blank");
      map_type->SetIdentity(i + 1);
      
      g_all_maps.AddTail(map_type);
   }
}

// create map types for each category on startup
int map_create_categories(void)
{
   // create a map of ProductName -> Category
   IMapHandlersRowsetPtr map_handlers;
   IMapSeriesRowsetPtr map_series_rowset;
   
   try
   {
      HRESULT hr = map_handlers.CreateInstance(__uuidof(MapHandlersRowset));
      if (FAILED(hr))
      {
         ERR_report("Unable to create MapHandlersRowset.");
         return FAILURE;
      }
      
      hr = map_series_rowset.CreateInstance(__uuidof(MapSeriesRowset));
      if (FAILED(hr))
      {
         ERR_report("Unable to create MapSeriesRowset.");
         return FAILURE;
      }
      
      map_handlers->SelectAll();
      hr = map_handlers->MoveFirst();
      while (hr == S_OK)
      {
         map_series_rowset->Initialize(map_handlers->m_MapHandlerName);
         map_series_rowset->SelectAll();
         
         HRESULT result = map_series_rowset->MoveFirst();
         while (result == S_OK)
         {
            MapCategory category(map_series_rowset->m_Category);         
            MapSource source(map_series_rowset->m_ProductName);
            MapScale scale(map_series_rowset->m_Scale, map_series_rowset->m_ScaleUnits);
            MapSeries series(map_series_rowset->m_SeriesName);
            
            MapType *map_type = new MapType(source, scale, series);
            map_type->SetHandlerName(CString((char *)map_handlers->m_MapHandlerName));
            map_type->SetIdentity(map_series_rowset->m_Identity);
            
            g_all_maps.AddTail(map_type);
            g_map_source_to_category_map[source] = category;
            
            BOOL cat_found = FALSE;
            POSITION pos = g_category_list.GetHeadPosition();
            while (pos)
            {
               if (g_category_list.GetNext(pos) == category)
               {
                  cat_found = TRUE;
                  break;
               }
            }
            if (!cat_found && category != MapCategory(""))
            {
               IMapRenderPtr smpRender;
               HRESULT hr = smpRender.CreateInstance(map_handlers->Getm_MapHandlerGuid());
               if (SUCCEEDED(hr))
                  g_category_list.AddTail(category);
            }

            if (category.get_string().CompareNoCase("Blank") == 0)
               AddBlankMapTypes();
            
            result = map_series_rowset->MoveNext();
         }
         
         hr = map_handlers->MoveNext();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error in creating categories: %s", (char *)e.Description());
      ERR_report(msg);
   }
   
   s_bCategoriesInitialized = TRUE;
   return SUCCESS;
}

// destroy map types for each category on exit
void map_destroy_categories(void)
{
   while (!g_all_maps.IsEmpty())
      delete g_all_maps.RemoveHead();
}

///////////////////////////////////////////////////////////////////////////////
// Static Functions

// Copy the elements of the src list to the dst list.  dst list must be empty.
// The actual pointers in src are copied directly into dst.  No new elements
// are created, so when dst is destroyed it should simply be emptied, but it's
// elements must not be deleted.
static int copy_map_type_list(CList<MapType *, MapType *> &dst, 
                              CList<MapType *, MapType *> &src)
{
   if (dst.GetCount())
   {
      ERR_report("Destination list must be empty.");
      return FAILURE;
   }

   // copy the elements of src to destination
   POSITION next = src.GetHeadPosition();
   while (next)
   {
      dst.AddTail(src.GetNext(next));
   }

   return SUCCESS;
}

// Copy the elements of the src list with the given MapSource to the dst list.
// dst list must be empty.  The actual pointers in src are copied directly into
// dst.  No new elements are created, so when dst is destroyed it should simply
// be emptied, but it's elements must not be deleted.
static int copy_map_type_list(CList<MapType *, MapType *> &dst, 
   CList<MapType *, MapType *> &src, const MapSource &source)
{
   MapType *map_type;

   if (dst.GetCount())
   {
      ERR_report("Destination list must be empty.");
      return FAILURE;
   }

   // copy the elements of src to destination
   POSITION next = src.GetHeadPosition();
   while (next)
   {
      map_type = src.GetNext(next);

      // if this map type has the requested MapSource, add it to the list
      if (map_type->get_source() == source)
         dst.AddTail(map_type);
   }

   return SUCCESS;
}

// Copy the elements of the src list with the given MapSource and MapSeries to 
// the dst list.  dst list must be empty.  The actual pointers in src are copied 
// directly into dst.  No new elements are created, so when dst is destroyed it 
// should simply be emptied, but it's elements must not be deleted.
static int copy_map_type_list(CList<MapType *, MapType *> &dst, 
   CList<MapType *, MapType *> &src, const MapSource &source, 
   const MapSeries &series)
{
   MapType *map_type;

   if (dst.GetCount())
   {
      ERR_report("Destination list must be empty.");
      return FAILURE;
   }

   // copy the elements of src to destination
   POSITION next = src.GetHeadPosition();
   while (next)
   {
      map_type = src.GetNext(next);

      // if this map type has the requested MapSource, add it to the list
      if (map_type->get_source() == source && map_type->get_series() == series)
         dst.AddTail(map_type);
   }

   return SUCCESS;
}

// Sets up the MapSource to MapCategory mappings for each unique MapSource
// in the given list of MapType.
static void add_source_to_category_mappings(const MapCategory &category, 
                                            CList<MapType *, MapType *> &list)
{
   MapType *map_type;
   BOOL found;
   int i;

   MapSource prev_source = NULL_SOURCE;
   POSITION next = list.GetHeadPosition();
   while (next)
   {
      // get the next map type in the list
      map_type = list.GetNext(next);

      // if this source is different from the previous source
      if (map_type->get_source() != prev_source)
      {
         // see if the MapSource for this MapType is already in sources
         found = FALSE;
         for (i=0; i < lcl_sources.GetSize(); i++)
         {
            if (lcl_sources[i] == map_type->get_source())
            {
               found = TRUE;

               // all MapTypes with the same MapSource must be in the same
               // category
               ASSERT(lcl_source_to_category[i] == category);

               break;
            }
         }

         // if the MapSource for this MapType was not found in sources, add it
         // to sources and add it's category to source_to_category
         if (found == FALSE)
         {
            prev_source = map_type->get_source();
            lcl_sources.Add(prev_source);
            lcl_source_to_category.Add(category);
         }
      }
   }
}

static int sort_map_types(CList<MapType *, MapType *> &list)
{
   if (list.GetCount() == 0)
   {
      ERR_report("list is empty.");
      return FAILURE;
   }

   // create an array large enough for all the pointers in list
   MapType **map_types = (MapType **)MEM_malloc(sizeof(MapType *) * list.GetCount());
   if (map_types == NULL)
   {
      ERR_report("MEM_malloc() failed.");
      return FAILURE;
   }

   // copy the MapTypes from list to map_types
   int i=0;
   POSITION next = list.GetHeadPosition();
   while (next)
   {
      map_types[i] = list.GetNext(next);
      i++;
   }
   ASSERT(i == list.GetCount());

   // sort map_types
   qsort(map_types, i, sizeof(MapType *), MapType_compare);
   
   // empty list, so you can fill it from the sorted map_types array
   while (list.IsEmpty() == FALSE)
      list.RemoveHead();

   int count = i;
   for (i=0; i < count; i++)
   {
      // the smallest scale will be at the head of the list
      list.AddTail(map_types[i]);
   }

   MEM_free(map_types);

   return SUCCESS;;
}

// -1    if (map1 < map2)
//  0    if (map1 == map2)
//  1    if (map1 > map2)
static
int __cdecl MapType_compare(const void *map1, const void *map2)
{
   MapType *p1 = *(MapType **)map1;
   MapType *p2 = *(MapType **)map2;

   // sort by scale first
   if (p1->get_scale() < p2->get_scale())
      return -1;
   if (p1->get_scale() > p2->get_scale())
      return 1;

   // if scales match, sort by priority
   if (p1->get_priority() > p2->get_priority())
      return -1;
   if (p1->get_priority() < p2->get_priority())
      return 1;

   // if priorities match, sort by source string
   int compare = p1->get_source().get_string().Compare(p2->get_source().get_string());

   // if source strings match, sort by series string
   if (compare == 0)
      compare = p1->get_series().get_string().Compare(p2->get_series().get_string());

   return compare;
}