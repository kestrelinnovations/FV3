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



// fovllist.cpp
// Implementation of the Recently Open File Overlay List

#include "stdafx.h"

#ifndef __AFXADV_H__
	#include <afxadv.h>  // CRecentFileList
#endif

#include "fovllist.h"
#include "overlay.h"
#include "param.h"
#include "err.h"
#include "..\FvCore\Include\GuidStringConverter.h"

// destructor
RecentFileOverlayList::~RecentFileOverlayList()
{
   // remove and delete all the elements in the list
   while (!m_list.IsEmpty())
      delete m_list.RemoveHead();
}
   
// add this file overlay to the list
void RecentFileOverlayList::add(GUID overlayDescGuid, const char *file_specification)
{
   POSITION position = find(file_specification);
   RecentlyOpenFileOverlay *element;

   // if the element is already in the list, move it to the head
   if (position)
   {
      element = m_list.GetAt(position);
      m_list.RemoveAt(position);
      m_list.AddHead(element);
      return;
   }

   // create a new element and add it to the list
   element = new RecentlyOpenFileOverlay(overlayDescGuid, file_specification);

   // add it to the begining of the list (head is file 0)
   m_list.AddHead(element);

   // remove tail if list contains too many items
   if (m_list.GetCount() > m_max_count)
   {
      // remove and delete the element
      element = m_list.RemoveTail();
      delete element;
   }

   ASSERT(m_list.GetCount() <= m_max_count);
}

// remove this file overlay from the list, FAILURE if not found
int RecentFileOverlayList::remove(GUID overlayDescGuid, const char *file_specification)
{
   POSITION position = find(file_specification);
   RecentlyOpenFileOverlay *element;

   if (position == NULL)
   {
      ERR_report("Not in list.");
      return FAILURE;
   }

   // get the element
   element = m_list.GetAt(position);

   // make sure class names match
   if (element->GetOverlayDescriptorGuid() != overlayDescGuid)
   {
      ERR_report("Class mismatch.");
      return FAILURE;
   }

   // remove and delete the element
   m_list.RemoveAt(position);
   delete element;

   return SUCCESS;
}

// get the class name of this file overlay, returns "" if the entry is not
// found
GUID RecentFileOverlayList::GetOverlayDescriptorGuid(const char *file_specification)
{
   POSITION position = find(file_specification);
   RecentlyOpenFileOverlay *element;

   if (position)
   {
      element = m_list.GetNext(position);
      return element->GetOverlayDescriptorGuid();
   }

   return GUID_NULL;
}

// load the recent file overlay list from the registry, and synchronize it
// with the recent file list.
/// <param name="recent_file_list">Recent file list object created by MFC's LoadProfileSettings.  Note
/// that this object can be NULL in the case that the group policy setting for "Do not keep history of
/// recently opened documents is Enabled.</param>
int RecentFileOverlayList::initialize(CRecentFileList *recent_file_list)
{
   char index[3];
   CString file_specification;
   CString value;
   CString msg;
   int size;
   int comma_loc; // string index of comma between class-name and file spec
   int spec_loc;  // string index of start of file spec
   int i;
   RecentlyOpenFileOverlay *element;

   // the list must be empty when this function is called
   ASSERT (m_list.IsEmpty());

   // get the size of the recent file list
   size = recent_file_list == NULL ? 0 : recent_file_list->GetSize();

   for (i=0; i<size; i++)
   {
      // get the file specification
      file_specification = (*recent_file_list)[i];

      // if the specification is "" the slot hasn't been used yet
      if (file_specification.GetLength() == 0)
         break;

      // read in the corresponding entry in the recent file overlays list
      sprintf_s(index, 3, "%d", i);
      value = PRM_get_registry_string("RecentFileOverlays2", index);
      if (value.GetLength() == 0)
      {
         msg.Format("Failure reading entry %d (index = %d) out of %d entries.",
            i+1, i, size);
         ERR_report(msg);
         return FAILURE;
      }

      // make sure the file-specifications are the same
      spec_loc = value.Find(file_specification);
      if (spec_loc == -1)
      {
         CString value_upper_case(value);
         CString spec_upper_case(file_specification);

         // do a case insensitive find, just in case
         value_upper_case.MakeUpper();
         spec_upper_case.MakeUpper();
         spec_loc = value_upper_case.Find(spec_upper_case); 
         if (spec_loc == -1)
         {
            msg.Format("File specifications don't match for entry %d "
               "(index = %d) out of %d "
               "entries.", i+1, i, size);
            ERR_report(msg);
            return FAILURE;
         }
      }

      // the comma separates the class-name from the file-specification
      comma_loc = value.Find(',');
      if (comma_loc == -1 || comma_loc !=  (spec_loc - 1))
      {
         msg.Format("Entry %d (index = %d) out of %d entries is not valid.",
            i+1, i, size);
         ERR_report(msg);
         return FAILURE;
      }

      // trim off the comma and file specification
      value = value.Left(comma_loc);

      std::string strGuid(value);
      string_utils::CGuidStringConverter guidConv(strGuid);

      // create an element to hold this file spec, class-name pair
      element = new RecentlyOpenFileOverlay(guidConv.GetGuid(), file_specification);

      // add it to the end of the list (head is file 0)
      m_list.AddTail(element);
   }

   // the list may contain at most size items
   m_max_count = size;

   m_initialized = TRUE;

   return SUCCESS;
}

// save the recent file overlay list to the registry
int RecentFileOverlayList::save()
{
   RecentlyOpenFileOverlay *element;
   POSITION position;
   char index[3];
   CString value;
   int i;

   if (!m_initialized)
      return SUCCESS;

   // remove the entire "RecentFileOverlays" section before writing out 
   // current state
   if (SUCCESS != PRM_delete_registry_section("RecentFileOverlays2"))
      ERR_report("Delete RecentFileOverlays failed.");
   
   i = 0;
   position = m_list.GetHeadPosition();
   while (position)
   {
      // get the next element, advance position
      element = m_list.GetNext(position);

      string_utils::CGuidStringConverter guidConv(element->GetOverlayDescriptorGuid());

      // build the registry string
      value.Format("%s,%s", guidConv.GetGuidString().c_str(), element->get_file_spec());

      // write out the overlay type guid, file-specification pair
      sprintf_s(index, 3, "%d", i);
      if (PRM_set_registry_string("RecentFileOverlays2", index, value) != 
         SUCCESS)
      {
         ERR_report("PRM_set_registry_string() failed.");
         return FAILURE;
      }

      i++;
   }

   return SUCCESS;
}

// get the position of this entry in the file list
POSITION RecentFileOverlayList::find(const char *file_specification)
{
   RecentlyOpenFileOverlay *element;
   POSITION position;
   POSITION element_position;
   CString value;

   position = m_list.GetHeadPosition();
   while (position)
   {
      // get the next element, advance position, save current position
      element_position = position;
      element = m_list.GetNext(position);

      // compare file specification
      value = element->get_file_spec();
      if (value.CompareNoCase(file_specification) == 0)
         return element_position;
   }

   return NULL;
}
