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



// CommaDelimitedString.cpp: implementation of the CCommaDelimitedString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommaDelimitedString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




//Test cases:
   /*
   CCommaDelimitedString fields("\"Howdy\",2,\"she said: \"\"hello\"\".\",\"food\",\"\",\"Atlanta, GA\",\"she said,\"\"hello\"\".\"");
   int type;
   CString str;
   while (fields.get_next(str, type));
/* 
   CCommaDelimitedString errors("\"she said: \"hello\",2");
   while (errors.get_next(str, type));

   CCommaDelimitedString errors2("1,2,3,ABC");
   while (errors2.get_next(str, type));

*/



CCommaDelimitedString::CCommaDelimitedString()
{
   m_position = NULL;
}



CCommaDelimitedString::CCommaDelimitedString(const CString & string)
{
   //TO DO: in debug version, do some checking on the string to be sure
   //its in the right format, e.g., even number of quotes etc
   
   int     index = 0;
   CString temp  = string;

   // (1) split on the commas
   do
   {
      CString* pField;

      //we have reached the end of a numeric field
      index = temp.Find(','); 
      if (index > -1)
      {
         //grap up to comma "012" of "012,456"  (index of comma is 3 and we want 3 chars)
         pField = new CString(temp.Left(index));  
         //grab after comma "456" of "012,456"  (length is 7, index is 3, we want 3 chars)
         temp = temp.Right(temp.GetLength()-index-1); 
      }
      else
      {
         pField = new CString(temp);               //we're at the last field, no more commas
      }

      //add the field 
      m_field_list.AddTail(pField);

   }
   while (index > -1 );

   // (2) join together any fields that have been split on a "," that should not have been
   // e.g. "Atlanta, GA"
   POSITION pos;
   pos = m_field_list.GetHeadPosition();
   while (pos)
   {

      POSITION next=pos;

      CString *pField;
      pField = m_field_list.GetNext(next);

      //count number of quotes in string
      if (!pField->IsEmpty())
      {
         int length = pField->GetLength();
         int count=0;
         for (int i =0;i<length;i++)
         {
            if ('\"' == pField->GetAt(i))
               count++;
         }

         if (count%2 != 0) // if odd number of quotes...
         {
            //odd number of quotes, so concatenate with next field in list
            //and retry counting this same field nex time through while loop

            POSITION current = next;                  //save position of field we are concatenating

            CString* pAdjacent;

            //if we do not have an even number of quotes, we should not yet
            //be at the end of the list
            if (!next)
            {
               ASSERT(0);  
               break;
            }

            pAdjacent = m_field_list.GetNext(next);
            ASSERT(pAdjacent);

            if (pAdjacent)
            {
                  (*pField) += ","; //add the comma we removed earlier when splitting
                  (*pField) += (*pAdjacent);
            }

            //remove the CString we concatenated 
            m_field_list.RemoveAt(current);
            if (pAdjacent)
               delete pAdjacent;


            //keep pos the same so we retry the same field (with the concatenation now)
         }
         else
         {  
            //even number of quotes, so go on to the next field
            pos = next;
         }
      }
   } //while

   //set position so that GetNext returns the next element in the list 
   //w/o the need to call GetNext
   m_position = m_field_list.GetHeadPosition();  //just in case no one calls get first

}



CCommaDelimitedString::~CCommaDelimitedString()
{

   //delete the CStrings in the field list
   CString* x;
   while (!m_field_list.IsEmpty())
   {
      VERIFY (x = m_field_list.RemoveHead());
      if (x)
         delete x;
   }

}


int CCommaDelimitedString::get_count() const
{
   return m_field_list.GetCount();
}


int CCommaDelimitedString::get_first(CString & field, int & type)
{
   //type = 0 alpha
   //type = 1 numeric

   //returns SUCCESS on success
   m_position = m_field_list.GetHeadPosition();
   return get_next(field, type);
}


//Note: GetNext() will return the first element in the list the first time 
//it is called.  GetFirst() can be used to reset the enumeration if needed
int CCommaDelimitedString::get_next(CString & field, int & type)
{
   //returns SUCCESS on success

   int result = FAILURE;

   if (m_position)
   {
      CString* p;
      p = m_field_list.GetNext(m_position);
      if (p)
         field = *p;
      else
      { 
         ASSERT(0);
         field = "";
      }

      if (convert_quotes(field))
         type = 0;  //alpha
      else
         type = 1;  //numeric

      result = SUCCESS;
   }

   return result;
}


int CCommaDelimitedString::convert_quotes(CString & str) const
{
   //remove any single quotes, and replace double quotes with single quotes
   //returns: 1 if any quotes were removed
   //         0 if no quotes were found

   int length = str.GetLength();
   CString temp;
   char ch;

   BOOL at_least_one_quote = FALSE;
   BOOL possible_first_of_double = FALSE;
   for (int i=0;i<length;i++)
   {
      ch = str.GetAt(i);
      if ('\"' == ch)
      {
         at_least_one_quote = TRUE;
         if (i!=0 && i!=length-1)
         {
            //if the last character was not possibly the first quote of a double quote...
            if (!possible_first_of_double)
               //this character may be the first quote of a double quote
               possible_first_of_double = TRUE;
            else
            {
               //found second quote of double quote
               temp+=ch;                           //add the quote to temp
               possible_first_of_double = FALSE;   //no longer looking for the second quote
            }
         }
         //else do nothing because we don't want the first or last quote
      }
      else
      {
         //we didn't find a quote so it cannot possibly be the first half of a double quote
         // we want the character though
         temp+=ch;                           //add the quote to temp
         possible_first_of_double = FALSE;
      }
   }

   if (at_least_one_quote) //if we've found and quotes (and thus made changes)
      str = temp; //assign to temp, the string we've been modifing

   return at_least_one_quote;
}



int CCommaDelimitedString::get_first(CString & field)
{
   int type;
   return get_first(field, type);
}


int CCommaDelimitedString::get_next(CString & field)
{
   int type;
   return get_next(field, type);
}


int CCommaDelimitedString::add(CString & string)
{
   CString* pField = new CString(string);

   *pField = "\"" + *pField + "\"";

   m_field_list.AddTail(pField);
   return SUCCESS;
}


int CCommaDelimitedString::add(int number)
{
   CString* pField = new CString();

   pField->Format("%d", number);

   m_field_list.AddTail(pField);
   return SUCCESS;
}


int CCommaDelimitedString::get_complete_string(CString & string)
{
   CString  field;
   POSITION pos;

   // empty the incoming string
   string.Empty();

   // add each element in succession
   pos = m_field_list.GetHeadPosition();

   while (pos)
   {
      field = *m_field_list.GetNext(pos);

      string += field;
      if (pos)
         string += ",";
   }

   return SUCCESS;
}

CCommaDelimitedString::operator CString()
{
   CString temp;

   get_complete_string(temp);
   return temp;
}
