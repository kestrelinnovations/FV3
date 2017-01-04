// Copyright (c) 1994-2011,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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


// gps.h

#ifndef GPS_H
#define GPS_H 1

#include "overlay.h"
#include "geo_tool_d.h"
#include "utils.h"   // for color_index_t
#include "ovlelem.h"
#include "FvMappingCore\Include\MapProjWrapper.h"
#include "..\..\..\fvw_core\FvBaseDataSet\FvBaseDataSet.h"

// auto saves every minute
static const UINT AUTO_SAVE_TIMEOUT = 60000;
static const UINT PREDICTIVE_PATH_TIMEOUT = 1000;
static const UINT THREE_D_RETRY_TIMEOUT = 2000;       // If no following updata
static const float GPS_UNKNOWN_ALTITUDE = -9999.0f;

// register window messages used to notify client applications of a gps connect,
// gps disconnect, and whenver a new point is added to the gps overlay
extern const UINT wm_Connect;
extern const UINT wm_Disconnect;

// DECLARED CLASSES
class C_gps_trail;
class GPSPointIconList;
class GPSPointSubList;
class GPSPointIcon;
class CGpsStyle;
class GPSToolsPropertySheet;
class CMovingMapAutoSaveTimer;
class CPredictivePathTimer;
class GPS3DPoint;
class CFrozenTrailTimer;

// REFERENCED CLASSES
class COverlayCOM;
class ActiveMap;        //declared in \include\map.h
class CCourseIndicator; //declared in \gps\cdi.h
class CGpsStyle;        //declared in \gps\gpsstyle.h
class CGpsPlaybackDlg;  //declared in \gps\playback.h
class CoastTrackManager;
class RangeBearingManager;
class RangeBearingDragger;
class RangeBearing;
class OvlSymbol;
class TabularEditorDataObject;
class GPSDataObject;
class MovingMapFeed;
class CLinksEditDlg;
class CPredictivePath;
class MovingMapCameraEvents;

#include "..\MovingMapOverlay\gpsstyle.h"

//our GPS global y2k rule
int GPS_get_y2k_compliant_year(int year);

// typedefs 
typedef unsigned short GPS_options_t;

typedef struct _rb_ref_t_
{
   _rb_ref_t_() { }
   _rb_ref_t_(RangeBearing *r, boolean_t fnt)
   {
      rb_obj = r;
      from_not_to = fnt;
   }

   RangeBearing *rb_obj;
   boolean_t from_not_to;
} rb_ref_t;

struct GPS_QUEUED_POINT
{
   float latitude;
   float longitude;
   float msl;
   float true_heading;
   float magnetic_heading;
   float speed_knots;
   float speed_km_hr;
   DATE dateTime;
   CString meta_data;
};

// GPS options bit flags 
#define GPS_LEAVE_TRAIL  ((GPS_options_t)0x0001)
#define GPS_AUTO_CENTER  ((GPS_options_t)0x0002)
#define GPS_AUTO_ROTATE  ((GPS_options_t)0x0004)
#define GPS_HEADING_TRUE ((GPS_options_t)0x0008) // true or magnetic heading
#define GPS_SPEED_KNOTS  ((GPS_options_t)0x0010) // knots or km/hr speed units
#define GPS_UPDATE_INFO  ((GPS_options_t)0x0020)
#define GPS_ALL_INPUT_ECHO_ON  ((GPS_options_t)0x4000)
#define GPS_MOVING_MAP_ECHO_ON ((GPS_options_t)0x8000)

#define RANGE_BEARING_RADIUS 10

class SymbolContainer
{
   // the constructor will open and read in a list of symbol files
   CList <OvlSymbol *, OvlSymbol *> m_symbol_lst;

   static SymbolContainer *s_pSymbolContainer;

   // constructor
   SymbolContainer();

public:

   static SymbolContainer *GetInstance()
   {
      if (s_pSymbolContainer == NULL)
         s_pSymbolContainer = new SymbolContainer;

      return s_pSymbolContainer;
   }

   static void DestroyInstance()
   {
      delete s_pSymbolContainer;
      s_pSymbolContainer = NULL;
   }

   // destructor
   ~SymbolContainer();

   int get_num_symbols() { return m_symbol_lst.GetCount(); }
   OvlSymbol *get_symbol(int i);
   int define_symbol(OvlSymbol *sym_to_set, CString sym_name);
};

CString extract_element(const CString &str, const CString element, int start_from, int& new_pos);

class GPSPointIcon;
typedef std::shared_ptr< GPSPointIcon* > GPSPointIconPtr;


//---------------------------------------------------------------------------
class GPSPointIcon: public C_icon
{

public:
   // Constructor
   GPSPointIcon();
   ~GPSPointIcon();
   GPSPointIcon(C_overlay *overlay, CRect &rect);
   void initialize();

private:
   CString m_comment;
   boolean_t m_has_comment;

   // is this GPS Point selected
   boolean_t m_selected;

   float m_latitude;
   float m_longitude; 

   CStringArray m_links;

public:
   // get/set for m_comment
   CString get_comment_txt() { return m_comment; }
   void set_comment_txt(CString txt) { m_comment = txt; }

   // get/set for m_has_comment
   boolean_t has_comment() { return m_has_comment; }
   void set_has_comment(boolean_t has_comment) { m_has_comment = has_comment; }

   // get/set for m_selected
   boolean_t get_selected() { return m_selected; }
   void set_selected(boolean_t selected) { m_selected = selected; }

   // The GPS trail point can have multiple locations associated with  (e.g, a 
   // Predator's target and sensor).  center_num = 0 is the normal ship's position
   //
   float get_latitude() { return m_latitude; }
   void set_latitude(float lat) { m_latitude = lat; }

   float get_longitude() { return m_longitude; }
   void set_longitude(float lon) { m_longitude = lon; }

   void GetLinks(CStringArray& links) 
   { 
      links.Append(m_links);
   }
   void SetLinks(CStringArray& links)
   {
      m_links.RemoveAll();
      m_links.Append(links);
   }
   void AddLink(CString& link)
   {
      m_links.Add(link);
   }

   int GetLinksCount() { return m_links.GetSize(); }

   void get_center(int &center_num, double &lat, double &lon);

   // returns whether this point has multiple centers associated with its meta-data
   boolean_t has_multiple_centers();


public:
   CRect m_rect;
   int m_x;
   int m_y;
   CPoint m_arrow_start;
   GPSPointIcon *m_prev;

   // in the display list calc we determine whether or not this point
   // is in the future, if it is then m_show_point will be set to FALSE
   // and it will not get drawn.
   // Also, when the view time is changed, when can set this flag 
   // appropriately.  This allows, the draw() member to call redraw() rather
   // than new_draw() while in playback
   boolean_t m_show_point;

   float m_speed_knots;
   float m_speed_km_hr;
   float m_true_heading;
   float m_magnetic_heading;
   float m_msl;               // mean sea level altitude, meters
   boolean_t m_viewable;

   // 3D info
   long m_3DHandle;
   GPSPointIconPtr m_sppThis;  // Self referenced

   CString m_meta_data;

