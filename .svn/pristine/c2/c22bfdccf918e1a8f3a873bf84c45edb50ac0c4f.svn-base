// Copyright (c) 1994-2011 Georgia Tech Research Corporation, Atlanta, GA
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

#if !defined(TAMASK_H__89E5255C_8652_11D3_8663_00105A9B4838__INCLUDED_)
#define TAMASK_H__89E5255C_8652_11D3_8663_00105A9B4838__INCLUDED_

#include "maps.h"
#include "overlay.h"
#include "FvMappingCore\Include\MapProjWrapper.h"

// DVL :: Max size of a tile_id is actually a short, but leave name as is for now
class CContourTile;

class CMapLongToContourTile;
class C_TAMask_ovl;
class CTAMaskStatus;
class CVerticalViewProjector;
class CMaskClipRgn;
class TAMaskParamListener_Interface;
#include "..\MovingMapOverlay\gpsstyle.h" // for CBullseyeProperties

#include "..\contour\ContourTile.h"
#import "DtedMapServer.tlb" no_namespace named_guids

class C_TAMask_point : public C_icon
{
private:
public:
   // Members
   int                     m_Elevation;

   degrees_t               m_latitude;
   degrees_t               m_longitude;
   int                     m_x;
   int                     m_y;
   CRect                   m_rect;
   CRect                   m_text_rect;
   BYTE                    m_rect_size_x;
   BYTE                    m_rect_size_y;
   CIconImage*             m_displayIcon;

public:
   // Functions/Operators
   C_TAMask_point();
   ~C_TAMask_point();

   // overlay point information routines
   virtual boolean_t is_kind_of(const char* class_name);
   virtual const char *get_class_name();

   // tool tip and hit test routines
   CString get_help_text();
   CString get_tool_tip();
   virtual boolean_t hit_test(CPoint point);
   virtual boolean_t in_view(MapProj* map);

   // draws the current point as a 'unhilighted or hilighted' point
   virtual void draw(CDC* pDC, int x, int y, boolean_t hilighted, boolean_t draw_labels);

   void invalidate();

   C_TAMask_ovl *get_overlay();
};

