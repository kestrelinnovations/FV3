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

// ContourLists.cpp: implementation of the CContourLists class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "refresh.h"    // for FVW_is_draw_interrupted()

#include "ContourPoint.h"
#include "ContourLists.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContourLists::CContourLists() : m_DataReady(false)
{
   m_Lines.SetSize(0, 50);
   m_Levels.SetSize(0, 10);
}

CContourLists::~CContourLists()
{
   ClearLists();
}

void CContourLists::AddPoint(int CellRow1, int CellCol1, int CellRow2, 
                             int CellCol2, double Lat, double Lon, 
                             int contour)
{
   ContourMultiMap* pMultiMap;

   if (!m_contourMultiMaps.Lookup(contour, pMultiMap))
   {
      // A list does not exist for this contour level, create one
      pMultiMap = new ContourMultiMap;
      m_contourMultiMaps.SetAt(contour, pMultiMap);
   }

   CContourPoint *p1 = new CContourPoint(CellCol1, CellRow1, CellCol2, CellRow2, Lat, Lon);
   pMultiMap->insert(std::make_pair(p1->m_CellID1, p1));
}

bool CContourLists::PointsConnected(CContourPoint* p1, CContourPoint* p2)
{
   if (p1->m_CellID1.ID == p2->m_CellID1.ID ||
       p1->m_CellID1.ID == p2->m_CellID2.ID ||
       p1->m_CellID2.ID == p2->m_CellID1.ID ||
       p1->m_CellID2.ID == p2->m_CellID2.ID)
   {
      return true;
   }

   return false;
}

void CContourLists::SortPoints()
{
   // If this is called, data is read and regardless of the number of lines
   // generated, we have traced the contours necessary.  Flag data as ready
   m_DataReady = true;

   // Loop through the map of lists getting each contour point list for processing into polylines
   int nContourLevel;
   ContourMultiMap *pMultiMapContourPoints;
   POSITION pos = m_contourMultiMaps.GetStartPosition();
   while (pos != NULL)
   {
      m_contourMultiMaps.GetNextAssoc(pos, nContourLevel, pMultiMapContourPoints);
      SortPoints(pMultiMapContourPoints, nContourLevel);
   }
}
  
void CContourLists::SortPoints(ContourMultiMap* pMultiMapContourPoints, int nContourLevel)
{
   // while there are points remaining in the multimap
   while(pMultiMapContourPoints->size() > 0)
   {
      // we are using a list rather than a std::stack because we need to iterate through each of 
      // the points on the "stack" to check for loop-backs
      std::list<CContourPoint *> pointStack;

      // remove a contour point from the multimap (any one will do).  Because a copy of the starting
      // point is a always added to the list, make sure we delete pInitial when done
      ContourMultiMapIt it = pMultiMapContourPoints->begin();
      CContourPoint* pInitial = it->second;
      pMultiMapContourPoints->erase(it);

      CContourPoint *pCurrent = pInitial;
      CContourList *pCurrentList = NULL;

      // while there are still points on the stack
      do
      {
         // check for neighbors.  if the current point has multiple neighbors then push it on
         // the stack and come back to it later
         ContourMultiMapIt itNeighbor;
         const BOOL bMultipleNeighbors = GetNextNeighbor(pMultiMapContourPoints, pCurrent, itNeighbor);
         if (bMultipleNeighbors)
            pointStack.push_back(pCurrent);

         // if the top of the stack has any neighbors
         while (itNeighbor != pMultiMapContourPoints->end())
         {
            // add it to the contour list (start a new list if necessary)
            //
            if (pCurrentList == NULL)
            {
               pCurrentList = new CContourList();
               CContourPoint *pStart = new CContourPoint();
               *pStart = *pCurrent;
               pCurrentList->AddTail(pStart);
            }

            // remove the used neighbor from the multimap
            CContourPoint *pCurrent = itNeighbor->second;
            pMultiMapContourPoints->erase(itNeighbor);

            pCurrentList->AddTail(pCurrent);

            const BOOL bMultipleNeighbors = GetNextNeighbor(pMultiMapContourPoints, pCurrent, itNeighbor);
            if (bMultipleNeighbors)
               pointStack.push_back(pCurrent);
         }

         // otherwise, if no neighbors, terminate the current line
         //
         if (pCurrentList != NULL)
         {
            CheckForLoopBack(pCurrentList, pointStack);
            m_Lines.Add(pCurrentList);
            m_Levels.Add(nContourLevel);
            pCurrentList = NULL;
         }

         if (!pointStack.empty())
         {
            pCurrent = pointStack.back();
            pointStack.pop_back();
         }

      } while (!pointStack.empty());

      delete pInitial;
   }
}

