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

// InfoZip.h: interface for the CInfoZip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFOZIP_H__A5C325B4_180B_11D3_ABD5_00A0C9B4D8AB__INCLUDED_)
#define AFX_INFOZIP_H__A5C325B4_180B_11D3_ABD5_00A0C9B4D8AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// The following symbol UZ_DCL_STRUCTVER must be incremented whenever an
// incompatible change is applied to the WinDLL API structure "DCL" !
#define UZ_DCL_STRUCTVER        0x600

#ifndef Z_UINT8_DEFINED
# if defined(__GNUC__)
   typedef unsigned long long    z_uint8;
#  define Z_UINT8_DEFINED
# elif (defined(_MSC_VER) && (_MSC_VER >= 1100))
   typedef unsigned __int64      z_uint8;
   #define Z_UINT8_DEFINED
   #define ZIP64_SUPPORT
# elif (defined(__WATCOMC__) && (__WATCOMC__ >= 1100))
   typedef unsigned __int64      z_uint8;
#  define Z_UINT8_DEFINED
# elif (defined(__IBMC__) && (__IBMC__ >= 350))
   typedef unsigned __int64      z_uint8;
#  define Z_UINT8_DEFINED
# elif (defined(__BORLANDC__) && (__BORLANDC__ >= 0x0500))
   typedef unsigned __int64      z_uint8;
#  define Z_UINT8_DEFINED
# elif (defined(__LCC__))
   typedef unsigned __int64      z_uint8;
#  define Z_UINT8_DEFINED
# endif
#endif

//-----------------------------------------------------------------------------
// unzip_buffer_t - buffer returned from call to UzpUnzipToMemory 
typedef struct
{
   // length of string
   unsigned long strlength; 

   // pointer to string
   char * strptr;

} unzip_buffer_t;

//-----------------------------------------------------------------------------
// callback function defs
typedef int (WINAPI DLLCOMMENT) (LPSTR);

typedef int (WINAPI DLLPRNT) (LPSTR, unsigned long);
typedef int (WINAPI DLLPASSWORD) (LPSTR pwbuf, int bufsiz,
   LPCSTR promptmsg, LPCSTR entryname);
# ifdef Z_UINT8_DEFINED
typedef int (WINAPI DLLSERVICE) (LPCSTR entryname, z_uint8 uncomprsiz);
typedef int (WINAPI DLLSERVICE_NO_INT64) (LPCSTR, unsigned long, unsigned long);
# else
typedef int (WINAPI DLLSERVICE) (LPCSTR entryname, unsigned long uncomprsiz);
# endif
typedef int (WINAPI DLLSERVICE_I32) (LPCSTR entryname,
   unsigned long ucsz_lo, unsigned long ucsz_hi);

typedef void (WINAPI DLLSND) (void);
typedef int (WINAPI DLLREPLACE) (LPSTR efnam, unsigned efbufsiz);
#ifdef Z_UINT8_DEFINED
typedef void (WINAPI DLLMESSAGE) (z_uint8 ucsize, z_uint8 csize,
   unsigned cfactor,
   unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
   char c, LPCSTR filename, LPCSTR methbuf, unsigned long crc, char fCrypt);
#else
typedef void (WINAPI DLLMESSAGE) (unsigned long ucsize, unsigned long csize,
   unsigned cfactor,
   unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
   char c, LPCSTR filename, LPCSTR methbuf, unsigned long crc, char fCrypt);
#endif
typedef void (WINAPI DLLMESSAGE_I32) (unsigned long ucsiz_l,
   unsigned long ucsiz_h, unsigned long csiz_l, unsigned long csiz_h,
   unsigned cfactor,
   unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
   char c, LPCSTR filename, LPCSTR methbuf, unsigned long crc, char fCrypt);

typedef int (WINAPI DLLSPLIT) (LPSTR);

//-----------------------------------------------------------------------------
// USERFUNCTIONS - used to specify callbacks for unzip functions