   // builds a list of OvlElements from the meta_data
   void build_display_elements();
   void build_display_elements_lines();
   void build_display_elements_polygon();
   void build_display_elements_ellipse();
   void build_display_elements_text();
   void build_display_elements_icon();
   void build_display_elements_symbol();
   void build_display_elements_rect();
   void draw_display_list(MapProj *map, CDC *pDC, BOOL top);
   void extract_font(const CString &data, OvlFont &font);
   void extract_properties(const CString &data, OvlPen &pen);
   COLORREF get_color(CString str);

   CList <OvlElement *, OvlElement *> m_display_list_top;
   CList <OvlElement *, OvlElement *> m_display_list_bottom;

public:
   COleDateTime m_dateTime;   //GPS Playback stuff
   float get_second() { return (float)m_dateTime.GetSecond(); };
   unsigned char get_minute() { return m_dateTime.GetMinute(); };
   unsigned char get_hour() { return m_dateTime.GetHour(); };
   unsigned char get_day() { return m_dateTime.GetDay(); };      //1-31
   unsigned char get_month() { return m_dateTime.GetMonth(); };  //1-12
   unsigned char get_year() { return m_dateTime.GetYear()%100; };    //100-99
   // All return 255 for unknown.
#define GPS_ATOMIC_CLOCK_BASE_TIME (COleDateTime(1980,1,6,0,0,0))
#define GPS_VALID_BASE_DATE (COleDateTime(1971,1,1,0,0,0)) //1971 and beyond is valid
   boolean_t valid_date() {return (m_dateTime.GetStatus()==COleDateTime::valid && m_dateTime >= GPS_VALID_BASE_DATE );};
   boolean_t valid_time() {return (m_dateTime.GetStatus()==COleDateTime::valid);};
   boolean_t invalid_time() {return (m_dateTime.GetStatus()==COleDateTime::invalid);};
   boolean_t null_time() {return (m_dateTime.GetStatus()==COleDateTime::null);};

   float get_time() {return m_dateTime.GetHour()*3600.0f + 
      m_dateTime.GetMinute()*60.0f + m_dateTime.GetSecond();};

   void set_time(float seconds)
   { 
      int s=(int)seconds;
      if (valid_date())
         m_dateTime.SetDateTime( m_dateTime.GetYear(), m_dateTime.GetMonth(), m_dateTime.GetDay(), s/3600, (s/60)%60,  s%60 );
      else
         m_dateTime.SetTime( s/3600, (s/60)%60, s%60 );
   };

   void get_date(int& year, int& month, int& day)
   {
      year=m_dateTime.GetYear();
      month=m_dateTime.GetMonth();
      day=m_dateTime.GetDay();
   };

   void set_date(int year, int month, int day)
   {
      year = GPS_get_y2k_compliant_year(year);
      if (valid_time())
         m_dateTime.SetDateTime(year,month,day,m_dateTime.GetHour(),m_dateTime.GetMinute(),m_dateTime.GetSecond());
      else
         m_dateTime.SetDate(year,month,day);
   };

   COleDateTime get_date_time() { return m_dateTime; };

public:

   // Returns TRUE if this icon is a instance of the given class.
   boolean_t is_kind_of(const char *class_name);

   // Returns a string identifying the class this object is an instance of.
   LPCSTR get_class_name() { return "GPSPointIcon"; }

   // Descriptive string functions
   CString get_help_text();
   CString get_tool_tip(); //{ return "GPS Point"; }

   // Determine if the point is over the turn point.
   boolean_t hit_test(CPoint point);

   // resets all member to their default values
   void reset_point();

   // calculates data not already set, like speed and heading, returns
   // FAILURE if the point is not valid
   int calc();

   // assignment operator
   GPSPointIcon& operator =(GPSPointIcon &point);

   // returns TRUE if the point contains a valid location, FALSE otherwise
   boolean_t is_valid();

   // get the overlay this point is in
   C_gps_trail *get_overlay();

   static void draw( CDC *pDC, boolean_t flag, CPoint pos, int radius );
   static void draw( CDC *pDC, boolean_t flag, int x, int y, int radius );
   static CPoint prevPoint;

   void draw( MapProj *map, CDC *pDC, gps_icon_t icon_type, int icon_fg_color);

   static double m_map_rotation;
   static CPen *m_p_pen_back;
   static CPen *m_p_pen_fore;
   static CPen *m_p_pen_prev;
}; // end GPSPointIcon


//---------------------------------------------------------------------------
class GPSPointIconList : public C_icon_list<GPSPointIcon, 100>
{
public:
   typedef CList <GPSPointSubList *, GPSPointSubList *> dSubtrailList;

private:
   dSubtrailList m_subtrail_list;
   GPSPointIcon *m_trail_end;  // point for last trail icon
   //   GPSPointIcon *m_trail_beg;  // point for first trail icon
   CRect m_rect;

public:
   // Constructor
   GPSPointIconList();

   // Remove and destroy all sub list elements
   void Clear();

   // add the icon to the end of the list
   int add_point(GPSPointIcon *icon, C_gps_trail *pTrail=NULL );

   // Add the icon list element to the sublist(s)
   int add_subtrail_point( POSITION icon_list_position );

   // determine if the point hits the one of the points in the list
   C_icon *hit_test(CPoint point);

   // get bounding rectangle on trail in view coordinates
   CRect &get_rect() { return m_rect; }

   // display all the points in the list from scratch
   int new_draw(ActiveMap* map, d_geo_t ll, d_geo_t ur, int point_radius, C_gps_trail *pTrail);

   // redisplay all the points in the list that fall within the invalidate
   // region, clip_rect
   int redraw(ActiveMap* map, CRect clip_rect, int radius, C_gps_trail *pTrail);

   // Get the last trail icon to get drawn.  This function is used during
   // the moving map display to manipulate the trail without having to do
   // a complete draw from scratch.
   GPSPointIcon *get_trail_end() { return m_trail_end; }

   // Set the last trail icon to get drawn to the given point.  This function
   // is used during the moving map display to manipulate the trail without
   // having to do a complete draw from scratch.
   void set_trail_end(GPSPointIcon *trail_end) { m_trail_end = trail_end; }

   // Adjust the bounding rect on the trail to include this rect.  This
   // function is used during the moving map display to manipulate the trail
   // without having to do a complete draw from scratch.
   void adjust_rect(CRect rect);

   // This may be an abomination of OOP but we need it for the GPSDataSource implementation
   dSubtrailList *GetSubtrailList() {return &m_subtrail_list;}
};
// end GPSPointIconList


//---------------------------------------------------------------------------
// This class is used to manage the display of a subset of the points in a
// C_gps_trail overlay, below.  It also assists in the hit_test operation.
// This class doesn't contain any points, it only knows the position in the
// icon list of the first point and the last point in the list.

class GPSPointSubList
{
public:
   static const int m_max_count;

private:
   GPSPointIconList &m_parent;
   POSITION m_first_element;
   POSITION m_last_element;
   int m_element_count;
   d_geo_t m_ll;
   d_geo_t m_ur;
   CRect m_rect;

public: 
   // Constructor
   GPSPointSubList(GPSPointIconList &parent, POSITION first_element);

   // Destructor
   virtual ~GPSPointSubList() {}

   // add a point to the list, expand the geo bounds accordingly.
   int add(POSITION next_element);

