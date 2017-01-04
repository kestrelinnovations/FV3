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

// shp.cpp -- Shape file overlay

#include "stdafx.h"
#include "mem.h"
#include <memory>
#include "param.h"
#include "showrmk.h"
#include "shpread.h"
#include "refresh.h"
#include "err.h"
#include "ovlutil.h"
#include "..\GoToPropSheet.h"
#include "GetObjPr.h"
#include "..\mapview.h"
#include "factory.h"
#include "..\StatusBarManager.h"
#include "..\mainfrm.h"
#include "maps.h"
#include "ovl_mgr.h"
#include "FvwGraphicDrawingSize.h"
#include "StringUtils.h"
#include "GeometryUtils.h"
#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

char shp_label[20][81];
static int pattern_num = 0;


// **************************************************************************
// **************************************************************************

C_shp_filter::C_shp_filter(CString text /* = "" */, BOOL selected /* = FALSE */)
{
   m_pDisplayProperties = new SHPDisplayProperties;

   m_text = text;
   m_selected = selected;
   m_num = 0;
   m_pDisplayProperties->width = 2;
   m_pDisplayProperties->fill_style = UTIL_FILL_NONE;
   m_pDisplayProperties->line_style = PS_SOLID;
   m_pDisplayProperties->color_rgb = RGB(255, 0, 0);
   m_pDisplayProperties->background = TRUE;
   m_pDisplayProperties->use_icon = FALSE;
   m_pDisplayProperties->diameter = 8;
   m_pDisplayProperties->pIconImage = NULL;
   m_label_code = 0;
}

C_shp_filter::~C_shp_filter()
{
   delete m_pDisplayProperties;
}

/*static*/
void C_shp_filter::SetIconText(CString iconText)
{ 
   m_icon_text = iconText; 

   if (m_icon_text.GetLength() > 0)
   {
      // construct icon name from icontext
      CString strIconName("Shape\\");
      strIconName += m_icon_text;
      strIconName += ".ico";

      m_pDisplayProperties->pIconImage = CIconImage::load_images(strIconName);
   }
   else
      m_pDisplayProperties->pIconImage = NULL;
}

// **************************************************************************
// *************************************************************

// Equal operator   
C_shp_filter &C_shp_filter::operator=(C_shp_filter &flt) 
{
   m_text = flt.m_text;
   m_selected = flt.m_selected;
   m_num = flt.m_num;
   m_pDisplayProperties->width = flt.m_pDisplayProperties->width;
   m_pDisplayProperties->fill_style = flt.m_pDisplayProperties->fill_style;
   m_pDisplayProperties->line_style = flt.m_pDisplayProperties->line_style;
   m_pDisplayProperties->color_rgb = flt.m_pDisplayProperties->color_rgb;
   m_pDisplayProperties->background = flt.m_pDisplayProperties->background;
   m_pDisplayProperties->use_icon = flt.m_pDisplayProperties->use_icon;
   m_pDisplayProperties->diameter = flt.m_pDisplayProperties->diameter;
   m_label_code = flt.m_label_code;
   *m_pDisplayProperties = *flt.m_pDisplayProperties;
   return *this;
}

// --------------------------------------------------------------------------
// --------------------OOOOOOO---VV----------VV--LL--------------------------
// --------------------OO---OO----VV--------VV---LL--------------------------
// --------------------OO---OO-----VV------VV----LL--------------------------
// --------------------OO---OO------VV----VV-----LL--------------------------
// --------------------OO---OO-------VV--VV------LL--------------------------
// --------------------OO---OO--------VVVV-------LL--------------------------
// --------------------OOOOOOO---------VV--------LLLLLLLLLL------------------


// **************************************************************************
// **************************************************************************

C_shp_ovl::C_shp_ovl(CString name /* = */) : CFvOverlayPersistenceImpl(name)
{
   m_filename = "";
   m_filter_fieldname = "";
   m_tooltip_fieldname1 = "";
   m_tooltip_fieldname2 = "";
   m_statbar_fieldname1 = "";
   m_statbar_fieldname2 = "";
   m_filter_selects = '\0';

   m_pDisplayProperties = new SHPDisplayProperties;
   m_pDisplayProperties->width = 2;
   m_pDisplayProperties->fill_style = UTIL_FILL_NONE;
   m_pDisplayProperties->line_style = UTIL_LINE_SOLID;
   m_pDisplayProperties->color_rgb = RGB(255, 0, 0);
   m_pDisplayProperties->background = TRUE;
   m_pDisplayProperties->diameter = 8;
   m_pDisplayProperties->use_icon = FALSE;
   m_pDisplayProperties->pIconImage = NULL;

   m_filter_mode = SHP_FILTER_NONE;
   m_near_line_mode = TRUE;
   m_label_code = 15;
   m_selected_obj = NULL;
   m_error_reported = FALSE;
   m_infoDialogBaseTitle = "Shape File Info --- ";
}
// end of constructor for C_shp_ovl

C_shp_ovl::~C_shp_ovl()
{
   // Most, if not all, destruction should be accomplished in Finalize.  
   // Due to C# objects registering for notifications, pointers to this object may not be 
   // released until the next garbage collection.

   ATLASSERT(
      m_vecShpObjects.empty()
      && m_filter_list.IsEmpty()
      && m_pDisplayProperties == NULL
      && "Finalize() not called?" );
}

void C_shp_ovl::Finalize()
{
   CWaitCursor waitCursor;

   save_properties();

   clear_ovl();
   clear_filter_list();

   if (this == CRemarkDisplay::get_focus_overlay())
      CRemarkDisplay::close_dlg();

   delete m_pDisplayProperties;
   m_pDisplayProperties = NULL;
}  // Finalize()


CBaseShapeFileOverlay::CBaseShapeFileOverlay() : m_clsHeapAllocator( FALSE )
{
   m_gptPrevScrnLL.lat = -1000.0; // Invalid
   m_hint = new HintText();
}

CBaseShapeFileOverlay::~CBaseShapeFileOverlay()
{
   delete m_hint;
}

void CBaseShapeFileOverlay::clear_ovl()
{
   HEAP_REPORT( _T("Before shpbase::clear_ovl()") )

#ifdef TIMING_TEST
   DWORD dw1 = GetTickCount();
#endif

   m_selected_obj = NULL;

#ifdef FAST_SHAPES_CLEAR
   std::for_each( m_vecShpObjects.begin(), m_vecShpObjects.end(),
      [&]( SHPObjectPtr& spShape )
   {
      spShape.detach();
   } );
   m_clsHeapAllocator.clear();   // Abandon the shape objects
#endif
   m_vecShpObjects.clear();
   m_gptPrevScrnLL.lat = -1000.0;   // Force point recalc

#ifdef TIMING_TEST
   ATLTRACE( _T("shpbase::clear_ovl(), %d ms\n"), (INT) ( GetTickCount() - dw1 ) );
#endif

#ifndef FAST_SHAPES_CLEAR
   HEAP_REPORT( _T("After shpbase::clear_ovl()") )
#endif

}  // shpbase::clear_ovl()


void ClearFilterList(CList<C_shp_filter*, C_shp_filter*>* filter_list)
{
   C_shp_filter *flt;

   // clear the list
   while (!filter_list->IsEmpty())
   {
      flt = filter_list->RemoveHead();
      delete flt;
   }
}

void C_shp_ovl::clear_filter_list()
{
   ClearFilterList(&m_filter_list);
}
// end of clear_filter_list

