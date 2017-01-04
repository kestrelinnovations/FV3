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



// lplookup.cpp : implementation file
//

#include "stdafx.h"
#include "lplookup.h"
#include "err.h"
#include "lpdao.h"

/////////////////////////////////////////////////////////////////////////////
// CMinLocalPointLookup

//TO DO: comment this in header
//THROWS CDaoException, CMemoryException
CLocalPointLookup::CLocalPointLookup(CDaoDatabase* database, const char* db_filespec)
   : CDBOpenerCloser(database, db_filespec)
{
   m_set = NULL;
   m_set = new CLocalPointSet(m_database);  
}

//---------------------------------------------------------------------------

//returns a pointer to a set open to the requested record
//THROWS: CDaoException, CMemoryException
//WARNING: points that are in the deleted table may be returned
CLocalPointSet* CLocalPointLookup::Lookup(CString table, CString id, CString group_name)
{

   ASSERT(m_database);
   ASSERT(m_set);

   CString SQL =  "SELECT * FROM " + table + " WHERE [ID] = \"" + id 
      + "\" AND [Group_Name] = \"" + group_name + "\"";

   try
   {
      m_set->Open(dbOpenSnapshot, SQL, dbForwardOnly | dbReadOnly);
   }
   catch (CDaoException* e)
   {
      CString msg="DAO exception in CLocalPointLookup: " +
         e->m_pErrorInfo->m_strDescription;
      ERR_report(msg);
      if (m_set->IsOpen())
         m_set->Close();
      throw;
   }

   if (m_set->IsEOF() || m_set->IsBOF()) //if not found in modified table..
   {
      m_set->Close();
      return NULL;
   }
   
   //else the set is still open to the record we want
   return m_set;
}

//---------------------------------------------------------------------------
CLocalPointSet* CLocalPointLookup::Lookup(CString id, CString group_name)
{
   CLocalPointSet* set = NULL;

   // Lookup the point in the Points__Modified table first,
   // but if the table doesn't exist, that's OK, just continue
   try
   {
      set = Lookup("Points__Modified", id, group_name);
      if (!set)
         set = Lookup ("Points", id, group_name); 
   }
   catch (CDaoException* e)
   {
      if (e->m_scode == E_DAO_InputTableNotFound)
         e->Delete();
      else
         throw e;
   }

   return set;
}

//---------------------------------------------------------------------------

CLocalPointLookup::~CLocalPointLookup()
{

   try
   {
      //destructor for CDBOpenerCloser is called after this, so
      //the database should still be open
      ASSERT(m_database->IsOpen());  

      //close record set
      if (m_set->IsOpen())
         m_set->Close();
      delete m_set;
      m_set=NULL;
   }
   catch (CDaoException* e)
   {
      // TO DO: what kind of cleanup here if any?
      CString msg="DAO exception in ~CLocalPointLookup: ";
      msg+=e->m_pErrorInfo->m_strDescription;
      ERR_report(msg);
      throw;
   }

}
