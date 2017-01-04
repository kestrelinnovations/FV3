// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// localpnt.cpp : implementation file
//

#include "stdafx.h"
#include "FalconView/localpnt/localpnt.h"

#include "FalconView/localpnt/DataObject.h"
#include "FalconView/localpnt/lpicon.h"
#include "FalconView/localpnt/PointOverlayFonts.h"
#include "include/points/GroupEnumerator.h"
#include "include/points/GroupFeature.h"
#include "include/points/PointsDataView.h"
#include "include/points/PointFeature.h"
#include "include/points/PointsEnumerator.h"

inline void CopyStdStringToCharArray(const std::string& in_string,
   char* out_char, unsigned int count)
{
   strncpy(out_char, in_string.c_str(), count);
   out_char[min(count,in_string.length())] = '\0';
}

inline char GetFirstCharacter(const std::string& in_string)
{
   if (in_string.length() > 0)
      return in_string[0];
   return ' ';
}

C_localpnt_point* C_localpnt_ovl::ReadPoint(
   points::PointsEnumerator* enumerator)
{
   C_localpnt_point* new_point = new C_localpnt_point(this);

   new_point->m_id = enumerator->GetId().c_str();
   new_point->m_description = enumerator->GetDescription().c_str();
   new_point->m_latitude = enumerator->GetLatitudeDegrees();
   new_point->m_longitude = enumerator->GetLongitudeDegrees();
   new_point->m_altitude_meters = enumerator->GetAltitudeMeters();
   new_point->m_elv = enumerator->GetElevationFeetAboveSeaLevel();
   new_point->m_area_code = GetFirstCharacter(enumerator->GetArea());
   CopyStdStringToCharArray(enumerator->GetCountryCode(),
      new_point->m_country_code, 2);
   CopyStdStringToCharArray(enumerator->GetDtdId(), new_point->m_dtd_id, 8);
   new_point->m_horz_accuracy =
      static_cast<float>(enumerator->GetHorizontalAccuracyFeet());
   new_point->m_vert_accuracy =
      static_cast<float>(enumerator->GetVerticalAccuracyFeet());
   new_point->SetIconName(enumerator->GetIconName().c_str());
   new_point->m_comment = enumerator->GetComment().c_str();
   new_point->m_qlt = GetFirstCharacter(enumerator->GetPointQuality());
   new_point->m_elv_src = enumerator->GetElevationSource().c_str();
   new_point->m_elev_src_map_handler_name =
      enumerator->GetElevationSourceMapHandlerName().c_str();
   new_point->m_elev_src_map_series_id =
      atoi(enumerator->GetElevationSourceMapSeriesName().c_str());
   new_point->m_group_name = enumerator->GetGroupName().c_str();
   new_point->m_icon_image =
      CIconImage::load_images(new_point->GetIconName());
   new_point->m_pedigree = enumerator->GetPedigree();

   return new_point;
}

void C_localpnt_ovl::StorePoint(points::PointsEnumerator* points_enumerator)
{
   C_localpnt_point* new_point = ReadPoint(points_enumerator);
   m_data_object->AddTail(new_point);

   // links
   std::string id = points_enumerator->GetId();
   std::string group_name = points_enumerator->GetGroupName();
   std::vector<std::string> links = points_enumerator->GetLinks();
   std::for_each(links.begin(), links.end(), [&](std::string& link)
   {
      POINT_LINK* new_link = new POINT_LINK;
      new_link->id = id.c_str();
      new_link->group_name = group_name.c_str();
      new_link->link_name = link.c_str();
      m_links_lst.AddTail(new_link);
   });

   // font
   points::Font points_font = points_enumerator->GetFont();
   if (points_font.Valid())
   {
      OvlFont ovlfont;
      ovlfont.SetFont(points_font.GetName().c_str(),
         points_font.GetSize(),
         points_font.GetBackgroundColor().c_str(),
         points_font.GetForegroundColor().c_str(),
         points_font.GetBackgroundEffect(),
         points_font.GetStrikeout(),
         points_font.GetUnderline(),
         points_font.GetBold(),
         points_font.GetItalic());
      FVPOINT point(id.c_str(), group_name.c_str());
      m_pPointOverlayFonts->SetPointFont(ovlfont, point);
   }
}

