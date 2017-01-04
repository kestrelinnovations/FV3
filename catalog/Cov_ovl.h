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



/*------------------------------------------------------------------
-  FILE NAME:         cov_ovl.h
-  LIBRARY NAME:      catalog
-
-  DESCRIPTION:
-
-
-------------------------------------------------------------------*/

#ifndef COV_OVL_H
#define COV_OVL_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include "common.h"
#include "sets.h"
#include "mdmsheet.h"
#include "MDSWrapper.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "LongArray.h"

class GeoBoundsDragger;
class HintText;

// global variables
extern int                 g_drawBrushStyle;
extern int                 g_mdm_all_lock_count;
extern int                 g_mdm_copy_lock_count;
extern int                 g_bDataPathsActiveMode;
extern CMDMSheet*          g_pMDMSheet;

#define MDM_ALLLOCK       1
#define MDM_ALLUNLOCK     2
#define MDM_COPYLOCK      3
#define MDM_COPYUNLOCK    4

#define  CV_ERRSELECT_UNKNOWNMODE         -4  // unknown selection mode...
#define  CV_ERRSELECT_CANNOTRESOLVE       -3  // cannot resolve ambiguities in selection
#define  CV_ERRSELECT_ATTEMPTDELETESOURCE -2  // attempted to select a source file using delete tool
#define  CV_ERRSELECT_ATTEMPTCOPYTARGET   -1  // attempted to select a target file using copy tool
#define  CV_ERRSELECT_EMPTYSELECTION       0  // empty rectangle... do nothing
#define  CV_SELECT_TARGET                  1  // select target files if any unselected target files found in rect
#define  CV_UNSELECT_TARGET                2  // unselect only if all in rect are already selected
#define  CV_SELECT_SOURCE                  3  // select source files if any unselected source files found in rect
#define  CV_UNSELECT_SOURCE                4  // unselect only if all in rect are already selected
#define  CV_UNSELECT_ALL                   5  // unselect all that are already selected

// global variable access routines
CMDMSheet*        cat_getMDMSheet();
void              cat_setMDMSheet(CMDMSheet* p);
int               cat_get_image_index(enum DataSourceTypeEnum eDSType, bool bOnline, bool bShared);
CString           cat_get_hd_data_from_registry();
void              cat_mdm_lock(int iLock);
BOOL              cat_is_mdm_all_locked(void);
BOOL              cat_is_mdm_either_locked(void);

// constants used for draw brush style
#define DRAWBRUSHSTYLE_HOLLOW       1
#define DRAWBRUSHSTYLE_HATCH        2
#define DRAWBRUSHSTYLE_PATTERN      3
#define DRAWBRUSHSTYLE_TRANSPARENT  4

// constants used to locate and name external files
#define COVERAGE_FILE         "coverage.mdb"
#define COVERAGE_BLANKFILE    "covblank.mdb"
#define COVERAGE_TMPFILE      "tmp_coverage.mdb"
#define CDLIBRARY_FILE        "cdlib.mdb"
#define COVERAGE_ANIMCOPY     "copyfile.avi"
#define COVERAGE_ANIMDELETE   "delfile.avi"
#define COVERAGE_ANIMREGEN    "genfile.avi"


// C_overlay class for catalog library display.
class CoverageOverlay : public FvOverlayImpl
{
private:
   static boolean_t m_edit_on;

private:
   GeoBoundsDragger *m_dragger;
   bool m_is_dragging;

   BOOL              m_bSelectMode;
   BOOL              m_bMultipleMapSelected;
   BOOL              m_bMDMvisible;
   BOOL              m_bMDMautohide;
   CRegionList       m_regionList;
   CGeoRect          m_viewBbox;
   CMapTypeSet       m_MapTypeSet;
   LongArray         m_SourceSet;
   BOOL              m_bShowSpec;
   BOOL              m_bRegionsVisible;
   BOOL              m_bDrawAsRegion;
   BOOL              m_bDirty;
   HintText          m_hint;
   bool              m_bShowOnline;
   CGeoRect          m_rectSelected;
   LongSet           m_HighlightedSet;
   bool              m_bDisplayExpiredChartsOnly;

public:
   // Constructor
   CoverageOverlay();

   void _Initialize();
   bool mInitialized;

   // *****************************************
   // C_overlay Functions Overrides
public:

