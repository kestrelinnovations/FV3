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



// VerticalViewDisplay.cpp : implementation of the CVerticalViewDisplay class
//
////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "stdlib.h"  // for log10() and floor() functions
#include "common.h"
#include "mapx.h"
#include "mainfrm.h"
#include "getobjpr.h"
#include "VerticalViewDisplay.h"
#include "VerticalViewProperties.h"
#include "VerticalViewPropertiesDialog.h"
#include "StatusBarInfoPane.h"
#include "FvVerticalViewImpl.h"
#include "fvwutil.h"

bool RedrawFlag = true;


IMPLEMENT_DYNCREATE(CVerticalViewDisplay, CView)

BEGIN_MESSAGE_MAP(CVerticalViewDisplay, CView)
   //{{AFX_MSG_MAP(CVerticalViewDisplay)
   ON_WM_MOUSEACTIVATE()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_ERASEBKGND()
   ON_WM_RBUTTONDOWN()
   ON_WM_DESTROY()
   ON_COMMAND(ID_OPTIONS_VERTICAL_DISPLAY_PROPERTIES, OnVerticalDisplayProperties)
   ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVerticalViewDisplay construction/destruction

CVerticalViewDisplay::CVerticalViewDisplay() : m_pFvVerticalView(NULL)
{
   // initialize object pointers...
   m_pFvwMainFrame = NULL;
   m_pVerticalDisplayProperties = NULL; 
   m_pVerticalDisplayPropertiesDialog = NULL;

   m_pBackingStore = NULL;

   // initialize the Vertical Display window height member variable...
   m_nCurrentVerticalDisplayWindowHeight = PRM_get_registry_int("VerticalDisplay", "LastViewHeight", 100);
}

FalconViewOverlayLib::IFvVerticalView *CVerticalViewDisplay::GetFvVerticalView()
{
   if (m_pFvVerticalView == NULL)
   {
      CComObject<CFvVerticalViewImpl>::CreateInstance(&m_pFvVerticalView);
      m_pFvVerticalView->AddRef();

      m_pFvVerticalView->SetVerticalView(this);
   }

   return m_pFvVerticalView;
}

CVerticalViewDisplay::~CVerticalViewDisplay()
{
   if (m_pBackingStore)
      delete m_pBackingStore;
}

void CVerticalViewDisplay::OnDestroy()
{
   // let the Vertical Display Properties class know that the Vertical Display is now closed...
   // if necessary, get the pointer to the Fvw mainframe...
   if (m_pFvwMainFrame == NULL)
      m_pFvwMainFrame = fvw_get_frame();

   if (m_pFvwMainFrame)
   {
      // get the pointer to the Vertical Display Properties object from the Fvw mainframe...
      m_pVerticalDisplayProperties = m_pFvwMainFrame->GetVerticalDisplayPropertiesObject();

      // let the VD Properties object know that the VD window is closed...
      if (m_pVerticalDisplayProperties)
         m_pVerticalDisplayProperties->SetVerticalDisplayPointer(NULL);

      // get the current height of the VD window (splitter row 1)...
      int curHeight, minHeight;
      m_pFvwMainFrame->m_wndSplitter.GetRowInfo(1, curHeight, minHeight);

      // set the "LastViewHeight" registry key to the current VD window height, making
      // sure that the height is at least 100 pixels...
      if (curHeight < 100)
         curHeight = 100;
      PRM_set_registry_int("VerticalDisplay", "LastViewHeight", curHeight);
   }
   else
      AfxMessageBox("Error getting pointer to the mainframe...");
}




/////////////////////////////////////////////////////////////////////////////////////////////////
// CVerticalViewDisplay drawing
//
void CVerticalViewDisplay::OnDraw(CDC* pDC)
{
   // prepare the window for drawing...
   ASSERT(pDC);

   CRect clientRect;
   GetClientRect(clientRect);

   // Nothing to draw if the map hasn't already been initialized
   MapView* map_view = fvw_get_view();
   if (!map_view || !map_view->GetMapInitialized())
      return;

   // create a memory DC that we will draw to (in order to avoid screen flashing)...
   CDC memoryDC;
   memoryDC.CreateCompatibleDC(pDC);

   // check if we need to create the initial backingstore or re-alloc because of window resize
   if (m_pBackingStore == NULL || m_Extents != clientRect || RedrawFlag)
   {
      if (m_pBackingStore != NULL)
         delete m_pBackingStore;

      m_Extents = clientRect;

      m_pBackingStore = new CBitmap();
      if (m_pBackingStore == NULL)
         // unable to continue
         return;

      // update the new bitmap using the resized window dimensions...
      m_pBackingStore->CreateCompatibleBitmap(pDC, m_Extents.Width(), m_Extents.Height());

      // select the new bitmap into the memory DC and then draw it...
      memoryDC.SelectObject(m_pBackingStore);
      DrawFromScratch(&memoryDC);
   }
   else
   {
      // window wasn't resized, so just select the new bitmap into the memory DC...
      memoryDC.SelectObject(m_pBackingStore);
   }

   // copy the bitmap from the memory DC to the passed-in DC (pDC)...
   pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memoryDC, 0, 0, SRCCOPY);
}


