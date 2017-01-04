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

// nitf.h -- nitf file overlay header



#pragma once

//#define ALLOW_DEAD_CODE

#include <list>
#include <float.h>
#include <memory>
#include "overlay.h"
#include "maps_d.h"
#include "ovlelem.h"
#include "ovl_mgr.h"

typedef MSXML2::IXMLDOMDocument2Ptr XML2DocumentPtr;
typedef MSXML2::IXMLDOMNodePtr      XMLNodePtr;
typedef MSXML2::IXMLDOMNodeListPtr  XMLNodeListPtr;

#ifdef GOV_RELEASE
#  define JPEG2000_SUPPORT
#  define JPIP_SUPPORT
#endif

#define NITF_CONVERSIONS_ONLY
#include "Utility\NITFUtilities.h"
#undef NITF_CONVERSIONS_ONLY

#include "Utility\WebReference\WebReference.h"
#include "ImageLibCallbackBase.h"
#include "WStringILess.h"

#pragma warning( push )
#pragma warning( disable : 4192 )
#import "ImageLib.tlb" no_namespace, named_guids
#pragma warning( pop )

#import "NITFDBServer.tlb" no_namespace, named_guids, exclude("EnumImageLibStatus")
#import "NITFSourcesCtrl.tlb" no_namespace, named_guids
enum NITFTempDisplayMaskEnum
{
   NITF_TEMP_DISPLAY_NORMAL_MASK    = ( 0x0001 << QUERY_DLG_TEMP_DISPLAY_NORMAL ),
   NITF_TEMP_DISPLAY_KEEP_TEMP_MASK = ( 0x0001 << QUERY_DLG_TEMP_DISPLAY_KEEP_TEMP ),
   NITF_TEMP_DISPLAY_TEMP_MASK      = ( 0x0001 << QUERY_DLG_TEMP_DISPLAY_TEMP ),
   NITF_TEMP_DISPLAY_SELECTED_MASK  = ( 0x0001 << QUERY_DLG_TEMP_DISPLAY_SELECTED ),
   NITF_TEMP_DISPLAY_FLASH_MASK     = ( 0x0001 << QUERY_DLG_TEMP_DISPLAY_FLASH )
};
#define QUERY_DLG_TOOLBAR_BUTTON_ENABLE_MASK ( 0x0001 << QUERY_DLG_TOOLBAR_BUTTON_ENABLE_BIT )
#define QUERY_DLG_TOOLBAR_BUTTON_CHECKED_MASK ( 0x0001 << QUERY_DLG_TOOLBAR_BUTTON_CHECKED_BIT )

#include "..\nitf\NITF_QueryDlg.h"
#ifdef WARP_TEST
#include "..\nitf\WARP_QueryDlg.h"
#endif

static const VARIANT VARIANT_NULL = { VT_NULL };
static const ULONGLONG MAX_SOURCE_CACHE_BYTES = 1000000000ULL; // 1GB for now

using namespace std;

enum NITFFileTypeEnum
{
   NITF_UNDEFINED_FILE = 0,
   NITF_NITF_FILE = 1,
   NITF_TIFF_FILE = 2,
   NITF_MRSID_FILE = 3,
   NITF_PDF_FILE = 4,
   NITF_JP2_FILE = 5,
   NITF_JPIP_FILE = 6,
   NITF_MISC_FILE = 7
};

static const COLORREF DFLT_QUERY_REGION_COLOR = RGB( 255, 128, 0 );        // Bright orange
static const COLORREF DFLT_FRAME_BUSY_COLOR = RGB( 192, 192, 0 );          // Medium yellow
static const COLORREF DFLT_FRAME_MONO_IMAGE_COLOR = RGB( 224, 224, 224 );  // Soft white
static const COLORREF DFLT_FRAME_COLOR_IMAGE_COLOR = RGB( 255, 0, 0 );     // Red
static const COLORREF DFLT_FRAME_STD_BKGD_COLOR = RGB( 0, 0, 0 );          // Black
//static const COLORREF DFLT_FRAME_SEL_BKGD_COLOR = RGB( 0, 0, 255 );        // Bright blue
static const COLORREF DFLT_FRAME_SEL_BKGD_COLOR = RGB( 0, 230, 0 );        // Bright green
static const COLORREF DFLT_FRAME_TEMP_BKGD_COLOR = RGB( 0, 0, 200 );       // Medium Blue
static const COLORREF DFLT_FRAME_TEMP_KEEP_BKGD_COLOR = RGB( 0, 0, 140 );  // Dark blue
static const INT DFLT_UNSELECTED_BKGD_FRAME_WIDTH = 6;
//static const INT DFLT_SELECTED_BKGD_FRAME_WIDTH = 8;
static const INT DFLT_SELECTED_BKGD_FRAME_WIDTH = 6;  // See color diff
static const INT SELECT_FLASH_TICKS = 2000;
static const INT FLASH_ARROW_ARROW_WIDTH = 40;     // Where full arrow is 100 long
static const INT FLASH_ARROW_LINE_WIDTH = 5;

