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

// ExcelParser.cpp
//

#include "stdafx.h"
#include "ExcelParser.h"
#include "err.h"

// BOF record from Microsoft Excel based on http://www.wotsit.org/download.asp?f=excelfileformat&sc=304688603

typedef struct _xlbof
{
   char bofMarker; // Should be 0x09

   char vers;  // Version indicator for biff2, biff3, and biff4
               // = 0x00 -> Biff2
               // = 0x02 -> Biff3
               // = 0x04 -> Biff4
               // = 0x08 -> Biff5/Biff7/Biff8

   short int bofType; // Should = 0x0010 for worksheet

   short int vers2;  // Version number
                     // 0x0500 -> Biff5/Biff7
                     // 0x0600 -> Biff8

   short int buildYear;     // not used by FV

   int rupBuild;  // File History Flag (not used by FV)
   int rupYear;   // Lowest Excel that can read this file (Not used by FV)
} XLBOF;

typedef struct 
{
   short id;
   unsigned short size;
} record_header_t;

typedef struct
{
   BYTE unknown[6];
   short length;
} boundsheet_t;

typedef struct
{
   // contains 0x0600 for BIFF8
   short version;

   // WORKBOOK_GLOBALS = 0x5, VB_MODULE = 0x6, WORKSHEET = 0x10, CHART = 0x20
   short data_type;

   short int rupBuild;  // Internal build identifier
   short int rupYear;   // Internal Build year

   int file_history_flags;

   // lowest Excel version that can read all records in this file
   int lowest_version;
} XL_BOF;

const int XL_EOF = 0xA;
const int BOF = 0x0809;
const int BOF_VER = 0x0600;
const int XL_BOUNDSHEET = 0x85;
const int XL_SST = 0xFC;
const int XL_CONTINUE = 0x3C;
const int XL_RK = 0x27E;
const int XL_LABELSST = 0xFD;
const int XL_NUMBER = 0x203;
const int XL_MULRK = 0xBD;


// constructor
ExcelParser::ExcelParser(CString file_name) : 
   m_pStorage(NULL), m_pStream(NULL)
{
   m_properties = new ExcelParserProperties;
   m_file_name = file_name;
}

// destructor
ExcelParser::~ExcelParser()
{
   delete m_properties;
   close_file();

   m_string_table.RemoveAll();
}

// open the file to be parsed.  Returns SUCCESS/FAILURE
int ExcelParser::open_file(BOOL bSkipToStartRow /* = FALSE */)
{
   // Translate filename to Unicode
   TRACE(_T("This code requires testing due to security changes (Line %d File %s).  Remove this message after this line has been executed."), __LINE__, __FILE__);
   WCHAR wcFilename[MAX_PATH+1];
   size_t charsConverted;
   int i = mbstowcs_s(&charsConverted, wcFilename, MAX_PATH+1, m_file_name, strlen(m_file_name));
   wcFilename[i] = 0;
   
   HRESULT hr;
   XLBOF xlbof;
   
   // Open the document as an OLE compound document
   hr = ::StgOpenStorage(wcFilename, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &m_pStorage);
   
   if(!FAILED(hr)) 
   {
      // Open the data-stream where Microsoft Excel stores the data
      hr = m_pStorage->OpenStream(L"Workbook", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &m_pStream);
      
      // If "Workbook" does not exist, try "Book"
      if(FAILED(hr)) 
      {
         hr = m_pStorage->OpenStream(L"Book", NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &m_pStream);
      }
      if(!FAILED(hr)) 
      {
         // Read the relevant BOF information
         DWORD bytes_read;
         ULONG size = sizeof(XLBOF);
         m_pStream->Read(&xlbof, size, &bytes_read);
      }
      else
         return FAILURE;
   }
   else 
      return FAILURE;
   
   // Determine which version to return
   int version;
   if(xlbof.vers != 0x08) 
      version = (xlbof.vers + 4) / 2;
   else 
   {
      switch(xlbof.vers2) 
      {
      case 0x0500:  // Either Biff5 or Biff7
         // Biff7's rupYear is at least 1994
         if(xlbof.rupYear < 1994) return 5;
         
         // Check for specific builds of Microsoft Excel 5
         switch(xlbof.rupBuild) 
         {
         case 2412: // XL5a
         case 3218: // XL5c
         case 3321: // NT XL5
            version = 5;
         default:
            version = 7;
         }
         
         case 0x0600:  version = 8;
      }
   }
   
   // Version not recognized. Perhaps there is a newer version.
   if (version != 8)
      return FAILURE;

   m_read_first_row = false;
   m_eof = false;

   return SUCCESS;
}

