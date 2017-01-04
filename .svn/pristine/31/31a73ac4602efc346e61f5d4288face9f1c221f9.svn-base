// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.



// dbase.cpp : implementation file
//
// This class performs operations on dBase databases
// NOTE - Memo fields are not supported
//

#include "stdafx.h"
#include "dbase.h"
#include "err.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDbase


CDbase::CDbase()
{
   m_MemGood = FALSE;
   m_DbHead = (DbHead_t *) malloc(sizeof(DbHead_t));
   if (m_DbHead == NULL)
      return;
   m_DbHead->Field = NULL;
   m_RecBuf = (char *) malloc(65535L);
   if (m_RecBuf == NULL)
   {
      free(m_DbHead);
      return;
   }
   m_MemGood = TRUE;
   m_DbOpened = FALSE;
   m_file = NULL;
   m_CurRec = 0;
   m_MaxRec = 0;
}

CDbase::~CDbase()
{
   if (m_DbOpened)
      CloseDbf();
   if (m_RecBuf)
      free(m_RecBuf);
   if (m_DbHead)
      free(m_DbHead);
}



/////////////////////////////////////////////////////////////////////////////

/*************************************************************/

// valid open types are OF_READ and OF_READWRITE 
int CDbase::OpenDbf (CString NameStr, int open_type)
{
   int k;
   int tmp;
   short stmp;
   int offset;

   if (!m_MemGood)
      return DB_MEM_ALLOC_ERROR;

   if (m_file)
      CloseDbf();

   if (m_DbOpened)
      CloseDbf();

   m_file = new CFile;

   if ( !m_file->Open( NameStr, CFile::modeRead | CFile::shareDenyWrite) )
   {
      delete m_file;
      m_file = NULL;
      return DB_FILE_ERROR;
   }

   m_DbOpened = TRUE;
   m_CurFieldNum = 0;


   m_file->Read(m_DbBuf, 10000);

   if ((m_DbBuf[0] & 0x0f) != 3)
   {
      CloseDbf();
      return DB_FILE_ERROR;
   }

   m_DbHead->ID = m_DbBuf[0];
   m_DbInfo.Year = (int) m_DbBuf[1] + 1900;
   m_DbInfo.Month = (int) m_DbBuf[2];
   m_DbInfo.Day   = (int) m_DbBuf[3];
   memmove(&tmp, m_DbBuf+4, 4);
   m_DbInfo.NumRec = tmp;
   m_DbHead->Num = tmp;
   m_MaxRec = (int) tmp;
   memmove(&stmp, m_DbBuf+6, 2);
   m_DbHead->TopNum = stmp;
   memmove(&stmp, m_DbBuf+8, 2);
   m_DbInfo.HeadLen = stmp;
   m_DbHead->HeadLen = stmp;
   memmove(&stmp, m_DbBuf+10, 2);
   m_DbInfo.RecLen = stmp;
   m_DbHead->RecLen = stmp;

   m_DbInfo.NumFields = (int) ((m_DbInfo.HeadLen - 32) / 32);
   m_DbHead->Field = (FieldDes_t*) malloc(m_DbInfo.NumFields * sizeof(FieldDes_t));

   // memmove(m_DbHead->Field, m_DbBuf+32, sizeof(DbHead_t));
   offset = 1;
   for (k=0; k<m_DbInfo.NumFields; k++)
   {
      strncpy_s(m_DbHead->Field[k].Name, 11, m_DbBuf+32+(k*32), 10);
      m_DbHead->Field[k].Name[10] = '\0';
      m_DbHead->Field[k].Tipe = m_DbBuf[32+(k*32)+11];
      m_DbHead->Field[k].Length = m_DbBuf[32+(k*32)+16];
      m_DbHead->Field[k].Decimal = m_DbBuf[32+(k*32)+17];
      m_DbHead->Field[k].Offset = offset;
      offset += m_DbHead->Field[k].Length;
   }

   // don't trust the header, calculate the record length
   m_DbInfo.RecLen = offset;
   m_DbHead->RecLen = offset;
   m_DbOpened = TRUE;

   return DB_NO_ERROR;
}
// end of OpenDbf

