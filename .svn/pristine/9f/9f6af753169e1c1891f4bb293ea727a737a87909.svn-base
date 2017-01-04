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

// TextParser.cpp
//

#include "stdafx.h"
#include "TextParser.h"
#include "err.h"

// constructor
TextParserProperties::TextParserProperties()
{
	m_start_row = 1;
   m_is_delimited = TRUE;
	m_delimiter = DELIM_TAB;
	m_consecutive_delimiters = 0;
}

int TextParserProperties::get_block_size()
{
	int block_size = ParserProperties::get_block_size();

	block_size += sizeof(m_is_delimited);
	block_size += sizeof(m_delimiter);
	block_size += sizeof(m_consecutive_delimiters);
	block_size += sizeof(m_other_delimiter);

	block_size += sizeof(int) + m_field_list.size() * sizeof(int);

	return block_size;
}

int TextParserProperties::save_settings(BYTE *&block_ptr)
{
	ParserProperties::save_settings(block_ptr);

	save_int(block_ptr, m_is_delimited);
	save_int(block_ptr, m_delimiter);
	save_int(block_ptr, m_consecutive_delimiters);

	memcpy(block_ptr, &m_other_delimiter, sizeof(char));
	block_ptr += sizeof(char);

	int size = m_field_list.size();
	save_int(block_ptr, size);
	for(int i=0;i<size;i++)
		save_int(block_ptr, m_field_list[i]);

	return SUCCESS;
}

int TextParserProperties::load_settings(BYTE *&block_ptr)
{
	ParserProperties::load_settings(block_ptr);

	m_is_delimited = load_int(block_ptr);
	m_delimiter = load_int(block_ptr);
	m_consecutive_delimiters = load_int(block_ptr);

	memcpy(&m_other_delimiter, block_ptr, sizeof(char));
	block_ptr += sizeof(char);

   m_field_list.erase(m_field_list.begin(), m_field_list.end());
	int size = load_int(block_ptr);
	for(int i=0;i<size;i++)
		m_field_list.push_back(load_int(block_ptr));

	return SUCCESS;
}

void TextParserProperties::add_field_position(int pos)
{
	int found = 0;
	int size = m_field_list.size();
	for(int i=0;i<size;i++)
		if (m_field_list[i] == pos)
		{
			found = 1;
			break;
		}

	if (!found)
	{
		m_field_list.push_back(pos);
		std::sort(m_field_list.begin(), m_field_list.end());
	}
}

void TextParserProperties::remove_field_position(int pos)
{
	std::vector<int> tmp = m_field_list;
	m_field_list.erase(m_field_list.begin(), m_field_list.end());

	int found = 0;
	int size = tmp.size();
	for(int i=0;i<size;i++)
		if (tmp[i] != pos)
			m_field_list.push_back(tmp[i]);
}

// constructor
TextParser::TextParser(CString file_name)
{
	m_properties = new TextParserProperties;
	m_file_name = file_name;
}

TextParser::~TextParser()
{
	delete m_properties;
}

// returns TRUE if the text is delimited (tab, commas, etc...)
BOOL TextParser::is_text_delimited() 
{ 
	TextParserProperties *prop = (TextParserProperties *)m_properties;
   return prop->get_is_delimited();
}

// return the number of lines of data in the given text file
int TextParser::get_number_lines() 
{ 
	// this number is used to set the maximum value on the spin control in
	// step 1 - there really is no need to return the actual number of lines
	// in the text value
	return 99999; 
}

// returns the set of delimiter(s)
CString TextParser::get_delimiter()
{
	CString delimiter;

	TextParserProperties *prop = (TextParserProperties *)m_properties;

	int d = prop->get_delimiter();

	if ( (d & DELIM_TAB) == DELIM_TAB)
		delimiter += "\t";

	if ( (d & DELIM_SPACE) == DELIM_SPACE)
		delimiter += " ";

	if ( (d & DELIM_SEMICOLON) == DELIM_SEMICOLON)
		delimiter += ";";

	if ( (d & DELIM_COMMA) == DELIM_COMMA)
		delimiter += ",";

	if ( (d & DELIM_OTHER) == DELIM_OTHER)
		delimiter += prop->get_other_delimiter();

	return delimiter;
}

// open the file to be parsed
int TextParser::open_file(BOOL bSkipToStartRow /* = FALSE */)
{
	CFileException ex; 

	if (m_file.Open(m_file_name, CFile::modeRead, &ex) == FALSE)
	{
		CString msg;
		msg.Format("Unable to open %s for importing", m_file_name);
		ERR_report(msg);
		return FAILURE;
	}

   // skip past the rows that are not being read
   if (bSkipToStartRow)
   {
      const int nStartRow = m_properties->get_start_row() - 1;
      for(int i=0;i<nStartRow;++i)
      {
         CString line;
         get_line(line);
      }
   }

	return SUCCESS;
}

int TextParser::close_file()
{
	try
	{
		m_file.Close();
	}
	catch(CFileException *e)
	{
		ERR_report("Error closing file");
		e->Delete();
		return FAILURE;
	}

	return SUCCESS;
}

// get the next line in the file
int TextParser::get_line(CString &line)
{
	char c;
	
	while (1)
	{
		if (m_file.Read(&c, 1) < 1 || c == '\n')
			break;
		
		line += c;
	}
	
	return SUCCESS;
}

// search like strtok but return empty tokens and don't keep a static variable
//current pos should start at 0 an will be updated with the location after the next found delimiter
// return: 
// true if a delimiter is found
// false if the end of string is reached with no delimiters 
// tok may or may not be empty
bool get_tok(CString buf,CString Delim,int &current_pos,CString &Tok )
{

	Tok = "";
	for (;current_pos<buf.GetLength();current_pos++)
	{
		if (Delim.Find(buf[current_pos]) != -1)  // found a delimiter so return
		{
			current_pos++;
			return true;
		}
		else
		{  //not a delimiter so add it to tok and keep going

			Tok+=buf[current_pos];
		}
	}
	return false;
	
}



