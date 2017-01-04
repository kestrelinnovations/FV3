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



// icon.cpp

// This file defines the non-in-line member functions of the C_icon
// base class.

#include "stdafx.h"
#include "ovl_d.h"
#include "overlay.h"
#include "err.h"
#include "..\mapview.h"
#include "utils.h"

// ****************************************************************************
// C_icon
// ****************************************************************************

// Constructors
C_icon::C_icon(C_overlay *overlay)
{
   m_overlay = overlay;

   // Generate a GUID for this object
   //
   GUID guid;
   ::CoCreateGuid(&guid);

   wchar_t buf[45];
   StringFromGUID2(guid, buf, 45);

   m_strObjectGuid = (char *)_bstr_t(buf);

   m_dtEventStart = COleDateTime::GetCurrentTime();
}

// Destructor
C_icon::~C_icon()
{ 
   // The CFVMenuNode class is vulnerable to volitile C_icon pointers, to solve
   // that problem, the C_icon destructor will notify the CFVMenuNode class when
   // a C_icon object is deleted.
   CFVMenuNode::on_delete_icon(this); 
}

// Returns TRUE if this icon is a instance of the given class.
// For example: is_kind_of("C_icon") would return TRUE.  Derived classes
// should implement an is_kind_of function that returns TRUE for their
// class name and returns C_icon::is_kind_of(clase_name) otherwise.
boolean_t C_icon::is_kind_of(const char *class_name)
{
   return (strcmp(class_name, "C_icon") == 0);
}

// ****************************************************************************
// CFVMenuNode
// ****************************************************************************

CList<C_icon *, C_icon *> CFVMenuNode::m_icon_list;

// used to generate a separator line (bogus parameter)
CFVMenuNode::CFVMenuNode(char* op_str)
	: m_label(op_str)
{ 
   m_uiMFFlags = 0;
   m_icon = NULL; 
}

// Destructor
CFVMenuNode::~CFVMenuNode()
{
   if (m_icon)
      remove_icon(m_icon);
}

// Returns the C_icon pointer passed to the constructor only if it is still
// valid.
C_icon *CFVMenuNode::get_icon() const 
{
   if (is_icon_in_list(m_icon))
      return m_icon;

   return NULL;
}

// If icon is a non-NULL pointer and icon->is_kind_of("C_icon") returns
// true, this icon will be added to the icon list.  If this function is
// called more than once with the same C_icon *, then that pointer will
// be added each time.  This function returns TRUE if the icon was added,
// and FALSE otherwise.
boolean_t CFVMenuNode::add_icon_if_valid(C_icon *icon)
{
   if (icon && icon->is_kind_of("C_icon"))
   {
      m_icon_list.AddTail(icon);
      return TRUE;
   }

   return FALSE;
}

// Looks for this C_icon in the icon list.
boolean_t CFVMenuNode::is_icon_in_list(C_icon *icon)
{
   return (m_icon_list.Find(icon) != NULL);
}

// Removes the first match to icon in the icon list, if one is found.
void CFVMenuNode::remove_icon(C_icon *icon)
{
   POSITION position = m_icon_list.Find(icon);

   // If a match was found, remove it.
   if (position)
      m_icon_list.RemoveAt(position);
}

// Removes all matches to icon in the icon list.  This function needs
// to get called if the C_icon object gets deleted.
void CFVMenuNode::on_delete_icon(C_icon *icon)
{
   POSITION position;
   POSITION start_after = NULL;

   do
   {
      // Search for icon starting with the element after the one at 
      // start_after.  When start_after == NULL, the search starts with the
      // first element in the list.
      position = m_icon_list.Find(icon, start_after);

      // If a match was found.
      if (position)
      {
         // Set start_after to the position of the element before the matching
         // element.
         start_after = position;
         m_icon_list.GetPrev(start_after);

         // Remove the matching element.
         m_icon_list.RemoveAt(position);
      }

   } while (position);  // continue the until no match is found 
}

long CFVMenuNode::isMenuItemEnabled()
{
   return (m_uiMFFlags & (MF_GRAYED | MF_DISABLED)) == 0;
}

long CFVMenuNode::isMenuItemChecked()
{
   return m_uiMFFlags & MF_CHECKED;
}

// CCIconMenuItem
//

CCIconMenuItem::CCIconMenuItem(const char *label, C_icon *icon, fv_menu_callback_t callback) : 
m_callback(callback)
{
   m_label = label;
   m_icon = icon;

   // If this fails, either a NULL pointer was passed or the is_kind_of member
   // for the C_icon derived class for this object was not properly implemented.
   if (add_icon_if_valid(m_icon) == FALSE)
   {
      ERR_report("Invalid C_icon.");
      ASSERT(0);
   }
}