void C_shp_ovl::fill_filter_list() 
{
   CDbase dbf;
   const int DBFNAME_LEN = 201;
   char dbfname[DBFNAME_LEN];
   const int FIELDNAME_LEN = 21;
   char fieldname[FIELDNAME_LEN];
   char data[256];
   int numrec, k, rslt;
   CList<CString* , CString*> list;
   CString *txt;
   C_shp_filter *flt;
   CString temp;
   POSITION next;
   BOOL found;

   if (m_filter_fieldname.GetLength() < 1)
      return;

   strncpy_s(dbfname, DBFNAME_LEN, m_filename.c_str(), 200);
   dbfname[200] = '\0';
   strcat_s(dbfname, DBFNAME_LEN, ".dbf");
   rslt = dbf.OpenDbf(dbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
   {
      return;
   }

   strncpy_s(fieldname, FIELDNAME_LEN, m_filter_fieldname, 12);

   numrec = dbf.GetNumberOfRecords();
   if (numrec < 1)
   {
      dbf.CloseDbf();
      return;
   }

   // go through the records, making a list of the unique entries
   for (k=1; k<=numrec; k++)
   {
      rslt = dbf.GotoRec(k);
      if (rslt != DB_NO_ERROR)
      {
         k = numrec;
         continue;
      }

      rslt = dbf.GetFieldData( fieldname, data);
      temp = data;

      // check to see if the item is already in the list
      found = FALSE;
      next = list.GetHeadPosition();
      while (next != NULL)
      {
         txt = list.GetNext(next);
         if (!txt->Compare(temp))
         {
            found = TRUE;
            next = NULL;
            continue;
         }
      }
      if (!found)
      {
         txt = new CString;
         *txt = temp;
         list.AddTail(txt);
      }
   }

   dbf.CloseDbf();

   // clear the overlay filter list
   while (!m_filter_list.IsEmpty())
   {
      flt = m_filter_list.RemoveHead();
      delete flt;
   }

   // put the strings in the filter list
   k = 1;
   next = list.GetHeadPosition();
   while (next != NULL)
   {
      txt = list.GetNext(next);
      flt = new C_shp_filter;
      flt->m_text = *txt;
      flt->m_text.TrimRight();
      flt->m_text.TrimLeft();
      flt->m_selected = FALSE;
      flt->m_num = k;
      m_filter_list.AddTail(flt);
      k++;
   }

   // clear the temporary list
   while (!list.IsEmpty())
   {
      txt = list.RemoveHead();
      delete txt;
   }
}
// end of fill_filter_list

// set icon text and load the icon

void C_shp_ovl::SetIconText(const CString& iconText)
{ 
   m_icon_text = iconText;

   if (m_icon_text.GetLength() > 0)
   {
      // construct icon name from icontext
      CString strIconName("Shape\\");
      strIconName += m_icon_text;
      strIconName += ".ico";

      m_pDisplayProperties->pIconImage = CIconImage::load_images(strIconName);
   }
   else
      m_pDisplayProperties->pIconImage = NULL;
}


int C_shp_ovl::draw(ActiveMap* map)
{
   int ret = DrawShapeObjects( map );
   set_valid(TRUE);
   return ret;
}

int CBaseShapeFileOverlay::DrawShapeObjects(ActiveMap* map)
{
   map_scale_t mapscale, smallest_scale;
   BOOL redraw = FALSE;
   CFvwUtil& futil = *CFvwUtil::get_instance();

   if (map == NULL)
      return FAILURE;

   // Precompute the offsets to go around the world
   int pixels_around_world;
   map->get_pixels_around_world( &pixels_around_world );
   m_dHalfWorldDot = ( 0.5 * pixels_around_world ) * pixels_around_world;
   
   double dRot = DEG_TO_RAD( map->actual_rotation() );
   m_dWorldX = pixels_around_world * cos( dRot ),
   m_dWorldY = pixels_around_world * sin( dRot ),

   m_iWorldX = static_cast<int>( floor( m_dWorldX + 0.5 ) );   // Rounded
   m_iWorldY = static_cast<int>( floor( m_dWorldY + 0.5 ) );

   // get the map bounds to clip symbol drawing 
   if ( map->get_vmap_bounds( &m_gptCurScrnLL, &m_gptCurScrnUR ) != SUCCESS )
      return FAILURE;

   m_bMapChanged =   // If map moved need to recalc screen locations
         1e-7 < fabs( m_gptCurScrnLL.lat - m_gptPrevScrnLL.lat )
      || 1e-7 < fabs( m_gptCurScrnLL.lon - m_gptPrevScrnLL.lon )
      || 1e-7 < fabs( m_gptCurScrnUR.lat - m_gptPrevScrnUR.lat )
      || 1e-7 < fabs( m_gptCurScrnUR.lon - m_gptPrevScrnUR.lon );
   m_gptPrevScrnLL = m_gptCurScrnLL;
   m_gptPrevScrnUR = m_gptCurScrnUR;

   // stagger the dithered fill pattern
   if ((m_type == SHPT_POLYGON) || (m_type == SHPT_POLYGONZ) || (m_type == SHPT_POLYGONM))
      pattern_num++;

   // if there are no shape objects, there is nothing to do
   if (m_vecShpObjects.empty())
      return SUCCESS;

   mapscale = map->scale();
   smallest_scale = get_smallest_scale();
   if (smallest_scale > mapscale)
   {
      if (m_selected_obj != NULL)
         CRemarkDisplay::close_dlg();
      return SUCCESS;
   }

  // set flag to draw/not-draw labels based up current map scale
   BOOL drawLabels = get_smallest_labels_scale() <= mapscale 
      && get_smallest_labels_scale() != NULL_SCALE
      && ( ( m_type != SHPT_POLYGON && m_type != SHPT_POLYGONZ && m_type != SHPT_POLYGONM )
         || get_largest_area_labels_scale() == NULL_SCALE
         || get_largest_area_labels_scale() >= mapscale );

   CDC* pDC = map->get_CDC();

   const size_t numShapes = m_vecShpObjects.size();
   for ( size_t iShape = 0; iShape < numShapes; iShape++ )
   {
      SHPObject* pShapeObject = &*m_vecShpObjects[ iShape ];

      // Check for interrupt
      if ( ( iShape % 100 ) == 0
            && FVW_is_draw_interrupted() )
         return FAILURE;

      // the list of shapes is sorted such that NOT_DISPLAYED and NULL shapes are last.  The first time we
      // encounter either case we can stop looping immediately
      if ( pShapeObject == NULL || pShapeObject->displayOrder == SHAPE_NOT_DISPLAYED )
         break;

      // If the shape intersects the screen
      if ( GEO_intersect(
                        pShapeObject->m_grcBounds.ll.lat, pShapeObject->m_grcBounds.ll.lon,
                        pShapeObject->m_grcBounds.ur.lat, pShapeObject->m_grcBounds.ur.lon,
                        m_gptCurScrnLL.lat, m_gptCurScrnLL.lon,
                        m_gptCurScrnUR.lat, m_gptCurScrnUR.lon ) )
      {
         if ( pShapeObject != m_selected_obj )
            DrawShapeObject( map, pShapeObject, FALSE );
      }
      else
         for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
            pShapeObject->GetPart( iPart ).bias_min = +127; // Mark part as not drawn

   }  // Shapes loop

   // Draw the selected one last (again)
   if ( m_selected_obj != nullptr )
      DrawShapeObject( map, m_selected_obj, TRUE );

   // Draw labels in a separate pass so that they are drawn on top of all objects
   if ( drawLabels )
   {
      int iFontSize = TEXT_ELEMENT_FONT_SIZE( 12, pDC->IsPrinting() );

      for ( size_t iShape = 0; iShape < numShapes; iShape++ )
      {
         SHPObject* pShapeObject = &*m_vecShpObjects[ iShape ];
         
         // Check for interrupt
         if ( ( iShape % 100 ) == 0
               && FVW_is_draw_interrupted() )
            return FAILURE;

         // the list of shapes is sorted such that NOT_DISPLAYED and NULL shapes are last.  The first time we
         // encounter either case we can stop looping immediately
         if ( pShapeObject == NULL || pShapeObject->displayOrder == SHAPE_NOT_DISPLAYED )
            break;

         // Skip the selected object since it is always drawn last
         if ( pShapeObject != m_selected_obj && !pShapeObject->IsLabelEmpty()
               && GEO_intersect(
                              pShapeObject->m_grcBounds.ll.lat, pShapeObject->m_grcBounds.ll.lon,
                              pShapeObject->m_grcBounds.ur.lat, pShapeObject->m_grcBounds.ur.lon,
                              m_gptCurScrnLL.lat, m_gptCurScrnLL.lon,
                              m_gptCurScrnUR.lat, m_gptCurScrnUR.lon ) )
            DrawShapeObjectLabels( map, pShapeObject, iFontSize, FALSE );
      }  // Shapes loop

      // Draw the selected one last (again)
      if ( m_selected_obj != nullptr )
         DrawShapeObjectLabels( map, m_selected_obj, iFontSize, TRUE );

    }  // Drawing labels

   return SUCCESS;
}
// end of DrawShapeObjects()


void CBaseShapeFileOverlay::DrawShapeObjectWithParts( ActiveMap* pMap, SHPObject* pShapeObject )
{
   pShapeObject->m_screen_coordinates.resize( pShapeObject->m_cVertices );

   for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
   {
      SHPPart& part = pShapeObject->GetPart( iPart );

      // Geo vertices for this part
      size_t cPartVertices = pShapeObject->GetPartSize( iPart );
      const SHPVertex* pPartVertices = pShapeObject->GetVertices( iPart );

      // Beginning of screen coordinates for this part
      POINT* pPartPoints = &pShapeObject->m_screen_coordinates[ part.part_start ];

      // Calculated unshifted parts
      part.bias_min = 0;
      part.bias_max = 0;

      INT
         iPrevX = 0,
         iPrevY = 0,
         iBiasX = 0, // Pixels
         iBiasY = 0,
         iBias = 0;  // World offset index
      BOOL bHasVisibleRef = FALSE;

      for ( size_t iVertex = 0; iVertex < cPartVertices; iVertex++ )
      {
         DOUBLE
            dLon = pPartVertices[ iVertex ].lon,
            dLat = pPartVertices[ iVertex ].lat;
         if ( pMap->projection_type() == MERCATOR_PROJECTION )
         {
            if ( dLat == +90.0 )
               dLat -= 1e-7;
            else if ( dLat == -90.0 )
               dLat += 1e-7;
         }

         int x, y;
         INT iRslt = pMap->geo_to_surface( dLat, dLon, &x, &y );
         ASSERT( iRslt == SUCCESS );

         // If non-visible orthographic projection point
         if ( x == -2147483647 
            &&  pMap->projection_type() == ORTHOGRAPHIC_PROJECTION )
         {
            if ( bHasVisibleRef )   // If prior visible point
            {
               x = pPartPoints[ iVertex - 1 ].x;  // Replicate
               y = pPartPoints[ iVertex - 1 ].y;
            }
         }
         else  // Visible point
         {
            if ( !bHasVisibleRef )
            {
               // If previous non-visible vertices, fill them with the current vertex
               for ( size_t iVertex2 = 0; iVertex2 < iVertex; iVertex2++ )
               {
                  pPartPoints[ iVertex2 ].x = x;
                  pPartPoints[ iVertex2 ].y = y;
               }
               bHasVisibleRef = TRUE;     // In case contains nonvisible vertices
            }

            // Unwrap segments that take a world leap
            if ( iVertex > 0 )
            {
               DOUBLE dWorldDot =
                  ( m_dWorldX * ( x - iPrevX ) ) + ( m_dWorldY * ( y - iPrevY ) );
               if ( dWorldDot > m_dHalfWorldDot )
               {
                  iBiasX -= m_iWorldX;
                  iBiasY -= m_iWorldY;
                  if ( --iBias < part.bias_min )
                     part.bias_min = iBias;
               }
               else if ( dWorldDot + m_dHalfWorldDot < 0 )
               {
                  iBiasX += m_iWorldX;
                  iBiasY += m_iWorldY;
                  if ( ++iBias > part.bias_max )
                     part.bias_max = iBias;
               }
            }
            iPrevX = x; iPrevY = y;
            x += iBiasX; y += iBiasY;
         }

         POINT& point = pPartPoints[ iVertex ];
         point.x = x;
         point.y = y;
      }  // Vertex loop

      if ( !bHasVisibleRef )
         for ( size_t iVertex = 0; iVertex << cPartVertices; iVertex++ )
            pPartPoints[ iVertex ].x = pPartPoints[ iVertex ].y = -100000;   // Entire part not on screen
   }  // Parts
}

void CBaseShapeFileOverlay::DrawShapeObject( ActiveMap* pMap,
   SHPObject* pShapeObject, BOOL selected )
{
   // if the Shapefile didn't completely load properly, the display properties
   // is NULL
   if (!pShapeObject->pDisplayProperties)
   {
      ERR_report("NULL display properties.  This Shapefile may not have loaded "
         "successfully.");
      return;
   }

   // translate UTIL_LINE_DOT line style into UTIL_LINE_DASH.  This allows for
   // line widths greater than 1
   pShapeObject->pDisplayProperties->line_style =
      (pShapeObject->pDisplayProperties->line_style == UTIL_LINE_DOT)
      ? UTIL_LINE_SQUARE_DOT : pShapeObject->pDisplayProperties->line_style;

   if ( m_bMapChanged )  // If map has moved
      DrawShapeObjectWithParts( pMap, pShapeObject ); // Compute screen points
   DrawShapeObject( pMap->get_CDC(), pShapeObject, selected );

}  // DrawShapeObject()


void CBaseShapeFileOverlay::DrawShapeObject( CDC* pDC, SHPObject* pShapeObject, BOOL selected )
{
   switch( pShapeObject->m_mFeatureMask
      & ( SHPObject::FEATURE_POINT | SHPObject::FEATURE_POLYLINE | SHPObject::FEATURE_POLYGON ) )
   {
      case SHPObject::FEATURE_POINT:
         DrawShapeObjectPoint(pDC, pShapeObject, selected );
         break;

      case SHPObject::FEATURE_POLYLINE: 
         DrawShapeObjectPolyline(pDC, pShapeObject, selected );
         break;
   
      case SHPObject::FEATURE_POLYGON: 
         DrawShapeObjectPolygon(pDC, pShapeObject, selected );
         break;
   
      default:
         ASSERT( FALSE && "Invalid type" );
   }
}

void CBaseShapeFileOverlay::DrawShapeObjectPoint(CDC *pDC, SHPObject* pShapeObject, BOOL selected )
{
   UtilDraw utilDraw(pDC);

   const BOOL isPrinting = pDC->IsPrinting();
   int radius = ICON_ELEMENT_DRAWING_SIZE(
            __max( 1, ( (pShapeObject->pDisplayProperties == nullptr )
                     ? 1 : ( pShapeObject->pDisplayProperties->diameter / 2 ) ) ), isPrinting );

   const POINT& currentPt = pShapeObject->m_screen_coordinates[ 0 ];
   
   if (pShapeObject->pDisplayProperties->use_icon && pShapeObject->pDisplayProperties->pIconImage != NULL)
   {
      int icon_adjusted_size = ICON_ELEMENT_DRAWING_SIZE( 32, isPrinting );
      if (selected)
         pShapeObject->pDisplayProperties->pIconImage->draw_hilighted( pDC, currentPt.x, currentPt.y, icon_adjusted_size );
      else
         pShapeObject->pDisplayProperties->pIconImage->draw_icon( pDC, currentPt.x, currentPt.y, icon_adjusted_size );
   }
   else
   {
      CPen pen;
      int pensize = __max( 1, (int) ( 0.5 + ( 0.25 * radius ) ) );
      if (pShapeObject->pDisplayProperties->background)
      {
         radius += __max( 1, pensize / 2 );
         pen.CreatePen(PS_SOLID, pensize, RGB(0, 0, 0));
      }
      else
         pen.CreatePen(PS_SOLID, pensize, pShapeObject->pDisplayProperties->color_rgb);

      CBrush brush(pShapeObject->pDisplayProperties->color_rgb);
      CPen* oldpen = (CPen*)pDC->SelectObject(&pen);
      CBrush* oldbrush = (CBrush*)pDC->SelectObject(&brush);

      if (selected)
      {
         CBrush yellowbrush;
         yellowbrush.CreateSolidBrush(RGB(255, 255, 0));
         int r2 = radius + ICON_ELEMENT_DRAWING_SIZE( 3, isPrinting );
         pDC->SelectObject(&yellowbrush);
         pDC->Ellipse( currentPt.x - r2, currentPt.y - r2, currentPt.x + r2, currentPt.y + r2 );
         pDC->SelectObject(&brush);
      }

      pDC->Ellipse(currentPt.x-radius, currentPt.y-radius, currentPt.x+radius, currentPt.y+radius);

      if (oldpen != NULL)
         pDC->SelectObject(oldpen);
      if (oldbrush != NULL)
         pDC->SelectObject(oldbrush);
   }
}

 
void CBaseShapeFileOverlay::DrawShapeObjectLabels( ActiveMap* pMap, SHPObject* pShapeObject, int iFontSize, BOOL selected )
{
   CDC* pDC = pMap->get_CDC();

   switch ( pShapeObject->m_mFeatureMask &
            ( SHPObject::FEATURE_POINT | SHPObject::FEATURE_POLYLINE | SHPObject::FEATURE_POLYGON ) )
   {
      case SHPObject::FEATURE_POINT:
      {
         const BOOL isPrinting = pDC->IsPrinting();
         int radius = ICON_ELEMENT_DRAWING_SIZE(
            __max( 1, ( (pShapeObject->pDisplayProperties == nullptr )
                     ? 1 : ( pShapeObject->pDisplayProperties->diameter / 2 ) ) ), isPrinting );

         for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
         {
            SHPPart& part = pShapeObject->GetPart( iPart );
            if ( part.bias_min > part.bias_max )
               continue;   // Not drawn

            const POINT& point = pShapeObject->m_screen_coordinates[ part.part_start ];
            draw_point_label( pDC, point.x, point.y + radius + 3, pShapeObject->GetLabel() );
         }

         break;
      }

      case SHPObject::FEATURE_POLYLINE:
         DrawShapeObjectPolylineLabels( pDC, pShapeObject, iFontSize, selected );
         break;

      case SHPObject::FEATURE_POLYGON:
         DrawShapeObjectPolygonLabels( pMap, pShapeObject, iFontSize, selected );
         break;

      default:
         assert( FALSE && "Illegal type" );
   }  // switch( type )
}

void CBaseShapeFileOverlay::DrawShapeObjectPolygonLabels(
                  ActiveMap* pMap, SHPObject* pShapeObject, int iFontSize, BOOL selected )
{
   CFvwUtil& futil = *CFvwUtil::get_instance();

   int iSurfaceXLimit, iSurfaceYLimit;
   pMap->get_surface_size( &iSurfaceXLimit, &iSurfaceYLimit );
   iSurfaceXLimit--; iSurfaceYLimit--; // Make max right/bottom value

   MapView* map_view = fvw_get_view();
   BOOL bKeepInView = KeepAreaLabelsInView()
      && ( map_view == nullptr
         || map_view->GetMapProjParams().type != GENERAL_PERSPECTIVE_PROJECTION );
   
   if ( !IsAreaLabelEnabled( *pShapeObject ) )
      return;

   // Where it must fit.  Quick offscreen reject
   int* piScreenBounds = pShapeObject->GetPolygonScreenBounds();
   if ( m_bMapChanged )
   {
      pMap->geo_to_surface(
            pShapeObject->m_grcBounds.ll.lat, pShapeObject->m_grcBounds.ll.lon,
            &piScreenBounds[0], &piScreenBounds[1] );
      pMap->geo_to_surface(
            pShapeObject->m_grcBounds.ur.lat, pShapeObject->m_grcBounds.ur.lon,
            &piScreenBounds[2], &piScreenBounds[3] );
   }
   if ( piScreenBounds[0] > iSurfaceXLimit || piScreenBounds[1] < 0
         || piScreenBounds[2] < 0 || piScreenBounds[3] > iSurfaceYLimit )
      return;

   // Text must fit within the extrema of the shape
   int iTextWidth, iTextHeight;
   futil.get_text_size( pMap->get_CDC(), pShapeObject->GetLabel(), "Arial",
                  iFontSize, UTIL_FONT_BOLD, &iTextWidth, &iTextHeight );

   if ( piScreenBounds[1] - piScreenBounds[3] < iTextHeight + 2
         || piScreenBounds[2] - piScreenBounds[0] < iTextWidth + 2 )
      return;

   int
      iTextHalfWidth = iTextWidth >> 1,
      iTextHalfHeight = iTextHeight >> 1;

   for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
   {
      SHPPart& part = pShapeObject->GetPart( iPart );
      if ( part.bias_min > part.bias_max )
         continue;      // Wasn't drawn

      size_t cPartVertices = pShapeObject->GetPartSize( iPart );
      if ( cPartVertices < 3 )
         continue;         // No label for 2 vertex shape

      PolygonLabelInfo& pliPart = pShapeObject->GetPolygonLabelInfo( iPart );
      SHPCentroid& centroid = pliPart.m_gptCentroid;
      int& iCenterX = pliPart.m_iLabelX;
      int& iCenterY = pliPart.m_iLabelY;

      if ( m_bMapChanged )
      {
         SHPVertex* pSHPVertices = pShapeObject->GetVertices( iPart );

         const POINT* pVertices = &pShapeObject->m_screen_coordinates[ part.part_start ];

         // Maybe try to keep the label onscreen
         if ( bKeepInView )
         {
            // Finding centroid of on-screen part of the part
            int
               iX0 = __max( 0, __min( iSurfaceXLimit, pVertices[ 0 ].x ) ),
               iY0 = __max( 0, __min( iSurfaceYLimit, pVertices[ 0 ].y ) ),
               iDX1 = __max( 0, __min( iSurfaceXLimit, pVertices[ 1 ].x ) ) - iX0,
               iDY1 = __max( 0, __min( iSurfaceYLimit, pVertices[ 1 ].y ) ) - iY0,
               i2xAreas = 0,
               i6xMomentsX = 0,
               i6xMomentsY = 0;
            for ( size_t iVertex = 2; iVertex < cPartVertices; iVertex++ )
            {
               int
                  iDX2 = __max( 0, __min( iSurfaceXLimit, pVertices[ iVertex ].x ) ) - iX0,
                  iDY2 = __max( 0, __min( iSurfaceYLimit, pVertices[ iVertex ].y ) ) - iY0,
                  i2xArea = ( iDX2 * iDY1 ) - ( iDX1 * iDY2 );
               i2xAreas += i2xArea;
               i6xMomentsX += i2xArea * ( iDX1 + iDX2 );
               i6xMomentsY += i2xArea * ( iDY1 + iDY2 );

               iDX1 = iDX2; iDY1 = iDY2;  // For next time
            }
            if ( i2xAreas == 0 )
               continue;      // No visible area

            iCenterX = iX0 + ( i6xMomentsX / ( 3 * i2xAreas ) );
            iCenterY = iY0 + ( i6xMomentsY / ( 3 * i2xAreas ) );
         }
         else  // No keep-in-view, use shape centroid for text
         {
            // May need to compute part bounds and centroid
            if ( centroid.lat < -90.0 )
            {
               assert( sizeof(SHPVertex) == sizeof(XY)
                  && "Bad programming assumption, want to use d_geo_t as XY pair" );

               GeometryUtils::PolygonCentroid( cPartVertices,
                  reinterpret_cast< DOUBLE(*)[2] >( pSHPVertices),
                  reinterpret_cast< DOUBLE* >( &centroid ) );
            }  // Centroid calculation

            pMap->geo_to_surface( centroid.lat, centroid.lon,
               &iCenterX, &iCenterY );
         }  // Locked text location

         // Adjust (keep-in-view) or reject (don't try to keep-in-view) if the label overhangs the part boundary
         BOOL bFit = FALSE;   // Assume no room
         for ( int iPass = 0; iPass <= bKeepInView ? 4 : 1; iPass++ )
         {
            // See how far the text overlaps
            int
               iLabelTopY = iCenterY - iTextHalfHeight,   // Text bounds
               iLabelBottomY = iCenterY + iTextHalfHeight,
               iLabelLeftX = iCenterX - iTextHalfWidth,
               iLabelRightX = iCenterX + iTextHalfWidth,
               iTopLeftBoundX = INT_MAX,   // Bounds of shape at text corners
               iBottomLeftBoundX = INT_MAX,
               iTopRightBoundX = INT_MIN,
               iBottomRightBoundX = INT_MIN,
               iLeftUpperBoundY = INT_MAX,
               iLeftLowerBoundY = INT_MIN,
               iRightUpperBoundY = INT_MAX,
               iRightLowerBoundY = INT_MIN;

            for ( size_t i1 = 0; i1 < cPartVertices; i1++ )
            {
               int i2 = ( i1 + 1 ) % cPartVertices;   // Close the polygon

               // L-R limits for top of text box
               if ( ( pVertices[ i1 ].y >= iLabelTopY
                  && pVertices[ i2 ].y <= iLabelTopY )
                  || ( pVertices[ i1 ].y <= iLabelTopY
                  && pVertices[ i2 ].y >= iLabelTopY ) )
               {
                  int i = ( pVertices[ i1 ].y == pVertices[ i2 ].y )
                     ? pVertices[ i1 ].x  // Same y, no interpolation
                     : ( pVertices[ i1 ].x
                     + ( ( pVertices[ i2 ].x - pVertices[ i1 ].x ) * ( iLabelTopY - pVertices[ i1 ].y ) )
                     / ( pVertices[ i2 ].y - pVertices[ i1 ].y ) );

                  NEW_MAX( iTopLeftBoundX, i );
                  NEW_MAX( iTopRightBoundX, i );
               }

               // L-R limits for bottom of text box
               if ( ( pVertices[ i1 ].y >= iLabelBottomY
                  && pVertices[ i2 ].y <= iLabelBottomY )
                  || ( pVertices[ i1 ].y <= iLabelBottomY
                  && pVertices[ i2 ].y >= iLabelBottomY ) )
               {
                  int i = ( pVertices[ i1 ].y == pVertices[ i2 ].y )
                     ? pVertices[ i1 ].x  // Same y, no interpolation
                     : ( pVertices[ i1 ].x
                     + ( ( pVertices[ i2 ].x - pVertices[ i1 ].x ) * ( iLabelBottomY - pVertices[ i1 ].y ) )
                     / ( pVertices[ i2 ].y - pVertices[ i1 ].y ) );

                  NEW_MIN( iBottomLeftBoundX, i );
                  NEW_MAX( iBottomRightBoundX, i );
               }

               // U-D limits for left of text box
               if ( ( pVertices[ i1 ].x >= iLabelLeftX
                  && pVertices[ i2 ].x <= iLabelLeftX )
                  || ( pVertices[ i1 ].x <= iLabelLeftX
                  && pVertices[ i2 ].x >= iLabelLeftX ) )
               {
                  int i = ( pVertices[ i1 ].x == pVertices[ i2 ].x )
                     ? pVertices[ i1 ].y  // Same x, no interpolation
                     : ( pVertices[ i1 ].y
                     + ( ( pVertices[ i2 ].y - pVertices[ i1 ].y ) * ( iLabelLeftX - pVertices[ i1 ].x ) )
                     / ( pVertices[ i2 ].x - pVertices[ i1 ].x ) );

                  NEW_MIN( iLeftUpperBoundY, i );
                  NEW_MAX( iLeftLowerBoundY, i );
               }

               // U-D limits for right of text box
               if ( ( pVertices[ i1 ].x >= iLabelRightX
                  && pVertices[ i2 ].x <= iLabelRightX )
                  || ( pVertices[ i1 ].x <= iLabelRightX
                  && pVertices[ i2 ].x >= iLabelRightX ) )
               {
                  int i = ( pVertices[ i1 ].x == pVertices[ i2 ].x )
                     ? pVertices[ i1 ].y  // Same x, no interpolation
                     : ( pVertices[ i1 ].y
                     + ( ( pVertices[ i2 ].y - pVertices[ i1 ].y ) * ( iLabelRightX - pVertices[ i1 ].x ) )
                     / ( pVertices[ i2 ].x - pVertices[ i1 ].x ) );

                  NEW_MIN( iRightUpperBoundY, i );
                  NEW_MAX( iRightLowerBoundY, i );
               }

            }  // Vertex bounds search loop

            // Adjust text box to keep within the part
            int
               iCenterMinX = __max( iTopLeftBoundX, iBottomLeftBoundX ) + iTextHalfWidth,
               iCenterMaxX = __min( iTopRightBoundX, iBottomRightBoundX ) - iTextHalfWidth,
               iCenterMinY = __max( iLeftUpperBoundY, iRightUpperBoundY ) + iTextHalfHeight,
               iCenterMaxY = __min( iLeftLowerBoundY, iRightLowerBoundY ) - iTextHalfHeight;

            if ( !bKeepInView )
            {
               bFit = iCenterMinX <= iCenterX && iCenterX <= iCenterMaxX
                  && iCenterMinY <= iCenterY && iCenterY <= iCenterMaxY
                  && iTopRightBoundX != INT_MIN // Must have been in bounds
                  && iBottomRightBoundX != INT_MIN
                  && iLeftLowerBoundY != INT_MIN
                  && iRightLowerBoundY != INT_MIN;
               break;
            }

            // If X center is ok
            if ( iCenterMinX <= iCenterX && iCenterX <= iCenterMaxX )
            {
               if ( iCenterMinY <= iCenterY && iCenterY <= iCenterMaxY )
               {
                  bFit = TRUE;      // Ok
                  break;
               }

               // Adjust Y to try to fit
               if ( iCenterMinY > iCenterMaxY ) // If no Y room at this X
               {
                  // Can maybe move left or right



                  break;      // Can't move either up or down
               }

               // Set Y to the offending limit and make another pass
               iCenterY = __max( iCenterMinY, __min( iCenterMaxY, iCenterY ) );
               continue;
            }

            if ( iCenterMinX > iCenterMaxX ) // If no X room at this Y
            {
               // Can maybe move up or down



               break;
            }

            // Set X to the offending limit and make another pass
            iCenterX = __max( iCenterMinX, __min( iCenterMaxX, iCenterX ) );
            continue;
         }  // Passes

         if ( !bFit )
            continue;      // No room for label
      }  // MapChanged

      POINT pts[ 4 ];      // Returned text outline
      futil.draw_text( pMap->get_CDC(), pShapeObject->GetLabel(), iCenterX, iCenterY, 
               UTIL_ANCHOR_CENTER_CENTER, "Arial", iFontSize, 
               UTIL_FONT_BOLD, 
               UTIL_BG_RECT, UTIL_COLOR_BLACK,
               UTIL_COLOR_CREAM, 0.0, pts );

   }  // Parts loop
 }  // DrawShapeObjectPolygonLabels()


 void CBaseShapeFileOverlay::draw_point_label(CDC *dc, int x, int y,
   const char* lpstrLabel) const
{
   POINT pt[4];
   CFvwUtil *futil = CFvwUtil::get_instance();

   futil->draw_text(dc, lpstrLabel, x, y, 
      UTIL_ANCHOR_UPPER_CENTER, "Arial",
      TEXT_ELEMENT_FONT_SIZE( 12, dc->IsPrinting() ), 
      UTIL_FONT_BOLD, 
      UTIL_BG_RECT, UTIL_COLOR_BLACK,
      UTIL_COLOR_CREAM, 0, pt);
}

std::unique_ptr<POINT[]> CBaseShapeFileOverlay::ApplyBias( const SHPObject* pShapeObject, int nNumVertices,
   const POINT* pCurrentPart, int iBias )
{
   int
      iWorldX = iBias * m_iWorldX,
      iWorldY = iBias * m_iWorldY;
   std::unique_ptr<POINT[]> upCurrentPart =
      std::unique_ptr<POINT[]>( new POINT[ nNumVertices ] );
   POINT* pVertex2 = upCurrentPart.get();
   for ( int iVertex = 0; iVertex < nNumVertices; iVertex++ )
   {
      pVertex2[ iVertex ].x = pCurrentPart[ iVertex ].x - iWorldX;
      pVertex2[ iVertex ].y = pCurrentPart[ iVertex ].y - iWorldY;
   }

   return upCurrentPart;
}

std::unique_ptr<POINT[]> CBaseShapeFileOverlay::DrawPolyline( SHPObject* pShapeObject, BOOL selected,
   UtilDraw* utilDraw, int pen_size_sel, int pen_size_bkgd, int pen_size_norm,
   size_t iPart, int iBias )
{
   const POINT* pCurrentPart = &pShapeObject->m_screen_coordinates[ pShapeObject->GetPart( iPart ).part_start ];
   size_t cPartVertices = pShapeObject->GetPartSize( iPart );
   std::unique_ptr<POINT[]> upCurrentPart = nullptr;

   // If need a world-shifted part
   if ( iBias != 0 )
   {
      upCurrentPart = CBaseShapeFileOverlay::ApplyBias( pShapeObject, cPartVertices,
         pCurrentPart, iBias );
      pCurrentPart = upCurrentPart.get(); // Use shifted part
   }

   // construct a POINT array adapter class used by DrawPolyLine
   CLineSegmentArray lineSegmentArray( const_cast< POINT* >( pCurrentPart ),
      cPartVertices, false );

   // draw the selected outline if this object is selected
   if (selected)
   {
      utilDraw->set_pen(SELECTED_SHAPE_COLOR, UTIL_LINE_SOLID, pen_size_sel );
      utilDraw->DrawPolyLine(lineSegmentArray);
   }
   // draw the background outline if requested
   if (pShapeObject->pDisplayProperties->background)
   {
      utilDraw->set_pen(RGB(0, 0, 0), pShapeObject->pDisplayProperties->line_style, pen_size_bkgd );
      utilDraw->DrawPolyLine(lineSegmentArray);
   }
   // draw the foreground lines
   utilDraw->set_pen(pShapeObject->pDisplayProperties->color_rgb, pShapeObject->pDisplayProperties->line_style, pen_size_norm );
   utilDraw->DrawPolyLine(lineSegmentArray);

   return upCurrentPart;
}  // DrawPolyline()


void CBaseShapeFileOverlay::DrawShapeObjectPolyline( CDC* pDC,
   SHPObject* pShapeObject, BOOL selected )
{
   UtilDraw utilDraw( pDC );

   const BOOL isPrinting = pDC->IsPrinting();
   int
      pen_size_norm = LINE_ELEMENT_LINE_WIDTH(
         pShapeObject->pDisplayProperties->width, isPrinting ),
      pen_size_bkgd = LINE_ELEMENT_LINE_WIDTH(
         pShapeObject->pDisplayProperties->width + 2, isPrinting ),
      pen_size_sel = LINE_ELEMENT_LINE_WIDTH(
         pShapeObject->pDisplayProperties->width + 4, isPrinting );

   // for each part of the polyline
   for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
   {
      SHPPart& part = pShapeObject->GetPart( iPart );

      // Process any world-unwrapping offsets
      for ( int iBias = part.bias_min; iBias <= part.bias_max; iBias++ )
      {
         std::unique_ptr<POINT> upCurrentPart =
            DrawPolyline( pShapeObject, selected, &utilDraw,
               pen_size_sel, pen_size_bkgd, pen_size_norm,
               iPart, iBias );

         const POINT* pCurrentPart;
         size_t num_screen_coordinates = pShapeObject->GetPartSize( iPart );
         if ( upCurrentPart == nullptr )
            pCurrentPart = &pShapeObject->m_screen_coordinates[ part.part_start ];
         else
            pCurrentPart = upCurrentPart.get();

      }  // Biases
   }  // Parts
}  // End of DrawShapeObjectPolyline()

void CBaseShapeFileOverlay::DrawShapeObjectPolylineLabels( CDC* pDC,
   SHPObject* pShapeObject, int iFontSize, BOOL selected )
{
   // for each part of the polyline
   for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
   {
      SHPPart& part = pShapeObject->GetPart( iPart );
      size_t cPartPoints = pShapeObject->GetPartSize( iPart );
      if ( m_aTempPoints.size() < cPartPoints )
         m_aTempPoints.resize( cPartPoints );

      // Process any world-unwrapping offsets
      for ( int iBias = part.bias_min; iBias <= part.bias_max; iBias++ )
      {
         const POINT* pCurrentPoints =
            &pShapeObject->m_screen_coordinates[ part.part_start ];
         if ( iBias != 0 )
         {
            for ( size_t iVertex = 0; iVertex < cPartPoints; iVertex++ )
            {
               m_aTempPoints[ iVertex ].x = pCurrentPoints[ iVertex ].x
                  + ( iBias * m_iWorldX );
               m_aTempPoints[ iVertex ].y = pCurrentPoints[ iVertex ].y
                  + ( iBias * m_iWorldY );
            }
            pCurrentPoints = &m_aTempPoints[ 0 ];
         }

#if 1    // Weighted locations
         draw_line_labels( pDC, cPartPoints, pCurrentPoints,
                iFontSize, pShapeObject->GetLabel() );
#else    // Between end points
         draw_line_label( pDC, pCurrentPart[0].x, pCurrentPart[0].y,
               pCurrentPart[ cPartPoints - 1 ].x,
               pCurrentPart[ cPartPoints - 1 ].y,
               iFontSize, pShapeObject->GetLabel() );
#endif
      }  // Biases
   }  // Parts
}  // End of DrawShapeObjectPolyline()


void CBaseShapeFileOverlay::draw_line_label( CDC* dc,
   int x1, int y1,
   int x2, int y2,
   int iFontSize,
   const char* lpstrLabel) const
{
   double angle;
   CFvwUtil *futil = CFvwUtil::get_instance();
   int length, width, height;
   POINT pt[4];

   int tx = (x1 + x2) / 2;
   int ty = (y1 + y2) / 2;
   if ((tx < 0) || (ty < 0))
      return;

   if (x1 < x2)
      angle = futil->line_angle(x1, y1, x2, y2);
   else
      angle = futil->line_angle(x2, y2, x1, y1);

   length = futil->magnitude(x1, y1, x2, y2);

   futil->get_text_size(dc, lpstrLabel, "Arial", iFontSize, UTIL_FONT_BOLD, &width, &height);
   if ( length > (width + 30) )
      futil->draw_text( dc, lpstrLabel, tx, ty, 
            UTIL_ANCHOR_CENTER_CENTER, "Arial", iFontSize, 
            UTIL_FONT_BOLD, 
            UTIL_BG_RECT, UTIL_COLOR_BLACK,
            UTIL_COLOR_CREAM, angle, pt);
}


// Draw labels (possibly more than one) along a polyline
void CBaseShapeFileOverlay::draw_line_labels( CDC *dc, int cVertices, const POINT* pptVertices,
               int iFontSize, LPCSTR pszLabel ) const
{
   if ( cVertices < 2 )
      return;     // Need two points at least

   CFvwUtil& futil = *CFvwUtil::get_instance();

   // Find screen length (possibly angled) of the text
   int iTextWidth, iTextHeight;
   futil.get_text_size( dc, pszLabel, "Arial", iFontSize, UTIL_FONT_BOLD, &iTextWidth, &iTextHeight );

   int
      iStart = 0,       // First vertex
      iNext;            // Next to test
   do
   {
      int iPrev = iStart;
      iNext = iStart + 1;
      
      double
         dWeightedSumX = 0.0,
         dWeightedSumY = 0.0,
         dSumWeights = 0.0;
      do
      {
         double dWeight = futil.magnitude( 
               pptVertices[ iPrev ].x, pptVertices[ iPrev ].y,
               pptVertices[ iNext ].x , pptVertices[ iNext ].y );
         dWeightedSumX += dWeight * ( pptVertices[ iPrev ].x + pptVertices[ iNext ].x );
         dWeightedSumY += dWeight * ( pptVertices[ iPrev ].y + pptVertices[ iNext ].y );
         dSumWeights += dWeight;

         if ( futil.magnitude(
                  pptVertices[ iStart ].x, pptVertices[ iStart ].y,
                  pptVertices[ iNext ].x , pptVertices[ iNext ].y )
               >= ( ( iNext + 1 == cVertices ) ? iTextWidth + 20 : 4 * iTextWidth ) )
            break;
         
         iPrev = iNext;
         if ( ++iNext >= cVertices )
            return;     // Ran out of room for the next label

      } while ( TRUE );

      int
         iX = (int) ( 0.5 * dWeightedSumX / dSumWeights ),
         iY = (int) ( 0.5 * dWeightedSumY / dSumWeights );

      double dAng = futil.line_angle(
                     pptVertices[ iStart ].x, pptVertices[ iStart ].y,
                     pptVertices[ iNext ].x , pptVertices[ iNext ].y );
      if ( dAng > 90.0 && dAng <= 270.0 )
         if ( dAng >= 180.0 )
            dAng -= 180.0;
         else
            dAng += 180.0;

      POINT pts[ 4 ];      // Returned text outline
      futil.draw_text( dc, pszLabel, iX, iY, 
            UTIL_ANCHOR_CENTER_CENTER, "Arial", iFontSize, 
            UTIL_FONT_BOLD, 
            UTIL_BG_RECT, UTIL_COLOR_BLACK,
            UTIL_COLOR_CREAM, dAng, pts );
      
   } while ( ( iStart = iNext ) < cVertices - 1 );

}  // draw_line_labels()


void CBaseShapeFileOverlay::DrawShapeObjectPolygon(CDC *pDC, SHPObject* pShapeObject, BOOL selected)
{
   UtilDraw utilDraw(pDC);

   // region used to fill polygon if necessary.  If the fill style is None there is no need to keep up with a region
   CRgn* pFillRgn = (pShapeObject->pDisplayProperties->fill_style == UTIL_FILL_NONE) ? NULL : new CRgn;

   const BOOL isPrinting = pDC->IsPrinting();
   int
      pen_size_norm = LINE_ELEMENT_LINE_WIDTH( pShapeObject->pDisplayProperties->width, isPrinting ),
      pen_size_bkgd = LINE_ELEMENT_LINE_WIDTH( pShapeObject->pDisplayProperties->width + 2, isPrinting ),
      pen_size_sel = LINE_ELEMENT_LINE_WIDTH( pShapeObject->pDisplayProperties->width + 4, isPrinting );

   // for each part of the polygon
   for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
   {
      const SHPPart& part = pShapeObject->GetPart( iPart );
      size_t cVertices = pShapeObject->GetPartSize( iPart );

      // Process any world-unwrapping offsets
      for ( int iBias = part.bias_min; iBias <= part.bias_max; iBias++ )
      {
         std::unique_ptr<POINT> upCurrentPart =
            DrawPolyline( pShapeObject, selected, &utilDraw,
               pen_size_sel, pen_size_bkgd, pen_size_norm,
               iPart, iBias );

         const POINT* pCurrentPart;
         if ( upCurrentPart == nullptr )
            pCurrentPart = &pShapeObject->m_screen_coordinates[ part.part_start ];
         else
            pCurrentPart = upCurrentPart.get();

         // append to fill region if necessary
         if (pFillRgn != NULL)
         {
            if (pFillRgn->m_hObject == NULL)
            {
               pFillRgn->CreatePolygonRgn( const_cast< POINT* >( pCurrentPart ),
                  cVertices, ALTERNATE );
            }
            else
            {
               CRgn currentRgn;
               currentRgn.CreatePolygonRgn( const_cast< POINT* >( pCurrentPart ),
                  cVertices, ALTERNATE );

               if (currentRgn.m_hObject != NULL)
               {
                  CFvwUtil* futil = CFvwUtil::get_instance();
                  const int mode =
                     futil->polygon_is_clockwise( const_cast< POINT* >( pCurrentPart ),
                        cVertices ) ? RGN_OR : RGN_DIFF;
                  pFillRgn->CombineRgn(pFillRgn, &currentRgn, mode);
               }
            }
         }
      }  // Biases
   }  // Parts

   // fill the polygon
   if (pFillRgn != NULL && pFillRgn->m_hObject != NULL)
   {
      // clip the fill region with the screen
      CRgn screenRgn;
      screenRgn.CreateRectRgn(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
      pFillRgn->CombineRgn(pFillRgn, &screenRgn, RGN_AND);

      if (pShapeObject->pDisplayProperties->fill_style == UTIL_FILL_SHADE)
      {
         WORD hatchBits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };

         GraphicsUtilities utils(pDC->GetSafeHdc());
         utils.draw_shade_regn_rgb((HRGN)pFillRgn->GetSafeHandle(), 
            pShapeObject->pDisplayProperties->color_rgb, hatchBits);
      }
      else
      {
        utilDraw.draw_polygon(pShapeObject->pDisplayProperties->fill_style, (HRGN)(pFillRgn->GetSafeHandle()));
      }
   }

   delete pFillRgn;
}

// Returns the first shape hit
SHPObject* CBaseShapeFileOverlay::ShapeHitTest(map_projection_utils::CMapProjWrapper* map, const CPoint& pt)
{
   d_geo_t scrn_ll, scrn_ur;
   if (map->get_vmap_bounds(&scrn_ll, &scrn_ur) != SUCCESS)
      return NULL;

   // for each shape
   const size_t numShapes = m_vecShpObjects.size();
   for (size_t i=0; i<numShapes; i++)
   {
      SHPObject* pShapeObject = &*m_vecShpObjects[i];

      // the list of shapes is sorted such that NOT_DISPLAYED and NULL shapes are last.  The first time we
      // encounter either case we can stop looping immediately
      if (pShapeObject == NULL || pShapeObject->displayOrder == SHAPE_NOT_DISPLAYED)
         break;

      // if the shape intersects the screen and the given point hits the shape
      if ( GEO_intersect(
               pShapeObject->m_grcBounds.ll.lat, pShapeObject->m_grcBounds.ll.lon,
               pShapeObject->m_grcBounds.ur.lat, pShapeObject->m_grcBounds.ur.lon,
               scrn_ll.lat, scrn_ll.lon,
               scrn_ur.lat, scrn_ur.lon )
            && ShapeHitTest( pShapeObject, pt ) )
      {
         return pShapeObject;
      }
   }

   return NULL;
}  // ShapeHitTest()

// Returns true if the given shape is hit by the given point
bool CBaseShapeFileOverlay::ShapeHitTest(SHPObject* pShapeObject, const CPoint& pt)
{
   CFvwUtil* futil = CFvwUtil::get_instance();
   int d;
   UINT mFeatures = pShapeObject->m_mFeatureMask
         & ( SHPObject::FEATURE_POINT | SHPObject::FEATURE_POLYGON | SHPObject::FEATURE_POLYLINE );
   switch ( mFeatures )
   {
      case SHPObject::FEATURE_POINT:
         d = ICON_ELEMENT_DRAWING_SIZE( 4, FALSE );
         break;
      case SHPObject::FEATURE_POLYLINE:
         d = LINE_ELEMENT_LINE_WIDTH( 4, FALSE );
         break;
      case SHPObject::FEATURE_POLYGON:
         d = LINE_ELEMENT_LINE_WIDTH( 8, FALSE );
         break;
      default:
         d = 0;      // Shouldn't get here
   }

   for ( size_t iPart = 0; iPart < pShapeObject->m_cParts; iPart++ )
   {
      SHPPart& part = pShapeObject->GetPart( iPart );
      if ( part.bias_min > part.bias_max )
         continue;   // Not drawn

      size_t cPartVertices = pShapeObject->GetPartSize( iPart );
      const POINT* pPartPoints = &pShapeObject->m_screen_coordinates[ part.part_start ];

      switch ( mFeatures )
      {
         case SHPObject::FEATURE_POINT:
            if ( abs( pt.x - pPartPoints[ 0 ].x ) <= d
                  && abs( pt.y - pPartPoints[ 0 ].y ) <= d )
               return true;
            break;
      
         case SHPObject::FEATURE_POLYLINE:
            if ( cPartVertices < 2 )
               continue;

            for ( size_t iVertex = 1; iVertex < cPartVertices; iVertex++ )
               if ( futil->distance_to_line(
                     pPartPoints[ iVertex - 1 ].x, pPartPoints[ iVertex - 1 ].y,
                     pPartPoints[ iVertex - 0 ].x, pPartPoints[ iVertex - 0 ].y,
                     pt.x, pt.y ) < d )
                  return true;

            break;
      
         case SHPObject::FEATURE_POLYGON:
         {
            if ( cPartVertices < 2 )
               continue;
            
            size_t iVertex2 = cPartVertices - 1;
            for ( size_t iVertex1 = 0; iVertex1 < cPartVertices; iVertex1++ )
            {
               if ( futil->distance_to_line(
                     pPartPoints[ iVertex1 ].x, pPartPoints[ iVertex1 ].y,
                     pPartPoints[ iVertex2 ].x, pPartPoints[ iVertex2 ].y,
                     pt.x, pt.y ) < d )
                  return true;

               iVertex2 = iVertex1;
            }
            break;
         }
   
         default:
            ASSERT( FALSE && "Invalid type" );
      }  // switch ( mFeatures )
   }  // Pars loop

   return false;
}  // ShapeHitTest()


// if return TRUE - msg contains list of field name and data
// if return FALSE - msg contains error message
BOOL CBaseShapeFileOverlay::get_shp_info(const CString& filename, int recnum, CString & msg) const
{
   CShapeRead shp( const_cast< CBaseShapeFileOverlay* >( this ) );
   CString *txt;
   CString tstr;
   CList<CString*, CString*> list;
   CDbase dbf;
   const int DBFNAME_LEN = 201;
   char dbfname[DBFNAME_LEN];

   strncpy_s(dbfname, DBFNAME_LEN, filename, 200);
   dbfname[200] -='\0';
   strcat_s(dbfname, DBFNAME_LEN, ".dbf");
   int rslt = dbf.OpenDbf(dbfname, OF_READ);
   if (rslt != DB_NO_ERROR)
   {
      msg = "Cannot open the DBF file";
      return FALSE;
   }

   dbf.GotoRec(recnum);
   dbf.GetRecordData(list);
   dbf.CloseDbf();

   msg = "";
   while (!list.IsEmpty())
   {
      txt = list.RemoveHead(); 
      tstr = *txt;
      tstr.TrimRight();
      if ((msg.GetLength() > 0) && (tstr.GetLength() > 0))
         msg += "\n";
      msg += *txt;
      delete txt;
   }
   return TRUE;
}

void C_shp_ovl::release_focus()
{
   if (m_selected_obj != NULL)
   {
      m_selected_obj = NULL;

      // since the selected object changed we need to invalidate the overlay
      OVL_get_overlay_manager()->InvalidateOverlay(this);
   }
}

int C_shp_ovl::on_mouse_moved(IFvMapView *pMapView, CPoint point, UINT flags)
{
   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());

   HCURSOR hCursor = NULL;
   HintText *hint = NULL;
   int ret = OnTestSelected(&mapProjWrapper, point, &hCursor, &hint);
   if (hCursor != NULL)
      pMapView->SetCursor(hCursor);
   if (ret == SUCCESS && hint != NULL)
   {
      pMapView->SetTooltipText(_bstr_t(hint->get_tool_tip()));
      pMapView->m_pFvFrameWnd()->m_pFvStatusBarManager()->SetStatusBarHelpText(_bstr_t(hint->get_help_text()));
   }

   return ret;
}