// CheckForLoopBack - check to see if end of current line connects back to itself.  If so, add a new vertex to the 
// list to complete the loop
void CContourLists::CheckForLoopBack(CContourList* pCurrentList, std::list<CContourPoint *>& pointStack)
{
   POSITION pos = pCurrentList->GetTailPosition();
   ASSERT(pos); // any time a line is added an initial point is added
   pCurrentList->GetPrev(pos);

   // check to see if the end of the current list loops back to any of the points on the stack.
   // a point is on the stack only if it had two or more neighbors
   if (pos != NULL)
   {
      CContourPoint *pPrev = pCurrentList->GetAt(pos);
      std::list<CContourPoint *>::const_iterator itStack = pointStack.begin();
      while (itStack != pointStack.end())
      {
         // we don't want to loop-back with the immediate previous point in the list which can potentially
         // be on the stack
         if (*itStack == pPrev)
         {  
            itStack++;
            continue;
         }

         if (PointsConnected(pCurrentList->GetTail(), *itStack))
         {
            // if so, create a new point to close the loop
            CContourPoint *pCloseLoop = new CContourPoint();
            *pCloseLoop = **itStack;
            pCurrentList->AddTail(pCloseLoop);
            break; // there can only be a single loop
         }

         itStack++;
      }
   }
}

BOOL CContourLists::GetNextNeighbor(ContourMultiMap* pMultiMapContourPoints, CContourPoint *p, ContourMultiMapIt& itNeighbor)
{
   ContourMultiMapIt itEnd = pMultiMapContourPoints->end();
   itNeighbor = itEnd;

   int nNeighborCount = 0;

   for(int j=-1;j<=1;++j)
   {
      for(int i=-1;i<=1;++i)
      {
         // skip upper left and lower right corners (not necessary due to assumption about id1.row < id2.row and id1.col < id2.col)
         if (i==j && i!=0)
            continue;

         CellID cellId(p->m_CellID1.Row + j, p->m_CellID1.Col + i);

         ContourMultiMapIt it = pMultiMapContourPoints->lower_bound(cellId);
         while(it != itEnd && it->first.ID == cellId.ID)
         {
            if (PointsConnected(it->second, p))
            {
               itNeighbor = it;
               nNeighborCount++;
            }
            it++;
         }
      }
   }

   return nNeighborCount > 1;
}

// Create the contour point lists 
// 
// First, Search the rows for points which cross the cells
// then search the columns.
//
// We can tell if a point crosses a cell boundary by the 
// following algorithm
//    given: * 0 is always a contour
//           * interval is the user specified contour interval
//           * p1, p2 are the 2 points on the cell edge
//
//    dPoint will be dLat or dLon in fvw,...
//
//    dPoint = abs(E1x-E2x)
//    dElev = abs(E1 - E2)
//    Interval = desired contour interval
//    offset = reset at each pair, is the contour level being discovered
//
//    dPoint   dElev interval Elev1 Elev2 offset
//       1     200      100    310  510     0
//                             210  410   100
//                             110  310   200
//                              10  210   300 ** contour at 300 found
//                             -90  110   400 ** contour at 400 found
//                            -190   10   done
//    general position formula
//       pos = Ex + (((I - Ep)/dElev) * dPoint)
//
// ContourLevel = 0;
// if (p1.elev != p2.elev)
//    e1 = p1.elev
//    e2 = p2.elev
//    while (forever)
//    {
//       if (p1<interval && p2<interval)
//          // no more intersections exist
//          break;
//       // Note: both equal will not exist! Protected above
//       if (p1<=interval)
//       {
//          // An intersection has been found, record the point
//          // By inference, we also know this is the "lower" point
//
//          ContourLevel = e1 - e1%interval;
//       }
//       else // if (p1<=interval)
//       {
//          // An intersection has been found, record the point
//       }
//
//       // Both are greater, only case left decrement both
//       e1 -= interval
//       e2 -= interval
//    }
//