POINT_GROUP* ReadGroup(points::GroupEnumerator* group_enumerator)
{
   POINT_GROUP* new_group = new POINT_GROUP;
   new_group->name = group_enumerator->GetGroupName().c_str();
   new_group->search = group_enumerator->GetSearch();
   return new_group;
}

void C_localpnt_ovl::StoreGroup(points::GroupEnumerator* group_enumerator)
{
   POINT_GROUP* group = ReadGroup(group_enumerator);
   m_group_lst.AddTail(group);

   points::Font points_font = group_enumerator->GetFont();
   if (points_font.Valid())
   {
      OvlFont ovlfont;
      ovlfont.SetFont(points_font.GetName().c_str(),
         points_font.GetSize(),
         points_font.GetBackgroundColor().c_str(),
         points_font.GetForegroundColor().c_str(),
         points_font.GetBackgroundEffect(),
         points_font.GetStrikeout(),
         points_font.GetUnderline(),
         points_font.GetBold(),
         points_font.GetItalic());
      m_pPointOverlayFonts->SetGroupFont(ovlfont,
         group_enumerator->GetGroupName());
   }
}

int C_localpnt_ovl::open_lps(const CString& filespec)
{
   std::string filespec_str(filespec);

   try
   {
      points::PointsDataView data_view(filespec_str);

      std::unique_ptr<points::PointsEnumerator> points_enumerator;
      points_enumerator.reset(data_view.SelectAllPoints());
      for (bool success = points_enumerator->MoveFirst();
         success == true; success = points_enumerator->MoveNext())
      {
         StorePoint(points_enumerator.get());
      }

      // We must clear the groups, because creating the overlay automatically adds
      // a default group into the list.
      ClearGroups();

      std::unique_ptr<points::GroupEnumerator> group_enumerator;
      group_enumerator.reset(data_view.SelectAllGroups());
      for (bool success = group_enumerator->MoveFirst();
         success == true;
         success = group_enumerator->MoveNext())
      {
         StoreGroup(group_enumerator.get());
      }

      points::Font overlay_points_font = data_view.GetFont();
      if (overlay_points_font.Valid())
      {
         OvlFont overlay_ovlfont;
         overlay_ovlfont.SetFont(overlay_points_font.GetName().c_str(),
            overlay_points_font.GetSize(),
            overlay_points_font.GetBackgroundColor().c_str(),
            overlay_points_font.GetForegroundColor().c_str(),
            overlay_points_font.GetBackgroundEffect(),
            overlay_points_font.GetStrikeout(),
            overlay_points_font.GetUnderline(),
            overlay_points_font.GetBold(),
            overlay_points_font.GetItalic());
         m_pPointOverlayFonts->SetOverlayFont(overlay_ovlfont);
      }
   }
   catch(...)
   {
      return FAILURE;
   }
      
   return SUCCESS;
}

points::Font ConvertToPointsFont(const OvlFont& ovlfont)
{
   CString font_name;
   short font_size;
   CString bgColor;
   CString fgColor;
   short background_type;
   VARIANT_BOOL strikeout;
   VARIANT_BOOL underline;
   VARIANT_BOOL bold;
   VARIANT_BOOL italic;
   ovlfont.GetFont(font_name, font_size, bgColor, fgColor, background_type,
      strikeout, underline, bold, italic);

   points::Font points_font(true, std::string(font_name), font_size,
      std::string(bgColor), std::string(fgColor), background_type,
      strikeout == VARIANT_TRUE, underline == VARIANT_TRUE,
      bold == VARIANT_TRUE, italic == VARIANT_TRUE);
   return points_font;
}

