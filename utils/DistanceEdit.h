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

// DistanceEdit.h
//
#pragma once

#include "..\..\fvw_core\geo_tool\common.h"  // for units_t

// edit control that displays a distance in the given units
class CDistanceEdit : public CEdit
{
   double m_dDistMeters;
   units_t m_eUnits;
public:
   CDistanceEdit();

   void SetDistance(double dDistMeters);
   void SetUnits(units_t eUnits);

   // based on the text in the edit control and the current units, return a distance in meters
   double GetDistanceMeters();

protected:
   void UpdateWindowText();
};