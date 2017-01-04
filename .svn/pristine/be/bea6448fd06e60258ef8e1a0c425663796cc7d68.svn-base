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

// CustomInitializerCOM.cpp
//

#include "stdafx.h"
#include "OverlayTypeCustomInitializerCOM.h"
#include "ovl_mgr.h"
#include "..\Splash.h"
#include "err.h"
#include "Common\ComErrorObject.h"
#include "utils.h"
#include "FvCore\Include\GuidStringConverter.h"

CCustomInitializerCOM::CCustomInitializerCOM(CLSID classId) :
   m_classId(classId)
{
}

int CCustomInitializerCOM::InitializeAtStartup()
{
   int result = FAILURE;
      try
      {
         CO_CREATE(m_spFvCustomInitializer, m_classId);
         m_spFvCustomInitializer->InitializeAtStartup(CSplashWnd::GetFvSplashWnd(), OVL_get_overlay_manager()->GetFvOverlayManager());
         result = SUCCESS;
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
   return result;
}

void CCustomInitializerCOM::TerminateAtShutdown()
{
      try
      {
         if (m_spFvCustomInitializer != NULL)
            m_spFvCustomInitializer->TerminateAtShutdown();
      }
      catch(_com_error &e)
      {
         REPORT_COM_ERROR(e);
      }
}
