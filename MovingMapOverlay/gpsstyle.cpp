// Copyright (c) 1994-2011,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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



// gpsstyle.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "colordlg.h"
#include "graphics.h"
#include "param.h"
#include "..\multisel.h"
#include "..\mainfrm.h"
#include "..\FontDlg.h"
#include "gpsstyle.h"
#include "PredictivePathOptionsPage.h"
#include "BullseyeOptionsPage.h"
#include "gps.h"
#include "optndlg.h"
#include "factory.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

#define SERIALIZE_VALUE(b, v) \
      memcpy(b, &v, sizeof(v)); \
      b += sizeof(v);

#define DESERIALIZE_VALUE(b, v) \
      memcpy(&v, b, sizeof(v)); \
      b += sizeof(v);

// CBullseyeProperties implementation
//
CBullseyeProperties::CBullseyeProperties()
{
   m_bullseye_on = FALSE;
   m_range_radius = 9260.0; // meters
   m_range_units = NM;
   m_num_range_rings = 1;
   m_num_radials = 2;
   m_angle_between_radials = 360.0;
   m_offset_enabled = FALSE;
   m_relative_azimuth = 0.0;
   m_clip_tamask = TRUE;
}

// initialize the values of this properties object from the registry
int CBullseyeProperties::initialize_from_registry(CString key)
{
   // range ring on/off
   m_bullseye_on = PRM_get_registry_int(key, "RangeRingOn", FALSE);

   // range ring radius (in meters)
   m_range_radius = PRM_get_registry_double(key, "RangeRadius", 9260.0);

   // range ring units
   m_range_units = (units_t)PRM_get_registry_int(key, "RangeUnits", NM);

   m_num_range_rings = PRM_get_registry_int(key, "NumRangeRings", 1);
   m_num_radials = PRM_get_registry_int(key, "NumRadials", 2);
   m_angle_between_radials = PRM_get_registry_int(key, "AngleBetweenRadials", 360);
   m_offset_enabled = PRM_get_registry_int(key, "OffsetEnabled", FALSE);
   m_relative_azimuth = PRM_get_registry_int(key, "RelativeAzimuth", 0);
   m_clip_tamask = PRM_get_registry_int(key, "ClipTAMask", TRUE);

   return SUCCESS;
}

// store the values of this properties object to the registry
int CBullseyeProperties::store_in_registry(CString key)
{
   // bullseye on/off
   PRM_set_registry_int(key, "RangeRingOn", m_bullseye_on);

   // range ring radius (in meters)
   PRM_set_registry_double(key, "RangeRadius", m_range_radius);

   // range ring units
   PRM_set_registry_int(key, "RangeUnits", m_range_units);

   PRM_set_registry_int(key, "NumRangeRings", m_num_range_rings);
   PRM_set_registry_int(key, "NumRadials", m_num_radials);
   PRM_set_registry_int(key, "AngleBetweenRadials", static_cast<int>(m_angle_between_radials));
   PRM_set_registry_int(key, "OffsetEnabled", m_offset_enabled);
   PRM_set_registry_int(key, "RelativeAzimuth", static_cast<int>(m_relative_azimuth));
   PRM_set_registry_int(key, "ClipTAMask", m_clip_tamask);

   return SUCCESS;
}

// Returns the size, in bytes, necessary to store the bullseye properties
int CBullseyeProperties::get_block_size()
{
   return sizeof(m_bullseye_on) +
      sizeof(m_range_radius) +
      sizeof(m_range_units) +
      sizeof(m_num_range_rings) +
      sizeof(m_num_radials) +
      sizeof(m_angle_between_radials) +
      sizeof(m_offset_enabled) +
      sizeof(m_relative_azimuth) +
      sizeof(m_clip_tamask);
}

/// <summary>
/// Fills in an array of bytes used to store the bullseye properties.  The given
/// pointer is incremented by get_block_size() bytes
/// </summary>
void CBullseyeProperties::Serialize(BYTE *&block_ptr)
{
   SERIALIZE_VALUE(block_ptr, m_bullseye_on);
   SERIALIZE_VALUE(block_ptr, m_range_radius);
   SERIALIZE_VALUE(block_ptr, m_range_units);
   SERIALIZE_VALUE(block_ptr, m_num_range_rings);
   SERIALIZE_VALUE(block_ptr, m_num_radials);
   SERIALIZE_VALUE(block_ptr, m_angle_between_radials);
   SERIALIZE_VALUE(block_ptr, m_offset_enabled);
   SERIALIZE_VALUE(block_ptr, m_relative_azimuth);
   SERIALIZE_VALUE(block_ptr, m_clip_tamask);
}

/// <summary>
/// Fills in bullseye properties with a given array of bytes
/// </summary>
void CBullseyeProperties::Deserialize(BYTE *block_ptr)
{
   DESERIALIZE_VALUE(block_ptr, m_bullseye_on);
   DESERIALIZE_VALUE(block_ptr, m_range_radius);
   DESERIALIZE_VALUE(block_ptr, m_range_units);
   DESERIALIZE_VALUE(block_ptr, m_num_range_rings);
   DESERIALIZE_VALUE(block_ptr, m_num_radials);
   DESERIALIZE_VALUE(block_ptr, m_angle_between_radials);
   DESERIALIZE_VALUE(block_ptr, m_offset_enabled);
   DESERIALIZE_VALUE(block_ptr, m_relative_azimuth);
   DESERIALIZE_VALUE(block_ptr, m_clip_tamask);
}

