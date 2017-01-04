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

//  IconList.cpp
//  Implementation of PrintIconList class for maintaining a list of PrintIcon
// objects.

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/IconList.h"   // PrintIconList

// system includes

// third party files

// other FalconView headers

// this project's headers
#include "FalconView/PrintToolOverlay/PrntPage.h"   // CPrinterPage
#include "FalconView/PrintToolOverlay/AChart.h"     // CAreaChart
#include "FalconView/PrintToolOverlay/SChart.h"     // CStripChart

static int get_print_icon_type(PrintIcon *icon);
static PrintIcon *create_print_icon(int type, CPrintToolOverlay *overlay);

PrintIconList::PrintIconList()
{
}

PrintIconList::~PrintIconList()
{
   DeleteAll();
}

int PrintIconList::CalcAll()
{
   bool surface_too_large = false;
   bool failure = false;
   int status;

   // Loop through all our elements and recalc each one
   for (int i = 0; i < GetSize(); i++)
   {
      status = GetAt(i)->Calc(true, true);
      if (status == SURFACE_TOO_LARGE)
         surface_too_large = true;
      else if (status == FAILURE)
         failure = true;
   }

   // if Calc failed on any element, return FAILURE
   if (failure)
   {
      ERR_report("Calc() failed.");
      return FAILURE;
   }

   // if Calc never failed, but it returned SURFACE_TOO_LARGE on any element,
   // return SURFACE_TOO_LARGE
   if (surface_too_large)
      return SURFACE_TOO_LARGE;

   return SUCCESS;
}

void PrintIconList::DrawAll(MapProj *map, CDC* pDC)
{
   // Loop through all our elements and draw each one to the device context
   // passed in
   for (int i = 0; i < GetSize(); i++)
   {
      GetAt(i)->Draw(map, pDC);
   }
}

PrintIcon *PrintIconList::GetAtViewCoords(const CPoint& pt)
{
   int i;
   PrintIcon *icon;

   // Loop through all our elements in the opposite order in which they are
   // drawn.
   for (i = (GetSize() - 1); i >= 0; i--)
   {
      // if a PrintIcon is hit by this point
      icon = GetAt(i)->GetAtViewCoords(pt);
      if (icon)
         return icon;    // return it
   }

   return NULL;
}

void PrintIconList::UnSelectAll()
{
   // Loop through all our elements and 'unselect' them
   for (int i = 0; i < GetSize(); i++)
   {
      GetAt(i)->UnSelect();
   }
}

// Unselect every icon except for this one.
void PrintIconList::UnSelectAllExcept(PrintIcon *except)
{
   PrintIcon *icon;

   // Loop through all our elements and 'unselect' them
   for (int i = 0; i < GetSize(); i++)
   {
      icon = GetAt(i);
      if (icon != except)
         icon->UnSelect();
   }
}

void PrintIconList::Delete(PrintIcon* icon)
{
   for (int i = 0; i < GetSize(); i++)
   {
      PrintIcon *current  = GetAt(i);
      if (current == icon)
      {
         // erase from the view
         current->Invalidate();

         // remove from list
         RemoveAt(i);

         // delete the element
         delete current;
      }
   }
}

void PrintIconList::DeleteAll()
{
   // remove and delete all elements
   for (int i = (GetSize()-1); i >= 0; i--)
   {
      // get the item
      PrintIcon *icon = GetAt(i);

      // remove it from the list
      RemoveAt(i);

      // destroy it
      delete icon;
   }

    // If the above code worked, we should have nothing left in the list
    ASSERT(GetSize() == 0);
}

void PrintIconList::Remove(PrintIcon* icon)
{
   // See if the page passed in is in our list - if so, remove it
   for (int i = 0; i < GetSize(); i++)
   {
      if (GetAt(i) == icon)
      {
         RemoveAt(i);
         // DO NOT DELETE THE POINTER, THIS FUNCTION SIMPLY REMOVES IT FROM
         //   THE MASTER LIST - CALL DELETPAGE() TO DELETE THE PAGE
      }
   }
}

void PrintIconList::Serialize(CArchive* ar, PageLayoutFileVersion *version,
                              CPrintToolOverlay *overlay)
{
   PrintIcon *icon;
   int count;
   int i;
   int type;

   if (ar->IsStoring())
   {
      // save the number of elements in the list
      count = GetSize();
      *ar << count;

      // distinguish between different types of objects, so we can read them
      // back in below
      for (i = 0; i < count; i++)
      {
         // get the next element
         icon = GetAt(i);

         // save the type of this element
         type = get_print_icon_type(icon);
         *ar << type;

         // save the data for this element
         icon->Serialize(ar, version);
      }
   }
   else
   {
      // get the number of elements
      *ar >> count;

      for (i = 0; i < count; i++)
      {
         // get type from file
         *ar >> type;

         // created an object of that type of PrintIcon
         icon = create_print_icon(type, overlay);

         // get the data for that object and add it
         if (icon)
         {
            icon->Serialize(ar, version);

            // if the object was properly initialized by the Serialize member,
            // it can be added to the list
            if (icon->WasLoadOK())
               Add(icon);
            else
               delete icon;   // otherwise we will just ignore it
         }
      }
   }
}

// This function is tied to create_print_icon().  Class names in
// print_icon_types must remain in their original position.  New classes must
// be added to the end of the list.  Never insert new classes at the begining
// or middle of the list.
int get_print_icon_type(PrintIcon *icon)
{
   char *print_icon_types[] =
   {"CPrinterPage", "CStripChart", "CAreaChart", ""};
   int i = 0;

   while (strlen(print_icon_types[i]) > 0)
   {
      // the icon-type for a PrintIcon derived class is the index into the
      // print_icon_types array
      if (strcmp(print_icon_types[i], icon->get_class_name()) == 0)
         return i;

      i++;
   }

   return -1;
}

// This function is tied to get_print_icon_type().  The integers in the type
// switch statement correspond to array indices in print_icon_types.
PrintIcon *create_print_icon(int type, CPrintToolOverlay *overlay)
{
   PrintIcon *icon;

   switch (type)
   {
      case 0:
         icon = new CPrinterPage(overlay);
         break;

      case 1:
         icon = new CStripChart(overlay);
         break;

      case 2:
         icon = new CAreaChart(overlay);
         break;

      default:
         icon = NULL;
   }

   return icon;
}