/**********************************************************************/

char * CDbase::UpperCase(char * text)
{
   int len, k;

   len = strlen(text);

   for (k=0; k<len; k++)
      text[k] = toupper(text[k]);
   return text;
}
// end of UpperCase

/*************************************************************/

char * CDbase::trim(char * sline)
{
   int len;

   len = strlen(sline);
   while ((len > 0) && (sline[len-1] == ' '))
   {
      sline[len-1] = '\0';
      len--;
   }
   return sline;
}
// end of trim

/*********************************************************************/

int CDbase::GetFieldDataIndex(const char* fieldname)
{
   const int LEN = 11;
   char fname[LEN], dname[LEN];

   if (!m_MemGood)
      return DB_MEM_ALLOC_ERROR;

   if (!m_DbOpened)
      return   DB_FILE_ERROR;

   strncpy_s(fname, LEN, fieldname, sizeof(fname));
   trim( fname );
   UpperCase(fname);

   int k = 0;
   while (k < m_DbInfo.NumFields)
   {
      strncpy_s(dname, LEN, m_DbHead->Field[k].Name, 10);
      dname[10] = '\0';
      UpperCase(dname);
      trim(dname);
      if (strcmp(dname, fname) == 0)
         return k;
      ++k;
   }

   return DB_FIELD_NAME_ERROR;
}

int CDbase::GetFieldDataByIndex(int field_index, char* Data)
{
   if (m_CurRec == 0)
      return DB_POSITION_ERROR;

   int recseek = (int) m_DbHead->Field[field_index].Offset;
   int len = m_DbHead->Field[field_index].Length;
   if (len > 255)
      len = 255;
   memmove(Data, m_RecBuf+recseek, len);
   Data[len] = '\0';
   return DB_NO_ERROR;
}

int CDbase::GetTrimmedStringByIndex(int field_index, char* buf, CString* str)
{
   int result = GetFieldDataByIndex(field_index, buf);
   if (result != DB_NO_ERROR)
      return result;

   *str = buf;
   str->Trim();
   return DB_NO_ERROR;
}

int CDbase::GetIntByIndex(int field_index, char* buf, int* value)
{
   int result = GetFieldDataByIndex(field_index, buf);
   if (result != DB_NO_ERROR)
      return result;

   *value = atoi(buf);
   return DB_NO_ERROR;
}

int CDbase::GetDoubleByIndex(int field_index, char* buf, double* value)
{
   int result = GetFieldDataByIndex(field_index, buf);
   if (result != DB_NO_ERROR)
      return result;

   *value = atof(buf);
   return DB_NO_ERROR;
}

int CDbase::GetFieldData(const char* fieldname, char* Data)
{
   int field_index = GetFieldDataIndex(fieldname);
   if (field_index < 0)
      return field_index;  // return error code

   return GetFieldDataByIndex(field_index, Data);
} 

/**********************************************************************/

int CDbase::CloseDbf()
{
   if (m_file)
      delete m_file;

   m_file = NULL;

   if (m_DbHead->Field != NULL)
   {
      free(m_DbHead->Field);
      m_DbHead->Field = NULL;
   }

   m_DbOpened = FALSE;
   return DB_NO_ERROR;
}
// end of CloseDbf

/**********************************************************************/

BOOL CDbase::IsDbfOpened()
{
   return m_DbOpened && (m_DbInfo.NumRec > 0);
}
// end of IsDbfOpened

/**********************************************************************/

