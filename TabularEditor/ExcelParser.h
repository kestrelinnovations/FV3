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

// ExcelParser.h 
//

#pragma once

#include "Parser.h"

class ExcelParserProperties : public ParserProperties
{
   int m_import_sheet_num;

public:

   // set the 1-based excel sheet number to import from
   void set_import_sheet_num(int n) { m_import_sheet_num = n; }
   int get_import_sheet_num() { return m_import_sheet_num; }

   virtual int get_block_size();
   virtual int save_settings(BYTE *&block_ptr);
   virtual int load_settings(BYTE *&block_ptr);
};

class ExcelParser : public Parser
{
   IStorage *m_pStorage;
   IStream *m_pStream;

   bool m_read_first_row;
   bool m_eof;

   CList <CString, CString> m_string_table;
   int m_current_row;

public:

   // constructor
   ExcelParser(CString file_name);

   // destructor
   ~ExcelParser();

   // open the file to be parsed.  Returns SUCCESS/FAILURE
   virtual int open_file(BOOL bSkipToStartRow = FALSE);

   // close the file.  Returns SUCCESS/FAILURE.
   virtual int close_file();

   virtual int save_settings(CString filename);
   virtual int load_settings(CString filename);

   // get the next row of data in the excel file.  The data will be
   // returned tab-delimited.  Returns SUCCESS/FAILURE.
   int get_row(CString &line);

   // get a list of tokens from the given line
   virtual int get_tokens(CList<CString, CString>& token_list);

   // return a list of the sheets in a workbook
   int get_sheet_names(CList<CString,CString> &sheet_list);

   virtual bool is_kind_of(CString str) 
   { 
      if (str.Compare("ExcelParser") == 0) 
         return true;

      return Parser::is_kind_of(str);
   }
   virtual CString get_info_text();

private:
   int read_string_table();
   int skip_sheet();
};