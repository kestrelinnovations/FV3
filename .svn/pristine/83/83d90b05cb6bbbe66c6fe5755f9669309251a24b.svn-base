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

// labeling.h - Labeling Options class
#ifndef FALCONVIEW_PRINTTOOLOVERLAY_LABELING_H_
#define FALCONVIEW_PRINTTOOLOVERLAY_LABELING_H_

// system includes
// third party files
// other FalconView headers
#include "FalconView/include/common.h"
// this project's headers
// forward definitions

class CLabelingOptions
{
   boolean_t m_print_map_type_and_scale;
   boolean_t m_print_chart_series_and_date;
   boolean_t m_print_date_and_time;
   boolean_t m_print_echum_currency;
   boolean_t m_print_dafif_currency;
   boolean_t m_print_cadrg_currency;
   boolean_t m_print_compass_rose;
   boolean_t m_print_compass_rose_when_not_north_up;

public:
   // constructor
   CLabelingOptions();

public:
   // get functions
   boolean_t get_print_map_type_and_scale()
   {
      return m_print_map_type_and_scale;
   }
   boolean_t get_print_chart_series_and_date()
   {
      return m_print_chart_series_and_date;
   }
   boolean_t get_print_date_and_time()
   {
      return m_print_date_and_time;
   }
   boolean_t get_print_echum_currency()
   {
      return m_print_echum_currency;
   }
   boolean_t get_print_dafif_currency()
   {
      return m_print_dafif_currency;
   }
   boolean_t get_print_cadrg_currency()
   {
      return m_print_cadrg_currency;
   }

   boolean_t get_print_compass_rose()
   {
      return m_print_compass_rose;
   }
   boolean_t get_print_compass_rose_when_not_north_up()
   {
      return m_print_compass_rose_when_not_north_up;
   }

   // set functions
   void set_print_map_type_and_scale(boolean_t on)
   {
      m_print_map_type_and_scale = on;
   }
   void set_print_chart_series_and_date(boolean_t on)
   {
      m_print_chart_series_and_date = on;
   }
   void set_print_date_and_time(boolean_t on)
   {
      m_print_date_and_time = on;
   }
   void set_print_echum_currency(boolean_t on)
   {
      m_print_echum_currency = on;
   }
   void set_print_dafif_currency(boolean_t on)
   {
      m_print_dafif_currency = on;
   }
   void set_print_cadrg_currency(boolean_t on)
   {
      m_print_cadrg_currency = on;
   }

   void set_print_compass_rose(boolean_t on)
   {
      m_print_compass_rose = on;
   }
   void set_print_compass_rose_when_not_north_up(boolean_t on)
   {
      m_print_compass_rose_when_not_north_up = on;
   }

public:
   // initialize values from the registry into CLabelingOptions given a
   // registry key
   int initialize_from_registry(CString key);

   // Store values of CLabelingOptions into the registry given a key
   int save_in_registry(CString key);

   // serialization
   void Serialize(CArchive *ar);
};

#endif  // FALCONVIEW_PRINTTOOLOVERLAY_LABELING_H_
