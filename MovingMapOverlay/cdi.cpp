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



/*
   Filename :  CourseIndicator.cpp
   Date     :  1998-SEP-15
   Author   :  Ray E. Bornert II
   Purpose  :  Course Deviation Indicator Class implmentation
*/

//**************************************************************
// INCLUDES
//**************************************************************
#include "stdafx.h"
#include "wm_user.h"

#include "cdi.h"
#include "gps.h"
#include "param.h"
#include "err.h"
#include "unit.h"
#include "..\overlay\OverlayCOM.h"
#include "ovl_mgr.h"

/////////////////////////////////////////////////////////////////////////////
// CCourseIndicator

IMPLEMENT_DYNCREATE(CCourseIndicator, CWnd)

void CCourseIndicator::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CWnd::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CCourseIndicator, CWnd)
	//{{AFX_MSG_MAP(CCourseIndicator)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
   ON_WM_DESTROY()
   ON_WM_CLOSE()
	ON_COMMAND(ID_CDI_CENTER_COURSE, OnCdiCenterCourse)
	ON_UPDATE_COMMAND_UI(ID_CDI_CENTER_COURSE, OnUpdateCdiCenterCourse)
	ON_COMMAND(ID_CDI_CENTER_SHIP, OnCdiCenterShip)
	ON_UPDATE_COMMAND_UI(ID_CDI_CENTER_SHIP, OnUpdateCdiCenterShip)
	ON_COMMAND(ID_CDI_UNITS_FEET, OnCdiUnitsFeet)
	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_FEET, OnUpdateCdiUnitsFeet)
	ON_COMMAND(ID_CDI_UNITS_KILOMETERS, OnCdiUnitsKilometers)
	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_KILOMETERS, OnUpdateCdiUnitsKilometers)
	ON_COMMAND(ID_CDI_UNITS_METERS, OnCdiUnitsMeters)
	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_METERS, OnUpdateCdiUnitsMeters)
	ON_COMMAND(ID_CDI_UNITS_NAUTICAL_MILES, OnCdiUnitsNauticalMiles)
	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_NAUTICAL_MILES, OnUpdateCdiUnitsNauticalMiles)
	ON_COMMAND(ID_CDI_UNITS_STATUTE_MILES, OnCdiUnitsStatuteMiles)
	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_STATUTE_MILES, OnUpdateCdiUnitsStatuteMiles)
	ON_COMMAND(ID_CDI_UNITS_YARDS, OnCdiUnitsYards)
	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_YARDS, OnUpdateCdiUnitsYards)
   ON_MESSAGE(WM_USER_UPDATE, Update )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	ON_COMMAND(ID_CDI_UNITS_YARDS, OnCdiUnitsYards)
//	ON_UPDATE_COMMAND_UI(ID_CDI_UNITS_YARDS, OnUpdateCdiUnitsYards)


BEGIN_DISPATCH_MAP(CCourseIndicator, CWnd)
	//{{AFX_DISPATCH_MAP(CCourseIndicator)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICourseIndicator to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {B4BC16A6-4C12-11D2-816E-00609733A64E}
static const IID IID_ICourseIndicator =
{ 0xb4bc16a6, 0x4c12, 0x11d2, { 0x81, 0x6e, 0x0, 0x60, 0x97, 0x33, 0xa6, 0x4e } };

BEGIN_INTERFACE_MAP(CCourseIndicator, CWnd)
	INTERFACE_PART(CCourseIndicator, IID_ICourseIndicator, Dispatch)
END_INTERFACE_MAP()

// {B4BC16A7-4C12-11D2-816E-00609733A64E}
IMPLEMENT_OLECREATE(CCourseIndicator, "gps.CourseIndicator", 0xb4bc16a7, 0x4c12, 0x11d2, 0x81, 0x6e, 0x0, 0x60, 0x97, 0x33, 0xa6, 0x4e)

//**************************************************************
// COURSE INDICATOR
//**************************************************************

long CCourseIndicator::m_centerMode='SHIP';
long CCourseIndicator::m_units='NM';

CCourseIndicator::CCourseIndicator() : m_endingTpId(0)
{
#if _DEBUG
#define VERIFYMETHOD (0)
#if VERIFYMETHOD
   VerifyMethod();
#endif
#endif

	m_deviation = new CUnitConverter;

   //on course
   *m_deviation = 0;
   m_pRoute=NULL;

   //load the popup menu
   VERIFY(m_menu.LoadMenu(IDR_CDI_POPUP));

   //gotta tell windows about the class
   CString className = AfxRegisterWndClass
   (
      CS_BYTEALIGNWINDOW,
		0,
      0,
      0
   );

   //how large do we want the client to be
   CRect pos(0,0,320,40);
   CRect wrect = pos;

   CWnd *pParentWnd = AfxGetApp()->m_pMainWnd;
   //create the windows window
   BOOL bResult = CreateEx
   (
      WS_EX_TOOLWINDOW,             //DWORD dwExStyle,
      className,                    //LPCTSTR lpszClassName,
      "Course Deviation Indicator", //LPCTSTR lpszWindowName,
      WS_SYSMENU|WS_POPUP|WS_CAPTION,//DWORD dwStyle,
      wrect,                        //const RECT& rect,
      pParentWnd,                   //CWnd* pParentWnd,
      0,                            //UINT nID,
      NULL                          //LPVOID lpParam = NULL
   );

   //what are the current sizes
   CRect crect;
   GetClientRect(crect);
   CSize csize = crect.Size();
   GetWindowRect(wrect);
   CSize wsize = wrect.Size();

   //what are the deltas
   CSize clientSizeWanted = pos.Size();
   int dx = clientSizeWanted.cx - csize.cx;
   int dy = clientSizeWanted.cy - csize.cy;

   //position the cdi near the top of the client
   CRect prect;
   pParentWnd->GetWindowRect(&prect);
   CSize psize = prect.Size();
   int px = prect.left + (psize.cx-wsize.cx)/2;
   int py = prect.top  + (psize.cy-wsize.cy)/4;

   //change the size
   bResult = SetWindowPos
   (
      &wndBottom,//Most,      //const CWnd* pWndInsertAfter,
      px,py,                  //int x, int y,
      wsize.cx+dx,wsize.cy+dy,//int cx, int cy,
      SWP_NOACTIVATE          //UINT nFlags
      | SWP_NOOWNERZORDER
   );

   //it better be the size we want
   GetClientRect(crect);
   ASSERT( crect == pos );

   //display it
   if (C_gps_trail::m_Tcdi)
      ShowWindow(SW_SHOW);

   //make sure it gets painted
   UpdateWindow();

   //load registry settings
   m_units = PRM_get_registry_int("GPS Options", "CDI Units", m_units);
   m_centerMode = PRM_get_registry_int("GPS Options", "CDI Center Mode", m_centerMode);
}

