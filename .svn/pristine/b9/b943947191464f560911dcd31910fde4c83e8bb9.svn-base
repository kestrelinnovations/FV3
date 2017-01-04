// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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



// MovingMapFeed.cpp
//

#include "stdafx.h"
#include "MovingMapFeed.h"
#include "err.h"
#include "param.h"
#include "..\getobjpr.h"
#include "gps.h"
#include "..\PlaybackDialog\viewtime.h"
#include "ovl_mgr.h"

// constructor
MovingMapFeed::MovingMapFeed(C_gps_trail *parent) 
{
	m_parent = parent;
	m_is_connected = FALSE;

	m_created_feed_type = -1;

	// create the feed component
	m_pGPSFeed = NULL;
   m_pMovingMapFeed = NULL;
	if (CreateFeedComponent() != SUCCESS)
		ERR_report("Failed creating feed component");
}

// destructor
MovingMapFeed::~MovingMapFeed()
{
   if (m_pMovingMapFeed)
   {
      try
		{
			if (m_is_connected)
				m_pMovingMapFeed->Disconnect();

			m_pMovingMapFeed->Exit();
			m_pMovingMapFeed->Release();
		}
		catch(_com_error e)
		{
			ERR_report("Error calling gps feed component's Exit()");
		}

		m_pMovingMapFeed = NULL;
   }
	else if (m_pGPSFeed)
	{
		try
		{
			if (m_is_connected)
				m_pGPSFeed->Disconnect();

			m_pGPSFeed->Exit();
			m_pGPSFeed->Release();
		}
		catch(_com_error e)
		{
			ERR_report("Error calling gps feed component's Exit()");
		}

		m_pGPSFeed = NULL;
	}

	m_parent->stop_frozen_trail_warning(FALSE);
}

int MovingMapFeed::CreateFeedComponent()
{
	CLSID clsid_gps;
	CString msg;
	USES_CONVERSION;
	
	// try to get the class id of the GPS interface
	CString key;
	key.Format("GPS Feeds\\%d", get_feed_type());
	CString classIDString = PRM_get_registry_string(key,"classIDString", "");
	if (CLSIDFromString((LPOLESTR) T2COLE(classIDString),&clsid_gps) != S_OK)
	{
		msg.Format("Unable to get the moving map feed class ID from %s", classIDString);
		ERR_report(msg);
		return FAILURE;
	}

   // First, try to create it as a IMovingMapFeed 
   HRESULT hr = CoCreateInstance(clsid_gps, NULL, CLSCTX_ALL,
      fvw::IID_IMovingMapFeed, (void FAR* FAR*)&m_pMovingMapFeed);

   if (FAILED(hr))
   {
      // If the component doesn't implement the newer IMovingMapFeed, see if 
      // it implements IGPSFeed
      HRESULT hresult = CoCreateInstance(clsid_gps, NULL, CLSCTX_ALL, 
         fvw::IID_IGPSFeed, (void FAR* FAR*)&m_pGPSFeed);
      
      if (FAILED(hresult))
      {
         msg.Format("CoCreateInstance of %s failed with HRESULT = %X", classIDString, hresult);
         return FAILURE;
      }
   }

	m_created_feed_type = get_feed_type();

	return SUCCESS;
}

// called when the feed type has changed
void MovingMapFeed::on_feed_changed()
{
	// if the feed type hasn't changed, there is nothing to do
	if (m_created_feed_type == get_feed_type())
		return;

	try
	{
		// if we are connected, then disconnect
      if (m_pMovingMapFeed != NULL)
      {
         if (m_is_connected)
				disconnect();
			m_pMovingMapFeed->Exit();
			m_pMovingMapFeed->Release();
			m_pMovingMapFeed = NULL;
      }
		else if (m_pGPSFeed != NULL)
		{
			if (m_is_connected)
				disconnect();
			m_pGPSFeed->Exit();
			m_pGPSFeed->Release();
			m_pGPSFeed = NULL;
		}
	}
	catch(_com_error e)
	{
		ERR_report("Error disconnecting from old feed");
	}

	CreateFeedComponent();
}

// toggles between connect/disconnect state
int MovingMapFeed::ToggleConnection()
{
	return m_is_connected ? disconnect() : connect();
}

// display the options dialog for this feed
int MovingMapFeed::Options()
{
	if (m_pMovingMapFeed == NULL && m_pGPSFeed == NULL)
	{
		ERR_report("Moving Map Feed not created.  Cannot open options dialog");
		return FAILURE;
	}

	try
	{
		const long FV_window_handle = (long)AfxGetMainWnd()->GetSafeHwnd();

      if (m_pMovingMapFeed)
		   m_pMovingMapFeed->Options(FV_window_handle);
      else if (m_pGPSFeed)
         m_pGPSFeed->Options(FV_window_handle);
	}
	catch(_com_error e)
	{
		ERR_report("Error calling feed component's Options()");
		return FAILURE;
	}

	return SUCCESS;
}

