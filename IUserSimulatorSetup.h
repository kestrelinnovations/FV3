// Copyright (c) 1994-2009,2012 Georgia Tech Research Corporation, Atlanta, GA
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

// IUserSimulatorSetup.h
//


class UserSimulatorSetup: public CCmdTarget
{
   DECLARE_DYNCREATE(UserSimulatorSetup)
   DECLARE_OLECREATE(UserSimulatorSetup)

   UserSimulatorSetup();           // protected constructor used by dynamic creation

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(UserSimulatorSetup)
   public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL

// Implementation
private:

protected:
   virtual ~UserSimulatorSetup();

   DECLARE_MESSAGE_MAP()

   // Add tool name and button hint text to button ID lookup table
   afx_msg long AddToolbarButtonLookup( __in UINT uiID,
                     __in LPCTSTR pszToolbarName, __in LPCTSTR pszHintText );

   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
};

// End of IUserSimulatorSetup.h

