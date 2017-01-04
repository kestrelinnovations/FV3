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

// TAMaskStatus.cpp: implementation of the CTAMaskStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\resource.h"
#include "factory.h"
#include "TAMaskStatus.h"
#include "TAMask.h"        // For access to TAMask overlay existance/status
#include "OvlFctry.h"
#include "factory.h"
#include "..\getobjpr.h"
#include "ovl_mgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTAMaskStatus::CTAMaskStatus()
{
   m_WarnColor = RGB(255, 0, 0);
   m_CautionColor = RGB(255, 255, 0);
   m_OKColor = RGB(0, 255, 0);
   m_ShowWarn = true;
   m_ShowCaution = true;
   m_ShowOK = true;
   m_TestAlt = 2500;
   m_WarnClearance = 100;
   m_CautionClearance = 300;
   m_OKClearance = 500;

	m_Altitude = -99999;
	m_CautionText = _T("");
	m_OKText = _T("");
	m_WarnText = _T("");
   m_ShowClearances = TRUE;
};

CTAMaskStatus::CTAMaskStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CTAMaskStatus::IDD, pParent)
{
   m_WarnColor = RGB(255, 0, 0);
   m_CautionColor = RGB(255, 255, 0);
   m_OKColor = RGB(0, 255, 0);
   m_ShowWarn = true;
   m_ShowCaution = true;
   m_ShowOK = true;
   m_TestAlt = 2500;
   m_WarnClearance = 100;
   m_CautionClearance = 300;
   m_OKClearance = 500;

	//{{AFX_DATA_INIT(TAMaskStatus)
	m_Altitude = -99999;
	m_CautionText = _T("");
	m_OKText = _T("");
	m_WarnText = _T("");
   m_ShowClearances = TRUE;
	//}}AFX_DATA_INIT
}

CTAMaskStatus::~CTAMaskStatus()
{
   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   if (PrefMgr != NULL)
   {
      try 
      {
         // Load the original settings from the registry
         PrefMgr->ReadFromRegistry("Software\\XPlan\\FalconView\\TAMask");

         // Put the settings we may have changed back into the preference object
         PrefMgr->SetValueINT("ShowClearances", m_ShowClearances);

         // Save the changed preferences and the originals back in the registry
         PrefMgr->WriteToRegistry("Software\\XPlan\\FalconView\\TAMask");
      }
      catch (...)
      {
         MessageBox("TAMask status window could not save preferences.");
      }
   }
}

BOOL CTAMaskStatus::OnInitDialog()
{
   // Set up the defaults
   set_preferences(NULL);

   CDialog::OnInitDialog();

   return TRUE;
}

void CTAMaskStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TAMaskStatus)
	DDX_Text(pDX, IDC_ALT, m_Altitude);
   DDV_MinMaxInt(pDX, m_Altitude, -30000, 300000);
	DDX_Text(pDX, IDC_CAUTION, m_CautionText);
	DDX_Text(pDX, IDC_OK, m_OKText);
	DDX_Text(pDX, IDC_WARN, m_WarnText);
   DDX_Check(pDX, IDC_SHOWCLEARANCE, m_ShowClearances);
	//}}AFX_DATA_MAP
}

void CTAMaskStatus::OnOK() 
{
   if (UpdateData(TRUE))
   {
      C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();

      if (ovl_mgr)
      {
         C_TAMask_ovl *pOvl = (C_TAMask_ovl*)ovl_mgr->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask);
   
         if (pOvl)
         {
            pOvl->SetAltitude(static_cast<float>(m_Altitude), NULL);
         }
      }
      else
         MessageBox("Unable to get pointer to FalconView overlay manager");

      // The overlay will have instructed us that the altiutude was set from GPS, 
      // Correct the display by a forced display here.
      set_altitude(m_Altitude);
   }


   // We do not want the dialog to close -- just accept the altitude
	//CDialog::OnOK();
}


