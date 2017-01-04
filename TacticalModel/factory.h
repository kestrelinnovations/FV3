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

class C_overlay_factory;
class InternalToolbarImpl;

#include "FalconView/DisplayElementProviderFactory_Interface.h"
#include "OvlFctry.h"
#include "ovl_d.h"         // attribute defines

extern const GUID FVWID_Overlay_TacticalModel;

class C_model_ovl_editor : 
   public IFvOverlayEditor
{
public:
   C_model_ovl_editor();

   virtual HCURSOR get_default_cursor();
   virtual int set_edit_on(boolean_t edit_on);

   // IFvOverlayEditor
public:
   // Called when the editor mode is activated
   virtual HRESULT ActivateEditor()
   {
      return set_edit_on(TRUE) == SUCCESS ? S_OK : E_FAIL;
   }

   // If the cursor is not set by any overlay, then the current editor's default cursor is used
   virtual HRESULT GetDefaultCursor(long *hCursor)
   {
      *hCursor = reinterpret_cast<long>(get_default_cursor());
      return S_OK;
   }

   // Called when the user leaves this editor mode
   virtual HRESULT DeactivateEditor()
   {
      return set_edit_on(FALSE) == SUCCESS ? S_OK : E_FAIL;
   }

private:
   std::unique_ptr<InternalToolbarImpl> m_toolbar;
};


class ModelDisplayElementProviderFactory :
   public DisplayElementProviderFactory_Interface
{
   virtual void CreateInstance(DisplayElementProvider_Interface** dep);
   virtual ~ModelDisplayElementProviderFactory() { }
};
