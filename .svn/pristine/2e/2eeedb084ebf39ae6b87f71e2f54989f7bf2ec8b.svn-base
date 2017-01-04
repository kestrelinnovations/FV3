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



// CDBOpenerCloser : header file
//


#ifndef DB_OPENER_CLOSER_H
#define DB_OPENER_CLOSER_H 1


/////////////////////////////////////////////////////////////////////////////
// CDBOpenerCloser

//forward
class CLocalPointSet;

class CDBOpenerCloser 
{

   BOOL m_this_object_created_database;

protected:
   CDaoDatabase *m_database;  //this can be accessed in derived classes

public:
   CDBOpenerCloser(CDaoDatabase* database, const char* db_filespec);
   virtual ~CDBOpenerCloser();
   BOOL CreatedDatabase() {return m_this_object_created_database;}  
   CDaoDatabase *GetDatabase() {ASSERT (m_database->IsOpen()); return m_database; } 
};


#endif