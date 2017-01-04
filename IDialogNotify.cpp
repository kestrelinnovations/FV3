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

// IDialogNotify.cpp : implementation file
//

#include "stdafx.h"
#include "IDialogNotify.h"

#import "fvw.tlb" named_guids

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>  // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>  // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

DialogNotify* DialogNotify::m_pDlg = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DialogNotify

IMPLEMENT_DYNCREATE(DialogNotify, CCmdTarget)

DialogNotify::DialogNotify()
{
   EnableAutomation();

   m_pDlg = this;;
   m_DlgIDs[0] = 1;
}

DialogNotify::~DialogNotify()
{
}

void DialogNotify::OnFinalRelease()
{
   // When the last reference for an automation object is released
   // OnFinalRelease is called.  The base class will automatically
   // deletes the object.  Add additional cleanup required for your
   // object before calling the base class.

   CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(DialogNotify, CCmdTarget)
   //{{AFX_MSG_MAP(DialogNotify)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(DialogNotify, CCmdTarget)
   //{{AFX_DISPATCH_MAP(DialogNotify)
   DISP_FUNCTION(DialogNotify, "RegisterForCallbacks", RegisterForCallbacks, VT_I4, VTS_VARIANT)
   DISP_FUNCTION(DialogNotify, "UnregisterForCallbacks", UnregisterForCallbacks, VT_I4, VTS_VARIANT)
   //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDIalogNotify to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {A6ADB64A-694D-460D-90C0-310BCAB0766F}
static const IID IID_IDialogNotify =
{ 0xa6adb64a, 0x694d, 0x460d, { 0x90, 0xc0, 0x31, 0xb, 0xca, 0xb0, 0x76, 0x6f } };

BEGIN_INTERFACE_MAP(DialogNotify, CCmdTarget)
   INTERFACE_PART(DialogNotify, IID_IDialogNotify, Dispatch)
END_INTERFACE_MAP()

// {240CCEB6-1AAA-401D-B3DE-DCF97C1A7AD7}
IMPLEMENT_OLECREATE(DialogNotify, "FalconView.DialogNotify", 0x240cceb6, 0x1aaa, 0x401d, 
                0xb3, 0xde, 0xdc, 0xf9, 0x7c, 0x1a, 0x7a, 0xd7)

/////////////////////////////////////////////////////////////////////////////
// DialogNotify message handlers
//-----------------------------------------------------------------------------
long DialogNotify::RegisterForCallbacks(VARIANT FAR& dispatch_pointer) 
{
   // TODO: Add your dispatch handler code here
   // AfxMessageBox("DialogNotify::RegisterForCallbacks");

#if TRUE
   VARIANT variant;
   VariantInit(&variant);

   // make a copy of the dispatch pointer variant so that the IDispatch object gets
   // copied properly
   if (VariantCopy(&variant, &dispatch_pointer ) != S_OK)
   {
      return E_FAIL; // FAILURE;
   }
#endif

   IDispatch FAR* ptr = variant.pdispVal;
   m_dispatch_lst.AddTail( ptr );

//  SuppressDialogDisplay( 12 ); // TEST

   return 0;
}

//-----------------------------------------------------------------------------
long DialogNotify::UnregisterForCallbacks( VARIANT FAR& dispatch_pointer) 
{
   // TODO: Add your dispatch handler code here
   // AfxMessageBox("DialogNotify::UnregisterForCallbacks");

   IDispatch FAR* ptr = dispatch_pointer.pdispVal;
   POSITION position = m_dispatch_lst.Find( ptr );
   if (position)
      m_dispatch_lst.RemoveAt(position);

   return 0;
}

//-----------------------------------------------------------------------------
SHORT DialogNotify::SuppressDialogDisplay( UINT dialog_id )
{
   if ( m_pDlg )
   {
      return m_pDlg->SuppressDialogDisplayEx( dialog_id );
   };

   return 0;
}

//-----------------------------------------------------------------------------
SHORT DialogNotify::SuppressDialogDisplayEx( UINT dialog_id ) 
{
   VARIANT_BOOL suppress_display = VARIANT_FALSE;  // Assume no suppression
   SHORT how_handled = 0;  // Assume not handled
   
   POSITION position = m_dispatch_lst.GetHeadPosition();
   while (position)
   {
      IDispatch *pdisp = m_dispatch_lst.GetNext(position);

      if (pdisp == NULL)
      {
         // ERR_report("Client's dispatch pointer is not set");
         continue;
      }

      try
      {
         fvw::IDialogCallbackPtr callback = pdisp;
         callback->OnShowDialog( dialog_id, &how_handled );
      }
      catch(_com_error e)
      {
         OLECHAR FAR* szMember = L"OnShowDialog";
         
         DISPID dispid;
         HRESULT hresult = pdisp->GetIDsOfNames(IID_NULL, &szMember, 1, 
            LOCALE_USER_DEFAULT, &dispid);
         if (hresult != S_OK)
         {
            continue;
         }
         
         // Invoke the method.  The arguments in dispparams are in reverse order
         // so the first argument is the one with the highest index in the array
         DISPPARAMS dispparams;
         dispparams.rgvarg = (VARIANTARG *)malloc(sizeof(VARIANTARG)*2);
         dispparams.rgdispidNamedArgs = (DISPID *)malloc(sizeof(DISPID)*2);
         dispparams.rgvarg[1].vt = VT_I4;
         dispparams.rgvarg[1].lVal = (long)dialog_id;
         dispparams.rgvarg[0].vt = VT_I2|VT_BYREF;
         dispparams.rgvarg[0].piVal = &how_handled;
         dispparams.cArgs = 2;
         dispparams.cNamedArgs = 0;
         
         hresult = pdisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT,
            DISPATCH_METHOD, &dispparams, NULL, NULL, NULL);
         if (FAILED(hresult))
         {
            free(dispparams.rgvarg);
            free(dispparams.rgdispidNamedArgs);
            
            continue;
         }
         
         // AfxMessageBox("OnShowDialog Callback"); // TEST

         free(dispparams.rgvarg);
         free(dispparams.rgdispidNamedArgs);
      }
   }

   return how_handled;
}