CCourseIndicator::~CCourseIndicator()
{
   //save registry settings
   PRM_set_registry_int("GPS Options", "CDI Units", m_units);
   PRM_set_registry_int("GPS Options", "CDI Center Mode", m_centerMode);

	delete m_deviation;

   DestroyWindow();
}


void CCourseIndicator::SetDeviation(double d)
{
   m_deviation->SetNM(d);
   invalidate(false);
}

double CCourseIndicator::GetDeviation(CString &s)
{
   switch (m_units)
   {
   default:
   case 'NM': s="NM"; return m_deviation->GetNM(); break;
   case 'MI': s="MI"; return m_deviation->GetMiles(); break;
   case 'KM': s="KM"; return m_deviation->GetMeters()/1000; break;
   case 'ME': s="M "; return m_deviation->GetMeters(); break;
   case 'YD': s="YD"; return m_deviation->GetYards(); break;
   case 'FT': s="FT"; return m_deviation->GetFeet(); break;
   };
}

long CCourseIndicator::SetUnits(long unit)
{
   m_units = unit;
   invalidate(false);
   return m_units;
}

long CCourseIndicator::SetCenterMode(long mode)
{
   m_centerMode = mode;
   invalidate(false);
   return m_centerMode;
}

long CCourseIndicator::GetCenterMode()
{
   return m_centerMode;
}

void CCourseIndicator::OnPaint( void )
{
   CPaintDC pdc(this);
   OnDraw( &pdc );
}

void CCourseIndicator::OnClose(void)
{
//   CWnd::OnClose();
   C_gps_trail::m_Tcdi = FALSE;
   ShowWindow(SW_HIDE);
}

void CCourseIndicator::OnDestroy(void)
{
   CWnd::OnDestroy();
}

void CCourseIndicator::OnDraw(CDC *pDC)
{
   drawDeviation(pDC);
}

void Commatize( CString &numericStr )
{
   int strLength = numericStr.GetLength();
   int decimalPlace = numericStr.Find('.');
   int numDigits=strLength;
   if (decimalPlace >= 0)
      numDigits -= (strLength-decimalPlace);

   int numCommas = (numDigits-1)/3;
   if (numCommas)
   {
      int i;
      CString temp=numericStr;
      for (i=0;i<numCommas;i++)
         numericStr+=" ";
      int numLeading= (numDigits-3*numCommas);
      for (i=0;i<numLeading;i++)
         numericStr.SetAt(i,temp[i]);
      int j=i;
      for (int c=numCommas; c; c--)
      {
         numericStr.SetAt(j++,',');
         for (int k=0;k<3; k++)
            numericStr.SetAt(j++,temp[i++]);
      }
      for (;i<strLength;)
         numericStr.SetAt(j++,temp[i++]);
   }
}

