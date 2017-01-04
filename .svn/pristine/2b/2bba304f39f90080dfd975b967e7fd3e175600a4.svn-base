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

// dib.cpp



#include "stdafx.h"

#include "Dib.h"
#include "common.h"  // for SUCCESS and FAILURE
#include <windowsx.h>

#ifndef AC_SRC_ALPHA
   #define AC_SRC_ALPHA (0x01)   /* Not defined in wingdi.h */
#endif

//#pragma comment(linker, "/delayload:msimg32.dll")
#pragma comment(lib, "msimg32")
#pragma comment(lib, "delayimp")


// ****************************************************************
// ****************************************************************

BITMAPINFO* CMyDib::m_static_dib_bitmapinfo;
unsigned char* CMyDib::m_static_dib_data = NULL;
int CMyDib::m_static_num_row_bytes;
int CMyDib::m_static_image_width;
int CMyDib::m_static_image_height;
int CMyDib::m_static_bits_pixel;
BYTE* CMyDib::m_static_lut;

// ****************************************************************
// ****************************************************************

CMyDib::CMyDib( )
{
   // constructor
	m_dib_data = NULL;
	m_static_dib_data = NULL;
	m_dib_bitmapinfo = NULL;
	m_hdib = NULL;
	m_pal_set = FALSE;
	clear( );
	m_histogram = NULL;
	m_color_index = NULL;
	m_bits_pixel = 24;
}

// ****************************************************************
// ****************************************************************

CMyDib::~CMyDib( )
{
   // destructor
   clear( );
	if (m_histogram != NULL)
		free(m_histogram);
	if (m_color_index != NULL)
		free(m_color_index);
	if (m_dib_data != NULL)
		free(m_dib_data);
	if ( m_dib_bitmapinfo != NULL )
		free(m_dib_bitmapinfo);
}

// ****************************************************************
// ****************************************************************

void CMyDib::clear( )
{
   // clear the dib to uninitialized state
   m_initialized = FALSE;
   m_image_width = 0;
   m_image_height = 0;
   m_num_pixels = 0;
   m_num_row_bytes = 0;
	m_number_unique_colors = 0;
   m_static_num_row_bytes = 0;
   m_num_extra_row_bytes = 0;
   if( m_dib_data != NULL )
      free(m_dib_data);
	m_dib_data = NULL;
	if ( m_dib_bitmapinfo != NULL )
		free(m_dib_bitmapinfo);
	m_dib_bitmapinfo = NULL;
	m_dib_data = NULL;
	m_static_dib_data = NULL;
	m_dib_bitmapinfo = NULL;

	// clear the palette
	m_pal_set = FALSE;
}

// ****************************************************************
// ****************************************************************

void CMyDib::erase( unsigned char red, unsigned char green, unsigned char blue )
{
   // erases the dib to the specified color

   int hpix, vpix, index;

   // check for initialized
   if( !m_initialized ) 
		return;

   for( vpix = m_image_height-1, index = 0; vpix >= 0; vpix--, index += m_num_extra_row_bytes )
   {
      for( hpix = 0; hpix < m_image_width; hpix++, index += 3 )
      {
         m_dib_data[index] = blue;
         m_dib_data[index+1] = green;
         m_dib_data[index+2] = red;
      }
   }

}

// ****************************************************************
// ****************************************************************

void CMyDib::erase_216( unsigned char red, unsigned char green, unsigned char blue )
{
   // erases the dib to the specified color

   int k, hpix, vpix, index;

   // check for initialized
   if ( !m_initialized ) 
		return;

	for (k=0; k<256; k++)
	{
		m_lut[(k*4)+0] = red;
		m_lut[(k*4)+1] = green;
		m_lut[(k*4)+2] = blue;
		m_lut[(k*4)+3] = 0;
	}

	for (vpix = 0; vpix < m_image_height; vpix++)
	{
		for (hpix = 0; hpix < m_image_width; hpix++)
		{
			index = (vpix * m_num_row_bytes) + hpix;
			m_dib_data[index] = 0;
		}
	}
}

// ****************************************************************
// ****************************************************************

void CMyDib::erase_1( unsigned char color )
{
   // erases the dib to the specified color

   int hpix, vpix, index;

   // check for initialized
   if ( !m_initialized ) 
		return;

	for (vpix = 0; vpix < m_image_height; vpix++)
	{
		for (hpix = 0; hpix < m_num_row_bytes; hpix++)
		{
			index = (vpix * m_num_row_bytes) + hpix;
			if (color == 0)
				m_dib_data[index] = 0;
			else
				m_dib_data[index] = 255;
		}
	}
}

// ****************************************************************
// ****************************************************************

int CMyDib::initialize( int width, int height, unsigned char red, unsigned char green,
                      unsigned char blue )
{
   // initialize dib with specified height, width, and color
   // returns 0 for success, -1 for failure
   
   int i;
//   int rslt;
   
//	if (m_bits_pixel == 8)
//	{
//		rslt = initialize_216(width, height, red, green, blue);
//		return rslt;
//	}

   // first, clear the dib
   clear( );

   // if specified width or height is < 1, return failure
   if( width < 1 || height < 1 ) 
		return -1;

	m_bits_pixel = 24;
	m_static_bits_pixel = 24;

   m_image_width = width;
   m_image_height = height;
   m_num_pixels = m_image_width * m_image_height;

   // determine number of bytes per row, must be padded to even number of 4-byte words
   m_num_row_bytes = 3 * m_image_width;
   i = m_num_row_bytes / 4;
   if ( 4*i != m_num_row_bytes ) 
		m_num_row_bytes = 4 * (i + 1);
   m_num_extra_row_bytes = m_num_row_bytes - 3 * m_image_width;

   // setup bitmap info structure
	m_dib_bitmapinfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
   m_dib_bitmapinfo->bmiHeader.biSize = 40;                  // size of BITMAPINFO structre
   m_dib_bitmapinfo->bmiHeader.biWidth = m_image_width;      // width of image
   m_dib_bitmapinfo->bmiHeader.biHeight = m_image_height;    // height of image
   m_dib_bitmapinfo->bmiHeader.biPlanes = 1;                 // number of planes - must be 1
   m_dib_bitmapinfo->bmiHeader.biBitCount = 24;              // bits per pixel
   m_dib_bitmapinfo->bmiHeader.biCompression = BI_RGB;       // uncompressed RGB triplets
   m_dib_bitmapinfo->bmiHeader.biSizeImage = 0;              // zero for BI_RGB compression
   m_dib_bitmapinfo->bmiHeader.biXPelsPerMeter = 1000;       // horizontal pixels per meter
   m_dib_bitmapinfo->bmiHeader.biYPelsPerMeter = 1000;       // vertical pixels per meter
   m_dib_bitmapinfo->bmiHeader.biClrUsed = 0;                // number of colors used - zero for 24-bit
   m_dib_bitmapinfo->bmiHeader.biClrImportant = 0;           // number of important colors - zero for all

   m_dib_bitmapinfo->bmiColors[0].rgbBlue = 0;               // color table is null for 24-bit
   m_dib_bitmapinfo->bmiColors[0].rgbGreen = 0;
   m_dib_bitmapinfo->bmiColors[0].rgbRed = 0;
   m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;

	m_static_dib_bitmapinfo = m_dib_bitmapinfo;
	m_static_image_width = m_image_width;
	m_static_image_height = m_image_height;
	m_static_num_row_bytes = m_num_row_bytes;

   // allocate memory for dib data
	m_dib_size = m_image_height * m_num_row_bytes;
   m_dib_data = (unsigned char*) malloc(m_dib_size);
   if ( m_dib_data == NULL )
   {
      clear( );
      return -1;
   }
	m_static_dib_data = m_dib_data;

   m_initialized = TRUE;
   
	if (red >= 0)
	   // erase to specified color
		erase( red, green, blue );

   return 0;
}
// end of initialize

// ****************************************************************
// ****************************************************************