int CDbase::GotoRec(int RecNum)
{
   int recseek, rslt;

   if (RecNum > m_DbInfo.NumRec)
      return DB_POSITION_ERROR;

   if (RecNum < 1)
      return DB_POSITION_ERROR;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (RecNum-1);
   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(m_RecBuf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
      return DB_FILE_ERROR;

   m_CurRec = RecNum;

   return DB_NO_ERROR;
}
// end of GotoRec

/**********************************************************************/
/**********************************************************************/

int CDbase::GetRecordData(CList<CString*, CString*> &list)
{
   int recseek, rslt;
   char *buf;
   int k, off, len;
   CString *txt;
   const int TSTR_LEN = 256;
   char tstr[TSTR_LEN];

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   // build the data block
   buf = (char*) malloc(m_DbInfo.RecLen);

   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (int) (m_CurRec-1);
   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(buf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
      return DB_FILE_ERROR;

   off = 1;
   for (k=0; k<m_DbInfo.NumFields; k++)
   {
      len = m_DbHead->Field[k].Length;
      if (len > 254)
      {
         ERR_report("Invalid field length");
         return DB_FILE_ERROR;
      }

      ASSERT((off+len) <= m_DbInfo.RecLen);

      strncpy_s(tstr, TSTR_LEN, buf+off, len);
      tstr[len] = '\0';
      txt = new CString;
      *txt = m_DbHead->Field[k].Name;
      *txt += ": ";
      *txt += tstr;
      list.AddTail(txt);
      off += len;
   }

   free(buf);
   return DB_NO_ERROR;
}
// end of GetRecordData

/**********************************************************************/
/**********************************************************************/

int CDbase::FindFieldData(CString search_text, CList<CString*, CString*> &list)
{
   int recseek, rslt;
   char *buf;
   int k, off, len;
   CString *txt;
   const int TSTR_LEN = 256;
   char tstr[TSTR_LEN];
   const int NAME_LEN = 21;
   char name[NAME_LEN];
   CString tname;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   // build the data block
   buf = (char*) malloc(m_DbInfo.RecLen);

   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (int) (m_CurRec-1);

   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(buf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
      return DB_FILE_ERROR;

   off = 1;
   for (k=0; k<m_DbInfo.NumFields; k++)
   {
      strncpy_s(name, NAME_LEN, m_DbHead->Field[k].Name, 11);
      name[11] = '\0';
      tname = name;
      len = m_DbHead->Field[k].Length;
      if (len > 254)
      {
         ERR_report("Invalid field length");
         return DB_FILE_ERROR;
      }
      if (tname.Find(search_text) >= 0)
      {
         strncpy_s(tstr, TSTR_LEN, buf+off, len);
         tstr[len] = '\0';
         txt = new CString;
         *txt = tstr;
         list.AddTail(txt);
      }
      off += len;
   }

   free(buf);
   return DB_NO_ERROR;
}
// end of FindFieldData

/**********************************************************************/

int CDbase::UpdateRecord(field_data_t *data)
{
   int recseek, rslt;
   char *buf;
   int k, off, len;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   // build the data block
   // buf = (char*) malloc(m_DbInfo.RecLen + 1);
   const int BUF_LEN = 10000;
   buf = (char*) malloc(BUF_LEN);
   buf[0] = 32;
   off = 1;
   for (k=0; k<m_DbInfo.NumFields; k++)
   {
      len = m_DbHead->Field[k].Length;
      strncpy_s(buf+off, BUF_LEN-off, data[k], len);
      off += len;
   }

   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (int) (m_CurRec-1);

   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(buf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
   {
      free(buf);
      return DB_FILE_ERROR;
   }

   free(buf);
   if (rslt == HFILE_ERROR)
      return DB_FILE_ERROR;
   m_file->Flush();

   return DB_NO_ERROR;
}
// end of UpdateRecord

/**********************************************************************/
/**********************************************************************/

int CDbase::GetRecordData(field_data_t *data)
{
   int recseek, rslt;
   char *buf;
   int k, off, len;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   // build the data block
   buf = (char*) malloc(m_DbInfo.RecLen);

   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (int) (m_CurRec-1);

   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(buf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
   {
      free(buf);
      return DB_FILE_ERROR;
   }

   off = 1;
   for (k=0; k<m_DbInfo.NumFields; k++)
   {
      len = m_DbHead->Field[k].Length;
      strncpy_s(data[k], len+1, buf+off, len);
      data[k][len] = '\0';
      off += len;
   }

   free(buf);
   return DB_NO_ERROR;
}
// end of GetRecordData

/**********************************************************************/

int CDbase::GotoNext()
{
   int recseek, rslt;

   if (m_CurRec >= m_MaxRec)
      return DB_POSITION_ERROR;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   m_CurRec++;
   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (int) (m_CurRec - 1);

   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(m_RecBuf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
      return DB_FILE_ERROR;

   return DB_NO_ERROR;
}
// end of GotoNext

/**********************************************************************/

int CDbase::GotoPrevious()
{
   int recseek, rslt;

   if (m_CurRec < 2)
      return DB_POSITION_ERROR;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   m_CurRec--;
   recseek = (int) m_DbInfo.HeadLen;
   recseek += (int) m_DbInfo.RecLen * (int) (m_CurRec - 1);

   m_file->Seek(recseek, CFile::begin);
   rslt = m_file->Read(m_RecBuf, m_DbInfo.RecLen);
   if (rslt < m_DbInfo.RecLen)
      return DB_FILE_ERROR;

   return DB_NO_ERROR;
}
// end of GotoPrevious

/**********************************************************************/

int CDbase::GetNumberOfRecords()
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   return m_MaxRec;
}
// end of GetNumberOfRecords

/**********************************************************************/

int CDbase::GetCurrentRecordNumber()
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   return m_CurRec;
}
// end of GetCurrentRecordNumber

/**********************************************************************/

int CDbase::GetYear()
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   return m_DbInfo.Year;

}
// end of GetYear

/**********************************************************************/

int CDbase::GetMonth()
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   return m_DbInfo.Month;

}
// end of GetMonth

/**********************************************************************/

int CDbase::GetDay()
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   return m_DbInfo.Day;

}
// end of GetDay

/**********************************************************************/

int CDbase::GetNumFields()
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   return m_DbInfo.NumFields;

}
// end of GetNumFields

