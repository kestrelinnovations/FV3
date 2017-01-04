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

// IFavorite.cpp
//

#include "StdAfx.h"
#include "IFavorite.h"
#include "err.h"        // for ERR_report
#include "utils.h"
#include "fvwutil.h"
#include "favorite.h"
#include "mainfrm.h"

IMPLEMENT_DYNCREATE(Favorite, CCmdTarget)

IMPLEMENT_OLECREATE(Favorite, "FalconView.Favorite", 0x6F540426, 0xE251, 0x413c, 
                   0x88, 0xB7, 0x19, 0x3B, 0x0C, 0x40, 0xEA, 0x4F)

Favorite::Favorite()
{
	EnableAutomation();
}

Favorite::~Favorite()
{
}

void Favorite::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(Favorite, CCmdTarget)
	//{{AFX_MSG_MAP(Favorite)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Favorite, CCmdTarget)
	//{{AFX_DISPATCH_MAP(Favorite)
   DISP_FUNCTION(Favorite, "OpenFavorite",OpenFavorite, VT_I4, VTS_BSTR)
   DISP_FUNCTION(Favorite, "GetNumFavorites",GetNumFavorites, VT_I4, VTS_PI4)
   DISP_FUNCTION(Favorite, "GetFavoriteName",GetFavoriteName, VT_I4, VTS_I4 VTS_PBSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFavorite to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_IFavorite =
{ 0x50201718, 0x38EC, 0x421e, { 0x90, 0xFB, 0xF6, 0x2D, 0x1F, 0xB5, 0x98, 0x52 } };

BEGIN_INTERFACE_MAP(Favorite, CCmdTarget)
	INTERFACE_PART(Favorite, IID_IFavorite, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Favorite message handlers

long Favorite::OpenFavorite(LPCTSTR favorite_name)
{
   FavoriteList *fav_list = FavoriteList::get_instance();
   
   // obtain an index to the favorite with the given name
   int index = fav_list->Find(favorite_name);
   if (index != -1)
   {
      CMainFrame *main_frm = (CMainFrame *)UTL_get_frame();
      if (main_frm == NULL)
      {
         ERR_report("Unable to get pointer to CMainFrame.  Favorite not opened");
         return FAILURE;
      }

      main_frm->goto_favorite(index);
      
      return SUCCESS;
   }

   return FAILURE;
}

long Favorite::GetNumFavorites(long *num_favorites)
{
   FavoriteList *fav_list = FavoriteList::get_instance();
   *num_favorites = fav_list->get_count();

   return SUCCESS;
}

long Favorite::GetFavoriteName(int favorite_num, BSTR *favorite_name)
{
   FavoriteList *fav_list = FavoriteList::get_instance();
   CFvwUtil *futil = CFvwUtil::get_instance();

   *favorite_name = _bstr_t(fav_list->get_name(favorite_num)).Detach();

   return SUCCESS;
}