BOOL CVerticalViewDisplay::OnEraseBkgnd(CDC* pDC) 
{
   // return TRUE so that the screen does not get painted black which
   // causes flashing
   return TRUE;
}


void CVerticalViewDisplay::OnSize(UINT nType, int cx, int cy)
{
   CView::OnSize(nType, cx, cy);
}


void CVerticalViewDisplay::DrawAltitudeGrid(CDC *pDC)
{
   int i;

   // if necessary, get the pointer to the Fvw mainframe...
   if (m_pFvwMainFrame == NULL)
      m_pFvwMainFrame = fvw_get_frame();

   // get the pointer to the Vertical Display Properties object from the Fvw mainframe...
   if (m_pFvwMainFrame)
      m_pVerticalDisplayProperties = m_pFvwMainFrame->GetVerticalDisplayPropertiesObject();
   else
      AfxMessageBox("Error getting pointer to the FalconView mainframe object...");

   if (m_pVerticalDisplayProperties)
   {
      // let the Vertical Display Properties class know that the Vertical Display is open...
      m_pVerticalDisplayProperties->SetVerticalDisplayPointer(this);

      CRect winSize;
      GetClientRect(winSize);


      COLOR16 topR, topG, topB;
      COLOR16 bottomR, bottomG, bottomB;
      float dBottomEdgeScaleFactor, dTopEdgeScaleFactor;

      // get the minimum and maximum altitudes that will be displayed in the Vertical Display...
      int nCurrentVerticalDisplayAltitudeMin = m_pVerticalDisplayProperties->m_nBaseAltitudeFeet;
      int nCurrentVerticalDisplayAltitudeMax = m_pVerticalDisplayProperties->m_nBaseAltitudeFeet + m_pVerticalDisplayProperties->m_nAltitudeRangeFeet;

      // use these min/max values to calculate scale factors that will be used to
      // calculate RGB color values at the bottom and top edges of the VD window... 
      dBottomEdgeScaleFactor = (nCurrentVerticalDisplayAltitudeMin - MIN_BASE_MSL)/(float)(MAX_MSL_IN_VERTICAL_DISPLAY - MIN_BASE_MSL); 
      dTopEdgeScaleFactor = (nCurrentVerticalDisplayAltitudeMax - MIN_BASE_MSL)/(float)(MAX_MSL_IN_VERTICAL_DISPLAY - MIN_BASE_MSL); 

      // calculate the RGB color values at the bottom and top edges of the VD window...
      bottomR = 0xff00 - (COLOR16)(dBottomEdgeScaleFactor * (0xff00 - ((WORD)GetRValue(m_pVerticalDisplayProperties->m_GridBackgroundColor)<<8)));
      bottomG = 0xff00 - (COLOR16)(dBottomEdgeScaleFactor * (0xff00 - ((WORD)GetGValue(m_pVerticalDisplayProperties->m_GridBackgroundColor)<<8)));
      bottomB = 0xff00 - (COLOR16)(dBottomEdgeScaleFactor * (0xff00 - ((WORD)GetBValue(m_pVerticalDisplayProperties->m_GridBackgroundColor)<<8)));
      topR = 0xff00 - (COLOR16)(dTopEdgeScaleFactor * (0xff00 - ((WORD)GetRValue(m_pVerticalDisplayProperties->m_GridBackgroundColor)<<8)));
      topG = 0xff00 - (COLOR16)(dTopEdgeScaleFactor * (0xff00 - ((WORD)GetGValue(m_pVerticalDisplayProperties->m_GridBackgroundColor)<<8)));
      topB = 0xff00 - (COLOR16)(dTopEdgeScaleFactor * (0xff00 - ((WORD)GetBValue(m_pVerticalDisplayProperties->m_GridBackgroundColor)<<8)));

      // draw the gradient background for the VD window...
      TRIVERTEX vert[2];
      GRADIENT_RECT gRect;

      vert[0].x     = 0;
      vert[0].y     = 0;
      vert[0].Red   = topR;
      vert[0].Green = topG;
      vert[0].Blue  = topB;
      vert[0].Alpha = 0x0000;

      vert[1].x     = winSize.right;
      vert[1].y     = winSize.bottom; 
      vert[1].Red   = bottomR;
      vert[1].Green = bottomG;
      vert[1].Blue  = bottomB;
      vert[1].Alpha = 0x0000;

      gRect.UpperLeft  = 0;
      gRect.LowerRight = 1;
      GradientFill(*pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);


      // calculate current altitude range as a double...
      double dGridAltitudeRangeFeet = (double)nCurrentVerticalDisplayAltitudeMax - (double)nCurrentVerticalDisplayAltitudeMin;


      ///  Calculate the grid drawing parameters ////////////////////////////////////////////////////////////////////////
      // figure out the number of grid divisions and the grid line increment (in feet MSL)...
      int nNumGridDivisions;  // number of divisions in the VD grid
      int nNumGridLines;  // number of grid lines in the VD grid
      int nGridLineIncrementFeet;  // height of each grid division, in feet MSL
      int nGridAltitudeRangeFeet;  // height of the entire VD grid, in feet MSL

      nGridAltitudeRangeFeet = (int)(dGridAltitudeRangeFeet + 0.5);  // all values of dGridAltitudeRangeFeet will
      // be positive, so we can add 0.5 to the double
      // value and cast it to eliminate rounding errors
      // in nGridAltitudeRangeFeet
      int multiplier = 1;

      int exponent = (int)(log10(dGridAltitudeRangeFeet)) - 1;

      if (dGridAltitudeRangeFeet > 100000)
         exponent--;

      int base_grid_increment = (int)pow(10.0, exponent);

      // find the multiple of the base grid increment that will result in MAX_NUM_GRID_LINES or fewer
      // horizontal grid lines, evenly spaced within the Vertical Display...
      bool done = false;
      while (!done)
      {
         nGridLineIncrementFeet = multiplier * base_grid_increment;

         nNumGridLines = (int)( dGridAltitudeRangeFeet/(double)nGridLineIncrementFeet );

         if (nNumGridLines <= MAX_NUM_GRID_LINES)
         {
            // the current grid line increment produces 8 or fewer grid lines, so we are done
            nNumGridDivisions = nNumGridLines + 1;
            done = true;
         }
         else
            multiplier++;  // try the next greater grid line increment...
      }



      // Draw the VD grid lines and labels using the grid drawing parameters ///////////////////////////////////////////
      //
      int viewHeight = winSize.Height();
      int viewWidth = winSize.Width();
      int leftX = 0;  // left x-coordinate for grid lines
      int rightX;  // right x-coordinate for grid lines
      double ratio;
      int y_coord;
      int nLabelWidth, nLabelHeight;

      UtilDraw utilDraw(pDC);

      CFvwUtil *futil = CFvwUtil::get_instance();  // use CFvwUtil to draw the text
      CFvwUtil *pFvwUtil = CFvwUtil::get_instance();  // instance of CFvwUtil for use in rounding
      POINT current_polygon[4];
      CString cstrAltitudeText;
      int nAltitude;

      // draw the grid lines, beginning at the bottom of the vertical view and working upwards...
      for (i=1; i<nNumGridLines; i++)
      {
         ratio = (double)(i*nGridLineIncrementFeet) / dGridAltitudeRangeFeet;
         y_coord = viewHeight - (int)(ratio * viewHeight);

         if (y_coord < viewHeight-10 && y_coord > 10)  // if the y-axis position lies within the vertical view
         {  // and more than 10 pixels from the view edge...

            // calculate the altitude that corresponds to the current VD grid line...
            if (nCurrentVerticalDisplayAltitudeMin > 0)
            {
               // when the minimum VD altitude is a positive value, add 0.5 to eliminate rounding error...
               nAltitude = (int)((double)nCurrentVerticalDisplayAltitudeMin + 0.5) + i*nGridLineIncrementFeet;
            }
            else
            {
               // when the minimum VD altitude is a negative value, subtract 0.5 to eliminate rounding error...
               nAltitude = (int)((double)nCurrentVerticalDisplayAltitudeMin - 0.5) + i*nGridLineIncrementFeet;
            }

            //  convert the grid line altitude into a string for use in drawing the label...
            cstrAltitudeText.Format("%d", nAltitude);

            // find the pixel width required to display this string...
            futil->get_text_size(pDC, cstrAltitudeText,
               m_pVerticalDisplayProperties->m_cstrFontName, 
               m_pVerticalDisplayProperties->m_nFontSize,
               m_pVerticalDisplayProperties->m_nFontAttributes,
               &nLabelWidth, &nLabelHeight);

            // set the x-coordinate of the right end of the grid line, allowing for 5 pixels of space 
            // on either side of the label...
            rightX = viewWidth - nLabelWidth - 10;

            // draw the grid line...
            utilDraw.set_pen(m_pVerticalDisplayProperties->m_GridLineColor, UTIL_LINE_SOLID, 
               m_pVerticalDisplayProperties->m_nGridLineWidth, TRUE);
            utilDraw.draw_line(leftX, y_coord, rightX, y_coord);

            // draw the corresponding altitude text label at the right end of the grid line...
            futil->draw_text(pDC, cstrAltitudeText, rightX+5, y_coord, UTIL_ANCHOR_CENTER_LEFT, 
               m_pVerticalDisplayProperties->m_cstrFontName, 
               m_pVerticalDisplayProperties->m_nFontSize, 
               m_pVerticalDisplayProperties->m_nFontAttributes, 
               m_pVerticalDisplayProperties->m_nLabelBackground, 
               m_pVerticalDisplayProperties->m_FontColor, 
               m_pVerticalDisplayProperties->m_LabelBackgroundColor,
               0.0, current_polygon);
         }
      }

      // update the mainframe's member variable with the current VD window height...
      m_pFvwMainFrame->m_nVerticalDisplayHeight = winSize.Height();
   }
   else
      AfxMessageBox("Error getting pointer to the Vertical Display Properties object...");
}


