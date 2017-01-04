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



/////////////////////////////////////////////////////
//
// Class to allow toolbar buttons to spawn external apps
// based on registry entries
//
////////////////////////////////////////////////////

#ifndef _CUSTOMTB_H_
#define  _CUSTOMTB_H_

typedef struct {
   HWND parent_hwnd;
   HWND child_hwnd;
   PROCESS_INFORMATION pi;
   CString command_line;
   int delay;
   BOOL is_modal;
} CHILD_PROCESS_PARAMETERS;

class CCustomTBClass : public CWnd
{
protected:
   DECLARE_DYNCREATE(CCustomTBClass)
   
public:
   CCustomTBClass();
   virtual ~CCustomTBClass();
   
   int GetNumRegisteredButtons() {return m_registered_buttons;}

   // reg setup reads the default toolbar resource then looks in the registry
   // for custom button info.  Each custom button replaces the previous buttons
   // name and icon.  The registry also defines the custom app executable path and 
   // the window name which is used to prevent multiple instances of the same program 
   // from starting
   // return SUCCESS or FAILURE
   int AddCustomToolsToPanel(CMFCRibbonPanel* panel);

   // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CustomTBClass)
	//}}AFX_VIRTUAL

   // This is intended to be called by the MainFrame when ever a button 
   // on the custom toolbar is pressed. ID = ID_CUSTOM_TB1 - ID_CUSTOM_TB20
   // this can be acomplished with an ON_COMMAND_RANGE(ID_CUSTOM_TB1,ID_CUSTOM_TB20,foo)
   // message map entry.
   // I could not figure out how to automatically get the window messages sent to 
   // this toolbar for the button presses.
   // NOTE: the COMMAND_IDs must be contigious from ID_CUSTOM_TB1 to ID_CUSTOM_TB20.
   // If more or less than 20 IDs are defined, change the LAST_BUTTON_ID macro in the 
   // CustomTB.cpp file.
   afx_msg void OnCustomTB(int nID);
   afx_msg void OnUpdateButton(CCmdUI* pCmdUI);

   // this is for the spawned child process
	void activate_child();
   static BOOL CALLBACK enum_windows_proc( HWND hwnd, LPARAM lParam );
   CHILD_PROCESS_PARAMETERS m_child_params;
   afx_msg LONG OnChildCreated( UINT wParam, LONG lParam );
   afx_msg LONG OnFindChildWindow( UINT wParam, LONG lParam );
   afx_msg LONG OnChildNotCreated( UINT wParam, LONG lParam );
   afx_msg LONG OnChildTerminated( UINT wParam, LONG lParam );

	afx_msg void OnCustomizeToolbar();

   // Generated message map functions
protected:
	//{{AFX_MSG(CCustomTBClass)
	//}}AFX_MSG
    
	DECLARE_MESSAGE_MAP()

   
protected:
   int m_registered_buttons;

   CString *executable_name;
   CString *window_name;
   BOOL  *is_showing;

   // new for possibly modal children
   BOOL* m_is_modal_child;
   int*  m_delay_after_launch; 

   // for help files and IDs for the items
   CStringArray m_help_files;
   DWORD* m_help_IDs;
};

#endif
