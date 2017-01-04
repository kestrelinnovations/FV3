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

// MAPIWrapper.h
//

#pragma once

#include <mapi.h>

typedef ULONG(PASCAL *mapi_send_mail_f)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);

class MAPIWrapper
{
   HINSTANCE m_mapi_handle;
	mapi_send_mail_f m_lpfnSendMail;
	BOOL m_is_mail_available;

public:
	// constructor
	MAPIWrapper();

	// destructor
	~MAPIWrapper();

	BOOL is_mail_available() { return m_is_mail_available; }

	// pathname - fully qualified path of the attached file. This path should 
	// include the disk drive letter and directory name. 
	//
	// filename - the attachment filename seen by the recipient, which may differ 
	// from the filename in the pathname member if temporary files are being used. 
	// If the filename member is empty or NULL, the filename from pathname is used.
	int send_mail(CString pathname, CString filename);
	int send_mail(CList<CString,CString> &pathname_lst, CList<CString,CString> &filename_lst);
};