int TextParser::get_tokens(CList<CString, CString>& token_list)
{
	TextParserProperties *prop = (TextParserProperties *)m_properties;

	// get the next line in the file
	CString line;
	get_line(line);

	//ASSERT(line.GetLength() < 1024);

	if (prop->get_is_delimited())
	{
		int pos = 0;
		CString delimiter = get_delimiter();
		CString token;
		while (pos<line.GetLength())
		{
			get_tok(line, delimiter, pos, token);
			if(prop->get_consecutive_delimiters()==0 || token != "") // if token is empty and consecutvive delim check pox checked, skip it
			{
				token_list.AddTail(token);
			}
		}

	}
	else
	{
		CString tmp;
		int num_pos = prop->get_num_field_positions();
		int start = 0;
		int chars_copied = 0;
		for(int i=0;i<num_pos;i++)
		{
			int end = prop->get_field_position_at(i);
			if (start >= line.GetLength() || end >= line.GetLength())
				 break;

			tmp = line.Mid(start, end-start);
			token_list.AddTail(tmp);
			
			chars_copied += end-start;
			start = end;
		}
		if (chars_copied < line.GetLength())
			token_list.AddTail(line.Right(line.GetLength() - chars_copied));
	}

	return SUCCESS;
}

int TextParser::save_settings(CString filespec) 
{
	CFile file;
   CFileException ex;

	// get the number of bytes required to store the data to disk
   int block_size = 0;

   // size of file header and version info
   block_size += 6 + 2 + sizeof(short);

	// header + block size for properties
	block_size += 4 + sizeof(int);

	int properties_block_size = m_properties->get_block_size();
	block_size += properties_block_size;

	// allocate block
   BYTE *block = new BYTE[block_size];
	if (!block)
	{
		ERR_report("Unable to allocate memory to save properties block");
		return FAILURE;
	}

	BYTE *block_ptr = block;

	write_header(block_ptr);

	// write signature
   const int SIGNATURE_LEN = 5;
	char signature[SIGNATURE_LEN];
	strcpy_s(signature, SIGNATURE_LEN, "IPT2");
	memcpy(block_ptr, signature, 4);
	block_ptr += 4;
		
	// write size of ipt block
	memcpy(block_ptr, &properties_block_size, sizeof(int));
	block_ptr += sizeof(int);

	m_properties->save_settings(block_ptr);

	// write block to disk
   if (!file.Open(filespec, CFile::modeWrite |
      CFile::shareExclusive | CFile::modeCreate, &ex))
   {
      ERR_report_exception("Couldn't open source file for writing", ex);
      AfxMessageBox("Couldn't open source file for writing.  See error log for details");
      
      delete [] block; 
      return E_FAIL;
   }

	file.Write(block, block_size);
   file.Close();

	// clean up allocated memory
	delete [] block;

	return SUCCESS;
}

int TextParser::load_settings(CString filename) 
{ 
	// read in properties and create new packets for each
	CFile file;
	CFileException ex;

	// try to open the file for reading
   if (!file.Open(filename, CFile::modeRead | CFile::shareExclusive, &ex))
   {
      ERR_report_exception("Couldn't open source file for reading", ex);
      AfxMessageBox("Couldn't open source file for reading.  See error log for details");
      
      return FAILURE;
   }

	if (read_header(file) != SUCCESS)
	{
		ERR_report("Unable to read options file");
		return FAILURE;
	}

	// get block signature.  If the number of bytes returned
   // from the Read() function is < 0 then we are probably at
   // the end of the file
	char signature[4];
   int byte_count = file.Read(signature, 4);

	if (strncmp(signature,"IPT2",4) != 0)
	{
		ERR_report("Invalid block signature in .ipt file - you may have an older version of an .ipt file");
		return FAILURE;
	}

	if (byte_count == 4)
	{
		int block_size;

		// get number of bytes in the current block
      file.Read(&block_size, sizeof(int));

		if (block_size > 0)
		{
			// get the block of data 
         BYTE *block = new BYTE[block_size];
         file.Read(block, block_size);

			BYTE *block_ptr = block;

			m_properties->load_settings(block_ptr);

         delete [] block;
		}
	}

	return SUCCESS;
}

CString TextParser::get_info_text()
{
	CString ret;
	TextParserProperties *prop = (TextParserProperties *)m_properties;

	CString str;
	
	// setup info text
	str.Format("Importing: %s\r\n", get_file_name());
	ret += str;
	
	str.Format("Starting from row: %d\r\n\r\n", prop->get_start_row());
	ret += str;

	if (prop->get_is_delimited())
	{
		int d = prop->get_delimiter();

		str.Format("Delimited:\r\n");
		
		if ( (d & DELIM_TAB) == DELIM_TAB)
			str += "     Tab\r\n";
		
		if ( (d & DELIM_SPACE) == DELIM_SPACE)
			str += "     Space\r\n";
		
		if ( (d & DELIM_SEMICOLON) == DELIM_SEMICOLON)
			str += "     Semicolon\r\n";
		
		if ( (d & DELIM_COMMA) == DELIM_COMMA)
			str += "     Comma\r\n";
		
		if ( (d & DELIM_OTHER) == DELIM_OTHER)
			str += "     " + CString(prop->get_other_delimiter()) + "\r\n";

		ret += str;
	}
	else
	{
		ret += "Fixed Width\r\n";
	}

	return ret;
}
