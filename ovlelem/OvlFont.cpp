// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.



// OvlFont.cpp
#include "stdafx.h"
#include "ovlelem.h"
#include "mapx.h"
#include "..\FontDlg.h"    // CFontDlg
#include "graphics.h"

// necessary for OvlFont constructor to initialize the
// boolean_t reference
boolean_t temporary_boolean_t;

#define UTIL_FONT_ATTRIBUTES (UTIL_FONT_BOLD | UTIL_FONT_ITALIC | UTIL_FONT_UNDERLINE | UTIL_FONT_STRIKEOUT)


//*****************************************************************************
// OvlFont
//
// This class is a container for text common text properties which can be used
// in drawing single and multiple line text.  These properties include the font
// name, size, and attirbutes (BOLD, ITALICS, UNDERLINE).  The text color is
// also included in this object, as well as the background color and type.  It
// is meant for use with OvlElement objects.  The allow_redraw parameter is
// supposed to be the m_allow_redraw flag for the OvlElement using this font.
// Through allow_redraw, this font can force a draw from scratch when needed.
//*****************************************************************************

OvlFont::OvlFont(boolean_t &allow_redraw, CString key) : m_allow_redraw(allow_redraw),
   m_font_name(UTIL_FONT_NAME_ARIAL), m_font_size(12), m_font_attributes(0),
   m_fg_color(BRIGHT_WHITE), m_bg_type(UTIL_BG_NONE), m_bg_color(-1), m_key(key)
{
   m_color_type_is_RGB = false;
}

OvlFont::OvlFont(CString key /*= ""*/) : m_allow_redraw(temporary_boolean_t),
   m_font_name(UTIL_FONT_NAME_ARIAL), m_font_size(12), m_font_attributes(0),
   m_fg_color(BRIGHT_WHITE), m_bg_type(UTIL_BG_NONE), m_bg_color(-1), m_key(key)

{
   m_color_type_is_RGB = false;
}


// Set the font.  See utils.h for font names, and font attributes.
int OvlFont::set_font(const char *name, int size, int attributes)
{
   if ((attributes & UTIL_FONT_ATTRIBUTES) != attributes)
   {
      ERR_report("Invalid attributes.");
      return FAILURE;
   }

   if (size < 6 || size > 72)
   {
      ERR_report("Invalid size.");
      return FAILURE;
   }

   // if nothing changed then do nothing
   if (m_font_attributes == attributes && m_font_size == size &&
      m_font_name == name)
      return SUCCESS;

   m_allow_redraw = FALSE;
   m_font_name = name;
   m_font_size = size;
   m_font_attributes = attributes;

   return SUCCESS;
}

// Set the text foreground color.
int OvlFont::set_foreground(int color)
{
   if (color < 0 || color > 255)
   {
      ERR_report("Invalid color.");
      return FAILURE;
   }

   m_fg_color = color;

   m_color_type_is_RGB = false;
   return SUCCESS;
}

// set the text foreground color RGB
int OvlFont::set_foreground_RGB(COLORREF color)
{
   m_fg_color_RGB = color;
   return SUCCESS;
}

// Set text background type and color.  See utils.h for text background
// types.
int OvlFont::set_background(int type, int color)
{
   if (type != UTIL_BG_NONE &&
      type != UTIL_BG_SHADOW &&
      type != UTIL_BG_RECT &&
      type != UTIL_BG_3D &&
      type != UTIL_BG_SHADE)
   {
      ERR_report("[OvlFont::set_background] Invalid type.");
      return FAILURE;
   }

   if (color < -1 || color > 255)
   {
      ERR_report("[OvlFont::set_background] Invalid color.");
      return FAILURE;
   }

   // a change in the background type will force a fresh draw
   if (m_bg_type != type)
   {
      m_bg_type = type;
      m_allow_redraw = FALSE;
   }

   m_bg_color = color;

   m_color_type_is_RGB = false;
   return SUCCESS;
}

int OvlFont::set_background_RGB(int type, COLORREF color)
{
   if (type != UTIL_BG_NONE &&
      type != UTIL_BG_SHADOW &&
      type != UTIL_BG_RECT &&
      type != UTIL_BG_3D &&
      type != UTIL_BG_SHADE)
   {
      ERR_report("[OvlFont::set_background] Invalid type.");
      return FAILURE;
   }

   // a change in the background type will force a fresh draw
   if (m_bg_type != type)
   {
      m_bg_type = type;
      m_allow_redraw = FALSE;
   }

   m_bg_color_RGB = color;

   return SUCCESS;
}