typedef struct {
   DLLPRNT *print;
   DLLSND *sound;
   DLLREPLACE *replace;
   DLLPASSWORD *password;
   DLLMESSAGE *SendApplicationMessage;
   DLLSERVICE *ServCallBk;
   DLLMESSAGE_I32 *SendApplicationMessage_i32;
   DLLSERVICE_I32 *ServCallBk_i32;
#ifdef Z_UINT8_DEFINED
   z_uint8 TotalSizeComp;
   z_uint8 TotalSize;
   z_uint8 NumMembers;
#else
   struct _TotalSizeComp {
      unsigned long u4Lo;
      unsigned long u4Hi;
   } TotalSizeComp;
   struct _TotalSize {
      unsigned long u4Lo;
      unsigned long u4Hi;
   } TotalSize;
   struct _NumMembers {
      unsigned long u4Lo;
      unsigned long u4Hi;
   } NumMembers;
#endif
   unsigned CompFactor;
   WORD cchComment;
} USERFUNCTIONS, far * LPUSERFUNCTIONS;

//-----------------------------------------------------------------------------
// ZIPUSERFUNCTIONS - used to specify callbacks for zip functions
typedef struct {
   DLLPRNT *print;
   DLLCOMMENT *comment;
   DLLPASSWORD *password;
   DLLSPLIT *split;      /* This MUST be set to NULL unless you want to be queried
                         for a destination for each split archive. */
#ifdef ZIP64_SUPPORT
   DLLSERVICE *ServiceApplication64;
   DLLSERVICE_NO_INT64 *ServiceApplication64_No_Int64;
#else
   DLLSERVICE *ServiceApplication;
#endif
} ZIPUSERFUNCTIONS, far * LPZIPUSERFUNCTIONS;


//-----------------------------------------------------------------------------
// ZCL struct - passed into the zip call
typedef struct {
   int  argc;            /* Count of files to zip */
   LPSTR lpszZipFN;      /* name of archive to create/update */
   char **FNV;           /* array of file names to zip up */
   LPSTR lpszAltFNL;     /* pointer to a string containing a list of file
                         names to zip up, separated by whitespace. Intended
                         for use only by VB users, all others should set this
                         to NULL. */
} ZCL, far *LPZCL;
//-----------------------------------------------------------------------------
// ZPOPT struct - passed into setoptions call before a zip
typedef struct {        /* zip options */
   LPSTR Date;             /* Date to include after */
   LPSTR szRootDir;        /* Directory to use as base for zipping */
   LPSTR szTempDir;        /* Temporary directory used during zipping */
   BOOL fTemp;             /* Use temporary directory '-b' during zipping */
   BOOL fSuffix;           /* include suffixes (not implemented) */
   BOOL fEncrypt;          /* encrypt files */
   BOOL fSystem;           /* include system and hidden files */
   BOOL fVolume;           /* Include volume label */
   BOOL fExtra;            /* Exclude extra attributes */
   BOOL fNoDirEntries;     /* Do not add directory entries */
   BOOL fExcludeDate;      /* Exclude files newer than specified date */
   BOOL fIncludeDate;      /* Include only files newer than specified date */
   BOOL fVerbose;          /* Mention oddities in zip file structure */
   BOOL fQuiet;            /* Quiet operation */
   BOOL fCRLF_LF;          /* Translate CR/LF to LF */
   BOOL fLF_CRLF;          /* Translate LF to CR/LF */
   BOOL fJunkDir;          /* Junk directory names */
   BOOL fGrow;             /* Allow appending to a zip file */
   BOOL fForce;            /* Make entries using DOS names (k for Katz) */
   BOOL fMove;             /* Delete files added or updated in zip file */
   BOOL fDeleteEntries;    /* Delete files from zip file */
   BOOL fUpdate;           /* Update zip file--overwrite only if newer */
   BOOL fFreshen;          /* Freshen zip file--overwrite only */
   BOOL fJunkSFX;          /* Junk SFX prefix */
   BOOL fLatestTime;       /* Set zip file time to time of latest file in it */
   BOOL fComment;          /* Put comment in zip file */
   BOOL fOffsets;          /* Update archive offsets for SFX files */
   BOOL fPrivilege;        /* Use privileges (WIN32 only) */
   BOOL fEncryption;       /* TRUE if encryption supported, else FALSE.
                           this is a read only flag */
   LPSTR szSplitSize;      /* This string contains the size that you want to
                           split the archive into. i.e. 100 for 100 bytes,
                           2K for 2 k bytes, where K is 1024, m for meg
                           and g for gig. If this string is not NULL it
                           will automatically be assumed that you wish to
                           split an archive. */
   LPSTR szIncludeList;    /* Pointer to include file list string (for VB) */
   long IncludeListCount;  /* Count of file names in the include list array */
   char **IncludeList;     /* Pointer to include file list array. Note that the last
                           entry in the array must be NULL */
   LPSTR szExcludeList;    /* Pointer to exclude file list (for VB) */
   long ExcludeListCount;  /* Count of file names in the include list array */
   char **ExcludeList;     /* Pointer to exclude file list array. Note that the last
                           entry in the array must be NULL */
   int  fRecurse;          /* Recurse into subdirectories. 1 => -r, 2 => -R */
   int  fRepair;           /* Repair archive. 1 => -F, 2 => -FF */
   char fLevel;            /* Compression level (0 - 9) */
} ZPOPT, far *LPZPOPT;

