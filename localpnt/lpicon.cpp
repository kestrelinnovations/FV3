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
#include "FalconView/localpnt/localpnt.h"

#include <cctype>

#include "FalconView/localpnt/lpicon.h"
#include "FalconView/include/fvwutil.h"
#include "FalconView/localpnt/minlpset.h"
#include "FalconView/include/map.h"
#include "FalconView/include/param.h"
#include "CoT\LPointsMessage.h"  // codec for CoT messages
#include "PointOverlayFonts.h"
#include "FalconView/localpnt/newptdef.h"
#include "FalconView/include/ovl_mgr.h"

// --------------------------------------------------------------------------
////////////////////////////////////////////////
// C_localpnt_point functions
////////////////////////////////////////////////
// --------------------------------------------------------------------------
C_localpnt_point::C_localpnt_point() : C_icon()
{
   initialize();
}

// --------------------------------------------------------------------------

C_localpnt_point::C_localpnt_point(C_overlay *overlay) :
      C_icon(overlay)
{
   initialize();
}
// --------------------------------------------------------------------------

int C_localpnt_point::set_point_data(CMinLocalPointSet &set, MapProj* map)
{
   m_id = set.m_ID;
   m_group_name = set.m_Group_Name;
   m_latitude = (float) set.m_Latitude;
   m_longitude = (float) set.m_Longitude;
   m_description = set.m_Description;

   std::string icon_name = set.m_Icon_Name;
   // trim trailing whitespace
   icon_name.erase(std::find_if(icon_name.rbegin(), icon_name.rend(),
      std::not1(std::ptr_fun<int, int>(std::isspace))).base(), icon_name.end());
   // if icon name is empty, use default
   if (icon_name.length() == 0)
      icon_name = "Local Point";
   icon_name = "localpnt\\" + icon_name + ".ico";

   m_icon_name = icon_name.c_str();

   m_icon_image = CIconImage::load_images(m_icon_name);

   if (map != NULL)
      refresh_screen_coordinates(map);

   return SUCCESS;
}
// --------------------------------------------------------------------------

void C_localpnt_point::refresh_screen_coordinates(MapProj* map)
{
   map->geo_to_surface(m_latitude, m_longitude, &m_x, &m_y);
   m_rect.SetRect(m_x-16, m_y-16, m_x+16, m_y+16);
}


int C_localpnt_point::initialize()
{
   m_id = "";
   m_group_name = "";

   //fields displayed by FalconView
   m_latitude=(float) 0.0;
   m_longitude= (float) 0.0;
   m_altitude_meters = 0.0;
   m_description=" ";//space (because zero lenth fields are not allowed)
   m_icon_name="red_dot";

   //other (do not correspond to fields in the database)
   //m_rect=0;
   //m_icon_image=NULL;

   strcpy(m_dtd_id, "");
   m_horz_accuracy = -1.0;
   m_vert_accuracy = -1.0;
   m_elv = 0;
   m_elv_src = "UNK";
   m_elev_src_map_series_id = 0;
   strcpy(m_country_code, "");
   m_area_code = ' ';
   m_qlt = '0';

   m_dtEventStart = COleDateTime::GetCurrentTime();

   m_handle = -1;
   m_pedigree = "-1";

   return SUCCESS;
}
// --------------------------------------------------------------------------

