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

// MAPIWrapper.cpp
//
#include "stdafx.h"
#include "common.h"
#include "MAPIWrapper.h"

// constructor
MAPIWrapper::MAPIWrapper() :
   m_lpfnSendMail(NULL), m_mapi_handle(NULL)
{
	m_is_mail_available = ::GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0 &&
			SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0;
}

// destructor
MAPIWrapper::~MAPIWrapper()
{
	if (m_mapi_handle != NULL)
		::FreeLibrary(m_mapi_handle);
}

// pathname - fully qualified path of the attached file. This path should 
// include the disk drive letter and directory name. 
//
// filename - the attachment filename seen by the recipient, which may differ 
// from the filename in the pathname member if temporary files are being used. 
// If the filename member is empty or NULL, the filename from pathname is used.
int MAPIWrapper::send_mail(CString pathname, CString filename)
{
	CList<CString, CString> pathname_lst;
	pathname_lst.AddTail(pathname);

	CList<CString, CString> filename_lst;
	filename_lst.AddTail(filename);

	return send_mail(pathname_lst, filename_lst);
}

int MAPIWrapper::send_mail(CList<CString,CString>& pathname_lst, CList<CString,CString>& filename_lst)
{
	ASSERT(pathname_lst.GetCount() == filename_lst.GetCount());

	CWaitCursor wait;

	const int file_count = pathname_lst.GetCount();

	MapiFileDesc *fileDesc = new MapiFileDesc[file_count];
	POSITION position = pathname_lst.GetHeadPosition();
	POSITION file_pos = filename_lst.GetHeadPosition();
	int index = 0;
	while (position && file_pos)
	{
		CString& pathname = pathname_lst.GetNext(position);
		CString& filename = filename_lst.GetNext(file_pos);

		memset(&fileDesc[index], 0, sizeof(MapiFileDesc));
		fileDesc[index].nPosition = (ULONG)-1;
		fileDesc[index].lpszPathName = (char *)(LPCTSTR)pathname;
		fileDesc[index].lpszFileName = (char *)(LPCTSTR)filename;

		index++;
	}
	
	// prepare the message 
	//
	
	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.nFileCount = file_count;
	message.lpFiles = fileDesc;
	
	
	// send the message
	//
	
	// prepare for modal dialog box
	AfxGetApp()->EnableModeless(FALSE);
	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, &hWndTop);
	
	// some extra precautions are required to use MAPISendMail as it
	// tends to enable the parent window in between dialogs (after
	// the login dialog, but before the send note dialog).
	pParentWnd->SetCapture();
	::SetFocus(NULL);
	pParentWnd->m_nFlags |= WF_STAYDISABLED;

   // if the MAPI DLL has not yet been loaded
   if (m_lpfnSendMail == NULL)
   {
      m_mapi_handle = ::LoadLibraryA("MAPI32.DLL");
      if (m_mapi_handle == NULL)
      {
         AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
         return FAILURE;
      }

      // get the pointer to the MAPISendMail method
      //
      (FARPROC&)m_lpfnSendMail = GetProcAddress(m_mapi_handle, "MAPISendMail");
      if (m_lpfnSendMail == NULL)
      {
         AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
         ::FreeLibrary(m_mapi_handle);
         return FAILURE;
      }
   }
	
	int nError = m_lpfnSendMail(0, (ULONG)pParentWnd->GetSafeHwnd(),
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);
	
	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	::ReleaseCapture();
	pParentWnd->m_nFlags &= ~WF_STAYDISABLED;
	
	pParentWnd->EnableWindow(TRUE);
	::SetActiveWindow(NULL);
	pParentWnd->SetActiveWindow();
	pParentWnd->SetFocus();
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	AfxGetApp()->EnableModeless(TRUE);

	delete [] fileDesc;
	
	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT)
	{
      CString msg;
      switch(nError)
      {
      case MAPI_E_AMBIGUOUS_RECIPIENT: 
         msg = "A recipient matched more than one of the recipient descriptor structures and MAPI_DIALOG was not set. No message was sent.";
         break;
      case MAPI_E_ATTACHMENT_NOT_FOUND:
         msg = "The specified attachment was not found. No message was sent.";
         break;
      case MAPI_E_ATTACHMENT_OPEN_FAILURE:
         msg = "The specified attachment could not be opened. No message was sent.";
         break;
      case MAPI_E_BAD_RECIPTYPE:
         msg = "The type of a recipient was not MAPI_TO, MAPI_CC, or MAPI_BCC. No message was sent.";
         break;
      case MAPI_E_FAILURE:
         msg = "One or more unspecified errors occurred. No message was sent.";
         break;
      case MAPI_E_INSUFFICIENT_MEMORY:
         msg = "There was insufficient memory to proceed. No message was sent.";
         break;
      case MAPI_E_INVALID_RECIPS:
         msg = "One or more recipients were invalid or did not resolve to any address.";
         break;
      case MAPI_E_LOGIN_FAILURE:
         msg = "There was no default logon, and the user failed to log on successfully when the logon dialog box was displayed. No message was sent.";
         break;
      case MAPI_E_TEXT_TOO_LARGE:
         msg = "The text in the message was too large. No message was sent.";
         break;
      case MAPI_E_TOO_MANY_FILES:
         msg = "There were too many file attachments. No message was sent.";
         break;
      case MAPI_E_TOO_MANY_RECIPIENTS:
         msg = "There were too many recipients. No message was sent.";
         break;
      case MAPI_E_UNKNOWN_RECIPIENT:
         msg = "A recipient did not appear in the address list. No message was sent.";
         break;
      }

		AfxMessageBox(msg);
		return FAILURE;
	}
	
	return SUCCESS;
}