// Equalization modes defined in NITFDBServer.idl
#define NITF_EQ_NONE EQUALIZATION_MODE_FIXED_LINEAR
#define NITF_EQ_HIST EQUALIZATION_MODE_HISTOGRAM
#define NITF_EQ_AUTO EQUALIZATION_MODE_AUTO_LINEAR
#define NITF_EQ_MANUAL EQUALIZATION_MODE_MANUAL_LINEAR

static const struct ImageFrameLineStyle
{
   FLOAT fMinPixelSize;    // In meters
   INT   iLineStyle;
} ImageFrameLineStyles[] =
{
   { 5.1F, UTIL_LINE_SOLID },    // Above 5 meters
   { 1.1F, UTIL_LINE_DASH },     // Above 1 meter but below 5
   { FLT_MIN, UTIL_LINE_DOT }    // Below 1 meter
};


class ImageLibCallbackInterface : public CImageLibCallbackBase< CComSingleThreadModel >
{
public:

   ImageLibCallbackInterface() : m_dProgressPercent( 0.0 ){}

   STDMETHOD(raw_imagelib_progress)( DOUBLE dPercent, BSTR bsLabel );

   // Worker thread support
   enum
   {
      CALLBACK_MODE_NORMAL,      // Normal UI display
      CALLBACK_MODE_BACKGROUND,  // Background mode busy
      CALLBACK_MODE_ABORTING,    // Background mode abort
   }        m_eCallbackMode;
   DOUBLE   m_dProgressPercent;  // From last callback
}; // End of class ImageLibCallbackInterface

class C_nitf_obj;

class CJPIPCallback : public CImageLibCallbackBase< CComMultiThreadModel >
{
public:
   DECLARE_GET_CONTROLLING_UNKNOWN()

   STDMETHOD( raw_ImageLibCallbackSpecialFunction )(
      /*[in]*/ EnumImageLibCallbackSpecFuncCode eFunctionCode,
      /*[in]*/ VARIANT varInParam1,
      /*[in]*/ VARIANT varInParam2,
      /*[out]*/ VARIANT* pvarOutParam );

   C_nitf_obj* m_pNITFObj;

}; // End of CJPIPCallback


// ************************************************************************
// ************************************************************************
//
// C_nitf_obj
//
// ************************************************************************
// ************************************************************************

class C_nitf_obj: public C_icon
{
   friend class NITFObjPtr;

public:
   union
   {
      struct
      {
         d_geo_t m_bnd_ul;
         d_geo_t m_bnd_ur;
         d_geo_t m_bnd_lr;
         d_geo_t m_bnd_ll;
      };
      d_geo_t m_bnds[ 4 ];
   };
   d_geo_t m_geo_rect_ll;
   d_geo_t m_geo_rect_ur;
   CString m_file_title;
   CString m_img_title;
   CString m_img_date;
   CString m_help_text;
   CString m_label;
   BOOL m_use_icon;
   BOOL m_image_loaded;
   IImageLibPtr   m_pImageLib;
   EnumSourceLocationType  m_eSourceLocationType;  // See NITFDBServer.idl
   CString m_icon_text;
   int m_width;
   int m_height;
   NITFFileTypeEnum m_eFileType;
   ULONG m_mTempDisplayMap;
   CRect m_rect;
   BOOL m_bSelected;
   BOOL m_has_geo;
   EnumFillControl m_eFillControl;
   BOOL m_translucent;
   BOOL m_is_color;
   BOOL m_is_multispectral;
   int m_eq_mode;
   int m_trans_value;
   CString  m_filename;
   CString  m_csFilespec;
   enum FileAccessStatusEnum { FILE_ACCESS_NOT_CHECKED, FILE_ACCESS_OK, FILE_ACCESS_NOT_ACCESSIBLE }
   m_eFileAccessStatus;
   INT      m_iImageInFile;
   CString  m_csVolumeLabel;
   POINT m_pt[4];
#if 0
   POINT m_old_pt[4];
#endif
   BOOL m_bSelectHit;
   double m_lat_per_pix;
   double m_pixel_size_in_meters;
   CString m_raw_date;
   CString m_info;
   int m_contr_minval;
   int m_contr_ctrval;
   int m_contr_maxval;
   int m_priority;
   XML2DocumentPtr m_pxdDisplayParams;
   DWORD m_dwBeginFlashTicks;
   BYTE m_stretch_map[256];
   INT m_cDisplayImagePixels;
   BOOL m_bIncompleteDraw;
   BOOL m_bShowSecondaryImages;
   BOOL m_map_is_rotated;
   std::auto_ptr< BYTE > m_apbRGBBuffer;
   PBYTE m_red_buffer, m_grn_buffer, m_blu_buffer;

