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



// RE-ARCH
// SerializableString member functions.
// MapSource and MapCategory are identical in form and function, but in order
// make the compiler catch people mixing up a MapSource with a MapCategory,
// they need to be separate classes.  This base class provides all of the
// functionality.

#include "stdafx.h"
#include "maps_d.h"
#include "param.h"   // PRM_get_registry_binary and PRM_set_registry_binary
#include "mem.h"     // MEM_malloc, MEM_free
#include "err.h"     // ERR_report

// The maximum value ever returned in size by the serialize member of MapSource.
// If you write the contents of buffer to the registry, you will need to pass
// a buffer of this size to the registry reading function, in order to read
// it back in.  The MAP_get_registry_source function depends on this value.
#define MAP_SOURCE_MAX_BUFFER_SIZE 512

// TO DO: put each of these in their respective libraries
MapSource get_blank_map_source(); //(do that here locally)
MapSource VEC_get_map_source();
MapSource VMAP_get_map_source();
MapSource DNC_get_map_source();
MapSource TRS_get_map_source();
MapSource CDG_get_map_source();
MapSource ECRG_get_map_source();
MapSource CIB_get_map_source();
MapSource CMS_get_map_source();
MapSource NDTD_get_map_source();
MapSource TIF_get_map_source();


///////////////////////////////////////////////////////////////////////////////
// Global Variables

const MapSource SourceDefines::m_blank = get_blank_map_source();
const MapSource SourceDefines::m_vector = VEC_get_map_source();
const MapSource SourceDefines::m_vmap = VMAP_get_map_source();
const MapSource SourceDefines::m_dnc = DNC_get_map_source();
const MapSource SourceDefines::m_tiros = TRS_get_map_source();
const MapSource SourceDefines::m_cadrg = CDG_get_map_source();
const MapSource SourceDefines::m_ecrg = ECRG_get_map_source();
const MapSource SourceDefines::m_cib = CIB_get_map_source();
const MapSource SourceDefines::m_dted_cms = CMS_get_map_source();
const MapSource SourceDefines::m_dted_nima = NDTD_get_map_source();
const MapSource SourceDefines::m_geotiff = TIF_get_map_source();
const MapSource SourceDefines::m_null;


// Constructor
// Use deserialize() or the assignment operator to set objects created with
// the default contructor.
SerializableString::SerializableString() : m_string("")
{
   //memset(&m_id, 0, sizeof(UUID));
}

// The given string is the user interface name for this source.  It will be
// returned by get_string().
SerializableString::SerializableString(const char *string) : 
   m_string(string)
{
}

// An buffer of the necessary size is dynamically created and filled in with
// this object's data.  size is set to the number of bytes in buffer.  You
// must call free_buffer() to release this memory.
int SerializableString::serialize(BYTE **buffer, DWORD *size) const
{
   unsigned char *uuid_string;
   UUID uuid = GUID_NULL;

   if (UuidToString(&uuid, &uuid_string) == RPC_S_OUT_OF_MEMORY)
   {
      ERR_report("UuidToString() failed.");
      return FAILURE;
   }

   // buffer will contain an int with the length of m_string, the characters
   // in m_string, the characters in the UUID string, and a '\0'
   int string_length = m_string.GetLength();
   *size = sizeof(int) + string_length + strlen((const char *)uuid_string) + 1;

   *buffer = (BYTE *)MEM_malloc(*size);
   if (*buffer == NULL)
   {
      *size = 0;
      RpcStringFree(&uuid_string);
      ERR_report("MEM_malloc() failed.");
      return FAILURE;
   }

   // string length + string + uuid string + '\0'
   memcpy(*buffer, &string_length, sizeof(int));
   int offset = sizeof(int);
   memcpy(*buffer + offset, m_string, m_string.GetLength());
   offset += m_string.GetLength();
   memcpy(*buffer + offset, uuid_string, strlen((const char *)uuid_string));
   offset += strlen((const char *)uuid_string);
   (*buffer)[offset] = '\0';

   // free the memory allocated by UuidToString
   RpcStringFree(&uuid_string);

   return SUCCESS;
}

// free a buffer created by serialize().
int SerializableString::free_buffer(BYTE *buffer) const
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
int SerializableString::deserialize(BYTE *buffer, DWORD size)
{
   int string_length =  *((int *)buffer);

   if (string_length < 0 || string_length > (int)size)
   {
      ERR_report("Invalid length for m_string.");
      return FAILURE;
   }

   // extract m_string into dynamically allocated array
   char *string = new char[string_length + 1];
   if (string_length > 0)
      memcpy(string, &buffer[sizeof(int)], string_length);
   string[string_length] = '\0';

   // only set member variables upon success
   m_string = string;

   // delete temporary array created above
   delete [] string;

   return SUCCESS;
}

