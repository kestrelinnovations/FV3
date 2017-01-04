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



#include "stdafx.h"
#include "utils.h"
#include "errx.h" 
#include "param.h"

/////////////////////////////////////////////////////////////////////////////

//
// returns whether timing code is enabled (that is, whether timing 
// functions and classes are operating properly)
//
boolean_t UTL_timing_on(void)
{
   static boolean_t timing_on;
   static boolean_t init = FALSE;

   if (!init)
   {
      timing_on = 
         TO_BOOLEAN_T(PRM_get_registry_int("Developer", "TimingOn", 0));
      init = TRUE;
   }

   return timing_on;
}

//////////////////////////////////////////////////////////////////////////
// UTL_StopWatch class
//////////////////////////////////////////////////////////////////////////

UTL_StopWatch::UTL_StopWatch()
{
   m_seconds = 0.0;
   m_started = FALSE;
   m_time_valid = FALSE;
}

int UTL_StopWatch::start(void)
{
   if (!UTL_timing_on())
      return SUCCESS;

   if (m_started)
   {
      ERR_report("timing already started");
      return FAILURE;
   }

   m_time_valid = FALSE;
   m_started = FALSE;
   m_seconds = 0.0;

   if (::QueryPerformanceCounter(&m_start_count) == 0)
   {
      m_start_count.QuadPart = 0;
      return FAILURE;
   }

   m_started = TRUE;

   return SUCCESS;
}

int UTL_StopWatch::stop(void)
{
   if (!UTL_timing_on())
      return SUCCESS;

   LARGE_INTEGER end_count;

   if (!m_started)
      return FAILURE;

   if (::QueryPerformanceCounter(&end_count) == 0)
   {
      m_started = FALSE;
      return FAILURE;
   }

   m_started = FALSE;

   LARGE_INTEGER freq;
   if (::QueryPerformanceFrequency(&freq) == 0)
   {
      return FAILURE;
   }

   // set seconds by dividing the elapsed count by the counts per second
   const double count = 
      static_cast<double>(end_count.QuadPart - m_start_count.QuadPart);
   m_seconds = count/((double)(freq.QuadPart));

   m_time_valid = TRUE;

   return SUCCESS;
}

double UTL_StopWatch::get_final_seconds(void)
{
   if (!UTL_timing_on())
      return 0.0;

   if (!m_time_valid)
       return 0.0;

   return m_seconds;
}

double UTL_StopWatch::get_final_milliseconds(void)
{
   return get_final_seconds()*1000.0;
}

double UTL_StopWatch::get_time_now_seconds(void) const
{
   if (!UTL_timing_on())
      return 0.0;

   LARGE_INTEGER end_count;

   if (!m_started)
      return 0.0;

   if (::QueryPerformanceCounter(&end_count) == 0)
   {
      return 0.0;
   }

   LARGE_INTEGER freq;
   if (::QueryPerformanceFrequency(&freq) == 0)
   {
      return 0.0;
   }

   // set seconds by dividing the elapsed count by the counts per second
   const double count = 
      static_cast<double>(end_count.QuadPart - m_start_count.QuadPart);
   double seconds = count/((double)(freq.QuadPart));

   return seconds;
}

double UTL_StopWatch::get_time_now_milliseconds(void) const
{
   return get_time_now_seconds()*1000.0;
}

//////////////////////////////////////////////////////////////////////////
// UTL_TimingStatistics class
//////////////////////////////////////////////////////////////////////////

UTL_TimingStatistics::UTL_TimingStatistics()
{
   m_count = 0;
   m_total_in_millisecs = 0.0;
   m_min_time_in_millisecs = DBL_MAX;
   m_max_time_in_millisecs = 0.0;
}

void UTL_TimingStatistics::add_milliseconds(double millisecs)
{
   m_total_in_millisecs += millisecs;
   m_count++;

   if (millisecs < m_min_time_in_millisecs)
      m_min_time_in_millisecs = millisecs;
   if (millisecs > m_max_time_in_millisecs)
      m_max_time_in_millisecs = millisecs;
}

void UTL_TimingStatistics::add_seconds(double secs)
{
   add_milliseconds(secs*1000.0);
}

double UTL_TimingStatistics::min_time_in_milliseconds(void) const
{
   if (m_count == 0)
      return 0.0;

   return m_min_time_in_millisecs;
}

double UTL_TimingStatistics::max_time_in_milliseconds(void) const
{
   if (m_count == 0)
      return 0.0;

   return m_max_time_in_millisecs;
}

double UTL_TimingStatistics::average_time_in_milliseconds(void) const
{
   if (m_count == 0)
      return 0.0;

   return m_total_in_millisecs/static_cast<double>(count());
}