int MovingMapFeed::SaveProperties(CString strFilename)
{
   if (m_pMovingMapFeed == NULL)
   {
      ERR_report("Must implement IMovingMapFeed to support properties");
      return FAILURE;
   }

   CFile fileSettings;
   if (!fileSettings.Open(strFilename, CFile::modeCreate | CFile::modeWrite))
   {
      ERR_report("Error opening .mms file for writing");
      return FAILURE;
   }

   try
   {
      _bstr_t cbstrSettings;
      HRESULT hr = m_pMovingMapFeed->GetProperties(cbstrSettings.GetAddress());
      if (FAILED(hr))
      {
         ERR_report("Failed retrieving properties");
         return FAILURE;
      }

      CString key;
	   key.Format("GPS Feeds\\%d", get_feed_type());
	   CString classIDString = PRM_get_registry_string(key,"classIDString", "");

      const int fgColor = m_parent->get_properties()->get_ship_fg_color();
      const int bgColor = m_parent->get_properties()->get_ship_bg_color();
      const CString label = m_parent->get_properties()->get_nickname_label();

      CString strSettings;
      strSettings.Format("%d,%d,%s<classIDString>%s</classIDString>", fgColor, bgColor, label, classIDString);

      strSettings += CString((char *)cbstrSettings);

      fileSettings.Write(strSettings.GetBuffer(strSettings.GetLength()),
         strSettings.GetLength());
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error calling moving map GetProperties : %s", (char *)e.Description());
      ERR_report(msg);
   }

   fileSettings.Close();

   return SUCCESS;
}

int MovingMapFeed::LoadProperties(CString strFilename, BOOL bAutoConnect)
{
   ASSERT(m_parent);

   CFile fileSettings;
   if (!fileSettings.Open(strFilename, CFile::modeRead))
   {
      ERR_report("Unable to open .mms for reading");
      return FAILURE;
   }

   CString strSettings;
   char c;
   while (fileSettings.Read(&c, 1))
      strSettings += c;

   // Extract the name of the classIDString from the settings
   //
   int start = strSettings.Find("<classIDString>");
   int end = strSettings.Find("</classIDString>");

   if (start == -1 || end == -1)
   {
      ERR_report("Unable to find classIDString in settings file");
      return FAILURE;
   }

   // newer .mms files will contain ship color and label
   CString shipProperties = strSettings.Left(start);
   if (shipProperties.GetLength() > 0)
   {
      int pos = shipProperties.Find(",");
      const int fgColor = atoi(shipProperties.Left(pos));

      int pos2 = shipProperties.Find(",", pos + 1);
      const int bgColor = atoi(shipProperties.Mid(pos + 1, pos2 - pos));

      CString label = shipProperties.Mid(pos2 + 1);

      m_parent->get_properties()->set_ship_fg_color(fgColor);
      m_parent->get_properties()->set_ship_bg_color(bgColor);
      m_parent->get_properties()->set_nickname_label(label);
   }

   start += CString("classIDString").GetLength() + 2; // eliminate start tag
   CString classIDString = strSettings.Mid(start, end - start);

   // strip the classIDString from the settings - this was added by FV not
   // the moving map feed
   strSettings = strSettings.Mid(end + CString("</classIDString>").GetLength());

   CGPSProperties *pGPSProperties = m_parent->get_properties();
	if (pGPSProperties == NULL)
	{
		ERR_report("Properties invalid");
		return FAILURE;
	}

   BOOL bFound = FALSE;
   int i = 0;
   CString tmpClassIDString;
   do
   {
      CString num_str;
      num_str.Format("\\%d", i);

      tmpClassIDString = PRM_get_registry_string("GPS Feeds" + num_str, "classIDString", "DNE");
      if (tmpClassIDString == classIDString)
      {
         pGPSProperties->set_feed_type(i);
         bFound = TRUE;
         break;
      }

      i++;

   } while (tmpClassIDString != "DNE");

   if (bFound == FALSE)
   {
      ERR_report("Unable to find matching moving map feed");
      return FAILURE;
   }

   if (m_is_connected)
      disconnect();

   on_feed_changed();

   if (m_pMovingMapFeed == NULL)
   {
      ERR_report("Must implement IMovingMapFeed to support properties");
      return FAILURE;
   }

   try
   {
      m_pMovingMapFeed->SetProperties(_bstr_t(strSettings));
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Error calling moving map SetProperties : %s", (char *)e.Description());
      ERR_report(msg);
   }

   if (bAutoConnect)
      connect();

   return SUCCESS;
}

