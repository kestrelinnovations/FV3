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

// ado.cpp

#include "stdafx.h"
#include "dbdaoint.h"
#include "..\include\ado.h"
#include "..\include\err.h"

// **********************************************************************************************
// **********************************************************************************************
CAdoSet::CAdoSet()
{
	m_open = FALSE;
   m_pConn = NULL;
}
// END OF CAdoSet standard constructor


// **********************************************************************************************
// **********************************************************************************************
CAdoSet::~CAdoSet()
{
	if ( (m_pConn != NULL) && ((m_pConn->State & adStateOpen) == adStateOpen))
		m_pConn->Close();
}
// END OF CAdoSet standard destructor


// **********************************************************************************************
// **********************************************************************************************
int CAdoSet::Open(CString filename, CString tablename, CString sql_qualifier)
{
	CString txt, msg;

	txt.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source = %s", filename);

	// close the db if it is open...
	if (m_open)
		Close();

	// establishing a connection to the data source...
	try	
	{
		HRESULT hr = m_pConn.CreateInstance (__uuidof (Connection));

		if (FAILED (hr))
		{
			msg.Format("Error opening drawing file: %s", filename);
			ERR_report(msg);
			AfxMessageBox (msg);
			return FAILURE;
		}
		

		if (FAILED (m_pConn->Open (_bstr_t(txt), L"", L"", adModeUnknown)))
		{
			msg.Format("Error opening drawing file: %s", filename);
			ERR_report(msg);
			AfxMessageBox (msg);
			return FAILURE;
		}
	}
	catch ( _com_error &e )
	{
		msg.Format("Database Error: %s", (char *)e.Description());
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg.Format("Error opening drawing file: %s", filename);
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	try	
	{
		HRESULT hr = m_pCommand.CreateInstance (__uuidof (Command));

		if (FAILED (hr))
		{
			AfxMessageBox ("Can't create an instance of Command");
			return FAILURE;
		}

		m_pCommand->ActiveConnection = m_pConn;
		txt.Format("SELECT * FROM %s %s", tablename, sql_qualifier);
		txt.TrimRight(" ");

		m_pCommand->CommandText = _bstr_t(txt);

		hr = m_pRecordset.CreateInstance (__uuidof (Recordset));

		if (FAILED (hr))
		{
			AfxMessageBox ("Can't create an instance of Recordset");
			return FAILURE;
		}

		m_pRecordset->CursorLocation = adUseClient;
		m_pRecordset->Open ((IDispatch *) m_pCommand, vtMissing, adOpenForwardOnly,
							adLockOptimistic, adCmdUnknown);
	} 
	catch ( _com_error &e )
	{
		msg.Format("Database Error: %s", (char *)e.Description());
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg.Format("Error opening drawing file: %s", filename);
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	m_open = TRUE;
	return SUCCESS;
}
// END OF CAdoSet::Open


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::Close()
{
	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	m_pRecordset->Close ();

	m_open = FALSE;
}
// END OF CAdoSet::Close


// **********************************************************************************************
// **********************************************************************************************
BOOL CAdoSet::IsEOF()
{
	if (!m_open)
		return TRUE;

	if (m_pRecordset == NULL)
		return TRUE;

	return m_pRecordset->GetadoEOF();
}
// END OF CAdoSet::IsEOF


// **********************************************************************************************
// **********************************************************************************************
int CAdoSet::MoveNext()
{
	if (!m_open)
		return FAILURE;

	if (m_pRecordset == NULL)
		return FAILURE;

	m_pRecordset->MoveNext();
	return SUCCESS;
}
// END OF CAdoSet::MoveNext


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::AddNew()
{
	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	m_pRecordset->AddNew();
}
// END OF CAdoSet::AddNew


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::Update()
{
	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	m_pRecordset->Update();
}
// END OF CAdoSet::Update


// **********************************************************************************************
// **********************************************************************************************
CString CAdoSet::get_string_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	CString tstr("");

	if (!m_open)
		return tstr;

	if (m_pRecordset == NULL)
		return tstr;

	tstr = adotool.get_string_value(m_pRecordset, field_name);
	return tstr;
}
// END OF CAdoSet::get_string_value


// **********************************************************************************************
// **********************************************************************************************
double CAdoSet::get_double_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	double tf = 0.0;

	if (!m_open)
		return tf;

	if (m_pRecordset == NULL)
		return tf;

	tf = adotool.get_double_value(m_pRecordset, field_name);
	return tf;
}
// END OF CAdoSet::get_double_value


