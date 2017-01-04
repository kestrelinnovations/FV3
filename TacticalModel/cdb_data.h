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

// cdbdata.h

#pragma once

#define CDB_DATA_SENTINEL  "CDB_REC_DAT"
#define CDB_GEOCELL_SENTINEL  "CDB_GEOCELL_DAT"

struct cdb_item_rec
{
    double lat;
    double lon;
    double bsr;
    double alt_meters;
    unsigned char lod;
    unsigned char man_or_nat;  // 1 = man-made,  2 = natural
    char   filename[260];  // includes the relative path from the base
};

struct cdb_rec_file_header
{
    char sentinel[12]; // "CDB_REC_DAT"
    char version[5];   // "0001"
    char date[9];      // YYYYMMDD
    char hdr_len[3];   // ascii lengh of header
    char rec_len[5];   // ascii lengh of each record
    char path[260];    // path to data root
};

typedef struct
{
    char sentinel[16];  // "CDB_GEOCELL_DAT" including NULL
    char version[5];   // "0001"
    char date[9];      // YYYYMMDD
    char hdr_len[3];   // ascii lengh of header
    char rec_len[5];   // ascii lengh of each record
    unsigned int num_recs;
    double ll_lat;
    double ll_lon;
    double ur_lat;
    double ur_lon;
    char north_str[3];
    char east_str[4];
} geocell_header_t;


typedef struct
{
   short lod;
   short code;  // 100 or 101
   double ll_lat;
   double ll_lon;
   double ur_lat;
   double ur_lon;
   char rel_path[34]; // ex. "N33\W118\100_GSFeature\L03\U4"
   int up_num;
   int right_num;
   int model_cnt;
} geocell_t;

class C_cdb_data
{
public:
    C_cdb_data();
    ~C_cdb_data();

    int load(CString path, CString & err_msg);
    int load_geocell_index(CString path, CString & err_msg);
    void close();
    void close_geocell_index();
    int get_num_recs();
    int get_num_index_recs();
    int get_data(int recnum, cdb_item_rec *rec);
    int get_geocell_index_data(int recnum, geocell_t *rec);
    int get_file_size(CString filename, unsigned int *file_size);
    int get_big_file_size( CString csFileSpec, UINT64& qwFileSize );
    int read_file(HANDLE & file, void * buf, int bytes_to_read);

    CString get_date();
    FILE* create(CString path,  CString data_root);
    FILE* create_geocell_index(CString path, double ll_lat, double ll_lon, double ur_lat, double ur_lon);

    HANDLE m_fh;
    HANDLE m_ndx_fh;
    cdb_rec_file_header m_header;
    geocell_header_t m_ndx_header;
    int m_numrecs;
    int m_num_ndx_recs;
    CString m_path;
    CString m_ndx_path;

};