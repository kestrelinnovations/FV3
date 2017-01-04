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

// IFavoriteTabs.cpp
//

#include "StdAfx.h"
#include "IFavoriteTabs.h"

#include "getobjpr.h"
#include "mainfrm.h"
#include "MapViewTabsCtrl.h"
#include "err.h"        // for ERR_report

IMPLEMENT_DYNCREATE(FavoriteTabs, CCmdTarget)


// {9E83296F-B7F3-48b3-8FDC-3CAFB948D4B1}
IMPLEMENT_OLECREATE(FavoriteTabs, "FalconView.FavoriteTabs", 0x9e83296f, 0xb7f3, 0x48b3, 
                    0x8f, 0xdc, 0x3c, 0xaf, 0xb9, 0x48, 0xd4, 0xb1);

FavoriteTabs::FavoriteTabs()
{
	EnableAutomation();
}

FavoriteTabs::~FavoriteTabs()
{
}

void FavoriteTabs::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(FavoriteTabs, CCmdTarget)
	//{{AFX_MSG_MAP(FavoriteTabs)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(FavoriteTabs, CCmdTarget)
	//{{AFX_DISPATCH_MAP(FavoriteTabs)
   DISP_FUNCTION(FavoriteTabs, "Add", Add, VT_I4, VTS_BSTR)
   DISP_FUNCTION(FavoriteTabs, "Rename", Rename, VT_I4, VTS_I4 VTS_BSTR)
   DISP_FUNCTION(FavoriteTabs, "Close", Close, VT_I4, VTS_I4)
   DISP_FUNCTION(FavoriteTabs, "CloseAllExcept", CloseAllExcept, VT_I4, VTS_I4)
   DISP_FUNCTION(FavoriteTabs, "SetCurrent", SetCurrent, VT_I4, VTS_I4)
   DISP_FUNCTION(FavoriteTabs, "GetCount", GetCount, VT_I4, VTS_PI4)
   DISP_FUNCTION(FavoriteTabs, "GetName", GetName, VT_I4, VTS_I4 VTS_PBSTR)
   DISP_FUNCTION(FavoriteTabs, "HideOverlay", HideOverlay, VT_I4, VTS_I4 VTS_I4 VTS_I4)
   DISP_FUNCTION(FavoriteTabs, "IsOverlayHidden", IsOverlayHidden, VT_I4, VTS_I4 VTS_I4 VTS_PI4)
   DISP_FUNCTION(FavoriteTabs, "GetCurrent", GetCurrent, VT_I4, VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFavoriteTabs to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {D78DCCBC-CE6B-4958-A0C9-E0A117A5BA8E}
static const IID IID_IFavoriteTabs = 
{ 0xd78dccbc, 0xce6b, 0x4958, { 0xa0, 0xc9, 0xe0, 0xa1, 0x17, 0xa5, 0xba, 0x8e } };


BEGIN_INTERFACE_MAP(FavoriteTabs, CCmdTarget)
	INTERFACE_PART(FavoriteTabs, IID_IFavoriteTabs, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FavoriteTabs message handlers

long FavoriteTabs::Add(LPCTSTR strTabName)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::Add - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::Add - Favorite tabs is NULL");
      return FAILURE;
   }

   pFavoriteTabs->AddTab();
   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   pFavoriteTabs->GetCtrl()->RenameTab(nNumTabs - 1, std::string(strTabName));

   return SUCCESS;
}

long FavoriteTabs::Rename(int nIndex, LPCTSTR strTabName)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::Rename - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::Rename - Favorite tabs is NULL");
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::Rename - nIndex out of range");
      return FAILURE;
   }

   pFavoriteTabs->GetCtrl()->RenameTab(nIndex, std::string(strTabName));
   return SUCCESS;
}

long FavoriteTabs::Close(int nIndex)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::Close - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::Close - Favorite tabs is NULL");
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::Close - nIndex out of range");
      return FAILURE;
   }

   pFavoriteTabs->GetCtrl()->Close(nIndex);
   return SUCCESS;
}

long FavoriteTabs::CloseAllExcept(int nIndex)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::CloseAllExcept - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::CloseAllExcept - Favorite tabs is NULL");
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::CloseAllExcept - nIndex out of range");
      return FAILURE;
   }

   pFavoriteTabs->GetCtrl()->CloseOtherTabs(nIndex);
   return SUCCESS;
}

long FavoriteTabs::SetCurrent(int nIndex)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::SetCurrent - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::SetCurrent - Favorite tabs is NULL");
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::SetCurrent - nIndex out of range");
      return FAILURE;
   }

   pFavoriteTabs->GetCtrl()->GotoTab(nIndex);
   return SUCCESS;
}

long FavoriteTabs::GetCount(int* pCount)
{
   *pCount = 0;

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::GetCount - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::GetCount - Favorite tabs is NULL");
      return FAILURE;
   }

   *pCount = pFavoriteTabs->GetCtrl()->GetNumTabs();
   return SUCCESS;
}

long FavoriteTabs::GetName(int nIndex, BSTR* pTabName)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::GetName - CMainFrame is NULL");
      *pTabName = _bstr_t("").Detach();
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::GetName - Favorite tabs is NULL");
      *pTabName = _bstr_t("").Detach();
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::GetName - nIndex out of range");
      *pTabName = _bstr_t("").Detach();
      return FAILURE;
   }

   *pTabName = _bstr_t(pFavoriteTabs->GetCtrl()->GetTabName(nIndex)).Detach();
   return SUCCESS;
}

long FavoriteTabs::HideOverlay(int nIndex, int nOverlayHandle, int bHide)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::HideOverlay - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::HideOverlay - Favorite tabs is NULL");
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::HideOverlay - nIndex out of range");
      return FAILURE;
   }

   pFavoriteTabs->GetCtrl()->HideOverlay(nIndex, nOverlayHandle, bHide);
   return SUCCESS;
}

long FavoriteTabs::IsOverlayHidden(int nIndex, int nOverlayHandle, int* pbIsHidden)
{
   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame == NULL)
   {
      ERR_report("FavoriteTabs::HideOverlay - CMainFrame is NULL");
      return FAILURE;
   }

   CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
   if (pFavoriteTabs == NULL)
   {
      ERR_report("FavoriteTabs::HideOverlay - Favorite tabs is NULL");
      return FAILURE;
   }

   UINT nNumTabs = pFavoriteTabs->GetCtrl()->GetNumTabs();
   if (nIndex < 0 || (UINT)nIndex >= nNumTabs)
   {
      ERR_report("FavoriteTabs::HideOverlay - nIndex out of range");
      return FAILURE;
   }

   *pbIsHidden = pFavoriteTabs->GetCtrl()->IsOverlayHidden(nIndex, nOverlayHandle);
   if( *pbIsHidden == FAILURE )
   {
	   return FAILURE;
   }
   return SUCCESS;
}

long FavoriteTabs::GetCurrent(int* pIndex)
{
	*pIndex = -1;

	CMainFrame *pFrame = fvw_get_frame();
	if (pFrame == NULL)
	{
		ERR_report("FavoriteTabs::GetCurrent - CMainFrame is NULL");
		return FAILURE;
	}

	CMapViewTabsBar *pFavoriteTabs = pFrame->GetMapViewTabsBar();
	if (pFavoriteTabs == NULL)
	{
		ERR_report("FavoriteTabs::GetCurrent - Favorite tabs is NULL");
		return FAILURE;
	}

	*pIndex = pFavoriteTabs->GetCtrl()->GetCurrentTab();
	return SUCCESS;
}