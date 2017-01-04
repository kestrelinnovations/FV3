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

// NITFTBClass.h - Toolbars for NITF



#pragma once

#include "TBClass.h"
#include <map>

class CNITFTBClass : public COverlayEditorToolbar
{
protected:
   DECLARE_DYNCREATE( CNITFTBClass )
   
public:
   CNITFTBClass();

   virtual BOOL GetMessageString( UINT nID, CString& rMessage ) const;

   static INT GetToolbarButtonNum( UINT nID );   // Button number from resource ID

   // Lookup table to convert NITF Query toolbar button numbers to resource IDs
   static const struct ButtonLookup
   {
      INT   iButtonNum;
      UINT  uiButtonID;
   }                    s_blButtonLookup[];

private:

   typedef std::map< UINT, INT > ButtonNumMap;
   static ButtonNumMap s_mpButtonNums;

#if 0
   typedef std::map< QueryDlgToolbarButtonEnum, UINT > ButtonIDMap;
   static ButtonIDMap   s_mpButtonIDs;
#endif
};

// End of NITFTBClass