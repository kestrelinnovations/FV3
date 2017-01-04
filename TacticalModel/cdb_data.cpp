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

// cdb_data.cpp

#include "stdafx.h"
#include "cdb_data.h"
#include "common.h"

// **************************************************************************
// **************************************************************************

C_cdb_data::C_cdb_data()
{
    m_fh = NULL;
    m_numrecs = 0;
}

// **************************************************************************
// **************************************************************************

C_cdb_data::~C_cdb_data()
{
    close();
}

// **************************************************************************
// **************************************************************************

void C_cdb_data::close()
{
    if (m_fh != NULL)
        CloseHandle(m_fh);

    m_fh = NULL;
}

// **************************************************************************
// **************************************************************************

void C_cdb_data::close_geocell_index()
{
    if (m_ndx_fh != NULL)
        CloseHandle(m_ndx_fh);

    m_ndx_fh = NULL;
}

// **************************************************************************
// **************************************************************************

int C_cdb_data::read_file(HANDLE & file, void * buf, int bytes_to_read)
{
   BOOL rslt;
   DWORD nBytesRead = 0;
   DWORD dwSize = bytes_to_read;

   if (file == INVALID_HANDLE_VALUE)
      return FAILURE;

   rslt = ReadFile( file, buf, dwSize, &nBytesRead, NULL );

   if (!rslt)
   {
      CloseHandle(file);
      file = INVALID_HANDLE_VALUE;
      return FAILURE;
   }

//   if (nBytesRead == 0)
//      return END_OF_FILE;

   if ( nBytesRead != dwSize )
   {
      CloseHandle(file);
      file = INVALID_HANDLE_VALUE;
      return FAILURE;
   }

   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

 int C_cdb_data::load(CString path, CString & err_msg)
{
    int rslt;
    CString tstr;
    unsigned int file_size;

    rslt = get_file_size(path, &file_size);

    if (m_fh != NULL)
        close();

   m_fh = CreateFile( path, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if (m_fh == INVALID_HANDLE_VALUE)
   {
      err_msg.Format("Error opening - %s", path);
      return FAILURE;
   }

   rslt = read_file(m_fh, &m_header, sizeof(cdb_rec_file_header));
   if (rslt != SUCCESS)
   {
      err_msg.Format("Error opening - %s", path);
      return FAILURE;
   }

    m_header.sentinel[11] = 0;
    tstr = m_header.sentinel;
    if (tstr.Compare(CDB_DATA_SENTINEL))
    {
        CloseHandle(m_fh);
        m_fh = NULL;
        return FAILURE;
    }

    m_path = path;

    m_numrecs = get_num_recs();

    return SUCCESS;
 }
// **************************************************************************
// **************************************************************************

int C_cdb_data::load_geocell_index(CString path, CString & err_msg)
{
    int rslt;
    CString tstr;
    unsigned int file_size;

    rslt = get_file_size(path, &file_size);

    if (m_ndx_fh != NULL)
        close();

   m_ndx_fh = CreateFile( path, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if (m_ndx_fh == INVALID_HANDLE_VALUE)
   {
      err_msg.Format("Error opening - %s", path);
      return FAILURE;
   }

   rslt = read_file(m_ndx_fh, &m_ndx_header, sizeof(geocell_header_t));
   if (rslt != SUCCESS)
   {
      err_msg.Format("Error opening - %s", path);
      return FAILURE;
   }

    m_ndx_header.sentinel[15] = 0;
    tstr = m_ndx_header.sentinel;
    if (tstr.Compare(CDB_GEOCELL_SENTINEL))
    {
        CloseHandle(m_ndx_fh);
        m_ndx_fh = NULL;
        AfxMessageBox("Index file is not valid");
        return FAILURE;
    }

    m_ndx_header.version[4] = 0;
    tstr = m_ndx_header.version;
    if (tstr.Compare("0002"))
    {
        CloseHandle(m_ndx_fh);
        m_ndx_fh = NULL;
        AfxMessageBox("Index file is wrong version");
        return FAILURE;
    }

    m_ndx_path = path;

    m_num_ndx_recs = get_num_index_recs();

    return SUCCESS;
 }

// **************************************************************************
// **************************************************************************

int C_cdb_data::get_num_recs()
{
    int num, rslt;
    unsigned int filesize, headsize, recsize;

    if (m_fh == NULL)
        return -1;

    rslt = get_file_size(m_path, &filesize);
    if (rslt != SUCCESS)
        return -1;

    headsize = sizeof(cdb_rec_file_header);
    recsize = sizeof(cdb_item_rec);
    filesize -= headsize;
    num = filesize / recsize;

    return num;
}

// **************************************************************************
// **************************************************************************

int C_cdb_data::get_num_index_recs()
{
    int num, rslt;
    unsigned int filesize, headsize, recsize;

    if (m_ndx_fh == NULL)
        return -1;

    rslt = get_file_size(m_ndx_path, &filesize);
    if (rslt != SUCCESS)
        return -1;

    headsize = sizeof(geocell_header_t);
    recsize = sizeof(geocell_t);
    filesize -= headsize;
    num = filesize / recsize;

    return num;
}

// **************************************************************************
// **************************************************************************

int C_cdb_data::get_data(int recnum, cdb_item_rec *rec)
{
    unsigned int headsize, recsize, pos;
    int rslt;

    if (m_fh == NULL)
        return FAILURE;

    if (recnum < 1)
        return FAILURE;

    headsize = sizeof(cdb_rec_file_header);
    recsize = sizeof(cdb_item_rec);

    pos = headsize + (recsize * recnum);

    if (m_fh == NULL)
        return FAILURE;
    SetFilePointer(m_fh, pos, NULL, FILE_BEGIN);
    
    if (m_fh == NULL)
        return FAILURE;
    rslt = read_file(m_fh, rec, recsize);
    if (rslt != SUCCESS)
        return FAILURE;

    return SUCCESS;
}


// **************************************************************************
// **************************************************************************

int C_cdb_data::get_geocell_index_data(int recnum, geocell_t *rec)
{
    unsigned int headsize, recsize, pos;
    int rslt;

    if (m_ndx_fh == NULL)
        return FAILURE;

    if (recnum < 1)
        return FAILURE;

    headsize = sizeof(geocell_header_t);
    recsize = sizeof(geocell_t);

    pos = headsize + (recsize * recnum);

    if (m_ndx_fh == NULL)
        return FAILURE;
    SetFilePointer(m_ndx_fh, pos, NULL, FILE_BEGIN);
    
    if (m_ndx_fh == NULL)
        return FAILURE;
    rslt = read_file(m_ndx_fh, rec, recsize);
    if (rslt != SUCCESS)
        return FAILURE;

    return SUCCESS;
}


// **************************************************************************
// **************************************************************************

// get the size of a file

int C_cdb_data::get_big_file_size( CString csFileSpec, UINT64& qwFileSize )
{
   WIN32_FILE_ATTRIBUTE_DATA wfad;
   if ( !GetFileAttributesEx( csFileSpec, GetFileExInfoStandard, &wfad ) )
      return FAILURE;

   qwFileSize = ( ( (UINT64) wfad.nFileSizeHigh ) << 32 ) | wfad.nFileSizeLow;
   return SUCCESS;
}

// **************************************************************************
// **************************************************************************

int C_cdb_data::get_file_size(CString filename, unsigned int *file_size)
{
    UINT64 size;
    int rslt;

    rslt = get_big_file_size(filename, size);

    *file_size = (unsigned int) size;

    return rslt;
}
// end of get_file_size

// **************************************************************************
// **************************************************************************

CString C_cdb_data::get_date()
{
    CString date_str;
    SYSTEMTIME time;
    
   ::GetSystemTime(&time);
   date_str.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);

   return date_str;
}

// **************************************************************************
// **************************************************************************

FILE* C_cdb_data::create(CString path,  CString data_root)
{
    cdb_rec_file_header hdr;
    char blank = ' ';
    CString tstr, date;
    int len;
    FILE *fp;

    date = get_date();

    // fill the header with blanks
    memcpy(&hdr, &blank, sizeof(cdb_rec_file_header));

    strncpy_s(hdr.sentinel, 12, CDB_DATA_SENTINEL, _TRUNCATE);
    strncpy_s(hdr.version, 5,  "0001", _TRUNCATE);
    strncpy_s(hdr.date, date, 8);
    tstr.Format("%02d", sizeof(cdb_rec_file_header));
    strncpy_s(hdr.hdr_len, 3, tstr.GetBuffer(2), _TRUNCATE);
    tstr.Format("%02d", sizeof(cdb_item_rec));
    strncpy_s(hdr.rec_len, 5, tstr.GetBuffer(5), _TRUNCATE);
    len = data_root.GetLength();
    strncpy_s(hdr.path, 200, data_root.GetBuffer(len), _TRUNCATE);
    hdr.path[len] = 0;

    fopen_s(&fp, path, "wb");
    if (fp == NULL)
        return fp;

    size_t rslt = fwrite(&hdr, sizeof(cdb_rec_file_header), 1, fp);
    if (rslt == 1)
        return fp;

    fclose(fp);
    return NULL;
}

// **************************************************************************
// **************************************************************************

FILE* C_cdb_data::create_geocell_index(CString path, double ll_lat, double ll_lon, double ur_lat, double ur_lon)
{
    geocell_header_t hdr;
    char blank = ' ';
    CString tstr, date;
    FILE *fp;

    date = get_date();

    // fill the header with blanks
    memcpy(&hdr, &blank, sizeof(geocell_header_t));

    strncpy_s(hdr.sentinel, 16, CDB_GEOCELL_SENTINEL, _TRUNCATE);
    strncpy_s(hdr.version, 5,  "0002", _TRUNCATE);
    strncpy_s(hdr.date, date, 8);
    tstr.Format("%02d", sizeof(geocell_header_t));
    strncpy_s(hdr.hdr_len, 3, tstr.GetBuffer(2), _TRUNCATE);
    tstr.Format("%02d", sizeof(geocell_t));
    strncpy_s(hdr.rec_len, 5, tstr.GetBuffer(5), _TRUNCATE);

    hdr.ll_lat = ll_lat;
    hdr.ll_lon = ll_lon;
    hdr.ur_lat = ur_lat;
    hdr.ur_lon = ur_lon;

    fopen_s(&fp, path, "wb");
    if (fp == NULL)
        return fp;

    size_t rslt = fwrite(&hdr, sizeof(geocell_header_t), 1, fp);
    if (rslt == 1)
        return fp;

    fclose(fp);
    return NULL;
}

// **************************************************************************
// **************************************************************************

