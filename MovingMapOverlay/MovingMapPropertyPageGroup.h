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

#pragma once

#include "optndlg.h"    // for CPropertyPageGroup

class CGPSProperties;

class CMovingMapRootPropertyPage : public CFvOverlayPropertyPageImpl
{
   static CGPSProperties* s_pProperties;

public:
   CMovingMapRootPropertyPage();
   ~CMovingMapRootPropertyPage();

   static CGPSProperties* GetProperties() { return s_pProperties; }

   virtual HRESULT OnApply();

protected:
   virtual CPropertyPage *CreatePropertyPage() { return NULL; }
};