// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

// FvSecurityLabelPluginImpl.h : Base class implementation of IFvSecurityLabelPlugin

#pragma once


// This class exists to wrap the IFvSecurityLabelExtensionPtr, IFvSecurityLabelPluginPtr
// pointers below.  mapview.h declares a member pointer to this wrapper class.  There 
// are a host of files that #include mapview.h.  We don't want them to all know about the
// above-mentioned smart pointers (which requires that the #import FVSecurityLabelMgr.tlb.)
// mapview can have a pointer to this wrapper class without including this header file, 
// which only needs to be included in mapview.cpp.
class CSecurityLabelExtensionCOM
{
public:
   void Initialize();
   bool UseBannerPage();
   void OnCustomSecurityButton(long hWnd);
   void PrintBannerPage(long hDC);
   bool PrintSecurityLabel();
   CString SecurityLabel();
   CString CustomSecurityButtonLabel();
   bool UseCustomSecurityButton();
   bool UseTitleBarDisplay();
   CString TitlebarSecurityText();


   // Security extensions permit customizable security labeling on 
   // printed materials.  In particular, JMPS uses its own security 
   // downgrade mechanism.
private:
#if GOV_RELEASE
   IFvSecurityLabelExtensionPtr  m_spSecurityLabelExtension;
   IFvSecurityLabelPluginPtr     m_spSecurityLabelPlugin;
#endif

};