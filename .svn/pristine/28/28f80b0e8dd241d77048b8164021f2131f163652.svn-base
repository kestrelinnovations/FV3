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
#include "common.h"
#include "maps.h"
#include "errx.h"


// ------------------------------------------------------------------------

inline static
double meters_to_feet(INT2 meters)
{
   return METERS_TO_FEET(meters);
}

inline static
INT4 round(double val)
{
   if (val >= 0.0)
      return ((INT4)(val+0.5));
   else
      return ((INT4)(val-0.5));
}


int DTD_get_elevation_in_feet(degrees_t lat, degrees_t lon, INT4* elev, 
                              long *vert_accuracy_ft /*= NULL*/)
{
   short DTEDLevelUsed;
   
   try
   {
      IDtedPtr dted(__uuidof(Dted));
      
      *elev = dted->GetElevation(lat, lon, 0, DTED_ELEVATION_FEET, &DTEDLevelUsed);
      dted->Terminate();
      
      if (*elev == MISSING_DTED_ELEVATION)
         *elev = MAXLONG;
      
      if (DTEDLevelUsed == 0)
         return FV_NO_DATA;
   }
   catch(_com_error &e)
   {
      CString msg;
      msg.Format("Failed retrieving DTED elevation: (%s)", (char *)e.Description());
      ERR_report(msg);
      return FAILURE;
   }
   
   // obtain the vertical accuracy for the point if requested
   try
   {
      if (vert_accuracy_ft != NULL)
      {
         *vert_accuracy_ft = 0;
         
         ICoverageRowsetPtr cov(__uuidof(MDSUtilCoverage));
         IDataSourcesRowsetPtr data_sources(__uuidof(DataSourcesRowset));
         IDtedFrameFilePtr ff(__uuidof(DtedFrameFile));
         IDtedAttributesPtr attr = ff;
         
         cov->Initialize("Dted");
         const double EPS = 1e-6;
         HRESULT hr = cov->SelectByGeoRect(DTEDLevelUsed, lat, lon, lat+EPS, lon+EPS);
         if (hr == S_OK)
         {
            std::string location_spec = std::string(cov->m_LocationSpec);
            
            std::string filename = std::string(cov->GetLocalOrRemoteFolderName(cov->m_DataSourceId)) 
               + location_spec;
            
            attr->ReadAttributes(_bstr_t(filename.c_str()));
            *vert_accuracy_ft = static_cast<long>(METERS_TO_FEET(attr->GetVerticalAccuracy()));
         }
      }
      
      return SUCCESS;
   }
   catch(_com_error e)
   {
      // it's okay if we weren't able to retrieve the vertical accuracy
      return SUCCESS;
   }

   return SUCCESS;
}

int DTD_get_elevation_in_meters(degrees_t lat, degrees_t lon, INT2* elev)
{
   INT4 elevFeet;
   int ret = DTD_get_elevation_in_feet(lat, lon, &elevFeet, NULL);

   *elev = static_cast<INT2>(FEET_TO_METERS(elevFeet));
   return ret;
}

