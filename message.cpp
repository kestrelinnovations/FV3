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



// message.cpp : CFVMessageDlg implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "message.h"
#include "getobjpr.h"
#include "err.h"
#include "getobjpr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFVMessageDlg dialog

CFVMessageDlg::CFVMessageDlg(CWnd* pParent, const char *message, const char *title)
	: CDialog(CFVMessageDlg::IDD, pParent), m_message(message), m_title(title)
{
	//{{AFX_DATA_INIT(CFVMessageDlg)
	//}}AFX_DATA_INIT

   // save the parent window's CWnd *
   if (pParent)
      m_parent = pParent;
   else
      m_parent = fvw_get_view();

   // create the dialog box, ShowWindow will be used to display it
   if (!Create(IDD_MESSAGE, m_parent))
      ERR_report("Create() failed.");
}

CFVMessageDlg::CFVMessageDlg(CWnd* pParent)
	: CDialog(CFVMessageDlg::IDD, pParent)
{
   // save the parent window's CWnd *
   if (pParent)
      m_parent = pParent;
   else
      m_parent = fvw_get_view();

   // create the dialog box, ShowWindow will be used to display it
   if (!Create(IDD_MESSAGE, m_parent))
      ERR_report("Create() failed.");
}


void CFVMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFVMessageDlg)
	DDX_Control(pDX, IDC_MESSAGE_3_LINE, m_three_line);
	DDX_Control(pDX, IDC_MESSAGE_2_LINE, m_two_line);
	DDX_Control(pDX, IDC_MESSAGE_1_LINE, m_one_line);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFVMessageDlg, CDialog)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CFVMessageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFVMessageDlg message handlers

BOOL CFVMessageDlg::OnInitDialog() 
{
   CRect parent_rect;
   CRect rect;

   CDialog::OnInitDialog();

   // relocate the dialog box at the center of its parent window
   m_parent->GetWindowRect(&parent_rect);
   GetWindowRect(&rect);
   SetWindowPos(NULL, parent_rect.left + parent_rect.Width()/2 - rect.Width()/2, 
      parent_rect.top + parent_rect.Height()/2 - rect.Height()/2, -1, -1,
      SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

   // update the title and message in the window
   SetWindowText(m_title);
   set_message_text();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


int CFVMessageDlg::set_message_text()
{
   CDC *dc;
   CSize message_extent;
   CSize test_extent;
   CString test("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

   // Get the text extent for the message string and the reference sting.
   dc = GetDC();
   message_extent = dc->GetTextExtent(m_message, m_message.GetLength());
   test_extent = dc->GetTextExtent(test, test.GetLength());

   // only one line of text, center it in the dialog
   if (message_extent.cx <= test_extent.cx)
   {
      m_one_line.ShowWindow(SW_SHOW);
      m_two_line.ShowWindow(SW_HIDE);
      m_three_line.ShowWindow(SW_HIDE);
      m_one_line.SetWindowText(m_message);   
   }
   else if (message_extent.cx > (2 * test_extent.cx))
   {
	  // three lines
      m_one_line.ShowWindow(SW_HIDE);
      m_two_line.ShowWindow(SW_HIDE);
      m_three_line.ShowWindow(SW_SHOW);
      m_three_line.SetWindowText(m_message);
   }
   else
   {
	  // two or three lines (dependant on where the line breaks are)
      char *buffer;
      char white_space;
      int i;
      int length;
      	  
      // save the length of the message
      length = m_message.GetLength();  
      
      // get a buffer containing the message string    
      buffer = m_message.GetBuffer(length);
      
      // start the search for the line break at the end of the string
	   i = length-1;
	   do
      {
         // find the next white space from the end of the string
         while (buffer[i] != ' ' && buffer[i] != '\t' && i > 0)
            i--;

         // save the white space
         white_space = buffer[i];

         // check the text extent of the start of the string
         buffer[i] = '\0';
         message_extent = dc->GetTextExtent(buffer, i);

         // replace the white space
         buffer[i] = white_space;

         //decrement so no longer indexes white space 
         i--;
      }
      while (message_extent.cx > test_extent.cx);

      i+=2; //indexes first non-whitespace on 2nd line

  	   message_extent = dc->GetTextExtent(&buffer[i], length - i);
      if (message_extent.cx > test_extent.cx)
      {         
         m_one_line.ShowWindow(SW_HIDE);
         m_two_line.ShowWindow(SW_HIDE);
         m_three_line.ShowWindow(SW_SHOW);
         m_three_line.SetWindowText(buffer);
      }
      else
      {
         m_one_line.ShowWindow(SW_HIDE);
         m_two_line.ShowWindow(SW_SHOW);
         m_three_line.ShowWindow(SW_HIDE);
         m_two_line.SetWindowText(buffer);
      }

      m_message.ReleaseBuffer();
   }

   ReleaseDC(dc);
   
   return SUCCESS;
}
LRESULT CFVMessageDlg::OnCommandHelp(WPARAM, LPARAM)
{
   CMainFrame *pFrame = fvw_get_frame();
   // THIS PATH NEEDS TO BE CHANGED
   if (pFrame != NULL)
      pFrame->LaunchHtmlHelp(0, HELP_CONTEXT, 0, getHelpURIPath());

	return 1;
}

