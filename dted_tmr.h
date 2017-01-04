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



#ifndef __DTED_TMR_H_
#define __DTED_TMR_H_ 1


#include "utils.h"  // for FVW_Timer

enum DtedUnitsEnum { DTED_UNITS_METERS, DTED_UNITS_FEET };
enum DtedModeEnum { DTED_MODE_MSL, DTED_MODE_HAE };
enum DtedVertDatumEnum { DTED_VDATUM_EGM84, DTED_VDATUM_EGM96, DTED_VDATUM_EGM08 };

class TimerActive
{
public:
	TimerActive(bool &bTimerActive)
	{
		m_bTimerActive = bTimerActive;
		m_bWeOwnTimer = false;
	}
	~TimerActive()
	{
		if (m_bWeOwnTimer)
			m_bTimerActive = false;
	}
	bool IsTimerActive()
	{
		return m_bTimerActive;
	}
	void SetTimerActive()
	{
		m_bTimerActive = true;
		m_bWeOwnTimer = true;
	}
	bool		m_bTimerActive;
	bool		m_bWeOwnTimer;
};


//--------------------- DTED Timer -----------------------

class DTEDTimer: public FVW_Timer
{
   
   DECLARE_DYNAMIC(DTEDTimer)

   DtedUnitsEnum m_eDtedUnits;
   DtedModeEnum  m_eDtedMode;
   DtedVertDatumEnum m_eDtedVertDatum;

public:
   virtual void expired(void);
   DTEDTimer(int period);

   void SetDtedUnits(DtedUnitsEnum eDtedUnits)
   {
      m_eDtedUnits = eDtedUnits;
   }

   void SetDtedMode(DtedModeEnum eDtedMode)
   {
      m_eDtedMode = eDtedMode;
   }

   void SetDtedVertDatum(DtedVertDatumEnum eDtedVertDatum)
   {
      m_eDtedVertDatum = eDtedVertDatum;
   }

   void Reset(const d_geo_t& geo);

	bool		m_bTimerActive;

protected:
   double GetHaeDelta(double dLat, double dLon);
   CString FormatElevation(DtedModeEnum eDtedMode, double dElevation, double dVertAccuracy,
      double dLat, double dLon);
   CString FormatElevationType(BSTR bstrProduct, double dScale, MapScaleUnitsEnum eScaleUnits, BSTR bstrSeries);
   void UpdateHorizontalAccuracy(MapProj *pMap, double dLat, double dLon);

private:
   // Location used when the timer expires
   d_geo_t m_location;
};

#endif