   // Returns TRUE if the sublist has the maximum number of elements for a
   // sublist.  The add member will fail if the list is full.
   boolean_t is_full() { return m_element_count == m_max_count; }

   // get bounding rectangle on sub-trail in view coordinates
   CRect &get_rect() { return m_rect; }

   // return the bounding rect in lat lon coordinates
   void get_bounds(d_geo_t *ll, d_geo_t *ur) { *ll=m_ll; *ur=m_ur;}

   // determine if the point hits the one of the points in this sub-trail
   C_icon *hit_test(CPoint point);

   // expand the bounding rectangle on the sub-trail to include this rectangle
   void union_rect(CRect rect);

   // display the points in this GPS sub-trail from scratch
   int new_draw(ActiveMap* map, d_geo_t ll, d_geo_t ur, int point_radius, C_gps_trail *pTrail);

   // redisplay all the points in the sub-trail that fall within the invalidate
   // region, clip_rect
   int redraw(ActiveMap* map, CRect clip_rect, int radius, C_gps_trail *pTrail);

   POSITION get_first() {return m_first_element;}
   POSITION get_last()  {return m_last_element;}
}; // end class GPSPointSubList


// GPS3DPoint - path points as drawn in 3D
typedef std::shared_ptr< GPS3DPoint* > GPS3DPointPtr;

class GPS3DPointBase
{
public:
   C_gps_trail*         m_pGPSTrail;
   d_geo_t              m_gptLatLon;
   double               m_dAltMeters;
   double               m_dHeadingDeg;
   double               m_dSpeedFtPerSec;
   COleDateTime         m_dtDateTime;
   int                  m_iUpdateSequence;

   // 3D info
   enum GPSPT_TYPE_ENUM
   { GPSPT_UNKNOWN, GPSPT_SHIP, GPSPT_TRAIL,
      GPSPT_NORM_PATH, GPSPT_WARN_PATH, GPSPT_CRIT_PATH }
                        m_ePointType;

   long                 m_3DHandle;

   GPS3DPointBase()    // Default constructor
      {
#        if GPSPT_UNKNOWN != 0
#           error GPSPT_UNKNOWN assumed to be zero
#        endif
         ZeroMemory( this, sizeof(*this) );
      }
}; // class GPS3DPointBase


class GPS3DPoint : public GPS3DPointBase
{
public:
   GPS3DPointPtr m_sppThis;  // Self referenced

   GPS3DPoint()
   {
      m_sppThis = std::make_shared< GPS3DPoint* >( this );
   }

   GPS3DPoint( C_gps_trail* pGPSTrail )
   {
      m_pGPSTrail = pGPSTrail;
      m_sppThis = std::make_shared< GPS3DPoint* >( this );
   }

   GPS3DPoint( const GPS3DPoint& ppt )
   {
      *this = ppt;
   }

   ~GPS3DPoint();
   
   // Copy the data but the self-reference shared pointer is new
   GPS3DPoint& operator=( const GPS3DPoint& pt )
   {
      ATLASSERT( pt.m_3DHandle == 0 && "Can't copy active point" );
      *static_cast< GPS3DPointBase* >( this ) =
         *static_cast< const GPS3DPointBase* >( &pt );
      m_sppThis = std::make_shared< GPS3DPoint* >( this );
      return *this;
   }

   void RemoveFrom3D();
}; // class GPS3DPoint

typedef std::vector< GPS3DPoint > GPS3DPoints;
typedef GPS3DPoints::iterator GPS3DPointsIter;


//---------------------------------------------------------------------------
#define SHIP_SELECTED_BORDER (4)    //how much extra sizing for the border
#define POINT_SELECTED_BORDER (4)