/**********************************************************************/

BOOL CDbase::GetFirstFieldName(char *Name, int Name_len, int *Len, int *Decimal)
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   m_CurFieldNum = 1;
   if (m_CurFieldNum > m_DbInfo.NumFields)
      return FALSE;

   strncpy_s(Name, Name_len, m_DbHead->Field[m_CurFieldNum - 1].Name, 10);
   Name[10] = '\0';
   *Len =  (int) m_DbHead->Field[m_CurFieldNum - 1].Length;
   *Decimal =  (int) m_DbHead->Field[m_CurFieldNum - 1].Decimal;
   return DB_NO_ERROR;
}
// end of GetFirstNameField

/**********************************************************************/

BOOL CDbase::GetNextFieldName(char *Name, int Name_len, int *Len, int *Decimal)
{
   if (!m_DbOpened)
      return DB_FILE_ERROR;

   if (m_CurFieldNum < 1)
      return DB_POSITION_ERROR;

   m_CurFieldNum++;
   if (m_CurFieldNum > m_DbInfo.NumFields)
      return DB_POSITION_ERROR;

   strcpy_s(Name, Name_len, m_DbHead->Field[m_CurFieldNum - 1].Name);
   *Len =  (int) m_DbHead->Field[m_CurFieldNum - 1].Length;
   *Decimal =  (int) m_DbHead->Field[m_CurFieldNum - 1].Decimal;
   return DB_NO_ERROR;
}
// end of GetNextFieldName

/**********************************************************************/

void CDbase::ZeroFill(char* txt)
{
   int k, notdone;

   k = strlen(txt) - 1;
   if (k < 0)
      return;

   notdone = TRUE;
   while (notdone)
   {
      if (isalpha(txt[k]))
      {
         notdone = FALSE;
         continue;
      }
      else
         txt[k] = '\0';
      k--;
      if (k<0)
         notdone = FALSE;
   }
}
// end of ZeroFill

