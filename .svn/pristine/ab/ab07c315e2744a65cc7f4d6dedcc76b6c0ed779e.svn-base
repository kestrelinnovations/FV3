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



/*
   Filename :  unit.h
   Date     :  1998-SEP-15
   Author   :  Ray E. Bornert II
   Purpose  :  universal unit converter
*/
#pragma once

#define METERS_PER_INCH     ( 0.0254 )
#define METERS_PER_FOOT     ( 0.3048 )
#define METERS_PER_YARD     ( 0.9144 )
#define METERS_PER_METER    ( 1.0 )
#define METERS_PER_MILE     ( 1609.344 )
#define METERS_PER_NAUT     ( 1852.0 )           // International unit 
#define METERS_PER_KM       ( 1000.0 )

class CUnitConverter
{
private:
   double m_dMeters;

public:
   CUnitConverter(double dMeters = 0.0) { m_dMeters = dMeters; }

   operator double() const;

   void SetInches(double v)     { m_dMeters = v * METERS_PER_INCH; }
   void SetFeet(double v)       { m_dMeters = v * METERS_PER_FOOT; }
   void SetYards(double v)      { m_dMeters = v * METERS_PER_YARD; }
   void SetMeters(double v)     { m_dMeters = v; }
   void SetMiles(double v)      { m_dMeters = v * METERS_PER_MILE; }
   void SetNM(double v)         { m_dMeters = v * METERS_PER_NAUT; }
   void SetKM(double v)         { m_dMeters = v * METERS_PER_KM; }

   double GetInches()     { return m_dMeters / METERS_PER_INCH; }
   double GetFeet()       { return m_dMeters / METERS_PER_FOOT; }
   double GetYards()      { return m_dMeters / METERS_PER_YARD; }
   double GetMeters()     { return m_dMeters; }
   double GetMiles()      { return m_dMeters / METERS_PER_MILE; }
   double GetNM()         { return m_dMeters / METERS_PER_NAUT; }
   double GetKM()         { return m_dMeters / METERS_PER_KM; }
};