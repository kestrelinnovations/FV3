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



// VerticalViewDisplay.h : interface of the CVerticalViewDisplay class
//
///////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "GenericProjector.h"
#include "map.h"  // until GetSurfaceSize is moved into .cpp
#include "mapview.h"

#define GRID_TEXT_WIDTH 40	// pixel width allowance for altitude text
#define ALTITUDE_TEXT_SIZE 3  // size of altitude text
#define R_VALUE 0  // red value of text color
#define G_VALUE 0  // green value of text color
#define B_VALUE 0  // blue value of text color
#define MINIMUM_VD_WINDOW_HEIGHT 100	// minimum allowable height of the Vertical Display window
#define MAX_NUM_GRID_LINES 8		// maximum allowable number of grid lines in the Vertical Display


//
// forward declarations
//
class CMainFrame;
class CVerticalViewProperties;
class CVerticalViewPropertiesDialog;
class ViewMapProj;
class ViewMapProjImpl;
class CFvVerticalViewImpl;


class CVerticalViewDisplay : public CBaseView
{
protected: // create from serialization only
	CVerticalViewDisplay();
	DECLARE_DYNCREATE(CVerticalViewDisplay)

// Attributes
public:

// Operations
public:

   FalconViewOverlayLib::IFvVerticalView *CVerticalViewDisplay::GetFvVerticalView();

// Implementation
public:
	virtual ~CVerticalViewDisplay();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	void RedrawVerticalDisplay();
   void DrawFromScratch(CDC* memoryDC);

// Generated message map functions
protected:
	//{{AFX_MSG(CColorView)
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVerticalDisplayProperties();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void DrawAltitudeGrid(CDC *pDC);
	void DrawOverlays(CDC *pDC);

   CComObject<CFvVerticalViewImpl> *m_pFvVerticalView;
	CMainFrame* m_pFvwMainFrame;  // pointer to the Fvw mainframe object
	CVerticalViewProperties* m_pVerticalDisplayProperties;  // pointer to the Vertical Display Properties object

   CBitmap* m_pBackingStore;
   CRect    m_Extents;
   CPoint m_tool_tip_point;

	int m_nCurrentVerticalDisplayWindowHeight;
	
public:	
	CVerticalViewPropertiesDialog* m_pVerticalDisplayPropertiesDialog;  // pointer to the Vertical Display Properties Dialog object
};



/////////////////////////////////////////////////////////////////////////////////////////////////////
// CVerticalViewProjector - this class is responsible for converting a latitude, longitude, and 
// altitude to the appropriate screen coordinates in the Vertical Display.
//
class CVerticalViewProjector : public CGenericProjector
{
public:
	ViewMapProj *m_pCurrentMap;
	double m_dMinAltitudeMeters, m_dMaxAltitudeMeters;
	int m_nVerticalDisplayHeightPixels;

	CVerticalViewProjector(ViewMapProj *pCurrentMap, double dMinAltitudeMeters, double dMaxAltitudeMeters,
						   int nVerticalDisplayHeightPixels);

	virtual bool InSurface(double dLat, double dLon, double dAltitudeMeters);
	virtual int ToSurface(double dLat, double dLon, double dAltitudeMeters, int& rX, int& rY);
   virtual double ActualRotation() { return 0.0; }
   int GetSurfaceSize(int* pWidth, int* pHeight)
   {
      m_pCurrentMap->get_surface_size(pWidth, pHeight);
      *pHeight = m_nVerticalDisplayHeightPixels;      
      return SUCCESS;
   }
};


// Implements IFvVerticalViewProjector defined in FalconViewOverlay.tlb
//
class CFvVerticalViewProjectorImpl :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<FalconViewOverlayLib::IFvVerticalViewProjector, &FalconViewOverlayLib::IID_IFvVerticalViewProjector, &FalconViewOverlayLib::LIBID_FalconViewOverlayLib>
{
   CVerticalViewProjector *m_pVerticalViewProj;

public:
   CFvVerticalViewProjectorImpl() : m_pVerticalViewProj(NULL)
   {
   }
   ~CFvVerticalViewProjectorImpl();

BEGIN_COM_MAP(CFvVerticalViewProjectorImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(FalconViewOverlayLib::IFvVerticalViewProjector)
END_COM_MAP()

   void Initialize(ViewMapProj *pCurrentMap, double dMinAltitudeMeters, double dMaxAltitudeMeters,
						   int nVerticalDisplayHeightPixels);

// IFvVerticalViewProjector

   STDMETHOD(raw_GeoInSurface)(double dLat, double dLon, double dAltitudeMeters, long *pbInSurface);
   STDMETHOD(raw_GeoToSurface)(double dLat, double dLon, double dAltitudeMeters, long *x, long *y, long *pResult);
   STDMETHOD(raw_GetSurfaceSize)(long* pWidth, long* pHeight);
};