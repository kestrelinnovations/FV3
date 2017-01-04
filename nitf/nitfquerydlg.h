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

// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CNITFQueryDlg wrapper class

class CNITFQueryDlg : public CWnd
{
protected:
	DECLARE_DYNCREATE(CNITFQueryDlg)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xd3e727a5, 0xfec0, 0x42e6, { 0xbc, 0x2, 0x52, 0xa8, 0x5, 0x46, 0xa, 0x4e } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:

// Operations
public:
	void Initialize(LPCTSTR bsConnectString, LPCTSTR bsLogFile, LPCTSTR bsPermanentLocalPath);
	void EnableCtrl(long lEnableQueryEditor, long lEnableMetadataList);
	long ExecuteSQL(LPCTSTR bsSQLString);
	LPDISPATCH QuerySQL(LPCTSTR bsSelectString, LPCTSTR bsFilterString);
	void EnableDisplayOptions(long eDisplayOptions);
	void SetControlMode(long eControlMode);
	long SpecialFunction(long eFunctionCode, const VARIANT& varParam1, const VARIANT& varParam2, const VARIANT& varParam3, VARIANT* pvarResult1, VARIANT* pvarResult2);
#if 0
	void GetInitialEnableCtrl(long* plEnableQueryEditor, long* plEnableMetadataList);
#endif
};