// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvSecurityLabelPluginImpl.h : Base class implementation of IFvSecurityLabelPlugin

#include "StdAfx.h"
#include "SecurityLabelExtensionCOM.h"
#include "err.h"


   void CSecurityLabelExtensionCOM::Initialize()
   {
   // Don't need to do anything for open source version
#if GOV_RELEASE
      try
      {
         // Create the custom security label extension manager
         m_spSecurityLabelExtension.CreateInstance(CLSID_FvSecurityLabelExtension);

         if ( m_spSecurityLabelExtension != 0 )
            m_spSecurityLabelExtension->Initialize();

         if ( m_spSecurityLabelExtension != 0 && m_spSecurityLabelExtension->HasPlugin() )
            m_spSecurityLabelPlugin = m_spSecurityLabelExtension;
         else
            m_spSecurityLabelPlugin = 0;
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
   };

   bool CSecurityLabelExtensionCOM::UseBannerPage()
   {
#if GOV_RELEASE
      try
      {
         return m_spSecurityLabelExtension != 0 && m_spSecurityLabelExtension->HasPlugin() && 
            m_spSecurityLabelPlugin->UseBannerPage;
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      // Fall through in GOV_RELEASE
      return false;
   };

   void CSecurityLabelExtensionCOM::OnCustomSecurityButton(long hWnd)
   {
#if GOV_RELEASE
      try
      {
         if ( m_spSecurityLabelExtension != 0 && m_spSecurityLabelExtension->HasPlugin() )
            m_spSecurityLabelPlugin->OnCustomButton(hWnd);
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return;
   };

   void CSecurityLabelExtensionCOM::PrintBannerPage(long hDC)
   {
#if GOV_RELEASE
      try
      {
         if ( UseBannerPage() )
            m_spSecurityLabelPlugin->PrintBannerPage(hDC);
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return;
   };

   bool CSecurityLabelExtensionCOM::PrintSecurityLabel()
   {
#if GOV_RELEASE
      try
      {
         return m_spSecurityLabelExtension != 0 && m_spSecurityLabelExtension->HasPlugin();
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return false;
   };

   CString CSecurityLabelExtensionCOM::SecurityLabel()
   {
#if GOV_RELEASE
      try
      {
         return CString((LPCSTR)m_spSecurityLabelPlugin->SecurityLabel);
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return CString("");
   };

   CString CSecurityLabelExtensionCOM::CustomSecurityButtonLabel()
   {
#if GOV_RELEASE
      try
      {
         return CString((LPCSTR)m_spSecurityLabelPlugin->GetCustomButtonText());
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return CString("");
   };

   bool CSecurityLabelExtensionCOM::UseTitleBarDisplay()
   {
#if GOV_RELEASE
      try
      {
         return m_spSecurityLabelExtension != 0 && m_spSecurityLabelExtension->HasPlugin();
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return false;
   };


   CString CSecurityLabelExtensionCOM::TitlebarSecurityText()
   {
#if GOV_RELEASE
      try
      {
         return CString((LPCSTR)m_spSecurityLabelPlugin->GetTitlebarSecurityText());
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return CString("");
   };

   bool CSecurityLabelExtensionCOM::UseCustomSecurityButton()
   {
#if GOV_RELEASE
      try
      {
         return m_spSecurityLabelExtension != 0 && m_spSecurityLabelExtension->HasPlugin() && 
            m_spSecurityLabelPlugin->UseTitlebarDisplay;
      }
      catch( _com_error& e )
      {
         REPORT_COM_ERROR(e);
      }
#endif
      return false;
   };






