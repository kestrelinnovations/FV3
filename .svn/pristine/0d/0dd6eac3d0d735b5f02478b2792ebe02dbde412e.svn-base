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

// UnitsComboBox.cpp
//
#include "stdafx.h"
#include "UnitsComboBox.h"

CString CUnits::ToString()
{
   switch(m_eUnits)
   {
   case KM: return "km";
   case NM: return "NM";
   case M: return "m";
   case YARDS: return "yards";
   case FEET: return "ft";
   }

   ASSERT(0);     // unknown / invalid units
   return "";
}

// Adds a string representation of the given units to the end of the list
void CUnitsComboBox::AddUnits(units_t eUnits)
{
   const int nIndex = AddString(CUnits(eUnits).ToString());
   SetItemData(nIndex, eUnits);
}

// Adds a string representation of the standard units to the end of the list
void CUnitsComboBox::AddStandardUnits()
{
   AddUnits(NM);
   AddUnits(KM);
   AddUnits(M);
   AddUnits(YARDS);
   AddUnits(FEET);
}

// Selects the given units in the list box of a combo box
void CUnitsComboBox::SetCurUnits(units_t eUnits)
{
   SelectString(-1, CUnits(eUnits).ToString());
}

// Returns the units of the currently selected item, if any, in the list box of a combo box
units_t CUnitsComboBox::GetCurUnits()
{
   return static_cast<units_t>(GetItemData(GetCurSel()));
};