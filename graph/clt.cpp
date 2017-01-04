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



/*------------------------------------------------------------------
-  FILE NAME:    clt.cpp
-
-  DESCRIPTION:
-
-  These functions control access to the palette (color lookup table).
-  
-------------------------------------------------------------------*/

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "stdafx.h"
#include "graphics.h"

/*------------------------------------------------------------------
-                         Static Variables
-------------------------------------------------------------------*/

// Constants for standard colors.  These colors include the 16 VGA colors,
// the additional four standard windows colors, and the four FalconView
// specific colors.  All of these colors are maintained in the palette by
// the graphics library.  This array is used to convert standard color
// constants to COLORREFs and vice-versa, along with standard_color_ref_table,
// below.foo
static int standard_color_code_table[GRA_NUMBER_OF_STANDARD_COLORS] = {
   BLACK, STD_RED, STD_GREEN, YELLOW, 
   BLUE, MAGENTA, CYAN, LIGHT_GREY,
   WHITE, STD_BRIGHT_RED, STD_BRIGHT_GREEN, BRIGHT_YELLOW,
   BRIGHT_BLUE, BRIGHT_MAGENTA, BRIGHT_CYAN, BRIGHT_WHITE,
   PALE_GREEN, LIGHT_BLUE, OFF_WHITE, MEDIUM_GRAY,
   RED, GREEN, BRIGHT_RED, BRIGHT_GREEN};

// This array is used along with standard_color_code_table to convert standard
// color constants to COLORREFs and vice-versa.  It must be initialized by the
// gra_open_clt function.
static COLORREF standard_color_ref_table[GRA_NUMBER_OF_STANDARD_COLORS];

/*------------------------------------------------------------------
-                     Static Function Prototypes
-------------------------------------------------------------------*/

static COLORREF get_color_ref(BYTE red, BYTE green, BYTE blue);

/*------------------------------------------------------------------
-  FUNCTION NAME: gra_open_clt
-
-  PURPOSE:
-
-      Set up the color table and enable color table functions. 
-
-------------------------------------------------------------------*/

