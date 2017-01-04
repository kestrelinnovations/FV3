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

// OverlayInvalidatedListener_Interface.h
//

#ifndef FALCONVIEW_OVERLAYINVALIDATEDLISTENER_H_
#define FALCONVIEW_OVERLAYINVALIDATEDLISTENER_H_

// system includes
// third party files
// other FalconView headers
// this project's headers
// forward definitions
class MapType;

class OverlayInvalidatedListener_Interface
{
public:
   virtual void Invalidate() = 0;
   virtual ~OverlayInvalidatedListener_Interface()
   {
   }
};
#endif  // FALCONVIEW_OVERLAYINVALIDATEDLISTENER_H_