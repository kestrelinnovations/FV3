// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

#include "stdafx.h"
#include "SystemHealthRibbonButton.h"
#include "Common\ComErrorObject.h"
#include "err.h"
#include "resource.h"

int SystemHealthRibbonButton::m_selected_row = -1;
IMPLEMENT_DYNCREATE(SystemHealthRibbonButton, CMFCRibbonGallery)

   const int SYSTEM_HEALTH_ENTRY_ROW_WIDTH = 160;
const int SYSTEM_HEALTH_ENTRY_ROW_HEIGHT = 50;
const int NUM_COLUMNS = 2;

namespace
{
   int ScaleToDPI(int value, int get_dev_caps_index)
   {
      HDC dc = ::GetDC(NULL);
      int ret = static_cast<int>(value * ::GetDeviceCaps(dc,
         get_dev_caps_index) / 96.0);
      ::ReleaseDC(NULL, (HDC)dc);
      return ret;
   }
}

SystemHealthRibbonButton::SystemHealthRibbonButton(UINT nID, LPCTSTR lpszText, 
   int nSmallImageIndex, int nLargeImageIndex) : CMFCRibbonGallery(nID, 
   lpszText, nSmallImageIndex, nLargeImageIndex, 
   CSize(ScaleToDPI(SYSTEM_HEALTH_ENTRY_ROW_WIDTH, LOGPIXELSX),
   ScaleToDPI(SYSTEM_HEALTH_ENTRY_ROW_HEIGHT, LOGPIXELSY)), 0, FALSE),
   m_spSystemHealthStatus(nullptr),
   m_spSystemHealthNotify(nullptr)
{
   SetIconsInRow(NUM_COLUMNS);
   LoadXMLConfiguration();
   UpdateButtonState();
   ConnectToCP(true);
}

SystemHealthRibbonButton::~SystemHealthRibbonButton()
{
   ConnectToCP(false);
}

void SystemHealthRibbonButton::UpdateButtonState()
{
   try
   {
      // determine which button to use based on the overall system status
      if (m_spSystemHealthStatus->OverallSystemStateGood() == VARIANT_TRUE)
      {
         m_nSmallImageIndex = m_nLargeImageIndex = 66;
      }
      else
      {
         m_nSmallImageIndex = m_nLargeImageIndex = 67;
      }
      Redraw();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error getting System Health status - %s", 
         (char *)e.Description());
      ERR_report(msg);
   }
}

void SystemHealthRibbonButton::NotifyClient(GUID UID)
{
   UpdateButtonState();
}


void SystemHealthRibbonButton::ConnectToCP(bool ConnectToSource)
{
   try
   {
      if (ConnectToSource)
      {
         // We just create our own object for System Health.  Since it is a singleton, we end up with the one from Mainframe.
         if (m_spSystemHealthStatus == NULL)
            CO_CREATE(m_spSystemHealthStatus, CLSID_SystemHealthStatus);

         if (m_spSystemHealthNotify == NULL)
            CO_CREATE(m_spSystemHealthNotify, CLSID_SystemHealthLegacyNotify);

         // This is necessary so that the legacy notify COM object can call back to this class.
         CSystemHealthClient* pme(this);
         m_spSystemHealthNotify->RegisterForNotificiations((long*) pme, m_spSystemHealthStatus);
      }
      else 
      {
         if (m_spSystemHealthNotify != NULL)
            m_spSystemHealthNotify->UnregisterForNotifications(m_spSystemHealthStatus);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error registering for System Health Notifications - %s", 
         (char *)e.Description());
      ERR_report(msg);
   }
}