int CMyDib::initialize_216( int width, int height, unsigned char red, unsigned char green,
                      unsigned char blue )
{
   // initialize dib with specified height, width, and color
   // returns 0 for success, -1 for failure
   
   int i, k;
   
   // first, clear the dib
   clear( );

	// clear the lut
	for (k=0; k<1024; k++)
		m_lut[k] = 0;

   // if specified width or height is < 1, return failure
   if( width < 1 || height < 1 ) 
		return -1;

	m_bits_pixel = 8;
	m_static_bits_pixel = 8;

   m_image_width = width;
   m_image_height = height;
   m_num_pixels = m_image_width * m_image_height;

   // determine number of bytes per row, must be padded to even number of 4-byte words
   m_num_row_bytes = m_image_width;
   i = m_num_row_bytes / 4;
   if ( 4*i != m_num_row_bytes ) 
		m_num_row_bytes = 4 * (i + 1);
   m_num_extra_row_bytes = m_num_row_bytes - m_image_width;

   // setup bitmap info structure
	m_dib_bitmapinfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO) + 1024);
   m_dib_bitmapinfo->bmiHeader.biSize = 40;                  // size of BITMAPINFO structre
   m_dib_bitmapinfo->bmiHeader.biWidth = m_image_width;      // width of image
   m_dib_bitmapinfo->bmiHeader.biHeight = m_image_height;    // height of image
   m_dib_bitmapinfo->bmiHeader.biPlanes = 1;                 // number of planes - must be 1
   m_dib_bitmapinfo->bmiHeader.biBitCount = 8;               // bits per pixel
   m_dib_bitmapinfo->bmiHeader.biCompression = BI_RGB;       // uncompressed RGB triplets
   m_dib_bitmapinfo->bmiHeader.biSizeImage = 0;              // zero for BI_RGB compression
   m_dib_bitmapinfo->bmiHeader.biXPelsPerMeter = 1000;       // horizontal pixels per meter
   m_dib_bitmapinfo->bmiHeader.biYPelsPerMeter = 1000;       // vertical pixels per meter
   m_dib_bitmapinfo->bmiHeader.biClrUsed = 256;                // number of colors used - zero for 24-bit
   m_dib_bitmapinfo->bmiHeader.biClrImportant = 256;           // number of important colors - zero for all

	for (k=0; k<256; k++)
	{
		m_dib_bitmapinfo->bmiColors[k].rgbBlue = (BYTE) k;               
		m_dib_bitmapinfo->bmiColors[k].rgbGreen = (BYTE) k;
		m_dib_bitmapinfo->bmiColors[k].rgbRed = (BYTE) k;
		m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;
	}
	m_static_dib_bitmapinfo = m_dib_bitmapinfo;
	m_static_image_width = m_image_width;
	m_static_image_height = m_image_height;
	m_static_num_row_bytes = m_num_row_bytes;

   // allocate memory for dib data
	m_dib_size = m_image_height * m_num_row_bytes;
   m_dib_data = (unsigned char*) malloc(m_dib_size);
   if ( m_dib_data == NULL )
   {
      clear( );
      return -1;
   }
	m_static_dib_data = m_dib_data;

   m_initialized = TRUE;
   
	if (red >= 0)
	   // erase to specified color
		erase_216( red, green, blue );

   return 0;
}
// end of initialize_216

// ****************************************************************
// ****************************************************************

int CMyDib::initialize_1( int width, int height, unsigned char color)
{
   // initialize dib with specified height, width, and color
   // returns 0 for success, -1 for failure
   
   int i;
   
   // first, clear the dib
   clear( );

   // if specified width or height is < 1, return failure
   if ( width < 1 || height < 1 ) 
		return -1;

	m_bits_pixel = 1;
	m_static_bits_pixel = 1;

   m_image_width = width;
   m_image_height = height;
   m_num_pixels = m_image_width * m_image_height;

   // determine number of bytes per row, must be padded to even number of 4-byte words
   m_num_row_bytes = m_image_width / 8;
   i = m_num_row_bytes / 4;
   if ( 4*i != m_num_row_bytes ) 
		m_num_row_bytes = 4 * (i + 1);
   m_num_extra_row_bytes = m_num_row_bytes - m_image_width;

   // setup bitmap info structure
	m_dib_bitmapinfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO) + 1024);
   m_dib_bitmapinfo->bmiHeader.biSize = 40;                  // size of BITMAPINFO structre
   m_dib_bitmapinfo->bmiHeader.biWidth = m_image_width;      // width of image
   m_dib_bitmapinfo->bmiHeader.biHeight = m_image_height;    // height of image
   m_dib_bitmapinfo->bmiHeader.biPlanes = 1;                 // number of planes - must be 1
   m_dib_bitmapinfo->bmiHeader.biBitCount = 1;               // bits per pixel
   m_dib_bitmapinfo->bmiHeader.biCompression = BI_RGB;       // uncompressed RGB triplets
   m_dib_bitmapinfo->bmiHeader.biSizeImage = 0;              // zero for BI_RGB compression
   m_dib_bitmapinfo->bmiHeader.biXPelsPerMeter = 1000;       // horizontal pixels per meter
   m_dib_bitmapinfo->bmiHeader.biYPelsPerMeter = 1000;       // vertical pixels per meter
   m_dib_bitmapinfo->bmiHeader.biClrUsed = 2;                // number of colors used - zero for 24-bit
   m_dib_bitmapinfo->bmiHeader.biClrImportant = 2;           // number of important colors - zero for all

	// setup the color table
	m_dib_bitmapinfo->bmiColors[0].rgbBlue = (BYTE) 0;               
	m_dib_bitmapinfo->bmiColors[0].rgbGreen = (BYTE) 0;
	m_dib_bitmapinfo->bmiColors[0].rgbRed = (BYTE) 0;
	m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;
	m_dib_bitmapinfo->bmiColors[1].rgbBlue = (BYTE) 255;               
	m_dib_bitmapinfo->bmiColors[1].rgbGreen = (BYTE) 255;
	m_dib_bitmapinfo->bmiColors[1].rgbRed = (BYTE) 255;
	m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;

	m_static_dib_bitmapinfo = m_dib_bitmapinfo;
	m_static_image_width = m_image_width;
	m_static_image_height = m_image_height;
	m_static_num_row_bytes = m_num_row_bytes;

   // allocate memory for dib data
	m_dib_size = m_image_height * m_num_row_bytes;
	m_dib_data = (unsigned char*) malloc(m_dib_size);
	if ( m_dib_data == NULL )
	{
		clear( );
		return -1;
	}
	m_static_dib_data = m_dib_data;

	m_initialized = TRUE;

	erase_1( color );

	return 0;
}
// end of initialize_1

// ****************************************************************
// ****************************************************************
/*
int CMyDib::display_image( CDC *pDC, int dest_hpix, int dest_vpix )
{
   // displays the entire dib on the specified device context with the specified
   // destination upper left corner
   // returns 0 for success, -1 for failure

   CPalette pal, *oldpal;

   if( !m_initialized ) 
		return -1;
   
	if (m_bits_pixel <= 8)
	{
		if (!m_pal_set)
		{
			m_pal_set = TRUE;
			create_palette(&m_pal);
		}
		oldpal = pDC->SelectPalette( &m_pal, TRUE);
	}

	pDC->RealizePalette();

	pDC->SetMapMode(MM_TEXT);

   SetDIBitsToDevice( pDC->m_hDC, dest_hpix, dest_vpix, m_image_width, m_image_height,
                    0, 0, 0, m_image_height, m_dib_data, m_dib_bitmapinfo,
                      DIB_RGB_COLORS );

	if (m_bits_pixel <= 8)
		pDC->SelectPalette( oldpal, TRUE);

   return 0;
}
// end of display_image
*/
// ****************************************************************
// ****************************************************************

int CMyDib::display_image( CDC *pDC, int dest_hpix, int dest_vpix )
{
   // displays the entire dib on the specified device context with the specified
   // destination upper left corner
   // returns 0 for success, -1 for failure

   CPalette pal, *oldpal;

   if( !m_initialized ) 
		return -1;
   
	if (m_bits_pixel <= 8)
	{
		if (!m_pal_set)
		{
			m_pal_set = TRUE;
			create_palette(&m_pal);
		}
		oldpal = pDC->SelectPalette( &m_pal, TRUE);
	}

	pDC->RealizePalette();

	CBitmap src;
	HBITMAP mask;
	CDC memdc;

	src.CreateBitmap(m_image_width, m_image_height, 1, 24, m_dib_data);

	mask = CreateBitmapMask((HBITMAP) src, RGB(0,0,0));

	memdc.CreateCompatibleDC(pDC);

	memdc.SelectObject(&mask);
	pDC->BitBlt(dest_hpix, dest_vpix, m_image_width, m_image_height, &memdc, 0, 0, SRCAND);
	
	memdc.SelectObject(&src);
	pDC->BitBlt(dest_hpix, dest_vpix, m_image_width, m_image_height, &memdc, 0, 0, SRCPAINT   );
	
//   SetDIBitsToDevice( pDC->m_hDC, dest_hpix, dest_vpix, m_image_width, m_image_height,
//                    0, 0, 0, m_image_height, m_dib_data, m_dib_bitmapinfo,
//                      DIB_RGB_COLORS );

	if (m_bits_pixel <= 8)
		pDC->SelectPalette( oldpal, TRUE);

   return 0;
}
// end of display_image

// ****************************************************************
// ****************************************************************

int CMyDib::display_image( CDC *dc, int dest_hpix, int dest_vpix, int width, int height )
{
   // displays the entire dib on the specified device context with the specified
   // destination upper left corner
   // returns 0 for success, -1 for failure

	CPalette pal, *oldpal;
	CPen pen;
	int nsavedc;

   if ( !m_initialized ) 
		return -1;
   
	if (m_bits_pixel <= 8)
	{
		if (!m_pal_set)
		{
			m_pal_set = TRUE;
			create_palette(&m_pal);
		}
		oldpal = dc->SelectPalette( &m_pal, TRUE);
	}

	dc->RealizePalette();

	nsavedc = dc->SaveDC();

//	pen.CreateStockObject(NULL_PEN);
//	dc->SelectObject(&pen);
//	dc->BeginPath();
//	dc->MoveTo(dest_hpix, dest_vpix);
//	dc->LineTo(dest_hpix+width, dest_vpix);
//	dc->LineTo(dest_hpix+width, dest_vpix+height);
//	dc->LineTo(dest_hpix, dest_vpix+height);
//	dc->LineTo(dest_hpix, dest_vpix);

//	dc->EndPath();

//	if (!dc->IsPrinting())
//		dc->SelectClipPath(RGN_COPY);
//	else
//		dc->SelectClipPath(RGN_AND);


	CBitmap src;
//	HBITMAP mask;
	CDC memdc;

//	src.CreateBitmap(m_image_width, m_image_height, 1, 24, m_dib_data);

//	mask = CreateBitmapMask((HBITMAP) src, RGB(0,0,0));

	memdc.CreateCompatibleDC(dc);

//	memdc.SelectObject(&mask);
//	dc->BitBlt(dest_hpix, dest_vpix, m_image_width, m_image_height, &memdc, 0, 0, SRCAND);
	
//	memdc.SelectObject(&src);

//	SelectBitmap(memdc.m_hDC, (HBITMAP) src);
//	dc->BitBlt(dest_hpix, dest_vpix, m_image_width, m_image_height, &memdc, 0, 0, SRCPAINT   );
//	dc->BitBlt(dest_hpix, dest_vpix, m_image_width, m_image_height, &memdc, 0, 0, SRCCOPY   );

   SetDIBitsToDevice( dc->m_hDC, dest_hpix, dest_vpix, m_image_width, m_image_height,
                    0, 0, 0, m_image_height, m_dib_data, m_dib_bitmapinfo, DIB_RGB_COLORS );

	if (m_bits_pixel <= 8)
		dc->SelectPalette( oldpal, TRUE);

	if (nsavedc != 0)
		dc->RestoreDC(nsavedc);

   return 0;
}
// end of display_image

