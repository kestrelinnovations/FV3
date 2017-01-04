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



// optndlg.cpp : implementation file
//

#include "stdafx.h"
#include "OvlMgrDl.h"
#include "optndlg.h"
#include "OverlayPropertyPageCOM.h"
#include "..\getobjpr.h"
#include "Common\SafeArray.h"
#include "FvCore\Include\GuidStringConverter.h"
#include "ovl_mgr.h"

// property page headers
#include "..\factory.h"
#include "..\MovingMapOverlay\gpsstyle.h"
#include "..\MovingMapOverlay\PredictivePathOptionsPage.h"
#include "..\MovingMapOverlay\BullseyeOptionsPage.h"
#include "..\MovingMapOverlay\MovingMapPropertyPageGroup.h"
#include "..\MovingMapOverlay\factory.h"
#include "..\MovingMapoverlay\AuxDataOptionsPage.h"
#include "..\grid_map\grid_pp.h"
#include "..\grid_map\factory.h"
#include "..\PrintToolOverlay\GenProp.h"
#include "..\shp\shp_pp.h"
#include "..\pointex\PntExpPP.h"
#include "..\pointex\factory.h"
#include "..\PrintToolOverlay\PageProp.h"
#include "..\PrintToolOverlay\SCProp.h"
#include "..\PrintToolOverlay\ACProp.h"
#include "..\PrintToolOverlay\factory.h"
#include "..\shp\env_pp.h"
#include "..\shp\factory.h"
#include "..\shp\BoundariesOverlayPropertyPage.h"
#include "..\contour\contour_pp.h"
#include "..\contour\factory.h"
#include "..\scalebar\scaledlg.h"
#include "..\scalebar\factory.h"
#include "..\ar_edit\aredt_pp.h"
#include "..\SkyViewOverlay\factory.h"
#include "..\SkyViewOverlay\SkyViewOptionsPage.h"
#include "mov_sym.h"
#include "..\ar_edit\factory.h"
#include "..\TAMask\factory.h"
#ifdef SINGLE_PAGE_NITF_OPTIONS
#include "..\nitf\NitfOvlOptnPage2.h"
#else
#include "..\nitf\NitfOvlOptnPages.h"
#endif
#include "..\nitf\factory.h"
#include "FalconView/localpnt/lp_pp.h"
#include "..\TacticalModel\TacModel_pp.h"
#include "..\TacticalModel\factory.h"

const CString RegKeyFalconViewMain = "Software\\XPlan\\FalconView\\Workspace\\OptionsDialog";
const CString RegValueName = "Window";

// CFvOverlayPropertySheet
//

IMPLEMENT_DYNAMIC(CFvOverlayPropertySheet, CPropertySheet)

	CFvOverlayPropertySheet::CFvOverlayPropertySheet() : CPropertySheet(""), m_pEventObserver(NULL)
{
	m_psh.dwFlags |= PSH_WIZARD;
	m_psh.dwFlags &= ~PSH_HASHELP;
}

BEGIN_MESSAGE_MAP(CFvOverlayPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CFvOverlayPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFvOverlayPropertySheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// clean up the property sheet to our liking
	{
		GetDlgItem(ID_WIZBACK)->ShowWindow(FALSE);
		GetDlgItem(ID_WIZNEXT)->ShowWindow(FALSE);
		GetDlgItem(IDCANCEL)->ShowWindow(FALSE);
		GetDlgItem(IDHELP)->ShowWindow(FALSE);

		// divider bar that is a part of the property sheet, control ID
		// found using Spy++
		GetDlgItem(0x3026)->ShowWindow(FALSE);
	}

	return bResult;
}

BOOL CFvOverlayPropertySheet::OnApply()
{
	return GetPage(0)->OnApply();
}

BOOL CFvOverlayPropertySheet::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F1 && m_pEventObserver != NULL)
	{
		m_pEventObserver->OnPropertyPageHelp();
		return TRUE;
	}
	else if (pMsg->message == WM_HELP || pMsg->message == 0x4D)
	{
		// We already handled help from the F1 key press.  There is no need to
		// handle it again.  This can happen if the focus was on an ActiveX
		// control contained in the options dialog.
		return TRUE;
	}

	return CPropertySheet::PreTranslateMessage(pMsg);
}

LRESULT CFvOverlayPropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == PSM_CHANGED && m_pEventObserver != NULL)
	{
		m_pEventObserver->OnPropertyPageModified( );
	}

	return CPropertySheet::WindowProc(message, wParam, lParam);
}

// CFvOverlayPropertyPageImpl
//

