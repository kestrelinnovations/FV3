// Copyright (c) 1994-2009,2013,2014 Georgia Tech Research Corporation, Atlanta, GA
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



// iconlist.cpp

// This file contains the GPS trail icon list class, GPSPointIconList, and
// the GPS trail icon sub-list class, GPSPointSubList.  The GPSPointSubList
// class is used by the GPSPointIconList class to manage long lists of GPS
// point icons efficiently.

#include "stdafx.h"
#include "gps.h"
#include "err.h"
#include "map.h"
#include "ovl_mgr.h"

//************************************************************************
// GPSPointIconList class members

// Constructor
GPSPointIconList::GPSPointIconList() : m_rect(0,0,0,0)
{
   m_trail_end = NULL;
}

// Remove and destroy all sub list elements
void GPSPointIconList::Clear()
{
   while (!m_subtrail_list.IsEmpty())
      delete m_subtrail_list.RemoveHead();

   delete_all();
}

// add the icon to the end of the list
int GPSPointIconList::add_point(GPSPointIcon* pIcon, C_gps_trail *pTrail)
{
   GPSPointSubList *subtrail = NULL;
   POSITION position;

   //PLAYBACK
   if (pTrail && pTrail->in_playback_mode())
   {
      add( pIcon );
      position = pTrail->get_last_position();
   }
   else
   {
      if ( pTrail->get_properties()->get_trail_rewind_enabled() )
      {
         COleDateTime
            dt1 = pIcon->get_date_time();
         BOOL bRemoveDone = FALSE;
         while ( !is_empty()
            && dt1 < get_last()->get_date_time() )
         {
            bRemoveDone = TRUE;
            delete get_last();
            remove_at( get_last_position() );
         }

         // Rebuild the subtrail list if any pruning done
         if ( bRemoveDone )
         {
            // Redraw the entire gps trail so old points get scrubbed
            pTrail->set_modified( TRUE );
            OVL_get_overlay_manager()->InvalidateOverlay( pTrail );
 
            // Discard the previous subtrail list
            while ( !m_subtrail_list.IsEmpty() )
               delete m_subtrail_list.RemoveTail();

            // Copy the icon list into the subtrails
            POSITION icon_list_position = get_first_position();
            while ( icon_list_position != NULL )
            {
               add_subtrail_point( icon_list_position );
               get_next( icon_list_position );
            }
            pTrail->get_current_position().reset_point();
         }
      }  // Rewind enabled

      // add the new icon to the end of the icon list and get its position
      add( pIcon );
      position = get_last_position();
   }
   pTrail->Add3DPoint( pIcon );     // In case running in 3D mode

   // Add the point to the sublists
   return add_subtrail_point( position );

}  // add_point()

// Add the icon list element to the subtrail collection
int GPSPointIconList::add_subtrail_point( POSITION icon_list_position )
{
   GPSPointSubList* subtrail = NULL;

   // if the tail subtrail exists and it isn't full, the point will be added
   // to it, otherwise a new subtrail will have to be created
   if ( !m_subtrail_list.IsEmpty() )
   {
      subtrail = m_subtrail_list.GetTail();
      if ( subtrail->is_full() )
         subtrail = NULL;
   }
   
   // if an existing subtrail is not available, create one and add it to the
   // subtrail list
   if ( subtrail == NULL )
   {
      subtrail = new GPSPointSubList( *this, icon_list_position );
      m_subtrail_list.AddTail( subtrail );
      return SUCCESS;
   }

   // add the new icon to the tail subtrail
   if ( subtrail->add( icon_list_position ) != SUCCESS )
   {
      ERR_report("add() failed.");
      return FAILURE;
   }

   return SUCCESS;
}

// determine if the point hits the one of the points in the list
C_icon *GPSPointIconList::hit_test(CPoint point)
{
   GPSPointSubList *subtrail;
   POSITION position;
   C_icon *gps_icon;

   // no subtrails implies no trail points, implies no hit
   if (m_subtrail_list.IsEmpty())
      return NULL;

   // quick reject
   if (!m_rect.PtInRect(point))
      return NULL;

   // call the hit test of each subtrail in top down order (relative to display
   // order), return the first gps point icon hit
   position = m_subtrail_list.GetTailPosition();
   while (position)
   {
      // get the subtrail at position and the position of the previous subtrail
      subtrail = m_subtrail_list.GetPrev(position);

      //ASSERT(subtrail);

      gps_icon = subtrail->hit_test(point);
      if (gps_icon)
         return gps_icon;
   }

   return NULL;   // no hits
}

