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



// PageList.cpp
// Implementation of CPrinterPageList class for maintaining a list of
// CPrinterPage objects.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/PageList.h"
// system includes
// third party files
// other FalconView headers
// this project's headers
#include "FalconView/PrintToolOverlay/PrntPage.h"   // CPrinterPage
#include "FalconView/PrintToolOverlay/SChart.h"     // CStripChart

CPrinterPageList::CPrinterPageList()
{
}

CPrinterPageList::~CPrinterPageList()
{
   DeleteAll();
}

CPrinterPage* CPrinterPageList::GetAtViewCoords(const CPoint& pt)
{
   int i;
   CPrinterPage* pRetPage = NULL;

   // Loop through all our pages in the opposite order in which they are
   // drawn.
   for (i = (GetSize() - 1); i >= 0; i--)
   {
      // Check our Pageangle hit test
      if (GetAt(i)->hit_test(pt))
      {
         pRetPage = GetAt(i);
         break;
      }
   }

   return(pRetPage);
}

void CPrinterPageList::UnSelectAll()
{
   // Loop through all our Pages and 'unselect' them
   for (int i = 0; i < GetSize(); i++)
   {
      GetAt(i)->UnSelect();
   }
}

CPrinterPage* CPrinterPageList::GetSelected()
{
   // Loop through all our pages and 'unselect' them
   for (int i = 0; i < GetSize(); i++)
   {
      if (GetAt(i)->IsSelected())
      {
         return(GetAt(i));
      }
   }

   return(NULL);
}

void CPrinterPageList::DeleteSelectedPage()
{
   // Loop through all our Pages and 'unselect' them
   for (int i = 0; i < GetSize(); i++)
   {
      if (GetAt(i)->IsSelected())
      {
         // Invalidate the view
         GetAt(i)->Invalidate();
         // Delete the page - the destructor will remove the page from the
            //   list
         delete GetAt(i);
         break;
      }
   }
}

void CPrinterPageList::Delete(CPrinterPage* pPage)
{
   for (int i = 0; i < GetSize(); i++)
   {
      CPrinterPage* pCurPage = GetAt(i);
      if (pCurPage == pPage)
      {
         // Invalidate the view
         pCurPage->Invalidate();
         // delete the page - the destructor will remove the page from
            //  the list
         delete pCurPage;
      }
   }
}

void CPrinterPageList::DeleteAll()
{
   // Pointer Cleanup
    // NOTE: Walk backwards through the list because the destructor of each
    //       page will remove it from the list
   for (int i = (GetSize()-1); i >= 0; i--)
   {
      CPrinterPage* pPage = GetAt(i);
      delete pPage;
   }

    // If the above code worked, we should have nothing left in the list
    ASSERT(GetSize() == 0);
}

int CPrinterPageList::GetPageIndex(CPrinterPage* pPage)
{
   // Loop through all our pages and 'unselect' them
   for (int i = 0; i < GetSize(); i++)
   {
      if (GetAt(i) == pPage)
      {
         return i;
      }
   }

   return -1;
}

void CPrinterPageList::Remove(CPrinterPage* pPage)
{
   // See if the page passed in is in our list - if so, remove it
   for (int i = 0; i < GetSize(); i++)
   {
      if (GetAt(i) == pPage)
      {
         RemoveAt(i);
         // DO NOT DELETE THE POINTER, THIS FUNCTION SIMPLY REMOVES IT FROM
         //   THE MASTER LIST - CALL DELETPAGE() TO DELETE THE PAGE
      }
   }
}