//------------------------------------------------------------------------------
//-------------------------------- C_TAMask_ovl -------------------------------
//------------------------------------------------------------------------------
// C_TAMask_ovl is a general contour line drawing overlay.  It is able to 
// utilize DTED to draw controur lines at a user specified separation.
//------------------------------------------------------------------------------
class C_TAMask_ovl : 
   public IDispatchImpl<tamo::ITerrainAvoidanceMaskOverlay, &tamo::IID_ITerrainAvoidanceMaskOverlay, &tamo::LIBID_tamo>,
   public FvOverlayImpl
{
friend class GeospatialScene;
   // --- Construction ----------------------------------------
public:
   C_TAMask_ovl();

   BEGIN_COM_MAP(C_TAMask_ovl)
      COM_INTERFACE_ENTRY(ITerrainAvoidanceMaskOverlay)
      COM_INTERFACE_ENTRY_CHAIN(FvOverlayImpl)
   END_COM_MAP()

   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

protected:
   // call Release instead
   virtual ~C_TAMask_ovl();

   virtual void Finalize();

   // --- Public Methods -- General overlay functionality -----
public:

   // ITerrainAvoidanceMaskOverlay
   //
   STDMETHOD(raw_UpdateAltitude)(double dCenterLat, double dCenterLon, double dAltitudeFeet, double dTrueHeading,
      IUnknown * pFvOverlay, 
   struct tamo::BullseyeProperties bullseyeProperties);

   int draw_to_base_map(ActiveMap* map);

   // Each overlay is given a chance to modify the base map in draw_to_base_map.  They
   // can either add new alpha blended pixmaps via IGraphicContext's PutPixmapAlpha or 
   // they can add a pixmap via PutPixmap.  If this overlay will be drawing pixmaps
   // to the base map with PutPixmap the rendering engine has to be setup to do this.  Return
   // TRUE if your overlay will call IGraphicsContext::PutPixmap in draw_to_base_map
   virtual BOOL can_add_pixmaps_to_base_map() { return TRUE; }

   int draw(ActiveMap* map);

   int DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector);

   bool m_DrawAsMask;

   int C_TAMask_ovl::pre_close(boolean_t *cancel);

   // General overlay responsibilities
   const CString& get_name() const; // Long/Text name of the overlay
   const char *get_class_name(void);// i.e. "C_vmap_ovl"

   // Right click menu
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list);

   // Displays a CRemarkDisplay dialog for the currently selected object.
   void show_info();

   // returns the top-most item on the map at the specified point.  Underlying items
   // are in the selected list (0 it the top one)
   C_icon *icon_hit_test(CPoint point);
   C_icon* hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point){return hit_test(map, point, false);}
   C_icon* hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point, bool Verbose);

   static void peak_info(ViewMapProj *map, C_icon *pt);
   // Get information for pt on the map
   static void point_info(C_overlay *static_overlay);

   static void ToggleStatusWindow(C_overlay *static_overlay);

   int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT /*flags*/);
   int selected(IFvMapView* pMapView, CPoint, UINT);
   void invalidate_selected();

   int show_info(ViewMapProj* map);
   int show_info(CPoint);

   void release_focus();

   HintText *get_default_hint(MapProj* map);

   boolean_t test_snap_to(ViewMapProj* map, CPoint point);
   boolean_t do_snap_to(ViewMapProj* map, CPoint point,
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

   const CString& get_specification () const;




   // --- Acccessor Methods -----------------------------------

   void SetReferenceAltitudeBreakpoints(float warning_ft, float caution_ft,
      float ok_ft);
   void SetWarningClearanceColor(COLORREF color) {m_WarnColor = color; m_PreferencesUpdated=true;}
   void SetShowWarningLevel(bool Value) { m_ShowWarnLevel = Value; InvalidateMap();m_PreferencesUpdated=true;}
   void SetCautionClearanceColor(COLORREF color) {m_CautionColor = color;m_PreferencesUpdated=true;}
   void SetShowCautionLevel(bool Value) { m_ShowCautionLevel = Value; InvalidateMap();m_PreferencesUpdated=true;}
   void SetOKClearanceColor(COLORREF color) {m_OKColor = color;m_PreferencesUpdated=true;}
   void SetShowOKLevel(bool Value) { m_ShowOKLevel = Value; InvalidateMap();m_PreferencesUpdated=true;}
   void SetShowNoDataMask(bool Value) { m_ShowNoDataMask = Value; InvalidateMap();}
   void SetNoDataColor(COLORREF color) {m_NoDataColor = color;}

   void SetShowLabels(int state) {m_ShowLabels = state; }
   void SetDisplayThreshold (const MapScale& scale) { m_DisplayThreshold = scale; }
   void SetLabelThreshold (const MapScale& scale) { m_LabelThreshold = scale; }

   void SetTestAlt(float altitude) { 
      m_TestAlt = altitude; 
      SetAltitude(altitude, NULL);
   }
   static void SetUserAltitude(C_overlay *static_overlay);

   /// <param name="pOverlay">Indicates which overlay the altitude was set from.  This can be NULL
   /// if the altitude is being set explicitly by the user rather than from another overlay</param>
   void SetAltitude(float altitude, C_overlay* pOverlay);

   // update the altitude from the given overlay.  Determines if the given overlay is top-most and clips
   // the mask based on the given bullseye properties
   void UpdateAltitude(double dCenterLat, double dCenterLon, float altitudeFeet, double dTrueHeading,
      C_overlay* pOverlay, CBullseyeProperties* pBullseyeProperties);

   // The terrain altitude mask can be clipped to help improve performance.  For example, the
   // moving map overlay will enable clipping whenever the bullseye is enabled.  The following
   // methods can be use to setup a clip region for the mask.

   /// <summary>
   /// Set up a circular clip region with the given center and radius
   /// </summary>
   /// <param name="dCenterLat">Center latitude of the circular region to be clipped</param>
   /// <param name="dCenterLon">Center longitude of the circular region to be clipped</param>
   /// <param name="dRadiusMeters">Radius, in meters, of the circular region to be clipped</param>
   /// <param name="dStartHeading">Starting heading of the circular arc to be clipped</param>
   /// <param name="dEndHeading">Ending heading of the circular arc to be clipped</param>
   /// <param name="dSweepAngle">Sweep angle of the clip region</param>
   void SelectCircularClipRgn(double dCenterLat, double dCenterLon, double dRadiusMeters,
      double dStartHeading, double dEndHeading, double dSweepAngle);

   /// <summary>
   /// Sets up a null clip region.  This call effectively disables any clipping
   /// <summary>
   void SelectNullClipRgn();

   void SetLabelColor(int LabelColor);
   void SetLabelBackgroundColor(int LabelBackgroundColor);
   void SetLabelFontName(const CString& LabelFontName);
   void SetLabelSize(int LabelSize);
   void SetLabelAttributes(int LabelAttributes);
   void SetLabelBackground(int LabelBackground);

   void SetSensitivity(int Sensitivity) {m_Sensitivity = Sensitivity; }
   int GetSensitivity(void) { return m_Sensitivity; }

   void SetDrawContours(int flag) { m_DrawContours = flag; }
   void SetDrawMask(int flag) { m_DrawMask = flag; }

   void SetShading (int level) { m_Shading = level; }
   void SetDataSource (int Source) { m_Source = Source; }

   CIconImage* m_peak_icon;

   COLORREF ConvertAltitudeToColor(double dAltitudeMeters, double dElevationMeters);

   void ResetAltitudeUpdated() { m_altitude_updated = false; }

   void AddParamListener(TAMaskParamListener_Interface* listener)
   {
      m_listeners.push_back(listener);
   }

   // --- Private Methods -------------------------------------
private:
   // Based on the current screen, load all tiles that cover the area
   // returned in the coverage list from the framework
   int load_tiles(ActiveMap* map);
   int load_tiles(ActiveMap* map, int DTED_Type, float DTED_Seconds, bool Second_Chance = false);
   int load_single_tile(DataItem *item, int DTED_Type);

   void unload_tiles(); // unload m_tiles to force re-loading

   virtual MapScale get_display_threshold();

   // Clear the loaded DTED/contour tiles
   int clear_tiles(CMapLongToContourTile *&tiles);

   void InvalidateMap();                  // Force draw_to_base_map to be called



   // Member data (operating variables)------------------------

   HintText m_hintText;
   CString     m_overlay_name;   // Child should initialize this in constructor
   char        m_class_name[20]; // Child should initialize this in constructor

   // The list of tiles that cover the current map's visible areas
   CMapLongToContourTile *m_Tiles;
   DataCache   m_DataCache;

   // Use tile_id to retrieve the DTED level specification of the MAIN
   // tiles of DTED data (NOT THE SUB-TILES).  This is important since the 
   // sub-tiles are not held by the map data manager so the smallest dted
   // "item" we can have is a filel DTED tile.
   CMap<int, int, int, int> m_TileLevels;

   static double  m_CurrentLat;
   static double  m_CurrentLon;
   static int     m_CurrentElevation;

   // The DTED levels used to generate the mask
   long m_bDtedTypesUsed[3];

   static CString m_specification;

   // *** INACTIVE *** 
   // Symbology engine lookups are done through here.

   const CString  m_RegistrySection;

   bool m_ContoursValid;

   C_TAMask_point *m_current_selection_pt;// selected object (pointer to below if selected)
   C_TAMask_point m_selection_pt;         // selected object returned from hit_test, etc,... 
   HintText       m_hint;
   CList<C_TAMask_point*, C_TAMask_point*> m_selected_list;

   int            m_MaxElevation;         // Peak height and location(s)
   CList<d_geo_t, d_geo_t&> m_MaxLocation;

   IDtedPtr       m_spDted;               // Used to retrieve DTED data



   // Member data (User Options)
   MapScale m_DisplayThreshold;
   MapScale m_LabelThreshold;

   int      m_ShowLabels;           // User option to show contour labels

   COLORREF m_WarnColor;            // Color for warning clearance level
   COLORREF m_CautionColor;         // Color for Caution clearance level
   COLORREF m_OKColor;              // Color for OK Clearance level
   COLORREF m_NoDataColor;          // Color for No Data mask

   int      m_Shading;              // % Shading of filled areas
   int      m_Source;               // DTED Source selection (0-auto)

   int      m_SpecialDTEDMapDisp;   // Special case display on DTED maps
   int      m_DrawContours;         // Draw contour lines for altitude masking
   int      m_DrawMask;             // Draw blended area mask for altitude masking
   bool     m_MissingData;          // Missing data was detected in display area

   float    m_TestAlt;              // Fixed point altitude when not using automation
   float    m_Altitude;             // automation supplied altitude (MSL expected)

   float    m_WarnClearance;        // Upper (Red) contour -- no fly area
   float    m_CautionClearance;     // Lower (Yellow) contour -- fly with caution, low AGL altitude
   float    m_OKClearance;          // Bottom (Green) contour -- OK to fly

   bool     m_ShowWarnLevel;        // Always true, but still code option
   bool     m_ShowCautionLevel;     // Show the caution level mask / contours
   bool     m_ShowOKLevel;          // Show the OK level mask / contours
   bool     m_ShowNoDataMask;       // Show the No Data mask

   int      m_Unit;
   int      m_ThinningLevel;

   float    m_MaxAlt;               // Maximum Altitude in search area
   long     m_Lat;                  // Location of peak
   long     m_Lon;

   COLORREF m_LabelColor;           // Font options
   COLORREF m_LabelBackgroundColor;
   CString  m_LabelFontName;
   int      m_LabelSize;
   int      m_LabelAttributes;
   int      m_LabelBackground;

   int      m_Sensitivity;          // During live playback, alt change before accepted

   int      m_screen_mask_size;     // Hold the memory for the screen mask to 
   unsigned char* m_screen_mask;    // block unnecessary reallocs

   double   m_dpp_lat_DTED;         // Degree resolution of DTED used
   double   m_dpp_lon_DTED;

   CTAMaskStatus* m_StatusDlg;      // Status window for the overlay
   bool     m_StatusDlgShowing;     // Is the status window supposed to be visible?
   bool     m_PreferencesUpdated;   // Flag to refresh the status dialog settings

   CMaskClipRgn *m_pMaskClipRgn;    // The clipping region for the mask

   bool     m_DrawSkipped;          // It was determined in draw_to_base_map that the requested area is offscreen

   // This flag is set whenever an overlay updates the altitude. It is reset whenever the 
   // TA mask is drawn.
   bool m_altitude_updated;
   C_overlay* m_altitude_updated_by;
   CBullseyeProperties m_BullseyePropertiesRef;

   std::vector<TAMaskParamListener_Interface *> m_listeners;
};

#endif // !defined(VVOD_H__89E5255C_8652_11D3_8663_00105A9B4838__INCLUDED_)
