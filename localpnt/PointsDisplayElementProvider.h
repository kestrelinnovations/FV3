// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
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

// PointsDisplayElementProvider.h

#ifndef LOCALPNT_POINTSDISPLAYELEMENTPROVIDER_H_
#define LOCALPNT_POINTSDISPLAYELEMENTPROVIDER_H_

#include "FalconView/DisplayElementProvider_Interface.h"

class C_localpnt_point;
class C_overlay;

class PointsDisplayElementProvider :
   public DisplayElementProvider_Interface
{
public:
   virtual HRESULT Initialize(
      C_overlay* overlay,
      FalconViewOverlayLib::IDisplayElements* display_elements,
      FalconViewOverlayLib::IWorkItems* work_items);
   virtual ~PointsDisplayElementProvider() { }

private:
   void Add3DPoint(FalconViewOverlayLib::IDisplayElements* display_elements,
      C_localpnt_point* point);
};

#endif  // LOCALPNT_POINTSDISPLAYELEMENTPROVIDER_H_