// ****************************************************************
// ****************************************************************

// code 8-31-04

void CMyDib::display_transparent(CDC * pDC, int dest_hpix, int dest_vpix, COLORREF crColour)
//int CDib::display_image( CDC *pDC, int dest_hpix, int dest_vpix )
{
	HBITMAP bmp, mask, scrbmp;
	BITMAP bm;
	HBITMAP hBmOld;
	HDC hdc;
	BYTE *img = NULL;
	CDC memDC, scrDC;

	hdc = pDC->GetSafeHdc();

	// create the src image bitmap
	bmp = CreateDIBSection(hdc, m_dib_bitmapinfo, DIB_RGB_COLORS, (void**) &img, NULL, NULL);
	memcpy(img, m_dib_data, m_image_width *  m_image_height * 3);

	::GetObject( bmp, sizeof( bm ), &bm );
	CPoint size( bm.bmWidth, bm.bmHeight );
//	pDC->DPtoLP(&size);

	CPoint org(0,0);
//	pDC->DPtoLP(&org);

	// Create a memory DC compatible with the destination DC
	memDC.CreateCompatibleDC( pDC );
	
	mask = CreateBitmapMask((HBITMAP) bmp, crColour);

	scrDC.CreateCompatibleDC( pDC );
	scrbmp = CreateCompatibleBitmap(hdc, size.x, size.y);

	// copy the screen to the offscreen bitmap
	::SelectObject( scrDC.m_hDC, scrbmp );
	scrDC.BitBlt(0, 0, size.x, size.y, pDC, dest_hpix, dest_vpix, SRCCOPY);

	// and the mask with the offscreen bitmap
	hBmOld = (HBITMAP)::SelectObject( memDC.m_hDC, mask );
	scrDC.BitBlt(0, 0, size.x, size.y, &memDC, org.x, org.y, SRCAND);
	
	// OR the image with the offscreen bitmap
	SelectObject( memDC.m_hDC, bmp );
	scrDC.BitBlt(0, 0, size.x, size.y, &memDC, org.x, org.y, SRCPAINT);

	// copy the offscreen bitmap to the screen
	::SelectObject( scrDC.m_hDC, scrbmp );
	pDC->BitBlt(dest_hpix, dest_vpix, size.x, size.y, &scrDC, org.x, org.y, SRCCOPY);

	::SelectObject( memDC.m_hDC, hBmOld );
}
// end of display_transparent


// ****************************************************************
// ****************************************************************

// code 8-31-04

void CMyDib::display_hashed_transparent(CDC * pDC, int dest_hpix, int dest_vpix, int trans_code)
{
	HBITMAP bmp, mask, scrbmp;
	BITMAP bm;
	HBITMAP hBmOld;
	HDC hdc;
	BYTE *img = NULL;
	CDC memDC, scrDC;
	int freq, trans,k, bsize, x, y, inc;

	trans = trans_code;

	trans /= 26;
	freq = 25;
	switch(trans)
	{
		case 9: freq = 25; break;
		case 8: freq = 12; break;
		case 7: freq = 8; break;
		case 6: freq = 7; break;
		case 5: freq = 6; break;
		case 4: freq = 5; break;
		case 3: freq = 4; break;
		case 2: freq = 3; break;
		case 1: freq = 2; break;
		case 0: freq = 1; break;
	}

	bsize = m_image_width *  m_image_height;

	hdc = pDC->GetSafeHdc();

	// create the src image bitmap
	bmp = CreateDIBSection(hdc, m_dib_bitmapinfo, DIB_RGB_COLORS, (void**) &img, NULL, NULL);
	memcpy(img, m_dib_data, m_image_width *  m_image_height * 3);

	inc = 0;
	if ((freq > 0) && (freq < 25))
	{
		// make some pixels black
		for (y=0; y<m_image_height; y+=freq)
		{
			inc++;
			for (x=0; x<m_image_width; x+=freq)
			{
				k = (y*m_image_width) + x;
				if (((inc%2) == 0) && (x < m_image_width-freq))
					k += freq / 2;
				img[k*3+0] = 0;
				img[k*3+1] = 0;
				img[k*3+2] = 0;
			}
		}
	}

	::GetObject( bmp, sizeof( bm ), &bm );
	CPoint size( bm.bmWidth, bm.bmHeight );

	CPoint org(0,0);

	// Create a memory DC compatible with the destination DC
	memDC.CreateCompatibleDC( pDC );
	
	mask = CreateBitmapMask((HBITMAP) bmp, RGB(0,0,0));

	scrDC.CreateCompatibleDC( pDC );
	scrbmp = CreateCompatibleBitmap(hdc, size.x, size.y);

	// copy the screen to the offscreen bitmap
	::SelectObject( scrDC.m_hDC, scrbmp );
	scrDC.BitBlt(0, 0, size.x, size.y, pDC, dest_hpix, dest_vpix, SRCCOPY);

	// and the mask with the offscreen bitmap
	hBmOld = (HBITMAP)::SelectObject( memDC.m_hDC, mask );
	scrDC.BitBlt(0, 0, size.x, size.y, &memDC, org.x, org.y, SRCAND);
	
	// OR the image with the offscreen bitmap
	SelectObject( memDC.m_hDC, bmp );
	scrDC.BitBlt(0, 0, size.x, size.y, &memDC, org.x, org.y, SRCPAINT);

	// copy the offscreen bitmap to the screen
	::SelectObject( scrDC.m_hDC, scrbmp );
	pDC->BitBlt(dest_hpix, dest_vpix, size.x, size.y, &scrDC, org.x, org.y, SRCCOPY);

	::SelectObject( memDC.m_hDC, hBmOld );
}
// end of display_transparent


// ****************************************************************
// ****************************************************************

HBITMAP CMyDib::CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;

    // Create monochrome (1 bit) mask bitmap.  

    GetObject(hbmColour, sizeof(BITMAP), &bm);
    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver

    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);

    SelectBitmap(hdcMem, hbmColour);
    SelectBitmap(hdcMem2, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.

    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    // Clean up.

    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);

    return hbmMask;
}


// ****************************************************************
// ****************************************************************

int CMyDib::stretch_display_image( CDC *pDC, int dest_min_hpix, int dest_min_vpix,
                                 int dest_max_hpix, int dest_max_vpix )
{
   // displays the entire dib on the specified device context with the specified
   // destination upper left corner and lower right corner by stretching as needed
   // returns 0 for success, -1 for failure

   int dest_width, dest_height;
   CPalette pal, *oldpal;

   if( !m_initialized ) 
		return -1;

   dest_width = dest_max_hpix - dest_min_hpix +1;
   dest_height = dest_max_vpix - dest_min_vpix +1;
   
   // check for valid destination rectangle
   if( dest_width < 1 || dest_height < 1 ) 
		return -1;
   
	if (m_bits_pixel <= 8)
	{
		if (!m_pal_set)
		{
			m_pal_set = TRUE;
			create_palette(&m_pal);
		}
		oldpal = pDC->SelectPalette( &m_pal, TRUE);
	}

	pDC->RealizePalette();

   StretchDIBits( pDC->m_hDC, dest_min_hpix, dest_min_vpix, dest_width, dest_height,
                  0, 0, m_image_width, m_image_height, m_dib_data, m_dib_bitmapinfo,
                  DIB_RGB_COLORS, SRCCOPY );

	if (m_bits_pixel <= 8)
		pDC->SelectPalette( oldpal, TRUE);

   return 0;
}

// ****************************************************************
// ****************************************************************

int CMyDib::display_subimage( CDC *pDC, int dest_hpix, int dest_vpix,
                            int min_hpix, int min_vpix, int max_hpix, int max_vpix )
{
   // displays the specified dib subimage on the specified device context with
   // the specified destination upper left corner
   // returns 0 for success, -1 for failure

   if( !m_initialized ) 
		return -1;
   
   // check for valid source rectangle
   if( min_hpix < 0 || max_hpix < min_hpix || max_hpix >= m_image_width ||
       min_vpix < 0 || max_vpix < min_vpix || max_vpix >= m_image_height ) 
		 return -1;
   
   SetDIBitsToDevice( pDC->m_hDC, dest_hpix, dest_vpix, max_hpix-min_hpix+1, 
                      max_vpix-min_vpix+1, min_hpix, m_image_height-1-max_vpix, 0, m_image_height, m_dib_data,
                      m_dib_bitmapinfo, DIB_RGB_COLORS );

   return 0;
}