void SystemHealthRibbonButton::LoadXMLConfiguration()
{
   // Add the Ribbon Bar according to configuration file.
   try
   {
      if (m_spSystemHealthStatus == NULL)
         CO_CREATE(m_spSystemHealthStatus, CLSID_SystemHealthStatus);

      IFvSystemHealthConfigPtr spSystemHealthConfig;
      CO_CREATE(spSystemHealthConfig, CLSID_FvSystemHealthConfig);

      spSystemHealthConfig->Initialize();
      BOOL bRet = spSystemHealthConfig->SelectAll(true);
      _bstr_t lastGroup = L"";
      int groupCount = 0;
      while (bRet)
      {
         if (lastGroup != spSystemHealthConfig->GroupName)
         {
            if (groupCount > 0)
            {
               AddGroup(lastGroup, NUM_COLUMNS * groupCount);
            }

            lastGroup = spSystemHealthConfig->GroupName;
            groupCount = 0;
         }

         // Create a system status object for this entry.
         ISystemHealthStatusStatePtr spSystemHealthStatusState = NULL;
         CO_CREATE(spSystemHealthStatusState, CLSID_SystemHealthStatusState);

         spSystemHealthStatusState->Initialize(
            spSystemHealthConfig->SystemHealthDescGuid,
            spSystemHealthConfig->DisplayName,
            spSystemHealthConfig->GroupName,
            spSystemHealthConfig->IconFile,
            spSystemHealthConfig->NoCurrencyMsg,
            spSystemHealthConfig->ExpiredMsg,
            spSystemHealthConfig->CurrentMsg,
            spSystemHealthConfig->CurrencyDialogClsid,
            spSystemHealthConfig->DataCurrencyClsid,
            spSystemHealthConfig->RequestDataSourceChangeEventsClsid,
            spSystemHealthConfig->HelpURI
            );

         m_spSystemHealthStatus->AddSystemHealthStatusState(spSystemHealthStatusState);

         groupCount++;

         bRet = spSystemHealthConfig->MoveNext();
      }

      if (groupCount > 0)
      {
         AddGroup(lastGroup, NUM_COLUMNS * groupCount);
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error loading system health types - %s", 
         (char *)e.Description());
      ERR_report(msg);
   }
}

void SystemHealthRibbonButton::OnDrawPaletteIcon(CDC* pDC, CRect rectIcon, 
   int nIconIndex, CMFCRibbonGalleryIcon* /*pIcon*/, COLORREF /*clrText*/)
{
   ASSERT_VALID(this);
   ASSERT_VALID(pDC);

   const int row_number = nIconIndex / NUM_COLUMNS;
   const int col_number = nIconIndex % NUM_COLUMNS;

   // Highlight the row the cursor is over
   if (row_number == m_hilighted_row)
   {
      CDrawingManager dm(*pDC);

      // The top third will be filled with one gradient, the bottom two
      // thirds another
      const int y_val_to_switch_color = static_cast<int>(
         .37 * rectIcon.Height()) + rectIcon.TopLeft().y;

      CRect top_half(rectIcon.TopLeft().x, rectIcon.TopLeft().y, 
         rectIcon.BottomRight().x, y_val_to_switch_color);

      CRect bottom_half(rectIcon.TopLeft().x, y_val_to_switch_color,
         rectIcon.BottomRight().x, rectIcon.BottomRight().y);

      dm.FillGradient(top_half, RGB(255, 252, 217), RGB(255, 231, 143));
      dm.FillGradient(bottom_half, RGB(255, 215, 81), RGB(255, 231, 153));
   }

   // Get the deescription and icon for the current row
   ISystemHealthStatusStatePtr spSystemHealthStatusState = m_spSystemHealthStatus->FindIndex(row_number);
   CComBSTR strStatus;
   SystemHealthStatusTypeEnum statusIcon;
   spSystemHealthStatusState->GetState(&strStatus, &statusIcon);

   UINT resource_id = IDI_RED_X;
   switch(statusIcon)
   {
   case SYSTEM_HEALTH_GREEN_CHECK:
      resource_id = IDI_GREEN_CHECK;
      break;

   case SYSTEM_HEALTH_RED_X:
      resource_id = IDI_RED_X;
      break;

   case SYSTEM_HEALTH_YELLOW_QUESTION:
      resource_id = IDI_YELLOW_QUESTION;
      break;
   }

   // Draw the text for the description
   if (col_number == 0)
   {
      rectIcon.DeflateRect(5, 5);
      pDC->DrawText(CString(strStatus), rectIcon, 
         DT_LEFT | DT_WORDBREAK);
   }
   // Draw the icon
   else if (col_number == 1)
   {
      HICON icon = ::LoadIcon(AfxGetInstanceHandle(), 
         MAKEINTRESOURCE(resource_id));

      pDC->DrawIcon(CPoint(rectIcon.CenterPoint().x - 16, 
         rectIcon.CenterPoint().y - 16), icon);

      ::DestroyIcon(icon);
   }
}

void SystemHealthRibbonButton::NotifyHighlightListItem(int nIndex)
{
   m_hilighted_row = nIndex < 0 ? -1 : nIndex / NUM_COLUMNS;
   if (m_hilighted_row != -1)
      m_selected_row = m_hilighted_row;

   RedrawIcons();
   CMFCRibbonGallery::NotifyHighlightListItem(nIndex);
}