void CVerticalViewDisplay::DrawOverlays(CDC *pDC)
{
   CRect clientRect;
   GetClientRect(clientRect);

   ViewMapProj *pMapProj = UTL_get_current_view_map();

   if (pMapProj)
   {
      CVerticalViewProjector projector(pMapProj, m_pVerticalDisplayProperties->m_dMinAltitudeMeters, 
         m_pVerticalDisplayProperties->m_dMaxAltitudeMeters,
         clientRect.Height());

      // give overlays a chance to draw to the Vertical Display view
      OVL_get_overlay_manager()->DrawToVerticalDisplay(pDC, &projector);
   }
}

int CVerticalViewDisplay::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
   // side-step CView's implementation since we don't want to activate
   //  this view
   return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CVerticalViewDisplay::OnMouseMove(UINT nFlags, CPoint point)
{
   CRect clientRect;
   GetClientRect(clientRect);

   if (m_pVerticalDisplayProperties == NULL)
      return;

   ViewMapProj *pMapProj = UTL_get_current_view_map();

   CVerticalViewProjector projector(pMapProj, m_pVerticalDisplayProperties->m_dMinAltitudeMeters, 
      m_pVerticalDisplayProperties->m_dMaxAltitudeMeters,
      clientRect.Height());

   CMainFrame *pFrame = fvw_get_frame();
   static CTextInfoPane *pHelpTextInfoPane = NULL;
   if (pFrame != NULL)
      pHelpTextInfoPane = static_cast<CTextInfoPane *>(m_pFvwMainFrame->GetStatusBarInfoPane(STATUS_BAR_PANE_HELP_TEXT));

   // give overlays a chance to return tooltip and status bar text
   HCURSOR cursor = NULL;
   HintText *pHint = NULL;
   if (OVL_get_overlay_manager()->test_select_vertical(&projector, point, nFlags, &cursor, &pHint) == SUCCESS)
   {
      // status bar text
      if (pHelpTextInfoPane != NULL && pHint != NULL)
         pHelpTextInfoPane->SetText(pHint->get_help_text());

      if (pHint != NULL)
         do_tool_tip(pHint, point);
   }
   else if (pHelpTextInfoPane)
   {
      clear_tool_tip();
      pHelpTextInfoPane->SetText("");
   }
}