HRESULT CFvOverlayPropertyPageImpl::OnCreate(long hWndParent, CComObject<CPropertyPageEventObserver> *pEventObserver)
{
	m_pPropertyPage = CreatePropertyPage();
	if (m_pPropertyPage == NULL)
		return S_OK;

	m_propertySheet.AddPage(m_pPropertyPage);

	m_propertySheet.Create(CWnd::FromHandle(reinterpret_cast<HWND>(hWndParent)), WS_CHILD, 0);
	m_propertySheet.ModifyStyleEx(0,WS_EX_CONTROLPARENT);
	m_propertySheet.ModifyStyle(WS_BORDER, WS_TABSTOP | WS_EX_TRANSPARENT);

	// compute the offset of the top-left corner of the property page relative to the property sheet.  We will adjust the position
	// of the property sheet so that the top-left corner of the property page corresponds with the top-left corner of the parent window.
	CPoint pt(0, 0);
	m_propertySheet.GetPage(0)->ClientToScreen(&pt);
	m_propertySheet.ScreenToClient(&pt);

	m_propertySheet.AddPropertyPageEventObserver(pEventObserver);
	m_propertySheet.SetWindowPos(NULL, -pt.x, -pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

	return S_OK;
}

HRESULT CFvOverlayPropertyPageImpl::OnApply()
{
	m_propertySheet.OnApply();
	return S_OK;
}

// CPropertyPageEntry
//

CPropertyPageEntry::~CPropertyPageEntry()
{
	delete m_pOverlayPropPage;

	size_t nSize = m_childEntries.size();
	for (size_t i=0; i<nSize; ++i)
		delete m_childEntries[i];
}

void CPropertyPageEntry::InvalidateAssocs()
{
	size_t nSize = m_overlayAssocs.size();
	for (size_t i=0; i<nSize; ++i)
		OVL_get_overlay_manager()->InvalidateOverlaysOfType(m_overlayAssocs[i]);
}

// returns TRUE if all the overlay type's associated with this property page entry are enabled
BOOL CPropertyPageEntry::AllOverlayAssocsEnabled()
{
	size_t nSize = m_overlayAssocs.size();
	for (size_t i=0; i<nSize; ++i)
	{
		if (OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(m_overlayAssocs[i]) == NULL)
			return FALSE;
	}

	return TRUE;
}

BOOL CPropertyPageEntry::IsAssociatedWith(GUID overlayDescGuid)
{
	size_t nSize = m_overlayAssocs.size();
	for (size_t i=0; i<nSize; ++i)
	{
		if (m_overlayAssocs[i] == overlayDescGuid)
			return TRUE;
	}

	return FALSE;
}

CPropertyPageEntry *CPropertyPageEntry::GetFirstChild()
{
	m_nCrntChildIndex = 0;
	return GetNextChild();
}

CPropertyPageEntry *CPropertyPageEntry::GetNextChild()
{
	if ( m_nCrntChildIndex < m_childEntries.size())
		return m_childEntries[m_nCrntChildIndex++];

	return NULL;
}

void CPropertyPageEntry::CreatePropertyPage()
{
	if (m_containerWnd.GetSafeHwnd() == NULL)
	{
		m_containerWnd.Create(NULL, "", SS_OWNERDRAW | WS_HSCROLL | WS_VSCROLL | WS_CLIPCHILDREN | WS_CHILD, m_placementRect, m_pParentWnd, 0);
		m_containerWnd.ModifyStyleEx(0,WS_EX_CONTROLPARENT);

		m_pOverlayPropPage->OnCreate(reinterpret_cast<long>(m_containerWnd.GetSafeHwnd()), m_pEventObserver);
	}
}

void CPropertyPageEntry::ShowPropertyPage()
{
	CreatePropertyPage();
	m_containerWnd.ShowWindow(SW_SHOW);
}

void CPropertyPageEntry::HidePropertyPage()
{
	if (m_containerWnd.GetSafeHwnd() != NULL)
		m_containerWnd.ShowWindow(SW_HIDE);
}

void CPropertyPageEntry::MoveWindow(CRect rectPosition) 
{ 
	if (m_containerWnd.GetSafeHwnd() != NULL)
		m_containerWnd.MoveWindow(&rectPosition);
}

void CPropertyPageEntry::OnApply()
{
	if (m_pOverlayPropPage != NULL)
		m_pOverlayPropPage->OnApply();
}

void CPropertyPageEntry::OnHelp()
{
	CMainFrame *pFrame = fvw_get_frame();
	if (pFrame != NULL)
		pFrame->LaunchHtmlHelp(m_helpId, HELP_CONTEXT, m_helpFileName, m_helpURI);
}

void CLayerOvlPropertyPageEntry::OnHelp()
{
	m_pLayerOvlFactory->on_help();
}

/////////////////////////////////////////////////////////////////////////////
// OverlayOptionsDlg dialog

OverlayOptionsDlg::OverlayOptionsDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(OverlayOptionsDlg::IDD, pParent),
	m_initialOverlayDescGuid(GUID_NULL),
	m_initialPropertyPageUid(GUID_NULL),
	m_pCrntSelectedEntry(NULL)
{
	//{{AFX_DATA_INIT(OverlayOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

OverlayOptionsDlg::~OverlayOptionsDlg()
{
	const size_t nSize = m_propertyPageEntries.size();
	for (size_t i=0; i<nSize; ++i)
		delete m_propertyPageEntries[i];

	if (m_pPropertyPageEventObserver != NULL)
		m_pPropertyPageEventObserver->Release();
}

void OverlayOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OverlayOptionsDlg)
	DDX_Control(pDX, IDC_TREE, m_tree_ctrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OverlayOptionsDlg, CResizableDialog)
	//{{AFX_MSG_MAP(OverlayOptionsDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OverlayOptionsDlg message handlers

BOOL OverlayOptionsDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	m_tree_ctrl.SetWindowPos(&CWnd::wndTop,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// initialize the image list
	m_images.Create(16, 16, ILC_COLOR8, 20, 5);
	m_images.SetBkColor(RGB(255,255,255));
	m_tree_ctrl.SetImageList(&m_images, TVSIL_NORMAL);

	InitializePropertyPageEntries();

	// add the property page entries to the tree control
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = NULL;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	for(size_t i=0; i<m_propertyPageEntries.size(); i++)
	{
		CPropertyPageEntry *pCrntEntry = m_propertyPageEntries[i];

		const int nImagePos = m_images.Add(pCrntEntry->GetIconImage()->get_icon(16));

		tvInsert.item.pszText = (LPTSTR)(LPCTSTR)pCrntEntry->GetDisplayName();
		tvInsert.item.iImage = nImagePos;
		tvInsert.item.iSelectedImage = nImagePos;
		HTREEITEM parent = m_tree_ctrl.InsertItem(&tvInsert);

		m_tree_ctrl.SetItemData(parent, reinterpret_cast<DWORD_PTR>(pCrntEntry));

		// add children
		CPropertyPageEntry *pChildEntry = pCrntEntry->GetFirstChild();
		while (pChildEntry != NULL)
		{
			int nChildImagePos = nImagePos;
			if (pChildEntry->GetIconImage() != NULL)
				nChildImagePos = m_images.Add(pChildEntry->GetIconImage()->get_icon(16));

			HTREEITEM child_handle = m_tree_ctrl.InsertItem(
				TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, pChildEntry->GetDisplayName(),
				nChildImagePos, nChildImagePos , 0, 0, 0, parent, NULL);

			m_tree_ctrl.SetItemData(child_handle, reinterpret_cast<DWORD_PTR>(pChildEntry));

			if ( IsInitialPage(pChildEntry) )
			{
				m_tree_ctrl.Select(child_handle, TVGN_CARET);
				m_tree_ctrl.Expand(child_handle, TVE_EXPAND);
			}

			pChildEntry = pCrntEntry->GetNextChild();
		}

		if ( IsInitialPage(pCrntEntry) )
		{
			m_tree_ctrl.Select(parent, TVGN_CARET);
			m_tree_ctrl.Expand(parent, TVE_EXPAND);
		}
	}

	GotoDlgCtrl(&m_tree_ctrl);

	// set the edit control's (IDC_TITLE) font
	CFont font;
	font.CreateFont(12,0,0,0,FW_BOLD,FALSE,FALSE,0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	GetDlgItem(IDC_TITLE)->SetFont(&font);

	// initially disable the apply button until a change has been made
	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

	AddAnchor(IDC_TREE, TOP_LEFT, BOTTOM_LEFT);
	AddAnchor(IDC_PLACEMENT, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_BAR, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT);
	AddAnchor(IDC_APPLY, BOTTOM_RIGHT);
	AddAnchor(ID_HELP, BOTTOM_RIGHT);

	Invalidate();

	EnableSaveRestore(HKEY_CURRENT_USER, (LPCTSTR) RegKeyFalconViewMain, (LPCTSTR) RegValueName);

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool OverlayOptionsDlg::IsInitialPage(CPropertyPageEntry* pEntry)
{
	// if a specific property page is specified to be opened initially, then expand the item
	// OR if the current entry is associated with the requested initial overlay type then select and expand the item
	return pEntry->GetPropertyPageUid() != GUID_NULL && pEntry->GetPropertyPageUid() == m_initialPropertyPageUid ||
		pEntry->IsAssociatedWith(m_initialOverlayDescGuid);
}

void OverlayOptionsDlg::InitializePropertyPageEntries()
{
	CRect placementRect;
	GetDlgItem(IDC_PLACEMENT)->GetWindowRect(&placementRect);
	ScreenToClient(placementRect);

	// before adding elements to the tree we will first add them to a
	// list and then sort the entries since a tree control doesn't have a way to sort items
	CWnd *pParentWnd = this;

	CComObject<CPropertyPageEventObserver>::CreateInstance(&m_pPropertyPageEventObserver);
	m_pPropertyPageEventObserver->AddRef();
	m_pPropertyPageEventObserver->Initialize(this);

	// A typical entry in the OverlayOptionsConfig.xml will look like:
	/*
	<OverlayOptionsDialog>
	<OverlayOptionsPage clsid="DB9FC1E5-D09A-4607-8176-3BC14F2C501E"  <- object implementing the IFvOverlayOptionsPage interface
	displayName="Airports/Heliports"               <- display name shown in the overlay options dialog box when the page is selected
	iconFile="dafif\airport.ico"                  <- icon used in the tree control in the overlay options dialog box for the property page
	helpFilename="help\fvw.chm" helpId="123" />   <- help file and ID used when the Help button is pressed in the overlay options dialog box

	<!--- Use the OverlayAssoc tag(s) to associate this property page with one or more overlay types.
	This is used by the overlay options dialog to automatically choose an initial property
	page based on the current stack of overlays ---!>
	<OverlayAssoc clsid="8E03BDB7-BF38-4339-940F-14E128F521D5" />
	<OverlayAssoc clsid="8E03BDB7-BF38-4339-940F-14E128F521D5" />
	</OverlayOptionsPage>
	</OverlayOptionsDialog>
	*/

	// for each property page entry listed in the OverlayOptionsConfig.xml (we are just faking this for now until these
	// are moved out into COM objects)
	//
	IFvOverlayPropertyPage *pOverlayPropPage;
	CIconImage *pIconImage;
	CPropertyPageEntry* pEntry;
	CPropertyPageEntry* pParentEntry = NULL;

   try
   {
      IFvOverlayPropertyPagesConfig3Ptr spPropertyPagesConfig;
      CO_CREATE(spPropertyPagesConfig, CLSID_FvOverlayPropertyPagesConfig);

		if (spPropertyPagesConfig->Initialize() == S_OK)
		{
			long lRet = spPropertyPagesConfig->SelectAll();
			while (lRet)
			{
				if (COverlayTypeDescriptorList::IsRuntimeEnabled(spPropertyPagesConfig->RuntimeEnabledClsid) == FALSE)
				{
					lRet = spPropertyPagesConfig->MoveNext();
					continue;
				}

				CString displayName = (char *)spPropertyPagesConfig->DisplayName;
				COverlayPropertyPageCOM* pOverlayPropPage = new COverlayPropertyPageCOM(displayName, 
					spPropertyPagesConfig->PropertyPageGuid, spPropertyPagesConfig->PropertyPageClsid);

				CIconImage* pIconImage = CIconImage::load_images((char *)spPropertyPagesConfig->IconFilename);

				// See if the HelpFilename specifies the full path to a help file.
				// If not, we'll prefix the path with HD_DATA.
				CString helpFileName = (char *)spPropertyPagesConfig->HelpFilename;
				if (::GetFileAttributes(helpFileName) == INVALID_FILE_ATTRIBUTES)
				{
					helpFileName = PRM_get_registry_string("Main", "HD_DATA");
					helpFileName += '\\';
					helpFileName += (char *)spPropertyPagesConfig->HelpFilename;
				}

				pEntry = new CPropertyPageEntry(displayName, placementRect, pParentWnd,
					pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage, helpFileName, 
					spPropertyPagesConfig->PropertyPageHelpId, (char *)spPropertyPagesConfig->HelpURI);

				if (spPropertyPagesConfig->IsChildPage)
				{
					if (pParentEntry != NULL)
						pParentEntry->AddChildEntry(pEntry);
					else
						delete pEntry;
				}
				else
				{
					// check overlay assocs
					SAFEARRAY *psaOverlayAssocs = spPropertyPagesConfig->OverlayAssocs;
					BstrSafeArray saOverlayAssocs(psaOverlayAssocs);
					SafeArrayDestroy(psaOverlayAssocs);

					const int nNumElements = saOverlayAssocs.GetNumElements();
					for (int i=0; i<nNumElements; ++i)
					{
						string_utils::CGuidStringConverter guidConv((char*)_bstr_t(saOverlayAssocs[i]));
						pEntry->AddOverlayAssoc(guidConv.GetGuid());
					}

					// if one of the associated overlays is not enabled, then this property page entry will be skipped
					if (!pEntry->AllOverlayAssocsEnabled())
					{
						delete pEntry;
						pParentEntry = NULL;
					}
					else
					{
						m_propertyPageEntries.push_back(pEntry);
						pParentEntry = pEntry;
					}
				}

				lRet = spPropertyPagesConfig->MoveNext();
			}
		}
	}
	catch(_com_error &e)
	{
		CString msg = (char *)e.Description();
		AfxMessageBox(msg);
	}

	pOverlayPropPage = new CContourLinesPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\contour.ico");
	pEntry = new CPropertyPageEntry("Contour Lines", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Contour_Lines_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_ContourLines);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CCoordinateGridPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\grid.ico");
	pEntry = new CPropertyPageEntry("Coordinate Grid", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Coordinate_Grid_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_CoordinateGrid);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

   pOverlayPropPage = new CBoundariesOverlayPropertyPage();
   pIconImage = CIconImage::load_images("ovlmgr\\boundaries_overlay.ico");
   pEntry = new CPropertyPageEntry("Boundaries Overlay", placementRect, pParentWnd,
      pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
      "", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Boundaries_Overlay_Options.htm");
   pEntry->AddOverlayAssoc(FVWID_Overlay_Boundaries);
   pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;


   pOverlayPropPage = new CMovingMapRootPropertyPage();
   pIconImage = CIconImage::load_images("ovlmgr\\gps.ico");
   pParentEntry = new CPropertyPageEntry("Moving Map", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
      "", 0, "fvw.chm::\\fvw_core.chm::\\mvmp\\Moving_Map_General_Options.htm");
   pParentEntry->AddOverlayAssoc(FVWID_Overlay_MovingMapTrail);
   if (pParentEntry->AllOverlayAssocsEnabled())
   {
      pOverlayPropPage = new CMovingMapGeneralPropertyPage();
      pEntry = new CPropertyPageEntry("General", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
         "", 0, "fvw.chm::\\fvw_core.chm::\\mvmp\\Moving_Map_General_Options.htm");
      pParentEntry->AddChildEntry(pEntry);

	pOverlayPropPage = new CEnvironmentalPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\env.ico");
	pEntry = new CPropertyPageEntry("Environmental", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Environmental_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_Environmental);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

		pOverlayPropPage = new CMovingMapPredictivePathPropertyPage();
		pEntry = new CPropertyPageEntry("Predictive Path", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\fvw_core.chm::\\mvmp\\Moving_Map_Predictive_Path_Options.htm");
		pParentEntry->AddChildEntry(pEntry);

		pOverlayPropPage = new CMovingMapBullseyePropertyPage();
		pEntry = new CPropertyPageEntry("Bullseye", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\fvw_core.chm::\\mvmp\\Moving_Map_Ship_Bullseye_Options.htm");
		pParentEntry->AddChildEntry(pEntry);

		m_propertyPageEntries.push_back(pParentEntry);
	}
	else
		delete pParentEntry;

	pOverlayPropPage = new CNitfImageryRootPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\nitf.ico");
	pParentEntry = new CPropertyPageEntry("Tactical Imagery", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Tactical_Imagery_Overlay_Options_Display.htm");
	pParentEntry->AddOverlayAssoc(FVWID_Overlay_NitfFiles);
	if (pParentEntry->AllOverlayAssocsEnabled())
	{
		pOverlayPropPage = new CNitfDisplayOptionsPropertyPage();
		pEntry = new CPropertyPageEntry("Display Options", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Tactical_Imagery_Overlay_Options_Display.htm");
		pParentEntry->AddChildEntry(pEntry);

		pOverlayPropPage = new CNitfSingleFilesPropertyPage();
		pEntry = new CPropertyPageEntry("Single Imagery Files", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Tactical_Imagery_Overlay_Options_Single_File.htm");
		pParentEntry->AddChildEntry(pEntry);

		pOverlayPropPage = new CNitfDirectoryPathsPropertyPage();
		pEntry = new CPropertyPageEntry("Directory Paths", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Tactical_Imagery_Overlay_Options_Directory_Paths.htm");
		pParentEntry->AddChildEntry(pEntry);

		m_propertyPageEntries.push_back(pParentEntry);
	}
	else
		delete pParentEntry;

	pOverlayPropPage = new CPageLayoutRootPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\printovl.ico");
	pParentEntry = new CPropertyPageEntry("Page Layout", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\PageLayout.chm::\\Page_Layout_topics\\Page_Layout_Single_Page_Options.htm");
	pParentEntry->AddOverlayAssoc(FVWID_Overlay_PageLayout);
	if (pParentEntry->AllOverlayAssocsEnabled())
	{
		pOverlayPropPage = new CPageLayoutSinglePagePropertyPage();
		pEntry = new CPropertyPageEntry("Single Page", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\PageLayout.chm::\\Page_Layout_topics\\Page_Layout_Single_Page_Options.htm", PROPPAGEID_PageLayout_SinglePage);
		pParentEntry->AddChildEntry(pEntry);

		pOverlayPropPage = new CPageLayoutStripChartPropertyPage();
		pEntry = new CPropertyPageEntry("Strip Chart", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\PageLayout.chm::\\Page_Layout_topics\\Page_Layout_Strip_Chart_Options.htm", PROPPAGEID_PageLayout_StripChart);
		pParentEntry->AddChildEntry(pEntry);

		pOverlayPropPage = new CPageLayoutAreaChartPropertyPage();
		pEntry = new CPropertyPageEntry("Area Chart", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, NULL,
			"", 0, "fvw.chm::\\PageLayout.chm::\\Page_Layout_topics\\Page_Layout_Area_Chart_Options.htm", PROPPAGEID_PageLayout_AreaChart);
		pParentEntry->AddChildEntry(pEntry);

		m_propertyPageEntries.push_back(pParentEntry);
	}
	else
		delete pParentEntry;

   pOverlayPropPage = new CPointsPropertyPage();
   pIconImage = CIconImage::load_images("ovlmgr\\localpnt.ico");
   pEntry = new CPropertyPageEntry("Points", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
      "", 0, "fvw.chm::\\PointsEditor.chm::\\Points_Editor_topics\\Points_Overlay_Options.htm");
   pEntry->AddOverlayAssoc(FVWID_Overlay_Points);
   pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CPointExportPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\pntexprt.ico");
	pEntry = new CPropertyPageEntry("Point Export", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\pntxpt\\Point_Export_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_PointExport);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CScaleBarPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\scalebar.ico");
	pEntry = new CPropertyPageEntry("Scale Bar", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Scale_Bar_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_ScaleBar);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CShapeFilePropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\shp.ico");
	pEntry = new CPropertyPageEntry("Shape Files", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Shape_Files_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_ShapeFile);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CSkyviewPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\movsym.ico");
	pEntry = new CPropertyPageEntry("SkyView", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\3dview\\SkyView_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_SkyView);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CTerrainAvoidanceMaskPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\TAMask.ico");
	pEntry = new CPropertyPageEntry("Terrain Avoidance Mask", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\Terrain_Avoidance_Mask_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_TerrainAvoidanceMask);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CTrackOrbitPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\ar_edit.ico");
	pEntry = new CPropertyPageEntry("Track/Orbit", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\TrackOribitEditor.chm::\\Track_Oribit_Editor_topics\\Track_Orbit_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_TrackOrbit);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	pOverlayPropPage = new CTacticalModelPropertyPage();
	pIconImage = CIconImage::load_images("ovlmgr\\TacticalModel.ico");
	pEntry = new CPropertyPageEntry("Tactical Model Files", placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
		"", 0, "fvw.chm::\\fvw_core.chm::\\overlays\\TacticalModel_Overlay_Options.htm");
	pEntry->AddOverlayAssoc(FVWID_Overlay_TacticalModel);
	pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;

	// add the legacy plug-in overlay property pages
	//
	OVL_get_type_descriptor_list()->ResetEnumerator();
	while (OVL_get_type_descriptor_list()->MoveNext())
	{
		OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->m_pCurrent;

		CLayerOvlFactory *pLayerOvlFactory = dynamic_cast<CLayerOvlFactory *>(pOverlayTypeDesc->pOverlayFactory);
		if (pLayerOvlFactory != NULL)
		{
			if (!pLayerOvlFactory->get_prog_ID().IsEmpty())
			{
				pOverlayPropPage = new CLayerOvlPropertyPage(pLayerOvlFactory);
				pIconImage = pOverlayTypeDesc->pIconImage;
				CLayerOvlPropertyPageEntry *pEntry = new CLayerOvlPropertyPageEntry(pLayerOvlFactory,
					pOverlayTypeDesc->displayName, placementRect, pParentWnd, pOverlayPropPage, m_pPropertyPageEventObserver, pIconImage,
					"", 0, "");
				pEntry->AddOverlayAssoc(pOverlayTypeDesc->overlayDescriptorGuid);
				pEntry->AllOverlayAssocsEnabled() ? m_propertyPageEntries.push_back(pEntry) : delete pEntry;
			}
		}
	}

	std::sort(m_propertyPageEntries.begin(), m_propertyPageEntries.end(), CPropertyPageEntry::SortByDisplayName);
}

void OverlayOptionsDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hSelectedItem = m_tree_ctrl.GetSelectedItem();
	if (hSelectedItem != NULL)
	{
		// hide the current property page
		if (m_pCrntSelectedEntry != NULL)
			m_pCrntSelectedEntry->HidePropertyPage();

		// display the newly selected property page if it has changed
		CPropertyPageEntry *pSelectedEntry = reinterpret_cast<CPropertyPageEntry *>(m_tree_ctrl.GetItemData(hSelectedItem));
		if (pSelectedEntry != NULL && pSelectedEntry != m_pCrntSelectedEntry)
		{
			pSelectedEntry->ShowPropertyPage();

			// Resize the property page to the correct size.  This is in case the dialog was resized since the page was created.
			// Have to do this after showing the property page, since the window may not be created yet if first time shown.
			WINDOWPLACEMENT wp;
			GetDlgItem(IDC_PLACEMENT)->GetWindowPlacement(&wp);
			pSelectedEntry->MoveWindow(wp.rcNormalPosition);

			// Set the title in the dialog
			CString displayName;
			HTREEITEM hParentItem = m_tree_ctrl.GetParentItem(hSelectedItem);
			if (hParentItem != NULL)
			{
				CPropertyPageEntry *pParentEntry = reinterpret_cast<CPropertyPageEntry *>(m_tree_ctrl.GetItemData(hParentItem));

				displayName.Format("%s - %s", pParentEntry->GetDisplayName(), pSelectedEntry->GetDisplayName());
			}
			else
				displayName = pSelectedEntry->GetDisplayName();

			GetDlgItem(IDC_TITLE)->SetWindowText(displayName);

			// update the current property page
			m_pCrntSelectedEntry = pSelectedEntry;
		}
	}

	// make sure the focus stays on the list control after a key press rather than going directly to the property page
	GotoDlgCtrl(&m_tree_ctrl);

	*pResult = 0;
}

HBRUSH OverlayOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CResizableDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// make the background of the edit control, IDC_TITLE, transparent
	if (pWnd->GetDlgCtrlID() == IDC_TITLE)
	{
		hbr = GetSysColorBrush(COLOR_3DSHADOW);
		pDC->SetBkColor(GetSysColor(COLOR_3DSHADOW));
	}

	return hbr;
}

void OverlayOptionsDlg::OnApply()
{
	std::set<CPropertyPageEntry *> parentEntries;

	std::set<CPropertyPageEntry *>::iterator it = m_dirtyPropertyPageEntries.begin();
	while (it != m_dirtyPropertyPageEntries.end() )
	{
		CPropertyPageEntry *pEntry = *it;
		pEntry->OnApply();

		// If the property page entry's parent is non-NULL, then add it to the parent entries set.  We
		// will call OnApply on these parent entries after all dirty property page entries have been
		// handled.  This can be used to perform a single, final action for the entire collection of dirty pages (e.g.,
		// writing properties back to disk a single time instead of once for each dirty child page)
		CPropertyPageEntry *pParentEntry = pEntry->GetParentEntry();
		if ( pParentEntry != NULL )
			parentEntries.insert(pParentEntry);

		it++;
	}

	// handle any parent entries added above
	it = parentEntries.begin();
	while (it != parentEntries.end())
	{
		// make sure the parent has already been created
		(*it)->CreatePropertyPage();
		(*it)->OnApply();
		it++;
	}

	m_dirtyPropertyPageEntries.erase(m_dirtyPropertyPageEntries.begin(), m_dirtyPropertyPageEntries.end());

	// disable the apply button until a change has been made
	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}

void OverlayOptionsDlg::InvalidateAssociatedOverlays()
{
	HTREEITEM hTreeItem = m_tree_ctrl.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CPropertyPageEntry *pEntry = reinterpret_cast<CPropertyPageEntry *>(m_tree_ctrl.GetItemData(hTreeItem));
		if (pEntry != NULL)
			pEntry->InvalidateAssocs();
	}
}

HRESULT OverlayOptionsDlg::OnPropertyPageModified()
{
	HTREEITEM hTreeItem = m_tree_ctrl.GetSelectedItem();
	if (hTreeItem != NULL)
	{
		CPropertyPageEntry *pEntry = reinterpret_cast<CPropertyPageEntry *>(m_tree_ctrl.GetItemData(hTreeItem));
		if (pEntry != NULL)
			m_dirtyPropertyPageEntries.insert(pEntry);
	}

	GetDlgItem(IDC_APPLY)->EnableWindow();
	return S_OK;
}

HRESULT OverlayOptionsDlg::OnPropertyPageHelp()
{
	on_help();
	return S_OK;
}

void OverlayOptionsDlg::OnOK()
{
	OnApply();
	CResizableDialog::OnOK();
}

void OverlayOptionsDlg::OnHelp()
{
	on_help();
}

void OverlayOptionsDlg::on_help()
{
	HTREEITEM selected_item = m_tree_ctrl.GetSelectedItem();
	if (selected_item != NULL)
	{
		CPropertyPageEntry *pEntry = reinterpret_cast<CPropertyPageEntry *>(m_tree_ctrl.GetItemData(selected_item));
		if (pEntry != NULL)
			pEntry->OnHelp();
	}
}

BOOL OverlayOptionsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN && GetFocus() == &m_tree_ctrl)
			m_tree_ctrl.Expand(m_tree_ctrl.GetSelectedItem(),TVE_TOGGLE);
	}

	return CResizableDialog::PreTranslateMessage(pMsg);
}


LRESULT OverlayOptionsDlg::OnCommandHelp(WPARAM, LPARAM lParam)
{
	on_help();
	return TRUE;
}

void OverlayOptionsDlg::SetInitialPropertyPage(GUID overlayDescGuid, GUID propertyPageUid)
{
	m_initialOverlayDescGuid = overlayDescGuid;
	m_initialPropertyPageUid = propertyPageUid;
}

void OverlayOptionsDlg::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	ArrangeLayout();
	UpdateSizeGrip();

	CWnd* wndPlacement = GetDlgItem(IDC_PLACEMENT);
	
	if (wndPlacement != NULL)
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		wndPlacement->GetWindowPlacement(&wp);

		for(size_t i=0; i<m_propertyPageEntries.size(); i++)
		{
			CPropertyPageEntry *pCrntEntry = m_propertyPageEntries[i];
			pCrntEntry->MoveWindow(wp.rcNormalPosition);
		}
	}
}

BOOL OverlayOptionsDlg::OnEraseBkgnd(CDC* pDC) 
{
	EraseBackground(pDC);
	return TRUE;
}

void OverlayOptionsDlg::OnDestroy() 
{
	RemoveAllAnchors();

   CResizableDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// OCXContainerPage property page

IMPLEMENT_DYNCREATE(OCXContainerPage, CPropertyPage)

	OCXContainerPage::OCXContainerPage() :
CPropertyPage(OCXContainerPage::IDD)
{
	//{{AFX_DATA_INIT(OCXContainerPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

OCXContainerPage::~OCXContainerPage()
{
}

void OCXContainerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OCXContainerPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OCXContainerPage, CPropertyPage)
	//{{AFX_MSG_MAP(OCXContainerPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCXContainerPage message handlers

BEGIN_EVENTSINK_MAP(OCXContainerPage, CPropertyPage)
	ON_EVENT(OCXContainerPage, IDC_OCX_PROPERTY_CTRL, 1, OnModified, VTS_NONE)
	ON_EVENT(OCXContainerPage, IDC_OCX_PROPERTY_CTRL, 2, ApplyNow, VTS_NONE)
END_EVENTSINK_MAP()

BOOL OCXContainerPage::OnModified()
{
	return HandleOnModifiedEvent();
}

BOOL OCXContainerPage::HandleOnModifiedEvent()
{
	SetModified();
	return TRUE;
}

BOOL OCXContainerPage::ApplyNow()
{
	set_preference_string(m_control_wrapper.GetPreferences());
	OVL_get_overlay_manager()->invalidate_all();

	return TRUE;
}

BOOL OCXContainerPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// get the client rect for the property page.  The ActiveX
	// control will fill the entire property page.
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	CString prog_ID = get_prog_ID();

	// add the control with the give prog_ID
	if (m_control_wrapper.CreateControl(prog_ID,
		NULL, WS_VISIBLE | WS_TABSTOP, rect, this, IDC_OCX_PROPERTY_CTRL) == FALSE)
	{
		CString msg;
		msg.Format("Unable to create the ActiveX control with ProgID = %s", prog_ID);
		ERR_report(msg);
		return TRUE;
	}

	// get the preference string from the overlay that this
	//
	CString pref_string = get_preference_string();

	_bstr_t temp = (LPCTSTR)pref_string;
	m_control_wrapper.SetPreferences(temp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL OCXContainerPage::DestroyWindow()
{
	m_control_wrapper.DestroyWindow();

	return CPropertyPage::DestroyWindow();
}

void OCXContainerPage::OnOK()
{
	set_preference_string(m_control_wrapper.GetPreferences());
	CPropertyPage::OnOK();
}

bool COCXControlWrapper::GetIDOfName(OLECHAR FAR* MethodName, DISPID& dispid)
{
	IDispatchPtr dp = GetControlUnknown();
	if (dp == NULL)
		return false;

	HRESULT hr = dp->GetIDsOfNames(IID_NULL, &MethodName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);

	CString Msg;
	switch (hr)
	{
	case S_OK:
		return true;

	case E_OUTOFMEMORY:
		Msg.Format("Out of memory error calling %S on COCXControlWrapper", MethodName);
		break;

	case DISP_E_UNKNOWNNAME:
		Msg.Format("Unknown name %S on COCXControlWrapper", MethodName);
		break;

	case DISP_E_UNKNOWNLCID:
		Msg.Format("Unknown LCID calling %S on COCXControlWrapper", MethodName);
		break;
	}

	ERR_report(Msg);

	return false;
}

void COCXControlWrapper::SetPreferences(OLECHAR FAR* preference_string)
{
	DISPID dispid;
	OLECHAR FAR* MethodName = L"SetPreferences";
	if (GetIDOfName(MethodName, dispid))
	{
		long ret;

		static BYTE parameter_type_lst[] = VTS_PBSTR;
		InvokeHelper(dispid, DISPATCH_METHOD, VT_I4, (void*)&ret, parameter_type_lst, &preference_string);
	}

	return;
};

CString COCXControlWrapper::GetPreferences()
{
	DISPID dispid;
	OLECHAR FAR* MethodName = L"GetPreferences";
	if (GetIDOfName(MethodName, dispid))
	{
		_bstr_t bstrPreferences;
		long ret;

		static BYTE parameter_type_lst[] = VTS_PBSTR;
		InvokeHelper(dispid, DISPATCH_METHOD, VT_I4, (void*)&ret, parameter_type_lst, bstrPreferences.GetAddress());

		return CString((char *)bstrPreferences);
	}

	return "";
};

// CPropertyPageEventObserver
//

STDMETHODIMP CPropertyPageEventObserver::raw_InvalidateOverlay()
{
	// all overlays associated with the current property page will be invalidated
	m_pOverlayOptionsDlg->InvalidateAssociatedOverlays();
	return S_OK;
}

STDMETHODIMP CPropertyPageEventObserver::raw_OnPropertyPageModified()
{
	m_pOverlayOptionsDlg->OnPropertyPageModified();
	return S_OK;
}

STDMETHODIMP CPropertyPageEventObserver::raw_OnPropertyPageHelp()
{
	m_pOverlayOptionsDlg->OnPropertyPageHelp();
	return S_OK;
}