typedef struct {
  unsigned StructVersID;  /* struct version id (= UZ_DCL_STRUCTVER) */
  int ExtractOnlyNewer;   /* TRUE for "update" without interaction
                             (extract only newer/new files, without queries) */
  int SpaceToUnderscore;  /* TRUE if convert space to underscore */
  int PromptToOverwrite;  /* TRUE if prompt to overwrite is wanted */
  int fQuiet;             /* quiet flag:
                             { 0 = all | 1 = few | 2 = no } messages */
  int ncflag;             /* write to stdout if TRUE */
  int ntflag;             /* test zip file */
  int nvflag;             /* verbose listing */
  int nfflag;             /* "freshen" (replace existing files by newer versions) */
  int nzflag;             /* display zip file comment */
  int ndflag;             /* controls (sub)dir recreation during extraction
                             0 = junk paths from filenames
                             1 = "safe" usage of paths in filenames (skip ../)
                             2 = allow unsafe path components (dir traversal)
                           */
  int noflag;             /* always overwriting existing files if TRUE */
  int naflag;             /* do end-of-line translation */
  int nZIflag;            /* get ZipInfo output if TRUE */
  int B_flag;             /* backup existing files if TRUE */
  int C_flag;             /* be case insensitive if TRUE */
  int D_flag;             /* controls restoration of timestamps
                             0 = restore all timestamps (default)
                             1 = skip restoration of timestamps for folders
                                 created on behalf of directory entries in the
                                 Zip archive
                             2 = no restoration of timestamps; extracted files
                                 and dirs get stamped with current time */
  int U_flag;             /* controls UTF-8 filename coding support
                             0 = automatic UTF-8 translation enabled (default)
                             1 = recognize UTF-8 coded names, but all non-ASCII
                                 characters are "escaped" into "#Uxxxx"
                             2 = UTF-8 support is disabled, filename handling
                                 works exactly as in previous UnZip versions */
  int fPrivilege;         /* 1 => restore ACLs in user mode,
                             2 => try to use privileges for restoring ACLs */
  LPSTR lpszZipFN;        /* zip file name */
  LPSTR lpszExtractDir;   /* directory to extract to. This should be NULL if
                             you are extracting to the current directory. */
} DCL, far * LPDCL;

//-----------------------------------------------------------------------------
// function defs
typedef int (WINAPI * _DLL_UNZIP_TO_MEM)(LPSTR, LPSTR, LPUSERFUNCTIONS, unzip_buffer_t *);
typedef int (WINAPI * _DLL_UNZIP)(int, char **, int, char **, LPDCL, LPUSERFUNCTIONS);
typedef int (WINAPI * _DLL_ZIP)(ZCL, LPZPOPT);
typedef int (WINAPI * _ZIP_USER_FUNCTIONS)(LPZIPUSERFUNCTIONS);
//typedef BOOL (WINAPI * ZIPSETOPTIONS)(LPZPOPT);

class CInfoZip
{
   _DLL_UNZIP_TO_MEM m_unzip_to_memory;
   _DLL_UNZIP m_unzip_single_entry;
   _DLL_ZIP m_zip_archive;
   _ZIP_USER_FUNCTIONS m_zip_init;
   // ZIPSETOPTIONS m_zip_set_options;


public:
   int Initialize();

   // extract given file from the given archive into a memory buffer.  The
   // buffer needs to be freed by the caller
   int ExtractToMemory(const char *archive_name, const char *file_name,
      unzip_buffer_t *buffer);

   int ExtractFile(const char* archive_name, const char* target_path,
      const char* file_name);

   // update the archive with the given file - if the file does not 
   // exist, then it will be added to the archive
   int Zip(const char* archive_name, const char *file_name,
      BOOL update, BOOL delete_files);
};

#endif // !defined(AFX_INFOZIP_H__A5C325B4_180B_11D3_ABD5_00A0C9B4D8AB__INCLUDED_)
