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



#pragma once

#ifndef OVL_D_H
#define OVL_D_H

#include "common.h"

//
// forward declarations
//
class C_overlay;
class C_overlay_factory;
struct OverlayTypeDescriptor;
class C_icon;
class ViewMapProj;

// definition of structure used to handle the Overlay->View submenu
struct VIEW_MENU_DATA 
{
	CString display_title;
	CString file_string;
	OverlayTypeDescriptor* pOverlayTypeDesc;
   boolean_t   bIsMenuFile;
   long   bHasBeenSaved;

   VIEW_MENU_DATA()
   {
      pOverlayTypeDesc = NULL;
      bIsMenuFile = FALSE;
      bHasBeenSaved = TRUE;
   }
};

//****************************** Definitions ********************************//

// This means the name of any class derived from C_overlay can not be longer
// than 30 characters.
#define MAX_CLASS_NAME_LENGTH 30

// --------------------------------------------------------------------------
// function prototypes
// --------------------------------------------------------------------------

// pointer to member function used in overlay menu callbacks
typedef void (*fv_menu_callback_t)(ViewMapProj *, C_icon *);
typedef void (*fv_menu_callback2_t)(ViewMapProj *, C_icon *, LPARAM);
typedef void (*fv_menu_callback3_t)(C_overlay *);
typedef void (*fv_menu_callback4_t)(CString strMenuItemName, LPARAM);
typedef void (*fv_menu_callback5_t)(ViewMapProj *, C_overlay*, LPARAM);
typedef void (*fv_menu_callback6_t)(ViewMapProj *, C_icon*, CString, bool);

// class to encapsulate menu string with callback function
class CFVMenuNode
{
// Needed so C_icon can call on_delete_icon().
friend class C_icon;

private:
   static CList<C_icon *, C_icon *> m_icon_list;

protected:
   UINT                 m_uiMFFlags;
   C_icon*					m_icon;

public:
   CString              m_label;

   CFVMenuNode() : m_uiMFFlags(0), m_icon(NULL) { }

   // used to generate a separator line (bogus parameter)
   CFVMenuNode(char* op_str);

   // Destructor
   virtual ~CFVMenuNode();

   const char * get_label() const { return m_label; }
   VOID SetMFFlags( UINT uiFlags ) { m_uiMFFlags = uiFlags; }
   UINT GetMFFlags() { return m_uiMFFlags; }
   C_icon *get_icon() const;

   long isMenuItemEnabled();
   long isMenuItemChecked();

   virtual void OnMenuItemSelected() { }

protected:
   // If icon is a non-NULL pointer and icon->is_kind_of("C_icon") returns
   // true, this icon will be added to the icon list.  If this function is
   // called more than once with the same C_icon *, then that pointer will
   // be added each time.  This function returns TRUE if the icon was added,
   // and FALSE otherwise.
   static boolean_t add_icon_if_valid(C_icon *icon);

   // Looks for this C_icon in the icon list.
   static boolean_t is_icon_in_list(C_icon *icon);

   // Removes the first match to icon in the icon list, if one is found.
   static void remove_icon(C_icon *icon);

   // Removes all matches to icon in the icon list.  This function needs
   // to get called if the C_icon object gets deleted.
   static void on_delete_icon(C_icon *icon);
};

// used with functions which only take a C_icon*
class CCIconMenuItem : public CFVMenuNode
{
   fv_menu_callback_t	m_callback;

public:
   CCIconMenuItem(const char *label, C_icon *icon, fv_menu_callback_t callback);

   /*virtual */ void OnMenuItemSelected();
};

// used with functions which need only need name of menu item and LPARAM
class CLparamMenuItem : public CFVMenuNode
{
   fv_menu_callback4_t  m_callback;
   LPARAM					m_data;

public:
   CLparamMenuItem(const char *label, LPARAM lparam, fv_menu_callback4_t callback);
    /*virtual */ void OnMenuItemSelected();
};

// used with functions which need C_icon* and LPARAM
class CCIconLparamMenuItem : public CFVMenuNode
{
   fv_menu_callback2_t	m_callback;
   LPARAM					m_data;
   
public:
   CCIconLparamMenuItem(const char *label, C_icon *icon, fv_menu_callback2_t callback,
		LPARAM data=0);

   /*virtual */ void OnMenuItemSelected();
};

// used with functions which need a C_overlay and an LPARAM
class CCOverlayLparamMenuItem : public CFVMenuNode
{
   C_overlay* m_pOverlay;
   fv_menu_callback5_t  m_callback;
   LPARAM               m_data;

public:
   CCOverlayLparamMenuItem(const char *label, C_overlay* pOverlay, LPARAM data, 
      fv_menu_callback5_t callback);

   /*virtual */ void OnMenuItemSelected();
};