// CPredictivePathProperties implementation
//

CPredictivePathProperties::CPredictivePathProperties()
{
   m_predictive_path_on = FALSE;
   m_predictive_path_icon_color = UTIL_COLOR_BLUE;
   m_predictive_path_center_line_on = TRUE;
   m_num_trend_dots = 3;
   m_trend_dot_time[0] = 10;
   m_trend_dot_time[1] = 20;
   m_trend_dot_time[2] = 30;
   m_trend_dot_time[3] = 60;
}

// initialize the values of this properties object from the registry
int CPredictivePathProperties::initialize_from_registry(CString key)
{
   m_predictive_path_on = PRM_get_registry_int(key, "PredPathOn", 0);
   m_predictive_path_icon_color = PRM_get_registry_int(key, "PredPathIconColor", UTIL_COLOR_BLUE);
   m_predictive_path_center_line_on = PRM_get_registry_int(key, "PredPathCenterLineOn", 1);
   m_num_trend_dots = PRM_get_registry_int(key, "NumTrendDots", 3);
   m_trend_dot_time[0] = PRM_get_registry_int(key, "TrendDotTime1", 10);
   m_trend_dot_time[1] = PRM_get_registry_int(key, "TrendDotTime2", 20);
   m_trend_dot_time[2] = PRM_get_registry_int(key, "TrendDotTime3", 30);
   m_trend_dot_time[3] = PRM_get_registry_int(key, "TrendDotTime4", 60);

   m_bstrNormalPath3DIconFile = PRM_get_registry_string( key, "PredictedPathNormal3DIconFile", "shape\\green dot2.ico" );
   m_bstrWarningPath3DIconFile = PRM_get_registry_string( key, "PredictedPathWarning3DIconFile", "shape\\yelo_x.ico" );
   m_bstrCriticalPath3DIconFile = PRM_get_registry_string( key, "PredictedPathCritical3DIconFile", "shape\\red_x.ico" );

   return SUCCESS;
}

// store the values of this properties object to the registry
int CPredictivePathProperties::store_in_registry(CString key)
{
   PRM_set_registry_int(key, "PredPathOn", m_predictive_path_on);
   PRM_set_registry_int(key, "PredPathIconColor", m_predictive_path_icon_color);
   PRM_set_registry_int(key, "PredPathCenterLineOn", m_predictive_path_center_line_on);
   PRM_set_registry_int(key, "NumTrendDots", m_num_trend_dots);
   PRM_set_registry_int(key, "TrendDotTime1", m_trend_dot_time[0]);
   PRM_set_registry_int(key, "TrendDotTime2", m_trend_dot_time[1]);
   PRM_set_registry_int(key, "TrendDotTime3", m_trend_dot_time[2]);
   PRM_set_registry_int(key, "TrendDotTime4", m_trend_dot_time[3]);
   PRM_set_registry_string( key, "PredictedPathNormal3DIconFile", m_bstrNormalPath3DIconFile );
   PRM_set_registry_string( key, "PredictedPathWarning3DIconFile", m_bstrWarningPath3DIconFile );
   PRM_set_registry_string( key, "PredictedPathCritical3DIconFile", m_bstrCriticalPath3DIconFile );
   return SUCCESS;
}

/// <summary>
/// Returns the size, in bytes, necessary to store the predictive path properties
/// </summary>
int CPredictivePathProperties::get_block_size()
{
   int test = sizeof(m_trend_dot_time);

   return sizeof(m_predictive_path_on) +
      sizeof(m_predictive_path_icon_color) +
      sizeof(m_predictive_path_center_line_on) +
      sizeof(m_num_trend_dots) +
      sizeof(m_trend_dot_time);

   return 0;
}

/// <summary>
/// Fills in an array of bytes used to store the predictive path properties
/// </summary>
void CPredictivePathProperties::Serialize(BYTE *&block_ptr)
{
   SERIALIZE_VALUE(block_ptr, m_predictive_path_on);
   SERIALIZE_VALUE(block_ptr, m_predictive_path_icon_color);
   SERIALIZE_VALUE(block_ptr, m_predictive_path_center_line_on);
   SERIALIZE_VALUE(block_ptr, m_num_trend_dots);
   for (int i=0; i<4; ++i)
   {
      const int nTrendDotTime = m_trend_dot_time[i];
      SERIALIZE_VALUE(block_ptr, nTrendDotTime);
   }
}

/// <summary>
/// Fills in predictive path properties with a given array of bytes
/// </summary>
void CPredictivePathProperties::Deserialize(BYTE *block_ptr)
{
   DESERIALIZE_VALUE(block_ptr, m_predictive_path_on);
   DESERIALIZE_VALUE(block_ptr, m_predictive_path_icon_color);
   DESERIALIZE_VALUE(block_ptr, m_predictive_path_center_line_on);
   DESERIALIZE_VALUE(block_ptr, m_num_trend_dots);
   for (int i=0; i<4; ++i)
   {
      int nTrendDotTime = 0;
      DESERIALIZE_VALUE(block_ptr, nTrendDotTime);
      m_trend_dot_time[i] = nTrendDotTime;
   }
}
// CAuxDataProperties implementation
//

// CAuxDataProperties implementation
//

CAuxDataProperties::CAuxDataProperties()
{  
}

CAuxDataProperties::~CAuxDataProperties()
{
}

