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

#if !defined(AFX_AUXDATAOPTIONSPAGE_H__2021EBE1_3976_4EA2_844C_1BF306111C6D__INCLUDED_)
#define AFX_AUXDATAOPTIONSPAGE_H__2021EBE1_3976_4EA2_844C_1BF306111C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 
#pragma once
#include "..\resource.h" 


// CGPSAuxData dialog
class C_gps_trail;
class CAuxDataProperties;

class CAuxDataOptionsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAuxDataOptionsPage)

public:
	CAuxDataOptionsPage(CAuxDataProperties* pProperties = NULL, C_overlay* pOverlay = NULL);
	virtual ~CAuxDataOptionsPage();

	bool m_bApplyImmediately;

	BOOL store_values();

// Dialog Data
	enum { IDD = IDD_GPS_AUXDATA };
	CButton *m_buttons;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* msg);
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	void InitializeControls();
	afx_msg void OnModified();	
	DECLARE_MESSAGE_MAP()


	int m_NumAuxTypes;

	 CAuxDataProperties *m_prop;
	C_overlay *m_pOverlay;
   boolean_t m_modified;
};
#endif // !defined(AFX_AUXDATAOPTIONSPAGE_H__2021EBE1_3976_4EA2_844C_1BF306111C6D__INCLUDED_)
