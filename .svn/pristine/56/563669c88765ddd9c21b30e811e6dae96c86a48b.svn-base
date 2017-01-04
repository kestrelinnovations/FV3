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



#include "stdafx.h"


// This routine processes any pending Windows messages in the queue.  This
// allows us to check for things such as the user pressing the escape key.
// To use this, just call it once in each iteration of your long processing
// routine.  The virtKey parameter is the virtual keycode of an abort key.
// Specifying 0 disables this feature (i.e. no keypresses will cause the
// function to return nonzero).
//
// e.g.
// 
// ...
// for(interminable_time)
// {
//    ... // do processing
//    int retVal = UTL_MessagePump(VK_ESC);
//    if (retVal || user_cancel)    // user_cancel is set by a message handler
//       abort;
// }
// ...
//
// Returns:
//    2  when the user quits the program
//    1  when the user presses the specified virtual key
//    0  otherwise
//
// Blatantly mooched and modified from Prosise's _Programming Windows 95_.
//
int UTL_MessagePump(int virtKey)
{
   MSG msg;
   
   //AfxGetMainWnd()->m_hWnd NULL
   
   while(::PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, 0, 0, PM_NOREMOVE))
   {
      // block the keystroke we're looking for
      if ((virtKey) && (msg.message == WM_KEYUP) &&
         (virtKey == (int) msg.wParam))
      {
         // pop this keystroke out of the queue
         ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
         return 1;
      }

      // everything else gets through, pumped manually
      else if(!AfxGetApp()->PumpMessage())
      {
         ::PostQuitMessage(0);
         return 2;
      }
   }


/* Cut out by Barnes on 2/97.  Calling OnIdle() destroys temporary
   GDI objects, and so if it happens while a pen exists in a subroutine,
   then CPen objects get leaked.  Therefore this should not be done here.

   // allow idle processing (very necessary for debug versions especially)
   LONG lIdle = 0;
   while (AfxGetApp()->OnIdle(lIdle++));
*/

   return 0;
}
