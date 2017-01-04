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

// ILayer3.cpp
//

#include "StdAfx.h"
#include "ILayer3.h"
#include "LayerOvl.h"
#include "getobjpr.h"   // for fvw_get_frame
#include "ovlElementContainer.h"

IMPLEMENT_DYNCREATE(Layer3, CCmdTarget)

IMPLEMENT_OLECREATE(Layer3, "FalconView.Layer3", 0x6AC55192, 0x31BC, 0x4d49, 
                    0x9A, 0x9F, 0x60, 0x67, 0xE1, 0x85, 0x90, 0x30)

Layer3::Layer3()
{
	EnableAutomation();
}

Layer3::~Layer3()
{
}

void Layer3::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Layer3, CCmdTarget)
	//{{AFX_MSG_MAP(Layer3)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Layer3, CCmdTarget)
	//{{AFX_DISPATCH_MAP(Layer3)
   DISP_FUNCTION(Layer3, "GetLayerEditors", GetLayerEditors, VT_I4, VTS_PVARIANT)
   DISP_FUNCTION(Layer3, "GetLayerEditor", GetLayerEditor, VT_I4, VTS_I4 VTS_PVARIANT)
   DISP_FUNCTION(Layer3, "ShowPlaybackToolbar", ShowPlaybackToolbar, VT_I4, VTS_BOOL)
   DISP_FUNCTION(Layer3, "ShowFileOverlayListToolbar", ShowFileOverlayListToolbar, VT_I4, VTS_BOOL)
   DISP_FUNCTION(Layer3, "AddPredefinedSymbol", AddPredefinedSymbol, VT_I4, VTS_I4 VTS_R8 VTS_R8 VTS_I4 VTS_R8 VTS_R8)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ILayer3 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

static const IID IID_ILayer3 =
{ 0x6B461D47, 0xE2A8, 0x49bb, { 0x8C, 0x9F, 0x1A, 0xBA, 0x2C, 0xBD, 0xAC, 0x15 } };

BEGIN_INTERFACE_MAP(Layer3, CCmdTarget)
	INTERFACE_PART(Layer3, IID_ILayer3, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ILayer3 method implementations

long Layer3::GetLayerEditors(VARIANT FAR* pLayerEditors)
{
   // Get the total number of Layer Editors
   //
   DWORD dwNumElements = 0;
   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      if (dynamic_cast<CLayerOvlFactory *>(OVL_get_type_descriptor_list()->m_pCurrent->pOverlayFactory) != NULL)
         dwNumElements++;
   }
   
   COleSafeArray saLayerEditors;
   saLayerEditors.Create(VT_DISPATCH, 1, &dwNumElements);

   IDispatch **ppDispatch;
   saLayerEditors.AccessData(reinterpret_cast<void **>(&ppDispatch));

   // Loop through layer editors and add IDispatch to safe array
   //
   long nCurrentCount = 0;
   OVL_get_type_descriptor_list()->ResetEnumerator();
   while (OVL_get_type_descriptor_list()->MoveNext())
   {
      CLayerOvlFactory *pLayerOvlFactory = dynamic_cast<CLayerOvlFactory *>(OVL_get_type_descriptor_list()->m_pCurrent->pOverlayFactory);
      if (pLayerOvlFactory != NULL)
      {
         LayerEditorImpl *pLayerEditorImpl = pLayerOvlFactory->get_ILayerEditor();
         IDispatch *pDispatch = pLayerEditorImpl->GetDispatchInterface();

         if (pDispatch != NULL)
         {
            ppDispatch[nCurrentCount] = pDispatch;
            nCurrentCount++;
         }
      }
   }

   saLayerEditors.UnaccessData();

   *pLayerEditors = saLayerEditors.Detach();

   return SUCCESS;
}

long Layer3::GetLayerEditor(long layer_handle, VARIANT FAR* layer_editor_variant)
{
   if (!layer_editor_variant)
   {
      return FAILURE;
   }

   layer_editor_variant->vt = VT_DISPATCH;
   layer_editor_variant->pdispVal = NULL;

   C_overlay *overlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   if (!overlay)
   {
      return FAILURE;
   }

   CBaseLayerOvl *layer_overlay = dynamic_cast<CBaseLayerOvl *>(overlay);
   if (layer_overlay != NULL)
   {
      LayerEditorImpl *layer_editor_impl = layer_overlay->get_ILayerEditor();
      if (layer_editor_impl)
      {
         IDispatch *dispatch = layer_editor_impl->GetDispatchInterface();
         if (dispatch != NULL)
         {
	         layer_editor_variant->pdispVal = dispatch;
         }
      }
   }

   return SUCCESS;
}

long Layer3::ShowPlaybackToolbar(BOOL show)
{
   // No longer necessary with ribbon
   return SUCCESS;
}

long Layer3::ShowFileOverlayListToolbar(BOOL show)
{
   // No longer necessary with ribbon
   return SUCCESS;
}

long Layer3::AddPredefinedSymbol(long layer_handle, double lat, double lon,
   long symbolType, double scale_factor, double rotation) 
{
   // check for valid lat-lon
   if (!GEO_valid_degrees(lat, lon))
   {
      ERR_report("Invalid lat/lon in ILayer3::AddPredefinedSymbol");
      return FAILURE;
   }

   // get a pointer to the overlay associated with the given handle
	C_overlay *pOverlay = OVL_get_overlay_manager()->lookup_overlay(layer_handle);
   
   // make sure we get a valid pointer
   if (pOverlay == NULL)
      return FAILURE;

   // make sure this is a CBaseLayerOvl pointer
   if (dynamic_cast<CBaseLayerOvl *>(pOverlay) == NULL)
      return FAILURE;

   return pOverlay->GetOvlElementContainer()->add_predefined_symbol(lat, lon, symbolType,
      scale_factor, rotation);
}
