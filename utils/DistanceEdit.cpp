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

// DistanceEdit.cpp
//
#include "stdafx.h"
#include "DistanceEdit.h"
#include "unit.h"          // for CUnit

CDistanceEdit::CDistanceEdit() :
   m_dDistMeters(0.0),
   m_eUnits(M)
{
}

void CDistanceEdit::SetDistance(double dDistMeters)
{
   m_dDistMeters = dDistMeters;
   UpdateWindowText();
}

void CDistanceEdit::SetUnits(units_t eUnits)
{
   m_eUnits = eUnits;
   UpdateWindowText();
}

// based on the text in the edit control and the current units, return a distance in meters
double CDistanceEdit::GetDistanceMeters()
{
   // get the value from the edit control
   CString sText;
   GetWindowText(sText);
   const double v = atof(sText);

   // convert the value to meters
   CUnitConverter dist;
   switch(m_eUnits)
   {
   case KM: dist.SetKM(v); break;
   case NM: dist.SetNM(v); break;
   case M: dist.SetMeters(v); break;
   case YARDS: dist.SetYards(v); break;
   case FEET: dist.SetFeet(v); break;
   }
   return dist.GetMeters();
}

void CDistanceEdit::UpdateWindowText()
{
   // convert the internal distance in meters to the appropriate units
   CUnitConverter dist(m_dDistMeters);
   double v = 0.0;
   switch(m_eUnits)
   {
   case KM: v = dist.GetKM(); break;
   case NM: v = dist.GetNM(); break;
   case M: v = dist.GetMeters(); break;
   case YARDS: v = dist.GetYards(); break;
   case FEET: v = dist.GetFeet(); break;
   }

   if (v > INT_MAX)
      v = INT_MAX;
   else if (v < INT_MIN)
      v = INT_MIN;

   // round to the nearest integer and set the edit control's text
   CString sText;
   sText.Format("%d", static_cast<int>(v + 0.5));
   SetWindowText(sText);
}