// ****************************************************************
// ****************************************************************

int CMyDib::stretch_display_subimage( CDC *pDC, int dest_min_hpix, int dest_min_vpix,
                                    int dest_max_hpix, int dest_max_vpix,
                                    int min_hpix, int min_vpix, int max_hpix, int max_vpix )
{
   // displays the specified dib subimage on the specified device context with the specified
   // destination upper left corner and lower right corner by stretching as needed
   // returns 0 for success, -1 for failure

   int dest_width, dest_height;
   int src_width, src_height;
   
   if( !m_initialized ) 
		return -1;

   // check for valid destination rectangle
   dest_width = dest_max_hpix - dest_min_hpix +1;
   dest_height = dest_max_vpix - dest_min_vpix +1;
   if( dest_width < 1 || dest_height < 1 ) 
		return -1;
   
   // check for valid source rectangle
   // check for valid source rectangle
   if( min_hpix < 0 || max_hpix < min_hpix || max_hpix >= m_image_width ||
       min_vpix < 0 || max_vpix < min_vpix || max_vpix >= m_image_height ) 
		 return -1;
   src_width = max_hpix - min_hpix +1;
   src_height = max_vpix - min_vpix +1;
   if( src_width < 1 || src_height < 1 ) 
		return -1;
   
   StretchDIBits( pDC->m_hDC, dest_min_hpix, dest_min_vpix, dest_width, dest_height,
                  min_hpix, m_image_height-1-max_vpix, src_width, src_height, m_dib_data, m_dib_bitmapinfo,
                  DIB_RGB_COLORS, SRCCOPY );

   return 0;
}

// ****************************************************************
// ****************************************************************

int CMyDib::set_pixel( int hpix, int vpix, unsigned char red, unsigned char green, 
               unsigned char blue )
{
   // sets the specified pixel in the dib to the specified color
   // returns 0 for success or -1 for failure

   int index;
   
   // check for valid conditions
   if( !m_initialized || hpix < 0 || hpix >= m_image_width ||
       vpix < 0 || vpix >= m_image_height ) 
		 return -1;

   index = 3*hpix + m_num_row_bytes * (m_image_height - 1 - vpix);

   m_dib_data[index] = blue;
   m_dib_data[index+1] = green;
   m_dib_data[index+2] = red;

   return 0;
}

// ****************************************************************
// ****************************************************************

int CMyDib::set_pixel_216( int pos, unsigned char color )
{
   // sets the specified pixel in the dib to the specified color
   // returns 0 for success or -1 for failure

   int index;
   
   index = m_num_row_bytes * m_image_height;
	if (pos < 0)
		return -1;
	if (pos >= index)
		return -1;
	if (m_dib_data == NULL)
		return -1;

   m_dib_data[pos] = color;

   return 0;
}

// ****************************************************************
// ****************************************************************

int CMyDib::set_pixel_1( int hpix, int vpix, unsigned char color )
{
	// sets the specified pixel in the dib to the specified color
	// returns 0 for success or -1 for failure

	int index, pos;
	BYTE mask, mask0, pix;

	// check for valid conditions
	if ( !m_initialized || hpix < 0 || hpix >= m_image_width || vpix < 0 || vpix >= m_image_height ) 
		return -1;

	index = hpix + m_num_row_bytes * (m_image_height - 1 - vpix);
	pos = index % 8;
	index /= 8;
	mask = 128 >> pos;
	mask0 = mask ^ 255;

	pix = m_dib_data[index];
	if (color == 0)
		pix = pix & mask0;
	else
		pix = pix | mask;

	m_dib_data[index] = pix;

	return 0;
}

// ****************************************************************
// ****************************************************************

BYTE CMyDib::get_pixel_1( int hpix, int vpix )
{
	// sets the specified pixel in the dib to the specified color
	// returns 0 for success or -1 for failure

	int index, pos;
	BYTE mask, mask0, pix;

	// check for valid conditions
	if ( !m_initialized || hpix < 0 || hpix >= m_image_width || vpix < 0 || vpix >= m_image_height ) 
		return -1;

	index = hpix + m_num_row_bytes * (m_image_height - 1 - vpix);
	pos = index % 8;
	index /= 8;
	mask = 128 >> pos;
	mask0 = mask ^ 255;

	pix = m_dib_data[index];

	if (pix & mask)
		return 1;

	return 0;
}

// ****************************************************************
// ****************************************************************

// get pixel with no interperlation

int CMyDib::get_pixel( int hpix, int vpix, unsigned char &red,
                     unsigned char &green, unsigned char &blue )
{
   // sets the specified pixel in the dib to the specified color
   // returns 0 for success or -1 for failure

   int index;
   
   // check for valid conditions
   if( !m_initialized || hpix < 0 || hpix >= m_image_width ||
       vpix < 0 || vpix >= m_image_height ) 
		 return -1;

   index = 3*hpix + m_num_row_bytes * (m_image_height - 1 - vpix);

   blue = m_dib_data[index];
   green = m_dib_data[index+1];
   red = m_dib_data[index+2];

   return 0;
}

// ****************************************************************
// ****************************************************************

int CMyDib::limit(int num, int min, int max)
{
	int val = num;

	if (val < min)
		val = min;
	if (val > max)
		val = max;
	return val;
}

// ****************************************************************
// ****************************************************************

// get pixel by interperlating the 4 nearest pixels

int CMyDib::get_pixel( double hpix, double vpix, unsigned char &red,
                     unsigned char &green, unsigned char &blue )
{
   // sets the specified pixel in the dib to the specified color
   // returns 0 for success or -1 for failure

   int index;
	unsigned char red1, grn1, blu1;
	unsigned char red2, grn2, blu2;
	unsigned char red3, grn3, blu3;
	unsigned char red4, grn4, blu4;
	int tx1, ty1, tx2, ty2, tx3, ty3, tx4, ty4;
	double xoff, yoff, dred, dgrn, dblu;
	double lower_val = 0.4;
	double upper_val = 0.6;
//   int ti;

   // check for valid conditions
   if( !m_initialized || hpix < 0.0 || hpix >= (double) m_image_width ||
       vpix < 0.0 || vpix >= (double) m_image_height ) 
		 return -1;

	xoff = hpix - (double) ((int) hpix);
	yoff = vpix - (double) ((int) vpix);

	// check for quick solution
	if ((xoff >= lower_val) && (xoff <= upper_val) &&
		 (yoff >= lower_val) && (yoff <= upper_val))
	{
		index = 3* (int) hpix + m_num_row_bytes * (m_image_height - 1 - (int) vpix);
		blue = m_dib_data[index];
		green = m_dib_data[index+1];
		red = m_dib_data[index+2];
		return 0;
	}


	tx1 = (int) hpix;
	ty1 = (int) vpix;
	tx2 = (int) hpix + 1;
	ty2 = (int) vpix;
	tx3 = (int) hpix;
	ty3 = (int) vpix + 1;
	tx4 = (int) hpix + 1;
	ty4 = (int) vpix + 1;

	// insure valid values
	tx1 = limit(tx1, 0, m_image_width-1);
	tx2 = limit(tx2, 0, m_image_width-1);
	tx3 = limit(tx3, 0, m_image_width-1);
	tx4 = limit(tx4, 0, m_image_width-1);
	ty1 = limit(ty1, 0, m_image_height-1);
	ty2 = limit(ty2, 0, m_image_height-1);
	ty3 = limit(ty3, 0, m_image_height-1);
	ty4 = limit(ty4, 0, m_image_height-1);


	// get the values for the four pixels
   index = 3*tx1 + m_num_row_bytes * (m_image_height - 1 - ty1);
   blu1 = m_dib_data[index];
   grn1 = m_dib_data[index+1];
   red1 = m_dib_data[index+2];

   index = 3*tx2 + m_num_row_bytes * (m_image_height - 1 - ty2);
   blu2 = m_dib_data[index];
   grn2 = m_dib_data[index+1];
   red2 = m_dib_data[index+2];

   index = 3*tx3 + m_num_row_bytes * (m_image_height - 1 - ty3);
   blu3 = m_dib_data[index];
   grn3 = m_dib_data[index+1];
   red3 = m_dib_data[index+2];

   index = 3*tx4 + m_num_row_bytes * (m_image_height - 1 - ty4);
   blu4 = m_dib_data[index];
   grn4 = m_dib_data[index+1];
   red4 = m_dib_data[index+2];

	dred = ((1.0 - xoff) * (double) red1) +  (xoff * (double) red2);
	dred += ((1.0 - yoff) * (double) red1) +  (yoff * (double) red3);
	red = (int) dred / 2;

	dgrn = ((1.0 - xoff) * (double) grn1) +  (xoff * (double) grn2);
	dgrn += ((1.0 - yoff) * (double) grn1) +  (yoff * (double) grn3);
	green= (int) dgrn / 2;

	dblu = ((1.0 - xoff) * (double) blu1) +  (xoff * (double) blu2);
	dblu += ((1.0 - yoff) * (double) blu1) +  (yoff * (double) blu3);
	blue = (int) dblu / 2;


   return 0;
}

