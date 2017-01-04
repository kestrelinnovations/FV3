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

// PrntIcon.cpp

// stdafx first
#include "stdafx.h"

// this file's header
#include "FalconView/PrintToolOverlay/PrntIcon.h"   // PrintIcon

// system includes

// third party files

// other FalconView headers
#include "FalconView/include/PrntTool.h"   // CPrintToolOverlay

// this project's headers
#include "FalconView/PrintToolOverlay/factory.h"

// Construction
PrintIcon::PrintIcon(CPrintToolOverlay *parent) : C_icon(parent)
{
   ASSERT(m_overlay);
}

// Destruction
PrintIcon::~PrintIcon()
{
}

CPrintToolOverlay *PrintIcon::get_parent() const
{
   return dynamic_cast<CPrintToolOverlay *>(m_overlay);
}

// Returns TRUE if this icon is a instance of the given class.
boolean_t PrintIcon::is_kind_of(const char *class_name)
{
   if (strcmp(class_name, "PrintIcon") == 0)
      return TRUE;

   return C_icon::is_kind_of(class_name);
}