// used with functions which only take a C_overlay*
class CCOverlayMenuItem : public CFVMenuNode
{
   C_overlay* m_static_overlay;
   fv_menu_callback3_t m_callback;

public:
   CCOverlayMenuItem(const char *label, C_overlay *static_overlay, 
      fv_menu_callback3_t callback) : m_static_overlay(static_overlay), m_callback(callback)
   {
      m_label = label;
   }

   void OnMenuItemSelected();
};

class CCallbackContextMenuItem : public CFVMenuNode
{
   IDispatch* m_pdisp;
   CString m_method_name;
   int m_layer_handle;
   int m_object_handle;
   CString m_strMenuItemData;

public:
   CCallbackContextMenuItem(const char *label, IDispatch* pdisp, 
      CString method_name, int layer_handle, int object_handle, 
      CString strMenuItemData)
   {
      m_label = label;
      m_pdisp = pdisp;
      m_method_name = method_name;
      m_layer_handle = layer_handle;
      m_object_handle = object_handle;
      m_strMenuItemData = strMenuItemData;
   }

   /*virtual */ void OnMenuItemSelected();
};

class CFVMenuNodeContextMenuItem : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvContextMenuItem, &FalconViewOverlayLib::IID_IFvContextMenuItem, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CFVMenuNode *m_pMenuNode;
public:

   ~CFVMenuNodeContextMenuItem();

   // Initialize takes ownership of the given CFVMenuNode and is responsible for deleting it
   void Initialize(CFVMenuNode *pMenuNode);

BEGIN_COM_MAP(CFVMenuNodeContextMenuItem)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvContextMenuItem)
END_COM_MAP()

   // IFvContextMenuItem
public:
   STDMETHOD(get_MenuItemName)(BSTR *pMenuItemName);
   STDMETHOD(get_MenuItemEnabled)(long *pMenuItemEnabled);
   STDMETHOD(get_MenuItemChecked)(long *pMenuItemChecked);

   STDMETHOD(raw_MenuItemSelected)();
};

// **************************
// icon base class definition - A C_icon is a graphical representation of a
// part of a C_overlay object which is displayed in a particular map window 
// (view).  The C_overlay object has a separate list of all C_icon's for each
// map window (view) it is displayed in.

class C_icon
{
// public member variables
public:
   C_overlay *m_overlay;
   std::string m_strObjectGuid;
   COleDateTime m_dtEventStart;

public:
   // Constructor
   C_icon(C_overlay *overlay = NULL);
 
   // Destructor 
   virtual ~C_icon(); 

   void SetParentOverlay(C_overlay *pOverlay) { m_overlay = pOverlay; }

   // Returns TRUE if this icon is a instance of the given class.
   // For example: is_kind_of("C_icon") would return TRUE.  Derived classes
   // should implement an is_kind_of function that returns TRUE for their
   // class name and returns C_icon::is_kind_of(clase_name) otherwise.
   virtual boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   virtual const char *get_class_name() = 0;

   // Descriptive string functions
   virtual CString get_help_text() = 0;
   virtual CString get_tool_tip() = 0;
   
   // Determine if the point is over the icon.
   virtual boolean_t hit_test(CPoint point) = 0;

   //return the database key as a comma delimitted string
   //if there is no key, derived classes should return ""
   //To DO: kevin: make this pure virtual
   virtual CString get_key() const {return "";} 


   virtual int get_lat(degrees_t & /*lat*/) const { return FAILURE; }
   virtual int get_lon(degrees_t & /*lon*/) const { return FAILURE; }

   //To DO:kevin: make this pure virtual
   //return the RS_POINT.point_source as defined in the PFPS Route Server ICD
   virtual int get_RS_POINT_point_source() const {return -1;}

   std::string GetObjectGuid() { return m_strObjectGuid; }
   void SetObjectGuid(std::string& strGuid) { m_strObjectGuid = strGuid; }
};
// end C_icon definition


// HintText class definition
// This class is a container for the Help Pane and Tool Tip text that an
// overlay or the overlay manager wants to display.
class HintText
{
	friend class C_ovl_mgr;

private:
   CString		m_help_text;
   CString		m_tool_tip;
   void*			m_object;

public:
   // Constructors
   HintText() : m_object(NULL) {};
   HintText(const char *help_text, const char *tool_tip) :
      m_help_text(help_text), m_tool_tip(tool_tip), m_object(NULL) {}

   // set/get the string for the Help Text Pane
   void set_help_text(const char *help_text) { m_help_text = help_text; }
   CString &get_help_text() { return m_help_text; }

   // set/get the string for the Tool Tip
   void set_tool_tip(const char *tool_tip) { m_tool_tip = tool_tip; }
   CString &get_tool_tip() { return m_tool_tip; }

   // set/get the icon or overlay object associated with this hint
   void *get_object() { return m_object; }

   // copy a C_icon objects help text and tool tip text into a HintText object
   const HintText& operator =(C_icon &icon)
   {
      set_help_text(icon.get_help_text());
      set_tool_tip(icon.get_tool_tip());
      m_object = (void *)&icon;

      return *this;
   }
};
// end of HintText class definition

#endif