class C_gps_trail : 
   public IDispatchImpl<FvDataSourcesLib::IFvDataSourceProvider, &__uuidof(FvDataSourcesLib::IFvDataSourceProvider), &FvDataSourcesLib::LIBID_FvDataSourcesLib, /* wMajor = */ 1>,
   public IDispatchImpl<ISkyViewConnectionEvents, &__uuidof(ISkyViewConnectionEvents), &LIBID_FvSkyViewOverlayServerLib, /* wMajor = */ 1>,
   public IDispatchImpl<ISkyViewConnectionCameraEvents, &__uuidof(ISkyViewConnectionCameraEvents), &LIBID_FvSkyViewOverlayServerLib, /* wMajor = */ 1>,
   public CFvOverlayPersistenceImpl,
   public PlaybackEventsObserver_Interface,
   public PlaybackTimeSegment_Interface
{

public:
   friend class CGPSDataSource;
   friend class GPSTrailDisplayElementProvider;

   // Static critical_section struct
   class InterlockData
   {
   public:
      InterlockData()
      {
         ::InitializeCriticalSection( &m_csLock );
      }
      ~InterlockData()
      {
         ::DeleteCriticalSection( &m_csLock );
      }
      operator CRITICAL_SECTION& (){ return m_csLock; }
   private:
      CRITICAL_SECTION m_csLock;
   };  // InterlockData

   class C3DCatchUpTimer : public FVW_Timer
   {
   private:
      C_gps_trail *m_pMovingMapOverlay;

   public:
      C3DCatchUpTimer( C_gps_trail* pMovingMapOverlay, int period) :
         FVW_Timer(period),
         m_pMovingMapOverlay(pMovingMapOverlay)
      {
      }

      // override of base class function
      virtual void expired()
      {
         m_pMovingMapOverlay->update_3d_camera();
      } 

   }; // class C3DCatchUpTimer


   // Constructor
   C_gps_trail();

   virtual void Finalize();

   BEGIN_COM_MAP(C_gps_trail)
      COM_INTERFACE_ENTRY2(IDispatch, ISkyViewConnectionCameraEvents)
      COM_INTERFACE_ENTRY(ISkyViewConnectionEvents)
      COM_INTERFACE_ENTRY(ISkyViewConnectionCameraEvents)
      COM_INTERFACE_ENTRY(FvDataSourcesLib::IFvDataSourceProvider)
      COM_INTERFACE_ENTRY_CHAIN(CFvOverlayPersistenceImpl)
   END_COM_MAP()

   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   virtual void set_modified(boolean_t mod_flag);

   // IFvDataSourceProvider
public:
   STDMETHOD(raw_GetDataSource)(IFvDataSource **DataSource);
   STDMETHOD(raw_GetDefaultDataSetName)(BSTR* defaultDataSetName){return E_NOTIMPL;}//TODO
   STDMETHOD(raw_GetNextFeatureId)(BSTR* nextFeatureId){return E_NOTIMPL;}//TODO

   // ISkyViewConnectionEvents
public:
   STDMETHOD(raw_OnSkyViewConnectionInitialized)();
   STDMETHOD(raw_OnSkyViewConnectionTerminating)();

   // ISkyViewConnectionCameraEvents
public:
   STDMETHOD(raw_OnAttachedCameraChanging)();

protected:
   // Destructor - call Release instead
   virtual ~C_gps_trail();

private:

   static GPS_options_t m_gps_options;
   CRect m_rect;
   CRect m_invalidate_rect;
   C_icon *m_last_hit;
   GPSPointIcon *m_selected_point;  // point for last selected trail point
   GPSPointIcon *m_new_end_point;   // the point at the tip end of the trail
   GPSPointIcon m_next_point;       // next point to be added to the trail
   GPSPointIcon m_current_point;    // point for current ship position
   GPSPointIcon m_previous_point;   // point for previous ship position
   int m_next_point_type;            // type of sentence to start m_next_point
#if 0
   float m_current_time;             // the last current time in the trail
#endif
   unsigned char m_satellites;
   float m_north_up_angle;           // computed heading relative to due North
   CRect m_auto_center_rect;         // recenter box for auto-center GPS 
   GPSPointIconList m_icon_list;
   int m_bad_sentence_count;
   boolean_t m_show_communications;
   CString m_moving_map_echo_file;
   CString m_all_input_echo_file;

   // 3D stuff
   GPS3DPoint m_ptShip3D;
   std::vector< BYTE > m_aShipSymbolRawBytes;
   static CLSID s_clsidPngEncoder;
   static int GetPngEncoderClsid();
   DWORD m_dwLastShip3DUpdateTicks;

   //
   boolean_t m_update_3d_camera;      // when true the mov_symbol overlay gets updated when add_point is called. CAB 1/28/99
   HintText m_hint;
   boolean_t m_is_closing;
   boolean_t m_playback_slide;
   COleDateTime m_dt_old;

   OvlBullseye m_bullseye;
   CRect m_altitude_label_bounds;
   CRect m_nickname_label_bounds;

   // can range from -50% to +50% of screen_width/height (based on center). 
   // Used to position the ship in smooth-scrolling+autocenter mode
   static double m_rotation_frac_pos_x;
   static double m_rotation_frac_pos_y;

   static CLinksEditDlg *m_pLinksEditDlg;
   static GPSPointIcon *m_pLinksEditPoint;   // the point being edited

   // need to remember the index of the range bearing object that we are
   // dragging.
   int m_rb_object_index;          
   d_geo_t m_old_rb_location;

   // the gps properties object 
   CGPSProperties m_properties;

   // is the comment dialog up (add/edit)
   boolean_t m_comment_dlg_up;

   // should we update the coast tracks in set_current_view_time.  This
   // is necessary, so the coast tracks don't get updated everytime a new
   // GPS point is added and the update rate is, say, every 15 seconds
   boolean_t m_update_coast_tracks;

   MovingMapFeed *m_moving_map_feed;

   GPSDataObject *m_data_object;

   // Predictive Path points
   CPredictivePath *m_pPredictivePath;
   //CList<GPSPointIcon, GPSPointIcon &> m_predictive_path_points;
   GPS3DPoints m_aPredictivePoints3D;

   boolean_t m_show_symbol;
   boolean_t m_halt_warning_thread;
   boolean_t m_warning_displayed;
   HANDLE m_halt_warning_thread_event;
   boolean_t m_warning_thread_started;

   // center that is being tracked (e.g, the Predator feed can be centered on
   // the sensor or the target)
   int m_tracking_center_num;

   CMovingMapAutoSaveTimer*   m_pAutoSaveTimer;
   CPredictivePathTimer*      m_pPredictivePathTimer;
   CFrozenTrailTimer*         m_pFrozenTrailTimer;
   C3DCatchUpTimer*           m_p3DCatchUpTimer;
   CString                    m_strAutoSaveName; // name used to set the auto save filename

   static InterlockData s_idThreadInterlock;
   CList<GPS_QUEUED_POINT, GPS_QUEUED_POINT> m_listAddPointQueue;

   CStringArray m_links;

   boolean_t m_bSuspendedConnection;
   CComObject<MovingMapCameraEvents>* m_camera_events;

public:
   static CRITICAL_SECTION& get_critical_section(){ return s_idThreadInterlock; }

   void set_update_coast_tracks(boolean_t update) { m_update_coast_tracks = update; }

   void start_frozen_trail_warning();
   void stop_frozen_trail_warning(BOOL invalidate = TRUE);
   void set_show_symbol(boolean_t show) { m_show_symbol = show; }
   boolean_t get_show_symbol() { return m_show_symbol; }
   boolean_t get_halt_warning_thread() { return m_halt_warning_thread; }
   void set_warning_thread_started(boolean_t s) { m_warning_thread_started = s; }
   HANDLE get_halt_warning_thread_event() { return m_halt_warning_thread_event; }
   void set_warning_displayed(boolean_t d) { m_warning_displayed = d; }
   boolean_t is_frozen() { return !m_halt_warning_thread; }

   void cycle_tracking_center_num();

   void SetAutoSaveName(CString strAutoSaveName) { m_strAutoSaveName = strAutoSaveName; }

public:
   // a list of HWNDs that all gps overlays need to notify whenever they 
   // connect, disconnect, or add a gps point
   static CList<HWND, HWND&> m_hwnd_connect_notify_lst;
   CList<HWND, HWND&> m_hwnd_current_pos_notify_lst;
   static int m_connected_handle;

   // add/remove an HWND to/from the connect, disconnect notification list
   static void add_HWND_to_connect_notify_lst(HWND hWnd);
   static void remove_HWND_from_connect_notify_lst(HWND hWnd);

   // add/remove an HWND to/from the current position notification list
   void add_HWND_to_current_pos_notify_lst(HWND hWnd);
   void remove_HWND_from_current_pos_notify_lst(HWND hWnd);

   void notify_clients(GPSPointIcon &point);

public:
   static int m_radius;                     // trail icon radius
   boolean_t m_used_external_client_feed;


   // public member functions
public:

   MovingMapFeed *GetMovingMapFeed() { return m_moving_map_feed; }

   // toggle the gps connection (connect/disconnect)
   int toggle_connection();

   // returns TRUE if this overlay is connected to its feed
   BOOL is_connected();

   // return information about this trail's current gps point
   void get_current_point(GPSPointIcon *gps_point)
   {
      *gps_point = m_current_point;
   }

   // return the currently selected point; returns NULL if no point is
   // currently selected
   GPSPointIcon *get_selected_point() { return m_selected_point; }

   // set the currently selected gps point; can be NULL
   void set_selected_point(GPSPointIcon *point) { m_selected_point = point; }

   // returns TRUE if the info box is up and it was put there
   // by this overlay
   boolean_t get_info_box_up();

   // calculate the correct rectangle for the object and invalidate it
   CRect invalidate_object(GPSPointIcon *point, boolean_t bInvalidate=TRUE );

   // Returns a string identifying the class this object is an instance of.
   const char *get_class_name() { return "C_gps_trail"; }

   // Display GPS symbol at the current position and display the trail
   // if the GPS_LEAVE_TRAIL option is set.
   int draw(ActiveMap* map);

   // Derived classes can implement this method if they need to draw to the 
   // vertical view
   virtual int DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector);

   // make overlay current or get information on GPS point
   int selected(IFvMapView* pMapView, CPoint point, UINT flags);

   // This function sets the cursor and hint to give the user feedback 
   // indicating what will happen if the selected() member is called with the
   // same view, point, flags.
   int on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags);

   static HCURSOR get_default_cursor();

   // Allows the overlay manager to pass double-click notices to the gps overlay,
   // If the point hits a coast track, then the coast track data dialog will
   // be displayed
   virtual int on_dbl_click(IFvMapView *pMapView, UINT flags, CPoint point);

   // determine if the point hits the object's selection region(s)
   C_icon *hit_test(map_projection_utils::CMapProjWrapper* map, CPoint point);

   // add menu items to the popup menu based on context
   void menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode *,
      CFVMenuNode *> &list);

   // menu callbacks
   static void get_point_info(ViewMapProj *map, C_icon *icon);

   // enable/disable update of 3d viewer CAB 1/28/99
   static void enable_3d_camera_update(CString strMenuLabel, LPARAM lparam);   

   static void add_comment(ViewMapProj *map, C_icon *icon);
   static void edit_comment(ViewMapProj *map, C_icon *icon);
   static void delete_comment(ViewMapProj *map, C_icon *icon);
   static void edit_gps_point_links_callback(ViewMapProj *map, C_icon* point);
   static void static_save_links(CString handle, CStringArray &links);
   static void view_link_callback(ViewMapProj *map, C_icon* point, LPARAM index);
   void set_link_edit_dlg_focus(GPSPointIcon *pPoint);

   static void stop_coast_track(ViewMapProj *map, C_icon *icon, LPARAM lparam);
   static void edit_coast_track(ViewMapProj *map, C_icon *icon, LPARAM lparam);
   static void delete_coast_track(ViewMapProj *map, C_icon *icon, LPARAM lparam);

   // add a comment to the the given icon
   void add_comment_hlpr(GPSPointIcon *gps_icon);

   // edit the comment of the given icon
   void edit_comment_hlpr(GPSPointIcon *gps_icon);

   // returns the number of comments associated with this trail
   int get_number_comments();

   static void get_next_new_default_name(CString &default_name);

   // return default file extension for gps files
   const char *get_default_extension();

   // Return FALSE because read-only gps trails are handled.
   boolean_t get_no_read_only() { return FALSE; }

   // Returns the number of points in the trail.
   int get_point_list_length(int *length);

   // Move the GPS symbol to the location given in line if successful.
   // If the GPS_LEAVE_TRAIL option is set then a dot is left at the previous
   // location. If GPS_AUTO_CENTER or GPS_AUTO_ROTATE then the new map center
   // and/or angle will be set based on the results of the recenter test.
   int process_sentence(const char *line);

   // When LEAVE_TRAIL is set all GPS points in the trail will be displayed
   // each time draw() or process_sentence is called. If at some point the
   // user wishes to forget the points and start again, a call to this function
   // will clear the trail.
   int clear_trail(void);

   // Saves the trail into an NMEA compliant gps file.
   int save_as(const CString &filename, long nSaveFormat);

   // Save as GPX http://www.topografix.com/GPX/1/1/#top
   int save_as_gpx(const CString &filespec);

   // get the block of bytes needed to save this gps overlay to disk
   int Serialize(BYTE *&block_ptr);

   // get the block of bytes needed to save a gps trail to disk
   int serialize_trail(BYTE *&block_ptr);
   int serialize_meta_data(BYTE *&block_ptr);
   int serialize_link_data(BYTE *&block_ptr);

   // returns the number of bytes necessary to write out the gps
   // overlay to disk
   int get_block_size();

   // get the number of bytes needed to write out the current gps trail
   int get_trail_block_size();

   // get the number of bytes needed to write out meta data info 
   // for the trail points
   int get_meta_data_block_size();

   // get the number of bytes needed to write out links for trail points
   int get_links_block_size();

   // save the gps in the old ascii file format
   int old_save_as(const CString &filespec);

   // test to see if this route can do a single point snap to at this point
   boolean_t test_snap_to(ViewMapProj *view, CPoint point); 

   // get the snap to objects for this route at this point
   boolean_t do_snap_to(ViewMapProj *view, CPoint point, 
      CList<SnapToInfo *, SnapToInfo *> &snap_to_list);

   // Inform the route that it is about to be closed, if the route is dirty
   // the user will be asked if they want to save the changes.  If the user
   // cancels the close this fucntion will return cancel == TRUE.
   int pre_close(boolean_t *cancel);

   // trigger auto saving the gps trail
   int auto_save_gps_trail();

   virtual int file_new();

   // Loads the trail from an NMEA compliant gps file. It also sets the map
   // center to the last point in the path.
   virtual int open(const CString &filespec);

   // open a gps file using the previous ascii format
   int open_old(CString filespec);

   // open a gps file using the new binary gps format
   int open_new(CString filespec);

   // open a gpx file using IFVGdalDataSource
   int open_gpx(CString filespec);

   // deserialize the given block of data
   int deserialize_trail(BYTE *block);
   int deserialize_meta_data(BYTE *block);
   int deserialize_link_data(BYTE *block);

   // center map so the end of the GPS trail shows up
   virtual boolean_t center_on(ViewMapProj *map);

   // show the gps point info for the last point in the trail
   int show_last_point_info(ViewMapProj *map);

   // returns TRUE to indicate read-only gps trail files can be opened
   static boolean_t open_read_only_check(const char *filespec, CString &excuse,
      UINT &help_id)
   {
      return TRUE;
   }

   int show_communication(const char *line, boolean_t new_buffer = FALSE);

   // get current position
   GPSPointIcon &get_current_position() { return m_current_point; }

   // called by CRemarkDisplay when this overlay looses the info box focus
   void release_focus();

   // get the name of the moving map echo file
   CString &get_moving_map_echo_file() { return m_moving_map_echo_file; }

   // set the name of the moving map echo file
   void set_moving_map_echo_file(const char *file);

   // get the name of the name of the all input echo file
   CString &get_all_input_echo_file() { return m_all_input_echo_file; }

   // set the name of the all input echo file
   void set_all_input_echo_file(const char *file);

   // update the current position icon with this new point
   int add_point(GPSPointIcon &point);
   int add_point(GPS_QUEUED_POINT &qpoint);
   int add_point_from_queue();

   GPSPointIconList *get_icon_list() { return &m_icon_list; }
   GPS3DPoints& get_predictive_points_3D() { return m_aPredictivePoints3D; }
   GPS3DPoint& get_ship_3D() { return m_ptShip3D; }
   std::vector< BYTE >& GetShipSymbolRawBytes() { return m_aShipSymbolRawBytes; }