void CAuxDataProperties::set_num_values(int numTypes)
{
   if((int)m_values.size() != numTypes)
   {
      m_values.clear();
      m_values.resize(numTypes);
   }
}

// initialize the values of this properties object from the registry
int CAuxDataProperties::initialize_from_registry(CString key)
{
   int numValues = PRM_get_registry_int(key, "NumValues", 0);
   if(numValues > 0)
   {     
      m_values.clear();
      m_values.resize(numValues);
   }
   for(int i = 0; i < numValues; i++)
   {
      m_values[i] = PRM_get_registry_int(key, "Value" + i, 0);

   }
   return SUCCESS;
}

// store the values of this properties object to the registry
int CAuxDataProperties::store_in_registry(CString key)
{
    PRM_set_registry_int(key, "NumValues", m_values.size());
   for(int i = 0; i < (int)m_values.size(); i++)
   {
      PRM_set_registry_int(key, "Value" + 1, m_values[i]);
   }
   return SUCCESS;
}

/// <summary>
/// Returns the size, in bytes, necessary to store the predictive path properties
/// </summary>
int CAuxDataProperties::get_block_size()
{
   return sizeof(boolean_t) * (int)m_values.size();
}

/// <summary>
/// Fills in an array of bytes used to store the predictive path properties
/// </summary>
void CAuxDataProperties::Serialize(BYTE *&block_ptr)
{
   int size = (int)m_values.size();
   SERIALIZE_VALUE(block_ptr, size);
   for(int i = 0; i < (int)m_values.size(); i++)
   {
     SERIALIZE_VALUE(block_ptr, m_values[i]);
   }
}

/// <summary>
/// Fills in predictive path properties with a given array of bytes
/// </summary>
void CAuxDataProperties::Deserialize(BYTE *block_ptr)
{
   int numValues = 0;
   DESERIALIZE_VALUE(block_ptr, numValues);
   if(numValues > 0)
   {     
      m_values.resize(numValues);      
   }  
   for(int i = 0; i < numValues; i++)
   {
     DESERIALIZE_VALUE(block_ptr, m_values[i]);
   }   
}

// CGPSProperties implementation
//
int CGPSProperties::m_nAutoColor = UTIL_COLOR_DARK_GRAY;

CGPSProperties::CGPSProperties()
{
   // set defaults 
   m_symbol_type = "Bomber";
   m_ship_fg_color = UTIL_COLOR_BLUE;
   m_ship_bg_color = UTIL_COLOR_BLACK;
   m_ship_size = 16;
   m_ship_line_size = 4;
   m_trail_icon_type = GPS_ICON_CIRCLE;
   m_trail_icon_fg_color = UTIL_COLOR_RED;
   m_ctrack_update_rate = 15;

   m_feed_type = 0;
   m_frozen_feed_timeout = 10;

   m_auto_save_trail = TRUE;

   m_display_altitude_label = FALSE;

   m_nickname_font.set_font("Arial",12,0);
   m_nickname_font.set_foreground(UTIL_COLOR_WHITE);
   m_nickname_font.set_background(UTIL_BG_SHADOW, UTIL_COLOR_BLACK);

   m_bAutoColorIncrement = FALSE;
   m_bDisplayNickname = TRUE;
   m_bOriginAtNose = FALSE;
}

int CGPSProperties::GetNextAutoColor()
{
   switch (m_nAutoColor)
   {
      case (UTIL_COLOR_GREEN) : m_nAutoColor = UTIL_COLOR_RED; break;
   case (UTIL_COLOR_RED) : m_nAutoColor = UTIL_COLOR_YELLOW; break;
   case (UTIL_COLOR_YELLOW) : m_nAutoColor = UTIL_COLOR_BLUE; break;
   case (UTIL_COLOR_BLUE) : m_nAutoColor = UTIL_COLOR_MAGENTA; break;
   case (UTIL_COLOR_MAGENTA) : m_nAutoColor = UTIL_COLOR_CYAN; break;
   case (UTIL_COLOR_CYAN) : m_nAutoColor = UTIL_COLOR_WHITE; break;
   case (UTIL_COLOR_WHITE) : m_nAutoColor = UTIL_COLOR_DARK_RED; break;
   case UTIL_COLOR_DARK_RED: m_nAutoColor = UTIL_COLOR_DARK_GREEN; break;
   case UTIL_COLOR_DARK_GREEN: m_nAutoColor = UTIL_COLOR_DARK_YELLOW; break;
   case UTIL_COLOR_DARK_YELLOW: m_nAutoColor = UTIL_COLOR_DARK_BLUE; break;
   case UTIL_COLOR_DARK_BLUE: m_nAutoColor = UTIL_COLOR_DARK_MAGENTA; break;
   case UTIL_COLOR_DARK_MAGENTA: m_nAutoColor = UTIL_COLOR_DARK_CYAN; break;
   case UTIL_COLOR_DARK_CYAN : m_nAutoColor = UTIL_COLOR_LIGHT_GRAY; break;
   case UTIL_COLOR_LIGHT_GRAY: m_nAutoColor = UTIL_COLOR_SKY_BLUE; break;
   case UTIL_COLOR_SKY_BLUE: m_nAutoColor = UTIL_COLOR_CREAM; break;
   case UTIL_COLOR_CREAM: m_nAutoColor = UTIL_COLOR_MEDIUM_GRAY; break;
   case UTIL_COLOR_MEDIUM_GRAY: m_nAutoColor = UTIL_COLOR_DARK_GRAY; break;
   case UTIL_COLOR_DARK_GRAY: m_nAutoColor = UTIL_COLOR_GREEN; break;
   }

   return m_nAutoColor;
}