int CBaseShapeFileOverlay::OnTestSelected(map_projection_utils::CMapProjWrapper* map,
                                             const CPoint& point, HCURSOR *cursor, HintText **hint)
{
   SHPObject* pShapeObject = ShapeHitTest(map, point);
   if ( pShapeObject != NULL && pShapeObject->m_pchShapeStrings != nullptr )
   {
      *cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

      m_hint->set_tool_tip( pShapeObject->GetToolTip() );
      m_hint->set_help_text( pShapeObject->GetHelpText() );
      *hint = m_hint;

      // return that we handled the mouse move
      return SUCCESS;
   }

   // pass through to the next overlay
   return FAILURE;
}

int C_shp_ovl::selected(IFvMapView* pMapView, CPoint point, UINT flags)
{
   boolean_t drag;
   HCURSOR cursor;
   HintText *hint = NULL;

   map_projection_utils::CMapProjWrapper mapProjWrapper(pMapView->m_pCrntMapProj());
   if (OnSelected(this, &mapProjWrapper, point, &drag, &cursor, &hint) == FAILURE)
   {
      release_focus();
      return FAILURE;
   }

   return SUCCESS;
}

int CBaseShapeFileOverlay::OnSelected(C_overlay* pOverlay, map_projection_utils::CMapProjWrapper* map,
                                       const CPoint& point, boolean_t *drag, HCURSOR *cursor, HintText **hint)
{
   *drag = FALSE;
   *cursor = NULL;
   *hint = NULL;

   SHPObject* pShapeObject = ShapeHitTest(map, point);
   if (pShapeObject == NULL)
   {
      // allow the next overlay to handle the select
      return FAILURE;
   }

   show_info(pOverlay, pShapeObject);

   return SUCCESS;
}
// end of selected