// return a list of sheets in the Excel file
int ExcelParser::get_sheet_names(CList<CString,CString> &sheet_list)
{
   if (m_pStream == NULL)
   {
      ERR_report("Excel file not yet opened.");
      return FAILURE;
   }

   record_header_t record_header;
   do
   {
      DWORD bytes_read;
      HRESULT hr = m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);

      if (FAILED(hr))
         return FAILURE;

      // the boundsheet records contain the names of the excel 
      if (record_header.id == XL_BOUNDSHEET)
      {
         boundsheet_t bsheet;
         m_pStream->Read(&bsheet, sizeof(boundsheet_t), &bytes_read);
         char *name = new char[bsheet.length + 1];
         m_pStream->Read((void *)name, bsheet.length, &bytes_read);
         name[bsheet.length] = '\0';
         sheet_list.AddTail(CString(name));
         delete [] name;
      }
      else
      {
         //TRACE("Rec ID: %xx  Size: %xx",record_header.id,record_header.size);
         LARGE_INTEGER large_int;
         large_int.LowPart = record_header.size;
         large_int.HighPart = 0;
         m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);
      }
      
   } while (record_header.id != XL_EOF);

   return SUCCESS;
}

double rk_to_double(int b1, int b2, int b3, int b4) 
{
   double d;
   int i;
   unsigned char *ieee;
   
   // integer value
   if ((b1 & 0x2) != 0)
   {
      int v;
      v = b1 | (b2 << 8) | (b3 << 16) | (b4 << 24);
      v = v >> 2;
      d = (double)v;
   }
   // ieee floating point value
   else
   {
      ieee = (unsigned char *) &d;
      for (i = 0; i < 8; i++) ieee[i] = 0;
      
      ieee[4] = ((int)b1) & 0xff;
      ieee[5] = ((int)b2) & 0xff;
      ieee[6] = ((int)b3) & 0xff;
      ieee[7] = ((int)b4) & 0xff;
      if ((ieee[4] & 0x01) != 0)
         d /= 100;
   }
   
   return d;
}

