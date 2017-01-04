// Copyright (c) 1994-2012 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(tm).
//
// FalconView(tm) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// FalconView(tm) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(tm).  If not, see <http://www.gnu.org/licenses/>.
//
// FalconView(tm) is a trademark of Georgia Tech Research Corporation.
//
// FileOverlayListComboBox.h
//

#include "ovl_mgr.h"
#include "OverlayStackChangedObserver_Interface.h"

class FileOverlayListComboBox : 
   public CMFCRibbonComboBox,
   public OverlayStackChangedObserver_Interface
{
   bool m_show_time_sensitive_only;

public:
   FileOverlayListComboBox();
   ~FileOverlayListComboBox();

   // Do not allow adding this control to the QAT
   virtual BOOL CanBeAddedToQuickAccessToolBar() const override
   {
      return FALSE;
   }

   bool GetShowTimeSensitiveOnly() const
   {
      return m_show_time_sensitive_only;
   }
   void SetShowTimeSensitiveOnly(bool stso)
   {
      m_show_time_sensitive_only = stso;
      Update();
   }

   void Update();

   // OverlayStackChangedObserver_Interface implementation
   //
   virtual void OverlayAdded(C_overlay* overlay) override
   {
      Update();
   }
   virtual void OverlayRemoved(C_overlay* overlay) override
   {
      Update();
   }
   virtual void OverlayOrderChanged() override
   {
      Update();
   }
   virtual void OverlayFileSpecificationChanged(C_overlay* overlay) override
   {
      Update();
   }
   virtual void OverlayDirtyChanged(C_overlay* overlay) override
   {
   }
};