// *****************************************************************
// *****************************************************************

#ifdef ENABLE_MAKE_DERIVED_SHAPEFILES

int CBaseShapeFileOverlay::extract_local_shape_files(
            LPCSTR pszFolderIn, LPCSTR pszFolderOut, LPCSTR pszBaseShapefileName,
            const FileFieldExclusions& mpFileFieldExclusions, CString& csErrorMsg )
{
   CWaitCursor waitCursor;

#ifdef TIMING_TEST_2
   LONGLONG llDistTime = 0;
#endif

   // Hook shape reader to private heap
   CShapeRead shpFile( this ), shpFile2( this );

   SHPHandle hSHP_input;
   SHPHandle hSHP_outputs[ 6 ][ 12 ];      // 30x30 degree squares;

   CDbase dbf_input;
   CDbase dbf_outputs[ 6 ][ 12 ];

   int iRslt = SUCCESS; // Assumed

   WIN32_FIND_DATAA find_file_data;    // For LoD files

   // Loop for full resolution file then all level-of-detail files
   HANDLE hFindFile = INVALID_HANDLE_VALUE;  // Full resolution pass first

   CStringA
      csBaseShapefileName = pszBaseShapefileName,  // Should have no extension
      csBaseInputFilespec = pszFolderIn   // Should have trailing backslash
         + csBaseShapefileName,
      csBaseOutputFilespec = pszFolderOut
         + csBaseShapefileName,
     csLocalizedOutputFilespec,
     csFilespec,
     csLoDTag = "";
   RecordFieldExclusions mpRecordFieldExclusions;
   std::auto_ptr< DbfField_t > apFld;     // Auxiliary data
   std::vector< std::vector< SHPVertex > > aaVertices;    // Reusable vertex list

   do
   {
      // Ready to open the output files
      ZeroMemory( hSHP_outputs, sizeof(hSHP_outputs) );

      //////////////////////////////////////////////////////////
      ///////////  Open the files  ///////////////////////
      //////////////////////////////////////////////////////////

      // Open the input .shp file
      csFilespec = csBaseInputFilespec + csLoDTag + ".shp";
      hSHP_input = shpFile.SHPOpen( csFilespec, "rb" );
      if ( hSHP_input == NULL )
         goto InputOpenError;

      // Open the input .dbf file.  Assume LoD data has same .dbf
      csFilespec = csBaseInputFilespec + ".dbf";
      if ( DB_NO_ERROR != dbf_input.OpenDbf( csFilespec, OF_READ ) )
      {
InputOpenError:
         csErrorMsg.Format( _T("Unable to open %s for input"), CA2T( csFilespec ) );
         goto PassFailure;
      }
   
      // Create the field structure
      apFld.reset( new DbfField_t[ dbf_input.m_DbInfo.NumFields ] );
      DbfField_t* fld = apFld.get();
      mpRecordFieldExclusions.clear();

      for ( int k=0; k < dbf_input.m_DbInfo.NumFields; k++ )
      {
         strncpy_s( fld[k].Name, 11, dbf_input.m_DbHead->Field[k].Name, 11 );
         fld[k].Tipe = dbf_input.m_DbHead->Field[k].Tipe;
         fld[k].Length = dbf_input.m_DbHead->Field[k].Length;
         fld[k].Decimal = dbf_input.m_DbHead->Field[k].Decimal;

         // Convert exclusion field name to field number
         FileFieldExclusions::const_iterator it = mpFileFieldExclusions.find( fld[k].Name );
         if ( it != mpFileFieldExclusions.end() )
            mpRecordFieldExclusions[ k ] = it->second;
      }
      
      // Find number of shapes in the input
      int nShapeType, cEntities;
      double adfMinBound[ 4 ], adfMaxBound[ 4 ];
      shpFile.SHPGetInfo( hSHP_input, &cEntities, &nShapeType, adfMinBound, adfMaxBound );

      // Pass the input records to the most appropriate output files
      for ( int iEntity = 0; iEntity < cEntities; iEntity++ )
      {
         // Read the next shape from the input .shp file
         SHPObjectPtr spShape( new
            ( m_clsHeapAllocator.allocate( sizeof(SHPObject) ) ) // Allocate from private heap
            SHPObject( &m_clsHeapAllocator ) );
         SHPObject& shape = *spShape;

         shpFile.SHPReadObject( hSHP_input, iEntity, shape );

         int rslt = dbf_input.GotoRec( iEntity + 1 );

         // Check whether this record is to be excluded
         field_data_t tmpdata, recdata[ 32 ];
         dbf_input.GetRecordData( recdata );
         BOOL bSkipRecord = FALSE;

         for ( RecordFieldExclusions::const_iterator it1 = mpRecordFieldExclusions.begin();
            it1 != mpRecordFieldExclusions.end() && !bSkipRecord; it1++ )
         {
            int k = fld[ it1->first ].Length;
            strncpy_s( tmpdata, sizeof(tmpdata), recdata[ it1->first ], k );
            while ( k > 0 && tmpdata[ --k ] == ' ' )  // Remove trailing spaces
               ;
            tmpdata[ k + 1 ] = '\0';
            if ( it1->second.find( tmpdata ) != it1->second.end() )
               bSkipRecord = TRUE;
         }
         if ( bSkipRecord )
            continue;      // Ignore this shape record

#if 0 && defined _DEBUG
         if ( 0 == strcmp( recdata[1], "AE" ) && 0 == strcmp( recdata[2], "SA" ) )
            int q=0;
#endif
#if 0
         SHPVertex* pV = shape.GetVertices();
         for ( size_t i = 0; i < shape.m_cVertices; i++ )
         {
            double d = pV[ i ].lon;
            BOOL b = d >= 180.0;
         }
#endif

         // Check for need to add extra vertices
         size_t cExtraVertices = 0;
         SHPObjectPtr spShape3;     // Rebuilt shape

         if ( ( shape.m_mFeatureMask & ( SHPObject::FEATURE_POLYLINE | SHPObject::FEATURE_POLYGON ) )
            != 0 )
         {
            if ( shape.m_cParts > aaVertices.size() )
               aaVertices.resize( shape.m_cParts );      // Make sure vertex array is large enough

            for ( size_t iPart = 0; iPart < shape.m_cParts; iPart++ )
            {
               std::vector< SHPVertex >& aVertices = aaVertices[ iPart ];
               aVertices.clear();   // Vertices for current part

               const SHPVertex* pPartVerticesIn = shape.GetVertices( iPart );
               aVertices.push_back( pPartVerticesIn[ 0 ] );  // First vertex in part

               size_t cPartVerticesIn = shape.GetPartSize( iPart );
               if ( cPartVerticesIn > 1 )
               {
                  for ( size_t iPartVertex = 1; iPartVertex < cPartVerticesIn; iPartVertex++ )
                  {
#ifdef TIMING_TEST_2
                     LARGE_INTEGER liT1;
                     QueryPerformanceCounter( &liT1 );
#endif
                     DOUBLE dBearing, dDistMeters;
#if 1 // Slightly faster
                     DOUBLE
                        dLatRad = M_PI * pPartVerticesIn[ iPartVertex ].lat / 180.0,
                        dLonRad = M_PI * pPartVerticesIn[ iPartVertex ].lon / 180.0,
                        dZ = sin( dLatRad ),
                        dCosLat = cos( dLatRad ),
                        dX = dCosLat * cos( dLonRad ),
                        dY = dCosLat * sin( dLonRad );

                     dLatRad = M_PI * pPartVerticesIn[ iPartVertex - 1 ].lat / 180.0;
                     dLonRad = M_PI * pPartVerticesIn[ iPartVertex - 1 ].lon / 180.0;
                     dZ -= sin( dLatRad );
                     dCosLat = cos( dLatRad );
                     dX -= dCosLat * cos( dLonRad );
                     dY -= dCosLat * sin( dLonRad );

                     dBearing = 0.0;
                     dDistMeters = sqrt( ( dX * dX ) + ( dY * dY ) + ( dZ * dZ ) )
                        * 1e7 * 2.0 / M_PI;  // Approximate
#else
                     int iResult = GEO_geo_to_distance(
                        pPartVerticesIn[ iPartVertex - 1 ].lat, pPartVerticesIn[ iPartVertex - 1 ].lon ,  // geo1 in degrees
                        pPartVerticesIn[ iPartVertex ].lat, pPartVerticesIn[ iPartVertex ].lon ,  // geo2
                        &dDistMeters, &dBearing );
                     ATLASSERT( iResult == SUCCESS && "geo_to_distance() failed" );
#endif
#ifdef TIMING_TEST_2
                     LARGE_INTEGER liT2;
                     QueryPerformanceCounter( &liT2 );
                     llDistTime += liT2.QuadPart - liT1.QuadPart;
#endif

                     // If more than 50 km separation, insert intermediate points
                     if ( dDistMeters > 50.0 * 1000.0 )
                     {
#if 1 && defined _DEBUG
                        ATLTRACE(_T("Shape %d, part %d, vert2 %d,")
                           _T(" gpt1=%.3f,%.3f, gpt2=%.3f,%.3f, len=%.3f km, bearing=%.1f\n"),
                           iEntity, iPart, iPartVertex,
                           pPartVerticesIn[ iPartVertex - 1 ].lat, pPartVerticesIn[ iPartVertex - 1 ].lon ,  // geo1 in degrees
                           pPartVerticesIn[ iPartVertex ].lat, pPartVerticesIn[ iPartVertex ].lon ,  // geo2
                           dDistMeters / 1000.0, dBearing );
                        for ( int k = 0; k < dbf_input.m_DbInfo.NumFields; k++ )
                        {
                           ATLTRACE( _T("Field %d, %s=%s\n"), k,
                              dbf_input.m_DbHead->Field[ k ].Name,
                              recdata[ k ] );
                        }
#endif
                        size_t cExtraPartVertices = (INT) ( dDistMeters / ( 50.0 * 1000.0 ) ); // Extra vertices
                        cExtraVertices += cExtraPartVertices;
                        
                        // Interpolate intermediate points
                        for ( size_t k = 1; k <= cExtraPartVertices; k++ )
                        {
                           DOUBLE dF = k / (DOUBLE)( cExtraPartVertices + 1 );
                           SHPVertex vtx;
                           vtx.lat = pPartVerticesIn[ iPartVertex - 1 ].lat
                              + ( dF * ( pPartVerticesIn[ iPartVertex ].lat - pPartVerticesIn[ iPartVertex - 1 ].lat ) );
                           vtx.lon = pPartVerticesIn[ iPartVertex - 1 ].lon
                              + ( dF * ( pPartVerticesIn[ iPartVertex ].lon - pPartVerticesIn[ iPartVertex - 1 ].lon ) );
                           aVertices.push_back( vtx );
                        }  // Extra vertices

                     }  // More than 50km line

                     aVertices.push_back( pPartVerticesIn[ iPartVertex ] );   // Finish the line segment
                  
                  }  // Part vertices
               }  // At least 2 vertices in part
            }  // Part loop

            // If any extra vertices, rebuild shape
            if ( cExtraVertices > 0 )
            {
               spShape3 = new
                  ( m_clsHeapAllocator.allocate( sizeof(SHPObject) ) ) // Allocate from private heap
                  SHPObject( &m_clsHeapAllocator );
               SHPObject& shape3 = *spShape3;

               shpFile2.InitializeShapeData( shape.m_cParts, shape.m_cVertices + cExtraVertices, shape3 );
               shape3.nShapeId = shape.nShapeId;
               shape3.nSHPType = shape.nSHPType;
               shape3.m_mFeatureMask = shape.m_mFeatureMask;

               shape3.m_grcBounds.ll.lat = shape3.m_grcBounds.ll.lon = +DBL_MAX;
               shape3.m_grcBounds.ur.lat = shape3.m_grcBounds.ur.lon = -DBL_MAX;
            
               size_t iPartStart = 0;
               for ( size_t iPart = 0; iPart < shape.m_cParts; iPart++ )
               {
                  std::vector< SHPVertex >& aVertices = aaVertices[ iPart ];
                  size_t cPartVertices = aVertices.size();

                  SHPPart& part = shape3.GetPart( iPart );
                  part.part_start = iPartStart;
                  iPartStart += cPartVertices;
                  
                  part.part_type = shape.GetPart( iPart ).part_type;
                  SHPVertex* pPartVerticesOut = shape3.GetVertices( iPart );

                  for ( size_t iPartVertex = 0; iPartVertex < cPartVertices; iPartVertex++ )
                  {
                     pPartVerticesOut[ iPartVertex ] = aVertices[ iPartVertex ];
                     NEW_MIN( shape3.m_grcBounds.ll.lat, pPartVerticesOut[ iPartVertex ].lat )
                     NEW_MIN( shape3.m_grcBounds.ll.lon, pPartVerticesOut[ iPartVertex ].lon )
                     NEW_MAX( shape3.m_grcBounds.ur.lat, pPartVerticesOut[ iPartVertex ].lat )
                     NEW_MAX( shape3.m_grcBounds.ur.lon, pPartVerticesOut[ iPartVertex ].lon )
                  }

               }  // Parts loop
            }  // Any extra vertices
         }  // Multiple line segments

         SHPObject& curShape = cExtraVertices > 0 ? *spShape3 : shape;

         // Sort polyline segments among the various 30x30 degree output files.
         if ( ( curShape.m_mFeatureMask & SHPObject::FEATURE_POLYLINE ) != 0 )
         {
            SHPObjectPtr spShape2( new
               ( m_clsHeapAllocator.allocate( sizeof(SHPObject) ) ) // Allocate from private heap
               SHPObject( &m_clsHeapAllocator ) );
            SHPObject& shape2 = *spShape2;

            for ( size_t iPart = 0; iPart < curShape.m_cParts; iPart++ )
            {
               size_t cPartVertices = curShape.GetPartSize( iPart );
               ATLASSERT( cPartVertices >= 2 && "Part should have >= 2 vertices" );
               int
                  iLastRow = -1,
                  iLastCol;

               const SHPVertex* pVerticesIn = curShape.GetVertices( iPart );

               std::vector< SHPVertex > aVertices;
               aVertices.push_back( pVerticesIn[ 0 ] );

               for ( size_t iVertex = 1; iVertex < cPartVertices; iVertex++ )
               {
                  ATLASSERT( pVerticesIn[ iVertex ].lon <= +180.0 && "Invalid shape lon value" );

                  // Use the 2nd point of each segment to decide which 30x30 output file
                  int
                     iRow = (int) ( ( pVerticesIn[ iVertex ].lat + 90.0 ) / 30.0 ),
                     iCol = __min( (int) ( ( pVerticesIn[ iVertex ].lon + 180.0 ) / 30.0 ), 11 );
                  if ( iRow != iLastRow || iCol != iLastCol || iVertex + 1 == cPartVertices )
                  {
                     if ( iVertex + 1 == cPartVertices )
                        aVertices.push_back( pVerticesIn[ iVertex ] ); // Append last vertex

                     // Change of tile, write out the previous tile if not empty
                     if ( aVertices.size() > 1 )
                     {
                        shpFile2.InitializeShapeData( 1, aVertices.size(), shape2 );
                        shape2.nSHPType = curShape.nSHPType;
                        shape2.m_mFeatureMask = curShape.m_mFeatureMask;
                        shape2.GetPart( 0 ).part_start = 0;

                        SHPVertex* pVerticesOut = shape2.GetVertices( 0 );
                        shape2.m_grcBounds.ll.lat = shape2.m_grcBounds.ll.lon = +DBL_MAX;
                        shape2.m_grcBounds.ur.lat = shape2.m_grcBounds.ur.lon = -DBL_MAX;
                        for ( size_t k = 0; k < aVertices.size(); k++ )
                        {
                           pVerticesOut[ k ] = aVertices[ k ];
                           NEW_MIN( shape2.m_grcBounds.ll.lat, pVerticesOut[ k ].lat )
                              NEW_MIN( shape2.m_grcBounds.ll.lon, pVerticesOut[ k ].lon )
                              NEW_MAX( shape2.m_grcBounds.ur.lat, pVerticesOut[ k ].lat )
                              NEW_MAX( shape2.m_grcBounds.ur.lon, pVerticesOut[ k ].lon )
                        }

                        if ( iLastRow < 0 )  // Probably 2 vertex part
                        {
                           iLastRow = iRow;
                           iLastCol = iCol;
                        }

                        SHPHandle& hSHP_output = hSHP_outputs[ iLastRow ][ iLastCol ];
                        CDbase& dbf_output = dbf_outputs[ iLastRow ][ iLastCol ];

                        if ( hSHP_output == NULL )    // Need to open outputs for this "tile"
                        {
                           csLocalizedOutputFilespec.Format( "%s%s_local_%c%02d_%c%03d%s",
                              pszFolderOut, csBaseShapefileName,
                              iLastRow >= 3 ? 'N' : 'S', 30 * abs( iLastRow - 3 ),
                              iLastCol >= 6 ? 'E' : 'W', 30 * abs( iLastCol - 6 ),
                              csLoDTag );

                           csFilespec = csLocalizedOutputFilespec + ".shp";
                           if ( NULL == ( hSHP_output = shpFile.SHPCreate( csFilespec, nShapeType ) ) )
                              goto OpenOutputFail;

                           csFilespec = csLocalizedOutputFilespec + ".dbf";
                           if ( DB_NO_ERROR != dbf_output.CreateDbf( csFilespec, fld, dbf_input.m_DbInfo.NumFields ) )
                              goto OpenOutputFail;
                        }

                        dbf_output.AppendRecord( recdata );
                        shpFile2.SHPWriteObject( hSHP_output, -1, &shape2 );

                        aVertices.clear();
                        aVertices.push_back( pVerticesIn[ iVertex - 1 ] ); // Start new set
                     }

                     iLastRow = iRow; iLastCol = iCol;
                  }  // Tile change
                  aVertices.push_back( pVerticesIn[ iVertex ] ); // Start new set
               }  // Vertex loop
            }  // Parts loop
         }  // Simple polyline

         else  // Not simple polyline
         {

            // Convert mean lat/lon to indices of 30x30 degree grid
            int
               iCol = (int) ( ( ( 0.5 * ( shape.m_grcBounds.ll.lon + shape.m_grcBounds.ur.lon ) ) + 180.0 ) / 30.0 ),
               iRow = (int) ( ( ( 0.5 * ( shape.m_grcBounds.ll.lat + shape.m_grcBounds.ur.lat ) ) + 90.0 ) / 30.0 );
            if ( iCol < 0 || iCol >= 12 || iRow < 0 || iRow >= 6 )
            {
               csErrorMsg.Format( _T("Shape object lat/lon indices out of range (%d,%d)"), iRow, iCol );
               goto PassFailure;
            }

            SHPHandle& hSHP_output = hSHP_outputs[ iRow ][ iCol ];
            CDbase& dbf_output = dbf_outputs[ iRow ][ iCol ];

            if ( hSHP_output == NULL )    // Need to open outputs for this "tile"
            {
               csLocalizedOutputFilespec.Format( "%s%s_local_%c%02d_%c%03d%s",
                  pszFolderOut, csBaseShapefileName,
                  iRow >= 3 ? 'N' : 'S', 30 * abs( iRow - 3 ),
                  iCol >= 6 ? 'E' : 'W', 30 * abs( iCol - 6 ),
                  csLoDTag );

               csFilespec = csLocalizedOutputFilespec + ".shp";
               if ( NULL == ( hSHP_output = shpFile.SHPCreate( csFilespec, nShapeType ) ) )
                  goto OpenOutputFail;

               csFilespec = csLocalizedOutputFilespec + ".dbf";
               if ( DB_NO_ERROR != dbf_output.CreateDbf( csFilespec, fld, dbf_input.m_DbInfo.NumFields ) )
               {
OpenOutputFail:   
                  csErrorMsg.Format( "Unable to open %s for output", CA2T( csFilespec ) );
                  goto PassFailure;
               }
            }  // Open output tile

            dbf_output.AppendRecord( recdata );

            shpFile.SHPWriteObject( hSHP_output, -1, &shape );
         }
#ifdef FAST_SHAPES_CLEAR
         spShape.detach(); // Abandon the shape data
         spShape3.detach();
         if ( ( iEntity % 100 ) == 99 )   // Occasionally purge the private heap
            CBaseShapeFileOverlay::clear_ovl();
#endif
      }  // Entity loop
      goto EndPass;

PassFailure:
      iRslt = FAILURE;

EndPass:
      // Close all LoD files
      shpFile.SHPClose( hSHP_input );
      dbf_input.CloseDbf();

      for ( int iRow = 0; iRow < 6; iRow++ )
      {
         for ( int iCol = 0; iCol < 12; iCol++ )
         {
            CDbase& dbf_output = dbf_outputs[ iRow ][ iCol ];
            if ( hSHP_outputs[ iRow ][ iCol ] != NULL )
            {
               shpFile.SHPClose( hSHP_outputs[ iRow ][ iCol ] );
               if ( dbf_output.m_DbOpened )
               {
                  dbf_output.EndAppend();
                  dbf_output.CloseDbf();
               }
               else
                  ATLASSERT( false && "db should be open" );
            }
            else
               ATLASSERT( !dbf_output.m_DbOpened && "db should not be open" );
         }
      }

      if ( iRslt != SUCCESS )
         break;

      // Look for more level-of-detail (xxx_LoDn.shp) files
      if ( hFindFile == INVALID_HANDLE_VALUE )
      {
         if ( INVALID_HANDLE_VALUE ==
               ( hFindFile = FindFirstFileA( csBaseInputFilespec + "_LoD*.shp", &find_file_data ) ) )
            break;   // No LoD files
      }
      else if ( !FindNextFileA( hFindFile, &find_file_data ) ) // If no more LoDs
         break;

      *strrchr( find_file_data.cFileName, '.' ) = '\0';   // Remove extension
      LPCH psz = strstr( find_file_data.cFileName, "_LoD" );
      if ( psz == NULL )
      {
         csErrorMsg = _T("Mixed case level-of-detail (_LoDn) tag");
         break;
      }
      csLoDTag = psz;
   } while ( TRUE ); // Level-of-detail loop

   //
   clear_ovl();   // Decoded shapes
   if ( hFindFile != INVALID_HANDLE_VALUE )
      FindClose( hFindFile );

#ifdef TIMING_TEST_2
   LARGE_INTEGER liFreq;
   QueryPerformanceFrequency( &liFreq );
   ATLTRACE( _T("  Dist time = %.3f\n"), llDistTime / (DOUBLE) liFreq.QuadPart );
#endif
   return iRslt;

}
// End of extract_local_shape_files()

