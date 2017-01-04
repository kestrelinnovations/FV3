// Copyright (c) 1994-2013 Georgia Tech Research Corporation, Atlanta, GA
// This file is part of FalconView(R).

// FalconView(R) is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// FalconView(R) is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with FalconView(R).  If not, see <http://www.gnu.org/licenses/>.

// FalconView(R) is a registered trademark of Georgia Tech Research Corporation.

#include "StdAfx.h"
#include "GdalInitializer.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "err.h"

/*static*/ bool GdalInitializer::s_gdal_initialized = false;
/*static*/ bool GdalInitializer::s_test_mode = false;
/*static*/ CPLErr GdalInitializer::s_last_err_class = CE_None;
/*static*/ int GdalInitializer::s_last_err_no = CPLE_None;
/*static*/ std::string GdalInitializer::s_last_err_msg;

/*static*/ void GdalInitializer::InitGDAL(bool test_mode /*= false*/)
{ 
   if (!s_gdal_initialized)
   {
      s_test_mode = test_mode;
      CPLSetErrorHandler(&GDALErrorHandler);
      OGRRegisterAll(); // register all OGR drivers
      GDALAllRegister();
      s_gdal_initialized = true;
   }
}

/*static*/ void GdalInitializer::UninitGDAL()
{ 
   if (s_gdal_initialized)
   {
      OGRCleanupAll();
      GDALDestroyDriverManager();
      s_test_mode = false;
      s_gdal_initialized = false;
   }
}

/*static*/ void CPL_STDCALL GdalInitializer::GDALErrorHandler(
   CPLErr err_class, int err_no, const char *msg)
{
   s_last_err_class = err_class;
   s_last_err_no = err_no;
   s_last_err_msg = msg;

   if (s_test_mode)
      return; // don't log

   std::stringstream ss;

   switch (err_class)
   {
   case CE_None:
      break;
   case CE_Debug:
   case CE_Warning:
      ss << "GDAL Message: " << msg;
      INFO_report(ss.str().c_str());
      break;
   case CE_Failure:
   case CE_Fatal:
   default:
      ss << "GDAL Error: " << msg;
      ERR_report(ss.str().c_str());
      break;
   }
}

/*static*/ void GdalInitializer::GetLastError(
   CPLErr *err_class, int *err_no, std::string *msg)
{
   *err_class = s_last_err_class;
   *err_no = s_last_err_no;
   *msg = s_last_err_msg;
}