// ****************************************************************
// ****************************************************************
 
/*
int CMyDib::get_pixel( double hpix, double vpix, unsigned char &red,
                     unsigned char &green, unsigned char &blue )
{
   // sets the specified pixel in the dib to the specified color
   // returns 0 for success or -1 for failure

   int index;
	unsigned char red1, grn1, blu1;
	unsigned char red2, grn2, blu2;
	unsigned char red3, grn3, blu3;
	unsigned char red4, grn4, blu4;
	int tx1, ty1, tx2, ty2, tx3, ty3, tx4, ty4;
	double xoff, yoff, dred, dgrn, dblu;
	double lower_val = 0.4;
	double upper_val = 0.6;
   int ti;

   // check for valid conditions
   if( !m_initialized || hpix < 0.0 || hpix >= (double) m_image_width ||
       vpix < 0.0 || vpix >= (double) m_image_height ) 
		 return -1;

	xoff = hpix - (double) ((int) hpix);
	yoff = vpix - (double) ((int) vpix);

	// check for quick solution
	if ((xoff >= lower_val) && (xoff <= upper_val) &&
		 (yoff >= lower_val) && (yoff <= upper_val))
	{
		index = 3* (int) hpix + m_num_row_bytes * (m_image_height - 1 - (int) vpix);
		blue = m_dib_data[index];
		green = m_dib_data[index+1];
		red = m_dib_data[index+2];
		return 0;
	}


	tx1 = (int) hpix;
	ty1 = (int) vpix;
	tx2 = (int) hpix;
	ty2 = (int) vpix;
	tx3 = (int) hpix;
	ty3 = (int) vpix;
	tx4 = (int) hpix;
	ty4 = (int) vpix;
	if (xoff > upper_val)
	{
		tx2 = (int) hpix + 1;
		tx4 = (int) hpix + 1;
	}
	if (xoff < lower_val)
	{
		tx2 = (int) hpix - 1;
		tx4 = (int) hpix - 1;
	}
	if (yoff > upper_val)
	{
		ty2 = (int) vpix + 1;
		ty4 = (int) vpix + 1;
	}
	if (yoff < lower_val)
	{
		ty2 = (int) vpix - 1;
		ty4 = (int) vpix - 1;
	}

	// insure valid values
	tx1 = limit(tx1, 0, m_image_width-1);
	tx2 = limit(tx2, 0, m_image_width-1);
	tx3 = limit(tx3, 0, m_image_width-1);
	tx4 = limit(tx4, 0, m_image_width-1);
	ty1 = limit(ty1, 0, m_image_height-1);
	ty2 = limit(ty2, 0, m_image_height-1);
	ty3 = limit(ty3, 0, m_image_height-1);
	ty4 = limit(ty4, 0, m_image_height-1);

	if (xoff < lower_val)
	{
		ti = tx2;
		tx2 = tx1;
		tx1 = ti;
		ti = tx3;
		tx3 = tx4;
		tx4 = ti;
		xoff = 1.0 - xoff;
	}
	if (yoff < lower_val)
	{
		ti = ty2;
		ty2 = ty1;
		ty1 = ti;
		ti = ty3;
		ty3 = ty4;
		ty4 = ti;
		yoff = 1.0 - yoff;
	}

	// get the values for the four pixels
   index = 3*tx1 + m_num_row_bytes * (m_image_height - 1 - ty1);
   blu1 = m_dib_data[index];
   grn1 = m_dib_data[index+1];
   red1 = m_dib_data[index+2];

   index = 3*tx2 + m_num_row_bytes * (m_image_height - 1 - ty2);
   blu2 = m_dib_data[index];
   grn2 = m_dib_data[index+1];
   red2 = m_dib_data[index+2];

   index = 3*tx3 + m_num_row_bytes * (m_image_height - 1 - ty3);
   blu3 = m_dib_data[index];
   grn3 = m_dib_data[index+1];
   red3 = m_dib_data[index+2];

   index = 3*tx4 + m_num_row_bytes * (m_image_height - 1 - ty4);
   blu4 = m_dib_data[index];
   grn4 = m_dib_data[index+1];
   red4 = m_dib_data[index+2];

	dred = ((1.0 - xoff) * (double) red1) +  (xoff * (double) red2);
	dred += ((1.0 - yoff) * (double) red1) +  (yoff * (double) red3);
	red = (int) dred / 2;

	dgrn = ((1.0 - xoff) * (double) grn1) +  (xoff * (double) grn2);
	dgrn += ((1.0 - yoff) * (double) grn1) +  (yoff * (double) grn3);
	green= (int) dgrn / 2;

	dblu = ((1.0 - xoff) * (double) blu1) +  (xoff * (double) blu2);
	dblu += ((1.0 - yoff) * (double) blu1) +  (yoff * (double) blu3);
	blue = (int) dblu / 2;


   return 0;
}
*/

// ****************************************************************
// ****************************************************************

BYTE CMyDib::get_pixel_216( int hpix, int vpix )
{
   int index;
   int color;

   // check for valid conditions
   if( !m_initialized || hpix < 0 || hpix >= m_image_width ||
       vpix < 0 || vpix >= m_image_height ) 
		 return 0;

   index = hpix + m_num_row_bytes * (m_image_height - 1 - vpix);

   color = m_dib_data[index];
	return (BYTE) color;
}


// ********************************************************************
// ****************************************************************

int CMyDib::get_static_pixel( int hpix, int vpix, unsigned char &red,
                     unsigned char &green, unsigned char &blue )
{
   // sets the specified pixel in the dib to the specified color
   // returns 0 for success or -1 for failure

   int index;
   
   // check for valid conditions
   if ( hpix < 0 || hpix >= m_static_image_width ||
       vpix < 0 || vpix >= m_static_image_height ) 
		 return -1;

   index = 3*hpix + m_static_num_row_bytes * (m_static_image_height - 1 - vpix);

   blue = m_static_dib_data[index];
   green = m_static_dib_data[index+1];
   red = m_static_dib_data[index+2];

   return 0;
}

// ********************************************************************
// ****************************************************************

void CMyDib::set_rgb_image( unsigned char *rgb_array )
{

   // sets the entire image from the source array, which should contain
   // rgb bytes for all of the pixels in the image, without row padding,
   // starting at the upper left corner and ending at the lower right 
   // corner. Note that the destination dib is arranged bgr and can 
   // have padded rows.

   int hpix, vpix, source_index, dest_index;
   unsigned char red, green, blue;

   for( vpix = 0, source_index = 0; vpix < m_image_height; vpix++ )
   {
      for( hpix = 0; hpix < m_image_width; hpix++, source_index += 3 )
      {
         red = rgb_array[source_index];
         green = rgb_array[source_index+1];
         blue = rgb_array[source_index+2];
         dest_index = 3*hpix + m_num_row_bytes * (m_image_height - 1 - vpix);
         m_dib_data[dest_index] = blue;
         m_dib_data[dest_index+1] = green;
         m_dib_data[dest_index+2] = red;
      }
   }
}

// ****************************************************************
// ****************************************************************

void CMyDib::set_rgb_image( BYTE *redbuf, BYTE *grnbuf, BYTE *blubuf )
{

   // sets the entire image from the source array, which should contain
   // rgb bytes for all of the pixels in the image, without row padding,
   // starting at the upper left corner and ending at the lower right 
   // corner. Note that the destination dib is arranged bgr and can 
   // have padded rows.

   int hpix, vpix, source_index, dest_index;
   unsigned char red, grn, blu;

   source_index = 0;

   for( vpix = 0; vpix < m_image_height; vpix++ )
   {
      for( hpix = 0; hpix < m_image_width; hpix++ )
      {
         red = redbuf[source_index];
         grn = grnbuf[source_index];
         blu = blubuf[source_index];
         dest_index = 3*hpix + m_num_row_bytes * (m_image_height - 1 - vpix);
         m_dib_data[dest_index] = blu;
         m_dib_data[dest_index+1] = grn;
         m_dib_data[dest_index+2] = red;
		 source_index++;
      }
   }
}

// ****************************************************************
// ********************************************************************

void CMyDib::copy_to_clipboard( )
{
   HGLOBAL HGlobalMem;
   unsigned char *ptr;

   HGlobalMem = ::GlobalAlloc( GMEM_MOVEABLE, sizeof( BITMAPINFO ) - 4 + 
                               m_image_height * m_num_row_bytes );

   ptr = (unsigned char*)::GlobalLock( HGlobalMem );

	CopyMemory( ptr, &m_dib_bitmapinfo, sizeof( BITMAPINFO ) - 4 );
   CopyMemory( &ptr[sizeof( BITMAPINFO )-4],
               m_dib_data, m_image_height * m_num_row_bytes );
   
   ::GlobalUnlock( HGlobalMem );
   
   OpenClipboard( NULL );

   EmptyClipboard( );

   SetClipboardData( CF_DIB, HGlobalMem );

   CloseClipboard( );
}

// ********************************************************************
// ********************************************************************