void CContourLists::TraceContours(short *ElevMap, int width, int height, float interval,
                                  double SW_Lat, double SW_Lon, double NE_Lat, double NE_Lon)
{
   // NOTE :: dvl
   // The contour interval is passed on to AddPoint as "int((ContourLevel+interval)*1000)"
   // because we need the contour level as an integer for the key 

   double BottomLat = SW_Lat;
   double LeftLon = SW_Lon;

   // Do not need normalization, always aligned to an int boundary so will not
   // cross the date line, etc,...
   const double LatSteps = (NE_Lat - SW_Lat) / (height-1);
   const double LonSteps = (NE_Lon - SW_Lon) / (width -1);

   // If this assert hits, the likely cause is that somethig has caused the contours to be 
   // regenerated without causing the data_cache to be re-loaded.  Since the data is not
   // generally needed after the lines are generated, the actual elevation map is freed.
   // If the contours need to be regenerated, force the data to be loaded again by
   // emptying the data_cache
   ASSERT (ElevMap);

   short* pElev = ElevMap;
   double Lat, Lon;

   Lat = 0;
   for (int row=0; row<height; row++)
   {
      Lon = 0;
      for (int col=0; col<width; col++)
      {
         const int pOffset = row*width + col;
         const float e1 = (float)(*(pElev + pOffset));

         // Go through the row line segments looking for crossings
         if (col < width-1)
         {
            const float e2 = (float)(*(pElev + pOffset + 1)); // next pixel
            if (e1 != e2)
            {
               const float fDeltaElev = fabs(e1 - e2);
               const float fStart = ceil(min(e1, e2) / interval) * interval;
               const float fEnd = floor(max(e1, e2) / interval) * interval;
               for(float fCurrentInterval = fStart; fCurrentInterval <= fEnd; fCurrentInterval += interval)
               {
                  const float pos = col + fabs(e1 - fCurrentInterval) / fDeltaElev;
                  AddPoint(row-1, col+1, row, col+1, BottomLat + Lat, (LeftLon + LonSteps*pos), int(fCurrentInterval*1000));
               }
            }
         }

         // Look across the column line segments looking for crossings
         if (row < height-1)
         {
            const float e2 = (float)(*(pElev + pOffset + width)); // next row
            if (e1 != e2)
            {
               const float fDeltaElev = fabs(e1 - e2);
               const float fStart = ceil(min(e1, e2) / interval) * interval;
               const float fEnd = floor(max(e1, e2) / interval) * interval;
               for(float fCurrentInterval = fStart; fCurrentInterval <= fEnd; fCurrentInterval += interval)
               {
                  const float pos = row + fabs(e1 - fCurrentInterval) / fDeltaElev;
                  AddPoint(row, col, row, col+1, (BottomLat + LatSteps*pos), LeftLon + Lon, int(fCurrentInterval*1000));
               }
            }
         }

         Lon += LonSteps;
      }

      Lat += LatSteps;
   }

   SortPoints();
}