void CCourseIndicator::drawDeviation(CDC *pDC)
{
   UtilDraw util(pDC);

   // get the client
   CRect pos;
   GetClientRect(&pos);

   // background
   CBrush back(RGB(0,0,0));
   pDC->FillRect(&pos,&back);
   back.DeleteObject();

   // measurements based on rect size
   int width = pos.Width();
   int height= pos.Height();
   int unit = width/20;
   CPoint center = pos.CenterPoint();

   CString unitStr;
   double deviation=GetDeviation(unitStr);

   // where is the deviation
   double nm_unit = 0.1;
	if (deviation != 0.0)
	{
		nm_unit = pow(10,floor(log10(fabs(deviation))));
		if (nm_unit < 0.1)
			nm_unit = 0.1;
	}

   CPoint devpntShip = center+CPoint((int)( deviation*unit/nm_unit),0);
   CPoint devpntCour = center+CPoint((int)(-deviation*unit/nm_unit),0);

   CPoint coursePnt= (m_centerMode=='COUR'?center:devpntCour);
   CPoint shipPnt  = (m_centerMode=='SHIP'?center:devpntShip);

   // draw course line
   util.set_pen(RGB(0,255,0), UTIL_LINE_SOLID, 2);
   util.draw_line(coursePnt.x, coursePnt.y-height/2, coursePnt.x, coursePnt.y+height/2);

   // draw the plane based on the current type
   util.set_pen(RGB(0,255,255), UTIL_LINE_SOLID, 2);

   // airlift
   {
      // fuselage
      util.draw_line(shipPnt.x, shipPnt.y-height/2, shipPnt.x, shipPnt.y+height/2);
      
      // tail
      util.draw_line(max(pos.left ,shipPnt.x-8),shipPnt.y+height/2-1,
         min(pos.right,shipPnt.x+7),shipPnt.y+height/2-1);

      // wing
      util.draw_line(max(pos.left ,shipPnt.x-20),
         shipPnt.y-3,min(pos.right,shipPnt.x+19),shipPnt.y-3);
   }
   
   // a brush
   CBrush gray(RGB(128,128,128));
   void *prevBrush = pDC->SelectObject(gray);

   // setup the 2 rects for drawing digits and dots
   CRect rect(center,center);
   rect.OffsetRect(-9*unit,0);
   CRect dot=rect;
   rect.InflateRect(8,8);
   dot.InflateRect(1,1);
   rect.OffsetRect(0,-height/4);

   pDC->SetBkMode(TRANSPARENT);
   pDC->SetTextColor(RGB(128,128,128));
   //draw the dots and the digits
   for (int i=-9;i<=9;i++)
   {
      pDC->FillRect(&dot,&gray);

      //now make a text string with the first 2 degree digits
      CString digtxt;
      digtxt.Format("%1d",labs(i)%10);

      //make it so
      if (i)
      pDC->DrawText(digtxt,rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

      //move the rectangles over
      rect.OffsetRect(unit,0);
      dot.OffsetRect(unit,0);
   }

   //the bottom half rectangle
   rect = pos;
   rect.DeflateRect(0,height/4);
   rect.OffsetRect(0,height/4);

   //get the rects that are left and right of center line
   CRect leftRect=rect;
   CRect rightRect=rect;
   leftRect.right=center.x;
   rightRect.left=center.x;
   CString leftText;
   CString rightText;

   //the side word left/right
   CString sideText;
   if (deviation < 0)
      sideText = "Left";
   else
   if (deviation > 0)
      sideText = "Right";
   else
      sideText = "";

   //the deviation number
   CString devText;
   devText.Format("%1.2f",fabs(deviation));
   Commatize(devText);

   //how is the text oriented
   double devOrient;
   if (m_centerMode == 'COUR')
      devOrient =  deviation;
   else
      devOrient = -deviation;
 
   //setup the explanation text
   CString explanation;
   if (devOrient>0) //RIGHT
   {
      rightText = " " + sideText;
      leftText = devText + " ";
   }
   else
   if (devOrient<0) //LEFT
   {
      leftText = sideText + " ";
      rightText = " " + devText;
   }
   else           //CENTER
   {
      leftText="On ";
      rightText=" Course";
   }

   //choose the font we want
   CFont font;
   font.CreatePointFont(100,"arial",pDC);
   void *prevFont = pDC->SelectObject(font);

   //choose the text color based on deviation
   if (fabs(deviation)<1)
      pDC->SetTextColor(RGB(0,255,0)); //green
   else
      pDC->SetTextColor(RGB(255,0,0)); //red

   //write the deviation
   pDC->DrawText(leftText,leftRect,DT_SINGLELINE|DT_VCENTER|DT_RIGHT);
   pDC->DrawText(rightText,rightRect,DT_SINGLELINE|DT_VCENTER|DT_LEFT);

   //put commas in numeric strings with 4 or more digits
   CString numericStr;
   numericStr.Format("%1.0f",nm_unit*10);
   Commatize(numericStr);

   //write the scale units
   CString scaleTxt;
   scaleTxt.Format("%s%s",numericStr,unitStr);
   pDC->DrawText(scaleTxt,rect,DT_SINGLELINE|DT_VCENTER|DT_LEFT);
   pDC->DrawText(scaleTxt,rect,DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

   //put everything back the way it was
   pDC->SelectObject(prevBrush);
   pDC->SelectObject(prevFont);
}

void CCourseIndicator::invalidate(bool erase_background)
{
   Invalidate(erase_background);
}

Vector CCourseIndicator::calc_leg_deviation(Vector point0, Vector point1, Vector position )
{
   //return value
   Vector vResult;

   //for now this is a 2D calculation
   //maybe we'll do this for the shuttle someday and this line can be deleted
   point0.z = point1.z = position.z = 0;

   //what is the route vector from starting point to ending point
   Vector vRoute = point1-point0;

   //remember the length of this leg
   vResult.z = vRoute.magnitude();  //***********

   //make this a unit vector
   //this will be used to determine a progress point along the route leg
   vRoute.normalize();

   //what is the vector from the starting point to current position
   Vector vPosition = position-point0;

   //FORWARD PROGRESS
   //project the position vector onto the route
   //this will tell us how far along the route we've progressed
   //positive value means we are somewhere beyond the starting point
   //negative value means we are somewhare behind the starting point
   //zero is dead on start
   //calc the forward progress using dot product
   vResult.y = vPosition ^ vRoute;  //***********

   //LEFT/RIGHT DEVIATION
   //assuming the z-altitude components are all equal or very near equal,
   //the current position is either right or left of the course line
   //the z component will determine distance and direction left or right of course
   //positive is right of course
   //negative is left  of course
   //zero is dead on path
   //calc the left right delta using cross product
   Vector vLeftRight = vPosition % vRoute;

   //what is the leftRight deviation
   vResult.x = vLeftRight.z;        //***********
 
   //return result
   return vResult;
}

void CCourseIndicator::UpdateWindowText(const CString& startingTpLabel, const CString& endingTpLabel)
{
	
   CString text("Course Deviation Indicator ");
	
   //we have to have a route
   if (m_pRoute)
   {
      text += OVL_get_overlay_manager()->GetOverlayDisplayName(m_pRoute);
      text += " (" + startingTpLabel + "-" + endingTpLabel + ")";
   }
   else
      text += " (no route)";
	
   SetWindowText(text);
}

//called from addPoint
LRESULT CCourseIndicator::Update(WPARAM wParam, LPARAM position)
{
   COverlayCOM *pRoute = reinterpret_cast<COverlayCOM *>(wParam);

   //remember the route
   m_pRoute = pRoute;

   //we have to have a route
   if (!pRoute)
      return 0;

   //our geo point
   d_geo_t curr_pos;
   curr_pos.lat = ((GPSPointIcon *)position)->get_latitude();
   curr_pos.lon = ((GPSPointIcon *)position)->get_longitude();

   try
   {
      IRouteOverlayPtr spRouteOverlay = pRoute->GetFvOverlay();
      if (spRouteOverlay != NULL)
      {
         double legDistanceMinMeters;

         double closestLat, closestLon, altitudeMeters, heading;
         long startingTpId, endingTpId;

         const long routeIndex = spRouteOverlay->GetRouteInFocus();

         if (spRouteOverlay->ClosestPointAlongRoute(routeIndex,
            curr_pos.lat, curr_pos.lon, &legDistanceMinMeters, &closestLat, &closestLon,
            &altitudeMeters, &heading, &startingTpId, &endingTpId) == S_OK)
         {
            SetDeviation( CUnitConverter(legDistanceMinMeters).GetNM() );
         
            if (endingTpId != m_endingTpId)
            {
               spRouteOverlay->SetTurnpointInFocus(routeIndex, endingTpId);
               m_endingTpId = endingTpId;
            }

            //make the window text informative
            UpdateWindowText((char *)spRouteOverlay->GetTurnpointLabel(routeIndex, startingTpId),
               (char *)spRouteOverlay->GetTurnpointLabel(routeIndex, endingTpId));

            m_bHeadingTypeIsGreatCircle = spRouteOverlay->HeadingTypeIsGreatCircle;
         }
         else
            SetDeviation( 0 );
      }
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("CCourseIndicator::Update failed - %s", (char *)e.Description());
      ERR_report(msg);
   }

   return 0;
}

#define CDI_METHOD calc_distance_from_leg_exact

//convert lat lon info to vectors
void CCourseIndicator::geo_to_vec(double latP0, double lonP0, double latP1, 
											 double lonP1, double latShip, double lonShip,
											 Vector& vp0, Vector& vp1, Vector& vsh)
{
   //the starting point of the route leg will be the origin for our frame of reference
   vp0 = Vector(0,0,0);

   //the end point is on the positive x axis
   double dist0;
   double angle0;
   calc_range_and_bearing(latP0,lonP0,latP1,lonP1,&dist0,&angle0);
   vp1 = Vector((float)dist0,0,0);

   //the plane position
   double dist1;
   double angle1;
   calc_range_and_bearing(latP0,lonP0,latShip,lonShip,&dist1,&angle1);
   double delta = -DEG_TO_RAD(angle1-angle0);
   vsh = Vector((float)(dist1*cos(delta)),(float)(dist1*sin(delta)),0);
}

//determine the minimum distance from any point on the leg line
double CCourseIndicator::calc_distance_from_leg_vector
(
   double latP0  , double lonP0  ,
   double latP1  , double lonP1  ,
   double latShip, double lonShip
)
{
   //a vector for each point
   Vector point0;
   Vector point1;
   Vector position;

   //convert the lat lon points to vectors
   geo_to_vec
   (
      latP0  , lonP0  ,
      latP1  , lonP1  ,
      latShip, lonShip,
      point0, point1, position
   );
   
   //do we have a zero length leg
   if (point0 == point1)
   {
      double gcd,angle;
      calc_range_and_bearing(latP0,lonP0, latShip, lonShip, &gcd, &angle );
      return gcd;
   };

   //calculate the deviation
   Vector vResult = calc_leg_deviation( point0, point1, position );

   //find the minimum distance to any part of the route leg
   if (vResult.y <= 0)
   {
      //deviation is a straight line to the starting leg point
      double gcd,angle;
      calc_range_and_bearing(latP0,lonP0, latShip, lonShip, &gcd, &angle );
      if (vResult.x)
         //preserve the sign so we know which side of the leg line we're on
         vResult.x = (float)gcd * (vResult.x / (float)fabs(vResult.x));
      else
         vResult.x = (float)gcd;
   }
   else
   if (vResult.y >= vResult.z)
   {
      //deviation is a straight line to the ending leg point
      double gcd,angle;
      calc_range_and_bearing(latP1,lonP1, latShip, lonShip, &gcd, &angle );
      if (vResult.x)
         //preserve the sign so we know which side of the leg line we're on
         vResult.x = (float)gcd * (vResult.x / (float)fabs(vResult.x));
      else
         vResult.x = (float)gcd;
   }

   // return result
   return vResult.x;
}

#if _DEBUG
void CCourseIndicator::VerifyMethod()
{
/*
   COURSE DEVIATION - METHOD VERIFICATION
   Consider a route leg:
      - of any length (say 4000 nautical miles)
      - coincident with the equator (latitude for each point is 0 degrees)
      - crosses the prime meridian at some point (lat/lon = 0/0)
   Consider a ship position:
      - in the northern hemisphere (say latitude 30N)
      - coincident with the prime meridian (longitude = 0 degrees)
      - flying east (ship is left of route leg)
   Course deviation is the shortest distance
   from the ship to any point on the leg.
   This line from the ship to the leg will be perpindicular to the leg:

                        > (plane - 30N 0E)
                       /|
                     /  |
                r1 / )a |              r2
      -----------*------+---------------*--- (equator - latitude=0)
                        |
                     (longitude=0)

   Since we have chosen our scenerio carefully,
   it is easy to determine the course deviation for the ship.
   It is the length of the great circle segment from
      0N 0E to 30N 0E
   Which is 30 degrees * 60 seconds = 1800 Nautical Miles.

   However, in practice it will not be this easy.
   We will only have the lat/lon for r1,r2 and the ship.
   From this information alone we must calculate a deviation.

   Is there a relationship between this perpindicular distance
   and the heading of the plane as measured from some point on the leg

   We should be able to use the sine function appropiately.
   By selecting the largest of 4 deviation angles we can minimize error
   due to inordinate route leg distance.

   Course deviation is equal to:
      pick some point - M - along the route leg.
      the line from this point to the ship is MS
      the deviation angle is the angle formed by MS and r1r2 
   The sine of the deviation angle multiplied times the length of MS
   should yield a sufficiently accurate course deviation

*/

   double r0Lat = 0;
   double r0Lon = -60; //run this from 0 to -90
   double r1Lat = 0;
   double r1Lon = 60;
   double sLat = 30; //run this from 0 to 90
   double sLon = 0;

   double alpha,beta,legDist;
   double maxErr=0,_alpha,_beta;
   double maxErrPct=0;

   //an easy way to get pi
   double pi = acos(-1.0);

#define COMPLETE_VERIFY (0)
#if COMPLETE_VERIFY

#define START_LEGD (0)
#define STOP_LEGD (180)
#define STEP_LEGD (15)     //use even multiple of stop to avoid infinite loop

#define START_ALPHA  (-180)
#define STOP_ALPHA  (180)
#define STEP_ALPHA   (15)  //use even multiple of stop to avoid infinite loop

#define START_BETA    (-90)
#define STOP_BETA   ( 90)
#define STEP_BETA     (15) //use even multiple of stop to avoid infinite loop

#else
//15 degrees is 900 NM
//this should cover 90% of all the cases
//that is route legs <= 900NM , course deviation <= 900NM
#define START_LEGD (1)
#define STOP_LEGD (15)
#define STEP_LEGD (1)      //use even multiple of stop to avoid infinite loop

#define START_ALPHA  (0)
#define STOP_ALPHA  (-15)
#define STEP_ALPHA   (-1)  //use even multiple of stop to avoid infinite loop

#define START_BETA    (-5)
#define STOP_BETA   ( 5)
#define STEP_BETA     (1)  //use even multiple of stop to avoid infinite loop

#endif
/*
   CDIdata.txt contains tables showing the CDI err for the current method
   Each table title shows the equatorial route leg length in degrees longitude
   Table row labels represent the route leg starting point in degrees longitude
   Table column labels represent the ship position in degrees latitude 
   Table elements represent the percent error from the precise deviation
   NOTE:
   The ships longitude in each case is 0 degrees
   The route latitude in each case is 0 degrees
*/
   FILE *fout = NULL;
   fopen_s(&fout, "CDIdata.txt","w");
   ASSERT(fout);

   int larger=0;
   int smaller=0;
   int same=0;
   double maxSmallErr=0;
   
   for (legDist = START_LEGD; ; legDist += STEP_LEGD)
   {
//      if (legDist >= 180)
//       legDist = 179;
      fprintf(fout,"Route Leg Length in degrees longitude = %5.0f\n",legDist);
      fprintf(fout,"\n");
      //table column header
      fprintf(fout,"     ");
      for (beta  = START_BETA; ; beta  += STEP_BETA)
      {
         fprintf(fout," %5.0f",beta);
         if (beta==STOP_BETA)
            break;
      }
      fprintf(fout,"\n");

      for (alpha = START_ALPHA; ; alpha += STEP_ALPHA)
      {
         r0Lon = alpha;
         r1Lon = alpha + legDist;
         if (r1Lon > 180)
            r1Lon -= 360;

         fprintf(fout,"%4.0f: ",alpha);
         for (beta  = START_BETA; ; beta  += STEP_BETA)
         {
/*
            //////////////////
            if (r0Lon >   180)
               r0Lon -= 360;
            else
            if (r0Lon <= -180)
               r0Lon += 360;

            if (r1Lon >   180)
               r1Lon -= 360;
            else
            if (r1Lon <= -180)
               r1Lon += 360;
            //////////////////
*/
            sLat = beta;

            double gcdDeviation;
            double gcdR0S;
            double gcdR1S;
            double gcdMS;
            double angleR0S;
            double angleR1S;
            double angleMS;
            double sign;

            calc_range_and_bearing(r0Lat,r0Lon, sLat, sLon, &gcdR0S, &angleR0S );
            calc_range_and_bearing(r1Lat,r1Lon, sLat, sLon, &gcdR1S, &angleR1S );
            calc_range_and_bearing(0,0, sLat, sLon, &gcdMS, &angleMS );

            if (sLat > 0)
               sign = -1;
            else
               sign =  1;

            //is the route length zero
            if (r0Lon == r1Lon)
               gcdDeviation = fabs(gcdR0S);
            else
            //are we in the leg
            if (r0Lon <= 0 && r1Lon >=0)
            {
               //are we on course
               if (sLat == 0)
                  gcdDeviation = 0;
               else
                  gcdDeviation = sign * gcdMS;
            }
            else
            if (r0Lon > 0 && r1Lon < 0)
            {
               if (gcdR0S < gcdR1S)
                  gcdDeviation = sign*gcdR0S;
               else
                  gcdDeviation = sign*gcdR1S;
            }
            else
            //are we in front of the leg
            if (r0Lon > 0)
               gcdDeviation = sign * gcdR0S;
            else
            //are we in back of the leg
            if (r1Lon < 0)
               gcdDeviation = sign * gcdR1S;
            else
               ASSERT(0);

            double cdi;

            cdi = CDI_METHOD
            (
               r0Lat,r0Lon,
               r1Lat,r1Lon,
               sLat,sLon
            );

            if (cdi > gcdDeviation)
               larger++;
            else
            if (cdi < gcdDeviation)
            {
               smaller++;
               double err=fabs(gcdDeviation-cdi);
               if (err > maxSmallErr)
                  maxSmallErr=err;
            }
            else
               same++;

            //cdi should be very close to gcdDeviation
            double err = fabs(cdi-gcdDeviation);
            double errPct;
            if (gcdDeviation)
               errPct = err / gcdDeviation;
            else
               errPct = err;

            if (errPct > maxErrPct)
            {
               maxErr = err;
               maxErrPct = errPct;
               _alpha = alpha;
               _beta = beta;
            }
            fprintf(fout," %5.2f",errPct*100);
            if (beta==STOP_BETA)
               break;
         }
         fprintf(fout,"\n");
         if (alpha==STOP_ALPHA)
            break;
      }
      fprintf(fout,"\n");
      if (legDist==STOP_LEGD)
         break;
   }

   fclose(fout);
   CString msg;
   msg.Format("CDI maxErr=%8.1fNM (%5.2f%c) at %3fr,%3fs",maxErr,maxErrPct*100,'%',_alpha,_beta);
   AfxMessageBox(msg);
}
#endif

double CCourseIndicator::calc_distance_from_leg_trig(double latR1, double lonR1,
																	  double latR2, double lonR2,
																	  double latS, double lonS)
{
   double gcdR1R2,angleR1R2;
   double gcdR2R1,angleR2R1;
   double gcdR1S,angleR1S;
   double gcdR2S,angleR2S;
   bool before,after,onCourse;
   double angleDeviationR1;
   double angleDeviationR2;

   double cd = calc_leg_init(latR1, lonR1, latR2, lonR2, latS, lonS,
		gcdR1R2, angleR1R2, gcdR2R1, angleR2R1, gcdR1S , angleR1S,
      gcdR2S , angleR2S, angleDeviationR1, angleDeviationR2, before, 
		after, onCourse);

   if (before || after || onCourse)
      return cd;

   //ok now we know that the perpindicular great circle from the ship to the leg
   //is not zero length
   //intersects the leg between the endpoints

   double sign;
   double angle;
   double radius;
   double gcdMS;
   double angleMS;
   double latM,lonM;

   //which half of the leg 
   if (angleDeviationR1 > angleDeviationR2)
   {  //first
      sign = 1;
      //should we use angle R1S as the sine angle
      if (gcdR2S > gcdR2R1)
      {  //yes
         radius = gcdR1S;
         angle = angleDeviationR1;
      }
      else
      {  //no
         //find the point on leg that is the same distance from R2 as the ship is from R2
         calc_end_point( latR2,lonR2, gcdR2S, angleR2R1, &latM, &lonM );
         calc_range_and_bearing(latM,lonM, latS, lonS, &gcdMS, &angleMS );
         radius = gcdMS;
         angle = angleMS - angleR1R2;
      }
   }
   else
   if (angleDeviationR2 > angleDeviationR1)
   {  //second
      sign = -1; //we're looking at the deviation from the perspective of R2
      //should we use angle R2S as the sine angle
      if (gcdR1S > gcdR1R2)
      {  //yes
         radius = gcdR2S;
         angle = angleDeviationR2;
      }
      else
      {  //no
         //find the point on leg that is the same distance from R1 as the ship is from R1
         calc_end_point( latR1,lonR1, gcdR1S, angleR1R2, &latM, &lonM );
         calc_range_and_bearing(latM,lonM, latS, lonS, &gcdMS, &angleMS );
         radius = gcdMS;
         angle = angleMS - angleR2R1;
      }
   }
   else
   {  //50 yard line
      //find the center point
      calc_end_point( latR1,lonR1, gcdR1R2/2, angleR1R2, &latM, &lonM );
      calc_range_and_bearing(latM,lonM, latS, lonS, &gcdMS, &angleMS );
      if (angleMS < 90 || angleMS > 270)
         sign = 1;
      else
         sign = -1;
      return sign * gcdMS;
   }

   if (angle < 0)
      angle += 360;
   if (angle > 360)
      angle -= 360;

   //grab a quick value for pi
   double pi = acos(-1.0);

   //convert to radians
   double radians = pi * angle/180;

   //take the sine of the angle
   cd = sign * radius * sin(radians);

   //return the signed course deviation
   return cd;
}

/*
   This routine rocks!
   It yields near perfect results
*/
double CCourseIndicator::calc_distance_from_leg_exact
(
   double latR1  , double lonR1  ,
   double latR2  , double lonR2  ,
   double latS   , double lonS
)
{
   double gcdR1R2,angleR1R2;
   double gcdR2R1,angleR2R1;
   double gcdR1S,angleR1S;
   double gcdR2S,angleR2S;
   bool before,after,onCourse;
   double angleDeviationR1;
   double angleDeviationR2;
   double cd = calc_leg_init
   (
      latR1  , lonR1  ,
      latR2  , lonR2  ,
      latS   , lonS   ,
      gcdR1R2, angleR1R2,
      gcdR2R1, angleR2R1,
      gcdR1S , angleR1S,
      gcdR2S , angleR2S,
      angleDeviationR1,
      angleDeviationR2,
      before , after, onCourse
   );

   if (before || after || onCourse)
      return cd;

   //ok now we know that the perpindicular great circle from the ship to the leg
   //is not zero length
   //intersects the leg between the endpoints

   //grab a quick value for pi
   double pi = acos(-1.0);

   double latM=latR1;
   double lonM=lonR1;
   double gcdM = 0;
   double gcdMS = gcdR1S;
   double interval = gcdR1R2;
   double angleMS = angleR1S;
   double angleMR2 = angleR1R2;
   double gcdMR2 =0;

   int it;

   //keep track of iterations in case there's an infinite loop lurking
   //this shouldn't happen but let's make sure it can't
#define IT_TOLERANCE (8) //we will allow no more than this
   for (it=1;it<=IT_TOLERANCE;it++)
   {
      angleMS -= angleMR2;
      if (angleMS > 180)
         angleMS -= 360;
      if (angleMS <-180)
         angleMS += 360;

      //normalize 
//      double angleErr = fabs(angleMS)-90;
      double angleErr = angleMS;

#define CDI_TOLERANCE (1) //degrees
/*
   !!!WARNING!!!
   Setting the tolerance to zero will pretty much cause an ininite loop.
   The nature of the floating point round err for the sine function
   just will not allow convergence on a unique point.
   As long as we can get a midpoint within 1 degree of the true point,
   we should be just fine.
   A field of 1 degree is surely huge compared to the accuracy of the trig functions,
   and so we should be fine with 1.
   If an infinite loop does occur, then the tolerance should be increased.
*/
//      if (fabs(angleErr) <= CDI_TOLERANCE)
      if (fabs( fabs(angleErr)-90 ) <= CDI_TOLERANCE)
         break;

      //convert degrees to radians
      double radians = pi * angleErr/180;

      //move closer to the true mid point
//      interval = (gcdMS * sin(radians));
      interval = (gcdMS * cos(radians));

      //now we're closer
      gcdM += interval;

      //now get the mid point
      calc_end_point( latR1,lonR1, gcdM, angleR1R2, &latM, &lonM );

      //now get the distance to the ship
      calc_range_and_bearing(latM,lonM, latS, lonS, &gcdMS, &angleMS );

      //now get the new heading to R2
      calc_range_and_bearing(latM,lonM, latR2, lonR2, &gcdMR2, &angleMR2 );
   }

//let's remember the maximum number of iterations during a run
static int maxIt=1;
   if (it > maxIt)
   {
#if _DEBUG
      maxIt=it;
      CString msg;
      msg.Format("Required CDI iterations has risen to %d",maxIt);
      AfxMessageBox(msg);
//      ASSERT(maxIt<IT_TOLERANCE);
#endif
   }

   //which side of the leg are we on
   double sign = angleMS / fabs(angleMS);

   //set the sign
   cd = sign * gcdMS;

   //return the course deviation
   return cd;
}

double CCourseIndicator::calc_leg_init
(
   double latR1  , double lonR1  ,
   double latR2  , double lonR2  ,
   double latS   , double lonS   ,
   double &gcdR1R2, double &angleR1R2, //range and bearing from R1 to R2
   double &gcdR2R1, double &angleR2R1, //range and bearing from R2 to R1
   double &gcdR1S , double &angleR1S,  //range and bearing from R1 to ship
   double &gcdR2S , double &angleR2S,  //range and bearing from R2 to ship
   double &angleDeviationR1,   //the ship deviation from the leg as viewed from R1
   double &angleDeviationR2,   //the ship deviation from the leg as viewed from R2
   bool &before,  //set to true if the ship hasn't reach the first point
   bool &after,   //set to true if the ship is past the second point
   bool &onCourse //set to true if the ship is within the leg and on course
)
{
   //make sure the flags are defaulted
   before=after=onCourse=false;

   //r1 to r2
   calc_range_and_bearing(latR1,lonR1, latR2, lonR2, &gcdR1R2, &angleR1R2 );

   //r2 to r1
   calc_range_and_bearing(latR2,lonR2, latR1, lonR1, &gcdR2R1, &angleR2R1 );

   //r1 to ship
   calc_range_and_bearing(latR1,lonR1, latS, lonS, &gcdR1S, &angleR1S );

   //r2 to ship
   calc_range_and_bearing(latR2,lonR2, latS, lonS, &gcdR2S, &angleR2S );

   //is this a zero length route leg?
   if (gcdR1R2 == 0)
   {
      before=after=true;
      return fabs(gcdR1S);
   }

   double signR1=1;
   double signR2=1;

   //is the plane behind R1
   angleDeviationR1 = angleR1S - angleR1R2;
   if (angleDeviationR1 < 0)
      angleDeviationR1 += 360;

   if (angleDeviationR1 >= 90)
   if (angleDeviationR1 <=270)
   {
      before=true;
      if (angleDeviationR1 <= 180)
         signR1= 1;
      else
         signR1=-1;
   }

   //is the plane behind R2
   angleDeviationR2 = angleR2S - angleR2R1;
   if (angleDeviationR2 < 0)
      angleDeviationR2 += 360;

   if (angleDeviationR2 >= 90)
   if (angleDeviationR2 <=270)
   {
      after=true;
      if (angleDeviationR2 >= 180)
         signR2= 1;
      else
         signR2=-1;
   }

   if (before || after)
   {
      //On a sphere, it is possible to be both before and after a leg
      //so we choose the minimum value in this case
      if (before && after)
      {
         if (gcdR1S < gcdR2S)
            return signR1*gcdR1S;
         else
            return signR2*gcdR2S;
      }
      else
      if (before)
         return signR1*gcdR1S;
      else
      if (after)
         return signR2*gcdR2S;
   }
   ///////////////////////////

   //is the ship on the route leg as viewed from R1
   if (angleDeviationR1 == 0 || angleDeviationR2 == 0)
      onCourse=true;

   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CCourseIndicator message handlers

void CCourseIndicator::OnCdiCenterCourse() 
{
	// TODO: Add your command handler code here
   SetCenterMode('COUR');
}

void CCourseIndicator::OnUpdateCdiCenterCourse(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
   pCmdUI->SetCheck( m_centerMode=='COUR' );
}

void CCourseIndicator::OnCdiCenterShip() 
{
	// TODO: Add your command handler code here
   SetCenterMode('SHIP');
}

void CCourseIndicator::OnUpdateCdiCenterShip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
   pCmdUI->SetCheck( m_centerMode=='SHIP' );
}

void CCourseIndicator::OnCdiUnitsFeet() 
{
	// TODO: Add your command handler code here
	SetUnits('FT');
}

void CCourseIndicator::OnUpdateCdiUnitsFeet(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	   pCmdUI->SetCheck( m_units=='FT' );
}

void CCourseIndicator::OnCdiUnitsKilometers() 
{
	// TODO: Add your command handler code here
	SetUnits('KM');	
}

void CCourseIndicator::OnUpdateCdiUnitsKilometers(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	   pCmdUI->SetCheck( m_units=='KM' );
}

void CCourseIndicator::OnCdiUnitsMeters() 
{
	// TODO: Add your command handler code here
	SetUnits('ME');	
}

void CCourseIndicator::OnUpdateCdiUnitsMeters(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	   pCmdUI->SetCheck( m_units=='ME' );
}

void CCourseIndicator::OnCdiUnitsNauticalMiles() 
{
	// TODO: Add your command handler code here
	SetUnits('NM');	
}

void CCourseIndicator::OnUpdateCdiUnitsNauticalMiles(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	   pCmdUI->SetCheck( m_units=='NM' );
}

void CCourseIndicator::OnCdiUnitsStatuteMiles() 
{
	// TODO: Add your command handler code here
	SetUnits('MI');	
}

void CCourseIndicator::OnUpdateCdiUnitsStatuteMiles(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
   pCmdUI->SetCheck( m_units=='MI' );
}

void CCourseIndicator::OnCdiUnitsYards() 
{
	// TODO: Add your command handler code here
	SetUnits('YD');	
}

void CCourseIndicator::OnUpdateCdiUnitsYards(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
   pCmdUI->SetCheck( m_units=='YD' );
}

void CCourseIndicator::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   if (pWnd != this)
      return;

   CMenu* pPopup = m_menu.GetSubMenu(0);
   ASSERT(pPopup != NULL);
   CRect wrect;
   GetWindowRect(&wrect);

   pPopup->TrackPopupMenu
   (
      TPM_LEFTALIGN | TPM_RIGHTBUTTON,
      point.x, //wrect.left+point.x,
      point.y, //wrect.top +point.y,
      AfxGetMainWnd()//this
   );
}

int CCourseIndicator::calc_range_and_bearing
(
   double latP0,
   double lonP0,
   double latP1,
   double lonP1,
   double *range,    //returns nauts
   double *bearing
)
{
   if (!m_pRoute)
      return FAILURE;

   d_geo_t p0;
   d_geo_t p1;
   p0.lat = latP0;
   p0.lon = lonP0;
   p1.lat = latP1;
   p1.lon = lonP1;

   int iResult = GEO_calc_range_and_bearing(p0, p1, *range, *bearing, m_bHeadingTypeIsGreatCircle);

   *range = CUnitConverter(*range).GetNM();

   return iResult;
}

int CCourseIndicator::calc_end_point
(
   double latP0,
   double lonP0,
   double range,  //nauts
   double bearing,
   double *latP1,
   double *lonP1
)
{
   if (!m_pRoute)
      return FAILURE;

   d_geo_t p0;
   d_geo_t p1;
   p0.lat = latP0;
   p0.lon = lonP0;

   CUnitConverter unit;
   unit.SetNM(range);
   range = unit.GetMeters();

   //get the endpoint
   int iResult = GEO_calc_end_point(p0, range, bearing, p1, m_bHeadingTypeIsGreatCircle);

   //plug the return values
   *latP1 = p1.lat;
   *lonP1 = p1.lon;

   return iResult;
}