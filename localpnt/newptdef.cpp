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



#include "stdafx.h"
#include "newptdef.h"
#include "FalconView/include/param.h"

// newptdef.cpp : implementation file

CNewLocalPointDefaults::CNewLocalPointDefaults()
{
   m_save_changes_when_deleted=TRUE;
   m_prefix = PRM_get_registry_string("Local Points",  "Default ID Prefix", "A");
   m_group_name = PRM_get_registry_string("Local Points", "Default Group Name", "Default");
   //set group name to "Default" if group name = ""
   if (m_group_name.IsEmpty())
      m_group_name = "Default";
   load_count();   
}

CNewLocalPointDefaults::~CNewLocalPointDefaults()
{
   if (m_save_changes_when_deleted)
   {
      PRM_set_registry_string("Local Points", "Default ID Prefix", m_prefix);
      PRM_set_registry_string("Local Points","Default Group Name", m_group_name);
      save_count();   
   }
}


//save count for m_prefix
void CNewLocalPointDefaults::save_count()
{
   PRM_set_registry_int("Local Points\\Prefixes\\"+m_group_name, m_prefix , m_count);
}

//load count for m_prefix
void CNewLocalPointDefaults::load_count() 
{
   m_count = PRM_get_registry_int("Local Points\\Prefixes\\"+m_group_name, m_prefix, 0);
}


CString CNewLocalPointDefaults::GetID()
{
   // Have the count as two characters (e.g., "A01", "A02") so the points sort better
   // in the tabular editor
   CString strCount;
   strCount.Format("%d", m_count);
   while (strCount.GetLength() < 2)
      strCount = "0" + strCount;

   return m_prefix + strCount;
}

CString CNewLocalPointDefaults::get_and_increment_id()
{
   CString result = GetID();
   m_count++;
   return result;
}


void CNewLocalPointDefaults::SetIDPrefix(CString& prefix)
{
   save_count();     //save count of old prefix/group name
   m_prefix=prefix;  //assign new prefix
   load_count();     //load count of new prefix/group name
}

void CNewLocalPointDefaults::SetGroupName(CString& group_name)
{
   save_count();     //save count of old prefix/group name
   m_group_name = group_name;
   load_count();     //load count of new prefix/group name
}
   
