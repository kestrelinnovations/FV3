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

// dib.h

#ifndef DIB_H
#define DIB_H

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define PALVERSION   0x300
// Handle to a DIB 
//#ifndef HDIB
//DECLARE_HANDLE(HDIB);
//#endif

// ********************************************************************
// ********************************************************************

typedef struct
{
   int num_colors;
   BOOL small_enough;
   BYTE min_red;
   BYTE max_red;
   BYTE min_green;
   BYTE max_green;
   BYTE min_blue;
   BYTE max_blue;
} rgb_box_t;

typedef struct
{
	BYTE red;
	BYTE green;
	BYTE blue;
} rgb_t;

// ********************************************************************
// ********************************************************************

class CMyDib
{
public:

   BOOL m_initialized;
   int m_image_width, m_image_height, m_num_pixels;
   int m_num_row_bytes, m_num_extra_row_bytes;
   BITMAPINFO *m_dib_bitmapinfo;
   unsigned char *m_dib_data;
	int m_bits_pixel;
	int m_number_unique_colors;
	int m_dib_size;
	BYTE m_lut[1024];
	static BYTE *m_static_lut;
	int *m_histogram;
	BYTE *m_color_index;
	HANDLE m_hdib;
	CPalette m_pal;
	BOOL m_pal_set;

   static BITMAPINFO *m_static_dib_bitmapinfo;
	static unsigned char *m_static_dib_data;
	static int m_static_num_row_bytes;
	static int m_static_image_width;
	static int m_static_image_height;
	static int m_static_bits_pixel;

	int limit(int num, int min, int max);
	WORD WINAPI DIBNumColors(LPSTR lpbi);
	WORD WINAPI PaletteSize(LPSTR lpbi);
	LPSTR WINAPI FindDIBBits(LPSTR lpbi);
	DWORD WINAPI DIBWidth(LPSTR lpDIB);
	DWORD WINAPI DIBHeight(LPSTR lpDIB);
	HGLOBAL WINAPI CopyHandle (HGLOBAL h);









   CMyDib( );
   ~CMyDib( );
   void clear( );
   void erase( unsigned char red, unsigned char green, unsigned char blue );
   void erase_216( unsigned char red, unsigned char green, unsigned char blue );
   void erase_1( unsigned char color );

   int initialize( int width, int height, unsigned char red, unsigned char green,
                   unsigned char blue );
   int initialize_216(int width, int height, unsigned char red, unsigned char green,
                   unsigned char blue );
   int initialize_1( int width, int height, unsigned char color);

   int display_image( CDC *pDC, int dest_hpix, int dest_vpix );
   int display_image( CDC *pDC, int dest_hpix, int dest_vpix, int width, int height );

   void display_transparent(CDC * pDC, int dest_hpix, int dest_vpix, COLORREF crColour);


   HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

   int stretch_display_image( CDC *pDC, int dest_min_hpix, int dest_min_vpix,
                              int dest_max_hpix, int dest_max_vpix );
   int display_subimage( CDC *pDC, int dest_hpix, int dest_vpix,
                         int min_hpix, int min_vpix, int max_hpix, int max_vpix );
   int stretch_display_subimage( CDC *pDC, int dest_min_hpix, int dest_min_vpix,
                                 int dest_max_hpix, int dest_max_vpix,
                                 int min_hpix, int min_vpix, int max_hpix, int max_vpix );
	int set_pixel_216( int pos, unsigned char color );
	int set_pixel( int hpix, int vpix, unsigned char red, unsigned char green, 
				  unsigned char blue );
	int set_pixel_1( int hpix, int vpix, unsigned char color );
	void set_rgb_image( unsigned char *rgb_array );
   void set_rgb_image( BYTE *redbuf, BYTE *grnbuf, BYTE *blubuf );


   int get_pixel( int hpix, int vpix, unsigned char &red, unsigned char &green, 
                  unsigned char &blue );
	int get_pixel( double hpix, double vpix, unsigned char &red,
                     unsigned char &green, unsigned char &blue );
	BYTE get_pixel_216( int hpix, int vpix );
	BYTE get_pixel_1( int hpix, int vpix );
	int create_palette(CPalette* cpal);
   void create_null_palette(CPalette* cpal);
	void copy_to_clipboard( );
   void copy_from_clipboard( );

	int load_bmp(const BITMAPINFOHEADER &bminfo, RGBQUAD *pallet, BYTE *pbits);
	int load_bmp(VARIANT *dib_variant);


	BOOL save_bmp(CString filename, CString &error_txt);

	static int get_static_pixel( int hpix, int vpix, unsigned char &red,
                     unsigned char &green, unsigned char &blue );

	void decode_color16(unsigned short color, BYTE *red, BYTE *grn, BYTE *blu);
	void decode_color32(unsigned int color, BYTE *red, BYTE *grn, BYTE *blu);
	int number_unique_colors(); // number of unique colors in an 8-bit image

	int initialize_alpha( int width, int height, BYTE red, BYTE green, BYTE blue );
	void set_rgb_image_alpha( unsigned char *rgb_array );
	void set_rgb_image_alpha( BYTE *red_buffer, BYTE *grn_buffer, BYTE *blu_buffer );

	void display_alpha(CDC * pDC, int dest_hpix, int dest_vpix, int transcode);

	void display_hashed_transparent(CDC * pDC, int dest_hpix, int dest_vpix, int trans_code);

};

#endif  // ifndef DIB_H