/**********************************************************************/

// create a new dbase database with fld fields

int CDbase::CreateDbf(CString filename, DbfField_t *fld, int numfields)
{
   const int TSTR_LEN = 3;
   char tstr[TSTR_LEN];
   int day, month, year;
   int j, k, headlen;
   int cnt;
   int zero;
   BYTE ch;
   const int TIMESTR_LEN = 21;
   char timestr[TIMESTR_LEN];

   _strdate_s(timestr, TIMESTR_LEN);
   strncpy_s(tstr, TSTR_LEN, timestr, 2);
   tstr[2] = '\0';
   month = atoi(tstr);
   strncpy_s(tstr, TSTR_LEN, timestr+3, 2);
   tstr[2] = '\0';
   day = atoi(tstr);
   strncpy_s(tstr, TSTR_LEN, timestr+6, 2);
   tstr[2] = '\0';
   year = atoi(tstr);
   year += 100;  // the year is referenced to 1900

   // year = time.GetYear()  % 1900;
   // month = time.GetMonth();
   // day = time.GetDay();

   if (m_file)
      CloseDbf();

   m_file = new CFile;

   if (!m_file->Open(filename, CFile::modeCreate | CFile::modeWrite))
      return DB_FILE_ERROR;

   m_DbHead->ID = 3;
   m_DbHead->Date[0] = year;
   m_DbHead->Date[1] = month;
   m_DbHead->Date[2] = day;
   m_DbHead->Num = 0;
   m_DbHead->TopNum = 0;
   m_DbHead->Field = (FieldDes_t*) malloc(numfields * sizeof(FieldDes_t));

   cnt = 0;
   for (k=0; k<numfields; k++)
      cnt += fld[k].Length;
   m_DbHead->RecLen = cnt + 1;

   // write the ID byte
   ch = 3;
   m_file->Write((char*) &ch, 1);
   // write the date
   ch = (BYTE) year;
   m_file->Write((char*) &ch, 1);
   ch = (BYTE) month;
   m_file->Write((char*) &ch, 1);
   ch = (BYTE) day;
   m_file->Write((char*) &ch, 1);

   // write number of records, 0
   cnt = 0;
   m_file->Write((char*) &cnt, 2);
   m_file->Write((char*) &cnt, 2);

   // write length of header structure
   cnt = 32 + (numfields * 32) + 1;
   m_file->Write((char*) &cnt, 2);
   headlen = cnt;

   // write the record length
   m_file->Write((char*) &(m_DbHead->RecLen), 2);

   // write filler
   ch = 0;
   for (k=0; k<17; k++)
      m_file->Write((char*) &ch, 1);

   // write language code
   ch = 0x57;
   m_file->Write((char*) &ch, 1);

   // write filler
   ch = 0;
   m_file->Write((char*) &ch, 1);
   m_file->Write((char*) &ch, 1);

   // write the field descriptions
   zero = 0;
   cnt = 0;
   for (k=0; k<numfields; k++)
   {
      //  ZeroFill(fld[k].Name);
      m_file->Write(fld[k].Name, 11);
      m_file->Write((char*) &(fld[k].Tipe), 1);
      m_file->Write((char*) &cnt, 2);
      m_file->Write((char*) &zero, 2);
      m_file->Write((char*) &(fld[k].Length), 1);
      m_file->Write((char*) &(fld[k].Decimal), 1);
      for (j=0; j<14; j++)
         m_file->Write((char*) &zero, 1);
      strncpy_s(m_DbHead->Field[k].Name, 11, fld[k].Name, 11);
      m_DbHead->Field[k].Tipe = fld[k].Tipe;
      m_DbHead->Field[k].Offset = cnt;
      m_DbHead->Field[k].TopAdd = 0;
      m_DbHead->Field[k].Length = fld[k].Length;
      m_DbHead->Field[k].Decimal = fld[k].Decimal;

      cnt += fld[k].Length;
   }

   // write the terminator
   ch = 0x0d;
   m_file->Write((char*) &ch, 1);

   m_DbInfo.Year = (int) year;
   m_DbInfo.Month = (int) month;
   m_DbInfo.Day   = (int) day;
   m_DbInfo.NumRec = 0;
   m_MaxRec = 0;
   //   memmove(&tmp, m_DbBuf+6, 2);
   m_DbInfo.HeadLen = headlen;
   m_DbInfo.RecLen = m_DbHead->RecLen;

   m_DbInfo.NumFields = (int) ((m_DbInfo.HeadLen - 32) / 32);
   m_DbOpened = TRUE;
   return DB_NO_ERROR;
}
// end of CreateDbf

