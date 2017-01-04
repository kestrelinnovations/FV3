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

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/prntfile.h"

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/refresh.h"    // FVW_display_error_string

// this project's headers

PageLayoutFileVersion::PageLayoutFileVersion()
{
   reset();
}

bool PageLayoutFileVersion::Read(CArchive *ar)
{
   reset();

   if (ar->IsLoading())
   {
      CString line;

      // make sure this is a page layout file
      *ar >> line;
      if (line == PAGE_LAYOUT_FILE_STRING)
      {
         // get the FalconView version string
         // the last revision for each release will be supported
         *ar >> line;
         if (line == PAGE_LAYOUT_FILE_FVW_VERSION_STRING_311)
         {
            // valid application string
            m_app_version = line;

            int revision;
            *ar >> revision;

            // if a valid revision number is read
            if (revision >= 0 && revision <= PAGE_LAYOUT_FILE_REVISION_311)
            {
               m_revision = revision;
               return last_revision();
            }
         }
      }
   }

   return false;
}

void PageLayoutFileVersion::Write(CArchive *ar)
{
   if (ar->IsStoring())
   {
      *ar << PAGE_LAYOUT_FILE_STRING;

      *ar << PAGE_LAYOUT_FILE_FVW_VERSION_STRING_311;

      *ar << PAGE_LAYOUT_FILE_REVISION_311;
   }
}

// Returns TRUE if the file read contains the last revision.
bool PageLayoutFileVersion::last_revision()
{
   // for later versions of FalconView

   // for FalconView 3.11
   if (get_application_version() == PAGE_LAYOUT_FILE_FVW_VERSION_STRING_311)
      return (get_revision() == PAGE_LAYOUT_FILE_REVISION_311);

   return false;
}

void PageLayoutFileVersion::ReportFileVersionProblem()
{
   CString *msg = NULL;

   // if no valid information was retrieved from the header
   if (!valid_data())
   {
      msg = new CString();
      msg->Format("Error reading Page Layout File.  This is not a valid "
         "Page Layout File, or it was produced with an early Alpha "
         "version of FalconView 3.11.");
   }
   // if this is a FalconView 3.11 revision
   else if (get_application_version() ==
      PAGE_LAYOUT_FILE_FVW_VERSION_STRING_311)
   {
      msg = new CString();
      msg->Format("This is a FalconView 3.11 Revision %d Page Layout File.  "
         "The file format has changed.  Only Revision %d Page Layout Files "
         "are supported by this version of FalconView.", get_revision(),
         PAGE_LAYOUT_FILE_REVISION_311);
   }
   else
      return;

   FVW_display_error_string(msg);
}

// Sets member variables to initial state.
void PageLayoutFileVersion::reset()
{
   m_app_version.Empty();
   m_revision = -1;
}

// Returns TRUE if valid application version and revision data was read.
bool PageLayoutFileVersion::valid_data()
{
   return (m_app_version.GetLength() && m_revision >= 0);
}
