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

// InfoZip.cpp: implementation of the CInfoZip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InfoZip.h"
#include "err.h"

#define UNZ_DLL_NAME "unz32dll.DLL"
#define ZIP_DLL_NAME "zip32.DLL"

#define ZE_EOF          2       /* unexpected end of zip file */
#define ZE_FORM         3       /* zip file structure error */
#define ZE_MEM          4       /* out of memory */
#define ZE_LOGIC        5       /* internal logic error */
#define ZE_BIG          6       /* entry too large to split */
#define ZE_NOTE         7       /* invalid comment format */
#define ZE_TEST         8       /* zip test (-T) failed or out of memory */
#define ZE_ABORT        9       /* user interrupt or termination */
#define ZE_TEMP         10      /* error using a temp file */
#define ZE_READ         11      /* read or seek error */
#define ZE_NONE         12      /* nothing to do */
#define ZE_NAME         13      /* missing or empty zip file */
#define ZE_WRITE        14      /* error writing to a file */
#define ZE_CREAT        15      /* couldn't open to write */
#define ZE_PARMS        16      /* bad command line */
#define ZE_OPEN         18      /* could not open a specified file to read */

int CInfoZip::Initialize()
{
   // load the DLL
   HMODULE hUnzipDll = LoadLibrary(UNZ_DLL_NAME);
   HMODULE hZipDll = LoadLibrary(ZIP_DLL_NAME);
   if (hUnzipDll == NULL || hZipDll == NULL)
   {
      ERR_report("Unable to load UNZIP32.DLL and/or ZIP32.DLL");
      return FAILURE;
   }

   m_unzip_to_memory = (_DLL_UNZIP_TO_MEM)GetProcAddress(hUnzipDll, 
      "Wiz_UnzipToMemory");

   m_unzip_single_entry = (_DLL_UNZIP)GetProcAddress(hUnzipDll, "Wiz_SingleEntryUnzip");

   m_zip_archive = (_DLL_ZIP)GetProcAddress(hZipDll, "ZpArchive");
   //   m_zip_set_options = (ZIPSETOPTIONS)GetProcAddress(hZipDll, "ZpSetOptions");
   m_zip_init = (_ZIP_USER_FUNCTIONS)GetProcAddress(hZipDll, "ZpInit");

   return SUCCESS;
}

int WINAPI OnPrint(LPSTR s, unsigned long ul) { /*TRACE("[OnPrint] %s %d\n", s, ul);*/ return 1; }
int WINAPI OnPassword(LPSTR, int, LPCSTR, LPCSTR) { return 1; }
int WINAPI OnService(LPCSTR entryname, z_uint8 uncomprsiz) { return 0; }
void WINAPI OnSound(void) { }
int WINAPI OnReplace(LPSTR, unsigned efbufsiz) { return 1; }
void WINAPI OnMessage(
   z_uint8 ucsize, z_uint8 csize,
   unsigned cfactor,
   unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
   char c, LPCSTR filename, LPCSTR methbuf, unsigned long crc, char fCrypt) { }
int WINAPI OnComment(LPSTR) { return 1; }
void WINAPI OnMessageI32(unsigned long ucsiz_l,
   unsigned long ucsiz_h, unsigned long csiz_l, unsigned long csiz_h,
   unsigned cfactor,
   unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
   char c, LPCSTR filename, LPCSTR methbuf, unsigned long crc, char fCrypt) { }
int WINAPI OnServiceI32(LPCSTR entryname,
   unsigned long ucsz_lo, unsigned long ucsz_hi) { return 0; }
int WINAPI OnServiceNoInt64(LPCSTR, unsigned long, unsigned long) { return 0; }

// extract given file from the given archive into a memory buffer.  The
// buffer needs to be freed by the caller
int CInfoZip::ExtractToMemory(const char *archive_name, const char *file_name,
   unzip_buffer_t *buffer)
{
   USERFUNCTIONS callbacks;
   memset(&callbacks, 0, sizeof(USERFUNCTIONS));
   callbacks.print = OnPrint;
   callbacks.sound = OnSound;
   callbacks.replace = OnReplace;
   callbacks.password = OnPassword;
   callbacks.SendApplicationMessage = OnMessage;
   callbacks.ServCallBk = OnService;
   callbacks.SendApplicationMessage_i32 = OnMessageI32;
   callbacks.ServCallBk_i32 = OnServiceI32;

   int ret = m_unzip_to_memory((char *)archive_name, (char *)file_name, &callbacks, buffer);

   return (ret == 1) ? SUCCESS : FAILURE;
}

