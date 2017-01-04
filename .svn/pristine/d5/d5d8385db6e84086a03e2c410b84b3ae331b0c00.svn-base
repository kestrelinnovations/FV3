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



// filelist.h
// Definition of the Recently Open File Overlay List

#ifndef RECENT_FILE_OVERLAY_LIST
#define RECENT_FILE_OVERLAY_LIST 1

#include "common.h"

// Forward Definition of Recently Open File Overlay List Element class.
class RecentlyOpenFileOverlay;

class RecentFileOverlayList
{
private:
   CList<RecentlyOpenFileOverlay *, RecentlyOpenFileOverlay *> m_list;
   int m_max_count;  // max number of elements in list
   boolean_t m_initialized;
   CString m_empty_string;
   
public:
   // constructor
   RecentFileOverlayList() : m_max_count(0), m_initialized(FALSE) {}

   // destructor
   virtual ~RecentFileOverlayList();

   // add this file overlay to the list
   void add(GUID overlayDescGuid, const char *file_specification);
   
   // remove this file overlay from the list, FAILURE if not found
   int remove(GUID overlayDescGuid, const char *file_specification);

   // get the class name of this file overlay, returns "" if the entry is not
   // found
   GUID GetOverlayDescriptorGuid(const char *file_specification);
   
   // load the recent file overlay list from the registry, and synchronize it
   // with the recent file list.
   int initialize(CRecentFileList *recent_file_list);

   // save the recent file overlay list to the registry
   int save();

private:
   // get the position of this entry in the file list
   POSITION find(const char *file_specification);
};

class RecentlyOpenFileOverlay
{
private:
   GUID m_overlayDescGuid;
   CString m_file_spec;

public:
   RecentlyOpenFileOverlay(GUID overlayDescGuid, const char *file_spec) :
      m_overlayDescGuid(overlayDescGuid), m_file_spec(file_spec) {}

   GUID GetOverlayDescriptorGuid() { return m_overlayDescGuid; }
   const CString &get_file_spec() { return m_file_spec; }
};

#endif