#endif   // def ENABLE_MAKE_DERIVED_SHAPEFILES

// ***************************************************************
// **************************************************************************
// **************************************************************************

std::string C_shp_ovl::GetFilenameFromShape(const SHPObject* pShapeObject) const
{
   // the shape file overlay only has a single file for all shapes
   return m_filename;
}

// Display info about the given shape object
int CBaseShapeFileOverlay::show_info( C_overlay* pOverlay, SHPObject* pShapeObject)
{
   m_selected_obj = pShapeObject;

   // since the selected object changed we need to invalidate the overlay
   OVL_get_overlay_manager()->InvalidateOverlay(pOverlay);

   std::string filename = GetFilenameFromShape(pShapeObject);

   CString edit;
   if (get_shp_info(filename.c_str(), pShapeObject->nShapeId + 1, edit))
   {
      std::string name;
      int cnt, k;
      char ch, slash;
      BOOL found = FALSE;
      // strip the path 
      cnt = filename.length();
      k = cnt-1;
      slash = '\\';
      while ((k>=0) && !found)
      {
         ch = filename[k];
         if ((ch==slash) || (ch==':'))
         {
            found = TRUE;
            continue;
         }
         k--;
      }
      name = "";
      if (found)
         name = filename.substr(k+1, cnt-k);

      //Display the dialog with the information in it
      std::string title = m_infoDialogBaseTitle; 
      title += name;
      if ( !pShapeObject->IsToolTipEmpty() )
      {
         title += " -- ";
         title += pShapeObject->GetToolTip();
      }
      CRemarkDisplay::display_dlg( AfxGetApp()->m_pMainWnd, edit, title.c_str(), pOverlay );
   }

   return SUCCESS;
}
// end of show_info

