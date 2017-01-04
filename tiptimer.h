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



#ifndef __TIP_TIMER_H_
#define __TIP_TIMER_H_ 1


#include "utils.h"

class CBaseView;
class MapView;

//--------------------- Tooltip Timer -----------------------
class TipTimer: public FVW_Timer
{
   DECLARE_DYNAMIC(TipTimer)

private:
   CBaseView* m_view;

   // requested tooltip text (not necessarily displayed yet)
   static CString m_tooltipText;

   // text of the tooltip that is currently display
   static CString m_crntTooltipText;

   static void *m_object;

public:
   // constructor
   TipTimer(CBaseView* view, int period) : 
      FVW_Timer(period) { m_view = view;}

   // overide of base class function
   virtual void expired();

   // added to check if the timer has stopped
   virtual boolean_t stopped() { return m_stopped; }

   // get the text in the last tool tip
   static CString &get_text() { return m_crntTooltipText; }

   // get the object associated with the last tool tip, the object is
   // usually a instance of a C_icon or C_overlay derived class
   static void *get_object() { return m_object; }

   static void set_text(const CString& tooltipText) { m_tooltipText = tooltipText; }
};

//--------------------- Auto smooth pan timer -----------------------
class SmoothPanTimer : public FVW_Timer
{
   DECLARE_DYNAMIC(SmoothPanTimer)

private:
   MapView* m_view;

public:
   SmoothPanTimer(MapView* view, int period) : FVW_Timer(period) 
   { 
      m_view = view; 
      m_force_smooth_pan = FALSE;
   }

   // override of base class function
   virtual void expired();

   int m_force_smooth_pan;
};


#endif  /* __TIP_TIMER_H_ */
