// Copyright (c) 1994-2014 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "stdafx.h"

#ifndef __AFXADV_H__
   #include <afxadv.h>  // CRecentFileList
#endif

#include "gps.h"
#include "fulldrag.h"
#include "getobjpr.h"
#include "mem.h"
#include "graphics.h"
#include "mapx.h"
#include "test.h"
#include "tstsheet.h"
#include "utils\appexpiration.h"
#include "CommandLine.h"
#include "prnttool.h"
#include "favorite.h"
#include "factory.h"
#include "FctryLst.h"
#include "MdsUtilities.h"
#include "MapDataServer\PostgresDefs.h"
#include "catalog\MDSWrapper.h"
#include "scene_mgr\SDSWrapper.h"
#include "SystemHealthDialog.h"
#include "mov_sym.h"
#include "fvw\appname.h"
#include "fvw\aboutdlg.h"
#include "appinfo.h"
#include "nitf.h"
#include "fovllist.h"
#include "Splash.h"
#include "TipDlg.h"
#include "showrmk.h"
#include "FvCore\Include\ProcessUtils.h"
#define NO_GRAPHIC_ELEMENT_DRAWING_CACHE_MACROS
#include "FvwGraphicDrawingSize.h"
IMPLEMENT_GRAPHIC_ELEMENT_DRAWING_SIZE_CACHE
IMPLEMENT_EDITOR_TOOLBAR_BUTTON_SIZE_CACHE
IMPLEMENT_MAP_TABS_BAR_INFO_CACHE
#include "ExpirationDialog.h"
#include "SlatToolLauncher.h"
#include "PlaybackDialog\viewtime.h"
#include "mapdoc.h"
#include "MapEngineCOM.h"
#include "FvCore/Include/Registry.h"
#include "GdalInitializer.h"
#include "scene_mgr/SDSWrapper.h"
#include "net_fetcher.h"
#include "GeospatialViewController.h"
#include "GeospatialViewer.h"

// ------------------------------------------------------------------------
// static function prototypes
// ------------------------------------------------------------------------

static int change_to_startup_favorite(MapView* view, int view_width, int view_height, const CString& favorite_name);

// Ignore WM_PAINT messages during COM external calls
BOOL CMyMessageFilter::OnMessagePending(const MSG* pMsg)
{
   //The base class function dispatches WM_PAINT
   //by returning FALSE no messages are being processed; the
   //user can add code here to appropriately handle messages.
   //WARNING: Not processing WM_PAINT messages will cause the user
   //         interface to appear to hang and not update until the
   //         current COM method call returns.
   MSG msg;
   bool dispatched_wm_paint = false;
   while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE|PM_NOYIELD))
   {
      // We set this to true because we do not want to draw while dispatching
      // this next msg (which is a WM_PAINT message)
      m_dispatching_wm_paint = true;
      // Now we dispatch the WM_PAINT message in the queue to clear it out of
      // the queue
      ::DispatchMessage(&msg);

      // Go back to default state
      m_dispatching_wm_paint = false;

      // We want to make sure the WM_PAINT does get properly handled
      // (eventually)
      dispatched_wm_paint = true;
   }

   if (dispatched_wm_paint)
      OVL_get_overlay_manager()->invalidate_view();


   return FALSE;
}

// Special case MRU to handle URLs (Http:)
void CUnverifiedRecentFileList::Add(LPCTSTR lpszPathName)
{
   ASSERT(m_arrNames != NULL);
   ASSERT(AfxIsValidString(lpszPathName));

   // fully qualify the path name
   TCHAR szTemp[_MAX_PATH];

   if (lpszPathName == NULL)
   {
      ASSERT(FALSE);
      // MFC requires paths with length < _MAX_PATH
      // No other way to handle the error from a void function
      AfxThrowFileException(CFileException::badPath);
   }

   if (DoesFileExist(lpszPathName))
   {
      CRecentFileList::Add(lpszPathName);
      return;
   }
   else
   {
      // this is not a file that exists, but it may be a
      // generalized connect string
      if (lstrlen(lpszPathName) <= _MAX_PATH)
      {
         lstrcpyn(szTemp, lpszPathName, _MAX_PATH);
      }
      else
      {
         return;
      }
   }

   // update the MRU list, if an existing MRU string matches file name
   int iMRU;
   for (iMRU = 0; iMRU < m_nSize-1; iMRU++)
   {
      if (lstrcmpi(m_arrNames[iMRU], szTemp)==0)
         break;      // iMRU will point to matching entry
   }

   // move MRU strings before this one down
   for (; iMRU > 0; iMRU--)
   {
      ASSERT(iMRU > 0);
      ASSERT(iMRU < m_nSize);
      m_arrNames[iMRU] = m_arrNames[iMRU-1];
   }

   // place this one at the beginning
   m_arrNames[0] = szTemp;
}

BOOL CUnverifiedRecentFileList::GetDisplayName(CString& strName, int nIndex,
   LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName) const
{
   ASSERT(m_arrNames != NULL);
   ENSURE_ARG(nIndex < m_nSize);
   if (m_arrNames[nIndex].IsEmpty())
      return FALSE;

   int nLenName = m_arrNames[nIndex].GetLength();
   CString sTemp = m_arrNames[nIndex];

   if (DoesFileExist(sTemp))
   {
      return CRecentFileList::GetDisplayName(strName, nIndex,
         lpszCurDir, nCurDir, bAtLeastName);
   }
   else
   {
      // this is not a file that exists, but it may be a connect string
      if (sTemp.Left(20) == "<WMS_CONNECT_STRING>")
      {
         // special case for WMS connect strings
         int iStart = sTemp.Find("<URL>") + 5;
         int iEnd = sTemp.Find("</URL>", iStart);
         strName = sTemp.Mid(iStart, iEnd - iStart);
      }
      else if (nLenName < m_nMaxDisplayLength)
      {
         strName = sTemp;
      }
      else
      {
         strName = sTemp.Left(m_nMaxDisplayLength - 3) + "...";
      }

      return TRUE;
   }
}

// ------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CFVApp

BEGIN_MESSAGE_MAP(CFVApp, FV_APP_BASE_CLASS)
   //{{AFX_MSG_MAP(CFVApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
   ON_COMMAND(ID_FILE_CLOSE_FV, OnFileCloseFv)
   ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_FV, OnUpdateFileCloseFv)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_FV, OnUpdateFileSaveFv)
   ON_COMMAND(ID_HELP_TECHNICALSUPPORT, OnHelpTechnicalsupport)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_FV, OnUpdateFileSaveAsFv)
   ON_COMMAND(ID_FILE_NEW, OnFileNewFv)
   ON_COMMAND(ID_FILE_SAVE_AS_FV, OnFileSaveAs)
   ON_COMMAND(ID_FILE_SAVE_FV, OnFileSave)
   ON_COMMAND(ID_CURRENT_NEW, OnCurrentNew)
   ON_COMMAND(ID_CURRENT_OPEN, OnCurrentOpen)
   ON_COMMAND(ID_CURRENT_SAVE, OnCurrentSave)
   ON_UPDATE_COMMAND_UI(ID_CURRENT_SAVE, OnUpdateCurrentSave)
   ON_COMMAND(ID_PURGE_RECENT_FILE_LIST, OnPurgeRecentFileList)
   ON_UPDATE_COMMAND_UI(ID_PURGE_RECENT_FILE_LIST, OnUpdatePurgeRecentFileList)
   //}}AFX_MSG_MAP
   // Standard print setup command
   ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
   // Global help commands
   ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)// Standard CWinApp help
   ON_COMMAND(ID_HELP, OnHelp)               // Standard CWinApp help
   ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)  // Standard CWinApp help
   ON_COMMAND(ID_DEFAULT_HELP, OnHelpFinder) // Standard CWinApp help
   ON_COMMAND(ID_HELP_USING, OnHelpUsing)    // Standard CWinApp help
   ON_COMMAND(ID_DAFIF_SELECTION , OnDafifSelection)
   ON_COMMAND(ID_TOOLS_TEST, OnToolsTest)      // added manually by DEP 31 July 96
   ON_COMMAND(ID_SLAT_TOOL, OnSLAT)            // added manually by WHS on 14 Mar 08
   ON_COMMAND(ID_PLUGIN_MANAGER, OnPluginManager)            // added manually by WHS on 9 Mar 2010
   ON_COMMAND(ID_SAVE_ALL, OnSaveAll)
   ON_UPDATE_COMMAND_UI(ID_SAVE_ALL, OnUpdateSaveAll)
   ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
   ON_COMMAND(ID_HELP_TIPOFTHEDAY, ShowTipOfTheDay)
   ON_COMMAND(ID_XPLAN_OPTIONS, XPlanOptions)
#ifdef _DEBUG
   ON_COMMAND(ID_MEMORY_DEBUGGING, OnMemoryDebugging)
   ON_UPDATE_COMMAND_UI(ID_MEMORY_DEBUGGING, OnUpdateMemoryDebugging)
   ON_COMMAND(ID_CHECK_MEMORY, OnCheckMemory)
#endif

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFVApp construction

CFVApp::CFVApp(LPCSTR theAppName) : FV_APP_BASE_CLASS()
{
   m_pMainWnd = NULL;

#ifdef _DEBUG
   m_debugVersion = TRUE;
#else
   m_debugVersion = FALSE;
#endif

   m_recent_file_list = new RecentFileOverlayList();

#ifdef _DEBUG
   m_memory_debugging_on = FALSE;
#endif

   m_expiration = new CAppExpiration();

}