// add shape file overlay menu items
void C_shp_ovl::menu(ViewMapProj* map, CPoint point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   OnMenu(this, map, point, list);
}

void CBaseShapeFileOverlay::OnMenu(C_overlay* pOverlay, ViewMapProj* map, const CPoint& point, CList<CFVMenuNode*,CFVMenuNode*> & list)
{
   // for each shape
   const size_t numShapes = m_vecShpObjects.size();
   for (size_t i=0; i<numShapes; i++)
   {
      SHPObject* pShapeObject = &*m_vecShpObjects[i];

      // the list of shapes is sorted such that NOT_DISPLAYED and NULL shapes are last.  The first time we
      // encounter either case we can stop looping immediately
      if (pShapeObject == NULL || pShapeObject->displayOrder == SHAPE_NOT_DISPLAYED)
         break;

      if (ShapeHitTest(pShapeObject, point))
      {
         std::string menuItemText("Info on ShapeFile:\\ ");
         std::string fileName = GetFilenameFromShape(pShapeObject);
         std::replace( fileName.begin(), fileName.end(), '\\', '/');
         menuItemText += fileName;

         if ( !pShapeObject->IsLabelEmpty() )
         {
            menuItemText += ", ";
            menuItemText += pShapeObject->GetLabel();
         }

         list.AddTail(new CCOverlayLparamMenuItem(menuItemText.c_str(), pOverlay,
            reinterpret_cast<LPARAM>(pShapeObject),
            CBaseShapeFileOverlay::OnMenuItemSelected));
      }
   }
}

void CBaseShapeFileOverlay::OnMenuItemSelected(ViewMapProj *map, C_overlay* pOverlay, LPARAM lParam)
{
   CBaseShapeFileOverlay* pShapeFileOverlay = dynamic_cast<CBaseShapeFileOverlay *>(pOverlay);
   SHPObject* pShapeObject = reinterpret_cast<SHPObject *>(lParam);

   if (pShapeFileOverlay != NULL && pShapeObject != NULL)
      pShapeFileOverlay->show_info(pOverlay, pShapeObject);
}
// end of point_info

// re-apply filter and label properties to shape objects
void C_shp_ovl::OnSettingsChanged()
{
#ifdef TIMING_TEST
   DWORD dw1 = GetTickCount();
#endif
   CDbase dbf;
   std::string dbfFilename = m_filename;
   dbfFilename += ".dbf";
   int ret = dbf.OpenDbf(dbfFilename.c_str(), OF_READ);
   if (ret != DB_NO_ERROR)
   {
      // unable to open DBF file
      CString msg;
      msg.Format("Unable to open DBF file: %s\n: error = %d", dbfFilename, ret);
      ERR_report(msg);
      AfxMessageBox(msg);
      return;
   }

   // remove current selection (in case the selected object is filtered out)
   release_focus();

   if (m_filter_mode == SHP_FILTER_NONE)
   {
      // for each shape, set displayOrder = 1 and set properties pointer to overlay's properties object
      const size_t numShapes = m_vecShpObjects.size();
      for (size_t i=0; i<numShapes; i++)
      {
         SHPObject* pShapeObject = &*m_vecShpObjects[i];
         if (pShapeObject != NULL)
         {
            pShapeObject->displayOrder = 1;  // always display this shape
            pShapeObject->pDisplayProperties = m_pDisplayProperties;
         }
      }
   }
   else if (m_filter_mode == SHP_FILTER_LIST)
   {
      // for each shape
      const size_t numShapes = m_vecShpObjects.size();
      for (size_t i=0; i<numShapes; i++)
      {
         SHPObject* pShapeObject = &*m_vecShpObjects[i];
         if (pShapeObject == NULL)
            continue;

         // read the appropriate field from the dbf
         char buf[256];
         dbf.GotoRec(pShapeObject->nShapeId + 1);
         dbf.GetFieldData( const_cast<char *>((LPCSTR)m_filter_fieldname), buf);
         CString fieldData(buf);
         fieldData.TrimRight();
         fieldData.TrimLeft();

         // determine which, if any, of the filters applies.  Set the display order and properties of the object appropriately
         pShapeObject->displayOrder = SHAPE_NOT_DISPLAYED;
         POSITION position = m_filter_list.GetHeadPosition();
         int filter_num = 1;
         while (position)
         {
            C_shp_filter* pFilter = m_filter_list.GetNext(position);
            if (pFilter->m_selected && pFilter->m_text.Compare(fieldData) == 0)
            {
               pShapeObject->displayOrder = filter_num;
               pShapeObject->pDisplayProperties = pFilter->m_pDisplayProperties;
               break;
            }
            filter_num++;
         }
      }
   }
   else if (m_filter_mode == SHP_FILTER_RANGE)
   {
      // for each shape
      const size_t numShapes = m_vecShpObjects.size();
      for (size_t i=0; i<numShapes; i++)
      {
         SHPObject* pShapeObject = &*m_vecShpObjects[i];
         if (pShapeObject == NULL)
            continue;

         // read the appropriate field(s) from the dbf
         char buf[256];
         dbf.GotoRec(pShapeObject->nShapeId + 1);
         dbf.GetFieldData( const_cast<char *>((LPCSTR)m_fieldname1), buf);
         m_fielddata1 = CString(buf);
         dbf.GetFieldData( const_cast<char *>((LPCSTR)m_fieldname2), buf);
         m_fielddata2 = CString(buf);

         // if the shape is not filtered, then set displayOrder = 1 and set properties pointer to overlay's properties object
         pShapeObject->displayOrder = filter_hit() ? 1 : SHAPE_NOT_DISPLAYED;
         pShapeObject->pDisplayProperties = m_pDisplayProperties;
      }
   }

   // sort objects based on their display order
   std::sort(m_vecShpObjects.begin(), m_vecShpObjects.end(), CBaseShapeFileOverlay::SortShapesByDisplayOrder);

   CString fieldStr[4], tooltipStr, statusBarStr, labelStr;

   BOOL bDefaultToolTip =
      ( m_tooltip_fieldname1.empty() || m_tooltip_fieldname1 == "<none>" )
         && ( m_tooltip_fieldname2.empty() || m_tooltip_fieldname2 == "<none>" );

   const size_t numShapes = m_vecShpObjects.size();
   for ( size_t iShape = 0; iShape < numShapes; iShape++ )
   {
      SHPObject* pShapeObject = &*m_vecShpObjects[ iShape ];

      // the list of shapes is sorted such that NOT_DISPLAYED and NULL shapes are last.  The first time we
      // encounter either case we can stop looping immediately
      if (pShapeObject == NULL || pShapeObject->displayOrder == SHAPE_NOT_DISPLAYED)
         break;

      dbf.GotoRec(pShapeObject->nShapeId + 1);

      fieldStr[0] = GetField(&dbf, m_tooltip_fieldname1.c_str());
      fieldStr[1] = GetField(&dbf, m_tooltip_fieldname2.c_str());
      fieldStr[2] = GetField(&dbf, m_statbar_fieldname1.c_str());
      fieldStr[3] = GetField(&dbf, m_statbar_fieldname2.c_str());

      // construct tooltip
      if ( bDefaultToolTip )  // No field name specified.  Use first containing "NAME" in the field dname
      {
         CList<CString*, CString*> fieldDataList;
         dbf.FindFieldData("NAME", fieldDataList);
         if ( !fieldDataList.IsEmpty() )
            tooltipStr = *fieldDataList.GetHead();
         else
            tooltipStr.Empty();

         while ( !fieldDataList.IsEmpty() )
            delete fieldDataList.RemoveHead();
      }
      else // At least one field name specified for the tooltip
      {
         tooltipStr = fieldStr[0];
         tooltipStr.TrimRight();
         if ( !fieldStr[1].IsEmpty() )
         {
            if ( !tooltipStr.IsEmpty() )
               tooltipStr += ", ";
            tooltipStr += fieldStr[1];
         }
      }
      tooltipStr.TrimRight();

      // construct status bar text
      statusBarStr = fieldStr[2];
      statusBarStr.TrimRight();
      if ( !fieldStr[3].IsEmpty() )
      {
         if ( !statusBarStr.IsEmpty() )
            statusBarStr += ", ";
         statusBarStr += fieldStr[3];
         statusBarStr.TrimRight();
      }

      // construct the label from the individual fields
      labelStr.Empty();
      for(int i=0; i<4; i++)
      {
         // if the ith bit in m_label_code is set then append the label
         if ( (1 << i) & m_label_code)
         {
            fieldStr[i].TrimLeft();
            if ( !fieldStr[i].IsEmpty() )
            {
               if ( !labelStr.IsEmpty() )
                  labelStr += ", ";
               labelStr += fieldStr[i];
               labelStr.TrimRight();
            }
         }
      }
#if 0 && defined _DEBUG // To see record number
      CString cs;
      cs.Format( _T(",%d,%d,%d"),
         pShapeObject->nShapeId, pShapeObject->m_cParts, pShapeObject->m_cVertices );
      tooltipStr += cs;
#endif
      // Combine the strings into a single buffer
      pShapeObject->ClearStrings(); // Previous helptext, tooltop, label

      size_t c = pShapeObject->m_wHelpTextOffset = tooltipStr.GetLength() + 1;

      size_t cBufferLen = c + statusBarStr.GetLength() + 1 + labelStr.GetLength() + 1;
      pShapeObject->m_pchShapeStrings = reinterpret_cast< LPCH >
         ( m_clsHeapAllocator.allocate( cBufferLen * sizeof(CHAR) ) );

      strcpy_s( pShapeObject->m_pchShapeStrings, cBufferLen, tooltipStr );
      cBufferLen -= c;

      strcpy_s( pShapeObject->m_pchShapeStrings + c, cBufferLen, statusBarStr );
      c = statusBarStr.GetLength() + 1;
      cBufferLen -= c;

      pShapeObject->m_wLabelOffset = pShapeObject->m_wHelpTextOffset + c;
      strcpy_s( pShapeObject->m_pchShapeStrings + pShapeObject->m_wLabelOffset, cBufferLen, labelStr );
   }

   dbf.CloseDbf();
#ifdef TIMING_TEST
   ATLTRACE( _T("shape::OnSettingsChanged(), %d ms\n"), (INT) ( GetTickCount() - dw1 ) );
#endif
}