void CCIconMenuItem::OnMenuItemSelected()
{
   ViewMapProj* pMap = UTL_get_current_view_map();
   m_callback(pMap, get_icon());
}

// CLparamMenuItem
//

CLparamMenuItem::CLparamMenuItem(const char *label, LPARAM lparam, 
                                 fv_menu_callback4_t callback) :
m_callback(callback), m_data(lparam)
{
   m_label = label;
}

void CLparamMenuItem::OnMenuItemSelected()
{
   m_callback(m_label, m_data);
}

// CCIconLparamMenuItem
//

CCIconLparamMenuItem::CCIconLparamMenuItem(const char *label, C_icon *icon, 
                                           fv_menu_callback2_t callback,
                                           LPARAM data /* = 0 */) :
m_callback(callback), m_data(data)
{
   m_label = label;
   m_icon = icon;

   // If this fails, either a NULL pointer was passed or the is_kind_of member
   // for the C_icon derived class for this object was not properly implemented.
   if (add_icon_if_valid(m_icon) == FALSE)
   {
      ERR_report("Invalid C_icon.");
      ASSERT(0);
   }
}

void CCIconLparamMenuItem::OnMenuItemSelected()
{
   ViewMapProj* pMap = UTL_get_current_view_map();

   if (get_icon())
      m_callback(pMap, get_icon(), m_data);
}

// CCOverlayLparamMenuItem
//

CCOverlayLparamMenuItem::CCOverlayLparamMenuItem(const char *label, 
                                                 C_overlay* pOverlay, 
                                                 LPARAM data, 
                                                 fv_menu_callback5_t callback):
m_pOverlay(pOverlay), m_data(data), m_callback(callback)
{
   m_label = label;
}

void CCOverlayLparamMenuItem::OnMenuItemSelected()
{
   ViewMapProj* pMap = UTL_get_current_view_map();
   m_callback(pMap, m_pOverlay, m_data);
}

// COverlayMenuItem
//

void CCOverlayMenuItem::OnMenuItemSelected()
{
   m_callback(m_static_overlay);
}

// CCallbackContextMenuItem
//

void CCallbackContextMenuItem::OnMenuItemSelected()
{
   if ( NULL == m_pdisp )
      return;

   // Retrieve the dispatch identifier for the method
   _bstr_t bstrMethodName(m_method_name);
   OLECHAR FAR* szMember = (wchar_t *)bstrMethodName;
   DISPID dispid;
   HRESULT hresult = m_pdisp->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid);
   if (hresult != S_OK)
   {
      CString msg;
      msg.Format("Error calling client menu item : unable to get ID of %s from client dispatch pointer.  HRESULT = %d", m_method_name, hresult);
      ERR_report(msg);

      return;
   }

   // Invoke the method
   try
   {
      COleDispatchDriver callbackImpl(m_pdisp, FALSE);

      // The name of the method can be appended with additional menu item data (converted to a long)
      if (m_strMenuItemData.GetLength())
      {
         static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4;
         callbackImpl.InvokeHelper(dispid, DISPATCH_METHOD, VT_EMPTY, NULL, parms, m_layer_handle, m_object_handle, atol(m_strMenuItemData));
      }
      else
      {
         static BYTE parms[] = VTS_I4 VTS_I4;
         callbackImpl.InvokeHelper(dispid, DISPATCH_METHOD, VT_EMPTY, NULL, parms, m_layer_handle, m_object_handle);
      }
   }
   catch(COleException *e)
   {
      ERR_report_exception("Error calling client menu item", *e);
      e->Delete();
   }
}

// CFVMenuNodeContextMenuItem
//

CFVMenuNodeContextMenuItem::~CFVMenuNodeContextMenuItem()
{
   delete m_pMenuNode;
}

// Initialize takes ownership of the given CFVMenuNode and is responsible for deleting it
void CFVMenuNodeContextMenuItem::Initialize(CFVMenuNode *pMenuNode)
{
   m_pMenuNode = pMenuNode;
}

STDMETHODIMP CFVMenuNodeContextMenuItem::get_MenuItemName(BSTR *pMenuItemName)
{
   *pMenuItemName = _bstr_t(m_pMenuNode->m_label).Detach();
   return S_OK;
}

STDMETHODIMP CFVMenuNodeContextMenuItem::get_MenuItemEnabled(long *pMenuItemEnabled)
{
   *pMenuItemEnabled = m_pMenuNode->isMenuItemEnabled();
   return S_OK;
}

STDMETHODIMP CFVMenuNodeContextMenuItem::get_MenuItemChecked(long *pMenuItemChecked)
{
   *pMenuItemChecked = m_pMenuNode->isMenuItemChecked();
   return S_OK;
}

STDMETHODIMP CFVMenuNodeContextMenuItem::raw_MenuItemSelected()
{
   m_pMenuNode->OnMenuItemSelected();
   return S_OK;
}