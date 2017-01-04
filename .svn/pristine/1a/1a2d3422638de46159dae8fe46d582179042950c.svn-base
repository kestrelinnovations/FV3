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



// AppExpiration.cpp
// CAppExpiration

#include "stdafx.h"
#include "appinfo.h"
#include "AppExpiration.h"
#include "param.h"

// Procedure to update key for new version of FalconView :
//
//		1.  Choose an arbitratry date, use date encoder (found in 
//			 Source Safe $/Date Encoder) to get a corresponding
//			 key.  Modify CAppExpiration's constructor to use this
//			 date and update the comment.  The installation will need
//			 to be updated with the key (e.g, "0B163061E1E10082") upon
//			 final release to never expire.
//
//		2.  Update get_extended_expiration_date() and change the value
//			 that the key is stored in, e.g. "extension_3_3_0", to correspond 
//        with the version of FV.
//
//		3.  Do the same for set_expiration_date()

// constructor
CAppExpiration::CAppExpiration()
{
	COleDateTime extended_expiration_date;

	m_expiration_date = get_hard_coded_expiration_date();
  	
	extended_expiration_date = get_extended_expiration_date();
  
  	// if the date is set to 9/11/2001, FV never expires
   // this corresponds to a expiration code of "0B163061E1E10082"
 	m_never_expires = (9 == extended_expiration_date.GetMonth() &&
	                   11 == extended_expiration_date.GetDay() && 
					   2001 == extended_expiration_date.GetYear());
    
	if (m_never_expires)
	{
		// if FV never expires, set to largest OleDateTime Dec 31, 9999
		m_expiration_date.SetDate( 9999, 12, 31 );
	}
	else
	{
		// if FalconView is expired, and the extended expiration date > the hard
		// coded expiration date then set the expiration date to the extended 
		// expriration date (the reason we check to see if it is expired is that
		// we do not want to show any old extensions (which also might be possible hacks)
		if (is_expired() && extended_expiration_date > m_expiration_date)
			m_expiration_date = extended_expiration_date;
	}
}

// NOTE: must use this function to see if is expired - do not just 
// check expiration date
BOOL CAppExpiration::is_expired() const
{
	// if current date > expiration date, expired = TRUE
	return COleDateTime::GetCurrentTime() > m_expiration_date;
}

// returns the number of days remaining until FalconView expires
int CAppExpiration::number_of_days_until_expiration()
{
	COleDateTimeSpan time_span = m_expiration_date - COleDateTime::GetCurrentTime();

	return (int)time_span.GetTotalDays();
}

BOOL CAppExpiration::never_expires() const
{
	return m_never_expires;
}

unsigned char CAppExpiration::convert_ascii_hex_to_byte(char c) const
{
	unsigned char result = 0;

	// converts an upper case ascii hex value to base 10
	// converts an acsii representation of a hex number (e.g "A") to its base 10 equivalent
	// (e.g., 10)
	ASSERT('F'-'A'==5);
	ASSERT('9'-'0'==9);

	if (c >= 'A' && c <= 'F')
		result =  c - 'A' + 10;
	else
		if (c >= '0' && c <= '9')
			result = c - '0' + 0;
		
	return result;
}

COleDateTime CAppExpiration::decode_date(CString &cipherText) const
{
	COleDateTime date;
	// ciphertext is a 16 byte string of hex characters (e.g., 0A 5B F6 A1 0A 5B F6 A1)

	// remove spaces and make uppercase
	cipherText = cipherText.SpanExcluding(" ");
	cipherText.MakeUpper();

	if (cipherText.GetLength() == 16)
	{
		int i;
		int year, month, day;

		unsigned char byteCipherText[8];

		// sequence[] is the correct sequence of the bytes in the cypherText to yield a date 
		// which can be parsed; note 8 = '/'  
		unsigned char sequence[] = {1,3,0,4,7,2,6,5}; 
		
		
		// convert hex string to number (byte)
		for (i=0;i<8;i++)
		{
			byteCipherText[i]=convert_ascii_hex_to_byte(cipherText.GetAt(i*2)) * 16 +
						  convert_ascii_hex_to_byte(cipherText.GetAt(i*2+1));
		}

		// xor with simple key
		for (i=0;i<8;i++)
			byteCipherText[i] ^= ((i+1+8 + 1) << i);

		// calculate month, day, and year by putting in the correct sequence
		// as determined by sequence[]
		month = byteCipherText[sequence[0]]*10+byteCipherText[sequence[1]];
		day = byteCipherText[sequence[2]]*10+byteCipherText[sequence[3]];
		year = byteCipherText[sequence[4]]*1000+byteCipherText[sequence[5]]*100 +
				 byteCipherText[sequence[6]]*10+byteCipherText[sequence[7]];
		
		date.SetDate(year, month, day);
	}
	return date;
}



COleDateTime CAppExpiration::get_extended_expiration_date() const
{
   CString cipherText = PRM_get_registry_string("Main", "extension_4_1_0", "");
	return decode_date(cipherText);
}

//static 
COleDateTime CAppExpiration::get_hard_coded_expiration_date() const
{
	// expires in the last minute of the 180th day
	COleDateTimeSpan span(179,23,59,59); 
	return get_app_build_date() + span;
}


void CAppExpiration::set_expiration_date(CString &cipherText)
{
	// expires in the last minute of the day
	COleDateTimeSpan span(0,23,59,59); 

	PRM_set_registry_string("Main", "extension_4_1_0", cipherText);
	m_expiration_date = decode_date(cipherText) + span;
}


COleDateTime CAppExpiration::get_expiration_date() const
{
	return m_expiration_date;
}


CString CAppExpiration::get_expiration_date_as_string() const
{
	// VAR_DATEVALUEONLY is not used for the following reasons
	// 1) it did not give me the proper result
	// 2) it throws an exception if the country is not US/English (see Q167169)
   
	return m_expiration_date.Format("%B %d, %Y");
}

