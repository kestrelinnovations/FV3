// Copyright (c) 1994-2009,2013 Georgia Tech Research Corporation, Atlanta, GA
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



#ifndef ICON_DIB_H
#define ICON_DIB_H

// -------------------------------------------------------------------------

class icon_dib;

// -------------------------------------------------------------------------

class icon_dib_mask 
{

public:

   icon_dib_mask();
   virtual ~icon_dib_mask();

   virtual int create(HBITMAP ddb) = 0;
   void clear();

   int get_width() { return m_width; }
   int get_height() { return m_height; }
   const BITMAPINFO& get_bitmap_info() const
   { return reinterpret_cast< const BITMAPINFO& >( *m_dib ); }
   const PBYTE get_mask() const { return m_img; }

protected:

   void* allocate_dib(size_t num_bytes);

   unsigned char* m_dib;
   unsigned char* m_img;
   unsigned char* m_halftone_img;
   int m_width;
   int m_height;
};

// -------------------------------------------------------------------------

//
//  a DIB with white everywhere that the icon is transparent and
//  black everywhere that is opaque
//
class icon_dib_and_mask : public icon_dib_mask
{

public:

   int create(HBITMAP ddb); 
   int draw(CDC* dc, int x, int y);
   int draw(CDC* dc, int x, int y, int width, int height);

protected:

   friend icon_dib;

};

// -------------------------------------------------------------------------

//
//  a DIB with black everywhere that the icon is transparent and
//  unchanged everywhere that is opaque
//
class icon_dib_xor_mask : public icon_dib_mask
{

public:

   int create(HBITMAP ddb); 
   int draw(CDC* dc, int x, int y, bool use_halftone);
   int draw(CDC* dc, int x, int y, int width, int height, bool use_halftone);

protected:

   friend icon_dib;

};

// -------------------------------------------------------------------------

//
// this class takes an icon and converts it to 2 dib section masks that can
// be used to draw icons in a device-independent manner
//
class icon_dib
{

public:

   icon_dib() {}
   virtual ~icon_dib() {}

   int create(HICON icon);
   int draw(CDC* dc, int x, int y, bool use_halftone);
   int draw(CDC* dc, int x, int y, int width, int height, bool use_halftone,
      int new_width = -1, int new_height = -1);
   int draw_using_set_pixel(CDC* dc, int x, int y);
   int draw_using_set_pixel(CDC* dc, int x, int y, int width, int height,
      int new_width = -1, int new_height = -1);

   /// generate a 16x16 4-bit color bitmap from the icon
   int generate_bitmap(CBitmap *bitmap);

public:

   icon_dib_and_mask m_and_mask;
   icon_dib_xor_mask m_xor_mask;

};

#endif