int ExcelParser::read_string_table()
{
   record_header_t record_header;

   // seek to the beginning of the stream
   LARGE_INTEGER large_int;
   large_int.LowPart = 0;
   large_int.HighPart = 0;
   m_pStream->Seek(large_int, STREAM_SEEK_SET, NULL);
   
   // clear out string table
   m_string_table.RemoveAll();
   
   // store SST entries from worksheet header
   do 
   {
      DWORD bytes_read;
      HRESULT hr = m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);
      
      if (FAILED(hr))
         return FAILURE;
      
      // read in string table
      if (record_header.id == XL_SST)
      {
         int start = record_header.size;
         
         int total_num_strings;
         m_pStream->Read(&total_num_strings, 4, &bytes_read);
         record_header.size -= 4;
         
         int num_entries;
         m_pStream->Read(&num_entries, 4, &bytes_read);
         record_header.size -= 4;
         
         for(int i=0;i<num_entries;i++)
         {
            if (record_header.size == 0)
            {
               m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);
               ASSERT(record_header.id == XL_CONTINUE);
            }
            
            short num_chars;
            m_pStream->Read(&num_chars, 2, &bytes_read);
            record_header.size -= 2;
            
            BYTE options_flags;
            m_pStream->Read(&options_flags, 1, &bytes_read);
            record_header.size -= 1;
            
            bool extended_string = ((options_flags & 0x04) != 0);
            bool rich_string = ((options_flags & 0x08) != 0);
            bool is_ascii = ((options_flags & 0x01) == 0);
            
            int run_length;
            if (rich_string) 
            {
               short t;
               m_pStream->Read(&t, 2, &bytes_read);
               record_header.size -= 2;
               run_length = t;
            } 
            else if (extended_string)
            {
               m_pStream->Read(&run_length, 4, &bytes_read);
               record_header.size -= 4;
            }

            // ascii encoding
            if (is_ascii)
            {
               int old_size = record_header.size;
               int N = min(record_header.size, num_chars);
               char *c = new char[N + 1];
               m_pStream->Read(c, N, &bytes_read);
               c[N] = '\0';
               CString str(c);
               CString str2;
               
               // if the record is split
               if (record_header.size < num_chars)
               {
                  m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);
                  ASSERT(record_header.id == XL_CONTINUE);
                  
                  // repeat options flags.  NOTE : split string can be unicode in
                  // one half and ascii in the other!
                  m_pStream->Read(&options_flags, 1, &bytes_read);
                  record_header.size -= 1;

                  bool is_ascii = ((options_flags & 0x01) == 0);
                  
                  // read the rest
                  if (is_ascii)
                  {
                     char *c2 = new char[num_chars - old_size + 1];
                     m_pStream->Read(c2, num_chars - old_size, &bytes_read);
                     record_header.size -= (num_chars - old_size);
                     c2[num_chars - old_size] = '\0';
                     str2 = CString(c2);
                     delete [] c2;
                  }
                  else
                  {
                     int chars_to_read = num_chars - old_size;

                     wchar_t *w = new wchar_t[chars_to_read + 1];
                     m_pStream->Read(w, chars_to_read*2, &bytes_read);
                     record_header.size -= (chars_to_read*2);

                     AfxBSTR2CString(&str2, _bstr_t(w));
                     delete [] w;
                  }
                  
                  str += str2;
                  //delete c2;
               }
               else
                  record_header.size -= N;
               
               m_string_table.AddTail(str);
               delete [] c;
            } // end is_ascii
            
            // unicode encoding
            else 
            {
               int old_size = record_header.size;
               int N = min(record_header.size, num_chars*2);
               CString str, str2;

               wchar_t *w = new wchar_t[N/2 + 1];
               wchar_t *w2 = NULL;
               m_pStream->Read(w, N, &bytes_read);
               
               // split
               if (record_header.size < num_chars*2)
               {
                  m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);
                  ASSERT(record_header.id == XL_CONTINUE);
                  
                  // repeat options flags.  NOTE : split string can be unicode in
                  // one half and ascii in the other!
                  m_pStream->Read(&options_flags, 1, &bytes_read);
                  record_header.size -= 1;

                  bool is_ascii = ((options_flags & 0x01) == 0);
                  
                  // read the rest
                  int chars_to_read = num_chars - old_size/2;

                  if (is_ascii)
                  {
                     char *c = new char[chars_to_read + 1];
                     m_pStream->Read(c, chars_to_read, &bytes_read);
                     record_header.size -= chars_to_read;

                     c[chars_to_read] = '\0';
                     str2 = CString(c);
                     delete [] c;
                  }
                  else
                  {
                     w2 = new wchar_t[chars_to_read];
                     
                     m_pStream->Read(w2, chars_to_read*2, &bytes_read);
                     record_header.size -= (chars_to_read * 2);
                     
                     AfxBSTR2CString(&str2, _bstr_t(w2));
                     delete [] w2;
                  }
               }
               else
                  record_header.size -= N;
               
               try
               {
                  AfxBSTR2CString(&str, _bstr_t(w));
               }
               catch(...) { }
               
               m_string_table.AddTail(str + str2);
               delete [] w;
            } // end unicode
            
            // skip unnecessary bytes in stream
            large_int.HighPart = 0;
            if (rich_string) 
            {
               large_int.LowPart = 4 * run_length;
               m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);
               record_header.size -= 4 * run_length;
            }
            else if (extended_string) 
            {
               large_int.LowPart = run_length;
               m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);
               record_header.size -= run_length;
            }
         }
      }
      else
      {
         
         large_int.LowPart = record_header.size;
         large_int.HighPart = 0;
         m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);
      }
      
   } while (record_header.id != XL_EOF);

   return SUCCESS;
}

// moves the stream pointer to the beginning of the next sheet
//
int ExcelParser::skip_sheet()
{
   record_header_t record_header;
   DWORD bytes_read;

   do
   {
      if (FAILED(m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read)))
      {
         ERR_report("Error reading from stream");
         return FAILURE;
      }

      LARGE_INTEGER large_int;
      large_int.LowPart = record_header.size;
      large_int.HighPart = 0;
      
      m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);
   } while (record_header.id != XL_EOF);

   return SUCCESS;
}