void CVerticalViewDisplay::OnLButtonDown(UINT nFlags, CPoint point)
{
   CRect clientRect;
   GetClientRect(clientRect);

   if (m_pVerticalDisplayProperties == NULL)
      return;

   ViewMapProj *pMapProj = UTL_get_current_view_map();

   CVerticalViewProjector projector(pMapProj, m_pVerticalDisplayProperties->m_dMinAltitudeMeters, 
      m_pVerticalDisplayProperties->m_dMaxAltitudeMeters,
      clientRect.Height());

   HCURSOR cursor;
   HintText *pHint = NULL;
   OVL_get_overlay_manager()->select_vertical(&projector, point, nFlags, &cursor, &pHint);
}

void CVerticalViewDisplay::OnActivateView(BOOL, CView*, CView*)
{
   ASSERT(FALSE);      // output only view - should never be active
}


void CVerticalViewDisplay::OnRButtonDown(UINT nFlags, CPoint point) 
{
   CMenu menu;

   menu.CreatePopupMenu();

   menu.AppendMenu(MF_STRING, ID_OPTIONS_VERTICAL_DISPLAY_PROPERTIES, "Vertical Display Properties...");

   RECT rect;
   GetWindowRect(&rect);
   menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left + point.x, rect.top + point.y, this, &rect);
}