// **********************************************************************************************
// **********************************************************************************************
long CAdoSet::get_long_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	long tf = 0;

	if (!m_open)
		return tf;

	if (m_pRecordset == NULL)
		return tf;

	tf = adotool.get_long_value(m_pRecordset, field_name);
	return tf;
}
// END OF CAdoSet::get_long_value


// **********************************************************************************************
// **********************************************************************************************
short CAdoSet::get_short_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	short tf = 0;

	if (!m_open)
		return tf;

	if (m_pRecordset == NULL)
		return tf;

	tf = adotool.get_short_value(m_pRecordset, field_name);
	return tf;
}
// END OF CAdoSet::get_short_value


// **********************************************************************************************
// **********************************************************************************************
unsigned char CAdoSet::get_uchar_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	unsigned char tf = 0;

	if (!m_open)
		return tf;

	if (m_pRecordset == NULL)
		return tf;

	tf = adotool.get_uchar_value(m_pRecordset, field_name);
	return tf;
}
// END OF CAdoSet::get_uchar_value


// **********************************************************************************************
// **********************************************************************************************
char CAdoSet::get_char_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	unsigned char tf = 0;

	if (!m_open)
		return tf;

	if (m_pRecordset == NULL)
		return tf;

	tf = adotool.get_char_value(m_pRecordset, field_name);
	return tf;
}
// END OF CAdoSet::get_char_value


// **********************************************************************************************
// **********************************************************************************************
DATE CAdoSet::get_date_value( const TCHAR* field_name )
{
	CAdoTool adotool;
	DATE retval(0.0); // Initialize to absurd value (~100 AD)

	if (!m_open)
		return retval;

	if (m_pRecordset == NULL)
		return retval;

	retval = adotool.get_date_value(m_pRecordset, field_name);
	return retval;
}
// END OF CAdoSet::get_date_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_string_value( const TCHAR* field_name, const TCHAR* text )
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_string_value(m_pRecordset, field_name, text);
}
// END OF CAdoSet::set_string_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_double_value( const TCHAR* field_name, const double num )
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_double_value(m_pRecordset, field_name, num);
}
// END OF CAdoSet::set_double_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_short_value( const TCHAR* field_name, const short num )
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_short_value(m_pRecordset, field_name, num);
}
// END OF CAdoSet::set_short_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_long_value( const TCHAR* field_name, const long num )
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_long_value(m_pRecordset, field_name, num);
}
// END OF CAdoSet::set_long_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_date_value(const TCHAR* field_name, const DATE dat)
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_date_value(m_pRecordset, field_name, dat);
}
// END OF CAdoSet::set_date_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_uchar_value(const TCHAR* field_name, const unsigned char ch)
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_uchar_value(m_pRecordset, field_name, ch);
}
// END OF CAdoSet::set_uchar_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoSet::set_char_value(const TCHAR* field_name, const char ch)
{
	CAdoTool adotool;

	if (!m_open)
		return;

	if (m_pRecordset == NULL)
		return;

	adotool.set_char_value(m_pRecordset, field_name, ch);
}
// END OF CAdoSet::set_char_value




//###################################################################################################
//###################################################################################################
//###################################################################################################
//###################################################################################################


// **********************************************************************************************
// **********************************************************************************************
CString CAdoTool::get_string_value( _Recordset* set, const TCHAR* field_name )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

//   std::wstring retval( _T("") );
   CString retval;
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_BSTR:
         retval = val.bstrVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_string_value