   std::auto_ptr< BYTE > m_apbOldImg;
   double m_old_ul_lat;
   double m_old_ul_lon;
   double m_old_lr_lat;
   double m_old_lr_lon;
   int m_old_width;
   int m_old_height;

   unsigned int *m_freq_lum;

   CComObject<ImageLibCallbackInterface> m_cbImageLibCallback1;   // For main thread
   CComObject<ImageLibCallbackInterface> m_cbImageLibCallback2;   // For overview build thread
   CComObject<CJPIPCallback>           m_cbJPIPCallback;          // For JPIP images

   BOOL                                m_bTileCacheServiceInitialized;
   CNITFServiceNTLMAuth                m_snaoNTLMAuthorization;
   CNITFServiceBasicAuth               m_sbaoBasicAuthorization;
   BOOL                                m_bBasicAuthorizationUsed;
   TileCacheService::CTileCacheService m_tcsTileCacheService;
   _bstr_t                             m_bstrTileCacheSubserviceName;
   _bstr_t                             m_bstrSubserviceUserName;
   _bstr_t                             m_bstrSubservicePassword;
   _bstr_t                             m_bstrTileCacheImageIdentifier;

public:
   // Constructor
   C_nitf_obj( C_overlay* pNITFOverlay ); 

   ~C_nitf_obj(); 

   int draw(ActiveMap* map);
   int draw_label(ActiveMap* map);
   void invalidate(BOOL erase = FALSE);
   CString get_help_text();
   CString get_tool_tip();
   const char *get_class_name() { return "C_nitf_obj"; }
   boolean_t is_kind_of(const char* class_name);
   BOOL hit_test( CPoint ptTest );
   void draw_bounds(ActiveMap* map);
   INT get_map_image( const CString& csFilename, INT iWidth, INT iHeight, 
      DOUBLE ul_lat, DOUBLE ul_lon, DOUBLE lr_lat, DOUBLE lr_lon, 
      PBYTE pbImgData, PINT piErr, CString& csErrorMsg );
   INT get_rotated_map_image( const CString& csFilename, 
      INT ulx, INT uly, INT urx, INT ury, 
      INT lrx, INT lry, INT llx, INT lly, 
      DOUBLE ul_lat, DOUBLE ul_lon, DOUBLE ur_lat, DOUBLE ur_lon, 
      DOUBLE lr_lat, DOUBLE lr_lon, DOUBLE ll_lat, DOUBLE ll_lon, 
      INT cTiepoints, DOUBLE* pdTiepointsX, DOUBLE* pdTiepointsY,
      DOUBLE* pdTiepointsLat, DOUBLE* pdTiepointsLon,
      INT& iWidth, INT& iHeight, 
      PBYTE& pbImageData, INT& iErr, CString& csErrorMsg );

   int draw_map_image(ActiveMap* map);
   int draw_map_image_rotated( ActiveMap* map, INT& iErr );
   void make_translucent(int width, int height, BYTE *img);  // img is assumed to consist of RGB triples

   BOOL get_nitf_info(CString & info);

   // Find the geo and corresponding screen quadrangle that contains a possibly rotated image
   int find_bounding_quadrangle(
      ActiveMap* map,                        // IN
      INT cIntersectionVertices,             // IN
      const POINT* pptIntersectionVertices,  // IN
      POINT* imgpt,                          // OUT
      double *ilat, double *ilon );          // OUT

   int find_intersection( POINT *src_pt, // IN
      int src_pt_cnt,  // IN
      POINT *clip_pt,    // IN
      int clip_pt_cnt );   // IN