bool CTAMaskStatus::set_preferences(LPCTSTR preference_string)
{
   IXMLPrefMgrPtr PrefMgr;
   PrefMgr.CreateInstance(CLSID_XMLPrefMgr);

   if (PrefMgr == NULL)
      return false;

   if (preference_string != NULL)
   {
      try 
      {
         // XML DOM throws if the string is not XML
         PrefMgr->SetXMLString((LPCTSTR)preference_string);

         m_WarnColor = PrefMgr->GetValueINT("WarnColor", m_WarnColor);
         m_CautionColor = PrefMgr->GetValueINT("CautionColor", m_CautionColor);
         m_OKColor = PrefMgr->GetValueINT("OKColor", m_OKColor);

         m_ShowWarn = PrefMgr->GetValueBOOL("ShowWarnLevel", (m_ShowWarn)?VARIANT_TRUE:VARIANT_FALSE) != VARIANT_FALSE;
         m_ShowCaution = PrefMgr->GetValueBOOL("ShowCautionLevel", (m_ShowCaution)?VARIANT_TRUE:VARIANT_FALSE) != VARIANT_FALSE;
         m_ShowOK = PrefMgr->GetValueBOOL("ShowOKLevel", (m_ShowOK)?VARIANT_TRUE:VARIANT_FALSE) != VARIANT_FALSE;

         m_TestAlt = PrefMgr->GetValueINT("TestAlt", m_TestAlt);
         m_WarnClearance = PrefMgr->GetValueINT("WarnClearance", m_WarnClearance);
         m_CautionClearance = PrefMgr->GetValueINT("CautionClearance", m_CautionClearance);
         m_OKClearance = PrefMgr->GetValueINT("OKClearance", m_OKClearance);

         m_ShowClearances = PrefMgr->GetValueINT("ShowClearances", m_ShowClearances);
      }
      catch (...)
      {
         MessageBox("TAMask status window could not interpret preferences.");

         // The passed in string is not XML or not readable.
         return false;
      }
   }

   m_StatusColor = m_CautionColor;
   set_altitude(m_TestAlt);

   return true;
}

void CTAMaskStatus::set_altitude(int Altitude)
{
   // This function is used for manual settings passed in from external callers

   // The user has passed in a bad elevation, set the warning color and 
   // leave the mask as it was
   if (Altitude == -99999)
   {
      m_StatusColor = m_WarnColor;
   }
   else
   {
      m_StatusColor = m_CautionColor;

      m_Altitude = m_TestAlt = Altitude;

      if (m_ShowClearances)
      {
         if (m_ShowWarn)
            m_WarnText.Format("A-%d ft", m_WarnClearance);
         else 
            m_WarnText = "";

         if (m_ShowCaution)
            m_CautionText.Format("A-%d ft", m_CautionClearance);
         else
            m_CautionText = "";

         if (m_ShowOK)
            m_OKText.Format("A-%d ft", m_OKClearance);
         else
            m_OKText = "";
      }
      else
      {
         if (m_ShowWarn)
            m_WarnText.Format("%d ft", m_TestAlt - m_WarnClearance);
         else 
            m_WarnText = "";

         if (m_ShowCaution)
            m_CautionText.Format("%d ft", m_TestAlt - m_CautionClearance);
         else
            m_CautionText = "";

         if (m_ShowOK)
            m_OKText.Format("%d ft", m_TestAlt - m_OKClearance);
         else
            m_OKText = "";
      }

      if (GetSafeHwnd())
      {
         SetDlgItemText(IDC_STATUS, "User specified Altitude");

         GetDlgItem(IDC_ALT)->EnableWindow(true);

         UpdateData(FALSE);
      }
   }

   if (GetSafeHwnd())
   {
      SetWindowText("TAMask Status");
      Invalidate();
   }
}