// disconnect from the moving map feed
int MovingMapFeed::disconnect()
{
	// if we are not connected, there is nothing to do
	if (!m_is_connected)
		return SUCCESS;

	if (m_pGPSFeed == NULL && m_pMovingMapFeed == NULL)
	{
		ERR_report("Moving Map Feed not created.  Cannot disconnect");
		return FAILURE;
	}

	// notify any registered clients that we have disconnected
   POSITION position = C_gps_trail::m_hwnd_connect_notify_lst.GetHeadPosition();
   while (position)
   {
      HWND hWnd = C_gps_trail::m_hwnd_connect_notify_lst.GetNext(position);

      if (!::IsWindow(hWnd))
      {
         ERR_report("Invalid window handle in disconnect notification");
      }
      else if (!::PostMessage(hWnd, wm_Disconnect, 0, 
         (LPARAM)OVL_get_overlay_manager()->get_overlay_handle(m_parent)))
      {
         CString msg;
         msg.Format("PostMessage FVW_GPS_DISCONNECT failed for HWND=%d",hWnd);
         ERR_report(msg);
      }
   }

	try
	{
      if (m_pMovingMapFeed)
		   m_pMovingMapFeed->Disconnect();
      else if (m_pGPSFeed)
         m_pGPSFeed->Disconnect();
	}
	catch(_com_error e)
	{
		ERR_report("Error calling feed component's Disconnect()");
		return FAILURE;
	}

	// close the communications dialog
   m_parent->CloseCommDlg();

	m_is_connected = FALSE;
	return SUCCESS;
}

// connect to the moving map feed
int MovingMapFeed::connect()
{
	// if we are already connected, there is nothing to do
	if (m_is_connected)
		return SUCCESS;

	if (m_pGPSFeed == NULL && m_pMovingMapFeed == NULL)
	{
		ERR_report("GPS Feed not created.  Cannot connect");
		return FAILURE;
	}

	// notify any registered clients that a connection has been made
	// send a registered message to any HWND in the notification list
   POSITION position = C_gps_trail::m_hwnd_connect_notify_lst.GetHeadPosition();
   while (position)
   {
      HWND hWnd = C_gps_trail::m_hwnd_connect_notify_lst.GetNext(position);

      if (!::IsWindow(hWnd))
      {
         ERR_report("Invalid window handle in connect notification");
      }
      else if (!::PostMessage(hWnd, wm_Connect, 0, 
         (LPARAM)OVL_get_overlay_manager()->get_overlay_handle(m_parent)))
      {
         CString msg;
         msg.Format("PostMessage FVW_GPS_CONNECT failed for HWND=%d",hWnd);
         ERR_report(msg);
      }
   }

	C_gps_trail::m_connected_handle = OVL_get_overlay_manager()->get_overlay_handle(m_parent);
	
	try
	{
		const long overlay_handle = (long)(OVL_get_overlay_manager()->
			get_overlay_handle(m_parent));

      if (m_pMovingMapFeed)
		   m_pMovingMapFeed->Connect(overlay_handle);
      else if (m_pGPSFeed)
         m_pGPSFeed->Connect(overlay_handle);
	}
	catch(_com_error e)
	{
		ERR_report("Error calling feed component's Connect()");
		return FAILURE;
	}

	// automatically turn on the time display
	CMainFrame::GetPlaybackDialog().set_draw_time_on_map(TRUE);
	OVL_get_overlay_manager()->invalidate_all();
	
	// display the communication dialog
	communication_dlg();
	
	m_is_connected = TRUE;
	return SUCCESS;
}


// ^> not sure about where this should be yet.  I'm thinking that we
// should let the feed component fill in this dialog with whatever values
// it thinks appropriate
void MovingMapFeed::communication_dlg()
{
   const int EDIT_LEN = 256;
   char edit[EDIT_LEN];

	CString feed_name = "GPS Feed: " + get_feed_name();
	strcpy_s(edit, EDIT_LEN, feed_name);

	strcat_s(edit, EDIT_LEN, "\r\n");
	strcat_s(edit, EDIT_LEN, "\r\n");
   strcat_s(edit, EDIT_LEN, "Waiting for data..........");
   strcat_s(edit, EDIT_LEN, "\r\n");

   m_parent->show_communication(edit, TRUE);
}

int MovingMapFeed::get_feed_type()
{
	ASSERT(m_parent);
	
	CGPSProperties *prop = m_parent->get_properties();
	if (prop == NULL)
	{
		ERR_report("Properties invalid");
		return 0;
	}
	
	return prop->get_feed_type();
}

CString MovingMapFeed::get_feed_name()
{
	CString key;
	key.Format("GPS Feeds\\%d", get_feed_type());
	CString feed_name = PRM_get_registry_string(key,"FeedName", "");

	return feed_name;
}
