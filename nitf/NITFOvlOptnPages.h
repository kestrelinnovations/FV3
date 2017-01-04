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

// NitfOvlOptnPages.h - Declarations for the CNitfOvlOptn<General,Files,Paths>Page classes.



//////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "NITFSourcesDlg.h"
//}}AFX_INCLUDES

#pragma once

#include "common.h"
#include "overlay_pp.h"
#include "optndlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNITFNoEditorWarningDlg dialog

class CNITFNoEditorWarningDlg : public CDialog
{
	DECLARE_DYNCREATE( CNITFNoEditorWarningDlg )

// Construction
public:
	CNITFNoEditorWarningDlg();

// Dialog Data
	enum { IDD = IDD_OVL_NITF_NO_EDITOR_WARNING };
	CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/overlays/Tactical_Imagery_Overlay.htm";}
	INT	m_iDontShowMsgAgain;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNITFNoEditorWarningDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNITFNoEditorWarningDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBoundingAreas();
afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // End of CNITFNoEditorWarningDlg class

class CNitfImageryRootPropertyPage : public CFvOverlayPropertyPageImpl
{
public:
   virtual HRESULT OnApply()
   {
      return S_OK;
   }
protected:
   virtual CPropertyPage *CreatePropertyPage() { return NULL; }
};

/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnGeneralPage dialog

class CNitfOvlOptnGeneralPage : public COverlayPropertyPage
{
	DECLARE_DYNCREATE( CNitfOvlOptnGeneralPage )

// Construction
public:
	CNitfOvlOptnGeneralPage();
	~CNitfOvlOptnGeneralPage();

// Dialog Data
	//{{AFX_DATA(CNitfOvlOptnGeneralPage)
	enum { IDD = IDD_OVL_NITF_OPT_GENERAL_DLG };

	INT	m_iShowBounds;
	INT	m_iHideDuringScroll;
   INT   m_iSecondaryImages;
   INT   m_iNewExplorerDisplayOnly;
   INT   m_iNewExplorerCenterMapAlways;
   INT   m_iNewExplorerCenterMapAsk;
   INT   m_iNewExplorerNewDisplayTabAlways;
   INT   m_iNewExplorerNewDisplayTabAsk;
   INT   m_iImagesOnlyNoEditor;
   INT   m_iImagesOnlyNoEditorOriginal;
   BOOL  m_bImagesOnlyNoEditorChanged;
   INT   m_iOnlyFilteredObjects;
   BOOL  m_bOnlyFilteredObjectsChanged;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNitfOvlOptnGeneralPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNitfOvlOptnGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBoundingAreas();
	virtual void OnOK();
   virtual void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   VOID InitVars();

  	afx_msg VOID OnClickedImagesOnlyNoEditor();
   afx_msg VOID OnClickedOnlyFilteredObjects();
   afx_msg VOID OnClickedResetAllDisplayAdjustments();

   CString GetRegistryValue( LPCSTR pszKeyName, LPCSTR pszDefaultValue );
   VOID SetRegistryValue( LPCSTR pszKeyName, LPCSTR pszValue );
   BOOL GetRegistryValue( LPCSTR pszKeyName, BOOL bDefault );
   VOID SetRegistryValue( LPCSTR pszKeyName, BOOL bValue );
   INT GetRegistryCheckValue( LPCSTR pszKeyName, INT iDefault );
   VOID SetRegistryCheckValue( LPCSTR pszKeyName, INT iValue );

}; // End of CNitfOvlGeneralPage class

class CNitfDisplayOptionsPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CNitfOvlOptnGeneralPage(); }
};


/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnFilesPage dialog

class CNitfOvlOptnFilesPage : public COverlayPropertyPage
{
	DECLARE_DYNCREATE( CNitfOvlOptnFilesPage )

// Construction
public:
	CNitfOvlOptnFilesPage();
	~CNitfOvlOptnFilesPage();

// Dialog Data
	//{{AFX_DATA(CNitfOvlOptnFilesPage)
	enum { IDD = IDD_OVL_NITF_OPT_FILES_DLG };

	CNITFSourcesDlg	m_NITF_files;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNitfOvlOptnFilesPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNitfOvlOptnFilesPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBoundingAreas();
	virtual void OnOK();
   virtual void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE( CNITFSourcesDlgEventSink )

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CNITFSourcesDlgEventSink)
	afx_msg void OnHelp( LONG lHelpType );
   afx_msg void OnNotifyChange();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
   DWORD m_dwSourcesDlgEventsCookie;

}; // End of CNitfOvlFilesPage class

class CNitfSingleFilesPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CNitfOvlOptnFilesPage(); }
};

/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnPathsPage dialog

class CNitfOvlOptnPathsPage : public COverlayPropertyPage
{
	DECLARE_DYNCREATE( CNitfOvlOptnPathsPage )

// Construction
public:
	CNitfOvlOptnPathsPage();
	~CNitfOvlOptnPathsPage();

// Dialog Data
	//{{AFX_DATA(CNitfOvlOptnPathsPage)
	enum { IDD = IDD_OVL_NITF_OPT_PATHS_DLG };

   CNITFSourcesDlg	m_NITF_paths;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNitfOvlOptnPathsPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNitfOvlOptnPathsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBoundingAreas();
	virtual void OnOK();
   virtual void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // End of CNitfOvlPathsPage class

class CNitfDirectoryPathsPropertyPage : public CFvOverlayPropertyPageImpl
{
protected:
   virtual CPropertyPage *CreatePropertyPage() { return new CNitfOvlOptnPathsPage(); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// End of NITFOvlOptnPages.h