   int load_image_file(); 
   int convert_date(CString raw_date, CString &date); 
   void draw_nitf_icon( ActiveMap* pMap, CDC* pDC, int x, int y, boolean_t hilighted);
   void draw_missing_icon(CDC *dc, int x, int y);
   void auto_contrast();
   int get_imagelib_histogram(CString & error_msg);
   void stretch_contrast();
   void auto_stretch_contrast();
   int update_map_image_buffer(int width, int height, double ul_lat, double ul_lon, double lr_lat, double lr_lon, 
      INT& iErr, CString& csErrorMsg );
   int update_rotated_map_image_buffer(  int ulx, int uly, int urx, int ury, 
      int lrx, int lry, int llx, int lly, 
      double ul_lat, double ul_lon, double ur_lat, double ur_lon, 
      double lr_lat, double lr_lon, double ll_lat, double ll_lon, 
      INT cTiepoints, DOUBLE* pdTiepointsX, DOUBLE* pdTiepointsY,
      DOUBLE* pdTiepointsLat, DOUBLE* pdTiepointsLon,
      INT& iWidth, INT& iHeight, INT& iErr, CString& csErrorMsg );
   BOOL  m_bDisplayOverlays;

   BOOL show_fill();
   BOOL IsBusy();

   VOID  set_display_mode();
   VOID  set_display_params();
   int get_histogram(CString & error_msg);  // get luminance histogram i.e. m_freq_lum
   VOID Close();


private:
   // Methods
   VOID     SetRGBBuffers( PBYTE pbRGBData, INT cPixels );
   VOID     InitImageLib();
   HRESULT  ImageLibProxyLoad( PINT piErr, BSTR* pbsErrMsg );
   VOID     DrawLocatorArrow( ActiveMap* pMap, INT cWidth, INT cHeight,
      DOUBLE dAngle, INT iColor );

   // Worker thread support
   INT      StartBackgroundBuildThread();
   static UINT __stdcall BackgroundBuildProc1( LPVOID pvThis );   // Used by AfxBeginThread()
   UINT     BackgroundBuildProc2();
   HANDLE   m_hBackgroundBuildThread;
   BOOL     m_bBackgroundBuildThreadFailed;

   // Data
   INT      m_cRefCount;
   ActiveMap*
      m_pActiveMap;
   _bstr_t  m_bstrBackgroundBuildParam;
   enum ImageLibLoadStatus
   {
      IMAGELIB_NOT_LOADED,
      IMAGELIB_LOAD_ERROR,
      IMAGELIB_LOAD_DELAYED,     // JPIP server busy
      IMAGELIB_LOADED_FILE,
      IMAGELIB_LOADED_TILE_CACHE
   }        m_eImageLibLoadStatus;
   auto_ptr< POINT >
      m_apptViewIntersectionVertices;
   INT      m_cViewIntersectionVertices;

};
// End of C_nitf_obj


// ************************************************************************
// ************************************************************************
//
// NITFObjPtr
//
// Smart pointer to handle clean up of NITF objects when referenced in two
// different collections
//
// ************************************************************************
// ************************************************************************

class NITFObjPtr
{
private:
   C_nitf_obj* m_pObj;

public:
   NITFObjPtr() : m_pObj( NULL ){}
   NITFObjPtr( C_nitf_obj* pObj ) : m_pObj( pObj )
   {
      ATLASSERT( pObj != NULL );
      pObj->m_cRefCount++;
   }
   NITFObjPtr( const NITFObjPtr& nop )
   {
      if ( NULL != ( m_pObj = nop.m_pObj ) )
         m_pObj->m_cRefCount++;
   }
   NITFObjPtr& operator=( const NITFObjPtr& nop )
   {
      if ( m_pObj != NULL )
         if ( --( m_pObj->m_cRefCount ) == 0 )
            delete m_pObj;
      if ( NULL != ( m_pObj = nop.m_pObj ) )
         m_pObj->m_cRefCount++;
      return *this;
   }
   NITFObjPtr& operator=( C_nitf_obj* pObj )
   {
      if ( m_pObj != NULL )
         if ( --( m_pObj->m_cRefCount ) == 0 )
            delete m_pObj;
      if ( NULL != ( m_pObj = pObj ) )
         m_pObj->m_cRefCount++;
      return *this;
   }
   C_nitf_obj* operator->() const
   {
      return m_pObj;
   }
   C_nitf_obj& operator*() const
   {
      return *m_pObj;
   }
   operator C_nitf_obj*() const
   {
      return m_pObj;
   }
   BOOL operator==( const NITFObjPtr& nop ) const
   {
      return m_pObj == nop.m_pObj;
   }
   ~NITFObjPtr()
   {
      if ( m_pObj != NULL )
      {
         if ( --( m_pObj->m_cRefCount ) == 0 )
            delete m_pObj;
      }
   }
}; // class NITFObjPtr



// ************************************************************************
// ************************************************************************
//
// GeoBoundsPtr
//
// ************************************************************************
// ************************************************************************

