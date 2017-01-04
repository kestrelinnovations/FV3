// Copyright (c) 1994-2011,2013 Georgia Tech Research Corporation, Atlanta, GA
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



// cdbase.h : header file
//

#ifndef CDBASE_H
#define CDBASE_H

typedef struct
{
   char    Name[11];
   char    Tipe;
   BYTE    Length;
   BYTE    Decimal;
} DbfField_t;

static const int FIELD_DATA_T_LEN = 256;
typedef char field_data_t[FIELD_DATA_T_LEN];

#define MAX_FIELDS 128
#define DB_NO_ERROR          0
#define DB_MEM_ALLOC_ERROR  -1
#define DB_POSITION_ERROR   -2
#define DB_FILE_ERROR       -3
#define DB_FIELD_NAME_ERROR -4

typedef struct 
{
   int        NumRec;
   int      NumFields;
   int      Month;
   int      Day;
   int      Year;
   DbfField_t Field[MAX_FIELDS];
} DbfHeader_t;

#define   BlockSize   128
#define   BufSize     512

typedef struct 
{
   char   Name[11];
   char   Tipe;
   int    Offset;
   int    TopAdd;
   BYTE   Length;
   BYTE   Decimal;
} FieldDes_t;

typedef struct 
{
   BYTE   ID;
   BYTE   Date[3];
   int    Num;
   int    TopNum;
   int    HeadLen;
   int    RecLen;
   FieldDes_t  *Field;
} DbHead_t;

typedef struct 
{
   int    NumRec;
   int    HeadLen;
   int    RecLen;
   int    NumFields;
   int    Month;
   int    Day;
   int    Year;
} m_dbinfo_t;


/////////////////////////////////////////////////////////////////////////////
// CDbase view

class CDbase : public CObject
{
public:
   CDbase();  // protected constructor used by dynamic creation
   ~CDbase();
// Attributes
public:

// Operations
public:

// Implementation

public:


m_dbinfo_t m_DbInfo;
DbHead_t *m_DbHead;
FieldDes_t m_Field;

char *m_RecBuf;
char m_DbBuf[10000];
char m_FieldBuf[456];

char szString[128];   /* variable to load resource strings         */

char szAppName[20];   /* class name for the window               */
HWND hInst;
HWND hWndMain;
//OFSTRUCT of;
//int m_fh;
CFile *m_file;
int m_CurRec;
int m_MaxRec;
int m_CurFieldNum;
BOOL m_MemGood;
BOOL m_DbOpened;

private:
void ZeroFill(char* txt);


public:

// valid open types are OF_READ and OF_READWRITE 
int CDbase::OpenDbf (CString NameStr, int open_type);

// Return the index of the given field name
int GetFieldDataIndex(const char* fieldname);
int GetFieldDataByIndex(int field_index, char* Data);
int GetTrimmedStringByIndex(int field_index, char* buf, CString* str);
int GetIntByIndex(int field_index, char* buf, int* value);
int GetDoubleByIndex(int field_index, char* buf, double* value);

int  GetFieldData(const char * FieldName, char * Data);
int  FindFieldData(CString search_text, CList<CString*, CString*> &list);
int  CloseDbf();
int  GetRecordData(field_data_t *data);
int  GetRecordData(CList<CString*, CString*> &list);
int  GotoRec(int RecNum);
int  GotoNext();
int  GotoPrevious();
int  GetCurrentRecordNumber();
int  GetNumberOfRecords();
int  GetYear();
int  GetMonth();
int  GetDay();
int  GetNumFields();
BOOL IsDbfOpened();
BOOL GetFirstFieldName(char *Name, int Name_len, int *Len, int *Decimal);
BOOL GetNextFieldName(char *Name, int Name_len, int *Len, int *Decimal);
int  UpdateRecord(field_data_t *data);
int  CreateDbf(CString filename, DbfField_t *fld, int numfields);
int  AddRecord();
int  Delete();
int  UnDelete();
BOOL IsDeleted();
int  Pack();
int  AppendRecord(field_data_t *data);
int  EndAppend();

protected:
char * UpperCase(char * text);
char * trim(char * sline);
};


#endif

/////////////////////////////////////////////////////////////////////////////