CString CBaseShapeFileOverlay::GetField(CDbase* pDbase, LPCSTR fieldName)
{
   if ( fieldName[0] != '\0' )
   {
      char buf[256];
      if (pDbase->GetFieldData( fieldName, buf ) == DB_NO_ERROR)
         return buf;
   }

   return "";
}

/* static */
// returns true if a < b.  NULL shapes are placed at the end of the array
bool CBaseShapeFileOverlay::SortShapesByDisplayOrder( const SHPObjectPtr& a, const SHPObjectPtr& b )
{
   return a->displayOrder < b->displayOrder;
}

// ******************************************************************************
// ******************************************************************************

void C_shp_ovl::load_properties()
{
   std::string error_txt;
   int rslt;

   rslt = LoadPreferences(m_filename, m_pDisplayProperties, &m_filter_mode,
      &m_near_line_mode, &m_label_code,
      &m_tooltip_fieldname1, &m_tooltip_fieldname2,
      &m_statbar_fieldname1, &m_statbar_fieldname2,
      &m_fieldname1, &m_fieldname2,
      &m_field1_text1, &m_field1_text2,
      &m_field2_text1, &m_field2_text2,
      &m_field_op, &m_filter_fieldname,
      &m_icon_text, &m_filter_list, &error_txt);
   SetIconText(m_icon_text);

}
// end of load_properties

// ******************************************************************************
// ******************************************************************************

void C_shp_ovl::save_properties()
{
   CString error_txt;
   int rslt;

   rslt = save_pref_file(error_txt);
}
// end of save_properties

int C_shp_ovl::open(const CString& filename)
{
   CWaitCursor waitCursor;

   m_fileSpecification = filename;

   CShapeRead shp(  static_cast< CBaseShapeFileOverlay* >( this ) );
   std::string error_txt;
   char *basename;
   int i, size;

   size = filename.GetLength();
   basename = (char*) malloc(size+1);
   strncpy_s( basename, size+1, filename, size );
   basename[size] = '\0';
   for ( i = strlen(basename)-1; 
         i > 0 && basename[i] != '.' && basename[i] != '/'
             && basename[i] != '\\';
         i-- ) {}

   if ( basename[i] == '.' )
      basename[i] = '\0';

   // why do we need m_filename?  We should be able to use m_fileSpecification instead
   m_filename = basename;

   free(basename);

#ifdef TIMING_TEST
   DWORD dw1 = GetTickCount();
#endif

   if ( SUCCESS != shp.open( NULL, m_filename.c_str(), error_txt, &m_vecShpObjects ) )
   {
      AfxMessageBox(error_txt.c_str());
      return FAILURE;
   }

#ifdef TIMING_TEST
   ATLTRACE( _T("shape::open(\"%s\"), %d ms\n"), filename, (INT) ( GetTickCount() - dw1 ) );
#endif

   // set the overlay's type based on the first object (all objects will have the same type)
   m_type = (m_vecShpObjects.size() > 0) ? m_vecShpObjects[0]->nSHPType : -1;

   load_properties();

   // apply filter and label properties to the shape objects (only once until they are changed in overlay options)
   OnSettingsChanged();

   if (error_txt.length() > 0)
   {
      if (!m_error_reported)
         AfxMessageBox(error_txt.c_str());
      m_error_reported = TRUE;
   }

   set_valid(TRUE);
   return SUCCESS;
}
// end of open

// save the current overlay to a shp file

int C_shp_ovl::save(CString filename)
{
   CShapeRead shp( this );
   CString error_txt;
   const int BASENAME_LEN = 201;
   char basename[BASENAME_LEN];
   int i;
// int rslt;

   strncpy_s( basename, BASENAME_LEN, filename, 200 );
   basename[200] = '\0';
   for ( i = strlen(basename)-1; 
         i > 0 && basename[i] != '.' && basename[i] != '/'
             && basename[i] != '\\';
         i-- ) {}

   if ( basename[i] == '.' )
      basename[i] = '\0';

   m_filename = basename;

   set_valid(TRUE);
   return SUCCESS;
}
// end of save

// ******************************************************************************
// *****************************************************************

map_scale_t C_shp_ovl::get_smallest_scale()
{ 
   CString display_above = PRM_get_registry_string("ShapeFile", 
      "ShapesHideAbove", "1:5 M");
   set_smallest_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_scale; 
}


// *****************************************************************
// *****************************************************************

void C_shp_ovl::set_smallest_scale(map_scale_t scale) 
{ 
   m_smallest_scale = scale; 
}

// *****************************************************************
// *****************************************************************

map_scale_t C_shp_ovl::get_smallest_labels_scale()
{ 
   CString display_above = PRM_get_registry_string("ShapeFile", 
      "LabelsHideAbove", "1:250 K");

   set_smallest_labels_scale(MAP_get_scale_from_string(display_above));
   return m_smallest_labels_scale; 
}
// end of get_smallest_labels_scale

// *****************************************************************
// *****************************************************************

void C_shp_ovl::set_smallest_labels_scale(map_scale_t scale) 
{ 
   m_smallest_labels_scale = scale; 
}

// *****************************************************************
// *****************************************************************

BOOL C_shp_ovl::is_pref_file_writable() 
{ 
   CFile file;
   std::string filename;

   filename = m_filename;
   filename += ".prf";

   if ( !file.Open( filename.c_str(), CFile::modeRead | CFile::modeWrite ) )
      return FALSE;

   file.Close();
   return TRUE;
}

// *****************************************************************
// *****************************************************************

int C_shp_ovl::save_pref_file(CString &error_txt) 
{ 
   CFile file;
   char *buf;
   int len, k;
   std::string filename;
   CString temp, data, tmp;
   POSITION next;
   C_shp_filter *flt;
   CFvwUtil *futil = CFvwUtil::get_instance();
   BYTE red, grn, blu;

   filename = m_filename;
   filename += ".prf";

   if ( !file.Open( filename.c_str(), CFile::modeCreate | CFile::modeWrite ) )
   {
      // save it in the falcon data shape directory
      // isolate the filename
      k = filename.rfind('\\');
      if (k >= 0)
      {
         OverlayTypeDescriptor *pOverlayTypeDesc = OVL_get_type_descriptor_list()->GetOverlayTypeDescriptor(FVWID_Overlay_ShapeFile);
         temp = pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;
         temp += filename.substr(k).c_str();
      }
      if ( !file.Open( temp, CFile::modeCreate | CFile::modeWrite ) )
      {
         error_txt = "Unable to create Preferences file -- ";
         error_txt += temp;
         return FAILURE;
      }
   }
   
   const int BUF_LEN = 500;
   buf = (char*) malloc(BUF_LEN);

   // write the ID line
   strcpy_s(buf, BUF_LEN, "ShpPrefFile");
   buf[11] = 0x0d;
   buf[12] = 0x0a;
   file.Write(buf, 13);
   
   strcpy_s(buf, BUF_LEN, "Color, ");
   temp.Format("%3d", 0);
   strcat_s(buf, BUF_LEN, temp.Left(3));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "ColorRGB, ");
   red = GetRValue(m_pDisplayProperties->color_rgb);
   grn = GetGValue(m_pDisplayProperties->color_rgb);
   blu = GetBValue(m_pDisplayProperties->color_rgb);
   temp.Format("%3u%3u%3u", red, grn, blu);
   strcat_s(buf, BUF_LEN, temp.Left(9));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "Width, ");
   temp.Format("%3d", m_pDisplayProperties->width);
   strcat_s(buf, BUF_LEN, temp.Left(3));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "Fill, ");
   temp.Format("%3d", m_pDisplayProperties->fill_style);
   strcat_s(buf, BUF_LEN, temp.Left(3));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "LineStyle, ");
   temp.Format("%3d", m_pDisplayProperties->line_style);
   strcat_s(buf, BUF_LEN, temp.Left(3));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "Diameter, ");
   temp.Format("%3d", m_pDisplayProperties->diameter);
   strcat_s(buf, BUF_LEN, temp.Left(3));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "Background, ");
   if (m_pDisplayProperties->background)
      strcat_s(buf, BUF_LEN, "Y");
   else
      strcat_s(buf, BUF_LEN, "N");
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterMode, ");
   if (m_filter_mode == SHP_FILTER_LIST)
      strcat_s(buf, BUF_LEN, "L");
   else if (m_filter_mode == SHP_FILTER_RANGE)
      strcat_s(buf, BUF_LEN, "R");
   else
      strcat_s(buf, BUF_LEN, "N");
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "UseIcon, ");
   if (m_pDisplayProperties->use_icon)
      strcat_s(buf, BUF_LEN, "Y");
   else
      strcat_s(buf, BUF_LEN, "N");
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "NearLineMode, ");
   if (m_near_line_mode)
      strcat_s(buf, BUF_LEN, "Y");
   else
      strcat_s(buf, BUF_LEN, "N");
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "LabelCode, ");
   temp.Format("%2d", m_label_code);
   strcat_s(buf, BUF_LEN, temp.Left(3));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   // write the icon name
   strcpy_s(buf, BUF_LEN, "IconText, ");
   strcat_s(buf, BUF_LEN, GetIconText().Left(32));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "TooltipField1, ");
   strcat_s(buf, BUF_LEN, m_tooltip_fieldname1.substr(0,12).c_str());
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "TooltipField2, ");
   strcat_s(buf, BUF_LEN, m_tooltip_fieldname2.substr(0,12).c_str());
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "StatbarField1, ");
   strcat_s(buf, BUF_LEN, m_statbar_fieldname1.substr(0,12).c_str());
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "StatbarField2, ");
   strcat_s(buf, BUF_LEN, m_statbar_fieldname2.substr(0,12).c_str());
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterFieldName1, ");
   strcat_s(buf, BUF_LEN, m_fieldname1.Left(12));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterFieldName2, ");
   strcat_s(buf, BUF_LEN, m_fieldname2.Left(12));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterField1Text1, ");
   strcat_s(buf, BUF_LEN, m_field1_text1);
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterField1Text2, ");
   strcat_s(buf, BUF_LEN, m_field1_text2);
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterField2Text1, ");
   strcat_s(buf, BUF_LEN, m_field2_text1);
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterField2Text2, ");
   strcat_s(buf, BUF_LEN, m_field2_text2);
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterFieldOp, ");
   if (m_field_op == SHP_FILTER_OP_AND)
      strcat_s(buf, BUF_LEN, "A");
   else
      strcat_s(buf, BUF_LEN, "O");
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   strcpy_s(buf, BUF_LEN, "FilterField, ");
   strcat_s(buf, BUF_LEN, m_filter_fieldname.Left(12));
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   k = m_filter_list.GetCount();
   len = m_filter_fieldname.GetLength();
   if ((k < 1) && (len > 1))
      fill_filter_list();

   next = m_filter_list.GetHeadPosition();
   if (m_filter_fieldname.GetLength() < 1)
      next = NULL;
   while (next != NULL)
   {
      flt = m_filter_list.GetNext(next);
      // check for invalid values
      flt->m_pDisplayProperties->width = futil->limit(flt->m_pDisplayProperties->width, 1, 20);
      flt->m_pDisplayProperties->fill_style = futil->limit(flt->m_pDisplayProperties->fill_style, 0, UTIL_FILL_SHADE);
      if ((flt->m_pDisplayProperties->line_style != UTIL_LINE_SOLID) && (flt->m_pDisplayProperties->line_style != UTIL_LINE_DOT))
         flt->m_pDisplayProperties->line_style = UTIL_LINE_SOLID;
      flt->m_pDisplayProperties->diameter = futil->limit(flt->m_pDisplayProperties->diameter, 0, 20);
      data = "FilterData, ";
      data += flt->m_text.Left(180);
      data += "|";
      temp.Format("%3d", 0);
      data += temp;
      temp.Format("%3d", flt->m_pDisplayProperties->width);
      data += temp;
      temp.Format("%3d", flt->m_pDisplayProperties->fill_style);
      data += temp;
      temp.Format("%3d", flt->m_pDisplayProperties->line_style);
      data += temp;
      temp.Format("%3d", flt->m_pDisplayProperties->diameter);
      data += temp;
      if (flt->m_pDisplayProperties->background)
         data += "Y";
      else
         data += "N";
      if (flt->m_selected)
         data += "Y";
      else
         data += "N";
      if (flt->m_pDisplayProperties->use_icon)
         data += "Y";
      else
         data += "N";
      red = GetRValue(flt->m_pDisplayProperties->color_rgb);
      grn = GetGValue(flt->m_pDisplayProperties->color_rgb);
      blu = GetBValue(flt->m_pDisplayProperties->color_rgb);
      temp.Format("%3u%3u%3u", red, grn, blu);
      data += temp.Left(9);
      data += flt->GetIconText().Left(32);
      len = data.GetLength();
      len += 2;

      free(buf);
      buf = (char*) malloc(len);
      if (buf == NULL)
      {
         file.Close();
         return FAILURE;
      }

      strcpy_s(buf, len, data);
      len = strlen(buf);
      buf[len] = 0x0d;
      buf[len+1] = 0x0a;
      file.Write(buf, len+2);
   }

   free(buf);
   buf = (char*) malloc(50);

   // write the last line
   strcpy_s(buf, 50, "EndOfFile");
   len = strlen(buf);
   buf[len] = 0x0d;
   buf[len+1] = 0x0a;
   file.Write(buf, len+2);

   free(buf);

   file.Close();

   return SUCCESS;
}
// end of save_pref_file

// *****************************************************************
// *****************************************************************
/*static*/
int C_shp_ovl::readline(CFile *file, char *buf, int maxlen)
{
   char ch;
   int pos = 0;

   if (maxlen < 1)
      return SUCCESS;

   file->Read(&ch, 1);
   buf[pos] = ch;
   pos++;
   while (ch != 0x0a)
   {
      try
      {
         file->Read(&ch, 1);
         buf[pos] = ch;
         pos++;
         if (pos >= maxlen)
         {
            buf[maxlen] = '\0';
            return SUCCESS;
         }
      }
      catch(...)
      {
         return FAILURE;
      }
   }
   buf[pos-2] = '\0';

   return SUCCESS;
}

// *****************************************************************
// *****************************************************************

bool ReadInField(const std::string& field_name, char* buf, BOOL* out)
{
   const int BUF_LEN = 200;
   if (!strncmp(buf, field_name.c_str(), field_name.length()))
   {
      if (out != nullptr )
      {
         if (buf[field_name.length()] == 'Y')
            *out = TRUE;
         else
            *out = FALSE;
      }
      return true;
   }
   return false;
}

bool ReadInField(const std::string& field_name,
   char* buf, char* buf2, int length, int* out)
{
   const int BUF_LEN = 200;
   if (!strncmp(buf, field_name.c_str(), field_name.length()))
   {
      if (out != nullptr )
      {
         strncpy_s(buf2, BUF_LEN, buf+field_name.length(), length);
         buf2[length] = '\0';
         *out = atoi(buf2);
      }
      return true;
   }
   return false;
}

bool ReadInField(const std::string& field_name,
   char* buf, char* buf2, int length, bool trim, CString* out)
{
   const int BUF_LEN = 200;
   if (!strncmp(buf, field_name.c_str(), field_name.length()))
   {
      if (out != nullptr)
      {
         strncpy_s(buf2, BUF_LEN, buf+field_name.length(), length);
         buf2[length] = '\0';
         *out = buf2;
         if (trim)
         {
            out->TrimRight();
            out->TrimLeft();
         }
      }
      return true;
   }
   return false;
}

bool ReadInField(const std::string& field_name,
   char* buf, char* buf2, int length, bool trim_result, std::string* out)
{
   const int BUF_LEN = 200;
   if (!strncmp(buf, field_name.c_str(), field_name.length()))
   {
      if (out != nullptr)
      {
         strncpy_s(buf2, BUF_LEN, buf+field_name.length(), length);
         buf2[length] = '\0';
         *out = buf2;
         if (trim_result)
         {
            *out = trim(out);
         }
      }
      return true;
   }
   return false;
}

