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

// OverlayFactoryCOM.cpp
//

#include "stdafx.h"
#include "OverlayFactoryCOM.h"

#include "OverlayCOM.h"

COverlayFactoryCOM::COverlayFactoryCOM(
   const char *lpszDisplayName, CLSID classId) :
   m_displayName(lpszDisplayName),
   m_classId(classId)
{ 
}

HRESULT COverlayFactoryCOM::CreateOverlayInstance(C_overlay **ppOverlay)
{
   *ppOverlay = new COverlayCOM(m_displayName.c_str(), m_classId);
   return S_OK;
}

CFileOverlayFactoryCOM::CFileOverlayFactoryCOM(
   const char *lpszDisplayName, CLSID classId) :
   m_displayName(lpszDisplayName),
   m_classId(classId)
{
}

HRESULT CFileOverlayFactoryCOM::CreateOverlayInstance(C_overlay **ppOverlay)
{
   *ppOverlay = new CFileOverlayCOM(m_displayName.c_str(), m_classId);
   return S_OK;
}