// draws the current point as a 'selected' or 'hilighted' point.  The icon can
// be drawn as 'halftone'
void C_localpnt_point::draw(MapProj* map, CDC* dc, boolean_t selected, boolean_t hilighted,
                            boolean_t draw_labels, CPointOverlayFonts* pPointOverlayFonts,
                            boolean_t halftone)
{
   ASSERT(map != NULL);
   ASSERT(dc != NULL);
   ASSERT(pPointOverlayFonts != NULL);

   boolean_t is_printing = dc->IsPrinting();

   if (map->point_in_surface(m_x, m_y))
   {
      int icon_adjusted_size = 32;

      if (!is_printing && halftone)
         m_icon_image->set_drawing_mode(CIconImage::ICON_HALFTONE);

      // display the point's icon (32x32 centered)
      // we can display one of four ways:
      // plain
      // selected (with handles in editor)
      // hilighted (with background)
      // selected and hilighted (in editor)
      do
      {
         if ( !is_printing )
         {
            if ( hilighted )
            {
               m_icon_image->draw_hilighted( dc, m_x, m_y, icon_adjusted_size );
               break;
            }
            if ( selected )
            {
               m_icon_image->draw_selected( dc, m_x, m_y, icon_adjusted_size );
               break;
            }
         }
         m_icon_image->draw_icon( dc, m_x, m_y, 32, icon_adjusted_size );
      } while ( FALSE );

      if (!is_printing && halftone)
         m_icon_image->set_drawing_mode(CIconImage::ICON_NORMAL);

      // set record icon's bounding box
      int iHalfSize = icon_adjusted_size / 2;
      m_rect.SetRect( m_x - iHalfSize, m_y - iHalfSize, m_x + iHalfSize, m_y + iHalfSize );


      //-------------------- begin draw labels -----------------
      CFvwUtil *fvwutl = CFvwUtil::get_instance();
      CPoint cpt[4];

      if (draw_labels)
      {
         CString label = m_id;
         if (!label.IsEmpty()) //should never happen
         {
            OvlFont& font = pPointOverlayFonts->GetFont(std::string(m_id), std::string(m_group_name));

            int text_x_offset = 18;
            int text_y_offset = 16;
            CString strFontName;
            int nFontSize;
            int nFontAttrib;
            COLORREF fgColor, bgColor;
            int bgType;

            font.get_font(strFontName, nFontSize, nFontAttrib);
            font.get_foreground_RGB(fgColor);
            font.get_background_RGB(bgType, bgColor);

            if (dc->IsPrinting())
            {
               double icon_adjust_percentage =
                  (double)PRM_get_registry_int("Printing",
                  "IconAdjustSizePercentage", 0);
               text_x_offset = (int)(18.0 + 18.0*icon_adjust_percentage/100.0);
               text_y_offset = (int)(16.0 + 16.0*icon_adjust_percentage/100.0);

               double text_adjust_percentage =
                  (double)PRM_get_registry_int("Printing",
                  "FontAdjustSizePercentage", 0);
               nFontSize = (int)(nFontSize + nFontSize*text_adjust_percentage/100.0);
            }


            fvwutl->draw_multi_line_text_RGB(dc, label,
               m_x + text_x_offset, m_y + text_y_offset,
               UTIL_ANCHOR_LOWER_LEFT, strFontName, nFontSize, nFontAttrib,
               bgType, fgColor, bgColor, 0, cpt);

            // convert and save the rectangle of the text block
            // (will be used in case object is moved)
            m_text_rect = CRect(cpt[0], cpt[2]);
            m_text_rect.InflateRect(1, 1);
         }
         else
         {
            ASSERT(0);
            m_text_rect.SetRectEmpty();
         }
      }
      else
         m_text_rect.SetRectEmpty();

      //-------------------- end draw labels -----------------
   }
   else
      // point is not on the screen
      m_rect.SetRectEmpty();
}

// --------------------------------------------------------------------------
BOOL C_localpnt_point::operator==(C_localpnt_point &point2) const
{
   return (
      m_id              == point2.m_id              &&
      m_group_name      == point2.m_group_name      &&
      m_latitude        == point2.m_latitude        &&
      m_longitude       == point2.m_longitude       &&
      m_altitude_meters == point2.m_altitude_meters &&
      m_description     == point2.m_description     &&
      m_icon_name       == point2.m_icon_name       );

   //not compared:
   //- m_icon_image
   //- m_rect

}

// --------------------------------------------------------------------------
const C_localpnt_point& C_localpnt_point::operator=(C_localpnt_point &src)
{
   m_id              = src.m_id;
   m_group_name      = src.m_group_name;
   m_latitude        = src.m_latitude;
   m_longitude       = src.m_longitude;
   m_description     = src.m_description;
   m_icon_name       = src.m_icon_name;

   m_icon_image      = src.m_icon_image;
   m_rect            = src.m_rect;


   //members of C_Icon (from which C_localpnt_point is derived)
   //TO DO: call the base class = operator instead (which still needs to be written)
   m_overlay         = src.m_overlay;

   return *this;
}

// --------------------------------------------------------------------------
// updates member varibles that are dependent on the center
// call this when the center has changed
// 1) clears the values we don't know
// 2) tries to set those we can
int C_localpnt_point::update_members_dependent_on_center()
{
   //TO DO: fix this to use database
   /*

   //TO DO: do we want to calulate any of these?
   m_horz_accuracy=-1;
   m_vert_accuracy=-1;

   m_elev_src="UNK";
   m_country=" ";
   m_pt_quality="0";
   m_area=" ";

   // if DTED is available for the point, fill in the elevation
   int elevation;

   if (DTD_get_elevation_in_feet(m_latitude, m_longitude, &elevation) == SUCCESS)
   {
      m_elev=elevation;
      m_elev_src = "DTED";
   }
   else
      m_elev=0;
   */

   return SUCCESS;
}
// --------------------------------------------------------------------------

boolean_t C_localpnt_point::hit_test(CPoint point)
{
   return m_rect.PtInRect(point);
}

// --------------------------------------------------------------------------