/*static*/
int C_shp_ovl::LoadPreferences(const std::string& shape_filename,
   SHPDisplayProperties* display_properties, int* filter_mode,
   BOOL* near_line_mode, int* label_code,
   std::string* tooltip_fieldname1, std::string* tooltip_fieldname2,
   std::string* statbar_fieldname1, std::string* statbar_fieldname2,
   CString* fieldname1, CString* fieldname2,
   CString* field1_text1, CString* field1_text2,
   CString* field2_text1, CString* field2_text2,
   int* field_op, CString* filter_fieldname,
   CString* icon_text, CList<C_shp_filter* , C_shp_filter*>* filter_list,
   std::string *error_txt)
{
   CFile file;
   const int BUF_LEN = 200;
   char buf[BUF_LEN], buf2[BUF_LEN];
   CString datstr;
   int len, pos, k, rslt;
   CStringArray list;
   CString tmp, temp;
   BOOL notdone, any_selected;
   C_shp_filter *flt;
   CFvwUtil *futil = CFvwUtil::get_instance();
   BYTE red, grn, blu;

   if (filter_list != nullptr)
      ClearFilterList(filter_list);

   std::string filename = shape_filename;
   filename += ".prf";
   tmp = filename.c_str();
   if (!file.Open(filename.c_str(), CFile::modeRead))
   {
      // try to load it from the falcon data shape directory
      // isolate the filename
      len = tmp.GetLength();
      k = tmp.ReverseFind('\\');
      if (k >= 0)
      {
         OverlayTypeDescriptor *pOverlayTypeDesc =
            OVL_get_type_descriptor_list()->
            GetOverlayTypeDescriptor(FVWID_Overlay_ShapeFile);
         temp = pOverlayTypeDesc->fileTypeDescriptor.defaultDirectory;
         temp += tmp.Right(len-k-1);
         filename = temp;
      }
      if (!file.Open(filename.c_str(), CFile::modeRead))
      {
         *error_txt = "Unable to open Preferences file -- ";
         *error_txt += temp;
         return FAILURE;
      }
   }
   
   rslt = readline(&file, buf, 199);
   if (rslt != SUCCESS)
   {
      *error_txt = "Error reading Preferences file -- ";
      *error_txt += filename.c_str();
      return FAILURE;
   }
   
   if (strncmp(buf, "ShpPrefFile",11))
   {
      *error_txt = "Invalid Preferences file -- ";
      *error_txt += filename.c_str();
      return FAILURE;
   }
   
   // set display properties to defaults
   if (display_properties != nullptr )
   {
      display_properties->width = 2;
      display_properties->fill_style = UTIL_FILL_NONE;
      display_properties->line_style = PS_SOLID;
      display_properties->color_rgb = RGB(255, 0, 0);
      display_properties->background = TRUE;
      display_properties->use_icon = FALSE;
      display_properties->diameter = 8;
      display_properties->pIconImage = NULL;
   }

   any_selected = FALSE;
   notdone = TRUE;
   while (notdone)
   {
      rslt = readline(&file, buf, 199);
      if (rslt != SUCCESS)
      {
         notdone = FALSE;
         continue;
      }
      if (!strncmp(buf, "EndOfFile", 9))
      {
         notdone = FALSE;
         continue;
      }
      if (!strncmp(buf, "Color, ", 7))
      {
         strncpy_s(buf2, BUF_LEN, buf+7, 3);
         buf2[3] = '\0';
         // we no longer use the Color (only ColorRGB)
         continue;
      }
      if (!strncmp(buf, "ColorRGB, ", 10))
      {
         if (display_properties != nullptr )
         {
            strncpy_s(buf2, BUF_LEN, buf+10, 3);
            buf2[3] = '\0';
            red = atoi(buf2);
            strncpy_s(buf2, BUF_LEN, buf+13, 3);
            buf2[3] = '\0';
            grn = atoi(buf2);
            strncpy_s(buf2, BUF_LEN, buf+16, 3);
            buf2[3] = '\0';
            blu = atoi(buf2);
            display_properties->color_rgb = RGB(red, grn, blu);
         }
         continue;
      }

      if (ReadInField("Width, ", buf, buf2, 3,
         display_properties == nullptr ?  nullptr : &display_properties->width))
         continue;

      if (ReadInField("Fill, ", buf, buf2, 3,
         display_properties == nullptr ?
         nullptr : &display_properties->fill_style))
         continue;

      if (ReadInField("LineStyle, ", buf, buf2, 3,
         display_properties == nullptr ?
         nullptr : &display_properties->line_style))
         continue;

      if (ReadInField("Diameter, ", buf, buf2, 3,
         display_properties == nullptr ?
         nullptr : &display_properties->diameter))
         continue;

      if (ReadInField("Background, ", buf,
         display_properties == nullptr ?
         nullptr :  &display_properties->background))
         continue;

      if (!strncmp(buf, "UseFilter, ", 11))
      {
         if (filter_mode != nullptr )
         {
            if (buf[11] == 'Y')
               *filter_mode = SHP_FILTER_LIST;
            else
               *filter_mode = SHP_FILTER_NONE;
         }
         continue;
      }
      if (!strncmp(buf, "FilterMode, ", 12))
      {
         if (filter_mode != nullptr )
         {
            *filter_mode = SHP_FILTER_NONE;
            if (buf[12] == 'L')
               *filter_mode = SHP_FILTER_LIST;
            if (buf[12] == 'R')
               *filter_mode = SHP_FILTER_RANGE;
         }
         continue;
      }

      if (ReadInField("UseIcon, ", buf,
         display_properties == nullptr ?
         nullptr : &display_properties->use_icon))
         continue;

      if (ReadInField("NearLineMode, ", buf, near_line_mode))
         continue;

      if (ReadInField("LabelCode, ", buf, buf2, 3, label_code))
         continue;

      if (ReadInField("IconText, ", buf, buf2, 32, false, icon_text))
         continue;

      if (ReadInField("TooltipField1, ", buf, buf2, 10, false,
         tooltip_fieldname1))
         continue;

      if (ReadInField("TooltipField2, ", buf, buf2, 10, false,
         tooltip_fieldname2))
         continue;

      if (ReadInField("StatbarField1, ", buf, buf2, 10, false,
         statbar_fieldname1))
         continue;

      if (ReadInField("StatbarField2, ", buf, buf2, 10, false,
         statbar_fieldname2))
         continue;

      if (ReadInField("FilterFieldName1, ", buf, buf2, 10, true, fieldname1))
         continue;

      if (ReadInField("FilterFieldName2, ", buf, buf2, 10, true, fieldname2))
         continue;

      if (ReadInField("FilterField1Text1, ", buf, buf2, 10, true, field1_text1))
         continue;

      if (ReadInField("FilterField1Text2, ", buf, buf2, 10, true, field1_text2))
         continue;

      if (ReadInField("FilterField2Text1, ", buf, buf2, 10, true, field2_text1))
         continue;

      if (ReadInField("FilterField2Text2, ", buf, buf2, 10, true, field2_text2))
         continue;

      if (!strncmp(buf, "FilterFieldOp, ", 15))
      {
         if (field_op != nullptr )
         {
            if (buf[15] == 'A')
               *field_op = SHP_FILTER_OP_AND;
            else
               *field_op = SHP_FILTER_OP_OR;
         }
         continue;
      }

      if (ReadInField("FilterField, ", buf, buf2, 10, true, filter_fieldname))
         continue;

      if (!strncmp(buf, "FilterData, ", 12))
      {
         if (filter_fieldname == nullptr ||
            filter_fieldname->GetLength() < 1 ||
            !filter_fieldname->Compare("<none>"))
         {
            continue;
         }
         strncpy_s(buf2, BUF_LEN, buf+12, 100);
         buf2[100] = '\0';
         temp = buf2;
         pos = temp.Find('|');
         if (pos > -1)
         {
            tmp = temp.Left(pos);
            flt = new C_shp_filter;
            flt->m_text = tmp;
            len = temp.GetLength();
            tmp = temp.Right(len-pos-1);
            tmp += "                        ";
            temp = tmp.Left(3);
            temp = tmp.Mid(3, 3);
            flt->m_pDisplayProperties->width = futil->limit(atoi(temp), 1, 20);
            temp = tmp.Mid(6, 3);
            flt->m_pDisplayProperties->fill_style =
               futil->limit(atoi(temp), 0, UTIL_FILL_SHADE);
            temp = tmp.Mid(9, 3);
            flt->m_pDisplayProperties->line_style = atoi(temp);
            if ((flt->m_pDisplayProperties->line_style != UTIL_LINE_SOLID) &&
               (flt->m_pDisplayProperties->line_style != UTIL_LINE_DOT))
            {
               flt->m_pDisplayProperties->line_style = UTIL_LINE_SOLID;
            }
            temp = tmp.Mid(12, 3);
            flt->m_pDisplayProperties->diameter =
               futil->limit(atoi(temp), 1, 20);
            if (tmp.GetAt(15) != 'Y')
               flt->m_pDisplayProperties->background = FALSE;
            if (tmp.GetAt(16) == 'Y')
               flt->m_selected = TRUE;
            if (tmp.GetAt(17) == 'Y')
               flt->m_pDisplayProperties->use_icon = TRUE;
            temp = tmp.Mid(18, 3);
            red = atoi(temp);
            temp = tmp.Mid(21, 3);
            grn = atoi(temp);
            temp = tmp.Mid(24, 3);
            blu = atoi(temp);
            flt->m_pDisplayProperties->color_rgb = RGB(red, grn, blu);
            len = tmp.GetLength();
            tmp = tmp.Right(len-27);
            tmp.TrimRight();
            flt->SetIconText(tmp);
            if (flt->m_selected)
               any_selected = TRUE;
            filter_list->AddTail(flt);
         }
      }
   }
   
   file.Close();

   return SUCCESS;
}
// end of LoadPreferences

BOOL C_shp_ovl::is_valid_ovl_ptr(C_overlay *testovl) 
{
   C_overlay *ovl;

   ovl = OVL_get_overlay_manager()->get_first_of_type(FVWID_Overlay_ShapeFile);
   if (ovl == NULL)
      return FALSE;

   if (ovl == testovl)
      return TRUE;

   while (ovl != NULL)
   {
      ovl = OVL_get_overlay_manager()->get_next_of_type(ovl, FVWID_Overlay_ShapeFile);
      if (ovl != NULL) 
         if (ovl == testovl)
            return TRUE;
   }

   return FALSE;
}

// end of is_valid_ovl_ptr

// *****************************************************************
// **************************************************************************

BOOL C_shp_ovl::is_number(const CString& txt)
{
   int len, k;
   BOOL isnum = FALSE;
   char ch;

   len = txt.GetLength();
   for (k=0; k<len; k++)
   {
      ch = txt[k];
      if ((ch != '.') && (ch != '-') && (ch != ' ') &&
         ((ch < '0') || (ch > '9')))
         return FALSE;
   }
   return TRUE;
}

// *****************************************************************
// **************************************************************************

BOOL C_shp_ovl::filter_hit() const
{
   CString text1, text2, text3;
   int rslt;
   BOOL hit1, hit2, blank1, blank3;
   double num1, num2, num3;

   if (m_filter_mode == SHP_FILTER_NONE)
      return TRUE;

   if (m_filter_mode == SHP_FILTER_LIST)
      return FALSE;

   // sanity check
   if (m_filter_mode != SHP_FILTER_RANGE)
      return FALSE;


   if ((m_fielddata1.GetLength() < 1) && (m_fielddata2.GetLength() < 1))
      return FALSE;
   if ((m_field1_text1.GetLength() < 1) && (m_field1_text2.GetLength() < 1) &&
      (m_field2_text1.GetLength() < 1) && (m_field2_text2.GetLength() < 1))
      return FALSE;

   hit1 = FALSE;
   hit2 = FALSE;

   text1 = m_field1_text1;
   text2 = m_fielddata1;
   text3 = m_field1_text2;
   text1.MakeUpper();
   text1.TrimLeft();
   text1.TrimRight();
   text2.MakeUpper();
   text2.TrimLeft();
   text2.TrimRight();
   text3.MakeUpper();
   text3.TrimLeft();
   text3.TrimRight();

   // check for numeric
   if (is_number(text1) && is_number(text2) && is_number(text3))
   {
      num1 = atoi(text1);
      num2 = atoi(text2);
      num3 = atoi(text3);
      if ((num1 <= num2) && (num2 <= num3))\
         hit1 = TRUE;
   }
   else
   {
      blank1 = text1.GetLength() < 1;
      blank3 = text3.GetLength() < 1;
      rslt = text2.Compare(text1);
      if ((rslt >= 0) || blank1)
      {
         rslt = text2.Compare(text3);
         if ((rslt <= 0) || blank3)
            hit1 = TRUE;
      }
   }

   text1 = m_field2_text1;
   text2 = m_fielddata2;
   text3 = m_field2_text2;
   text1.MakeUpper();
   text1.TrimLeft();
   text1.TrimRight();
   text2.MakeUpper();
   text2.TrimLeft();
   text2.TrimRight();
   text3.MakeUpper();
   text3.TrimLeft();
   text3.TrimRight();
   blank1 = text1.GetLength() < 1;
   blank3 = text3.GetLength() < 1;
   if (blank1 && blank3)
      return hit1;

   // check for numeric
   if (is_number(text1) && is_number(text2) && is_number(text3))
   {
      num1 = atoi(text1);
      num2 = atoi(text2);
      num3 = atoi(text3);
      if ((num1 <= num2) && (num2 <= num3))\
         hit2 = TRUE;
   }
   else
   {
      rslt = text2.Compare(text1);
      if ((rslt >= 0) || blank1)
      {
         rslt = text2.Compare(text3);
         if ((rslt <= 0) || blank3)
            hit2 = TRUE;
      }
   }


   if (m_field_op == SHP_FILTER_OP_OR)
   {
      if (hit1 || hit2)
         return TRUE;
   }

   if (m_field_op == SHP_FILTER_OP_AND)
   {
      if (hit1 && hit2)
         return TRUE;
   }

   return FALSE;
}
// end field_hit

#ifdef HEAP_TRACE
void CBaseShapeFileOverlay::HeapTrace( LPCTSTR pszWhere )
{
   int cSegments = 0, cUsed = 0;
   DWORD dwCommitted = 0, dwUnCommitted = 0;
   const HANDLE& hHeap = m_clsHeapAllocator.GetHeap();
   if ( hHeap != INVALID_HANDLE_VALUE && hHeap != NULL )
   {
      PROCESS_HEAP_ENTRY phe;
      phe.lpData = NULL;
      while ( ::HeapWalk( hHeap, &phe ) )
      {
         if ( ( phe.wFlags & PROCESS_HEAP_ENTRY_BUSY ) != 0 )
            cUsed++;

         if ( ( phe.wFlags & PROCESS_HEAP_REGION ) == 0 )
            continue;

         cSegments++;
         dwCommitted += phe.Region.dwCommittedSize;
         dwUnCommitted += phe.Region.dwUnCommittedSize;
      }
   }
   ATLTRACE( _T("%s,\n")
            _T("  heap segments allocated = %d, buffers = %d,\n")
            _T("  committed = 0x%x (%d KB),\n")
            _T("  uncommitted = 0x%x (%d KB),\n")
            _T("  total = 0x%x (%d KB)\n"),
            pszWhere,
            cSegments, cUsed,
            dwCommitted, dwCommitted / 1000,
            dwUnCommitted, dwUnCommitted / 1000,
            dwCommitted + dwUnCommitted, ( dwCommitted + dwUnCommitted ) / 1000 );
}  // HeapTrace()
#endif

// End of shp.cpp

