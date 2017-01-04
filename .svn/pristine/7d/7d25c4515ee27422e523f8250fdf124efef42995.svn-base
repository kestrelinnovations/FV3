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



#pragma once

#ifndef ERRX_H
#define ERRX_H 1

/*
 *  errx - extended error reporting
 *
 *  This file is separate from err.h because the MFC stuff (CString) in this
 *  file caused problems for some libraries.
 */

#include "err.h"

/*
 *  NOTE: Theses macro MUST be called with a double set a parentheses for
 *  them to work, e.g. ERR_printf(("%d", 10))
 */
#define ERR_printf(x)                     \
   do {                                   \
      CString s;                          \
      s.Format x;                         \
      _ERR_report(s, __FILE__, __LINE__); \
   } while (0);

#define INFO_printf(x)                     \
   do {                                    \
      CString s;                           \
      s.Format x;                          \
      _INFO_report(s, __FILE__, __LINE__); \
   } while (0);

#endif