// Constructor
// Use deserialize() or the assignment operator to set objects created with
// the default contructor.
MapSource::MapSource() : SerializableString() 
{
}

// The given string is the user interface name for this source.  It will be
// returned by get_string().
MapSource::MapSource(const char *string) :
   SerializableString(string) 
{
}

// Copy constructor
MapSource::MapSource(const MapSource &source) : SerializableString()
{
   m_string = source.m_string;
}

// Assignment
const MapSource& MapSource::operator =(const MapSource &source)
{
   m_string = source.m_string;

   return *this;
}

// Comparison
BOOL operator ==(const MapSource &s1, const MapSource &s2)
{
   return s1.m_string.CompareNoCase(s2.m_string) == 0;
}

BOOL operator !=(const MapSource &s1, const MapSource &s2)
{
   return s1.m_string.CompareNoCase(s2.m_string) != 0;
}

// Read a MapSource from the registry.
MapSource MAP_get_registry_source(const char* section, const char* value_name, 
   const MapSource &default_source)
{
   DWORD size = MAP_SOURCE_MAX_BUFFER_SIZE;
   BYTE *buffer;

   buffer = (BYTE *)MEM_malloc(size);
   if (buffer == NULL)
   {
      ERR_report("MEM_malloc() failed.");
      return default_source;
   }

   // if we can read a binary buffer from the specified location
   if (PRM_get_registry_binary(section, value_name, buffer, &size) == SUCCESS)
   {
      MapSource source;

      // and we can deserialize that buffer into a MapScale
      if (source.deserialize(buffer, size) == SUCCESS)
      {
         MEM_free(buffer);
         return source;                // return the regitry value
      }
   }

   MEM_free(buffer);

   return default_source;              // return the default value
}

// Write a MapSource to the registry.
int MAP_set_registry_source(const char* section, const char* value_name, 
   const MapSource &source)
{
   // serialize the source
   BYTE *buffer;
   DWORD size;
   if (source.serialize(&buffer, &size) != SUCCESS)
   {
      ERR_report("serialize() failed.");
      return FAILURE;
   }

   int status;

   // avoid writing what can not be read - MAP_get_registry_source
   if (size > MAP_SOURCE_MAX_BUFFER_SIZE)
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
   source.free_buffer(buffer);

   return status;
}


// Read a projection type from the registry.
ProjectionEnum MAP_get_registry_projection(const char* section, const char* value_name, 
   const ProjectionEnum &default_proj_type)
{
   DWORD size = MAP_SOURCE_MAX_BUFFER_SIZE;
   BYTE *buffer;

   buffer = (BYTE *)MEM_malloc(size);
   if (buffer == NULL)
   {
      ERR_report("MEM_malloc() failed.");
      return default_proj_type;
   }

   // if we can read a binary buffer from the specified location
   if (PRM_get_registry_binary(section, value_name, buffer, &size) == SUCCESS)
   {
      ProjectionID proj_type;

      // and we can deserialize that buffer into a projection_t
      if (proj_type.deserialize(buffer, size) == SUCCESS)
      {
         MEM_free(buffer);
         return proj_type.GetProjectionType();                // return the registry value
      }
   }

   MEM_free(buffer);

   return default_proj_type;              // return the default value
}


// Write a projection type to the registry.
int MAP_set_registry_projection(const char* section, const char* value_name, 
   const ProjectionEnum &proj)
{
   ProjectionID proj_type(proj);

   // serialize the source
   BYTE *buffer;
   DWORD size;
   if (proj_type.serialize(&buffer, &size) != SUCCESS)
   {
      ERR_report("serialize() failed.");
      return FAILURE;
   }

   int status;

   // avoid writing what can not be read - MAP_get_registry_source
   if (size > MAP_SOURCE_MAX_BUFFER_SIZE)
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
   proj_type.free_buffer(buffer);

   return status;
}


// TO DO: put each of these in their respective libraries

//(do that here locally)
MapSource get_blank_map_source()
{
   return "Blank";
}

MapSource VEC_get_map_source()
{
   return "Vector";
}

MapSource TRS_get_map_source()
{
   return "TIROS";
}

MapSource CDG_get_map_source()
{
   return "CADRG";
}

MapSource ECRG_get_map_source()
{
   return "ECRG";
}

MapSource CIB_get_map_source()
{
   return "CIB";
}

MapSource CMS_get_map_source()
{
   return "CMS DTED";
}

MapSource NDTD_get_map_source()
{
   return "DTED";
}

MapSource TIF_get_map_source()
{
   return "GEOTIFF";
}

MapSource VMAP_get_map_source()
{
   return "VMAP";
}

MapSource DNC_get_map_source()
{
   return "DNC";
}