/**********************************************************************/

// add a new record to the bottom of the database

int CDbase::AddRecord()
{
   BYTE ch;
   int k;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   // write record
   m_file->Seek(0, CFile::end);
   ch = 32;
   for (k=0; k<=m_DbHead->RecLen; k++)
      m_file->Write((char*) &ch, 1);

   // update header
   m_file->Seek(4, CFile::begin);
   m_file->Write((char*) &k, 4);
   k++;
   m_file->Seek(4, CFile::begin);
   m_file->Write((char*) &k, 4);

   m_DbInfo.NumRec = k;
   m_MaxRec = k;

   // goto the new record
   GotoRec(k);
   return DB_NO_ERROR;
}
// end of AddRecord

/**********************************************************************/
/**********************************************************************/

// add a new record to the bottom of the database

int CDbase::AppendRecord(field_data_t *data)
{
   char *buf;
   int k, off, len;

   if (!m_DbOpened)
      return DB_FILE_ERROR;

   // write record
   m_file->Seek(0, CFile::end);

   // build the data block
   // buf = (char*) malloc(m_DbInfo.RecLen + 1);
   const int BUF_LEN = 10000;
   buf = (char*) malloc(BUF_LEN);
   buf[0] = 32;
   off = 1;
   for (k=0; k<m_DbInfo.NumFields; k++)
   {
      len = m_DbHead->Field[k].Length;
      strncpy_s(buf+off, BUF_LEN-off, data[k], len);
      off += len;
   }

   m_file->Write(buf, m_DbInfo.RecLen);
   free(buf);

   return DB_NO_ERROR;
}
// end of AppendRecord

/**********************************************************************/

// tidy up after appending records

int CDbase::EndAppend()
{
   ULONGLONG size;

   // return if invalid reclen
   if (m_DbInfo.RecLen == 0)
      return DB_NO_ERROR;

   m_file->Flush();

   // get the size of the file
   CFileStatus status;
   if ( !m_file->GetStatus( status ) )  
      return DB_FILE_ERROR;
   size = status.m_size;
   size -=  m_DbInfo.HeadLen;
   size = size / m_DbInfo.RecLen;

   // update header
   m_file->Seek(4, CFile::begin);
   m_file->Write((char*) &size, 4);

   m_DbInfo.NumRec = (int) size;
   m_MaxRec = (int) size;

   return DB_NO_ERROR;
}
// end of EndAppend

/**********************************************************************/

int CDbase::Delete()
{
   return DB_NO_ERROR;
}
// end of Delete
/**********************************************************************/

int CDbase::UnDelete()
{
   return DB_NO_ERROR;
}
// end of UnDelete

/**********************************************************************/

BOOL CDbase::IsDeleted()
{
   return DB_NO_ERROR;
}
// end of IsDeleted

/**********************************************************************/

int CDbase::Pack()
{
   return DB_NO_ERROR;
}
// end of Pack

/**********************************************************************/



/**********************************************************************/



/**********************************************************************/


/////////////////////////////////////////////////////////////////////////////
// CDbase message handlers