// Get the font name, size, and attributes.
void OvlFont::get_font(CString &name, int &size, int &attributes)
{
   name = m_font_name;
   size = m_font_size;
   attributes = m_font_attributes;
}

// Get the text foreground color.
void OvlFont::get_foreground(int &color)
{
   color = m_fg_color;
}

// get the text RGB foreground color
void OvlFont::get_foreground_RGB(COLORREF &color)
{
   color = m_fg_color_RGB;
}

// Get text background type and color.
void OvlFont::get_background(int &type, int &color)
{
   type = m_bg_type;
   color = m_bg_color;
}

// get text background type and RGB color
void OvlFont::get_background_RGB(int &type, COLORREF &color)
{
   type = m_bg_type;
   color = m_bg_color_RGB;
}

// Equal operator for CFontDlg.  Used to retrieve data
// from the font dialog box
OvlFont &OvlFont::operator=(CFontDlg &fontdlg)
{
   m_font_name = fontdlg.GetFont();
   m_font_size = fontdlg.GetPointSize();
   m_font_attributes = fontdlg.GetAttributes();
   m_bg_type = fontdlg.GetBackground();

   if (fontdlg.GetEnableRgb())
   {
      m_fg_color_RGB = (COLORREF)fontdlg.GetColor();
      m_bg_color_RGB = (COLORREF)fontdlg.GetBackgroundColor();
      m_color_type_is_RGB = true;
   }
   else
   {
      m_fg_color = fontdlg.GetColor();
      m_bg_color = fontdlg.GetBackgroundColor();
      m_color_type_is_RGB = false;
   }

   return *this;
}

// Equal operator
OvlFont &OvlFont::operator=(OvlFont &font)
{
   CString fontname;
   int size, attributes;
   int fg_color, bg_color, type;
   COLORREF fg_colorref, bg_colorref;

   font.get_font(fontname, size, attributes);
   font.get_foreground(fg_color);
   font.get_foreground_RGB(fg_colorref);
   font.get_background(type, bg_color);
   font.get_background_RGB(type, bg_colorref);

   set_font(fontname, size, attributes);
   set_foreground(fg_color);
   set_foreground_RGB(fg_colorref);
   set_background(type, bg_color);
   set_background_RGB(type, bg_colorref);

   m_color_type_is_RGB = font.color_type_is_RGB();

   return *this;
}

// Store values into the OvlFont given a registry key
int OvlFont::initialize_from_registry(CString key, CString default_name,
                                      int default_size, int default_attributes,
                                      int default_fg_color, int default_bg_type,
                                      int default_bg_color)
{
   m_font_name = PRM_get_registry_string(key,"Name", default_name);
   m_font_size = PRM_get_registry_int(key,"Size", default_size);
   m_font_attributes = PRM_get_registry_int(key,"Attributes", default_attributes);
   m_fg_color = PRM_get_registry_int(key,"Foreground Color", default_fg_color);
   m_bg_type = PRM_get_registry_int(key,"Background Type",default_bg_type);
   m_bg_color = PRM_get_registry_int(key,"Background Color",default_bg_color);

   return SUCCESS;
}

// Store values into the OvlFont using m_key as the registry key
int OvlFont::initialize_from_registry(CString default_name,
                                      int default_size, int default_attributes,
                                      int default_fg_color, int default_bg_type,
                                      int default_bg_color)
{
   if (m_key.IsEmpty())
   {
      ERR_report("OvlFont::initialize_from_registry, empty key.");
      return FAILURE;
   }
   return initialize_from_registry(m_key,
      default_name, default_size, default_attributes,
      default_fg_color, default_bg_type, default_bg_color);
}

// Store values into the registry given a key
int OvlFont::save_in_registry(CString key)
{
   if (PRM_set_registry_string(key, "Name", m_font_name) != SUCCESS
       || PRM_set_registry_int(key, "Size", m_font_size) != SUCCESS
       || PRM_set_registry_int(key, "Attributes", m_font_attributes) != SUCCESS
       || PRM_set_registry_int(key, "Foreground Color", m_fg_color) != SUCCESS
       || PRM_set_registry_int(key, "Background Type", m_bg_type) != SUCCESS
       || PRM_set_registry_int(key, "Background Color", m_bg_color) != SUCCESS) {
      ERR_report("OvlFont::save_in_registry, PRM_set_registry_key failed.");
      return FAILURE;
   }
   return SUCCESS;
}

