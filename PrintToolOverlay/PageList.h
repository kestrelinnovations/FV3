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

// PageList.h
// Description of CPrinterPageList class for maintaining a list of CPrinterPage
// objects.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_PAGELIST_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_PAGELIST_H_

// system includes
// third party files
// other FalconView headers
// this project's headers
// forward definitions
class CPrinterPage;
class CPrintToolOverlay;
class MapProj;
class PageLayoutFileVersion;

class CPrinterPageList : public CArray< CPrinterPage*, CPrinterPage* >
{
public:
   CPrinterPageList();
   ~CPrinterPageList();

   // UI
   CPrinterPage* GetAtViewCoords(const CPoint& pt);

   // Selection
   void UnSelectAll();
   CPrinterPage* GetSelected();
   void DeleteSelectedPage();

   // Misc
   void DeleteAll();
   int GetPageIndex(CPrinterPage* pPage);

   // Remove/Delete
   void Delete(CPrinterPage* pPage);
   void Remove(CPrinterPage* pPage);
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_PAGELIST_H_