boolean_t C_localpnt_point::is_kind_of(const char *class_name)
{
   ASSERT (class_name);

   if (!strcmp(class_name, "C_localpnt_point"))
      return TRUE;
   else
      return C_icon::is_kind_of(class_name);
}

// --------------------------------------------------------------------------

CString C_localpnt_point::get_help_text()
{
    CString text;
    CString temp("");

    if (m_overlay == NULL)
        return "";

    // TO DO: trim these somewhere else?
    m_id.TrimRight();

    text.Format("%s, Point Group: %s, From: %s, H Accuracy: ",
        m_id, m_group_name, OVL_get_overlay_manager()->GetOverlayDisplayName(m_overlay));

    if (m_horz_accuracy !=-1)
    {
        temp.Format("%.1f ft,", m_horz_accuracy);
        text += temp;
    }
    else
        text += "unk,";

    text += " V Accuracy: ";
    if (m_vert_accuracy !=-1)
    {
        temp.Format("%.1f ft", m_vert_accuracy);
        text += temp;
    }
    else
        text += "unk";



    return text;
}
// end of get_help_text

// --------------------------------------------------------------------------

CString C_localpnt_point::get_tool_tip()
{
    CString text;

    // TO DO: trim these somewhere else?
    m_id.TrimRight();
    m_description.TrimRight();

    CNewLocalPointDefaults newLocalPointDefaults;
    if (m_group_name != newLocalPointDefaults.GetGroupName())
        text.Format("%s,  %s", m_id, m_group_name);
    else
        text=m_id;

    if( !m_description.IsEmpty())
    {
        text += "\n";

        text += m_description;
    }

    return text;
}
// end of get_tooltip


// --------------------------------------------------------------------------

//TO DO: document this
//TO DO: look at CHUM
void C_localpnt_point::draw_selected(CWnd* pWnd, boolean_t immediate)
{
   if (immediate)
   {
      CClientDC dc(pWnd);

      m_icon_image->draw_selected(&dc, m_x, m_y, 32);
   }
   else
      OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}


// --------------------------------------------------------------------------

//TO DO: document this - why is this the same as "unselect"
void C_localpnt_point::invalidate(boolean_t recalc_text)
{
   OVL_get_overlay_manager()->InvalidateOverlay(m_overlay);
}

// --------------------------------------------------------------------------

//TO DO: document this - why is this the same as "unselect"
void C_localpnt_point::draw_hilighted()
{
   invalidate();
}


// --------------------------------------------------------------------------

void C_localpnt_point::unselect()
{
   invalidate();
}

// --------------------------------------------------------------------------

//NOTE: this only indicates if the center is in view
boolean_t C_localpnt_point::in_view(MapProj* map)
{
   return map->point_in_surface(m_x, m_y);
}


// --------------------------------------------------------------------------


CString C_localpnt_point::get_key() const
{
   return "\""+ m_id +"\",\""+ m_group_name +"\"";
}
// --------------------------------------------------------------------------


int C_localpnt_point::get_RS_POINT_point_source() const
{
   const int POINT_EXPORT_SRC_LOCAL = 5;
   return POINT_EXPORT_SRC_LOCAL;
}

_bstr_t C_localpnt_point::ToXml(bool bDeletePoint)
{
   CLocalPointsMessage msg;

   CString strFriendlyName;
   strFriendlyName.Format("%s\\%s", m_group_name, m_id);
   CString strDtdId(m_dtd_id);
   if (strDtdId.GetLength() > 0)
      strFriendlyName += "[" + strDtdId + "]";

   msg.m_bstrFriendlyName = _bstr_t(strFriendlyName);

   msg.m_hFalconviewOverlay = OVL_get_overlay_manager()->get_overlay_handle(m_overlay);
   msg.m_bstrRemarksSource = _bstr_t(static_cast<C_localpnt_ovl *>(m_overlay)->GetOverlayGuid().c_str());

   msg.m_bstrEventUID = _bstr_t(m_strObjectGuid.c_str());

   msg.m_bstrEventType = "t-x-f";

   msg.m_Point.m_fPointLat = m_latitude;
   msg.m_Point.m_fPointLon = m_longitude;
   msg.m_Point.m_fPointCE = 1.0F;
   msg.m_Point.m_fPointLE = 0.0F;
   msg.m_Point.m_fPointHAE = this->m_elv;

   msg.m_dtEventStart = m_dtEventStart;
   msg.m_dtEventTime = COleDateTime::GetCurrentTime();

   if (bDeletePoint)
      msg.m_dtEventStale = 0;
   else
      msg.m_dtEventStale = (DATE)(COleDateTime(msg.m_dtEventTime) + COleDateTimeSpan(30, 0, 0, 0));

   msg.m_pFVIcon = new FVIcon;
   msg.m_pFVIcon->m_bIsRelativePath = true;
   msg.m_pFVIcon->m_bstrIconPath = m_icon_name;

   /* IMPLEMENT LINKS
   msg.m_pFVLinks = new FVLinks;
   msg.m_pFVLinks->push_back( _bstr_t("http:\\foofoo.com") );
   */

   msg.m_pFVPointEx = new FVPointEx;
   msg.m_pFVPointEx->m_bstrAreaCode = _bstr_t(m_area_code);
   msg.m_pFVPointEx->m_bstrCountryCode = _bstr_t(m_country_code);
   msg.m_pFVPointEx->m_bstrDtdId = _bstr_t(m_dtd_id);
   msg.m_pFVPointEx->m_bstrElevationSource = _bstr_t(m_elv_src);
   msg.m_pFVPointEx->m_bstrQuality = _bstr_t(m_qlt);

   msg.m_pFVPointsOverlay = new FVPointsOverlay;
   msg.m_pFVPointsOverlay->m_bstrId = _bstr_t(m_id);
   msg.m_pFVPointsOverlay->m_bstrGroupName = _bstr_t(m_group_name);
   msg.m_pFVPointsOverlay->m_bstrPointDescription = _bstr_t(m_description);

   return msg.Marshal();
}

