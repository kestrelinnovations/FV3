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

// OverlayCOM.h
//

#pragma once

#include "FalconView/include/overlay.h"
#include "Common\ComErrorObject.h"
#include "Common\ScopedCriticalSection.h"

class CFvOverlayEventSink;
class TabularEditorDataObject;
class CFvTabularEditorImpl;


class COverlayCOM :
   public C_overlay,
   public OverlayElement_Interface,
   public OverlayElementEnumerator_Interface,
   public PlaybackEventsObserver_Interface,
   public PlaybackTimeSegment_Interface,
   public OverlayTypeOverrides_Interface
{
protected:
   std::string m_displayName;
   CLSID m_clsid;

   CComObject<CFvOverlayEventSink> *m_pOverlayEventSink;

   FalconViewOverlayLib::IFvOverlayPtr m_spFvOverlay;
   FalconViewOverlayLib::IFvOverlay2Ptr m_spFvOverlay2;
   FalconViewOverlayLib::IFvOverlayTeardownEventsPtr m_spFvOverlayTeardownEvents;
   FalconViewOverlayLib::IFvOverlayRendererPtr m_spFvOverlayRenderer;
   FalconViewOverlayLib::IFvOverlayRenderer3DPtr m_spFvOverlayRenderer3D;
   FalconViewOverlayLib::IFvOverlayBaseMapRendererPtr m_spFvOverlayBaseMapRenderer;
   FalconViewOverlayLib::IFvOverlayVerticalViewRendererPtr m_spFvOverlayVerticalViewRenderer;
   FalconViewOverlayLib::IFvOverlayUIEventsPtr m_spFvOverlayUIEvents;
   FalconViewOverlayLib::IFvOverlayVerticalViewUIEventsPtr m_spFvOverlayVerticalViewUIEvents;
   FalconViewOverlayLib::IFvOverlayUIEventRoutingOverridesPtr m_spFvOverlayUIEventRoutingOverrides;
   FalconViewOverlayLib::IFvOverlayContextMenuPtr m_spFvOverlayContextMenu;
   FalconViewOverlayLib::IFvOverlaySnappablePointsPtr m_spFvOverlaySnappablePoints;
   FalconViewOverlayLib::IFvOverlayCopyPastePtr m_spFvOverlayCopyPaste; 
   FalconViewOverlayLib::IFvOverlayElementPtr m_spFvOverlayElement;
   FalconViewOverlayLib::IFvOverlayElementEnumeratorPtr m_spFvOverlayElementEnumerator;
   FalconViewOverlayLib::IFvPlaybackEventsObserverPtr m_spFvPlaybackEventsObserver;
   FalconViewOverlayLib::IFvPlaybackTimeIntervalPtr m_spFvPlaybackTimeInterval;
   FalconViewOverlayLib::IFvOverlayTypeOverridesPtr m_spFvOverlayTypeOverrides;
   FalconViewOverlayLib::IFvOverlayTypeOverrides2Ptr m_spFvOverlayTypeOverrides2;
   FalconViewOverlayLib::IFvOverlayUndoRedoPtr m_spFvOverlayUndoRedo;
   FalconViewTabularEditorLib::IFvTabularEditablePtr m_spFvTabularEditable;

   CComObject<CFvTabularEditorImpl>* m_pFvTabularEditor;

public:
   COverlayCOM(const char* lpszDisplayName, CLSID clsid);
   COverlayCOM(const DWORD owning_thread_id,
      FalconViewOverlayLib::IFvOverlay *fv_overlay);
   ~COverlayCOM();

   virtual FalconViewOverlayLib::IFvOverlayPtr GetFvOverlay() override
   {
      return m_spFvOverlay;
   }
   
   FalconViewTabularEditorLib::ITabularEditor* GetTabularEditor();

   // C_overlay
   //
   STDMETHOD_(ULONG, AddRef)() override
   {
      fv_common::ScopedCriticalSection lock(&m_critical_section);
      ++m_reference_count;
      return m_reference_count;
   }

   STDMETHOD_(ULONG, Release)() override
   {
      bool delete_now = false;
      // We must eliminate the ScopedCriticalSection before deleting "this".
      {
         fv_common::ScopedCriticalSection lock(&m_critical_section);
         --m_reference_count;
         if (m_reference_count <=0)
         {
            delete_now = true;
         }
      }
      if (delete_now)
      {
         delete this;
         return 0;
      }
      return m_reference_count;
   }

   virtual int draw(ActiveMap* map);

private:
   int m_reference_count;
   CRITICAL_SECTION m_critical_section;

public:
   // IFvOverlay
   //

   // Returns the overlay descriptor guid for this overlay.  
   virtual GUID get_m_overlayDescGuid();

   // True if the overlay is visible, false otherwise.
   virtual HRESULT put_m_bIsOverlayVisible(long bIsOverlayVisible);
   virtual long get_m_bIsOverlayVisible();

   // Initialize the overlay.  The read-only property m_overlayDescGuid should be set by this method
   virtual HRESULT InternalInitialize(GUID overlayDescGuid);
   HRESULT InternalInitializeHelper(GUID overlayDescGuid);

   virtual void QueryForInterfaces();
   void SetEventSink();
   void SendInitialPlaybackNotifications();

   virtual int pre_save(boolean_t *cancel);
   virtual int pre_close(boolean_t *cancel = NULL);

   // IFvOverlay2
   //
   // override C_overlay:SetFrameWnd
   virtual void SetFrameWnd(FalconViewOverlayLib::IFvFrameWnd2* pFrameWnd);

   // IFvOverlayTeardownEvents
   //

   virtual void Finalize();

   // IFvOverlayBaseMapRenderer
   //
   virtual int OnDrawToBaseMap(IActiveMapProj *map);
   virtual BOOL can_add_pixmaps_to_base_map();

   // IFvOverlayRenderer3D
   virtual HRESULT OnDrawDraped(IActiveMapProj* pActiveMap) override;
   virtual BOOL SupportsDrawDraped() override;

   // IFvOverlayVerticalViewRenderer
   //
   virtual int DrawToVerticalDisplay(CDC *pDC, CVerticalViewProjector* pProjector);

   // IFvOverlayVerticalViewUIEvents
   //
   virtual int test_selected_vertical(CVerticalViewProjector* /*proj*/, CPoint /*point*/, UINT /*flags*/, 
      HCURSOR *cursor, HintText **hint);
   virtual int selected_vertical(CVerticalViewProjector* /*pProj*/, CPoint /*point*/, UINT /*flags*/,
      HCURSOR *cursor, HintText **hint);

   // IFvOverlayUIEvents
   //

   // MouseMove occurs when the mouse pointer was moved in the view
   virtual HRESULT MouseMove(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseLeftButtonDown occurs when the left mouse button is pressed while the mouse pointer is over the given view
   virtual HRESULT MouseLeftButtonDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseLeftButtonUp occurs when the left mouse button is released while the mouse point is over the given view
   virtual HRESULT MouseLeftButtonUp(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseRightButtonDown occurs when the right mouse button is pressed while the mouse pointer is over the given view.
   virtual HRESULT MouseRightButtonDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseDoubleClick occurs when the mouse button is double clicked over the given view
   virtual HRESULT MouseDoubleClick(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nFlags, long *pbHandled);

   // MouseWheel occurs when the user rotates the mouse wheel while the mouse pointer is over the given view
   virtual HRESULT MouseWheel(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long zDelta, long nFlags, long *pbHandled);

   // KeyDown occurs when a key is pressed
   virtual HRESULT KeyDown(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled);

   // KeyUp occurs when a key is released
   virtual HRESULT KeyUp(FalconViewOverlayLib::IFvMapView *pMapView, long x, long y, long nChar, long nRepCnt, long nFlags, long *pbHandled);

   // IFvOverlayUIEventRoutingOverrides
   //
   virtual long get_m_bDirectlyRouteMouseMove();
   virtual long get_m_bDirectlyRouteMouseLeftButtonDown();
   virtual long get_m_bDirectlyRouteMouseLeftButtonUp();
   virtual long get_m_bDirectlyRouteKeyDown();
   virtual long get_m_bDirectlyRouteKeyUp();

   // IFvOverlayContextMenu
   //
   virtual HRESULT AppendMenuItems(FalconViewOverlayLib::IFvContextMenu *pContextMenu, FalconViewOverlayLib::IFvMapView *pMapView, long x, long y);

   // IFvOverlaySnappablePoints
   //

   // test to see if this overlay can do a single point snap to at this point
   virtual boolean_t test_snap_to(ViewMapProj* /*map*/, CPoint /*point*/);

   // get the snap to objects for this overlay at this point
   virtual boolean_t do_snap_to(ViewMapProj* /*map*/, CPoint /*point*/, CList<SnapToInfo *, SnapToInfo *> & /*snap_to_list*/);

   // IFvOverlayCopyPaste
   //

   // paste OLE data object(s) onto an overlay. Defaults to not being handled
   virtual boolean_t paste_OLE_data_object( ViewMapProj* /*map*/, CPoint* /*pPt*/, 
      COleDataObject* /*data_object*/ ); 
   // test to see if we can read clipboard data
   virtual boolean_t can_drop_data( COleDataObject* /*data_object*/, CPoint& /*pt*/);

   // return TRUE if data from the current overlay's data can be copied to the "clipboard" (Edit | Copy will be enabled)
   virtual boolean_t is_copy_to_clipboard_allowed(); 

   // copy data from the current overlay to the "clipboard" (handle Edit | Copy)
   virtual void copy_to_clipboard(); 

   // return TRUE if data can be pasted from the "clipboard" to the current overlay (Edit | Paste will be enabled)
   virtual boolean_t is_paste_from_clipboard_allowed(); 

   // paste data from the "clipboard" to the current overlay
   virtual void paste_from_clipboard(); 

   // IFvOverlayUndoRedo
   //
   virtual boolean_t can_undo();
   virtual boolean_t can_redo();
   virtual void undo();
   virtual void redo();

   // IFvOverlayElement
   //

   // registers varDispatch to receive callbacks
   virtual HRESULT RegisterForCallbacks(VARIANT varDispatch);

   // Updates an element of the overlay using the given XML string
   virtual HRESULT UpdateElement(BSTR bstrElementXml);

   // By default an overlay implementing this interface supports IFvOverlayElement.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvOverlayElement or not.
   virtual BOOL SupportsFvOverlayElement();

   // IFvOverlayElementEnumerator
   //

   // The XML that describes the current element
   virtual BSTR bstrCurrentXml();

   // Resets the enumerator.  After a call to Reset, the current element will be NULL until MoveNext is called
   virtual HRESULT Reset();

   // Moves to the next element in enumeration
   virtual HRESULT MoveNext(long *pbRet);

   // By default an overlay implementing this interface supports IFvOverlayElementEnumerator.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvOverlayElementEnumerator or not.
   virtual BOOL SupportsFvOverlayElementEnumerator();

   // IFvPlaybackEventsObserver
   //

   // Called when a playback is started
   virtual HRESULT OnPlaybackStarted();

   // Called when the playback is stopped
   virtual HRESULT OnPlaybackStopped();

   // Called whenever the playback time changes
   virtual HRESULT OnPlaybackTimeChanged(DATE dateCurrentTime);

   // Called whenever the playback rate changes
   virtual HRESULT OnPlaybackRateChanged(long lNewPlaybackRate);

   // IFvPlaybackTimeSegment
   //

   // Returns the beginning and ending times for the overlay's time segment.  If the time segment is valid 
   // set pbTimeSegmentValue to a non-zero integer, otherwise set it to zero.  An example where the time segment
   // would be invalid is an uncalculated route.  In this case, the overlay is not accounted for in the overall
   // clock time and does not show up in the playback dialog's gannt chart
   virtual HRESULT GetTimeSegment( DATE *pBegin, DATE *pEnd, long *pbTimeSegmentValid);

   // Returns the color that is used by the playback dialog's gannt chart for this overlay
   virtual HRESULT GetGanntChartColor(COLORREF *pColor);

   // By default an overlay implementing this interface supports IFvPlaybackTimeSegment.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvPlaybackTimeSegment or not.
   virtual BOOL SupportsFvPlaybackTimeSegment();

   // IFvOverlayTypeOverrides
   //

   // Override the display name defined in the overlay's factory.  This is used to support
   // per-instance overlay display names (e.g., an overlay created via the ILayer::CreateLayer method)
   virtual HRESULT get_m_displayName(BSTR *pVal);

   // This icon will override the icon defined in the overlay's descriptor.  This ASSERT(m_spFvOverlayPersistence != NULL);
   // is used to support per-instance overlay icons (e.g., an overlay created via the
   // ILayer::CreateLayerEx interface).  This icon is displayed in the overlay manager's
   // current list of opened overlays.  The filename can either be the full specification
   // or a path relative to HD_DATA\icons.
   virtual HRESULT get_m_iconFilename(BSTR* pVal);

   // By default an overlay implementing this interface supports IFvOverlayTypeOverrides.  However, some generic overlays
   // such as COverlayCOM need to dynamically state whether they support IFvOverlayTypeOverrides or not.
   virtual BOOL SupportsFvOverlayTypeOverrides();

   // Override UserControllable will override the attribute userControllable
   // defined in the overlay's descriptor overlay config file.
   virtual HRESULT get_m_UserControllable(long *pbUserControllable);

   // Assuming that initialize has been called, perform some final setup
   // operations
   void PostInitialize();

   // Tabular editor support
   // Returns TRUE if this overlay can be edited by the tabular editor
   boolean_t is_tabular_editable() override;
   TabularEditorDataObject* GetTabularEditorDataObject() override;

private:
   FalconViewOverlayLib::IFvMapViewPtr m_map_view;

   // STA COM operations must be performed on the owning thread.
   DWORD m_owning_thread_id;

   bool m_drawing;
};

class CFileOverlayCOM :
   public COverlayCOM,
   public OverlayPersistence_Interface
{
   FalconViewOverlayLib::IFvOverlayPersistencePtr m_spFvOverlayPersistence;
   FalconViewOverlayLib::IFvOverlayPersistence2Ptr m_spFvOverlayPersistence2;

public:
   CFileOverlayCOM(const char* lpszDisplayName, CLSID clsid);
   CFileOverlayCOM(const DWORD owning_thread_id,
      FalconViewOverlayLib::IFvOverlay *pFvOverlay);

   // Initialize the overlay.  The read-only property m_overlayDescGuid should be set by this method
   virtual HRESULT InternalInitialize(GUID overlayDescGuid);

   virtual void QueryForInterfaces();

   // IFvOverlayPersistence
   //

   // Return the file specification of the overlay
   HRESULT get_m_fileSpecification(BSTR* pFileSpecification);
   HRESULT put_m_fileSpecification(BSTR pFileSpecification);

   // True if the file overlay has been persisted.  False otherwise.
   HRESULT put_m_bHasBeenSaved(long pVal);
   HRESULT get_m_bHasBeenSaved(long* pVal);

   // True if the file overlay is dirty and needs to be saved.  False otherwise.
   HRESULT put_m_bIsDirty(long pVal);
   HRESULT get_m_bIsDirty(long* pVal);

   // True if the file overlay is a read-only file overlay
   HRESULT put_m_bIsReadOnly(long pVal);
   HRESULT get_m_bIsReadOnly(long* pVal);

   // The overlay has been created from scratch.  The m_fileSpecification property should be initialized
   // in this method.
   HRESULT FileNew();

   // Open the overlay with the given file specification
   HRESULT FileOpen(BSTR fileSpecification);

   // Save the overlay to the given file specification and format.  The parameter nSaveFormat corresponds
   // to the selected filter in the Save As dialog.  If the format is unspecified,  then eSaveFileFormat 
   // will be zero (save to the overlay's default file format in this case).
   HRESULT FileSaveAs(BSTR fileSpecification, long nSaveFormat);   

   // IFvOverlayPersistence2
   //
   HRESULT Revert(BSTR fileSpecification) override;
   BOOL SupportsRevertOverride() override;
};

class CDisplayChangeNotifyEventsImpl :
   public FalconViewOverlayLib::IDisplayChangeNotifyEvents
{
protected:
   COverlayCOM *m_pOverlay;

public:
   CDisplayChangeNotifyEventsImpl() : m_pOverlay(NULL)
   {
   }

   void SetOverlay(COverlayCOM *pOverlay);

   STDMETHOD(raw_InvalidateOverlay)();
   STDMETHOD(raw_InvalidateBaseMap)();
   STDMETHOD(raw_InvalidateVerticalDisplay)();
};

class CMapChangeNotifyEventsImpl :
   public FalconViewOverlayLib::IMapChangeNotifyEvents
{
   STDMETHOD(raw_OnRequestMapCenterChange)(double dLat, double dLon);
   STDMETHOD(raw_OnRequestMapRotationChange)(double dRotationDegrees);
   STDMETHOD(raw_OnRequestMapCenterAndRotationChange)(double dLat, double dLon, double dRotationDegrees);
};

// "sink" for overlay events - an overlay will call back to this object to notify the overlay manager of certain events
class ATL_NO_VTABLE CFvOverlayEventSink : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public IPropertyNotifySink,
   public CDisplayChangeNotifyEventsImpl,
   public CMapChangeNotifyEventsImpl

{
   // IPropertyNotifySink
public:
   STDMETHOD(OnChanged)(DISPID dispID);
   STDMETHOD(OnRequestEdit)(DISPID dispID) 
   { 
      return S_OK; 
   }

public:

BEGIN_COM_MAP(CFvOverlayEventSink)
   COM_INTERFACE_ENTRY(IPropertyNotifySink)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IDisplayChangeNotifyEvents)
   COM_INTERFACE_ENTRY(FalconViewOverlayLib::IMapChangeNotifyEvents)
END_COM_MAP()
};

