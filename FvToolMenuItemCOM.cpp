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

// OverlayTypeCustomInitializerCOM.h
//

#include "stdafx.h"
#include "FvToolMenuItemCOM.h"
#include "err.h"
#include "Common\ComErrorObject.h"
#include "getobjpr.h"

CFvToolMenuItemCOM::CFvToolMenuItemCOM(GUID menuItemGuid, CLSID classId, 
                                       CString helpFilename, long helpID, CString helpURI) :
	m_menuItemGuid(menuItemGuid), m_classId(classId), 
   m_helpFilename(helpFilename), m_helpID(helpID),
   m_helpURI(helpURI)
{
}

void CFvToolMenuItemCOM::MenuItemSelected()
{
	try
	{
		if (m_spFvToolMenuItem == NULL)
      {
			CO_CREATE(m_spFvToolMenuItem, m_classId);
         m_spFvToolMenuItem->Initialize(m_menuItemGuid);
      }

		m_spFvToolMenuItem->MenuItemSelected();
	}
	catch(_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void CFvToolMenuItemCOM::LaunchHelp()
{
   CMainFrame* pFrame = fvw_get_frame();
   if (pFrame)
	{
		pFrame->LaunchHtmlHelp( m_helpID, HELP_CONTEXT, (LPCTSTR) m_helpFilename, (LPCTSTR) m_helpURI);
	}
}
