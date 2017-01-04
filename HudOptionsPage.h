// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#ifndef _FALCONVIEW_HUDOPTIONSPAGE_H_
#define _FALCONVIEW_HUDOPTIONSPAGE_H_

#include "resource.h"
#include "geocx1.h"

struct HudProperties;

class HudOptionsPage : public CPropertyPage
{
   DECLARE_DYNCREATE(HudOptionsPage)

public:
   HudOptionsPage() { }
   HudOptionsPage(HudProperties* hud_properties);

protected:
   virtual void OnOK() override;
   virtual BOOL OnInitDialog() override;
   virtual void DoDataExchange(CDataExchange* pDX) override;
   afx_msg void OnNewLocation(double latitude, double longitude);
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   afx_msg void OnColorClicked();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   DECLARE_MESSAGE_MAP()
   DECLARE_EVENTSINK_MAP()

private:
   HudProperties* m_hud_properties;

   // Dialog data
   enum { IDD = IDD_HUD_OPTIONS_PAGE };
   CString getHelpURIPath() const
   {
      return "fvw.chm::/fvw_core.chm::/FVW/HUD_Options.htm";
   }
   CGeocx1 m_geocx;
};

#endif  // _FALCONVIEW_HUDOPTIONSPAGE_H_
