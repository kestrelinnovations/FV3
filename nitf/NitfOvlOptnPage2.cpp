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

// NitfOvlOptnPage2.cpp : implementation file
//


#include "stdafx.h"
#include "..\\resource.h"
#include "NITF.h"
#include "param.h"
#include "NitfOvlOptnPages.h"
#include "NitfOvlOptnPage2.h"
#include "factory.h"

/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnPage2 property page

IMPLEMENT_DYNCREATE(CNitfOvlOptnPage2, COverlayPropertyPage)

CNitfOvlOptnPage2::CNitfOvlOptnPage2() : COverlayPropertyPage(FVWID_Overlay_NitfFiles, CNitfOvlOptnPage2::IDD)
{
	//{{AFX_DATA_INIT(CNitfOvlOptnPage2)
	//}}AFX_DATA_INIT

}

CNitfOvlOptnPage2::~CNitfOvlOptnPage2()
{

}

void CNitfOvlOptnPage2::DoDataExchange(CDataExchange* pDX)
{
	COverlayPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNitfOvlOptnPage2)
	DDX_Check(pDX, IDC_NITF_BOUNDING_AREAS, m_show_bounds);
	DDX_Check(pDX, IDC_NITF_HIDE_DURING_SCROLL, m_hide_during_scroll);
   DDX_Check( pDX, IDC_NITF_FILLED_ONLY_NO_EDITOR, m_bImagesOnlyNoEditor );
//	DDX_Check(pDX, IDC_TOGGLE, m_toggle_overlay);
	DDX_Control(pDX, IDC_NITF_SOURCES_FILES_DLG, m_NITF_files);
	DDX_Control(pDX, IDC_NITF_SOURCES_PATHS_DLG, m_NITF_paths);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNitfOvlOptnPage2, COverlayPropertyPage)
	//{{AFX_MSG_MAP(CNitfOvlOptnPage2)
	ON_BN_CLICKED( IDC_NITF_BOUNDING_AREAS, OnModified )
	ON_BN_CLICKED( IDC_TOGGLE, OnToggleOverlay )
   ON_BN_CLICKED( IDC_NITF_HIDE_DURING_SCROLL, OnModified )
   ON_BN_CLICKED( IDC_NITF_FILLED_ONLY_NO_EDITOR, OnModified )
	ON_CBN_SELCHANGE( IDC_LABEL_THRESHOLD, OnModified )
	ON_CBN_SELCHANGE( IDC_THRESHOLD, OnModified )
   ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNitfOvlOptnPage2 message handlers

BOOL CNitfOvlOptnPage2::OnInitDialog()
{
	// Bug 1556
	// An hourglass cursor should be added to the NITF options page in OnInitDialog
	//	since attaching to the database, when necessary, can be lengthy.
	CWaitCursor wait;

  // Icon/frame display threshold
   m_display_threshold = PRM_get_registry_string("NitfFile", "NITFDisplayAbove", "1:5 M");

   // Label threshold
   m_label_threshold =  PRM_get_registry_string("NitfFile", "NITFLabelThreshold", "1:250 K");

	CString sdata = PRM_get_registry_string("NitfFile", "ShowBounds", "Y");
   m_show_bounds = sdata.Compare("Y") == 0;

	sdata = PRM_get_registry_string("NitfFile", "HideDuringScroll", "Y");
	m_hide_during_scroll = sdata.Compare("Y") == 0;

	sdata = PRM_get_registry_string("NitfFile", "ImagesOnlyNoEditor", "N");
	m_bImagesOnlyNoEditorOriginal = m_bImagesOnlyNoEditor = sdata.Compare("Y") == 0;

	COverlayPropertyPage::OnInitDialog();

	sdata = PRM_get_registry_string("Main", "USER_DATA");
	sdata += "\\NITF";

   m_NITF_paths.SetControlMode( SOURCES_CTRL_CONTROL_MODE_PATHS );
	m_NITF_paths.Initialize( NULL, NULL, sdata );

   m_NITF_files.SetControlMode( SOURCES_CTRL_CONTROL_MODE_SINGLE_FILES );
	m_NITF_files.Initialize( NULL, NULL, sdata );

//	UpdateData( FALSE );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}  // End of CNitfOvlOptnPage2::OnInitDialog


void CNitfOvlOptnPage2::OnOK()
{
	UpdateData(TRUE);

   CString sdata;

   // Check for new images-only if no editor option
	sdata = PRM_get_registry_string( "NitfFile", "ImagesOnlyNoEditorWarning", "Y" );
   if ( sdata.Compare( _T("Y") ) == 0
      && m_bImagesOnlyNoEditor && !m_bImagesOnlyNoEditorOriginal )
   {
      CNITFNoEditorWarningDlg dlg;
      if ( IDOK != dlg.DoModal() )
         return;        // Cancelled

      PRM_set_registry_string( "NitfFile", "ImagesOnlyNoEditorWarning",
         dlg.m_iDontShowMsgAgain != BST_UNCHECKED ? "N" : "Y" );
   }

	// Icon/frame display threshold
	sdata = PRM_get_registry_string("NitfFile", "NITFDisplayAbove", "1:5 M");
	if ( m_display_threshold.Compare( sdata ) != 0 )
	{
		PRM_set_registry_string("NitfFile", "NITFDisplayAbove", m_display_threshold);
	}

	// Label display threshold
	sdata = PRM_get_registry_string("NitfFile", "NITFLabelThreshold", "1:250 K");
	if ( m_label_threshold.Compare( sdata ) != 0 )
	{
		PRM_set_registry_string("NitfFile", "NITFLabelThreshold", m_label_threshold);
	}
#if 1
   C_nitf_ovl::s_bParamsUpdate = TRUE;
#else
	PRM_set_registry_string("NitfFile", "DisplayDirty", "Y");
#endif

   PRM_set_registry_string("NitfFile", "ShowBounds", m_show_bounds ? "Y" : "N" );

	PRM_set_registry_string("NitfFile", "HideDuringScroll", m_hide_during_scroll ? "Y" : "N");

   PRM_set_registry_string("NitfFile", "ImagesOnlyNoEditor", m_bImagesOnlyNoEditor ? "Y" : "N");
   if ( C_nitf_ovl::s_pNITFOvl != NULL )
      C_nitf_ovl::s_pNITFOvl->m_eImagesOnlyNoEditor = m_bImagesOnlyNoEditor
         ? C_nitf_ovl::IMAGES_ONLY_NO_EDITOR_TRUE : C_nitf_ovl::IMAGES_ONLY_NO_EDITOR_FALSE;

	COverlayPropertyPage::OnOK();
}  // End of CNitfOvlOptnPage2::OnOK()


void CNitfOvlOptnPage2::OnDestroy()
{
   if ( C_nitf_ovl::s_pNITFOvl != NULL )
      C_nitf_ovl::s_pNITFOvl->m_bReload = TRUE;

	OVL_get_overlay_manager()->invalidate_all( FALSE );

}  // End of CNitfOvlOptnPage2::OnDestory()

// End of CNitfOvlOptnPage2.cpp