// Destructor
CFVApp::~CFVApp()
{
   ASSERT(m_recent_file_list);

   delete m_recent_file_list;

   delete m_expiration;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFVApp object

// The application title is specified via IDR_MAINFRAME_FALCONVIEW in version
// resource string table.  However, the string that shows up in the title bar
// of message boxes is the one specified by AFX_IDS_APP_TITLE.
CFVApp theApp(NULL);

/////////////////////////////////////////////////////////////////////////////
// CFVApp initialization

BOOL CFVApp::InitInstance()
{
   CFVCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   ::CoInitialize(NULL);

   if (cmdInfo.get_run_all_tests())
   {
      RunAllTests();
      return TRUE;
   }

   m_is_regserver = cmdInfo.IsRegServer();
   m_is_unregserver = cmdInfo.IsUnRegServer();

   //***************************************************************************************
   // The MFC DAO classes that ship with Visual C++ 6.0 load DAO v3.5 (Dao350.dll) by 
   // default. DAO v3.5 uses Jet 3.5, which can only open Jet 3.5 format (or earlier) 
   // databases.  Access 2000 (and later) creates Jet 4.0 format database files, which 
   // are unrecognizable to Jet 3.5.  In order to successfully open the new Access XP 
   // version of the DAFIF database when using the MFC DAO classes, we need to use 
   // DAO v3.6 (Dao360.dll).  DAO v3.6 uses Jet 4.0, which can open any available Access 
   // database format.
   //
   // In order for FalconView to use version 3.6 of DAO, we must update the version of MFC 
   // at run time to MFC version 6.01.  We specify that we want MFC to use DAO 3.6 by 
   // inserting the following line of code before we open the Access XP DAFIF database.
   // This single line of code specifies that MFC will load DAO v3.6 (Dao360.dll) instead
   // of DAO v3.5 (Dao350.dll), and this will allow the existing MFC DAO classes in
   // FalconView to open Access 2000 (or later) databases.

   AfxGetModuleState()->m_dwVersion = 0x0601;

   ::CoInitialize(NULL);

   //
   // Enable heap terminate-on-corruption.
   // We can continue to run even if this call fails, so it is safe to
   // ignore the return value
   ::HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

   // Ensure that exceptions are not swallowed when dispatching certain Windows
   // messages.
   process_utils::DisableExceptionSwallowing();

   // create the one and only CFvwUtil instance
   CFvwUtil::create_instance();

   // determine whether or not this is the first instance of the program
   m_Mutex = CreateMutex(NULL,TRUE,"FalconViewPreventSecondInstance");

   if (GetLastError() == ERROR_ALREADY_EXISTS)
   {
      CFvwUtil *futil = CFvwUtil::get_instance();
      HWND hWnd;
      UTL_set_instance(2);

      CString str = PRM_get_registry_string("Main", "allow_second_instance", "NO");

      // if we are not allowed to have second instance of FalconView open
      if (str.CompareNoCase("YES") != 0)
      {
         // Bring the existing instance of FalconView into focus
         if (futil->GetExternalApplicationIsOpen("- FalconView", &hWnd, FALSE))
         {
            WINDOWPLACEMENT wndpl;
            ::GetWindowPlacement(hWnd, &wndpl);

            // restore the window if FalconView is minimized
            if (wndpl.showCmd == SW_SHOWMINIMIZED)
            {
               wndpl.showCmd = SW_RESTORE;
               ::SetWindowPlacement(hWnd, &wndpl);
            }

            ::SetForegroundWindow(hWnd);

            // determine if the /F (goto favorite) option was specified.  If
            // it was, then we will send a message to the existing instance
            // to open the favorite with the given index
            if (cmdInfo.get_favorite_name() != "")
            {
               FavoriteList *fav_list = FavoriteList::get_instance();

               // obtain an index to the favorite with the given name
               int index = fav_list->Find(cmdInfo.get_favorite_name());

               if (index != -1)
               {
                  PostMessage(hWnd, WM_COMMAND,
                     (WPARAM)index + IDRANGE_FAVORITE_MENU_ITEM_1, NULL);
               }
            }

            // determine if the /O (open overlay) option was specified.  If
            // it was, then we will notify the existing instance to open the
            // overlay with the given name
            if (cmdInfo.get_overlay_name() != "")
            {
               char overlayName[MAX_PATH];
               strcpy_s(overlayName, MAX_PATH, cmdInfo.get_overlay_name());

               COPYDATASTRUCT copyData;
               copyData.cbData = MAX_PATH;
               copyData.lpData = reinterpret_cast<void *>(overlayName);

               ::SendMessage(hWnd, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&copyData));
            }
         }

         return FALSE;
      }
      // open another FalconView but notify the user that some features may be
      // disabled
      else
         AfxMessageBox("Note: This is not the first instance of "+ appShortName() +
                                          ".  Some features are disabled.");
   }
   else
   {
      UTL_set_instance(1);
   }

   // CAB moved initializiation up here so I can use GDIplus in the splash screen  (GRA_open_graphics calls GdiplusStartup)
   if (GRA_open_graphics() != SUCCESS)
   {
      const int MSG_LEN = 80;
      char msg[MSG_LEN];
      sprintf_s(msg, MSG_LEN, "Graphics initialization failed. See %s file.", (const char*) appErrorFile());
      AfxMessageBox(msg);

      return FALSE;
   }

   // Show splash screen if not started as an automation or embedded server.
   // The splash screen keeps the server from starting properly in these cases.
   if ( !am_automated() && !am_embedded() && !m_is_regserver && !m_is_unregserver )
   {
      CSplashWnd::SILENT=FALSE;
      CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
      CSplashWnd::ShowSplashScreen(FALSE);
      CSplashWnd::Message("Initializing FalconView...");
   }
   else
      CSplashWnd::SILENT=TRUE;

   // initialize GDAL
   GdalInitializer::InitGDAL();

   /*
   *  Initialize OLE libraries
   */
   if (!AfxOleInit())
   {
      AfxMessageBox(IDP_OLE_INIT_FAILED);
      return FALSE;
   }

   // register new message filter handler to ignore WM_PAINT
   // messages during COM esternal calls
   CWinThread* pThread = AfxGetThread();
   if (pThread != NULL)
   {
      // Destroy message filter, thereby unregistering it.
      delete pThread->m_pMessageFilter;
      pThread->m_pMessageFilter = NULL;

      // Create the new message filter object.
      pThread->m_pMessageFilter = new CMyMessageFilter;
      ASSERT(AfxOleGetMessageFilter() != NULL);

      // Register the new message filter object.
      AfxOleGetMessageFilter()->Register();
   }

   // Disable the dialog box that appears when a called application is busy
   // or not responding
   COleMessageFilter *filter = AfxOleGetMessageFilter();
   filter->EnableBusyDialog(FALSE);
   filter->EnableNotRespondingDialog(FALSE);

   /*
   *  Register all OLE server (factories) as running.  This enables
   *  the OLE 2.0 libraries to create objects from other applications.
   *
   *  Note:  make sure near the start of this function in order to
   *  make sure clients won't have any problem creating a FalconView
   *  object.
   */
   if (UTL_get_instance() == 1 && !m_is_unregserver)
   {
      COleTemplateServer::RegisterAll();

      if (!am_embedded() && !am_automated())
      {
         //if (PRM_get_registry_int("Developer", "INIT_VERBOSE", 0))
         //   INFO_report("Not embedded and not automated.");

         // When a server application is launched stand-alone, it is a good
         // idea to update the system registry in case it has been damaged.
         COleObjectFactory::UpdateRegistryAll();
      }
      else
      {
         //if (PRM_get_registry_int("Developer", "INIT_VERBOSE", 0))
         //   INFO_report("Running embedded or automated.");

         // Do to the fact that there is a conflict between the route server
         // and map server automation interfaces the route server class must
         // be aware that FalconView was started as an automation server, so
         // it can avoid the conflict.
         if (UTL_is_win32_windows())
         {
            //if (PRM_get_registry_int("Developer", "INIT_VERBOSE", 0))
            //   INFO_report("Calling ActivateWindows95KludgeWait()");
         }
      }
   }
   else if (m_is_unregserver)
      COleTemplateServer::RevokeAll();

   // When an OLE object is created via C#, it does the following:
   // 1. Posts a message which ends up calling AfxOleLockApp in FV
   // 2. FV creates an instance of the object
   // 3. Finally, posts a message that eventually calls AfxOleUnlockApp
   //
   // AfxOleUnlockApp will shut down the application when all the objects have been
   // released (the application's reference count goes to zero).  But, the application will
   // only shut down this way in the case that the user is not in control of the app.  The user
   // is not in control of the app when the app is started by automation, for instance.  When message
   // 1) is handled, the reference count becomes one.  When message 3) is handled the reference count
   // becomes zero and the application exits.
   //
   // While connecting to the route server other messages are allowed to be processed
   // (in this case the messages from 1 and 3).  The same problem can be demonstrated by
   // replacing the call to connect to the route server with a call to AfxMessageBox, say.
   // Anything that allows messages to be pumped.
   //
   // For some reason, the messages in 1) and 3) are not posted to FV when an object is created
   // via C++ or VB.  C# is creating the objects differently.
   //
   // The solution is to make sure the user is control of the app before making any calls
   // that can pump messages.  Note that this will cause a MapServerOnly client to leave the
   // fvw.exe process alive even after all objects are destroyed.
   //
   // Better would be to figure out why C# is creating objects differently that C++ and VB and
   // figure out to create them in a similar manner.
   //
   AfxOleSetUserCtrl(TRUE);

   // if /CLEANCOVDB specified on the command line, then wipe data sources, coverage tables,
   // and region table
   if (cmdInfo.CleanCoverageDB())
   {
      CSplashWnd::Message("Cleaning coverage database...");
      CleanCoverageDB();
      CSplashWnd::Message("Initializing FalconView...");
   }

   // make sure that FalconView is running on an supported platform
   if (VerifyPlatform() != SUCCESS)
      return FALSE;

   //  Figure out the number of colors. If they are running in less than
   //  HiColor mode (16bit), display a "warning" message and exit FalconView.
   if (!m_is_regserver && VerifyColorDepth() != SUCCESS)
      return FALSE;

   ERR_writeStartupInfoToLog();

   {
      // We don't want to process any incoming COM calls until the overlay type
      // list has finished initializing. For example, if ILayer::CreateLayer
      // is called and one of the overlay types makes an outgoing COM call
      // (e.g., a C# plug-in), then the CreateLayer could be processed before
      // the entire type list is initialized and thus fail.
      static_cast<CMyMessageFilter *>(pThread->m_pMessageFilter)->SetCanHandleIncomingCalls(FALSE);

      // initialize the overlay type descriptors
      if (!m_is_regserver && !m_is_unregserver)
         OVL_get_type_descriptor_list()->Initialize();

      static_cast<CMyMessageFilter *>(pThread->m_pMessageFilter)->SetCanHandleIncomingCalls(TRUE);
   }

   // Setup registry entry point before any MFC calls to access the registry,
   // and before any PRM calls.
   InitRegistry();

   // make sure the map data server is initialized - exit gracefully if not
   if (!m_is_unregserver && !m_is_regserver)
   {
      CSplashWnd::Message("Initializing Map Data Server...");
      if (CMdsNotifyEvents::CreateInstance() == FALSE)
      {
         CSplashWnd::Message("Initialization failed");
         // don't destroy the window right away so that the user can see the error message in the splash screen
         Sleep(3500);
         if (CSplashWnd::GetWnd() != NULL)
            CSplashWnd::GetWnd()->DestroyWindow();

         return FALSE;
      }
   }

   // make sure the scene data server is initialized - exit gracefully if not
   if (!m_is_unregserver && !m_is_regserver)
   {
      CSplashWnd::Message("Initializing Scene Data Server...");

      // Check to see if the database has been initialized
      long targetSourceId;
      if(scene_mgr::SDSWrapper::GetInstance()->GetTargetSourceId(&targetSourceId) != SUCCESS)
      {
         // Get the default scene data path
         std::string default_path = reg::get_registry_string("Main",
            "ReadWriteUserData", "");
         default_path += "\\Scenes";

         // Initialize the database
         if( scene_mgr::SDSWrapper::GetInstance()->InitializeDatabase(default_path) != SUCCESS)
         {
            CSplashWnd::Message("Initialization failed");
            // don't destroy the window right away so that the user can see the error message in the splash screen
            Sleep(3500);
            if (CSplashWnd::GetWnd() != NULL)
               CSplashWnd::GetWnd()->DestroyWindow();

            return FALSE;
         }
      }
   }

   CSplashWnd::Message("Initializing FalconView...");

   if (m_is_regserver)
   {
      // rewrite the the ReadWriteUserData registry key so it doesn't contain
      // a trailing backslash to be consistent with other directory keys
      // in 'Main'
      // TODO: remove this block when a custom action is no longer required
      // by the installer to create the directory (custom action #35 will
      // always append a trailing backslash).
      {
         CString crnt = PRM_get_registry_string("Main", "ReadWriteUserData");
         const int length = crnt.GetLength();
         if (length && crnt[length - 1] == '\\')
         {
            PRM_set_registry_string("Main", "ReadWriteUserData",
               crnt.Left(length - 1));
         }
      }
      // Same for PublicCommonAppData registry entry
      {
         CString crnt = PRM_get_registry_string("Main", "PublicCommonAppData");
         const int length = crnt.GetLength();
         if (length && crnt[length - 1] == '\\')
         {
            PRM_set_registry_string("Main", "PublicCommonAppData",
               crnt.Left(length - 1));
         }
      }

#ifdef USE_POSTGRESQL
      // Make sure the PostgreSQL service is running
      RunSQLService();

      // Retrieve the PostgreSQL binary path for use in running the psql
      // scripts, etc, below
      ULONG nChars = MAX_PATH;
      char postgresBinDir[MAX_PATH];
      CRegKey regkey;
      regkey.Open(HKEY_LOCAL_MACHINE, "Software\\MissionPlanning", KEY_READ);
      regkey.QueryStringValue("postgresBinDir", postgresBinDir, &nChars);
      PathAddBackslash(postgresBinDir);

      // Retrieve the port on which the database is running
      DWORD postgresPort;
      regkey.QueryDWORDValue("postgresPort", postgresPort);

      // Get the path at which the psql scripts are stored
      CString supportDir = PRM_get_registry_string("Main", "USER_DATA", "");
      supportDir += "\\scripts\\";

      // Retrieve JMPS directory for checking the initial DAFIF database name
      DWORD size = MAX_PATH;
      char jmpsDir[MAX_PATH];
      regkey.Open(HKEY_LOCAL_MACHINE, "SOFTWARE\\JMPS\\FW", KEY_READ);
      regkey.QueryStringValue("JmpsDir", jmpsDir, &size);
      PathAddBackslash(jmpsDir);

      // Create MDS database
      std::stringstream create_db;
      create_db << "\"" << postgresBinDir << "psql.exe\""
         << " -h " << PG_LOCALHOST
         << " -p " << postgresPort
         << " -d " << PG_MASTER_DATABASE_NAME
         << " -U " << PG_FALCONVIEW_UID
         << " -f \"" << supportDir << "init_mds.psql\"";
      DWORD exit_code = process_utils::LaunchAppAndWait(create_db.str());
      if (exit_code != ERROR_SUCCESS)
      {
         CString msg;
         msg.Format("Create MDS database failed. Exit code = %d. Make sure"
            " that service \"PostgreSQL (XPlan)\" is running.", exit_code);
         ERR_report(msg);
      }

      // Populate the map groups table
      try {
         IMapGroupsPtr pMapGroups(CLSID_MapGroups);
         pMapGroups->UpdateTable();
      }
      catch(_com_error e)
      {
         CString msg("Map groups table could not be created and updated."
            " Make sure that service \"PostgreSQL (XPlan)\" is running.");
         ERR_report(msg);
      }

      // Create Tactical Imagery database
      INITFDBAgentPtr smpNITFDBAgent( _uuidof( NITFDBAgent ) );
      smpNITFDBAgent->raw_Open( L"", L"", L"", NULL );
      C_nitf_ovl::InitRegistry( FALSE );  // Register as imagery handler for certain types


#ifdef GOV_RELEASE
      // Only create the DAFIF database if the DAFIF.DUMP file is available.
      // If DAFIF is not installed, then this file will not exist.
      std::string filename = "" + supportDir + "DAFIF.dump";
      const int read_mode = 04;  // Read-only
      if (0 == _taccess(filename.c_str(), read_mode))
      {
         // Determine the name of the default DAFIF database
         bool successful = true;
         _bstr_t default_database_name;
         {
            _bstr_t filePath = jmpsDir;
            filePath += "dafif_active_database.txt";
            std::ifstream file((char *) filePath);
            if (file)
            {
               std::string buffer;
               file >> buffer;
               default_database_name = buffer.data();
            }
            else
               successful = false;
         }

         // Create the DAFIF database
         if(successful)
         {
            std::stringstream create_dafif_database;
            create_dafif_database << "\"" << postgresBinDir << "createdb.exe\""
               << " -h " << PG_LOCALHOST
               << " -p " << postgresPort
               << " -U " << PG_FALCONVIEW_UID
               << " -T postgis_template"
               << " \"" << (char *) default_database_name << "\"";
            exit_code = process_utils::LaunchAppAndWait(create_dafif_database.str());
            if (exit_code != ERROR_SUCCESS)
            {
               CString msg;
               msg.Format("DAFIF database creation failed. Exit code = %d", exit_code);
               ERR_report(msg);
               successful = false;
            }
         }

         // Create the dafif schema in the database
         if(successful)
         {
            std::stringstream create_dafif_schema;
            create_dafif_schema << "\"" << postgresBinDir << "psql.exe\""
               << " -h " << PG_LOCALHOST
               << " -p " << postgresPort
               << " -d " << (char *) default_database_name
               << " -U " << PG_FALCONVIEW_UID
               << " -c " << " \"CREATE SCHEMA dafif; ALTER DATABASE \\\""
               << (char *) default_database_name
               << "\\\" SET search_path TO dafif, postgis, topology, public;\"";
            exit_code = process_utils::LaunchAppAndWait(create_dafif_schema.str());
            if (exit_code != ERROR_SUCCESS)
            {
               CString msg;
               msg.Format("DAFIF schema creation failed. Exit code = %d", exit_code);
               ERR_report(msg);
               successful = false;
            }
         }

         // Populate DAFIF database
         if(successful)
         {
            std::stringstream populate_dafif_database;
            populate_dafif_database << "\"" << postgresBinDir << "pg_restore.exe\""
               << " -h " << PG_LOCALHOST
               << " -p " << postgresPort
               << " -d " << (char *) default_database_name
               << " -U " << PG_FALCONVIEW_UID
               << " -j 4"
               << " -n dafif"
               << " \"" << filename << "\"";
            exit_code = process_utils::LaunchAppAndWait(populate_dafif_database.str());
            if (exit_code != ERROR_SUCCESS)
            {
               CString msg;
               msg.Format("DAFIF database population failed. Exit code = %d", exit_code);
               ERR_report(msg);
            }
         }
      }
#endif

#else
      // make a connection to the SQL server when installing as admin so the proper user
      // permissions are set up in the database (any call to the DB will do)
      try
      {
         if (EnableSqlExpressTcpIpProtocol())
         {
            // make sure the LDF has been deleted.  This can only be done after
            // the service is stopped
            DeleteLDFs( IDR_MAINFRAME_FALCONVIEW );

            RunSQLService();     // Get SQL service running again

            IMapHandlersRowsetPtr smpMapHandlers(__uuidof(MapHandlersRowset));
            smpMapHandlers->SelectAll();

            INITFDBAgentPtr smpNITFDBAgent( _uuidof( NITFDBAgent ) );
            smpNITFDBAgent->raw_Open( L"", L"", L"", NULL );
            C_nitf_ovl::InitRegistry( FALSE );  // Register as imagery handler for certain types

            // Attach the DAFIF database using the same code as MapDataServer and NITF
            //
#ifdef GOV_RELEASE
            IDAFIFUtilityPtr smpDAFIFUtility;
            CO_CREATE(smpDAFIFUtility,  _uuidof( DAFIFDataAccessAgent ) );

            _bstr_t DAFIFFilePath;
            smpDAFIFUtility->GetDAFIFFilepath(DAFIFFilePath.GetAddress());

            _bstr_t DAFIFDBName;
            _bstr_t DAFIFFileName;
            smpDAFIFUtility->GetDAFIFDBName(DAFIFFilePath, DAFIFDBName.GetAddress(), DAFIFFileName.GetAddress());

            AttachDB(GetSqlServerName().c_str(), (char*)DAFIFDBName, (char*)DAFIFFileName);
#endif // #ifdef GOV_RELEASE
         }
      }
      catch(_com_error& e)
      {
         CString msg;
         msg.Format("Failed initializing DBs: %s", (char *)e.Description());
         ERR_report(msg);
      }
#endif // #ifdef USE_POSTGRESQL else
   }

   // If the registry flag is set to hide the application when it acting as a
   // map server.
   if (PRM_get_registry_int("Main", "Map Server Only", 0))
   {
      // Do not show any user interface if we are acting as a map server.
      m_gui_hidden = (am_embedded() || am_automated());
   }
   else
      m_gui_hidden = false;


   if (!m_is_regserver)
   {
      // setup the overlay to be opened specified on the command line with the /O option
      OVL_get_overlay_manager()->set_startup_overlay(cmdInfo.get_overlay_name());
   }

   // Stop the SQL service when /UNREGSERVER is used
   if (m_is_unregserver)
   {
#ifdef USE_POSTGRESQL
      try
      {
         // Make sure the PostgreSQL service is started
         RunSQLService();

         // Retrieve the PostgreSQL binary path for use in running the psql
         // scripts, etc, below
         ULONG nChars = MAX_PATH;
         char postgresBinDir[MAX_PATH];
         CRegKey regkey;
         regkey.Open(HKEY_LOCAL_MACHINE, "Software\\MissionPlanning", KEY_READ);
         regkey.QueryStringValue("postgresBinDir", postgresBinDir, &nChars);
         PathAddBackslash(postgresBinDir);

         // Retrieve the port on which the database is running
         DWORD postgresPort;
         regkey.QueryDWORDValue("postgresPort", postgresPort);

         // Drop the MDS database
         std::stringstream drop_db;
         drop_db << "\"" << postgresBinDir << "dropdb.exe\""
            << " -h 127.0.0.1"
            << " -p " << postgresPort
            << " -U postgres"
            << " FV-JMPSMapCov";
         DWORD exit_code = process_utils::LaunchAppAndWait(drop_db.str());
         if (exit_code != ERROR_SUCCESS)
         {
            CString msg;
            msg.Format("Drop MDS database failed. Exit code = %d", exit_code);
            ERR_report(msg);
         }

         // Drop the NITF database
         std::stringstream drop_db_2;
         drop_db_2 << "\"" << postgresBinDir << "dropdb.exe\""
            << " -h 127.0.0.1"
            << " -p " << postgresPort
            << " -U postgres"
            << " NITFImagery";
         exit_code = process_utils::LaunchAppAndWait(drop_db_2.str());
         if (exit_code != ERROR_SUCCESS)
         {
            CString msg;
            msg.Format("Drop NITFImagery database failed. Exit code = %d", exit_code);
            ERR_report(msg);
         }

         // Need to find a way to drop DAFIF databases?
      }
      catch(...)
      {
         ERR_report("Unable to drop PostgreSQL databases.");
      }
#else
      try
      {
         // Detach the database
         AttachDB(GetSqlServerName().c_str(), _T(MDS_DB_NAME), NULL);
         AttachDB(GetSqlServerName().c_str(), _T("NITFImagery"), NULL);
         StopSQLService( IDR_MAINFRAME_FALCONVIEW );

         // make sure the LDF has been deleted.  This can only be done after
         // the service is stopped
         DeleteLDFs( IDR_MAINFRAME_FALCONVIEW );
      }
      catch(_com_error &e)
      {
         CString msg;
         msg.Format("Unable to shutdown SQL service: %s", (char *)e.Description());
         ERR_report(msg);
      }
#endif
      C_nitf_ovl::InitRegistry( TRUE );  // Unregister as imagery handler
   }

   // if the command line option /REGSERVER or /UNREGSERVER is used, then exit
   // the program (after registering the server above, of course. (Note that
   // the server will not be registered if this is not the first instance of
   // FalconView - I don't know if this will be a problem)
   if (m_is_regserver || m_is_unregserver)
   {
      return FALSE;
   }

   AfxEnableControlContainer();

   // load standard registry options (including MRU)...
   LoadStdProfileSettings(MAX_NUM_FILES_IN_MRU_LIST);


   // remove old timing log to avoid confusion
   UTL_remove_timing_log_if_it_has_not_been_opened();

   if (PRM_initialize() != SUCCESS)
   {
      ERR_report("PRM_initialize failed.");

      return FALSE;
   }

   // synchronize Fvw's m_recent_file_list with MFC's m_pRecentFileList, which was
   // initialized by LoadStdProfileSettings()...
   if (m_recent_file_list->initialize(m_pRecentFileList) != SUCCESS)
   {
      CString msg;

      ERR_report("RecentFileOverlayList::initialize() failed.");

      msg.Format("Your \"Recent File List\" and \"RecentFileOverlays\" "
               "sections of the registry are out of sync with each other.\n\n"
               "To fix this problem %s will purge the recent file list "
               "in the File menu, and exit.", appShortName());
      AfxMessageBox(msg);

      // delete and recreate the recent file overlay list, so it is empty and
      // it won't be saved
      delete m_recent_file_list;
      m_recent_file_list = new RecentFileOverlayList();

      // remove the recent file overlay list section from the registry
      PRM_delete_registry_section("RecentFileOverlays2");

      // delete the MFC recent file list object, so it won't be saved
      delete m_pRecentFileList;
      m_pRecentFileList = NULL;

      // remove the MFC recent file list section from the registry
      PRM_delete_registry_section("Recent File List");

      return FALSE;
   }

   // if the user specifies /NOOVERLAYS on the cmd line remove the restore list from the registry
   if(!cmdInfo.IsRestoreOverlays())
   {
      PRM_set_registry_int("Overlay Manager\\Startup2","Overlay Count", 0);
   }

   // Initialize all managers used by the ribbon control
   //
   InitContextMenuManager();
   InitKeyboardManager();
   InitTooltipManager();

   CMFCToolTipInfo params;
   params.m_bVislManagerTheme = TRUE;

   theApp.GetTooltipManager()->SetTooltipParams(0xFFFF, RUNTIME_CLASS(CMFCToolTipCtrl), &params);

   // Register the application's document templates.  Document templates
   // serve as the connection between documents, frame windows and views.
   int thisMainFrameReference;
   thisMainFrameReference = IDR_MAINFRAME_FALCONVIEW;

   AddDocTemplate(new CSingleDocTemplate(thisMainFrameReference,
               RUNTIME_CLASS(CMapDoc),
               RUNTIME_CLASS(CMainFrame),
               RUNTIME_CLASS(MapView)));

#ifdef GOV_RELEASE
   // if FV is going to expire in 30 days or less, then put up
   // a warning to notify users
   int days_left = fvw_get_app()->m_expiration->number_of_days_until_expiration();
   if (days_left <= 30 && days_left > 0)
   {
      CString msg;
      msg.Format("This release of FalconView will expire in %d days!!!\n", days_left);
      AfxMessageBox(msg);
   }

   // check to see if FalconView has expired, if so put up message
   if (fvw_get_app()->m_expiration->is_expired())
   {
      CExpirationDialog dlg;
      dlg.DoModal();
      // if falconView is still expired (user hasn't entered extension code) then exit
      if (fvw_get_app()->m_expiration->is_expired())
         PostQuitMessage(0);  //TO DO: kevin: we get memory leaks when this happens
   }

   // display pre-release warning message - if needed
   if (need_to_display_non_release_warning_dialog())
   {
      if (!PRM_test_string("Settings", "Developer", "Yes"))
      {
         char app_name[80];
         char versionStr[128];

         // get information from version resource
         CString buildType = project_build_type();
         project_product_name(app_name, 80);
         project_short_version_str(versionStr, 128);

         // build basic message string
         CString message;
         message.Format("This is an uncertified pre-release %s version of %s "
            "%s.\n", buildType, app_name, versionStr);
         message += "This product is under development and should be used for "
            "demonstration\npurposes only.\n\n";

         // append expiration date information
         if (fvw_get_app()->m_expiration->never_expires())
            message += "Expiration date: NONE";
         else
            message += "Expiration date: " + fvw_get_app()->m_expiration->get_expiration_date_as_string();

         // add point of contact info
         message+="\n\nFor information on the release features or schedule, contact:\n\n"
            "\tFalconView Program Management Office\n"
            "\tGeorgia Tech Research Institute\n"
            "\t(404) 894-0889\n"
            "\tfalconview@gtri.gatech.edu";

         // Put up disclaimer.
         AfxMessageBox(message, MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
      }
   }
#endif  // GOV_RELEASE

   // create a new (empty) document - eventually calls CMainFrame::OnCreate
   OnFileNew();

   // A CMainFrame has now been defined, can handle accelerators now.
   // look to see if the KeyboardDefault key is in the registry, if it is not then create it.
   HKEY key;
   DWORD dwDisposition;
   LONG openRes = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\XPlan\\FalconView\\KeyboardDefault", 0, "REG_BINARY", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &dwDisposition);

   // accelerators from fvw resource
   HINSTANCE hInst = AfxFindResourceHandle(ATL_MAKEINTRESOURCE(IDR_MAINFRAME_FALCONVIEW), ATL_RT_ACCELERATOR); 
   HACCEL pTableReg = ::LoadAccelerators(hInst, ATL_MAKEINTRESOURCE(IDR_MAINFRAME_FALCONVIEW));
   int nAccelSize = ::CopyAcceleratorTable(pTableReg, NULL, 0);
   LPACCEL lpAccel = new ACCEL [nAccelSize]; 
   ::CopyAcceleratorTable(pTableReg, lpAccel, nAccelSize);

   // accelerators from the KeyboardDefault key
   CSettingsStoreSP regSP;
   CSettingsStore& reg = regSP.Create(FALSE, FALSE);
   reg.Open("Software\\XPlan\\FalconView\\KeyboardDefault");
   UINT uiSize;
   LPACCEL lpAccelDefault;
   HACCEL hAccelTableDefault;
   int nAccelSizeDefault = -1;
   bool keyboardDefaultDefined = false;
   bool keyboardDefaultMismatch = false;
   if (reg.Read("Accelerators", (LPBYTE*) &lpAccelDefault, &uiSize))
   {
      nAccelSizeDefault = uiSize / sizeof(ACCEL);
      ENSURE(lpAccelDefault != NULL);
      for (int i = 0; i < nAccelSizeDefault; i ++)
      {
         if (!CMFCToolBar::IsCommandPermitted(lpAccelDefault[i].cmd))
         {
            lpAccelDefault[i].cmd = 0;
         }
      }
      hAccelTableDefault = ::CreateAcceleratorTable(lpAccelDefault, nAccelSizeDefault);
      keyboardDefaultDefined = true;
   }
   else // the value of the KeyboardDefault key does not exist. Assign it to the fvw resource accelerators.
   {
      lpAccelDefault = lpAccel;
      RegSetValueEx(key, "Accelerators", 0, REG_BINARY, (LPBYTE) lpAccel, nAccelSize * sizeof(ACCEL));
      CString strSection;
      strSection.Format(_T("%sKeyboard-%d"), "Software\\XPlan\\FalconView\\Workspace\\", 0);
      fvw_get_frame()->m_hAccelTable = pTableReg;
      LONG openRes = RegDeleteKey(HKEY_CURRENT_USER, strSection); // delete any existing cached registry accelerators. This key will be redefined upon exit of program.
      nAccelSizeDefault = nAccelSize;
   }
   if (nAccelSizeDefault != nAccelSize) {
      keyboardDefaultMismatch = true; // KeyboardDefault defined but it of different size than fvw resource
   } 

   // decides if default accelerators differ from fvw resource accelerators
   if (keyboardDefaultDefined && !keyboardDefaultMismatch) 
   {
      for (int a=0; a<nAccelSize; a++) 
      {
         if (memcmp(&lpAccelDefault[a], &lpAccel[a], sizeof(ACCEL))) 
         {
            keyboardDefaultMismatch = true;
         }
      }
   }

   if (keyboardDefaultMismatch) // the size of DefaultKeyboard accelerators and fvw resource accelerators is different and/or there was a mismatched entry
   {
      lpAccelDefault = lpAccel;
      RegSetValueEx(key, "Accelerators", 0, REG_BINARY, (LPBYTE) lpAccel, nAccelSize * sizeof(ACCEL));
      CString strSection;
      strSection.Format(_T("%sKeyboard-%d"), "Software\\XPlan\\FalconView\\Workspace\\", 0);
      fvw_get_frame()->m_hAccelTable = pTableReg;
      LONG openRes = RegDeleteKey(HKEY_CURRENT_USER, strSection); // delete any existing cached registry accelerators. This key will be redefined upon exit of program.
   }
   else 
   {
      // nothing. Proper accelerators are already loaded into position.
   }

   RegCloseKey(key);
   delete [] lpAccel;

   // CFrameWndEx::LoadFrame can reset application's user control status (by
   // eventually reparsing the command-line parameters in
   // CComandLineInfo::ParseParamFlag). Reset it now.
   AfxOleSetUserCtrl(TRUE);

   // Splash window is still required for custom initializers that are setup in CMainFrame
   if (CSplashWnd::GetWnd() != NULL)
     CSplashWnd::GetWnd()->DestroyWindow();

   //
   // make sure that the view has been created
   //
   MapView* view = fvw_get_view();
   ASSERT(view != NULL);

   if (view == NULL)
      return FALSE;

   int view_width;
   int view_height;
   {
      CRect rect;
      view->GetClientRect(&rect);
      view_width = rect.Width();
      view_height = rect.Height();
   }

   // create the playback dialog
   if (!CMainFrame::GetPlaybackDialog().Create(IDD_VIEWTIME, NULL))
   {
      ERR_report("Playback Dialog initialization failed...");
      return -1;
   }
   CMainFrame::GetPlaybackDialog().ShowWindow(SW_HIDE);
   CMainFrame::SetPlaybackDialogActive(FALSE);

   // [Bug 1675] Creating the playback dialog at startup changes focus to an invisible window.  We need
   // to set the focus back to the main FalconView window
   view->SetFocus();

   CMainFrame *pFrame = fvw_get_frame();
   if (pFrame)
      pFrame->PostCreateWindow();

   if (!m_gui_hidden)
   {
      //
      // If the UI is not hidden, then change the map to the startup favorite, if one
      //
      change_to_startup_favorite(view, view_width, view_height, cmdInfo.get_favorite_name());
   }

   OVL_get_overlay_manager()->RestoreStartupOverlays();

   //Set timer
   //CONSTRUCTION NOTICE:
   //(1) need to define BASE_TIMER instead of FV_TOLL_TIP_TIME_OUT
   //(2) need to kill timer in OnDestroy
   if (!fvw_get_frame()->SetTimer(FV_TOOL_TIP_TIME_OUT, TICK_PERIOD, NULL))
      ERR_report("SetTimer() failed.");

   if ( !am_automated() && !am_embedded() )  // not automation server or OLE embedded
      ShowTipAtStartup();

   if (!m_is_unregserver && !m_is_regserver)
   {
      StartFeatureService();
   }

   return TRUE;
}

void CFVApp::StartFeatureService()
{
#ifdef GOV_RELEASE
   try
   {
      using namespace xplan_package_service;
      IFeatureServiceStarterPtr fss;
      if (SUCCEEDED(fss.CreateInstance(CLSID_FeatureServiceStarter)))
      {
         fss->StartFeatureService();
      }
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
#endif
}

#include "gtest/gtest.h"
void CFVApp::RunAllTests()
{
   // This allows the user to override the flag on the command line.
   ::testing::InitGoogleTest(&__argc, __targv);

   RUN_ALL_TESTS();
}

void CFVApp::LoadStdProfileSettings(UINT nMaxMRU)
{
   ASSERT_VALID(this);
   ASSERT(m_pRecentFileList == NULL);

   BOOL bNoRecentDocs = FALSE;
   GetSysPolicyValue(_AFX_SYSPOLICY_NORECENTDOCHISTORY, &bNoRecentDocs);
   if (nMaxMRU != 0 && !bNoRecentDocs )
   {
      // create file MRU since nMaxMRU not zero
      m_pRecentFileList = new CUnverifiedRecentFileList(0, _T("Recent File List"), _T("File%d"), nMaxMRU);
      m_pRecentFileList->ReadList();
   }
   // 0 by default means not set
   m_nNumPreviewPages = GetProfileInt(_T("Settings"), _T("PreviewPages"), 0);
}

#ifndef USE_POSTGRESQL  // RP
BOOL CFVApp::EnableSqlExpressTcpIpProtocol()
{
   INFO_report("Enabling the TCP/IP protocol in the SQL Server instance");

   // use FvCommand.exe EnableTcpIp to enable to TCP/IP protocol in the Sql Express instance
   //
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   std::string commandLine = PRM_get_registry_string("Main", "HD_DATA", "");
   commandLine += "\\..\\..\\MapDataServer\\FvCommand.exe EnableTcpIp";
   if( !::CreateProcess( NULL, const_cast<char *>(commandLine.c_str()), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
   {
      ERR_report("Unable to enable the TCP/IP protocol in the SQL Server instance");
      return FALSE;
   }

   WaitForSingleObject( pi.hProcess, INFINITE );

   // check return code
   DWORD dwExitCode = 0;
   if ( ::GetExitCodeProcess(pi.hProcess, &dwExitCode) && dwExitCode != ERROR_SUCCESS )
   {
      CString msg;
      msg.Format("Unable to enable the TCP/IP protocol in the SQL Server instance.  FvCommand EnableTcpIp returned %d", dwExitCode);
      ERR_report(msg);
   }

   CloseHandle( pi.hProcess );
   CloseHandle( pi.hThread );

   // the SQL server service needs to be restarted.  We'll just stop it here and let the next MDS command restart it automatically.
   if (dwExitCode == ERROR_SUCCESS)
   {
      StopSQLService( IDR_MAINFRAME_FALCONVIEW );
      return TRUE;
   }

   return FALSE;
}
#endif


// callback function added to correct character buffer overwrite error introduced by the upgrade to DAO 3.6
STDAPI MyDaoStringAllocCallback(DWORD dwLen, DWORD pData, void** ppv)
{
   LPTSTR lpsz;
   CString* pstr = (CString*)pData;

   dwLen++;

   TRY
   {
      //Allocate twice the space needed so that DAO does not overwrite the buffer
      lpsz = pstr->GetBufferSetLength(2*dwLen/sizeof(TCHAR));
      *ppv = (void*)(dwLen > 0 ? lpsz : NULL);
   }
   CATCH_ALL(e)
   {
      e->Delete();
      return E_OUTOFMEMORY;
   }
   END_CATCH_ALL

   return S_OK;
}

void CFVApp::CleanCoverageDB()
{
   try
   {
      IMapHandlersRowsetPtr smpMapHandlersRowset;
      ICoverageRowsetPtr smpCoverageRowset;
      IDataSourcesRowsetPtr smpDataSourcesRowset;
      IRegionRowsetPtr smpRegionRowset;
      ICsdIndexRowsetPtr smpCsdIndexRowset;

      CO_CREATE(smpMapHandlersRowset, __uuidof(MapHandlersRowset));
      CO_CREATE(smpCoverageRowset, __uuidof(CoverageRowset));
      CO_CREATE(smpDataSourcesRowset, __uuidof(DataSourcesRowset));
      CO_CREATE(smpRegionRowset, __uuidof(RegionRowset));
      CO_CREATE(smpCsdIndexRowset, __uuidof(CsdIndexRowset));

      HRESULT hr = smpMapHandlersRowset->SelectAll();
      while (hr == S_OK)
      {
         smpCoverageRowset->Initialize(smpMapHandlersRowset->m_MapHandlerName);
         smpCoverageRowset->DeleteAll();
         hr = smpMapHandlersRowset->MoveNext();
      }

      smpDataSourcesRowset->DeleteAll();
      smpRegionRowset->DeleteAll();
      smpCsdIndexRowset->DeleteAll();
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed cleaning database: %s", (char *)e.Description());
      ERR_report(msg);
   }
}

// make sure that FalconView is running on an supported platform
int CFVApp::VerifyPlatform()
{
   const CString app_name = appShortName();
   char ver_num[128];
   project_short_version_str(ver_num, 128);

   if (UTL_is_win32s())
   {
      CString msg;
      msg.Format("%s %s does not run on Windows 3.1x.  Please upgrade "
         "to the newest version of Windows 95 or Windows NT and "
         "reinstall %s.",
         (const char*) app_name, ver_num, (const char*) app_name);
      AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);

      if (UTL_get_instance() == 1)
      {
         ReleaseMutex(m_Mutex);
         CloseHandle(m_Mutex);
      }
      return FAILURE;
   }
   else if (UTL_is_win32_nt() && UTL_get_windows_major_version_number() < 4)
   {
      CString msg;
      msg.Format("%s %s will not run on this version of NT.  Please "
         "upgrade to Windows NT 4.0 or later.", 
         (const char*) app_name, ver_num);
      AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);
      if (UTL_get_instance() == 1)
      {
         ReleaseMutex(m_Mutex);
         CloseHandle(m_Mutex);
      }
      return FAILURE;
   }

   return SUCCESS;
}

//  Figure out the number of colors. If running in less than
//  HiColor mode (16bit), display a "warning" message and return FAILURE
int CFVApp::VerifyColorDepth()
{
   DEVMODE dm;
   dm.dmSize = sizeof(DEVMODE);
   dm.dmDriverExtra = 0;
   EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
   if (dm.dmBitsPerPel < 15)
   {
      AfxMessageBox("256 color or lesser graphics mode detected.\n\n"+
         appShortName()+" is designed to run in High Color (16bit) mode or better "
         "in resolutions above 1024x768.  If your graphics card and monitor "
         "support High Color mode, you can switch to this mode by selecting "
         "\"Display\" in the \"Control Panel\" and modifying the "
         "appropriate settings.");

      if (CSplashWnd::GetWnd() != NULL)
         CSplashWnd::GetWnd()->DestroyWindow();

      if (UTL_get_instance() == 1)
      {
         ReleaseMutex(m_Mutex);
         CloseHandle(m_Mutex);
      }

      return FAILURE;
   }

   return SUCCESS;
}

// --------------------------------------------------------------------------

// Sets up the registry entry point for GetProfile and WriteProfile.calls to
// "\Software\PFPS\FalconView\3.1", so MFC reads and writes the registry from
// the same point our parameter library does.
void CFVApp::InitRegistry()
{
   // Sets m_pszRegistryKey to "PFPS".
   // Sets m_pszProfileName to m_pszAppName, i.e., "FalconView".
   SetRegistryKey("XPlan");

   if (m_is_regserver)
   {
      long status = reg::CreateSymLink();
      if (status == ERROR_SUCCESS)
      {
         INFO_report("Symbolic link from HKLM\\SOFTWARE\\PFPS to "
            "HKLM\\SOFTWARE\\XPlan added successfully");
      }
      else
      {
         ERR_report("Failed adding symbolic link from HKLM\\SOFTWARE\\PFPS to "
            "HKLM\\SOFTWARE\\XPlan");
      }
   }
   else if (m_is_unregserver)
   {
      reg::DeleteSymLink();
   }
}

BOOL CFVApp::RegisterTypeLibrary(const CString& strTypeLibPath)
{
   CString msg;

   // register FalconView's type library
   ITypeLibPtr spTypeLib;
   HRESULT hr = ::LoadTypeLib(_bstr_t(strTypeLibPath), &spTypeLib);
   if (SUCCEEDED(hr))
   {
      hr = ::RegisterTypeLib(spTypeLib, _bstr_t(strTypeLibPath), NULL);
      if (FAILED(hr))
      {
         msg.Format("Unable to register type library %s [%X]", strTypeLibPath, hr);
         ERR_report(msg);
      }
      else
         return TRUE;
   }
   else
   {
      msg.Format("Unable to load type library %s [%X]", strTypeLibPath, hr);
      ERR_report(msg);
   }

   return FALSE;
}

// --------------------------------------------------------------------------

static
int change_to_startup_favorite(MapView* view, int view_width,
   int view_height, const CString& favorite_name)
{
   // if the name of the given favorite is not "", then try to
   // open that favorite
   if (favorite_name != "")
   {
      FavoriteList *fav_list = FavoriteList::get_instance();

      // obtain an index to the favorite with the given name
      int index = fav_list->Find(favorite_name);
      if (index != -1)
      {
         CString filename = fav_list->GetFavoriteFilename(index);
         FavoriteData favorite;
         if (fav_list->LoadData(filename, favorite) == SUCCESS)
         {
            fvw_get_frame()->GotoFavorite(&favorite);
            return SUCCESS;
         }
      }
   }

   return SUCCESS;
}

// --------------------------------------------------------------------------
// exit program
int CFVApp::ExitInstance()
{
   // destroy the singletons
   //
   FavoriteList::destroy_instance();
   CMdsNotifyEvents::DestroyInstance();
   MDSWrapper::DestroyInstance();
   scene_mgr::SDSWrapper::DestroyInstance();
   SymbolContainer::DestroyInstance();
   Cmov_sym_overlay::DestroyStateIndicatorsInstance();
   CRemarkDisplay::DestroyCallbackInstance();

   MAP_close();

   GRA_shutdown_graphics();

   if (UTL_get_windows_major_version_number() >= 4)
   {
  /*
   *  if full drag was disabled at startup and FalconView disabled it,
   *  then re-enable it if it is still disabled.
   */
      if (m_full_drag_enabled_at_startup && !is_full_drag_on())
      {
         set_full_drag(m_full_drag_enabled_at_startup);
      }
   }

   // save the state of the recent file overlay list to the registry
   m_recent_file_list->save();

   // destroy singleton CFvwUtil
   CFvwUtil::destroy_instance();

   // release the mutex object created in 4
   if (UTL_get_instance() == 1)
   {
      BOOL ret = ReleaseMutex(m_Mutex);
      CloseHandle(m_Mutex);
   }

   // clean up the overlay type descriptors
   OVL_get_type_descriptor_list()->Terminate();

   // uninitialize GDAL
   GdalInitializer::UninitGDAL();

   // uninitialize FvNetFetcher
   net_fetcher::cleanup();

   return FV_APP_BASE_CLASS::ExitInstance();
}

// App command to run the dialog
void CFVApp::OnAppAbout()
{
   CAboutDlg aboutDlg;

   // [Bug 1215].  Locking the temporary maps is necessary to work around an MFC bug in
   // COccManager::IsDialogMessage when a dialog contains an ActiveX control
   AfxLockTempMaps();
   aboutDlg.DoModal();
   AfxUnlockTempMaps();
}

/////////////////////////////////////////////////////////////////////////////
// CFVApp commands

// Allow the user to select the overlay type from a list.  Create an overlay
// of that type.
void CFVApp::OnFileNewFv()
{
   // close print preview window if active
   fvw_get_frame()->ClosePrintPreview();

   NewOverlay(FALSE);
}

// Used to create an overlay of the same type as the current overlay,
// but it was changed to work just like OnFileNewFv().  To change back
// change FALSE back to TRUE.
void CFVApp::OnCurrentNew()
{
   NewOverlay(FALSE);
}

// Creates a new overlay.  If same_as_current is TRUE then an overlay of the
// same type as the current overlay is created, otherwise the user is presented
// with a list to select an overlay type from.
void CFVApp::NewOverlay(boolean_t same_as_current)
{
   const boolean_t show_type_list = !same_as_current;
   if (OVL_get_overlay_manager()->create(show_type_list) != SUCCESS)
      ERR_report("C_ovl_mgr::create() failed.");
}

// Allow the user to select the overlay type from a list.  Open an overlay
// of that type.
void CFVApp::OnFileOpen()
{
   // close print preview window if active
   fvw_get_frame()->ClosePrintPreview();

   OpenOverlay();
}

// Used to open an overlay of the same type as the current overlay, but
// it was changed to match OnFileOpen.  To change back change FALSE to
// TRUE.
void CFVApp::OnCurrentOpen()
{
   OpenOverlay();
}

// Opens a new overlay.  If same_as_current is TRUE then an overlay of the
// same type as the current overlay is opened, otherwise the user is presented
// with a list to select an overlay type from.
void CFVApp::OpenOverlay()
{
   C_overlay *overlay;
   if (OVL_get_overlay_manager()->open(overlay) != SUCCESS)
      ERR_report("C_ovl_mgr::open() failed.");
}

// Allow the user to select an overlay from a list of all overlays currently
// opened.  The chosen overlay will be closed.
void CFVApp::OnFileCloseFv()
{
   if (OVL_get_overlay_manager()->close() != SUCCESS)
      ERR_report("C_ovl_mgr::close() failed.");
}


void CFVApp::OnUpdateFileCloseFv(CCmdUI* pCmdUI)
{
   // see if there is a file overlay to close
   C_overlay *pOverlay = OVL_get_overlay_manager()->get_first();
   while (pOverlay != NULL)
   {
      if (dynamic_cast<OverlayPersistence_Interface *>(pOverlay) != NULL)
      {
         pCmdUI->Enable(TRUE);
         return;
      }

      pOverlay = OVL_get_overlay_manager()->get_next(pOverlay);
   }

   pCmdUI->Enable(FALSE);
}

void CFVApp::OnSaveAll()
{
   // close print preview window if active
   fvw_get_frame()->ClosePrintPreview();

   // do a save all
   if (OVL_get_overlay_manager()->save_all() != SUCCESS)
      ERR_report("C_ovl_mgr::save_all() failed.");
}

void CFVApp::OnUpdateSaveAll(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(is_save_enabled(TRUE));
}

// Allow the user to select an overlay from a list of all dirty file overlays
// that are currently opened.  The chosen overlay will be saved.
void CFVApp::OnFileSave()
{
   // close print preview window if active
   fvw_get_frame()->ClosePrintPreview();

   SaveOverlay(FALSE);
}


// Used to save the current overlay, but it was changed to work like
// OnFileSave().  To change back, change FALSE to TRUE.
void CFVApp::OnCurrentSave()
{
   SaveOverlay(FALSE);
}


void CFVApp::SaveOverlay(boolean_t current_overlay)
{
   if (OVL_get_overlay_manager()->save(!current_overlay) != SUCCESS)
      ERR_report("C_ovl_mgr::save() failed.");
}


void CFVApp::OnUpdateFileSaveFv(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(is_save_enabled(TRUE));
}

void CFVApp::OnUpdateCurrentSave(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(is_save_enabled(TRUE));
}

// Allow the user to select an overlay from a list of all file overlays
// that are currently opened.  A Save As will be done for the chosen overlay.
void CFVApp::OnFileSaveAs()
{
   // do a save as
   if (OVL_get_overlay_manager()->save_as() != SUCCESS)
      ERR_report("C_ovl_mgr::save_as() failed.");
}

void CFVApp::OnUpdateFileSaveAsFv(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(is_save_enabled(FALSE));
}

// We overide this so that we can tell the PrntTools overlay that the printer
// setup has changed
void CFVApp::OnFilePrintSetup()
{
   // First call the base class
   FV_APP_BASE_CLASS::OnFilePrintSetup();

   // Now, notify the Print Tool overlay that the printer setup has changed
   int failure_count;
   int invalid_surface_count;
   CPrintToolOverlay::SetPrinterDefaults(&failure_count, &invalid_surface_count);
   if (failure_count > 0 || invalid_surface_count > 0)
   {
      CString msg;

      if (failure_count > 0)
         msg.Format("During the process of adjusting your Page Layout, %d "
            "failures occurred.  See %s for more information.",
            failure_count, (const char*) appErrorFile());
      else
         msg.Format("The change in printer setup caused the surface size to "
            "become invalid in %d of your Page Layout overlays.  This is "
            "generally caused by the paper size being too large for a "
            "particular map type.  It may also be due to a combination of "
            "paper size, map type, rotation, and Scale Factor.",
            invalid_surface_count);

      AfxMessageBox(msg);
   }
}

// *****************************************************
//
// This handles the "DAFIF Selection" menu item (real or via sendmessage)
// The work is done in the mainframe; we simply bounce the message there.
//

void CFVApp::OnDafifSelection()
{
   CMainFrame* frame = fvw_get_frame();   // try to get a pointer to the main frame

   if ( frame )                           // if we got a frame pointer...
      frame->select_dafif();              // start the tool
}

// *****************************************************

void CFVApp::OnHelpTechnicalsupport()
{
   CAboutDlg aboutDlg;
   aboutDlg.SetInitialPage(CONTACTS_TECHNICAL_SUPPORT_PAGE_NUMBER);

   // [Bug 1215].  Locking the temporary maps is necessary to work around an MFC bug in
   // COccManager::IsDialogMessage when a dialog contains an ActiveX control
   AfxLockTempMaps();
   aboutDlg.DoModal();
   AfxUnlockTempMaps();
}

void CFVApp::OnToolsTest()
{
   // Prepare and invoke the Test Settings property sheet

   CTestSheet testSheet("Test Settings", AfxGetMainWnd(), 0);

   if (testSheet.DoModal() != IDOK)
      return;

   CMapAutoTester mapAutoTester;

   if (mapAutoTester.Init() != SUCCESS)
   {
      AfxMessageBox("Error initializing test suite");
      mapAutoTester.Finish();
      return;
   }

   if (testSheet.InitializeTestSuite(&mapAutoTester) != SUCCESS)
   {
      mapAutoTester.Finish();
      return;
   }

   // run the test!
   int err = mapAutoTester.Execute();

   // finish the test before displaying the status message box
   mapAutoTester.Finish();

   // display the status message box
   if (err != SUCCESS)
      AfxMessageBox("Error encountered during testing");
   else
      AfxMessageBox("No errors encountered during testing.");
}

void CFVApp::OnSLAT()
{
   SlatToolLauncher().LaunchTool();
}

void CFVApp::OnPluginManager()
{
   // use ShellExecute() to run PluginManager.exe located in Program Files\PFPS\falcon
   CString pluginmanager_path = PRM_get_registry_string("Main", "HD_DATA", "") + "\\..\\PluginManager.exe";

   ShellExecute(AfxGetMainWnd()->m_hWnd, "open", pluginmanager_path, NULL, NULL, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
// MRU file list default implementation

BOOL CFVApp::OnOpenRecentFile(UINT nID)
{
   CString file_specification;
   int nIndex;

   ASSERT_VALID(this);
   ASSERT(m_pRecentFileList != NULL);
   ASSERT(m_recent_file_list != NULL);
   ASSERT(nID >= ID_FILE_MRU_FILE1);
   ASSERT(nID < (ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize()));

   // convert to array index
   nIndex = nID - ID_FILE_MRU_FILE1;

   // get full file spec for selected item
   file_specification = (*m_pRecentFileList)[nIndex];

   // get class name for the file overlay
   GUID overlayDescGuid = m_recent_file_list->GetOverlayDescriptorGuid((const char *)(*m_pRecentFileList)[nIndex]);
   if (overlayDescGuid == GUID_NULL)
   {
      CString msg;

      // remove this file from the MFC recent file list
      m_pRecentFileList->Remove(nIndex);

      msg.Format("Registry Corrupted!  Cannot get class name for %s.",
         file_specification);
      AfxMessageBox(msg);
      return TRUE;
   }

   // if the factory was disabled in the exclusion list then warn the user
   if (OVL_get_type_descriptor_list()->IsOverlayEnabled(overlayDescGuid) == FALSE)
   {
      CString msg;
      msg.Format("The file '%s' cannot be opened.\n\n"
                 "This is because the overlay associated with a file of this type\n"
                 "has been disabled or no longer exists.  Do you want to remove\n"
                 "the reference to it from the Recent list?", file_specification);
      if (AfxMessageBox(msg, MB_YESNO) == IDYES)
      {
         m_pRecentFileList->Remove(nIndex);
         m_recent_file_list->remove(overlayDescGuid, file_specification);
      }
      return TRUE;
   }

   // open the file overlay
   C_overlay *ret_overlay;
   if (OVL_get_overlay_manager()->OpenFileOverlay(overlayDescGuid, file_specification, ret_overlay) != SUCCESS)
   {
      m_pRecentFileList->Remove(nIndex);
      m_recent_file_list->remove(overlayDescGuid, file_specification);
   }

   return TRUE;
}

void CFVApp::add_to_recent_file_list(GUID overlayDescGuid, const char *file_specification)
{
   // update the MRU menu and MFC recent file list
   if (m_pRecentFileList != NULL)
      m_pRecentFileList->Add(file_specification);

   // update class name, file spec list
   m_recent_file_list->add(overlayDescGuid, file_specification);
}


// Remove all entries in the recent file list and clean out the registry
// sections.
void CFVApp::OnPurgeRecentFileList()
{
   CString class_name;
   CString file_spec;
   CMenu *menu;
   int i;

   ASSERT(m_pRecentFileList != NULL);
   ASSERT(m_recent_file_list != NULL);
   ASSERT(m_pRecentFileList->GetSize() > 0);
   ASSERT(fvw_get_frame()->GetMenu());

   // Get the main menu.
   menu = fvw_get_frame()->GetMenu();

   // Note GetSize() always returns the maximum size of the recent file list,
   // which is not necessarily the number of elements in the recent file list.
   i = m_pRecentFileList->GetSize() - 1;
   while (i >= 0)
   {
      if ((*m_pRecentFileList)[i].GetLength() > 0)
      {
         // get full file spec for item
         file_spec = (*m_pRecentFileList)[i];

         // get class name for the file overlay
         GUID overlayDescGuid = m_recent_file_list->GetOverlayDescriptorGuid((const char *)file_spec);

         // Remove the entry from the recent file overlay list, if it was found.
         // Report an error otherwise.
         if (overlayDescGuid != GUID_NULL)
            m_recent_file_list->remove(overlayDescGuid, file_spec);
         else
         {
            CString msg;

            msg.Format("Registry Corrupted!  Cannot get class name for %s.",
               (const char *)file_spec);
            AfxMessageBox(msg);
         }

         // remove the item from the MFC recent file list
         m_pRecentFileList->Remove(i);

         // Remove the corresponding menu item for every case except for item
         // 0, because it is needed for the MRU file list place holder entry.
         // That entry will be updated, i.e., the label will be set and it will
         // be disabled, by the UpdateMenu member of the CRecentFileList class.
         // Recall m_pRecentFileList is an instance of CRecentFileList.
         if (i > 0)
            menu->DeleteMenu(ID_FILE_MRU_FILE1 + i, MF_BYCOMMAND);
      }

      i--;
   }

   // remove the recent file overlay list section from the registry
   PRM_delete_registry_section("RecentFileOverlays2");

   // remove the MFC recent file list section from the registry
   PRM_delete_registry_section("Recent File List");

   // redraw menu bar after change
   fvw_get_frame()->DrawMenuBar();
}

// Disable menu items unless it is not empty.  Also require that
// the MFC recent file list object and the internal recent file
// overlay list objects exist.
void CFVApp::OnUpdatePurgeRecentFileList(CCmdUI* pCmdUI)
{
   if (m_recent_file_list && m_pRecentFileList &&
      m_pRecentFileList->GetSize() > 0 &&
      (*m_pRecentFileList)[0].GetLength() > 0)
      pCmdUI->Enable(TRUE);
   else
      pCmdUI->Enable(FALSE);
}

void CFVApp::ShowTipAtStartup(void)
{
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);
   if (cmdInfo.m_bShowSplash)
   {
      CTipDlg dlg;
      if (dlg.m_bStartup)
         dlg.DoModal();
   }
}

void CFVApp::ShowTipOfTheDay(void)
{
   CTipDlg dlg;
   dlg.DoModal();
}

void CFVApp::XPlanOptions()
{
#ifdef GOV_RELEASE
   try
   {
      xplan_package_service::IXPlanOptionsDialogPtr xplan_options;
      CO_CREATE(xplan_options, xplan_package_service::CLSID_XPlanOptionsDialog);

      xplan_options->ShowDialog();
   }
   catch (_com_error& e)
   {
      REPORT_COM_ERROR(e);
   }
#endif
}

//
// needed because CWinApp::RunAutomated should only be called once
//
boolean_t CFVApp::am_automated(void)
{
   static boolean_t init = FALSE;
   static boolean_t automated = FALSE;

   if (init)
      return automated;

   automated = RunAutomated();
   init = TRUE;

   return automated;
}

//
// needed because CWinApp::RunEmbedded should only be called once
//
boolean_t CFVApp::am_embedded(void)
{
   static boolean_t init = FALSE;
   static boolean_t embedded = FALSE;

   if (init)
      return embedded;

   embedded = RunEmbedded();
   init = TRUE;

   return embedded;
}

// Returns TRUE if there are any file overlays or any dirty overlays.
// Returns FALSE otherwise.
boolean_t CFVApp::is_save_enabled(BOOL bTestModified)
{
   C_ovl_mgr *ovl_mgr = OVL_get_overlay_manager();
   C_overlay *overlay;

   // look for either a dirty overlay or a file overlay
   overlay = ovl_mgr->get_first();
   while (overlay)
   {
      if (bTestModified && overlay->is_modified())
         return TRUE;

      // if bTestModified is FALSE, then we just care that the overlay is a
      // file overlay and that the save filter is non-empty.
      OverlayTypeDescriptor *pOverlayTypeDesc =
         OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(
            overlay->get_m_overlayDescGuid());
      if (!bTestModified && pOverlayTypeDesc &&
          pOverlayTypeDesc->fileTypeDescriptor.bIsFileOverlay &&
          !pOverlayTypeDesc->fileTypeDescriptor.saveFileDialogFilter.IsEmpty())
      {
         return TRUE;
      }

      overlay = ovl_mgr->get_next(overlay);
   }

   return FALSE;
}

#ifdef _DEBUG

void CFVApp::OnCheckMemory()
{
   int ret;

   {
      CWaitCursor cursor;
      ret = _CrtCheckMemory();
   }

   if (ret == FALSE)
      AfxMessageBox("Memory error detected");
}

void CFVApp::OnMemoryDebugging()
{
   if (m_memory_debugging_on)
   {
      m_memory_debugging_on = FALSE;
      int temp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
      temp &= ~_CRTDBG_CHECK_ALWAYS_DF;
      _CrtSetDbgFlag(temp);
   }

   else
   {
      m_memory_debugging_on = TRUE;
      int temp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
      temp |= _CRTDBG_CHECK_ALWAYS_DF;
      _CrtSetDbgFlag(temp);
   }
}

void CFVApp::OnUpdateMemoryDebugging(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_memory_debugging_on);
}

#endif

BOOL CFVApp::PreTranslateMessage(MSG* msg)
{
   while (msg->wParam == VK_MENU) // for break or return
   {
      // In flight mode we want to swallow messages involving the ALT
      // key so that the menu items don't open.

      MapView *map_view = fvw_get_view();
      if (map_view == nullptr)
         break;

      GeospatialViewController *geospatial_view_controller
         = map_view->GetGeospatialViewController();
      if (geospatial_view_controller == nullptr)
         break;

      GeospatialView *geospatial_view
         = geospatial_view_controller->GetGeospatialView();
      if (geospatial_view == nullptr)
         break;

      FlightInputDeviceStateType *flight_input_device_state_type
         = geospatial_view_controller->GetInputDevice();

      if (flight_input_device_state_type == nullptr)
         break;

      if (flight_input_device_state_type->GetActiveCam()
         == fvw::CAMERA_MODE_NONE)
         break;

      return TRUE;  // gulp!
   }

   return CWinThread::PreTranslateMessage(msg);
}