int CGPSProperties::initialize_from_registry(CString key)
{
   // symbol type
   m_symbol_type = PRM_get_registry_string(key, "SymbolType", "Bomber");

   // symbol foreground color
   m_ship_fg_color = PRM_get_registry_int(key, "SymbolFgColor", UTIL_COLOR_BLUE);

   // symbol background color
   m_ship_bg_color = PRM_get_registry_int(key, "SymbolBgColor", UTIL_COLOR_BLACK);

   // ship size
   m_ship_size = PRM_get_registry_int(key, "ShipSize", 16);

   // ship line size
   m_ship_line_size = PRM_get_registry_int(key, "ShipLineSize", 4);

   m_bullseyeProp.initialize_from_registry(key + "\\Bullseye");

   // trail icon type
   m_trail_icon_type = (gps_icon_t)PRM_get_registry_int(key, "TrailIconType", 
      GPS_ICON_CIRCLE);

   // trail icon fg color
   m_trail_icon_fg_color = PRM_get_registry_int(key, "TrailIconFgColor", UTIL_COLOR_RED);

   // trail timeout
   m_timeout_on = PRM_get_registry_int(key, "TimeoutOn", 0);
   m_timeout_seconds = PRM_get_registry_int(key, "TimeoutSeconds", 0);

   // coast track update rate
   m_ctrack_update_rate = PRM_get_registry_int(key, "CTrackUpdateRate", 15);

   m_predictivePathProp.initialize_from_registry(key + "\\PredictivePath");

   this->m_auxDataProp.initialize_from_registry(key + "\\AuxData");

   m_feed_type = PRM_get_registry_int(key, "DefaultFeedType", 0);

   m_frozen_feed_timeout = PRM_get_registry_int(key,"FrozenTimeout",10);

   m_auto_save_trail = PRM_get_registry_int(key,"AutoSaveTrail", 1);

   m_display_altitude_label = PRM_get_registry_int(key, "ShowAltitudeLabel", FALSE);

   m_nickname_label = PRM_get_registry_string(key, "Nickname", "");
   m_nickname_font.initialize_from_registry(key + "\\Nickname Font",
      "Arial",12,0,UTIL_COLOR_WHITE, UTIL_BG_SHADOW, UTIL_COLOR_BLACK);

   m_bAutoColorIncrement = PRM_get_registry_int(key, "AutoColorIncrement", FALSE);

   m_bDisplayNickname = PRM_get_registry_int(key, "DisplayNickname", TRUE);
   m_bOriginAtNose = PRM_get_registry_int(key, "OriginAtNose", FALSE);

   m_trail_rewind_enabled = PRM_get_registry_int( key, "TrailRewindEnabled", FALSE );

   CString cs = PRM_get_registry_string( "Main", "ReadOnlyAppData", "" );
   ASSERT( !cs.IsEmpty() && "Missing registry key" );
   cs += _T("\\Icons\\");
   m_bstr3DIconFilePath = cs;

   m_bstrTrail3DIconFile =
      PRM_get_registry_string( key, "Trail3DIconFile", "shape\\red dot2.ico" );

   return SUCCESS;
}

int CGPSProperties::store_in_registry(CString key)
{
   // symbol type
   PRM_set_registry_string(key, "SymbolType", m_symbol_type);

   // symbol foreground color
   PRM_set_registry_int(key, "SymbolFgColor", m_ship_fg_color);

   // symbol background color
   PRM_set_registry_int(key, "SymbolBgColor", m_ship_bg_color);

   // ship size
   PRM_set_registry_int(key, "ShipSize", m_ship_size);

   // ship line size
   PRM_set_registry_int(key, "ShipLineSize", m_ship_line_size);

   m_bullseyeProp.store_in_registry(key + "\\Bullseye");

   // trail icon type
   PRM_set_registry_int(key, "TrailIconType", m_trail_icon_type);

   // trail icon fg color
   PRM_set_registry_int(key, "TrailIconFgColor", m_trail_icon_fg_color);

   // trail timeout
   PRM_set_registry_int(key, "TimeoutOn", m_timeout_on);
   PRM_set_registry_int(key, "TimeoutSeconds", m_timeout_seconds);

   // coast track update rate
   PRM_set_registry_int(key, "CTrackUpdateRate", m_ctrack_update_rate);

   m_predictivePathProp.store_in_registry(key + "\\PredictivePath");

   m_auxDataProp.store_in_registry(key + "\\AuxData");

   PRM_set_registry_int(key, "DefaultFeedType", m_feed_type);

   PRM_set_registry_int(key,"FrozenTimeout",m_frozen_feed_timeout);

   PRM_set_registry_int(key,"AutoSaveTrail", m_auto_save_trail);

   PRM_set_registry_int(key, "ShowAltitudeLabel", m_display_altitude_label);

   PRM_set_registry_string(key, "Nickname", m_nickname_label);
   m_nickname_font.save_in_registry(key + "\\Nickname Font");

   PRM_set_registry_int(key, "AutoColorIncrement", m_bAutoColorIncrement);

   PRM_set_registry_int(key, "DisplayNickname", m_bDisplayNickname);

   PRM_set_registry_int(key, "OriginAtNose", m_bOriginAtNose);

   PRM_set_registry_string( key, "Trail3DIconFile", m_bstrTrail3DIconFile );

   return SUCCESS;
}