// **********************************************************************************************
// **********************************************************************************************
const double CAdoTool::get_double_value( _Recordset* set, const TCHAR* field_name )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   double retval(0.0);
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_R8:
         retval = val.dblVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_double_value


// **********************************************************************************************
// **********************************************************************************************
const short CAdoTool::get_short_value( _Recordset* set, const TCHAR* field_name )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   short retval(0);
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_I2:
         retval = val.iVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_short_value


// **********************************************************************************************
// **********************************************************************************************
const long CAdoTool::get_long_value( _Recordset* set, const TCHAR* field_name )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   long retval(0);
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_I4:
         retval = val.lVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_long_value


// **********************************************************************************************
// **********************************************************************************************
const _bstr_t CAdoTool::get_bstr_value( _Recordset* set, const TCHAR* field_name )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _bstr_t retval( _T("") );
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_BSTR:
         retval = val.bstrVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_bstr_value


// **********************************************************************************************
// **********************************************************************************************
const DATE CAdoTool::get_date_value(_Recordset* set, const TCHAR* field_name)
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   DATE retval(0.0); // Initialize to absurd value (~100 AD)
   _variant_t val = set->adoFields->GetItem(field_name)->Value;

   switch(val.vt)
   {
      // expected case - set return value to converted field value
      case VT_DATE:
         retval = val.date;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_date_value


// **********************************************************************************************
// **********************************************************************************************
const unsigned char CAdoTool::get_uchar_value(_Recordset* set, const TCHAR* field_name)
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   unsigned char retval(0);
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_UI1:
         retval = (unsigned char) val.uiVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_uchar_value


// **********************************************************************************************
// **********************************************************************************************
const char CAdoTool::get_char_value(_Recordset* set, const TCHAR* field_name)
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   char retval(0);
   _variant_t val = set->adoFields->GetItem( field_name )->Value;

   switch(val.vt)
   {
      // expected case - set return value to field value
      case VT_I1:
         retval = (char) val.cVal;
         break;

      // NULL and EMPTY are OK - just return the default values
      case VT_NULL:  
      case VT_EMPTY:
         break;

      // any other type will throw
      default:
         ASSERT(false);
         _com_issue_error(E_UNEXPECTED);
   }

   return retval;
}
// END OF CAdoTool::get_char_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_string_value( _Recordset* set, const TCHAR* field_name, const TCHAR* text )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
	CString tstr;

	// make sure the string is not empty
	tstr = text;
	if (tstr.GetLength() < 1)
		tstr = " ";
	   
	val.SetString( tstr );
	set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_string_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_double_value( _Recordset* set, const TCHAR* field_name, const double num )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
   
   val.ChangeType(VT_R8);
   val.dblVal = num;

   set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_double_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_short_value( _Recordset* set, const TCHAR* field_name, const short num )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
   
   val.ChangeType(VT_I2);
   val.iVal = num;

   set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_short_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_long_value( _Recordset* set, const TCHAR* field_name, const long num )
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
   
   val.ChangeType(VT_I4);
   val.lVal = num;

   set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_long_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_date_value(_Recordset* set, const TCHAR* field_name, const DATE dat)
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
   
   val.ChangeType(VT_DATE);
   val.date = dat;

   set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_date_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_uchar_value(_Recordset* set, const TCHAR* field_name, const unsigned char ch)
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
   
   val.ChangeType(VT_UI1);
   val.uiVal = ch;

   set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_uchar_value


// **********************************************************************************************
// **********************************************************************************************
void CAdoTool::set_char_value(_Recordset* set, const TCHAR* field_name, const char ch)
{
   ASSERT(NULL != set);
   if (NULL == set)
      _com_issue_error(E_POINTER);

   _variant_t val;
   
   val.ChangeType(VT_I1);
   val.cVal = ch;

   set->adoFields->GetItem( field_name )->Value = val;
}
// END OF CAdoTool::set_char_value