private:
   GPSPointIcon m_tmp_point_info;

   // private member functions
private:
   // draw gps trail over a fresh map according to m_options
   int new_draw(ActiveMap* map);

   // redraw gps trail over the same map according to m_options
   int redraw(ActiveMap* map);

   // set the icon the info box is focused on
   void set_info_box_focus(GPSPointIcon *point);

   // appends line to the end of the moving map echo file
   void moving_map_echo(const char *line);

   // appends line to the end of the gps input echo file
   void all_input_echo(const char *line);

   // Gets the true heading at the current position, if it is available.  If it
   // isn't available, it will try to compute the heading using the current 
   // position and the next closest trail icon.  This angle is used to set
   // m_north_up_angle, which is used to choose the appropriate icon in
   // display_icon().
   float get_current_heading(int tracking_center_num);

   void set_new_map(MapProj *map, int x, int y);
   void get_delta_xy_discrete(int window_width, int window_height,
      double point_angle, double &delta_x, double &delta_y);
   void get_delta_xy_continuous(int window_width, int window_height,
      double point_angle, double &delta_x, double &delta_y);

   // NMEA functions
   int process_RMC_sentence(const char *sentence);
   int process_GGA_sentence(const char *sentence);
   int process_GLL_sentence(const char *sentence);
   int process_VTG_sentence(const char *sentence);

   static void show_point_info(GPSPointIcon *point);



   // Causes the entire map window to be redrawn completely if no overlay is
   // being dragged when it is called.  It will do nothing otherwise.
   void invalidate_all(boolean_t erase=TRUE);

   // If an overlay is being dragged when this function is called it will add
   // the given rectangle to an internal rectangle and return.  If no overlay
   // is being dragged it will invalidate the saved invalidate rectangle plus
   // the given rectangle.
   void invalidate_rect(CRect &rect);

   static long s_skyview_object_handle;
   static ISampledMotionPlaybackPtr s_sampled_motion_playback;
   static bool s_sv_playback_time_advancement_enabled;
   static double s_sv_playback_time_advancement_multiplier;

