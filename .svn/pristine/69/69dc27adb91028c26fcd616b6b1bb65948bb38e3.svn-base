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



#ifndef GRA_DEF_H
#define GRA_DEF_H

//
// The logical palette indices and RGB values of the standard predefined 
// Windows VGA colors and our non-standard predefined colors.
//
// Note that the logical palette indices should be used rather than
// the RGB values.  Only the graphics library should use the RGB values.
// They are defined here so that clients of the graphics library can see
// the RGB values corresponding to each index.
//

#define GRA_NUMBER_OF_STANDARD_COLORS 24

// the 16 VGA colors

const unsigned char BLACK            =   0;
const unsigned char STD_RED          =   1;
const unsigned char STD_GREEN        =   2;
const unsigned char YELLOW           =   3;
const unsigned char BLUE             =   4;
const unsigned char MAGENTA          =   5;
const unsigned char CYAN             =   6;
const unsigned char LIGHT_GREY       =   7;
const unsigned char WHITE            = 248;
const unsigned char STD_BRIGHT_RED   = 249;
const unsigned char STD_BRIGHT_GREEN = 250;
const unsigned char BRIGHT_YELLOW    = 251;
const unsigned char BRIGHT_BLUE      = 252;
const unsigned char BRIGHT_MAGENTA   = 253;
const unsigned char BRIGHT_CYAN      = 254;
const unsigned char BRIGHT_WHITE     = 255;

                           //   R    G    B  values

#define RGB_BLACK                0,   0,   0
#define RGB_STD_RED            128,   0,   0
#define RGB_STD_GREEN            0, 128,   0
#define RGB_YELLOW             128, 128,   0
#define RGB_BLUE                 0,   0, 128
#define RGB_MAGENTA            128,   0, 128
#define RGB_CYAN                 0, 128, 128
#define RGB_LIGHT_GREY         192, 192, 192
#define RGB_WHITE              128, 128, 128
#define RGB_STD_BRIGHT_RED     255,   0,   0
#define RGB_STD_BRIGHT_GREEN     0, 255,   0
#define RGB_BRIGHT_YELLOW      255, 255,   0
#define RGB_BRIGHT_BLUE          0,   0, 255
#define RGB_BRIGHT_MAGENTA     255,   0, 255
#define RGB_BRIGHT_CYAN          0, 255, 255
#define RGB_BRIGHT_WHITE       255, 255, 255

// 
// the four extra colors gotten with a 256-color driver under Windows
// 
const unsigned char PALE_GREEN       =   8;
const unsigned char LIGHT_BLUE       =   9;
const unsigned char OFF_WHITE        = 246;
const unsigned char MEDIUM_GRAY      = 247;

#define RGB_PALE_GREEN         192, 220, 192
#define RGB_LIGHT_BLUE         166, 202, 240
#define RGB_OFF_WHITE          255, 251, 240
#define RGB_MEDIUM_GRAY        160, 160, 164

// 
//  non-standard predefined colors added for places where the Windows
//  standard colors are not sufficient (e.g. coverage overlays)
// 
const unsigned char RED              = 242;
const unsigned char GREEN            = 243;
const unsigned char BRIGHT_RED       = 244;
const unsigned char BRIGHT_GREEN     = 245;

#define RGB_RED                170,   0,   0
#define RGB_GREEN                0, 170,   0
#define RGB_BRIGHT_RED         255,  85,  85
#define RGB_BRIGHT_GREEN        85, 255,  85

//
//  Line style
//
#define SOLID  PS_SOLID
#define DASHED PS_DASH   // note: only valid when pen width is 1

/*------------------------------------------------------------------
-                           Typedefs 
- -----------------------------------------------------------------*/

typedef int color_index_t;

#endif
