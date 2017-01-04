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

// NITFTBClass.cpp -  implementation of NITF toolbars



#include "stdafx.h"
#include <map>
#include "nitf.h"
#include "NITFTBClass.h"

IMPLEMENT_DYNCREATE( CNITFTBClass, COverlayEditorToolbar )

// Maps to convert button codes to resources IDs and vice versa
CNITFTBClass::ButtonNumMap CNITFTBClass::s_mpButtonNums;

// Lookup table to convert NITF Query toolbar button numbers to query button codes and resource IDs
const struct CNITFTBClass::ButtonLookup CNITFTBClass::s_blButtonLookup[] =
   {
      { QUERY_DLG_TOOLBAR_BUTTON_SELECT - QUERY_DLG_TOOLBAR_MIN_BUTTON,             ID_NITF_SELECT },
      { QUERY_DLG_TOOLBAR_BUTTON_AREA_SELECT - QUERY_DLG_TOOLBAR_MIN_BUTTON,        ID_NITF_AREA_SELECT },
      { QUERY_DLG_TOOLBAR_BUTTON_ERASE_ALL_ROIS - QUERY_DLG_TOOLBAR_MIN_BUTTON,     ID_NITF_ERASE_AREAS },
      { QUERY_DLG_TOOLBAR_BUTTON_SHOW_QUERY_EDITOR - QUERY_DLG_TOOLBAR_MIN_BUTTON,  ID_NITF_QUERY_EDITOR },
      { QUERY_DLG_TOOLBAR_BUTTON_SHOW_METADATA_LIST - QUERY_DLG_TOOLBAR_MIN_BUTTON, ID_NITF_METADATA_LIST },
#if 0
      { QUERY_DLG_TOOLBAR_BUTTON_PRESET_1 - QUERY_DLG_TOOLBAR_MIN_BUTTON,           ID_NITF_OPEN_CONFIG_1 },
      { QUERY_DLG_TOOLBAR_BUTTON_PRESET_2 - QUERY_DLG_TOOLBAR_MIN_BUTTON,           ID_NITF_OPEN_CONFIG_2 },
      { QUERY_DLG_TOOLBAR_BUTTON_PRESET_3 - QUERY_DLG_TOOLBAR_MIN_BUTTON,           ID_NITF_OPEN_CONFIG_3 },
      { QUERY_DLG_TOOLBAR_BUTTON_PRESET_4 - QUERY_DLG_TOOLBAR_MIN_BUTTON,           ID_NITF_OPEN_CONFIG_4 },
#endif
      { QUERY_DLG_TOOLBAR_BUTTON_LOCAL_SEARCH - QUERY_DLG_TOOLBAR_MIN_BUTTON,       ID_NITF_LOCAL_SEARCH },
#if 0
      { QUERY_DLG_TOOLBAR_BUTTON_NETWORK_SEARCH - QUERY_DLG_TOOLBAR_MIN_BUTTON,     ID_NITF_NETWORK_SEARCH },
#endif
      { QUERY_DLG_TOOLBAR_BUTTON_WEB_SEARCH - QUERY_DLG_TOOLBAR_MIN_BUTTON,         ID_NITF_WEB_SEARCH },
      { -1 }  // Stopper
   };

//
// CNITFTBClass() - constructor
//
CNITFTBClass::CNITFTBClass()
{
   if ( s_mpButtonNums.empty() )
   {
      const ButtonLookup* pbl = s_blButtonLookup;
      do
      {
         s_mpButtonNums.insert(
            ButtonNumMap::value_type( pbl->uiButtonID, pbl->iButtonNum ) );
      } while ( (++pbl)->iButtonNum != -1 );
   }
}


//
// GetToolbarButtonNum()
//
INT CNITFTBClass::GetToolbarButtonNum( UINT nID )
{
   ButtonNumMap::iterator it;
   return ( ( it = s_mpButtonNums.find( nID ) ) != s_mpButtonNums.end() )
      ? it->second : -1;   // -1 if button not found
}


//
// GetMessageString()
//
BOOL CNITFTBClass::GetMessageString( UINT nID, CString& rMessage ) const
{
   do
   {
      INT iButtonNum;
      if ( ( iButtonNum = GetToolbarButtonNum( nID ) ) < 0 )
         break;               // Not one of our IDs

      if ( !C_nitf_ovl::s_pNITFOvl->m_csMessageStrings[ iButtonNum ].IsEmpty() )
      {
         rMessage = C_nitf_ovl::s_pNITFOvl->m_csMessageStrings[ iButtonNum ];
         return TRUE;
      }
   } while ( FALSE );
   return FALSE;        // No replacement string
}

// End of NITFTBClass.cpp