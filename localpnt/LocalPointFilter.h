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



// AimpointFilter.h: interface for the CAimpointFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALPOINTFILTER_H__245EEE66_734F_11D2_8F3E_00104B242B5F__INCLUDED_)
#define AFX_LOCALPOINTFILTER_H__245EEE66_734F_11D2_8F3E_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FalconView/include/fv_filter.h"

class CLocalPointFilter : public CPointOverlayFilter
{
public:
   CLocalPointFilter();
   CLocalPointFilter(const CString &ovl_name, const CString &reg_key);
   virtual ~CLocalPointFilter();
   virtual bool passesThrough(int lar);
   virtual bool passesThrough(char usage);
};

#endif // !defined(AFX_LOCALPOINTFILTER_H__245EEE66_734F_11D2_8F3E_00104B242B5F__INCLUDED_)