   virtual void Finalize();

   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "CoverageOverlay"; }

   // Draw the coverage overlay.  Creates the file icons, as needed.  Sets the
   // properties of each file icon: on top, on screen, on target, selected.
   int draw(ActiveMap* map);

   // If the coverage overlay is the current overlay (Map Data Manager mode),
   // then this function starts a file selection (*drag == TRUE).  Otherwise,
   // it will make the coverage overlay the current overlay, if the given point
   // hits one of the file icons.
   int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   virtual int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);
   virtual int on_left_mouse_button_up(IFvMapView *pMapView, CPoint point, UINT nFlags);

   // Updates the rubber band box during a file selection.
   void drag(ViewMapProj* map, CPoint point, UINT flags, HCURSOR *cursor, 
      HintText **hint);

   // Completes a file selection.   
   void drop(ViewMapProj* map, CPoint point, UINT flags);

   // Cancels an in progress file selection.
   void cancel_drag(ViewMapProj* map);

   // Receives all keydown messeges from the Overlay Manager and filters them 
   // for this overlay... a return of false will allow the keystroke to be 
   // processed normally in MapView. This function will get called only if 
   // the File Overlay is current and open in its editor.
   virtual boolean_t offer_keydown(IFvMapView *pMapView, UINT /*nChar*/, UINT /*nRepCnt*/,
      UINT /*nFlags*/, CPoint /*point*/);

   // Returns the file icon below the point, NULL for none.
   C_icon *hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point);

   // add menu items to the popup menu based on context
   void menu(ViewMapProj* map, CPoint point, 
      CList<CFVMenuNode *, CFVMenuNode *> &list) { }

   BOOL GetViewBbox(MapProj* map, CGeoRect& viewBbox);

   // Returns a cursor based on current mode of map data manager
   HCURSOR CoverageOverlay::GetCursorType(UINT flags);

   // Inform the coverage overlay that it is about to be closed, if files are
   // selected the user will be warned that their selection will be lost.  If
   // the user cancels the close this fucntion will return cancel == TRUE.
   int pre_close(boolean_t *cancel);

   // Turn the Map Data Manager on or off.
   static int set_edit_on(boolean_t edit_on);

   // This function is called to get the default cursor associated with the
   // Map Data Manager mode.
   static HCURSOR get_default_cursor();

   // End C_overlay Functions Overrides
   // *****************************************

public:
   int  CreateMDMDialog(BOOL bShow);
   int  RefreshMDMDialog();
   int  DestroyMDMDialog();
   void ShowMDMDialog(BOOL bShow);
   void AutoHideMDMDialog();
   BOOL IsMDMDialogVisible();
   void SetSelectMode(int mode);
   int  GetSelectMode();
   void SetMultipleMapSelected(boolean_t b);
   boolean_t IsMultipleMapSelected();
   void SelectAllDisplayedData();
   void ClearSelection(long lDatasource);
   void ClearLibrarySelection();
   void InvalidateAsNeeded(void);

   BOOL IsCoverageDirty(void) { return m_bDirty; };
   void MarkCoverageAsDirty(BOOL b) { m_bDirty = b; };

   bool GetShowOnlineFlag(void) { return m_bShowOnline; };
   void SetShowOnlineFlag(bool b) { m_bShowOnline = b; };

   void InvalidateOverlay(void);
   void InvalidateOverlay(LPCRECT pRect);

   void HighlightCatalogSet(LongSet& set);

   int CopySelectedFiles(void);

   // returns TRUE when in mdm mode
   static boolean_t get_edit_on() { return m_edit_on; }

   void SetSources(LongArray* p);
   void SetAllSources(void);
   void HideAllSources(void);
   void GetSources(LongArray* p);
   void RemoveSource(long id); // Used by MDSWrapper
   void AddSource(long id);

   void DisplayExpiredChartsOnly(bool bExpired);
   bool GetDisplayExpired() { return m_bDisplayExpiredChartsOnly; }
   void SetActiveMDMSheet(CMDMSheet::MDMActive_t page);

   __int64  GetFreeSpaceMB(void);
   CString GetFreeSpaceMBAsString(void);

   // Methods and vars relating to MDSWrapper

   static void PopulateMDSMapTypes();
   static void RemoveAllMDSMapTypes();
   static void AddMDSMapType(MDSMapType *map_type);
   static MDSMapType *GetOneMDSMapType();
   static MDSMapTypeVector *GetMDSMapTypes();

protected:
   // Protected destructor. Call Release instead
   ~CoverageOverlay();

private:
   // Static so that state is maintained between MDM sessions
   static MDSMapTypeVector s_map_type_vector;
}; // end CoverageOverlay

// global variable access routines (must be declared after CoverageOverlay defined)
int               cat_getDrawBrushStyle();
void              cat_setDrawBrushStyle(int style);
CoverageOverlay*  cat_get_coverage_overlay();
void cat_set_coverage_overlay(CoverageOverlay *pOverlay);
CoverageOverlay*  cat_get_valid_coverage_overlay();

#endif
