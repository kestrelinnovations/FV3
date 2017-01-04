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
// Description of PrintIconList class for maintaining a list of PrintIcon
// objects.

#ifndef FALCONVIEW_PRINTTOOLOVERLAY_ICONLIST_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_ICONLIST_H_

// system includes
// third party files
// other FalconView headers
// this project's headers
// forward definitions
class CPrintToolOverlay;
class MapProj;
class PageLayoutFileVersion;
class PrintIcon;

class PrintIconList : public CArray< PrintIcon*, PrintIcon* >
{
public:
   // Construction/Destruction
   PrintIconList();
   ~PrintIconList();

   // Re-calculating all printer dependent values.
   int CalcAll();

   // Drawing
   void DrawAll(MapProj *map, CDC *pDC);

   // UI
   PrintIcon *GetAtViewCoords(const CPoint& pt);

   // Selection
   void UnSelectAll();
   void UnSelectAllExcept(PrintIcon *except);

   // Misc
   void DeleteAll();

   // Remove/Delete
   void Delete(PrintIcon* pPage);
   void Remove(PrintIcon* pPage);

   // Serialization
   void Serialize(CArchive* ar, PageLayoutFileVersion *version,
      CPrintToolOverlay* pOverlay);
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_ICONLIST_H_
