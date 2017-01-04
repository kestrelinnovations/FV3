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

// Parser.cpp
//

#include "stdafx.h"
#include "Parser.h"

#include "err.h"

const int MAJOR_VERSION = 4;
const int MINOR_VERSION = 0;
const int IPT_REVISION = 0;
const int MAX_STR_LENGTH = 256;

void ParserProperties::set_start_row(int s)
{ 
	m_start_row = s; 
}

void ParserProperties::delete_filter(int i)
{
   POSITION position = m_filter_list.FindIndex(i);
	if (position)
		m_filter_list.RemoveAt(position);
	else
		ERR_report("Invalid index");
}

int ParserProperties::get_block_size()
{
	int block_size = 0;

	// column header list entries
	block_size += sizeof(int);
	POSITION position = m_column_header_list.GetHeadPosition();
	while (position)
		block_size += sizeof(int) + m_column_header_list.GetNext(position).GetLength();

	block_size += sizeof(int);
	position = m_filter_list.GetHeadPosition();
	while (position)
	{
		filter_t filter = m_filter_list.GetNext(position);
		block_size += sizeof(int) + filter.column_header.GetLength();
		block_size += sizeof(int) + filter.filter_string.GetLength();
		block_size += sizeof(int) + filter.icon_assigned.GetLength();
      block_size += sizeof(int) + filter.group_name.GetLength();
      block_size += sizeof(int);    // color
      block_size += sizeof(int) + filter.shape_type.GetLength();
   }

	block_size += sizeof(m_start_row);

	return block_size;
}

int ParserProperties::save_settings(BYTE *&block_ptr)
{
	save_int(block_ptr, m_column_header_list.GetCount());
	POSITION position = m_column_header_list.GetHeadPosition();
	while (position)
		save_string(block_ptr, m_column_header_list.GetNext(position));

	save_int(block_ptr, m_filter_list.GetCount());
	position = m_filter_list.GetHeadPosition();
	while (position)
	{
		filter_t filter = m_filter_list.GetNext(position);
		save_string(block_ptr, filter.column_header);
		save_string(block_ptr, filter.filter_string);
		save_string(block_ptr, filter.icon_assigned);
      save_string(block_ptr, filter.group_name);
      save_int(block_ptr, filter.color);
      save_string(block_ptr, filter.shape_type);
	}

	save_int(block_ptr, m_start_row);

	return SUCCESS;
}

int ParserProperties::load_settings(BYTE *&block_ptr)
{
   m_column_header_list.RemoveAll();
	int n = load_int(block_ptr);
   int i;
	for(i=0;i<n;i++)
		m_column_header_list.AddTail(load_string(block_ptr));

   m_filter_list.RemoveAll();
	n = load_int(block_ptr);
	for(i=0;i<n;i++)
	{
		filter_t filter;
		filter.column_header = load_string(block_ptr);
		filter.filter_string = load_string(block_ptr);
		filter.icon_assigned = load_string(block_ptr);
      filter.group_name = load_string(block_ptr);
      filter.color = load_int(block_ptr);
      filter.shape_type = load_string(block_ptr);
      m_filter_list.AddTail(filter);
	}

	m_start_row = load_int(block_ptr);

	return SUCCESS;
}

void ParserProperties::save_string(BYTE *&block_ptr, CString str)
{
	int length = str.GetLength();

	if (length > (MAX_STR_LENGTH - 1))
		length = MAX_STR_LENGTH - 1;

	memcpy(block_ptr, &length, sizeof(int));
	block_ptr += sizeof(int);

	if (length > 0)
	{
		memcpy(block_ptr, str.GetBuffer(length), length);
		block_ptr += length;
	}
}

CString ParserProperties::load_string(BYTE *&block_ptr)
{
	CString str;
	int length;

	memcpy(&length, block_ptr, sizeof(int));
	block_ptr += sizeof(int);

	char tmp_buffer[MAX_STR_LENGTH];

	if (length != 0) 
   {
		memcpy(tmp_buffer,block_ptr,length);
		block_ptr += length;
		tmp_buffer[length] = '\0';

		str = CString(tmp_buffer);
	}

	return str;
}

void ParserProperties::save_int(BYTE *&block_ptr, int v)
{
	memcpy(block_ptr, &v, sizeof(int));
	block_ptr += sizeof(int);
}

int ParserProperties::load_int(BYTE *&block_ptr)
{
	int i;
	memcpy(&i, block_ptr, sizeof(int));
	block_ptr += sizeof(int);

	return i;
}

int Parser::write_header(BYTE *&block_ptr)
{
   const int HEADER_LEN = 7;
   char header[HEADER_LEN];
   
   // write the file's header
   strcpy_s(header, HEADER_LEN, "FVWIPT");
   memcpy(block_ptr, header, 6);
   block_ptr += 6;
   
   // write the version info
   BYTE major = ((int)MAJOR_VERSION/10 << 4) | (MAJOR_VERSION-(10*((int)MAJOR_VERSION/10)));
   BYTE minor = ((int)MINOR_VERSION/10 << 4) | (MINOR_VERSION-(10*((int)MINOR_VERSION/10)));
   memcpy(block_ptr, &major, 1);
   block_ptr++;
   memcpy(block_ptr, &minor, 1);
   block_ptr++;
   short revision = IPT_REVISION;
   memcpy(block_ptr, &revision, sizeof(short));
   block_ptr += sizeof(short);

	return SUCCESS;
}

int Parser::read_header(CFile &file)
{
	// read and validate the header
   char header[6];
   file.Read(header, 6);
   if (strncmp(header, "FVWIPT", 6) != 0)
      return FAILURE;
   
   // read and validate the file version number
   BYTE major, minor;
   short revision;
   file.Read(&major, 1);
   file.Read(&minor, 1);
   file.Read(&revision, sizeof(short));

	int M = (major >> 4)*10 + (major & 0x0F);
	int m = (minor >> 4)*10 + (minor & 0x0F);

	if (M != MAJOR_VERSION || m != MINOR_VERSION || revision != IPT_REVISION)
	{
		ERR_report("Version of file opened is incorrect");
		return FAILURE;
	}

	return SUCCESS;
}
