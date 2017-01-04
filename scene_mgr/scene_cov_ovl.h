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



/*------------------------------------------------------------------
-  FILE NAME:         scene_cov_ovl.h
-  LIBRARY NAME:      catalog
-
-  DESCRIPTION:
-
-
-------------------------------------------------------------------*/

#ifndef SCENE_COV_OVL_H
#define SCENE_COV_OVL_H 1

/*------------------------------------------------------------------
-                            Includes
-------------------------------------------------------------------*/

#include <afx.h>
#include "overlay.h"

#include "common.h"
#include "smsheet.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "SceneIconItem.h"  // for CGeoRect
#include <vector>


/*------------------------------------------------------------------
-                            Defines
-------------------------------------------------------------------*/

#define SM_ALLLOCK       1
#define SM_ALLUNLOCK     2
#define SM_COPYLOCK      3
#define SM_COPYUNLOCK    4

#define  SM_ERRSELECT_UNKNOWNMODE         -4  // unknown selection mode...
#define  SM_ERRSELECT_CANNOTRESOLVE       -3  // cannot resolve ambiguities in selection
#define  SM_ERRSELECT_ATTEMPTDELETESOURCE -2  // attempted to select a source file using delete tool
#define  SM_ERRSELECT_ATTEMPTCOPYTARGET   -1  // attempted to select a target file using copy tool
#define  SM_ERRSELECT_EMPTYSELECTION       0  // empty rectangle... do nothing
#define  SM_SELECT_TARGET                  1  // select target files if any unselected target files found in rect
#define  SM_UNSELECT_TARGET                2  // unselect only if all in rect are already selected
#define  SM_SELECT_SOURCE                  3  // select source files if any unselected source files found in rect
#define  SM_UNSELECT_SOURCE                4  // unselect only if all in rect are already selected
#define  SM_UNSELECT_ALL                   5  // unselect all that are already selected

// constants used for draw brush style
#define SM_DRAWBRUSHSTYLE_HOLLOW       1
#define SM_DRAWBRUSHSTYLE_HATCH        2
#define SM_DRAWBRUSHSTYLE_PATTERN      3
#define SM_DRAWBRUSHSTYLE_TRANSPARENT  4

// constants used to locate and name external files
#define SM_COVERAGE_ANIMCOPY     "copyfile.avi"
#define SM_COVERAGE_ANIMDELETE   "delfile.avi"
#define SM_COVERAGE_ANIMREGEN    "genfile.avi"

// forward declarations
class C_icon;
class GeoBoundsDragger;
class HintText;


namespace scene_mgr
{

// forward declarations
class SDSSceneType;

// global variables
extern int       g_sm_DrawBrushStyle;
extern int       g_sm_all_lock_count;
extern int       g_sm_copy_lock_count;
extern int       g_sm_bDataPathsActiveMode;
extern CSMSheet* g_sm_pSMSheet;

// global variable access routines
CSMSheet*        sm_getSMSheet();
void             sm_setSMSheet(CSMSheet* p);
int              sm_get_image_index(enum DataSourceTypeEnum eDSType, bool bOnline, bool bShared);
CString          sm_get_hd_data_from_registry();
void             sm_lock(int iLock);
BOOL             sm_is_sm_all_locked(void);
BOOL             sm_is_sm_either_locked(void);


// C_overlay class for catalog library display.
class SceneCoverageOverlay : public FvOverlayImpl
{
private:
   static boolean_t m_edit_on;

private:
   GeoBoundsDragger *m_dragger;
   bool m_is_dragging;

   BOOL              m_bSelectMode;
   //BOOL              m_bMultipleSceneTypesSelected;
   BOOL              m_bSMvisible;
   BOOL              m_bSMautohide;
   BOOL              m_bDrawAsRegion;
   BOOL              m_bRegionsVisible;
   BOOL              m_bDirty;
   CGeoRect          m_viewBbox;
   bool              m_bShowOnline;
#if 0
   CRegionList       m_regionList;
   CMapTypeSet       m_MapTypeSet;
#endif
   std::vector<long> m_SourceSet;
   BOOL              m_bShowSpec;
   HintText          m_hint;
   CGeoRect          m_rectSelected;
   std::vector<long> m_HighlightedSet;

public:
   // Constructor
   SceneCoverageOverlay();

   void _Initialize();
   bool mInitialized;

   // *****************************************
   // C_overlay Functions Overrides
public:

   virtual void Finalize();

   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "SceneCoverageOverlay"; }

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
   HCURSOR SceneCoverageOverlay::GetCursorType(UINT flags);

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
   int  CreateSMDialog(BOOL bShow);
   int  RefreshSMDialog();
   int  DestroySMDialog();
   void ShowSMDialog(BOOL bShow);
   void AutoHideSMDialog();
   BOOL IsSMDialogVisible();
   void SetSelectMode(int mode);
   int  GetSelectMode();
   boolean_t AreMultipleSceneTypesSelected();
   void SelectAllDisplayedData();
   void ClearSelection(long lDatasource);
   void InvalidateAsNeeded(void);

   BOOL IsCoverageDirty(void) { return m_bDirty; };
   void MarkCoverageAsDirty(BOOL b) { m_bDirty = b; };

   bool GetShowOnlineFlag(void) { return m_bShowOnline; };
   void SetShowOnlineFlag(bool b) { m_bShowOnline = b; };

   void InvalidateOverlay(void);
   void InvalidateOverlay(LPCRECT pRect);

   void HighlightCatalogSet(std::vector<long>& set);

   int CopySelectedFiles(HWND parent_window);

   // returns TRUE when in sdm mode
   static boolean_t get_edit_on() { return m_edit_on; }

   void SetSources(std::vector<long>* p);
   void SetAllSources(void);
   void HideAllSources(void);
   void GetSources(std::vector<long>* p);
   void RemoveSource(long id); // Used by SDSWrapper
   void AddSource(long id);

   void SetActiveSMSheet(CSMSheet::SMActive_t page);

   __int64  GetFreeSpaceMB(void);
/*
   CString GetFreeSpaceMBAsString(void);

*/
   // Methods and vars relating to MDSWrapper

   static void AddSDSSceneType(SDSSceneType *scene_type);
   static void PopulateSDSSceneTypes();
   static SDSSceneType *GetOneSDSSceneType();
   static void RemoveAllSDSSceneTypes();
   static std::vector<SDSSceneType*> *GetSDSSceneTypes();

protected:
   // Protected destructor. Call Release instead
   ~SceneCoverageOverlay();

private:
   // Static so that state is maintained between SM sessions
   static std::vector<SDSSceneType*> s_scene_type_vector;
};


void sm_set_coverage_overlay(SceneCoverageOverlay *pOverlay);
SceneCoverageOverlay *sm_get_coverage_overlay();
SceneCoverageOverlay *sm_get_valid_coverage_overlay();
int sm_getDrawBrushStyle();
void sm_setDrawBrushStyle(int style);
CSMSheet* sm_getSMSheet();
void sm_setSMSheet(CSMSheet* p);

};  // namespace scene_mgr

#endif