// get the next line of data in the excel file.  The data will be
// tab-delimited.  Returns SUCCESS/FAILURE.
//
int ExcelParser::get_row(CString &line)
{
   if (m_eof)
      return FAILURE;

   record_header_t record_header;

   // if this is the first call to get_row since the file has been opened
   if (!m_read_first_row)
   {
      // only read the string table once
      if (m_string_table.GetCount() == 0)
      {
         if (read_string_table() != SUCCESS)
         {
            ERR_report("Failure reading string table from worksheet header");
            return FAILURE;
         }
      }
      // we've already read string table, skip over worksheet's header
      else
      {
         do 
         {
            DWORD bytes_read;
            HRESULT hr = m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);

            LARGE_INTEGER large_int;
            large_int.LowPart = record_header.size;
            large_int.HighPart = 0;
            m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);

         } while (record_header.id != XL_EOF);
      }
   
      // skip to the m_import_sheet_num sheet
      const int sheet_num = ((ExcelParserProperties *)m_properties)->get_import_sheet_num();
      for(int i=0;i<sheet_num;i++)
         skip_sheet();

      m_read_first_row = true;
      m_current_row = -1;
   }

   m_current_row++;

   int current_col = 0;
   do
   {
      // store the current position of the stream
      LARGE_INTEGER large_int;
      large_int.LowPart = 0;
      large_int.HighPart = 0;
      ULARGE_INTEGER current_seek_pos;
      m_pStream->Seek(large_int, STREAM_SEEK_CUR, &current_seek_pos);
      
      DWORD bytes_read;      
      HRESULT hr = m_pStream->Read(&record_header, sizeof(record_header_t), &bytes_read);
      
      if (FAILED(hr))
         break;

      if (bytes_read == 0)
         return SUCCESS;
      
      // BOF
      if (record_header.id == BOF)
      {
         XL_BOF bof;
         m_pStream->Read(&bof, sizeof(bof), &bytes_read);
         
         // verify version
         if (bof.version != BOF_VER)
         {
            ERR_report("Invalid version of BOF");
            return 0;
         }
      }

      // RK
      else if (record_header.id == XL_RK)
      {
         short row, col, xf_index;

         // get the current seek location
         m_pStream->Read(&row, 2, &bytes_read);
         if (row != m_current_row)
         {
            large_int.LowPart = current_seek_pos.LowPart;
            HRESULT hr = m_pStream->Seek(large_int, STREAM_SEEK_SET, NULL);
            return SUCCESS;
         }

         m_pStream->Read(&col, 2, &bytes_read);

         m_pStream->Read(&xf_index, 2, &bytes_read);
         BYTE b[4];
         m_pStream->Read(b, 4, &bytes_read);
         double d = rk_to_double(b[0], b[1], b[2], b[3]);
         
         record_header.size -= 10;

         while (current_col < col)
         {
            current_col++;
            line += "\t";
         }
         CString rk_str;
         rk_str.Format("%f", d);
         line += rk_str;
      }

      // LABELSST
      else if (record_header.id == XL_LABELSST)
      {
         short row, col, xf_index;
         int string_table_index;

         m_pStream->Read(&row, 2, &bytes_read);
         if (row != m_current_row)
         {
            large_int.LowPart = current_seek_pos.LowPart;
            HRESULT hr = m_pStream->Seek(large_int, STREAM_SEEK_SET, NULL);
            return SUCCESS;
         }

         m_pStream->Read(&col, 2, &bytes_read);

         m_pStream->Read(&xf_index, 2, &bytes_read);
         m_pStream->Read(&string_table_index, 4, &bytes_read);
         
         POSITION pos = m_string_table.FindIndex(string_table_index);
         if (pos)
         {
            CString str = m_string_table.GetAt(pos);
            
            while (current_col < col)
            {
               current_col++;
               line += "\t";
            }
            line += str;
         }
      }
      // NUMBER
      else if (record_header.id == XL_NUMBER)
      {
         short row, col, xf_index;
         double value;
         
         m_pStream->Read(&row, 2, &bytes_read);
         if (row != m_current_row)
         {
            large_int.LowPart = current_seek_pos.LowPart;
            HRESULT hr = m_pStream->Seek(large_int, STREAM_SEEK_SET, NULL);
            return SUCCESS;
         }

         m_pStream->Read(&col, 2, &bytes_read);

         m_pStream->Read(&xf_index, 2, &bytes_read);
         m_pStream->Read(&value, 8, &bytes_read);

         while (current_col < col)
         {
            current_col++;
            line += "\t";
         }
         CString num_str;
         num_str.Format("%f", value);
         line += num_str;
      }
      // MULRK
      else if (record_header.id == XL_MULRK)
      {
         short row, first_col, xf_index;

         m_pStream->Read(&row, 2, &bytes_read);
         if (row != m_current_row)
         {
            large_int.LowPart = current_seek_pos.LowPart;
            HRESULT hr = m_pStream->Seek(large_int, STREAM_SEEK_SET, NULL);
            return SUCCESS;
         }

         m_pStream->Read(&first_col, 2, &bytes_read);

         while (current_col < first_col)
         {
            current_col++;
            line += "\t";
         }

         int num_cols = (record_header.size - 6)/6;
         for(int i=0;i<num_cols;i++)
         {
            m_pStream->Read(&xf_index, 2, &bytes_read);
            BYTE b[4];
            m_pStream->Read(b, 4, &bytes_read);
            double d = rk_to_double(b[0], b[1], b[2], b[3]);

            CString rk_str;
            rk_str.Format("%f", d);
            line += rk_str;

            line += "\t";
            current_col++;
         }
         short last_col;
         m_pStream->Read(&last_col, 2, &bytes_read);
      }
      else
      {
         LARGE_INTEGER large_int;
         large_int.LowPart = record_header.size;
         large_int.HighPart = 0;
         
         m_pStream->Seek(large_int, STREAM_SEEK_CUR, NULL);
      }
      
   } while (record_header.id != XL_EOF);

   m_eof = true;
   if (line.GetLength() > 0)
   {
      return SUCCESS;
   }

   return FAILURE;
}

   // get a list of tokens from the given line
