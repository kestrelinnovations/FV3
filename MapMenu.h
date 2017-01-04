// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

#include "MAPS_D.H"

class DisableAddToQuickAccessToolbarRibbonButton;

class MapMenuEntry
{
public:
   MapSource m_source;
   MapScale m_scale;
   MapSeries m_series;
   BOOL m_is_soft_scale;
   int nGroupIdentity;
   int nIdentity;
   CString ToString();
   int iStatus; // -1 = deleted, 0 = exists, +1 added (for custom maps dialog)

protected:
   // for a given map type, return a string for the hotkey.  For example, Tiros World map is F4.
   // If no hotkey is associated, then this method returns an empty string
   CString GetHotkeyString();
};

class MapMenu
{
public:
   // used to maintain a list of CMenu popups created by the map menu
   std::vector<CMenu *> m_mapMenuPopups;

   DisableAddToQuickAccessToolbarRibbonButton* m_ribbon_btn;

   // used to store assocation between command IDs and map source, scale, and 
   // series for the Map menu
   CMap <UINT, UINT, MapMenuEntry, MapMenuEntry> m_map_menu_assoc;
};