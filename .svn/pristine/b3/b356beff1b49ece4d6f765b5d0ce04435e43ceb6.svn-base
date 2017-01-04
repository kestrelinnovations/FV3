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

#pragma once

class OvlTerrainMask;
class ATL_NO_VTABLE COvlTerrainMaskStatus : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COvlTerrainMaskStatus, &FVTerrainMaskingLib::CLSID_FvTerrainMaskingStatus>,
	public IDispatchImpl<FVTerrainMaskingLib::IFvTerrainMaskingStatus, 
		&__uuidof(FVTerrainMaskingLib::IFvTerrainMaskingStatus), &FVTerrainMaskingLib::LIBID_FVTerrainMaskingLib, /* wMajor = */ 1>
{
	OvlTerrainMask*	m_pMask;

public:
   // constructor
	COvlTerrainMaskStatus() : m_pMask( NULL ) {}

	void	Initialize( OvlTerrainMask* pMask ) { m_pMask = pMask; }

	BEGIN_COM_MAP(COvlTerrainMaskStatus)
		COM_INTERFACE_ENTRY(IFvTerrainMaskingStatus)
	END_COM_MAP()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() {}

	STDMETHOD(raw_OnPercentCompleted)(BSTR maskId, unsigned short percentCompleted);
	STDMETHOD(raw_OnMaskingCompleted)(BSTR maskId);
};