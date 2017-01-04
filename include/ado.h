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

// ado.h

#ifndef OVLADO_H
#define OVLADO_H
#define INITGUID

#include "icrsint.h"

#pragma once

// **********************************************************************************
class CAdoSet
{
	public:
		// standard constructor and destructor
		CAdoSet();
		~CAdoSet();

		_ConnectionPtr	m_pConn;
		_CommandPtr		m_pCommand;
		_RecordsetPtr	m_pRecordset;
		BOOL			m_open;

		int		Open(CString filename, CString tablename, CString sql_qualifier = "");
		void	Close();
		BOOL	IsEOF();
		int		MoveNext();
		void	AddNew();
		void	Update();

		CString			get_string_value(const TCHAR* field_name);
		double			get_double_value(const TCHAR* field_name);
		short			get_short_value(const TCHAR* field_name);
		long			get_long_value(const TCHAR* field_name);
		DATE			get_date_value(const TCHAR* field_name);
		unsigned char	get_uchar_value(const TCHAR* field_name);
		char			get_char_value(const TCHAR* field_name);

		void	set_string_value(const TCHAR* field_name, const TCHAR* text);
		void	set_double_value(const TCHAR* field_name, const double num);
		void	set_short_value(const TCHAR* field_name, const short num);
		void	set_long_value(const TCHAR* field_name, const long num);
		void	set_date_value(const TCHAR* field_name, const DATE dat);
		void	set_uchar_value(const TCHAR* field_name, const unsigned char ch);
		void	set_char_value(const TCHAR* field_name, const char ch);
};
// **********************************************************************************




// **********************************************************************************
class CAdoTool
{
	public:
		CAdoTool(){}

		CString				get_string_value(_Recordset* set, const TCHAR* field_name);
		const double		get_double_value(_Recordset* set, const TCHAR* field_name);
		const short			get_short_value(_Recordset* set, const TCHAR* field_name);
		const long			get_long_value(_Recordset* set, const TCHAR* field_name);
		const _bstr_t		get_bstr_value(_Recordset* set, const TCHAR* field_name);
		const DATE			get_date_value(_Recordset* set, const TCHAR* field_name);
		const unsigned char	get_uchar_value(_Recordset* set, const TCHAR* field_name);
		const char			get_char_value(_Recordset* set, const TCHAR* field_name);
		
		void	set_string_value(_Recordset* set, const TCHAR* field_name, const TCHAR* text);
		void	set_double_value(_Recordset* set, const TCHAR* field_name, const double num);
		void	set_short_value(_Recordset* set, const TCHAR* field_name, const short num);
		void	set_long_value(_Recordset* set, const TCHAR* field_name, const long num);
		void	set_date_value(_Recordset* set, const TCHAR* field_name, const DATE dat);
		void	set_uchar_value(_Recordset* set, const TCHAR* field_name, const unsigned char ch);
		void	set_char_value(_Recordset* set, const TCHAR* field_name, const char ch);
};

#endif
