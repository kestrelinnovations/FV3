// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
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



// fvw.h : main header file for the FVW application
//

#ifndef FVW_H
#define FVW_H

#include "..\include\common.h"
#include "resource.h"      // main symbols


#define MAX_NUM_FILES_IN_MRU_LIST 6  // The maximum number of file specifications that will be tracked
                                     // and displayed in the Most Recently Used (MRU) file list.

// Forward Definitions
class RecentFileOverlayList;
class CAppExpiration;
class MapView;

////
// The JMPS Route plugin running in FalconViewCOM is making external (synchronous) calls within the mouse move 
// handler and since these calls are currently taking a very long time, WM_PAINT messages receivied by 
// FalconView were causing the throwing of the RPC_E_CANTCALLOUT_INEXTERNALCALL exception (value of 0x80010005)
// Overriding COleMessageFilter's OnMessagePending to ignore WM_PAINT messages during these calls. 
// For more information see Microsoft's KB 179692: "An HRESULT of 0x80010005 error occurs when you try to 
// call methods on a COM server in Visual C++".
class CMyMessageFilter : public COleMessageFilter
{
public:
   CMyMessageFilter() : COleMessageFilter(), m_dispatching_wm_paint(false)
   {
   }

   virtual BOOL OnMessagePending(const MSG* pMsg);

   void SetCanHandleIncomingCalls(BOOL can_handle)
   {
      m_nBusyCount = !can_handle;
   }
   bool GetDispatchingPaint()
   {
      return m_dispatching_wm_paint;
   }

private:
   bool m_dispatching_wm_paint;
};

// Override Default handler of MRU file list so we can use URLs and Files in the list (CAB)
class CUnverifiedRecentFileList : public CRecentFileList
{
public:
   CUnverifiedRecentFileList(UINT nStart, LPCTSTR lpszSection,
      LPCTSTR lpszEntryFormat, int nSize,
      int nMaxDispLen = AFX_ABBREV_FILENAME_LEN):
   CRecentFileList(nStart,lpszSection,lpszEntryFormat,nSize,nMaxDispLen){}

   virtual void Add(LPCTSTR lpszPathName);
   virtual BOOL GetDisplayName(CString& strName, int nIndex,
      LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName = TRUE) const;
};
/////////////////////////////////////////////////////////////////////////////
// CFVApp:
// See fvw.cpp for the implementation of this class
//

#define FV_APP_BASE_CLASS CWinAppEx

class CFVApp : public FV_APP_BASE_CLASS
{
private:
   RecentFileOverlayList *m_recent_file_list;
   boolean_t m_is_regserver;
   boolean_t m_is_unregserver;
   bool m_gui_hidden;

   // used to determine if an instance of FalconView is already opened
   HANDLE m_Mutex;

public:
   // Constructor
   CFVApp(LPCSTR);

   // Destructor
   ~CFVApp();

   BOOL m_font_loaded;
   int m_debugVersion;  // indicates whether or not this is a debug build
   CAppExpiration *m_expiration;  //expiration object which controls FV expiration date

   // note: the following is only used with Windwos 95 (not NT or 3.1)
   boolean_t m_full_drag_enabled_at_startup;  

// Overrides
   virtual BOOL InitInstance();
   virtual int  ExitInstance();
   virtual BOOL PreTranslateMessage(MSG* msg);

   void add_to_recent_file_list(GUID overlayDescGuid, const char *file_specification);

   // If this function returns true then the user interface for FalconView is 
   // hidden.  That means that under no circumstances should we display dialog
   // boxes, message boxes, etc.
   bool is_gui_hidden() { return m_gui_hidden; }

   CRecentFileList* GetRecentFileOverlayList()
   {
      return m_pRecentFileList;
   }

// Implementation

   //{{AFX_MSG(CFVApp)
   afx_msg void OnAppAbout();
   afx_msg void OnFileOpen();
   afx_msg void OnFileCloseFv();
   afx_msg void OnUpdateFileCloseFv(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileSaveFv(CCmdUI* pCmdUI);
   afx_msg void OnHelpTechnicalsupport();
   afx_msg void OnUpdateFileSaveAsFv(CCmdUI* pCmdUI);
   afx_msg void OnFileNewFv();
   afx_msg void OnFileSaveAs();
   afx_msg void OnFileSave();
   afx_msg void OnCurrentNew();
   afx_msg void OnCurrentOpen();
   afx_msg void OnCurrentSave();
   afx_msg void OnUpdateCurrentSave(CCmdUI* pCmdUI);
   afx_msg void OnToolsTest();
   afx_msg void OnSLAT();
   afx_msg void OnPluginManager();
   afx_msg void OnPurgeRecentFileList();
   afx_msg void OnUpdatePurgeRecentFileList(CCmdUI* pCmdUI);
   //}}AFX_MSG
   afx_msg void OnFilePrintSetup();
   afx_msg void OnDafifSelection();
   afx_msg void OnSaveAll();
   afx_msg void OnUpdateSaveAll(CCmdUI* pCmdUI);
   afx_msg BOOL OnOpenRecentFile(UINT nID);
   afx_msg void ShowTipOfTheDay();
   afx_msg void XPlanOptions();
#ifdef _DEBUG
   afx_msg void OnMemoryDebugging();
   afx_msg void OnUpdateMemoryDebugging(CCmdUI* pCmdUI);
   afx_msg void OnCheckMemory();
#endif
   DECLARE_MESSAGE_MAP()

protected:
   void LoadStdProfileSettings(UINT nMaxMRU=4);

private:
   void NewOverlay(boolean_t same_as_current);
   void OpenOverlay();
   void SaveOverlay(boolean_t current_overlay);
   void InitRegistry();
   BOOL RegisterTypeLibrary(const CString& strTypeLibPath);

   void RunAllTests();

   // cleans out the coverage database (coverage tables, data sources, and region rowset)
   void CleanCoverageDB();

   // make sure that FalconView is running on an supported platform
   int VerifyPlatform();

   //  Figure out the number of colors. If running in less than
   //  HiColor mode (16bit), display a "warning" message and return FAILURE
   int VerifyColorDepth();

   // Returns TRUE if there are any file overlays or any dirty overlays.
   // Returns FALSE otherwise.
   boolean_t is_save_enabled(BOOL bTestModified);

#ifndef USE_POSTGRESQL  // RP
   BOOL EnableSqlExpressTcpIpProtocol();
#endif

protected:
   //
   // use this instead of CWinApp::RunAutomated
   //
   boolean_t am_automated(void);
   //
   // use this instead of CWinApp::RunEmbedded
   //
   boolean_t am_embedded(void);

#ifdef _DEBUG
   //
   // indicates whether the debug run-time library is 
   // checking for memory errors during execution
   //
   boolean_t m_memory_debugging_on;
#endif

private:
   void ShowTipAtStartup(void);
   void StartFeatureService(void);
};


// callback function added to correct character buffer overwrite error introduced by the upgrade to DAO 3.6
STDAPI MyDaoStringAllocCallback(DWORD dwLen, DWORD pData, void** ppv);


/////////////////////////////////////////////////////////////////////////////
#endif
