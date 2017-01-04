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



#include "stdafx.h"
#include <errno.h>
#include "CustomTB.h"
#include "fvwutil.h"
#include "resource.h"
#include "param.h"

#include "wm_user.h"


// If more or less than 20 button are defined change this
#define LAST_BUTTON_ID ID_CUSTOM_TB20

namespace 
{
const int MAX_NUM_BUTTONS = 20;
}

const char *CUSTOM_REG_PATH = "Custom Tools";

IMPLEMENT_DYNCREATE(CCustomTBClass, CWnd)

BEGIN_MESSAGE_MAP(CCustomTBClass, CWnd)
   //{{AFX_MSG_MAP(CCustomTBClass)
	//}}AFX_MSG_MAP
   ON_UPDATE_COMMAND_UI_RANGE(ID_CUSTOM_TB1, LAST_BUTTON_ID, OnUpdateButton)
   // child process message
   ON_MESSAGE( WM_CHILD_CREATED, OnChildCreated )
   ON_MESSAGE( WM_FIND_CHILD_WINDOW, OnFindChildWindow )
   ON_MESSAGE( WM_CHILD_NOT_CREATED, OnChildNotCreated )
   ON_MESSAGE( WM_CHILD_TERMINATED, OnChildTerminated )

	ON_COMMAND(ID_CUSTOMIZE_TOOLBAR, OnCustomizeToolbar)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// This is the task that launches the child process, waits for it to terminate,
// and then sends a message back to the dialog when the child has terminated.
// Communication with the frame is via Windows messages.
//////////////////////////////////////////////////////////////////////////////

UINT spawn_child_process( LPVOID pParam )
{
   CHILD_PROCESS_PARAMETERS* params = ( CHILD_PROCESS_PARAMETERS* )pParam;

   STARTUPINFO si;

   memset( &si, 0, sizeof( STARTUPINFO ));
   si.cb = sizeof( STARTUPINFO );
   si.dwFlags = STARTF_FORCEONFEEDBACK;

   memset( &params->pi, 0, sizeof( PROCESS_INFORMATION ));

   BOOL result = CreateProcess( NULL, 
      params->command_line.GetBuffer( params->command_line.GetLength() + 1 ),
      NULL,
      NULL,
      FALSE,
      NORMAL_PRIORITY_CLASS,
      NULL,
      NULL,
      &si,
      &params->pi );

   if ( result )
   {
      SendMessage( params->parent_hwnd, WM_CHILD_CREATED, params->is_modal, 0 );

      if ( params->is_modal )
         Sleep( params->delay );          // wait before looking
         
      SendMessage( params->parent_hwnd, WM_FIND_CHILD_WINDOW, params->is_modal, 0 );

      WaitForSingleObject( params->pi.hProcess, INFINITE );
      SendMessage( params->parent_hwnd, WM_CHILD_TERMINATED, params->is_modal, 0 );
   }
   else
   {
      DWORD error = GetLastError();
      SendMessage( params->parent_hwnd, WM_CHILD_NOT_CREATED, 0, error );
   }

   return 0;
}

CCustomTBClass::CCustomTBClass()
{
   window_name = NULL;
   executable_name = NULL;
   is_showing = NULL;

   m_is_modal_child = NULL;
   m_delay_after_launch = NULL;

	m_child_params.child_hwnd = NULL;

   m_help_IDs = NULL;
}

CCustomTBClass::~CCustomTBClass()
{
   delete [] window_name; 
   delete [] executable_name; 
   delete [] is_showing; 

   delete [] m_is_modal_child;
   delete [] m_delay_after_launch;

   m_help_files.RemoveAll();
   delete [] m_help_IDs;

}

void CCustomTBClass::OnCustomizeToolbar()
{
	CWnd *wnd = AfxGetMainWnd();
	if (wnd)
		wnd->PostMessage(WM_COMMAND, ID_CUSTOM_TOOLS_MANAGER, 0);
}