int CMyDib::create_palette(CPalette* cpal)
{
	BOOL rslt;
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	int i;                   // loop index

	if (m_dib_bitmapinfo == NULL)
		return FAILURE;

	lpPal = (LPLOGPALETTE) malloc(sizeof(LOGPALETTE) + 1024);

	// set version and number of palette entries 
	lpPal->palVersion = PALVERSION;
	lpPal->palNumEntries = 256;
	for (i = 0; i < 256; i++)
	{
		lpPal->palPalEntry[i].peRed = m_dib_bitmapinfo->bmiColors[i].rgbRed;
		lpPal->palPalEntry[i].peGreen = m_dib_bitmapinfo->bmiColors[i].rgbGreen;
		lpPal->palPalEntry[i].peBlue = m_dib_bitmapinfo->bmiColors[i].rgbBlue;
		lpPal->palPalEntry[i].peFlags = 0;
	}

	rslt = cpal->CreatePalette(lpPal);

	free(lpPal);

	if (rslt)
		return SUCCESS;
	else
		return FAILURE;
}

// ********************************************************************
// ********************************************************************

void CMyDib::create_null_palette(CPalette* cpal)
{
	BOOL rslt;
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	int i;                   // loop index

	lpPal = (LPLOGPALETTE) malloc(sizeof(LOGPALETTE) + 1024);

	// set version and number of palette entries 
	lpPal->palVersion = PALVERSION;
	lpPal->palNumEntries = 1;
	for (i = 0; i < 1; i++)
	{
		lpPal->palPalEntry[i].peRed = 0;
		lpPal->palPalEntry[i].peGreen = 0;
		lpPal->palPalEntry[i].peBlue = 0;
		lpPal->palPalEntry[i].peFlags = 0;
	}

	rslt = cpal->CreatePalette(lpPal);

	free(lpPal);
}
// end of create_null_palette

// ********************************************************************
// ********************************************************************

WORD WINAPI CMyDib::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}


// ********************************************************************
// ********************************************************************

WORD WINAPI CMyDib::PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

// ********************************************************************
// ********************************************************************

LPSTR WINAPI CMyDib::FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

// ********************************************************************
// ********************************************************************

DWORD WINAPI CMyDib::DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether Win 3.0 and old) */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB width if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its width */
		return (DWORD)lpbmc->bcWidth;
}

// ********************************************************************
// ********************************************************************

DWORD WINAPI CMyDib::DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biHeight;
	else  /* it is an other-style DIB, so return its height */
		return (DWORD)lpbmc->bcHeight;
}

// ********************************************************************
// ********************************************************************

#define HDIB HANDLE

// ********************************************************************
// ********************************************************************

HGLOBAL WINAPI CMyDib::CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

// ********************************************************************
// ********************************************************************
void CMyDib::decode_color16(unsigned short color, BYTE *red, BYTE *grn, BYTE *blu)
{
	unsigned short	clr;

	clr = color;
	clr = (unsigned short) (clr & 0x1f);
	clr = (unsigned short) (clr << 3);
	*blu = (BYTE) clr;

	clr = (unsigned short) (color >> 6);
	clr = (unsigned short) (clr & 0x1f);
	clr = (unsigned short) (clr << 3);
	*grn = (BYTE) clr;

	clr = (unsigned short) (color >> 11);
	clr = (unsigned short) (clr << 3);
	*red = (BYTE) clr;
}

// ********************************************************************
// ********************************************************************

void CMyDib::decode_color32(unsigned int color, BYTE *red, BYTE *grn, BYTE *blu)
{
	unsigned int	clr;

	clr = (unsigned int) (color & 0x0000ff0000);
	clr = (unsigned int) (clr >> 16);
	*red = (BYTE) clr;

//	clr = color;
//	clr = (unsigned int) (clr & 0xff000000);
//	clr = (unsigned int) (clr >> 24);
//	*red = (BYTE) clr;

	clr = (unsigned int) (color & 0xff00);
	clr = (unsigned int) (clr >> 8);
	*grn = (BYTE) clr;

	clr = (unsigned int) (color & 0xff);
	*blu = (BYTE) clr;
}

// ********************************************************************
// ********************************************************************

void CMyDib::copy_from_clipboard( )
{
	HDIB hNewDIB = NULL;
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
//	LPBITMAPINFO lpbminfo;
	int width, height, palsize;
	int byte_width;

	if (OpenClipboard( NULL ))
	{
		CWaitCursor wait;

		hNewDIB = (HDIB) CopyHandle(::GetClipboardData(CF_DIB));

		if (hNewDIB != NULL)
		{
			LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) hNewDIB);
			lpbmi = (LPBITMAPINFOHEADER) lpDIB;
			byte_width = WIDTHBYTES((lpbmi->biWidth)*((DWORD)lpbmi->biBitCount));
//			palsize = lpbmi->biClrUsed * 4;
			palsize = 1024;
			width = lpbmi->biWidth;
			height = lpbmi->biHeight;
			m_image_width = width;
			m_image_height = height;
			initialize( m_image_width, m_image_height, 0, 0, 0 );
			// clear dib to black
			erase( 0, 0, 0 );

			if (lpbmi->biBitCount == 32)
			{
				int num_row_bytes,inpos, x, y, vpix;
				unsigned long inval;			 
				BYTE red, grn, blu;
				LPBITMAPINFO lpbi;
				BYTE *ptr;	

				lpbi = (LPBITMAPINFO) lpDIB;
				ptr = (BYTE*) lpDIB + lpbmi->biSize;
				num_row_bytes = 4 * m_image_width;
				for ( y = 0; y < height; y++ )
				{
					for ( x = 0; x < width; x++ )
					{
						vpix = m_image_height - y - 1;
						inpos = vpix * num_row_bytes +x * 4;
						memcpy(&inval, (void*) &ptr[inpos], 4);
						if ((y==height/6) && (x == width/2))
							red = 0;
						if ((y==height/2) && (x == width/2))
							red = 0;
						if ((y==(5*height)/6) && (x == width/2))
							red = 0;
						decode_color32(inval, &red, &grn, &blu);
						set_pixel( x, y, red, grn, blu);
					}
				}
			}
			else if (lpbmi->biBitCount == 24)
			{
				CopyMemory( (void*) m_dib_data, (void*) &lpDIB[sizeof( BITMAPINFO )-4],
								m_dib_size );
			}
			else if (lpbmi->biBitCount == 16)
			{
				int i, num_row_bytes,inpos, x, y, vpix;
				unsigned short inval;			 
				BYTE red, grn, blu;
				LPBITMAPINFO lpbi;
				BYTE *ptr;	

				lpbi = (LPBITMAPINFO) lpDIB;
				ptr = (BYTE*) lpDIB + lpbmi->biSize;
				num_row_bytes = 2 * m_image_width;
				i = num_row_bytes / 4;
				if ( 4*i != num_row_bytes ) 
					num_row_bytes = 4 * (i + 1);
				for ( y = 0; y < height; y++ )
				{
					for ( x = 0; x < width; x++ )
					{
						vpix = m_image_height - y - 1;
						inpos = vpix * num_row_bytes +x * 2;
						memcpy(&inval, (void*) &ptr[inpos], 2);
						decode_color16(inval, &red, &grn, &blu);
						set_pixel( x, y, red, grn, blu);
					}
				}
			}
			else if (lpbmi->biBitCount == 8)
			{
				// copy the paletted data into our 24 bit DIB
				BYTE clut[768];
				int k, hpix, vpix, pos, val, y;
				BYTE red, grn, blu;
				LPBITMAPINFO lpbi;
				BYTE *ptr;	

				lpbi = (LPBITMAPINFO) lpDIB;

				for (k=0; k<256; k++)
				{
					clut[(k*3)+0] = ((LPBITMAPINFO)lpbi)->bmiColors[k].rgbRed;
					clut[(k*3)+1] = ((LPBITMAPINFO)lpbi)->bmiColors[k].rgbGreen;
					clut[(k*3)+2] = ((LPBITMAPINFO)lpbi)->bmiColors[k].rgbBlue;
				}

				// find the bits
				ptr = (BYTE*) lpDIB + lpbmi->biSize + palsize;
//				ptr = (BYTE*) ::FindDIBBits(lpDIB);

				// copy subimage to dib
				for ( y = 0; y < height; y++ )
				{
					for ( hpix = 0; hpix < width; hpix++ )
					{
						vpix = m_image_height - y - 1;
						pos = (vpix * byte_width) + hpix;
						val = ptr[pos];
						red = clut[(val * 3) + 0];
						grn = clut[(val * 3) + 1];
						blu = clut[(val * 3) + 2];
						set_pixel( hpix, y, red, grn, blu);
					}
				}

			}
			else
				AfxMessageBox("Unsupported bitmap type on clipboard");

		}
		CloseClipboard();
		::GlobalUnlock(hNewDIB);
		::GlobalFree(hNewDIB);
	}
}
// end of copy_from_clipboard

// ********************************************************************
// ********************************************************************

// save image as BMP file

int CMyDib::number_unique_colors()
{
	int hist[256];
	int k, hpix, vpix, cnt;
	BYTE color;

	if (m_dib_data == NULL)
		return 0;

	if (m_bits_pixel > 8)
		return 0;

	// clear the hist sum
	for (k=0; k<256; k++)
		hist[k] = 0;

	// calculate histogram
	for (vpix = 0; vpix < m_image_height; vpix++)
	{
		for (hpix = 0; hpix < m_image_width; hpix++)
		{
			color = get_pixel_216(hpix, vpix);
			hist[color]++;
		}
	}

	// find the number of non-zero entris
	cnt = 0;
	for (k=0; k<256; k++)
	{
		if (hist[k] > 0)
			cnt++;
	}
	return cnt;
}

