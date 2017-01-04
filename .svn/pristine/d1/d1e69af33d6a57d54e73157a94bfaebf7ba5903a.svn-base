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



// newptdef.h : header file
//


#ifndef NEW_POINT_DEFAULLTS_H
#define NEW_POINT_DEFAULLTS_H 1


/////////////////////////////////////////////////////////////////////////////
// CNewLocalPointDefaults


class CNewLocalPointDefaults 
{
   CString m_group_name;
   CString m_prefix;
   int m_count;
   BOOL m_save_changes_when_deleted;

protected:
   
   void load_count();
   void save_count();

public:
 
   CNewLocalPointDefaults();   //read from registry  
   ~CNewLocalPointDefaults();  //write to retistry
   
   void SetSaveChangesWhenDeleted(BOOL save) {m_save_changes_when_deleted=save;}
   void SetIDPrefix(CString& prefix);
   CString GetIDPrefix() {return m_prefix;}
   CString GetID();
   CString get_and_increment_id();
   
   //int GetCount() {return m_count;}

   void SetGroupName(CString& group_name);
   CString&  GetGroupName() {return m_group_name;}
   
      
   //SetID();
   
};


#endif