class GeoBoundsPtr : public C_icon
{
private:
   class GeoBoundsPtrInfo
   {
   public:
      GeoBounds*  pGeoBounds;
      INT         cRefCount;
      GeoBoundsPtrInfo()
      {
         pGeoBounds = NULL;
         cRefCount = 1;
      }
      ~GeoBoundsPtrInfo()
      {
         if ( pGeoBounds != NULL )
            delete pGeoBounds;
      }
   };


   GeoBoundsPtrInfo* pGeoBoundsPtrInfo;

public:
   GeoBoundsPtr()
   {
      pGeoBoundsPtrInfo = NULL;
   }
   GeoBoundsPtr( const GeoBoundsPtr& gbp )
   {
      if ( ( pGeoBoundsPtrInfo = gbp.pGeoBoundsPtrInfo ) != NULL )
         pGeoBoundsPtrInfo->cRefCount++;
   }
   GeoBoundsPtr( GeoBounds* pgb )
   {
      pGeoBoundsPtrInfo = new GeoBoundsPtrInfo;
      pGeoBoundsPtrInfo->pGeoBounds = pgb;
   }
   ~GeoBoundsPtr()
   {
      if ( pGeoBoundsPtrInfo != NULL )
      {
         if ( --pGeoBoundsPtrInfo->cRefCount <= 0 )
            delete pGeoBoundsPtrInfo;
      }
   }
   GeoBounds& GetGeoBounds()
   {
      ASSERT( pGeoBoundsPtrInfo != NULL );
      ASSERT( pGeoBoundsPtrInfo->pGeoBounds != NULL );
      return *pGeoBoundsPtrInfo->pGeoBounds;
   }
   LPCSTR get_class_name() { return "GeoBoundsPtr"; }
   CString get_help_text() { return ""; }
   CString get_tool_tip() { return ""; }
   BOOL hit_test( CPoint pt ) { return FALSE; }

}; // End of GeoBoundsPtr class



// ************************************************************************
// ************************************************************************
//
// CNITFFlashTimer
//
// ************************************************************************
// ************************************************************************

class CNITFFlashTimer : public FVW_Timer
{
public:
   CNITFFlashTimer() : FVW_Timer( SELECT_FLASH_TICKS ){}

protected:
   virtual VOID expired()
   {
      OVL_get_overlay_manager()->invalidate_all( FALSE );   // Refresh to erase flash
   }
}; // End of CNITFFlashTimer class


// ************************************************************************
// ************************************************************************
//
// CNITFJPIPProbeHost
//
// ************************************************************************
// ************************************************************************

class CNITFJPIPProbeHost : public NITFImagery::CRemoteServerFileProbeHost
{
public:
   // Methods
   CNITFJPIPProbeHost( C_nitf_ovl* pNITFOvl );
   VOID ThreadClear();

   // Data
   INITFDBAgentPtr   m_spNITFDBAgent;

protected:
   virtual HRESULT QuerySQL( const std::wostringstream& woss, _Recordset** pprs );

private:
   CNITFJPIPProbeHost(){}   // Not callable
   const C_nitf_ovl* m_pNITFOvl;

}; // class CNITFJPIPProbeHost


// ************************************************************************
// ************************************************************************
//
// C_nitf_ovl
//
// ************************************************************************
// ************************************************************************

class C_nitf_ovl : public FvOverlayImpl
{
   friend class C_nitf_obj;
   friend class CNITFTBClass;
   friend class CNITF_QueryDlg;

public:
   d_geo_t m_scrn_ll;
   d_geo_t m_scrn_ur;
   d_geo_t m_last_ll;
   d_geo_t m_last_ur;

   typedef CList< NITFObjPtr, NITFObjPtr > NITFObjList;
   NITFObjList m_objlist;

   static C_nitf_ovl*      s_pNITFOvl;
   static CNITF_QueryDlg   s_QueryDlg;

#ifdef WARP_TEST
   static CWARP_QueryDlg m_WarpQueryDlg;
#endif

   static INITFDBAgentPtr  s_pNITFDBAgent;
   static enum ::EnumImageDBServerType
      s_eDBServerType;

   // Overlay Modes
   typedef enum { EDIT_MODE_SELECT, 
      EDIT_MODE_AREA} EditModeEnum;
   static EditModeEnum  s_eEditMode;
   static void SetEditMode( EditModeEnum eMode ) { s_eEditMode = eMode; };
   static EditModeEnum GetEditMode() { return s_eEditMode; };
   static VOID InitRegistry( BOOL bUnregister );

