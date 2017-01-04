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

#pragma once
#include "afxwin.h"


// CEffectiveScaleDialog dialog

class CEffectiveScaleDialog : public CDialog
{
	DECLARE_DYNAMIC(CEffectiveScaleDialog)

public:
	CEffectiveScaleDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectiveScaleDialog();

// Dialog Data
	enum { IDD = IDD_EFFECTIVE_SCALE };
 CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Effective_Scale.htm";}

   int m_nEffectiveScale;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

   int m_nMinEffectiveScale;
   int m_nMaxEffectiveScale;
   bool m_bCancelled;
public:
   afx_msg void OnEnKillfocusScale();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   CEdit m_scaleCtrl;
};