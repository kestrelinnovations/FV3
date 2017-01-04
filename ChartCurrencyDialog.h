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



#if !defined(AFX_CURRDLG_H__C4317AF2_A983_11D1_B2C6_00C0F01676E1__INCLUDED_)
#define AFX_CURRDLG_H__C4317AF2_A983_11D1_B2C6_00C0F01676E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CurrDlg.h : header file
//

#include "common.h"
#include "resource.h" // for IDD_CHART_CURRENCY

/////////////////////////////////////////////////////////////////////////////
// ChartCurrencyDlg dialog

class ChartCurrencyDlg
{
// Construction
public:
   ChartCurrencyDlg();   // standard constructor
   ~ChartCurrencyDlg();

public:

   static int detect_CSD_cd(CString path, CsdIndexTypeEnum eCsdIndexType, boolean_t* need_to_import);
   static int handle_new_CSD_disk(CString path, CsdIndexTypeEnum eCsdIndexType);
};

#endif // !defined(AFX_CURRDLG_H__C4317AF2_A983_11D1_B2C6_00C0F01676E1__INCLUDED_)