   static POINT   s_ptMenuPoint;
   static BOOL    s_bUserAbort;
   static BOOL    s_bEditOn;
   static BOOL    s_bParamsUpdate;
   BOOL           m_bDrawLabels;
   BOOL           m_bErrorReported;
   BOOL           m_bDrawInProgress;
   BOOL           m_bInvalidateRequested;
   BOOL           m_bShowBounds;
   BOOL           m_bHideDuringScroll;
   BOOL           m_bShowSecondaryImages;
   enum
   {
      NEW_EXPLORER_IMAGE_IDLE,
      NEW_EXPLORER_IMAGE_DISPLAY_ONLY,
      NEW_EXPLORER_IMAGE_CENTER_MAP_ALWAYS,
      NEW_EXPLORER_IMAGE_CENTER_MAP_ASK,
      NEW_EXPLORER_IMAGE_CENTER_MAP_LAST,
      NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ALWAYS,
      NEW_EXPLORER_IMAGE_NEW_DISPLAY_TAB_ASK,
   }              m_eNewExplorerInitialImageDisplay,
   m_eNewExplorerCurrentImageDisplay;
      enum
      {
         IMAGES_ONLY_NO_EDITOR_UNKNOWN,
         IMAGES_ONLY_NO_EDITOR_FALSE,
         IMAGES_ONLY_NO_EDITOR_PRETRUE,
         IMAGES_ONLY_NO_EDITOR_TRUE
      }              m_eImagesOnlyNoEditor;
      enum
      {
         QUERY_FILTER_DISPLAY_UNSPECIFIED,
         QUERY_FILTER_DISPLAY_FALSE,
         QUERY_FILTER_DISPLAY_TRUE
      }              m_eQueryFilterDisplay;
      int            m_scrn_width;
      int            m_scrn_height;
      static CIconImage*
         s_piiNITFSystemIcon;
      CIconImage*    m_nitf_icon;               // Color image icons
      CIconImage*    m_tiff_icon;
      CIconImage*    m_sid_icon;
      CIconImage*    m_pdf_icon;
      CIconImage*    m_j2k_icon;
      CIconImage*    m_misc_icon;
      CIconImage*    m_nitf_m_icon;             // Monochrome image icons
      CIconImage*    m_tiff_m_icon;
      CIconImage*    m_sid_m_icon;
      CIconImage*    m_pdf_m_icon;
      CIconImage*    m_j2k_m_icon;
      CIconImage*    m_misc_m_icon;
      BOOL           m_bReload;                 // Load new image list
      BOOL           m_bNeedNewTempDB;
      BOOL           m_bImageSourceBusy;        // Some JPIP server busy
      BOOL           m_bIncompleteDraw;         // Some image drawn with incomplete data
      DWORD          m_dwLastDrawTicks;         // For timed redraw
      INT            m_iRedrawDelayTicks;       //
      DWORD          m_dwMainThreadID;          // For PostThreadMessage

      CComObject<ImageLibCallbackInterface> m_imagelib_callback;

      //CString m_filename;  // full path of the shape file excluding the extention
      static MapScale m_smallest_scale;
      MapScale       m_smallest_labels_scale;
      BOOL           m_bSelectedObj;
      CString        m_name;

      typedef std::list< GeoBoundsPtr > GeoBoundsPtrList;
      typedef GeoBoundsPtrList::iterator GeoBoundsPtrIter;
      GeoBoundsPtrList  m_agbpQueryRegions;
      d_geo_t           m_gptViewCorners[4]; // UL, UR, LR, LL
      typedef std::map< CString, NITFFileTypeEnum, CStringILess > FileTypeLookup;
      typedef FileTypeLookup::iterator FileTypeLookupIter;
      FileTypeLookup    m_mpFileTypeLookup;

public:                         

   // Constructor
   C_nitf_ovl();
   C_nitf_ovl( CString csOverlayName );
   virtual void Finalize();
   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   // Public member functions
   void              clear_ovl();
   void              init_vars();

   virtual const CString&
      get_specification() const { return s_csOverlaySpecification; }

   int               draw(ActiveMap* map);

   int               open( const CString& csFilespec );

   static INT        open_nitf_database();
   static VOID       close_nitf_database();
   static wstring&   QuoteDBString( wstring& wstrInOut );
   int             select_nitf_records( ActiveMap& active_map );
   int             make_topmost_or_bottommost( const C_nitf_obj* const pObj, BOOL bTopmost );
   int               make_topmost_or_bottommost( const bstr_t& bstrFilespec, INT iImageInFile, BOOL bTopmost );
   int             set_file_display_params( const CString& filespec, INT iImageInFile,
      int eq_mode, int opacity, 
      int min_level, int mid_level, int max_level,
      const _bstr_t& bstrDisplayParamsXML );
   int             set_file_fill_control( const CString& filespec, INT iImageInFile, EnumFillControl eFillControl );

