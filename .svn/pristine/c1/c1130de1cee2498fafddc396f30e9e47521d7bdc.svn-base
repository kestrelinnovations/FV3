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



// MBString.cpp: implementation of the CMBString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MBString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMBString::CMBString()
{

}

CMBString::~CMBString()
{

}

CString CMBString::FormatMB(double fBytes, int nPrec/*=3*/)
{
   const double oneKB = 1024.0;
   const double oneMB = oneKB * oneKB;
   const double oneGB = oneMB * oneKB;
   CString s;

   if (fBytes > oneGB)
      s.Format("%.*f GB", nPrec, fBytes / oneGB);
   else if (fBytes > oneMB)
      s.Format("%.*f MB", nPrec, fBytes / oneMB);
   else if (fBytes > oneKB)
      s.Format("%.*f KB", nPrec, fBytes / oneKB);
   else if (fBytes > 0.0)
      s.Format("%.0f", fBytes);
   else if (fBytes == 0.0)
      s.Format("---");
   else if (fBytes > -oneKB)
      s.Format("%.0f", fBytes);
   else if (fBytes > -oneMB)
      s.Format("%.*f KB", nPrec, fBytes / oneKB);
   else if (fBytes > -oneGB)
      s.Format("%.*f MB", nPrec, fBytes / oneMB);
   else
      s.Format("%.*f GB", nPrec, fBytes / oneGB);

   return s;
}

// Initialize the time routine with number of items to process
CString CMBString::TimeInitialize(long lTotalCount, double fTotalMB)
{
   m_starttime = COleDateTime::GetCurrentTime();
   m_fTotalMB = fTotalMB;
	m_MovingAvgSecs.RemoveAll();

   // do a moving average over 10% values or 80 whichever is smaller but no less than 1.
	long lSize = lTotalCount/10L;
   m_MovingAvgSize = (lSize > 80L) ? 80L : (lSize > 0L) ? lSize : 1L;

   return "Estimating...";
}

// Step the time routine with number of items left to process
CString CMBString::TimeRemaining(double fRemainingMB)
{
   CString s;
   BOOL bApproximate = TRUE;  // estimates are approximate until threshold is reached

   COleDateTimeSpan timespan = COleDateTime::GetCurrentTime() - m_starttime;

   double fSecondsPerMB = (double)timespan.GetTotalSeconds() / (m_fTotalMB - fRemainingMB);
	double fSum = 0.0;

	//	Take the moving average of the last m_MovingAvgSize results
   m_MovingAvgSecs.Add(fRemainingMB * fSecondsPerMB);

   // Once we reach the size threshold remove early entries
	if (m_MovingAvgSecs.GetSize() > m_MovingAvgSize)
   {
		m_MovingAvgSecs.RemoveAt(0);
      bApproximate = FALSE;
   }

	for (int i = 0; i < m_MovingAvgSecs.GetSize(); ++i)
		fSum += m_MovingAvgSecs[i];

	double fSecondsRemaining = fSum / m_MovingAvgSecs.GetSize();

	//	Now round up to the nearest 5 seconds
	fSecondsRemaining = (floor(fSecondsRemaining / 5.0) + 1.0) * 5.0;

   s = FormatTime(fSecondsRemaining);

   if (bApproximate)
      return s;   // "~" + s;
   else
      return s;
}

// Format the remaining number of seconds.
CString CMBString::FormatTime(double fSecondsRemaining)
{
   CString s;

	COleDateTimeSpan togospan(fSecondsRemaining / 60.0 / 60.0 / 24.0);

   const double twoDays    = 2.0 * 24.0 * 60.0 * 60.0 - 1.0;
   const double oneDay     = 1.0 * 24.0 * 60.0 * 60.0 - 1.0;
   const double twoHours   = 2.0 * 60.0 * 60.0 - 1.0;
   const double oneHour    = 1.0 * 60.0 * 60.0 - 1.0;
   const double twoMinutes = 2.0 * 60.0 - 1.0;
   const double oneMinute  = 1.0 * 60.0 - 1.0;
   const double twoSeconds = 2.0 - 1.0;
   const double oneSecond  = 1.0 - 1.0;

   double fSecs = togospan.GetTotalSeconds();

   // Take the highest format applicable
   // NOTE: Despite the documentation Formats' %D parameter does not work.
   // Hence we need to figure out the # days ourselves.
	if (fSecs > twoDays)          s.Format("%d days %s", (int)togospan.GetTotalDays(), togospan.Format("%#H hours"));
	else if (fSecs > oneDay)      s.Format("%d day %s", (int)togospan.GetTotalDays(), togospan.Format("%#H hours"));
	else if (fSecs > twoHours)    s = togospan.Format("%#H hours %#M minutes");
	else if (fSecs > oneHour)     s = togospan.Format("%#H hour %#M minutes");
	else if (fSecs > twoMinutes)  s = togospan.Format("%#M minutes");
	else if (fSecs > oneMinute)   s = togospan.Format("%#M minute %#S seconds");
	else if (fSecs > twoSeconds)  s = togospan.Format("%#S seconds");
	else if (fSecs > oneSecond)   s = "1 second";
   else                          s = "Done";

   return s;
}

// Print thresholds.
void CMBString::DebugFormatTime(void)
{
   CMBString sMB;
   CString s;
   double fSecs;

   const double twoDays    = 2.0 * 24.0 * 60.0 * 60.0 - 1.0;
   const double oneDay     = 1.0 * 24.0 * 60.0 * 60.0 - 1.0;
   const double twoHours   = 2.0 * 60.0 * 60.0 - 1.0;
   const double oneHour    = 1.0 * 60.0 * 60.0 - 1.0;
   const double twoMinutes = 2.0 * 60.0 - 1.0;
   const double oneMinute  = 1.0 * 60.0 - 1.0;
   const double twoSeconds = 2.0 - 1.0;
   const double oneSecond  = 1.0 - 1.0;

   TRACE("twoDays---------------------------\n");
   for (fSecs = twoDays - 5.0; fSecs < twoDays + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("oneDay---------------------------\n");
   for (fSecs = oneDay - 5.0; fSecs < oneDay + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("twoHours---------------------------\n");
   for (fSecs = twoHours - 5.0; fSecs < twoHours + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("oneHour---------------------------\n");
   for (fSecs = oneHour - 5.0; fSecs < oneHour + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("twoMinutes---------------------------\n");
   for (fSecs = twoMinutes - 5.0; fSecs < twoMinutes + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("oneMinute---------------------------\n");
   for (fSecs = oneMinute - 5.0; fSecs < oneMinute + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("twoSeconds---------------------------\n");
   for (fSecs = twoSeconds - 5.0; fSecs < twoSeconds + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("oneSecond---------------------------\n");
   for (fSecs = oneSecond - 5.0; fSecs < oneSecond + 5.0; fSecs += 1.0)
      TRACE("%f secs --> [%s]\n", fSecs, sMB.FormatTime(fSecs));
   TRACE("---------------------------\n");
}
