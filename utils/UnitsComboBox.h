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

// UnitsComboBox.h
//
#pragma once

class CDistanceEdit;

#include "geo_tool\common.h"  // for units_t

// Simple class that wraps the units enumeration and provides a formatter
class CUnits
{
   units_t m_eUnits;
public:
   CUnits(units_t eUnits) { m_eUnits = eUnits; }
   CString ToString();
};

// ComboBox that contains a list of values of the units_t enumeration.  Initialize the combobox with required
// units using the AddUnits method
class CUnitsComboBox : public CComboBox
{
public:
   // Adds a string representation of the given units to the end of the list
   void AddUnits(units_t eUnits);  

   // Adds a string representation of the standard units to the end of the list
   void AddStandardUnits();

   // Selects the given units in the list box of a combo box
   void SetCurUnits(units_t eUnits);

   // Returns the units of the currently selected item, if any, in the list box of a combo box
   units_t GetCurUnits();
};