// Store values into the registry using m_key as the registry key
int OvlFont::save_in_registry()
{
   if (m_key.IsEmpty()) {
      ERR_report("OvlFont::save_in_registry, empty key.");
      return FAILURE;
   }
   return save_in_registry(m_key);
}

void OvlFont::SetFont(CString strFontName, int nFontSize,
      CString strBackgroundColor, CString strForegroundColor,
      short nBackgroundType, bool bStrikeout,
      bool bUnderline, bool bBold, bool bItalic)
{
   m_font_name = strFontName;
   m_font_size = nFontSize;
   m_bg_color_RGB = StringToColorref(strBackgroundColor);
   m_fg_color_RGB = StringToColorref(strForegroundColor);
   m_color_type_is_RGB = true;

   m_bg_type = nBackgroundType;

   m_font_attributes |= bStrikeout ? UTIL_FONT_STRIKEOUT : 0;
   m_font_attributes |= bUnderline ? UTIL_FONT_UNDERLINE : 0;
   m_font_attributes |= bBold ? UTIL_FONT_BOLD : 0;
   m_font_attributes |= bItalic ? UTIL_FONT_ITALIC : 0;
}

void OvlFont::GetFont(CString& strFontName, short& nFontSize,
      CString& strBackgroundColor, CString& strForegroundColor,
      short& nBackgroundType, VARIANT_BOOL& bStrikeout,
      VARIANT_BOOL& bUnderline, VARIANT_BOOL& bBold,
      VARIANT_BOOL& bItalic) const
{
   strFontName = m_font_name;
   nFontSize = m_font_size;
   strBackgroundColor = ColorrefToString(m_bg_color_RGB);
   strForegroundColor = ColorrefToString(m_fg_color_RGB);

   nBackgroundType = m_bg_type;

   bStrikeout = (m_font_attributes & UTIL_FONT_STRIKEOUT) == UTIL_FONT_STRIKEOUT ? VARIANT_TRUE : VARIANT_FALSE;
   bUnderline = (m_font_attributes & UTIL_FONT_UNDERLINE) == UTIL_FONT_UNDERLINE ? VARIANT_TRUE : VARIANT_FALSE;
   bBold = (m_font_attributes & UTIL_FONT_BOLD) == UTIL_FONT_BOLD ? VARIANT_TRUE : VARIANT_FALSE;
   bItalic = (m_font_attributes & UTIL_FONT_ITALIC) == UTIL_FONT_ITALIC ? VARIANT_TRUE : VARIANT_FALSE;
}

// Reads a PFPS formatted font type from the registry
int OvlFont::ReadFromRegistry(CString strKey, CString strDefaultFontName, int nDefaultFontSize,
                              CString strDefaultBackgroundColor, CString strDefaultForegroundColor,
                              short nDefaultBackgroundType, bool bDefaultStrikeout,
                              bool bDefaultUnderline, bool bDefaultBold, bool bDefaultItalic)
{
   m_font_name = PRM_get_registry_string(strKey, "FontName", strDefaultFontName);
   m_font_size = PRM_get_registry_int(strKey, "FontSize", nDefaultFontSize);

   m_bg_color_RGB = StringToColorref(PRM_get_registry_string(strKey, "BackgroundColor", strDefaultBackgroundColor));
   m_fg_color_RGB = StringToColorref(PRM_get_registry_string(strKey, "ForegroundColor", strDefaultForegroundColor));
   m_color_type_is_RGB = true;

   short nBackgroundType = PRM_get_registry_int(strKey, "BackgroundType", nDefaultBackgroundType);
   m_bg_type = nBackgroundType;

   m_font_attributes |= PRM_get_registry_int(strKey, "Strikeout", bDefaultStrikeout) ? UTIL_FONT_STRIKEOUT : 0;
   m_font_attributes |= PRM_get_registry_int(strKey, "Underline", bDefaultUnderline) ? UTIL_FONT_UNDERLINE : 0;
   m_font_attributes |= PRM_get_registry_int(strKey, "Bold", bDefaultBold) ? UTIL_FONT_BOLD : 0;
   m_font_attributes |= PRM_get_registry_int(strKey, "Italic", bDefaultItalic) ? UTIL_FONT_ITALIC : 0;

   return SUCCESS;
}

