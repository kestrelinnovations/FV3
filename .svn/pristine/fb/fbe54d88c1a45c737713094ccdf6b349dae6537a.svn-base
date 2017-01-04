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



// CAppExpiration header
#pragma once

class CAppExpiration
{
   COleDateTime m_expiration_date;
   BOOL m_never_expires;

public:
	
   CAppExpiration();
	
	// NOTE: must use this function to see if is expired - do not just 
	// check expiration date because the application may "never expire" 
	// which overrides the expiration date

	// returns the number of days remaining until FalconView expires so
	// we can start warning the user 30 days before we expire
	int number_of_days_until_expiration();

	BOOL is_expired() const; 
	BOOL never_expires() const; 
	COleDateTime get_extended_expiration_date() const;
	COleDateTime get_hard_coded_expiration_date() const;
	void set_expiration_date(CString &cipherText);
	COleDateTime get_expiration_date() const;
	CString get_expiration_date_as_string() const;
	unsigned char convert_ascii_hex_to_byte(char c) const;
	COleDateTime decode_date(CString &hexCipherText) const;
};



 


