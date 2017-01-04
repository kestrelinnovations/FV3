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

// prntfile.h

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_PRNTFILE_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_PRNTFILE_H_ 1

#define PAGE_LAYOUT_FILE_FVW_VERSION_STRING_311 "FalconView 3.20"
#define PAGE_LAYOUT_FILE_STRING "Page Layout File"
#define PAGE_LAYOUT_FILE_REVISION_311 7

class PageLayoutFileVersion
{
private:
   CString m_app_version;
   int m_revision;

public:
   // Constructor
   PageLayoutFileVersion();

   // Destructor
   ~PageLayoutFileVersion()
   {
   }

   // Reads in file version information from a print file.  Returns true
   // if given CArchive had the latest revision for it's application version.
   // Otherwise false is returned.
   bool Read(CArchive *ar);

   // Returns the application version string read from the file.  The string
   // will be empty if the Read function was unable to read a valid string
   // from the CArchive.
   CString &get_application_version()
   {
      return m_app_version;
   }

   // Returns the revision number read from the file.  A value of -1 will be
   // returned if the Read function was unable to read a valid value.
   int get_revision()
   {
      return m_revision;
   }

   // Displays a message box describing what the problem is with the print file
   void ReportFileVersionProblem();

   // Writes file version information for the latest print file format, i.e.,
   // the format written by this version of FalconView.
   static void Write(CArchive *ar);

private:
   // Sets member variables to initial state.
   void reset();

   // Returns TRUE if valid application version and revision data was read.
   bool valid_data();

   // Returns TRUE if the CArchive contained the last revision.
   bool last_revision();
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_PRNTFILE_H_
