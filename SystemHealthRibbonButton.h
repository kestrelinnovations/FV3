// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// SystemHealthRibbonButton - uses a ribbon gallery to display a table of
// system health states and status icons. Gets notifications from the system
// status so that it can update its icon appropriately
//

#include "SystemHealthDialog.h"
#include "FvSystemHealth\SystemHealthLegacyNotifyClient.h"

class SystemHealthRibbonButton : 
   public CMFCRibbonGallery,
   public CSystemHealthClient
{
	DECLARE_DYNCREATE(SystemHealthRibbonButton)

 // Construction:
public:
	SystemHealthRibbonButton(UINT nID, LPCTSTR lpszText, int nSmallImageIndex, 
      int nLargeImageIndex);
	~SystemHealthRibbonButton();
	void LoadXMLConfiguration();

   static int GetSelectedRow() { return m_selected_row; }

   virtual void NotifyClient(GUID UID);

// Overrides:
protected:
	virtual void OnDrawPaletteIcon(CDC* pDC, CRect rectIcon, int nIconIndex, 
      CMFCRibbonGalleryIcon* pIcon, COLORREF clrText);
   virtual void NotifyHighlightListItem(int nIndex);
	void ConnectToCP(bool ConnectToSource);
   void UpdateButtonState();
private:
   int m_hilighted_row;
   static int m_selected_row;

	ISystemHealthStatusPtr m_spSystemHealthStatus;
   ISystemHealthLegacyNotifyPtr m_spSystemHealthNotify;

	// Not Allowed
   SystemHealthRibbonButton() { }

}; // class SystemHealthRibbonButton