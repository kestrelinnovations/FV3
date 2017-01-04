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

// nitf_db.h

#ifndef NITF_DB_H
#define NITF_DB_H 1

class CAdoSet;

class C_nitf_file_set
{
public:

	C_nitf_file_set()
	{
		m_ID = 0;
		m_Filespec = "";
		m_VolumeLabel = "";
		m_FileName = "";
		m_FileSize = 0;
		m_FileWriteTime = 0;
		m_ImageType = "";
		m_ImageInFile = 0;
		m_SearchPath = "";
		m_BoolGeoAvailable = 0;
		m_LLLat = 0.0;
		m_LLLong = 0.0;
		m_LRLat = 0.0;
		m_LRLong = 0.0;
		m_ULLat = 0.0;
		m_ULLong = 0.0;
		m_URLat = 0.0;
		m_URLong = 0.0;
		m_MinLat = 0.0;
		m_MaxLat = 0.0;
		m_MinLong = 0.0;
		m_MaxLong = 0.0;
		m_PixelWidth = 0.0;
		m_PixelHeight = 0.0;
		m_ImageWidth = 0;
		m_ImageHeight = 0;
		m_ImageDate = "";
		m_FileTitle = "";
		m_ImageTitle = "";
		m_Resolution = 0.0;
		m_BoolChecked = 0;
		m_BoolDisplayImage = 0;
		m_BoolColor = 0;
		m_ContrastMode = 0;
	}

	int     m_ID;
	CString	m_Filespec;
	CString	m_VolumeLabel;
	CString m_FileName;
	int     m_FileSize;
	int 	m_FileWriteTime;
	CString	m_ImageType;
	int     m_ImageInFile;
	CString m_SearchPath;
	int		m_BoolGeoAvailable;
	double	m_LLLat;
	double	m_LLLong;
	double	m_LRLat;
	double	m_LRLong;
	double	m_ULLat;
	double	m_ULLong;
	double	m_URLat;
	double	m_URLong;
	double	m_MinLat;
	double	m_MaxLat;
	double	m_MinLong;
	double	m_MaxLong;
	double	m_PixelWidth;
	double	m_PixelHeight;
	int     m_ImageWidth;
	int  	m_ImageHeight;
	CString	m_ImageDate;
	CString	m_FileTitle;
	CString	m_ImageTitle;
	double	m_Resolution;
	int     m_BoolChecked;
	int     m_BoolDisplayImage;
	int		m_BoolColor;
	int     m_ContrastMode;
};

// ******************************************************************************
// ******************************************************************************

class C_nitf_path_set
{
public:

	C_nitf_path_set()
	{
		m_ID = 0;
		m_SearchPath = "";
		m_BoolIncludeSubDirs = 0;
	}

	int     m_ID;
	CString m_SearchPath;
	int m_BoolIncludeSubDirs;
};

// ******************************************************************************
// ******************************************************************************

class C_nitf_db
{
public:
	C_nitf_db();
	~C_nitf_db();

	CAdoSet *m_file_set;
	CAdoSet *m_path_set;

	int open_file_set();
	int open_path_set();

	void close_file_set();
	void close_path_set();
	int get_first_path(C_nitf_path_set * set);
	int get_next_path(C_nitf_path_set * set);
	int get_first_file(C_nitf_file_set * set);
	int get_next_file(C_nitf_file_set * set);
	int add_path(C_nitf_path_set * set);
	int add_file(C_nitf_file_set * set);
	int delete_all_paths();
	int delete_all_files();
	int delete_files_by_source_path(CString path);
	int delete_files_by_path_and_file_name(CString path, CString filename);

	CString get_database_path();
	BOOL database_exists_and_is_read_write();
	int get_highest_file_set_id();
	int get_highest_path_set_id();
};


// ******************************************************************************
// ******************************************************************************













#endif  // ifndef NITF_DB_H