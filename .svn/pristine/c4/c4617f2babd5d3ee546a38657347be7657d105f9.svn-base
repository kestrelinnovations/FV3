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

// NitfOvlOptnPage2.h: interface for the CNitfOvlOptnPage2 class.
//
//////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "nitfsourcesdlg.h"
//}}AFX_INCLUDES

#pragma once

#include "common.h"
#include "overlay_pp.h"

/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnPage2 dialog

class CNitfOvlOptnPage2 : public COverlayPropertyPage
{
	DECLARE_DYNCREATE(CNitfOvlOptnPage2)

// Construction
public:
	CNitfOvlOptnPage2();
	~CNitfOvlOptnPage2();

// Dialog Data
	//{{AFX_DATA(CNitfOvlOptnPage2)
	enum { IDD = IDD_OVL_NITF2_DLG };
	BOOL	m_show_bounds;
	CNITFSourcesDlg	m_NITF_files;
	CNITFSourcesDlg	m_NITF_paths;
	BOOL	m_hide_during_scroll;
   BOOL  m_bImagesOnlyNoEditor;
   BOOL  m_bImagesOnlyNoEditorOriginal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNitfOvlOptnPage2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNitfOvlOptnPage2)
	virtual BOOL OnInitDialog();
	afx_msg void OnBoundingAreas();
	virtual void OnOK();
   virtual void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// End of NITFOvlOptnPage2.h