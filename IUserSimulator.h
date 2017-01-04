// Copyright (c) 1994-2009,2012,2013 Georgia Tech Research Corporation, Atlanta, GA
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

// IUserSimulator.h
//


class UserSimulator: public CCmdTarget
{
   DECLARE_DYNCREATE(UserSimulator)
   DECLARE_OLECREATE(UserSimulator)

   UserSimulator();           // protected constructor used by dynamic creation

   enum ToolbarTypeEnum { CORE_TOOLBAR_TYPE, CLIENT_TOOLBAR_TYPE, FV_TOOLBAR_TYPE };

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(UserSimulator)
   public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL

   // Add tool name and button hint text to button ID lookup table
   static void AddToolbarButtonLookup( __in UINT uiID,
                     __in LPCTSTR pszToolbarName, __in LPCTSTR pszHintText );
   static afx_msg LRESULT OnSimulatorToolbarEvent( __in WPARAM nButtonID,
                     __in LPARAM /*(BSTR)*/ bsControlString );

// Implementation
private:
   // Methods
   int LookupIDD( __in const CString& csControlString, __inout ToolbarTypeEnum& eToolbarType );
   long ActivateControlString( __in BOOL bToolbarOp, LPCTSTR pszControlString );
   BOOL GetCurrentState( __inout long& iID, __out int& iCurrentState ); // False if state determination not possible
   BOOL GetCurrentState( __in BOOL bToolbarOp, 
      __in const CString& csControlString, __out int& iCurrentState );  // False if state determination not possible
   BOOL GetTargetState( __inout CString& csControlString, __out int& iTargetState );
   BOOL ProcessSpecialCommand( __in UINT uiID, __in const CString& csControlString  );

   // Data
   CMainFrame& m_mainFrame;

protected:
   virtual ~UserSimulator();

   DECLARE_MESSAGE_MAP()
   // Generated OLE dispatch map functions
   //{{AFX_DISPATCH(UserSimulator)

   afx_msg long ActivateMenuItem( LPCTSTR menu_string );
   afx_msg long ActivateToolbarButton( LPCTSTR toolbar_string );
   
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
};

// End of IUserSimulator.h

