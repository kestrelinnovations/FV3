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
#include "skyview.h"
#include "err.h"
#include "mov_sym.h"
#include "utils.h"
#include "map.h"

SkyViewInterface::SkyViewInterface()
{
   try
   {
      CO_CREATE(m_spSkyViewConnection, CLSID_SkyViewConnection);

      CComObject<CSkyViewConnectionSymbolEvents>::CreateInstance(&m_pConnectionSymbolEvents);
      m_pConnectionSymbolEvents->AddRef();
      m_pConnectionSymbolEvents->RegisterNotifyEvents(true, m_spSkyViewConnection);

   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Initialization of SkyView interface object failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

SkyViewInterface::~SkyViewInterface()
{
   try
   {
      if (m_pConnectionSymbolEvents != NULL)
      {
         m_pConnectionSymbolEvents->RegisterNotifyEvents(false, m_spSkyViewConnection);
         m_pConnectionSymbolEvents->Release();
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Termination of SkyView interface object failed - %s", (char *)e.Description());
      ERR_report(msg);
   }
}

int SkyViewInterface::init()
{
	try
	{
      if (m_spSkyViewConnection != NULL)
      {
         m_spSkyViewConnection->InitializeConnection();
         m_skyview = m_spSkyViewConnection->GetSkyViewConnection();
      }
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return SUCCESS;
}

void SkyViewInterface::uninit()
{
	try
	{
      if (m_spSkyViewConnection != NULL)
         m_spSkyViewConnection->UninitializeConnection();
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

BOOL SkyViewInterface::can_rotate_symbol()
{
   try
	{
      if (m_spSkyViewConnection != NULL)
         return m_spSkyViewConnection->CanRotateSymbol;
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}

   return FALSE;
}

void SkyViewInterface::set_can_rotate_symbol(BOOL bCanRotate)
{
   try
	{
      if (m_spSkyViewConnection != NULL)
         m_spSkyViewConnection->CanRotateSymbol = bCanRotate;
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

BOOL SkyViewInterface::can_move_symbol()
{
   try
	{
      if (m_spSkyViewConnection != NULL)
         return m_spSkyViewConnection->CanMoveSymbol;
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}

   return FALSE;
}

void SkyViewInterface::set_can_move_symbol(BOOL bCanMoveSymbol)
{
   try
	{
      if (m_spSkyViewConnection != NULL)
         m_spSkyViewConnection->CanMoveSymbol = bCanMoveSymbol;
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

BOOL SkyViewInterface::camera_attached_to_symbol()
{
   try
	{
      if (m_spSkyViewConnection != NULL)
         return m_spSkyViewConnection->CameraAttachedToSymbol;
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}

   return FALSE;
}

void SkyViewInterface::attach_camera_to_object(long nSkyViewObjectHandle, ISkyViewConnectionCameraEvents *pCameraEvents)
{
   try
   {
      if (m_spSkyViewConnection != NULL)
         m_spSkyViewConnection->AttachCameraToObject(nSkyViewObjectHandle, pCameraEvents);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

void SkyViewInterface::detach_camera_from_object(long nSkyViewObjectHandle)
{
   try
   {
      if (m_spSkyViewConnection != NULL)
         m_spSkyViewConnection->DetachCameraFromObject(nSkyViewObjectHandle);
   }
   catch(_com_error &e)
   {
      REPORT_COM_ERROR(e);
   }
}

// ISkyView method wrappers

long SkyViewInterface::Add3DObject(CString FileName)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->Add3DObject(_bstr_t(FileName));
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::Add3DShape(short Shape)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->Add3DShape(Shape);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::SetPosition(long Handle, double Latitude, double Longitude, float Alt_MSL)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->SetPosition(Handle, Latitude, Longitude, Alt_MSL);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::SetOrientation(long Handle, float Heading, float Pitch, float Roll)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->SetOrientation(Handle, Heading, Pitch, Roll);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::DeleteObject(long Handle)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->DeleteObject(Handle);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::Scale3DObject(long Handle, float Scale)
{
   long ret = FAILURE;
   
   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->Scale3DObject(Handle, Scale);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}
   
   return ret;
}

long SkyViewInterface::GetCameraHandle()
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->GetCameraHandle();
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}
	
long SkyViewInterface::SetPositionMeters(long Handle, float X, float Y, float Z)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->SetPositionMeters(Handle, X, Y, Z);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::AttachObject(long Handle, long ParentHandle)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->AttachObject(Handle, ParentHandle);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::AddStringObject(CString Text)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->AddStringObject(_bstr_t(Text));
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::SetStringText(long Handle, CString Text)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->SetStringText(Handle, _bstr_t(Text));
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

float SkyViewInterface::SetFOVAngle(short Select, float AngleInDeg)
{
   float ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->SetFOVAngle(Select, AngleInDeg);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

short SkyViewInterface::SelectFOV(short NewSelect)
{
   short ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->SelectFOV(NewSelect);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::LoadMap(double Latitude, double Longitude, long MapScale, short LoadNextAvailable)
{
   long ret = FAILURE;

   try
   {
      if (m_skyview != NULL)
         ret = m_skyview->LoadMap(Latitude, Longitude, MapScale, LoadNextAvailable);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::Size3DObject(long Handle, float length, float width, float height)
{
   long ret = FAILURE;
   float curl,curw,curh;
   try
   {
      if (m_skyview != NULL)
      {
         ret = GetObjectSize(Handle,&curl,&curw,&curh);
         if (ret == 0)
         {
            ret = m_skyview->Strech3DObject(Handle, width/curw, length/curl, height/curh);
         }
      }
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::SetColor(long Handle, float Red, float Green, float Blue)
{
   long ret = FAILURE;

   try
   {  
      if (m_skyview != NULL)
         ret = m_skyview->SetColor(Handle, Red, Green, Blue);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;
}

long SkyViewInterface::SetDisplay(long Handle, short is_solid_not_wireframe)
{
	long ret = FAILURE;
	
   try
   {  
      if (m_skyview != NULL)
         ret = m_skyview->SetDisplay(Handle, is_solid_not_wireframe);
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}
	
   return ret;
}

long SkyViewInterface::GetObjectSize(long Handle, float *length, float *width, float *height)
{
   long ret = FAILURE;

   try
   {  
      if (m_skyview != NULL)
      {
	      // recall that object are supposed to look down the y axis with the x axid out there right side
         ret = m_skyview->GetObjectSize(Handle, width, length, height);
      }
   }
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
      return FAILURE;
	}

   return ret;

}

// ISkyView2 method wrappers

void SkyViewInterface::AddCOM3DObject(CString progid, ISkyView3DObjectPtr &object, long &object_handle)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
		   m_skyview2->AddCOM3DObject(_bstr_t(progid), &object, &object_handle);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void SkyViewInterface::CreateAndAssociateCOMMotionPlayback(
   CString progid, 
   long object_handle, 
   IMotionPlaybackPtr &playback)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
		   playback = m_skyview2->CreateAndAssociateCOMMotionPlayback(_bstr_t(progid), object_handle);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void SkyViewInterface::UnassociateAndReleaseCOMMotionPlayback(long object_handle)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
		   m_skyview2->UnassociateAndReleaseCOMMotionPlayback(object_handle);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void SkyViewInterface::EnablePlayback(bool enable)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
         m_skyview2->EnablePlayback(enable ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void SkyViewInterface::SetPlaybackTime(DATE time)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
         m_skyview2->SetPlaybackTime(time);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void SkyViewInterface::SetPlaybackTimeAdvancementMultiplier(double multiplier)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
         m_skyview2->SetPlaybackTimeAdvancementMultiplier(multiplier);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

void SkyViewInterface::EnablePlaybackTimeAdvancement(bool enable)
{
	try
	{
      if (m_skyview2 == NULL)
         m_skyview2 = m_skyview;

      if (m_skyview2 != NULL)
         m_skyview2->EnablePlaybackTimeAdvancement(enable ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch (_com_error &e)
	{
      REPORT_COM_ERROR(e);
	}
}

// CSkyViewConnectionEvents
//

STDMETHODIMP CSkyViewConnectionSymbolEvents::raw_OnSetSymbolPosition(double dLat, double dLon, double dAltitudeMeters, double dTrueCourse)
{
   CView *pView;
   ViewMapProj *map;

   CRect start,end,inv;
   Csym_icon *sym=NULL;

   sym = Cmov_sym_overlay::GetStateIndicators()->m_symbol;
   if (!sym) 
      return FAILURE;

   if (Cmov_sym_overlay::m_in_drag && (sym == Cmov_sym_overlay::m_pviewpoint))
   { // if we are currently dragging the symbol, ignore the move message
      return SUCCESS; // CAB should we return success failure or somthing else
   }

   sym->set_msl((float)METERS_TO_FEET(dAltitudeMeters)); // no redraw needed here so just change it
 
   if ((sym->get_latitude() == dLat) && // these require a redraw so make sure something really changed
      (sym->get_longitude() == dLon) &&
      (sym->get_heading() == dTrueCourse))
      return S_OK;

   sym->set_location(dLat,dLon);
   sym->set_heading(dTrueCourse);
   
   start = sym->get_rect();
   pView = UTL_get_active_non_printing_view();
   if (pView == NULL)
   { 
      // we are print previewing so we don't need to invalidate anything
      return S_OK;
   }

   map = UTL_get_current_view_map(pView);
   if (!map)
   {
      ERR_report("Failure getting current map projection.");
      return S_OK; // since there is no map proj we just won't invalidate
   }

   // invalidate the old position of the symbol before the move
	sym->invalidate();

	// apply new lat lon now done in draw
   sym->set_location(dLat,dLon);
   sym->update_position(map); 

	// invalidate where the symbol is now
   sym->invalidate();

   Cmov_sym_overlay::send_drop_to_all_windows(sym);

   return S_OK;
}

STDMETHODIMP CSkyViewConnectionSymbolEvents::raw_OnUnselectSymbol()
{
   if (Cmov_sym_overlay::m_pviewpoint != NULL)
   {
      Cmov_sym_overlay::m_pviewpoint->unselect();
      Cmov_sym_overlay::m_pviewpoint = NULL;
   }

   return S_OK;
}