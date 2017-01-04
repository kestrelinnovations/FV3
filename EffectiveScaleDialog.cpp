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

// EffectiveScaleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EffectiveScaleDialog.h"
#include "utils.h"
#include "maps.h"
#include "MAP.H"
#include "mapview.h"
#include "getobjpr.h"
#include "MapEngineCOM.h"

// CEffectiveScaleDialog dialog

IMPLEMENT_DYNAMIC(CEffectiveScaleDialog, CDialog)

CEffectiveScaleDialog::CEffectiveScaleDialog(CWnd* pParent /*=NULL*/)
: CDialog(CEffectiveScaleDialog::IDD, pParent)
   , m_nEffectiveScale(0), m_nMinEffectiveScale(0), m_nMaxEffectiveScale(0),
   m_bCancelled(false)
{

}

CEffectiveScaleDialog::~CEffectiveScaleDialog()
{
}

void CEffectiveScaleDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_SCALE, m_nEffectiveScale);
   DDX_Control(pDX, IDC_SCALE, m_scaleCtrl);
}

BOOL CEffectiveScaleDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   // get current map and map engine
   //
   ViewMapProj *pCurrMap = UTL_get_current_view_map();
   MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   MapEngineCOM *pMapEngine = NULL;
   if (pMapView != NULL)
      pMapEngine = pMapView->get_map_engine();

   if (pCurrMap != NULL && pMapEngine != NULL)
   {
      double dScaleFactor = 1.0;
      if (pCurrMap->requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
         pMapEngine->GetToScaleFactor(&dScaleFactor);

      // get the current scale's denominator
      double d;
      MAP_scale_t_to_scale_denominator(pCurrMap->scale(), &d);

      m_nEffectiveScale = static_cast<int>(d * dScaleFactor * (100.0/pCurrMap->actual_zoom_percent()) + 0.5);

      m_nMinEffectiveScale = static_cast<int>(d * dScaleFactor * (100.0/MapView::MAX_ZOOM_PERCENT) + 0.5);
      m_nMaxEffectiveScale = static_cast<int>(d * dScaleFactor * (100.0/MapView::MIN_ZOOM_PERCENT) + 0.5);

      UpdateData(FALSE);
   }

   m_scaleCtrl.SetSel(0, -1);
   m_scaleCtrl.SetFocus();

   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CEffectiveScaleDialog::OnOK()
{
   UpdateData();

   if (m_nEffectiveScale < m_nMinEffectiveScale || m_nEffectiveScale > m_nMaxEffectiveScale)
   {
      CString msg;
      msg.Format("Effective scale must be between %s and %s", MAP_convert_scale_denominator_to_scale_string(m_nMinEffectiveScale), 
         MAP_convert_scale_denominator_to_scale_string(m_nMaxEffectiveScale));
      AfxMessageBox(msg);
      return;
   }

   ViewMapProj *pCurrMap = UTL_get_current_view_map();
   MapView *pMapView = static_cast<MapView *>(UTL_get_active_non_printing_view());
   MapEngineCOM *pMapEngine = NULL;
   if (pMapView != NULL)
      pMapEngine = pMapView->get_map_engine();

   if (pCurrMap != NULL && pMapEngine != NULL)
   {
      double d;
      MAP_scale_t_to_scale_denominator(pCurrMap->scale(), &d);

      if (pCurrMap->requested_zoom_percent() == TO_SCALE_ZOOM_PERCENT)
      {
         pMapEngine->SetToScaleFactor(m_nEffectiveScale / d);
         pMapEngine->SetCurrentMapInvalid();
         pMapView->invalidate_view();
      }
      else
      {
         const int nZoomPercent = static_cast<int>((d * 100.0)/m_nEffectiveScale + 0.5);
         pMapView->ChangeZoomPercent(nZoomPercent);
      }
   }

   CDialog::OnOK();
}

void CEffectiveScaleDialog::OnCancel()
{
   m_bCancelled = true;
   CDialog::OnCancel();
}

BEGIN_MESSAGE_MAP(CEffectiveScaleDialog, CDialog)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_EN_KILLFOCUS(IDC_SCALE, &CEffectiveScaleDialog::OnEnKillfocusScale)
END_MESSAGE_MAP()


// CEffectiveScaleDialog message handlers

void CEffectiveScaleDialog::OnEnKillfocusScale()
{
   if (!m_bCancelled)
      UpdateData();
}
LRESULT CEffectiveScaleDialog::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

   return 1;
}

