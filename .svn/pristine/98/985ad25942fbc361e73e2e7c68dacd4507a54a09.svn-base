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

// EarthManipulatorEvents_Interface.h

#ifndef FALCONVIEW_EARTH_MANIPULATOR_EVENTS_INTERFACE_H
#define FALCONVIEW_EARTH_MANIPULATOR_EVENTS_INTERFACE_H

// EarthManipulatorEvents_Interface can be implemented to receive
// notifications of various manipulator events. Sign up for notifications
// using AddListener. Use RemoveListener when you no longer need notifications.
class EarthManipulatorEvents_Interface
{
public:
   virtual void DistanceChanged(double distance_meters) = 0;

   virtual ~EarthManipulatorEvents_Interface() { }
};

#endif  // FALCONVIEW_EARTH_MANIPULATOR_EVENTS_INTERFACE_H