points::GroupFeature C_localpnt_ovl::ConvertToGroupFeature(
   POINT_GROUP* group)
{
   std::string group_name(std::string(group->name));
   points::GroupFeature group_feature(group_name);
   group_feature.SetSearch(group->search == TRUE);
   if (m_pPointOverlayFonts->DoesGroupFontExist(group_name))
   {
      OvlFont group_ovlfont =
         m_pPointOverlayFonts->GetFont("", group_name);
      points::Font group_points_font = ConvertToPointsFont(group_ovlfont);
      group_feature.SetFont(group_points_font);
   }
   return group_feature;
}

points::PointFeature C_localpnt_ovl::ConvertToPointFeature(
   C_localpnt_point* point)
{
   std::string id(point->m_id);
   std::string group_name(point->m_group_name);
   points::PointFeature point_feature(id, group_name,
      point->m_latitude, point->m_longitude);
   point_feature.SetPedigree(point->get_pedigree());

   std::string area;
   area += point->m_area_code;
   point_feature.SetArea(area);
   point_feature.SetComment(std::string(point->m_comment));
   point_feature.SetCountryCode(std::string(point->m_country_code));
   point_feature.SetDescription(std::string(point->m_description));
   point_feature.SetDtdId(std::string(point->m_dtd_id));
   point_feature.SetAltitudeMeters(point->m_altitude_meters);
   point_feature.SetElevationFeetAboveSeaLevel(point->m_elv);
   point_feature.SetElevationSource(std::string(point->m_elv_src));
   point_feature.SetElevationSourceMapHandlerName(std::string(point->m_elev_src_map_handler_name));
   std::stringstream elevation_source_map_series_name;
   elevation_source_map_series_name << point->m_elev_src_map_series_id;
   point_feature.SetElevationSourceMapSeriesName(elevation_source_map_series_name.str());
   point_feature.SetHorizontalAccuracyFeet(static_cast<int>(point->m_horz_accuracy));
   std::string icon_name(point->GetIconName());
   point_feature.SetIconName(icon_name);
   std::string point_quality;
   point_quality += point->m_qlt;
   point_feature.SetPointQuality(point_quality);
   point_feature.SetVerticalAccuracyFeet(static_cast<int>(point->m_vert_accuracy));

   if (m_pPointOverlayFonts->DoesPointFontExist(id, group_name))
   {
      OvlFont ovlfont = m_pPointOverlayFonts->GetFont(id, group_name);
      points::Font points_font = ConvertToPointsFont(ovlfont);
      point_feature.SetFont(points_font);
   }

   std::vector<std::string> links;
   POSITION position = m_links_lst.GetHeadPosition();
   while (position)
   {
      POINT_LINK *point_link = m_links_lst.GetNext(position);
      if (point_link->id == point->m_id &&
         point_link->group_name == point->m_group_name)
      {
         links.push_back(std::string(point_link->link_name));
      }
   }
   if (links.size() > 0 )
   {
      point_feature.SetLinks(links);
   }

   return point_feature;
}

// saves the points overlay into a spatialite database with the given name
int C_localpnt_ovl::save_as_lps(const CString & filespec)
{
   std::string filespec_str(filespec);
   points::PointsDataView data_view(filespec_str);

   OvlFont ovlfont = m_pPointOverlayFonts->GetFont("","");
   points::Font overlay_points_font = ConvertToPointsFont(ovlfont);
   data_view.SetFont(overlay_points_font);

   // add all the groups
   if (!m_group_lst.IsEmpty())
   {
      POSITION position = m_group_lst.GetHeadPosition();
      while (position != nullptr)
      {
         POINT_GROUP* group = m_group_lst.GetNext(position);
         points::GroupFeature group_feature = ConvertToGroupFeature(group);
         data_view.Add(group_feature);
      }
   }

   // all all the points
   if (!m_data_object->IsEmpty())
   {
      POSITION position = m_data_object->GetHeadPosition();
      while (position != nullptr)
      {
         C_localpnt_point* point = m_data_object->GetNext(position);
         points::PointFeature point_feature = ConvertToPointFeature(point);
         data_view.Add(point_feature);
         point->set_pedigree(point_feature.GetPedigree());
      }
   }

   return SUCCESS;
}