   virtual INT       selected( IFvMapView* pMapView, CPoint pt, UINT uiFlags );
   virtual INT       on_mouse_moved( IFvMapView* pMapView, CPoint pt, UINT uiFlags );
   virtual INT       on_left_mouse_button_up( IFvMapView* pMapView, CPoint point, UINT nFlags );
   virtual VOID      drag( ViewMapProj* pMap, CPoint pt, UINT uiFlags,
      HCURSOR* phCursor, HintText** ppHint );
#ifdef JPIP_SUPPORT
   virtual boolean_t can_drop_data( COleDataObject* pDataObject, CPoint& pt );
#endif
   virtual boolean_t paste_OLE_data_object( ViewMapProj* pMapProj, CPoint* pPt, 
      COleDataObject* pDataObject );
   virtual boolean_t is_paste_from_clipboard_allowed() { return TRUE; }
   virtual void      paste_from_clipboard();
   virtual VOID      cancel_drag( ViewMapProj* pMap );
   virtual VOID      drop( ViewMapProj* pMap, CPoint pt, UINT uiFlags );
   int               show_info( C_icon *icon);
   void              menu( ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*>& list );
   void              MenuEx( MapProj* map_proj, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list,
      C_nitf_ovl *nitf_ovl, BOOL bMultiHit, BOOL bMultiSel );
   LRESULT           OnNITFUtilityThreadEvent( WPARAM wParam, LPARAM lParam );

   // Right-click handlers
   static VOID       ResetAllImageDisplayAdjustments();

private:
   static VOID       ResetAllPerImageDisplayAdjustments( CString strMenuItemName, LPARAM )
   { ResetImageDisplayAdjustments( NULL, NULL ); }
   static void       point_info(ViewMapProj* pMap, C_icon* pObj );
   static void       set_show_image(ViewMapProj* pMap, C_icon* pObj );
   static void       set_hide_image(ViewMapProj* pMap, C_icon* pObj );
   static void       toggle_translucent(ViewMapProj* pMap, C_icon* pObj );
   static void       go_to_native_scale(ViewMapProj* pMap, C_icon* pObj );
   static VOID       ResetImageDisplayAdjustments( ViewMapProj* pMap, C_icon* pObj );
   static VOID       ToggleImagesOnlyNoEditor( CString strMenuItemName );
   static VOID       ToggleQueryFilterDisplay( CString strMenuItemName );
   static void       hide_all_images( CString strMenuItemName, LPARAM );
   static void       view_image(ViewMapProj* pMap, C_icon* pObj );
   static void       send_to_back(ViewMapProj* pMap, C_icon* pObj );
   static void       bring_to_front(ViewMapProj* pMap, C_icon* pObj );

   VOID              RotateImageStack( CPoint ptTest, BOOL bRaiseStack );
   static VOID       RemoveQueryRegion( ViewMapProj* pMap, C_icon* pObj, LPARAM lParam );
   static VOID CALLBACK
      RedrawTimerProc( HWND hWnd, UINT uiMsg, UINT_PTR idEvent, DWORD dwTime );

public:
   static int       set_edit_on( boolean_t edit_on );
   virtual int       pre_close( boolean_t* pbCancel );

   VOID              UpdateQueryToolToolbar();

   void              clear_selects();
   void              release_focus();
   void              get_label( int recnum, CString& label );
   const CString &   get_name() const { return m_name; }

   static MapScale   get_smallest_scale(); 
   static void       set_smallest_scale( MapScale scale );
   MapScale          get_smallest_labels_scale();
   void              set_smallest_labels_scale( MapScale scale ); 

   // Determine if the point hits the object's selection region(s)
   C_icon* hit_test( map_projection_utils::CMapProjWrapper* pMap, CPoint ptTest );
   BOOL              image_hit_test( int mx, int my );

   LPCSTR            get_class_name() const { return "C_nitf_ovl"; }

   static LPCSTR     get_default_extension_static()
   { return "ntf"; }
   virtual LPCSTR    get_default_extension()
   { return get_default_extension_static(); }

   BOOL              AddExplorerFile( LPCWSTR pwszFilename, BOOL& bNeedNewTempDB );
   VOID              DropTempDB();
   BOOL              GetNITFDBInfoPage( const C_nitf_obj * const pObj, CString& csInfo );
   VOID              SetTempDisplayMask( ULONG mTempDisplayMask )
   { m_mTempDisplayMask = mTempDisplayMask; }