// display all the points in the list from scratch
int GPSPointIconList::new_draw(ActiveMap* map, d_geo_t ll, d_geo_t ur, int radius, C_gps_trail *pTrail)
{
   GPSPointIcon::prevPoint=CPoint(-1,-1);

   GPSPointSubList *subtrail;
   POSITION position;

   // mark bounding rect as empty
   m_rect.SetRectEmpty();

   // no points
   if (m_subtrail_list.IsEmpty())
      return SUCCESS;

   position = m_subtrail_list.GetHeadPosition();
   while (position)
   {
      subtrail = m_subtrail_list.GetNext(position);

      //ASSERT(subtrail);

      if (subtrail->new_draw(map, ll, ur, radius, pTrail) != SUCCESS)
      {
         ERR_report("new_draw() failed.");
         return FAILURE;
      }

      // expand rect to include last subtrail drawn
      m_rect |= subtrail->get_rect();
   }

   return SUCCESS;
}

// redisplay all the points in the list that fall within the invalidate
// region, clip_rect
int GPSPointIconList::redraw(ActiveMap* map, CRect clip_rect, int radius,  C_gps_trail *pTrail)
{
   GPSPointIcon::prevPoint=CPoint(-1,-1);

   GPSPointSubList *subtrail;
   POSITION position;

   // no points
   if (m_subtrail_list.IsEmpty())
      return SUCCESS;

   // if the trail has no points in the view
   if (m_rect.IsRectEmpty())
      return SUCCESS;

   // if the trail has no points in the clipping region
   clip_rect &= m_rect;
   if (clip_rect.IsRectEmpty())
      return SUCCESS;

   position = m_subtrail_list.GetHeadPosition();
   while (position)
   {
      subtrail = m_subtrail_list.GetNext(position);

      //ASSERT(subtrail);

      if (subtrail->redraw(map, clip_rect, radius, pTrail) != SUCCESS)
      {
         ERR_report("redraw() failed.");
         return FAILURE;
      }
   }

   return SUCCESS;
}

void GPSPointIconList::adjust_rect(CRect rect)
{
   GPSPointSubList *subtrail;

   // no subtrails implies no trail points, implies no hit
   if (m_subtrail_list.IsEmpty())
      return;

   // expand the rectangle of the last subtrail
   subtrail = m_subtrail_list.GetTail();
   subtrail->union_rect(rect);

   // expand the rect of the entire trail
   m_rect |= rect;
}

//************************************************************************
// GPSPointSubList class members

const int GPSPointSubList::m_max_count = 60;

// Constructor
GPSPointSubList::GPSPointSubList(GPSPointIconList &parent, 
   POSITION first_element) : m_parent(parent), m_rect(0,0,0,0)
{
   GPSPointIcon *point;

   // the first and last element in this sub-list is the same, since the list
   // contains only one element
   m_first_element = first_element;
   m_last_element = first_element;
   m_element_count = 1;

   // get the point, so its lat-lon can be used to set the geo-bounds
   point = m_parent.get_next(first_element);
   ASSERT(point);

   // set both corners of the geo-bounds to the first points location
   m_ll.lat = (degrees_t)point->get_latitude();
   m_ll.lon = (degrees_t)point->get_longitude();
   m_ur.lat = (degrees_t)point->get_latitude();
   m_ur.lon = (degrees_t)point->get_longitude();
}

// add a point to the list, expand the geo bounds accordingly.
int GPSPointSubList::add(POSITION next_element)
{
   GPSPointIcon *point;
   POSITION position;

   // maximum sublist length is contant
   if (is_full())
   {
      ERR_report("Sublist already contains maximum number of elements.");
      return FAILURE;
   }

   // save the position of the element being added, since will change the value
   // of next_element
   position = next_element;

   // get the point at the given position
   point = m_parent.get_next(next_element);
   if (point == NULL)
   {
      ERR_report("Invalid position.");
      return FAILURE;
   }

   // update the list size and end of list members
   m_last_element = position;
   m_element_count++;

   // if the point is south of the southern edge, its latitude will become the
   // new southern edge
   if (m_ll.lat > (degrees_t)point->get_latitude())
      m_ll.lat = (degrees_t)point->get_latitude();
   // if the point is north of the northern edge, its latitude will become the
   // new northern edge
   else if (m_ur.lat < (degrees_t)point->get_latitude())
      m_ur.lat = (degrees_t)point->get_latitude();

   // if the western edge is east-of the point, its longitude will become the
   // new western edge
   if (GEO_east_of_degrees(m_ll.lon, (degrees_t)point->get_longitude()))
      m_ll.lon = (degrees_t)point->get_longitude();
   // if the point is east-of the eastern edge, its longitude will become the
   // new eastern edge
   else if (GEO_east_of_degrees((degrees_t)point->get_longitude(), m_ur.lon))
      m_ur.lon = (degrees_t)point->get_longitude();

   return SUCCESS;
}

// determine if the point hits the one of the points in this sub-trail
C_icon *GPSPointSubList::hit_test(CPoint point)
{
   GPSPointIcon *icon;
   POSITION position;
   int count;

   // quick reject
   if (!m_rect.PtInRect(point))
      return NULL;

   count = 0;
   position = m_last_element;
   while (position && count < m_element_count)
   {
      // get the next lowest icon in the draw order
      icon = m_parent.get_prev(position);

      //ASSERT(icon);

      // if the point hits this icon return it
      if (icon->hit_test(point))
         return icon; 

      count++;
   }

   return NULL;
}

