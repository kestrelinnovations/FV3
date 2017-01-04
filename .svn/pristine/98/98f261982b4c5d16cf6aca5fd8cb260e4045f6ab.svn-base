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

// Parser.h
//

#pragma once

typedef struct
{
	CString column_header;
	CString filter_string;

	// local point filter
	CString icon_assigned;
	CString group_name;

	// drawing filter
	int color;
	CString shape_type;

} filter_t;

class ParserProperties
{
protected:
	CList<CString, CString> m_column_header_list;
	CList<filter_t, filter_t> m_filter_list;

	int m_start_row;
	
public:
	CList<CString, CString> &get_column_header_list() { return m_column_header_list; }
	
	void add_filter(filter_t filter) { m_filter_list.AddTail(filter); }
	void delete_filter(int i);

	int get_num_filters() { return m_filter_list.GetCount(); }
	filter_t get_filter(int i)
	{
		POSITION pos = m_filter_list.FindIndex(i);
		ASSERT(pos);

		return m_filter_list.GetAt(pos);
	}

	void set_start_row(int s);
	int get_start_row() { return m_start_row; }

	virtual int get_block_size();
	virtual int save_settings(BYTE *&block_ptr);
	void save_string(BYTE *&block_ptr, CString str);
	void save_int(BYTE *&block_ptr, int v);

	virtual int load_settings(BYTE *&block_ptr);
	CString load_string(BYTE *&block_ptr);
	int load_int(BYTE *&block_ptr);
};

class Parser
{
protected:
	CString m_file_name;
	ParserProperties *m_properties;

public:
	CString get_file_name() { return m_file_name; }

	virtual int get_tokens(CList<CString, CString>& token_list) = 0;

	// open the file to be parsed.  Returns SUCCESS/FAILURE
	virtual int open_file(BOOL bSkipToStartRow = FALSE) = 0;

	// close the file.  Returns SUCCESS/FAILURE.
	virtual int close_file() = 0;

	virtual int save_settings(CString filename) = 0;
	virtual int load_settings(CString filename) = 0;
	int write_header(BYTE *&block_ptr);
	int read_header(CFile &file);

	virtual bool is_kind_of(CString str) { return str.Compare("Parser") == 0; }

	ParserProperties *get_properties() { return m_properties; }
	virtual CString get_info_text() = 0;
};
