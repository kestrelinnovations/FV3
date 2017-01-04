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

// InternalToolbarEventSink.h
//

#ifndef FALCONVIEW_INCLUDE_INTERNALTOOLBAREVENTSINK_H_
#define FALCONVIEW_INCLUDE_INTERNALTOOLBAREVENTSINK_H_

#include "ToolbarEventSink.h"

// The InternalToolbarEventSink class can be used by internal (non-plugin)
// editors when using the IFvToolbar's connection point. The toolbar button
// pressed handler calls the frame's OnCmdMsg effectively matching the
// existing behavior without having to rewrite each internal editor to
// implement its own custom handler.
class InternalToolbarEventSink : public ToolbarEventSink
{
public:
   STDMETHOD(raw_OnButtonPressed)(long lButtonId);
};

#endif  // FALCONVIEW_INCLUDE_INTERNALTOOLBAREVENTSINK_H_