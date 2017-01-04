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



// Filter.h: interface for the C_point_ovl_filter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTER_H__245EEE65_734F_11D2_8F3E_00104B242B5F__INCLUDED_)
#define AFX_FILTER_H__245EEE65_734F_11D2_8F3E_00104B242B5F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPointOverlayFilter  
{
private:
	// to validate the filter
   CString m_overlay_name;

public:
	CPointOverlayFilter() { }
	CPointOverlayFilter(const CString &ovl_name, const CString& reg_key) 
		: m_overlay_name(ovl_name) { }
	
	CString get_overlay_name() { return m_overlay_name; }

	virtual ~CPointOverlayFilter(){ }

   virtual bool passesThrough(int length) { return true; }
	virtual bool passesThrough(char usage) { return true; }
};


#endif // !defined(AFX_FILTER_H__245EEE65_734F_11D2_8F3E_00104B242B5F__INCLUDED_)