// ********************************************************************
// ********************************************************************

int CMyDib::load_bmp(VARIANT *dib_variant)
{
	COleSafeArray oledib;
	BITMAPINFOHEADER *bminfo;
	RGBQUAD *pallet;
	BYTE *pBits;

	oledib.Attach(*dib_variant);

	if (oledib.GetDim() != 1) // dib is a one dimentional array
	{
		ASSERT(0);
		return FAILURE;
	}

	// read the dib
	oledib.AccessData( (void**)&bminfo );
	if (bminfo == NULL)
	{
		AfxMessageBox("Could not access safe array");
		return FAILURE;
	}
	if (bminfo->biBitCount == 8)
	{
		pallet = (RGBQUAD *)(((BYTE *)bminfo) + sizeof(BITMAPINFOHEADER));
		if (bminfo->biClrUsed == 0)
			pBits = ((BYTE *)pallet) +  256*sizeof(RGBQUAD);
		else
			pBits = ((BYTE *)pallet) +  bminfo->biClrUsed*sizeof(RGBQUAD);  
	}
	else
	if (bminfo->biBitCount == 24)
	{
		pallet = (RGBQUAD *)(((BYTE *)bminfo) + sizeof(BITMAPINFOHEADER));
		pBits = ((BYTE *)pallet) +  bminfo->biClrUsed*sizeof(RGBQUAD);  
	}
	else
	if (bminfo->biBitCount ==16)
	{
		if(bminfo->biCompression == BI_BITFIELDS)
		{
			pallet = (RGBQUAD *)(((BYTE *)bminfo) + sizeof(BITMAPINFOHEADER));
			pBits = ((BYTE *)pallet) +  3*sizeof(RGBQUAD);  
		}
		else
		{
			pallet = (RGBQUAD *)(((BYTE *)bminfo) + sizeof(BITMAPINFOHEADER));
			pBits = ((BYTE *)pallet) +  bminfo->biClrUsed*sizeof(RGBQUAD); 
		}
	}
	else
	{
		AfxMessageBox("Error getting bitmap from FalconView, unsupported color depth");
		return FAILURE;
	}


	load_bmp(*bminfo, pallet, pBits);
	oledib.UnaccessData();

	return SUCCESS;
}
// end load_bmp

// ********************************************************************
// ********************************************************************

int CMyDib::load_bmp(const BITMAPINFOHEADER &bminfo, RGBQUAD *pallet, BYTE *pbits)
{
	int i, j, k, v, width, height, rslt;
	int scanwidth;
	int border = 0;

	m_image_width = bminfo.biWidth;
	m_image_height = bminfo.biHeight;
	width = m_image_width;
	height = m_image_height;

	rslt = initialize( width, height, 0, 0, 0);

	if (bminfo.biBitCount == 1) // monocrome handle special
	{
		for (j=0;j<height;j++)
		{
			for (i=0;i<(width+(2*border))/8;i++)
			{
				for(k=0;k<8;k++)
				{
					if (*pbits & (1<<(7-k)))
						set_pixel(i, j, 255, 255, 255);
					else
						set_pixel(i, j, 0, 0, 0);
				}
				pbits++;

			}
		}
	}
	else if (bminfo.biBitCount == 24)
	{
		if (bminfo.biCompression != BI_RGB)
		{
			TRACE ("ERROR CMyDib::load_bmp() - Unsupported bitmap format biCompression != BI_RGB\n");
			return -1;
		}
		scanwidth = width*3*sizeof(BYTE);
		scanwidth = scanwidth + scanwidth%4; // make sure the with is on a 4 byte boundry
		for (int i = 0;i<height;i++)
		{
			v = height - i;
			memcpy(m_dib_data+(scanwidth*v),pbits+(scanwidth*v),scanwidth);
		}
	}
	else if (bminfo.biBitCount == 16)
	{
		if (bminfo.biCompression != BI_RGB && bminfo.biCompression !=BI_BITFIELDS)
		{
			TRACE ("ERROR CMyDib::load_bmp() - Unsupported bitmap format biCompression != BI_RGB\n");
			return -1;
		}
		//take the 16bit per pixel and make a 24 bit block of data out of it
		short *pshortpixels = (short *)pbits;
		int rshift, gshift, bshift,rmask,gmask,bmask;  // how to extract the rgb components from the word
		int rbits,gbits,bbits;
		if (bminfo.biCompression == BI_RGB)
		{	// all colors are 5 bits RGB red is most significant 
			rshift = 10;
			rmask = 0x7c00;
			gshift = 5;
			gmask = 0x03e0;
			bshift = 0;
			bmask = 0x001F;
			rbits = 5;
			gbits = 5;
			bbits = 5;
		}
		else
		{ 
			int tmpmask;
			ASSERT(bminfo.biCompression == BI_BITFIELDS);
			DWORD *pdw_pallet = (DWORD*)pallet;
			rmask = (int)pdw_pallet[0];
			gmask = (int)pdw_pallet[1];
			bmask = (int)pdw_pallet[2];

			tmpmask = rmask;
			for(rshift=0;!(tmpmask&0x01)&&tmpmask;tmpmask>>=1,rshift++);	// how far do we shift to get the mask into the lsb
			tmpmask = gmask;
			for(gshift=0;!(tmpmask&0x01)&&tmpmask;tmpmask>>=1,gshift++);	// how far do we shift to get the mask into the lsb
			tmpmask = bmask;
			for(bshift=0;!(tmpmask&0x01)&&tmpmask;tmpmask>>=1,bshift++);	// how far do we shift to get the mask into the lsb

			// how many red, blue or green bits in the mask
			for(rbits=0, tmpmask = rmask>>rshift;tmpmask;rbits++)
			{
				tmpmask>>=1;
			}
			for(gbits=0, tmpmask = gmask>>gshift;tmpmask;gbits++)
			{
				tmpmask>>=1;
			}
			for(bbits=0, tmpmask = bmask>>bshift;tmpmask;bbits++)
			{
				tmpmask>>=1;
			}
		}
		// make a rgb triple (24bits) out of each 16 bit color by
		// masking the non red bits then shift the bits into the 
		// a byte so a 
		scanwidth = width*3*sizeof(BYTE);
		scanwidth = scanwidth + scanwidth%4; // make sure the with is on a 4 byte boundry
		m_dib_data = (BYTE*)malloc(scanwidth*height);
		for(j=0;j<height;j++)
		{
			for(i=0;i<width;i++)
			{
				set_pixel(i, j, ((pshortpixels[j*width+i]&rmask)>>rshift)<<(8-rbits), 
								((pshortpixels[j*width+i]&gmask)>>gshift)<<(8-gbits),
								((pshortpixels[j*width+i]&bmask)>>bshift)<<(8-bbits));
			}
		}
	}
       
   else if(bminfo.biBitCount == 8)
   {

	   RGBQUAD entry;
	   
	   {
		   
		   scanwidth = width*3*sizeof(BYTE);
		   scanwidth = scanwidth + scanwidth%4; // make sure the with is on a 4 byte boundry
		   m_dib_data = (BYTE*)malloc(scanwidth*height);
		   for(j=0;j<height;j++)
			   for(i=0;i<width;i++)
			   {
				   entry = pallet[pbits[j*width+i]];
				   set_pixel(i, height - j, entry.rgbRed, entry.rgbGreen, entry.rgbBlue);
			   }
	   }
   }
   else
   {
	   TRACE ("ERROR cimage::load_bmp() - Unsupported bitmap format BitCount = %d\n",bminfo.biBitCount);
	   return -1;
	   
   }
   
   return 0;
}
// load_bmp

// ********************************************************************
// ********************************************************************

// save image as BMP file

BOOL CMyDib::save_bmp(CString filename, CString &error_txt)
{
	BITMAPFILEHEADER bmfHdr; // Header for Bitmap file
	LPBITMAPINFOHEADER lpBI;   // Pointer to DIB info structure
	LPBITMAPINFO lpinfo;
	DWORD dwDIBSize;
	CString ext;
	CFile file;

	CWaitCursor wait;

	if (m_dib_data == NULL)
	{
		error_txt = "There is not currently loaded bitmap";
		return FALSE;
	}

	// extract the extension
	ext = filename.Right(3);
	ext.MakeUpper();
	if (ext.Compare("BMP"))
	{
		error_txt.Format("Invalid extention in filename -- %s", filename);
		return FALSE;
	}

	if (!file.Open(filename, CFile::modeCreate | CFile::modeWrite)) 
	{							
		error_txt = "Unable to create BMP file -- ";
		error_txt += filename;
		return FALSE;
	}
	
	// Get a pointer to the DIB memory, the first of which contains
	// a BITMAPINFO structure
	lpBI = (LPBITMAPINFOHEADER) m_dib_bitmapinfo;
	lpinfo = (LPBITMAPINFO) m_dib_bitmapinfo;

	if (lpBI == NULL)
		return FALSE;

//	if (!IS_WIN30_DIB(lpBI))
//	{
//		return FALSE;       // It's an other-style DIB (save not supported)
//	}

	// Fill in file type (first 2 bytes must be "BM" for a bitmap) 
	memmove((void*) &(bmfHdr.bfType), "BM", 2);

	// Calculating the size of the DIB is a bit tricky (if we want to
	// do it right).  The easiest way to do this is to call GlobalSize()
	// on our global handle, but since the size of our global memory may have
	// been padded a few bytes, we may end up writing out a few too
	// many bytes to the file (which may cause problems with some apps).
	//
	// So, instead let's calculate the size manually (if we can)
	//
	// First, find size of header plus size of color table.  Since the
	// first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
	// the size of the structure, let's use this.

	dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);  // Partial Calculation

	// Now calculate the size of the image

	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// It's an RLE bitmap, we can't calculate size, so trust the
		// biSizeImage field

		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only

		// It's not RLE, so size is Width (DWORD aligned) * Height

		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;

		dwDIBSize += dwBmBitsSize;

		// Now, since we have calculated the correct size, why don't we
		// fill in the biSizeImage field (this will fix any .BMP files which
		// have this field incorrect).

