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

// TextParser.h
//

#pragma once

#include "Parser.h"

const int DELIM_NONE = 0x0;
const int DELIM_TAB = 0x1; 
const int DELIM_SPACE = 0x2;
const int DELIM_SEMICOLON = 0x4;
const int DELIM_COMMA = 0x8;
const int DELIM_OTHER = 0x10;


#include <vector>
#include <algorithm>

class TextParserProperties : public ParserProperties
{
	int m_is_delimited;
	int m_delimiter;
	int m_consecutive_delimiters;
	char m_other_delimiter;

	std::vector<int> m_field_list;

public:

	// constructor/destructor
	TextParserProperties();

public:
	void set_is_delimited(int d) { m_is_delimited = d; }
	int get_is_delimited() { return m_is_delimited; }

	void set_delimiter(int d) { m_delimiter = d; }
	int get_delimiter() { return m_delimiter; }

	void set_consecutive_delimiters(int c) { m_consecutive_delimiters = c; }
	int get_consecutive_delimiters() { return m_consecutive_delimiters; }

	void set_other_delimiter(char c) { m_other_delimiter = c; }
	char get_other_delimiter() { return m_other_delimiter; }

	void add_field_position(int i);
	void remove_field_position(int i);

	int get_num_field_positions() { return m_field_list.size(); }
	int get_field_position_at(int i) { return m_field_list[i]; }

	virtual int get_block_size();
	virtual int save_settings(BYTE *&block_ptr);
	virtual int load_settings(BYTE *&block_ptr);
};

class TextParser : public Parser
{
private:
	CFile m_file;

public:

	// constructor
	TextParser(CString file_name);
	~TextParser();

	// returns TRUE if the text is delimited (tab, commas, etc...)
	BOOL is_text_delimited();

	// return the number of lines of data in the given text file
	int get_number_lines();

	// returns the set of delimiter(s)
	CString get_delimiter();

	// open the file to be parsed.  Returns SUCCESS/FAILURE
	virtual int open_file(BOOL bSkipToStartRow = FALSE);

	// close the file.  Returns SUCCESS/FAILURE.
	virtual int close_file();

	virtual int save_settings(CString filename);
	virtual int load_settings(CString filename);

	// get the next line in the file.  Returns SUCCESS/FAILURE.
	int get_line(CString &line);

	virtual int get_tokens(CList<CString, CString>& token_list);

	virtual bool is_kind_of(CString str) 
	{ 
		if (str.Compare("TextParser") == 0) 
			return true;

		return Parser::is_kind_of(str);
	}
	virtual CString get_info_text();
};
