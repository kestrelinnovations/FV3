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

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "StdAfx.h"
#include "FvSystemHealthCOM.h"
#include "Common\ComErrorObject.h"

CFvSystemHealthCOM::CFvSystemHealthCOM(void)
{
   CreateComponents();
}

void CFvSystemHealthCOM::CreateComponents()
{
   CO_CREATE(m_spSystemHealthStatus, CLSID_SystemHealthStatus);
}

CFvSystemHealthCOM::~CFvSystemHealthCOM(void)
{
}

ISystemHealthStatusStatePtr CFvSystemHealthCOM::FindIndex(long Index)
{
   return m_spSystemHealthStatus->FindIndex(Index);
}

ISystemHealthStatusStatePtr CFvSystemHealthCOM::FindUID(const GUID UID)
{
   return m_spSystemHealthStatus->FindUID(UID);
}

HRESULT CFvSystemHealthCOM::DataSourceChangeEvents(_bstr_t strDataSourcePath, VARIANT_BOOL* bDataHandled)
{
   return m_spSystemHealthStatus->DataSourceChangeEvents(strDataSourcePath, bDataHandled);
}
