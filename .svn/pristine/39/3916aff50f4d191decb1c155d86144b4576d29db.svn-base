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

// FullScreenDialog.h
//

#ifndef FALCONVIEW_FULLSCREENDIALOG_H_
#define FALCONVIEW_FULLSCREENDIALOG_H_

#include <atlhost.h>
#include "resource.h"

class FullScreenDialog :
   public CAxDialogImpl<FullScreenDialog>
{
public:
   FullScreenDialog();
   virtual ~FullScreenDialog();

   enum { IDD = IDD_FULL_SCREEN_DIALOG };

BEGIN_MSG_MAP(FullScreenDialog)
   MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
   COMMAND_ID_HANDLER(IDOK, OnOK)
   COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()

   bool m_noremind;
   _bstr_t m_prompt;

   LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif  // FALCONVIEW_FULLSCREENDIALOG_H_