// update the archive with the given file - if the file does not 
// exist, then it will be added to the archive
int CInfoZip::Zip(const char* archive_name, const char *file_name,
   BOOL update, BOOL delete_files)
{
   CString dir(file_name);
   dir = dir.Left(dir.ReverseFind('\\'));

   // if the last character is a colon (e.g, "D:") then we will
   // append a backslash
   if (dir.GetLength() && dir[dir.GetLength() - 1] == ':')
      dir += '\\';

   ZIPUSERFUNCTIONS callbacks;
   memset(&callbacks, 0, sizeof(ZIPUSERFUNCTIONS));
   callbacks.print = OnPrint;
   callbacks.password = OnPassword;
   callbacks.comment = OnComment;
   callbacks.ServiceApplication64 = OnService;
   callbacks.ServiceApplication64_No_Int64 = OnServiceNoInt64;

   m_zip_init(&callbacks);

   ZPOPT ZpOpt;
   ZpOpt.Date = NULL;            /* Not using, set to NULL pointer */
   if (dir.GetLength() != 0)
      ZpOpt.szRootDir = dir.GetBuffer(dir.GetLength()+1); /* Set directory */
   else
      ZpOpt.szRootDir = "C:\\";

   ZpOpt.szTempDir = "C:\\";     // we are not writing to a temp dir (see fTemp param), but somewhere
   // in the InfoZip code they are reading this value so it needs to be set
   ZpOpt.fTemp = 0;
   ZpOpt.fSuffix = FALSE;        /* include suffixes (not yet implemented) */
   ZpOpt.fEncrypt = FALSE;       /* true if encryption wanted */
   ZpOpt.fSystem = FALSE;        /* true to include system/hidden files */
   ZpOpt.fVolume = FALSE;        /* true if storing volume label */
   ZpOpt.fExtra = FALSE;         /* true if including extra attributes */
   ZpOpt.fNoDirEntries = FALSE;  /* true if ignoring directory entries */
   ZpOpt.fExcludeDate = FALSE;
   ZpOpt.fIncludeDate = FALSE;
   ZpOpt.fVerbose = FALSE;       /* true if full messages wanted */
   ZpOpt.fQuiet = FALSE;         /* true if minimum messages wanted */
   ZpOpt.fCRLF_LF = FALSE;       /* true if translate CR/LF to LF */
   ZpOpt.fLF_CRLF = FALSE;       /* true if translate LF to CR/LF */
   ZpOpt.fJunkDir = FALSE;       /* true if junking directory names */
   ZpOpt.fGrow = FALSE;          /* true if allow appending to zip file */
   ZpOpt.fForce = FALSE;         /* true if making entries using DOS names */
   ZpOpt.fMove = FALSE;          /* true if deleting files added or updated */
   ZpOpt.fDeleteEntries = delete_files; /* true if deleting files from archive */
   ZpOpt.fUpdate = update;        /* true if updating zip file--overwrite only
                                  if newer */
   ZpOpt.fFreshen = FALSE;       /* true if freshening zip file--overwrite only */
   ZpOpt.fJunkSFX = FALSE;       /* true if junking sfx prefix*/
   ZpOpt.fLatestTime = FALSE;    /* true if setting zip file time to time of
                                 latest file in archive */
   ZpOpt.fComment = FALSE;       /* true if putting comment in zip file */
   ZpOpt.fOffsets = FALSE;       /* true if updating archive offsets for sfx
                                 files */
   ZpOpt.fPrivilege = 0;
   ZpOpt.fEncryption = FALSE;
   ZpOpt.szSplitSize = 0;
   ZpOpt.szIncludeList = 0;
   ZpOpt.IncludeListCount = 0;
   ZpOpt.IncludeList = 0;
   ZpOpt.szExcludeList = 0;
   ZpOpt.ExcludeListCount = 0;
   ZpOpt.ExcludeList = 0;

   ZpOpt.fRecurse = 0;           /* subdir recursing mode: 1 = "-r", 2 = "-R" */
   ZpOpt.fRepair = 0;            /* archive repair mode: 1 = "-F", 2 = "-FF" */
   ZpOpt.fLevel = '9';


   ZCL ZpZCL;
   ZpZCL.argc = 1;
   ZpZCL.lpszZipFN = (char *)archive_name;

   // setup the list of filenames to be updated (in this case only one)
   char **list;
   list = (char **)malloc(sizeof(char **) * 1);
   list[0] = (char *)file_name;
   ZpZCL.FNV = list;

   // set the options
   // m_zip_set_options(&ZpOpt);

   int retcode = m_zip_archive(ZpZCL, &ZpOpt);

   // if failed, write error to log
   switch (retcode)
   {
   case ZE_EOF: ERR_report("Unexpected end of zip file"); break;
   case ZE_FORM: ERR_report("Zip file structure error"); break;
   case ZE_MEM: ERR_report("Out of memory"); break;
   case ZE_LOGIC: ERR_report("Internal logic error"); break;
   case ZE_BIG: ERR_report("Entry too large to split"); break;
   case ZE_NOTE: ERR_report("Invalid comment format"); break;
   case ZE_TEST: ERR_report("Zip test failed or out of memory"); break;
   case ZE_ABORT: ERR_report("User interrupt or temination"); break;
   case ZE_TEMP: ERR_report("Error using a temp file"); break;
   case ZE_READ: ERR_report("Read or seek error"); break;
   case ZE_NONE: ERR_report("Nothing to do"); break;
   case ZE_NAME: ERR_report("Missing or empty zip file"); break;
   case ZE_WRITE: ERR_report("Error writing to a file"); break;
   case ZE_CREAT: ERR_report("Couldn't open to write"); break;
   case ZE_PARMS: ERR_report("Bad command line"); break;
   case ZE_OPEN: ERR_report("Could not open a aspecifed file to read"); break;
   }

   free(list);

   return (retcode == 0) ? SUCCESS : FAILURE;
}

