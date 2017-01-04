// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

// nitf_db.cpp

// database interface file sof nitf overlay

#include "stdafx.h"
#include "nitf_db.h"
#include "file.h"
#include "param.h"
#include "err.h"
#include "ado.h"

// ******************************************************************************
// ******************************************************************************

C_nitf_db::C_nitf_db()
{
	m_file_set = NULL;
	m_path_set = NULL;
}


// ******************************************************************************
// ******************************************************************************

C_nitf_db::~C_nitf_db()
{
	close_file_set();
	close_path_set();
}

// ******************************************************************************
// ******************************************************************************

void C_nitf_db::close_file_set()
{
	if (m_file_set)
	{
//		m_file_set->Close();
		delete (m_file_set);
		m_file_set = NULL;
	}
}

// ******************************************************************************
// ******************************************************************************

void C_nitf_db::close_path_set()
{
	if (m_path_set)
	{
//		m_path_set->Close();
		delete (m_path_set);
		m_path_set = NULL;
	}
}

// ******************************************************************************
// ******************************************************************************

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::open_path_set()
{
	CString msg, filename;
	int rslt;

	filename = get_database_path();
	
	if (!database_exists_and_is_read_write())
		return FAILURE;

	close_path_set();
	
	m_path_set = new CAdoSet;

	try
	{
		rslt = m_path_set->Open(filename, "SEARCH_PATHS");

		if (rslt != SUCCESS)
		{
			delete m_path_set;
			m_path_set = NULL;
			ERR_report("NITF Overlay Database Access Open Error");
			return FAILURE;
		}
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::open_file_set()
{
	CString msg, filename;
	int rslt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	close_file_set();
	
	filename = get_database_path();
	
	m_file_set = new CAdoSet;

	try
	{
		rslt = m_file_set->Open(filename, "FILES");

		if (rslt != SUCCESS)
		{
			delete m_file_set;
			m_file_set = NULL;
			ERR_report("NITF Overlay Database Access Open Error");
			return FAILURE;
		}
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::get_first_path(C_nitf_path_set * set)
{
	CString msg, filename, sdata;
	int rslt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	close_path_set();
	
	filename = get_database_path();
	
	if (m_path_set != NULL)
		delete m_path_set;

	m_path_set = new CAdoSet;

	try
	{
		rslt = m_path_set->Open(filename, "SEARCH_PATHS");

		if (rslt != SUCCESS)
		{
			delete m_path_set;
			m_path_set = NULL;
			ERR_report("NITF Overlay Database Access Open Error");
			return FAILURE;
		}

		if (m_path_set->IsEOF())
			return FAILURE;

		set->m_SearchPath = m_path_set->get_string_value("SearchPath");
		set->m_BoolIncludeSubDirs = m_path_set->get_short_value("BoolIncludeSubDirs");
		m_path_set->MoveNext();

	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::get_next_path(C_nitf_path_set * set)
{
	CString msg;

	if (m_path_set == NULL)
		return FAILURE;

	try
	{
		if (m_path_set->IsEOF())
			return FAILURE;

		set->m_SearchPath = m_path_set->get_string_value("SearchPath");
		set->m_BoolIncludeSubDirs = m_path_set->get_short_value("BoolIncludeSubDirs");
		m_path_set->MoveNext();
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::add_path(C_nitf_path_set * set)
{
	CString msg;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	if (m_path_set == NULL)
		return FAILURE;

	try
	{
		m_path_set->AddNew();
		m_path_set->set_long_value("ID", set->m_ID);
		m_path_set->set_string_value("SearchPath", set->m_SearchPath);
		m_path_set->set_short_value("BoolIncludeSubDirs", set->m_BoolIncludeSubDirs);
		m_path_set->Update();
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::delete_all_paths()
{
	CString msg;
	int cnt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	if (m_path_set == NULL)
		return FAILURE;

	if (m_path_set->IsEOF())
		return SUCCESS;

	try
	{
		cnt = m_path_set->m_pRecordset->GetRecordCount();
		if (cnt < 1)
			return SUCCESS;

		while (!m_path_set->IsEOF())
		{
			m_path_set->m_pRecordset->Delete(adAffectCurrent);
			if (!m_path_set->IsEOF())
				m_path_set->MoveNext();
		}
//		m_path_set->Update();
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::delete_all_files()
{
	CString msg;
	int cnt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	if (m_file_set == NULL)
		return FAILURE;

	if (m_file_set->IsEOF())
		return SUCCESS;

	try
	{
		cnt = m_file_set->m_pRecordset->GetRecordCount();
		if (cnt < 1)
			return SUCCESS;

		while (!m_file_set->IsEOF())
		{
			m_file_set->m_pRecordset->Delete(adAffectCurrent);
			if (!m_file_set->IsEOF())
				m_file_set->MoveNext();
		}
//		m_path_set->Update();
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}

// ******************************************************************************
// ******************************************************************************

// delete all file records with the given source path

int C_nitf_db::delete_files_by_source_path(CString path)
{
	CString msg, curpath;
	int cnt, rslt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	if (m_file_set == NULL)
	{
		rslt = open_file_set();
		if (rslt != SUCCESS)
			return FAILURE;
	}

	try
	{
		cnt = m_file_set->m_pRecordset->GetRecordCount();
		if (cnt < 1)
			return SUCCESS;

		m_file_set->m_pRecordset->MoveFirst();
		if (m_file_set->IsEOF())
			return SUCCESS;

		while (!m_file_set->IsEOF())
		{
			curpath = m_file_set->get_string_value("SearchPath");
			if (!path.CompareNoCase(curpath))
				m_file_set->m_pRecordset->Delete(adAffectCurrent);
			if (!m_file_set->IsEOF())
				m_file_set->MoveNext();
		}
//		m_path_set->Update();
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}
// delete_files_by_source_path

// ******************************************************************************
// ******************************************************************************

// delete all file records with the given source path and filename

int C_nitf_db::delete_files_by_path_and_file_name(CString path, CString filename)
{
	CString msg, curfilename, curpath;
	int cnt, rslt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	if (m_file_set == NULL)
	{
		rslt = open_file_set();
		if (rslt != SUCCESS)
			return FAILURE;
	}

	try
	{
		cnt = m_file_set->m_pRecordset->GetRecordCount();
		if (cnt < 1)
			return SUCCESS;

		m_file_set->m_pRecordset->MoveFirst();
		if (m_file_set->IsEOF())
			return SUCCESS;

		while (!m_file_set->IsEOF())
		{
			curpath = m_file_set->get_string_value("SEARCH_PATH");
			curfilename = m_file_set->get_string_value("FILENAME");
			if (!filename.CompareNoCase(curfilename) && !path.CompareNoCase(curpath))
				m_file_set->m_pRecordset->Delete(adAffectCurrent);
			if (!m_file_set->IsEOF())
				m_file_set->MoveNext();
		}
//		m_path_set->Update();
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}
// end of delete_files_by_path_and_file_name

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::get_first_file(C_nitf_file_set * set)
{
	CString msg, filename, tstr;
	int rslt;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	close_file_set();
	
	filename = get_database_path();
	
	if (m_file_set != NULL)
		delete m_file_set;

	m_file_set = new CAdoSet;
	if (m_file_set == NULL)
		return FAILURE;

	try
	{
		rslt = m_file_set->Open(filename, "FILES");

		if (rslt != SUCCESS)
		{
			delete m_file_set;
			m_file_set = NULL;
			ERR_report("NITF Overlay Database Access Open Error");
			return FAILURE;
		}

		if (m_file_set->IsEOF())
		{
			delete m_file_set;
			m_file_set = NULL;
			return FAILURE;
		}

		set->m_ID = m_file_set->get_long_value("ID");
		set->m_Filespec = m_file_set->get_string_value("Filespec");
		set->m_VolumeLabel = m_file_set->get_string_value("VolumeLabel");
		set->m_FileName = m_file_set->get_string_value("FileName");
		set->m_FileWriteTime = m_file_set->get_long_value("FileWriteTime");
		set->m_FileSize = m_file_set->get_long_value("FileSize");
		set->m_ImageType = m_file_set->get_string_value("ImageType");
		set->m_ImageInFile = m_file_set->get_short_value("ImageInFile");
		set->m_SearchPath = m_file_set->get_string_value("SearchPath");
		set->m_BoolGeoAvailable = m_file_set->get_short_value("BoolGeoAvailable");
		set->m_LLLat = m_file_set->get_double_value("LLLat");
		set->m_LLLong = m_file_set->get_double_value("LLLong");
		set->m_LRLat = m_file_set->get_double_value("LRLat");
		set->m_LRLong = m_file_set->get_double_value("LRLong");
		set->m_ULLat = m_file_set->get_double_value("ULLat");
		set->m_ULLong = m_file_set->get_double_value("ULLong");
		set->m_URLat = m_file_set->get_double_value("URLat");
		set->m_URLong = m_file_set->get_double_value("URLong");
		set->m_MinLat = m_file_set->get_double_value("MinLat");
		set->m_MaxLat = m_file_set->get_double_value("MaxLat");
		set->m_MinLong = m_file_set->get_double_value("MinLong");
		set->m_MaxLong = m_file_set->get_double_value("MaxLong");
		set->m_PixelWidth = m_file_set->get_double_value("PixelWidth");
		set->m_PixelHeight = m_file_set->get_double_value("PixelHeight");
		set->m_ImageWidth = m_file_set->get_long_value("ImageWidth");
		set->m_ImageHeight = m_file_set->get_long_value("ImageHeight");
		set->m_ImageDate = m_file_set->get_string_value("ImageDate");
		set->m_FileTitle = m_file_set->get_string_value("FileTitle");
		set->m_ImageTitle = m_file_set->get_string_value("ImageTitle");
		set->m_Resolution = m_file_set->get_double_value("Resolution");
		set->m_BoolChecked = m_file_set->get_short_value("BoolChecked");
		set->m_BoolDisplayImage = m_file_set->get_short_value("BoolDisplayImage");
		set->m_BoolColor = m_file_set->get_short_value("BoolColor");
		set->m_ContrastMode = m_file_set->get_short_value("ContrastMode");

		m_file_set->MoveNext();

	}
	catch ( _com_error &e )
	{
		_bstr_t bstrDescription (e.Description());
		CString tstr;
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		delete m_file_set;
		m_file_set = NULL;
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		delete m_file_set;
		m_file_set = NULL;
		return FAILURE;
	}

	return SUCCESS;
}
// end of get_first_file

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::get_next_file(C_nitf_file_set * set)
{
	CString msg;

	if (m_file_set == NULL)
		return FAILURE;

	try
	{
		if (m_file_set->IsEOF())
			return FAILURE;

		set->m_ID = m_file_set->get_long_value("ID");
		set->m_Filespec = m_file_set->get_string_value("Filespec");
		set->m_VolumeLabel = m_file_set->get_string_value("VolumeLabel");
		set->m_FileName = m_file_set->get_string_value("FileName");
		set->m_FileWriteTime = m_file_set->get_long_value("FileWriteTime");
		set->m_FileSize = m_file_set->get_long_value("FileSize");
		set->m_ImageType = m_file_set->get_string_value("ImageType");
		set->m_ImageInFile = m_file_set->get_short_value("ImageInFile");
		set->m_SearchPath = m_file_set->get_string_value("SearchPath");
		set->m_BoolGeoAvailable = m_file_set->get_short_value("BoolGeoAvailable");
		set->m_LLLat = m_file_set->get_double_value("LLLat");
		set->m_LLLong = m_file_set->get_double_value("LLLong");
		set->m_LRLat = m_file_set->get_double_value("LRLat");
		set->m_LRLong = m_file_set->get_double_value("LRLong");
		set->m_ULLat = m_file_set->get_double_value("ULLat");
		set->m_ULLong = m_file_set->get_double_value("ULLong");
		set->m_URLat = m_file_set->get_double_value("URLat");
		set->m_URLong = m_file_set->get_double_value("URLong");
		set->m_MinLat = m_file_set->get_double_value("MinLat");
		set->m_MaxLat = m_file_set->get_double_value("MaxLat");
		set->m_MinLong = m_file_set->get_double_value("MinLong");
		set->m_MaxLong = m_file_set->get_double_value("MaxLong");
		set->m_PixelWidth = m_file_set->get_double_value("PixelWidth");
		set->m_PixelHeight = m_file_set->get_double_value("PixelHeight");
		set->m_ImageWidth = m_file_set->get_long_value("ImageWidth");
		set->m_ImageHeight = m_file_set->get_long_value("ImageHeight");
		set->m_ImageDate = m_file_set->get_string_value("ImageDate");
		set->m_FileTitle = m_file_set->get_string_value("FileTitle");
		set->m_ImageTitle = m_file_set->get_string_value("ImageTitle");
		set->m_Resolution = m_file_set->get_double_value("Resolution");
		set->m_BoolChecked = m_file_set->get_short_value("BoolChecked");
		set->m_BoolDisplayImage = m_file_set->get_short_value("BoolDisplayImage");
		set->m_BoolColor = m_file_set->get_short_value("BoolColor");
		set->m_ContrastMode = m_file_set->get_short_value("ContrastMode");

		m_file_set->MoveNext();

	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}
// end of get_next_file

// ******************************************************************************
// ******************************************************************************

int C_nitf_db::add_file(C_nitf_file_set * set)
{
	CString msg;

	if (!database_exists_and_is_read_write())
		return FAILURE;

	if (m_file_set == NULL)
		return FAILURE;

	try
	{
		m_file_set->AddNew();
		m_file_set->set_long_value("ID", set->m_ID);
		m_file_set->set_string_value("Filespec", set->m_Filespec.Left(254));
		m_file_set->set_string_value("VolumeLabel", set->m_VolumeLabel.Left(254));
		m_file_set->set_string_value("FileName", set->m_FileName.Left(254));
		m_file_set->set_long_value("FileWriteTime", set->m_FileWriteTime);
		m_file_set->set_long_value("FileSize", set->m_FileSize);
		m_file_set->set_short_value("BoolGeoAvailable", set->m_BoolGeoAvailable);
		if (set->m_FileTitle.GetLength() > 0)
			m_file_set->set_string_value("FileTitle", set->m_FileTitle.Left(254));
		if (set->m_ImageTitle.GetLength() > 0)
			m_file_set->set_string_value("ImageTitle", set->m_ImageTitle.Left(254));
		if (set->m_SearchPath.GetLength() > 0)
			m_file_set->set_string_value("SearchPath", set->m_SearchPath.Left(254));
		else
			m_file_set->set_string_value("SearchPath", " ");
		m_file_set->set_string_value("ImageDate", set->m_ImageDate.Left(19));
		m_file_set->set_double_value("LLLat", set->m_LLLat);
		m_file_set->set_double_value("LLLong", set->m_LLLong);
		m_file_set->set_double_value("LRLat", set->m_LRLat);
		m_file_set->set_double_value("LRLong", set->m_LRLong);
		m_file_set->set_double_value("ULLat", set->m_ULLat);
		m_file_set->set_double_value("ULLong", set->m_ULLong);
		m_file_set->set_double_value("URLat", set->m_URLat);
		m_file_set->set_double_value("URLong", set->m_URLong);
		m_file_set->set_double_value("MinLat", set->m_MinLat);
		m_file_set->set_double_value("MaxLat", set->m_MaxLat);
		m_file_set->set_double_value("MinLong", set->m_MinLong);
		m_file_set->set_double_value("MaxLong", set->m_MaxLong);
		m_file_set->set_double_value("PixelWidth", set->m_PixelWidth);
		m_file_set->set_double_value("PixelHeight", set->m_PixelHeight);
		m_file_set->set_long_value("ImageWidth", set->m_ImageWidth);
		m_file_set->set_long_value("ImageHeight", set->m_ImageHeight);
		m_file_set->set_double_value("Resolution", set->m_Resolution);
		m_file_set->set_short_value("BoolChecked", set->m_BoolChecked);
		m_file_set->set_short_value("BoolDisplayImage", set->m_BoolDisplayImage);
		m_file_set->set_short_value("BoolColor", set->m_BoolColor);
		m_file_set->set_short_value("ContrastMode", set->m_ContrastMode);
		m_file_set->Update();
	}
	catch ( _com_error &e )
	{
		_bstr_t bstrDescription (e.Description());
		msg.Format("Database Error: %s", bstrDescription);
		ERR_report(msg);
		AfxMessageBox (msg);
		return FAILURE;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return FAILURE;
	}

	return SUCCESS;
}
// end of add_file

// ******************************************************************************
// ******************************************************************************

CString C_nitf_db::get_database_path()
{
	CString filename;
   const int PATH_LEN = 201;
	char path[PATH_LEN];

	// get HD data path 
//	if (PRM_get_value(PRM_HD_DATA, path) != SUCCESS)
//	{
//		ERR_report("Failed getting Hard Disk Data Path.");
//		return "";
//	} 
	// the block of code above was replaced by this in order to eliminate the use of PRM_get_value
	CString reg_string = PRM_get_registry_string("Main", "USER_DATA");
	strncpy_s(path, PATH_LEN, reg_string, (sizeof(path)-1));	

	filename = path;
	filename += "\\NITF\\nitf_cov.mdb";

	return filename;
}

// ******************************************************************************
// ******************************************************************************

BOOL C_nitf_db::database_exists_and_is_read_write()
{
	CString filename, msg;
	DWORD attributes;
	
	filename = get_database_path();

	if (FIL_access(filename, FIL_EXISTS) != SUCCESS)
	{
		msg.Format("Cannot find NITF Overlay database file: \r\n\r\n%s", filename);
		AfxMessageBox(msg);
		return FALSE;
	}


	attributes = GetFileAttributes(filename);
	if (attributes == -1)
	{
		ERR_report("GetFileAttributes() failed.");
		return FALSE;
	}

	if (attributes & FIL_ATTR_READONLY)
	{
		filename = get_database_path();
		msg = "Cannot complete operation, NITF Overlay database file is read-only:\r\n\r\n";
		msg += filename;
		AfxMessageBox(msg);
		return FALSE;
	}

	return TRUE;
}

// ******************************************************************************
// ******************************************************************************

// find highest id value, error returns -1

int C_nitf_db::get_highest_file_set_id()
{
	CString msg;
	int rslt, id, maxid;

	if (!database_exists_and_is_read_write())
		return -1;

//	if (m_file_set == NULL)
	{
		rslt = open_file_set();
		if (rslt != SUCCESS)
			return -1;
	}

	// check for empty database
	if (m_file_set->m_pRecordset->BOF)
		return -1;

	maxid = -1;
	try
	{
		m_file_set->m_pRecordset->MoveFirst();
		if (m_file_set->IsEOF())
			return -1;

		while (!m_file_set->IsEOF())
		{
			id = m_file_set->get_long_value("ID");
			if (id > maxid)
				maxid = id;
			if (!m_file_set->IsEOF())
				m_file_set->MoveNext();
		}
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return -1;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return -1;
	}

	return maxid;
}
// get_highest_file_set_id

// ******************************************************************************
// ******************************************************************************

// find highest id value, error returns -1

int C_nitf_db::get_highest_path_set_id()
{
	CString msg;
	int rslt, id, maxid;

	if (!database_exists_and_is_read_write())
		return -1;

//	if (m_path_set == NULL)
	{
		rslt = open_path_set();
		if (rslt != SUCCESS)
			return -1;
	}

	// check for empty database
	if (m_path_set->m_pRecordset->BOF)
		return -1;

	maxid = -1;
	try
	{
		m_path_set->m_pRecordset->MoveFirst();
		if (m_path_set->IsEOF())
			return -1;

		while (!m_path_set->IsEOF())
		{
			id = m_path_set->get_long_value("ID");
			if (id > maxid)
				maxid = id;
			if (!m_path_set->IsEOF())
				m_path_set->MoveNext();
		}
	}
	catch ( _com_error &e )
	{
		CString tstr;
		_bstr_t bstrDescription (e.Description());
		AfxBSTR2CString(&tstr, bstrDescription);
		msg.Format("Database Error: %s", tstr);
		ERR_report(msg);
		AfxMessageBox (msg);
		return -1;
	}
	catch (...)
	{
		msg = "Database Error";
		ERR_report(msg);
		AfxMessageBox (msg);
		TRACE ( "*** Unhandled Exception ***" );
		return -1;
	}

	return maxid;
}
// get_highest_path_set_id

// ******************************************************************************
// ******************************************************************************
