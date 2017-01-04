// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
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

// OverlayStackChangedObserver_Interface.h

#ifndef FALCONVIEW_INCLUDE_OVERLAY_STACK_CHANGED_OBSERVER_INTERFACE_H_
#define FALCONVIEW_INCLUDE_OVERLAY_STACK_CHANGED_OBSERVER_INTERFACE_H_

class C_overlay;

// OverlayStackChangedObserver_Interface can be implemented to receive
// notifications when a overlay is added or removed from the overlay
// stack, or if the order of overlays in the stack has changed. Sign
// up for notifications using RegisterEvents. Use UnregisterEvents when
// you no longer need notifications.
class OverlayStackChangedObserver_Interface
{
public:
   virtual void OverlayAdded(C_overlay* overlay) = 0;
   virtual void OverlayRemoved(C_overlay* overlay) = 0;
   virtual void OverlayOrderChanged() = 0;
   virtual void OverlayFileSpecificationChanged(C_overlay* overlay) = 0;
   virtual void OverlayDirtyChanged(C_overlay* overlay) = 0;
   virtual ~OverlayStackChangedObserver_Interface() { }
};

#endif  // FALCONVIEW_INCLUDE_OVERLAY_STACK_CHANGED_OBSERVER_INTERFACE_H_