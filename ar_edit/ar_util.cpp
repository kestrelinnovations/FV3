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

// ar_util.h

#include "stdafx.h"
#include "common.h"
#include "ar_util.h"

// ********************************************************************
// ********************************************************************

CString CArUtil::fix_altitude(CString alt)
{
	CString tstr("");
	CString tstr2("");
	int pos, len;

	tstr2 = alt;
	// trim leading zeros
	tstr2 += " ";
	while (tstr2.Find('0') == 0)
		tstr2 = tstr2.Right(tstr2.GetLength() -1);

	// setup the default 
	tstr = tstr2;

	// change AMSL to MSL
	len = tstr2.GetLength();
	pos = tstr2.Find("AMSL");
	if (pos >= 0)
	{
		// remove the A in AMSL
		tstr = tstr2.Left(pos);
		tstr += " ";
		tstr += tstr2.Mid(pos+1, len-pos-1);
	}
	else
	{
		// put a space between the alt and the alt type
		pos = tstr2.Find("AGL");
		if (pos >= 0)
		{
			tstr = tstr2.Left(pos);
			tstr += " ";
			tstr += tstr2.Mid(pos, len-pos);
		}
		else
		{
			// put a space between the alt and the alt type
			pos = tstr2.Find("MSL");
			if (pos >= 0)
			{
				tstr = tstr2.Left(pos);
				tstr += " ";
				tstr += tstr2.Mid(pos, len-pos);
			}
		}
	}
	tstr.TrimRight();

	return tstr;
}
// end of fix_altitude

// ********************************************************************

// decode altitude description and data and produce string

int CArUtil::decode_altitude(CString desc, CString alt1, CString alt2, CString & output)
{
	CString tstr1, tstr2;

	tstr1 = fix_altitude(alt1);
	tstr2 = fix_altitude(alt2);

	if (!desc.Compare("AA"))
	{
		output = "At or Above ";
		output += tstr1;
		return SUCCESS;
	}
	if (!desc.Compare("AB"))
	{
		output = "At or Below ";
		output += tstr1;
		return SUCCESS;
	}
	if (!desc.Compare("BT"))
	{
		output = "Between ";
		output += tstr1;
		output += " and ";
		output += tstr2;
		return SUCCESS;
	}
	if (!desc.Compare("AT"))
	{
		output = "At ";
		output += tstr1;
		return SUCCESS;
	}
	if (!desc.Compare("AS"))
	{
		output = "As Assigned";
		return SUCCESS;
	}
	output = alt1 + "  " + alt2;
	return FAILURE;
}
// end of decode_altitude

// ********************************************************************
// ***********************************************************************

char CArUtil::get_navaid_usage(int type)
{
   switch(type)
   {
      case 3:   return 'T';
      case 1:   return 'V';
      case 5:   return 'N';
      case 9:   return 'M';
      case 2:   return 'R';
      case 4:   return 'E';
      case 7:   return 'B';
      
      default:
           return 0;   //not found
   }
}
// end of get_navaid_usage

// ***********************************************************************
