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
#include "errx.h"
#include "mem.h"
#include "icon_dib.h"

#define USE_DIB_SECT 0

// ----------------------------------------------------------------
// ----------------------------------------------------------------

icon_dib_mask::icon_dib_mask() 
{ 
   m_dib = NULL; 
   m_img = NULL;
	m_halftone_img = NULL;
   m_width = 0; 
   m_height = 0; 
}

icon_dib_mask::~icon_dib_mask() 
{ 
   clear();
}

void icon_dib_mask::clear()
{
	if (m_dib != NULL)
   {
      MEM_free(m_dib);
   }

	if (m_halftone_img != NULL)
		MEM_free(m_halftone_img);
}

void* icon_dib_mask::allocate_dib(size_t num_bytes)
{
   return MEM_malloc(num_bytes);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------

int icon_dib::create(HICON icon)
{
   ICONINFO info;

   if (::GetIconInfo(icon, &info) == 0)
   {
      ERR_report("GetIconInfo failed");
      return FAILURE;
   }

   if (info.hbmMask == NULL || info.hbmColor == NULL)
   { 
      ERR_report("invalid icon");
      return FAILURE;
   } 

   int and_ret = m_and_mask.create(info.hbmMask);
   int xor_ret = m_xor_mask.create(info.hbmColor);

   if (::DeleteObject(info.hbmMask) == 0 || 
      ::DeleteObject(info.hbmColor) == 0)
   {
      ERR_report("DeleteObject failed");
      return FAILURE;
   }

   if (and_ret != SUCCESS)
   {
      ERR_report("create failed");
      return FAILURE;
   }
   if (xor_ret != SUCCESS)
   {
      ERR_report("create failed");
      return FAILURE;
   }

   return SUCCESS;
}

int icon_dib::draw(CDC* dc, int x, int y, bool use_halftone)
{
   if (m_and_mask.draw(dc, x, y) != SUCCESS ||
      m_xor_mask.draw(dc, x, y, use_halftone) != SUCCESS)
   {
      return FAILURE;
   }

   return SUCCESS;
}

int icon_dib::draw(CDC* dc, int x, int y, int width, int height, bool use_halftone,
						 int new_width /*=-1*/, int new_height /*=-1*/)
{
	if (dc->IsPrinting() || (new_width != width && new_width != -1) || (new_height != height && new_height != -1))
	{
		return draw_using_set_pixel(dc, x, y, width, height, new_width, 
			new_height);
	}
	else
	{
		m_and_mask.draw(dc,x,y,width,height);
		m_xor_mask.draw(dc,x,y,width,height, use_halftone);
	}

   return SUCCESS;
}

int icon_dib::draw_using_set_pixel(CDC* dc, int x, int y, int old_width, int old_height,
											  int new_width /* =-1 */, int new_height /* =-1 */)
{
   int row, col;

   if (m_and_mask.m_width != old_width && m_and_mask.m_height != old_height)
   {
      ERR_report("invalid dimensions");
      return FAILURE;
   }

	// if no values were set for new_width or new_height, then set them to
	// equal the old width or old height respectively
	if (new_width == -1)
		new_width = old_width;
	if (new_height == -1)
		new_height = old_height;

	// calculate the new upper left corner
	x = (x + (old_width-new_width)/2);
	y = (y + (old_height-new_height)/2);

   //
   //  these vars declared outside of the loop for efficiency
   //
   CRect rect;
   CBrush brush;
   COLORREF brush_color;
   unsigned char and_color_index;
	RGBQUAD and_col;
	boolean_t opaque;
	unsigned char xor_color_index;
	RGBQUAD xor_col;
	COLORREF byte_color;

   //
   // initialize so that the first reference in first interation of loop works
   //
   brush_color = RGB(255, 255, 255);
   brush.CreateSolidBrush(brush_color);

   const RGBQUAD* const and_colors = (RGBQUAD*) (m_and_mask.m_dib + sizeof(BITMAPINFOHEADER));
   const RGBQUAD* const xor_colors = (RGBQUAD*) (m_xor_mask.m_dib + sizeof(BITMAPINFOHEADER));
	
   const unsigned char* and_ptr = m_and_mask.m_img; 
   const unsigned char* xor_ptr = m_xor_mask.m_img;
	int outer_error = 0;
   for (row=new_height-1; row >= 0; row--)
   {
		int inner_error = 0;
		const unsigned char* and_ptr_inner = and_ptr;
		const unsigned char* xor_ptr_inner = xor_ptr;
		bool use_high_byte = true;
      for (col=0; col<new_width; col++)
      {
         //
         // determine if each the byte is transparent or opaque
         //
			and_color_index = (use_high_byte) ? 
				(*and_ptr_inner) >> 4 : (*and_ptr_inner) & 0x0F;
			and_col = and_colors[and_color_index];

			opaque = (and_col.rgbRed == 0 && and_col.rgbGreen == 0 &&
				and_col.rgbBlue == 0);

         //
         //  get the color for the opaque pixel (if any) and draw
         //
         if (opaque)
         {
            xor_color_index = (use_high_byte) ? 
					(*xor_ptr_inner) >> 4 : (*xor_ptr_inner) & 0x0F;
            xor_col = xor_colors[xor_color_index];
            byte_color = RGB(xor_col.rgbRed, xor_col.rgbGreen,
               xor_col.rgbBlue);
				
				rect.SetRect(x+col, y+row, x+col+1, y+row+1);
				
				if (byte_color != brush_color)
				{
					brush.DeleteObject();
					if (brush.CreateSolidBrush(byte_color) != 0)
					{
						dc->FillRect(&rect, &brush);
						brush_color = byte_color;
					}
				}
				else
				{
					dc->FillRect(&rect, &brush);
				}
			}

			inner_error += old_width;
			if (inner_error >= new_width)
			{
				if (use_high_byte)
					use_high_byte = false;
				else
				{
					and_ptr_inner++;
					xor_ptr_inner++;
					use_high_byte = true;
				}
				inner_error -= new_width;
			}
      }

		outer_error += old_height;
		if (outer_error >= new_height)
		{
			and_ptr += old_width / 2;
			xor_ptr += old_width / 2;
			outer_error -= new_height;
		}
   }

   return SUCCESS;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------

int icon_dib_and_mask::draw(CDC* dc, int x, int y)
{
   return draw(dc, x, y, m_width, m_height);
}

int icon_dib_and_mask::draw(CDC* dc, int x, int y, int width, int height)
{
#if USE_DIB_SECT

   CDC mem_dc;
   mem_dc.CreateCompatibleDC(dc);

   HBITMAP old = (HBITMAP) ::SelectObject(mem_dc.GetSafeHdc(), m_dib_section);

   if (::StretchBlt(dc->GetSafeHdc(), x, y, width, height, mem_dc.GetSafeHdc(),
      0, 0, m_width, m_height, SRCAND) == 0)
   {
      ERR_report("stretchblt");
   } 

   ::SelectObject(mem_dc.GetSafeHdc(), old);

#else

   int ret = ::StretchDIBits(dc->GetSafeHdc(), x, y, width, height,
      0, 0, m_width, m_height, m_img, (BITMAPINFO*)m_dib, DIB_RGB_COLORS, 
      SRCAND);
   if (ret == GDI_ERROR)
   {
      return FAILURE;
   }

#endif

   return SUCCESS;
}

//#define NUM_AND_ENTRIES 2
#define NUM_AND_ENTRIES 16

int icon_dib_and_mask::create(HBITMAP ddb)
{
   BITMAP and_info;

   if (::GetObject(ddb, sizeof(BITMAP), &and_info) == 0)
   {
      ERR_report("GetObject failed");
      return FAILURE;
   }

   if (and_info.bmBitsPixel != 1)
   {
      ERR_report("invalid icon");
      return FAILURE;
   }

   m_width = and_info.bmWidth;
   m_height = and_info.bmHeight;


//  HAS TO BE A MULTIPLE OF 4 ?????

	// divided by two because we will use 4 bits per pixel (so two 
	// pixels will fit in a single byte)
   LONG image_size_in_bytes = (m_height * m_width)/2;

   size_t size = 
      sizeof(BITMAPINFOHEADER) + NUM_AND_ENTRIES*sizeof(RGBQUAD) + image_size_in_bytes;
   m_dib = (unsigned char*) allocate_dib(size);
   if (m_dib == NULL)
   {
      ERR_report("memory allocation error");
      return FAILURE;
   }

   m_img = m_dib + sizeof(BITMAPINFOHEADER) + NUM_AND_ENTRIES*sizeof(RGBQUAD);

   BITMAPINFOHEADER* hdr = (BITMAPINFOHEADER*) m_dib;

   hdr->biSize = sizeof(BITMAPINFOHEADER);
   hdr->biWidth = m_width;
   hdr->biHeight = m_height;
   hdr->biPlanes = 1;
   hdr->biBitCount = 4;
   hdr->biCompression = BI_RGB;
   hdr->biSizeImage = 0;
   hdr->biXPelsPerMeter = 0;
   hdr->biYPelsPerMeter = 0;
   hdr->biClrUsed = 0;
   hdr->biClrImportant = 0;

   int ret;
   {
      CWnd* desktop_win = CWnd::FromHandle(::GetDesktopWindow());
      CWindowDC desktop_dc(desktop_win);

      // CHECK FOR ERRORS
        
      COLORREF old_bk = desktop_dc.SetBkColor(RGB(255, 255, 255));
      COLORREF old_text = desktop_dc.SetTextColor(RGB(0, 0, 0));

      ret = ::GetDIBits(desktop_dc.GetSafeHdc(), ddb, 0, m_height, m_img, 
         (BITMAPINFO*)m_dib, DIB_RGB_COLORS);

      desktop_dc.SetBkColor(old_bk);
      desktop_dc.SetTextColor(old_text);

      if (ret == 0)
      {
         ERR_report("GetDIBits failed");
         MEM_free(m_dib); // ???
         return FAILURE;
      }
   }

#if USE_DIB_SECT // GUE

   void* bits;
   m_dib_section = ::CreateDIBSection(NULL, (BITMAPINFO*)m_dib, DIB_RGB_COLORS,
      &bits, NULL, NULL);
   if (m_dib_section == NULL)
   {
      ERR_report("CreateDibSection");
      return FAILURE;
   }

   // copy the bits
   memcpy(bits, m_img, image_size_in_bytes);
      
#endif

   return SUCCESS;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------

int icon_dib_xor_mask::draw(CDC* dc, int x, int y,  bool use_halftone)
{
   return draw(dc, x, y, m_width, m_height, use_halftone);
}

int icon_dib_xor_mask::draw(CDC* dc, int x, int y, int width, int height, bool use_halftone)
{
#if USE_DIB_SECT

   CDC mem_dc;
   mem_dc.CreateCompatibleDC(dc);

   HBITMAP old = (HBITMAP) ::SelectObject(mem_dc.GetSafeHdc(), m_dib_section);

   if (::StretchBlt(dc->GetSafeHdc(), x, y, width, height, mem_dc.GetSafeHdc(),
      0, 0, m_width, m_height, SRCINVERT) == 0)
   {
      ERR_report("stretchblt");
   } 

   ::SelectObject(mem_dc.GetSafeHdc(), old);

#else

	unsigned char *img = use_halftone ? m_halftone_img : m_img;

   int ret = ::StretchDIBits(dc->GetSafeHdc(), x, y, width, height,
      0, 0, m_width, m_height, img, (BITMAPINFO*)m_dib, DIB_RGB_COLORS, 
      SRCINVERT);
   if (ret == GDI_ERROR)
   {
      return FAILURE;
   }

#endif
   
   return SUCCESS;
}

int icon_dib_xor_mask::create(HBITMAP ddb)
{
   BITMAP xor_info;

   if (::GetObject(ddb, sizeof(BITMAP), &xor_info) == 0)
   {
      ERR_report("GetObject failed");
      return FAILURE;
   }

   m_width = xor_info.bmWidth;
   m_height = xor_info.bmHeight;


//  HAS TO BE A MULTIPLE OF 4 ?????
   LONG image_size_in_bytes = (m_height * m_width)/2;
// WRONG ***

   size_t size = 
      sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD) + image_size_in_bytes;
   m_dib = (unsigned char*) allocate_dib(size);
   if (m_dib == NULL)
   {
      ERR_report("memory allocation error");
      return FAILURE;
   }

   m_img = 
      m_dib + sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD);

	// allocate memory for halftone version of the icon
	m_halftone_img = (unsigned char*)MEM_malloc(image_size_in_bytes);

   BITMAPINFOHEADER* hdr = (BITMAPINFOHEADER*) m_dib;

   hdr->biSize = sizeof(BITMAPINFOHEADER);
   hdr->biWidth = m_width;
   hdr->biHeight = m_height;
   hdr->biPlanes = 1;
   hdr->biBitCount = 4;
   hdr->biCompression = BI_RGB;
   hdr->biSizeImage = 0;
   hdr->biXPelsPerMeter = 0;
   hdr->biYPelsPerMeter = 0;
   hdr->biClrUsed = 0;
   hdr->biClrImportant = 0;

   int ret;
   {
      CWnd* desktop_win = CWnd::FromHandle(::GetDesktopWindow());
      CWindowDC desktop_dc(desktop_win);

      // CHECK FOR ERRORS
        
      COLORREF old_bk = desktop_dc.SetBkColor(RGB(255, 255, 255));
      COLORREF old_text = desktop_dc.SetTextColor(RGB(0, 0, 0));

      ret = ::GetDIBits(desktop_dc.GetSafeHdc(), ddb, 0, m_height, m_img, 
         (BITMAPINFO*)m_dib, DIB_RGB_COLORS);

      desktop_dc.SetBkColor(old_bk);
      desktop_dc.SetTextColor(old_text);

      if (ret == 0)
      {
         ERR_report("GetDIBits failed");
         MEM_free(m_dib); // ???
			MEM_free(m_halftone_img);
         return FAILURE;
      }

		// create the halftone icon by setting every other pixel of the original
		// icon to black
		memcpy(m_halftone_img, m_img, image_size_in_bytes);
		unsigned char *halftone = m_halftone_img;
		for (int j=0;j<m_height;j++)
		{
			for(int i=0;i<m_width;i+=2)
			{
				if ((i+j) % 2 == 0)
					*halftone &= 0xF;
				if ((i+j+1) % 2 == 0)
					*halftone &= 0xF0;

				halftone++;
			}
		}
   }

#if USE_DIB_SECT // GUE

   void* bits;
   m_dib_section = ::CreateDIBSection(NULL, (BITMAPINFO*)m_dib, DIB_RGB_COLORS,
      &bits, NULL, NULL);
   if (m_dib_section == NULL)
   {
      ERR_report("CreateDibSection");
      return FAILURE;
   }

   // copy the bits
   memcpy(bits, m_img, image_size_in_bytes);
      
#endif

   return SUCCESS;
}

// generate a 16x16 4-bit color bitmap from the icon
int icon_dib::generate_bitmap(CBitmap *bitmap)
{
	if (m_and_mask.m_width != 16 && m_and_mask.m_height != 16)
   {
      ERR_report("invalid dimensions");
      return FAILURE;
   }

	const int width = 16;
	const int height = 16;
	LONG image_size_in_bytes = (height * width)/2;

	size_t size = sizeof(BITMAPINFOHEADER) + NUM_AND_ENTRIES*sizeof(RGBQUAD) + image_size_in_bytes;

   unsigned char*dib = (unsigned char*) MEM_malloc(size);
   if (dib == NULL)
   {
      ERR_report("memory allocation error");
      return FAILURE;
   }

   unsigned char *img = dib + sizeof(BITMAPINFOHEADER) + NUM_AND_ENTRIES*sizeof(RGBQUAD);
	
	BITMAPINFOHEADER* hdr = (BITMAPINFOHEADER*)dib;

   hdr->biSize = sizeof(BITMAPINFOHEADER);
   hdr->biWidth = width;
   hdr->biHeight = height;
   hdr->biPlanes = 1;
   hdr->biBitCount = 4;
   hdr->biCompression = BI_RGB;
   hdr->biSizeImage = 0;
   hdr->biXPelsPerMeter = 0;
   hdr->biYPelsPerMeter = 0;
   hdr->biClrUsed = 0;
   hdr->biClrImportant = 0;

	void* bits;
	HDC hdc = CreateCompatibleDC(NULL);
   HBITMAP dib_section = ::CreateDIBSection(hdc, (BITMAPINFO*)dib, DIB_RGB_COLORS, &bits, NULL, NULL);
   if (dib_section == NULL)
   {
      ERR_report("CreateDibSection failed");
      return FAILURE;
   }

	int row, col;
	
   unsigned char hi_and_color_index;
   RGBQUAD hi_and_col;
   unsigned char hi_xor_color_index;
   RGBQUAD hi_xor_col;
   COLORREF hi_byte_color;
   unsigned char lo_and_color_index;
   RGBQUAD lo_and_col;
   unsigned char lo_xor_color_index;
   RGBQUAD lo_xor_col;
   COLORREF lo_byte_color;

   //
   const RGBQUAD* const and_colors = (RGBQUAD*) (m_and_mask.m_dib + sizeof(BITMAPINFOHEADER));
   const RGBQUAD* const xor_colors = (RGBQUAD*) (m_xor_mask.m_dib + sizeof(BITMAPINFOHEADER));

   const unsigned char* and_ptr = m_and_mask.m_img; 
   const unsigned char* xor_ptr = m_xor_mask.m_img; 

	// read the first 4-bits and initialize the bitmap using this index
	int value = xor_ptr[0] >> 4;
	value |= (value << 4);
	memset(bits, value, 8*16);

	BYTE *bits_ptr = (BYTE *)bits;


   for (row=m_and_mask.m_height-1; row >= 0; row--)
   {
      //
      // process 2 pixels at a time
      //
		BYTE *bits_ptr_inner = bits_ptr;
      for (col=0; col<m_and_mask.m_width; col += 2)
      {

			BYTE b = *bits_ptr_inner;

         //
         // hi byte
         //
         {
            hi_and_color_index = (*and_ptr) >> 4;
            hi_and_col = and_colors[hi_and_color_index];
            if (hi_and_col.rgbRed == 0 && hi_and_col.rgbGreen == 0 &&
               hi_and_col.rgbBlue == 0)
            {
               hi_xor_color_index = (*xor_ptr) >> 4;
               hi_xor_col = xor_colors[hi_xor_color_index];
               hi_byte_color = RGB(hi_xor_col.rgbRed, hi_xor_col.rgbGreen,
                  hi_xor_col.rgbBlue);

					b = b & 0x0F;
					b |= (hi_xor_color_index << 4);
            }
         }

         //
         // lo byte
         //
         {
            lo_and_color_index = (*and_ptr) & 0x0F;
            lo_and_col = and_colors[lo_and_color_index];
            if (lo_and_col.rgbRed == 0 && lo_and_col.rgbGreen == 0 &&
               lo_and_col.rgbBlue == 0)
            {
               lo_xor_color_index = (*xor_ptr) & 0x0F;
               lo_xor_col = xor_colors[lo_xor_color_index];
               lo_byte_color = RGB(lo_xor_col.rgbRed, lo_xor_col.rgbGreen,
                  lo_xor_col.rgbBlue);

					b = b & 0xF0;
					b |= lo_xor_color_index;
            }
         }

			*bits_ptr_inner = b;

         and_ptr ++;
         xor_ptr ++;
			bits_ptr_inner++;
      }

		bits_ptr += 8;
   }
	
	bitmap->Attach(dib_section);
		//CreateBitmap(16, 16, 1, 4, bits);

	BITMAP info;
	GetObject((HBITMAP)*bitmap, sizeof(BITMAP), &info);

	/*
	{
		BYTE *ptr = (BYTE *)bits;
		for(int y=0;y<16;y++)
		{
			BYTE *p2 = ptr;
			TRACE("\n");
			for(int x=0;x<8;x++)
			{
				TRACE("%X",*p2);
				p2++;
			}
			ptr += 8;
		}
		TRACE("\n");
	}
	*/

	return SUCCESS;
}