public:
   // add a point to the end of the gps trail, and update the
   // display according to m_gps_options
   int add_point();

   void set_update_3d_camera_enabled(boolean_t enable,
      boolean_t update_mapview = TRUE);
   boolean_t get_update_3d_camera_enabled() const
   {
      return m_update_3d_camera;
   }
   void set_motion_playback_samples();

   // returns true if any opened gps overlays are conencted
   static boolean_t connected();

   static boolean_t auto_center_on();
   static boolean_t auto_rotate_on();
   static void reset_auto_center_and_rotate(ViewMapProj *map, boolean_t flag);

public:
   static int        m_toolbar_id;      // current toolbar resource id
   static boolean_t  m_edit_on;
   //turn on/off the moving map mode dialog
   static int set_edit_on(boolean_t edit);
   static boolean_t get_edit_on() { return m_edit_on; }
   static int get_toolbar_id() { return m_toolbar_id; }

   virtual void release_edit_focus();

   //toolbar button toggles
   static boolean_t m_TautoCenter;
   static boolean_t m_TautoRotate;
   static boolean_t m_TtrailPoints;
   static boolean_t m_Tcdi;
   static boolean_t m_Tplayback;
   static boolean_t m_TcontinuousCentering;
   static boolean_t set_toggle( long key );
   static boolean_t get_toggle( long key );
   static void open_options_dialog();
   void open_options_dialog_hlpr();

   void OnConnect(void);            //when the user presses connect
   void OnDisconnect(void);         //when the user presses disconnect
   void CloseCommDlg(void);         //close the communications dialog

   //CDI
   //   static CourseIndicator *m_cdi;
   static CCourseIndicator *m_cdi;
   void cdi_update(void);

   // Skyview 3d terrain viewer (CAB 1/28/99) 
   void update_3d_camera();
   void update_skyview();
   void update_predictive_points();

   // Terrain Avoidance Mask overlay notification
   void update_TAMask();
   // Power status information
   virtual void power_broadcast( UINT nPowerEvent, UINT nEventData );

   // update the predictive path dots
   void update_predictive_path(MapProj *map, COleDateTime time);
   void ComputePredictivePath();
   void ComputePredictivePath(COleDateTime time);
   void timer_update_predictive_path();
   int m_predictive_path_timer_delta;

   // open the GPS Options property sheet for this trail
   void on_gps_options();

   CGPSProperties *get_properties() { return &m_properties; }
   void set_properties(CGPSProperties *properties);

   //the object that contains the stroke symbol
   OvlSymbol *m_shipSymbol;
   OvlSymbol *m_shipSymbol_2;  // duplicate ship symbol for use in the Vertical Display
   void DefineShipSymbols();
   double DefineSymbol(OvlSymbol *shipSymbol);
#if 0 // Not used
   void define_symbol_airlift();
   void define_symbol_bomber();
   void define_symbol_fighter();
   void define_symbol_helicopter();
#endif

   //REGISTRY STUFF
   void load_registry_settings();
   void save_registry_settings();

   //invalidate trails
   static void invalidate_trails();

   //determines the proximity of two points TRUE=too close  FALSE=far enough away
   //   static boolean_t visual_occlusion( GPSPointIcon *p1, GPSPointIcon *p2 );
   //   static boolean_t visual_occlusion( GPSPointIcon *p1, CPoint p2 );
   static boolean_t visual_occlusion( CPoint p1, CPoint p2 );

   //TRAIL PLAYBACK STUFF
   GPSPointIcon *m_playback_head;
   GPSPointIcon *m_playback_tail;
   POSITION m_playback_position_tail; //the list position of the tail
   POSITION m_playback_position_next; //the list position of the point following tail
   GPSPointIcon *get_playback_head();
   GPSPointIcon *get_playback_tail();
#if 0
   COleDateTimeSpan get_playback_elapsed();
#endif
   boolean_t is_in_the_future( GPSPointIcon *point );
   boolean_t in_playback_mode();
   static boolean_t OnGpsPlayback();
   static boolean_t InGpsPlayback();
   boolean_t OnOpenPlayback();
   boolean_t OnClosePlayback();
   void OnTimerPlayback();
   POSITION get_last_position();
   void OnPlaybackReset();
   void OnPlaybackEndset();
   COleDateTimeSpan get_trail_duration();

   boolean_t set_current_view_time_trail(MapProj *map);
   void on_position_changed(MapProj *map);

   // Returns the color that is used by the playback dialog's gannt chart for this overlay
   virtual HRESULT GetGanntChartColor(COLORREF *pColor);

   void SetPlaybackPosition( COleDateTime dt );
   virtual void invalidate();
   int get_trail_count();
   void invalidate_trail_time_slice(COleDateTime dt0, COleDateTime dt1,
      boolean_t do_invalidate);
   boolean_t display_trail_points();
   boolean_t is_topmost_trail();
   boolean_t is_autocentering();
   boolean_t is_autorotating();
   static C_gps_trail *get_topmost_trail();
   COleDateTimeSpan m_rollover;
   COleDateTime m_best_date;
   void midnight_rollover_check( GPSPointIcon& prev, GPSPointIcon& next );

   void DoDateTimeFirstPassAnalysis
      (
      GPSPointIcon *&firstValidPoint,
      GPSPointIcon *&lastValidPoint,
      GPSPointIcon *&firstInvalidPoint,
      GPSPointIcon *&lastInvalidPoint,
      int& validCount,
      int& invalidCount,
      int& validBadSequenceCount,
      int& invalidBadSequenceCount,
      int& firstValidPointInvalidBadSequenceCount
      );
   boolean_t NormalizeTrailPointDates();
   void DoForcedDateSequencing();

   d_geo_t m_deg_per_pixel;
   void handle_mapscale_changes( ActiveMap* map);
   void handle_mapproj_changes();
   double view_distance_to_geo_distance( ActiveMap* map, double viewDistance);
   void set_viewable_points( ActiveMap* map );

   static boolean_t m_need_to_catch_up;
   static int m_playback_rate;

   //GPS III
   int m_gll_sentence_count;

   // Coast Track stuff
public:
   static GPSToolsPropertySheet m_gpstools_dialog;