void CTAMaskStatus::set_gps_altitude(int Altitude, C_overlay *pOverlay)
{
   // This version of the function is called from external callers passing in 
   // known good elevations -- like from GPS or Routes

   // Get the name of the type of overlay.  This is used to let the user know where the elevation
   // value came from.  Later it might be worth increasing the size of the status dialog and
   // using the name of the overlay, rather than the overlay type.
   CString strOverlayType(" ");
   if (pOverlay != NULL)
   {
      OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(pOverlay->get_m_overlayDescGuid());
      if (pOverlayTypeDesc != NULL)
      {
         strOverlayType = pOverlayTypeDesc->displayName + " ";
      }
   }

   CString strTitle("TAMask Status");

   // If we did not get an elevation from the GPS source, and the user has not switched
   // to manual mode already, warn the user the GPS feed is bad and leave the elevation
   // as the last known good elevation
   if (Altitude == -99999)
   {
      if (m_StatusColor != m_CautionColor)
      {
         m_StatusColor = m_WarnColor;

         CString msg("Last known ");
         msg += strOverlayType;
         msg += "Altitude";

         if (GetSafeHwnd())
         {
            SetDlgItemText(IDC_STATUS, msg);

            // Come forward if we have not alreay
            ShowWindow(SW_SHOW);

            GetDlgItem(IDC_ALT)->EnableWindow(true);
         }
      }
   }
   else
   {
      set_altitude(Altitude);
      m_StatusColor = m_OKColor;

      CString msg;
      msg += strOverlayType;
      msg += "Altitude";

      if (GetSafeHwnd())
      {
         SetDlgItemText(IDC_STATUS, msg);
         GetDlgItem(IDC_ALT)->EnableWindow(false);
      }

      if (pOverlay != NULL)
      {
         strTitle.Format("TAMask Status (%s)", OVL_get_overlay_manager()->GetOverlayDisplayName(pOverlay));
      }
   }

   if (GetSafeHwnd())
   {
      SetWindowText(strTitle);
      Invalidate();
   }
}