// expand the bounding rectangle on the sub-trail to include this rectangle
void GPSPointSubList::union_rect(CRect rect)
{
   if (m_rect.IsRectEmpty())
      m_rect = rect;
   else
      m_rect |= rect;
}

// display the points in this GPS sub-trail from scratch
int GPSPointSubList::new_draw(ActiveMap* map, d_geo_t ll, d_geo_t ur, int radius, C_gps_trail *pTrail )
{
//   GPSPointIcon::prevPoint=CPoint(-1,-1);

   int x, y;
   GPSPointIcon *point;
   POSITION previous;
   int count = 0;
   boolean_t old_rect_empty;

   old_rect_empty = m_rect.IsRectEmpty();

   // mark bounding rect as empty
   if (old_rect_empty)
      m_rect.SetRectEmpty();

   // if map bounds don't intersect the subtrail bounds it won't get drawn
   if (GEO_intersect_degrees(ll, ur, m_ll, m_ur) == FALSE)
   {
      // Set all point rectangles to empty to indicate that they are off the
      // map, iff this subtrail was displayed over the previous map.
      if (!old_rect_empty)
      {
         previous = m_first_element;
         while (previous && count < m_element_count)
         {
            point = m_parent.get_next(previous);
            point->m_rect.SetRectEmpty();
            count++;
         }
      }

      return SUCCESS;
   }

   previous = m_first_element;
   while (previous && count < m_element_count)
   {
      point = m_parent.get_next(previous);
/*
      //PLAYBACK
      if ((pTrail && pTrail->is_in_the_future(point)))
      {
         point->m_rect.SetRectEmpty();
         continue;
      }
*/
      // first clip the location against the map bounds, which is different
      // than the view bounds on a rotated map
      if (GEO_in_bounds(ll.lat, ll.lon, ur.lat, ur.lon,
         (degrees_t)point->get_latitude(), 
         (degrees_t)point->get_longitude()))
      {
         // since the point is on the map, it will be converted to a view
         // coordinate and checked to see if it is in the view
         map->geo_to_surface((degrees_t)point->get_latitude(),
            (degrees_t)point->get_longitude(), &x, &y);

         if (map->point_in_surface(x, y))
         {
            // if this is the first trail icon in the view, or this point's
            // center falls outside the rectangle of the previous point,
            // it will be drawn

            GPSPointIcon *endPoint = m_parent.get_trail_end();
            //are we too close for comfort?
/*
            if
            (
               (!(endPoint && C_gps_trail::visual_occlusion( endPoint->m_rect.CenterPoint(), CPoint(x,y))))
//               &&
//               (!(pTrail && pTrail->is_in_the_future(point)))
            )
*/
            if (point->m_viewable)
            {
               //PLAYBACK
               if (!(pTrail && pTrail->is_in_the_future(point)))
               {
                  // draw radius circle trail icon
                  GPSPointIcon::draw( map->get_CDC(), TRUE, x,y, radius );
               }

               // set trail icon's bounding box
               point->m_rect.SetRect(x-radius, y-radius, x+radius, y+radius);

               // expand gps trail's bounding box
               m_rect |= point->m_rect;

               // set new current position and new trail end
               m_parent.set_trail_end(point);
            }
            else
               // trail icon is a redundant point
               point->m_rect.SetRectEmpty();
         }
         else
         {
            // trail icon is not in the view
            point->m_rect.SetRectEmpty();
            GPSPointIcon::prevPoint=CPoint(-1,-1);
         }
      }
      else
         // trail icon is outside the map
         point->m_rect.SetRectEmpty();

      count++;
   } // end while

   return SUCCESS;
}

// redisplay all the points in the sub-trail that fall within the invalidate
// region, clip_rect
int GPSPointSubList::redraw(ActiveMap* map, CRect clip_rect, int radius, C_gps_trail *pTrail )
{
   CPoint center;
   GPSPointIcon *point;
   POSITION previous;
   CRect expose_rect;
   int count;

   // if this subtrail has no points on the screen
   if (m_rect.IsRectEmpty())
      return SUCCESS;

   // if this subtrail has no points in the clipping region
   clip_rect &= m_rect;
   if (clip_rect.IsRectEmpty())
      return SUCCESS;

   // draw trail icons
   count = 0;
   previous = m_first_element;
   while (previous && count < m_element_count)
   {
      point = m_parent.get_next(previous);

      // only draw icon if the point is in the window
      if (!point->m_rect.IsRectEmpty())
      {
         expose_rect = point->m_rect;
         expose_rect &= clip_rect;

         // only draw icon if the point is in clippiing region
         if (!expose_rect.IsRectEmpty())
         {
            // draw trail marker
            center = point->m_rect.CenterPoint();

            if (!(pTrail && pTrail->is_in_the_future(point) || !point->m_viewable))
            {
               GPSPointIcon::draw( map->get_CDC(), TRUE, center.x, center.y, radius );
            }
         }
      }

      count++;
   }

   return SUCCESS;
}