//
// create a palette that mirrors the system palette in that the lower
// 10 and upper ten indices contain the system colors
//
int gra_open_clt(void)
{
   //
   // setup the COLORREF to color-constant and color-constant to COLORREF 
   // conversions for all colors supported by the graphics library
   //

   // the 16 VGA colors
   standard_color_ref_table[0] = get_color_ref(RGB_BLACK);
   standard_color_ref_table[1] = get_color_ref(RGB_STD_RED);
   standard_color_ref_table[2] = get_color_ref(RGB_STD_GREEN);
   standard_color_ref_table[3] = get_color_ref(RGB_YELLOW);
   standard_color_ref_table[4] = get_color_ref(RGB_BLUE);
   standard_color_ref_table[5] = get_color_ref(RGB_MAGENTA);
   standard_color_ref_table[6] = get_color_ref(RGB_CYAN);
   standard_color_ref_table[7] = get_color_ref(RGB_LIGHT_GREY);
   standard_color_ref_table[8] = get_color_ref(RGB_WHITE); 
   standard_color_ref_table[9] = get_color_ref(RGB_STD_BRIGHT_RED);
   standard_color_ref_table[10] = get_color_ref(RGB_STD_BRIGHT_GREEN);
   standard_color_ref_table[11] = get_color_ref(RGB_BRIGHT_YELLOW);
   standard_color_ref_table[12] = get_color_ref(RGB_BRIGHT_BLUE);
   standard_color_ref_table[13] = get_color_ref(RGB_BRIGHT_MAGENTA);
   standard_color_ref_table[14] = get_color_ref(RGB_BRIGHT_CYAN);
   standard_color_ref_table[15] = get_color_ref(RGB_BRIGHT_WHITE);

   // the four extra colors gotten with a 256-color driver under Windows
   standard_color_ref_table[16] = get_color_ref(RGB_PALE_GREEN);
   standard_color_ref_table[17] = get_color_ref(RGB_LIGHT_BLUE);
   standard_color_ref_table[18] = get_color_ref(RGB_OFF_WHITE);
   standard_color_ref_table[19] = get_color_ref(RGB_MEDIUM_GRAY);

   //  non-standard predefined colors added for places where the Windows
   //  standard colors are not sufficient
   standard_color_ref_table[20] = get_color_ref(RGB_RED);
   standard_color_ref_table[21] = get_color_ref(RGB_GREEN);
   standard_color_ref_table[22] = get_color_ref(RGB_BRIGHT_RED);
   standard_color_ref_table[23] = get_color_ref(RGB_BRIGHT_GREEN);

   /* set flag for all functions which access the color lookup table */

   return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

// Use this function to get a the COLORREF for a built in color like
// BLACK, YELLOW, BRIGHT_WHITE, etc.  For example, for CYAN use the call
// get_color_ref(RGB_CYAN).  The purpose of this call is allow you
// to use the macro version of the RGB values for the built in colors,
// as opposed to the values they represent.  This function is necessary
// because RGB(RGB_CYAN) doesn't compile.
static COLORREF get_color_ref(BYTE red, BYTE green, BYTE blue) 
{ 
	if ((red == 255) && (green == 255) && (blue == 255))  // kludge to fix Win98 problem with white
		return RGB(255, 254, 250);
	else
		return PALETTERGB(red, green, blue); 
}

void rgb_to_xyz(float r, float g, float b, float &x, float &y, float &z)
{
	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;
	
	if (r <= 0.04045)
		r = r/12.92f;
	else 
		r = float(pow(((r+0.055)/1.055),2.4));

	if (g <= 0.04045)  
		g = g / 12.92f;
	else 
		g = float(pow(((g+0.055)/1.055),2.4));
	
	if ( b <= 0.04045 )  
		b = b / 12.92f;
	else 
		b = float(pow(((b+0.055)/1.055),2.4));
	
	x =  41.24f * r + 35.76f * g + 18.05f * b;
	y =  21.26f * r + 71.52f * g + 7.2f * b;
	z =  1.93f  * r + 11.92f * g + 95.05f * b;
}

void xyz_to_rgb(float x, float y, float z, int &r, int &g, int &b)
{
	double dr,dg,db;
	dr =  0.032406  * x - 0.015371 * y - 0.0049895 * z;
	dg = -0.0096891 * x + 0.018757 * y + 0.00041914* z;
	db =  0.00055708* x - 0.0020401* y + 0.01057   * z;

	if (dr <= 0.00313) 
		dr = dr*12.92;
	else 
		dr = exp(log(dr)/2.4)*1.055-0.055;

	if (dg <= 0.00313) 
		dg=dg*12.92;
	else 
		dg = exp(log(dg)/2.4)*1.055-0.055;

	if (db <= 0.00313) 
		db = db*12.92;
	else 
		db = exp(log(db)/2.4)*1.055-0.055;

	dr = dr*255;
	dg = dg*255;
	db = db*255;
	
	dr = min(255.0f, max(0.0f,dr));
	dg = min(255.0f, max(0.0f,dg));
	db = min(255.0f, max(0.0f,db));

	r = int(dr+0.5);
	g = int(dg+0.5);
	b = int(db+0.5);
}

// Given a one of the standard color constants defined above, this function
// returns the COLORREF for that color.  Returns the COLORREF for BRIGHT_WHITE
// by default.  This is the inverse function of GRA_get_color_constant().
COLORREF GRA_get_color_ref(int standard_color_constant)
{
   int i=0;

   while (i < GRA_NUMBER_OF_STANDARD_COLORS)
   {
      if (standard_color_code_table[i] == standard_color_constant)
         return standard_color_ref_table[i];

      i++;
   }

   return get_color_ref(RGB_BRIGHT_WHITE);
}



// Given a COLORREF for one of the standard color constants defined above, this
// function returns that color constant.  Returns BRIGHT_WHITE by default.
// This is the inverse function of GRA_get_color_ref().
int GRA_get_color_constant(COLORREF standard_color)
{
   int i=0;

   while (i < GRA_NUMBER_OF_STANDARD_COLORS)
   {
      if (standard_color_ref_table[i] == standard_color)
         return standard_color_code_table[i];

      i++;
   }

   return BRIGHT_WHITE;
}

// Given a COLORREF for one of the standard color constants defined above, this
// function returns that color constant.  Returns BRIGHT_WHITE by default.
// This is the inverse function of GRA_get_color_ref().
int GRA_get_color_constant_best_fit(COLORREF standard_color)
{
   int k=0;
   int minColorDistance=( 3*256*256 );
   int _r= GetRValue(standard_color);
   int _g= GetGValue(standard_color);
   int _b= GetBValue(standard_color);

   for (int i=0; i < GRA_NUMBER_OF_STANDARD_COLORS; i++)
   {
      COLORREF color = standard_color_ref_table[i];
      int r= GetRValue(color);
      int g= GetGValue(color);
      int b= GetBValue(color);
      r-=_r;
      g-=_g;
      b-=_b;
      int colorDistance = (r*r + g*g + b*b);

      if (colorDistance < minColorDistance)
      {
         minColorDistance = colorDistance;
         k=i;
      }      
   }

   //result is the closest fit
   return standard_color_code_table[k];
}

