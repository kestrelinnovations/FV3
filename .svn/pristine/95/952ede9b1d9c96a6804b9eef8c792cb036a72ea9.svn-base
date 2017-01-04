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

#ifndef __GDAL_INITIALIZER_H_
#define __GDAL_INITIALIZER_H_

#include "gdal.h"

// This class acts as a single point to initialize / uninitialize GDAL
// throughout the FalconView process.  Many libraries may use GDAL.  None
// of them should override the functions of this class.

class GdalInitializer
{
public:
   static void InitGDAL(bool test_mode = false);
   static void UninitGDAL();
   static bool IsGDALInitialized() { return s_gdal_initialized; }
   static void GetLastError(CPLErr *err_class, int *err_no, std::string *msg);

private:
   static void CPL_STDCALL GDALErrorHandler(
      CPLErr err_class, int err_no, const char *msg);

   static bool s_gdal_initialized, s_test_mode;
   static CPLErr s_last_err_class;
   static int s_last_err_no;
   static std::string s_last_err_msg;
};

#endif // #ifndef __GDAL_INITIALIZER_H_