   // Toolbar messages
   VOID              OnToolbarCmd( UINT nID );
   VOID              OnToolbarUpdateCmdUI( CCmdUI* pCmdUI );

protected:
   // Methods
   virtual boolean_t       offer_keydown( IFvMapView *pMapView, UINT nChar, UINT nRepCnt,
      UINT nFlags, CPoint point );
   virtual boolean_t       offer_mousewheel( IFvMapView *pMapView, SHORT zDelta,
      UINT nFlags, CPoint point );

   // Data
   CNITFFlashTimer         m_clsFlashTimer;

private:
   // Methods
   HRESULT                 QuerySQLStream( const wostringstream& wssSQLCmd, _Recordset*& pRecordset );
   HRESULT                 ExecuteSQLStream( const wostringstream& wssSQLCmd, PLONG plRecordsAffected = NULL );
   BOOL                    GetDefaultableDisplayAdjustments();
   BOOL                    CanDisplayAdjustmentsBeDefaulted( const C_nitf_obj* const pObj );
   VOID                    FilterDfltableDsplyAdjs( LPCWSTR pwszFilter );
   VOID                    AddExplorerFileEnd( BOOL bCheckThread );
   BOOL                    PostAddExplorerFile( LPCWSTR pwszFilespec, BOOL bRemovableMedia,
      wstring& wstrVolumeLabel );
   BOOL                    CheckUtilityThreadAlive();

   VOID                    SaveViewCorners( MapProj* pMap );
   static UINT __cdecl     UtilityProc( LPVOID pvThis );
   UINT                    UtilityProc();

   // Data
   const static CString    s_csOverlaySpecification;
   _RecordsetPtr           m_sprsDfltableDsplyAdjs;

   COLORREF                m_crQueryRegionFrgdColor;
   COLORREF                m_crFrameBusyFrgdColor;
   COLORREF                m_crFrameMonoImageFrgdColor;
   COLORREF                m_crFrameColorImageFrgdColor;
   COLORREF                m_crFrameStdBkgdColor;
   COLORREF                m_crFrameTempBkgdColor;
   COLORREF                m_crFrameTempKeepBkgdColor;
   COLORREF                m_crFrameSelectedBkgdColor;

   INT                     m_iUnselectedBkgdFrameWidth;
   INT                     m_iSelectedBkgdFrameWidth;

   ULONG                   m_mTempDisplayMask;  // See QueryDlgTempDisplayEnum for bits

   INT                     m_iInfoFrameId;      // For GetNITFDBInfo()
   UINT_PTR                m_uiRedrawTimerId;

   ULONG                   m_mQueryToolbarStates[ QUERY_DLG_TOOLBAR_MAX_BUTTON - QUERY_DLG_TOOLBAR_MIN_BUTTON + 1 ];
   CString                 m_csMessageStrings[ QUERY_DLG_TOOLBAR_MAX_BUTTON - QUERY_DLG_TOOLBAR_MIN_BUTTON + 1 ];

   CString                 m_csFVWUserDataRootFolder;
   BOOL                    m_bDragging;
   GeoBounds               m_gbDragQueryRegion;
   GeoBoundsPtrIter        m_itDragRegion;
   enum { DRAG_MODE_NONE, DRAG_MODE_NEW, DRAG_MODE_DRAGGER }
   m_eDragMode;
   CPoint                  m_ptDragOrigin;
   d_geo_t                 m_gptDragOrigin;

   _bstr_t                 m_bstrLastExplorerFilespec;

   CRITICAL_SECTION        m_csUtilityProcAccess;
   typedef std::list< _bstr_t >
      WorkerThreadData;
   WorkerThreadData        m_wtdUtilityControl;    // XML strings to control background analysis
   BOOL                    m_bMultipleExplorerFiles;
   BOOL                    m_bUtilityThreadActive;
   CWinThread*             m_pUtilityThread;
   IStream*                m_pNITFDBAgentIStream;
   CNITFJPIPProbeHost      m_fphJPIPProbeHost;

   HintText                m_htHintText;

   // Variables for dragging
   GeoBoundsPtr            m_gbpScratch;
   GeoBoundsDragger        m_gbdRegionDragger;

protected:
   // Protected destructor. Call Release instead
   ~C_nitf_ovl();

}; // end of C_nitf_ovl class declaration 

// ************************************************************************
// ************************************************************************

// End of nitf.h