// Stores a PFPS formatted font type to the registry
int OvlFont::SaveToRegistry(CString strKey)
{
   ASSERT(m_color_type_is_RGB);

   PRM_set_registry_string(strKey, "FontName", m_font_name);
   PRM_set_registry_int(strKey, "FontSize", m_font_size);

   PRM_set_registry_string(strKey, "BackgroundColor", ColorrefToString(m_bg_color_RGB));
   PRM_set_registry_string(strKey, "ForegroundColor", ColorrefToString(m_fg_color_RGB));

   PRM_set_registry_int(strKey, "BackgroundType", m_bg_type);

   PRM_set_registry_int(strKey, "Strikeout", (m_font_attributes & UTIL_FONT_STRIKEOUT) == UTIL_FONT_STRIKEOUT);
   PRM_set_registry_int(strKey, "Underline", (m_font_attributes & UTIL_FONT_UNDERLINE) == UTIL_FONT_UNDERLINE);
   PRM_set_registry_int(strKey, "Bold", (m_font_attributes & UTIL_FONT_BOLD) == UTIL_FONT_BOLD);
   PRM_set_registry_int(strKey, "Italic", (m_font_attributes & UTIL_FONT_ITALIC) == UTIL_FONT_ITALIC);

   return SUCCESS;
}

// Convert string representing hexadecimal string (RGB) to COLORREF
//    First character is '#' to indicate color string
//    Next two characters represent alpha
//    Next two characters represent red
//    Next two characters represent green
//    Last two characters represent blue
//
//    For example, #FF000000 (black)
COLORREF OvlFont::StringToColorref(CString strColor)
{
   if ((strColor.GetLength() == 8) && (strColor.GetAt(0) != '#'))
   {
      strColor = "#" + strColor;
   }

   if (strColor.GetLength() != 9)
   {
      CString msg;
      msg.Format("Invalid color: %s", strColor);
      ERR_report(msg);
      return RGB(0,0,0);
   }

   int a, r, g, b;
   if (sscanf_s(strColor, "#%2X%2X%2X%2X", &a, &r, &g, &b) != 4)
   {
      CString msg;
      msg.Format("Invalid color: %s", strColor);
      ERR_report(msg);
      return RGB(0,0,0);
   }

   return RGB(r,g,b);
}

// Convert COLORREF to hexadecimal string representation
CString OvlFont::ColorrefToString(COLORREF color) const
{
   CString ret;
   ret.Format("#FF%02X%02X%02X", GetRValue(color), GetGValue(color),
      GetBValue(color));
   return ret;
}

// returns TRUE if two OvlFonts are equal
int OvlFont::operator==(OvlFont font)
{
   if (m_color_type_is_RGB != font.color_type_is_RGB())
      return FALSE;

   // get the fontname, size, and attributes
   CString name;
   int size, attributes;
   font.get_font(name, size, attributes);

   if (m_color_type_is_RGB)
   {
      int type;
      COLORREF fg_color, bg_color;

      // get the text foreground color.
      font.get_foreground_RGB(fg_color);

      // get text background type and color.
      font.get_background_RGB(type, bg_color);

      return (
         m_font_name == name &&
         m_font_size == size &&
         m_font_attributes == attributes &&
         m_fg_color_RGB == fg_color &&
         m_bg_type == type &&
         m_bg_color_RGB == bg_color);
   }
   else
   {
      int type, fg_color, bg_color;

      // get the text foreground color.
      font.get_foreground(fg_color);

      // get text background type and color.
      font.get_background(type, bg_color);

      return (
         m_font_name == name &&
         m_font_size == size &&
         m_font_attributes == attributes &&
         m_fg_color == fg_color &&
         m_bg_type == type &&
         m_bg_color == bg_color);
   }
}

// return size in bytes needed to store members of this class
int OvlFont::get_block_size(void)
{
   return (
      sizeof(int) + m_font_name.GetLength() +   // storage for length of font and
                                                // the string
      sizeof(int) +                             // font size
      sizeof(int) +                             // font attributes
      sizeof(int) +                             // foreground color
      sizeof(int) +                             // background type
      sizeof(int));                             // background color
}

// Serialize returns an array of bytes corresponding to this object
void OvlFont::Serialize(BYTE *&block_ptr)
{
   // font name
   int str_size = m_font_name.GetLength();
   memcpy(block_ptr, &str_size, sizeof(int));
   block_ptr += sizeof(int);

   if (str_size != 0)
   {
      memcpy(block_ptr,m_font_name.GetBuffer(str_size), str_size);
      block_ptr += str_size;
   }

   // font size
   memcpy(block_ptr, &m_font_size, sizeof(int));
   block_ptr += sizeof(int);

   // font attributes
   memcpy(block_ptr, &m_font_attributes, sizeof(int));
   block_ptr += sizeof(int);

   // foreground color
   memcpy(block_ptr, &m_fg_color, sizeof(int));
   block_ptr += sizeof(int);

   // background type
   memcpy(block_ptr, &m_bg_type, sizeof(int));
   block_ptr += sizeof(int);

   // background color
   memcpy(block_ptr, &m_bg_color, sizeof(int));
   block_ptr += sizeof(int);
}

