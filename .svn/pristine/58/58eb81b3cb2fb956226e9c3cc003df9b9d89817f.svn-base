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

// FullScreenDialog.cpp
//

#include "stdafx.h"
#include "FalconView/FullScreenDialog.h"

FullScreenDialog::FullScreenDialog() :
   m_noremind(false)
{
   m_prompt = _T("Unspecified prompt.");
}

FullScreenDialog::~FullScreenDialog()
{
}


LRESULT FullScreenDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam,
   BOOL& bHandled)
{
   SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

   SendMessage(GetDlgItem(IDC_PROMPT),
      WM_SETTEXT, (WPARAM)(0), (LPARAM)(TCHAR*)(m_prompt));

   CenterWindow();

   return 1;  // Let the system set the focus
}

LRESULT FullScreenDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl,
   BOOL& bHandled)
{
   m_noremind = (SendMessage(GetDlgItem(IDC_NOREMIND_CHECK),
      BM_GETCHECK, (WPARAM)0, (LPARAM)0) == BST_CHECKED ? 1 : 0);

   EndDialog(wID);
   return 0;
}

LRESULT FullScreenDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl,
   BOOL& bHandled)
{
   return OnOK(wNotifyCode, wID, hWndCtl, bHandled);
}