private:
   static boolean_t m_is_gpstools_dialog_active;

   // CoastTrackManager - the coast track manager stores a list of coast
   // tracks and coast track intercepts.  The class communicates with the
   // coast track dialog and the intercept dialog(s) and insures that they
   // are updated.  This class will receive the time change notifications and
   // tell the coast tracks and coast track intercepts to update themselves.
   CoastTrackManager *m_coast_track_mgr;

   // RangeBearing - the range and bearing manager stores a list of 
   // range and bearing objects
   RangeBearingManager *m_rb_mgr;

   // RangeBearingDragger - handles the drawing of the range bearing drag object
   RangeBearingDragger *m_dragger;

public:
   // open the coast track dialog if it is not already opened.  Will
   // be initialized with the current coast track info from the coast
   // track manager
   static void open_gps_tools(int page_number);

   // returns TRUE if the given property page of the gps tools is active.
   // If a specific page number is not given then this function will return
   // TRUE if the gps tools dialog is active 
   static boolean_t is_gps_tools_opened(int page_number = -1);

   // return a pointer to this overlay's coast track manager
   CoastTrackManager *get_coast_track_mgr() { return m_coast_track_mgr; }

   // return a pointer to this overlay's range/bearing manager
   RangeBearingManager *get_rb_mgr() { return m_rb_mgr; }

private:
   CList <rb_ref_t, rb_ref_t> rb_ref_lst;
public:
   void add_range_bearing_reference(RangeBearing *rb_obj, 
      boolean_t from_not_to);
   void remove_range_bearing_reference(RangeBearing *rb_obj);

   static void set_is_gpstools_dialog_active(boolean_t active)
   { m_is_gpstools_dialog_active = active; } 

   // callback to handle the coast track timing events (coast tracks
   // will get updated automatically every so often)
   static void CALLBACK EXPORT coast_track_timer_proc(HWND hWnd,
      UINT nMsg, UINT nIDEvent, DWORD dwTime);

   //TIME PROCESSING STUFF
   COleDateTime m_current_view_time;
public:

   virtual HRESULT OnPlaybackStarted();
   virtual HRESULT OnPlaybackStopped();
   virtual HRESULT OnPlaybackTimeChanged(DATE dateCurrentTime);
   virtual HRESULT OnPlaybackRateChanged(long lNewPlaybackRate);

   // gets the beginning and ending times for this GPS overlay.  The begin and
   // end times take into account the gps trail as well as any coast tracks.
   // returns TRUE if the beging and end time are valid.
   boolean_t get_view_time_span(COleDateTime& begin, COleDateTime& end);

   // Returns the beginning and ending times for the overlay's time segment.  If the time segment is valid 
   // set pTimeSegmentValue to a non-zero integer, otherwise set it to zero.  An example where the time segment
   // would be invalid is an uncalculated route.  In this case, the overlay is not accounted for in the overall
   // clock time and does not show up in the playback dialog's gannt chart
   virtual HRESULT GetTimeSegment( DATE *pBegin, DATE *pEnd, long *pTimeSegmentValid);

   // get the time span for the current GPS trail.  Returns TRUE if the start
   // and end time are valid
   boolean_t get_view_time_span_trail(COleDateTime &start, COleDateTime &end);

   void set_current_view_time(MapProj *map, COleDateTime time );
   COleDateTime get_current_view_time() { return m_current_view_time; }

   //DISPLAY LIST STUFF
   std::vector<GPSPointIcon *> m_display_list;   
   void display_list_add( GPSPointIcon *point );
   void display_list_reset();
   void display_list_calc( ActiveMap *map );
   void display_list_draw(ActiveMap *map, CRect clip = CRect(-1,-1,-1,-1));
   void draw_predictive_points(ActiveMap *map); // 2D version
   void draw_aux_data(ActiveMap *map);
   bool m_display_list_valid;

   void ship_draw( ActiveMap *map, CRect clip_rect = CRect(-1,-1,-1,-1));
   void selected_draw( CDC *pDC );
   int point_transform( ActiveMap *map, GPSPointIcon *point, int& x, int& y);
   void auto_center_bounding_box_calc(ActiveMap *map);
   boolean_t need_to_recenter_map(ActiveMap* map);

   //SCROLLING TRACK STUFF
   double m_scroll_track_anchor_x; //value 0-1
   double m_scroll_track_anchor_y; //value 0-1
   double m_scroll_track_anchor_angle; //value -180 to 180
   void set_scrolling_track( double nx, double ny, double angle );
   void set_scrolling_track( CPoint p0, CPoint p1 );
   void orient_map_to_scrolling_track(ActiveMap *map);
   void orient_map_to_scrolling_track(ActiveMap *map, double x, double y, double angle );

   //NEW ADD POINT
   void remarkdlg_update();
   void map_update(MapProj* map, boolean_t force_update = FALSE);
   void on_new_end_point( GPSPointIcon *end_point );
   void set_current_point( GPSPointIcon *point );
   boolean_t is_active_trail();
   static C_gps_trail *get_active_trail();

   float get_map_rotation() {return m_north_up_angle;}

   // Returns TRUE if this overlay can be edited by the tabular editor
   virtual boolean_t is_tabular_editable() { return TRUE; }
   virtual TabularEditorDataObject* GetTabularEditorDataObject();

   void OnTabularEditor();
   GPSPointIcon *get_at_index(long i);
   int get_index(GPSPointIcon *);

   GPSPointIcon *FindClosestPointByTime(DATE dt);

protected:
   BOOL IsBetween(GPSPointIcon *pLeft, COleDateTime& dt, GPSPointIcon *pRight);
   GPSPointIcon *ClosestPoint(GPSPointIcon *pLeft, COleDateTime& dt, GPSPointIcon *pRight);

   // Returns true if either SkyView is installed or if the 3D projection
   // is available on this machine
   bool is_3D_camera_available();
   void set_update_skyview_enabled(boolean_t enable);

public:
   void Add3DPoint( GPSPointIcon* pPoint );
   void Remove3DPoint( GPSPointIcon* pPoint );
   void Update3DPoint( GPS3DPoint* pPoint );
   void Remove3DPoint( GPS3DPoint* pPoint );
   void Remove3DPoint( long hPointHandle );

   void SetWorkItems( FalconViewOverlayLib::IWorkItems* pIWorkItems )
   {
      m_work_items = pIWorkItems;
   }

private:
   // Used to queue up work for 3D elements in the backgruond 
   FalconViewOverlayLib::IWorkItemsPtr m_work_items;

}; // end C_gps_trail



// CFvGPSDataSource
// Returned by IFvOverlayData

class CFvGPSDataSource :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FvDataSourcesLib::IFvDataSource, &__uuidof(FvDataSourcesLib::IFvDataSource), &FvDataSourcesLib::LIBID_FvDataSourcesLib, /* wMajor = */ 1>
{
protected:
   C_gps_trail       *m_pGPSTrail;
   IFvDataSetPtr     m_spDataSet;

public:
   CFvGPSDataSource()
      :m_pGPSTrail(NULL), m_spDataSet(NULL)
   {
   }

   BEGIN_COM_MAP(CFvGPSDataSource)
      COM_INTERFACE_ENTRY2(IDispatch, FvDataSourcesLib::IFvDataSource)
      COM_INTERFACE_ENTRY(FvDataSourcesLib::IFvDataSource)
   END_COM_MAP()



   DECLARE_PROTECT_FINAL_CONSTRUCT()

