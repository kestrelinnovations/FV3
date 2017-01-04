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



// dbopncls.cpp : implementation file
//

#include "stdafx.h"
#include "dbopncls.h"
#include "err.h"

/////////////////////////////////////////////////////////////////////////////
// CMinLocalPointLookup

//TO DO: comment this in header
//THROWS CDaoException
CDBOpenerCloser::CDBOpenerCloser(CDaoDatabase* database, const char* db_filespec)
{
   //initialize memeber variables
   m_database=NULL;
   m_this_object_created_database=FALSE;

   //open database if we need to and note if we did
   if (database)
   {
      //a pointer to a presumably open database was passed in
      //this database will not be closed by this object when it is destroyed
      m_database=database; 

      ASSERT(database->IsOpen());  //somehow, the database was closed - this should not happen
      
      //if the database
      if (!m_database->IsOpen())
      {
         try
         {
            m_database->Open(db_filespec);
         }
         catch (CDaoException* e)
         {
            // TO DO: throw this exception again ?
            //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
            CString msg="DAO exception in CLocalPointLookup: ";
            msg+=e->m_pErrorInfo->m_strDescription;
            ERR_report(msg);
            throw;   //rethrow current exception
         }
      }
   }
   else
   {
      m_database = new CDaoDatabase();
      m_this_object_created_database=TRUE;

      try
      {
         m_database->Open(db_filespec);
      }
      catch (CDaoException* e)
      {
         // TO DO: throw this exception again ?
         //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
         CString msg="DAO exception in CDBOpenerCloser: ";
         msg+=e->m_pErrorInfo->m_strDescription;
         ERR_report(msg);
         throw;   //rethrow current exception
      }
   }
}


//---------------------------------------------------------------------------

CDBOpenerCloser::~CDBOpenerCloser()
{
   try
   {
      //close database and delete database object if we opened and created it
      if (m_this_object_created_database)
      {
         if (m_database->IsOpen())
            m_database->Close();
         delete m_database;
      }
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
