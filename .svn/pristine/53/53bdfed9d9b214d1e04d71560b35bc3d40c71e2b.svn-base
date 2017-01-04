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



// ExifImage.h

#include <gdiplus.h>

class ExifImage : public Gdiplus::Bitmap
{
   Gdiplus::PropertyItem* m_pPropertyItems;
   UINT m_nNumProperties;

public:
   ExifImage(WCHAR *filename);
   virtual ~ExifImage();

   BOOL GetDateTimeOrig(DATE& dtOrig);
   BOOL GetGpsLocation(double &dLat, double &dLon);
   BOOL GetImageDescription(CString &strImageDescription);

   BOOL SetGpsLocation(double dLat, double dLon);
   BOOL SetImageDescription(const CString& strImageDescription);

protected:
   void ConvertToRational(double value, ULONG rational[6]);
};