//		lpBI->biSizeImage = dwBmBitsSize;

		lpBI->biSizeImage = m_num_row_bytes * m_image_height;


	}

	lpBI->biSize = sizeof(BITMAPINFOHEADER);
	lpBI->biPlanes = 1;

	lpBI->biBitCount = (unsigned short) m_bits_pixel;
	if (m_bits_pixel == 24)
	{
		lpBI->biClrUsed = 0;
		lpBI->biClrImportant = 0;
	}
	else if (m_bits_pixel == 24)
	{
		lpBI->biClrUsed = 256;
		lpBI->biClrImportant = 256;
	}
	else
	{
		lpBI->biClrUsed = 2;
		lpBI->biClrImportant = 2;
	}

//	m_image_width = lpbi->biWidth;
//	m_image_height = lpbi->biHeight;

	// Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)
	bmfHdr.bfSize = lpBI->biSizeImage + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	int palsize;
	if (lpBI->biBitCount == 24)
		palsize = 0;
	else
		palsize = 1024;

	// Now, calculate the offset the actual bitmap bits will be in
	// the file -- It's the Bitmap file header plus the DIB header,
	// plus the size of the color table.
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize	+ palsize;

	TRY
	{
		// Write the file header
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
		// Write the DIB header 
		file.Write((LPSTR)lpBI, sizeof(BITMAPINFOHEADER));  
		// write the color table
		if (lpBI->biBitCount != 24)
			file.Write( (LPSTR) lpinfo->bmiColors, 1024);
		// Write the bits
		file.Write((LPSTR)m_dib_data, m_image_height * m_num_row_bytes);
	}
	CATCH (CFileException, e)
	{
		THROW_LAST();
	}
	END_CATCH

	file.Close();

	return TRUE;
}
// end of save_bmp

// ********************************************************************
// ****************************************************************

int CMyDib::initialize_alpha( int width, int height, BYTE red, BYTE green, BYTE blue )
{
	// initialize dib with specified height, width, and color
	// returns 0 for success, -1 for failure

	int i;
	// first, clear the dib
	clear( );

	// if specified width or height is < 1, return failure
	if ( width < 1 || height < 1 ) 
		return -1;

	m_bits_pixel = 32;
	m_static_bits_pixel = 32;

	m_image_width = width;
	m_image_height = height;
	m_num_pixels = m_image_width * m_image_height;

	// determine number of bytes per row, must be padded to even number of 4-byte words
	m_num_row_bytes = 4 * m_image_width;
	i = m_num_row_bytes / 4;
	if ( 4*i != m_num_row_bytes ) 
		m_num_row_bytes = 4 * (i + 1);
	m_num_extra_row_bytes = m_num_row_bytes - 4 * m_image_width;

	// setup bitmap info structure
	m_dib_bitmapinfo = (BITMAPINFO*) malloc(sizeof(BITMAPINFO));
	m_dib_bitmapinfo->bmiHeader.biSize = 40;                  // size of BITMAPINFO structre
	m_dib_bitmapinfo->bmiHeader.biWidth = m_image_width;      // width of image
	m_dib_bitmapinfo->bmiHeader.biHeight = m_image_height;    // height of image
	m_dib_bitmapinfo->bmiHeader.biPlanes = 1;                 // number of planes - must be 1
	m_dib_bitmapinfo->bmiHeader.biBitCount = 32;              // bits per pixel
	m_dib_bitmapinfo->bmiHeader.biCompression = BI_RGB;       // uncompressed RGB triplets
	m_dib_bitmapinfo->bmiHeader.biSizeImage = 0;              // zero for BI_RGB compression
	m_dib_bitmapinfo->bmiHeader.biXPelsPerMeter = 1000;       // horizontal pixels per meter
	m_dib_bitmapinfo->bmiHeader.biYPelsPerMeter = 1000;       // vertical pixels per meter
	m_dib_bitmapinfo->bmiHeader.biClrUsed = 0;                // number of colors used - zero for 32-bit
	m_dib_bitmapinfo->bmiHeader.biClrImportant = 0;           // number of important colors - zero for all

	m_dib_bitmapinfo->bmiColors[0].rgbBlue = 0;               // color table is null for 32-bit
	m_dib_bitmapinfo->bmiColors[0].rgbGreen = 0;
	m_dib_bitmapinfo->bmiColors[0].rgbRed = 0;
	m_dib_bitmapinfo->bmiColors[0].rgbReserved = 0;

	m_static_dib_bitmapinfo = m_dib_bitmapinfo;
	m_static_image_width = m_image_width;
	m_static_image_height = m_image_height;
	m_static_num_row_bytes = m_num_row_bytes;

	// allocate memory for dib data
	m_dib_size = m_image_height * m_num_row_bytes;
	m_dib_data = (unsigned char*) malloc(m_dib_size);
	if ( m_dib_data == NULL )
	{
		clear( );
		return -1;
	}
	m_static_dib_data = m_dib_data;

	m_initialized = TRUE;
   
	// erase to specified color
//	erase( red, green, blue );

	return 0;
}
// end of initialize_alpha

// ****************************************************************
// ****************************************************************

void CMyDib::set_rgb_image_alpha( unsigned char *rgb_array )
{

   // sets the entire image from the source array, which should contain
   // rgb bytes for all of the pixels in the image, without row padding,
   // starting at the upper left corner and ending at the lower right 
   // corner. Note that the destination dib is arranged bgr and can 
   // have padded rows.

   int hpix, vpix, source_index, dest_index;
   unsigned char red, green, blue;
   BYTE code;

   for( vpix = 0, source_index = 0; vpix < m_image_height; vpix++ )
   {
      for( hpix = 0; hpix < m_image_width; hpix++, source_index += 3 )
      {
         red = rgb_array[source_index];
         green = rgb_array[source_index+1];
         blue = rgb_array[source_index+2];
         dest_index = (4 * hpix) + (m_num_row_bytes * vpix);
		 code = 255;
		 if ((red == 0) && (green == 0) && (blue == 0))
			 code = 0;
         m_dib_data[dest_index] = blue;
         m_dib_data[dest_index+1] = green;
         m_dib_data[dest_index+2] = red;
         m_dib_data[dest_index+3] = code;
      }
   }
}

// ****************************************************************
// ****************************************************************

void CMyDib::set_rgb_image_alpha( BYTE *red_buffer, BYTE *grn_buffer, BYTE *blu_buffer )
{

   // sets the entire image from the source array, which should contain
   // rgb bytes for all of the pixels in the image, without row padding,
   // starting at the upper left corner and ending at the lower right 
   // corner. Note that the destination dib is arranged bgr and can 
   // have padded rows.

   int hpix, vpix, source_index, dest_index;
   unsigned char red, green, blue;
   BYTE code;

   source_index = 0;
   for( vpix = 0; vpix < m_image_height; vpix++ )
   {
      for( hpix = 0; hpix < m_image_width; hpix++ )
      {
         red = red_buffer[source_index];
         green = grn_buffer[source_index];
         blue = blu_buffer[source_index];
         dest_index = (4 * hpix) + (m_num_row_bytes * vpix);
		 code = 255;
		 if ((red == 0) && (green == 0) && (blue == 0))
			 code = 0;
         m_dib_data[dest_index] = blue;
         m_dib_data[dest_index+1] = green;
         m_dib_data[dest_index+2] = red;
         m_dib_data[dest_index+3] = code;
		 source_index++;
      }
   }
}

// ****************************************************************
// ****************************************************************


void CMyDib::display_alpha(CDC * pDC, int dest_hpix, int dest_vpix, int transcode)
{
	HBITMAP bmp;
	HBITMAP hBmOld;
	HDC hdc, memdc;
	BLENDFUNCTION bldfunc;

	bldfunc.BlendOp = AC_SRC_OVER;
	bldfunc.BlendFlags = 0;
	bldfunc.SourceConstantAlpha = transcode;
	bldfunc.AlphaFormat = 1;    

	hdc = pDC->GetSafeHdc();

	bmp = CreateBitmap( m_image_width, m_image_height, 1, 32, m_dib_data );
	if ( bmp == NULL )
		return;

	memdc = CreateCompatibleDC( hdc );
	hBmOld = (HBITMAP) SelectObject( memdc, bmp );

	AlphaBlend(hdc, dest_hpix, dest_vpix, m_image_width, m_image_height, memdc, 0, 0, 
				m_image_width, m_image_height, bldfunc);
	SelectObject( memdc, hBmOld );

	DeleteDC(memdc);
	DeleteObject(bmp);
}
// end of display_alphblend


// ****************************************************************