bool CContourLists::TraceClearanceContours(short *ElevMap, int width, int height, float WarnAlt, float CautionAlt, float OKAlt,
                                  double SW_Lat, double SW_Lon, double NE_Lat, double NE_Lon)
{

   // Note :: dvl 
   // Algorithm is very similar to above, but we are looking for specific contours and not for
   // general contour intervals.

   // NOTE :: dvl
   // The contour interval is passed on to AddPoint as "int((altN)*1000)"
   // because we need the contour level as an integer for the key 

   //g_width = width;

   // Just in case this is a re-generation of the tile, kill any existing lists.
   ClearLists();

   //cell_buffer[0] = new CContourPoint *[width + 1];
   //cell_buffer[1] = new CContourPoint *[width + 1];
   //ZeroMemory(cell_buffer[0], sizeof(CContourPoint *) * (width + 1));
   //ZeroMemory(cell_buffer[1], sizeof(CContourPoint *) * (width + 1));

   //current_buffer = 0;
   //last_buffer = 1;

   bool TraceWarnAlt = WarnAlt > -99999.0;
   bool TraceCautionAlt = CautionAlt > -99999.0;
   bool TraceOKAlt = OKAlt > -99999.0;

   if (!(TraceWarnAlt || TraceCautionAlt || TraceOKAlt))
      // Nothing to do,...
      return true;

   double BottomLat = SW_Lat;
   double LeftLon = SW_Lon;

   // Do not need normalization, always aligned to an int boundary so will not
   // cross the date line, etc,...
   double LatSteps = (NE_Lat - SW_Lat) / (height-1);
   double LonSteps = (NE_Lon - SW_Lon) / (width -1);

   CContourPoint  Pt;

   // If this assert hits, the likely cause is that somethig has caused the contours to be 
   // regenerated without causing the data_cache to be re-loaded.
   ASSERT (ElevMap);

   short* pElev = ElevMap;
   int dPoint = 1;
   double Lat, Lon;

   Lat = 0;
   for (int row=0; row<height; row++)
   {
      Lon = 0;
      for (int col=0; col<width; col++)
      {
         // Go through the row line segments looking for crossings
         if (col < width-1)
         {
            int pOffset = row*width + col;
            float e1 = (float)(*(pElev + pOffset));
            float e2 = (float)(*(pElev + pOffset + 1)); // next pixel
            
            if (e1 != e2)
            {
               float dElev = (float)fabs(e1 - e2);
               
               if (!((e1<=WarnAlt && e2<=WarnAlt) || (e1>WarnAlt && e2>WarnAlt)) && TraceWarnAlt)
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= WarnAlt)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = col + (((WarnAlt - e1)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(WarnAlt*1000));
                     
                  }
                  else if (e2 <= WarnAlt)
                  {
                     // An intersection has been found, record the point
                     float pos = col + dPoint - (((WarnAlt - e2)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(WarnAlt*1000));
                  }
               }
               
               if (! ((e1<=CautionAlt && e2<=CautionAlt) || (e1>CautionAlt && e2>CautionAlt)) && TraceCautionAlt)
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= CautionAlt)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = col + (((CautionAlt - e1)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(CautionAlt*1000));
                     
                  }
                  else if (e2 <= CautionAlt)
                  {
                     // An intersection has been found, record the point
                     float pos = col + dPoint - (((CautionAlt - e2)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(CautionAlt*1000));
                  }
               }

               if (! ((e1<=OKAlt && e2<=OKAlt) || (e1>OKAlt && e2>OKAlt)) && TraceOKAlt)
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= OKAlt)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = col + (((OKAlt - e1)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(OKAlt*1000));
                     
                  }
                  else if (e2 <= OKAlt)
                  {
                     // An intersection has been found, record the point
                     float pos = col + dPoint - (((OKAlt - e2)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(OKAlt*1000));
                  }
               }
            }
         }
         
         // Look across the column line segments looking for crossings
         if (row < height-1)
         {
            int pOffset = row*width + col;
            float e1 = (float)(*(pElev + pOffset));
            float e2 = (float)(*(pElev + pOffset + width)); // next row
            
            if (e1 != e2)
            {
               float dElev = (float)fabs(e1 - e2);
               
               if (! ((e1<=WarnAlt && e2<=WarnAlt) || (e1>WarnAlt && e2>WarnAlt)))
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= WarnAlt)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = row + (((WarnAlt - e1)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(WarnAlt*1000));
                  }
                  else if (e2 <= WarnAlt)
                  {
                     // An intersection has been found, record the point
                     float pos = row + dPoint - (((WarnAlt - e2)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(WarnAlt*1000));
                  }
               }

               if (! ((e1<=CautionAlt && e2<=CautionAlt) || (e1>CautionAlt && e2>CautionAlt)))
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= CautionAlt)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = row + (((CautionAlt - e1)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(CautionAlt*1000));
                  }
                  else if (e2 <= CautionAlt)
                  {
                     // An intersection has been found, record the point
                     float pos = row + dPoint - (((CautionAlt - e2)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(CautionAlt*1000));
                  }
               }

               if (! ((e1<=OKAlt && e2<=OKAlt) || (e1>OKAlt && e2>OKAlt)))
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= OKAlt)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = row + (((OKAlt - e1)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(OKAlt*1000));
                  }
                  else if (e2 <= OKAlt)
                  {
                     // An intersection has been found, record the point
                     float pos = row + dPoint - (((OKAlt - e2)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(OKAlt*1000));
                  }
               }
            }
         }

         Lon += LonSteps;
      }

     //last_buffer = current_buffer;
     //current_buffer = (++current_buffer) % 2;
     //ZeroMemory(cell_buffer[current_buffer], sizeof(CContourPoint *) * (width + 1));

     Lat += LatSteps;

      if (FVW_is_draw_interrupted())
         return false;
   }


   // The points for each contour line are separated into lists according to 
   // the contour level they belong to
   //delete cell_buffer[0];
   //delete cell_buffer[1];

   SortPoints();

   return true;
}