// Deserialize takes an array of bytes and initializes this object
void OvlFont::Deserialize(BYTE *&block_ptr)
{
   char tmp_buffer[256];

   // font name
   int str_size;
   memcpy(&str_size, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   if (str_size != 0)
   {
      memcpy(tmp_buffer,block_ptr,str_size);
      block_ptr += str_size;
      tmp_buffer[str_size] = '\0';
      m_font_name = tmp_buffer;
   }

   // font size
   memcpy(&m_font_size, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   // font attributes
   memcpy(&m_font_attributes, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   // foreground color
   memcpy(&m_fg_color, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   // background type
   memcpy(&m_bg_type, block_ptr, sizeof(int));
   block_ptr += sizeof(int);

   // background color
   memcpy(&m_bg_color, block_ptr, sizeof(int));
   block_ptr += sizeof(int);
}

// creates a CFont object from this OvlFont
int OvlFont::CreateFont(CFont &cfont)
{
   PLOGFONT plf = (PLOGFONT) LocalAlloc(LPTR, sizeof(LOGFONT));

   // Specify a font typeface name and weight.
   lstrcpyn(plf->lfFaceName, m_font_name.GetBuffer(sizeof(plf->lfFaceName)), LF_FACESIZE);
   plf->lfWeight = FW_NORMAL;

   // this legacy code make FV draw text too small for the specified point size
   plf->lfHeight = m_font_size;

// this code makes FV draw text in true point size on the screen
// plf->lfHeight = -MulDiv(font_size, dc->GetDeviceCaps(LOGPIXELSY), 72);

   plf->lfWidth = 0;

   // set the font attributes
   if (m_font_attributes & UTIL_FONT_BOLD)
      plf->lfWeight = 800;
   else
      plf->lfWeight = 400;

   if (m_font_attributes & UTIL_FONT_ITALIC)
      plf->lfItalic = TRUE;
   else
      plf->lfItalic = FALSE;

   if (m_font_attributes & UTIL_FONT_UNDERLINE)
      plf->lfUnderline = TRUE;
   else
      plf->lfUnderline = FALSE;

   if (m_font_attributes & UTIL_FONT_STRIKEOUT)
      plf->lfStrikeOut = TRUE;
   else
      plf->lfStrikeOut = FALSE;

// plf->lfQuality = PROOF_QUALITY;

   const double angle = 0.0;
   plf->lfEscapement = (long) (- (double) angle * 10.0);
   plf->lfOrientation = plf->lfEscapement;

   if (!cfont.CreateFontIndirect(plf))
   {
      // revert to Arial if the font doesn't exist
      lstrcpyn(plf->lfFaceName, "Arial", LF_FACESIZE);
      if (!cfont.CreateFontIndirect(plf))
      {
         LocalFree((LOCALHANDLE) plf);
         ERR_report("Couldn't create font");
         return FAILURE;
      }
   }

   LocalFree((LOCALHANDLE) plf);

   return SUCCESS;
}

IFvFont* OvlFont::get_fv_font(gfx::GraphicsContextWrapper* gc)
{
   if (m_fv_font == NULL)
   {
      m_fv_font = gc->create_font(_bstr_t(m_font_name), static_cast<float>(m_font_size),
      (FontAttributes)m_font_attributes, (FontBackgroundType)m_bg_type);
   }

   return m_fv_font;
}

IFvPen* OvlFont::get_foreground_pen(gfx::GraphicsContextWrapper* gc)
{
   if (m_fv_pen == NULL)
   {
      if (m_color_type_is_RGB)
         m_fv_pen = gc->create_pen(m_fg_color_RGB, m_bg_type != UTIL_BG_NONE, m_bg_color_RGB, LINE_STYLE_SOLID, 1.0f, FALSE);
      else
      {
         m_fv_pen = gc->create_pen(GRA_get_color_ref(m_fg_color), m_bg_type != UTIL_BG_NONE,
            GRA_get_color_ref(m_bg_color), LINE_STYLE_SOLID, 1.0f, FALSE);
      }
   }

   return m_fv_pen;
}