   HRESULT FinalConstruct()
   {
      return S_OK;
   }

   void FinalRelease()
   {
      m_pGPSTrail->Release();
   }

   void Initialize(C_gps_trail *pGPSTrail)
   {
      m_pGPSTrail = pGPSTrail;
   }

   C_gps_trail *GetTrail() { return m_pGPSTrail;}

   // IFvDataSource Methods
public:
   STDMETHOD(get_DataSetCount)(long * count);
   STDMETHOD(raw_GetDataSet)(long index, FvDataSourcesLib::IFvDataSet * * dataSet);
   STDMETHOD(get_Name)(BSTR * Name);
   STDMETHOD(raw_GetDataSetByName)(BSTR Name, FvDataSourcesLib::IFvDataSet * * dataSet);
   STDMETHOD(get_DataSourceCount)(long * count);
   STDMETHOD(raw_GetDataSource)(long index, FvDataSourcesLib::IFvDataSource * * dataSource);
   STDMETHOD(raw_GetDataSourceByName)(BSTR Name, FvDataSourcesLib::IFvDataSource * * dataSource);
   STDMETHOD(raw_Extent2D)(double* min_x, double* min_y, double* max_x, double* max_y);
   STDMETHOD(get_Description)(BSTR * Description);
   STDMETHOD(raw_RegisterForCallbacks)(FvDataSourcesLib::IFvDataSourceCallback * fvDataSourceCallback);
   STDMETHOD(raw_UnregisterForCallbacks)(FvDataSourcesLib::IFvDataSourceCallback * fvDataSourceCallback);
   STDMETHOD(raw_CheckConnectString)(BSTR connectString, VARIANT_BOOL * valid);
   STDMETHOD(raw_Connect)(BSTR connectString);
   STDMETHOD(get_connectString)(BSTR * connectString);
};

class CFvGPSDataSet:
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<IFeatureDataSet, &__uuidof(IFeatureDataSet), &LIBID_FvDataSourcesLib, /* wMajor = */ 1>,
   public IDispatchImpl<IFvDataSet, &__uuidof(IFvDataSet), &LIBID_FvDataSourcesLib, /* wMajor = */ 1>,
   public CFvBaseDataSet
{
public:
   BEGIN_COM_MAP(CFvGPSDataSet)
      COM_INTERFACE_ENTRY2(IDispatch, IFeatureDataSet)
      COM_INTERFACE_ENTRY(IFeatureDataSet)
      COM_INTERFACE_ENTRY(IFvDataSet)
   END_COM_MAP()


   CFvGPSDataSet():m_pDataSource(NULL),
      m_pIconList(NULL),
      m_current_icon_pos(NULL)
      ,m_current_subtrail_pos(NULL)
   {}


   DECLARE_PROTECT_FINAL_CONSTRUCT()

   HRESULT FinalConstruct()
   {
      return S_OK;
   }

   void FinalRelease()
   {
      m_pDataSource->Release();
      delete m_pFeatureDefn;
   }

   void Initialize(CFvGPSDataSource *pDSource, GPSPointIconList *pIconList);

   // Overrides of base class COM methods
   STDMETHOD(raw_Extent2D)(double* min_x, double* min_y, double* max_x, double* max_y);
   STDMETHOD(get_Name)(BSTR * Name);
   STDMETHOD(get_Description)(BSTR * Description);

   // Base class implementations.  I needed to implement these here because it apears that the
   // generated TLH declares the following methods and we cant compile unless we were to manually
   // remove them from tlh file.
   STDMETHOD(get_FeatureCount)(long * count)
   {
      return CFvBaseDataSet::get_FeatureCount(count);
   }

   STDMETHOD(raw_GetFeature)(long index, IFeature **feature)
   {
      return CFvBaseDataSet::raw_GetFeature(index,feature);
   }

   STDMETHOD(get_Filter)(FvDataSourcesLib::IFilter * * Filter)
   {
      return CFvBaseDataSet::get_Filter(Filter);
   }

   STDMETHOD(put_Filter)(FvDataSourcesLib::IFilter * Filter)
   {
      return CFvBaseDataSet::put_Filter(Filter);
   }

protected:
   OGRFeature *GPSIcon2OGRFeature(GPSPointIcon* picon);

   // Virtual Helper functions called by base class
   virtual OGRFeatureDefn* GetFeatureDefn();
   virtual OGRFeature   *SearchSubtrail(GPSPointSubList *subtrail, POSITION start_pos);
   virtual OGRFeature   *GetFirstFeature_prefilter();
   virtual OGRFeature   *GetNextFeature_prefilter();
   virtual int          GetUnfilteredFeatureCount();

   CFvGPSDataSource     *m_pDataSource;
   GPSPointIconList     *m_pIconList;
   POSITION             m_current_icon_pos;
   POSITION             m_current_subtrail_pos;

};

class CGPSCommentTextEdit : public CDialog
{
   // Construction
public:
   CGPSCommentTextEdit(CWnd* pParent = NULL);   // standard constructor
   ~CGPSCommentTextEdit(void);                  // destructor

   CString get_text(void) { return m_text; }
   void set_text(CString text) { m_text = text; }

   // Dialog Data
   //{{AFX_DATA(CGPSCommentTextEdit)
   enum { IDD = IDD_DYNAMIC_EDIT_ALT };
   CString getHelpURIPath(){return "fvw.chm::/fvw_core.chm::/FVW/Comment_Dialog.htm";}

   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CGPSCommentTextEdit)
protected:

   //}}AFX_VIRTUAL

   // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(CGPSCommentTextEdit)
   virtual BOOL OnInitDialog();
   virtual void OnOK();
   afx_msg void OnEditChange();
   afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
   afx_msg void OnApply();
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   CString m_text;
   boolean_t m_initialized;
};

class CMovingMapAutoSaveTimer : public FVW_Timer
{
private:
   C_gps_trail *m_pMovingMapOverlay;

public:
   CMovingMapAutoSaveTimer(C_gps_trail* pMovingMapOverlay, int period) : FVW_Timer(period),
      m_pMovingMapOverlay(pMovingMapOverlay)
   {
      start();
   }

   // override of base class function
   virtual void expired()
   {
      m_pMovingMapOverlay->auto_save_gps_trail();
      start();
   }
};

class CPredictivePathTimer : public FVW_Timer
{
private:
   C_gps_trail *m_pMovingMapOverlay;

public:
   CPredictivePathTimer(C_gps_trail* pMovingMapOverlay, int period) : FVW_Timer(period),
      m_pMovingMapOverlay(pMovingMapOverlay)
   {
   }

   // override of base class function
   virtual void expired()
   {
      m_pMovingMapOverlay->timer_update_predictive_path();
      start();
   }
};

class CFrozenTrailTimer : public FVW_Timer
{
private:
   C_gps_trail *m_pMovingMapOverlay;

public:
   CFrozenTrailTimer(C_gps_trail* pMovingMapOverlay, int period) : FVW_Timer(period),
      m_pMovingMapOverlay(pMovingMapOverlay)
   {
   }

   // override of base class function
   virtual void expired()
   {
      m_pMovingMapOverlay->start_frozen_trail_warning();
      start();
   }

}; // class CFrozenTrailTimer

#endif

// End of gps.h