int CCustomTBClass::AddCustomToolsToPanel(CMFCRibbonPanel* panel)
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   HWND hWnd;
   int i;
   int found_buttons = 0;
   CString reg_path,button_name,win_name,path_name,icon_name;

   BOOL is_modal_child;
   int delay_after_launch;

   window_name = new CString[MAX_NUM_BUTTONS];
   executable_name = new CString[MAX_NUM_BUTTONS];
   is_showing = new BOOL[MAX_NUM_BUTTONS];

   m_is_modal_child = new BOOL[MAX_NUM_BUTTONS];
   m_delay_after_launch = new int[MAX_NUM_BUTTONS];

   m_help_IDs = new DWORD[MAX_NUM_BUTTONS];

   if ( !window_name || !executable_name || !is_showing || !m_is_modal_child ||
      !m_delay_after_launch || !m_help_IDs )
   {
      return FAILURE;
   }

   for(i=0;i<MAX_NUM_BUTTONS;i++)
   {
      reg_path.Format("%s\\BUTTON%d",CUSTOM_REG_PATH,i+1);
      button_name = PRM_get_registry_string(reg_path,"Text","");

		CString help_file;
		DWORD help_id;

		if (button_name != "")
		{
			win_name = PRM_get_registry_string(reg_path,"Window","");

			// if no name was specified, then we will create a bogus window name
			if (win_name == "")
				win_name = "{E9FF2212-3E2C-489c-9183-3B6011FA2F57}";

			if (win_name != "")		// allow blank window names
			{
				path_name = PRM_get_registry_string(reg_path,"Path","");
				if (path_name != "")
				{
					icon_name = PRM_get_registry_string(reg_path,"Icon","");
					
					// these two are new to support modal child processes
					is_modal_child = PRM_get_registry_int(reg_path,"Modal",0);
					delay_after_launch = PRM_get_registry_int(reg_path,"Delay",0);
					
					// these support specifying the help ID & Filename
					help_file = PRM_get_registry_string(reg_path,"HelpFile","");
					help_id = DWORD( PRM_get_registry_int(reg_path,"HelpID", 0));
				}
			}
		}

      if (button_name != "" && win_name != "" && path_name != "")
      {
         found_buttons++;
         window_name[i] = win_name;
         executable_name[i] = path_name;

         m_is_modal_child[i] = is_modal_child;
         m_delay_after_launch[i] = delay_after_launch;

         m_help_files.SetAtGrow( i, help_file );
         m_help_IDs[i] = help_id;

         is_showing[i] = futil->GetExternalApplicationIsOpen(window_name[i], &hWnd, TRUE);

         HICON hicon = NULL;
         if (icon_name != "")
         {
            CIconImage* image = CIconImage::load_images(icon_name);
            hicon = image->get_icon(32);
         }
         else
         {
            SHFILEINFO file_info;
	         if (SHGetFileInfo(path_name, FILE_ATTRIBUTE_NORMAL, &file_info, 
		         sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | 
               SHGFI_USEFILEATTRIBUTES))
            {
               hicon = file_info.hIcon;
            }
         }

         if (found_buttons == 1)
         {
            panel->Add(new CMFCRibbonSeparator());
         }

         panel->Add(new CMFCRibbonButton(ID_CUSTOM_TB1+i, button_name, hicon));
      }
   }

   m_registered_buttons = found_buttons;
   
   panel->GetParentCategory()->GetParentRibbonBar()->ForceRecalcLayout();

   return SUCCESS;
}


void CCustomTBClass::OnUpdateButton(CCmdUI* pCmdUI)
{
   int button = pCmdUI->m_nID-ID_CUSTOM_TB1;

   pCmdUI->SetCheck(0);
   is_showing[button] = FALSE;
}