int ExcelParser::get_tokens(CList<CString, CString>& token_list)
{
   CString line;
   if (get_row(line) != SUCCESS)
      return FAILURE;

   CString current_token;
   for(int i=0;i<line.GetLength();i++)
   {
      if (line[i] == '\t')
      {
         token_list.AddTail(current_token);
         current_token = "";
      }
      else
         current_token += line[i];
   }

   token_list.AddTail(current_token);

   return SUCCESS;
}

// close the file.  Returns SUCCESS/FAILURE.
int ExcelParser::close_file()
{
   if (m_pStorage != NULL)
   {
      m_pStorage->Release();
      m_pStorage = NULL;
   }

   if (m_pStream != NULL)
   {
      m_pStream->Release();
      m_pStream = NULL;
   }

   return SUCCESS;
}

int ExcelParser::save_settings(CString filespec) 
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

int ExcelParser::load_settings(CString filespec)
{
   // read in properties and create new packets for each
   CFile file;
   CFileException ex;

   // try to open the file for reading
   if (!file.Open(filespec, CFile::modeRead | CFile::shareExclusive, &ex))
   {
      ERR_report_exception("Couldn't open source file for writing", ex);
      AfxMessageBox("Couldn't open source file for writing.  See error log for details");
      
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
      ERR_report("Invalid block signature in .ipt file");
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

int ExcelParserProperties::get_block_size()
{
   return ParserProperties::get_block_size() + sizeof(m_import_sheet_num);
}

int ExcelParserProperties::save_settings(BYTE *&block_ptr)
{
   ParserProperties::save_settings(block_ptr);

   save_int(block_ptr, m_import_sheet_num);

   return SUCCESS;
}

int ExcelParserProperties::load_settings(BYTE *&block_ptr)
{
   ParserProperties::load_settings(block_ptr);

   m_import_sheet_num = load_int(block_ptr);

   return SUCCESS;
}

CString ExcelParser::get_info_text()
{
   CString ret;
   ExcelParserProperties *prop = (ExcelParserProperties *)m_properties;

   CString str;
   
   // setup info text
   str.Format("Importing: %s\r\n", get_file_name());
   ret += str;
   
   str.Format("Starting from row: %d\r\n", prop->get_start_row());
   ret += str;

   // note: sheet number is 0-based
   str.Format("Importing sheet number: %d", prop->get_import_sheet_num() + 1);
   ret += str;

   return ret;
}
