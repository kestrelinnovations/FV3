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

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_GENPROP_H
#define FALCONVIEW_PRINTTOOLOVERLAY_GENPROP_H

// GenProp.h : header file
//

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/optndlg.h"

// this project's headers
// forward definitions
class CStripChartPropertyPage;
class CPrintToolsPropertyPage;
class CAreaChartPropertyPage;

class CPropertySheetPrintToolTrans : public CPropertySheet
{
public:
   virtual BOOL PreTranslateMessage(MSG* pMsg);
   void on_help();
};

class COvlPrintToolPropertyPage : public CPropertyPage
{
   DECLARE_DYNCREATE(COvlPrintToolPropertyPage)

// Construction
public:
   COvlPrintToolPropertyPage();
   ~COvlPrintToolPropertyPage();
   void on_help();

// Dialog Data
   // {{AFX_DATA(COvlPrintToolPropertyPage)
   enum
   { IDD = IDD_OVL_PRINTTOOL };
      // NOTE - ClassWizard will add data members here.
      //    DO NOT EDIT what you see in these blocks of generated code !
   // }}AFX_DATA


// Overrides
   // ClassWizard generate virtual function overrides
   // {{AFX_VIRTUAL(COvlPrintToolPropertyPage)
   public:
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void PostNcDestroy();
   // }}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   // {{AFX_MSG(COvlPrintToolPropertyPage)
   virtual BOOL OnInitDialog();
   // }}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

class CPageLayoutRootPropertyPage : public CFvOverlayPropertyPageImpl
{
public:
   virtual HRESULT OnApply()
   {
      return S_OK;
   }

protected:
   virtual CPropertyPage *CreatePropertyPage()
   {
      return NULL;
   }
};

// {{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately
// before the previous line.

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_GENPROP_H