void CCustomTBClass::OnCustomTB(int nID) 
{
   CFvwUtil *futil = CFvwUtil::get_instance();
   int ButtonIndex;
   CString Msg;
   
   ASSERT (nID>=nID-ID_CUSTOM_TB1 && nID <= LAST_BUTTON_ID);
   
   ButtonIndex = nID-ID_CUSTOM_TB1;

   //////////////////////////////////////////////////////////////////////////////
   // If the child is to be modal, we want to:
   //
   // (1) Kick off the thread to launch the child process & wait for it to 
   //     terminate
   // (2) Wait for messages: the worker thread will send messages when the child 
   //     process is created, when it is safe to look for the child window handle, 
   //     and when the child process terminates.
   //
   // Otherwise, use the origial spawn() method.
   //////////////////////////////////////////////////////////////////////////////

   if ( m_is_modal_child[ButtonIndex] )                        // if this is a modal child...
   {
      // build a parameter block to communicate with the worker thread

      m_child_params.parent_hwnd = m_hWnd;
      m_child_params.command_line = executable_name[ButtonIndex];
      m_child_params.is_modal = m_is_modal_child[ButtonIndex];
      m_child_params.delay = m_delay_after_launch[ButtonIndex];

      // start the thread (and the child process)

      AfxBeginThread( spawn_child_process, &m_child_params );
   }
   else                                                        // otherwise, do the original thing
   {  
      STARTUPINFO si;
      PROCESS_INFORMATION pi;

      ZeroMemory( &si, sizeof(si) );
      si.cb = sizeof(si);
      ZeroMemory( &pi, sizeof(pi) );

      if(!::CreateProcess( NULL, const_cast<char *>((LPCSTR)executable_name[ButtonIndex]), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
      {
         CString msg;
         msg.Format("Could not start %s GetLastError returned %d", executable_name[ButtonIndex], GetLastError());
         AfxMessageBox(Msg,MB_OK|MB_ICONEXCLAMATION );
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
// This handles the child not created condition. This message is sent by the 
// worker thread if the child was not successfully created. The LastError is
// returned in lParam as a DWORD.
//////////////////////////////////////////////////////////////////////////////

LONG CCustomTBClass::OnChildNotCreated( UINT wParam, LONG lParam )
{
   AfxMessageBox( "The child application could not be launched." );
   return LONG( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
// This handles the child created condition. This message is sent by the worker 
// thread after the child is successfully created. If this is a modal child, all
// we have to do is enter the modal state.
//////////////////////////////////////////////////////////////////////////////

LONG CCustomTBClass::OnChildCreated( UINT wParam, LONG lParam )
{
   BOOL is_modal = BOOL( wParam );

   if ( is_modal )
      AfxGetMainWnd()->BeginModalState(); // be modal while child is running

   return LONG( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
// This handles the notification that it should be OK to search for the child
// process window. This message is sent by the worker thread after the child 
// is successfully created and the specified delay period has elapsed.
//////////////////////////////////////////////////////////////////////////////

LONG CCustomTBClass::OnFindChildWindow( UINT wParam, LONG lParam )
{
   BOOL is_modal = BOOL( wParam );

   int tries = 5;                         // try up to 5 times to find the window
   BOOL result = FALSE;                   // initially not found

   do 
   {
      result = EnumWindows( enum_windows_proc, DWORD( &m_child_params ) );
      tries--;
   } while( result && tries );

   ASSERT( tries );                       // should have found it in 5 tries
   ASSERT( !result );                     // should have found the window by now

   return LONG( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
// This handles the child terminated condition.
//////////////////////////////////////////////////////////////////////////////

LONG CCustomTBClass::OnChildTerminated( UINT wParam, LONG lParam )
{
   BOOL is_modal = BOOL( wParam );

   m_child_params.child_hwnd = NULL;

   if ( is_modal )
   {
      AfxGetMainWnd()->EndModalState();
      ::SetForegroundWindow( m_hWnd );          // put us back on top
   }

   // AfxMessageBox( "Child application has closed." );	

   return LONG( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
// This is a callback used to search for the window handle of the process we 
// just launched with spawn_child_process() above. This is used as the worker
// by EnumWindows().
//////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CCustomTBClass::enum_windows_proc( 
HWND hwnd,        // handle to window 
LPARAM lParam )   // our CHILD_PROCESS_PARAMETERS block
{
   BOOL keep_iterating = TRUE;
   CHILD_PROCESS_PARAMETERS* params = ( CHILD_PROCESS_PARAMETERS* )lParam;

   DWORD pid;
   DWORD thread_id = GetWindowThreadProcessId( hwnd, &pid ); 

   if ( pid == params->pi.dwProcessId 
   && thread_id == params->pi.dwThreadId)
   {
      params->child_hwnd = hwnd;                   // remember the hwnd
      keep_iterating = FALSE;                      // we found it; stop looking
   }

   return keep_iterating;
}


//////////////////////////////////////////////////////////////////////////////
// Activate the child process window. Restore it if the user has minimized it.
//////////////////////////////////////////////////////////////////////////////

void CCustomTBClass::activate_child()
{
   // if the modal child was minimized by the user restore it
	if (NULL == m_child_params.child_hwnd)
		return;

   if ( ::IsIconic( m_child_params.child_hwnd ) )
      ::SendMessage( m_child_params.child_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0 );

   // make the modal child the foreground window

   if ( ::GetForegroundWindow() != m_child_params.child_hwnd )
      ::SetForegroundWindow( m_child_params.child_hwnd );

   // make sure the app window is on the top of the Z-order
   // but don't re-size it or activate it

   ::SetWindowPos( AfxGetMainWnd()->m_hWnd, m_child_params.child_hwnd, 
      0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
}