void CContourLists::TraceSingleContour(short *ElevMap, int width, int height, float alt1,
                                  double SW_Lat, double SW_Lon, double NE_Lat, double NE_Lon)
{

   // Note :: dvl 
   // Algorithm is very similar to above, but we are looking for specific contours and not for
   // general contour intervals.

   // NOTE :: dvl
   // The contour interval is passed on to AddPoint as "int((altN)*1000)"
   // because we need the contour level as an integer for the key 

   double BottomLat = SW_Lat;
   double LeftLon = SW_Lon;

   // Do not need normalization, always aligned to an int boundary so will not
   // cross the date line, etc,...
   double LatSteps = (NE_Lat - SW_Lat) / (height-1);
   double LonSteps = (NE_Lon - SW_Lon) / (width -1);

   CContourPoint  Pt;

   // If this assert hits, the likely cause is that somethig has caused the contours to be 
   // regenerated without causing the data_cache to be re-loaded.  Since the data is not
   // generally needed after the lines are generated, the actual elevation map is freed.
   // If the contours need to be regenerated, force the data to be loaded again by
   // emptying the data_cache
   ASSERT (ElevMap);

   short* pElev = ElevMap;
   int dPoint = 1;
   double Lat, Lon;

   Lat = 0;
   for (int row=0; row<height; row++)
   {
      Lon = 0;
      for (int col=0; col<width; col++)
      {
         // Go through the row line segments looking for crossings
         if (col < width-1)
         {
            int pOffset = row*width + col;
            float e1 = (float)(*(pElev + pOffset));
            float e2 = (float)(*(pElev + pOffset + 1)); // next pixel
            
            if (e1 != e2)
            {
               float dElev = (float)fabs(e1 - e2);
               
               if (! ((e1<alt1 && e2<alt1) || (e1>alt1 && e2>alt1)))
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= alt1)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = col + (((alt1 - e1)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(alt1*1000));
                     
                  }
                  else if (e2 <= alt1)
                  {
                     // An intersection has been found, record the point
                     float pos = col + dPoint - (((alt1 - e2)/dElev) * dPoint);
                     AddPoint(row-1, col+1, row, col+1,
                        BottomLat + Lat, (LeftLon + LonSteps*pos), int(alt1*1000));
                  }
               }
            }
         }
         
         // Look across the column line segments looking for crossings
         if (row < height-1)
         {
            int pOffset = row*width + col;
            float e1 = (float)(*(pElev + pOffset));
            float e2 = (float)(*(pElev + pOffset + width)); // next row
            
            if (e1 != e2)
            {
               float dElev = (float)fabs(e1 - e2);
               
               if (! ((e1<alt1 && e2<alt1) || (e1>alt1 && e2>alt1)))
               {
                  // Note: both equal will not exist! Protected above
                  if (e1 <= alt1)
                  {
                     // An intersection has been found, record the point
                     // By inference, we also know this is the "lower" point
                     
                     float pos = row + (((alt1 - e1)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(alt1*1000));
                  }
                  else if (e2 <= alt1)
                  {
                     // An intersection has been found, record the point
                     float pos = row + dPoint - (((alt1 - e2)/dElev) * dPoint);
                     AddPoint(row, col, row, col+1,
                        (BottomLat + LatSteps*pos), LeftLon + Lon, int(alt1*1000));
                  }
               }
            }
         }

         Lon += LonSteps;
      }

      Lat += LatSteps;
   }


   // The points for each contour line are separated into lists according to 
   // the contour level they belong to

   SortPoints();
}


void CContourLists::ClearLists()
{
   // free up multimaps - this can be moved to SortPoints once we are doen with the multimaps
   int nContourLevel;
   ContourMultiMap *pMultiMapContourPoints;
   POSITION pos = m_contourMultiMaps.GetStartPosition();
   while (pos)
   {
      m_contourMultiMaps.GetNextAssoc(pos, nContourLevel, pMultiMapContourPoints);
      delete pMultiMapContourPoints;
   }
   m_contourMultiMaps.RemoveAll();

   // free up CContourPoint memory in m_Lines
   const int size = m_Lines.GetSize();
   for (int ct=0; ct<size; ct++)
   {
      CContourList* pList = m_Lines[ct];
      int key = m_Levels[ct];

      while (!pList->IsEmpty())
         delete pList->RemoveHead();

      delete pList;
   }

   m_Lines.RemoveAll();
   m_Levels.RemoveAll();
}
