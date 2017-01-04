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



// MBString.h: interface for the CMBString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MBSTRING_H__5641AC00_8476_11D2_B965_00105A9B4C37__INCLUDED_)
#define AFX_MBSTRING_H__5641AC00_8476_11D2_B965_00105A9B4C37__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "catalog.h"
#include "sets.h"

class CMBString : public CString  
{
   COleDateTime m_starttime;
   double m_fTotalMB;
	DoubleSet m_MovingAvgSecs;
	int m_MovingAvgSize;

public:
	CMBString();
	virtual ~CMBString();

   CString FormatMB(double fBytes, int nPrec=3);
   CString TimeInitialize(long lTotalCount, double fTotalMB);
   CString TimeRemaining(double fRemainingMB);
   CString FormatTime(double fSecondsRemaining);
   void CMBString::DebugFormatTime(void);
};

#endif // !defined(AFX_MBSTRING_H__5641AC00_8476_11D2_B965_00105A9B4C37__INCLUDED_)