BEGIN_MESSAGE_MAP(CTAMaskStatus, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)

	//{{AFX_MSG_MAP(CTAMaskStatus)
	ON_WM_DRAWITEM()
   ON_WM_ERASEBKGND()
   ON_BN_CLICKED(IDC_SHOWCLEARANCE, OnRadioClicked)
   ON_WM_CLOSE()
   ON_EN_KILLFOCUS(IDC_ALT, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTAMaskStatus::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch (nIDCtl) 
	{
		case IDC_WARN_COLOR:
         if (m_ShowWarn)
			   draw_color_button_rgb(this, nIDCtl, m_WarnColor);
         else
            SetDlgItemText(IDC_WARN, "");

			break;

		case IDC_CAUTION_COLOR:
         if (m_ShowCaution) 
			   draw_color_button_rgb(this, nIDCtl, m_CautionColor);
         else
            SetDlgItemText(IDC_CAUTION, "");

			break;

      case IDC_OK_COLOR:
         if (m_ShowOK)
			   draw_color_button_rgb(this, nIDCtl, m_OKColor);
         else
            SetDlgItemText(IDC_OK, "");

			break;

      case IDC_SHOWCLEARANCE:
         {
	         CPen darkpen;
	         CWnd *wnd;
	         RECT rc;

	         wnd = GetDlgItem(IDC_SHOWCLEARANCE);

            if (wnd == NULL) 
            {
               MessageBox("Windows error drawing status window!");
               return;
            }

	         CWindowDC dc(wnd);
	         wnd->GetClientRect(&rc);

	         draw_basic_button(&dc, rc);

	         darkpen.CreatePen(PS_SOLID, 2, RGB(0,0,0));
	         CPen *oldpen = dc.SelectObject(&darkpen);
            int tip, base;
            if (m_ShowClearances)
            {
               tip = rc.bottom;
               base = tip + (rc.top - rc.bottom /2);
            }
            else
            {
               tip = rc.top;
               base = tip - (rc.top - rc.bottom /2);
            }

            int ctr = (rc.left+rc.right)/2;

	         dc.MoveTo(rc.left, base);
	         dc.LineTo(ctr, tip);
	         dc.LineTo(rc.right, base);
	         dc.MoveTo(ctr, rc.top);
	         dc.LineTo(ctr, rc.bottom);

	         dc.SelectObject(oldpen);

            break;
         }
	}
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CTAMaskStatus::OnEraseBkgnd(CDC* pDC)
{
   if (CDialog::OnEraseBkgnd(pDC) == FALSE)
      // If the standard call did not work, we will not either
      return FALSE;

   if (pDC == NULL)
      return FALSE;

	RECT rc;
   pDC->GetClipBox(&rc);

   CPen FramePen;
	FramePen.CreatePen(PS_SOLID, 10, m_StatusColor);
	CPen *oldpen = pDC->SelectObject(&FramePen);

   HGDIOBJ HollowBrush = ::GetStockObject(HOLLOW_BRUSH);

   if (HollowBrush)
   {
      HGDIOBJ oldBrush = ::SelectObject(pDC->GetSafeHdc(), HollowBrush);

      pDC->Rectangle(&rc);

      if (oldBrush)
	      ::SelectObject(pDC->GetSafeHdc(), oldBrush);
   }

	pDC->SelectObject(oldpen);

   return TRUE;
}

void CTAMaskStatus::draw_color_button_rgb(CWnd *cwnd, int button_id, COLORREF color)
{
	CPen lightpen, darkpen;
	CBrush brush;
	CWnd *wnd;
	RECT rc;

	wnd = cwnd->GetDlgItem(button_id);

   if (wnd == NULL) 
   {
      MessageBox("Windows error drawing status window!");

      return;
   }

	CWindowDC dc(wnd);
	wnd->GetClientRect(&rc);

	draw_basic_button(&dc, rc);

	darkpen.CreatePen(PS_SOLID, 2, RGB(0,0,0));
	lightpen.CreatePen(PS_SOLID, 2, RGB(255,255,255));
	brush.CreateSolidBrush(color);
	CPen *oldpen = dc.SelectObject(&darkpen);
	CBrush *oldbrush = dc.SelectObject(&brush);
	dc.Rectangle(&rc);
	dc.SelectObject(&lightpen);
	dc.MoveTo(rc.right-1, rc.top);
	dc.LineTo(rc.right-1, rc.bottom-1);
	dc.LineTo(rc.left, rc.bottom-1);
	dc.SelectObject(oldbrush);
	dc.SelectObject(oldpen);

	//brush.DeleteObject();
	//darkpen.DeleteObject();
	//lightpen.DeleteObject();
}

void CTAMaskStatus::draw_basic_button(CDC * dc, RECT rc)
{
	CPen darkpen, graypen, lightpen;
	CBrush graybrush;

	graybrush.CreateSolidBrush( RGB(192,220,192) );
	darkpen.CreatePen(PS_SOLID, 1, RGB (0, 0, 0) );
	graypen.CreatePen(PS_SOLID, 1, RGB(128,128,128) );
	lightpen.CreatePen(PS_SOLID, 1, RGB (255, 255, 255) );
	CPen *oldpen = dc->SelectObject(&graypen);
	CBrush *oldbrush = dc->SelectObject(&graybrush);

	dc->Rectangle( &rc );

	dc->SelectObject(&lightpen);

	dc->MoveTo( rc.right-1, rc.top );
	dc->LineTo( rc.right-1, rc.bottom-1 );
	dc->LineTo( rc.left, rc.bottom-1 );

	dc->SelectObject(oldbrush);
	dc->SelectObject(oldpen);

	//graybrush.DeleteObject();
	//darkpen.DeleteObject();
	//graypen.DeleteObject();
	//lightpen.DeleteObject();
}


void CTAMaskStatus::OnRadioClicked() 
{
   m_ShowClearances = !m_ShowClearances;

   set_altitude(m_TestAlt);
}


void CTAMaskStatus::OnClose()
{
   // Notify the overlay that the user wants to close the status window
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   
   if (ovl_mgr)
   {
      C_TAMask_ovl *pOvl = (C_TAMask_ovl*)ovl_mgr->get_first_of_type(FVWID_Overlay_TerrainAvoidanceMask);
      
      if (pOvl)
      {
         pOvl->ToggleStatusWindow(pOvl);
      }
   }

   CDialog::OnClose();
}
LRESULT CTAMaskStatus::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

