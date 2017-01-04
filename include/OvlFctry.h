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



#ifndef _OVLFCTRY_H_
#define _OVLFCTRY_H_

#include "common.h"

// forward declarations
class C_overlay;

// IFvOverlayEditor - interface implemented by an overlay editor
//
interface IFvOverlayEditor
{
   // Called when the editor mode is activated
   virtual HRESULT ActivateEditor() = 0;

   // If the cursor is not set by any overlay, then the current editor's default cursor is used
   virtual HRESULT GetDefaultCursor(long *hCursor) = 0;

   // Called when the user leaves this editor mode
   virtual HRESULT DeactivateEditor() = 0;

   virtual ~IFvOverlayEditor() { }
};

// IFvOverlayLimitUserInterface - optional interface implemented by an overlay editor.  This interface allows
//    an editor, when active, to limit certain aspects of the user interface
interface IFvOverlayLimitUserInterface
{
   // Return TRUE if the user should not be able to change the projection of the map
   virtual HRESULT get_m_bDisableProjectionUI(long* pVal) = 0;

   // Return TRUE if the user should not be able to rotate the map while the editor is active
   virtual HRESULT get_m_bDisableRotationUI(long* pVal) = 0;

   // Return TRUE if the map must be North-Up
   virtual HRESULT get_m_bRequiresNorthUp(long* pVal) = 0;

   // Return TRUE if the map must be projected with the Equal Arc projection when this editor is active
   virtual HRESULT get_m_bRequiresEqualArc(long* pVal) = 0;

   // Return TRUE if the order of overlays associated with this editor cannot be changed
   virtual HRESULT get_m_bDisableOverlayOrdering(long* pVal) = 0;
};

// IFvOverrideDefaultUserInterface - optional interface implemented by an overlay editor.  This interface allows
//    an editor to change the default behavior of the FalconView user interface
interface IFvOverrideDefaultUserInterface
{
   // Return TRUE if FalconView should activate the editor when a static overlay is toggled on or when the user chooses File | New
   // for a file overlay.  Return FALSE otherwise.  FalconView's default behavior is TRUE.
   virtual HRESULT get_m_bAutoEnterOverlayEditor(long* pVal) = 0;

   // Normally, FalconView will use the overlay type's icon as the editor toolbar button.  The following method can be
   // used to override the editor toolbar button's image.  Return an empty string if you would like to preserve the default behavior.
   virtual HRESULT get_m_bEditorToolbarButtonBitmap(BSTR* pEditorToolbarButtonBmp) = 0;
};

// IFvFileOverlayTypeDescriptor - provides information at runtime about the file specific characteristics of an 
//    overlay type.  This optional interface can be used to populate the fileTypeDescriptor subsection of the 
//    overlay type at runtime (rather than specified in the configuration file)
interface IFvFileOverlayTypeDescriptor
{
   // Get the default directory where files for this overlay type are stored
   virtual HRESULT GetDefaultDirectory(BSTR* pbstrDefaultDir) = 0;

   // Get the default file extension of files for this overlay type
   virtual HRESULT GetDefaultFileExtension(BSTR* pbstrDefaultExt) = 0;

   // Get the filter used in the common file dialog boxes (open/save).  Return one or more pairs of '|' separated
   // filter strings.  The first string in each pair is a display string that describes the filter, and the second
   // string specifies the filter pattern (for example, "*.rte").  Note that you can return a different filter for 
   // the Open vs. Save As dialogs
   virtual HRESULT GetFileDialogFilter(long bIsFileOpenDialog, BSTR* pbstrFilter) = 0;
};

// OverlayFactory - base class used to create an instance of an overlay of the
// templated type
interface IFvOverlayFactory
{
public:
   virtual HRESULT CreateOverlayInstance(C_overlay** ret) = 0;
   virtual ~IFvOverlayFactory() { }
};

template<typename T>
class OverlayFactory : public IFvOverlayFactory
{
public:
   // Create a new instance of an overlay type
   virtual HRESULT CreateOverlayInstance(C_overlay **ret)
   {
      CComObject<T> *overlay;
      CComObject<T>::CreateInstance(&overlay);
      overlay->AddRef();

      *ret = overlay;

      return S_OK;
   }

   virtual ~OverlayFactory() 
   {
   }
};

#endif