// return the number of bytes required to store the properties
int CGPSProperties::get_block_size()
{
   int size = 0;

   // symbol type
   size += sizeof(int) + m_symbol_type.GetLength();

   // symbol foreground color
   size += sizeof(int);

   // symbol background color
   size += sizeof(int);

   // ship size
   size += sizeof(int);

   // ship line size
   size += sizeof(int);

   // range on/off
   size += sizeof(boolean_t);

   // range radius
   size += sizeof(int);

   // range units
   size += sizeof(units_t);

   // trail icon type
   size += sizeof(gps_icon_t);

   // trail icon fg color
   size += sizeof(int);

   // trail timeout
   size += sizeof(boolean_t) + sizeof(int);

   // coast track update rate
   size += sizeof(int);

   // altitude label
   size += sizeof(boolean_t);

   // nickname
   size += sizeof(int) + m_nickname_label.GetLength();

   // nickname font
   size += m_nickname_font.get_block_size();

   // nickname displayed, m_bDisplayNickname
   size += sizeof(BOOL);

   // origin at nose
   size += sizeof(BOOL);

   return size;
}

int CGPSProperties::Serialize(BYTE *&block)
{
   // symbol type
   int size = m_symbol_type.GetLength();
   memcpy(block, &size, sizeof(int));
   block += sizeof(int);
   if (size > 0)
   {
      memcpy(block, m_symbol_type.GetBuffer(size), size);
      block += size;
   }
   
   // symbol foreground color
   memcpy(block, &m_ship_fg_color, sizeof(int));
   block += sizeof(int);
   
   // symbol background color
   memcpy(block, &m_ship_bg_color, sizeof(int));
   block += sizeof(int);
   
   // ship size
   memcpy(block, &m_ship_size, sizeof(int));
   block += sizeof(int);
   
   // ship line size
   memcpy(block, &m_ship_line_size, sizeof(int));
   block += sizeof(int);

   // range ring on/off - this will be moved into CBullseyeProperties::Serialize
   boolean_t unused = FALSE;
   memcpy(block, &unused, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // range radius - this will be moved into CBullseyeProperties::Serialize
   int nUnused = 0;
   memcpy(block, &nUnused, sizeof(int));
   block += sizeof(int);

   // range units - this will be moved into CBullseyeProperties::Serialize
   units_t eUnused = KM;
   memcpy(block, &eUnused, sizeof(units_t));
   block += sizeof(units_t);
   
   // trail icon type
   memcpy(block, &m_trail_icon_type, sizeof(gps_icon_t));
   block += sizeof(gps_icon_t);
   
   // trail icon fg color
   memcpy(block, &m_trail_icon_fg_color, sizeof(int));
   block += sizeof(int);

   // trail timeout
   memcpy(block, &m_timeout_on, sizeof(boolean_t));
   block += sizeof(boolean_t);
   memcpy(block, &m_timeout_seconds, sizeof(int));
   block += sizeof(int);

   // coast track update rate
   memcpy(block, &m_ctrack_update_rate, sizeof(int));
   block += sizeof(int);

   // altitude label on/off
   memcpy(block, &m_display_altitude_label, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // nickname
   size = m_nickname_label.GetLength();
   memcpy(block, &size, sizeof(int));
   block += sizeof(int);
   if (size > 0)
   {
      memcpy(block, m_nickname_label.GetBuffer(size), size);
      block += size;
   }

   // nickname font
   m_nickname_font.Serialize(block);

   // nickname displayed
   memcpy(block, &m_bDisplayNickname, sizeof(BOOL));
   block += sizeof(BOOL);

   // origin at nose
   memcpy(block, &m_bOriginAtNose, sizeof(BOOL));
   block += sizeof(BOOL);

   return SUCCESS;
}

int CGPSProperties::deserialize(BYTE *block)
{
   // symbol type
   int size;
   memcpy(&size, block, sizeof(int));
   block += sizeof(int);
   if (size > 0)
   {
      char tmp_buffer[80];
      memcpy(tmp_buffer,block,size);
      block += size;
      tmp_buffer[size] = '\0';
      
      m_symbol_type = CString(tmp_buffer);
   }
   
   // symbol foreground color
   memcpy(&m_ship_fg_color, block, sizeof(int));
   block += sizeof(int);
   
   // symbol background color
   memcpy(&m_ship_bg_color, block, sizeof(int));
   block += sizeof(int);
   
   // ship size
   memcpy(&m_ship_size, block, sizeof(int));
   block += sizeof(int);
   
   // ship line size
   memcpy(&m_ship_line_size, block, sizeof(int));
   block += sizeof(int);

   // range ring on/off - this will be moved into CBullseyeProperties::Serialize
   boolean_t bUnused;
   memcpy(&bUnused, block, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // range radius - this will be moved into CBullseyeProperties::Serialize
   int nUnused;
   memcpy(&nUnused, block, sizeof(int));
   block += sizeof(int);

   // range units - this will be moved into CBullseyeProperties::Serialize
   units_t eUnused;
   memcpy(&eUnused, block, sizeof(units_t));
   block += sizeof(units_t);
   
   // trail icon type
   memcpy(&m_trail_icon_type, block, sizeof(gps_icon_t));
   block += sizeof(gps_icon_t);
   
   // trail icon fg color
   memcpy(&m_trail_icon_fg_color, block, sizeof(int));
   block += sizeof(int);

   // trail timeout
   memcpy(&m_timeout_on, block, sizeof(boolean_t));
   block += sizeof(boolean_t);
   memcpy(&m_timeout_seconds, block, sizeof(int));
   block += sizeof(int);

   // coast track update rate
   memcpy(&m_ctrack_update_rate, block, sizeof(int));
   block += sizeof(int);

   // altitude label on/off
   memcpy(&m_display_altitude_label, block, sizeof(boolean_t));
   block += sizeof(boolean_t);

   // nickname
   memcpy(&size, block, sizeof(int));
   block += sizeof(int);
   if (size > 0)
   {
      char tmp_buffer[80];
      memcpy(tmp_buffer,block,size);
      block += size;
      tmp_buffer[size] = '\0';
      
      m_nickname_label = CString(tmp_buffer);
   }

   // nickname font
   m_nickname_font.Deserialize(block);

   memcpy(&m_bDisplayNickname, block, sizeof(BOOL));
   block += sizeof(BOOL);

   // origin at nose
   memcpy(&m_bOriginAtNose, block, sizeof(BOOL));
   block += sizeof(BOOL);
   
   return SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// CGpsStyle dialog

IMPLEMENT_DYNCREATE(CGpsStyle, COverlayPropertyPage)

CGpsStyle::CGpsStyle(CGPSProperties* pProperties /* = NULL */, C_gps_trail *trail /* = NULL */)
   : COverlayPropertyPage(FVWID_Overlay_MovingMapTrail, CGpsStyle::IDD),
     m_prop(pProperties),
     m_p_trail(trail)
{
   //{{AFX_DATA_INIT(CGpsStyle)
   m_lineSize = 3;
   m_shipSize = 16;
   m_timeout_seconds = 0;
   m_timeout_on = FALSE;
   //}}AFX_DATA_INIT
}

CGpsStyle::~CGpsStyle()
{
}

void CGpsStyle::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CGpsStyle)
   DDX_Control(pDX, IDC_AUTO_SAVE, m_auto_save);
   DDX_Control(pDX, IDC_FROZEN_FEED, m_frozen_feed);
   DDX_Control(pDX, IDC_ALTITUDE, m_altitude_on);
   DDX_Control(pDX, IDC_TIMEOUT_ON, m_timeout_on_ctrl);
   DDX_Control(pDX, IDC_TIMEOUT_SECONDS, m_timeout_edit_ctrl);
   DDX_Control(pDX, IDC_SLIDER_SHIP_SIZE, m_sliderShipC);
   DDX_Control(pDX, IDC_SLIDER_LINE_SIZE, m_sliderLineC);
   DDX_Control(pDX, IDC_GPS_STYLE_COLOR_BACKGROUND, m_backgroundC);
   DDX_Control(pDX, IDC_GPS_STYLE_COLOR_FOREGROUND, m_foregroundC);
   DDX_Control(pDX, IDC_ICON_COLOR, m_icon_fgC);
   DDX_Control(pDX, IDC_GPS_STYLE_SHIP_TYPE, m_shipTypeC);
   DDX_Control(pDX, IDC_GPSFEED, m_gpsfeed);
   DDX_Control(pDX, IDC_ICON_TYPE, m_icon_type);
   DDX_Text(pDX, IDC_TIMEOUT_SECONDS, m_timeout_seconds);
   DDV_MinMaxInt(pDX, m_timeout_seconds, 0, 32768);
   DDX_Check(pDX, IDC_TIMEOUT_ON, m_timeout_on);
   DDX_Text(pDX, IDC_NICKNAME_EDIT, m_nickname);
   DDX_Control(pDX, IDC_AUTO_INCREMENT, m_ckAutoIncrement);
   DDX_Control(pDX, IDC_DISPLAY_NICK, m_ckDisplayNick);
   DDX_Control(pDX, IDC_ORIGIN, m_ckOriginAtNose);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGpsStyle, CDialog)
   //{{AFX_MSG_MAP(CGpsStyle)
   ON_BN_CLICKED(IDC_GPS_STYLE_COLOR_BACKGROUND, OnGpsStyleColorBackground)
   ON_BN_CLICKED(IDC_GPS_STYLE_COLOR_FOREGROUND, OnGpsStyleColorForeground)
   ON_BN_CLICKED(IDC_ICON_COLOR, OnIconFgColor)
   ON_WM_HSCROLL()
   ON_CBN_SELCHANGE(IDC_GPS_STYLE_SHIP_TYPE, OnModified)
   ON_CBN_SELCHANGE(IDC_GPSFEED, OnSelchangeGPSFeed)
   ON_BN_CLICKED(ID_APPLY, OnApplyClicked)
   ON_CBN_SELCHANGE(IDC_ICON_TYPE, OnModified)
   ON_BN_CLICKED(IDC_TIMEOUT_ON, OnModified)
   ON_EN_KILLFOCUS(IDC_TIMEOUT_SECONDS, OnModified)
   ON_CBN_SELCHANGE(IDC_FROZEN_FEED, OnModified)
   ON_BN_CLICKED(IDC_AUTO_SAVE, OnModified)
   ON_BN_CLICKED(IDC_ALTITUDE, OnModified)
   ON_EN_KILLFOCUS(IDC_NICKNAME_EDIT, OnModified)
   ON_BN_CLICKED(IDC_FONT, OnFont)
   ON_BN_CLICKED(IDC_AUTO_INCREMENT, OnModified)
   ON_BN_CLICKED(IDC_DISPLAY_NICK, OnModified)
   ON_BN_CLICKED(IDC_ORIGIN, OnModified)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpsStyle message handlers
BOOL CGpsStyle::OnInitDialog() 
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   CPropertyPage::OnInitDialog();

   m_modified = FALSE;
   
   m_sliderLineC.SetRange(1,8);
   m_sliderShipC.SetRange(1,64);
   m_sliderShipC.SetTicFreq(4);
   m_sliderLineC.SetPageSize(1);
   m_sliderShipC.SetPageSize(1);

   // populate the ship type combo box with the symbols read in at startup
   const int nNumSymbols = SymbolContainer::GetInstance()->get_num_symbols();
   int i;
   for(i=0;i<nNumSymbols;i++)
      m_shipTypeC.AddString(SymbolContainer::GetInstance()->get_symbol(i)->get_name());

   // select the ship type string based on the value store in the properties.
   // if the symbol type was not found then select the first entry in the combo
   if (m_shipTypeC.SelectString(0, m_prop->get_symbol_type()) == CB_ERR)
      m_shipTypeC.SetCurSel(0); 

   m_shipSize = m_prop->get_ship_size();
   m_lineSize = m_prop->get_ship_line_size();

   m_sliderLineC.SetPos(m_lineSize);
   m_sliderShipC.SetPos(m_shipSize);

   m_foreground = futil->code2color(m_prop->get_ship_fg_color());
   m_background = futil->code2color(m_prop->get_ship_bg_color());
   m_icon_fg = futil->code2color(m_prop->get_trail_icon_fg_color());
   m_foregroundC.SetColor(m_foreground);
   m_backgroundC.SetColor(m_background);

   m_icon_fgC.SetColor(m_icon_fg);
   
   switch(m_prop->get_trail_icon_type())
   {
   case GPS_ICON_CIRCLE: m_icon_type.SetCurSel(0); break;
   case GPS_ICON_DIAMOND: m_icon_type.SetCurSel(1); break;
   case GPS_ICON_ARROW: m_icon_type.SetCurSel(2); break;
   }

   // initialize the GPS Feeds combo box
   CString gps_FeedName;
   CString key = "GPS Feeds";
   i = 0;
   do
   {
      CString num_str;
      num_str.Format("\\%d", i++);

      gps_FeedName = PRM_get_registry_string(key + num_str, "FeedName", "DNE");
      if (gps_FeedName != "DNE")
         m_gpsfeed.AddString(gps_FeedName);

   } while (gps_FeedName != "DNE");

   // if we didn't add anything in the combo then we need to add
   // the standard NMEA 0183 (this won't happen if the setup program
   // was run)
   if (i == 1 && gps_FeedName == "DNE")
      m_gpsfeed.AddString("NMEA 0183");

   m_gpsfeed.SetCurSel(m_prop->get_feed_type());

   if (m_prop->get_frozen_feed_timeout() == -1)
      m_frozen_feed.SelectString(0, "NEVER");
   else
   {
      CString str;
      str.Format("%d s", m_prop->get_frozen_feed_timeout());
      m_frozen_feed.SelectString(0, str);
   }

   // initialize the timeout controls
   m_timeout_on = m_prop->get_timeout_on();
   m_timeout_seconds = m_prop->get_timeout_seconds();

   // initialize the display altitude on checkbox
   m_altitude_on.SetCheck(m_prop->get_display_altitude_label());

   m_nickname = m_prop->get_nickname_label();

   m_auto_save.SetCheck(m_prop->get_auto_save_trail());

   // Show the auto increment check box if we are dealing with the overlay options
   m_ckAutoIncrement.ShowWindow(m_p_trail == NULL);
   m_ckAutoIncrement.SetCheck(m_prop->GetAutoColorIncrement());

   m_ckDisplayNick.SetCheck(m_prop->GetDisplayNickname());

   m_ckOriginAtNose.SetCheck(m_prop->GetOriginAtNose());

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CGpsStyle::OnApplyClicked()
{
   OnApply();
}

BOOL CGpsStyle::OnApply() 
{
   UpdateData(TRUE);

   if (!m_modified)
      return TRUE;

   store_values();

   // if the property page applies to a specific moving map trail then apply the properties now
   if (m_p_trail != NULL)
   {
      m_p_trail->set_properties(m_prop);
      m_p_trail->set_modified(TRUE);

      // redraw the gps trail so the changes will take place
      OVL_get_overlay_manager()->InvalidateOverlay(m_p_trail);
   }
      
   m_modified = FALSE;

   // if the view time dialog is opened, then the gant chart needs updating
   // to reflect the change in color (if any)
   CMainFrame::GetPlaybackDialog().update_gant_chart();

   return TRUE;
}

void CGpsStyle::store_values()
{
   CFvwUtil *futil = CFvwUtil::get_instance();

   m_lineSize = m_sliderLineC.GetPos();
   m_shipSize = m_sliderShipC.GetPos();

   m_prop->set_ship_fg_color(futil->color2code(m_foreground));
   m_prop->set_ship_bg_color(futil->color2code(m_background));
   m_prop->set_ship_line_size(m_lineSize);
   m_prop->set_ship_size(m_shipSize);

   m_prop->set_trail_icon_fg_color(futil->color2code(m_icon_fg));

   switch(m_icon_type.GetCurSel())
   {
   case 0: m_prop->set_trail_icon_type(GPS_ICON_CIRCLE); break;
   case 1: m_prop->set_trail_icon_type(GPS_ICON_DIAMOND); break;
   case 2: m_prop->set_trail_icon_type(GPS_ICON_ARROW); break;
   }

   // store ship type
   CString ship_type;
   m_shipTypeC.GetLBText(m_shipTypeC.GetCurSel(), ship_type);
   m_prop->set_symbol_type(ship_type);

   // store timeout data
   m_prop->set_timeout_on(m_timeout_on);
   m_prop->set_timeout_seconds(m_timeout_seconds);

   // store label properties
   m_prop->set_display_altitude_label(m_altitude_on.GetCheck());

   m_prop->set_nickname_label(m_nickname);

   m_prop->set_auto_save_trail(m_auto_save.GetCheck());

   // change the default GPS feed to the selected feed
   if (m_p_trail == NULL || !m_p_trail->is_connected())
      m_prop->set_feed_type(m_gpsfeed.GetCurSel());
   else
      m_gpsfeed.SetCurSel(m_prop->get_feed_type());

   int sel = m_frozen_feed.GetCurSel();
   if (sel != CB_ERR)
   {
      CString txt;
      m_frozen_feed.GetLBText(sel, txt);

      if (txt == "NEVER")
      {
         m_prop->set_frozen_feed_timeout(-1);
      }
      else
      {
         TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
         char* context;
         m_prop->set_frozen_feed_timeout(atoi(strtok_s((char *)(LPCTSTR)txt, " ", &context)));
      }
   }

   m_prop->SetAutoColorIncrement(m_ckAutoIncrement.GetCheck());
   m_prop->SetDisplayNickname(m_ckDisplayNick.GetCheck());
   m_prop->SetOriginAtNose(m_ckOriginAtNose.GetCheck());
}

void CGpsStyle::OnOK() 
{
   OnApply();

   CDialog::OnOK();
}

void GetColor( COLORREF &color, CString title)
{
   //get the FalconView color index
   int _color = GRA_get_color_constant_best_fit(color);

   //dialog object
   CDrawColorDlg cd;

   //use the title given
   cd.set_title(title);

   //use the current color
   cd.set_color(_color);

   //nothing but colors
   cd.m_no_radio_buttons = TRUE;

   //show the dialog
   if (cd.DoModal() == IDOK)
   {
      //get the chosen index
      _color = cd.get_color();

      //get a true color
      color = GRA_get_color_ref(_color);
   }
}

void CGpsStyle::OnGpsStyleColorBackground() 
{
   GetColor(m_background,"Outline");
   m_backgroundC.SetColor(m_background);
   SetModified(TRUE);
   m_modified = TRUE;
}

void CGpsStyle::OnGpsStyleColorForeground() 
{
   GetColor(m_foreground,"Foreground");
   m_foregroundC.SetColor(m_foreground);
   SetModified(TRUE);
   m_modified = TRUE;
}

void CGpsStyle::OnIconFgColor()
{
   GetColor(m_icon_fg, "Trail Point Color");
   m_icon_fgC.SetColor(m_icon_fg);
   SetModified(TRUE);
   m_modified = TRUE;
}

void CGpsStyle::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
   switch (nSBCode)
   {
   default:  //do nothing
      break;
   case   SB_LEFT:      //   Scroll to far left.
   case   SB_LINELEFT:  //   Scroll left.
   case   SB_PAGELEFT:  //   Scroll one page left.
      break;

   case   SB_RIGHT:     //   Scroll to far right.
   case   SB_LINERIGHT: //   Scroll right.
   case   SB_PAGERIGHT: //   Scroll one page right.
      break;

   case   SB_ENDSCROLL://   End scroll.
      break;

   case   SB_THUMBPOSITION://   Scroll to absolute position. The current position is specified by the nPos parameter.
   case   SB_THUMBTRACK:   //   Drag sc
      break;
   }

   SetModified(TRUE);
   m_modified = TRUE;
}


void CGpsStyle::OnSelchangeGPSFeed()
{
   SetModified(TRUE);
   m_modified = TRUE;
}

void CGpsStyle::OnModified()
{
   UpdateData();
   SetModified();
   m_modified = TRUE;
}

void CGpsStyle::OnFont()
{
   CFontDlg dlg;
   dlg.SetApplyCallback(this);
   dlg.set_font(m_prop->get_nickname_font());
   
   if (dlg.DoModal() == IDOK) 
   {
      m_prop->set_nickname_font(dlg.GetOvlFont());
      OnFontModified();
   }
}

void CGpsStyle::OnApplyFont(CFontDlg *dlg)
{
   m_prop->set_nickname_font(dlg->GetOvlFont());
   OnFontModified();
}


void CGpsStyle::OnFontModified()
{
   // m_p_trail is NULL implies we are dealing with the overlay options dialog box
   if (m_p_trail == NULL)
   {
      SetModified();
      m_modified = TRUE;

      CFvOverlayPropertySheet *pSheet = dynamic_cast<CFvOverlayPropertySheet *>(GetParent());
      if (pSheet != NULL)
         pSheet->OnApply();
   }
   else
   {
      m_p_trail->set_modified(TRUE);
      OVL_get_overlay_manager()->InvalidateOverlay(m_p_trail);
   }
}