void CVerticalViewDisplay::OnVerticalDisplayProperties()
{
   if (m_pVerticalDisplayPropertiesDialog == NULL)
   {
      // create the Vertical Display Properties dialog object...
      m_pVerticalDisplayPropertiesDialog = new CVerticalViewPropertiesDialog();

      // create the Vertical Display Properties dialog...
      m_pVerticalDisplayPropertiesDialog->Create(IDD_VERTICAL_DISPLAY_PROPERTIES, this);
   }
}

void CVerticalViewDisplay::RedrawVerticalDisplay()
{
   RedrawFlag = true;
}

void CVerticalViewDisplay::DrawFromScratch(CDC* memoryDC)
{
   // something changed, so update the backing store...
   DrawAltitudeGrid(memoryDC);
   DrawOverlays(memoryDC);

   RedrawFlag = false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVerticalViewProjector
//

CVerticalViewProjector::CVerticalViewProjector(ViewMapProj *pCurrentMap, double dMinAltitudeMeters, double dMaxAltitudeMeters,
      int nVerticalDisplayHeightPixels)
{
   m_pCurrentMap = pCurrentMap;
   m_dMinAltitudeMeters = dMinAltitudeMeters;
   m_dMaxAltitudeMeters = dMaxAltitudeMeters;
   m_nVerticalDisplayHeightPixels = nVerticalDisplayHeightPixels;
}

bool CVerticalViewProjector::InSurface(double dLat, double dLon, double dAltitudeMeters)
{
   // if the current altitude is outside of the Vertical Display's altitude range, do not draw...
   if ((dAltitudeMeters > m_dMaxAltitudeMeters) || (dAltitudeMeters < m_dMinAltitudeMeters))
      return false;

   // if the current lat/lon coordinates lie within the current visible map, go ahead and draw...
   return (m_pCurrentMap->geo_in_surface(dLat, dLon) == TRUE);
}

int CVerticalViewProjector::ToSurface(double dLat, double dLon, double dAltitudeMeters, int& rX, int& rY)
{
   if ( (m_dMaxAltitudeMeters - m_dMinAltitudeMeters) == 0.0)
      return FAILURE;

   int nUnusedY;
   if (m_pCurrentMap->geo_to_surface(dLat, dLon, &rX, &nUnusedY) != SUCCESS)
   {
      return FAILURE;
   }

   rY = m_nVerticalDisplayHeightPixels - static_cast<int>(((dAltitudeMeters - m_dMinAltitudeMeters) / (m_dMaxAltitudeMeters - m_dMinAltitudeMeters)) 
      * m_nVerticalDisplayHeightPixels + 0.5);

   return SUCCESS;
}

// CFvVerticalViewProjectorImpl
//

CFvVerticalViewProjectorImpl::~CFvVerticalViewProjectorImpl()
{
   delete m_pVerticalViewProj;
}

void CFvVerticalViewProjectorImpl::Initialize(ViewMapProj *pCurrentMap, double dMinAltitudeMeters, double dMaxAltitudeMeters,
   int nVerticalDisplayHeightPixels)
{
   m_pVerticalViewProj = new CVerticalViewProjector(pCurrentMap, dMinAltitudeMeters, dMaxAltitudeMeters, nVerticalDisplayHeightPixels);
}

STDMETHODIMP CFvVerticalViewProjectorImpl::raw_GeoInSurface(double dLat, double dLon, double dAltitudeMeters, long *pbInSurface)
{
   *pbInSurface = m_pVerticalViewProj->InSurface(dLat, dLon, dAltitudeMeters);
   return S_OK;
}

STDMETHODIMP CFvVerticalViewProjectorImpl::raw_GeoToSurface(double dLat, double dLon, double dAltitudeMeters, long *x, long *y, long *pResult)
{
   int rX, rY;
   *pResult = m_pVerticalViewProj->ToSurface(dLat, dLon, dAltitudeMeters, rX, rY);
   
   *x = rX;
   *y = rY;

   return S_OK;
}

STDMETHODIMP CFvVerticalViewProjectorImpl::raw_GetSurfaceSize(long* pWidth, long* pHeight)
{
   int width, height;
   m_pVerticalViewProj->GetSurfaceSize(&width, &height);

   *pWidth = width;
   *pHeight = height;

   return S_OK;
}