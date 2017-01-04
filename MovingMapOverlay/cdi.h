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



/*
   Filename :  cdi.h
   Date     :  1998-SEP-15
   Author   :  Ray E. Bornert II
   Purpose  :  Course Deviation Indictor Class
*/

//**************************************************************
// GATE
//**************************************************************
#if !defined(AFX_COURSEINDICATOR_H__B4BC16A8_4C12_11D2_816E_00609733A64E__INCLUDED_)
#define AFX_COURSEINDICATOR_H__B4BC16A8_4C12_11D2_816E_00609733A64E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "common.h"
#include "linear.h"

// foward declarations
class CourseIndicator;
class COverlayCOM;
class CUnitConverter;

class CCourseIndicator : public CWnd
{
	DECLARE_DYNCREATE(CCourseIndicator)

// Construction
public:
	CCourseIndicator();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCourseIndicator)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
public:
	long GetCenterMode(void);
	long SetCenterMode( long mode );
	long GetUnits(void);
	long SetUnits( long mode );

   void invalidate(bool erase_background);
   void drawDeviation(CDC *pDC);

   void OnDraw( CDC *pDC );
   void SetDeviation(double d);
   double GetDeviation(CString &s);

   //update the position deviation from the route
   LRESULT Update(WPARAM wParam, LPARAM lParam);

   //returns 3 values in a vector
   //x is the left right perpindicular distance from the leg line
   //y is the forward progress
   //z is the length of the leg
   Vector calc_leg_deviation
   (
      Vector point0,
      Vector point1,
      Vector position
   );

   //convert lat lon info to vector info
   void geo_to_vec
   (
      double latP0  , double lonP0  ,
      double latP1  , double lonP1  ,
      double latShip, double lonShip,
      Vector& vp0,
      Vector& vp1,
      Vector& vsh
   );

   //find the minimum distance from any point on the leg
   double calc_distance_from_leg_vector
   (
      double latP0  , double lonP0  ,
      double latP1  , double lonP1  ,
      double latShip, double lonShip
   );

   //find the minimum distance from any point on the leg
   double calc_distance_from_leg_trig
   (
      double latP0  , double lonP0  ,
      double latP1  , double lonP1  ,
      double latShip, double lonShip
   );

   //find the minimum distance from any point on the leg
   double calc_distance_from_leg_exact
   (
      double latP0  , double lonP0  ,
      double latP1  , double lonP1  ,
      double latShip, double lonShip
   );

   double calc_leg_init
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
   );

   void VerifyMethod();

   COverlayCOM *m_pRoute;

   int calc_range_and_bearing(double latP0, double lonP0, double latP1, 
      double lonP1, double *range, double *bearing);

   int calc_end_point(double latP0, double lonP0, double range, double bearing,
      double *latP1, double *lonP1);

private:
	CMenu m_menu;
   int m_endingTpId;
   BOOL m_bHeadingTypeIsGreatCircle;
   CUnitConverter *m_deviation; //nautical miles
	static long m_centerMode;
   static long m_units;

   void UpdateWindowText(const CString& startingTpLabel, const CString& endingTpLabel);

public:
	virtual ~CCourseIndicator();

	// Generated message map functions
public:
	//{{AFX_MSG(CCourseIndicator)
	afx_msg void OnCdiCenterCourse();
	afx_msg void OnUpdateCdiCenterCourse(CCmdUI* pCmdUI);
	afx_msg void OnCdiCenterShip();
	afx_msg void OnUpdateCdiCenterShip(CCmdUI* pCmdUI);
	afx_msg void OnCdiUnitsFeet();
	afx_msg void OnUpdateCdiUnitsFeet(CCmdUI* pCmdUI);
	afx_msg void OnCdiUnitsKilometers();
	afx_msg void OnUpdateCdiUnitsKilometers(CCmdUI* pCmdUI);
	afx_msg void OnCdiUnitsMeters();
	afx_msg void OnUpdateCdiUnitsMeters(CCmdUI* pCmdUI);
	afx_msg void OnCdiUnitsNauticalMiles();
	afx_msg void OnUpdateCdiUnitsNauticalMiles(CCmdUI* pCmdUI);
	afx_msg void OnCdiUnitsStatuteMiles();
	afx_msg void OnUpdateCdiUnitsStatuteMiles(CCmdUI* pCmdUI);
	afx_msg void OnCdiUnitsYards();
	afx_msg void OnUpdateCdiUnitsYards(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
   afx_msg void OnDestroy();
   afx_msg void OnClose();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNcRButtonDblClk(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CCourseIndicator)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCourseIndicator)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COURSEINDICATOR_H__B4BC16A8_4C12_11D2_816E_00609733A64E__INCLUDED_)