int CInfoZip::ExtractFile(const char* archive_name, const char* target_path,
   const char* file_name)
{
   USERFUNCTIONS callbacks;
   memset(&callbacks, 0, sizeof(USERFUNCTIONS));
   callbacks.print = OnPrint;
   callbacks.sound = OnSound;
   callbacks.replace = OnReplace;
   callbacks.password = OnPassword;
   callbacks.SendApplicationMessage = OnMessage;
   callbacks.ServCallBk = OnService;
   callbacks.SendApplicationMessage_i32 = OnMessageI32;
   callbacks.ServCallBk_i32 = OnServiceI32;

   DCL dcl;

   dcl.StructVersID = UZ_DCL_STRUCTVER;

   dcl.ncflag = 0; /* Write to stdout if true */
   dcl.fQuiet = 2; /* 0 = We want all messages.
                   1 = fewer messages,
                   2 = no messages */
   dcl.ntflag = 0; /* test zip file if true */
   dcl.nvflag = 0; /* give a verbose listing if true */
   dcl.nzflag = 0; /* display a zip file comment if true */
   dcl.ndflag = 0; /* Recreate directories if true */
   dcl.naflag = 0; /* Do not convert CR to CRLF */
   dcl.nfflag = 0; /* Do not freshen existing files only */
   dcl.noflag = 1; /* Over-write all files if true */
   dcl.ExtractOnlyNewer = 0; /* Do not extract only newer */
   dcl.PromptToOverwrite = 0; /* "Overwrite all" selected -> no query mode */
   dcl.lpszZipFN = (char *)archive_name;
   dcl.lpszExtractDir = (char *)target_path;
   dcl.SpaceToUnderscore = 0;
   dcl.nZIflag = 0;
   dcl.B_flag = 0;
   dcl.C_flag = 1;
   dcl.D_flag = 0;
   dcl.U_flag = 0;
   dcl.fPrivilege = 0;

   // setup the list of filenames to be extracted (in this case only one)
   char **list;
   list = (char **)malloc(sizeof(char **) * 1);
   list[0] = (char *)file_name;

   int retcode = m_unzip_single_entry(1, list, 0, NULL, &dcl, &callbacks);

   free(list);

   return (retcode == 0) ? SUCCESS : FAILURE;
}