int C_localpnt_point::InitFromMessage(CLocalPointsMessage *pMessage)
{
   m_strObjectGuid = (char *)pMessage->m_bstrEventUID;

   m_latitude = static_cast<float>(pMessage->m_Point.m_fPointLat);
   m_longitude = static_cast<float>(pMessage->m_Point.m_fPointLon);

   m_dtEventStart = pMessage->m_dtEventStart;

   if (pMessage->m_pFVIcon != NULL)
      m_icon_name = (char *)pMessage->m_pFVIcon->m_bstrIconPath;

   if (pMessage->m_pFVPointEx != NULL)
   {
      if (pMessage->m_pFVPointEx->m_bstrAreaCode.length() >= 1)
         m_area_code = CString((char *)pMessage->m_pFVPointEx->m_bstrAreaCode)[0];

      if (pMessage->m_pFVPointEx->m_bstrCountryCode.length() == 2)
      {
         memcpy(m_country_code, (char *)pMessage->m_pFVPointEx->m_bstrCountryCode, 2);
         m_country_code[2] = '\0';
      }

      const int nDtdIdLength = pMessage->m_pFVPointEx->m_bstrDtdId.length();
      if (nDtdIdLength > 0 && nDtdIdLength < 9)
      {
         memcpy(m_dtd_id, (char *)pMessage->m_pFVPointEx->m_bstrDtdId, nDtdIdLength);
         m_dtd_id[nDtdIdLength] = '\0';
      }

      m_elv_src = CString((char *)pMessage->m_pFVPointEx->m_bstrElevationSource);

      if (pMessage->m_pFVPointEx->m_bstrQuality.length() >= 1)
         m_qlt = CString((char *)pMessage->m_pFVPointEx->m_bstrAreaCode)[0];
   }

   if (pMessage->m_pFVPointsOverlay != NULL)
   {
      m_id = (char *)pMessage->m_pFVPointsOverlay->m_bstrId;
      m_group_name = (char *)pMessage->m_pFVPointsOverlay->m_bstrGroupName;
      m_description = (char *)pMessage->m_pFVPointsOverlay->m_bstrPointDescription;
   }

   if (pMessage->m_pLinks != NULL )
   {
      size_t size = pMessage->m_pLinks->size();
      for ( size_t i = 0; i < size; i++ )
      {
         COT_LINK link = (*pMessage->m_pLinks)[i];
         POINT_LINK* pLink = new POINT_LINK;

         pLink->link_name = link.url;
         pLink->id = m_id;
         pLink->group_name = m_group_name;

         ((C_localpnt_ovl*)m_overlay)->m_links_lst.AddTail(pLink);
      }
   }

   return SUCCESS;
}

void C_localpnt_point::SetIconName(const CString& icon_name)
{
   std::string icon_name_str = icon_name;

   // add a .ico file if there are no dots in the string
   if (icon_name_str.length() > 0 &&
      icon_name_str.find('.') == std::string::npos)
   {
      icon_name_str += ".ico";
   }

   std::string folder_prefix("\\icons\\");
   // strip off leading icons folder name
   if (icon_name_str.find(folder_prefix) != std::string::npos)
   {
      icon_name_str = icon_name_str.substr(
         icon_name_str.find(folder_prefix) + folder_prefix.length());
   }

   m_icon_name = icon_name_str.c_str();
}
